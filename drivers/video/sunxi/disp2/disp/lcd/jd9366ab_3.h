/* drivers/video/sunxi/disp2/disp/lcd/jd9366ab_3.h
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * jd9366ab_3 panel driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef _JD9366AB_3_H
#define _JD9366AB_3_H

#include "panels.h"

extern __lcd_panel_t jd9366ab_3_panel;

extern s32 bsp_disp_get_panel_info(u32 screen_id, disp_panel_para *info);

#endif /*End of file*/
