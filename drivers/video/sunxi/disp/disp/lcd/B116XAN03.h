/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __B116XAN03_PANEL_H__
#define  __B116XAN03_PANEL_H__

#include "panels.h"

extern struct __lcd_panel_t B116XAN03_panel;
extern s32 bsp_disp_get_panel_info(u32 screen_id, disp_panel_para *info);

#endif
