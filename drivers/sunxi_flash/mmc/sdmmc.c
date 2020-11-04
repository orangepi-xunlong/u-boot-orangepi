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

//-------------------------------------noraml
//interface--------------------------------------------
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

static int sunxi_flash_mmcs_read(unsigned int start_block, unsigned int nblock,
				void *buffer)
{
	debug("mmcboot read: start 0x%x, sector 0x%x\n", start_block, nblock);

	return mmc_sprite->block_dev.block_read(
	    &mmc_sprite->block_dev, start_block + CONFIG_MMC_LOGICAL_OFFSET,
	    nblock, buffer);
}

static int sunxi_flash_mmcs_write(unsigned int start_block, unsigned int nblock,
				 void *buffer)
{
	debug("mmcboot write: start 0x%x, sector 0x%x\n", start_block, nblock);

	return mmc_sprite->block_dev.block_write(
	    &mmc_sprite->block_dev, start_block + CONFIG_MMC_LOGICAL_OFFSET,
	    nblock, buffer);
}

static uint sunxi_flash_mmcs_size(void)
{

	return mmc_sprite->block_dev.lba;
}

static int
sunxi_flash_mmc_download_spl(unsigned char *buf, int len, unsigned int ext)
{
	if (mmc_boot->block_dev.block_write(&mmc_boot->block_dev,
		SUNXI_MMC_BOOT0_START_ADDRS, len / 512, buf))
		return 0;
	else
		return -1;

}

static int
sunxi_flash_mmc_download_toc(unsigned char *buf, int len,  unsigned int ext)
{
	if (mmc_boot->block_dev.block_write(
	    &mmc_boot->block_dev, SUNXI_MMC_TOC_START_ADDRS,
	    len/512, buf))
		return 0;
	else
		return -1;
}

static int
sunxi_flash_mmcs_download_spl(unsigned char *buf, int len, unsigned int ext)
{
	if (mmc_sprite->block_dev.block_write(&mmc_sprite->block_dev,
		SUNXI_MMC_BOOT0_START_ADDRS, len / 512, buf))
		return 0;
	else
		return -1;

}

static int
sunxi_flash_mmcs_download_toc(unsigned char *buf, int len,  unsigned int ext)
{
	if (mmc_sprite->block_dev.block_write(
	    &mmc_sprite->block_dev, SUNXI_MMC_TOC_START_ADDRS,
	    len/512, buf))
		return 0;
	else
		return -1;
}


static uint sunxi_flash_mmc_size(void)
{

	return mmc_boot->block_dev.lba;
}

static int sunxi_flash_mmc_init(int stage, int card_no)
{
	return sdmmc_init_for_boot(stage, card_no);
}


static int sunxi_flash_mmcs_init(int stage, int card_no)
{
/*	sdmmc_init_for_boot(stage,0);*/
	return sdmmc_init_for_sprite(stage, card_no);
}




static int sunxi_flash_mmc_exit(int force)
{
//	printf("not implement %s,%d\n", __FUNCTION__, __LINE__);
//	return 0;
	return mmc_exit();
	// return 0;
}


static int sunxi_flash_mmcs_exit(int force)
{
//	printf("not implement %s,%d\n", __FUNCTION__, __LINE__);
	return 0;
	/*return mmc_exit();*/
	// return 0;
}



int sunxi_flash_mmc_phyread(unsigned int start_block, unsigned int nblock,
			    void *buffer)
{
	/*return mmc_boot->block_dev.block_read_mass_pro(&mmc_boot->block_dev,
	 * start_block, nblock, buffer);
*/
	/* to do:implement block_read_mass_pro*/
	return mmc_boot->block_dev.block_read(&mmc_boot->block_dev, start_block,
					      nblock, buffer);
}

int sunxi_flash_mmc_phywrite(unsigned int start_block, unsigned int nblock,
			     void *buffer)
{
	/*return mmc_boot->block_dev.block_write_mass_pro(&mmc_boot->block_dev,
	 * start_block, nblock, buffer);
*/
	/* to do:implement block_write_mass_pro*/
	return mmc_boot->block_dev.block_write(&mmc_boot->block_dev,
					       start_block, nblock, buffer);
}

