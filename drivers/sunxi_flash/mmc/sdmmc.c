// SPDX-License-Identifier: GPL-2.0+
/*
 * sunxi SPI driver for uboot.
 *
 * Copyright (C) 2018
 * 2018.11.7 lixiang <lixiang@allwinnertech.com>
 */
#include <common.h>
#include <sys_config.h>
/*#include <sys_partition.h>*/
/*#include <boot_type.h>*/
#include "private_uboot.h"
#include <private_toc.h>
#include <private_boot0.h>
#include <sunxi_flash.h>
#include <sunxi_board.h>
#include <mmc.h>
#include <malloc.h>
#include <blk.h>
#include <asm/global_data.h>

#include "../flash_interface.h"
#include "../../mmc/mmc_def.h"

#define SUNXI_MMC_BOOT0_START_ADDRS	(16)
#define SUNXI_MMC_TOC_START_ADDRS	(32800)

static int  mmc_has_pre_init;
static int sunxi_flash_mmc_init(int stage, int card_no);
int sdmmc_init_for_sprite(int workmode, int card_no);
int sdmmc_init_for_boot(int workmode, int card_no);
static int mmc_secure_storage_read_key(int item, unsigned char *buf,
				       unsigned int len);
static int mmc_secure_storage_read_map(int item, unsigned char *buf,
				       unsigned int len);

static struct mmc *mmc_boot, *mmc_sprite;
static int mmc_no;
static unsigned char _inner_buffer[4096 + 64]; /*align temp buffer*/

/*
 *
 * normal boot interface
 *
 */

int sunxi_flash_mmc_probe(void)
{
	return 0;
}

static int sunxi_flash_mmc_init(int stage, int card_no)
{
	return sdmmc_init_for_boot(stage, card_no);
}

static int sunxi_flash_mmc_exit(int force)
{
	return mmc_exit();
}

static int sunxi_flash_mmc_read(unsigned int start_block, unsigned int nblock,
				void *buffer)
{
	debug("mmcboot read: start 0x%x, sector 0x%x\n", start_block, nblock);

	return mmc_boot->block_dev.block_read(
	    &mmc_boot->block_dev, start_block + CONFIG_MMC_LOGICAL_OFFSET,
	    nblock, buffer);
}

static int sunxi_flash_mmc_write(unsigned int start_block, unsigned int nblock,
				 void *buffer)
{
	debug("mmcboot write: start 0x%x, sector 0x%x\n", start_block, nblock);

	return mmc_boot->block_dev.block_write(
	    &mmc_boot->block_dev, start_block + CONFIG_MMC_LOGICAL_OFFSET,
	    nblock, buffer);
}

int card_verify_boot0(uint start_block, uint length);
static int sunxi_flash_mmc_download_spl(unsigned char *buf, int len,
					unsigned int ext)
{
	uint32_t i, fail_count;
	uint32_t write_offset[2] = { CONFIG_SUNXI_BOOT0_SDMMC_BACKUP_START_ADDR,
				     SUNXI_MMC_BOOT0_START_ADDRS };

#ifdef CONFIG_SUNXI_OTA_TURNNING
	if (sunxi_get_active_boot0_id() != 0) {
		write_offset[0] = SUNXI_MMC_BOOT0_START_ADDRS;
		write_offset[1] = CONFIG_SUNXI_BOOT0_SDMMC_BACKUP_START_ADDR;
	}
#endif

	fail_count = 0;
	for (i = 0; i < 2; i++) {
		if (!mmc_boot->block_dev.block_write(&mmc_boot->block_dev,
						     write_offset[i], len / 512,
						     buf)) {
			pr_force("%s: write %s failed\n", __func__,
				 write_offset[i] ==
						 SUNXI_MMC_BOOT0_START_ADDRS ?
					 "main spl" :
					 "back spl");
			fail_count++;
		} else {
			pr_force("%s: write %s done\n", __func__,
				 write_offset[i] ==
						 SUNXI_MMC_BOOT0_START_ADDRS ?
					 "main spl" :
					 "back spl");
		}
		if (card_verify_boot0(write_offset[i], len) < 0) {
			return -1;
		}
	}
	if (fail_count == 2) {
		return -1;
	}

	return 0;
}

static int
sunxi_flash_mmc_download_toc(unsigned char *buf, int len,  unsigned int ext)
{
	if (!mmc_boot->block_dev.block_write(&mmc_boot->block_dev,
					       SUNXI_MMC_TOC_START_ADDRS,
					       len / 512, buf)) {
		pr_err("%s: write main uboot failed\n");
		return -1;
	}

	if (!mmc_boot->block_dev.block_write(
		    &mmc_boot->block_dev, UBOOT_BACKUP_START_SECTOR_IN_SDMMC,
		    len / 512, buf)) {
		pr_err("%s: write back uboot failed\n");
		return -1;
	}

	return 0;
}

