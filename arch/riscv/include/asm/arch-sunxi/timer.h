/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Configuration settings for the Allwinner A10-evb board.
 */

#ifndef _SUNXI_TIMER_H_
#define _SUNXI_TIMER_H_

#ifndef __ASSEMBLY__

#include <linux/types.h>
#include <asm/arch/watchdog.h>

/* General purpose timer */
struct sunxi_timer {
	volatile u32 ctl;
	volatile u32 inter;
	volatile u32 val;
	u8 res[4];
};

/* Audio video sync*/
struct sunxi_avs {
	volatile u32 ctl;		/* 0x80 */
	volatile u32 cnt0;		/* 0x84 */
	volatile u32 cnt1;		/* 0x88 */
	u32 div;		/* 0x8c */
};

/* 64 bit counter */
struct sunxi_64cnt {
	volatile u32 ctl;		/* 0xa0 */
	volatile u32 lo;			/* 0xa4 */
	volatile u32 hi;			/* 0xa8 */
};

/* Rtc */
struct sunxi_rtc {
	volatile u32 ctl;		/* 0x100 */
	volatile u32 yymmdd;		/* 0x104 */
	volatile u32 hhmmss;		/* 0x108 */
};

/* Alarm */
struct sunxi_alarm {
	u32 ddhhmmss;		/* 0x10c */
	u32 hhmmss;		/* 0x110 */
	u32 en;			/* 0x114 */
	u32 irqen;		/* 0x118 */
	u32 irqsta;		/* 0x11c */
};

/* Timer general purpose register */
struct sunxi_tgp {
	u32 tgpd;
};

#if defined(CONFIG_MACH_SUN20IW1) || defined (CONFIG_MACH_SUN8IW20)
struct sunxi_timer_reg {
	volatile u32 tirqen;		/* 0x00 */
	volatile u32 tirqsta;	/* 0x04 */
	uint     res1[2];
	struct sunxi_timer timer[2];		/* We have 2 timers */
	uint  	 res2[0x70/4];			/* 0x70 */
	struct sunxi_wdog wdog[1];		/* 0xa0 */
	struct sunxi_avs avs;			/* 0xc0 */
};
#else
struct sunxi_timer_reg {
	u32 tirqen;		/* 0x00 */
	u32 tirqsta;		/* 0x04 */
	u8 res1[8];
	struct sunxi_timer timer[6];	/* We have 6 timers */
	u8 res2[16];
	struct sunxi_avs avs;
#if defined(CONFIG_SUNXI_GEN_SUN4I) || defined(CONFIG_MACH_SUN8I_R40)
	struct sunxi_wdog wdog;	/* 0x90 */
	/* XXX the following is not accurate for sun5i/sun7i */
	struct sunxi_64cnt cnt64;	/* 0xa0 */
	u8 res4[0x58];
	struct sunxi_rtc rtc;
	struct sunxi_alarm alarm;
	struct sunxi_tgp tgp[4];
	u8 res5[8];
	u32 cpu_cfg;
#elif defined(CONFIG_SUNXI_GEN_SUN6I)
	u8 res3[16];
	struct sunxi_wdog wdog[5];	/* We have 5 watchdogs */
#endif
};
#endif


struct timer_list {
	unsigned int expires;
	void (*function)(void *data);
	unsigned long data;
	int   timer_num;
};

extern int  timer_init(void);

extern void timer_exit(void);

extern void watchdog_disable(void);

extern void watchdog_enable(void);

extern void init_timer(struct timer_list *timer);

extern void add_timer(struct timer_list *timer);

extern void del_timer(struct timer_list *timer);

extern void __usdelay(unsigned long usec);

extern void __msdelay(unsigned long msec);
extern ulong get_timer_masked(void);

#endif /* __ASSEMBLY__ */

#endif
