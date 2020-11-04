/*
 * drivers/video/sunxi/disp2/disp/de/disp_vdevice.h
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
#ifndef __DISP_VDEVICE_H__
#define __DISP_VDEVICE_H__

#include "disp_private.h"
#include "disp_display.h"

extern disp_dev_t gdisp;
struct disp_device* disp_vdevice_register(struct disp_vdevice_init_data *data);
s32 disp_vdevice_unregister(struct disp_device *vdevice);
s32 disp_vdevice_get_source_ops(struct disp_vdevice_source_ops *ops);

#endif
