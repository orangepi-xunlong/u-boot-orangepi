/*
 * Allwinner Sun50iw3 clock register definitions
 *
 * (C) Copyright 2017  <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <asm/arch/timer.h>

void clock_init_uart(void)
{
	/*
	 * boot0 already inited,
	 * just reset clock to restart uart,
	 * clean up non-process bytes in RX FIFO
	 */
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	unsigned int reg_val = 0;
	volatile int i;

	reg_val = readl(&ccm->uart_gate_reset);
	reg_val &= ~(1 << (CCM_UART_RST_OFFSET + 0));
	writel(reg_val, &ccm->uart_gate_reset);
	for (i = 0; i < 100; i++)
		;

	reg_val = readl(&ccm->uart_gate_reset);
	reg_val |= (1 << (CCM_UART_RST_OFFSET + 0));
	writel(reg_val, &ccm->uart_gate_reset);

	reg_val = readl(&ccm->uart_gate_reset);
	reg_val &= ~(1 << (CCM_UART_GATING_OFFSET + 0));
	writel(reg_val, &ccm->uart_gate_reset);
	for (i = 0; i < 100; i++)
		;

	reg_val = readl(&ccm->uart_gate_reset);
	reg_val |= (1 << (CCM_UART_GATING_OFFSET + 0));
	writel(reg_val, &ccm->uart_gate_reset);
}

