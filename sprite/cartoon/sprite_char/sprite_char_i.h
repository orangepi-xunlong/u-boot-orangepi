/*
 * sprite/cartoon/sprite_char/sprite_char_i.h
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
#ifndef __UI_CHAR_I_H__
#define __UI_CHAR_I_H__

#include <asm/types.h>
/*#include <asm/arch/drv_display.h> */
#include "../sprite_cartoon_i.h"

typedef struct ui_char_info_t {
	char *crt_addr; /*当前用于显示的地址 */
	__u32 rest_screen_height; /*剩余的存储屏幕高度，剩余总高度, 字符单位，行 */
	__u32 rest_screen_width; /*剩余的存储屏幕宽度, 剩余总宽度, 字符单位，行 */
	__u32 rest_display_height; /*剩余的显示高度 */
	__u32 total_height;	/*用于显示总的高度 */
	__u32 current_height;      /*当前已经使用的高度 */
	__u32 x;		   /*显示位置的x坐标 */
	__u32 y;		   /*显示位置的y坐标 */
	int word_size;		   /*字符大小 */
} _ui_char_info_t;

extern  sprite_cartoon_source  sprite_source;


#endif   //__UI_CHAR_I_H__
