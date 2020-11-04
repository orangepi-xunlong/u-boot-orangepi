// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Aaron <leafy.myeh@allwinnertech.com>
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
//#include <asm/arch/platform.h>
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

//#include "../mmc_def.h"
#include "sunxi_mmc.h"
#include "sunxi_host_mmc.h"

extern char *spd_name[];
extern struct sunxi_mmc_priv mmc_host[4];
//static u8 ext_odly_spd_freq[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
//static u8 ext_sdly_spd_freq[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
//static u8 ext_odly_spd_freq_sdc0[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
//static u8 ext_sdly_spd_freq_sdc0[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];

static int mmc_clear_timing_para(int sdc_no)
{
	struct sunxi_mmc_priv *mmcpriv = &mmc_host[sdc_no];
	int i, j;

	for (i = 0; i < MAX_SPD_MD_NUM; i++) {
		for (j = 0; j < MAX_CLK_FREQ_NUM; j++) {
			mmcpriv->tm1.odly[i*MAX_CLK_FREQ_NUM+j] = 0xFF;
			mmcpriv->tm1.sdly[i*MAX_CLK_FREQ_NUM+j] = 0xFF;
			mmcpriv->tm1.def_odly[i*MAX_CLK_FREQ_NUM+j] = 0xFF;
			mmcpriv->tm1.def_sdly[i*MAX_CLK_FREQ_NUM+j] = 0xFF;
			mmcpriv->tm3.odly[i*MAX_CLK_FREQ_NUM+j] = 0xFF;
			mmcpriv->tm3.sdly[i*MAX_CLK_FREQ_NUM+j] = 0xFF;
			mmcpriv->tm3.def_odly[i*MAX_CLK_FREQ_NUM+j] = 0xFF;
			mmcpriv->tm3.def_sdly[i*MAX_CLK_FREQ_NUM+j] = 0xFF;
			mmcpriv->tm4.odly[i*MAX_CLK_FREQ_NUM+j] = 0xFF;
			mmcpriv->tm4.sdly[i*MAX_CLK_FREQ_NUM+j] = 0xFF;
			mmcpriv->tm4.def_odly[i*MAX_CLK_FREQ_NUM+j] = 0xFF;
			mmcpriv->tm4.def_sdly[i*MAX_CLK_FREQ_NUM+j] = 0xFF;
		}
	}

	for (j = 0; j < MAX_CLK_FREQ_NUM; j++) {
		mmcpriv->tm4.dsdly[j] = 0xFF;
		mmcpriv->tm4.def_dsdly[j] = 0xFF;
	}

	return 0;
}

