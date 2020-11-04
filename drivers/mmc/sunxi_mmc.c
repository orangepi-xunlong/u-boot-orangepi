// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Aaron <leafy.myeh@allwinnertech.com>
 *
 * MMC driver for allwinner sunxi platform.
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <malloc.h>
#include <mmc.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/timer.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gpio.h>
#include "sunxi_mmc.h"
#include "sunxi_host_mmc.h"
#include "mmc_def.h"

#ifndef readl
#define  readl(a)   *(volatile uint *)(ulong)(a)
#endif
#ifndef writel
#define  writel(v, c) *(volatile uint *)(ulong)(c) = (v)
#endif

#if !CONFIG_IS_ENABLED(DM_MMC)
/* support 4 mmc hosts */
struct sunxi_mmc_priv mmc_host[4];
struct mmc_reg_v4p1 mmc_host_reg_bak[3];

//extern u8 ext_odly_spd_freq[];
//extern u8 ext_sdly_spd_freq[];
//extern u8 ext_odly_spd_freq_sdc0[];
//extern u8 ext_sdly_spd_freq_sdc0[];
static u8 ext_odly_spd_freq[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
static u8 ext_sdly_spd_freq[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
static u8 ext_odly_spd_freq_sdc0[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];
static u8 ext_sdly_spd_freq_sdc0[MAX_SPD_MD_NUM*MAX_CLK_FREQ_NUM];

void mmc_dump_errinfo(struct sunxi_mmc_priv *smc_priv, struct mmc_cmd *cmd)
{
	MMCMSG(smc_priv->mmc, "smc %d err, cmd %d, %s%s%s%s%s%s%s%s%s%s%s\n",
		smc_priv->mmc_no, cmd ? cmd->cmdidx : -1,
		smc_priv->raw_int_bak & SDXC_RespErr     ? " RE"     : "",
		smc_priv->raw_int_bak & SDXC_RespCRCErr  ? " RCE"    : "",
		smc_priv->raw_int_bak & SDXC_DataCRCErr  ? " DCE"    : "",
		smc_priv->raw_int_bak & SDXC_RespTimeout ? " RTO"    : "",
		smc_priv->raw_int_bak & SDXC_DataTimeout ? " DTO"    : "",
		smc_priv->raw_int_bak & SDXC_DataStarve  ? " DS"     : "",
		smc_priv->raw_int_bak & SDXC_FIFORunErr  ? " FE"     : "",
		smc_priv->raw_int_bak & SDXC_HardWLocked ? " HL"     : "",
		smc_priv->raw_int_bak & SDXC_StartBitErr ? " SBE"    : "",
		smc_priv->raw_int_bak & SDXC_EndBitErr   ? " EBE"    : "",
		smc_priv->raw_int_bak == 0 ? " STO"    : ""
		);
}

void dumphex32(char *name, char *base, int len)
{
	__u32 i;

	printf("dump %s registers:", name);
	for (i = 0; i < len; i += 4) {
		if (!(i & 0xf))
			printf("\n0x%p : ", base + i);
		printf("0x%08x ", readl((ulong)base + i));
	}
	printf("\n");
}

static int sunxi_mmc_getcd_gpio(int sdc_no)
{
/*
	switch (sdc_no) {
	case 0: return sunxi_name_to_gpio(CONFIG_MMC0_CD_PIN);
	case 1: return sunxi_name_to_gpio(CONFIG_MMC1_CD_PIN);
	case 2: return sunxi_name_to_gpio(CONFIG_MMC2_CD_PIN);
	case 3: return sunxi_name_to_gpio(CONFIG_MMC3_CD_PIN);
	}
*/
	return -EINVAL;
}

static int mmc_resource_init(int sdc_no)
{
	struct sunxi_mmc_priv *priv = &mmc_host[sdc_no];
	struct sunxi_ccm_reg *ccm = (struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	int cd_pin, ret = 0;

	pr_debug("init mmc %d resource\n", sdc_no);

	switch (sdc_no) {
	case 0:
		priv->reg = (struct mmc_reg_v4p1 *)SUNXI_MMC0_BASE;
		priv->mclkreg = &ccm->sd0_clk_cfg;
		break;
	case 1:
		priv->reg = (struct mmc_reg_v4p1 *)SUNXI_MMC1_BASE;
		priv->mclkreg = &ccm->sd1_clk_cfg;
		break;
	case 2:
		priv->reg = (struct mmc_reg_v4p1 *)SUNXI_MMC2_BASE;
		priv->mclkreg = &ccm->sd2_clk_cfg;
		break;
#ifdef SUNXI_MMC3_BASE
	case 3:
		priv->reg = (struct mmc_reg_v4p1 *)SUNXI_MMC3_BASE;
		priv->mclkreg = &ccm->sd3_clk_cfg;
		break;
#endif
	default:
		printf("Wrong mmc number %d\n", sdc_no);
		return -1;
	}
#if defined(CONFIG_MACH_SUN50IW10) || defined(CONFIG_MACH_SUN50IW9) \
	|| defined(CONFIG_MACH_SUN50IW3) || defined(CONFIG_MACH_SUN8IW15) \
	|| defined(CONFIG_MACH_SUN8IW16) || defined(CONFIG_MACH_SUN8IW19) \
	|| defined(CONFIG_MACH_SUN50IW11)
	priv->hclkbase = (u32)&ccm->sd_gate_reset;
	priv->hclkrst = (u32)&ccm->sd_gate_reset;
#else
	priv->hclkbase = (u32)&ccm->ahb_gate0;
	priv->hclkrst = (u32)&ccm->ahb_reset0_cfg;
#endif
	priv->mmc_no = sdc_no;

	cd_pin = sunxi_mmc_getcd_gpio(sdc_no);
	if (cd_pin >= 0) {
		ret = gpio_request(cd_pin, "mmc_cd");
		if (!ret) {
			sunxi_gpio_set_pull(cd_pin, SUNXI_GPIO_PULL_UP);
			ret = gpio_direction_input(cd_pin);
		}
	}

	return ret;
}
#endif

static int sunxi_mmc_pin_set(int sdc_no)
{
	struct sunxi_mmc_priv *priv = &mmc_host[sdc_no];
	struct sunxi_mmc_pininfo *pin_default = &priv->pin_default;
	struct sunxi_mmc_pininfo *pin_disable = &priv->pin_disable;
	int ret = -1;

	if (priv->pwr_handler != 0 && pin_disable->pin_count > 0) {
		ret =  gpio_request_early(pin_disable->pin_set, pin_disable->pin_count, 1);
		gpio_write_one_pin_value(priv->pwr_handler, 1, "card-pwr-gpios");
		mdelay(priv->time_pwroff);
		gpio_write_one_pin_value(priv->pwr_handler, 0, "card-pwr-gpios");
		/*delay to ensure voltage stability*/
		mdelay(1);
	}

	if (pin_default->pin_count > 0) {
		ret =  gpio_request_early(pin_default->pin_set, pin_default->pin_count, 1);
	}
	return ret;
}

void sunxi_mmc_pin_release(int sdc_no)
{
	struct sunxi_mmc_priv *priv = &mmc_host[sdc_no];

	if (priv->pwr_handler != 0) {
		gpio_release(priv->pwr_handler, 0);
	}
}

int mmc_clk_io_onoff(int sdc_no, int onoff, int reset_clk)
{
	int rval;
	struct sunxi_mmc_priv *priv = &mmc_host[sdc_no];

#if defined(CONFIG_MACH_SUN50IW10) || defined(CONFIG_MACH_SUN50IW9) \
	|| defined(CONFIG_MACH_SUN50IW3) || defined(CONFIG_MACH_SUN8IW15) \
	|| defined(CONFIG_MACH_SUN8IW16) || defined(CONFIG_MACH_SUN8IW19)
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
#else
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
#endif
	/* config mod clock */
	if (reset_clk) {
		rval = readl(priv->mclkreg);
		/*set to 24M default value*/
		rval &= ~(0x7fffffff);
		writel(rval, priv->mclkreg);
		priv->mod_clk = 24000000;
	}

//	dumphex32("ccmu", (char *)SUNXI_CCM_BASE, 0x100);
//	dumphex32("gpio", (char *)SUNXI_PIO_BASE, 0x100);
//	dumphex32("mmc", (char *)priv->reg, 0x100);

	return 0;
}

static int mmc_update_clk(struct sunxi_mmc_priv *priv)
{
	unsigned int cmd;
	unsigned timeout_msecs = 10000;
	unsigned long start = get_timer(0);

	writel(readl(&priv->reg->clkcr)|(0x1 << 31), &priv->reg->clkcr);
	cmd = SUNXI_MMC_CMD_START |
	      SUNXI_MMC_CMD_UPCLK_ONLY |
	      SUNXI_MMC_CMD_WAIT_PRE_OVER;

	writel(cmd, &priv->reg->cmd);
	while (readl(&priv->reg->cmd) & SUNXI_MMC_CMD_START) {
		if (get_timer(start) > timeout_msecs) {
				dumphex32("mmc", (char *)priv->reg, 0x200);
				return -1;
			}
	}
	writel(readl(&priv->reg->clkcr) & (~(0x1 << 31)), &priv->reg->clkcr);

	/* clock update sets various irq status bits, clear these */
	writel(readl(&priv->reg->rint), &priv->reg->rint);

	return 0;
}

static unsigned mmc_config_delay(struct sunxi_mmc_priv *mmcpriv)
{
	unsigned int rval = 0;
	unsigned int mode = mmcpriv->timing_mode;
	unsigned int spd_md, freq;
	u8 odly, sdly;
	__attribute__((unused)) u8 dsdly = 0;
#ifdef CONFIG_MACH_SUN8IW7
	if (mode == SUNXI_MMC_TIMING_MODE_1) {
		spd_md = mmcpriv->tm1.cur_spd_md;
		freq = mmcpriv->tm1.cur_freq;
		if (mmcpriv->tm1.odly[spd_md * MAX_CLK_FREQ_NUM + freq] != 0xFF)
			odly = mmcpriv->tm1.odly[spd_md * MAX_CLK_FREQ_NUM + freq];
		else
			odly = mmcpriv->tm1.def_odly[spd_md * MAX_CLK_FREQ_NUM + freq];
		if (mmcpriv->tm1.sdly[spd_md * MAX_CLK_FREQ_NUM + freq] != 0xFF)
			sdly = mmcpriv->tm1.sdly[spd_md * MAX_CLK_FREQ_NUM + freq];
		else
			sdly = mmcpriv->tm1.def_sdly[spd_md * MAX_CLK_FREQ_NUM + freq];
		mmcpriv->tm1.cur_odly = odly;
		mmcpriv->tm1.cur_sdly = sdly;

		pr_debug("%s: odly: %d   sldy: %d\n", __FUNCTION__, odly, sdly);

		rval = readl(&mmcpriv->reg->ntsr);
		rval &= (~((0x3 << 4) | (0x3 << 0)));
		rval |= (((odly & 0x3) << 0) | ((sdly & 0x3) << 4));
		writel(rval, &mmcpriv->reg->ntsr);
	} else if (mode == SUNXI_MMC_TIMING_MODE_0) {
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
		pr_debug("%s: odly: %d   sldy: %d\n", __FUNCTION__, odly, sdly);

		rval |= (1U << 31) | (sdly << 20) | (odly << 8);
		writel(rval, mmcpriv->mclkreg);

		/* update clock */
		if (mmc_update_clk(mmcpriv)) {
			pr_err("%s: mmc_update_clk fail!! \n", __FUNCTION__);
			return -1;
		}
	} else {
		pr_err("%s: wrong timing mode %d\n", __FUNCTION__, mode);
		return -1;
	}
#else
	if (mode == SUNXI_MMC_TIMING_MODE_1) {
		spd_md = mmcpriv->tm1.cur_spd_md;
		freq = mmcpriv->tm1.cur_freq;
		if (mmcpriv->tm1.odly[spd_md*MAX_CLK_FREQ_NUM+freq] != 0xFF)
			odly = mmcpriv->tm1.odly[spd_md*MAX_CLK_FREQ_NUM+freq];
		else
			odly = mmcpriv->tm1.def_odly[spd_md*MAX_CLK_FREQ_NUM+freq];
		if (mmcpriv->tm1.sdly[spd_md*MAX_CLK_FREQ_NUM+freq] != 0xFF)
			sdly = mmcpriv->tm1.sdly[spd_md*MAX_CLK_FREQ_NUM+freq];
		else
			sdly = mmcpriv->tm1.def_sdly[spd_md*MAX_CLK_FREQ_NUM+freq];
		mmcpriv->tm1.cur_odly = odly;
		mmcpriv->tm1.cur_sdly = sdly;

		pr_debug("%s: odly: %d   sldy: %d\n", __FUNCTION__, odly, sdly);
		rval = readl(&mmcpriv->reg->drv_dl);
		rval &= (~(0x3<<16));
		rval |= (((odly&0x1)<<16) | ((odly&0x1)<<17));
		writel(rval, &mmcpriv->reg->drv_dl);

		rval = readl(&mmcpriv->reg->ntsr);
		rval &= (~(0x3<<4));
		rval |= ((sdly&0x3)<<4);
		writel(rval, &mmcpriv->reg->ntsr);
	} else if (mode == SUNXI_MMC_TIMING_MODE_3) {
		spd_md = mmcpriv->tm3.cur_spd_md;
		freq = mmcpriv->tm3.cur_freq;
		if (mmcpriv->tm3.odly[spd_md*MAX_CLK_FREQ_NUM+freq] != 0xFF)
			odly = mmcpriv->tm3.odly[spd_md*MAX_CLK_FREQ_NUM+freq];
		else
			odly = mmcpriv->tm3.def_odly[spd_md*MAX_CLK_FREQ_NUM+freq];
		if (mmcpriv->tm3.sdly[spd_md*MAX_CLK_FREQ_NUM+freq] != 0xFF)
			sdly = mmcpriv->tm3.sdly[spd_md*MAX_CLK_FREQ_NUM+freq];
		else
			sdly = mmcpriv->tm3.def_sdly[spd_md*MAX_CLK_FREQ_NUM+freq];
		mmcpriv->tm3.cur_odly = odly;
		mmcpriv->tm3.cur_sdly = sdly;

		pr_debug("%s: odly: %d   sldy: %d\n", __FUNCTION__, odly, sdly);
		rval = readl(&mmcpriv->reg->drv_dl);
		rval &= (~(0x3<<16));
		rval |= (((odly&0x1)<<16) | ((odly&0x1)<<17));
		writel(rval, &mmcpriv->reg->drv_dl);

		rval = readl(&mmcpriv->reg->samp_dl);
		rval &= (~SDXC_CfgDly);
		rval |= ((sdly&SDXC_CfgDly) | SDXC_EnableDly);
		writel(rval, &mmcpriv->reg->samp_dl);
	} else if (mode == SUNXI_MMC_TIMING_MODE_4) {
		spd_md = mmcpriv->tm4.cur_spd_md;
		freq = mmcpriv->tm4.cur_freq;

		if (mmcpriv->tm4.sdly[spd_md*MAX_CLK_FREQ_NUM+freq] != 0xFF)
			sdly = mmcpriv->tm4.sdly[spd_md*MAX_CLK_FREQ_NUM+freq];
		else
			sdly = mmcpriv->tm4.def_sdly[spd_md*MAX_CLK_FREQ_NUM+freq];

		if (mmcpriv->tm4.odly[spd_md*MAX_CLK_FREQ_NUM+freq] != 0xFF)
			odly = mmcpriv->tm4.odly[spd_md*MAX_CLK_FREQ_NUM+freq];
		else
			odly = mmcpriv->tm4.def_odly[spd_md*MAX_CLK_FREQ_NUM+freq];

		mmcpriv->tm4.cur_odly = odly;
		mmcpriv->tm4.cur_sdly = sdly;

		rval = readl(&mmcpriv->reg->drv_dl);
		rval &= (~(0x3<<16));
		rval |= (((odly&0x1)<<16) | ((odly&0x1)<<17));
		writel(rval, &mmcpriv->reg->drv_dl);

		rval = readl(&mmcpriv->reg->samp_dl);
		rval &= (~SDXC_CfgDly);
		rval |= ((sdly&SDXC_CfgDly) | SDXC_EnableDly);
		writel(rval, &mmcpriv->reg->samp_dl);

		if (spd_md == HS400) {
			if (mmcpriv->tm4.dsdly[freq] != 0xFF)
				dsdly = mmcpriv->tm4.dsdly[freq];
			else
				dsdly = mmcpriv->tm4.def_dsdly[freq];
			mmcpriv->tm4.cur_dsdly = dsdly;

			rval = readl(&mmcpriv->reg->ds_dl);
			rval &= (~SDXC_CfgDly);
			rval |= ((dsdly&SDXC_CfgDly) | SDXC_EnableDly);
			#ifdef FPGA_PLATFORM
			rval &= (~0x7);
			#endif
			writel(rval, &mmcpriv->reg->ds_dl);
		}
#if defined(CONFIG_MACH_SUN8IW15) || defined(CONFIG_MACH_SUN8IW16)
		rval = readl(&mmcpriv->reg->sfc);
		rval |= 0x1;
		writel(rval, &mmcpriv->reg->sfc);
		pr_debug("sfc 0x%x\n", readl(&mmcpriv->reg->sfc));
#endif
		pr_debug("%s: spd_md:%d, freq:%d, odly: %d; sdly: %d; dsdly: %d\n", __FUNCTION__, spd_md, freq, odly, sdly, dsdly);
	}
#endif
	return 0;
}

static int mmc_set_mod_clk(struct sunxi_mmc_priv *priv, unsigned int hz)
{
	unsigned int pll, pll_hz, div, n, oclk_dly, sclk_dly, mod_hz, freq_id;
#if defined(FPGA_PLATFORM) || defined(CONFIG_MACH_SUN50IW10)
	unsigned int rval;
#endif
	unsigned mode = priv->timing_mode;
	struct mmc *mmc = priv->mmc;
	bool new_mode = false;
	u32 val = 0;
	mod_hz = 0;

#if defined(CONFIG_MACH_SUN8IW16) || defined(CONFIG_MACH_SUN50IW9)\
	|| defined(CONFIG_MACH_SUN8IW19) || defined(CONFIG_MACH_SUN50IW10)\
	|| defined(CONFIG_MACH_SUN8IW15) || defined(CONFIG_MACH_SUN50IW11)
	if (IS_ENABLED(CONFIG_MMC_SUNXI_HAS_NEW_MODE) && (priv->mmc_no != 2))
#endif
		new_mode = true;

	/*
	 * The MMC clock has an extra /2 post-divider when operating in the new
	 * mode.
	 */
#ifdef CONFIG_MACH_SUN8IW7
	if (new_mode) {
		if (mmc->speed_mode == HSDDR52_DDR50)
			mod_hz = hz * 4;
		else
			mod_hz = hz * 2;
	} else
		mod_hz = hz;
#else
	if (new_mode && mode == SUNXI_MMC_TIMING_MODE_1) {
		if (mmc->speed_mode == HSDDR52_DDR50)
			mod_hz = hz * 4;
		else
			mod_hz = hz * 2;
	} else if (mode == SUNXI_MMC_TIMING_MODE_4) {
		if ((mmc->speed_mode == HSDDR52_DDR50)
				&& (mmc->bus_width == 8))
			mod_hz = hz * 4;/* 4xclk: DDR8(HS) */
		else
			mod_hz = hz * 2;/* 2xclk: SDR 1/4/8; DDR4(HS); DDR8(HS400) */
	}
#endif
	if (mod_hz <= 24000000) {
		pll = CCM_MMC_CTRL_OSCM24;
		pll_hz = 24000000;
	} else {
#ifdef CONFIG_MACH_SUN9I
		pll = CCM_MMC_CTRL_PLL_PERIPH0;
		pll_hz = clock_get_pll4_periph0();
#elif (defined(CONFIG_MACH_SUN50I_H6) || defined(CONFIG_MACH_SUN8IW16)\
	|| defined(CONFIG_MACH_SUN50IW9) || defined(CONFIG_MACH_SUN8IW19)\
	|| defined(CONFIG_MACH_SUN50IW10) || defined(CONFIG_MACH_SUN8IW15)\
	|| defined(CONFIG_MACH_SUN50IW11))
		pll = CCM_MMC_CTRL_PLL6X2;
		pll_hz = clock_get_pll6() * 2 *1000000;
#else
		pll = CCM_MMC_CTRL_PLL6;
		pll_hz = clock_get_pll6() * 1000000;
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
		pr_err("mmc %u error cannot set clock to %u\n", priv->mmc_no,
		       hz);
		return -1;
	}
	freq_id = CLK_50M;
	/* determine delays */
	if (hz <= 400000) {
		oclk_dly = 0;
		sclk_dly = 0;
		freq_id = CLK_400K;
	} else if (hz <= 25000000) {
		oclk_dly = 0;
		sclk_dly = 5;
		freq_id = CLK_25M;
#ifdef CONFIG_MACH_SUN9I
	} else if (hz <= 52000000) {
		oclk_dly = 5;
		sclk_dly = 4;
		freq_id = CLK_50M;
	} else {
		/* hz > 52000000 */
		oclk_dly = 2;
		sclk_dly = 4;
#else
	} else if (hz <= 52000000) {
		oclk_dly = 3;
		sclk_dly = 4;
		freq_id = CLK_50M;
	} else if (hz <= 100000000)
		freq_id = CLK_100M;
	else if (hz <= 150000000)
		freq_id = CLK_150M;
	else if (hz <= 200000000)
		freq_id = CLK_200M;
	else {
		/* hz > 52000000 */
		oclk_dly = 1;
		sclk_dly = 4;
		freq_id = CLK_50M;
#endif
	}

	pr_debug("freq_id:%d\n", freq_id);
	if (new_mode) {
#ifdef CONFIG_MMC_SUNXI_HAS_NEW_MODE
#if (!defined(CONFIG_MACH_SUN8IW16)) && (!defined(CONFIG_MACH_SUN50IW9))\
	&& (!defined(CONFIG_MACH_SUN8IW19)) && (!defined(CONFIG_MACH_SUN50IW10))\
	&& (!defined(CONFIG_MACH_SUN8IW15)) && (!defined(CONFIG_MACH_SUN50IW11))
		val = 0x1 << 30;//CCM_MMC_CTRL_MODE_SEL_NEW;
#endif
#ifdef FPGA_PLATFORM
		if(readl(SUNXI_MMMC_1X_2X_MODE_CTL_REG) & (0x1 << 3)) {
			rval = readl(&priv->reg->ntsr);
			rval |= SUNXI_MMC_NTSR_MODE_SEL_NEW;
			writel(rval, &priv->reg->ntsr);
		}else {
			rval = readl(&priv->reg->ntsr);
			rval &= ~SUNXI_MMC_NTSR_MODE_SEL_NEW;
			writel(rval, &priv->reg->ntsr);
		}
#else
		setbits_le32(&priv->reg->ntsr, SUNXI_MMC_NTSR_MODE_SEL_NEW);
#endif
#endif
	} else {
/*only user for some host,which default use 2x mode on the sdc2*/
#ifdef CONFIG_MACH_SUN50IW10
		rval = readl(&priv->reg->ntsr);
		rval &= ~SUNXI_MMC_NTSR_MODE_SEL_NEW;
		writel(rval, &priv->reg->ntsr);
#endif
		val = CCM_MMC_CTRL_OCLK_DLY(oclk_dly) |
			CCM_MMC_CTRL_SCLK_DLY(sclk_dly);
	}

#ifdef FPGA_PLATFORM
	if (mod_hz > (400000 * 2)) {
		writel(CCM_MMC_CTRL_ENABLE,  priv->mclkreg);
	} else {
		writel(CCM_MMC_CTRL_ENABLE | pll | CCM_MMC_CTRL_N(n) |
			CCM_MMC_CTRL_M(div) | val, priv->mclkreg);
	}
	if (hz <= 400000) {
		writel(readl(&priv->reg->drv_dl) & ~(0x1 << 7), &priv->reg->drv_dl);
	} else {
		writel(readl(&priv->reg->drv_dl) | (0x1 << 7), &priv->reg->drv_dl);
	}

#else
	writel(CCM_MMC_CTRL_ENABLE| pll | CCM_MMC_CTRL_N(n) |
	       CCM_MMC_CTRL_M(div) | val, priv->mclkreg);
#endif
	val = readl(&priv->reg->clkcr);
	val &= ~0xff;
	if (mmc->speed_mode == HSDDR52_DDR50)
		val |= 0x1;
	writel(val, &priv->reg->clkcr);
	if (mode == SUNXI_MMC_TIMING_MODE_1) {
		priv->tm1.cur_spd_md = mmc->speed_mode;
		priv->tm1.cur_freq = freq_id;
	} else if (mode == SUNXI_MMC_TIMING_MODE_3) {
		priv->tm3.cur_spd_md = mmc->speed_mode;
		priv->tm3.cur_freq = freq_id;
	} else if (mode == SUNXI_MMC_TIMING_MODE_4) {
		priv->tm4.cur_spd_md = mmc->speed_mode;
		priv->tm4.cur_freq = freq_id;
	}
	mmc_config_delay(priv);
	debug("mclk reg***%x\n", readl(priv->mclkreg));
	debug("clkcr reg***%x\n", readl(&priv->reg->clkcr));
	debug("mmc %u set mod-clk req %u parent %u n %u m %u rate %u\n",
	      priv->mmc_no, mod_hz, pll_hz, 1u << n, div, pll_hz / (1u << n) / div);

	return 0;
}

static int mmc_config_clock(struct sunxi_mmc_priv *priv, struct mmc *mmc)
{
	unsigned rval = readl(&priv->reg->clkcr);

	/* Disable Clock */
	rval &= ~SUNXI_MMC_CLK_ENABLE;
	writel(rval, &priv->reg->clkcr);
	if (mmc_update_clk(priv)) {
		pr_err("Disable clock: mmc update clk failed\n");
		return -1;
	}

	/* Set mod_clk to new rate */
	if (mmc_set_mod_clk(priv, mmc->clock))
		return -1;
#if 0
	/* Clear internal divider */
	rval &= ~SUNXI_MMC_CLK_DIVIDER_MASK;
	writel(rval, &priv->reg->clkcr);
#endif
	/* Re-enable Clock */
	rval = readl(&priv->reg->clkcr);
	rval |= SUNXI_MMC_CLK_ENABLE;
	writel(rval, &priv->reg->clkcr);
	if (mmc_update_clk(priv)) {
		pr_err("Re-enable clock: mmc update clk failed\n");
		return -1;
	}

	return 0;
}

static void mmc_ddr_mode_onoff(struct sunxi_mmc_priv *priv, int on)
{
	u32 rval = 0;

	rval = readl(&priv->reg->gctrl);
	rval &= (~(1U << 10));

	if (on) {
		rval |= (1U << 10);
		writel(rval, &priv->reg->gctrl);
		pr_debug("set %d rgctrl 0x%x to enable ddr mode\n", priv->mmc_no, readl(&priv->reg->gctrl));
	} else {
		writel(rval, &priv->reg->gctrl);
		pr_debug("set %d rgctrl 0x%x to disable ddr mode\n", priv->mmc_no, readl(&priv->reg->gctrl));
	}
}

static void mmc_hs400_mode_onoff(struct sunxi_mmc_priv *priv, int on)
{
	struct mmc_config *cfg = &priv->cfg;
	u32 rval = 0;

	if (cfg->host_no == 2) {
		rval = readl(&priv->reg->dsbd);
		rval &= (~(1U << 31));

		if (on) {
			rval |= (1U << 31);
			writel(rval, &priv->reg->dsbd);
			rval = readl(&priv->reg->csdc);
			rval &= ~0xF;
			rval |= 0x6;
			writel(rval, &priv->reg->csdc);
			pr_debug("set %d dsbd 0x%x to enable hs400 mode\n", priv->mmc_no, readl(&priv->reg->dsbd));
		} else {
			writel(rval, &priv->reg->dsbd);
			rval = readl(&priv->reg->csdc);
			rval &= ~0xF;
			rval |= 0x3;
			writel(rval, &priv->reg->csdc);
			pr_debug("set %d dsbd 0x%x to disable hs400 mode\n", priv->mmc_no, readl(&priv->reg->dsbd));
		}
	}
}

static int sunxi_mmc_set_ios_common(struct sunxi_mmc_priv *priv,
				    struct mmc *mmc)
{
	debug("set ios: bus_width: %x, clock: %d\n",
	      mmc->bus_width, mmc->clock);

	/* Change clock first */
	if (mmc->clock && mmc_config_clock(priv, mmc) != 0) {
		priv->fatal_err = 1;
		return -EINVAL;
	}

	/* Change bus width */
	if (mmc->bus_width == 8)
		writel(0x2, &priv->reg->width);
	else if (mmc->bus_width == 4)
		writel(0x1, &priv->reg->width);
	else
		writel(0x0, &priv->reg->width);

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

	return 0;
}

#if !CONFIG_IS_ENABLED(DM_MMC)
static int sunxi_mmc_core_init(struct mmc *mmc)
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
#if 1
#define  SMC_DATA_TIMEOUT     0xffffffU
#define  SMC_RESP_TIMEOUT     0xff
#else
#define  SMC_DATA_TIMEOUT     0x1ffffU
#define  SMC_RESP_TIMEOUT     0x2
#endif
	writel((SMC_DATA_TIMEOUT<<8)|SMC_RESP_TIMEOUT, &priv->reg->timeout); //Set Data & Response Timeout Value

	writel((512<<16)|(1U<<2)|(1U<<0), &priv->reg->thldc);
	writel(3, &priv->reg->csdc);
	writel(0xdeb, &priv->reg->dbgc);

//	if (priv->cfg.cal_delay_unit)
//		mmc_calibrate_delay_unit(priv);
	return 0;
}
#endif

#if (!defined(CONFIG_MACH_SUN8IW7))
static int mmc_save_regs(struct sunxi_mmc_priv *mmchost)
{
	struct mmc_reg_v4p1 *reg = (struct mmc_reg_v4p1 *)mmchost->reg;
	struct mmc_reg_v4p1 *reg_bak = (struct mmc_reg_v4p1 *)mmchost->reg_bak;

	reg_bak->gctrl     = readl(&reg->gctrl);
	reg_bak->clkcr     = readl(&reg->clkcr);
	reg_bak->timeout   = readl(&reg->timeout);
	reg_bak->width     = readl(&reg->width);
	reg_bak->imask     = readl(&reg->imask);
	reg_bak->ftrglevel = readl(&reg->ftrglevel);
	reg_bak->dbgc      = readl(&reg->dbgc);
	reg_bak->csdc      = readl(&reg->csdc);
	reg_bak->ntsr      = readl(&reg->ntsr);
	reg_bak->hwrst     = readl(&reg->hwrst);
	reg_bak->dmac      = readl(&reg->dmac);
	reg_bak->idie      = readl(&reg->idie);
	reg_bak->thldc     = readl(&reg->thldc);
	reg_bak->dsbd      = readl(&reg->dsbd);
	reg_bak->drv_dl    = readl(&reg->drv_dl);
	reg_bak->samp_dl   = readl(&reg->samp_dl);
	reg_bak->ds_dl     = readl(&reg->ds_dl);

	return 0;
}

static int mmc_restore_regs(struct sunxi_mmc_priv *mmchost)
{	struct mmc_reg_v4p1 *reg = (struct mmc_reg_v4p1 *)mmchost->reg;
	struct mmc_reg_v4p1 *reg_bak = (struct mmc_reg_v4p1 *)mmchost->reg_bak;

	writel(reg_bak->gctrl, &reg->gctrl);
	writel(reg_bak->clkcr, &reg->clkcr);
	writel(reg_bak->timeout, &reg->timeout);
	writel(reg_bak->width, &reg->width);
	writel(reg_bak->imask, &reg->imask);
	writel(reg_bak->ftrglevel, &reg->ftrglevel);
	if (reg_bak->dbgc)
		writel(0xdeb, &reg->dbgc);
	writel(reg_bak->csdc, &reg->csdc);
	writel(reg_bak->ntsr, &reg->ntsr);
	writel(reg_bak->hwrst, &reg->hwrst);
	writel(reg_bak->dmac, &reg->dmac);
	writel(reg_bak->idie, &reg->idie);
	writel(reg_bak->thldc, &reg->thldc);
	writel(reg_bak->dsbd, &reg->dsbd);
	writel(reg_bak->drv_dl, &reg->drv_dl);
	writel(reg_bak->samp_dl, &reg->samp_dl);
	writel(reg_bak->ds_dl, &reg->ds_dl);

	return 0;
}
#endif

static int mmc_trans_data_by_cpu(struct sunxi_mmc_priv *priv, struct mmc *mmc,
				 struct mmc_data *data)
{
	const int reading = !!(data->flags & MMC_DATA_READ);
	const uint32_t status_bit = reading ? SUNXI_MMC_STATUS_FIFO_EMPTY :
					      SUNXI_MMC_STATUS_FIFO_FULL;
	unsigned i;
	unsigned *buff = (unsigned int *)(reading ? data->dest : data->src);
	unsigned byte_cnt = data->blocksize * data->blocks;
	unsigned timeout_msecs = byte_cnt;
	unsigned long  start;

	if (timeout_msecs < 2000)
		timeout_msecs = 2000;

	/* Always read / write data through the CPU */
	setbits_le32(&priv->reg->gctrl, SUNXI_MMC_GCTRL_ACCESS_BY_AHB);

	start = get_timer(0);

	for (i = 0; i < (byte_cnt >> 2); i++) {
		while (readl(&priv->reg->status) & status_bit) {
			if (get_timer(start) > timeout_msecs)
				return -1;
		}

		if (reading)
			buff[i] = readl(&priv->reg->fifo);
		else
			writel(buff[i], &priv->reg->fifo);
	}

	return 0;
}

static int mmc_trans_data_by_dma(struct sunxi_mmc_priv *priv, struct mmc *mmc, struct mmc_data *data)
{
	struct mmc_des_v4p1 *pdes = priv->pdes;
	unsigned byte_cnt = data->blocksize * data->blocks;
	unsigned char *buff;
	unsigned des_idx = 0;
	unsigned buff_frag_num = 0;
	unsigned remain;
	unsigned i, rval;

	buff = data->flags & MMC_DATA_READ ?
			(unsigned char *)data->dest : (unsigned char *)data->src;
	buff_frag_num = byte_cnt >> SDXC_DES_NUM_SHIFT;
	remain = byte_cnt & (SDXC_DES_BUFFER_MAX_LEN - 1);
	if (remain)
		buff_frag_num++;
	else
		remain = SDXC_DES_BUFFER_MAX_LEN;
	flush_cache((unsigned long)buff, ALIGN((unsigned long)byte_cnt, CONFIG_SYS_CACHELINE_SIZE));
	for (i = 0; i < buff_frag_num; i++, des_idx++) {
		memset((void *)&pdes[des_idx], 0, sizeof(struct mmc_des_v4p1));
		pdes[des_idx].des_chain = 1;
		pdes[des_idx].own = 1;
		pdes[des_idx].dic = 1;
		if (buff_frag_num > 1 && i != buff_frag_num - 1)
			pdes[des_idx].data_buf1_sz = SDXC_DES_BUFFER_MAX_LEN;
		else
			pdes[des_idx].data_buf1_sz = remain;
#if defined(CONFIG_MACH_SUN50IW9) || defined(CONFIG_MACH_SUN50IW10) || defined(CONFIG_MACH_SUN50IW11)
		pdes[des_idx].buf_addr_ptr1 = ((ulong)buff + i * SDXC_DES_BUFFER_MAX_LEN) >> 2;
#else
		pdes[des_idx].buf_addr_ptr1 = ((ulong)buff + i * SDXC_DES_BUFFER_MAX_LEN);
#endif
		if (i == 0)
			pdes[des_idx].first_des = 1;

		if (i == buff_frag_num - 1) {
			pdes[des_idx].dic = 0;
			pdes[des_idx].last_des = 1;
			pdes[des_idx].end_of_ring = 1;
			pdes[des_idx].buf_addr_ptr2 = 0;
		} else {
#if defined(CONFIG_MACH_SUN50IW9) || defined(CONFIG_MACH_SUN50IW10) || defined(CONFIG_MACH_SUN50IW11)
			pdes[des_idx].buf_addr_ptr2 = ((ulong)&pdes[des_idx + 1]) >> 2;
#else
			pdes[des_idx].buf_addr_ptr2 = ((ulong)&pdes[des_idx + 1]);
#endif
		}
		debug("frag %d, remain %d, des[%d](%08x): "
			"[0] = %08x, [1] = %08x, [2] = %08x, [3] = %08x\n",
			i, remain, des_idx, (u32)&pdes[des_idx],
			(u32)((u32 *)&pdes[des_idx])[0], (u32)((u32 *)&pdes[des_idx])[1],
			(u32)((u32 *)&pdes[des_idx])[2], (u32)((u32 *)&pdes[des_idx])[3]);
	}
	flush_cache((unsigned long)pdes, ALIGN(sizeof(struct mmc_des_v4p1) * (des_idx + 1), CONFIG_SYS_CACHELINE_SIZE));
	__asm("DSB");
	__asm("ISB");

	/*
	 * GCTRLREG
	 * GCTRL[2]	: DMA reset
	 * GCTRL[5]	: DMA enable
	 *
	 * IDMACREG
	 * IDMAC[0]	: IDMA soft reset
	 * IDMAC[1]	: IDMA fix burst flag
	 * IDMAC[7]	: IDMA on
	 *
	 * IDIECREG
	 * IDIE[0]	: IDMA transmit interrupt flag
	 * IDIE[1]	: IDMA receive interrupt flag
	 */
	rval = readl(&priv->reg->gctrl);
	writel(rval | (1 << 5) | (1 << 2), &priv->reg->gctrl);	/* dma enable */
	writel((1 << 0), &priv->reg->dmac); /* idma reset */
	while (readl(&priv->reg->dmac) & 0x1) {
	} /* wait idma reset done */

	writel((1 << 1) | (1 << 7), &priv->reg->dmac); /* idma on */
	rval = readl(&priv->reg->idie) & (~3);
	if (data->flags & MMC_DATA_WRITE)
		rval |= (1 << 0);
	else
		rval |= (1 << 1);
	writel(rval, &priv->reg->idie);
#if defined(CONFIG_MACH_SUN50IW9) || defined(CONFIG_MACH_SUN50IW10) || defined(CONFIG_MACH_SUN50IW11)
	writel(((unsigned long)pdes) >> 2, &priv->reg->dlba);
#else
	writel(((unsigned long)pdes), &priv->reg->dlba);
#endif
#if defined(CONFIG_MACH_SUN8IW7)
	writel((2U<<28)|(7<<16)|8, &priv->reg->ftrglevel);
#else
	if (priv->cfg.host_no == 2) {
		writel((0x3<<28)|(15<<16)|240, &priv->reg->ftrglevel);
	} else {
		writel((0x2<<28)|(7<<16)|248, &priv->reg->ftrglevel);
	}

#endif
	return 0;
}

static int mmc_rint_wait(struct sunxi_mmc_priv *priv, struct mmc *mmc,
			 uint timeout_msecs, uint done_bit, const char *what, uint usedma)
{
	unsigned int status;
	unsigned int done = 0;
	unsigned long start = get_timer(0);
	do {
		status = readl(&priv->reg->rint);
		if ((get_timer(start) > timeout_msecs) || (status & SUNXI_MMC_RINT_INTERRUPT_ERROR_BIT)) {
			MMCMSG(mmc, "%s timeout %x status %x\n", what,
					status & SUNXI_MMC_RINT_INTERRUPT_ERROR_BIT, status);
			return -ETIMEDOUT;
		}
		if (usedma && !strncmp(what, "data", sizeof("data")))
			done = ((status & done_bit) && (readl(&priv->reg->idst) & 0x3)) ? 1 : 0;
		else
			done = (status & done_bit);
	} while (!done);

	return 0;
}

static int sunxi_mmc_send_cmd_common(struct sunxi_mmc_priv *priv,
				     struct mmc *mmc, struct mmc_cmd *cmd,
				     struct mmc_data *data)
{
	unsigned int cmdval = SUNXI_MMC_CMD_START;
	unsigned int timeout_msecs;
	int error = 0;
	unsigned int status = 0;
	unsigned int usedma = 0;
	unsigned int bytecnt = 0;

	if (priv->fatal_err)
		return -1;
	if (cmd->resp_type & MMC_RSP_BUSY)
		debug("mmc cmd %d check rsp busy\n", cmd->cmdidx);
	if (cmd->cmdidx == 12 && mmc->manual_stop_flag == 0)
		return 0;

	if (!cmd->cmdidx)
		cmdval |= SUNXI_MMC_CMD_SEND_INIT_SEQ;
	if (cmd->resp_type & MMC_RSP_PRESENT)
		cmdval |= SUNXI_MMC_CMD_RESP_EXPIRE;
	if (cmd->resp_type & MMC_RSP_136)
		cmdval |= SUNXI_MMC_CMD_LONG_RESPONSE;
	if (cmd->resp_type & MMC_RSP_CRC)
		cmdval |= SUNXI_MMC_CMD_CHK_RESPONSE_CRC;

	if (data) {
		if ((u32)(long)data->dest & 0x3) {
			error = -1;
			pr_err("%s,%d,dest is not 4 aligned\n", __FUNCTION__, __LINE__);
			goto out;
		}

		cmdval |= SUNXI_MMC_CMD_DATA_EXPIRE|SUNXI_MMC_CMD_WAIT_PRE_OVER;
		if (data->flags & MMC_DATA_WRITE)
			cmdval |= SUNXI_MMC_CMD_WRITE;
		if (data->blocks > 1)
			cmdval |= SUNXI_MMC_CMD_AUTO_STOP;
		writel(data->blocksize, &priv->reg->blksz);
		writel(data->blocks * data->blocksize, &priv->reg->bytecnt);
	} else {
		if (cmd->cmdidx == 12 && mmc->manual_stop_flag == 1) {
			cmdval |= SUNXI_MMC_CMD_STOP_ABORT;
			cmdval &= ~SUNXI_MMC_CMD_WAIT_PRE_OVER;
		}
	}

	debug("mmc %d, cmd %d(0x%08x), arg 0x%08x\n", priv->mmc_no,
	      cmd->cmdidx, cmdval | cmd->cmdidx, cmd->cmdarg);
	writel(cmd->cmdarg, &priv->reg->arg);

	if (!data)
		writel(cmdval | cmd->cmdidx, &priv->reg->cmd);
	/*
	 * transfer data and check status
	 * STATREG[2] : FIFO empty
	 * STATREG[3] : FIFO full
	 */
	if (data) {
		int ret = 0;

		bytecnt = data->blocksize * data->blocks;
		debug("trans data %d bytes\n", bytecnt);
#ifdef CONFIG_MMC_SUNXI_USE_DMA
		if (bytecnt > 64) {
#else
		if (0) {
#endif
			usedma = 1;
			writel(readl(&priv->reg->gctrl) & (~SUNXI_MMC_GCTRL_ACCESS_BY_AHB), &priv->reg->gctrl);
			ret = mmc_trans_data_by_dma(priv, mmc, data);
			writel(cmdval | cmd->cmdidx, &priv->reg->cmd);
		} else {
			writel(readl(&priv->reg->gctrl) | SUNXI_MMC_GCTRL_ACCESS_BY_AHB, &priv->reg->gctrl);
			writel(cmdval | cmd->cmdidx, &priv->reg->cmd);
			ret = mmc_trans_data_by_cpu(priv, mmc, data);
		}
		if (ret) {
			error = readl(&priv->reg->rint) &
				SUNXI_MMC_RINT_INTERRUPT_ERROR_BIT;
			error = -ETIMEDOUT;
			goto out;
		}
	}

	error = mmc_rint_wait(priv, mmc, 1000, SUNXI_MMC_RINT_COMMAND_DONE,
			      "cmd", usedma);
	if (error) {
		goto out;
	}

	if (data) {
		timeout_msecs = 6000;
		debug("cacl timeout %x msec\n", timeout_msecs);
		error = mmc_rint_wait(priv, mmc, timeout_msecs,
				      data->blocks > 1 ?
				      SUNXI_MMC_RINT_AUTO_COMMAND_DONE :
				      SUNXI_MMC_RINT_DATA_OVER,
				      "data", usedma);
		if (error) {
			goto out;
		}
	}

	if (cmd->resp_type & MMC_RSP_BUSY) {
		unsigned long start = get_timer(0);
		if (cmd->cmdidx == MMC_CMD_ERASE)
			timeout_msecs = 0x1fffffff;
		else
			timeout_msecs = 2000;

		do {
			status = readl(&priv->reg->status);
			if (get_timer(start) > timeout_msecs) {
				debug("busy timeout\n");
				error = -ETIMEDOUT;
				goto out;
			}
		} while (status & SUNXI_MMC_STATUS_CARD_DATA_BUSY);
	}

	if (cmd->resp_type & MMC_RSP_136) {
		cmd->response[0] = readl(&priv->reg->resp3);
		cmd->response[1] = readl(&priv->reg->resp2);
		cmd->response[2] = readl(&priv->reg->resp1);
		cmd->response[3] = readl(&priv->reg->resp0);
		debug("mmc resp 0x%08x 0x%08x 0x%08x 0x%08x\n",
		      cmd->response[3], cmd->response[2],
		      cmd->response[1], cmd->response[0]);
	} else {
		cmd->response[0] = readl(&priv->reg->resp0);
		debug("mmc resp 0x%08x\n", cmd->response[0]);
	}
out:
	if (error) {
		priv->raw_int_bak = readl(&priv->reg->rint) & SUNXI_MMC_RINT_INTERRUPT_ERROR_BIT;
		mmc_dump_errinfo(priv, cmd);
#if 0
	if (cmd->cmdidx == 1) {
		dumphex32("mmc", (char *)priv->reg, 0x200);
		dumphex32("ccmu_mmc2", (char *)SUNXI_CCM_BASE + 0x838, 0x4);
		dumphex32("ccmu_pll", (char *)SUNXI_CCM_BASE + 0x20, 0x20);
		dumphex32("ccmu_bgr", (char *)SUNXI_CCM_BASE + 0x84C, 0x4);
		dumphex32("ccmu_srr", (char *)SUNXI_CCM_BASE + 0x84C, 0x4);
		dumphex32("gpio_config", (char *)SUNXI_PIO_BASE + 0x48, 0x10);
		dumphex32("gpio_pull", (char *)SUNXI_PIO_BASE + 0x64, 0x8);
	}
#endif
	}
	if (data && usedma) {
		status = readl(&priv->reg->idst);
		writel(status, &priv->reg->idst);
		writel(0, &priv->reg->idie);
		writel(0, &priv->reg->dmac);
		writel(readl(&priv->reg->gctrl) & (~(1 << 5)), &priv->reg->gctrl);
	}
	if (error < 0) {
#if (!defined(CONFIG_MACH_SUN8IW7))
		/* during tuning sample point, some sample point may cause timing problem.
		for example, if a RTO error occurs, host may stop clock and device may still output data.
		we need to read all data(512bytes) from device to avoid to update clock fail.
		*/
		signed int timeout = 0;
		if (mmc->do_tuning && data && (data->flags&MMC_DATA_READ) && (bytecnt == 512)) {
			writel(readl(&priv->reg->gctrl)|0x80000000, &priv->reg->gctrl);
			writel(0xdeb, &priv->reg->dbgc);
			timeout = 1000;
			MMCMSG(mmc, "Read remain data\n");
			while (readl(&priv->reg->bbcr) < 512) {
				unsigned int tmp = readl(priv->database);
				tmp = tmp + 1;
				MMCDBG("Read data 0x%x, bbcr 0x%x\n", tmp, readl(&ipriv->reg->bbcr));
				__usdelay(1);
				if (!(timeout--)) {
					MMCMSG(mmc, "Read remain data timeout\n");
					break;
				}
			}
		}

		writel(0x7, &priv->reg->gctrl);
		while (readl(&priv->reg->gctrl)&0x7) {
			debug("mmc reset dma fifo and fifo\n");
		};

		{
			mmc_save_regs(priv);
			mmc_clk_io_onoff(priv->mmc_no, 0, 0);
			MMCMSG(mmc, "mmc %d close bus gating and reset\n", priv->mmc_no);
			mmc_clk_io_onoff(priv->mmc_no, 1, 0);
			mmc_restore_regs(priv);

			writel(0x7, &priv->reg->gctrl);
			while (readl(&priv->reg->gctrl)&0x7) {
				debug("mmc reset dma fifo and fifo\n");
			};
		}
#endif

		writel(SUNXI_MMC_GCTRL_RESET, &priv->reg->gctrl);
		mmc_update_clk(priv);
	}
	writel(0xffffffff, &priv->reg->rint);
	writel(readl(&priv->reg->gctrl) | SUNXI_MMC_GCTRL_FIFO_RESET,
	       &priv->reg->gctrl);

	return error;
}

#if !CONFIG_IS_ENABLED(DM_MMC)
static int sunxi_mmc_set_ios_legacy(struct mmc *mmc)
{
	struct sunxi_mmc_priv *priv = mmc->priv;

	return sunxi_mmc_set_ios_common(priv, mmc);
}

static int sunxi_mmc_send_cmd_legacy(struct mmc *mmc, struct mmc_cmd *cmd,
				     struct mmc_data *data)
{
	struct sunxi_mmc_priv *priv = mmc->priv;

	return sunxi_mmc_send_cmd_common(priv, mmc, cmd, data);
}

static int sunxi_mmc_getcd_legacy(struct mmc *mmc)
{
	struct sunxi_mmc_priv *priv = mmc->priv;
	int cd_pin;

	cd_pin = sunxi_mmc_getcd_gpio(priv->mmc_no);
	if (cd_pin < 0)
		return 1;

	return !gpio_get_value(cd_pin);
}

static int sunxi_decide_rty(struct mmc *mmc, int err_no, uint rst_cnt)
{
	struct sunxi_mmc_priv *mmcpriv = (struct sunxi_mmc_priv *)mmc->priv;
	unsigned tmode = mmcpriv->timing_mode;
	u32 spd_md, freq;
	u8 *sdly;
	u8 tm1_retry_gap = 1;
	u8 tm3_retry_gap = 8;
	u8 tm4_retry_gap = 8;

	if (rst_cnt) {
		mmcpriv->retry_cnt = 0;
	}

	if (err_no && (!(err_no & SDXC_RespTimeout) || (err_no == 0xffffffff))) {
		mmcpriv->retry_cnt++;

		if (tmode == SUNXI_MMC_TIMING_MODE_1) {
			spd_md = mmcpriv->tm1.cur_spd_md;
			freq = mmcpriv->tm1.cur_freq;
			sdly = &mmcpriv->tm1.sdly[spd_md*MAX_CLK_FREQ_NUM+freq];

			if (mmcpriv->retry_cnt * tm1_retry_gap <  MMC_CLK_SAMPLE_POINIT_MODE_1) {
				if ((*sdly + tm1_retry_gap) < MMC_CLK_SAMPLE_POINIT_MODE_1) {
					*sdly = *sdly + tm1_retry_gap;
				} else {
					*sdly = *sdly + tm1_retry_gap - MMC_CLK_SAMPLE_POINIT_MODE_1;
				}
				printf("Get next samply point %d at spd_md %d freq_id %d\n", *sdly, spd_md, freq);
			} else {
				printf("Beyond the retry times\n");
				return -1;
			}
		} else if (tmode == SUNXI_MMC_TIMING_MODE_3) {
			spd_md = mmcpriv->tm3.cur_spd_md;
			freq = mmcpriv->tm3.cur_freq;
			sdly = &mmcpriv->tm3.sdly[spd_md*MAX_CLK_FREQ_NUM+freq];

			if (mmcpriv->retry_cnt * tm3_retry_gap <  MMC_CLK_SAMPLE_POINIT_MODE_3) {
				if ((*sdly + tm3_retry_gap) < MMC_CLK_SAMPLE_POINIT_MODE_3) {
					*sdly = *sdly + tm3_retry_gap;
				} else {
					*sdly = *sdly + tm3_retry_gap - MMC_CLK_SAMPLE_POINIT_MODE_3;
				}
				printf("Get next samply point %d at spd_md %d freq_id %d\n", *sdly, spd_md, freq);
			} else {
				printf("Beyond the retry times\n");
				return -1;
			}
		} else if (tmode == SUNXI_MMC_TIMING_MODE_4) {
			spd_md = mmcpriv->tm4.cur_spd_md;
			freq = mmcpriv->tm4.cur_freq;
			if (spd_md == HS400)
				sdly = &mmcpriv->tm4.dsdly[freq];
			else
				sdly = &mmcpriv->tm4.sdly[spd_md*MAX_CLK_FREQ_NUM+freq];
			printf("Current spd_md %d freq_id %d sldy %d\n", spd_md, freq, *sdly);

			if (mmcpriv->retry_cnt * tm4_retry_gap <  MMC_CLK_SAMPLE_POINIT_MODE_4) {
				if ((*sdly + tm4_retry_gap) < MMC_CLK_SAMPLE_POINIT_MODE_4) {
					*sdly = *sdly + tm4_retry_gap;
				} else {
					*sdly = *sdly + tm4_retry_gap - MMC_CLK_SAMPLE_POINIT_MODE_4;
				}
				printf("Get next samply point %d at spd_md %d freq_id %d\n", *sdly, spd_md, freq);
			} else {
				printf("Beyond the retry times\n");
				return -1;
			}
		}

		mmcpriv->raw_int_bak = 0;
		return 0;
	}
	pr_err("rto or no error or software timeout,no need retry\n");

	return -1;
}

static int sunxi_detail_errno(struct mmc *mmc)
{
	struct sunxi_mmc_priv *mmcpriv = (struct sunxi_mmc_priv *)mmc->priv;
	u32 err_no = mmcpriv->raw_int_bak;
	mmcpriv->raw_int_bak = 0;
	return err_no;
}


static const struct mmc_ops sunxi_mmc_ops = {
	.send_cmd	= sunxi_mmc_send_cmd_legacy,
	.set_ios	= sunxi_mmc_set_ios_legacy,
	.init		= sunxi_mmc_core_init,
	.getcd		= sunxi_mmc_getcd_legacy,
	.decide_retry		= sunxi_decide_rty,
	.get_detail_errno	= sunxi_detail_errno,
};


int sunxi_mmcno_to_devnum(int sdc_no)
{
	struct mmc *m;
	struct sunxi_mmc_priv *ppriv = &mmc_host[sdc_no];
	if (ppriv->mmc_no != sdc_no) {
		printk("error,card no error\n");
		return -1;
	}
//	m = container_of((void *)ppriv, struct mmc, priv);
	m = ppriv->mmc;
	if (m == NULL) {
		printk("error card no error\n");
		return -1;
	}
	debug("%s ,devnum %d\n", __FUNCTION__,  m->block_dev.devnum);
	debug("devnum %d, pprv %x, bdesc %x\n",  m->block_dev.devnum, (u32)ppriv,  (u32)&m->block_dev);
	debug("m %x, ppriv %x", (u32)m, (u32)ppriv);
	return m->block_dev.devnum;
}

struct mmc *sunxi_mmc_init(int sdc_no)
{
	__attribute__((unused)) struct sunxi_ccm_reg *ccm = (struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	struct sunxi_mmc_priv *priv = &mmc_host[sdc_no];
	struct mmc_config *cfg = &priv->cfg;
	int ret;

	memset(priv, '\0', sizeof(struct sunxi_mmc_priv));
	pr_msg("mmc driver ver %s\n", DRIVER_VER);
	memset(&mmc_host_reg_bak[sdc_no], 0, sizeof(struct mmc_reg_v4p1));
	priv->reg_bak =  &mmc_host_reg_bak[sdc_no];

	if ((sdc_no == 2)) {
		cfg->odly_spd_freq = &ext_odly_spd_freq[0];
		cfg->sdly_spd_freq = &ext_sdly_spd_freq[0];
	} else if (sdc_no == 0) {
		cfg->odly_spd_freq = &ext_odly_spd_freq_sdc0[0];
		cfg->sdly_spd_freq = &ext_sdly_spd_freq_sdc0[0];
	}

	cfg->name = "SUNXI SD/MMC";
	cfg->host_no = sdc_no;
	cfg->ops  = &sunxi_mmc_ops;

	cfg->voltages = MMC_VDD_32_33 | MMC_VDD_33_34;
	cfg->host_caps = MMC_MODE_4BIT;
	if (sdc_no == 2)
		cfg->host_caps |= MMC_MODE_8BIT;
	cfg->host_caps |= MMC_MODE_HS_52MHz | MMC_MODE_HS | MMC_MODE_DDR_52MHz;
	cfg->b_max = CONFIG_SYS_MMC_MAX_BLK_COUNT;
	if (sdc_no == 0) {
		cfg->f_min = 400000;
		cfg->f_max = 50000000;
	} else if (sdc_no == 2) {
		cfg->f_min = 400000;
#ifdef CONFIG_MACH_SUN8IW7
		cfg->f_max = 50000000;
#else
		cfg->f_max = 200000000;
#endif
	}
#ifdef CONFIG_MACH_SUN8IW7
	if ((sdc_no == 0) || (sdc_no == 1))
		priv->timing_mode = SUNXI_MMC_TIMING_MODE_0; //SUNXI_MMC_TIMING_MODE_3
	else if ((sdc_no == 2))
		priv->timing_mode = SUNXI_MMC_TIMING_MODE_1;
#else
	if ((sdc_no == 0) || (sdc_no == 1))
		priv->timing_mode = SUNXI_MMC_TIMING_MODE_1; //SUNXI_MMC_TIMING_MODE_3
	else if ((sdc_no == 2))
		priv->timing_mode = SUNXI_MMC_TIMING_MODE_4;
#endif
	priv->pdes = memalign(CONFIG_SYS_CACHELINE_SIZE, 256 * 1024);
	if (priv->pdes == NULL) {
		pr_msg("get mem for descripter failed !\n");
		return NULL;
	} else {
		pr_error("get mem for descripter OK !\n");
	}
	if (sunxi_host_mmc_config(sdc_no) != 0) {
		pr_error("sunxi host mmc config failed!\n");
		return NULL;
	}
	if (sunxi_mmc_pin_set(sdc_no) != 0) {
		printf("sunxi mmc pin set failed!\n");
		return NULL;
	}
	if (mmc_resource_init(sdc_no) != 0) {
		pr_error("mmc resourse init failed!\n");
		return NULL;
	}
	mmc_clk_io_onoff(sdc_no, 1, 1);

	if (cfg->io_is_1v8) {
		pr_err("io is 1.8V\n");
		cfg->host_caps |= MMC_MODE_HS200;
		if (cfg->host_caps & MMC_MODE_8BIT)
			cfg->host_caps |= MMC_MODE_HS400;
	}

	if (cfg->host_caps_mask) {
		u32 mask = cfg->host_caps_mask;
		if (mask & DRV_PARA_DISABLE_MMC_MODE_HS400)
			cfg->host_caps &= (~MMC_MODE_HS400);
		if (mask & DRV_PARA_DISABLE_MMC_MODE_HS200)
			cfg->host_caps &= (~(MMC_MODE_HS200
						| MMC_MODE_HS400));
		if (mask & DRV_PARA_DISABLE_MMC_MODE_DDR_52MHz)
			cfg->host_caps &= (~(MMC_MODE_DDR_52MHz
						| MMC_MODE_HS400
						| MMC_MODE_HS200));
		if (mask & DRV_PARA_DISABLE_MMC_MODE_HS_52MHz)
			cfg->host_caps &= (~(MMC_MODE_HS_52MHz
						| MMC_MODE_DDR_52MHz
						| MMC_MODE_HS400
						| MMC_MODE_HS200));
		if (mask & DRV_PARA_DISABLE_MMC_MODE_8BIT)
			cfg->host_caps &= (~(MMC_MODE_8BIT
						| MMC_MODE_HS400));
		if (mask & DRV_PARA_ENABLE_EMMC_HW_RST)
			cfg->host_caps |= DRV_PARA_ENABLE_EMMC_HW_RST;
	}
	pr_debug("host_caps:0x%x\n", cfg->host_caps);

#ifdef FPGA_PLATFORM
	int i = 0;
	if (sdc_no == 0) {
		for (i = 0; i < 6; i++) {
			sunxi_gpio_set_cfgpin(SUNXI_GPF(i), SUNXI_GPF_SDC0);
			sunxi_gpio_set_pull(SUNXI_GPF(i), 1);
			sunxi_gpio_set_drv(SUNXI_GPF(i), 2);
		}
	} else {
		unsigned int pin;
		for (pin = SUNXI_GPC(0); pin <= SUNXI_GPF(25); pin++) {
				sunxi_gpio_set_cfgpin(pin, 2);
				sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
				sunxi_gpio_set_drv(pin, 2);
		}
	}
#endif

	/* config ahb clock */
	pr_info("init mmc %d clock and io\n", sdc_no);
#if (!defined(CONFIG_MACH_SUN50I_H6) && !defined(CONFIG_MACH_SUN8IW16)\
	&& !defined(CONFIG_MACH_SUN50IW9) && !defined(CONFIG_MACH_SUN8IW19)\
	&& !defined(CONFIG_MACH_SUN50IW10) && !defined(CONFIG_MACH_SUN8IW15)\
	&& !defined(CONFIG_MACH_SUN50IW11))
	setbits_le32(&ccm->ahb_gate0, 1 << AHB_GATE_OFFSET_MMC(sdc_no));

#ifdef CONFIG_SUNXI_GEN_SUN6I
	/* unassert reset */
	setbits_le32(&ccm->ahb_reset0_cfg, 1 << AHB_RESET_OFFSET_MMC(sdc_no));
#endif
#if defined(CONFIG_MACH_SUN9I)
	/* sun9i has a mmc-common module, also set the gate and reset there */
	writel(SUNXI_MMC_COMMON_CLK_GATE | SUNXI_MMC_COMMON_RESET,
	       SUNXI_MMC_COMMON_BASE + 4 * sdc_no);
#endif
#elif defined CONFIG_MACH_SUN50I_H6 /* CONFIG_MACH_SUN50I_H6 */
	setbits_le32(&ccm->sd_gate_reset, 1 << sdc_no);
	/* unassert reset */
	setbits_le32(&ccm->sd_gate_reset, 1 << (RESET_SHIFT + sdc_no));
#endif
	ret = mmc_set_mod_clk(priv, 24000000);
	if (ret)
		return NULL;
	return mmc_create(cfg, priv);
}
#else

static int sunxi_mmc_set_ios(struct udevice *dev)
{
	struct sunxi_mmc_plat *plat = dev_get_platdata(dev);
	struct sunxi_mmc_priv *priv = dev_get_priv(dev);

	return sunxi_mmc_set_ios_common(priv, &plat->mmc);
}

static int sunxi_mmc_send_cmd(struct udevice *dev, struct mmc_cmd *cmd,
			      struct mmc_data *data)
{
	struct sunxi_mmc_plat *plat = dev_get_platdata(dev);
	struct sunxi_mmc_priv *priv = dev_get_priv(dev);

	return sunxi_mmc_send_cmd_common(priv, &plat->mmc, cmd, data);
}

static int sunxi_mmc_getcd(struct udevice *dev)
{
	struct sunxi_mmc_priv *priv = dev_get_priv(dev);

	if (dm_gpio_is_valid(&priv->cd_gpio)) {
		int cd_state = dm_gpio_get_value(&priv->cd_gpio);

		return cd_state ^ priv->cd_inverted;
	}
	return 1;
}

static const struct dm_mmc_ops sunxi_mmc_ops = {
	.send_cmd	= sunxi_mmc_send_cmd,
	.set_ios	= sunxi_mmc_set_ios,
	.get_cd		= sunxi_mmc_getcd,
};

static int sunxi_mmc_probe(struct udevice *dev)
{
	struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
	struct sunxi_mmc_plat *plat = dev_get_platdata(dev);
	struct sunxi_mmc_priv *priv = dev_get_priv(dev);
	struct mmc_config *cfg = &plat->cfg;
	struct ofnode_phandle_args args;
	u32 *gate_reg;
	int bus_width, ret;

	cfg->name = dev->name;
	bus_width = dev_read_u32_default(dev, "bus-width", 1);

	cfg->voltages = MMC_VDD_32_33 | MMC_VDD_33_34;
	cfg->host_caps = 0;
	if (bus_width == 8)
		cfg->host_caps |= MMC_MODE_8BIT;
	if (bus_width >= 4)
		cfg->host_caps |= MMC_MODE_4BIT;
	cfg->host_caps |= MMC_MODE_HS_52MHz | MMC_MODE_HS;
	cfg->b_max = CONFIG_SYS_MMC_MAX_BLK_COUNT;
	cfg->f_min = 400000;
	cfg->f_max = 52000000;

	priv->reg = (void *)dev_read_addr(dev);

	/* We don't have a sunxi clock driver so find the clock address here */
	ret = dev_read_phandle_with_args(dev, "clocks", "#clock-cells", 0,
					  1, &args);
	if (ret)
		return ret;
	priv->mclkreg = (u32 *)ofnode_get_addr(args.node);

	ret = dev_read_phandle_with_args(dev, "clocks", "#clock-cells", 0,
					  0, &args);
	if (ret)
		return ret;
	gate_reg = (u32 *)ofnode_get_addr(args.node);
	setbits_le32(gate_reg, 1 << args.args[0]);
	priv->mmc_no = args.args[0] - 8;

	ret = mmc_set_mod_clk(priv, 24000000);
	if (ret)
		return ret;

	/* This GPIO is optional */
	if (!gpio_request_by_name(dev, "cd-gpios", 0, &priv->cd_gpio,
				  GPIOD_IS_IN)) {
		int cd_pin = gpio_get_number(&priv->cd_gpio);

		sunxi_gpio_set_pull(cd_pin, SUNXI_GPIO_PULL_UP);
	}

	/* Check if card detect is inverted */
	priv->cd_inverted = dev_read_bool(dev, "cd-inverted");

	upriv->mmc = &plat->mmc;

	/* Reset controller */
	writel(SUNXI_MMC_GCTRL_RESET, &priv->reg->gctrl);
	udelay(1000);

	return 0;
}

static int sunxi_mmc_bind(struct udevice *dev)
{
	struct sunxi_mmc_plat *plat = dev_get_platdata(dev);

	return mmc_bind(dev, &plat->mmc, &plat->cfg);
}

static const struct udevice_id sunxi_mmc_ids[] = {
	{ .compatible = "allwinner,sun5i-a13-mmc" },
	{ }
};

U_BOOT_DRIVER(sunxi_mmc_drv) = {
	.name		= "sunxi_mmc",
	.id		= UCLASS_MMC,
	.of_match	= sunxi_mmc_ids,
	.bind		= sunxi_mmc_bind,
	.probe		= sunxi_mmc_probe,
	.ops		= &sunxi_mmc_ops,
	.platdata_auto_alloc_size = sizeof(struct sunxi_mmc_plat),
	.priv_auto_alloc_size = sizeof(struct sunxi_mmc_priv),
};
#endif
