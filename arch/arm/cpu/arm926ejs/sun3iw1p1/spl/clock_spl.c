/*
**********************************************************************************************************************
*
*                                  the Embedded Secure Bootloader System
*
*
*                              Copyright(C), 2006-2014, Allwinnertech Co., Ltd.
*                                           All Rights Reserved
*
* File    :
*
* By      :
*
* Version : V2.00
*
* Date    :
*
* Descript:
**********************************************************************************************************************
*/
#include "common.h"
#include "asm/io.h"
#include "asm/arch/ccmu.h"
#include "asm/arch/timer.h"
#include "asm/arch/archdef.h"

extern int do_div( int divisor, int by);

void delay(uint us)
{
    uint i;
    for(i=0;i<us;i++);
        return ;
}


/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :calculate  AHB1 clk
*
*    note          :
*
*
************************************************************************************************************
*/
__u32 get_pll_ahb1_clk(void)
{
    __u32 ret;
    __u32 tmp,ahb1_apb_apb_clk,pll_periph_clk ;
    ahb1_apb_apb_clk =  readl(CCMU_AHB1_APB_HCLKC_CFG_REG);

    tmp = (ahb1_apb_apb_clk >>12) &0x3;
    switch(tmp)
    {
        case 0x1:          //0sc24M
        {
            ret = 24000000;
            break;
        }
        case  0x3:        //pll_periph/ahb1_div
        default:
        {
            tmp = ((ahb1_apb_apb_clk >> 6)& 0x3) + 1;
            pll_periph_clk = readl(CCMU_PLL_PERIPH_CTRL_REG);
            pll_periph_clk = 24000000 * (((pll_periph_clk>>8)&0x1f) + 1) * (((pll_periph_clk>>4)&3) + 1);
            ret = pll_periph_clk / tmp;         //AHB1_CLK_SRC_SEL
            tmp = ((ahb1_apb_apb_clk >> 4)& 0x3) + 1;
            ret = ret / tmp;

            break;
        }

    }

    return ret;
}


/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :calculate apb clk
*
*    note          :
*
*
************************************************************************************************************
*/
__u32 get_pll_apb_clk(void)
{
    __u32 ret, ahb1_apb_apb_clk, tmp;

    ahb1_apb_apb_clk = readl(CCMU_AHB1_APB_HCLKC_CFG_REG);

    ret = get_pll_ahb1_clk();

    tmp = (ahb1_apb_apb_clk>>8) & 0x3;
    if(tmp == 0)
        tmp = 1;
    ret = ret >> tmp;
    return ret;
}



void set_pll_cpux_axi(void)
{
    __u32 reg_val;
    //select CPUX  clock src: OSC24M
    writel((1<<16) , CCMU_CPUX_CLK_SRC_REG);
    delay(1000);

    //set PLL_CPUX, the default clk is 408M , PLL_OUTPUT= 24M*N*K/( M*P)
    //set N=17 k =1 M= 1
    writel((0x1000), CCMU_PLL_CPUX_CTRL_REG);
    //enable  pll
    writel((1<<31) | readl(CCMU_PLL_CPUX_CTRL_REG), CCMU_PLL_CPUX_CTRL_REG);
    //wait PLL_CPUX stable
#ifndef FPGA_PLATFORM
    while(!(readl(CCMU_PLL_CPUX_CTRL_REG) & (0x1<<28)));

#endif
    //set PLL0 to 408M, now BROM PLL0 is 204M
    //set and change cpu clk src to PLL_CPUX, PLL_CPUX:AXI0 = 408M:136M
    reg_val = readl(CCMU_CPUX_CLK_SRC_REG);
    reg_val &=  ~(3 << 16);
    reg_val |=  (2 << 16);
    writel(reg_val, CCMU_CPUX_CLK_SRC_REG);
    delay(1000);
    return ;
}



void set_pll_periph0_ahb_apb(void)
{
    uint reg_val;

    //set PLL6 to 600//816M   =24*n*k
    reg_val  = (600000000/24000000-1)<<8;
    reg_val |= (0x0<<4);
    reg_val |= (0x01<<18);
    reg_val |= (0x01<<31);
    writel(reg_val,CCMU_PLL_PERIPH_CTRL_REG);
    delay(1000);

    //set AHB and APB to PLL6

    //set AHB to 200//204M, set APB to 100AHB//102M AHB
    reg_val  = (0x03<<12);      //AHB from PLL_PERIPH/AHB_PRE_DIV
    reg_val |= ((3-1)<<6);      //AHB pre div
    reg_val |= ((1-1)<<4);      //AHB div
    reg_val |= ((2-1)<<8);      //APB div
    writel(reg_val,CCMU_AHB1_APB_HCLKC_CFG_REG);
    delay(1000);
    return ;

}
void set_pll_dma(void)
{
    //dma reset
    writel(readl(CCMU_BUS_SOFT_RST_REG0)  | (1 << 6), CCMU_BUS_SOFT_RST_REG0);
    delay(1000);
    //gating clock for dma pass
    writel(readl(CCMU_BUS_CLK_GATING_REG0) | (1 << 6), CCMU_BUS_CLK_GATING_REG0);
    delay(1000);
    return ;
}


void set_pll( void )
{
    set_pll_cpux_axi();
    set_pll_periph0_ahb_apb();
    set_pll_dma();

    return ;
}


void reset_pll( void )
{
    writel(0x10300, CCMU_CPUX_CLK_SRC_REG);
    return ;
}

void set_gpio_gate(void)
{
    return ;
}

