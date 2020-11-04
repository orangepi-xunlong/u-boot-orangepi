/*
 * drivers/video/sunxi/bootGUI/canvas_utils.h
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
#ifndef __CANVAS_UTILS_H__
#define __CANVAS_UTILS_H__

#ifdef BOOT_GUI_FONT_8X16
#undef CONFIG_VIDEO_FONT_4X6
#else
#define CONFIG_VIDEO_FONT_4X6
#endif

int get_canvas_utils(struct canvas *cv);

#endif /* #ifndef __CANVAS_UTILS_H__ */
