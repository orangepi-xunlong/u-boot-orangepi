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
	//select CPUX  clock src: OSC24M,AXI divide ratio is 3, system apb clk ratio is 4
	//cpu/axi /sys apb  clock ratio
	writel((1<<16) | (3<<8) | (2<<0), CCMU_CPUX_AXI_CFG_REG);
	__msdelay(1);

#if 0
	//set PLL_CPUX, the  default  clk is 408M  ,PLL_OUTPUT= 24M*N*K/( M*P)
	writel((0x1000), CCMU_PLL_CPUX_CTRL_REG);
	writel((1<<31) | readl(CCMU_PLL_CPUX_CTRL_REG), CCMU_PLL_CPUX_CTRL_REG);
#endif
	//set PLL_CPUX, the  default  clk is 1008M  ,PLL_OUTPUT= 24M*N*K/( M*P)
	reg_val = readl(CCMU_PLL_CPUX_CTRL_REG);
	reg_val &= ~((1<<31) | (0x03 << 16) | (0x1f << 8) | (0x03 << 4) | (0x03 << 0));
	reg_val |=  ((1<<31) | (0 << 16) | (20<<8) | (1<<4) | (0 << 0)) ;
	writel(reg_val, CCMU_PLL_CPUX_CTRL_REG);
	__msdelay(1);
	//wait PLL_CPUX stable
#ifndef FPGA_PLATFORM
	while(!(readl(CCMU_PLL_CPUX_CTRL_REG) & (0x1<<28)));
	__usdelay(20);
#endif

	//set and change cpu clk src to PLL_CPUX,  PLL_CPUX:AXI0 = 408M:136M
	reg_val = readl(CCMU_CPUX_AXI_CFG_REG);
	reg_val &=  ~(3 << 16);
	reg_val |=  (2 << 16);
	writel(reg_val, CCMU_CPUX_AXI_CFG_REG);
	__msdelay(2);
}


void set_pll_periph0_ahb_apb(void)
{
	if((1U << 31)&readl(CCMU_PLL_PERIPH0_CTRL_REG))
	{
		//fel has enable pll_periph0
		printf("periph0 has been enabled\n");
		return;
	}
	//change ahb src before set pll6
	writel((0x01 << 12) | (readl(CCMU_AHB1_APB1_CFG_REG)&(~(0x3<<12))), CCMU_AHB1_APB1_CFG_REG);

	//enable PLL6:  600M(1X)  1200M(2x)
	writel( 0x41811, CCMU_PLL_PERIPH0_CTRL_REG);
	writel( (1U << 31)|readl(CCMU_PLL_PERIPH0_CTRL_REG), CCMU_PLL_PERIPH0_CTRL_REG);
	__msdelay(1);
#ifndef FPGA_PLATFORM
	while(!(readl(CCMU_PLL_PERIPH0_CTRL_REG) & (0x1<<28)));
	__usdelay(20);
#endif

	//set AHB1/APB1 clock  divide ratio
	//ahb1 clock src is PLL6,                           (0x03<< 12)
	//apb1 clk src is ahb1 clk src, divide  ratio is 2  (1<<8)
	//ahb1 pre divide  ratio is 3:    0:1  , 1:2,  2:3,   3:4 (2<<6)
	//ahb1 divide  ratio is 2:        0:1  , 1:2,  2:4,   3:8 (1<<4)
	//PLL6:AHB1:APB1 = 600M:100M:50M
	writel((1<<8) | (2<<6) | (1<<4), CCMU_AHB1_APB1_CFG_REG);
	writel((0x03 << 12)|readl(CCMU_AHB1_APB1_CFG_REG), CCMU_AHB1_APB1_CFG_REG);
	__msdelay(1);
}


void set_pll_dma(void)
{
	//----DMA function--------
	//dma reset
	writel(readl(CCMU_BUS_SOFT_RST_REG0)  | (1 << 6), CCMU_BUS_SOFT_RST_REG0);
	__usdelay(20);
	//gating clock for dma pass
	writel(readl(CCMU_BUS_CLK_GATING_REG0) | (1 << 6), CCMU_BUS_CLK_GATING_REG0);
	//auto gating disable ---auto gating function on1680&1689 is ok,so not need disable
	writel(7, (SUNXI_DMA_BASE+0x28));
}

void set_pll_mbus(void)
{
	//reset mbus domain
	writel(0x80000000, CCMU_MBUS_RST_REG);
	//open MBUS,clk src is pll6(2x) , pll6/(m+1) = 400M
	//writel((1<<31) | (1<<24) | (2<<0), CCMU_MBUS_CLK_REG);
	writel((2<<0), CCMU_MBUS_CLK_REG); __usdelay(1);//set MBUS div
	writel((1<<24) | (2<<0), CCMU_MBUS_CLK_REG); __usdelay(1);//set MBUS clock source
	writel((1<<31) | (1<<24) | (2<<0), CCMU_MBUS_CLK_REG); __usdelay(1);//open MBUS clock
}

void set_pll( void )
{
	__msdelay(300);//wait for ADDA Bias stable
	//use new mode
	printf("set pll start\n");

	set_pll_cpux_axi();
	set_pll_periph0_ahb_apb();
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
	writel(0x01010, CCMU_AHB1_APB1_CFG_REG);
	writel(0x10300, CCMU_CPUX_AXI_CFG_REG);
	return ;
}

void set_gpio_gate(void)
{
	volatile unsigned int reg_val;
	// R_GPIO reset deassert
	reg_val = readl(SUNXI_RPRCM_BASE+0xb0);
	reg_val |= 1;
	writel(reg_val, SUNXI_RPRCM_BASE+0xb0);
	// R_GPIO GATING open
	reg_val = readl(SUNXI_RPRCM_BASE+0x28);
	reg_val |= 1;
	writel(reg_val, SUNXI_RPRCM_BASE+0x28);
}

