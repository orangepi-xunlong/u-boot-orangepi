/*
 * (C) Copyright 2017-2020
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *zhouhuacai <zhouhuacai@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/platform.h>

typedef struct core_pll_freq_tbl {
	int pll_value;
	int FactorN;
	int FactorM;
	int FactorP;
} PLL_TABLE;

/* pll = 24M*(N+1)/(M+1)/(P+1) */
static PLL_TABLE pll1_table[] = {
	/*pll     N    M   P*/
	{408,	  16,  0,  0},
	{1008,    41,  0,  0},
	{1440,    59,  0,  0},
};


static int clk_get_pll_para(PLL_TABLE *factor, int pll_clk)
{
	int i, size;
	PLL_TABLE *target_factor;

	size = ARRAY_SIZE(pll1_table);
	for (i = 0; i < size; i++) {
		if (pll1_table[i].pll_value == pll_clk)
			break;
	}
	if (i >= size) {
		i = 0;
		printf("cant find pll setting(%dM) from  pll table,use default(%dM)\n",
		       pll_clk, pll1_table[0].pll_value);
	}
	target_factor = &pll1_table[i];

	factor->FactorN = target_factor->FactorN;
	factor->FactorM = target_factor->FactorM;
	factor->FactorP = target_factor->FactorP;

	return 0;
}

int sunxi_clock_get_pll6(void)
{
	unsigned int reg_val;
	int factor_n, factor_m0, factor_m1, pll6;

	reg_val = readl(CCMU_PLL_PERI0_CTRL_REG);
	factor_n = ((reg_val >> 8) & 0xff) + 1;
	factor_m1 = ((reg_val >> 1) & 0x01) + 1;
	factor_m0 = ((reg_val >> 0) & 0x01) + 1;
	pll6 = 24 * factor_n / factor_m0 / factor_m1 / 2;

	return pll6;
}

int sunxi_clock_get_corepll(void)
{
	unsigned int reg_val;
	int 	div_m, div_p;
	int 	factor_n;
	int 	clock, clock_src;

	reg_val   = readl(CCMU_CPUX_AXI_CFG_REG);
	clock_src = (reg_val >> 24) & 0x03;

	switch (clock_src) {
	case 0:/*OSC24M  */
		clock = 24;
		break;
	case 1:/*RTC32K  */
		clock = 32 / 1000 ;
		break;
	case 2:/*RC16M  */
		clock = 16;
		break;
	case 3:/*PLL_CPUX  */
		reg_val  = readl(CCMU_PLL_CPUX_CTRL_REG);
		div_p    = 1 << ((reg_val >> 16) & 0x3);
		factor_n = ((reg_val >> 8) & 0xff) + 1;
		div_m    = ((reg_val >> 0) & 0x3) + 1;

		clock = 24 * factor_n / div_m / div_p;
		break;
	default:
		return 0;
	}
	return clock;
}


int sunxi_clock_get_axi(void)
{
	unsigned int reg_val = 0;
	int factor = 0;
	int clock = 0;

	reg_val   = readl(CCMU_CPUX_AXI_CFG_REG);
	factor    = ((reg_val >> 0) & 0x03) + 1;
	clock = sunxi_clock_get_corepll() / factor;

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
	factor_m  = ((reg_val >> 0) & 0x03) + 1;
	factor_n  = 1 << ((reg_val >> 8) & 0x03);

	switch (src) {
	case 0:/*OSC24M  */
		src_clock = 24;
		break;
	case 1:/*CCMU_32K  */
		src_clock = 32 / 1000;
		break;
	case 2:	/*RC16M  */
		src_clock = 16;
		break;
	case 3:/*PLL_PERI0(1X)  */
		src_clock   = sunxi_clock_get_pll6();
		break;
	default:
		return 0;
	}

	clock = src_clock / factor_m / factor_n;

	return clock;
}


int sunxi_clock_get_apb1(void)
{
	unsigned int reg_val = 0;
	int src = 0, src_clock = 0;
	int factor_m = 0, factor_n = 0;

	reg_val = readl(CCMU_APB1_CFG_GREG);
	factor_m  = (reg_val >> 0) & 0x03;
	factor_n  = 1 << ((reg_val >> 8) & 0x03);
	src = (reg_val >> 24) & 0x3;

	switch (src) {
	case 0:/*OSC24M  */
		src_clock = 24;
		break;
	case 1:/*CCMU_32K  */
		src_clock = 32 / 1000;
		break;
	case 2:	/*PSI  */
		src_clock = sunxi_clock_get_ahb();
		break;
	case 3:/*PLL_PERI0(1X)  */
		src_clock   = sunxi_clock_get_pll6();
		break;
	default:
		return 0;
	}

	return src_clock / factor_m / factor_n;
}