static uint sunxi_flash_mmc_size(void)
{

	return mmc_boot->block_dev.lba;
}

int sunxi_flash_mmc_phyread(unsigned int start_block, unsigned int nblock,
			    void *buffer)
{
	return mmc_boot->block_dev.block_read(&mmc_boot->block_dev, start_block,
					      nblock, buffer);
}

int sunxi_flash_mmc_phywrite(unsigned int start_block, unsigned int nblock,
			     void *buffer)
{
	return mmc_boot->block_dev.block_write(&mmc_boot->block_dev,
					       start_block, nblock, buffer);
}

int sunxi_flash_mmc_flush(void)
{
	return 0;
}

int sunxi_flash_mmc_phyerase(unsigned int start_block, unsigned int nblock,
				void *skip)
{
	if (nblock == 0) {
		printf("%s: @nr is 0, erase from @from to end\n", __FUNCTION__);
		nblock = mmc_boot->block_dev.lba - start_block - 1;
	}
	return mmc_boot->block_dev.block_mmc_erase(&mmc_boot->block_dev,
		start_block, nblock, skip);
}
/*
 *
 * normal sprite interface
 *
 */

int sunxi_sprite_mmc_probe(void)
{
#ifndef CONFIG_MACH_SUN50IW11
	return sdmmc_init_for_sprite(0, 2);
#else
	int workmode = uboot_spare_head.boot_data.work_mode;
	if (workmode == WORK_MODE_CARD_PRODUCT)
		return -1;
	else
		return sdmmc_init_for_sprite(0, 0);
#endif
}

static int sunxi_sprite_mmc_init(int stage, int card_no)
{
	return sdmmc_init_for_sprite(stage, card_no);
}

static int sunxi_sprite_mmc_exit(int force)
{
	return 0;
}

static int sunxi_sprite_mmc_read(unsigned int start_block, unsigned int nblock,
				void *buffer)
{
	debug("mmcboot read: start 0x%x, sector 0x%x\n", start_block, nblock);

	return mmc_sprite->block_dev.block_read(
	    &mmc_sprite->block_dev, start_block + CONFIG_MMC_LOGICAL_OFFSET,
	    nblock, buffer);
}

static int sunxi_sprite_mmc_write(unsigned int start_block, unsigned int nblock,
				 void *buffer)
{
	debug("mmcboot write: start 0x%x, sector 0x%x\n", start_block, nblock);

	return mmc_sprite->block_dev.block_write(
	    &mmc_sprite->block_dev, start_block + CONFIG_MMC_LOGICAL_OFFSET,
	    nblock, buffer);
}

static int sunxi_sprite_mmc_erase(int erase, void *mbr_buffer)
{
	return card_erase(erase, mbr_buffer);
}

int sunxi_sprite_mmc_flush(void)
{
	return 0;
}

static uint sunxi_sprite_mmc_size(void)
{

	return mmc_sprite->block_dev.lba;
}

int sunxi_sprite_mmc_phyread(unsigned int start_block, unsigned int nblock,
			     void *buffer)
{
	return mmc_sprite->block_dev.block_read(&mmc_sprite->block_dev,
						start_block, nblock, buffer);
}

int sunxi_sprite_mmc_phywrite(unsigned int start_block, unsigned int nblock,
			      void *buffer)
{
	return mmc_sprite->block_dev.block_write(&mmc_sprite->block_dev,
						 start_block, nblock, buffer);
}

int sunxi_sprite_mmc_phyerase(unsigned int start_block, unsigned int nblock,
				void *skip)
{
	if (nblock == 0) {
		printf("%s: @nr is 0, erase from @from to end\n", __FUNCTION__);
		nblock = mmc_sprite->block_dev.lba - start_block - 1;
	}
	return mmc_sprite->block_dev.block_mmc_erase(&mmc_sprite->block_dev,
		start_block, nblock, skip);
}

int sunxi_sprite_mmc_phywipe(unsigned int start_block, unsigned int nblock,
			     void *skip)
{
	if (nblock == 0) {
		printf("%s: @nr is 0, wipe from @from to end\n", __FUNCTION__);
		nblock = mmc_sprite->block_dev.lba - start_block - 1;
	}
	return mmc_sprite->block_dev.block_mmc_secure_wipe(&mmc_sprite->block_dev,
			start_block, nblock, skip);
}

