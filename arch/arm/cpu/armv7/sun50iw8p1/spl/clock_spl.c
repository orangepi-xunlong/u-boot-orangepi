/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
/* clock_spl.c
 *
 * Copyright (c) 2016 Allwinnertech Co., Ltd.
 * Author: zhouhuacai <zhouhuacai@allwinnertech.com>
 *
 * Define the set clock function interface for spl
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "common.h"
#include "asm/io.h"
#include "asm/arch/ccmu.h"
#include "asm/arch/timer.h"
#include "asm/arch/archdef.h"

#define HOSC		0
#define PLL_24M		1
#define HOSC_19M	1
#define HOSC_38M	2
#define HOSC_24M	3

u32 osc_type;

static void prode_which_osc(void)
{
	u32 val = 0;

	val = (readl(XO_CTRL_REG) & (0x3 << 14));
	osc_type = (val >> 14);
}

static u32 prode_pll_input_clk_src(void)
{
	u32 val = 0;
	u32 pll_src = 0;

	val = (readl(XO_CTRL_REG) & (0x1 << 13));
	pll_src = (val >> 13);
	return pll_src;
}

static void cfg_pll_clk_src_input(u32 pll_addr)
{
	u32 reg_value = 0;

	reg_value = prode_pll_input_clk_src();
	if (reg_value == HOSC) {
		reg_value = readl(pll_addr);
		reg_value |= (0x1 << 23);
		writel(reg_value, pll_addr);
	} else if (reg_value == PLL_24M) {
		reg_value = readl(pll_addr);
		reg_value &= (~(0x1 << 23));
		writel(reg_value, pll_addr);
	}
}

static void pll_lock_enable(u32 reg_addr, u32 enable_bit, u32 lock_bit)
{
	u32 reg_value = 0;

	reg_value = readl(reg_addr);
	reg_value |= (0x1 << enable_bit);
	writel(reg_value, reg_addr);

#ifndef FPGA_PLATFORM
	while (!(readl(reg_addr) & (0x1 << lock_bit))) {
		;
	}
	__usdelay(1);
#endif
}

static void pll_out_enable(u32 pll_addr, u32 enable_bit)
{
	u32 reg_value = 0;

	reg_value = readl(pll_addr);
	reg_value |= (0x1U << enable_bit);
	writel(reg_value , pll_addr);
}


static s32 prode_pll_enable(u32 reg_addr, u32 pll_enable)
{
	u32 val = 0;

	val = readl(reg_addr);
	if (val & (0x1U << pll_enable)) {
		return 0;
	}
	return -1;
}

static void cfg_pll_24M(void)
{
	u32 reg_value = 0;

	/*close and clean bypass*/
	reg_value = readl(CCMU_PLL_24M_CTRL_REG);
	reg_value &= (~((0x1U << 31) | (0x1 << 29) | (0x1 << 27) | (0x1 << 23)));
	writel(reg_value, CCMU_PLL_24M_CTRL_REG);

	/*set div*/
	if (osc_type == HOSC_38M) {
		reg_value = (readl(CCMU_PLL_24M_CTRL_REG));
		reg_value &= (~((0x1f << 16) | (0xff << 8) | (0x1 << 1) | (0x1 << 0)));
		reg_value |= (((0x13 << 16) | (0x31 << 8) | (0x1 << 1) | (0x1 << 0)));
		writel(reg_value, CCMU_PLL_24M_CTRL_REG);
	} else if (osc_type == HOSC_19M) {
		reg_value = (readl(CCMU_PLL_24M_CTRL_REG));
		reg_value &= (~((0x1f << 16) | (0xff << 8) | (0x1 << 1) | (0x1 << 0)));
		reg_value |= (((0x13 << 16) | (0x31 << 8) | (0x0 << 1) | (0x1 << 0)));
		writel(reg_value, CCMU_PLL_24M_CTRL_REG);
	}
	__usdelay(10);

	/*enable pll*/
	reg_value = (readl(CCMU_PLL_24M_CTRL_REG));
	reg_value |= (0x1U << 31);
	writel(reg_value, CCMU_PLL_24M_CTRL_REG);

	/*enable lock*/
	pll_lock_enable(CCMU_PLL_24M_CTRL_REG, 29, 28);

	/*pll out enable*/
	pll_out_enable(CCMU_PLL_24M_CTRL_REG, 27);
}

