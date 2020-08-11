/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __DEFAULT_PANEL_H__
#define  __DEFAULT_PANEL_H__

#include "panels.h"

extern struct __lcd_panel_t gm7121_cvbs;

extern s32 gm7121_tv_power_on(u32 on_off);
extern s32 gm7121_tv_open(void);
extern s32 gm7121_tv_close(void);
extern s32 gm7121_tv_get_mode(void);
extern s32 gm7121_tv_set_mode(disp_tv_mode tv_mode);
extern s32 gm7121_tv_get_mode_support(disp_tv_mode tv_mode);

#endif
