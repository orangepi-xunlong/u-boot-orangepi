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

static int dev_num;
void amp_preview_init(unsigned long fb_addr);
void amp_preview_save(void);

int get_framebuffer_num(void)
{
	return FRAMEBUFFER_NUM > dev_num ? dev_num : FRAMEBUFFER_NUM;
}

int save_disp_cmd(void)
{
	int i;
	for (i = FB_ID_0; i < get_framebuffer_num(); ++i) {
		fb_save_para(i);
	}
#ifdef CONFIG_DISP2_AMP_PREVIEW_DISPLAY
	amp_preview_save();
#endif
	return 0;
}

int save_disp_cmdline(void)
{
	int i;
	for (i = FB_ID_0; i < get_framebuffer_num(); ++i) {
		fb_update_cmdline(i);
	}
	return 0;
}

int boot_gui_init(void)
{
	int ret = 0;
#ifdef CONFIG_DISP2_AMP_PREVIEW_DISPLAY
	struct canvas *cv = NULL;
#endif

	tick_printf("%s:start\n", __func__);
	printf("%s:start\n", __func__);
	dev_num = disp_devices_open();
	if (dev_num < 0) {
		ret = dev_num;
		dev_num = 0;
	}
	ret += fb_init();
#ifdef CONFIG_DISP2_AMP_PREVIEW_DISPLAY
	cv = fb_lock(0);
	amp_preview_init((unsigned long)cv->base);
	fb_unlock(0, NULL, 1);
#endif
	tick_printf("%s:finish\n", __func__);
	return ret;
}
