/*
 * drivers/video/sunxi/disp2/disp/de/lowlevel_v2x/de_clock.h
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
#ifndef __DE_CLOCK_H__
#define __DE_CLOCK_H__

#include "../include.h"
#include "de_feat.h"

enum de_clk_id {
	DE_CLK_NONE = 0,

	DE_CLK_CORE0 = 1,
	DE_CLK_CORE1 = 2,
	DE_CLK_WB = 3,
};

struct de_clk_para {
	enum de_clk_id clk_no;
	u32 div;
	u32 ahb_gate_adr;
	u32 ahb_gate_shift;
	u32 ahb_reset_adr;
	u32 ahb_reset_shift;
	u32 dram_gate_adr;
	u32 dram_gate_shift;
	u32 mod_adr;
	u32 mod_enable_shift;
	u32 mod_div_adr;
	u32 mod_div_shift;
	u32 mod_div_width;
};

extern s32 de_clk_enable(u32 clk_no);
extern s32 de_clk_disable(u32 clk_no);
extern s32 de_clk_set_reg_base(uintptr_t reg_base);
#if defined(CONFIG_INDEPENDENT_DE)
extern s32 de1_clk_set_reg_base(uintptr_t reg_base);
#endif
#endif
