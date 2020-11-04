/*
 *SPDX-License-Identifier: GPL-2.0+
 *
 * (C) Copyright 2018
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 */
#include <common.h>
#include <sunxi_flash.h>
#include <malloc.h>
#include <private_toc.h>
#include <private_boot0.h>
#include <private_uboot.h>
#include <sunxi_board.h>
#include <blk.h>
#include <linux/libfdt.h>
#include "flash_interface.h"
#include "sprite_download.h"


__attribute__((section(".data"))) static sunxi_flash_desc *current_flash;

__attribute__((section(".data"))) static sunxi_flash_desc *sprite_flash;

static int sunxi_flash_init_blk(void);

int sunxi_flash_read(uint start_block, uint nblock, void *buffer)
{
	return current_flash->read(start_block, nblock, buffer);
}

int sunxi_flash_write(uint start_block, uint nblock, void *buffer)
{
	return current_flash->write(start_block, nblock, buffer);
}

int sunxi_flash_flush(void)
{
	return current_flash->flush();
}

int sunxi_flash_erase(int erase, void *mbr_buffer)
{
	return current_flash->erase(erase, mbr_buffer);
}

int sunxi_flash_force_erase(void)
{
	return current_flash->force_erase();
}

int sunxi_flash_phyread(uint start_block, uint nblock, void *buffer)
{
	return current_flash->phyread(start_block, nblock, buffer);
}

int sunxi_flash_phywrite(uint start_block, uint nblock, void *buffer)
{
	return current_flash->phywrite(start_block, nblock, buffer);
}

uint sunxi_flash_size(void)
{
	return current_flash->size();
}

int sunxi_secstorage_read(int item, unsigned char *buf, unsigned int len)
{
	return current_flash->secstorage_read(item, buf, len);
}

int sunxi_secstorage_write(int item, unsigned char *buf, unsigned int len)
{
	return current_flash->secstorage_write(item, buf, len);
}

int sunxi_flash_download_spl(unsigned char *buf, int len, unsigned int ext)
{
	return current_flash->download_spl(buf, len, ext);
}

int sunxi_flash_download_toc(unsigned char *buf, int len, unsigned int ext)
{
	return current_flash->download_toc(buf, len, ext);
}

int sunxi_flash_init(void)
{
	int boot_mode = get_boot_work_mode() == WORK_MODE_BOOT ? 1 : 0;
	return current_flash->init(boot_mode, 0);
}

int sunxi_flash_exit(int force)
{
	return current_flash->exit(force);
}

int sunxi_flash_write_end(void)
{
	int ret = 0;

	if (current_flash->write_end != NULL)
		ret = current_flash->write_end();

	return ret;
}

int sunxi_flash_erase_area(uint start_block, uint nblock)
{
	int ret = 0;
	if (current_flash->erase_area != NULL)
		ret = current_flash->erase_area(start_block, nblock);
	return ret;
}

int sunxi_sprite_read(uint start_block, uint nblock, void *buffer)
{
	return sprite_flash->read(start_block, nblock, buffer);
}

int sunxi_sprite_write(uint start_block, uint nblock, void *buffer)
{
	return sprite_flash->write(start_block, nblock, buffer);
}

int sunxi_sprite_flush(void)
{
	return sprite_flash->flush();
}

int sunxi_sprite_erase(int erase, void *mbr_buffer)
{
	return sprite_flash->erase(erase, mbr_buffer);
}

int sunxi_sprite_force_erase(void)
{
	return sprite_flash->force_erase();
}

int sunxi_sprite_phyread(uint start_block, uint nblock, void *buffer)
{
	return sprite_flash->phyread(start_block, nblock, buffer);
}

int sunxi_sprite_phywrite(uint start_block, uint nblock, void *buffer)
{
	return sprite_flash->phywrite(start_block, nblock, buffer);
}

uint sunxi_sprite_size(void)
{
	return sprite_flash->size();
}

