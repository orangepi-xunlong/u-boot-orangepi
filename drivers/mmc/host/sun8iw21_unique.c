// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * MMC driver for allwinner sunxi platform.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */
#include <common.h>
#include <asm/io.h>
#include <asm/arch-sunxi/clock.h>
#include <asm/arch-sunxi/cpu.h>
#include <asm/arch-sunxi/mmc.h>
#include <asm/arch-sunxi/timer.h>
#include <malloc.h>
#include <mmc.h>
#include <sys_config.h>
#include <linux/libfdt.h>
#include <fdt_support.h>
#include <private_uboot.h>

#include "../sunxi_mmc.h"
#include "../mmc_def.h"
#include "sunxi_mmc_host_common.h"

#define SDC_NUM 3
#define SDC_SRC_CLK_NUM 4

static int sdc_src_clk_freq[SDC_NUM][SDC_SRC_CLK_NUM] = {
	{24*1000*1000, 400*1000*1000, 300*1000*1000},
	{24*1000*1000, 400*1000*1000, 300*1000*1000},
	{24*1000*1000, 600*1000*1000, 400*1000*1000}
};

static int sdc_src_clk_def[SDC_NUM][SDC_SRC_CLK_NUM] = {
	{24*1000*1000, 400*1000*1000, 300*1000*1000},
	{24*1000*1000, 400*1000*1000, 300*1000*1000},
	{24*1000*1000, 600*1000*1000, 400*1000*1000}
};

static void sunxi_mmc_update_src_clk(int sdc_no)
{
	int i;
	int tmp = clock_get_pll6();
	if (tmp == 600)
		return ;

	MMCINFO("check sdmmc%d src clock,src clk freq:%d!!!!!!!!!!!\n", sdc_no, tmp);

	/*Prorated distribution, because the src clock depend to pll6*/
	for (i=1; i<SDC_SRC_CLK_NUM; i++) {
		sdc_src_clk_freq[sdc_no][i] = tmp * sdc_src_clk_def[sdc_no][i] / 600;
	}
}

int sunxi_mmc_get_src_clk_no(int sdc_no, int mod_hz, int tm)
{
	int mod, result, tmp = 0, tmp_mod = mod_hz, i;

	sunxi_mmc_update_src_clk(sdc_no);

	for (i=0; i<SDC_SRC_CLK_NUM; i++) {
		result = sdc_src_clk_freq[sdc_no][i]/mod_hz;
		if (result == 0)
			continue;
		mod = sdc_src_clk_freq[sdc_no][i] % mod_hz;
		if (mod == 0) {
			tmp = i;
			break;
		/*Select the one with the smallest remainder*/
		} else if (tmp_mod > mod) {
			tmp_mod = mod;
			tmp = i;
		}
	}

	return (tmp<<24);
}

int sunxi_host_src_clk(int sdc_no, int src_clk, int tm)
{
	sunxi_mmc_update_src_clk(sdc_no);

	if (sdc_no >= SDC_NUM || src_clk >= SDC_SRC_CLK_NUM ) {
		MMCINFO("no exist sdc%d or src_clk%d\n", sdc_no, src_clk);
		return -1;
	}

	return sdc_src_clk_freq[sdc_no][src_clk];
}
