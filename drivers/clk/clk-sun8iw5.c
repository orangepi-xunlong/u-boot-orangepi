/*
 * Copyright (C) 2013 Allwinnertech, kevin.z.m <kevin@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include "clk-sun8iw5.h"
#include "clk-sun8iw5_tbl.c"
#include<clk/clk_plat.h>
#include"clk_factor.h"
#include"clk_periph.h"
#include <linux/compat.h>
#include<div64.h>


#define FACTOR_SIZEOF(name) (sizeof(factor_pll##name##_tbl)/ \
			     sizeof(struct sunxi_clk_factor_freq))
#define FACTOR_SEARCH(name) (sunxi_clk_com_ftr_sr( \
		&sunxi_clk_factor_pll_##name, factor, \
		factor_pll##name##_tbl, index, \
		FACTOR_SIZEOF(name)))

#ifndef CONFIG_EVB_PLATFORM
	#define LOCKBIT(x) 31
#else
	#define LOCKBIT(x) x
#endif

static DEFINE_SPINLOCK(clk_lock);
void __iomem *sunxi_clk_base;
void __iomem *sunxi_clk_cpus_base;
int sunxi_clk_maxreg = SUNXI_CLK_MAX_REG;
int cpus_clk_maxreg = CPUS_CLK_MAX_REG;

#ifdef CONFIG_SUNXI_CLK_DUMMY_DEBUG
unsigned int dummy_reg[1024];
unsigned int dummy_readl(unsigned int *regaddr)
{
	unsigned int val;
	val = *regaddr;

	pr_info("--%s-- dummy_readl to read reg 0x%x with val 0x%x\n",
		__func__,
		((unsigned int)regaddr - (unsigned int)&dummy_reg[0]),
		val);

	return val;
}

void  dummy_writel(unsigned int val, unsigned int *regaddr)
{
	*regaddr = val;
	pr_info("--%s-- dummy_writel to write reg 0x%x with val 0x%x\n",
		__func__,
		((unsigned int)regaddr - (unsigned int)&dummy_reg[0]),
		val);
}

void dummy_reg_init(void)
{
	memset(dummy_reg, 0x0, sizeof(dummy_reg));
	dummy_reg[PLL1_CFG/4] = 0x00001000;
	dummy_reg[PLL2_CFG/4] = 0x00035514;
	dummy_reg[PLL3_CFG/4] = 0x03006207;
	dummy_reg[PLL4_CFG/4] = 0x03006207;
	dummy_reg[PLL5_CFG/4] = 0x00001000;
	dummy_reg[PLL6_CFG/4] = 0x00041811;
	dummy_reg[PLL8_CFG/4] = 0x03006207;
	dummy_reg[PLL9_CFG/4] = 0x03001300;
	dummy_reg[PLL10_CFG/4] = 0x03006207;
	dummy_reg[CPU_CFG/4] = 0x00001000;
	dummy_reg[AHB1_CFG/4] = 0x00001010;
	dummy_reg[APB2_CFG/4] = 0x01000000;
	dummy_reg[ATS_CFG/4] = 0x80000000;
	dummy_reg[PLL_LOCK/4] = 0x000000FF;
	dummy_reg[CPU_LOCK/4] = 0x000000FF;
}
#endif /* of CONFIG_SUNXI_CLK_DUMMY_DEBUG */


/*                          ns  nw  ks  kw  ms  mw  ps  pw  d1s  d1w  d2s  d2w  {frac  out  mode}  en-s   sdmss  sdmsw  sdmpat      sdmval*/
SUNXI_CLK_FACTORS(pll_cpu,   8,  5,  4,  2,  0,  2,  16, 2,  0,   0,   0,   0,    0,    0,   0,     31,   24,     0,       PLL_CPUPAT,  0xd1303333);
SUNXI_CLK_FACTORS(pll_audio, 8,  7,  0,  0,  0,  5,  16, 4,  0,   0,   0,   0,    0,    0,   0,     31,    0,     0,       0,        0);
SUNXI_CLK_FACTORS_UPDATE(pll_ddr0,  8,  5,  4,  2,  0,  2,  0,  0,  0,   0,   0,   0,    0,    0,   0,     31,    0,     0,       0,        0 , 20);
SUNXI_CLK_FACTORS(pll_periph, 8,  5,  4,  2,  0,  0,  0,  0,  0,   0,   0,   0,    0,    0,   0,     31,    0,     0,       0,        0);
SUNXI_CLK_FACTORS(pll_video, 8,  7,  0,  0,  0,  4,  0,  0,  0,   0,   0,   0,    1,    25,  24,    31,   20,     0,       PLL_VIDEOPAT, 0xd1303333);
SUNXI_CLK_FACTORS(pll_ve,    8,  7,  0,  0,  0,  4,  0,  0,  0,   0,   0,   0,    1,    25,  24,    31,   20,     0,       PLL_VEPAT   , 0xd1303333);
SUNXI_CLK_FACTORS(pll_gpu,   8,  7,  0,  0,  0,  4,  0,  0,  0,   0,   0,   0,    1,    25,  24,    31,   20,     0,       PLL_GPUPAT  , 0xd1303333);
SUNXI_CLK_FACTORS(pll_hsic,  8,  7,  0,  0,  0,  4,  0,  0,  0,   0,   0,   0,    1,    25,  24,    31,   20,     0,       PLL_HSICPAT , 0xd1303333);
SUNXI_CLK_FACTORS(pll_de,    8,  7,  0,  0,  0,  4,  0,  0,  0,   0,   0,   0,    1,    25,  24,    31,   20,     0,       PLL_DEPAT   , 0xd1303333);
SUNXI_CLK_FACTORS(pll_mipi,  8,  4,  4,  2,  0,  4,  0,  0,  0,   0,   0,   0,    0,    0,   0,     31,   20,     0,       PLL_MIPIPAT, 0xd1303333);
SUNXI_CLK_FACTORS_UPDATE(pll_ddr1,  8,  6,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,    0,    0,   0,     31,    0,     0,       0,        0 , 30);


static int get_factors_pll_cpu(u32 rate, u32 parent_rate, struct clk_factors_value *factor)
{
	int index;
    u64 tmp_rate;
    if (!factor)
		return -1;
    tmp_rate = rate > pllcpu_max ? pllcpu_max : rate;
    do_div(tmp_rate, 1000000);
    index = tmp_rate;
	if (FACTOR_SEARCH(cpu))
		return -1;
    return 0;
}

static int get_factors_pll_ddr0(u32 rate, u32 parent_rate, struct clk_factors_value *factor)
{
	int index;
	u64 tmp_rate;
	if (!factor)
		return -1;
	tmp_rate = rate > pllddr0_max ? pllddr0_max : rate;
	do_div(tmp_rate, 1000000);
	index = tmp_rate;
	if (FACTOR_SEARCH(ddr0))
		return -1;

	return 0;
}

