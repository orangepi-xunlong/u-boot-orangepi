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
#include "asm/arch/cpu.h"
#include "asm/arch/ccmu.h"
#include "asm/arch/timer.h"

static bool ahb_secure;

void set_ahb(void)
{
    /* set AHB1/APB1 clock  divide ratio */
	/* ahb1 clock src is PLL6,                           (0x02 << 12) */
	/* apb1 clk src is ahb1 clk src, divide  ratio is 2  (1<<8) */
	/* ahb1 pre divide  ratio is 2:    0:1  , 1:2,  2:3,   3:4 (2<<6) */
    /* PLL6:AHB1:APB1 = 600M:200M:100M */
    writel((0x01 << 12) | (1<<8) | (2<<6) | (1<<4), CCMU_AHB1_APB1_CFG_REG);
    __usdelay(100);
}

void set_pll_ahb_for_secure(void)
{
    /* set AHB1/APB1 clock  divide ratio */
	/* ahb1 clock src is PLL6,                           (0x02 << 12) */
	/* apb1 clk src is ahb1 clk src, divide  ratio is 2  (1<<8) */
	/* ahb1 pre divide  ratio is 2:    0:1  , 1:2,  2:3,   3:4 (2<<6) */
    /* PLL6:AHB1:APB1 = 1200M:100M:50M ,set MBUS=400M */
    writel((0x01 << 12) | (1<<8) | (2<<6) | (2<<4), CCMU_AHB1_APB1_CFG_REG);
    __usdelay(100);
    ahb_secure = 0;
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
	__u32 reg_val;

	/* select C0_CPUX  clock src: OSC24M，AXI divide ratio is 2 */
	/* cpu/axi clock ratio */
	writel((1<<16 | 1<<0), CCMU_CPUX_AXI_CFG_REG);
    /* set PLL_C0CPUX, the  default  clk is 408M  ,PLL_OUTPUT= 24M*N/P */
	reg_val = 0x82001100;
	writel(reg_val, CCMU_PLL_C0CPUX_CTRL_REG);
    /* wait pll1 stable */
#ifndef CONFIG_SUNXI_FPGA
	do {
		reg_val = readl(CCMU_PLL_STB_STATUS_REG);
	} while (!(reg_val & 0x1));
#endif

	/* set PLL_C1CPUX, the  default  clk is 408M  ,PLL_OUTPUT= 24M*N/P */
	reg_val = 0x82001100;
	writel(reg_val, CCMU_PLL_C1CPUX_CTRL_REG);
/* wait pll1 stable */
#ifndef CONFIG_SUNXI_FPGA
	do {
		reg_val = readl(CCMU_PLL_STB_STATUS_REG);
	} while (!(reg_val & 0x2));
#endif
	/* enable pll_hsic, default is 480M */
	writel(0x42800, CCMU_PLL_HSIC_CTRL_REG); /* set default value */
	writel(readl(CCMU_PLL_HSIC_CTRL_REG) | (1U << 31),
	       CCMU_PLL_HSIC_CTRL_REG);
#ifndef CONFIG_SUNXI_FPGA
	do {
		reg_val = readl(CCMU_PLL_STB_STATUS_REG);
	} while (!(reg_val & (1 << 8)));
#endif

	/* cci400 clk src is pll_hsic: (2<<24) , div is 1:(0<<0) */
	CP15DMB;
	CP15ISB;
	reg_val = readl(CCMU_CCI400_CFG_REG);
	/* src is osc24M */
    if (!(reg_val & (0x3 << 24))) {
	    writel(0x0, CCMU_CCI400_CFG_REG);
	    __usdelay(50);
    }
    writel((2 << 24 | 0 << 0), CCMU_CCI400_CFG_REG);
    __usdelay(100);
    CP15DMB;
    CP15ISB;
    /* change ahb src before set pll6 */
    writel((0x01 << 12) | (readl(CCMU_AHB1_APB1_CFG_REG)&(~(0x3<<12))), CCMU_AHB1_APB1_CFG_REG);
    /* enable PLL6 */
    writel((0x32<<8) | (1U << 31), CCMU_PLL_PERIPH_CTRL_REG);
    __usdelay(100);
#ifndef CONFIG_SUNXI_FPGA
    do {
	    reg_val = readl(CCMU_PLL_STB_STATUS_REG);
    } while (!(reg_val & (1 << 6)));
#endif

    if (ahb_secure) {
	    /* printf("set ahb in secure\n"); */
	    set_pll_ahb_for_secure();
    } else {
	    /* printf("set ahb in normal\n"); */
	    set_ahb();
    }
    writel((0x02 << 12) | readl(CCMU_AHB1_APB1_CFG_REG),
	   CCMU_AHB1_APB1_CFG_REG);
    /* set and change cpu clk src to pll1,  PLL1:AXI0 = 408M:204M */
    reg_val = readl(CCMU_CPUX_AXI_CFG_REG);
    reg_val &= ~(3 << 0);
    reg_val |= (1 << 0);  /* axi0  clk src is C0_CPUX , divide  ratio is 2 */
    reg_val |= (1 << 12); /* C0_CPUX clk src is PLL_C0CPUX */
    writel(reg_val, CCMU_CPUX_AXI_CFG_REG);
    /* set  C1_CPUX clk src is PLL_C1CPUX */
    reg_val = readl(CCMU_CPUX_AXI_CFG_REG);
    reg_val &= ~(3<<16);
    reg_val |= 1<<16;          /* axi1  clk src is C1_CPUX , divide  ratio is 2 */
    reg_val |= 1<<28;          /* C1_CPUX clk src is PLL_C1CPUX */
    writel(reg_val, CCMU_CPUX_AXI_CFG_REG);
    __usdelay(1000);
	/* ----DMA function-------- */
	/* dma reset */
	writel(readl(CCMU_BUS_SOFT_RST_REG0)  | (1 << 6), CCMU_BUS_SOFT_RST_REG0);
	__usdelay(20);
	/* gating clock for dma pass */
	writel(readl(CCMU_BUS_CLK_GATING_REG0) | (1 << 6), CCMU_BUS_CLK_GATING_REG0);
    /* auto gating disable */
	writel(7, (SUNXI_DMA_BASE+0x20));
	/* reset mbus domain */
	writel(0x80000000, CCMU_MBUS_RST_REG);
    /* open MBUS,clk src is pll6 , pll6/(m+1) = 300M */
	/* writel(0x81000002, CCMU_MBUS_CLK_REG); */
	writel(0x00000002, CCMU_MBUS_CLK_REG); __usdelay(1);/* set MBUS div */
	writel(0x01000002, CCMU_MBUS_CLK_REG); __usdelay(1);/* set MBUS clock source */
	writel(0x81000002, CCMU_MBUS_CLK_REG); __usdelay(1);/* open MBUS clock */

    /* open TIMESTAMP ,aw1673_cpux_cfg.pdf descript this register */
    writel(1, 0x01720000);

	return ;
}

void set_pll_in_secure_mode(void)
{
    ahb_secure = 1;
    set_pll();
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
void reset_pll( void )
{
    /* 切换CPU时钟源为24M */
    writel(0x00000000, CCMU_CPUX_AXI_CFG_REG);
    /* 还原PLL_C0CPUX为默认值 */
	writel(0x02001100, CCMU_PLL_C0CPUX_CTRL_REG);

	return ;
}
/*
************************************************************************************************************
*
*                                             function
*
*函数名称：
*
*参数列表：
*
*返回值  ：
*
*说明    ：
*
*
************************************************************************************************************
*/
void set_gpio_gate(void)
{

}
