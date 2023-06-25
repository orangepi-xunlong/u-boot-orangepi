/*
 * (C) Copyright 2018 allwinnertech  <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __SECURE_STORAGE_H__
#define __SECURE_STORAGE_H__

extern int sunxi_secure_storage_init(void);
extern int sunxi_secure_storage_exit(void);

extern int sunxi_secure_storage_list(void);
extern int sunxi_secure_storage_probe(const char *item_name);
extern int sunxi_secure_storage_read(const char *item_name, char *buffer, int length, int *data_len);

extern int sunxi_secure_storage_write(const char *item_name, char *buffer, int length);
extern int sunxi_secure_storage_erase(const char *item_name);
extern int sunxi_secure_storage_erase_all(void);
extern int sunxi_secure_storage_erase_data_only(const char *item_name);

extern int sunxi_secure_object_build(const char *name, char *buf, int len,
				     int encrypt, int write_protect,
				     char *secdata_buf);
extern int sunxi_secure_object_down( const char *name , char *buf, int len, int encrypt, int write_protect);
extern int sunxi_secure_object_up(const char *name,char *buf,int len);

extern int sunxi_secure_object_set(const char *item_name, int encyrpt,int replace, int, int, int);
extern int sunxi_secure_object_write(const char *item_name, char *buffer, int length);
extern int sunxi_secure_object_read(const char *item_name, char *buffer, int buffer_len, int *data_len);


extern int smc_load_sst_encrypt(
		char *name,
		char *in, unsigned int len,
		char *out, unsigned int *outLen);

#define SUNXI_SECURE_STORTAGE_INFO_HEAD_LEN (64 + 4 + 4 + 4)
#define SUNXI_HDCP_KEY_LEN (288)
typedef struct
{
	char     name[64];      //key name
	uint32_t len;           //the len fo key_data
	uint32_t encrypted;
	uint32_t write_protect;
	char    key_data[4096 - SUNXI_SECURE_STORTAGE_INFO_HEAD_LEN];//the raw data of key
}
sunxi_secure_storage_info_t;

#define SECURE_STORAGE_DUMMY_KEY_NAME "reserved_after_delete"

#endif
