/*
 * (C) Copyright 2000-2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Allwinner boot verify trust-chain
 */
#include <common.h>
#include <openssl_ext.h>
#include <private_toc.h>
#include <asm/arch/ss.h>
#include <sunxi_board.h>


static int sunxi_certif_pubkey_check( sunxi_key_t  *pubkey, u8 *hash_buf);
static int sunxi_root_certif_pk_verify(sunxi_certif_info_t *sunxi_certif, u8 *buf, u32 len, u8 *hash_buf);
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_verify_signature(void *buff, uint len, const char *cert_name)
{
	u8 hash_of_file[32];
	int ret;
	struct sbrom_toc1_head_info  *toc1_head;
	struct sbrom_toc1_item_info  *toc1_item;
	sunxi_certif_info_t  sub_certif;
	int i;

	memset(hash_of_file, 0, 32);
	sunxi_ss_open();
	ret = sunxi_sha_calc(hash_of_file, 32, buff, len);
	if(ret)
	{
		printf("sunxi_verify_signature err: calc hash failed\n");
		//sunxi_ss_close();

		return -1;
	}
	//sunxi_ss_close();
	printf("show hash of file\n");
	sunxi_dump(hash_of_file, 32);
	//获取来自toc1的证书序列
	toc1_head = (struct sbrom_toc1_head_info *)CONFIG_TOC1_STORE_IN_DRAM_BASE;
	toc1_item = (struct sbrom_toc1_item_info *)(CONFIG_TOC1_STORE_IN_DRAM_BASE + sizeof(struct sbrom_toc1_head_info));

	for(i=1;i<toc1_head->items_nr;i++, toc1_item++)
	{
		if(toc1_item->type == TOC_ITEM_ENTRY_TYPE_BIN_CERTIF)
		{
			printf("find cert name %s\n", toc1_item->name);
			if(!strcmp((const char *)toc1_item->name, cert_name))
			{
				//取出证书的扩展项
				if(sunxi_certif_probe_ext(&sub_certif, (u8 *)(CONFIG_TOC1_STORE_IN_DRAM_BASE + toc1_item->data_offset), toc1_item->data_len))
				{
					printf("%s error: cant verify the content certif\n", __func__);

					return -1;
				}
				//比较扩展项和hash
				printf("show hash in certif\n");
				sunxi_dump(sub_certif.extension.value[0], 32);
				if(memcmp(hash_of_file, sub_certif.extension.value[0], 32))
				{
					printf("hash compare is not correct\n");
					printf(">>>>>>>hash of file<<<<<<<<<<\n");
					sunxi_dump(hash_of_file, 32);
					printf(">>>>>>>hash in certif<<<<<<<<<<\n");
					sunxi_dump(sub_certif.extension.value[0], 32);

					return -1;
				}

				return 0;
			}
		}
	}

	printf("cant find a certif belong to %s\n", cert_name);

	return -1;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_verify_rotpk_hash(void *input_hash_buf, int len)
{
	u8 hash_of_pubkey[32];
	struct sbrom_toc1_item_info  *toc1_item;
	sunxi_certif_info_t  root_certif;

	if(len < 32)
	{
		printf("the input hash is not equal to 32 bytes\n");

		return -1;
	}
	sunxi_ss_open();
	memset(hash_of_pubkey, 0, 32);
	sunxi_ss_open();
	//获取来自toc1的证书序列
	toc1_item = (struct sbrom_toc1_item_info *)(CONFIG_TOC1_STORE_IN_DRAM_BASE + sizeof(struct sbrom_toc1_head_info));

	if(toc1_item->type == TOC_ITEM_ENTRY_TYPE_NULL)
	{
		printf("find cert name %s\n", toc1_item->name);
		//取出证书的公钥
		if(sunxi_root_certif_pk_verify(&root_certif, (u8 *)(CONFIG_TOC1_STORE_IN_DRAM_BASE + toc1_item->data_offset), \
			toc1_item->data_len, hash_of_pubkey))
		{
			printf("%s error: cant get the content certif publickey hash\n", __func__);

			return -1;
		}
		//比较hash值
		printf("show hash of publickey in certif\n");
		sunxi_dump(input_hash_buf, 32);
		if(memcmp(input_hash_buf, hash_of_pubkey, 32))
		{
			printf("hash compare is not correct\n");
			printf(">>>>>>>hash of certif<<<<<<<<<<\n");
			sunxi_dump(hash_of_pubkey, 32);
			printf(">>>>>>>hash of user input<<<<<<<<<<\n");
			sunxi_dump(input_hash_buf, 32);

			return -1;
		}
		else
		{
			printf("the hash of input data and toc are equal\n");
		}

		return 0;
	}

	return -1;
}

/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
#define RSA_BIT_WITDH 2048
static int sunxi_certif_pubkey_check( sunxi_key_t  *pubkey, u8 *hash_buf)
{
	char rotpk_hash[256];
	char all_zero[32];
	char pk[RSA_BIT_WITDH/8 * 2 + 256]; /*For the stupid sha padding */

	memset(all_zero, 0, 32);
	memset(pk, 0x91, sizeof(pk));
	char *align = (char *)(((u32)pk+31)&(~31));
	if( *(pubkey->n) ){
		memcpy(align, pubkey->n, pubkey->n_len);
		memcpy(align+pubkey->n_len, pubkey->e, pubkey->e_len);
	}else{
		memcpy(align, pubkey->n+1, pubkey->n_len-1);
		memcpy(align+pubkey->n_len-1, pubkey->e, pubkey->e_len);
	}
	if(sunxi_sha_calc( (u8 *)rotpk_hash, 32, (u8 *)align, RSA_BIT_WITDH/8*2 ))
	{
		printf("sunxi_sha_calc: calc  pubkey sha256 with hardware err\n");
		return -1;
	}
	memcpy(hash_buf, rotpk_hash, 32);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :  buf: 证书存放起始   len：数据长度
*
*    return        :
*
*    note          :  证书自校验
*
*
************************************************************************************************************
*/
static int sunxi_root_certif_pk_verify(sunxi_certif_info_t *sunxi_certif, u8 *buf, u32 len, u8 *hash_buf)
{
	X509 *certif;
	int  ret;

	//内存初始化
	sunxi_certif_mem_reset();
	//创建证书
	ret = sunxi_certif_create(&certif, buf, len);
	if(ret < 0)
	{
		printf("fail to create a certif\n");

		return -1;
	}
	//获取证书公钥
	ret = sunxi_certif_probe_pubkey(certif, &sunxi_certif->pubkey);
	if(ret)
	{
		printf("fail to probe the public key\n");

		return -1;
	}
#if 0
	printf("public key e: %d\n", sunxi_certif->pubkey.e_len);
	sunxi_dump(sunxi_certif->pubkey.e, sunxi_certif->pubkey.e_len);

	printf("public key n: %d\n", sunxi_certif->pubkey.n_len);
	sunxi_dump(sunxi_certif->pubkey.n, sunxi_certif->pubkey.n_len);
#endif
	ret = sunxi_certif_pubkey_check(&sunxi_certif->pubkey, hash_buf);
	if(ret){
		printf("fail to check the public key hash against efuse\n");

		return -1;
	}

	sunxi_certif_free(certif);

	return 0;
}


int do_rotpk_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u8 input_hash_buf[32];
	int ret;

	memset(input_hash_buf, 0, 32);
	ret = sunxi_verify_rotpk_hash(input_hash_buf, 32);

	return ret;
}

U_BOOT_CMD(
	rotpk_test, 3, 0, do_rotpk_test,
	"test the rotpk key",
	"usage: rotpk_test"
);

