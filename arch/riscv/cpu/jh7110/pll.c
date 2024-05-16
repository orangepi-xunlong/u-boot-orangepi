// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022-2023 StarFive Technology Co., Ltd.
 * Author:	yanhong <yanhong.wang@starfivetech.com>
 *		samin <samin.guo@starfivetech.com>
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clk.h>
#include <asm/arch/jh7110-regs.h>
#include <linux/bitops.h>
#include <log.h>

#define PLL0_DACPD_MASK		BIT(24)
#define PLL0_DSMPD_MASK		BIT(25)
#define PLL0_FBDIV_MASK		GENMASK(11, 0)
#define PLL0_FRAC_MASK		GENMASK(23, 0)
#define PLL0_PD_MASK		BIT(27)
#define PLL0_POSTDIV1_MASK	GENMASK(29, 28)
#define PLL0_PREDIV_MASK	GENMASK(5, 0)
#define PLL1_DACPD_MASK		BIT(15)
#define PLL1_DSMPD_MASK		BIT(16)
#define PLL1_FBDIV_MASK		GENMASK(28, 17)
#define PLL1_FRAC_MASK		GENMASK(23, 0)
#define PLL1_PD_MASK		BIT(27)
#define PLL1_POSTDIV1_MASK	GENMASK(29, 28)
#define PLL1_PREDIV_MASK	GENMASK(5, 0)
#define PLL2_DACPD_MASK		BIT(15)
#define PLL2_DSMPD_MASK		BIT(16)
#define PLL2_FBDIV_MASK		GENMASK(28, 17)
#define PLL2_FRAC_MASK		GENMASK(23, 0)
#define PLL2_PD_MASK		BIT(27)
#define PLL2_POSTDIV1_MASK	GENMASK(29, 28)
#define PLL2_PREDIV_MASK	GENMASK(5, 0)

#define PLL0_DACPD_OFFSET	0x18
#define PLL0_DSMPD_OFFSET	0x18
#define PLL0_FBDIV_OFFSET	0x1C
#define PLL0_FRAC_OFFSET	0x20
#define PLL0_PD_OFFSET		0x20
#define PLL0_POSTDIV1_OFFSET	0x20
#define PLL0_PREDIV_OFFSET	0x24
#define PLL1_DACPD_OFFSET	0x24
#define PLL1_DSMPD_OFFSET	0x24
#define PLL1_FBDIV_OFFSET	0x24
#define PLL1_FRAC_OFFSET	0x28
#define PLL1_PD_OFFSET		0x28
#define PLL1_POSTDIV1_OFFSET	0x28
#define PLL1_PREDIV_OFFSET	0x2c
#define PLL2_DACPD_OFFSET	0x2c
#define PLL2_DSMPD_OFFSET	0x2c
#define PLL2_FBDIV_OFFSET	0x2c
#define PLL2_FRAC_OFFSET	0x30
#define PLL2_PD_OFFSET		0x30
#define PLL2_POSTDIV1_OFFSET	0x30
#define PLL2_PREDIV_OFFSET	0x34

#define PLL_PD_OFF		1
#define PLL_PD_ON		0

