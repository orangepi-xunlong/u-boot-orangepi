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
#include <sunxi_nand.h>



/*normal*/
int sunxi_flash_init(void);
uint sunxi_flash_size(void);
int sunxi_flash_exit(int force);
int sunxi_flash_read(unsigned int start_block, unsigned int nblock,
		     void *buffer);
int sunxi_flash_write(unsigned int start_block, unsigned int nblock,
		      void *buffer);
int sunxi_flash_flush(void);
int sunxi_flash_erase(int erase, void *mbr_buffer);
int sunxi_flash_erase_area(uint start_block, uint nblock);
int sunxi_flash_force_erase(void);

int sunxi_flash_phyread(unsigned int start_block, unsigned int nblock,
			void *buffer);
int sunxi_flash_phywrite(unsigned int start_block, unsigned int nblock,
			 void *buffer);
int check_secure_storage_map(void *buffer);
int sunxi_secstorage_read(int item, unsigned char *buf, unsigned int len);
int sunxi_secstorage_write(int item, unsigned char *buf, unsigned int len);
int sunxi_flash_download_spl(unsigned char *buf, int len, unsigned int ext);
int sunxi_flash_download_toc(unsigned char *buf, int len, unsigned int ext);

/*sprite*/
int sunxi_sprite_init(int boot_mode);
uint sunxi_sprite_size(void);
int sunxi_sprite_exit(int force);
int sunxi_sprite_read(unsigned int start_block, unsigned int nblock,
		      void *buffer);
int sunxi_sprite_write(unsigned int start_block, unsigned int nblock,
		       void *buffer);
int sunxi_sprite_flush(void);
int sunxi_sprite_erase(int erase, void *mbr_buffer);
int sunxi_sprite_force_erase(void);

int sunxi_sprite_phyread(unsigned int start_block, unsigned int nblock,
			 void *buffer);
int sunxi_sprite_phywrite(unsigned int start_block, unsigned int nblock,
			  void *buffer);
int sunxi_sprite_secstorage_read(int item, unsigned char *buf,
				 unsigned int len);
int sunxi_sprite_secstorage_write(int item, unsigned char *buf,
				  unsigned int len);
int sunxi_sprite_download_spl(unsigned char *buf, int len, unsigned int ext);
int sunxi_sprite_download_toc(unsigned char *buf, int len, unsigned int ext);

int sunxi_flash_probe(void);
int sunxi_flash_init_ext(void);

int sunxi_flash_boot_init(int storage_type, int workmode);
int save_fdt_to_flash(void *fdt_buf, size_t fdt_size);

#define SUNXI_SECSTORE_VERSION 1

#define MAX_STORE_LEN 0xc00 /*3K payload*/
#define STORE_OBJECT_MAGIC 0x17253948
#define STORE_REENCRYPT_MAGIC 0x86734716
#define STORE_WRITE_PROTECT_MAGIC 0x8ad3820f
typedef struct {
	unsigned int magic; /* store object magic*/
	int id; /*store id, 0x01,0x02.. for user*/
	char name[64]; /*OEM name*/
	unsigned int re_encrypt; /*flag for OEM object*/
	unsigned int version;
	unsigned int
		write_protect; /*can be placed or not, =0, can be write_protectd*/
	unsigned int reserved[3];
	unsigned int actual_len; /*the actual len in data buffer*/
	unsigned char data[MAX_STORE_LEN]; /*the payload of secure object*/
	unsigned int crc; /*crc to check the sotre_objce valid*/
} store_object_t;

/* secure storage map, have the key info in the keysecure storage */
#define SEC_BLK_SIZE (4096)
#define MAP_KEY_NAME_SIZE	(64)
#define MAP_KEY_DATA_SIZE	(32)
struct map_info {
	unsigned char data[SEC_BLK_SIZE - sizeof(int) * 2];
	unsigned int magic;
	unsigned int crc;
};

#endif /* __SUNXI_FLASH_H__ */
