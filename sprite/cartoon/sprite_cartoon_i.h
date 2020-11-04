/*
 * sprite/cartoon/sprite_cartoon_i.h
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
#ifndef __SPRITE_CARTOON_I_H__
#define __SPRITE_CARTOON_I_H__

#include "sprite_draw/sprite_draw.h"
#include "sprite_progressbar/sprite_progressbar.h"

typedef struct {
	int screen_width;
	int screen_height;
	int screen_size;
	unsigned int color;
	int this_x;
	int this_y;
	char *screen_buf;
} sprite_cartoon_source;

extern  sprite_cartoon_source  sprite_source;


#endif  /* __SPRITE_CARTOON_I_H__ */