static int mmc_init_default_timing_para(int sdc_no)
{
	struct sunxi_mmc_priv *mmcpriv = &mmc_host[sdc_no];

	if (sdc_no == 0) {
		/* timing mode 0 */
		mmcpriv->tm0.cur_spd_md = DS26_SDR12;
		mmcpriv->tm0.cur_freq = CLK_400K;
		mmcpriv->tm0.sample_point_cnt = MMC_CLK_SAMPLE_POINIT_MODE_0;

		mmcpriv->tm0.def_odly[DS26_SDR12 * MAX_CLK_FREQ_NUM + CLK_400K] = 0;
		mmcpriv->tm0.def_sdly[DS26_SDR12 * MAX_CLK_FREQ_NUM + CLK_400K] = 0;
		mmcpriv->tm0.def_odly[DS26_SDR12 * MAX_CLK_FREQ_NUM + CLK_25M] = 3;
		mmcpriv->tm0.def_sdly[DS26_SDR12 * MAX_CLK_FREQ_NUM + CLK_25M] = 5;

		mmcpriv->tm0.def_odly[HSSDR52_SDR25 * MAX_CLK_FREQ_NUM + CLK_25M] = 3;
		mmcpriv->tm0.def_sdly[HSSDR52_SDR25 * MAX_CLK_FREQ_NUM + CLK_25M] = 5;
		mmcpriv->tm0.def_odly[HSSDR52_SDR25 * MAX_CLK_FREQ_NUM + CLK_50M] = 3;
		mmcpriv->tm0.def_sdly[HSSDR52_SDR25 * MAX_CLK_FREQ_NUM + CLK_50M] = 5;

		mmcpriv->tm0.def_odly[HSDDR52_DDR50 * MAX_CLK_FREQ_NUM + CLK_25M] = 3;
		mmcpriv->tm0.def_sdly[HSDDR52_DDR50 * MAX_CLK_FREQ_NUM + CLK_25M] = 5;
		mmcpriv->tm0.def_odly[HSDDR52_DDR50 * MAX_CLK_FREQ_NUM + CLK_50M] = 3;
		mmcpriv->tm0.def_sdly[HSDDR52_DDR50 * MAX_CLK_FREQ_NUM + CLK_50M] = 5;

		/* timing mode 1 */
		mmcpriv->tm1.cur_spd_md = DS26_SDR12;
		mmcpriv->tm1.cur_freq = CLK_400K;
		mmcpriv->tm1.sample_point_cnt = MMC_CLK_SAMPLE_POINIT_MODE_1;

		mmcpriv->tm1.def_odly[DS26_SDR12*MAX_CLK_FREQ_NUM+CLK_400K] = TM1_OUT_PH180;
		mmcpriv->tm1.def_sdly[DS26_SDR12*MAX_CLK_FREQ_NUM+CLK_400K] = TM1_IN_PH180;
		mmcpriv->tm1.def_odly[DS26_SDR12*MAX_CLK_FREQ_NUM+CLK_25M] = TM1_OUT_PH180;
		mmcpriv->tm1.def_sdly[DS26_SDR12*MAX_CLK_FREQ_NUM+CLK_25M] = TM1_IN_PH180;

		mmcpriv->tm1.def_odly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_25M] = TM1_OUT_PH180;
		mmcpriv->tm1.def_sdly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_25M] = TM1_IN_PH90;
		mmcpriv->tm1.def_odly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_50M] = TM1_OUT_PH180;
		mmcpriv->tm1.def_sdly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_50M] = TM1_IN_PH90;

		mmcpriv->tm1.def_odly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_25M] = TM1_OUT_PH90;
		mmcpriv->tm1.def_sdly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_25M] = TM1_IN_PH180;
		mmcpriv->tm1.def_odly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_50M] = TM1_OUT_PH90;
		mmcpriv->tm1.def_sdly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_50M] = TM1_IN_PH180;

		mmcpriv->tm1.def_odly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_25M] = TM1_OUT_PH180;
		mmcpriv->tm1.def_sdly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_25M] = TM1_IN_PH90;
		mmcpriv->tm1.def_odly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_50M] = TM1_OUT_PH180;
		mmcpriv->tm1.def_sdly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_50M] = TM1_IN_PH90;

		/* timing mode 3 */
		mmcpriv->tm3.cur_spd_md = DS26_SDR12;
		mmcpriv->tm3.cur_freq = CLK_400K;
		mmcpriv->tm3.sample_point_cnt = MMC_CLK_SAMPLE_POINIT_MODE_3;

		mmcpriv->tm3.def_odly[DS26_SDR12*MAX_CLK_FREQ_NUM+CLK_400K] = TM3_OUT_PH180;
		mmcpriv->tm3.def_sdly[DS26_SDR12*MAX_CLK_FREQ_NUM+CLK_400K] = 0x0;
		mmcpriv->tm3.def_odly[DS26_SDR12*MAX_CLK_FREQ_NUM+CLK_25M] = TM3_OUT_PH180;
		mmcpriv->tm3.def_sdly[DS26_SDR12*MAX_CLK_FREQ_NUM+CLK_25M] = 0x0;

		mmcpriv->tm3.def_odly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_25M] = TM3_OUT_PH180;
		mmcpriv->tm3.def_sdly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_25M] = 0x0;
		mmcpriv->tm3.def_odly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_50M] = TM3_OUT_PH180;
		mmcpriv->tm3.def_sdly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_50M] = 0x0;

		mmcpriv->tm3.def_odly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_25M] = TM1_OUT_PH90;
		mmcpriv->tm3.def_sdly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_25M] = 0x0;
		mmcpriv->tm3.def_odly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_50M] = TM1_OUT_PH90;
		mmcpriv->tm3.def_sdly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_50M] = 0x0;

		mmcpriv->tm3.def_odly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_25M] = TM3_OUT_PH180;
		mmcpriv->tm3.def_sdly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_25M] = 0x0;
		mmcpriv->tm3.def_odly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_50M] = TM3_OUT_PH180;
		mmcpriv->tm3.def_sdly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_50M] = 0x0;

	} else if (sdc_no == 2) {

		/* timing mode 1 */
		mmcpriv->tm1.cur_spd_md = DS26_SDR12;
		mmcpriv->tm1.cur_freq = CLK_400K;
		mmcpriv->tm1.sample_point_cnt = MMC_CLK_SAMPLE_POINIT_MODE_1;

		mmcpriv->tm1.def_odly[DS26_SDR12 * MAX_CLK_FREQ_NUM + CLK_400K] = 0;
		mmcpriv->tm1.def_sdly[DS26_SDR12 * MAX_CLK_FREQ_NUM + CLK_400K] = 0;
		mmcpriv->tm1.def_odly[DS26_SDR12 * MAX_CLK_FREQ_NUM + CLK_25M] = 0;
		mmcpriv->tm1.def_sdly[DS26_SDR12 * MAX_CLK_FREQ_NUM + CLK_25M] = 0;

		mmcpriv->tm1.def_odly[HSSDR52_SDR25 * MAX_CLK_FREQ_NUM + CLK_25M] = 1;
		mmcpriv->tm1.def_sdly[HSSDR52_SDR25 * MAX_CLK_FREQ_NUM + CLK_25M] = 1;
		mmcpriv->tm1.def_odly[HSSDR52_SDR25 * MAX_CLK_FREQ_NUM + CLK_50M] = 1;
		mmcpriv->tm1.def_sdly[HSSDR52_SDR25 * MAX_CLK_FREQ_NUM + CLK_50M] = 1;

		mmcpriv->tm1.def_odly[HSDDR52_DDR50 * MAX_CLK_FREQ_NUM + CLK_25M] = 1;
		mmcpriv->tm1.def_sdly[HSDDR52_DDR50 * MAX_CLK_FREQ_NUM + CLK_25M] = 1;
		mmcpriv->tm1.def_odly[HSDDR52_DDR50 * MAX_CLK_FREQ_NUM + CLK_50M] = 1;
		mmcpriv->tm1.def_sdly[HSDDR52_DDR50 * MAX_CLK_FREQ_NUM + CLK_50M] = 1;

		/* timing mode 4 */
		mmcpriv->tm4.cur_spd_md = DS26_SDR12;
		mmcpriv->tm4.cur_freq = CLK_400K;
		mmcpriv->tm4.sample_point_cnt = MMC_CLK_SAMPLE_POINIT_MODE_4;

		mmcpriv->tm4.def_odly[DS26_SDR12*MAX_CLK_FREQ_NUM+CLK_400K] = TM4_OUT_PH180;
		mmcpriv->tm4.def_sdly[DS26_SDR12*MAX_CLK_FREQ_NUM+CLK_400K] = 0x0;
		mmcpriv->tm4.def_odly[DS26_SDR12*MAX_CLK_FREQ_NUM+CLK_25M] = TM4_OUT_PH180;
		mmcpriv->tm4.def_sdly[DS26_SDR12*MAX_CLK_FREQ_NUM+CLK_25M] = 0x0;

		mmcpriv->tm4.def_odly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_400K] = TM4_OUT_PH180;
		mmcpriv->tm4.def_sdly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_400K] = 0;
		mmcpriv->tm4.def_odly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_25M] = TM4_OUT_PH180;
		mmcpriv->tm4.def_sdly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_25M] = 0;
		mmcpriv->tm4.def_odly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_50M] = TM4_OUT_PH180;
		mmcpriv->tm4.def_sdly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_50M] = 0;

		mmcpriv->tm4.def_odly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_400K] = TM4_OUT_PH180;
		mmcpriv->tm4.def_sdly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_400K] = 0xe;
		mmcpriv->tm4.def_odly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_25M] = TM4_OUT_PH180;
		mmcpriv->tm4.def_sdly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_25M] = 0xe;
		mmcpriv->tm4.def_odly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_50M] = TM4_OUT_PH180;
		mmcpriv->tm4.def_sdly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_50M] = 0xe;

		mmcpriv->tm4.def_odly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_400K] = TM4_OUT_PH180;
		mmcpriv->tm4.def_sdly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_400K] = 0x0;
		mmcpriv->tm4.def_odly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_25M] = TM4_OUT_PH90;
		mmcpriv->tm4.def_sdly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_25M] = 0x0;
		mmcpriv->tm4.def_odly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_50M] = TM4_OUT_PH90;
		mmcpriv->tm4.def_sdly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_50M] = 0x11;
		mmcpriv->tm4.def_odly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_100M] = TM4_OUT_PH90;
		mmcpriv->tm4.def_sdly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_100M] = 0x12;
		mmcpriv->tm4.def_odly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_150M] = TM4_OUT_PH90;
		mmcpriv->tm4.def_sdly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_150M] = 0x13;
		mmcpriv->tm4.def_odly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_200M] = TM4_OUT_PH90;
		mmcpriv->tm4.def_sdly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_200M] = 0x6;

		mmcpriv->tm4.def_odly[HS400*MAX_CLK_FREQ_NUM+CLK_400K] = TM4_OUT_PH180;
		mmcpriv->tm4.def_sdly[HS400*MAX_CLK_FREQ_NUM+CLK_400K] = 0x0;
		mmcpriv->tm4.def_odly[HS400*MAX_CLK_FREQ_NUM+CLK_25M] = TM4_OUT_PH90;
		mmcpriv->tm4.def_sdly[HS400*MAX_CLK_FREQ_NUM+CLK_25M] = 0x0;
		mmcpriv->tm4.def_odly[HS400*MAX_CLK_FREQ_NUM+CLK_50M] = TM4_OUT_PH90;
		mmcpriv->tm4.def_sdly[HS400*MAX_CLK_FREQ_NUM+CLK_50M] = 0x11;
		mmcpriv->tm4.def_odly[HS400*MAX_CLK_FREQ_NUM+CLK_100M] = TM4_OUT_PH90;
		mmcpriv->tm4.def_sdly[HS400*MAX_CLK_FREQ_NUM+CLK_100M] = 0x12;
		mmcpriv->tm4.def_odly[HS400*MAX_CLK_FREQ_NUM+CLK_150M] = TM4_OUT_PH90;
		mmcpriv->tm4.def_sdly[HS400*MAX_CLK_FREQ_NUM+CLK_150M] = 0x13;
		mmcpriv->tm4.def_odly[HS400*MAX_CLK_FREQ_NUM+CLK_200M] = TM4_OUT_PH90;
		mmcpriv->tm4.def_sdly[HS400*MAX_CLK_FREQ_NUM+CLK_200M] = 0x6;

		mmcpriv->tm4.def_dsdly[CLK_25M] = 0x0;
		mmcpriv->tm4.def_dsdly[CLK_50M] = 0x18;
		mmcpriv->tm4.def_dsdly[CLK_100M] = 0xd;
		mmcpriv->tm4.def_dsdly[CLK_150M] = 0x6;
		mmcpriv->tm4.def_dsdly[CLK_200M] = 0x3;
	}

	return 0;
}