static int get_factors_pll_ddr1(u32 rate, u32 parent_rate, struct clk_factors_value *factor)
{
	int index;
	u64 tmp_rate;
	if (!factor)
		return -1;
	tmp_rate = rate > pllddr1_max ? pllddr1_max : rate;
	do_div(tmp_rate, 1000000);
	index = tmp_rate;
	if (FACTOR_SEARCH(ddr1))
		return -1;

	return 0;
}
static int get_factors_pll_periph(u32 rate, u32 parent_rate, struct clk_factors_value *factor)
{
	int index;
    u64 tmp_rate;
    if (!factor)
		return -1;
    tmp_rate = rate > pllperiph_max ? pllperiph_max : rate;
    do_div(tmp_rate, 1000000);
    index = tmp_rate;
    if (FACTOR_SEARCH(periph))
		return -1;
    return 0;
}
static int get_factors_pll_audio(u32 rate, u32 parent_rate, struct clk_factors_value *factor)
{
    if (rate == 22579200) {
		factor->factorn = 78;
		factor->factorm = 20;
		factor->factorp = 3;
    } else if (rate == 24576000) {
		factor->factorn = 85;
		factor->factorm = 20;
		factor->factorp = 3;
    } else
		return -1;

    return 0;
}

static int get_factors_pll_video(u32 rate, u32 parent_rate, struct clk_factors_value *factor)
{
    u64 tmp_rate;
    int index;
    if (!factor)
		return -1;

    tmp_rate = rate > pllvideo_max ? pllvideo_max : rate;
    do_div(tmp_rate, 1000000);
    index = tmp_rate;
	if (FACTOR_SEARCH(video))
		return -1;
    if (rate == 297000000) {
		factor->frac_mode = 0;
		factor->frac_freq = 1;
		factor->factorm = 0;
    } else if (rate == 270000000) {
		factor->frac_mode = 0;
		factor->frac_freq = 0;
		factor->factorm = 0;
    } else {
		factor->frac_mode = 1;
		factor->frac_freq = 0;
    }

    return 0;
}
static int get_factors_pll_ve(u32 rate, u32 parent_rate, struct clk_factors_value *factor)
{
    u64 tmp_rate;
    int index;
    if (!factor)
		return -1;

    tmp_rate = rate > pllve_max ? pllve_max : rate;
    do_div(tmp_rate, 1000000);
    index = tmp_rate;
    if (FACTOR_SEARCH(ve))
		return -1;
    if (rate == 297000000) {
		factor->frac_mode = 0;
		factor->frac_freq = 1;
		factor->factorm = 0;
	} else if (rate == 270000000) {
		factor->frac_mode = 0;
		factor->frac_freq = 0;
		factor->factorm = 0;
    } else {
		factor->frac_mode = 1;
		factor->frac_freq = 0;
    }

    return 0;
}
static int get_factors_pll_gpu(u32 rate, u32 parent_rate, struct clk_factors_value *factor)
{
    u64 tmp_rate;
    int index;
    if (!factor)
		return -1;

    tmp_rate = rate > pllgpu_max ? pllgpu_max : rate;
    do_div(tmp_rate, 1000000);
    index = tmp_rate;
    if (FACTOR_SEARCH(gpu))
		return -1;
    if (rate == 297000000) {
		factor->frac_mode = 0;
		factor->frac_freq = 1;
		factor->factorm = 0;
    } else if (rate == 270000000) {
		factor->frac_mode = 0;
		factor->frac_freq = 0;
		factor->factorm = 0;
    } else {
		factor->frac_mode = 1;
		factor->frac_freq = 0;
    }

    return 0;
}
static int get_factors_pll_hsic(u32 rate, u32 parent_rate, struct clk_factors_value *factor)
{
    u64 tmp_rate;
    int index;
    if (!factor)
		return -1;

    tmp_rate = rate > pllhsic_max ? pllhsic_max : rate;
    do_div(tmp_rate, 1000000);
    index = tmp_rate;
    if (FACTOR_SEARCH(hsic))
		return -1;
    if (rate == 297000000) {
		factor->frac_mode = 0;
		factor->frac_freq = 1;
		factor->factorm = 0;
    } else if (rate == 270000000) {
		factor->frac_mode = 0;
		factor->frac_freq = 0;
		factor->factorm = 0;
    } else {
		factor->frac_mode = 1;
		factor->frac_freq = 0;
    }

    return 0;
}
static int get_factors_pll_de(u32 rate, u32 parent_rate, struct clk_factors_value *factor)
{
    u64 tmp_rate;
	int index;
	if (!factor)
		return -1;

    tmp_rate = rate > pllde_max ? pllde_max : rate;
    do_div(tmp_rate, 1000000);
    index = tmp_rate;
    if (FACTOR_SEARCH(de))
		return -1;
    if (rate == 297000000) {
		factor->frac_mode = 0;
		factor->frac_freq = 1;
		factor->factorm = 0;
    } else if (rate == 270000000) {
		factor->frac_mode = 0;
		factor->frac_freq = 0;
		factor->factorm = 0;
    } else {
		factor->frac_mode = 1;
		factor->frac_freq = 0;
    }

    return 0;
}
static int get_factors_pll_mipi(u32 rate, u32 parent_rate, struct clk_factors_value *factor)
{

    u64 tmp_rate;
    u32 delta1, delta2, want_rate, new_rate, save_rate = 0;
    int n, k, m;
    if (!factor)
		return -1;
	tmp_rate = rate > 1440000000 ? 1440000000 : rate;
    do_div(tmp_rate, 1000000);
    want_rate = tmp_rate;
    for (m = 1;			m <= 16 ;	m++)
		for (k = 2;			k <= 4 ;	k++)
			for (n = 1;			n <= 16;	n++) {
				new_rate = (parent_rate/1000000)*k*n/m;
				delta1 = (new_rate > want_rate) ? (new_rate - want_rate) : (want_rate - new_rate);
				delta2 =  (save_rate > want_rate) ? (save_rate - want_rate) : (want_rate - save_rate);
				if (delta1 < delta2) {
					factor->factorn = n-1;
					factor->factork = k-1;
					factor->factorm = m-1;
					save_rate = new_rate;
				}
			}

    return 0;
}

static unsigned long calc_rate_pll_cpu(u32 parent_rate, struct clk_factors_value *factor)
{
	u64 tmp_rate = (parent_rate ? parent_rate : 24000000);
	tmp_rate = tmp_rate * (factor->factorn+1) * (factor->factork+1);
	do_div(tmp_rate, (factor->factorm+1) * (1 << factor->factorp));
	return (unsigned long)tmp_rate;
}

static unsigned long calc_rate_pll_audio(u32 parent_rate, struct clk_factors_value *factor)
{
	u64 tmp_rate = (parent_rate ? parent_rate : 24000000);
	if ((factor->factorn == 78) && (factor->factorm == 20) && (factor->factorp == 3))
		return 22579200;
	else if ((factor->factorn == 85) && (factor->factorm == 20) && (factor->factorp == 3))
		return 24576000;
	else {
		tmp_rate = tmp_rate * (factor->factorn+1);
		do_div(tmp_rate, (factor->factorm+1) * (factor->factorp+1));
		return (unsigned long)tmp_rate;
	}
}

static unsigned long calc_rate_media(u32 parent_rate, struct clk_factors_value *factor)
{
	u64 tmp_rate = (parent_rate ? parent_rate : 24000000);
	if (factor->frac_mode == 0) {
		if (factor->frac_freq == 1)
			return 297000000;
		else
			return 270000000;
	} else {
		tmp_rate = tmp_rate * (factor->factorn+1);
		do_div(tmp_rate, factor->factorm+1);
		return (unsigned long)tmp_rate;
	}
}

