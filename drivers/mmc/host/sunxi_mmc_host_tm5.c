// SPDX-License-Identifier: GPL-2.0+
/*
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
#include "sunxi_mmc_host_tm5.h"

#define  SMC_DATA_TIMEOUT     0xffffffU
#define  SMC_RESP_TIMEOUT     0xff

#define SUNXI_DMA_TL_TM5_V5P3X                ((0x3<<28)|(15<<16)|240)
extern char *spd_name[];
extern struct sunxi_mmc_priv mmc_host[4];

static int mmc_init_default_timing_para(int sdc_no)
{
	/* timing mode 5 */
	struct sunxi_mmc_priv *mmcpriv = &mmc_host[sdc_no];
	mmcpriv->tm5.cur_spd_md = DS26_SDR12;
	mmcpriv->tm5.cur_freq = CLK_400K;
	mmcpriv->tm5.sample_point_cnt = MMC_CLK_SAMPLE_POINIT_MODE_5;

	mmcpriv->tm5.def_odly[DS26_SDR12*MAX_CLK_FREQ_NUM+CLK_400K] = TM5_OUT_PH180;
	mmcpriv->tm5.def_sdly[DS26_SDR12*MAX_CLK_FREQ_NUM+CLK_400K] = TM5_IN_PH180;
	mmcpriv->tm5.def_odly[DS26_SDR12*MAX_CLK_FREQ_NUM+CLK_25M] = TM5_OUT_PH180;
	mmcpriv->tm5.def_sdly[DS26_SDR12*MAX_CLK_FREQ_NUM+CLK_25M] = TM5_IN_PH180;

	mmcpriv->tm5.def_odly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_25M] = TM5_OUT_PH180;
	mmcpriv->tm5.def_sdly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_25M] = TM5_IN_PH90;
	mmcpriv->tm5.def_odly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_50M] = TM5_OUT_PH180;
	mmcpriv->tm5.def_sdly[HSSDR52_SDR25*MAX_CLK_FREQ_NUM+CLK_50M] = TM5_IN_PH90;

	mmcpriv->tm5.def_odly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_25M] = TM5_OUT_PH90;
	mmcpriv->tm5.def_sdly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_25M] = TM5_IN_PH180;
	mmcpriv->tm5.def_odly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_50M] = TM5_OUT_PH90;
	mmcpriv->tm5.def_sdly[HSDDR52_DDR50*MAX_CLK_FREQ_NUM+CLK_50M] = TM5_IN_PH180;

	mmcpriv->tm5.def_odly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_25M] = TM5_OUT_PH180;
	mmcpriv->tm5.def_sdly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_25M] = TM5_IN_PH90;
	mmcpriv->tm5.def_odly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_50M] = TM5_OUT_PH180;
	mmcpriv->tm5.def_sdly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_50M] = TM5_IN_PH90;
	mmcpriv->tm5.def_odly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_100M] = TM5_OUT_PH180;
	mmcpriv->tm5.def_sdly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_100M] = TM5_IN_PH90;
	mmcpriv->tm5.def_odly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_150M] = TM5_OUT_PH180;
	mmcpriv->tm5.def_sdly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_150M] = TM5_IN_PH90;
	mmcpriv->tm5.def_odly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_200M] = TM5_OUT_PH180;
	mmcpriv->tm5.def_sdly[HS200_SDR104*MAX_CLK_FREQ_NUM+CLK_200M] = TM5_IN_PH90;

	mmcpriv->tm5.def_odly[HS400*MAX_CLK_FREQ_NUM+CLK_25M] = TM5_OUT_PH90;
	mmcpriv->tm5.def_sdly[HS400*MAX_CLK_FREQ_NUM+CLK_25M] = TM5_IN_PH180;
	mmcpriv->tm5.def_odly[HS400*MAX_CLK_FREQ_NUM+CLK_50M] = TM5_OUT_PH90;
	mmcpriv->tm5.def_sdly[HS400*MAX_CLK_FREQ_NUM+CLK_50M] = TM5_IN_PH180;
	mmcpriv->tm5.def_odly[HS400*MAX_CLK_FREQ_NUM+CLK_100M] = TM5_OUT_PH90;
	mmcpriv->tm5.def_sdly[HS400*MAX_CLK_FREQ_NUM+CLK_100M] = TM5_IN_PH180;
	mmcpriv->tm5.def_odly[HS400*MAX_CLK_FREQ_NUM+CLK_150M] = TM5_OUT_PH90;
	mmcpriv->tm5.def_sdly[HS400*MAX_CLK_FREQ_NUM+CLK_150M] = TM5_IN_PH180;
	mmcpriv->tm5.def_odly[HS400*MAX_CLK_FREQ_NUM+CLK_200M] = TM5_OUT_PH90;
	mmcpriv->tm5.def_sdly[HS400*MAX_CLK_FREQ_NUM+CLK_200M] = TM5_IN_PH180;

	mmcpriv->tm5.def_dsdly[CLK_25M] = 0x0;
	mmcpriv->tm5.def_dsdly[CLK_50M] = 0x18;
	mmcpriv->tm5.def_dsdly[CLK_100M] = 0xd;
	mmcpriv->tm5.def_dsdly[CLK_150M] = 0x6;
	mmcpriv->tm5.def_dsdly[CLK_200M] = 0x3;

	return 0;
}