static int mmc_get_sdly_auto_sample(int sdc_no)
{
#if 1
	struct sunxi_mmc_priv *mmcpriv = &mmc_host[sdc_no];
	u32 *p = (u32 *)&mmcpriv->cfg.sdly.tm4_smx_fx[0];
	int spd_md, f;
	u32 val;
	int work_mode = uboot_spare_head.boot_data.work_mode;
	struct boot_sdmmc_private_info_t *priv_info =
		(struct boot_sdmmc_private_info_t *)(uboot_spare_head.boot_data.sdcard_spare_data);

	if (sdc_no != 2) {
		pr_info("don't support auto sample\n");
		return -1;
	}

	/* sdly is invalid */
	if (work_mode != WORK_MODE_BOOT) {
		pr_info("Is not Boot mode!\n");
		return 0;
	}

	if (!(((priv_info->ext_para0 & 0xFF000000) == EXT_PARA0_ID)
		&& (priv_info->ext_para0 & EXT_PARA0_TUNING_SUCCESS_FLAG))) {
		pr_info("Is not EXT_PARA0_ID or EXT_PARA0_TUNING_SUCCESS_FLAG!\n");
		return 0;
	}

#if 0
	for (f = 0; f < 5; f++)
		pr_err("0x%x 0x%x\n", p[f*2 + 0], p[f*2 + 1]);
#endif
	for (spd_md = 0; spd_md < MAX_SPD_MD_NUM; spd_md++) {
		if (spd_md == HS400) {
			val = p[spd_md*2 + 0];
			for (f = 0; f < 4; f++) {
				mmcpriv->tm4.dsdly[f] = (val>>(f*8)) & 0xFF;
				/*pr_err("hs400 dsdly-0 0x%x\n", mmcpriv->tm4.dsdly[f]);*/
			}

			val = p[spd_md*2 + 1];
			for (f = 4; f < MAX_CLK_FREQ_NUM; f++) {
				mmcpriv->tm4.dsdly[f] = (val>>((f-4)*8)) & 0xFF;
				/*pr_err("hs400 dsdly-1 0x%x\n", mmcpriv->tm4.dsdly[f]);*/
			}

			val = p[spd_md*2 + 2 + 0];
			for (f = 0; f < 4; f++) {
				mmcpriv->tm4.sdly[spd_md*MAX_CLK_FREQ_NUM + f] = (val>>(f*8)) & 0xFF;
				/*pr_err("hs400 sdly-0 0x%x\n", mmcpriv->tm4.sdly[spd_md*MAX_CLK_FREQ_NUM + f]);*/
			}

			val = p[spd_md*2 + 2 + 1];
			for (f = 4; f < MAX_CLK_FREQ_NUM; f++) {
				mmcpriv->tm4.sdly[spd_md*MAX_CLK_FREQ_NUM + f] = (val>>((f-4)*8)) & 0xFF;
				/*pr_err("hs400 sdly-1 0x%x\n", mmcpriv->tm4.sdly[spd_md*MAX_CLK_FREQ_NUM + f]);*/
			}
		} else {
			val = p[spd_md*2 + 0];
			for (f = 0; f < 4; f++) {
				mmcpriv->tm4.sdly[spd_md*MAX_CLK_FREQ_NUM + f] = (val>>(f*8)) & 0xFF;
				/*pr_err("sdly-0 0x%x\n", mmcpriv->tm4.sdly[spd_md*MAX_CLK_FREQ_NUM + f]);*/
			}

			val = p[spd_md*2 + 1];
			for (f = 4; f < MAX_CLK_FREQ_NUM; f++) {
				mmcpriv->tm4.sdly[spd_md*MAX_CLK_FREQ_NUM + f] = (val>>((f-4)*8)) & 0xFF;
				/*pr_err("sdly-1 0x%x\n", mmcpriv->tm4.sdly[spd_md*MAX_CLK_FREQ_NUM + f]);*/
			}
		}

	}
	return 0;
#else
	pr_err("don't support auto sample\n");
	return 0;
#endif
}