int sunxi_sprite_mmc_force_erase(void)
{
	unsigned int skip_space[1 + 2 * 2] = { 0 };

	return sunxi_sprite_phyerase(0, mmc_sprite->block_dev.lba, skip_space);
}

static int sunxi_sprite_mmc_download_spl(unsigned char *buf, int len,
					 unsigned int ext)
{
	if (!mmc_sprite->block_dev.block_write(&mmc_sprite->block_dev,
					       SUNXI_MMC_BOOT0_START_ADDRS,
					       len / 512, buf)) {
		pr_err("%s: write main spl failed\n");
		return -1;
	}

	if (!mmc_sprite->block_dev.block_write(&mmc_sprite->block_dev,
					       CONFIG_SUNXI_BOOT0_SDMMC_BACKUP_START_ADDR,
					       len / 512, buf)) {
		pr_err("%s: write backup spl failed\n");
		return -1;
	}

	return 0;
}

static int sunxi_sprite_mmc_download_toc(unsigned char *buf, int len,
					 unsigned int ext)
{
	if (!mmc_sprite->block_dev.block_write(&mmc_sprite->block_dev,
					       SUNXI_MMC_TOC_START_ADDRS,
					       len / 512, buf)) {
		pr_err("%s: write main uboot failed\n");
		return -1;
	}

	if (!mmc_sprite->block_dev.block_write(
		    &mmc_sprite->block_dev, UBOOT_BACKUP_START_SECTOR_IN_SDMMC,
		    len / 512, buf)) {
		pr_err("%s: write back uboot failed\n");
		return -1;
	}

	return 0;
}

/*
 *
 * secure boot interface
 *
 */