struct starfive_pll_freq jh7110_pll0_freq[] = {
	{
		.freq = 375000000,
		.prediv = 8,
		.fbdiv = 125,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
	{
		.freq = 500000000,
		.prediv = 6,
		.fbdiv = 125,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
	{
		.freq = 625000000,
		.prediv = 24,
		.fbdiv = 625,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
	{
		.freq = 750000000,
		.prediv = 4,
		.fbdiv = 125,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
	{
		.freq = 875000000,
		.prediv = 24,
		.fbdiv = 875,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
	{
		.freq = 1000000000,
		.prediv = 3,
		.fbdiv = 125,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
	{
		.freq = 1250000000,
		.prediv = 12,
		.fbdiv = 625,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
	{
		.freq = 1375000000,
		.prediv = 24,
		.fbdiv = 1375,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
	{
		.freq = 1500000000,
		.prediv = 2,
		.fbdiv = 125,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
	{
		.freq = 1625000000,
		.prediv = 24,
		.fbdiv = 1625,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
	{
		.freq = 1750000000,
		.prediv = 12,
		.fbdiv = 875,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
	{
		.freq = 1800000000,
		.prediv = 3,
		.fbdiv = 225,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
};

struct starfive_pll_freq jh7110_pll1_freq[] = {
	{
		.freq = 1066000000,
		.prediv = 12,
		.fbdiv = 533,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
	{
		.freq = 1200000000,
		.prediv = 1,
		.fbdiv = 50,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
	{
		.freq = 1400000000,
		.prediv = 6,
		.fbdiv = 350,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
	{
		.freq = 1600000000,
		.prediv = 3,
		.fbdiv = 200,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
};

struct starfive_pll_freq jh7110_pll2_freq[] = {
	{
		.freq = 1228800000,
		.prediv = 15,
		.fbdiv = 768,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
	{
		.freq = 1188000000,
		.prediv = 2,
		.fbdiv = 99,
		.postdiv1 = 1,
		.dacpd = 1,
		.dsmpd = 1
	},
};

#define getbits_le32(addr, mask) ((in_le32(addr)&(mask)) >> __ffs(mask))

#define GET_PLL(index, type) \
	getbits_le32(SYS_SYSCON_BASE + index##_##type##_OFFSET, index##_##type##_MASK)

#define SET_PLL(index, type, val) \
	clrsetbits_le32(SYS_SYSCON_BASE + index##_##type##_OFFSET, \
		index##_##type##_MASK, \
		((val) << __ffs(index##_##type##_MASK)) & index##_##type##_MASK)

static void pll_set_rate(enum starfive_pll_type pll,
			struct starfive_pll_freq *freq)
{
	switch (pll) {
	case PLL0:
		SET_PLL(PLL0, PD, PLL_PD_OFF);
		SET_PLL(PLL0, DACPD, freq->dacpd);
		SET_PLL(PLL0, DSMPD, freq->dsmpd);
		SET_PLL(PLL0, PREDIV, freq->prediv);
		SET_PLL(PLL0, FBDIV, freq->fbdiv);
		SET_PLL(PLL0, POSTDIV1, freq->postdiv1 >> 1);
		SET_PLL(PLL0, PD, PLL_PD_ON);
		break;

	case PLL1:
		SET_PLL(PLL1, PD, PLL_PD_OFF);
		SET_PLL(PLL1, DACPD, freq->dacpd);
		SET_PLL(PLL1, DSMPD, freq->dsmpd);
		SET_PLL(PLL1, PREDIV, freq->prediv);
		SET_PLL(PLL1, FBDIV, freq->fbdiv);
		SET_PLL(PLL1, POSTDIV1, freq->postdiv1 >> 1);
		SET_PLL(PLL1, PD, PLL_PD_ON);
		break;

	case PLL2:
		SET_PLL(PLL2, PD, PLL_PD_OFF);
		SET_PLL(PLL2, DACPD, freq->dacpd);
		SET_PLL(PLL2, DSMPD, freq->dsmpd);
		SET_PLL(PLL2, PREDIV, freq->prediv);
		SET_PLL(PLL2, FBDIV, freq->fbdiv);
		SET_PLL(PLL2, POSTDIV1, freq->postdiv1 >> 1);
		SET_PLL(PLL2, PD, PLL_PD_ON);
		break;

	default:
		debug("Unsupported pll type %d.\n", pll);
		break;
	}
}

static u64 pll_get_rate(enum starfive_pll_type pll,
			struct starfive_pll_freq *conf)
{
	u32 dacpd, dsmpd;
	u32 prediv, fbdiv, postdiv1, frac;
	u64 refclk = 24000000;
	u64 freq = 0;
	u64 deffreq;

	switch (pll) {
	case PLL0:
		dacpd = GET_PLL(PLL0, DACPD);
		dsmpd = GET_PLL(PLL0, DSMPD);
		prediv = GET_PLL(PLL0, PREDIV);
		fbdiv = GET_PLL(PLL0, FBDIV);
		postdiv1 = 1 << GET_PLL(PLL0, POSTDIV1);
		frac = GET_PLL(PLL0, FRAC);
		deffreq = 1000000000;
		break;

	case PLL1:
		dacpd = GET_PLL(PLL1, DACPD);
		dsmpd = GET_PLL(PLL1, DSMPD);
		prediv = GET_PLL(PLL1, PREDIV);
		fbdiv = GET_PLL(PLL1, FBDIV);
		postdiv1 = 1 << GET_PLL(PLL1, POSTDIV1);
		frac = GET_PLL(PLL1, FRAC);
		deffreq = 1066000000;
		break;

	case PLL2:
		dacpd = GET_PLL(PLL2, DACPD);
		dsmpd = GET_PLL(PLL2, DSMPD);
		prediv = GET_PLL(PLL2, PREDIV);
		fbdiv = GET_PLL(PLL2, FBDIV);
		postdiv1 = 1 << GET_PLL(PLL2, POSTDIV1);
		frac = GET_PLL(PLL2, FRAC);
		deffreq = 1188000000;
		break;

	default:
		debug("Unsupported pll type %d.\n", pll);
		return 0;
	}

	if (dacpd == 1 && dsmpd == 1)
		freq = (refclk * fbdiv) / (prediv * postdiv1);
	else if ((dacpd == 0 && dsmpd == 0))
		freq = deffreq;
	else {
		debug("Unkwnon pll mode.\n");
		return -EINVAL;
	}

	if (conf) {
		conf->dacpd = dacpd;
		conf->dsmpd = dsmpd;
		conf->fbdiv = fbdiv;
		conf->prediv = prediv;
		conf->frac = frac;
		conf->postdiv1 = postdiv1;
		conf->freq = freq;
	}

	return freq;
}

u64 starfive_jh7110_pll_get_rate(enum starfive_pll_type pll,
				struct starfive_pll_freq *conf)
{
	return pll_get_rate(pll, conf);
}

int starfive_jh7110_pll_set_rate(enum starfive_pll_type pll, u64 rate)
{
	struct starfive_pll_freq *conf;
	struct starfive_pll_freq *freq;
	int i;
	int num;

	switch (pll) {
	case PLL0:
		conf = jh7110_pll0_freq;
		num = ARRAY_SIZE(jh7110_pll0_freq);
		break;

	case PLL1:
		conf = jh7110_pll1_freq;
		num = ARRAY_SIZE(jh7110_pll1_freq);
		break;

	case PLL2:
		conf = jh7110_pll2_freq;
		num = ARRAY_SIZE(jh7110_pll2_freq);
		break;

	default:
		debug("Unsupported pll type %d.\n", pll);
		return 0;
	}

	for (i = 0; i < num; i++) {
		if (conf[i].freq == rate) {
			freq = &conf[i];
			break;
		}
	}
	if (i < num)
		pll_set_rate(pll, freq);
	else
		debug("Unsupported freq %lld.\n", rate);

	return 0;
}