static int mmc_get_dly_manual_sample(int sdc_no)
{
	struct sunxi_mmc_priv *mmcpriv = &mmc_host[sdc_no];
	struct mmc_config *cfg = &mmcpriv->cfg;
	int imd, ifreq;

	if (sdc_no == 2) {
		if (mmcpriv->timing_mode == SUNXI_MMC_TIMING_MODE_1) {
			struct sunxi_mmc_timing_mode1 *p = &mmcpriv->tm1;

			for (imd = 0; imd < MAX_SPD_MD_NUM; imd++) {
				for (ifreq = 0; ifreq < MAX_CLK_FREQ_NUM; ifreq++) {
					//pr_err("%d %d\n", cfg->odly_spd_freq[imd*MAX_CLK_FREQ_NUM + ifreq], cfg->sdly_spd_freq[imd*MAX_CLK_FREQ_NUM + ifreq]);
					p->odly[imd*MAX_CLK_FREQ_NUM + ifreq] = cfg->odly_spd_freq[imd*MAX_CLK_FREQ_NUM + ifreq];
					p->sdly[imd*MAX_CLK_FREQ_NUM + ifreq] = cfg->sdly_spd_freq[imd*MAX_CLK_FREQ_NUM + ifreq];
				}
				//pr_err("\n");
			}
		} else {
			pr_err("sdc %d timing mode %d error --1\n", sdc_no, mmcpriv->timing_mode);
			return -1;
		}
	} else if (sdc_no == 0) {
		if (mmcpriv->timing_mode == SUNXI_MMC_TIMING_MODE_0) {
			struct sunxi_mmc_timing_mode0 *p = &mmcpriv->tm0;

			for (imd = 0; imd < MAX_SPD_MD_NUM; imd++) {
				for (ifreq = 0; ifreq < MAX_CLK_FREQ_NUM; ifreq++) {
					//pr_err("%d %d\n", cfg->odly_spd_freq[imd*MAX_CLK_FREQ_NUM + ifreq], cfg->sdly_spd_freq[imd*MAX_CLK_FREQ_NUM + ifreq]);
					p->odly[imd*MAX_CLK_FREQ_NUM + ifreq] = cfg->odly_spd_freq[imd*MAX_CLK_FREQ_NUM + ifreq];
					p->sdly[imd*MAX_CLK_FREQ_NUM + ifreq] = cfg->sdly_spd_freq[imd*MAX_CLK_FREQ_NUM + ifreq];
				}
				//pr_err("\n");
			}
		} else {
			pr_err("sdc %d timing mode %d error --2\n", sdc_no, mmcpriv->timing_mode);
			return -1;
		}
	} else {
		pr_err("mmc_get_dly_manual_sample: error sdc_no %d\n", sdc_no);
		return -1;
	}

	return 0;
}

static int mmc_update_timing_para(int sdc_no)
{
	int ret = 0;
	struct sunxi_mmc_priv *mmcpriv = &mmc_host[sdc_no];
	struct mmc_config *p = &mmcpriv->cfg;

	if (p->sample_mode == AUTO_SAMPLE_MODE) {
		ret = mmc_get_sdly_auto_sample(sdc_no);
		if (ret) {
			pr_err("update auto sample timing para fail!\n");
		}
	} else if (p->sample_mode == MAUNAL_SAMPLE_MODE) {
		ret = mmc_get_dly_manual_sample(sdc_no);
		if (ret) {
			pr_err("update manual sample timing para fail!\n");
		}
	} else {
		pr_info("Using default timing para\n");
		mmcpriv->tm1.odly[HSDDR52_DDR50 * MAX_CLK_FREQ_NUM + CLK_50M] =
			mmcpriv->tm1.def_odly[HSDDR52_DDR50 * MAX_CLK_FREQ_NUM + CLK_50M];
		mmcpriv->tm1.sdly[HSDDR52_DDR50 * MAX_CLK_FREQ_NUM + CLK_50M] =
			mmcpriv->tm1.def_sdly[HSDDR52_DDR50* MAX_CLK_FREQ_NUM + CLK_50M];
		mmcpriv->tm4.odly[HSSDR52_SDR25 * MAX_CLK_FREQ_NUM + CLK_50M] =
			mmcpriv->tm4.def_odly[HSSDR52_SDR25 * MAX_CLK_FREQ_NUM + CLK_50M];
		mmcpriv->tm4.sdly[HSSDR52_SDR25 * MAX_CLK_FREQ_NUM + CLK_50M] =
			mmcpriv->tm4.def_sdly[HSSDR52_SDR25* MAX_CLK_FREQ_NUM + CLK_50M];
	}

	return ret;
}