static void switch_cpu_axi_to_osc(u32 reg_addr)
{
	__u32 reg_val;
	reg_val = readl(reg_addr);
	reg_val &= ~(0x03 << 24);
	writel(reg_val, reg_addr);
	__usdelay(1);

	reg_val = readl(reg_addr);
	reg_val &= ~((0x03 << 8) | (0x3 << 0));
	reg_val |= ((0x03 << 8) | (0x1 << 0));
	writel(reg_val, reg_addr);
	__usdelay(1);
}

static void switch_psi_ahb_apb_to_osc(u32 reg_addr)
{
	__u32 reg_val;
	reg_val = readl(reg_addr);
	reg_val &= ~(0x03 << 24);
	writel(reg_val, reg_addr);
	__usdelay(1);

	reg_val = readl(reg_addr);
	reg_val &= ~((0x03 << 8) | (0x3 << 0));
	writel(reg_val, reg_addr);
	__usdelay(1);
}

static void switch_mubs_to_osc(void)
{
	 __u32 reg_val;
	reg_val = readl(CCMU_MBUS_CFG_REG);
	reg_val &= ~(0x03 << 24);
	writel(reg_val, CCMU_MBUS_CFG_REG);
	__usdelay(1);

	reg_val = readl(CCMU_MBUS_CFG_REG);
	reg_val &= ~((0x7 << 0));
	writel(reg_val, CCMU_MBUS_CFG_REG);
	__usdelay(1);

}

static void close_pll(u32 reg_addr, u32 pll_enable, u32 lock_enable)
{
	__u32 reg_val;
	reg_val = readl(reg_addr);
	reg_val &= ~((0x01U << pll_enable) | (0x01U << lock_enable));
	writel(reg_val, reg_addr);
	__usdelay(1);

}

static void switch_cpu_axi_to_pll(u32 reg_addr)
{
	__u32 reg_val;
	/*set cpu = pll*/
	reg_val = readl(reg_addr);
	reg_val &=  ~(0x03 << 24);
	reg_val |=  (0x03 << 24);
	writel(reg_val, reg_addr);
	__usdelay(1);

	/*set axi div=0x1+1*/
	reg_val = readl(reg_addr);
	reg_val &=  ~(0x03 << 0);
	reg_val |=  (0x01 << 0);
	writel(reg_val, reg_addr);
	__usdelay(1);

	/*set apb div=default=0x3+1*/
	reg_val = readl(reg_addr);
	reg_val &=  ~(0x03 << 8);
	reg_val |=  (0x03 << 8);
	writel(reg_val, reg_addr);
	__usdelay(1);

}

void switch_psi_ahb_to_pll(u32 reg_addr)
{
	__u32 reg_val;

	/* PLL0:AHB1:APB1 = 600M:200M:100M */
	reg_val = readl(reg_addr);
	reg_val &= ~((0x03 << 8) | (0x03 << 0));
	reg_val |= ((0x0 << 8) | (0x2 << 0));
	writel(reg_val, reg_addr);
	__usdelay(1);

	reg_val = readl(reg_addr);
	reg_val &= ~(0x03 << 24);
	reg_val |= (0x03 << 24);
	writel(reg_val, reg_addr);
	__usdelay(1);

}

void switch_apb_to_pll(u32 reg_addr)
{
	__u32 reg_val;

	/* PLL0:AHB1:APB1 = 600M:200M:100M */
	reg_val = readl(reg_addr);
	reg_val &= ~((0x03 << 8) | (0x03 << 0));
	reg_val |= ((0x1 << 8) | (0x2 << 0));
	writel(reg_val, reg_addr);
	__usdelay(1);

	reg_val = readl(reg_addr);
	reg_val &= ~(0x03 << 24);
	reg_val |= (0x03 << 24);
	writel(reg_val, reg_addr);
	__usdelay(1);

}

