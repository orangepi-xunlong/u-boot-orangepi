/*
 * (C) Copyright 2007-2013
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef __SUNXI_FLASH_H__
#define __SUNXI_FLASH_H__
#include <common.h>

//sprite 
extern int  sunxi_sprite_init(int stage);
extern int  sunxi_sprite_erase(int erase, void *mbr_buffer);
extern int  sunxi_sprite_exit(int force);
extern uint sunxi_sprite_size(void);

extern int  sunxi_sprite_read(uint start_block,uint nblock,void * buffer);
extern int  sunxi_sprite_write(uint start_block,uint nblock,void * buffer);
extern int  sunxi_sprite_flush(void);
extern int  sunxi_sprite_phyread(unsigned int start_block, unsigned int nblock, void *buffer);
extern int  sunxi_sprite_phywrite(unsigned int start_block, unsigned int nblock, void *buffer);
extern int sunxi_sprite_force_erase(void);
extern int sunxi_sprite_mmc_phywrite(unsigned int start_block, unsigned int nblock, void *buffer);
extern int sunxi_sprite_mmc_phyread(unsigned int start_block, unsigned int nblock, void *buffer);
extern int sunxi_sprite_mmc_phyerase(unsigned int start_block, unsigned int nblock, void *skip);
extern int sunxi_sprite_mmc_phywipe(unsigned int start_block, unsigned int nblock, void *skip);
extern void board_mmc_pre_init(int card_num);


//normal 
extern int  sunxi_flash_init (int type);
extern uint sunxi_flash_size (void);
extern int  sunxi_flash_exit (int force);
extern int  sunxi_flash_read (unsigned int start_block, unsigned int nblock, void *buffer);
extern int  sunxi_flash_write(unsigned int start_block, unsigned int nblock, void *buffer);
extern int  sunxi_flash_flush(void);
extern int  sunxi_flash_phyread(unsigned int start_block, unsigned int nblock, void *buffer);
extern int  sunxi_flash_phywrite(unsigned int start_block, unsigned int nblock, void *buffer);

//video
extern uint sprite_cartoon_create(void);
extern int  sprite_cartoon_upgrade(int rate);
extern int  sprite_cartoon_destroy(void);


//other
extern int  nand_get_mbr(char* buffer, uint len);
extern int  NAND_build_all_partition(void);
extern int card_erase(int erase, void *mbr_buffer);

#ifdef CONFIG_SUNXI_SPINOR
extern int sunxi_sprite_setdata_finish(void);
extern int spinor_erase(int erase, void *mbr_buffer);
extern int spinor_download_uboot(uint length, void *buffer);
extern int spinor_download_boot0(uint length, void *buffer);
#endif

extern int read_boot_package(int storage_type, void *package_buf);
extern int sunxi_flash_upload_boot0(char * buffer, int size);
extern int sunxi_sprite_download_uboot(void *buffer, int production_media, int generate_checksum);
extern int sunxi_sprite_download_boot0(void *buffer, int production_media);
extern int sunxi_flash_get_boot0_size(void);

extern int nand_force_download_uboot(uint length,void *buffer);
extern uint add_sum(void *buffer, uint length);
extern int sunxi_flash_update_boot0(void);

extern int nand_secure_storage_read( int item, unsigned char *buf, unsigned int len);
extern int nand_secure_storage_write(int item, unsigned char *buf, unsigned int len);

/* for sdmmc secure storage */
extern int sunxi_flash_mmc_secread( int item, unsigned char *buf, unsigned int len);
extern int sunxi_flash_mmc_secread_backup( int item, unsigned char *buf, unsigned int len);
extern int sunxi_flash_mmc_secwrite( int item, unsigned char *buf, unsigned int len);
extern int sunxi_sprite_mmc_secwrite(int item ,unsigned char *buf,unsigned int nblock);
extern int sunxi_sprite_mmc_secread(int item ,unsigned char *buf,unsigned int nblock);
extern int sunxi_sprite_mmc_secread_backup(int item ,unsigned char *buf,unsigned int nblock);

extern int sunxi_secstorage_read(int item, unsigned char *buf, unsigned int len);
extern int sunxi_secstorage_write(int item, unsigned char *buf, unsigned int len);


#define SUNXI_SECSTORE_VERSION	1

#define MAX_STORE_LEN 0xc00 /*3K payload*/
#define STORE_OBJECT_MAGIC	0x17253948
#define STORE_REENCRYPT_MAGIC 0x86734716
#define STORE_WRITE_PROTECT_MAGIC   0x8ad3820f
typedef struct{
	unsigned int     magic ; /* store object magic*/
	int                 id ;    /*store id, 0x01,0x02.. for user*/
	char            name[64]; /*OEM name*/
	unsigned int    re_encrypt; /*flag for OEM object*/
	unsigned int    version ;
	unsigned int    write_protect ;  /*can be placed or not, =0, can be write_protectd*/
	unsigned int    reserved[3];
	unsigned int    actual_len ; /*the actual len in data buffer*/
	unsigned char   data[MAX_STORE_LEN]; /*the payload of secure object*/
	unsigned int    crc ; /*crc to check the sotre_objce valid*/
}store_object_t;


/* secure storage map, have the key info in the keysecure storage */
#define SEC_BLK_SIZE (4096)
struct map_info{
	unsigned char data[SEC_BLK_SIZE - sizeof(int)*2];
	unsigned int magic;
	unsigned int crc;
};



#endif  /* __SUNXI_FLASH_H__ */
