// SPDX-License-Identifier: GPL-2.0+
/*
 * sunxi SPI driver for uboot.
 *
 * Copyright (C) 2023
 * 2018.11.7 huangrongcun <huangrongcun@allwinnertech.com>
 */
#include <boot_param.h>
#include <spare_head.h>
#include <private_uboot.h>
#include <sunxi_board.h>
#include <sprite_download.h>
#include <sunxi_board.h>

DECLARE_GLOBAL_DATA_PTR;
// #define ALWAYS_DRAM_TRAINING

#define BOOT_PARAM_FES_TRANSFER_UBOOT_ADDR                                     \
	(CONFIG_DRAM_PARA_ADDR + SUNXI_DRAM_PARA_MAX * 4)
#define SUNXI_DRAM_PARA_MAX 32

typedef_sunxi_boot_param *sunxi_bootparam_get_buf_in_relocate_f(void)
{
	if (uboot_spare_head.boot_data.work_mode == WORK_MODE_USB_PRODUCT)
		return (typedef_sunxi_boot_param
				*)(BOOT_PARAM_FES_TRANSFER_UBOOT_ADDR);
	else {
		return (typedef_sunxi_boot_param *)(CONFIG_SYS_TEXT_BASE +
						    SUNXI_BOOTPARAM_OFFSET);
	}
}

int sunxi_bootparam_check_magic(typedef_sunxi_boot_param *sunxi_boot_param)
{
	if (strncmp((char *)sunxi_boot_param->header.magic, BOOT_PARAM_MAGIC,
		    strlen(BOOT_PARAM_MAGIC))) {
		return -1;
	}
	return 0;
}

static void
sunxi_bootparam_set_dram_flag(typedef_sunxi_boot_param *sunxi_boot_param)
{
	/* [23]:bit31 */
	/* 0:training dram  1: not training dram*/
	u32 flag	    = 0;
	unsigned int *pdram = (unsigned int *)sunxi_boot_param->ddr_info;
	flag		    = pdram[23];
	flag |= (0x1U << 31);
	pdram[23] = flag;
}

static int
sunxi_bootparam_read_update_flag(typedef_sunxi_boot_param *sunxi_boot_param)
{
	return (sunxi_boot_param->header.transfer_flag &&
		BOOTPARAM_DOWNLOAD_MASK);
}

static void
sunxi_bootparam_reset_update_flag(typedef_sunxi_boot_param *sunxi_boot_param)
{
	sunxi_boot_param->header.transfer_flag &= ~BOOTPARAM_DOWNLOAD_MASK;
}

void sunxi_bootparam_set_boot0_checksum(
	typedef_sunxi_boot_param *sunxi_boot_param, int boot0_checksum)
{
	sunxi_boot_param->header.boot0_checksum = boot0_checksum;
}

int sunxi_bootparam_format(typedef_sunxi_boot_param *sunxi_boot_param)
{
	if (sunxi_bootparam_check_magic(sunxi_boot_param) < 0) {
		return -1;
	}

	if (!sunxi_bootparam_read_update_flag(sunxi_boot_param)) {
		printf("skip update boot_param\n");
		return -1;
	}

#ifndef ALWAYS_DRAM_TRAINING
	sunxi_bootparam_set_dram_flag(sunxi_boot_param);
#endif
	sunxi_bootparam_reset_update_flag(sunxi_boot_param);
#if 0
	printf("%s...%d:dump boot param\n", __func__, __LINE__);
	sunxi_dump((void *)sunxi_boot_param,
		   sizeof(struct sunxi_boot_param_region));
	dump_dram_para(sunxi_boot_param->ddr_info, MAX_DRAMPARA_SIZE);
#endif
	return 0;
}

int sunxi_bootparam_down(void)
{
	return 0;
	typedef_sunxi_boot_param *sunxi_boot_param = gd->boot_param;

	if (sunxi_bootparam_format(sunxi_boot_param) < 0) {
		return -1;
	}

#if 0
	sunxi_dump((void *)sunxi_boot_param,
		   sizeof(struct sunxi_boot_param_region));
#endif
	sunxi_flash_download_boot_param();

	return 0;
}
