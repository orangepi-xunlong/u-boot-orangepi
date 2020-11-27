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
#include "asm/armv7.h"
#include "asm/arch/platform.h"
#include "asm/arch/ccmu.h"
#include "asm/arch/timer.h"
#include "asm/arch/archdef.h"

static void switch_cpu_axi_to_osc(u32 reg_addr)
{
	__u32 reg_val;
	reg_val = readl(reg_addr);
	reg_val &= ~(0x03 << 16);
    reg_val |= (0x01 << 16);
	writel(reg_val, reg_addr);
	__usdelay(1);

	reg_val = readl(reg_addr);
	reg_val &= ~((0x03 << 8) | (0x3 << 0));
	writel(reg_val, reg_addr);
	__usdelay(1);
}

static void switch_ahb1_apb1_to_osc(u32 reg_addr)
{
	__u32 reg_val;
	reg_val = readl(reg_addr);
	reg_val &= ~(0x03 << 12);
    reg_val |= (0x01 << 12);
	writel(reg_val, reg_addr);
	__usdelay(1);

	reg_val = readl(reg_addr);
	reg_val &= ~((0x03 << 8) | (0xf << 4));
    reg_val |= (0x1 << 4);
	writel(reg_val, reg_addr);
	__usdelay(1);
}

static void switch_ahb2_to_ahb1(u32 reg_addr)
{
	 __u32 reg_val;
	reg_val = readl(reg_addr);
	reg_val &= ~(0x03 << 0);
	writel(reg_val, reg_addr);
	__usdelay(1);

}

static void switch_apb2_to_osc(u32 reg_addr)
{
	 __u32 reg_val;
	reg_val = readl(reg_addr);
	reg_val &= ~(0x03 << 24);
    reg_val |= (0x01 << 24);
	writel(reg_val, reg_addr);
	__usdelay(1);

	reg_val = readl(reg_addr);
	reg_val &= ~((0x3 << 16) | (0x1f << 0));
	writel(reg_val, reg_addr);
	__usdelay(1);

}

static void switch_mubs_to_osc(u32 reg_addr)
{
	 __u32 reg_val;
	reg_val = readl(reg_addr);
	reg_val &= ~(0x03 << 24);
	writel(reg_val, reg_addr);
	__usdelay(1);

	reg_val = readl(reg_addr);
	reg_val &= ~((0x7 << 0));
	writel(reg_val, reg_addr);
	__usdelay(1);

}

static void switch_cpu_axi_to_pll(u32 reg_addr)
{
	__u32 reg_val;

	/*set axi div=0x1+1*/
	reg_val = readl(reg_addr);
	reg_val &=  ~(0x03 << 0);
	reg_val |=  (0x01 << 0);
	writel(reg_val, reg_addr);
	__usdelay(5);

	/*set apb div=default=0x3+1*/
	reg_val = readl(reg_addr);
	reg_val &=  ~(0x03 << 8);
	reg_val |=  (0x03 << 8);
	writel(reg_val, reg_addr);
	__usdelay(5);

    /*set cpu = pll_cpu*/
	reg_val = readl(reg_addr);
	reg_val &=  ~(0x03 << 16);
	reg_val |=  (0x03 << 16);
	writel(reg_val, reg_addr);
	__usdelay(5);

}

static void switch_ahb1_apb1_to_pll(u32 reg_addr, u32 secure_mode)
{
	__u32 reg_val;
	/* PLL0:AHB1:APB1 = 600M:200M:100M */
	reg_val = readl(reg_addr);
	reg_val &= ~((0x03 << 8) | (0x03 << 6) | (0x03 << 4));
	if (!secure_mode) {
		reg_val |= ((0x1 << 8) | (0x2 << 6) | (0x0 << 4));
	} else {
		reg_val |= ((0x1 << 8) | (0x2 << 6) | (0x1 << 4));
	}

	writel(reg_val, reg_addr);
	__usdelay(5);

	reg_val = readl(reg_addr);
	reg_val &= ~(0x03 << 12);
	reg_val |= (0x03 << 12);
	writel(reg_val, reg_addr);
	__usdelay(5);

}