int sunxi_flash_mmc_flush(void)
{
	return 0;
}
//-------------------------------------sprite
//interface--------------------------------------------
/*
static int sunxi_sprite_mmc_read(unsigned int start_block, unsigned int nblock,
				 void *buffer)
{
	debug("mmcsprite read: start 0x%x, sector 0x%x\n", start_block, nblock);

	return mmc_sprite->block_dev.block_read(
	    &mmc_sprite->block_dev, start_block + CONFIG_MMC_LOGICAL_OFFSET,
	    nblock, buffer);
}

static int sunxi_sprite_mmc_write(unsigned int start_block, unsigned int nblock,
				  void *buffer)
{
	debug("mmcsprite write: start 0x%x, sector 0x%x\n", start_block,
	      nblock);

	return mmc_sprite->block_dev.block_write(
	    &mmc_sprite->block_dev, start_block + CONFIG_MMC_LOGICAL_OFFSET,
	    nblock, buffer);
}
*/
static int sunxi_sprite_mmc_erase(int erase, void *mbr_buffer)
{
	return card_erase(erase, mbr_buffer);
}
/*
static uint sunxi_sprite_mmc_size(void)
{

	return mmc_sprite->block_dev.lba;
}

static int sunxi_sprite_mmc_init(int stage)
{
	return 0;
}

static int sunxi_sprite_mmc_exit(int force)
{
	return 0;
}
*/
int sunxi_sprite_mmcs_phyread(unsigned int start_block, unsigned int nblock,
			     void *buffer)
{
	/*return
	 * mmc_sprite->block_dev.block_read_mass_pro(mmc_sprite->block_dev.dev,
	 * start_block, nblock, buffer);*/
	/* to do:implement block_read_mass_pro*/
	return mmc_sprite->block_dev.block_read(&mmc_sprite->block_dev,
						start_block, nblock, buffer);
}

int sunxi_sprite_mmcs_phywrite(unsigned int start_block, unsigned int nblock,
			      void *buffer)
{
	/*return
	 * mmc_sprite->block_dev.block_write_mass_pro(mmc_sprite->block_dev.dev,
	 * start_block, nblock, buffer);*/
	/* to do:implement block_write_mass_pro*/
	return mmc_sprite->block_dev.block_write(&mmc_sprite->block_dev,
						 start_block, nblock, buffer);
}

int sunxi_sprite_mmcs_phyerase(unsigned int start_block, unsigned int nblock,
			      unsigned int *skip)
{
	if (nblock == 0) {
		printf("%s: @nr is 0, erase from @from to end\n", __FUNCTION__);
		nblock = mmc_sprite->block_dev.lba - start_block - 1;
	}
	/*to do */
	return mmc_sprite->block_dev.block_mmc_erase(&mmc_sprite->block_dev,
	 start_block, nblock, skip);
	/*return mmc_sprite->block_dev.block_erase(&mmc_sprite->block_dev,
						 start_block, nblock);*/
}

int sunxi_sprite_mmc_phywipe(unsigned int start_block, unsigned int nblock,
			     void *skip)
{
	if (nblock == 0) {
		printf("%s: @nr is 0, wipe from @from to end\n", __FUNCTION__);
		nblock = mmc_sprite->block_dev.lba - start_block - 1;
	}
	/*to do  */
	/*return mmc_sprite->block_dev.block_secure_wipe(&mmc_sprite->block_dev,
	 * start_block, nblock, skip);*/
	printf("not implement %s,%d\n", __FUNCTION__, __LINE__);
	return -1;
}

int sunxi_sprite_mmc_force_erase(void)
{
	return 0;
}