static void sunxi_mmc_clk_io_onoff(int sdc_no, int onoff, int reset_clk)
{
	int rval;
	struct sunxi_mmc_priv *priv = &mmc_host[sdc_no];

	/* config ahb clock */
	if (onoff) {
		rval = readl(priv->hclkrst);
		rval |= (1 << (16 + sdc_no));
		writel(rval, priv->hclkrst);
		rval = readl(priv->hclkbase);
		rval |= (1 << (0 + sdc_no));
		writel(rval, priv->hclkbase);

		rval = readl(priv->mclkreg);
		rval |= (1U << 31);
		writel(rval, priv->mclkreg);
	} else {
		rval = readl(priv->mclkreg);
		rval &= ~(1U << 31);
		writel(rval, priv->mclkreg);

		rval = readl(priv->hclkbase);
		rval &= ~(1 << (0 + sdc_no));
		writel(rval, priv->hclkbase);

		rval = readl(priv->hclkrst);
		rval &= ~(1 << (16 + sdc_no));
		writel(rval, priv->hclkrst);
	}

	/* config mod clock */
	if (reset_clk) {
		rval = readl(priv->mclkreg);
		/*set to 24M default value*/
		rval &= ~(0x7fffffff);
		sunxi_r_op(priv, writel(rval, priv->mclkreg));
		priv->mod_clk = 24000000;
	}
	/*
	   dumphex32("ccmu", (char *)SUNXI_CCM_BASE, 0x100);
	   dumphex32("gpio", (char *)SUNXI_PIO_BASE, 0x100);
	   dumphex32("mmc", (char *)priv->reg, 0x100);
	   */
}

