// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * oujiayu <oujiayu@allwinnertech.com>
 *
 */
#include <common.h>
#include <config.h>
#include <command.h>
#include <asm/io.h>
#include <fdt_support.h>
#include <fdtdec.h>
#include <environment.h>
#include "sunxi_lradc.h"

DECLARE_GLOBAL_DATA_PTR;

#define FDT_PATH_KEYBOARD	"/soc/keyboard"
#define FDT_REG			"reg"

static void lradc_ctrl_set(u32 reg_base,
		enum key_mode key_mode, u32 para)
{
	u32 ctrl_reg = 0;

	if (para != 0)
		ctrl_reg = readl(reg_base + LRADC_CTRL);
	if (CONCERT_DLY_SET & key_mode)
		ctrl_reg |= (FIRST_CONCERT_DLY & para);
	if (ADC_CHAN_SET & key_mode)
		ctrl_reg |= (ADC_CHAN_SELECT & para);
	if (KEY_MODE_SET & key_mode)
		ctrl_reg |= (KEY_MODE_SELECT & para);
	if (LRADC_HOLD_SET & key_mode)
		ctrl_reg |= (LRADC_HOLD_EN & para);
	if (LEVELB_VOL_SET & key_mode) {
		ctrl_reg |= (LEVELB_VOL & para);
#if defined(CONFIG_ARCH_SUN8IW18)
		ctrl_reg &= ~(u32)(3 << 4);
#endif
	}
	if (LRADC_SAMPLE_SET & key_mode)
		ctrl_reg |= (LRADC_SAMPLE_250HZ & para);
	if (LRADC_EN_SET & key_mode)
		ctrl_reg |= (LRADC_EN & para);
	writel(ctrl_reg, reg_base + LRADC_CTRL);
}

int lradc_reg_init(void)
{
	unsigned long mode, para;
	int nodeoffset;
	const void *blob = gd->fdt_blob;
	fdt_addr_t addr = 0;

	nodeoffset = fdt_path_offset(working_fdt, FDT_PATH_KEYBOARD);
	if (nodeoffset < 0) {
		printf("find dts keytboard err!\n");
		return -1;
	}

	addr = fdtdec_get_addr(blob, nodeoffset, FDT_REG);
	if (!addr) {
		printf("find dts reg_base err!!!\n");
		return -1;
	}

	mode = CONCERT_DLY_SET | ADC_CHAN_SET | KEY_MODE_SET
		| LRADC_HOLD_SET | LEVELB_VOL_SET
		| LRADC_SAMPLE_SET | LRADC_EN_SET;
	para = FIRST_CONCERT_DLY | LEVELB_VOL | KEY_MODE_SELECT
		| LRADC_HOLD_EN | ADC_CHAN_SELECT
		| LRADC_SAMPLE_250HZ | LRADC_EN;

	lradc_ctrl_set(addr, mode, para);
	return 0;
}
