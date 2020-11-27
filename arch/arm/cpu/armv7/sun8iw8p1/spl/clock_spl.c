/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#include "common.h"
#include "asm/io.h"
#include "asm/armv7.h"
#include "asm/arch/platform.h"
#include "asm/arch/ccmu.h"
#include "asm/arch/timer.h"
#include "asm/arch/archdef.h"

static int clk_set_divd(void)
{
    unsigned int reg_val;

    //config axi
    reg_val = readl(CCMU_CPUX_AXI_CFG_REG);
    reg_val &= ~(0x03 << 8);
    reg_val |=  (0x01 << 8);
    reg_val |=  (1 << 0);
    writel(reg_val, CCMU_CPUX_AXI_CFG_REG);

    //config ahb
    reg_val = readl(CCMU_AHB1_APB1_CFG_REG);;
    reg_val &= ~((0x03 << 12) | (0x03 << 8) |(0x03 << 4));
    reg_val |=  (0x02 << 12);
    reg_val |=  (2 << 6);
    reg_val |=  (1 << 8);

    writel(reg_val, CCMU_AHB1_APB1_CFG_REG);

    return 0;
}

/*******************************************************************************
*Function name: set_pll
*origin void set_pll( void )
*Function: change the CPU freq
*Para in: void
*Return: void
*Notice:
*******************************************************************************/
void set_pll( void )
{
    unsigned int reg_val;
    unsigned int i;

    //set the default timer freq 408M
    //convert to 24M
    reg_val = readl(CCMU_CPUX_AXI_CFG_REG);
    reg_val &= ~(0x01 << 16);
    reg_val |=  (0x01 << 16);
    reg_val |=  (0x01 << 0);
    writel(reg_val, CCMU_CPUX_AXI_CFG_REG);
    //wait for timer stable
    for(i=0; i<0x400; i++);
    //write back PLL1
    reg_val = (0x01<<12)|(0x01<<31);
    writel(reg_val, CCMU_PLL_CPUX_CTRL_REG);
#ifndef CONFIG_FPGA
    do
    {
        reg_val = readl(CCMU_PLL_CPUX_CTRL_REG);
    }
    while(!(reg_val & (0x1 << 28)));
#endif
    //Modify AXI,AHB,APB divid freq
    clk_set_divd();
    //dma reset
    writel(readl(CCMU_AHB1_RST_REG0) | (1 << 6), CCMU_BUS_SOFT_RST_REG0);
    for(i=0;i<100;i++);
    //gating clock for dma pass
    writel(readl(CCMU_BUS_CLK_GATING_REG0) | (1 << 6), CCMU_BUS_CLK_GATING_REG0);
    writel(7, (0x01c20000+0x20));
#if 0
    //open MBUS,clk src is pll6
    writel(0x80000000, CCMU_MBUS_RST_REG);  //Assert mbus domain
    writel(0x81000002, CCMU_MBUS_CLK_REG);  //dram>600M, so mbus from 300M->400M
    //enable PLL6
    writel(readl(CCM_PLL6_MOD_CTRL) | (1U << 31), CCM_PLL6_MOD_CTRL);
#endif
    //enable PLL6
    writel(readl(CCMU_PLL_PERIPH0_CTRL_REG) | (1U << 31), CCMU_PLL_PERIPH0_CTRL_REG);
    __usdelay(100);

    writel(0x00000002, CCMU_MBUS_CLK_REG);
    __usdelay(1);//set MBUS divid factor
    writel(0x01000002, CCMU_MBUS_CLK_REG);
    __usdelay(1);//choose MBUS source
    writel(0x81000002, CCMU_MBUS_CLK_REG);
    __usdelay(1);//open MBUS clock

    //convert clock to COREPLL
    reg_val = readl(CCMU_CPUX_AXI_CFG_REG);
    reg_val &= ~(0x03 << 16);
    reg_val |=  (0x02 << 16);
    writel(reg_val, CCMU_CPUX_AXI_CFG_REG);

    return  ;
}

void reset_pll( void )
{
    writel(0x00010000, CCMU_CPUX_AXI_CFG_REG);
    writel(0x00001000, CCMU_PLL_CPUX_CTRL_REG);
    writel(0x00001010, CCMU_AHB1_APB1_CFG_REG);

    return ;
}

void set_gpio_gate(void)
{
    writel(readl(CCMU_BUS_CLK_GATING_REG2) | (1 << 5), CCMU_BUS_CLK_GATING_REG2);
}