static int mmc_config_delay(struct sunxi_mmc_priv *mmcpriv)
{
	unsigned int rval = 0;
	unsigned int tmp = 0;
	unsigned int spd_md, freq;
	u8 odly, sdly, dsdly = 0;

	spd_md = mmcpriv->tm5.cur_spd_md;
	freq = mmcpriv->tm5.cur_freq;
	if (mmcpriv->tm5.odly[spd_md*MAX_CLK_FREQ_NUM+freq] != 0xFF)
		odly = mmcpriv->tm5.odly[spd_md*MAX_CLK_FREQ_NUM+freq];
	else
		odly = mmcpriv->tm5.def_odly[spd_md*MAX_CLK_FREQ_NUM+freq];

	if (mmcpriv->tm5.sdly[spd_md*MAX_CLK_FREQ_NUM+freq] != 0xFF)
		sdly = mmcpriv->tm5.sdly[spd_md*MAX_CLK_FREQ_NUM+freq];
	else
		sdly = mmcpriv->tm5.def_sdly[spd_md*MAX_CLK_FREQ_NUM+freq];

	mmcpriv->tm5.cur_odly = odly;
	mmcpriv->tm5.cur_sdly = sdly;

	MMCDBG("%s: odly: %d   sldy: %d\n", __FUNCTION__, odly, sdly);
	rval = readl(&mmcpriv->reg->drv_dl);
	rval &= (~(0x3<<16));
	rval |= (((odly&0x1)<<16) | ((odly&0x1)<<17));
	sunxi_r_op(mmcpriv, writel(rval, &mmcpriv->reg->drv_dl));

	if (spd_md == HS400) {
		if (mmcpriv->tm5.dsdly[freq] != 0xFF)
			dsdly = mmcpriv->tm5.dsdly[freq];
		else
			dsdly = mmcpriv->tm5.def_dsdly[freq];
		mmcpriv->tm5.cur_dsdly = dsdly;

/*
		rval = readl(&mmcpriv->reg->ntsr);
		rval |= 0x1;
		writel(rval, &mmcpriv->reg->ntsr);

		rval = readl(&mmcpriv->reg->ntdc);
		rval |= (0x1 << 7);
		writel(rval, &mmcpriv->reg->ntdc);
*/
		tmp = (dsdly / 16 + 3) % 4;
		rval = readl(&mmcpriv->reg->ntsr);
		rval &= (~(0x3<<8));
		rval |= ((tmp&0x3)<<8) | 0x1;
		writel(rval, &mmcpriv->reg->ntsr);

		tmp = dsdly % 16;
		rval = readl(&mmcpriv->reg->ntdc);
		rval &= (~SDXC_CfgDly);
		rval |= ((tmp&SDXC_CfgNewDly) | SDXC_EnableDly);
		writel(rval, &mmcpriv->reg->ntdc);
	} else {
		rval = readl(&mmcpriv->reg->ntsr);
		rval &= (~(0x3<<8));
		rval |= ((sdly&0x3)<<8);
		writel(rval, &mmcpriv->reg->ntsr);
	}

	/*enable hw skew auto mode*/
	rval = readl(&mmcpriv->reg->skew_ctrl);
	/*rval &= ~(0xf);*/
	rval |= (0x1<<4);
	writel(rval, &mmcpriv->reg->skew_ctrl);

	MMCDBG("%s: spd_md:%d, freq:%d, odly: %d; sdly: %d; dsdly: %d\n",
			__FUNCTION__, spd_md, freq, odly, sdly, dsdly);
	MMCDBG("%s: skew_ctrl :%d\n", __FUNCTION__, readl(&mmcpriv->reg->skew_ctrl));

	return 0;
}

