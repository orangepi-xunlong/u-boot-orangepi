/*
 * include/clk/clk.h
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef __CLK__H
#define __CLK__H
#include<clk/clk_plat.h>

void clk_put(struct clk *clk);
int clk_set_rate(struct clk *clk, unsigned long rate);
unsigned long clk_get_rate(struct clk *clk);
struct clk *clk_get(void *dev, const char *con_id);
struct clk *clk_register(struct clk_hw *hw);
int clk_set_parent(struct clk *clk, struct clk *parent);
struct clk *clk_get_parent(struct clk *clk);
int clk_init(void);
int clk_prepare_enable(struct clk *clk);
int clk_disable(struct clk *clk);
int of_periph_clk_config_setup(int node_offset);
struct clk* of_clk_get(int node_offset, int index);
long clk_round_rate(struct clk *clk, unsigned long rate);

#endif