int sunxi_sprite_secstorage_read(int item, unsigned char *buf, unsigned int len)
{
	return sprite_flash->secstorage_read(item, buf, len);
}

int sunxi_sprite_secstorage_write(int item, unsigned char *buf,
				  unsigned int len)
{
	return sprite_flash->secstorage_write(item, buf, len);
}

int sunxi_sprite_download_spl(unsigned char *buf, int len, unsigned int ext)
{
	return sprite_flash->download_spl(buf, len, ext);
}

int sunxi_sprite_download_toc(unsigned char *buf, int len, unsigned int ext)
{
	return sprite_flash->download_toc(buf, len, ext);
}

int sunxi_sprite_init(int boot_mode)
{
	return sprite_flash->init(boot_mode, 2);
}

int sunxi_sprite_exit(int force)
{
	return sprite_flash->exit(force);
}

int sunxi_sprite_write_end(void)
{
	int ret = 0;

	if (sprite_flash->write_end != NULL)
		ret = sprite_flash->write_end();

	return ret;
}

int sunxi_sprite_erase_area(uint start_block, uint nblock)
{
	int ret = 0;

	if (sprite_flash->erase_area != NULL)
		ret = sprite_flash->erase_area(start_block, nblock);

	return ret;
}

int sunxi_flash_boot_init(int storage_type, int workmode)
{
	int state;

	switch (storage_type) {
#ifdef CONFIG_SUNXI_NAND
	case STORAGE_NAND:
	case STORAGE_SPI_NAND: {
		int boot_mode = workmode == WORK_MODE_SPRITE_RECOVERY ? 0 : 1;
		current_flash = &sunxi_nand_desc;
		state	 = current_flash->init(boot_mode, 0);
	} break;
#endif

#ifdef CONFIG_SUNXI_SDMMC
	case STORAGE_SD:
	case STORAGE_EMMC:
	case STORAGE_EMMC3: {
		int card_no;
		//sdmmc handle init
		if (storage_type == STORAGE_SD)
			card_no = 0;
		else if (storage_type == STORAGE_EMMC)
			card_no = 2;
		else
			card_no = 3;
		if (workmode == WORK_MODE_CARD_PRODUCT)
			current_flash = &sunxi_sdmmcs_desc;
		else
			current_flash = &sunxi_sdmmc_desc;
		state		      = current_flash->init(workmode, card_no);
	} break;
#endif

#ifdef CONFIG_SUNXI_SPINOR
	case STORAGE_NOR: {
		current_flash = &sunxi_spinor_desc;
		state	 = current_flash->init(workmode, 0);
	} break;
#endif
	default: {
		pr_err("not support\n");
		state = -1;
	} break;
	}

	if (state != 0) {
		return -1;
	}
	sprite_flash = current_flash;
	tick_printf("sunxi flash init ok\n");
	return 0;
}

int sunxi_flash_probe(void)
{
	int state = 0;

//try emmc, nand, spi-nor
	do {
#ifdef CONFIG_SUNXI_SDMMC
		current_flash = &sunxi_sdmmcs_desc;
		state	 = current_flash->probe();
		if (state == 0)
			break;
		printf("try emmc fail\n");
#endif

#ifdef CONFIG_SUNXI_NAND
		current_flash = &sunxi_nand_desc;
		state	 = current_flash->probe();
		if (state == 0)
			break;
		printf("try nand fail\n");
#endif

#ifdef CONFIG_SUNXI_SPINOR
		current_flash = &sunxi_spinor_desc;
		state	 = current_flash->probe();
		if (state == 0)
			break;

		printf("try spinor fail\n");
#endif

		if (state != 0) {
			return -1;
		}

	} while(0);
	sprite_flash = current_flash;
#ifdef CONFIG_SUNXI_SDMMC
	if (get_boot_work_mode() == WORK_MODE_CARD_PRODUCT) {
		current_flash = &sunxi_sdmmc_desc;
		if (current_flash->init(0, 0))
			return -1;
	}
#endif
	return 0;
}

