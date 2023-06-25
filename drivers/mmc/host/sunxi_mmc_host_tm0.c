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
#include "sunxi_mmc_host_tm0.h"

#define  SMC_DATA_TIMEOUT     0xffffffU
#define  SMC_RESP_TIMEOUT     0xff

#define SUNXI_DMA_TL_TM0_V4P00X        ((0x2<<28)|(7<<16)|8)

extern char *spd_name[];
extern struct sunxi_mmc_priv mmc_host[4];

static int mmc_init_default_timing_para(int sdc_no)
{
	struct sunxi_mmc_priv *mmcpriv = &mmc_host[sdc_no];

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

	return 0;
}

static void sunxi_mmc_clk_io_onoff(int sdc_no, int onoff, int reset_clk)
{
	int rval;
	struct sunxi_mmc_priv *priv = &mmc_host[sdc_no];

	/* config ahb clock */
	if (onoff) {
		rval = readl(priv->hclkrst);
		rval |= (1 << (8 + sdc_no));
		writel(rval, priv->hclkrst);
		rval = readl(priv->hclkbase);
		rval |= (1 << (8 + sdc_no));
		writel(rval, priv->hclkbase);

		rval = readl(priv->mclkreg);
		rval |= (1U << 31);
		writel(rval, priv->mclkreg);
	} else {
		rval = readl(priv->mclkreg);
		rval &= ~(1U << 31);
		writel(rval, priv->mclkreg);

		rval = readl(priv->hclkbase);
		rval &= ~(1 << (8 + sdc_no));
		writel(rval, priv->hclkbase);

		rval = readl(priv->hclkrst);
		rval &= ~(1 << (8 + sdc_no));
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
	unsigned int spd_md, freq;
	u8 odly, sdly;

	spd_md = mmcpriv->tm0.cur_spd_md;
	freq = mmcpriv->tm0.cur_freq;

	rval = readl(mmcpriv->mclkreg);

	/* disable clock */
	rval &= (~(1U << 31));
	writel(rval, mmcpriv->mclkreg);

	/* set input and output delay, enable clock */
	if (mmcpriv->tm0.odly[spd_md * MAX_CLK_FREQ_NUM + freq] != 0xFF)
		odly = mmcpriv->tm0.odly[spd_md * MAX_CLK_FREQ_NUM + freq];
	else
		odly = mmcpriv->tm0.def_odly[spd_md * MAX_CLK_FREQ_NUM + freq];
	if (mmcpriv->tm0.sdly[spd_md * MAX_CLK_FREQ_NUM + freq] != 0xFF)
		sdly = mmcpriv->tm0.sdly[spd_md * MAX_CLK_FREQ_NUM + freq];
	else
		sdly = mmcpriv->tm0.def_sdly[spd_md * MAX_CLK_FREQ_NUM + freq];
	MMCDBG("%s: odly: %d   sldy: %d\n", __FUNCTION__, odly, sdly);

	rval |= (1U << 31) | (sdly << 20) | (odly << 8);
	writel(rval, mmcpriv->mclkreg);

	return 0;
}

static int mmc_set_mod_clk(struct sunxi_mmc_priv *priv, unsigned int hz)
{
	unsigned int pll, pll_hz, div, n, mod_hz, freq_id;
	struct mmc *mmc = priv->mmc;
	u32 val = 0;
	mod_hz = 0;

	/*
	 * The MMC clock has an extra /2 post-divider when operating in the new
	 * mode.
	 */
	if (mmc->speed_mode == HSDDR52_DDR50)
		mod_hz = hz * 2;
	else
		mod_hz = hz;

	if (mod_hz <= 24000000) {
		pll = CCM_MMC_CTRL_OSCM24;
		pll_hz = 24000000;
	} else {
#ifdef CCM_MMC_CTRL_PLL6
		pll = CCM_MMC_CTRL_PLL6;
		pll_hz = clock_get_pll6() * 1000000;
#else
		MMCINFO("There is no 1X pll!\n");
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
	if (hz <= 400000) {
		freq_id = CLK_400K;
	} else if (hz <= 25000000) {
		freq_id = CLK_25M;
	} else if (hz <= 52000000) {
		freq_id = CLK_50M;
	} else if (hz <= 100000000)
		freq_id = CLK_100M;
	else if (hz <= 150000000)
		freq_id = CLK_150M;
	else if (hz <= 200000000)
		freq_id = CLK_200M;
	else {
		/* hz > 52000000 */
		freq_id = CLK_50M;
	}

	MMCDBG("freq_id:%d\n", freq_id);

	sunxi_r_op(priv, writel(pll | CCM_MMC_CTRL_N(n) | CCM_MMC_CTRL_M(div), priv->mclkreg));

	val = readl(&priv->reg->clkcr);
	val &= ~0xff;
	if (mmc->speed_mode == HSDDR52_DDR50)
		val |= 0x1;
	writel(val, &priv->reg->clkcr);

	priv->tm0.cur_spd_md = mmc->speed_mode;
	priv->tm0.cur_freq = freq_id;

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

static void sunxi_mmc_set_speed_mode(struct sunxi_mmc_priv *priv,
		struct mmc *mmc)
{
	/* set speed mode */
	if (mmc->speed_mode == HSDDR52_DDR50)
		mmc_ddr_mode_onoff(priv, 1);
	else
		mmc_ddr_mode_onoff(priv, 0);
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

	writel(0xdeb, &priv->reg->dbgc);
}

void sunxi_mmc_host_tm0_init(int sdc_no)
{
	struct sunxi_mmc_priv *mmcpriv = &mmc_host[sdc_no];
	struct mmc_config *cfg = &mmcpriv->cfg;

	cfg->f_max = 50000000; /* 50M */

	mmcpriv->dma_tl = SUNXI_DMA_TL_TM0_V4P00X;
	mmcpriv->timing_mode = SUNXI_MMC_TIMING_MODE_0;
	mmcpriv->mmc_init_default_timing_para = mmc_init_default_timing_para;
	mmcpriv->mmc_set_mod_clk = mmc_set_mod_clk;
	mmcpriv->sunxi_mmc_set_speed_mode = sunxi_mmc_set_speed_mode;
	mmcpriv->sunxi_mmc_core_init = sunxi_mmc_core_init;
	mmcpriv->sunxi_mmc_clk_io_onoff = sunxi_mmc_clk_io_onoff;
}
