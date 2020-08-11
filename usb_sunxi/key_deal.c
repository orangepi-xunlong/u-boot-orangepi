/*
 * (C) Copyright 2007-2015
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * SPDX-License-Identifier:	GPL-2.0
 */

#include <common.h>
#include <securestorage.h>
#include <smc.h>
#include <u-boot/crc.h>
#include <asm/arch/ss.h>
#include <sunxi_board.h>
#include <smc.h>


#define HDCP_MAIGC		(0x5aa5a55a)
#define HDCP_KEY_LEN		(308)
#define AES_16_BYTES_ALIGN	(12)
#define HDCP_BUFFER_LEN         (HDCP_KEY_LEN + AES_16_BYTES_ALIGN)

#ifdef	CONFIG_SUNXI_HDCP_HASH
#define	HDCP_MD5_LEN		(16)
#define	RAW_HDCP_KEY_LEN	(288)
#define	HDCP_FILE_SIZE		(352 + HDCP_MD5_LEN)
#else
#define	HDCP_FILE_SIZE		(352)
#endif

#define	RSSK_SIZE_BITS		(256)
#define	RSSK_SIZE_BYTES		(RSSK_SIZE_BITS >> 3)


typedef struct {
	char  name[64];
	u32 len;
	u32 res;
	u8  *key_data;
}
sunxi_efuse_key_info_t;


typedef struct{
	unsigned int magic;                  //Magic number, Value is 0x5aa5a55a
	unsigned int version;                //Version of the structure
	unsigned int len;                    //Data length from start,but not include CRC
	unsigned char rak[16];               //Random 128 bit AES CBC key in small-endian
	unsigned char data[HDCP_BUFFER_LEN]; //HDCP key(308B), Padding HDCP key length to 16 bytes align after encrypt(320B)
#ifdef	CONFIG_SUNXI_HDCP_HASH
	unsigned char md5[HDCP_MD5_LEN];     /*MD5 hash value of raw HDCP Key*/
#endif
	unsigned int crc;                    //CRC32 value of the above data
}hdcp_object;

static unsigned char raw_hdcp_key[HDCP_BUFFER_LEN];


__attribute__((weak))
int sunxi_efuse_write(void *key_buf)
{
	printf("call weak fun: %s\n", __func__);
	return -1;
}

__attribute__((weak))
int sunxi_efuse_read(void *key_name, void *read_buf, int *len)
{
	printf("call weak fun: %s\n", __func__);
	return -1;
}

extern void sunxi_dump(void *addr, unsigned int size);
#ifdef CONFIG_BURN_NORMAL_EFUSE
extern int sunxi_efuse_write(void *key_buf);
extern int sunxi_efuse_read(void *key_name, void *read_buf, int *len);
#endif

int hdcp_key_parse(unsigned char *keydata, int keylen)
{
	int ret;
	hdcp_object *obj = (hdcp_object *)keydata;

	if (keylen < HDCP_FILE_SIZE)
	{
		printf("hdcp_key_parse: hdcp file is bad, size: %d\n", keylen);
		return -1;
	}
	if (obj->magic != HDCP_MAIGC)
	{
		printf("hdcp_key_parse: hdcp magic is bad\n");
		return -1;
	}
	if (obj->crc != crc32(0, (void*)obj, sizeof(hdcp_object)-4))
	{
		printf("hdcp_key_parse: hdcp crc is bad\n");
		return -1;
	}
#ifdef DUMP_KEY
	printf("hdcp, AES KEY:\n");
	sunxi_dump(obj->rak, 16);
	printf("hdcp encrypt data:\n");
	sunxi_dump(obj->data, HDCP_BUFFER_LEN);
#endif
	memset(raw_hdcp_key, 0x0, HDCP_BUFFER_LEN);
	ret = smc_aes_algorithm((char*)raw_hdcp_key,(char*)(obj->data), HDCP_BUFFER_LEN,
		(char*)(obj->rak), 0, 1);
	if (ret < 0)
	{
		printf("sunxi_aes_decrypt: failed\n");
		return -1;
	}
#ifdef DUMP_KEY
	printf("hdcp decrypt data:\n");
	sunxi_dump(raw_hdcp_key,HDCP_BUFFER_LEN);
#endif
	return 0;
}


void hdcp_key_convert(unsigned char *keyi,unsigned char *keyo)
{
	unsigned i;
	for(i=0; i<5; i++)
	{
		keyo[i] = keyi[i];
	}

	keyo[5] = keyo[6] = 0;

	for(i=0; i<280; i++)
	{
		keyo[7+i] = keyi[8+i];
	}

	keyo[287] = 0;
}

