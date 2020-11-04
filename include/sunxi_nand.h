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
#ifndef  _SUNXI_NAND_H
#define  _SUNXI_NAND_H

#include <stdbool.h>

enum size_type {
	BYTE,
	SECTOR,
	PAGE,
	BLOCK
};

/* the interface for nand with nftl */
extern int nand_get_mbr(char* buffer, unsigned int len);
extern int nand_uboot_init(int boot_mode);
extern int nand_uboot_exit(int force);
extern int nand_uboot_probe(void);
extern unsigned int  nand_uboot_read_history(unsigned int start,
		unsigned int sectors, void *buffer);
extern unsigned int nand_uboot_read(unsigned int start, unsigned int sectors,
		void *buffer);
extern unsigned int nand_uboot_write(unsigned int start, unsigned int sectors,
		void *buffer);
extern int ubi_nand_get_flash_info(void *info, unsigned int len);
extern int nand_download_boot0(unsigned int length, void *buffer);
extern int nand_download_uboot(unsigned int length, void *buffer);
extern int nand_force_download_uboot(unsigned int length ,void *buffer);
extern int nand_write_boot0(void *buffer,unsigned int length);
extern int nand_read_boot0(void *buffer,unsigned int length);
extern int nand_uboot_erase(int user_erase);
extern unsigned int nand_uboot_get_flash_info(void *buffer, unsigned int length);
extern unsigned int nand_uboot_set_flash_info(void *buffer, unsigned int length);
extern unsigned int nand_uboot_get_flash_size(void);
extern int nand_uboot_flush(void);
extern int NAND_Uboot_Force_Erase(void);
extern int nand_secure_storage_read(int item, unsigned char *buf,
		unsigned int len);
extern int nand_secure_storage_write(int item, unsigned char *buf,
		unsigned int len);

/* the interface for aw physic nand*/
extern int get_uboot_start_block(void);
extern int get_uboot_next_block(void);
extern int get_physic_block_reserved(void);
extern int nand_secure_storage_first_build(unsigned int start_block);
extern char *nand_get_chip_name(void);
extern void nand_get_chip_id(unsigned char *id, int cnt);
extern unsigned int nand_get_chip_die_cnt(void);
extern int nand_get_chip_page_size(enum size_type type);
extern int nand_get_chip_block_size(enum size_type type);
extern int nand_get_chip_die_size(enum size_type type);
extern unsigned long long nand_get_chip_opt(void);
extern unsigned int nand_get_chip_ddr_opt(void);
extern unsigned int nand_get_chip_ecc_mode(void);
extern unsigned int nand_get_chip_freq(void);
extern unsigned int nand_get_chip_cnt(void);
extern unsigned int  nand_get_twoplane_flag(void);
extern unsigned int nand_get_super_chip_cnt(void);
extern unsigned int nand_get_muti_program_flag(void);
extern unsigned int nand_get_support_v_interleave_flag(void);
extern unsigned int nand_get_super_chip_spare_size(void);
extern unsigned int nand_get_super_chip_page_size(void);
extern unsigned int nand_get_super_chip_block_size(void);
extern unsigned int nand_get_super_chip_pages_offset_to_block(void);
extern unsigned int nand_get_super_chip_size(void);
extern unsigned int nand_get_support_dual_channel(void);
extern unsigned int nand_get_chip_multi_plane_block_offset(void);

extern int nand_physic_read_boot0_page(unsigned int chip, unsigned int block,
		unsigned int page, unsigned int bitmap, unsigned char *mbuf,
		unsigned char *sbuf);
extern int nand_physic_read_page(unsigned int chip, unsigned int block,
		unsigned int page, unsigned int bitmap, unsigned char *mbuf,
		unsigned char *sbuf);
extern int nand_physic_write_page(unsigned int chip, unsigned int block,
		unsigned int page, unsigned int bitmap, unsigned char *mbuf,
		unsigned char *sbuf);
extern int nand_physic_erase_block(unsigned int chip, unsigned int block);
extern int nand_physic_bad_block_check(unsigned int chip, unsigned int block);



/* the interface for nand with ubi */
extern bool nand_use_ubi(void);
extern int ubi_nand_flush(void);
extern int ubi_nand_probe_uboot(void);
extern int ubi_nand_attach_mtd(void);
extern int ubi_nand_exit_uboot(int force);
extern int ubi_nand_init_uboot(int boot_mode);
extern unsigned int ubi_nand_size(void);
extern int ubi_nand_erase(int force);
extern int ubi_nand_force_erase(void);
extern int ubi_nand_read(unsigned int start, unsigned int sectors, void *buffer);
extern int ubi_nand_write(unsigned int start, unsigned int sectors, void *buf);
extern int ubi_nand_download_uboot(unsigned int len, void *buf);
extern int ubi_nand_download_boot0(unsigned int len, void *buf);
extern int ubi_nand_write_end(void);
extern int ubi_nand_update_ubi_env(void);
extern int ubi_nand_secure_storage_read(int item, void *buf, unsigned int len);
extern int ubi_nand_secure_storage_write(int item, void *buf, unsigned int len);


#endif

