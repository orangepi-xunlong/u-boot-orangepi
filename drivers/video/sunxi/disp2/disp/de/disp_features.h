/*
 * drivers/video/sunxi/disp2/disp/de/disp_features.h
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
#ifndef _DISP_FEATURES_H_
#define _DISP_FEATURES_H_

#include <common.h>

#if defined(CONFIG_MACH_SUN50IW9) || defined(CONFIG_MACH_SUN50IW9)
#ifndef DE_VERSION_V33X
#define DE_VERSION_V33X
#endif
#include "./lowlevel_v33x/de330/de_feat.h"
#include "./lowlevel_v33x/tcon_feat.h"
#elif defined(CONFIG_MACH_SUN8IW10)
#include "./lowlevel_sun8iw10/de_feat.h"
#elif defined(CONFIG_MACH_SUN8IW11) || defined(CONFIG_MACH_SUN8IW15)
#include "./lowlevel_v2x/de_feat.h"
#elif defined(CONFIG_MACH_SUN8IW12) || defined(CONFIG_MACH_SUN8IW16) \
   || defined(CONFIG_MACH_SUN8IW19) || defined(CONFIG_MACH_SUN8IW20)\
   || defined(CONFIG_MACH_SUN20IW1)
#include "./lowlevel_v2x/de_feat.h"
#elif defined(CONFIG_MACH_SUN8IW17) || defined(CONFIG_MACH_SUN8IW21)
#include "./lowlevel_v2x/de_feat.h"
#elif defined(CONFIG_MACH_SUN50IW1)
#include "./lowlevel_sun50iw1/de_feat.h"
#elif defined(CONFIG_MACH_SUN50IW2)
#include "./lowlevel_v2x/de_feat.h"
#elif defined(CONFIG_MACH_SUN50IW10)
#include "./lowlevel_v2x/de_feat.h"
#elif defined(CONFIG_MACH_SUN8IW7)
#include "./lowlevel_v2x/de_feat.h"
#elif defined(CONFIG_MACH_SUN50IW3)
#include "./lowlevel_v3x/de_feat.h"
#elif defined(CONFIG_MACH_SUN50IW6)
#include "./lowlevel_v3x/de_feat.h"
#elif defined(CONFIG_MACH_SUN8IW6)
#include "./lowlevel_v2x/de_feat.h"
#else
#error "undefined platform!!!"
#endif

#define DISP_DEVICE_NUM DEVICE_NUM
#define DISP_SCREEN_NUM DE_NUM
#define DISP_WB_NUM DE_NUM

struct disp_features {
	const int num_screens;
	const int *num_channels;
	const int *num_layers;
	const int *is_support_capture;
	const int *supported_output_types;
};

struct disp_feat_init {
	unsigned int chn_cfg_mode;
};

int bsp_disp_feat_get_num_screens(void);
int bsp_disp_feat_get_num_devices(void);
int bsp_disp_feat_get_num_channels(unsigned int disp);
int bsp_disp_feat_get_num_layers(unsigned int screen_id);
int bsp_disp_feat_get_num_layers_by_chn(unsigned int disp, unsigned int chn);
int bsp_disp_feat_is_supported_output_types(unsigned int screen_id, unsigned int output_type);
int bsp_disp_feat_is_support_capture(unsigned int disp);
int disp_feat_is_support_smbl(unsigned int disp);
int disp_init_feat(struct disp_feat_init *feat_init);
int disp_feat_is_using_rcq(unsigned int disp);
int disp_feat_is_using_wb_rcq(unsigned int wb);

#endif