#ifdef CONFIG_SUNXI_HDCP_HASH
#define ALIGN_LEN 32
int verify_hdcp_key_sha(unsigned char *raw_md5, char *buffer_convert)
{
	unsigned char hash_of_hdcp[HDCP_MD5_LEN] = {0};
	int ret = -1;
	unsigned char key_buf[RAW_HDCP_KEY_LEN + ALIGN_LEN] = {0};

	unsigned char *align = (unsigned char *)((unsigned long)(key_buf + ALIGN_LEN) & (~(ALIGN_LEN - 1)));
	memcpy((void *)align, (void *)buffer_convert, RAW_HDCP_KEY_LEN);
	if (sunxi_md5_calc((u8 *)hash_of_hdcp, HDCP_MD5_LEN, (u8 *)align, RAW_HDCP_KEY_LEN)) {
		printf("sunxi_md5_calc: failed\n");
		return -1;
	}
#ifdef DUMP_KEY
	printf("hdcp md5 sha:\n");
	sunxi_dump(hash_of_hdcp, 16);
#endif
	ret = memcmp((void *)hash_of_hdcp, (void *)raw_md5, (unsigned int)HDCP_MD5_LEN);
	if (ret != 0) {
		printf("hash compare is not correct\n");
		printf(">>>>>>>hash of file<<<<<<<<<<\n");
		sunxi_dump(hash_of_hdcp, HDCP_MD5_LEN);
		printf(">>>>>>>hash in certif<<<<<<<<<<\n");
		sunxi_dump(raw_md5, HDCP_MD5_LEN);
		printf("hdcp key md5 compare erro\n");
		return -1;
	}
	return 0;
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
int sunxi_deal_hdcp_key_hash(char *keydata, char *buffer_convert)
{
	int ret;
	hdcp_object *obj = (hdcp_object *)keydata;
	sunxi_efuse_key_info_t	efuse_key_info;
	const char *name = "hdcphash";

	if (obj->md5 == NULL) {
		printf("hdcp key file md5 is NULL\n");
		return -1;
	}

	ret = verify_hdcp_key_sha(obj->md5, buffer_convert);
	if (ret < 0) {
		printf("verify_hdcp_key_sha: failed\n");
		return -1;
	}

	strcpy(efuse_key_info.name, name);
	efuse_key_info.len = HDCP_MD5_LEN;
	efuse_key_info.key_data = obj->md5;

	if (sunxi_get_securemode() == SUNXI_NORMAL_MODE) {
		if (sunxi_efuse_write(&efuse_key_info)) {
			return -1;
		}
	} else {
		if (arm_svc_efuse_write(&efuse_key_info)) {
			printf("warning : hdcp md5 already burned : ignore hdcp md5\n");
			return 0;
		}
	}

	return 0;
}
#endif
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
extern	int sunxi_create_rssk(u8 *rssk_buf, u32 rssk_byte);
int sunxi_deal_rssk_key(void)
{
	int ret;
	sunxi_efuse_key_info_t  efuse_key_info;
	const char *name = "rssk";
	unsigned char rssk_buf[RSSK_SIZE_BYTES] __aligned(64);
	printf("%s %d\n ",__func__,__LINE__);
	ret = sunxi_create_rssk(rssk_buf, RSSK_SIZE_BYTES);
	if (ret < 0) {
		printf("sunxi_create_rssk fail\n");
		return -1;
	}
#ifdef DUMP_KEY
	printf("rssk data:\n");
	sunxi_dump(rssk_buf, RSSK_SIZE_BYTES);
#endif
	strcpy(efuse_key_info.name, name);
	efuse_key_info.len = RSSK_SIZE_BYTES;
	efuse_key_info.key_data = rssk_buf;

	if (sunxi_get_securemode() == SUNXI_NORMAL_MODE) {
		if (sunxi_efuse_write(&efuse_key_info)) {
			return -1;
		}
	} else {
		printf("rssk ready to burn\n");
		ret = arm_svc_efuse_write(&efuse_key_info);
		if (ret == 1) {
			printf("warning: rssk has been already burned \n");
			ret = 0;
		} else if (ret < 0) {
			return -1;
		}
	}
	return 0;
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
int sunxi_deal_hdcp_key(char *keydata, int keylen)
{
	int  ret ;
	char buffer_convert[4096];

	if (hdcp_key_parse((unsigned char *)keydata, keylen))
	{
		printf("hdcp_key_parse failed\n");
		return -1;
	}
	hdcp_key_convert((unsigned char *)raw_hdcp_key, (unsigned char *)buffer_convert);
#ifdef	CONFIG_SUNXI_HDCP_HASH
	ret = sunxi_deal_hdcp_key_hash(keydata, buffer_convert);

	if (ret < 0) {
		printf("sunxi_deal_hdcp_key_hash failed\n");
		return -1;
	}
	ret = sunxi_deal_rssk_key();
	if (ret < 0) {
		printf("warning : rsk already burned !!\n");
	}
#endif
	ret = sunxi_secure_object_down("hdcpkey", buffer_convert, SUNXI_HDCP_KEY_LEN,1,0);
	if(ret<0)
	{
		printf("sunxi secure storage write failed\n");
		return -1;
	}

	return 0;
}