static unsigned long calc_rate_pll_ddr0(u32 parent_rate, struct clk_factors_value *factor)
{
	u64 tmp_rate = (parent_rate ? parent_rate : 24000000);
	tmp_rate = tmp_rate * (factor->factorn+1) * (factor->factork+1);
	do_div(tmp_rate, factor->factorm+1);
	return (unsigned long)tmp_rate;
}
static unsigned long calc_rate_pll_ddr1(u32 parent_rate, struct clk_factors_value *factor)
{
	return (unsigned long)(parent_rate ? parent_rate : 24000000) * (factor->factorn + 1);
}
static unsigned long calc_rate_pll_periph(u32 parent_rate, struct clk_factors_value *factor)
{
	return (unsigned long)(parent_rate ? (parent_rate/2) : 12000000) * (factor->factorn+1) * (factor->factork+1);
}
static unsigned long calc_rate_pll_mipi(u32 parent_rate, struct clk_factors_value *factor)
{
	u64 tmp_rate = (parent_rate ? parent_rate : 24000000);
	tmp_rate = tmp_rate * (factor->factorn+1) * (factor->factork+1);
	do_div(tmp_rate, factor->factorm+1);
	return (unsigned long)tmp_rate;
}

u8 get_parent_pll_mipi(struct clk_hw *hw)
{
	u8 parent;
	unsigned long reg;
	struct sunxi_clk_factors *factor = to_clk_factor(hw);

	if (!factor->reg)
		return 0;
	reg = readl(factor->reg);
	parent = GET_BITS(21, 1, reg);

	return parent;
}
int set_parent_pll_mipi(struct clk_hw *hw, u8 index)
{
	unsigned long reg;
	struct sunxi_clk_factors *factor = to_clk_factor(hw);

	if (!factor->reg)
		return 0;
	reg = readl(factor->reg);
	reg = SET_BITS(21, 1, reg, index);
	writel(reg, factor->reg);
	return 0;
}

static int clk_enable_pll_mipi(struct clk_hw *hw)
{
	struct sunxi_clk_factors *factor = to_clk_factor(hw);
    struct sunxi_clk_factors_config *config = factor->config;
    unsigned long reg = readl(factor->reg);

    if (config->sdmwidth) {
		writel(config->sdmval, (void __iomem *)config->sdmpat);
		reg = SET_BITS(config->sdmshift, config->sdmwidth, reg, 1);
    }

    reg |= 0x3 << 22;
    writel(reg, factor->reg);
    udelay(100);

    reg = SET_BITS(config->enshift, 1, reg, 1);
    writel(reg, factor->reg);
    udelay(100);

    return 0;
}

static void clk_disable_pll_mipi(struct clk_hw *hw)
{
	struct sunxi_clk_factors *factor = to_clk_factor(hw);
	struct sunxi_clk_factors_config *config = factor->config;
	unsigned long reg = readl(factor->reg);

	if (config->sdmwidth)
		reg = SET_BITS(config->sdmshift, config->sdmwidth, reg, 0);
	reg = SET_BITS(config->enshift, 1, reg, 0);
	reg &= ~(0x3 << 22);
	writel(reg, factor->reg);
}


u8 get_parent_mipi(struct clk_hw *hw)
{
	u8 parent;
	unsigned long reg;
	struct sunxi_clk_factors *factor = to_clk_factor(hw);

	if (!factor->reg)
		return 0;

	reg = readl(factor->reg);
	parent = GET_BITS(21, 1, reg);

	return parent;
}

int set_parent_mipi(struct clk_hw *hw, u8 index)
{
	unsigned long reg;
	struct sunxi_clk_factors *factor = to_clk_factor(hw);

	if (!factor->reg)
		return 0;

	reg = readl(factor->reg);
	reg = SET_BITS(21, 1, reg, index);
	writel(reg, factor->reg);

	return 0;
}

static const char *hosc_parents[] = {"hosc"};
static const char *mipi_parents[] = {"pll_video", ""};
struct clk_ops mipi_ops;

struct factor_init_data sunxi_factos[] = {
	/* name         parent        parent_num, 	flags       			reg       lock_reg   		lock_bit    	pll_lock_ctrl_reg lock_en_bit lock_mode  				config                     get_factors          calc_rate       priv_ops*/
    {"pll_cpu",     hosc_parents, 1, CLK_GET_RATE_NOCACHE, PLL_CPU,   PLL_CPU,    LOCKBIT(28), PLL_CLK_CTRL,     0,          PLL_LOCK_NONE_MODE, &sunxi_clk_factor_pll_cpu,   &get_factors_pll_cpu,    &calc_rate_pll_cpu   , (struct clk_ops *)NULL},
    {"pll_audio",   hosc_parents, 1,          0,          PLL_AUDIO, PLL_AUDIO,  LOCKBIT(28), PLL_CLK_CTRL,     1,          PLL_LOCK_NONE_MODE, &sunxi_clk_factor_pll_audio, &get_factors_pll_audio,  &calc_rate_pll_audio , (struct clk_ops *)NULL},
    {"pll_video",   hosc_parents, 1,          0,          PLL_VIDEO, PLL_VIDEO,  LOCKBIT(28), PLL_CLK_CTRL,     2,          PLL_LOCK_NONE_MODE, &sunxi_clk_factor_pll_video, &get_factors_pll_video,  &calc_rate_media     , (struct clk_ops *)NULL},
    {"pll_ve",      hosc_parents, 1,          0,          PLL_VE,    PLL_VE,     LOCKBIT(28), PLL_CLK_CTRL,     3,          PLL_LOCK_NONE_MODE, &sunxi_clk_factor_pll_ve,    &get_factors_pll_ve,     &calc_rate_media     , (struct clk_ops *)NULL},
    {"pll_ddr0",    hosc_parents, 1, CLK_GET_RATE_NOCACHE, PLL_DDR0,  PLL_DDR0,   LOCKBIT(28), PLL_CLK_CTRL,     4,          PLL_LOCK_NONE_MODE, &sunxi_clk_factor_pll_ddr0,  &get_factors_pll_ddr0,   &calc_rate_pll_ddr0  , (struct clk_ops *)NULL},
    {"pll_periph",  hosc_parents, 1,          0,          PLL_PERIPH, PLL_PERIPH, LOCKBIT(28), PLL_CLK_CTRL,     5,          PLL_LOCK_NONE_MODE, &sunxi_clk_factor_pll_periph, &get_factors_pll_periph, &calc_rate_pll_periph, (struct clk_ops *)NULL},
    {"pll_gpu",     hosc_parents, 1,          0,          PLL_GPU,   PLL_GPU,    LOCKBIT(28), PLL_CLK_CTRL,     6,          PLL_LOCK_NONE_MODE, &sunxi_clk_factor_pll_gpu,   &get_factors_pll_gpu,    &calc_rate_media     , (struct clk_ops *)NULL},
    {"pll_hsic",    hosc_parents, 1,          0,          PLL_HSIC,  PLL_HSIC,   LOCKBIT(28), PLL_CLK_CTRL,     7,          PLL_LOCK_NONE_MODE, &sunxi_clk_factor_pll_hsic,  &get_factors_pll_hsic,   &calc_rate_media     , (struct clk_ops *)NULL},
    {"pll_de",      hosc_parents, 1,          0,          PLL_DE,    PLL_DE,     LOCKBIT(28), PLL_CLK_CTRL,     8,          PLL_LOCK_NONE_MODE, &sunxi_clk_factor_pll_de,    &get_factors_pll_de,     &calc_rate_media     , (struct clk_ops *)NULL},
    {"pll_mipi",    mipi_parents, 2,          0,          PLL_MIPI,  PLL_MIPI,   LOCKBIT(28), PLL_CLK_CTRL,     9,          PLL_LOCK_NONE_MODE, &sunxi_clk_factor_pll_mipi,  &get_factors_pll_mipi,   &calc_rate_pll_mipi  , &mipi_ops},
    {"pll_ddr1",    hosc_parents, 1, CLK_GET_RATE_NOCACHE, PLL_DDR1,  PLL_DDR1,   LOCKBIT(28), PLL_CLK_CTRL,     10,          PLL_LOCK_NONE_MODE, &sunxi_clk_factor_pll_ddr1,  &get_factors_pll_ddr1,   &calc_rate_pll_ddr1  , (struct clk_ops *)NULL},
};

