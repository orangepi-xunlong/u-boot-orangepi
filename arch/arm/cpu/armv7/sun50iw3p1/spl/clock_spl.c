/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
/*
**********************************************************************************************************************
*
*						           the Embedded Secure Bootloader System
*
*
*						       Copyright(C), 2006-2014, Allwinnertech Co., Ltd.
*                                           All Rights Reserved
*
* File    :
*
* By      :
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#include "common.h"
#include "asm/io.h"
#include "asm/arch/ccmu.h"
#include "asm/arch/timer.h"
#include "asm/arch/archdef.h"


void set_pll_cpux_axi(void)
{
	__u32 reg_val;
	/* select CPUX  clock src: OSC24M,AXI divide ratio is 3, system apb clk ratio is 4 */
	writel((0<<24) | (3<<8) | (2<<0), CCMU_CPUX_AXI_CFG_REG);
	__usdelay(1);

	/* set default val: clk is 408M  ,PLL_OUTPUT= 24M*N/( M*P)*/
	writel((0x02001000), CCMU_PLL_CPUX_CTRL_REG);

	/* lock enable */
	reg_val = readl(CCMU_PLL_CPUX_CTRL_REG);
	reg_val |= (1<<29);
	writel(reg_val, CCMU_PLL_CPUX_CTRL_REG);

	/* enable pll */
	reg_val = readl(CCMU_PLL_CPUX_CTRL_REG);
	reg_val |=  (1<<31);
	writel(reg_val, CCMU_PLL_CPUX_CTRL_REG);

	//wait PLL_CPUX stable
#ifndef FPGA_PLATFORM
	while(!(readl(CCMU_PLL_CPUX_CTRL_REG) & (0x1<<28)));
	__usdelay(1);
#endif
	/* lock disable */
	reg_val = readl(CCMU_PLL_CPUX_CTRL_REG);
	reg_val &= ~(1<<29);
	writel(reg_val, CCMU_PLL_CPUX_CTRL_REG);

	//set and change cpu clk src to PLL_CPUX,  PLL_CPUX:AXI0 = 408M:136M
	reg_val = readl(CCMU_CPUX_AXI_CFG_REG);
	reg_val &=  ~(0x03 << 24);
	reg_val |=  (0x03 << 24);
	writel(reg_val, CCMU_CPUX_AXI_CFG_REG);
	__usdelay(1);
}

void set_pll_periph0(void)
{
	__u32 reg_val;

	/*change  psi/ahb src to OSC24M before set pll6 */
	reg_val = readl(CCMU_PSI_AHB1_AHB2_CFG_REG);
	reg_val &= (~(0x3<<24));
	writel(reg_val,CCMU_PSI_AHB1_AHB2_CFG_REG);

	/* set default val*/
	writel(0x63<<8, CCMU_PLL_PERI0_CTRL_REG);
	
	/* lock enable */
	reg_val = readl(CCMU_PLL_PERI0_CTRL_REG);
	reg_val |= (1<<29);
	writel(reg_val, CCMU_PLL_PERI0_CTRL_REG);

	/* enabe PLL: 600M(1X)  1200M(2x) 2400M(4X) */
	reg_val =readl(CCMU_PLL_PERI0_CTRL_REG);
	reg_val |= (1<<31);
	writel(reg_val, CCMU_PLL_PERI0_CTRL_REG);

#ifndef FPGA_PLATFORM
	while(!(readl(CCMU_PLL_PERI0_CTRL_REG) & (0x1<<28)));
	__usdelay(20);
#endif
	/* lock disable */
	reg_val = readl(CCMU_PLL_PERI0_CTRL_REG);
	reg_val &= (~(1<<29));
	writel(reg_val, CCMU_PLL_PERI0_CTRL_REG);
	
}

void set_ahb(void)
{
	/* PLL6:AHB1:APB1 = 600M:200M:100M */
	writel((2<<0) | (0<<8), CCMU_PSI_AHB1_AHB2_CFG_REG);
	writel((0x03 << 24)|readl(CCMU_PSI_AHB1_AHB2_CFG_REG), CCMU_PSI_AHB1_AHB2_CFG_REG);
	__usdelay(1);
}

void set_apb(void)
{
	/*PLL6:APB1 = 600M:100M */
	writel((2<<0) | (1<<8), CCMU_APB1_CFG_GREG);
	writel((0x03 << 24)|readl(CCMU_APB1_CFG_GREG), CCMU_APB1_CFG_GREG);
	__usdelay(1);
}

/* SRAMC's clk source is AHB1, so the AHB1 clk must less than 100M */
void set_pll_ahb_for_secure(void)
{
	/* PLL6:AHB1 = 600M:100M */
	/* div M=3 N=2*/
	writel((2<<0) | (1<<8), CCMU_PSI_AHB1_AHB2_CFG_REG);
	writel((0x03 << 24)|readl(CCMU_PSI_AHB1_AHB2_CFG_REG), CCMU_PSI_AHB1_AHB2_CFG_REG);
	__usdelay(1);
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

	/* set MBUS div */
	reg_val = readl(CCMU_MBUS_CFG_REG);
	reg_val |= (2<<0);
	writel(reg_val, CCMU_MBUS_CFG_REG); 
	__usdelay(1);

	/* set MBUS clock source to pll6(2x), mbus=pll6/(m+1) = 400M*/
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
	set_pll_periph0();
	if(0)
		set_pll_ahb_for_secure();
	else
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
	__u32 reg_val;
	//set ahb,apb to default, use OSC24M
	reg_val = readl(CCMU_PSI_AHB1_AHB2_CFG_REG);
	reg_val &= (~(0x3<<24));
	writel(reg_val,CCMU_PSI_AHB1_AHB2_CFG_REG);

	reg_val = readl(CCMU_APB1_CFG_GREG);
	reg_val &= (~(0x3<<24));
	writel(reg_val,CCMU_APB1_CFG_GREG);

	//set cpux pll to default,use OSC24M
	writel(0x0301, CCMU_CPUX_AXI_CFG_REG);
	return ;
}

void set_gpio_gate(void)
{

}