static int mmc_set_mod_clk(struct sunxi_mmc_priv *priv, unsigned int hz)
{
	unsigned int pll, pll_hz, div, n,  mod_hz, freq_id;
#ifdef FPGA_PLATFORM
	unsigned int rval;
#endif
	struct mmc *mmc = priv->mmc;
	u32 val = 0;
	mod_hz = 0;


	/*
	 * The MMC clock has an extra /2 post-divider when operating in the new
	 * mode.
	 */
	if (mmc->speed_mode == HSDDR52_DDR50 ||
			mmc->speed_mode == HS400)
		mod_hz = hz * 4;/* 4xclk: DDR 4/8(HS); HS400; */
	else
		mod_hz = hz * 2;/* 2xclk: SDR 1/4/8; */
	if (mod_hz <= 24000000) {
		pll = CCM_MMC_CTRL_OSCM24;
		pll_hz = 24000000;
	} else {
#ifdef CCM_MMC2_CTRL_PLL6X2
		pll = CCM_MMC2_CTRL_PLL6X2;
		pll_hz = clock_get_pll6() * 2 *1000000;
#elif CCM_MMC_CTRL_PLL6X2
		pll = CCM_MMC_CTRL_PLL6X2;
		pll_hz = clock_get_pll6() * 2 *1000000;
#else
		MMCINFO("There is no 2X pll!\n");
		return -1;
#endif
	}

	div = pll_hz / mod_hz;
	if (pll_hz % mod_hz)
		div++;

	n = 0;
	while (div > 16) {
		n++;
		div = (div + 1) / 2;
	}

	if (n > 3) {
		MMCINFO("mmc %u error cannot set clock to %u\n", priv->mmc_no,
		       hz);
		return -1;
	}
	freq_id = CLK_50M;
	/* determine delays */
	if (hz <= 400000)
		freq_id = CLK_400K;
	else if (hz <= 25000000)
		freq_id = CLK_25M;
	else if (hz <= 52000000)
		freq_id = CLK_50M;
	else if (hz <= 100000000)
		freq_id = CLK_100M;
	else if (hz <= 150000000)
		freq_id = CLK_150M;
	else if (hz <= 200000000)
		freq_id = CLK_200M;
	else
		/* hz > 52000000 */
		freq_id = CLK_50M;

	MMCDBG("freq_id:%d\n", freq_id);

#ifdef FPGA_PLATFORM
	if (readl(IOMEM_ADDR(SUNXI_MMMC_1X_2X_MODE_CTL_REG)) & (0x1 << 3)) {
		rval = readl(&priv->reg->ntsr);
		rval |= SUNXI_MMC_NTSR_MODE_SEL_NEW;
		writel(rval, &priv->reg->ntsr);
	} else {
		rval = readl(&priv->reg->ntsr);
		rval &= ~SUNXI_MMC_NTSR_MODE_SEL_NEW;
		writel(rval, &priv->reg->ntsr);
	}

	if (mod_hz > (400000 * 2)) {
		sunxi_r_op(priv, writel(CCM_MMC_CTRL_ENABLE,  priv->mclkreg));
	} else {
		sunxi_r_op(priv, writel(pll | CCM_MMC_CTRL_N(n) |
			CCM_MMC_CTRL_M(div), priv->mclkreg));
	}
	if (hz <= 400000) {
		sunxi_r_op(priv, writel(readl(&priv->reg->drv_dl) & ~(0x1 << 7), &priv->reg->drv_dl));
	} else {
		sunxi_r_op(priv, writel(readl(&priv->reg->drv_dl) | (0x1 << 7), &priv->reg->drv_dl));
	}

#else
	val = readl(&priv->reg->ntsr);
	val |= SUNXI_MMC_NTSR_MODE_SEL_NEW;
	writel(val, &priv->reg->ntsr);

	sunxi_r_op(priv, writel(pll | CCM_MMC_CTRL_N(n) |
	       CCM_MMC_CTRL_M(div), priv->mclkreg));
#endif
	val = readl(&priv->reg->clkcr);
	val &= ~0xff;
	if (mmc->speed_mode == HSDDR52_DDR50 ||
			mmc->speed_mode == HS400)
		val |= 0x1;
	writel(val, &priv->reg->clkcr);

	priv->tm5.cur_spd_md = mmc->speed_mode;
	priv->tm5.cur_freq = freq_id;

	mmc_config_delay(priv);

	debug("mclk reg***%x\n", readl(priv->mclkreg));
	debug("clkcr reg***%x\n", readl(&priv->reg->clkcr));
	debug("mmc %u set mod-clk req %u parent %u n %u m %u rate %u\n",
	      priv->mmc_no, mod_hz, pll_hz, 1u << n, div, pll_hz / (1u << n) / div);

	return 0;
}