int sunxi_flash_init_ext(void)
{
	int workmode     = 0;
	int storage_type = 0;
	int state	= 0;

	workmode     = get_boot_work_mode();
	storage_type = get_boot_storage_type();

	tick_printf("workmode = %d,storage type = %d\n", workmode, storage_type);

	if (workmode == WORK_MODE_BOOT ||
	    workmode == WORK_MODE_SPRITE_RECOVERY) {
		state = sunxi_flash_boot_init(storage_type, workmode);
	} else if ((workmode & WORK_MODE_PRODUCT) || (workmode == 0x30)) {
		state = sunxi_flash_probe();
	} else if (workmode & WORK_MODE_UPDATE) {
	} else {
	}
	//init blk dev
	sunxi_flash_init_blk();

	return state;
}


#ifdef CONFIG_SUNXI_FDT_SAVE
int read_boot_package(int storage_type, void *package_buf)
{

	int total_length = 0;
	int read_len = uboot_spare_head.boot_data.boot_package_size;
	int ret = 0;
	sbrom_toc1_head_info_t *toc1_head = NULL;

	debug("boot package size: 0x%x\n", read_len);
	extern  int nand_read_uboot_data(unsigned char *buf, unsigned int len);
	switch (storage_type) {
#ifdef CONFIG_SUNXI_NAND
	case STORAGE_NAND:
		ret = nand_read_uboot_data(package_buf, read_len);
		break;
#endif
#ifdef CONFIG_SUNXI_SDMMC
	case STORAGE_EMMC:
	case STORAGE_SD:
	case STORAGE_EMMC3:
		ret = sunxi_flash_phyread(UBOOT_START_SECTOR_IN_SDMMC, read_len/512, package_buf);
		break;
#endif
#ifdef CONFIG_SUNXI_SPINOR
	case STORAGE_NOR:
		ret = sunxi_flash_phyread(CONFIG_SPINOR_UBOOT_OFFSET, read_len/512, package_buf);
		break;
#endif
	default:
		pr_error("%s:not support storage type %d\n", __func__, storage_type);
		ret = -1;
		break;
	}
	toc1_head = (struct sbrom_toc1_head_info *)package_buf;
	if (toc1_head->magic != TOC_MAIN_INFO_MAGIC) {
		pr_error("toc1 magic error\n");
		return -1;
	}
	total_length = toc1_head->valid_len;

	debug("read uboot from flash: ret %d\n", ret);
	return total_length;
}

int save_fdt_to_flash(void *fdt_buf, size_t fdt_size)
{
	int package_size;
	int i = 0;
	char *package_buf = NULL;
	int   package_buf_size;
	int   find_flag = 0;
	u32   dtb_base = 0;
	int   storage_type = 0;
	int   ret = -1;

	struct sbrom_toc1_head_info  *toc1_head = NULL;
	struct sbrom_toc1_item_info  *item_head = NULL;
	struct sbrom_toc1_item_info  *toc1_item = NULL;

	storage_type = get_boot_storage_type();

	/*1M buffer*/
	if (storage_type == STORAGE_NOR || storage_type == STORAGE_SPI_NAND) {
		package_buf_size = 1 << 20;
	} else {
		/*10M buffer*/
		package_buf_size = 10 << 20;
	}
	package_buf = (char *)memalign(CONFIG_SYS_CACHELINE_SIZE, package_buf_size);
	if (package_buf == NULL)
		return -1;

	memset(package_buf, 0, package_buf_size);
	package_size = read_boot_package(storage_type, package_buf);
	if (package_size <= 0) {
		goto _UPDATE_END;
	}

	toc1_head = (struct sbrom_toc1_head_info *)package_buf;
	item_head = (struct sbrom_toc1_item_info *)(package_buf + sizeof(struct sbrom_toc1_head_info));
	toc1_item = item_head;
	for (i = 0; i < toc1_head->items_nr; i++, toc1_item++) {
		debug("Entry_name		  = %s\n",	 toc1_item->name);
		if (strncmp(toc1_item->name, ITEM_DTB_NAME, sizeof(ITEM_DTB_NAME)) == 0) {
			find_flag = 1;
			break;
		}
	}
	if (!find_flag) {
		pr_error("error:can't find dtb\n");
		goto _UPDATE_END;
	}

	dtb_base = (u32)(package_buf + toc1_item->data_offset);
	if (fdt_check_header((void *)dtb_base)) {
		pr_error("%s: fdt header is error\n", __func__);
		goto _UPDATE_END;
	}

	if (fdt_size > CONFIG_SUNXI_DTB_RESERVE_SIZE) {
		pr_error("fdt size is too large\n");
		goto _UPDATE_END;
	}

	memcpy((void *)dtb_base, fdt_buf, fdt_size);

	ret = sunxi_sprite_download_uboot(package_buf, storage_type, 1);

_UPDATE_END:
	if (package_buf) {
		free(package_buf);
	}
	return ret;

}
#endif
static struct blk_desc sunxi_flash_blk_dev;