static const char *cpu_parents[] = {"losc", "hosc", "pll_cpu", "pll_cpu"};
static const char *axi_parents[] = {"cpu"};
static const char *pll_periphahb1_parents[] = {"pll_periph"};
static const char *ahb1_parents[] = {"losc", "hosc", "axi", "pll_periphahb1"};
static const char *apb1_parents[] = {"ahb1"};
static const char *apb2_parents[] = {"losc", "hosc", "pll_periph", "pll_periph"};
static const char *perph1_parents[] = {"hosc", "pll_periph"};
static const char *i2s_parents[] = {"pll_audiox8", "pll_audiox4", "pll_audiox2", "pll_audio"};
static const char *display_parents[] = {"pll_video", "", "pll_periphx2", "pll_gpu", "", "pll_de", "", ""};
static const char *lcd0_parents[] = {"pll_video", "", "pll_videox2", "", "pll_mipi", "", "", ""};
static const char *csi_s_parents[] = {"pll_video", "", "", "pll_de", "pll_mipi", "pll_ve", "", ""};
static const char *csi_m_parents[] = {"pll_video", "", "", "pll_de", "", "hosc", "", ""};
static const char *mbus_parents[] = {"hosc", "pll_periphx2", "pll_ddr0", "pll_ddr1"};
static const char *mipidsi_parents[] = {"pll_video", "", "pll_videox2"};
static const char *mipidphy_parents[] = {"pll_video", "", "pll_periph"};
static const char *adda_parents[] = {"pll_audio"};
static const char *addax4_parents[] = {"pll_audiox4"};
static const char *ve_parents[] = {"pll_ve"};
static const char *lvds_parents[] = {"lcd0ch0"};
static const char *gpu_parents[] = {"pll_gpu"};
static const char *ahb1mod_parents[] = {"ahb1"};
static const char *apb1mod_parents[] = {"apb1"};
static const char *apb2mod_parents[] = {"apb2"};
static const char *losc_parents[] = {"losc"};
static const char *sdmmc2_parents[] = {"sdmmc2_mode"};
static const char *hsic_parents[] =  {"pll_hsic"};
static const char *pll_periphcpus_parents[] = {"pll_periph"};

static const char *cpurcpus_parents[] = {"losc" , "hosc" , "pll_periphcpus" , "cpuosc" };
static const char *cpurahbs_parents[] = {"cpurcpus"};
static const char *cpurapbs_parents[] = {"cpurahbs"};
static const char *cpuruart_parents[] = {"cpurapbs"};
static const char *cpurpio_parents[] = {"cpurapbs"};



struct sunxi_clk_comgate com_gates[] = {
{"csi",      0,  0x3,    BUS_GATE_SHARE|RST_GATE_SHARE|MBUS_GATE_SHARE, 0},
{"adda",     0,  0x1,    BUS_GATE_SHARE|RST_GATE_SHARE,                 0},
{"mipidsi", 0,  0x1,    BUS_GATE_SHARE|RST_GATE_SHARE,                 0},
};

