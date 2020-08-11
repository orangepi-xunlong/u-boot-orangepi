/*
**********************************************************************************************************************
*
*						           the Embedded Secure Bootloader System
*
*
*						       Copyright(C), 2006-2014, Allwinnertech Co., Ltd.
*                                           All Rights Reserved
*
* File    :
*
* By      :
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#include "common.h"
#include <private_toc.h>
#include <private_uboot.h>
#include "asm/arch/platform.h"
#include "openssl_ext.h"
#include "asm/arch/ss.h"
#include "asm/arch/uart.h"

extern int sid_prode_key_ladder(void);
extern void ndump(u8 *buf, int count);

extern sbrom_toc0_config_t *toc0_config;
extern int mmc_config_addr;
SBROM_TOC0_KEY_ITEM_info_t *key_item = NULL;
int key_ladder_flag;

static void toc0_config_init(void)
{
	toc0_config = (sbrom_toc0_config_t *) (CONFIG_SBROMSW_BASE + 0xA0);
	mmc_config_addr = (int)(toc0_config->storage_data + 160);
}

/*
 * get_key1_pk
 *
 * fuction: get key1 from toc0 head
 *
 * return:
 *
 */

static void get_key1_pk(toc0_private_head_t *toc0)
{
	SBROM_TOC0_ITEM_info_t *toc0_item = NULL;
	int i=0;

	toc0_item = (SBROM_TOC0_ITEM_info_t *) (CONFIG_SBROMSW_BASE + sizeof(toc0_private_head_t));
	for ( i = 0; i < toc0->items_nr;i++) {
		if (toc0_item->name == ITEM_NAME_SBROMSW_KEY) {
			key_item = (SBROM_TOC0_KEY_ITEM_info_t *) (CONFIG_SBROMSW_BASE + toc0_item->data_offset);
			return ;
		}
		toc0_item++;
	}

}

/*
 * key_ladder_init
 *
 * fuction: It will prode the key ladder is valid,it will set toc0_config offset
 *          and get key1 from toc0 head.
 * return:
 *
 */

void key_ladder_init(toc0_private_head_t *toc0)
{
	key_ladder_flag = sid_prode_key_ladder();
	if (key_ladder_flag == 2) {
		toc0_config_init();
		get_key1_pk(toc0);
	}

}

#define RSA_BIT_WITDH 2048
int sunxi_key_ladder_pk_check(sunxi_key_t  *pubkey)
{
	char key_item_hash[256], rotpk_hash[256];
	char pk[RSA_BIT_WITDH/8 * 2 + 256];

	if(key_item == NULL) {
		pr_error("key_item is NULL!\n");
		return -1;
	}

 	memset(key_item_hash, 0, 32);
	memset(pk, 0x91, sizeof(pk));
	char *align = (char *)(((u32)pk+31)&(~31));
	if ( *(key_item->KEY1_PK) ) {
		memcpy(align, key_item->KEY1_PK, (key_item->KEY1_PK_mod_len + key_item->KEY1_PK_e_len));
	} else {
		memcpy(align, key_item->KEY1_PK+1, (key_item->KEY1_PK_mod_len-1 + key_item->KEY1_PK_e_len));
	}
	if (sunxi_sha_calc( (u8 *)key_item_hash, 32, (u8 *)align, RSA_BIT_WITDH/8*2 )) {
		printf("sunxi_sha_calc: calc  key item key1 pk sha256 with hardware err\n");
		return -1;
	}
	/*ndump((u8 *)align , RSA_BIT_WITDH/8*2 );*/

	memset(rotpk_hash, 0, 32);
	memset(pk, 0x91, sizeof(pk));
	if ( *(pubkey->n) ) {
		memcpy(align, pubkey->n, pubkey->n_len);
		memcpy(align+pubkey->n_len, pubkey->e, pubkey->e_len);
	} else {
		memcpy(align, pubkey->n+1, pubkey->n_len-1);
		memcpy(align+pubkey->n_len-1, pubkey->e, pubkey->e_len);
	}

	if (sunxi_sha_calc( (u8 *)rotpk_hash, 32, (u8 *)align, RSA_BIT_WITDH/8*2 )) {
		printf("sunxi_sha_calc: calc  pubkey sha256 with hardware err\n");
		return -1;
	}

	if (memcmp(rotpk_hash, key_item_hash, 32)) {
		pr_error("certif pk dump\n");
		ndump((u8 *)align , RSA_BIT_WITDH/8*2 );

		pr_error("calc certif pk hash dump\n");
		ndump((u8 *)rotpk_hash,32);

		pr_error("key_item_hash pk1 hash dump\n");
		ndump((u8 *)key_item_hash,32);

		pr_error("sunxi_key_ladder_pk_check: pubkey hash check err\n");
		return -1;
	}
	return 0 ;

}

/*
 * sunxi_key_ladder_verify
 *
 * fuction:if the key ladder is valid,it will check the pk
 *          of key item
 * return: 0-- check pk right
 *         1-- check pk failure
 *         2-- no key ladder check
 */

int sunxi_key_ladder_verify(sunxi_key_t  *pubkey)
{
	int ret = 0;
	if (key_ladder_flag == 2 ) {
		pr_force("enter the two key ladder pk check!\n");
		ret = sunxi_key_ladder_pk_check(pubkey);
		if (ret < 0) {
			return -1;
		}
		return 0;
	} else {
		pr_force("enter the one key ladder pk check!\n");
	}

	return 2;
}


