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

static void switch_cci_to_osc(void)
{
	__u32 reg_val;

	/*set cii=24M*/
	reg_val = readl(CCMU_CCI400_CFG_GREG);
	reg_val &= ~(0x07 << 24);
	writel(reg_val, CCMU_CCI400_CFG_GREG);
	__usdelay(1);

	/*set div=0*/
	reg_val = readl(CCMU_CCI400_CFG_GREG);
	reg_val &= ~(0x03 << 0);
	writel(reg_val, CCMU_CCI400_CFG_GREG);
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
	reg_val &= ~((0x01 << pll_enable) | (0x01 << lock_enable));
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


static void enable_cpu_pll(u32 reg_addr, u32 pll_enable, u32 lock_enable, u32 lock_flag)
{
	__u32 reg_val;

	/*disable pll*/
	close_pll(reg_addr, pll_enable, lock_enable);

	/*set cpu_pll=408M,P=0x0+1/N=0x10+1/K=0x0+1/M=0x0+1 divide*/
	reg_val = readl(reg_addr);
	reg_val &= ~((0x03 << 16) | (0x1f << 8) | (0x3 << 4) | (0x3 << 0));
	reg_val |= ((0x0 << 16) | (0x10 << 8) | (0x0 << 4) | (0x0 << 0));
	writel(reg_val, reg_addr);

	/* lock enable */
	reg_val = readl(reg_addr);
	reg_val |= (0x1 << lock_enable);
	writel(reg_val, reg_addr);

	/* enable pll */
	reg_val = readl(reg_addr);
	reg_val |=  (0x1 << pll_enable);
	writel(reg_val, reg_addr);

	/*wait PLL_CPUX stable*/
#ifndef FPGA_PLATFORM
	while (!(readl(reg_addr) & (0x1 << lock_flag))) {
		;
	}
	__usdelay(1);
#endif
}

void set_pll_cpux_axi(void)
{
	/*select CPUX  clock src: OSC24M,AXI divide ratio is 2, system apb clk ratio is 4*/
	switch_cpu_axi_to_osc(CCMU_C0_CPUX_AXI_CFG_REG);
	switch_cpu_axi_to_osc(CCMU_C1_CPUX_AXI_CFG_REG);

	/* set default val: clk is 408M  ,PLL_OUTPUT= 24M*N/( M*P)*/
	enable_cpu_pll(CCMU_PLL_C0_CPUX_CTRL_REG, 31, 29, 28);
	enable_cpu_pll(CCMU_PLL_C1_CPUX_CTRL_REG, 31, 29, 28);

	/*set and change cpu clk src to PLL_CPUX,  PLL_CPUX:AXI0 = 408M:204M*/
	switch_cpu_axi_to_pll(CCMU_C0_CPUX_AXI_CFG_REG);
	switch_cpu_axi_to_pll(CCMU_C1_CPUX_AXI_CFG_REG);
}

void set_pll_cci(void)
{
	__u32 reg_val;
	/*set cii=pll0/4=300M*/
	/*set div=0x3+1*/
	reg_val = readl(CCMU_CCI400_CFG_GREG);
	reg_val &= ~(0x3 << 0);
	reg_val |= (0x3 << 0);
	writel(reg_val, CCMU_CCI400_CFG_GREG);
	__usdelay(1);

	/*switch pll_peri0(2x)*/
	reg_val = readl(CCMU_CCI400_CFG_GREG);
	reg_val &= ~(0x7 << 24);
	reg_val |= (0x2 << 24);
	writel(reg_val, CCMU_CCI400_CFG_GREG);
	__usdelay(20);
}

void set_pll_hsic(void)
{
	__u32 reg_val;

	/*set default value, default is 480M*/
	writel(0x2701, CCMU_PLL_HSIC_CTRL_REG);

	/* lock enable */
	reg_val = readl(CCMU_PLL_HSIC_CTRL_REG);
	reg_val |= (1<<29);
	writel(reg_val, CCMU_PLL_HSIC_CTRL_REG);

	/* enabe PLL */
	reg_val =readl(CCMU_PLL_HSIC_CTRL_REG);
	reg_val |= (1<<31);
	writel(reg_val, CCMU_PLL_HSIC_CTRL_REG);
#ifndef FPGA_PLATFORM
	while(!(readl(CCMU_PLL_HSIC_CTRL_REG) & (0x1<<28)));
	__usdelay(20);
#endif
	/* lock disable */
	reg_val = readl(CCMU_PLL_HSIC_CTRL_REG);
	reg_val &= ~(1<<29);
	writel(reg_val, CCMU_PLL_HSIC_CTRL_REG);
}

void set_pll_periph0(void)
{
	__u32 reg_val;

	/*change  psi/ahb1~3/apb1~2/cci/mbus src to OSC24M before set pll0 */
	switch_psi_ahb_apb_to_osc(CCMU_PSI_AHB1_AHB2_CFG_REG);
	switch_psi_ahb_apb_to_osc(CCMU_AHB3_CFG_GREG);
	switch_psi_ahb_apb_to_osc(CCMU_APB1_CFG_GREG);
	switch_psi_ahb_apb_to_osc(CCMU_APB2_CFG_GREG);
	switch_cci_to_osc();
	switch_mubs_to_osc();

	/* set default val*/
	close_pll(CCMU_PLL_PERI0_CTRL_REG, 31, 29);
	reg_val = readl(CCMU_PLL_PERI0_CTRL_REG);
	reg_val &= ~((0x1f << 8) | (0x3 << 4) | (0x3 << 0));
	reg_val |= ((0x18 << 8) | (0x1 << 4) | (0x1 << 0));
	writel(reg_val, CCMU_PLL_PERI0_CTRL_REG);
	__usdelay(10);

	/* lock enable */
	reg_val = readl(CCMU_PLL_PERI0_CTRL_REG);
	reg_val |= (1<<29);
	writel(reg_val, CCMU_PLL_PERI0_CTRL_REG);

	/* enabe PLL: 600M(1X)  1200M(2x) */
	reg_val =readl(CCMU_PLL_PERI0_CTRL_REG);
	reg_val |= (1<<31);
	writel(reg_val, CCMU_PLL_PERI0_CTRL_REG);

#ifndef FPGA_PLATFORM
	while(!(readl(CCMU_PLL_PERI0_CTRL_REG) & (0x1<<28)));
	__usdelay(20);
#endif
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
	reg_val |= (0X01 << 31);
	writel(reg_val, CCMU_MBUS_CFG_REG);
	__usdelay(1);
}

void set_pll( void )
{
	printf("set pll start\n");
	set_pll_cpux_axi();
	/*set_pll_hsic();*/
	set_pll_periph0();
	set_pll_cci();
	set_ahb();
	set_apb();
	set_pll_dma();
	set_pll_mbus();

	printf("set pll end\n");
	return ;
}

void set_pll_in_secure_mode( void )
{
	set_pll();
}

void reset_pll( void )
{
	/*cpu axi to osc*/
	printf("reset pll\n");
	switch_cpu_axi_to_osc(CCMU_C0_CPUX_AXI_CFG_REG);
	switch_cpu_axi_to_osc(CCMU_C1_CPUX_AXI_CFG_REG);

	/*change  psi/ahb1~3/apb1~2/cci/mbus src to OSC24M */
	switch_psi_ahb_apb_to_osc(CCMU_PSI_AHB1_AHB2_CFG_REG);
	switch_psi_ahb_apb_to_osc(CCMU_AHB3_CFG_GREG);
	switch_psi_ahb_apb_to_osc(CCMU_APB1_CFG_GREG);
	switch_cci_to_osc();
	switch_mubs_to_osc();

	return ;
}

void set_gpio_gate(void)
{

}