/*
SUNXI_CLK_PERIPH(		name,  mux_reg,    mux_shift,	 mux_width, 		div_reg,    div_mshift, 	div_mwidth, 	div_nshift, 	div_nwidth,	 gate_flags, 	enable_reg, 	reset_reg, 	bus_gate_reg, drm_gate_reg, enable_shift, reset_shift, 	bus_gate_shift, 	dram_gate_shift, 			lock,	com_gate,	com_gate_off)
*/
SUNXI_CLK_PERIPH(cpu,     CPU_CFG,    16,        2,         0,          0,          0,          0,          0,          0,          0,          0,         0,            0,            0,            0,           0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(axi,     0,           0,        0,         CPU_CFG,    0,          2,          0,          0,          0,          0,          0,         0,            0,            0,            0,           0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(pll_periphahb1,  0,   0,        0,         AHB1_CFG,   6,          2,          0,          0,          0,          0,          0,         0,            0,            0,            0,           0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(ahb1,    AHB1_CFG,   12,        2,         AHB1_CFG,   0,          0,          4,          2,          0,          0,          0,         0,            0,            0,            0,           0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(apb1,    0,           0,        0,         AHB1_CFG,   0,          0,          8,          2,          0,          0,          0,         0,            0,            0,            0,           0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(apb2,    APB2_CFG,   24,        2,         APB2_CFG,   0,          5,          16,         2,          0,          0,          0,         0,            0,            0,            0,           0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(nand,    NAND_CFG,   24,        2,         NAND_CFG,   0,          4,          16,         2,          0,          NAND_CFG,   BUS_RST0, BUS_GATE0,   0,           31,           13,          13,              0,               &clk_lock, NULL,             0);

SUNXI_CLK_PERIPH(sdmmc0_mod, SD0_CFG,	24,      2,			SD0_CFG,	0,         4,          16,         2,          0,          SD0_CFG,		0,			0,			0,			31,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc0_bus, 0,			0,       0,			0,			0,         0,          0,          0,          0,          0,			0,			BUS_GATE0,	0,			0,          0,          8,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc0_rst, 0,			0,       0,			0,			0,         0,          0,          0,          0,          0,			BUS_RST0,	0,			0,			0,          8,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc1_mod, SD1_CFG,	24,      2,			SD1_CFG,	0,         4,          16,         2,          0,          SD1_CFG,		0,			0,			0,			31,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc1_bus, 0,			0,       0,			0,			0,         0,          0,          0,          0,          0,			0,			BUS_GATE0,	0,			0,          0,          9,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc1_rst, 0,			0,       0,			0,			0,         0,          0,          0,          0,          0,			BUS_RST0,	0,			0,			0,          9,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc2_mode, SD2_CFG,	24,      2,			SD2_CFG,	30,         1,          0,          0,          0,          0,          0,			0,			0,			0,			0,           0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc2_module,  0,		0,       0,			SD2_CFG,	0,			4,         16,          2,          0,          SD2_CFG,    0,			0,			0,			31,			0,           0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc2_bus, 0,			0,       0,			0,			0,         0,          0,          0,          0,          0,			0,			BUS_GATE0,	0,			0,          0,          10,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc2_rst, 0,			0,       0,			0,			0,         0,          0,          0,          0,          0,			BUS_RST0,	0,			0,			0,          10,         0,              0,             &clk_lock, NULL,             0);

SUNXI_CLK_PERIPH(ss,      SS_CFG,     24,        2,         SS_CFG,     0,          4,          16,         2,          0,          SS_CFG,     BUS_RST0, BUS_GATE0,   0,           31,            5,           5,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(spi0,    SPI0_CFG,   24,        2,         SPI0_CFG,   0,          4,          16,         2,          0,          SPI0_CFG,   BUS_RST0, BUS_GATE0,   0,           31,           20,          20,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(spi1,    SPI1_CFG,   24,        2,         SPI1_CFG,   0,          4,          16,         2,          0,          SPI1_CFG,   BUS_RST0, BUS_GATE0,   0,           31,           21,          21,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(i2s0,    I2S0_CFG,   16,        2,         0,          0,          0,          0,          0,          0,          I2S0_CFG,   BUS_RST3,  BUS_GATE2,    0,           31,           12,          12,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(i2s1,    I2S1_CFG,   16,        2,         0,          0,          0,          0,          0,          0,          I2S1_CFG,   BUS_RST3,  BUS_GATE2,    0,           31,           13,          13,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbohci, 0,           0,        0,         0,          0,          0,          0,          0,          0,          USB_CFG,    BUS_RST0, BUS_GATE0,   0,           16,           29,          29,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbhsic, 0,           0,        0,         0,          0,          0,          0,          0,          0,          USB_CFG,    USB_CFG,   USB_CFG,      0,           10,            2,          11,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbehci, 0,           0,        0,         0,          0,          0,          0,          0,          0,          0,          BUS_RST0, BUS_GATE0,   0,            0,           26,          26,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbotg,  0,           0,        0,         0,          0,          0,          0,          0,          0,          0,          BUS_RST0, BUS_GATE0,   0,            0,           24,          24,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbphy0, 0,           0,        0,         0,          0,          0,          0,          0,          0,          USB_CFG,    USB_CFG,   0,            0,            8,            0,           0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbphy1, 0,           0,        0,         0,          0,          0,          0,          0,          0,          USB_CFG,    USB_CFG,   0,            0,            9,            1,           0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(debe0,   BE_CFG,     24,        3,         BE_CFG,     0,          4,          0,          0,          0,          BE_CFG,     BUS_RST1, BUS_GATE1,   DRAM_GATE,   31,           12,          12,             26,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(defe0,   FE_CFG,     24,        3,         FE_CFG,     0,          4,          0,          0,          0,          FE_CFG,     BUS_RST1, BUS_GATE1,   DRAM_GATE,   31,           14,          14,             24,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(lcd0ch0, LCD_CH0,    24,        3,         0,          0,          0,          0,          0,          0,          LCD_CH0,    BUS_RST1, BUS_GATE1,   0,           31,            4,           4,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(csi_s,   CSI_CFG,    24,        3,         CSI_CFG,    16,         4,          0,          0,          0,          CSI_CFG,    BUS_RST1,  BUS_GATE1,  DRAM_GATE,   31,            8,           8,              1,               &clk_lock, &com_gates[0],    0);
SUNXI_CLK_PERIPH(csi_m,   CSI_CFG,     8,        3,         CSI_CFG,    0,          4,          0,          0,          0,          CSI_CFG,    BUS_RST1,  BUS_GATE1,  DRAM_GATE,   15,            8,           8,              1,               &clk_lock, &com_gates[0],    1);
SUNXI_CLK_PERIPH(ve,      0,           0,        0,         VE_CFG,     16,         3,          0,          0,          0,          VE_CFG,     BUS_RST1,  BUS_GATE1,  DRAM_GATE,   31,            0,           0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(lvds,    0,           0,        0,         0,          0,          0,          0,          0,          0,          0,          BUS_RST2,  0,           0,            0,            0,           0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(adda,    0,           0,        0,         0,          0,          0,          0,          0,          0,          ADDA_CFG,   BUS_RST3,   BUS_GATE2,   0,           31,            0,           0,              0,               &clk_lock, &com_gates[1],    0);
SUNXI_CLK_PERIPH(addax4,  0,           0,        0,         0,          0,          0,          0,          0,          0,          ADDA_CFG,   BUS_RST3,   BUS_GATE2,   0,           30,            0,           0,              0,               &clk_lock, &com_gates[1],    1);
SUNXI_CLK_PERIPH(avs,     0,           0,        0,         0,          0,          0,          0,          0,          0,          AVS_CFG,    0,          0,           0,           31,            0,           0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(mbus,    MBUS_CFG,   24,        2,         MBUS_CFG,   0,          3,          0,          0,          0,          MBUS_CFG,   0,          0,           0,           31,            0,           0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(mipidsi, MIPI_DSI,   24,        2,         MIPI_DSI,   16,         4,          0,          0,          0,          MIPI_DSI,   BUS_RST0,  BUS_GATE0,  0,           31,            1,           1,              0,               &clk_lock, &com_gates[2],    0);
SUNXI_CLK_PERIPH(mipidphy, MIPI_DSI,    8,        2,         MIPI_DSI,   0,          4,          0,          0,          0,          MIPI_DSI,   BUS_RST0,  BUS_GATE0,  0,           15,            1,           1,              0,               &clk_lock, &com_gates[2],    1);
SUNXI_CLK_PERIPH(sat,     0,           0,        0,         0,          0,          0,          0,          0,          0,          0,          BUS_RST1,  BUS_GATE1,  0,            0,           26,          26,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(drc0,    DRC_CFG,    24,        3,         DRC_CFG,    0,          4,          0,          0,          0,          DRC_CFG,    BUS_RST1,  BUS_GATE1,  DRAM_GATE,   31,           25,          25,             16,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(gpu,     0,           0,        0,         GPU_CFG,    0,          3,          0,          0,          0,          GPU_CFG,    BUS_RST1,  BUS_GATE1,  0,           31,           20,          20,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(ats,     ATS_CFG,    24,        2,         ATS_CFG,    0,          3,          0,          0,          0,          ATS_CFG,    0,          0,           0,           31,            0,           0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(spinlock, 0,           0,        0,         0,          0,          0,          0,          0,          0,          0,          BUS_RST1,  BUS_GATE1,  0,            0,           22,          22,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(msgbox,  0,           0,        0,         0,          0,          0,          0,          0,          0,          0,          BUS_RST1,  BUS_GATE1,  0,            0,           21,          21,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(dma,     0,           0,        0,         0,          0,          0,          0,          0,          0,          0,          BUS_RST0,  BUS_GATE0,  0,            0,            6,           6,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(pio,     0,           0,        0,         0,          0,          0,          0,          0,          0,          0,          0,          BUS_GATE2,   0,            0,            0,           5,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(twi0,    0,           0,        0,         0,          0,          0,          0,          0,          0,          0,          BUS_RST4,   BUS_GATE3,   0,            0,            0,           0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(twi1,    0,           0,        0,         0,          0,          0,          0,          0,          0,          0,          BUS_RST4,   BUS_GATE3,   0,            0,            1,           1,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(twi2,    0,           0,        0,         0,          0,          0,          0,          0,          0,          0,          BUS_RST4,   BUS_GATE3,   0,            0,            2,           2,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(uart0,   0,           0,        0,         0,          0,          0,          0,          0,          0,          0,          BUS_RST4,   BUS_GATE3,   0,            0,           16,          16,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(uart1,   0,           0,        0,         0,          0,          0,          0,          0,          0,          0,          BUS_RST4,   BUS_GATE3,   0,            0,           17,          17,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(uart2,   0,           0,        0,         0,          0,          0,          0,          0,          0,          0,          BUS_RST4,   BUS_GATE3,   0,            0,           18,          18,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(uart3,   0,           0,        0,         0,          0,          0,          0,          0,          0,          0,          BUS_RST4,   BUS_GATE3,   0,            0,           19,          19,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(uart4,   0,           0,        0,         0,          0,          0,          0,          0,          0,          0,          BUS_RST4,   BUS_GATE3,   0,            0,           20,          20,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(pll_periphcpus,  0,   0,        0,         CPUS_CFG,   8,          5,          0,          0,          0,          0,          0,         0,            0,            0,            0,           0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpurcpus, CPUS_CFG,   16,       2,         CPUS_CFG,   0,          0,           4,         2,          0,          0,				0,  	0, 			0,     		   0,           0,            0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpurahbs, 0,           0,       0,         0,          0,          0,           0,         0,          0,          0,				0,  	0, 			0,     		   0,           0,            0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpurapbs, 0,           0,       0,         CPUS_APB0,  0,          2,           0,         0,          0,          0,				0,  	0, 			0,     		   0,           0,            0,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpuruart, 0,           0,       0,         0,          0,          0,           0,         0,          0,          0,			CPUS_APB0_RST,  CPUS_APB0_GATE, 0,     0,           4,            4,              0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpurpio,  0,           0,       0,			0,			0,			0,          0,          0,          0,          0,			CPUS_APB0_GATE,  0,		0,             0,          0,          0,                 0,               &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(losc_out, 0,           0,        0,         0,          0,          0,          0,          0,          0,          0,          0,          LOSC_OUT_GATE, 0,           0,            0,           0,              0,               &clk_lock, NULL,             0);


struct periph_init_data sunxi_periphs_init[] = {
	{"cpu",      CLK_GET_RATE_NOCACHE,	cpu_parents,      ARRAY_SIZE(cpu_parents),      &sunxi_clk_periph_cpu},
	{"axi",      0,						axi_parents,      ARRAY_SIZE(axi_parents),      &sunxi_clk_periph_axi},
	{"pll_periphahb1", CLK_IGNORE_SYNCBOOT,	pll_periphahb1_parents,  ARRAY_SIZE(pll_periphahb1_parents),  &sunxi_clk_periph_pll_periphahb1},
	{"ahb1",     0,						ahb1_parents,     ARRAY_SIZE(ahb1_parents),     &sunxi_clk_periph_ahb1},
	{"apb1",     0,						apb1_parents,     ARRAY_SIZE(apb1_parents),     &sunxi_clk_periph_apb1},
	{"apb2",     0,						apb2_parents,     ARRAY_SIZE(apb2_parents),     &sunxi_clk_periph_apb2},
	{"nand",     0,						perph1_parents,   ARRAY_SIZE(perph1_parents),   &sunxi_clk_periph_nand},
	{"sdmmc0_mod",     0,               perph1_parents,   ARRAY_SIZE(perph1_parents),   &sunxi_clk_periph_sdmmc0_mod},
	{"sdmmc0_bus",     0,               perph1_parents,   ARRAY_SIZE(perph1_parents),   &sunxi_clk_periph_sdmmc0_bus},
	{"sdmmc0_rst",     0,               perph1_parents,   ARRAY_SIZE(perph1_parents),   &sunxi_clk_periph_sdmmc0_rst},
	{"sdmmc1_mod",     0,               perph1_parents,   ARRAY_SIZE(perph1_parents),   &sunxi_clk_periph_sdmmc1_mod},
	{"sdmmc1_bus",     0,               perph1_parents,   ARRAY_SIZE(perph1_parents),   &sunxi_clk_periph_sdmmc1_bus},
	{"sdmmc1_rst",     0,               perph1_parents,   ARRAY_SIZE(perph1_parents),   &sunxi_clk_periph_sdmmc1_rst},
	{"sdmmc2_mode",    0,               perph1_parents,   ARRAY_SIZE(perph1_parents),   &sunxi_clk_periph_sdmmc2_mode},
	{"sdmmc2_module",  0,               sdmmc2_parents,   ARRAY_SIZE(sdmmc2_parents),   &sunxi_clk_periph_sdmmc2_module},
	{"sdmmc2_bus",     0,               sdmmc2_parents,   ARRAY_SIZE(sdmmc2_parents),   &sunxi_clk_periph_sdmmc2_bus},
	{"sdmmc2_rst",     0,               sdmmc2_parents,   ARRAY_SIZE(sdmmc2_parents),   &sunxi_clk_periph_sdmmc2_rst},
	{"ss",       0,						perph1_parents,   ARRAY_SIZE(perph1_parents),   &sunxi_clk_periph_ss},
	{"spi0",     0,						perph1_parents,   ARRAY_SIZE(perph1_parents),   &sunxi_clk_periph_spi0},
	{"spi1",     0,						perph1_parents,   ARRAY_SIZE(perph1_parents),   &sunxi_clk_periph_spi1},
	{"i2s0",     0,						i2s_parents,      ARRAY_SIZE(i2s_parents),      &sunxi_clk_periph_i2s0},
	{"i2s1",     0,						i2s_parents,      ARRAY_SIZE(i2s_parents),      &sunxi_clk_periph_i2s1},
	{"usbohci",  0,						ahb1mod_parents,  ARRAY_SIZE(ahb1mod_parents),  &sunxi_clk_periph_usbohci},
	{"usbhsic",  0,						hsic_parents,     ARRAY_SIZE(hsic_parents),     &sunxi_clk_periph_usbhsic},
	{"usbehci",  0,						ahb1mod_parents,  ARRAY_SIZE(ahb1mod_parents),  &sunxi_clk_periph_usbehci},
	{"usbotg",   0,						ahb1mod_parents,  ARRAY_SIZE(ahb1mod_parents),  &sunxi_clk_periph_usbotg},
	{"usbphy0",  0,						hosc_parents,     ARRAY_SIZE(hosc_parents),     &sunxi_clk_periph_usbphy0},
	{"usbphy1",  0,						hosc_parents,     ARRAY_SIZE(hosc_parents),     &sunxi_clk_periph_usbphy1},
	{"debe0",    0,						display_parents,  ARRAY_SIZE(display_parents),  &sunxi_clk_periph_debe0},
	{"defe0",    0,						display_parents,  ARRAY_SIZE(display_parents),  &sunxi_clk_periph_defe0},
	{"lcd0ch0",  0,						lcd0_parents,     ARRAY_SIZE(lcd0_parents),     &sunxi_clk_periph_lcd0ch0},
	{"csi_s",    0,						csi_s_parents,    ARRAY_SIZE(csi_s_parents),    &sunxi_clk_periph_csi_s},
	{"csi_m",    0,						csi_m_parents,    ARRAY_SIZE(csi_m_parents),    &sunxi_clk_periph_csi_m},
	{"ve",       0,						ve_parents,       ARRAY_SIZE(ve_parents),       &sunxi_clk_periph_ve},
	{"lvds",     0,						lvds_parents,     ARRAY_SIZE(lvds_parents),     &sunxi_clk_periph_lvds},
	{"adda",   0,						adda_parents,     ARRAY_SIZE(adda_parents),     &sunxi_clk_periph_adda},
	{"addax4",   0,						addax4_parents,   ARRAY_SIZE(addax4_parents),   &sunxi_clk_periph_addax4},
	{"avs",      0,						hosc_parents,     ARRAY_SIZE(hosc_parents),     &sunxi_clk_periph_avs},
	{"mbus",     0,						mbus_parents,     ARRAY_SIZE(mbus_parents),     &sunxi_clk_periph_mbus},
	{"mipidsi",  0,						mipidsi_parents,  ARRAY_SIZE(mipidsi_parents),  &sunxi_clk_periph_mipidsi},
	{"mipidphy", 0,						mipidphy_parents, ARRAY_SIZE(mipidphy_parents), &sunxi_clk_periph_mipidphy},
	{"sat",      0,						ahb1mod_parents,  ARRAY_SIZE(ahb1mod_parents),  &sunxi_clk_periph_sat},
	{"drc0",     0,						display_parents,  ARRAY_SIZE(display_parents),  &sunxi_clk_periph_drc0},
	{"gpu",      0,						gpu_parents,      ARRAY_SIZE(gpu_parents),      &sunxi_clk_periph_gpu},
	{"ats",      0,						perph1_parents,   ARRAY_SIZE(perph1_parents),   &sunxi_clk_periph_ats},
	{"spinlock", 0,						ahb1mod_parents,  ARRAY_SIZE(ahb1mod_parents),  &sunxi_clk_periph_spinlock},
	{"msgbox",   0,						ahb1mod_parents,  ARRAY_SIZE(ahb1mod_parents),  &sunxi_clk_periph_msgbox},
	{"dma",      0,						ahb1mod_parents,  ARRAY_SIZE(ahb1mod_parents),  &sunxi_clk_periph_dma},
	{"pio",      0,						apb1mod_parents,  ARRAY_SIZE(apb1mod_parents),  &sunxi_clk_periph_pio},
	{"twi0",     0,						apb2mod_parents,  ARRAY_SIZE(apb2mod_parents),  &sunxi_clk_periph_twi0},
	{"twi1",     0,						apb2mod_parents,  ARRAY_SIZE(apb2mod_parents),  &sunxi_clk_periph_twi1},
	{"twi2",     0,						apb2mod_parents,  ARRAY_SIZE(apb2mod_parents),  &sunxi_clk_periph_twi2},
	{"uart0",    0,						apb2mod_parents,  ARRAY_SIZE(apb2mod_parents),  &sunxi_clk_periph_uart0},
	{"uart1",    0,						apb2mod_parents,  ARRAY_SIZE(apb2mod_parents),  &sunxi_clk_periph_uart1},
	{"uart2",    0,						apb2mod_parents,  ARRAY_SIZE(apb2mod_parents),  &sunxi_clk_periph_uart2},
	{"uart3",    0,						apb2mod_parents,  ARRAY_SIZE(apb2mod_parents),  &sunxi_clk_periph_uart3},
	{"uart4",    0,						apb2mod_parents,  ARRAY_SIZE(apb2mod_parents),  &sunxi_clk_periph_uart4},
	/*{"losc_out",	0,						losc_parents,     ARRAY_SIZE(losc_parents),     &sunxi_clk_periph_losc_out},   */
};

static struct periph_init_data sunxi_periphs_cpus_init[] = {
	{"pll_periphcpus",	CLK_GET_RATE_NOCACHE|CLK_READONLY, pll_periphcpus_parents,	ARRAY_SIZE(pll_periphcpus_parents),	&sunxi_clk_periph_pll_periphcpus},
	{"cpurcpus",	CLK_GET_RATE_NOCACHE|CLK_READONLY, cpurcpus_parents,	ARRAY_SIZE(cpurcpus_parents),	&sunxi_clk_periph_cpurcpus},
	{"cpurahbs",	CLK_GET_RATE_NOCACHE|CLK_READONLY, cpurahbs_parents,	ARRAY_SIZE(cpurahbs_parents),	&sunxi_clk_periph_cpurahbs},
	{"cpurapbs",	CLK_GET_RATE_NOCACHE|CLK_READONLY, cpurapbs_parents,	ARRAY_SIZE(cpurapbs_parents),	&sunxi_clk_periph_cpurapbs},
	{"cpuruart"   ,	CLK_GET_RATE_NOCACHE,              cpuruart_parents,	ARRAY_SIZE(cpuruart_parents),	&sunxi_clk_periph_cpuruart},
	{"cpurpio",        0,                          	cpurpio_parents,        ARRAY_SIZE(cpurpio_parents),        &sunxi_clk_periph_cpurpio},
	{"losc_out",		0,							losc_parents,			ARRAY_SIZE(losc_parents),		&sunxi_clk_periph_losc_out},
};

void  init_clocks(void)
{
	int     i;
	//struct clk *clk;
	struct factor_init_data *factor;
	struct periph_init_data *periph;

	/* get clk register base address */
	sunxi_clk_base = (void*)0x01c20000; //fixed base address.
//	sunxi_clk_cpus_base = (void*)0x01f01400; //fixed base address.
	sunxi_clk_factor_initlimits();

	sunxi_clk_get_factors_ops(&mipi_ops);
	mipi_ops.get_parent = get_parent_mipi;
	mipi_ops.set_parent = set_parent_mipi;
	mipi_ops.enable = clk_enable_pll_mipi;
	mipi_ops.disable = clk_disable_pll_mipi;

	clk_register_fixed_rate(NULL, "hosc", NULL, CLK_IS_ROOT, 24000000);
	//clk_register_fixed_rate(NULL, "pll_24m", NULL, CLK_IS_ROOT, 24000000);

	/* register normal factors, based on sunxi factor framework */
	for(i=0; i<ARRAY_SIZE(sunxi_factos); i++) {
		factor = &sunxi_factos[i];
		factor->priv_regops = NULL;
		sunxi_clk_register_factors(NULL, (void*)sunxi_clk_base, (struct factor_init_data*)factor);
	}

	/* register periph clock */
	for(i=0; i<ARRAY_SIZE(sunxi_periphs_init); i++) {
		periph = &sunxi_periphs_init[i];
		periph->periph->priv_regops = NULL;
		sunxi_clk_register_periph(periph, sunxi_clk_base);
	}

	for(i=0; i<ARRAY_SIZE(sunxi_periphs_cpus_init); i++) {
		periph = &sunxi_periphs_cpus_init[i];
		periph->periph->priv_regops = NULL;
#if 0
		sunxi_clk_register_periph(periph, sunxi_clk_cpus_base);
#endif
	}

	printk("%s: finish init_clocks.\n",__func__);
}

#ifdef CONFIG_OF
/**
 * set default rate for clk
 */
static int __set_clk_rates(struct device_node *node, struct clk *clk)
{
	u32 assigned_clock_rates = 0;
	bool res = -1;

	/*set pll default rate here , and make you know it is setted
	 * succeed or not
	 */
	if (!of_property_read_u32(node, "assigned-clock-rates",
		&assigned_clock_rates)) {
		u32 real_clock_rate = 0;

		clk_set_rate(clk, assigned_clock_rates);
		real_clock_rate = clk_get_rate(clk);
		if (real_clock_rate != assigned_clock_rates) {
			pr_info("%s-set_default_rate=%u, but real_get_rate=%u failured!\n",
				__clk_get_name(clk),
				assigned_clock_rates,
				real_clock_rate);
		} else {
			pr_info("%s-set_default_rate=%u success!\n",
				__clk_get_name(clk),
				assigned_clock_rates);
			res = 0;
		}
	}

	return res;
}

/**
 * set default clk source for clk
 */
static int __set_clk_parents(struct device_node *node, struct clk *clk)
{
	int index = 0, rc;
	struct of_phandle_args clkspec;
	struct clk *pclk;

	rc = of_parse_phandle_with_args(node, "assigned-clock-parents",
				"#clock-cells", index, &clkspec);
	if (rc < 0) {
		/* skip empty (null) phandles */
		return rc;
	}

	pclk = of_clk_get_from_provider(&clkspec);
	if (IS_ERR(pclk)) {
		pr_warn("clk: couldn't get parent clock %d for %s\n",
				index, node->full_name);
		return PTR_ERR(pclk);
	}

	rc = clk_set_parent(clk, pclk);
	if (rc < 0) {
		pr_err("%s-set_default_source=%s failed at: %d\n",
			__clk_get_name(clk), __clk_get_name(pclk), rc);
	} else {
		pr_info("%s-set_default_source=%s success!\n",
			__clk_get_name(clk), __clk_get_name(pclk));
	}

	return rc;
}

/*
 * of_sunxi_clocks_init() - Clocks initialize
 */
void of_sunxi_clocks_init(struct device_node *node)
{
	sunxi_clk_base = of_iomap(node, 0);
	sunxi_clk_cpus_base = of_iomap(node, 1);
	sunxi_clk_periph_losc_out.gate.bus = of_iomap(node, 2);
	/*do some initialize arguments here*/
	sunxi_clk_factor_initlimits();

	sunxi_clk_get_factors_ops(&mipi_ops);
	mipi_ops.get_parent = get_parent_mipi;
	mipi_ops.set_parent = set_parent_mipi;
	mipi_ops.enable = clk_enable_pll_mipi;
	mipi_ops.disable = clk_disable_pll_mipi;
}

/*
 * of_sunxi_fixed_clk_setup() - Setup function for fixed clk
 */
void of_sunxi_fixed_clk_setup(struct device_node *node)
{
	struct clk *clk;
	const char *clk_name = node->name;
	u32 rate;

	if (of_property_read_u32(node, "clock-frequency", &rate))
		return;

	of_property_read_string(node, "clock-output-names", &clk_name);

	clk = clk_register_fixed_rate(NULL, clk_name, NULL,
					 CLK_IS_ROOT, rate);
	if (!IS_ERR(clk)) {
		clk_register_clkdev(clk, clk_name, NULL);
		of_clk_add_provider(node, of_clk_src_simple_get, clk);
	}
}

/*
 * of_sunxi_fixed_factor_clk_setup() - Setup function for fixed factors clk
 */
void of_sunxi_fixed_factor_clk_setup(struct device_node *node)
{
	struct clk *clk;
	const char *clk_name = node->name;
	const char *parent_name;
	u32 div, mult;

	if (of_property_read_u32(node, "clock-div", &div)) {
		pr_err("%s Fixed factor clock <%s> must have a clock-div property\n",
			__func__, node->name);
		return;
	}

	if (of_property_read_u32(node, "clock-mult", &mult)) {
		pr_err("%s Fixed factor clock <%s> must have a clokc-mult property\n",
			__func__, node->name);
		return;
	}

	of_property_read_string(node, "clock-output-names", &clk_name);
	parent_name = of_clk_get_parent_name(node, 0);

	clk = clk_register_fixed_factor(NULL, clk_name, parent_name, 0,
					mult, div);
	if (!IS_ERR(clk)) {
		clk_register_clkdev(clk, clk_name, NULL);
		of_clk_add_provider(node, of_clk_src_simple_get, clk);
	}
}

/*
 * of_sunxi_pll_clk_setup() - Setup function for pll factors clk
 */
void of_sunxi_pll_clk_setup(struct device_node *node)
{
	struct clk *clk;
	const char *clk_name = node->name;
	const char *lock_mode = NULL;
	struct factor_init_data *factor;
	int i;
	int ret;

	of_property_read_string(node, "clock-output-names", &clk_name);
	ret = of_property_read_string(node, "lock-mode", &lock_mode);

	/*get pll clk init config */
	for (i = 0; i < ARRAY_SIZE(sunxi_factos); i++) {
		factor = &sunxi_factos[i];
		if (strcmp(clk_name, factor->name))
			continue;

		if (!strcmp(lock_mode, "new"))
			factor->lock_mode = PLL_LOCK_NEW_MODE;
		else if (!strcmp(lock_mode, "old"))
			factor->lock_mode = PLL_LOCK_OLD_MODE;
		else
			factor->lock_mode = PLL_LOCK_NONE_MODE;

		/*register clk */
		clk = sunxi_clk_register_factors(NULL,
				sunxi_clk_base, &clk_lock, factor);
		/*add to of */
		if (!IS_ERR(clk)) {
			clk_register_clkdev(clk, clk_name, NULL);
			of_clk_add_provider(node, of_clk_src_simple_get, clk);
			__set_clk_parents(node, clk);
			__set_clk_rates(node, clk);
			return;
		}
	}

	pr_err("clk %s not found in %s\n", clk_name, __func__);
}

/*
 * of_sunxi_periph_clk_setup() - Setup function for periph clk
 */
void of_sunxi_periph_clk_setup(struct device_node *node)
{
	struct clk *clk;
	const char *clk_name = node->name;
	struct periph_init_data *pd;
	unsigned int i;
	of_property_read_string(node, "clock-output-names", &clk_name);

	/*get pd clk init config */
	for (i = 0; i < ARRAY_SIZE(sunxi_periphs_init); i++) {
		pd = &sunxi_periphs_init[i];
		if (strcmp(clk_name, pd->name))
			continue;

		/*register clk */
		clk = sunxi_clk_register_periph(pd, sunxi_clk_base);
		/*add to of */
		if (!IS_ERR(clk)) {
			clk_register_clkdev(clk, clk_name, NULL);
			of_clk_add_provider(node, of_clk_src_simple_get, clk);
			__set_clk_parents(node, clk);
			__set_clk_rates(node, clk);
			return;
		}
	}
	pr_err("clk %s not found in %s\n", clk_name, __func__);
}

/*
 * of_periph_cpus_clk_setup() - Setup function for periph cpus clk
 */
struct sunxi_reg_ops priv_regops;
void of_sunxi_periph_cpus_clk_setup(struct device_node *node)
{
	struct clk *clk;
	const char *clk_name = node->name;
	struct periph_init_data *pd;
	unsigned int i;
	of_property_read_string(node, "clock-output-names", &clk_name);

	/*get periph cpus clk init config */
	for (i = 0; i < ARRAY_SIZE(sunxi_periphs_cpus_init); i++) {
		pd = &sunxi_periphs_cpus_init[i];
		if (strcmp(clk_name, pd->name))
			continue;
		/* register clk */
		clk = sunxi_clk_register_periph(pd,
				(strcmp(clk_name, "losc_out")
				 ? sunxi_clk_cpus_base
				 : 0));
		/* add to of */
		if (!IS_ERR(clk)) {
			clk_register_clkdev(clk, clk_name, NULL);
			of_clk_add_provider(node, of_clk_src_simple_get, clk);
			__set_clk_parents(node, clk);
			__set_clk_rates(node, clk);
			return;
		}
	}

	pr_err("clk %s not found in %s\n", clk_name, __func__);
}

CLK_OF_DECLARE(sunxi_clocks_init, "allwinner,sunxi-clk-init",
		 of_sunxi_clocks_init);
CLK_OF_DECLARE(sunxi_fixed_clk, "allwinner,fixed-clock",
		 of_sunxi_fixed_clk_setup);
CLK_OF_DECLARE(pll_clk, "allwinner,sunxi-pll-clock",
		 of_sunxi_pll_clk_setup);
CLK_OF_DECLARE(sunxi_fixed_factor_clk, "allwinner,fixed-factor-clock",
		 of_sunxi_fixed_factor_clk_setup);
CLK_OF_DECLARE(periph_clk, "allwinner,sunxi-periph-clock",
		 of_sunxi_periph_clk_setup);
CLK_OF_DECLARE(periph_cpus_clk, "allwinner,sunxi-periph-cpus-clock",
		 of_sunxi_periph_cpus_clk_setup);
#endif /* CONFIG_OF */
