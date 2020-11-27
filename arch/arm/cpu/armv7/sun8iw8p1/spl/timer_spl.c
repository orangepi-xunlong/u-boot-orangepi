/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/platform.h>
#include <asm/arch/timer.h>

int timer_init(void)
{
    struct sunxi_timer_reg *timer_reg = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;

    writel(readl(CCMU_AVS_CLK_REG) | (1U << 31), CCMU_AVS_CLK_REG);

    timer_reg->tirqen  = 0;
    timer_reg->tirqsta |= 0x03f;
    /* start avs as counter */
    //ccm_reg->avs_clk_cfg |= (1 << 31);
    timer_reg->avs.ctl  = 3; //enable avs cnt0 and cnt1,source is 24M
    /* div cnt0 12000 to 2000hz, high 32 bit means 1000hz.*/
    /* div cnt 1 12 to 2000000hz ,high 32 bit means 1000000hz */
    timer_reg->avs.div   = 0xc2ee0;
    timer_reg->avs.cnt0  = 0;
    timer_reg->avs.cnt1  = 0;
    //writel(0, TMRC_AVS_COUNT0);
    //writel(0, TMRC_AVS_COUNT1);

    return 0;
}

void __msdelay(unsigned long ms)
{
    u32 t1, t2;
    struct sunxi_timer_reg *timer_reg = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;

    t1 = timer_reg->avs.cnt0;
    t2 = t1 + ms;
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
    ;
}