static unsigned long sunxi_block_read(struct blk_desc *block_dev,
				      lbaint_t start, lbaint_t blkcnt,
				      void *buffer)
{
	int storage_type = get_boot_storage_type();
	/* debug("addr = %d, len = %d\n", (uint)start, (uint)blkcnt); */
	if (get_boot_work_mode() == WORK_MODE_CARD_PRODUCT ||
		(storage_type == STORAGE_SD) || (storage_type == STORAGE_EMMC))
		return sunxi_flash_phyread((uint)start, (uint)blkcnt, (void *)buffer);
	else
		return sunxi_flash_read((uint)start, (uint)blkcnt, (void *)buffer);
}

static unsigned long sunxi_block_write(struct blk_desc *block_dev,
				       lbaint_t start, lbaint_t blkcnt,
				       const void *buffer)
{
	int storage_type = get_boot_storage_type();
	/* debug("addr = %d, len = %d\n", (uint)start, (uint)blkcnt); */
	if (get_boot_work_mode() == WORK_MODE_CARD_PRODUCT ||
		(storage_type == STORAGE_SD) || (storage_type == STORAGE_EMMC))
		return sunxi_flash_phywrite((uint)start, (uint)blkcnt, (void *)buffer);
	else
		return sunxi_flash_write((uint)start, (uint)blkcnt, (void *)buffer);
}

int sunxi_flash_init_blk(void)
{
	debug("sunxi flash init uboot\n");
	sunxi_flash_blk_dev.if_type   = IF_TYPE_SUNXI_FLASH;
	sunxi_flash_blk_dev.part_type = PART_TYPE_EFI;
	sunxi_flash_blk_dev.devnum    = 0;
	sunxi_flash_blk_dev.lun       = 0;
	sunxi_flash_blk_dev.type      = 0;

	/* FIXME fill in the correct size (is set to 32MByte) */
	sunxi_flash_blk_dev.blksz       = 512;
	sunxi_flash_blk_dev.lba		= 0;
	sunxi_flash_blk_dev.removable   = 0;
	sunxi_flash_blk_dev.block_read  = sunxi_block_read;
	sunxi_flash_blk_dev.block_write = sunxi_block_write;

	/* Setup the universal parts of the block interface just once */

	return 0;
}

static int sunxi_flash_get_dev(int dev, struct blk_desc **descp)
{
	sunxi_flash_blk_dev.devnum = dev;
	sunxi_flash_blk_dev.lba    = sunxi_flash_size();

	*descp = &sunxi_flash_blk_dev;

	return 0;
}

U_BOOT_LEGACY_BLK(sunxi_flash) = {
	.if_typename   = "sunxi_flash",
	.if_type       = IF_TYPE_SUNXI_FLASH,
	.max_devs      = -1,
	.get_dev       = sunxi_flash_get_dev,
	.select_hwpart = NULL,
};
