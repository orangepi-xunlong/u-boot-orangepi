/*
 * fastlogo.h
 *
 * Copyright (c) 2007-2021 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
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
#ifndef _FASTLOGO_H
#define _FASTLOGO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/types.h>
#include <linux/stddef.h>

struct fastlogo_t;
struct parse_reg_t;
struct file_info_t;
struct raw_pic_t;

/**
 * fastlogo
 */
struct fastlogo_t {

	int (*display_fastlogo)(struct fastlogo_t *p_fastlogo);
	int (*destroy_fastlogo)(struct fastlogo_t *p_fastlogo);
	int (*reserve_memory)(struct fastlogo_t *p_fastlogo);
	int (*get_framebuffer_info)(struct fastlogo_t *p_fastlogo, __u32 *w,
				    __u32 *h, char **buf);

	__u32 project_id;
	struct parse_reg_t *p_parse_reg;
	struct file_info_t *p_reg_bin;
	struct file_info_t *p_logo;
	struct raw_pic_t *p_decoded_pic;
};

struct fastlogo_t *create_fastlogo_inst(char *logoname, char *logo_partition,
					char *regbin_name,
					char *regbin_partition);

struct fastlogo_t *get_fastlogo_inst(void);

//compatiable purpose
int sunxi_bmp_display(char *name);

#ifdef __cplusplus
}
#endif

#endif /*End of file*/
