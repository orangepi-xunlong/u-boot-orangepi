/*
 * drivers/video/sunxi/disp2/disp/de/disp_private.h
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
#ifndef _DISP_PRIVATE_H_
#define _DISP_PRIVATE_H_

#include <common.h>
#include "disp_features.h"

#if defined(DE_VERSION_V33X)
#include "./lowlevel_v33x/disp_al.h"
#elif defined(CONFIG_MACH_SUN8IW10)
#include "./lowlevel_sun8iw10/disp_al.h"
#elif defined(CONFIG_MACH_SUN8IW11) || defined(CONFIG_MACH_SUN8IW15)
#include "./lowlevel_v2x/disp_al.h"
#elif defined(CONFIG_MACH_SUN8IW12) || defined(CONFIG_MACH_SUN8IW16) \
   || defined(CONFIG_MACH_SUN8IW19) || defined(CONFIG_MACH_SUN20IW1) \
   || defined(CONFIG_MACH_SUN8IW20)
#include "./lowlevel_v2x/disp_al.h"
#elif defined(CONFIG_MACH_SUN8IW17) || defined(CONFIG_MACH_SUN8IW21)
#include "./lowlevel_v2x/disp_al.h"
#elif defined(CONFIG_MACH_SUN50IW1)
#include "./lowlevel_sun50iw1/disp_al.h"
#elif defined(CONFIG_MACH_SUN50IW2)
#include "./lowlevel_v2x/disp_al.h"
#elif defined(CONFIG_MACH_SUN50IW10)
#include "./lowlevel_v2x/disp_al.h"
#elif defined(CONFIG_MACH_SUN8IW7)
#include "./lowlevel_v2x/disp_al.h"
#elif defined(CONFIG_MACH_SUN50IW3)
#include "./lowlevel_v3x/disp_al.h"
#elif defined(CONFIG_MACH_SUN50IW6)
#include "./lowlevel_v3x/disp_al.h"
#elif defined(CONFIG_MACH_SUN8IW6)
#include "./lowlevel_v2x/disp_al.h"
#else
#error "undefined platform!!!"
#endif

struct disp_device *disp_get_lcd(u32 disp);
struct disp_device *disp_get_direct_lcd_compat(u32 disp, u32 index);

struct disp_device *disp_get_hdmi(u32 disp);

struct disp_manager *disp_get_layer_manager(u32 disp);

struct disp_layer *disp_get_layer(u32 disp, u32 chn, u32 layer_id);
struct disp_layer *disp_get_layer_1(u32 disp, u32 layer_id);
struct disp_smbl *disp_get_smbl(u32 disp);
struct disp_enhance *disp_get_enhance(u32 disp);
struct disp_capture *disp_get_capture(u32 disp);

s32 disp_delay_ms(u32 ms);
s32 disp_delay_us(u32 us);
s32 disp_init_lcd(disp_bsp_init_para *para);
s32 disp_init_hdmi(disp_bsp_init_para *para);
s32 disp_init_tv(void);//(disp_bsp_init_para * para);
s32 disp_tv_set_func(struct disp_device *ptv, struct disp_tv_func *func);
s32 disp_init_tv_para(disp_bsp_init_para *para);
s32 disp_tv_set_hpd(struct disp_device *ptv, u32 state);
s32 disp_init_vga(void);
s32 disp_init_edp(disp_bsp_init_para *para);

s32 disp_init_feat(struct disp_feat_init *feat_init);
s32 disp_init_mgr(disp_bsp_init_para *para);
s32 disp_init_enhance(disp_bsp_init_para *para);
s32 disp_init_smbl(disp_bsp_init_para *para);
s32 disp_init_capture(disp_bsp_init_para *para);
#ifdef CONFIG_EINK_PANEL_USED
int eink_display_one_frame(struct disp_eink_manager *manager);
struct disp_eink_manager *disp_get_eink_manager(unsigned int disp);
s32 disp_init_eink(disp_bsp_init_para *para);
s32 write_edma(struct disp_eink_manager *manager);
struct disp_eink_manager *disp_get_eink_manager(unsigned int disp);
s32 disp_mgr_clk_disable(struct disp_manager *mgr);
int __eink_clk_disable(struct disp_eink_manager *manager);

#endif

#include "disp_device.h"

u32 dump_layer_config(struct disp_layer_config_data *data);

void *disp_vmap(unsigned long phys_addr, unsigned long size);
void disp_vunmap(const void *vaddr);
#endif

