/*
 * sprite/cartoon/sprite_progressbar/sprite_progressbar.h
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
#ifndef __SPRITE_PROGRESSBAR_H__
#define __SPRITE_PROGRESSBAR_H__
#include "sprite_progressbar_i.h"
extern  progressbar_t *sprite_cartoon_progressbar_create(int x1, int y1, int x2, int y2, int op);
extern  int       	sprite_cartoon_progressbar_config(progressbar_t *p, int frame_color, int progress_color, int thickness);

extern  int 		sprite_cartoon_progressbar_active(progressbar_t *p);
extern  int 		sprite_cartoon_progressbar_destroy(progressbar_t *p);
extern  int 		sprite_cartoon_progressbar_upgrate(progressbar_t *p, int rate);



#endif   //__SPRITE_PROGRESSBAR_H__
