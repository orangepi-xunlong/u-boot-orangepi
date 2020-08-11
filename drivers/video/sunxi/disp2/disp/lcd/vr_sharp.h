/* drivers/video/sunxi/disp2/disp/lcd/vr_sharp.h
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * vr_sharp panel driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#ifndef __VR_SHARP_PANEL_H__
#define __VR_SHARP_PANEL_H__

#include "panels.h"

extern __lcd_panel_t vr_sharp_panel;
extern __s32 dsi_dcs_wr_0para(__u32 sel, __u8 cmd);
extern __s32 dsi_dcs_wr_1para(__u32 sel, __u8 cmd, __u8 para);
extern __s32 dsi_dcs_wr_2para(__u32 sel, __u8 cmd, __u8 para1, __u8 para2);
extern __s32 dsi_dcs_wr_3para(__u32 sel, __u8 cmd, __u8 para1, __u8 para2,
			      __u8 para3);
extern __s32 dsi_dcs_wr_4para(__u32 sel, __u8 cmd, __u8 para1, __u8 para2,
			      __u8 para3, __u8 para4);
extern __s32 dsi_dcs_wr_5para(__u32 sel, __u8 cmd, __u8 para1, __u8 para2,
			      __u8 para3, __u8 para4, __u8 para5);
extern __s32 dsi_gen_wr_0para(__u32 sel, __u8 cmd);
extern __s32 dsi_gen_wr_1para(__u32 sel, __u8 cmd, __u8 para);
extern __s32 dsi_gen_wr_2para(__u32 sel, __u8 cmd, __u8 para1, __u8 para2);
extern __s32 dsi_gen_wr_3para(__u32 sel, __u8 cmd, __u8 para1, __u8 para2,
			      __u8 para3);
extern __s32 dsi_gen_wr_4para(__u32 sel, __u8 cmd, __u8 para1, __u8 para2,
			      __u8 para3, __u8 para4);
extern __s32 dsi_gen_wr_5para(__u32 sel, __u8 cmd, __u8 para1, __u8 para2,
			      __u8 para3, __u8 para4, __u8 para5);

extern s32 bsp_disp_get_panel_info(u32 screen_id, disp_panel_para *info);

#endif