int sunxi_clock_get_apb2(void)
{
	unsigned int reg_val = 0;
	int clock = 0, factor_m = 0, factor_n = 0;
	int src = 0, src_clock = 0;

	reg_val = readl(CCMU_APB2_CFG_GREG);
	src = (reg_val >> 24) & 0x3;
	factor_m  = ((reg_val >> 0) & 0x03) + 1;
	factor_n  = ((reg_val >> 8) & 0x03) + 1;

	switch (src) {
	case 0:/*OSC24M  */
		src_clock = 24;
		break;
	case 1:/*CCMU_32K  */
		src_clock = 32 / 1000;
		break;
	case 2:	/*PSI  */
		src_clock = sunxi_clock_get_ahb();
		break;
	case 3:	/*PSI  */
		src_clock = sunxi_clock_get_pll6();
		break;
	default:
		return 0;
	}

	clock = src_clock / factor_m / factor_n;

	return clock;

}


int sunxi_clock_get_mbus(void)
{
	unsigned int reg_val;
	unsigned int src = 0, clock = 0, div = 0;
	reg_val = readl(CCMU_MBUS_CFG_REG);

	/*get src  */
	src = (reg_val >> 24) & 0x3;
	/*get div M, the divided clock is divided by M+1  */
	div = (reg_val & 0x3) + 1;

	switch (src) {
	case 0:/*src is OSC24M  */
		clock = 24;
		break;
	case 1:/*src is   pll_periph0(1x)/2  */
		clock = sunxi_clock_get_pll6() * 2;
		break;
	case 2:/*src is pll_ddr0  --not set in boot  */
		clock   = 0;
		break;
	case 3:/*src is pll_ddr1 --not set in boot  */
		clock   = 0;
		break;
	}

	clock = clock / div;

	return clock;
}

int sunxi_clock_set_corepll(int frequency)
{
	unsigned int reg_val;
	PLL_TABLE  pll_factor;

	/*switch to 24M  */
	reg_val = readl(CCMU_CPUX_AXI_CFG_REG);
	reg_val &= ~(0x03 << 24);
	reg_val |=  (0x00 << 24);
	writel(reg_val, CCMU_CPUX_AXI_CFG_REG);

	/*get config para form freq table  */
	clk_get_pll_para(&pll_factor, frequency);

	/* 24M*(N+1)/(M+1)/(P+1)*/
	reg_val = readl(CCMU_PLL_CPUX_CTRL_REG);
	reg_val &= ~((1 << 31) | (0x03 << 16) | (0xff << 8) | (0x03 << 0));
	reg_val |=  (pll_factor.FactorP << 16) | \
		    (pll_factor.FactorN << 8) | \
		    (pll_factor.FactorM << 0) ;
	writel(reg_val, CCMU_PLL_CPUX_CTRL_REG);
	/* lock enable */
	reg_val = readl(CCMU_PLL_CPUX_CTRL_REG);
	reg_val |= (1 << 29);
	writel(reg_val, CCMU_PLL_CPUX_CTRL_REG);

	/* enable pll */
	reg_val = readl(CCMU_PLL_CPUX_CTRL_REG);
	reg_val |=  (1 << 31);
	writel(reg_val, CCMU_PLL_CPUX_CTRL_REG);

	/*wait PLL_CPUX stable  */
#ifndef FPGA_PLATFORM
	while (!(readl(CCMU_PLL_CPUX_CTRL_REG) & (0x1 << 28)));
	__usdelay(1);
#endif
	/* lock disable */
	reg_val = readl(CCMU_PLL_CPUX_CTRL_REG);
	reg_val &= ~(1 << 29);
	writel(reg_val, CCMU_PLL_CPUX_CTRL_REG);

	/*set and change cpu clk src to PLL_CPUX,  PLL_CPUX:AXI0 = 408M:136M  */
	reg_val = readl(CCMU_CPUX_AXI_CFG_REG);
	reg_val &=  ~(0x03 << 24);
	reg_val |=  (0x03 << 24);
	writel(reg_val, CCMU_CPUX_AXI_CFG_REG);

	return  0;
}

#define set_wbit(addr, v) (*((volatile unsigned long  *)(addr)) |=  (unsigned long)(v))
#define clr_wbit(addr, v) (*((volatile unsigned long  *)(addr)) &= ~(unsigned long)(v))
u32 ccm_get_pll_periph_clk(void)
{

	return sunxi_clock_get_pll6();
}

void ccm_module_disable(u32 clk_id)
{
	clr_wbit(CCMU_SPI_BGR_REG, 0x1U << (SPI_RST_OFFSET + clk_id));
}

void ccm_module_enable(u32 clk_id)
{
	set_wbit(CCMU_SPI_BGR_REG, 0x1U << (SPI_RST_OFFSET + clk_id));
}

void ccm_clock_enable(u32 clk_id)
{
	set_wbit(CCMU_SPI_BGR_REG, 0x1U << (SPI_GATING_OFFSET + clk_id));
}

void ccm_clock_disable(u32 clk_id)
{
	clr_wbit(CCMU_SPI_BGR_REG, 0x1U << (SPI_GATING_OFFSET + clk_id));
}

void ccm_module_reset(u32 clk_id)
{
	ccm_module_disable(clk_id);
	ccm_module_enable(clk_id);
}

