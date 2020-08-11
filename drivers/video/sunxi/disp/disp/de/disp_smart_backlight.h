/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef _DISP_SMBL_H_
#define _DISP_SMBL_H_
#include "bsp_display.h"
#include "disp_private.h"

struct disp_smbl *disp_get_smbl(u32 screen_id);
s32 disp_smbl_shadow_protect(struct disp_smbl *smbl, bool protect);
s32 disp_init_smbl(struct __disp_bsp_init_para *para);

#endif