int sunxi_flash_mmc_secread(int item, unsigned char *buf, unsigned int nblock)
{
	int ret = 0;
	if (buf == NULL) {
		printf("input buf is NULL\n");
		ret = -1;
		goto OUT;
	}

	if (item > MAX_SECURE_STORAGE_MAX_ITEM) {
		printf("item exceed %d\n", MAX_SECURE_STORAGE_MAX_ITEM);
		ret = -1;
		goto OUT;
	}

	if (nblock > SDMMC_ITEM_SIZE) {
		printf("block count exceed %d\n", SDMMC_ITEM_SIZE);
		ret = -1;
		goto OUT;
	}
	if (mmc_boot->block_dev.block_read(&mmc_boot->block_dev,
		SDMMC_SECURE_STORAGE_START_ADD + SDMMC_ITEM_SIZE * 2 * item,
		nblock, buf) != nblock) {
		printf("read first backup failed in fun %s line %d\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto OUT;
	}
OUT:
	return ret;
}

int sunxi_flash_mmc_secread_backup(int item, unsigned char *buf,
				   unsigned int nblock)
{
	int ret = 0;
	if (buf == NULL) {
		printf("input buf is NULL\n");
		ret = -1;
		goto OUT;
	}

	if (item > MAX_SECURE_STORAGE_MAX_ITEM) {
		printf("item exceed %d\n", MAX_SECURE_STORAGE_MAX_ITEM);
		ret = -1;
		goto OUT;
	}

	if (nblock > SDMMC_ITEM_SIZE) {
		printf("block count exceed %d\n", SDMMC_ITEM_SIZE);
		ret = -1;
		goto OUT;
	}

	if (mmc_boot->block_dev.block_read(&mmc_boot->block_dev,
		SDMMC_SECURE_STORAGE_START_ADD + SDMMC_ITEM_SIZE * 2 * item + SDMMC_ITEM_SIZE,
		nblock, buf) != nblock) {
		printf("read second backup failed in fun %s line %d\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto OUT;
	}
OUT:
	return ret;
}

int sunxi_flash_mmc_secwrite(int item, unsigned char *buf, unsigned int nblock)
{
	int ret = 0;
	if (buf == NULL) {
		printf("input buf is NULL\n");
		ret = -1;
		goto OUT;
	}

	if (item > MAX_SECURE_STORAGE_MAX_ITEM) {
		printf("item exceed %d\n", MAX_SECURE_STORAGE_MAX_ITEM);
		ret = -1;
		goto OUT;
	}

	if (nblock > SDMMC_ITEM_SIZE) {
		printf("block count exceed %d\n", SDMMC_ITEM_SIZE);
		ret = -1;
		goto OUT;
	}

	if (mmc_boot->block_dev.block_write(&mmc_boot->block_dev,
		SDMMC_SECURE_STORAGE_START_ADD + SDMMC_ITEM_SIZE * 2 * item,
		nblock, buf) != nblock) {
		printf("write first backup failed in fun %s line %d\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto OUT;
	}

	if (mmc_boot->block_dev.block_write(&mmc_boot->block_dev,
		SDMMC_SECURE_STORAGE_START_ADD + SDMMC_ITEM_SIZE * 2 * item + SDMMC_ITEM_SIZE,
		nblock, buf) != nblock) {
		printf("write second backup failed in fun %s line %d\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto OUT;
	}
OUT:
	return ret;
}

/*
 *
 * secure sprite interface
 *
 */

int sunxi_sprite_mmc_secread(int item, unsigned char *buf, unsigned int nblock)
{
	int ret = 0;
	if (buf == NULL) {
		printf("input buf is NULL\n");
		ret = -1;
		goto OUT;
	}

	if (item > MAX_SECURE_STORAGE_MAX_ITEM) {
		printf("item exceed %d\n", MAX_SECURE_STORAGE_MAX_ITEM);
		ret = -1;
		goto OUT;
	}

	if (nblock > SDMMC_ITEM_SIZE) {
		printf("block count exceed %d\n", SDMMC_ITEM_SIZE);
		ret = -1;
		goto OUT;
	}
	if (mmc_sprite->block_dev.block_read(&mmc_sprite->block_dev,
		SDMMC_SECURE_STORAGE_START_ADD + SDMMC_ITEM_SIZE * 2 * item,
		nblock, buf) != nblock) {
		printf("read first backup failed in fun %s line %d\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto OUT;
	}
OUT:
	return ret;
}

int sunxi_sprite_mmc_secread_backup(int item, unsigned char *buf,
				    unsigned int nblock)
{
	int ret = 0;
	if (buf == NULL) {
		printf("input buf is NULL\n");
		ret = -1;
		goto OUT;
	}

	if (item > MAX_SECURE_STORAGE_MAX_ITEM) {
		printf("item exceed %d\n", MAX_SECURE_STORAGE_MAX_ITEM);
		ret = -1;
		goto OUT;
	}

	if (nblock > SDMMC_ITEM_SIZE) {
		printf("block count exceed %d\n", SDMMC_ITEM_SIZE);
		ret = -1;
		goto OUT;
	}

	if (mmc_sprite->block_dev.block_read(&mmc_sprite->block_dev,
		SDMMC_SECURE_STORAGE_START_ADD + SDMMC_ITEM_SIZE * 2 * item + SDMMC_ITEM_SIZE,
		nblock, buf) != nblock) {
		printf("read second backup failed in fun %s line %d\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto OUT;
	}
OUT:
	return ret;
}

int sunxi_sprite_mmc_secwrite(int item, unsigned char *buf, unsigned int nblock)
{
	int ret = 0;
	if (buf == NULL) {
		printf("input buf is NULL\n");
		ret = -1;
		goto OUT;
	}

	if (item > MAX_SECURE_STORAGE_MAX_ITEM) {
		printf("item exceed %d\n", MAX_SECURE_STORAGE_MAX_ITEM);
		ret = -1;
		goto OUT;
	}

	if (nblock > SDMMC_ITEM_SIZE) {
		printf("block count exceed %d\n", SDMMC_ITEM_SIZE);
		ret = -1;
		goto OUT;
	}

	if (mmc_sprite->block_dev.block_write(&mmc_sprite->block_dev,
		SDMMC_SECURE_STORAGE_START_ADD + SDMMC_ITEM_SIZE * 2 * item,
		nblock, buf) != nblock) {
		printf("write first backup failed in fun %s line %d\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto OUT;
	}

	if (mmc_sprite->block_dev.block_write(&mmc_sprite->block_dev,
		SDMMC_SECURE_STORAGE_START_ADD + SDMMC_ITEM_SIZE * 2 * item + SDMMC_ITEM_SIZE,
		nblock, buf) != nblock) {
		printf("write second backup failed in fun %s line %d\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto OUT;
	}
OUT:
	return ret;
}

int mmc_secure_storage_read(int item, unsigned char *buf, unsigned int len)
{
	if (item == 0)
		return mmc_secure_storage_read_map(item, buf, len);
	else
		return mmc_secure_storage_read_key(item, buf, len);
}

int mmc_secure_storage_write(int item, unsigned char *buf, unsigned int len)
{
	unsigned char *align;
	unsigned int blkcnt;
	int workmode;

	if (PT_TO_PHU(buf) % 32) { // input buf not align
		align = (unsigned char *)((PT_TO_PHU(_inner_buffer) + 0x20) &
					  (~0x1f));
		memcpy(align, buf, len);
	} else
		align = buf;

	blkcnt = (len + 511) / 512;
	workmode = uboot_spare_head.boot_data.work_mode;
	if (workmode == WORK_MODE_BOOT ||
	    workmode == WORK_MODE_SPRITE_RECOVERY) {
		return sunxi_flash_mmc_secwrite(item, align, blkcnt);
	} else if ((workmode & WORK_MODE_PRODUCT) || (workmode == 0x30)) {
		return sunxi_sprite_mmc_secwrite(item, align, blkcnt);
	} else {
		printf("workmode=%d is err\n", workmode);
		return -1;
	}
}

static int mmc_mirror_boo0_copy(uint32_t src, uint32_t dst)
{
	int ret, size;
	u8 buffer[512];
	u8 *verify_buffer = NULL;
	u8 *boot_buffer	  = NULL;
	uint32_t check_sum;
	int i;
	const int write_retry_cnt = 3;
	ret			  = sunxi_flash_mmc_phyread(src, 1, buffer);
	if (!ret) {
		pr_error("sunxi_flash_mmc_phyread fail\n");
		return -1;
	}

	if (sunxi_get_secureboard()) {
		toc0_private_head_t *toc0 = NULL;
		toc0			  = (toc0_private_head_t *)buffer;
		if (strncmp((const char *)toc0->name, TOC0_MAGIC, MAGIC_SIZE)) {
			pr_error("src toc0 magic is bad\n");
			return -1;
		}
		check_sum = toc0->check_sum;
		size	  = toc0->length;
	} else {
		boot0_file_head_t *boot0;
		boot0 = (boot0_file_head_t *)buffer;
		if (strncmp((const char *)boot0->boot_head.magic, BOOT0_MAGIC,
			    MAGIC_SIZE)) {
			pr_error("src boot0 magic is bad\n");
			return -1;
		}
		check_sum = boot0->boot_head.check_sum;
		size	  = boot0->boot_head.length;
	}

	boot_buffer = (u8 *)malloc(size);
	if (!boot_buffer) {
		pr_error("malloc buf fail\n");
		return -1;
	}
	verify_buffer = (u8 *)malloc(size);
	if (!verify_buffer) {
		pr_error("malloc buf fail\n");
		goto MIRROR_FAILED_;
	}

	ret = sunxi_flash_mmc_phyread(src, size / 512, boot_buffer);
	if (!ret) {
		pr_error("sunxi_flash_mmc_phyread fail\n");
		goto MIRROR_FAILED_;
	}

	if (sunxi_verify_checksum(boot_buffer, size, check_sum)) {
		pr_error("boot0 checksum is error\n");
		goto MIRROR_FAILED_;
	}

	for (i = 0; i < write_retry_cnt; i++) {
		ret = sunxi_flash_mmc_phywrite(dst, size / 512, boot_buffer);
		if (!ret) {
			pr_error("sunxi_flash_mmc_phywrite backup fail\n");
			continue;
		}

		memset(verify_buffer, 0, size);
		ret = sunxi_flash_mmc_phyread(dst, size / 512, verify_buffer);
		if (!ret) {
			pr_error("sunxi_flash_mmc_phyread backup fail\n");
			continue;
		}

		if (sunxi_verify_checksum(verify_buffer, size, check_sum)) {
			pr_error("recovery boot0 checksum is error\n");
			continue;
		}
	}
	if (boot_buffer)
		free(boot_buffer);
	if (verify_buffer)
		free(verify_buffer);
	return 0;

MIRROR_FAILED_:
	if (boot_buffer)
		free(boot_buffer);
	if (verify_buffer)
		free(verify_buffer);
	return -1;
}

#ifdef CONFIG_SUNXI_RECOVERY_BOOT0_COPY0
int sunxi_flash_mmc_recover_boot0_copy0(void)
{
	if (sunxi_get_active_boot0_id() != 0) {
		int ret;
		ret = mmc_mirror_boo0_copy(
			CONFIG_SUNXI_BOOT0_SDMMC_BACKUP_START_ADDR,
			SUNXI_MMC_BOOT0_START_ADDRS);
		if (0 == ret) {
			pr_force("recover boot0 copy0 ok\n");
		} else {
			pr_force("recover boot0 copy0 failed\n");
		}
		return ret;
	}
	pr_debug("boot0 copy0 ok, do not need recover");
	return 0;
}
#endif

int sunxi_flash_update_backup_boot0(void)
{
	int ret;
	u8 buffer[512];
	toc0_private_head_t *toc0 = NULL;

	if (sunxi_get_securemode() == SUNXI_NORMAL_MODE) {
		pr_msg("non secure, do not need update backup boot0 to toc0\n");
		return 0;
	}

	/*check if backup already a toc0*/
	memset(buffer, 0x0, 512);
	ret = sunxi_flash_mmc_phyread(
		CONFIG_SUNXI_BOOT0_SDMMC_BACKUP_START_ADDR, 1, buffer);
	if (!ret) {
		pr_error("sunxi_flash_mmc_phyread backup fail\n");
		return -1;
	}

	toc0 = (toc0_private_head_t *)buffer;
	if (strncmp((const char *)toc0->name, TOC0_MAGIC, MAGIC_SIZE) == 0) {
		pr_msg("toc0 magic is ok\n");
		return 0;
	}

	printf("update emmc backup boot0 start\n");
	if (0 ==
	    mmc_mirror_boo0_copy(SUNXI_MMC_BOOT0_START_ADDRS,
				 CONFIG_SUNXI_BOOT0_SDMMC_BACKUP_START_ADDR)) {
		printf("update emmc backup boot0 ok\n");
		return 0;
	} else {
		printf("update emmc backup boot0 failed\n");
		return -1;
	}
}

sunxi_flash_desc sunxi_sdmmc_desc = {
    .probe = sunxi_flash_mmc_probe,
    .init = sunxi_flash_mmc_init,
    .exit = sunxi_flash_mmc_exit,
    .read = sunxi_flash_mmc_read,
    .write = sunxi_flash_mmc_write,
    .erase = sunxi_sprite_mmc_erase,
    .flush = sunxi_flash_mmc_flush,
    .size = sunxi_flash_mmc_size,
#ifdef CONFIG_SUNXI_SECURE_STORAGE
    .secstorage_read = mmc_secure_storage_read,
    .secstorage_write = mmc_secure_storage_write,
#endif
    .phyread = sunxi_flash_mmc_phyread,
    .phywrite = sunxi_flash_mmc_phywrite,
    .phyerase = sunxi_flash_mmc_phyerase,
    .download_spl = sunxi_flash_mmc_download_spl,
    .download_toc = sunxi_flash_mmc_download_toc,
    .update_backup_boot0 = sunxi_flash_update_backup_boot0,
};

sunxi_flash_desc sunxi_sdmmcs_desc = {
    .probe = sunxi_sprite_mmc_probe,
    .init = sunxi_sprite_mmc_init,
    .exit = sunxi_sprite_mmc_exit,
    .read = sunxi_sprite_mmc_read,
    .write = sunxi_sprite_mmc_write,
    .erase = sunxi_sprite_mmc_erase,
    .force_erase = sunxi_sprite_mmc_force_erase,
    .flush = sunxi_sprite_mmc_flush,
    .size = sunxi_sprite_mmc_size,
#ifdef CONFIG_SUNXI_SECURE_STORAGE
    .secstorage_read = mmc_secure_storage_read,
    .secstorage_write = mmc_secure_storage_write,
#endif
    .phyread = sunxi_sprite_mmc_phyread,
    .phywrite = sunxi_sprite_mmc_phywrite,
    .phyerase = sunxi_sprite_mmc_phyerase,
    .download_spl = sunxi_sprite_mmc_download_spl,
    .download_toc = sunxi_sprite_mmc_download_toc,
};

//-----------------------------------end
//------------------------------------------------------

int sdmmc_init_for_boot(int workmode, int card_no)
{
	pr_debug("MMC:	 %d\n", card_no);

	if (!mmc_has_pre_init) {
		mmc_has_pre_init = 1;
		board_mmc_set_num(card_no);
		debug("set card number\n");
		board_mmc_pre_init(card_no);
		debug("begin to find mmc\n");
		mmc_boot = find_mmc_device(sunxi_mmcno_to_devnum(card_no));
	} else {
		mmc_boot = sunxi_mmc_init(card_no);
	}

	if (!mmc_boot) {
		printf("fail to find one useful mmc card\n");
		return -1;
	}
	debug("try to init mmc\n");
	if (mmc_init(mmc_boot)) {
		puts("MMC init failed\n");
		return -1;
	}
	debug("mmc %d init ok\n", card_no);
	return 0;
}

int sdmmc_init_for_sprite(int workmode, int card_no)
{
	static int mmc_has_init;
	if (!mmc_has_init) {
		printf("try card %d\n", card_no);
		mmc_has_init = 1;
	} else {
		printf("******Has init\n");
		return 0;
	}
	if (mmc_has_pre_init)  {
		mmc_sprite = sunxi_mmc_init(card_no);
	} else {
		mmc_has_pre_init = 1;
		board_mmc_set_num(card_no);
		printf("set card number %d\n", card_no);
		printf("get card number %d\n", board_mmc_get_num());
		board_mmc_pre_init(card_no);
		mmc_sprite = find_mmc_device(sunxi_mmcno_to_devnum(card_no));
	}

	mmc_no = card_no;
	if (!mmc_sprite) {
		printf("fail to find one useful mmc card2\n");
#ifdef CONFIG_MMC3_SUPPORT
		printf("try to find card3 \n");
		board_mmc_pre_init(3);
		mmc_sprite = find_mmc_device(3);
		mmc_no = 3;
		if (!mmc_sprite) {
			printf("try card3 fail \n");
			return -1;
		} else {
			set_boot_storage_type(STORAGE_EMMC3);
		}
#else
		return -1;
#endif
	} else {
		if (card_no == 0)
			set_boot_storage_type(STORAGE_EMMC0);
		else
			set_boot_storage_type(STORAGE_EMMC);
	}
	if (mmc_init(mmc_sprite)) {
		printf("MMC init failed\n");
		return -1;
	}
	debug("sunxi sprite has installed sdcard2 function\n");

	return 0;
}

int sdmmc_init_card0_for_sprite(void)
{
	// init card0
	board_mmc_pre_init(0);
	mmc_boot = find_mmc_device(0);
	if (!mmc_boot) {
		printf("fail to find one useful mmc card\n");
		return -1;
	}

	if (mmc_init(mmc_boot)) {
		printf("MMC sprite init failed\n");
		return -1;
	} else {
		printf("mmc init ok\n");
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
int card_read_boot0(void *buffer, uint length, int backup_id)
{
	int ret;
	int storage_type;
	storage_type = get_boot_storage_type();
	if ((STORAGE_EMMC == storage_type) || (STORAGE_EMMC0 == storage_type)) {
		ret = sunxi_sprite_phyread(
			backup_id == 0 ?
				BOOT0_SDMMC_START_ADDR :
				CONFIG_SUNXI_BOOT0_SDMMC_BACKUP_START_ADDR,
			(length + 511) / 512, buffer);
	} else {
		ret = sunxi_sprite_phyread(BOOT0_EMMC3_BACKUP_START_ADDR,
					   (length + 511) / 512, buffer);
	}

	if (!ret) {
		printf("%s: call fail\n", __func__);
		return -1;
	}
	return 0;
}

static int check_secure_storage_key(unsigned char *buffer)
{
	store_object_t *obj = (store_object_t *)buffer;

	if (obj->magic != STORE_OBJECT_MAGIC) {
		printf("Input object magic fail [0x%x]\n", obj->magic);
		return -1;
	}

	if (obj->crc != crc32(0, (void *)obj, sizeof(*obj) - 4)) {
		printf("Input object crc fail [0x%x]\n", obj->crc);
		return -1;
	}
	return 0;
}

static int mmc_secure_storage_read_key(int item, unsigned char *buf,
				       unsigned int len)
{
	unsigned char *align;
	unsigned int blkcnt;
	int ret, workmode;

	if (PT_TO_PHU(buf) % 32) {
		align = (unsigned char *)((PT_TO_PHU(_inner_buffer) + 0x20) &
					  (~0x1f));
		memset(align, 0, 4096);
	} else {
		align = buf;
	}

	blkcnt = (len + 511) / 512;

	workmode = uboot_spare_head.boot_data.work_mode;
	if (workmode == WORK_MODE_BOOT ||
	    workmode == WORK_MODE_SPRITE_RECOVERY) {
		ret = sunxi_flash_mmc_secread(item, align, blkcnt);
	} else if ((workmode & WORK_MODE_PRODUCT) || (workmode == 0x30)) {
		ret = sunxi_sprite_mmc_secread(item, align, blkcnt);
	} else {
		pr_error("workmode=%d is err\n", workmode);
		return -1;
	}
	if (!ret) {
		/*check copy 0 */
		if (!check_secure_storage_key(align)) {
			pr_debug("the secure storage item%d copy0 is good\n",
			       item);
			goto ok; /*copy 0 pass*/
		}
		pr_error("the secure storage item%d copy0 is bad\n", item);
	}

	// read backup
	memset(align, 0x0, len);
	pr_debug("read item%d copy1\n", item);
	if (workmode == WORK_MODE_BOOT ||
	    workmode == WORK_MODE_SPRITE_RECOVERY) {
		ret = (sunxi_flash_mmc_secread_backup(item, align, blkcnt) ==
		       blkcnt)
			  ? 0
			  : -1;
	} else if ((workmode & WORK_MODE_PRODUCT) || (workmode == 0x30)) {
		ret = sunxi_sprite_mmc_secread_backup(item, align, blkcnt);
	} else {
		pr_error("workmode=%d is err\n", workmode);
		return -1;
	}
	if (!ret) {
		/*check copy 1 */
		if (!check_secure_storage_key(align)) {
			pr_debug("the secure storage item%d copy1 is good\n",
			       item);
			goto ok; /*copy 1 pass*/
		}
		pr_error("the secure storage item%d copy1 is bad\n", item);
	}

	pr_error("sunxi_secstorage_read fail\n");
	return -1;

ok:
	if (PT_TO_PHU(buf) % 32)
		memcpy(buf, align, len);
	return 0;
}

static int mmc_secure_storage_read_map(int item, unsigned char *buf,
				       unsigned int len)
{
	unsigned char *align;
	unsigned int blkcnt;
	int ret, workmode;

	if (PT_TO_PHU(buf) % 32) {
		align = (unsigned char *)((PT_TO_PHU(_inner_buffer) + 0x20) &
					  (~0x1f));
		memset(align, 0, 4096);
	} else {
		align = buf;
	}

	blkcnt = (len + 511) / 512;

	pr_msg("read item%d copy0\n", item);
	workmode = uboot_spare_head.boot_data.work_mode;
	if (workmode == WORK_MODE_BOOT ||
	    workmode == WORK_MODE_SPRITE_RECOVERY) {
		ret = sunxi_flash_mmc_secread(item, align, blkcnt);
	} else if ((workmode & WORK_MODE_PRODUCT) || (workmode == 0x30)) {
		ret = sunxi_sprite_mmc_secread(item, align, blkcnt);
	} else {
		pr_error("workmode=%d is err\n", workmode);
		return -1;
	}
	if (!ret) {
		/*read ok*/
		ret = check_secure_storage_map(align);
		if (ret == 0) {
			pr_msg("the secure storage item0 copy0 is good\n");
			goto ok; /*copy 0 pass*/
		} else {
			pr_error("the secure storage item0 copy0 %s is bad\n",
				(ret == 2 ? "magic" : "crc"));
		}
	}

	// read backup
	memset(align, 0x0, len);
	pr_debug("read item%d copy1\n", item);
	if (workmode == WORK_MODE_BOOT ||
	    workmode == WORK_MODE_SPRITE_RECOVERY) {
		ret = sunxi_flash_mmc_secread(item, align, blkcnt);
	} else if ((workmode & WORK_MODE_PRODUCT) || (workmode == 0x30)) {
		ret = sunxi_sprite_mmc_secread(item, align, blkcnt);
	} else {
		pr_debug("workmode=%d is err\n", workmode);
		return -1;
	}
	if (!ret) {
		ret = check_secure_storage_map(align);
		if (ret == 0) {
			pr_debug("the secure storage item0 copy1 is good\n");
		} else {
			pr_error("the secure storage item0 copy1 %s is bad\n",
				(ret == 2 ? "magic" : "crc"));
			memset(align, 0x0, len);
		}
		goto ok;
	}
	pr_error("unknown error happen in item 0 read\n");
	return -1;

ok:
	if (PT_TO_PHU(buf) % 32)
		memcpy(buf, align, len);
	return 0;
}

#if defined(CONFIG_SUPPORT_EMMC_RPMB)
int sunxi_mmc_rpmb_burn_key(void *key)
{
	char original_part;
	int ret = -1;
#define RPMB_SZ_DATA 256
	char read_buf[RPMB_SZ_DATA];
	if (mmc_boot == NULL)
		return -4;
#ifndef CONFIG_BLK
	original_part = mmc_boot->block_dev.hwpart;
#else
	original_part = mmc_get_blk_desc(mmc_boot)->hwpart;
#endif
	if (blk_select_hwpart_devnum(IF_TYPE_MMC, board_mmc_get_num(),
				     MMC_PART_RPMB) != 0)
		return -1;

	if (mmc_rpmb_set_key(mmc_boot, key)) {
		if (1 == (mmc_rpmb_read(mmc_boot, read_buf, 0, 1, key))) {
			ret = -3;
		} else {
			ret = -2;
		}
	}

	/* Return to original partition */
	blk_select_hwpart_devnum(IF_TYPE_MMC, board_mmc_get_num(),
				 original_part);
	return ret;
}
#endif