static void mmc_ddr_mode_onoff(struct sunxi_mmc_priv *priv, int on)
{
	u32 rval = 0;

	rval = readl(&priv->reg->gctrl);
	rval &= (~(1U << 10));

	if (on) {
		rval |= (1U << 10);
		sunxi_r_op(priv, writel(rval, &priv->reg->gctrl));
		MMCDBG("set %d rgctrl 0x%x to enable ddr mode\n",
				priv->mmc_no, readl(&priv->reg->gctrl));
	} else {
		sunxi_r_op(priv, writel(rval, &priv->reg->gctrl));
		MMCDBG("set %d rgctrl 0x%x to disable ddr mode\n",
				priv->mmc_no, readl(&priv->reg->gctrl));
	}
}

static void mmc_hs400_mode_onoff(struct sunxi_mmc_priv *priv, int on)
{
	u32 rval = 0;

	rval = readl(&priv->reg->dsbd);
	rval &= (~(1U << 31));

	if (on) {
		rval |= (1U << 31);
		writel(rval, &priv->reg->dsbd);
		rval = readl(&priv->reg->csdc);
		rval &= ~0xF;
		rval |= 0x6;
		writel(rval, &priv->reg->csdc);
		MMCDBG("set %d dsbd 0x%x to enable hs400 mode\n",
				priv->mmc_no, readl(&priv->reg->dsbd));
	} else {
		writel(rval, &priv->reg->dsbd);
		rval = readl(&priv->reg->csdc);
		rval &= ~0xF;
		rval |= 0x3;
		writel(rval, &priv->reg->csdc);
		MMCDBG("set %d dsbd 0x%x to disable hs400 mode\n",
				priv->mmc_no, readl(&priv->reg->dsbd));
	}
}

static void sunxi_mmc_set_speed_mode(struct sunxi_mmc_priv *priv,
		struct mmc *mmc)
{
	/* set speed mode */
	if (mmc->speed_mode == HSDDR52_DDR50) {
		mmc_ddr_mode_onoff(priv, 1);
		mmc_hs400_mode_onoff(priv, 0);
	} else if (mmc->speed_mode == HS400) {
		mmc_ddr_mode_onoff(priv, 0);
		mmc_hs400_mode_onoff(priv, 1);
	} else {
		mmc_ddr_mode_onoff(priv, 0);
		mmc_hs400_mode_onoff(priv, 0);
	}
}

static void sunxi_mmc_core_init(struct mmc *mmc)
{
	struct sunxi_mmc_priv *priv = mmc->priv;

	/* Reset controller */
	writel(SUNXI_MMC_GCTRL_RESET, &priv->reg->gctrl);
	udelay(1000);
	/* release eMMC reset signal */
	writel(1, &priv->reg->hwrst);
	writel(0, &priv->reg->hwrst);
	udelay(1000);
	writel(1, &priv->reg->hwrst);
	udelay(1000);
	/* Set Data & Response Timeout Value */
	writel((SMC_DATA_TIMEOUT<<8)|SMC_RESP_TIMEOUT, &priv->reg->timeout);

	writel((512<<16)|(1U<<2)|(1U<<0), &priv->reg->thldc);
	writel(3, &priv->reg->csdc);
	writel(0xdeb, &priv->reg->dbgc);
}

void sunxi_mmc_host_tm5_init(int sdc_no)
{
	struct sunxi_mmc_priv *mmcpriv = &mmc_host[sdc_no];
	struct mmc_config *cfg = &mmcpriv->cfg;

	cfg->f_max = 200000000; /* 200M */

	mmcpriv->dma_tl = SUNXI_DMA_TL_TM5_V5P3X;
	mmcpriv->timing_mode = SUNXI_MMC_TIMING_MODE_5;
	mmcpriv->mmc_init_default_timing_para = mmc_init_default_timing_para;
	mmcpriv->mmc_set_mod_clk = mmc_set_mod_clk;
	mmcpriv->sunxi_mmc_set_speed_mode = sunxi_mmc_set_speed_mode;
	mmcpriv->sunxi_mmc_core_init = sunxi_mmc_core_init;
	mmcpriv->sunxi_mmc_clk_io_onoff = sunxi_mmc_clk_io_onoff;
}
