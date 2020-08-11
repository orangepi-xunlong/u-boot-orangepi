/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __DISP_LAYER_H__
#define __DISP_LAYER_H__

#include "disp_private.h"

s32 disp_init_lyr(struct __disp_bsp_init_para *para);
s32 disp_lyr_shadow_protect(struct disp_layer *lyr, bool protect);

/* only for sw_init, for bootlogo copy to fb0 */
u32 disp_layer_get_addr(u32 sel, u32 hid);
u32 disp_layer_set_addr(u32 sel, u32 hid, u32 addr);
u32 disp_layer_get_in_width(u32 sel, u32 hid);
u32 disp_layer_get_in_height(u32 sel, u32 hid);

#endif
