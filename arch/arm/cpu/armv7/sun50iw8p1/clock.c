/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * zhouhuacai <zhouhuacai@allwinnertech.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/platform.h>
struct core_pll_freq_tbl {
    int FactorN;
    int FactorK;
    int FactorM;
    int FactorP;
    int pading;
};

static int clk_get_pll_para(struct core_pll_freq_tbl *factor, int rate);
int sunxi_clock_get_pll6(void);

int sunxi_clock_get_corepll(void)
{
	unsigned int reg_val = 0;
	int 	div_m = 0, div_p = 0;
	int   factor_n = 0;
	int 	clock;

	reg_val  = readl(CCMU_PLL_CPUX_CTRL_REG);
	div_p    = 1 << ((reg_val >> 16) & 0x3);
	factor_n = ((reg_val >> 8) & 0xff) + 1;
	div_m    = ((reg_val >> 0) & 0x3) + 1;

	clock = 24*factor_n/div_m/div_p;

	return clock;
}

int sunxi_clock_get_axi(void)
{
	unsigned int reg_val = 0;
	int src = 0, src_clock = 0;
	int factor = 0;
	int clock = 0;

	reg_val   = readl(CCMU_CPUX_AXI_CFG_REG);
	src = (reg_val >> 24) & 0x03;
	factor    = ((reg_val >> 0) & 0x03) + 1;

	switch (src) {
	case 0: /* OSC24M */
		src_clock = 24;
		break;
	case 1: /* CCMU_32K */
		src_clock = 32 / 1000;
		break;
	case 2: /* RC16M */
		src_clock = 16;
		break;
	case 3: /* PLL_CPUX */
		src_clock = sunxi_clock_get_corepll();
		break;
	default:
		return 0;
	}

	clock = src_clock/factor;

	return clock;
}

int sunxi_clock_get_ahb(void)
{
	unsigned int reg_val = 0;
	int factor_m = 0, factor_n = 0;
	int clock = 0;
	int src = 0, src_clock = 0;

	reg_val = readl(CCMU_PSI_AHB1_AHB2_CFG_REG);
	src = (reg_val >> 24) & 0x3;
	factor_m = ((reg_val >> 0) & 0x03) + 1;
	factor_n = 1 << ((reg_val >> 8) & 0x03);

	switch (src) {
	case 0: /* OSC24M */
		src_clock = 24;
		break;
	case 1: /* CCMU_32K */
		src_clock = 32 / 1000;
		break;
	case 2: /* RC16M */
		src_clock = 16;
		break;
	case 3: /* PLL_PERI0(1X) */
		src_clock = sunxi_clock_get_pll6();
		break;
	default:
		return 0;
	}

	clock = src_clock/factor_m/factor_n;

	return clock;
}

int sunxi_clock_get_apb1(void)
{
	unsigned int reg_val = 0;
	int src = 0, src_clock = 0;
	int clock = 0, factor_m = 0, factor_n = 0;

	reg_val = readl(CCMU_APB1_CFG_GREG);
	factor_m  = ((reg_val >> 0) & 0x03) + 1;
	factor_n  = 1 << ((reg_val >> 8) & 0x03);
	src = (reg_val >> 24) & 0x3;

	switch (src) {
	case 0: /* OSC24M */
		src_clock = 24;
		break;
		break;
	case 1: /* CCMU_32K */
		src_clock = 32 / 1000;
		break;
	case 2: /* PSI */
		src_clock = sunxi_clock_get_ahb();
		break;
	case 3: /* PLL_PERI0(1X) */
		src_clock = sunxi_clock_get_pll6();
		break;
	default:
		return 0;
	}

	clock = src_clock/factor_m/factor_n;

	return clock;
}

int sunxi_clock_get_apb2(void)
{
	unsigned int reg_val = 0;
	int clock = 0, factor_m = 0, factor_n = 0;
	int src = 0, src_clock = 0;

	reg_val = readl(CCMU_APB2_CFG_GREG);
	src = (reg_val >> 24) & 0x3;
	factor_m  = ((reg_val >> 0) & 0x03) + 1;
	factor_n  = 1 << ((reg_val >> 8) & 0x03);

	switch (src) {
	case 0: /* OSC24M */
		src_clock = 24;
		break;
	case 1: /* CCMU_32K */
		src_clock = 32 / 1000;
		break;
	case 2: /* PSI */
		src_clock = sunxi_clock_get_ahb();
		break;
	case 3: /* PSI */
		src_clock = sunxi_clock_get_pll6();
		break;
	default:
		return 0;
	}

	clock = src_clock/factor_m/factor_n;

	return clock;

}


