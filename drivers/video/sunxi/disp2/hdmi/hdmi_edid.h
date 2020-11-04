/*
 * drivers/video/sunxi/disp2/hdmi/hdmi_edid.h
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef __HDMI_EDID_H_
#define __HDMI_EDID_H_

#define Abort_Current_Operation             0
#define Special_Offset_Address_Read         1
#define Explicit_Offset_Address_Write       2
#define Implicit_Offset_Address_Write       3
#define Explicit_Offset_Address_Read        4
#define Implicit_Offset_Address_Read        5
#define Explicit_Offset_Address_E_DDC_Read  6
#define Implicit_Offset_Address_E_DDC_Read  7

extern u8	Device_Support_VIC[512];
extern s32 hdmi_edid_parse(void);
//extern s32 DDC_Read(char cmd,char pointer,char offset,int nbyte,char * pbuf);
//extern void DDC_Init(void);
//extern void send_ini_sequence(void);
extern u32 hdmi_edid_is_hdmi(void);
extern u32 hdmi_edid_is_yuv(void);
extern uintptr_t hdmi_edid_get_data(void);

s32 hdmi_edid_check_init_vic_and_get_supported_vic(int init_vic);
s32 hdmi_get_edid_dt_timing_info(int dt_mode,
		struct disp_video_timings *timing);
#endif //__HDMI_EDID_H_
