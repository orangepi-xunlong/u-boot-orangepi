/*
 * drivers/video/sunxi/disp2/disp/de/disp_device.h
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
#ifndef __DISP_DEVICE_H__
#define __DISP_DEVICE_H__

#include "disp_private.h"

s32 disp_device_set_manager(struct disp_device* dispdev, struct disp_manager *mgr);
s32 disp_device_swap_manager(struct disp_device *dispdev1, struct disp_device *dispdev2);
s32 disp_device_unset_manager(struct disp_device* dispdev);
s32 disp_device_get_resolution(struct disp_device* dispdev, u32 *xres, u32 *yres);
s32 disp_device_get_timings(struct disp_device* dispdev, struct disp_video_timings *timings);
s32 disp_device_is_interlace(struct disp_device *dispdev);
bool disp_device_is_in_safe_period(struct disp_device *dispdev);
s32 disp_device_register(struct disp_device *dispdev);
s32 disp_device_unregister(struct disp_device *dispdev);
struct disp_device* disp_device_get(int disp, enum disp_output_type output_type);
struct disp_device* disp_device_find(int disp, enum disp_output_type output_type);
struct list_head* disp_device_get_list_head(void);
void disp_device_show_builtin_patten(struct disp_device *dispdev, u32 patten);


#endif