static u32 prode_pll_init(void)
{
	u32 val_1 = 0;
	u32 val_2 = 0;
	u32 pll_src = 0;

	val_1 = prode_pll_enable(CCMU_PLL_CPUX_CTRL_REG, 31);
	val_2 = prode_pll_enable(CCMU_PLL_PERI0_CTRL_REG, 31);
	if ((val_1 == 0) && (val_2 == 0)) {
		return 0;
	}

	if (val_1 < 0) {
		/*select CPUX  clock src: OSC24M,AXI divide ratio is 2, system apb clk ratio is 4*/
		switch_cpu_axi_to_osc(CCMU_CPUX_AXI_CFG_REG);
	}

	if (val_2 < 0) {
		switch_psi_ahb_apb_to_osc(CCMU_PSI_AHB1_AHB2_CFG_REG);
		switch_psi_ahb_apb_to_osc(CCMU_AHB3_CFG_GREG);
		switch_psi_ahb_apb_to_osc(CCMU_APB1_CFG_GREG);
		switch_psi_ahb_apb_to_osc(CCMU_APB2_CFG_GREG);
		switch_mubs_to_osc();
	}

	prode_which_osc();
	pll_src = prode_pll_input_clk_src();
	if (pll_src == PLL_24M) {
		pll_src = prode_pll_enable(CCMU_PLL_24M_CTRL_REG, 31);
		if (pll_src < 0) {
			cfg_pll_24M();
		}
	} else if (pll_src ==  HOSC) {
		;
	}

	return -1;
}


static void enable_cpu_pll(u32 reg_addr, u32 pll_enable, u32 lock_enable, u32 lock_flag)
{
	__u32 reg_val;

	/*disable pll*/
	close_pll(reg_addr, pll_enable, lock_enable);

	/*cfg pll src*/
	cfg_pll_clk_src_input(CCMU_PLL_CPUX_CTRL_REG);

	/*set cpu_pll=408M,P=0x0+1/N=0x10+1/K=0x0+1/M=0x0+1 divide*/
	reg_val = readl(reg_addr);
	reg_val &= ~((0x03 << 16) | (0xff << 8) | (0x3 << 0));
	reg_val |= ((0x0 << 16) | (0x10 << 8) | (0x0 << 0));
	writel(reg_val, reg_addr);

	/* lock enable */
	reg_val = readl(reg_addr);
	reg_val |= (0x1 << lock_enable);
	writel(reg_val, reg_addr);

	/* enable pll */
	reg_val = readl(reg_addr);
	reg_val |=  (0x1U << pll_enable);
	writel(reg_val, reg_addr);

	/*wait PLL_CPUX stable*/
#ifndef FPGA_PLATFORM
	while (!(readl(reg_addr) & (0x1 << lock_flag))) {
		;
	}
	__usdelay(1);
#endif
	/*pll out enable*/
	pll_out_enable(reg_addr, 27);
}

void set_pll_cpux_axi(void)
{
	/* set default val: clk is 408M  ,PLL_OUTPUT= 24M*N/(M*P)*/
	enable_cpu_pll(CCMU_PLL_CPUX_CTRL_REG, 31, 29, 28);

	/*set and change cpu clk src to PLL_CPUX,  PLL_CPUX:AXI0 = 408M:204M*/
	switch_cpu_axi_to_pll(CCMU_CPUX_AXI_CFG_REG);
}

void set_pll_periph0(void)
{
	__u32 reg_val;

	/* close pll*/
	close_pll(CCMU_PLL_PERI0_CTRL_REG, 31, 29);

	/*cfg pll clk src*/
	cfg_pll_clk_src_input(CCMU_PLL_PERI0_CTRL_REG);

	/*cfg pll div*/
	reg_val = readl(CCMU_PLL_PERI0_CTRL_REG);
	reg_val &= ~((0xFF << 8) | (0x3 << 0));
	if (osc_type == HOSC_38M) {
		reg_val |= ((0x31 << 8) | (0x0 << 1) | (0x1 << 0));
	} else {
		reg_val |= ((0x31 << 8) | (0x0 << 1) | (0x0 << 0));
	}
	writel(reg_val, CCMU_PLL_PERI0_CTRL_REG);
	__usdelay(10);

	/* lock enable */
	reg_val = readl(CCMU_PLL_PERI0_CTRL_REG);
	reg_val |= (1<<29);
	writel(reg_val, CCMU_PLL_PERI0_CTRL_REG);

	/* enabe PLL: 600M(1X)  1200M(2x) */
	reg_val = readl(CCMU_PLL_PERI0_CTRL_REG);
	reg_val |= (1U << 31);
	writel(reg_val, CCMU_PLL_PERI0_CTRL_REG);

#ifndef FPGA_PLATFORM
	while (!(readl(CCMU_PLL_PERI0_CTRL_REG) & (0x1 << 28)))
		;
	__usdelay(20);
#endif
	/*pll out enable*/
	pll_out_enable(CCMU_PLL_PERI0_CTRL_REG, 27);
}

