/*
 * drivers/video/sunxi/disp2/disp/de/disp_vga.h
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
#ifndef __DISP_VGA_H__
#define __DISP_VGA_H__

#include "disp_private.h"
#include "disp_display.h"

struct disp_vga_private_data {
	bool enabled;
	bool suspended;

	enum disp_tv_mode vga_mode;
	struct disp_tv_func tv_func;
	struct disp_video_timings *video_info;

	struct disp_clk_info lcd_clk;
	struct clk *clk;
	struct clk *clk_parent;

	s32 frame_per_sec;
	u32 usec_per_line;
	u32 judge_line;

	u32 irq_no;
	spinlock_t data_lock;
	struct mutex mlock;
};

s32 disp_init_vga(void);

#endif
