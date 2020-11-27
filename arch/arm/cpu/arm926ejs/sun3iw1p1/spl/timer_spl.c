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
#include <common.h>
#include <asm/io.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/timer.h>

int timer_init(void)
{
    struct sunxi_timer_reg *timer_reg = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;

    writel(readl(CCMU_AVS_CLK_REG) | (1U << 31), CCMU_AVS_CLK_REG);

    timer_reg->tirqen  = 0;
    timer_reg->tirqsta |= 0x07;

    /* div cnt0 1200 to 20000hz, high 32 bit means 10000hz.*/
    /* div cnt1 12 to 2000000hz ,high 32 bit means 1000000hz */
    timer_reg->avs.div   = 0x000b04af;
    timer_reg->avs.cnt0  = 0;
    timer_reg->avs.cnt1  = 0;
    timer_reg->avs.ctl   = 3;

    return 0;
}



void __msdelay(unsigned long ms)
{
    u32 t1, t2;
    struct sunxi_timer_reg *timer_reg = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;

    t1 = timer_reg->avs.cnt0;
    t2 = t1 + ms * 10;
    do
    {
        t1 = timer_reg->avs.cnt0;
    }
    while(t2 >= t1);

    return ;
}


void __usdelay(unsigned long us)
{
    u32 t1, t2;
    struct sunxi_timer_reg *timer_reg = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;

    t1 = timer_reg->avs.cnt1;
    t2 = t1 + us;
    do
    {
        t1 = timer_reg->avs.cnt1;
    }
    while(t2 >= t1);

    return ;
}

void timer_exit(void)
{
    return ;
}