//-----------------------------secure
//interface---------------------------------------
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

	if (((unsigned int)buf % 32)) { // input buf not align
		align = (unsigned char *)(((unsigned int)_inner_buffer + 0x20) &
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

int sunxi_flash_mmc_probe(void)
{
	printf("not implement %s,%d\n", __FUNCTION__, __LINE__);
	return 0;
}

int sunxi_flash_mmcs_probe(void)
{
	return sdmmc_init_for_sprite(0, 2);
}

#if 1
sunxi_flash_desc sunxi_sdmmc_desc = {
    .probe = sunxi_flash_mmc_probe,
    .init = sunxi_flash_mmc_init,
    .exit = sunxi_flash_mmc_exit,
    .read = sunxi_flash_mmc_read,
    .write = sunxi_flash_mmc_write,
    /*	.erase = sunxi_flash_mmc_erase,*/
    /*	.force_erase = sunxi_flash_mmc_force_erase,*/
    .flush = sunxi_flash_mmc_flush,
    .size = sunxi_flash_mmc_size,
#ifdef CONFIG_SUNXI_SECURE_STORAGE
    .secstorage_read = mmc_secure_storage_read,
    .secstorage_write = mmc_secure_storage_write,
#endif
    .phyread = sunxi_flash_mmc_phyread,
    .phywrite = sunxi_flash_mmc_phywrite,
    .download_spl = sunxi_flash_mmc_download_spl,
    .download_toc = sunxi_flash_mmc_download_toc,
};
#endif

sunxi_flash_desc sunxi_sdmmcs_desc = {
    .probe = sunxi_flash_mmcs_probe,
    .init = sunxi_flash_mmcs_init,
    .exit = sunxi_flash_mmcs_exit,
    .read = sunxi_flash_mmcs_read,
    .write = sunxi_flash_mmcs_write,
    /*	.erase = sunxi_flash_mmc_erase,*/
    .erase = sunxi_sprite_mmc_erase,
    /*	.force_erase = sunxi_flash_mmc_force_erase,*/
    .flush = sunxi_flash_mmc_flush,
    .size = sunxi_flash_mmcs_size,
#ifdef CONFIG_SUNXI_SECURE_STORAGE
    .secstorage_read = mmc_secure_storage_read,
    .secstorage_write = mmc_secure_storage_write,
#endif
    .phyread = sunxi_sprite_mmcs_phyread,
    .phywrite = sunxi_sprite_mmcs_phywrite,
//    .phyerase = sunxi_sprite_mmcs_phyerase,
    .download_spl = sunxi_flash_mmcs_download_spl,
    .download_toc = sunxi_flash_mmcs_download_toc,
};

//-----------------------------------end
//------------------------------------------------------

int sdmmc_init_for_boot(int workmode, int card_no)
{
//	void *tp = NULL;
	tick_printf("MMC:	 %d\n", card_no);

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


	/*mmc_no = card_no;*/
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
	/*
		sunxi_flash_init_pt  = sunxi_flash_mmc_init;
		sunxi_flash_read_pt  = sunxi_flash_mmc_read;
		sunxi_flash_write_pt = sunxi_flash_mmc_write;
		sunxi_flash_size_pt  = sunxi_flash_mmc_size;
		sunxi_flash_exit_pt  = sunxi_flash_mmc_exit;
		sunxi_flash_phyread_pt  = sunxi_flash_mmc_phyread;
		sunxi_flash_phywrite_pt = sunxi_flash_mmc_phywrite;
	*/
	// for fastboot
	/*
		sunxi_sprite_phyread_pt  = sunxi_flash_mmc_phyread;
		sunxi_sprite_phywrite_pt = sunxi_flash_mmc_phywrite;
		sunxi_sprite_read_pt  = sunxi_flash_read_pt;
		sunxi_sprite_write_pt = sunxi_flash_write_pt;
	*/
	/* for secure stoarge */
	/*
		sunxi_secstorage_read_pt  = mmc_secure_storage_read;
		sunxi_secstorage_write_pt = mmc_secure_storage_write;
	*/
	return 0;
}
#if 1
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
		set_boot_storage_type(STORAGE_EMMC);
	}
	if (mmc_init(mmc_sprite)) {
		printf("MMC init failed\n");
		return -1;
	}
	/*
		sunxi_sprite_init_pt  = sunxi_sprite_mmc_init;
		sunxi_sprite_exit_pt  = sunxi_sprite_mmc_exit;
		sunxi_sprite_read_pt  = sunxi_sprite_mmc_read;
		sunxi_sprite_write_pt = sunxi_sprite_mmc_write;
		sunxi_sprite_erase_pt = sunxi_sprite_mmc_erase;
		sunxi_sprite_size_pt  = sunxi_sprite_mmc_size;
		sunxi_sprite_phyread_pt  = sunxi_sprite_mmc_phyread;
		sunxi_sprite_phywrite_pt = sunxi_sprite_mmc_phywrite;
		sunxi_sprite_force_erase_pt = sunxi_sprite_mmc_force_erase;
	*/
	/* for secure stoarge */
	/*
		sunxi_secstorage_read_pt  = mmc_secure_storage_read;
		sunxi_secstorage_write_pt = mmc_secure_storage_write;
	*/
	debug("sunxi sprite has installed sdcard2 function\n");

	return 0;
}
#endif

#if 0
int sdmmc_init_for_sprite(int workmode, int card_no)
{
	static int mmc_has_init;
	mmc_has_init = 0;
	printf("try card %d\n", card_no);
	mmc_initialize(gd->bd);
	if (!mmc_has_init)
		mmc_has_init = 1;
	else{
		printf("******Has init\n");
		return 0;
	}
	mmc_sprite = sunxi_mmc_init(card_no);

	mmc_no = card_no;
	if (!mmc_sprite) {
		printf("fail to find one useful mmc card2\n");
#ifdef CONFIG_MMC3_SUPPORT
		printf("try to find card3 \n");
		mmc_sprite = sunxi_mmc_init(3);
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
		set_boot_storage_type(STORAGE_EMMC);
	}
	if (mmc_init(mmc_sprite)) {
		printf("MMC init failed\n");
		return -1;
	}
	debug("sunxi sprite has installed sdcard2 function\n");

	return 0;
}

