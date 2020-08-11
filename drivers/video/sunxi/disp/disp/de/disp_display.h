/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __DISP_DISPLAY_H__
#define __DISP_DISPLAY_H__

#include "disp_private.h"

struct __disp_screen_t {
	bool have_cfg_reg;
	u32 cache_flag;
	u32 cfg_cnt;
#ifdef __LINUX_PLAT__
	spinlock_t flag_lock;
	u32 wait_count;
	wait_queue_head_t wait;
#endif
	bool vsync_event_en;
	bool dvi_enable;
};

struct __disp_dev_t {
	struct __disp_bsp_init_para init_para;	/* para from driver */
	struct __disp_screen_t screen[3];
	u32 print_level;
	u32 lcd_registered[3];
	u32 hdmi_registered;
	u32 edp_registered;
};

struct disp_fps_data {
	u32 last_time;
	u32 current_time;
	u32 counter;
};

extern struct disp_fps_data fps_data[3];

extern struct __disp_dev_t gdisp;
extern struct __disp_al_private_data *disp_al_get_priv(u32 screen_id);
extern s32 Display_set_fb_timming(u32 sel);

s32 disp_init_connections(void);
s32 bsp_disp_cfg_get(u32 screen_id);
void sync_event_proc(u32 screen_id);
void sync_finish_event_proc(u32 screen_id);
#ifdef CONFIG_DEVFREQ_DRAM_FREQ_IN_VSYNC
s32 bsp_disp_get_vb_time(void);
s32 bsp_disp_get_next_vb_time(void);
s32 bsp_disp_is_in_vb(void);
#endif
s32 bsp_disp_get_panel_info(u32 screen_id, disp_panel_para *info);
void LCD_OPEN_FUNC(u32 screen_id, LCD_FUNC func, u32 delay);
void LCD_CLOSE_FUNC(u32 screen_id, LCD_FUNC func, u32 delay);

#endif