static void mmc_get_para_from_fex(int sdc_no)
{
	int rval, ret = 0;
	//int rval_ker, ret1 = 0;
	struct sunxi_mmc_priv *mmcpriv = &mmc_host[sdc_no];
	struct mmc_config *cfg = &mmcpriv->cfg;
	struct sunxi_mmc_pininfo *pin_default = &mmcpriv->pin_default;
	struct sunxi_mmc_pininfo *pin_disable = &mmcpriv->pin_disable;
	int nodeoffset = 0;
	int i, imd, ifreq;
	char ctmp[30];
	u32 handle[10] = {0};
	user_gpio_set_t card_pwr_gpio;

	if (sdc_no == 0) {
		nodeoffset =  fdt_path_offset(working_fdt, FDT_PATH_CARD0_BOOT_PARA);
		if (nodeoffset < 0) {
			pr_err("get card0 para fail\n");
			return ;
		}

		pin_default->pin_count = fdt_get_all_pin(nodeoffset, "pinctrl-0", pin_default->pin_set);
			if (pin_default->pin_count <= 0) {
				pr_err("get card0 default pin fail\n");
				return ;
		}

		/*avoid the error print from fdt_get_all_pin*/
		pin_disable->pin_count = fdt_getprop_u32(working_fdt, nodeoffset, "pinctrl-1", handle);
		if (pin_disable->pin_count > 0) {
			pin_disable->pin_count = fdt_get_all_pin(nodeoffset, "pinctrl-1", pin_disable->pin_set);
		}
		if (pin_disable->pin_count <= 0) {
			pr_info("get card0 disable pin fail\n");
		}

		/* set gpio */
		//gpio_request_simple("card0_boot_para", NULL);

		//ret = script_parser_fetch("card0_boot_para", "sdc_wipe", &rval, 1);
		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_wipe", (uint32_t *)(&rval));
		if (ret < 0)
			pr_debug("get sdc_wipe fail.\n");
		else {
			if (rval & DRV_PARA_DISABLE_SECURE_WIPE) {
				pr_info("disable driver secure wipe operation.\n");
				cfg->drv_wipe_feature |= DRV_PARA_DISABLE_SECURE_WIPE;
			}
			if (rval & DRV_PARA_DISABLE_EMMC_SANITIZE) {
				pr_info("disable emmc sanitize feature.\n");
				cfg->drv_wipe_feature |= DRV_PARA_DISABLE_EMMC_SANITIZE;
			}
			if (rval & DRV_PARA_DISABLE_EMMC_SECURE_PURGE) {
				pr_info("disable emmc secure purge feature.\n");
				cfg->drv_wipe_feature |= DRV_PARA_DISABLE_EMMC_SECURE_PURGE;
			}
			if (rval & DRV_PARA_DISABLE_EMMC_TRIM) {
				pr_info("disable emmc trim feature.\n");
				cfg->drv_wipe_feature |= DRV_PARA_DISABLE_EMMC_TRIM;
			}
		}

		//ret = script_parser_fetch("card0_boot_para", "sdc_erase", &rval, 1);
		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_erase", (uint32_t *)(&rval));
		if (ret < 0)
			pr_debug("get sdc0 sdc_erase fail.\n");
		else {
			if (rval & DRV_PARA_DISABLE_EMMC_ERASE) {
				pr_info("disable emmc erase.\n");
				cfg->drv_erase_feature |= DRV_PARA_DISABLE_EMMC_ERASE;
			}
			if (rval & DRV_PARA_ENABLE_EMMC_SANITIZE_WHEN_ERASE) {
				pr_info("enable emmc sanitize when erase.\n");
				cfg->drv_erase_feature |= DRV_PARA_ENABLE_EMMC_SANITIZE_WHEN_ERASE;
			}
		}

		//ret = script_parser_fetch("card0_boot_para", "sdc_boot", &rval, 1);
		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_boot", (uint32_t *)(&rval));
		if (ret < 0)
			pr_debug("get sdc0 sdc_boot fail.\n");
		else {
			if (rval & DRV_PARA_NOT_BURN_USER_PART) {
				pr_info("don't burn boot0 to user part.\n");
				cfg->drv_burn_boot_pos |= DRV_PARA_NOT_BURN_USER_PART;
			}
			if (rval & DRV_PARA_BURN_EMMC_BOOT_PART) {
				pr_info("burn boot0 to emmc boot part.\n");
				cfg->drv_burn_boot_pos |= DRV_PARA_BURN_EMMC_BOOT_PART;
			}
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_odly_50M", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc0 sdc_odly_50M fail.\n");
			cfg->boot_odly_50M = 0xff;
		} else {
			pr_info("get sdc0 sdc_odly_50M %d.\n", rval);
			cfg->boot_odly_50M = rval;
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_sdly_50M", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc0 sdc_sdly_50M fail.\n");
			cfg->boot_sdly_50M = 0xff;
		} else {
			pr_info("get sdc0 sdc_sdly_50M %d.\n", rval);
			cfg->boot_sdly_50M = rval;

		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_odly_50M_ddr", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc0 sdc_odly_50M_ddr fail.\n");
			cfg->boot_odly_50M_ddr = 0xff;
		} else {
			pr_info("get sdc0 sdc_odly_50M_ddr %d.\n", rval);
			cfg->boot_odly_50M_ddr = rval;
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_sdly_50M_ddr", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc0 sdc_sdly_50M_ddr fail.\n");
			cfg->boot_sdly_50M_ddr = 0xff;
		} else {
			pr_info("get sdc0 sdc_sdly_50M_ddr %d.\n", rval);
			cfg->boot_sdly_50M_ddr = rval;
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_freq", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc0 sdc_freq fail.\n");
			cfg->boot_hs_f_max = 0x0;
		} else {
			if (rval >= 50)
				cfg->boot_hs_f_max = 50;
			else
				cfg->boot_hs_f_max = rval;
			pr_info("get sdc0 sdc_freq %d.%d\n", rval, cfg->boot_hs_f_max);
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_b0p", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc0 sdc_b0p fail.\n");
			cfg->boot0_para = 0x0;
		} else {
			pr_info("get sdc0 sdc_b0p %d.\n", rval);
			cfg->boot0_para = rval;
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_ex_dly_used", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get card0_boot_para:sdc_ex_dly_used fail\n");
		} else {
			if (rval == 1) {  /* maual sample point from fex */
				cfg->sample_mode = MAUNAL_SAMPLE_MODE;
				pr_info("get sdc_ex_dly_used %d, use manual sdly in fex\n", rval);
			} else {
				cfg->sample_mode = 0x0;
				pr_info("undefined value %d, use default dly\n", rval);
			}
		}

		ret = fdt_get_one_gpio(FDT_PATH_CARD0_BOOT_PARA, "card-pwr-gpios",
							&card_pwr_gpio);
		if (ret < 0) {
			pr_info("get card-pwr-gpios faild\n");
			mmcpriv->pwr_handler = 0;
		} else {
			mmcpriv->pwr_handler = sunxi_gpio_request(&card_pwr_gpio, 1);
			pr_info("get card-pwr-gpios handler:%d\n", mmcpriv->pwr_handler);
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "time_pwroff_ms", (uint32_t *)(&rval));
		if (ret < 0) {
			mmcpriv->time_pwroff = 200;
			pr_info("get card0_boot_para:time_pwroff_ms:use default time_pwroff:%d\n",
				mmcpriv->time_pwroff);
		} else {
			mmcpriv->time_pwroff = rval;
			pr_info("get card0_boot_para:time_pwroff:%dms\n", mmcpriv->time_pwroff);
		}

		if (cfg->sample_mode == MAUNAL_SAMPLE_MODE) {
			for (imd = 0; imd < MAX_SPD_MD_NUM; imd++) {
				for (ifreq = 0; ifreq < MAX_CLK_FREQ_NUM; ifreq++) {
					sprintf(ctmp, "sdc_odly_%d_%d", imd, ifreq);
					ret = fdt_getprop_u32(working_fdt, nodeoffset, ctmp, (uint32_t *)(&rval));
					if (ret < 0) {
						pr_debug("get sdc0 %s fail.\n", ctmp);
						cfg->odly_spd_freq[imd*MAX_CLK_FREQ_NUM + ifreq] = 0x0;
					} else {
						cfg->odly_spd_freq[imd*MAX_CLK_FREQ_NUM + ifreq] = rval&0x1;
						pr_info("get sdc0 %s 0x%x for %d-%s %d.\n", ctmp,
								cfg->odly_spd_freq[imd*MAX_CLK_FREQ_NUM + ifreq], imd, spd_name[imd], ifreq);
					}

					sprintf(ctmp, "sdc_sdly_%d_%d", imd, ifreq);
					ret = fdt_getprop_u32(working_fdt, nodeoffset, ctmp, (uint32_t *)(&rval));
					if (ret < 0) {
						pr_debug("get sdc0 %s fail.\n", ctmp);
						cfg->sdly_spd_freq[imd*MAX_CLK_FREQ_NUM + ifreq] = 0xff;
					} else {
						cfg->sdly_spd_freq[imd*MAX_CLK_FREQ_NUM + ifreq] = rval&0xff;
						pr_info("get sdc0 %s 0x%x for %d-%s %d.\n", ctmp,
								cfg->sdly_spd_freq[imd*MAX_CLK_FREQ_NUM + ifreq], imd, spd_name[imd], ifreq);
					}
				}
			}
		}
	} else if (sdc_no == 2) {
		nodeoffset =  fdt_path_offset(working_fdt, FDT_PATH_GPIO_BIAS);
		if (nodeoffset < 0) {
			pr_err("get gpio bias fail\n");
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "pc_bias",
				(uint32_t *)(&rval));
		if (ret < 0) {
			pr_err("get gpio_bias: pc_bias fail\n");
		} else {
			if (rval == 1800) {
				ret = readl((SUNXI_PIO_BASE + GPIO_POW_MODE_REG));
				ret |= (1 << 2);
				writel(ret, (SUNXI_PIO_BASE + GPIO_POW_MODE_REG));
				pr_info("Set eMMC Power Mode to 1.8V!\n");
				cfg->pc_bias = 1800;
			} else {
				pr_warning("Warning! The eMMC power mode is 3.3V!\n");
			}
		}

		nodeoffset =  fdt_path_offset(working_fdt, FDT_PATH_CARD2_BOOT_PARA);
		if (nodeoffset < 0) {
			pr_err("get card2 para fail\n");
			return ;
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_io_1v8",
				(uint32_t *)(&rval));
		if (ret < 0) {
			pr_err("get card2_boot_para:sdc_io_1v8 fail\n");
		} else {
			if (rval == 1) {
				pr_info("card2 io is 1.8V.\n");
				cfg->io_is_1v8 = 1;
			} else {
				pr_info("card2 io is 3V.\n");
			}
		}

		pin_default->pin_count = fdt_get_all_pin(nodeoffset, "pinctrl-0", pin_default->pin_set);
		if (pin_default->pin_count <= 0) {
			pr_err("get card2 default pin fail\n");
			return ;
		}

		/*avoid the error print from fdt_get_all_pin*/
		pin_disable->pin_count = fdt_getprop_u32(working_fdt, nodeoffset, "pinctrl-1", handle);
		if (pin_disable->pin_count > 0) {
			pin_disable->pin_count = fdt_get_all_pin(nodeoffset, "pinctrl-1", pin_disable->pin_set);
		}
		if (pin_disable->pin_count <= 0) {
			pr_info("get card2 disable pin fail\n");
		}
		//gpio_request_simple("card2_boot_para", NULL);
		//pr_err("=====~~~~~~~~~~~~0x%x 0x%x 0x%x\n", *(volatile u32 *)(0x1c20848), *(volatile u32 *)(0x1c2084C), *(volatile u32 *)(0x1c20850));

		//ret = script_parser_fetch("card2_boot_para", "sdc_wipe", &rval, 1);
		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_wipe", (uint32_t *)(&rval));
		if (ret < 0)
			pr_debug("get sdc_wipe fail.\n");
		else {
			if (rval & DRV_PARA_DISABLE_SECURE_WIPE) {
				pr_info("disable driver secure wipe operation.\n");
				cfg->drv_wipe_feature |= DRV_PARA_DISABLE_SECURE_WIPE;
			}
			if (rval & DRV_PARA_DISABLE_EMMC_SANITIZE) {
				pr_info("disable emmc sanitize feature.\n");
				cfg->drv_wipe_feature |= DRV_PARA_DISABLE_EMMC_SANITIZE;
			}
			if (rval & DRV_PARA_DISABLE_EMMC_SECURE_PURGE) {
				pr_info("disable emmc secure purge feature.\n");
				cfg->drv_wipe_feature |= DRV_PARA_DISABLE_EMMC_SECURE_PURGE;
			}
			if (rval & DRV_PARA_DISABLE_EMMC_TRIM) {
				pr_info("disable emmc trim feature.\n");
				cfg->drv_wipe_feature |= DRV_PARA_DISABLE_EMMC_TRIM;
			}
		}

		//ret = script_parser_fetch("card2_boot_para", "sdc_erase", &rval, 1);
		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_erase", (uint32_t *)(&rval));
		if (ret < 0)
			pr_debug("get sdc2 sdc_erase fail.\n");
		else {
			if (rval & DRV_PARA_DISABLE_EMMC_ERASE) {
				pr_info("disable emmc erase.\n");
				cfg->drv_erase_feature |= DRV_PARA_DISABLE_EMMC_ERASE;
			}
			if (rval & DRV_PARA_ENABLE_EMMC_SANITIZE_WHEN_ERASE) {
				pr_info("enable emmc sanitize when erase.\n");
				cfg->drv_erase_feature |= DRV_PARA_ENABLE_EMMC_SANITIZE_WHEN_ERASE;
			}
		}

		//ret = script_parser_fetch("card2_boot_para", "sdc_boot", &rval, 1);
		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_boot", (uint32_t *)(&rval));
		if (ret < 0)
			pr_debug("get sdc2 sdc_boot fail.\n");
		else {
			if (rval & DRV_PARA_NOT_BURN_USER_PART) {
				pr_info("don't burn boot0 to user part.\n");
				cfg->drv_burn_boot_pos |= DRV_PARA_NOT_BURN_USER_PART;
			}
			if (rval & DRV_PARA_BURN_EMMC_BOOT_PART) {
				pr_info("burn boot0 to emmc boot part.\n");
				cfg->drv_burn_boot_pos |= DRV_PARA_BURN_EMMC_BOOT_PART;
			}
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_wp", (uint32_t *)(&rval));
		if (ret < 0)
			pr_debug("get sdc2 sdc_wp fail.\n");
		else {
			if (rval & DRV_PARA_ENABLE_EMMC_USER_PART_WP) {
				pr_info("enable emmc write protect.\n");
				cfg->drv_wp_feature |= DRV_PARA_ENABLE_EMMC_USER_PART_WP;
			}
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_hc_cap_unit", (uint32_t *)(&rval));
		if (ret < 0)
			pr_debug("get sdc2 sdc_hc_cap_unit fail.\n");
		else {
			if (rval & DRV_PARA_ENABLE_EMMC_USER_PART_WP) {
				pr_info("enable emmc high capacity-erase/high-capacity write protect unit size.\n");
				cfg->drv_hc_cap_unit_feature |= DRV_PARA_ENABLE_EMMC_HC_CAP_UNIT;
			}
		}


		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_cal_delay_unit", (uint32_t *)(&rval));
		if (ret < 0)
			pr_debug("get card2_boot_para:sdc_cal_delay_unit fail\n");
		else {
			pr_info("card2 calibration delay unit\n");
			cfg->cal_delay_unit = 1;
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_ex_dly_used", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_err("get card2_boot_para:sdc_ex_dly_used fail\n");

			/* fill invalid sample config */
			memset(mmcpriv->cfg.sdly.tm4_smx_fx,
					0xff, sizeof(mmcpriv->cfg.sdly.tm4_smx_fx));
		}
		pr_info("get sdc_ex_dly_used %d, use auto tuning sdly\n", rval);

		if (!(ret < 0)/* && !(ret1 < 0)*/) {
			int work_mode = uboot_spare_head.boot_data.work_mode;
			struct boot_sdmmc_private_info_t *priv_info =
				(struct boot_sdmmc_private_info_t *)(uboot_spare_head.boot_data.sdcard_spare_data);
			struct tune_sdly *sdly = &(priv_info->tune_sdly);
			u32 *p = (u32 *)&mmcpriv->cfg.sdly.tm4_smx_fx[0];

			if (/*(rval_ker == 2) &&*/ (rval == 2)) { //only when kernal use auto sample,uboot will use auto sample.
				cfg->sample_mode = AUTO_SAMPLE_MODE;
				pr_info("get sdc_ex_dly_used %d, use auto tuning sdly\n", rval);
				if (work_mode != WORK_MODE_BOOT) {
					/*usb product will auto get sample point, no need to get auto sdly, so first used default value*/
					pr_info("current is product mode, it will tune sdly later\n");
				} else {
					/* copy sample point cfg from uboot header to internal variable */
					if (((priv_info->ext_para0 & 0xFF000000) == EXT_PARA0_ID)
						&& (priv_info->ext_para0 & EXT_PARA0_TUNING_SUCCESS_FLAG)) {
						if (sdly != NULL)
							memcpy(p, sdly, sizeof(struct tune_sdly));
						else
							memset(p, 0xFF, sizeof(struct tune_sdly));
					} else {
						memset(p, 0xFF, sizeof(struct tune_sdly));
						pr_info("get sdly from uboot header fail\n");
					}
				}
			} else if (rval == 1) {  /* maual sample point from fex */
				cfg->sample_mode = MAUNAL_SAMPLE_MODE;
				pr_info("get sdc_ex_dly_used %d, use manual sdly in fex\n", rval);
			} else {
				cfg->sample_mode = 0x0;
				pr_info("undefined value %d, use default dly\n", rval);
			}
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_force_boot_tuning", (uint32_t *)(&rval));
		if (ret < 0)
			pr_debug("get card2_boot_para:sdc_force_boot_tuning fail\n");
		else {
			if (rval == 1) {
				pr_info("card2 force to execute tuning during boot.\n");
				cfg->force_boot_tuning = 1;
			} else {
				pr_info("card2 don't force to execute tuning during boot.\n");
				cfg->force_boot_tuning = 0;
			}
		}


		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_odly_50M", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc2 sdc_odly_50M fail.\n");
			cfg->boot_odly_50M = 0xff;
		} else {
			pr_info("get sdc2 sdc_odly_50M %d.\n", rval);
			cfg->boot_odly_50M = rval;
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_sdly_50M", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc2 sdc_sdly_50M fail.\n");
			cfg->boot_sdly_50M = 0xff;
		} else {
			pr_info("get sdc2 sdc_sdly_50M %d.\n", rval);
			cfg->boot_sdly_50M = rval;
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_odly_50M_ddr", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc2 sdc_odly_50M_ddr fail.\n");
			cfg->boot_odly_50M_ddr = 0xff;
		} else {
			pr_info("get sdc2 sdc_odly_50M_ddr %d.\n", rval);
			cfg->boot_odly_50M_ddr = rval;
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_sdly_50M_ddr", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc2 sdc_sdly_50M_ddr fail.\n");
			cfg->boot_sdly_50M_ddr = 0xff;
		} else {
			pr_info("get sdc2 sdc_sdly_50M_ddr %d.\n", rval);
			cfg->boot_sdly_50M_ddr = rval;
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_freq", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc2 sdc_freq fail.\n");
			cfg->boot_hs_f_max = 0x0;
		} else {
			if (rval >= 50)
				cfg->boot_hs_f_max = 50;
			else
				cfg->boot_hs_f_max = rval;
			pr_info("get sdc2 sdc_freq %d.%d\n", rval, cfg->boot_hs_f_max);
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_boot0_sup_1v8", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_err("get sdc2 sdc_boot0_sup_1v8 fail.\n");
			cfg->boot0_sup_1v8 = 0x0;
		} else {
			if (rval == 1)
				cfg->boot0_sup_1v8 = rval;
			else
				cfg->boot0_sup_1v8 = 0x0;
			pr_info("get sdc2 sdc_boot0_sup_1v8 %d\n", cfg->boot0_sup_1v8);
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_b0p", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc2 sdc_b0p fail.\n");
			cfg->boot0_para = 0x0;
		} else {
			cfg->boot0_para = rval;
			pr_info("get sdc2 sdc_b0p %d.\n", cfg->boot0_para);
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_tm4_win_th", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc2 sdc_tm4_win_th fail.\n");
			cfg->tm4_timing_window_th = 12;
		} else {
			if ((rval < 4) || (rval > 64))
				cfg->tm4_timing_window_th = 12;
			else
				cfg->tm4_timing_window_th = rval;
			pr_info("get sdc2 sdc_tm4_win_th %d.\n", cfg->tm4_timing_window_th);
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_tm4_r_cycle", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc2 sdc_tm4_r_cycle fail.\n");
			cfg->tm4_tune_r_cycle = 0x0;
		} else {
			if ((rval < 1) || (rval > 40))
				cfg->tm4_tune_r_cycle = 15;
			else
				cfg->tm4_tune_r_cycle = rval;
			pr_info("get sdc2 sdc_tm4_r_cycle %d.\n", cfg->tm4_tune_r_cycle);
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_tm4_hs200_max_freq", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc2 sdc_tm4_hs200_max_freq fail.\n");
			cfg->tm4_tune_hs200_max_freq = 0x0;
		} else {
			if ((rval < 50) || (rval > 200))
				cfg->tm4_tune_hs200_max_freq = 0x0;
			else
				cfg->tm4_tune_hs200_max_freq = rval;
			pr_info("get sdc2 sdc_tm4_hs200_max_freq %d.\n", cfg->tm4_tune_hs200_max_freq);
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_tm4_hs400_max_freq", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc2 sdc_tm4_hs400_max_freq fail.\n");
			cfg->tm4_tune_hs400_max_freq = 0x0;
		} else {
			if ((rval < 50) || (rval > 200))
				cfg->tm4_tune_hs400_max_freq = 0x0;
			else
				cfg->tm4_tune_hs400_max_freq = rval;
			pr_info("get sdc2 sdc_tm4_hs400_max_freq %d.\n", cfg->tm4_tune_hs400_max_freq);
		}

		for (i = 0; i < MAX_EXT_FREQ_POINT_NUM; i++) {
			sprintf(ctmp, "sdc_tm4_ext_freq_%d", i);
			ret = fdt_getprop_u32(working_fdt, nodeoffset, ctmp, (uint32_t *)(&rval));
			if (ret < 0) {
				pr_debug("get sdc2 %s fail.\n", ctmp);
				cfg->tm4_tune_ext_freq[i] = 0x0;
			} else {
				pr_info("get sdc2 %s 0x%x.\n", ctmp, rval);
				if ((rval & 0xff) >= 25)
					cfg->tm4_tune_ext_freq[i] = rval;
				else {
					cfg->tm4_tune_ext_freq[i] = 0x0;
					pr_info("invalid freq %d MHz, discard this ext freq point\n", (rval & 0xff));
				}
			}
		}

		ret = fdt_get_one_gpio(FDT_PATH_CARD2_BOOT_PARA, "card-pwr-gpios",
							&card_pwr_gpio);
		if (ret < 0) {
			pr_info("get card-pwr-gpios faild\n");
			mmcpriv->pwr_handler = 0;
		} else {
			mmcpriv->pwr_handler = sunxi_gpio_request(&card_pwr_gpio, 1);
			pr_info("get card-pwr-gpios handler:%d\n", mmcpriv->pwr_handler);
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "time_pwroff_ms", (uint32_t *)(&rval));
		if (ret < 0) {
			mmcpriv->time_pwroff = 200;
			pr_info("get card2_boot_para:time_pwroff_ms:use default time_pwroff:%d\n",
				mmcpriv->time_pwroff);
		} else {
			mmcpriv->time_pwroff = rval;
			pr_info("get card2_boot_para:time_pwroff:%dms\n", mmcpriv->time_pwroff);
		}

		if (cfg->sample_mode == MAUNAL_SAMPLE_MODE) {
			for (imd = 0; imd < MAX_SPD_MD_NUM; imd++) {
				for (ifreq = 0; ifreq < MAX_CLK_FREQ_NUM; ifreq++) {
					sprintf(ctmp, "sdc_odly_%d_%d", imd, ifreq);
					ret = fdt_getprop_u32(working_fdt, nodeoffset, ctmp, (uint32_t *)(&rval));
					if (ret < 0) {
						pr_debug("get sdc2 %s fail.\n", ctmp);
						cfg->odly_spd_freq[imd * MAX_CLK_FREQ_NUM + ifreq] = 0x0;
					} else {
						cfg->odly_spd_freq[imd * MAX_CLK_FREQ_NUM + ifreq] = rval & 0x1;
						pr_info("get sdc2 %s 0x%x for %d-%s %d.\n", ctmp,
							cfg->odly_spd_freq[imd*MAX_CLK_FREQ_NUM + ifreq], imd, spd_name[imd], ifreq);
					}

					sprintf(ctmp, "sdc_sdly_%d_%d", imd, ifreq);
					ret = fdt_getprop_u32(working_fdt, nodeoffset, ctmp, (uint32_t *)(&rval));
					if (ret < 0) {
						pr_debug("get sdc2 %s fail.\n", ctmp);
						cfg->sdly_spd_freq[imd * MAX_CLK_FREQ_NUM + ifreq] = 0xff;
					} else {
						cfg->sdly_spd_freq[imd * MAX_CLK_FREQ_NUM + ifreq] = rval&0xff;
						pr_info("get sdc2 %s 0x%x for %d-%s %d.\n", ctmp,
							cfg->sdly_spd_freq[imd * MAX_CLK_FREQ_NUM + ifreq], imd, spd_name[imd], ifreq);
					}
				}
			}
		}

		/* speed mode caps */
		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_dis_host_caps", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc2 sdc_dis_host_caps fail.\n");
			cfg->host_caps_mask = 0x0;
		} else {
			cfg->host_caps_mask = rval;
			pr_info("get sdc2 sdc_dis_host_caps 0x%x.\n", cfg->host_caps_mask);
		}

		ret = fdt_getprop_u32(working_fdt, nodeoffset, "sdc_kernel_no_limit", (uint32_t *)(&rval));
		if (ret < 0) {
			pr_debug("get sdc2 sdc_kernel_no_limit fail.\n");
			cfg->tune_limit_kernel_timing = 0x1;
		} else {
			if (rval == 1)
				cfg->tune_limit_kernel_timing = 0x0;
			else
				cfg->tune_limit_kernel_timing = 0x1;
			pr_info("get sdc2 sdc_kernel_no_limit 0x%x, limit 0x%x.\n", rval, cfg->tune_limit_kernel_timing);
		}

		//dumphex32("gpio_config", (char *)SUNXI_PIO_BASE + 0x48, 0x10);
		//dumphex32("gpio_pull", (char *)SUNXI_PIO_BASE + 0x64, 0x8);
		/* fmax, fmax_ddr */
	} else {
		pr_err("%s: input sdc_no error: %d\n", __FUNCTION__, sdc_no);
	}

	return ;
}

static void mmc_get_para_from_dtb(int sdc_no)
{
	int ret = 0;
	//struct sunxi_mmc_priv* mmcpriv = &mmc_priv[sdc_no];
	//struct mmc_config *cfg = &mmcpriv->cfg;
	int nodeoffset;
	char prop_path[128] = {0};
	//u32 prop_val = 0;

	if (sdc_no == 0) {
		strcpy(prop_path, "mmc0");
		nodeoffset = fdt_path_offset(working_fdt, prop_path);
		if (nodeoffset < 0) {
			pr_err("can't find node \"%s\",will add new node\n", prop_path);
			goto __ERRRO_END;
		}
	}

	return ;

__ERRRO_END:
	pr_err("fdt err returned %s\n", fdt_strerror(ret));
	return ;
}

int sunxi_host_mmc_config(int sdc_no)
{
	int ret;

	mmc_clear_timing_para(sdc_no);
	mmc_init_default_timing_para(sdc_no);
	mmc_get_para_from_fex(sdc_no);
	mmc_get_para_from_dtb(sdc_no);
	ret = mmc_update_timing_para(sdc_no);

	return ret;
	//return 0;
}
