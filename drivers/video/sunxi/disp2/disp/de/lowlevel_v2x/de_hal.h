/*
 * drivers/video/sunxi/disp2/disp/de/lowlevel_v2x/de_hal.h
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
#ifndef __DE_HAL_H__
#define __DE_HAL_H__
#include "de_rtmx.h"
#include "de_scaler.h"
#include "de_csc.h"
#if defined(CONFIG_EINK_PANEL_USED)
#include "de_eink.h"
#include "de_wb_type.h"
#include "disp_waveform.h"
#include "disp_eink_data.h"
/* #include "de_wb_type.h" */
#endif

int de_al_mgr_apply(unsigned int screen_id,
		    struct disp_manager_data *data);
int de_al_mgr_apply_color(unsigned int screen_id,
			  struct disp_csc_config *cfg);
int de_al_init(disp_bsp_init_para *para);
int de_al_lyr_apply(unsigned int screen_id,
		    struct disp_layer_config_data *data,
			   unsigned int layer_num, bool direct_show);
int de_al_mgr_sync(unsigned int screen_id);
int de_al_mgr_update_regs(unsigned int screen_id);
int de_al_query_irq(unsigned int screen_id);
int de_al_enable_irq(unsigned int screen_id, unsigned int en);
int de_update_device_fps(unsigned int sel, u32 fps);
int de_update_clk_rate(u32 rate);
int de_get_clk_rate(void);

int de_enhance_set_size(unsigned int screen_id, struct disp_rect *size);
int de_enhance_set_format(unsigned int screen_id, unsigned int format);

#endif