#endif

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
	/*
		sunxi_flash_init_pt  = sunxi_flash_mmc_init;
		sunxi_flash_read_pt  = sunxi_flash_mmc_read;
		sunxi_flash_write_pt = sunxi_flash_mmc_write;
		sunxi_flash_size_pt  = sunxi_flash_mmc_size;
		sunxi_flash_phyread_pt  = sunxi_flash_mmc_phyread;
		sunxi_flash_phywrite_pt = sunxi_flash_mmc_phywrite;
		sunxi_flash_exit_pt  = sunxi_flash_mmc_exit;
	*/
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
int card_read_boot0(void *buffer, uint length)
{
	int ret;
	int storage_type;
	storage_type = get_boot_storage_type();
	if (STORAGE_EMMC == storage_type) {
		ret = sunxi_sprite_phyread(BOOT0_SDMMC_START_ADDR,
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

	if (((unsigned int)buf % 32)) {
		align = (unsigned char *)(((unsigned int)_inner_buffer + 0x20) &
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
	if (((unsigned int)buf % 32))
		memcpy(buf, align, len);
	return 0;
}

static int mmc_secure_storage_read_map(int item, unsigned char *buf,
				       unsigned int len)
{
	int have_map_copy0;
	unsigned char *align;
	unsigned int blkcnt;
	int ret, workmode;
	char *map_copy0_buf;

	if (((unsigned int)buf % 32)) {
		align = (unsigned char *)(((unsigned int)_inner_buffer + 0x20) &
					  (~0x1f));
		memset(align, 0, 4096);
	} else {
		align = buf;
	}

	blkcnt = (len + 511) / 512;

	map_copy0_buf = (char *)memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN(blkcnt * 512, CONFIG_SYS_CACHELINE_SIZE));
	if (!map_copy0_buf) {
		pr_error("out of memory\n");
		return -1;
	}

	pr_msg("read item%d copy0\n", item);
	workmode = uboot_spare_head.boot_data.work_mode;
	if (workmode == WORK_MODE_BOOT ||
	    workmode == WORK_MODE_SPRITE_RECOVERY) {
		ret = sunxi_flash_mmc_secread(item, align, blkcnt);
	} else if ((workmode & WORK_MODE_PRODUCT) || (workmode == 0x30)) {
		ret = sunxi_sprite_mmc_secread(item, align, blkcnt);
	} else {
		pr_error("workmode=%d is err\n", workmode);
		free(map_copy0_buf);
		return -1;
	}
	if (!ret) {
		/*read ok*/
		ret = check_secure_storage_map(align);
		if (ret == 0) {
			pr_msg("the secure storage item0 copy0 is good\n");
			goto ok; /*copy 0 pass*/
		} else if (ret == 2) {
			memcpy(map_copy0_buf, align, len);
			have_map_copy0 = 1;
			pr_error("the secure storage item0 copy0 is bad\n");
		} else
			pr_error("the secure storage item0 copy 0 crc fail, the "
			       "data is bad\n");
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
		free(map_copy0_buf);
		return -1;
	}
	if (!ret) {
		ret = check_secure_storage_map(align);
		if (ret == 0) {
			pr_debug("the secure storage item0 copy1 is good\n");
			goto ok;
		} else if (ret == 2) {
			if (have_map_copy0 &&
			    !memcmp(map_copy0_buf, align, len)) {
				pr_debug("the secure storage item0 copy0 == "
				       "copy1, the data is good\n");
				goto ok; /*copy have no magic and crc*/
			} else {
				pr_error("the secure storage item0 copy0 != "
				       "copy1, the data is bad\n");
				free(map_copy0_buf);
				return -1;
			}
		} else {
			pr_error("the secure storage item0 copy 1 crc fail, the "
			       "data is bad\n");
			free(map_copy0_buf);
			return -1;
		}
	}
	pr_error("unknown error happen in item 0 read\n");
	free(map_copy0_buf);
	return -1;

ok:
	if (((unsigned int)buf % 32))
		memcpy(buf, align, len);
	free(map_copy0_buf);
	return 0;
}
