/*
 * drivers/video/sunxi/disp2/disp/de/lowlevel_sun50iw1/disp_al.h
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
#ifndef _DISP_AL_H_
#define _DISP_AL_H_

#include "../include.h"
#include "de_feat.h"
#include "de_hal.h"
#include "de_enhance.h"
#include "de_wb.h"
#include "de_smbl.h"
#include "de_csc.h"
#include "de_lcd.h"
#include "de_dsi.h"
#include "de_clock.h"
#include "de_rtmx.h"

struct lcd_clk_info {
	disp_lcd_if lcd_if;
	int tcon_div;
	int lcd_div;
	int dsi_div;
	int dsi_rate;
};

int disp_al_manager_init(unsigned int disp);
int disp_al_manager_exit(unsigned int disp);
extern int disp_al_manager_apply(unsigned int disp, struct disp_manager_data *data);
extern int disp_al_layer_apply(unsigned int disp, struct disp_layer_config_data *data, unsigned int layer_num);
extern int disp_init_al(disp_bsp_init_para * para);
extern int disp_al_manager_sync(unsigned int disp);
extern int disp_al_manager_update_regs(unsigned int disp);
int disp_al_manager_query_irq(unsigned int disp);
int disp_al_manager_enable_irq(unsigned int disp);
int disp_al_manager_disable_irq(unsigned int disp);

int disp_al_enhance_apply(unsigned int disp, struct disp_enhance_config *config);
int disp_al_enhance_update_regs(unsigned int disp);
int disp_al_enhance_sync(unsigned int disp);
int disp_al_enhance_tasklet(unsigned int disp);

int disp_al_smbl_apply(unsigned int disp, struct disp_smbl_info *info);
int disp_al_smbl_update_regs(unsigned int disp);
int disp_al_smbl_sync(unsigned int disp);
int disp_al_smbl_get_status(unsigned int disp);
int disp_al_smbl_tasklet(unsigned int disp);

int disp_al_capture_init(unsigned int disp);
int disp_al_capture_exit(unsigned int disp);
int disp_al_capture_sync(u32 disp);
int disp_al_capture_apply(unsigned int disp, struct disp_capture_config *cfg);
int disp_al_capture_get_status(unsigned int disp);

int disp_al_lcd_cfg(u32 screen_id, disp_panel_para * panel, panel_extend_para *extend_panel);
int disp_al_lcd_cfg_ext(u32 screen_id, panel_extend_para *extend_panel);
int disp_al_lcd_enable(u32 screen_id, disp_panel_para * panel);
int disp_al_lcd_disable(u32 screen_id, disp_panel_para * panel);
int disp_al_lcd_query_irq(u32 screen_id, __lcd_irq_id_t irq_id, disp_panel_para * panel);
int disp_al_lcd_tri_busy(u32 screen_id, disp_panel_para * panel);
int disp_al_lcd_tri_start(u32 screen_id, disp_panel_para * panel);
int disp_al_lcd_io_cfg(u32 screen_id, u32 enable, disp_panel_para * panel);
int disp_al_lcd_get_cur_line(u32 screen_id, disp_panel_para * panel);
int disp_al_lcd_get_start_delay(u32 screen_id, disp_panel_para * panel);
int disp_al_lcd_get_clk_info(u32 screen_id, struct lcd_clk_info *info, disp_panel_para * panel);
int disp_al_lcd_enable_irq(u32 screen_id, __lcd_irq_id_t irq_id, disp_panel_para * panel);
int disp_al_lcd_disable_irq(u32 screen_id, __lcd_irq_id_t irq_id, disp_panel_para * panel);

int disp_al_hdmi_enable(u32 screen_id);
int disp_al_hdmi_disable(u32 screen_id);
int disp_al_hdmi_cfg(u32 screen_id, struct disp_video_timings *video_info);

int disp_al_tv_enable(u32 screen_id);
int disp_al_tv_disable(u32 screen_id);
int disp_al_tv_cfg(u32 screen_id, struct disp_video_timings *video_info);
int disp_al_vdevice_cfg(u32 screen_id, struct disp_video_timings *video_info, struct disp_vdevice_interface_para *para);
int disp_al_vdevice_enable(u32 screen_id);
int disp_al_vdevice_disable(u32 screen_id);

int disp_al_device_get_cur_line(u32 screen_id);
int disp_al_device_get_start_delay(u32 screen_id);
int disp_al_device_query_irq(u32 screen_id);
int disp_al_device_enable_irq(u32 screen_id);
int disp_al_device_disable_irq(u32 screen_id);
int disp_al_device_get_status(u32 screen_id);

int disp_al_get_fb_info(unsigned int sel, struct disp_layer_info *info);
int disp_al_get_display_size(unsigned int sel, unsigned int *width, unsigned int *height);
void disp_al_show_builtin_patten(u32 hwdev_index, u32 patten);

static inline s32 disp_al_capture_set_rcq_update(u32 disp, u32 en) { return 0; }

static inline u32 disp_al_capture_query_irq_state(u32 disp, u32 irq_state) { return 0; }

static inline s32 disp_al_capture_set_all_rcq_head_dirty(u32 disp, u32 dirty) { return 0; }

static inline s32 disp_al_capture_set_irq_enable(u32 disp, u32 irq_flag, u32 en) { return 0; }

static inline s32 disp_al_manager_set_rcq_update(u32 disp, u32 en) { return 0; }

static inline s32 disp_al_manager_set_all_rcq_head_dirty(u32 disp, u32 dirty) { return 0; }

static inline s32 disp_al_manager_set_irq_enable(u32 disp, u32 irq_flag, u32 en) { return 0; }

static inline u32 disp_al_manager_query_irq_state(u32 disp, u32 irq_state) { return 0; }

static inline int disp_al_device_set_de_id(u32 screen_id, u32 de_id) { return 0; }

static inline int disp_al_device_set_de_use_rcq(u32 screen_id, u32 use_rcq) { return 0; }

static inline int disp_al_device_set_output_type(u32 screen_id, u32 output_type) { return 0; }

#endif
