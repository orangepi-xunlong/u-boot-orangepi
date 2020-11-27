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


#define HDCP_MAIGC		(0x5aa5a55a)
#define HDCP_FILE_SIZE		(352)
#define HDCP_KEY_LEN		(308)
#define AES_16_BYTES_ALIGN	(12)
#define HDCP_BUFFER_LEN         (HDCP_KEY_LEN + AES_16_BYTES_ALIGN)

typedef struct{
	unsigned int magic;                  //Magic number, Value is 0x5aa5a55a
	unsigned int version;                //Version of the structure
	unsigned int len;                    //Data length from start,but not include CRC
	unsigned char rak[16];               //Random 128 bit AES CBC key in small-endian
	unsigned char data[HDCP_BUFFER_LEN]; //HDCP key(308B), Padding HDCP key length to 16 bytes align after encrypt(320B)
	unsigned int crc;                    //CRC32 value of the above data
}hdcp_object;

static unsigned char raw_hdcp_key[HDCP_BUFFER_LEN];


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

	ret = sunxi_secure_object_down("hdcpkey", buffer_convert, SUNXI_HDCP_KEY_LEN,1,0);
	if(ret<0)
	{
		printf("sunxi secure storage write failed\n");
		return -1;
	}

	return 0;
}

