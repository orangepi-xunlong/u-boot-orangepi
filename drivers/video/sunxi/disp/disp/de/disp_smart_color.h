/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef _DISP_SMCL_H_
#define _DISP_SMCL_H_
#include "bsp_display.h"
#include "disp_private.h"

s32 disp_smcl_shadow_protect(struct disp_smcl *smcl, bool protect);
s32 disp_init_smcl(struct __disp_bsp_init_para *para);
s32 disp_smcl_set_size(struct disp_smcl *smcl, disp_size *size);

#endif
