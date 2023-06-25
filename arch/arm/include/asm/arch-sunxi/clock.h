/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 */

#ifndef _SUNXI_CLOCK_H
#define _SUNXI_CLOCK_H

#include <linux/types.h>

#define CLK_GATE_OPEN			0x1
#define CLK_GATE_CLOSE			0x0

/* clock control module regs definition */
#if defined(CONFIG_MACH_SUN8I_A83T)
#include <asm/arch/clock_sun8i_a83t.h>
#elif defined(CONFIG_MACH_SUN50IW3) || defined(CONFIG_MACH_SUN8IW16) || \
	defined(CONFIG_MACH_SUN8IW19)
#include <asm/arch/clock_sun50iw3.h>
#elif defined(CONFIG_MACH_SUN6I) || defined(CONFIG_MACH_SUN8I) || \
      defined(CONFIG_MACH_SUN50I)
#include <asm/arch/clock_sun6i.h>
#elif defined(CONFIG_MACH_SUN9I)
#include <asm/arch/clock_sun9i.h>
#elif defined(CONFIG_MACH_SUN8IW18)
#include <asm/arch/clock_sun8iw18.h>
#elif defined(CONFIG_MACH_SUN50IW9)
#include <asm/arch/clock_sun50iw9.h>
#elif defined(CONFIG_MACH_SUN50IW10)
#include <asm/arch/clock_sun50iw10.h>
#elif defined(CONFIG_MACH_SUN55IW3)
#include <asm/arch/plat-sun55iw3p1/clock_sun55iw3.h>
#elif defined(CONFIG_MACH_SUN50IW11)
#include <asm/arch/clock_sun50iw11.h>
#elif defined(CONFIG_MACH_SUN50IW12)
#include <asm/arch/clock_sun50iw12.h>
#elif defined(CONFIG_MACH_SUN8IW15)
#include <asm/arch/clock_sun8iw15.h>
#elif defined(CONFIG_MACH_SUN8IW7)
#include <asm/arch/clock_sun6i.h>
#elif defined(CONFIG_MACH_SUN20IW1)
#include <asm/arch/clock_sun20iw1.h>
#elif defined(CONFIG_MACH_SUN8IW20)
#include <asm/arch/clock_sun8iw20.h>
#elif defined(CONFIG_MACH_SUN8IW21)
#include <asm/arch/clock_sun8iw21.h>
#elif defined(CONFIG_MACH_SUN50IW5)
#include <asm/arch/clock_sun50iw5.h>
#elif defined(CONFIG_MACH_SUN8IW11)
#include <asm/arch/clock_sun6i.h>
#else
#include <asm/arch/clock_sun4i.h>
#endif

struct core_pll_freq_tbl {
    int FactorN;
    int FactorK;
    int FactorM;
    int FactorP;
    int pading;
};

#ifndef __ASSEMBLY__
int clock_init(void);
int clock_twi_onoff(int port, int state);
void clock_set_de_mod_clock(u32 *clk_cfg, unsigned int hz);
void clock_init_safe(void);
void clock_init_sec(void);
void clock_init_uart(void);

uint clock_get_corepll(void);
int clock_set_corepll(int frequency);
uint clock_get_pll6(void);
uint clock_get_ahb(void);
uint clock_get_apb1(void);
uint clock_get_apb2(void);
uint clock_get_axi(void);
uint clock_get_mbus(void);
void clock_open_timer(int timernum);
void clock_close_timer(int timernum);
#endif

#endif /* _SUNXI_CLOCK_H */