int sunxi_clock_get_mbus(void)
{
	unsigned int reg_val = 0;
	unsigned int src = 0, clock = 0, div = 0;
	reg_val = readl(CCMU_MBUS_CFG_REG);

	/*get src*/
	src = (reg_val >> 24) & 0x3;
	/*get div M, the divided clock is divided by M+1*/
	div = (reg_val & 0x3) + 1;

	switch (src) {
	case 0: /* src is OSC24M */
		clock = 24;
		break;
	case 1: /* src is   pll_periph0(2x) */
		clock = sunxi_clock_get_pll6() * 2;
		break;
	case 2: /* src is pll_ddr0  --not set in boot */
		clock = 0;
		break;
	case 3: /* src is pll_ddr1 --not set in boot */
		clock = 0;
		break;
	default:
		return 0;
	}

	clock = clock / div;

	return clock;
}

static int clk_get_pll_para(struct core_pll_freq_tbl *factor, int pll_clk)
{
	int index;

	index = pll_clk / 24;
	factor->FactorP = 0;
	factor->FactorN = (index - 1);
	factor->FactorM = 0;

	return 0;
}

int sunxi_clock_set_corepll(int frequency, int core_vol)
{
	unsigned int reg_val = 0;
	struct core_pll_freq_tbl  pll_factor;

	if (!frequency) {
		frequency = 408;
	} else if (frequency < 200) {
		printf("the core frequency must more than 200M\n");
		frequency = 200;
	} else if (frequency > 3072) {
		printf("the core frequency must less than 3072M\n");
		frequency = 3072;
	}

	/* switch to 24M*/
	reg_val = readl(CCMU_CPUX_AXI_CFG_REG);
	reg_val &= ~(0x03 << 24);
	writel(reg_val, CCMU_CPUX_AXI_CFG_REG);
	__udelay(20);

	/*pll output disable*/
	reg_val = readl(CCMU_PLL_CPUX_CTRL_REG);
	reg_val &= ~(0x01 << 27);
	writel(reg_val, CCMU_PLL_CPUX_CTRL_REG);

	/*get config para form freq table*/
	clk_get_pll_para(&pll_factor, frequency);

	reg_val = readl(CCMU_PLL_CPUX_CTRL_REG);
	reg_val &= ~((0x03 << 16) | (0xff << 8)  | (0x03 << 0));
	reg_val |=  (pll_factor.FactorP << 16) | (pll_factor.FactorN << 8) | (pll_factor.FactorM << 0) ;
	writel(reg_val, CCMU_PLL_CPUX_CTRL_REG);
	__udelay(20);

	/*enable lock*/
	reg_val = readl(CCMU_PLL_CPUX_CTRL_REG);
	reg_val |=  (0x1 << 29);
	writel(reg_val, CCMU_PLL_CPUX_CTRL_REG);
#ifndef FPGA_PLATFORM
	do {
		reg_val = readl(CCMU_PLL_CPUX_CTRL_REG);
	} while (!(reg_val & (0x1 << 28)));
#endif

	/*enable pll output*/
	reg_val = readl(CCMU_PLL_CPUX_CTRL_REG);
	reg_val |=  (0x1 << 27);
	writel(reg_val, CCMU_PLL_CPUX_CTRL_REG);

	/* switch clk src to COREPLL*/
	reg_val = readl(CCMU_CPUX_AXI_CFG_REG);
	reg_val &= ~(0x03 << 24);
	reg_val |=  (0x03 << 24);
	writel(reg_val, CCMU_CPUX_AXI_CFG_REG);

	return  0;
}

int sunxi_clock_get_pll6(void)
{
	unsigned int reg_val = 0;
	int factor_n = 0, factor_m0 = 0, factor_m1 = 0;
	int pll6 = 0;

	reg_val = readl(CCMU_PLL_PERI0_CTRL_REG);
	factor_n = ((reg_val >> 8) & 0xff) + 1;
	factor_m0 = ((reg_val >> 0) & 0x01) + 1;
	factor_m1 = ((reg_val >> 1) & 0x01) + 1;

	pll6 = 24 * factor_n/factor_m0/factor_m1/2;

	return pll6;
}
