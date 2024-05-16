// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022-2023 StarFive Technology Co., Ltd.
 * Author:	yanhong <yanhong.wang@starfivetech.com>
 *
 */

#ifndef __CLK_STARFIVE_H
#define __CLK_STARFIVE_H

enum starfive_pll_type {
	PLL0 = 0,
	PLL1,
	PLL2,
	PLL_MAX = PLL2
};

struct starfive_pll_freq {
	u64 freq;
	u32 prediv;
	u32 fbdiv;
	u32 frac;
	u32 postdiv1;
	u32 dacpd; /* Both daxpd and dsmpd set 1 while integer multiple mode */
	u32 dsmpd; /* Both daxpd and dsmpd set 0 while fraction multiple mode */
};

u64 starfive_jh7110_pll_get_rate(enum starfive_pll_type pll,
				struct starfive_pll_freq *conf);
int starfive_jh7110_pll_set_rate(enum starfive_pll_type pll, u64 rate);

#endif