void set_ahb(void)
{
	/* PLL0:AHB1:APB1 = 600M:200M:100M */
	switch_psi_ahb_to_pll(CCMU_PSI_AHB1_AHB2_CFG_REG);
	switch_psi_ahb_to_pll(CCMU_AHB3_CFG_GREG);
}

void set_apb(void)
{
	/*PLL6:APB1 = 600M:100M */
	switch_apb_to_pll(CCMU_APB1_CFG_GREG);
}

void set_pll_dma(void)
{
	/*dma reset*/
	writel(readl(CCMU_DMA_BGR_REG)  | (1 << 16), CCMU_DMA_BGR_REG);
	__usdelay(20);
	/*gating clock for dma pass*/
	writel(readl(CCMU_DMA_BGR_REG) | (1 << 0), CCMU_DMA_BGR_REG);
}

void set_pll_mbus(void)
{
	__u32 reg_val;

	reg_val = readl(CCMU_MBUS_CFG_REG);
	if ((reg_val & (0x3 << 24)) == 1) {
		return;
	}
	/*reset mbus domain*/
	reg_val = 1<<30;
	writel(1<<30, CCMU_MBUS_CFG_REG);
	__usdelay(1);

	/* set MBUS div m=2*/
	reg_val = readl(CCMU_MBUS_CFG_REG);
	reg_val |= (2<<0);
	writel(reg_val, CCMU_MBUS_CFG_REG);
	__usdelay(1);

	/* set MBUS clock source to pll6(2x), mbus=pll0/(m+1) = 400M*/
	reg_val = readl(CCMU_MBUS_CFG_REG);
	reg_val |= (1<<24);
	writel(reg_val, CCMU_MBUS_CFG_REG);
	__usdelay(1);

	/* open MBUS clock */
	reg_val = readl(CCMU_MBUS_CFG_REG);
	reg_val |= (0X01U << 31);
	writel(reg_val, CCMU_MBUS_CFG_REG);
	__usdelay(1);
}

void set_pll(void)
{
	printf("set pll start\n");
	if (prode_pll_init() == 0) {
		switch_psi_ahb_apb_to_osc(CCMU_PSI_AHB1_AHB2_CFG_REG);
		switch_psi_ahb_apb_to_osc(CCMU_AHB3_CFG_GREG);
		switch_psi_ahb_apb_to_osc(CCMU_APB1_CFG_GREG);
		switch_mubs_to_osc();
		set_pll_periph0();
		set_ahb();
		set_apb();
		set_pll_dma();
		set_pll_mbus();
		return;
	}
	set_pll_cpux_axi();
	set_pll_periph0();
	set_ahb();
	set_apb();
	set_pll_dma();
	set_pll_mbus();

	printf("set pll end\n");
	return ;
}

void set_pll_in_secure_mode(void)
{
	set_pll();
}

void reset_pll(void)
{
	/*cpu axi to osc*/
	printf("reset pll\n");
	switch_cpu_axi_to_osc(CCMU_CPUX_AXI_CFG_REG);

	/*change  psi/ahb1~3/apb1~2/cci/mbus src to OSC24M */
	switch_psi_ahb_apb_to_osc(CCMU_PSI_AHB1_AHB2_CFG_REG);
	switch_psi_ahb_apb_to_osc(CCMU_AHB3_CFG_GREG);
	switch_psi_ahb_apb_to_osc(CCMU_APB1_CFG_GREG);
	switch_mubs_to_osc();

	return ;
}

void set_gpio_gate(void)
{

}

int sunxi_key_clock_open(void)
{
	uint reg_val = 0;

	/* reset */
	reg_val = readl(CCMU_GPADC_BGR_REG);
	reg_val &= ~(1<<16);
	writel(reg_val, CCMU_GPADC_BGR_REG);

	__usdelay(2);

	reg_val |=  (1<<16);
	writel(reg_val, CCMU_GPADC_BGR_REG);

	/* enable KEYADC gating */
	reg_val = readl(CCMU_GPADC_BGR_REG);
	reg_val |= (1<<0);
	writel(reg_val, CCMU_GPADC_BGR_REG);

	return 0;
}

int sunxi_key_clock_close(void)
{
	uint reg_val = 0;

	/* disable KEYADC gating */
	reg_val = readl(CCMU_GPADC_BGR_REG);
	reg_val &= ~(1<<0);
	writel(reg_val, CCMU_GPADC_BGR_REG);

	return 0;
}
