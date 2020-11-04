/*
 * drivers/video/sunxi/bootGUI/boot_gui.c
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
#include <common.h>
#include <boot_gui.h>
#include "fb_con.h"
#include "boot_gui_config.h"

int save_disp_cmd(void)
{
	int i;
	for (i = FB_ID_0; i < FRAMEBUFFER_NUM; ++i) {
		fb_save_para(i);
	}
	return 0;
}

int save_disp_cmdline(void)
{
	int i;
	for (i = FB_ID_0; i < FRAMEBUFFER_NUM; ++i) {
		fb_update_cmdline(i);
	}
	return 0;
}

int boot_gui_init(void)
{
	int ret = 0;

	tick_printf("%s:start\n", __func__);
	ret = disp_devices_open();
	ret += fb_init();
	save_disp_cmdline();
	tick_printf("%s:finish\n", __func__);
	return ret;
}
