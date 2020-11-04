/*
 * sprite/cartoon/sprite_char/sfte/FontEngine.h
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
#ifndef __FontEngine_h
#define __FontEngine_h

#include <asm/io.h>
#include "sfte.h"
typedef struct _FE_FONT_t {
	SFTE_Face face;
	int bbox_ymin;		    // unit : pixel
	unsigned int line_distance; //
	unsigned int base_width;
	unsigned char *base_addr;
	unsigned int base_psize;

} FE_FONT_t, *FE_FONT;

extern int open_font(const char *font_file, int pixel_size, unsigned int width,
		     unsigned char *addr);
extern int close_font(void);
extern int check_change_line(unsigned int x, unsigned char ch);
extern int draw_bmp_ulc(unsigned int ori_x, unsigned int ori_y,
			unsigned int color);

#endif

/* end of FontEngine.h  */
