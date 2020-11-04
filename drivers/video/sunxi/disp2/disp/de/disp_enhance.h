/*
 * drivers/video/sunxi/disp2/disp/de/disp_enhance.h
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
#ifndef _DISP_ENHANCE_H_
#define _DISP_ENHANCE_H_

#include "disp_private.h"

static s32 disp_enhance_shadow_protect(struct disp_enhance *enhance, bool protect);
s32 disp_init_enhance(disp_bsp_init_para * para);
#if 0
//for inner debug
typedef struct
{
	//basic adjust
	u32         bright;
	u32         contrast;
	u32         saturation;
	u32         hue;
	u32         mode;
	//ehnance
	u32         sharp;	//0-off; 1~3-on.
	u32         auto_contrast;	//0-off; 1~3-on.
	u32					auto_color;	//0-off; 1-on.
	u32         fancycolor_red; //0-Off; 1-2-on.
	u32         fancycolor_green;//0-Off; 1-2-on.
	u32         fancycolor_blue;//0-Off; 1-2-on.
	struct disp_rect   window;
	u32         enable;
}disp_enhance_para;
#endif

s32 disp_enhance_set_para(struct disp_enhance* enhance, disp_enhance_para *para);


#endif

