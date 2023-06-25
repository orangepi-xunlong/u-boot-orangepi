/* drivers/video/sunxi/disp2/disp/lcd/kd101na5.h
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <anruliu@allwinnertech.com>
 *
 * kd101na5 panel driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef _KD101NA5_H
#define _KD101NA5_H

#include "panels.h"

extern __lcd_panel_t kd101na5_panel;

extern s32 bsp_disp_get_panel_info(u32 screen_id, disp_panel_para *info);

#endif /*End of file*/