uint clock_get_pll6(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	unsigned int reg_val = 0;
	int factor_n = 0, factor_m0 = 0, factor_m1 = 0;
	int pll6 = 0;

	reg_val   = readl(&ccm->pll6_cfg);
	factor_n  = ((reg_val >> 8) & 0xff) + 1;
	factor_m0 = ((reg_val >> 0) & 0x01) + 1;
	factor_m1 = ((reg_val >> 1) & 0x01) + 1;

	pll6 = 24 * factor_n / factor_m0 / factor_m1 / 2;

	return pll6;
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

int clock_set_corepll(int frequency)
{
	unsigned int reg_val = 0;
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	struct core_pll_freq_tbl  pll_factor;

	if (frequency == clock_get_corepll())
		return 0;
	else if (frequency >= 1008)
		frequency = 1008;


	/* switch to 24M*/
	reg_val = readl(&ccm->cpu_axi_cfg);
	reg_val &= ~(0x03 << 24);
	writel(reg_val, &ccm->cpu_axi_cfg);
	__udelay(20);

	/*pll output disable*/
	reg_val = readl(&ccm->pll1_cfg);
	reg_val &= ~(0x01 << 27);
	writel(reg_val, &ccm->pll1_cfg);

	/*get config para form freq table*/
	clk_get_pll_para(&pll_factor, frequency);

	reg_val = readl(&ccm->pll1_cfg);
	reg_val &= ~((0x03 << 16) | (0xff << 8)  | (0x03 << 0));
	reg_val |=  (pll_factor.FactorP << 16) | (pll_factor.FactorN << 8) | (pll_factor.FactorM << 0) ;
	writel(reg_val, &ccm->pll1_cfg);
	__udelay(20);

	/*enable lock*/
	reg_val = readl(&ccm->pll1_cfg);
	reg_val |=  (0x1 << 29);
	writel(reg_val, &ccm->pll1_cfg);
#ifndef FPGA_PLATFORM
	do {
		reg_val = readl(&ccm->pll1_cfg);
	} while (!(reg_val & (0x1 << 28)));
#endif

	/*enable pll output*/
	reg_val = readl(&ccm->pll1_cfg);
	reg_val |=  (0x1 << 27);
	writel(reg_val, &ccm->pll1_cfg);

	/* switch clk src to COREPLL*/
	reg_val = readl(&ccm->cpu_axi_cfg);
	reg_val &= ~(0x03 << 24);
	reg_val |=  (0x03 << 24);
	writel(reg_val, &ccm->cpu_axi_cfg);

	return  0;
}

uint clock_get_corepll(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	unsigned int reg_val;
	int div_m, div_p;
	int factor_n;
	int clock, clock_src;

	reg_val   = readl(&ccm->cpu_axi_cfg);
	clock_src = (reg_val >> 24) & 0x03;

	switch (clock_src) {
	case 0: /*OSC24M*/
		clock = 24;
		break;
	case 1: /*RTC32K*/
		clock = 32 / 1000;
		break;
	case 2: /*RC16M*/
		clock = 16;
		break;
	case 3: /*PLL_CPUX*/
		reg_val  = readl(&ccm->pll1_cfg);
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

uint clock_get_ahb(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	unsigned int reg_val = 0;
	int factor_m = 0, factor_n = 0;
	int clock = 0;
	int src = 0, src_clock = 0;

	reg_val  = readl(&ccm->psi_ahb1_ahb2_cfg);
	src      = (reg_val >> 24) & 0x3;
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
		src_clock = clock_get_pll6();
		break;
	default:
		return 0;
	}

	clock = src_clock / factor_m / factor_n;

	return clock;
}

uint clock_get_apb1(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	unsigned int reg_val = 0;
	int src = 0, src_clock = 0;
	int clock = 0, factor_m = 0, factor_n = 0;

	reg_val  = readl(&ccm->apb1_cfg);
	factor_m = ((reg_val >> 0) & 0x03) + 1;
	factor_n = 1 << ((reg_val >> 8) & 0x03);
	src      = (reg_val >> 24) & 0x3;

	switch (src) {
	case 0: /*OSC24M*/
		src_clock = 24;
		break;
	case 1: /*CCMU_32K*/
		src_clock = 32 / 1000;
		break;
	case 2: /*PSI*/
		src_clock = clock_get_ahb();
		break;
	case 3: /*PLL_PERI0(1X)*/
		src_clock = clock_get_pll6();
		break;
	default:
		return 0;
	}

	clock = src_clock / factor_m / factor_n;

	return clock;
}

uint clock_get_apb2(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	unsigned int reg_val = 0;
	int clock = 0, factor_m = 0, factor_n = 0;
	int src = 0, src_clock = 0;

	reg_val  = readl(&ccm->apb2_cfg);
	src      = (reg_val >> 24) & 0x3;
	factor_m = ((reg_val >> 0) & 0x03) + 1;
	factor_n = 1 << ((reg_val >> 8) & 0x03);

	switch (src) {
	case 0: /* OSC24M */
		src_clock = 24;
		break;
	case 1: /* CCMU_32K */
		src_clock = 32 / 1000;
		break;
	case 2: /* PSI */
		src_clock = clock_get_ahb();
		break;
	case 3: /* PSI */
		src_clock = clock_get_pll6();
		break;
	default:
		return 0;
	}

	clock = src_clock / factor_m / factor_n;

	return clock;
}

uint clock_get_mbus(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	unsigned int reg_val = 0;
	int factor_n = 0, factor_m0 = 0, factor_m1 = 0;
	int pll = 0;

	reg_val   = readl(&ccm->pll5_cfg);
	factor_n  = ((reg_val >> 8) & 0xff) + 1;
	factor_m0 = ((reg_val >> 0) & 0x01) + 1;
	factor_m1 = ((reg_val >> 1) & 0x01) + 1;

	pll = 24 * factor_n / factor_m0 / factor_m1;
	/* mbus = pll_ddr/4 */
	return pll / 4;
}

static void disable_otg_clk_reset_gating(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	u32 reg_temp = 0;

	reg_temp = readl(&ccm->usb_gate_reset);
	reg_temp &=
		~((0x1 << USBOTG_RESET_BIT) | (0x1 << USBOTG_CLK_ONOFF_BIT));
	writel(reg_temp, &ccm->usb_gate_reset);
}

static void disable_phy_clk_reset_gating(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	u32 reg_value = 0;

	reg_value = readl(&ccm->usb0_clk_cfg);
	reg_value &= ~((0x1 << USB0_PHY_CLK_ONOFF_BIT) |
		       (0x1 << USB0_PHY_RESET_BIT));
	writel(reg_value, &ccm->usb0_clk_cfg);
}

static void enable_otg_clk_reset_gating(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	u32 reg_value = 0;

	reg_value = readl(&ccm->usb_gate_reset);
	reg_value |= (1 << USBOTG_RESET_BIT);
	writel(reg_value, &ccm->usb_gate_reset);

	__usdelay(500);

	reg_value = readl(&ccm->usb_gate_reset);
	reg_value |= (1 << USBOTG_CLK_ONOFF_BIT);
	writel(reg_value, &ccm->usb_gate_reset);

	__usdelay(500);
}

static void enable_phy_clk_reset_gating(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	u32 reg_value = 0;

	reg_value = readl(&ccm->usb0_clk_cfg);
	reg_value |= (1 << USB0_PHY_CLK_ONOFF_BIT);
	writel(reg_value, &ccm->usb0_clk_cfg);

	__usdelay(500);

	reg_value = readl(&ccm->usb0_clk_cfg);
	reg_value |= (1 << USB0_PHY_RESET_BIT);
	writel(reg_value, &ccm->usb0_clk_cfg);
	__usdelay(500);
}

int usb_open_clock(void)
{
	u32 reg_value = 0;

	enable_phy_clk_reset_gating();
	enable_otg_clk_reset_gating();

	reg_value = readl(SUNXI_USBOTG_BASE + 0x420);
	reg_value |= (0x01 << 0);
	writel(reg_value, (SUNXI_USBOTG_BASE + 0x420));
	__msdelay(1);

	return 0;
}

int usb_close_clock(void)
{
	disable_otg_clk_reset_gating();
	disable_phy_clk_reset_gating();

	return 0;
}

int sunxi_set_sramc_mode(void)
{
	u32 reg_val;

	/* MAD SRAM:set sram to normal mode, default boot mode */
	reg_val = readl(SUNXI_SRAMC_BASE + 0X0004);
	reg_val &= ~(0x1 << 24);
	writel(reg_val, SUNXI_SRAMC_BASE + 0X0004);

	return 0;
}
