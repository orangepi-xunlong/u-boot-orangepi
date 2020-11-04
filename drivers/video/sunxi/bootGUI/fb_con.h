/*
 * drivers/video/sunxi/bootGUI/fb_con.h
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
#ifndef __FB_CON_H__
#define __FB_CON_H__

#include <boot_gui.h>

enum {
	FB_UNINIT = 0x0,
	FB_UNLOCKED,
	FB_LOCKED,
};

enum {
	FB_COMMIT_ADDR = 0x00000001,
	FB_COMMIT_GEOMETRY = 0x00000002,
};

struct buf_node {
	void *addr;
	int buf_size;
	int release_fence;
	rect_t dirty_rect;
	struct buf_node *next;
};

typedef struct framebuffer {
	unsigned int fb_id;
	int locked;
	rect_t interest_rect;
	struct canvas *cv;
	struct buf_node *buf_list;
	void *handle;
} framebuffer_t;

int fb_save_para(unsigned int fb_id);

/**
 * @name       :fb_update_cmdline
 * @brief      :update disp_reserve command line
 * @param[IN]  :fb_id: index of fb
 * @return     :0 if success
 */
int fb_update_cmdline(unsigned int fb_id);

#endif /* #ifndef __FB_CON_H__ */
