/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef _DISP_CURSOR_H_
#define _DISP_CURSOR_H_
#include "bsp_display.h"
#include "disp_private.h"

#define CURSOR_MAX_PALETTE_SIZE 1024
#define CURSOR_MAX_FB_SIZE (64*64*8/8)

s32 disp_cursor_shadow_protect(struct disp_cursor *cursor, bool protect);
s32 disp_init_cursor(struct __disp_bsp_init_para *para);

#endif