static void close_pll(u32 reg_addr, u32 pll_enable)
{
	__u32 reg_val;
	reg_val = readl(reg_addr);
	reg_val &= ~(0x01 << pll_enable);
	writel(reg_val, reg_addr);
	__usdelay(1);

}

static void enable_cpu_pll(u32 reg_addr, u32 pll_enable, u32 lock_flag)
{
	__u32 reg_val;

	/*disable pll*/
	close_pll(reg_addr, pll_enable);

	/*set cpu_pll=408M,P=0x0+1/N=0x10+1/K=0x0+1/M=0x0+1 divide*/
	reg_val = readl(reg_addr);
	reg_val &= ~((0x03 << 16) | (0x1f << 8) | (0x3 << 4) | (0x3 << 0));
	reg_val |= ((0x0 << 16) | (0x10 << 8) | (0x0 << 4) | (0x0 << 0));
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

static void set_pll_cpux_axi(void)
{
	/*select CPUX  clock src: OSC24M,AXI divide ratio is 2, system apb clk ratio is 4*/
	switch_cpu_axi_to_osc(CCMU_CPUX_AXI_CFG_REG);

	/* set default val: clk is 408M  ,PLL_OUTPUT= 24M*N/( M*P)*/
	enable_cpu_pll(CCMU_PLL_C0CPUX_CTRL_REG, 31, 28);

	/*set and change cpu clk src to PLL_CPUX,  PLL_CPUX:AXI0 = 408M:204M*/
	switch_cpu_axi_to_pll(CCMU_CPUX_AXI_CFG_REG);
}

static void set_pll_periph0(void)
{
	__u32 reg_val;

	/*change ahb1~2/apb1~2/mbus src to OSC24M before set pll0 */
	switch_ahb1_apb1_to_osc(CCMU_AHB1_APB1_CFG_REG);
	switch_ahb2_to_ahb1(CCMU_AHB2_CFG_GREG);
	switch_apb2_to_osc(CCMU_APB2_CFG_GREG);
	switch_mubs_to_osc(CCMU_MBUS_CLK_REG);

	/* set default val*/
	close_pll(CCMU_PLL_PERIPH0_CTRL_REG, 31);
	reg_val = readl(CCMU_PLL_PERIPH0_CTRL_REG);
	reg_val &= ~((0x1f << 8) | (0x3 << 4) | (0x3 << 0));
	reg_val |= ((0x18 << 8) | (0x1 << 4) | (0x1 << 0));
	writel(reg_val, CCMU_PLL_PERIPH0_CTRL_REG);
	__usdelay(10);

	/* enabe PLL: 600M(1X)  1200M(2x) */
	reg_val = readl(CCMU_PLL_PERIPH0_CTRL_REG);
	reg_val |= (1 << 31);
	writel(reg_val, CCMU_PLL_PERIPH0_CTRL_REG);

#ifndef FPGA_PLATFORM
	while (!(readl(CCMU_PLL_PERIPH0_CTRL_REG) & (0x1 << 28))) {
		;
	}
	__usdelay(20);
#endif
}

static void set_ahb(u32 secure_mode)
{
	/* PLL0:AHB1:AHB2:APB1 = 600M:200M:200M:100M */
	switch_ahb1_apb1_to_pll(CCMU_AHB1_APB1_CFG_REG, secure_mode);
	switch_ahb2_to_ahb1(CCMU_AHB2_CFG_GREG);
}

static void set_pll_dma(void)
{
	/*dma reset*/
	writel(readl(CCMU_BUS_SOFT_RST_REG0)  | (1 << 6), CCMU_BUS_SOFT_RST_REG0);
	__usdelay(20);
	/*gating clock for dma pass*/
	writel(readl(CCMU_BUS_CLK_GATING_REG0) | (1 << 6), CCMU_BUS_CLK_GATING_REG0);
	__usdelay(20);
}

static void set_pll_mbus(void)
{
	__u32 reg_val;

    /* open MBUS clock */
	reg_val = readl(CCMU_MBUS_CLK_REG);
	reg_val &= (~(0X01 << 31));
	writel(reg_val, CCMU_MBUS_CLK_REG);
	__usdelay(1);

	/* set MBUS div m=2*/
	reg_val = readl(CCMU_MBUS_CLK_REG);
	reg_val &= (~(7 << 0));
    reg_val |= (2 << 0);
	writel(reg_val, CCMU_MBUS_CLK_REG);
	__usdelay(5);

	/* set MBUS clock source to pll6(2x), mbus=pll0/(m+1) = 400M*/
	reg_val = readl(CCMU_MBUS_CLK_REG);
    reg_val &= (~(3 << 24));
	reg_val |= (1 << 24);
	writel(reg_val, CCMU_MBUS_CLK_REG);
	__usdelay(5);

	/* open MBUS clock */
	reg_val = readl(CCMU_MBUS_CLK_REG);
	reg_val |= (0X01 << 31);
	writel(reg_val, CCMU_MBUS_CLK_REG);
	__usdelay(5);

}

static void set_gpio(void)
{
    /* 打开CPUX GPIO */
    writel(readl(CCMU_BUS_CLK_GATING_REG2)  |  (1 << 5), CCMU_BUS_CLK_GATING_REG2);
    /* 打开CPUs GPIO */
    writel(readl(SUNXI_RPRCM_BASE + 0x28)   |  (1 << 0), SUNXI_RPRCM_BASE + 0x28);
}
/*******************************************************************************
*函数名称: set_pll
*函数原型：void set_pll( void )
*函数功能: 调整CPU频率
*入口参数: void
*返 回 值: void
*备    注:
*******************************************************************************/
void set_pll(void)
{
    __msdelay(300);
    printf("set pll start\n");
    set_pll_cpux_axi();
    set_pll_periph0();
    set_ahb(0);
	set_pll_dma();
	set_pll_mbus();
    set_gpio();
    CP15DMB;
	CP15ISB;
	printf("set pll end\n");

    return  ;
}

/*******************************************************************************
*函数名称: set_pll_in_secure_mode
*函数原型：void set_pll_in_secure_mode( void )
*函数功能: 调整CPU频率
*入口参数: void
*返 回 值: void
*备    注:
*******************************************************************************/
void set_pll_in_secure_mode(void)
{
    __msdelay(300);
    printf("set pll start\n");
    set_pll_cpux_axi();
    set_pll_periph0();
    set_ahb(1);
	set_pll_dma();
	set_pll_mbus();
    set_gpio();
    CP15DMB;
	CP15ISB;
	printf("set pll end\n");

    return  ;
}

/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
void reset_pll(void)
{
    /*cpu axi to osc*/
	printf("reset pll\n");
    switch_cpu_axi_to_osc(CCMU_CPUX_AXI_CFG_REG);
    switch_ahb1_apb1_to_osc(CCMU_AHB1_APB1_CFG_REG);
    switch_ahb2_to_ahb1(CCMU_AHB2_CFG_GREG);
    switch_apb2_to_osc(CCMU_APB2_CFG_GREG);
    switch_mubs_to_osc(CCMU_MBUS_CLK_REG);

	return ;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
void set_gpio_gate(void)
{
	writel(readl(CCMU_BUS_CLK_GATING_REG2)		|	(1 << 5), CCMU_BUS_CLK_GATING_REG2);
	writel(readl(SUNXI_RPRCM_BASE + 0x28)	|		0x01, SUNXI_RPRCM_BASE + 0x28);
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
void set_ccmu_normal(void)
{
	writel(7, CCMU_SEC_SWITCH_REG);
	writel(0xfff, SUNXI_DMA_BASE + 0x20);
}

