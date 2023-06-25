
// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 */

#ifndef _SUNXI_FLASH_INTERFACE_
#define _SUNXI_FLASH_INTERFACE_


typedef struct _sunxi_flash_desc {

	lbaint_t	lba;		/* number of blocks */
	unsigned long	blksz;		/* block size */
	int (*probe)(void);
	int (*init)(int stage, int);
	int (*exit)(int force);
	int	(*read)(uint start_block, uint nblock, void *buffer);
	int	(*write)(uint start_block, uint nblock, void *buffer);
	int (*erase)(int erase, void *mbr_buffer);
	int (*force_erase)(void);
	int (*flush)(void);
	uint (*size)(void) ;
	int (*phyread) (unsigned int start_block, unsigned int nblock, void *buffer);
	int (*phywrite)(unsigned int start_block, unsigned int nblock, void *buffer);
	int (*phyerase)(unsigned int start_block, unsigned int nblock, void *skip);
	int (*secstorage_read)( int item, unsigned char *buf, unsigned int len) ;
	int (*secstorage_write) (int item, unsigned char *buf, unsigned int len);
	int (*secstorage_flush) (void);
	int (*secstorage_fast_write) (int item, unsigned char *buf, unsigned int len);
	int (*download_spl) (unsigned char *buf, int len, unsigned int ext);
	int (*download_toc) (unsigned char *buf, int len, unsigned int ext);
	int (*write_end) (void);
	int (*erase_area)(uint start_bloca, uint nblock);
	int (*update_backup_boot0)(void);

}sunxi_flash_desc;


extern sunxi_flash_desc sunxi_nand_desc;
extern sunxi_flash_desc sunxi_sdmmc_desc;
extern sunxi_flash_desc sunxi_spinor_desc;

extern sunxi_flash_desc sunxi_sdmmcs_desc;


#endif
