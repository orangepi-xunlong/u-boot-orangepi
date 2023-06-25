/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * drivers/dsp/common/dsp_img.h
 *
 * Copyright (c) 2007-2025 Allwinnertech Co., Ltd.
 * Author: wujiayi <wujiayi@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details
 *
 */

#ifndef __DSP_IMG_H
#define __DSP_IMG_H

struct vaddr_range_t {
	unsigned long vstart;
	unsigned long vend;
	unsigned long offset;
};

int show_img_version(char *head_addr, u32 dsp_id);

int find_img_section(u32 img_addr,
			const char *section_name,
			unsigned long *section_addr);

int get_img_len(u32 img_addr,
		unsigned long section_addr,
		u32 *img_len);

int set_msg_dts(u32 img_addr,
		unsigned long section_addr,
		struct dts_msg_t *dts_msg);

unsigned long set_img_va_to_pa(unsigned long vaddr,
				struct vaddr_range_t *map,
				int size);

#endif

