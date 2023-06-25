// SPDX-License-Identifier: GPL-2.0+
/*
 * drivers/clk/clk_fixed_factor.c
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 * Author: huafenghuang <huafenghuang@allwinnertech.com>
 *
 */
#include <div64.h>
#include <clk/clk_plat.h>
#include <clk/clk.h>

#define to_clk_fixed_factor(_hw) container_of(_hw, struct clk_fixed_factor, hw)
/*
 * DOC: basic fixed multiplier and divider clock that cannot gate
 *
 * Traits of this clock:
 * prepare - clk_prepare only ensures that parents are prepared
 * enable - clk_enable only ensures that parents are enabled
 * rate - rate is fixed.  clk->rate = parent->rate / div * mult
 * parent - fixed parent.  No clk_set_parent support
 */

static unsigned long clk_factor_recalc_rate(struct clk_hw *hw,
		unsigned long parent_rate)
{
	struct clk_fixed_factor *fix = to_clk_fixed_factor(hw);
	unsigned long long int rate;

	rate = (unsigned long long int)parent_rate * fix->mult;
	do_div(rate, fix->div);
	return (unsigned long)rate;
}

static long clk_factor_round_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long *prate)
{
	struct clk_fixed_factor *fix = to_clk_fixed_factor(hw);

	return (*prate / fix->div) * fix->mult;
}

static int clk_factor_set_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long parent_rate)
{
	/*
	 * We must report success but we can do so unconditionally because
	 * clk_factor_round_rate returns values that ensure this call is a
	 * nop.
	 */

	return 0;
}

const struct clk_ops clk_fixed_factor_ops = {
	.round_rate = clk_factor_round_rate,
	.set_rate = clk_factor_set_rate,
	.recalc_rate = clk_factor_recalc_rate,
};

struct clk_hw *clk_hw_register_fixed_factor(void *dev,
		const char *name, const char *parent_name, unsigned long flags,
		unsigned int mult, unsigned int div)
{
	struct clk_fixed_factor *fix;
	struct clk_init_data init;
	struct clk_hw *hw;
	struct clk *clk;

	fix = malloc(sizeof(*fix));
	if (fix) {
		memset(fix, 0, sizeof(struct clk_fixed_factor));
	} else {
		printf("%s: could not allocate fixed rate clk\n", __func__);
		return NULL;
	}

	/* struct clk_fixed_factor assignments */
	fix->mult = mult;
	fix->div = div;
	fix->hw.init = &init;

	init.name = name;
	init.ops = &clk_fixed_factor_ops;
	init.flags = flags | CLK_IS_BASIC;
	init.parent_names = &parent_name;
	init.num_parents = 1;

	hw = &fix->hw;
	clk = clk_register(hw);
	if (!clk) {
		free(fix);
		return NULL;
	}

	return hw;
}

struct clk *clk_register_fixed_factor(void *dev, const char *name,
		const char *parent_name, unsigned long flags,
		unsigned int mult, unsigned int div)
{
	struct clk_hw *hw;

	hw = clk_hw_register_fixed_factor(dev, name, parent_name, flags, mult,
					  div);
	if (!hw)
		return NULL;
	return hw->clk;
}

