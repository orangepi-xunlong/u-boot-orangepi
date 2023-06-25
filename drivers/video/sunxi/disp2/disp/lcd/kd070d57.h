/* drivers/video/sunxi/disp2/disp/lcd/kd070d57.h
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <anruliu@allwinnertech.com>
 *
 * kd070d57 panel driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef _KD070D57_H
#define _KD070D57_H

#include "panels.h"

extern __lcd_panel_t kd070d57_panel;

extern s32 bsp_disp_get_panel_info(u32 screen_id, disp_panel_para *info);

#endif /*End of file*/
