/*
 * parse_reg.h
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
#ifndef _PARSE_REG_H
#define _PARSE_REG_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined (__UBOOT__)

#include <linux/types.h>
#include <linux/stddef.h>
#include <linux/printk.h>
#include <malloc.h>
#include <asm/arch/timer.h>
#include <asm/io.h>

#define parse_reg_wrn(msg...) pr_err(msg)
#define parse_reg_malloc(size) malloc(size)
#define parse_reg_free(size) free(size)
#define parse_reg_writel(v, a) writel(v, a)
#define parse_reg_readl(a) readl(a)
#define parse_reg_delayms(ms) __msdelay(ms);
#define parse_reg_delayus(us) __usdelay(us);

#elif defined (__KERNEL__)

#include <linux/types.h>
#include <linux/stddef.h>
#include <linux/slab.h>

#define parse_reg_wrn(msg...) pr_err(msg)
#define parse_reg_malloc(size) kmalloc(size, GFP_KERNEL | __GFP_ZERO)
#define parse_reg_free(size) kfree(size)
#define parse_reg_writel(v, a)
#define parse_reg_readl(a) a
#define parse_reg_delayms(ms)
#define parse_reg_delayus(us)

#elif defined(__APPLE__)

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
typedef uint32_t __u32;
typedef uint16_t __u16;
typedef uint8_t __u8;
#define parse_reg_wrn(msg...) printf(msg)
#define parse_reg_malloc(size) malloc(size)
#define parse_reg_free(size) free(size)
#define parse_reg_writel(v, a)
#define parse_reg_readl(a) a
#define parse_reg_delayms(ms)
#define parse_reg_delayus(us)

#else

#include <linux/types.h>
#include <linux/stddef.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define parse_reg_wrn(msg...) printf(msg)
#define parse_reg_malloc(size) malloc(size)
#define parse_reg_free(size) free(size)
#define parse_reg_writel(v, a)
#define parse_reg_readl(a) a
#define parse_reg_delayms(ms)
#define parse_reg_delayus(us)

#endif

#ifndef bool
typedef _Bool bool;
#endif

struct parse_reg_t;

struct logo_head_t {
	__u8 magic_number[4];
	__u16 project_config_table_size;
	__u16 logo_table_size;
	__u32 logo_data_size;
};

enum reg_type_t {
	REG_TYPE_BYTE = 0x1,
	REG_TYPE_WORD = 0x2,
	REG_TYPE_DWORD = 0x4,
	REG_TYPE_DELAY = 0xff,
	REG_TYPE_TOGGLE = 0xfe,
};

struct logo_reg_setting_t {
	__u32 reg_addr;
	__u32 reg_value;
	__u32 reg_mask;
	__u32 reg_type;
};

enum logo_table_type {
	LOGO_PLL_REG,
	LOGO_LVDS_REG,
	LOGO_OSD_REG,
	NUMBER_OF_LOGO_TABLE_TYPE, /*always put this definition at the end*/
};

/**
 * project info
 */
struct project_info_t {
	__u32 project_id;
	__u16 reg_data_idx[8];
};

/**
 * reg data type
 */
struct reg_data_t {
	__u32 reg_data_index;
	__u32 reg_data_size;
};

/**
 * reg data info
 */
struct reg_data_info_t {
	struct reg_data_t *p_info;
	struct logo_reg_setting_t *p_reg_set;
	__u32 reg_number;
	enum logo_table_type type;
};

/**
 * logo_table_t
 */
struct logo_table_t {
	enum logo_table_type table_type;
	__u32 table_offset;
	__u32 table_size;
};

enum osd_in_fmt {
	OSD_RGB_888 = 0x17,
	OSD_RGA_8888 = 0x19,
	OSD_INVALID_FMT = 0xff
};
/**
 * osd_buf_info
 */
struct osd_buf_info {
	 enum osd_in_fmt fmt;
	 __u32 width;
	 __u32 height;
	 __u32 stride;
	 __u32 bpp;
};

/**
 * logo reg bin structure
 */
struct parse_reg_t {
	bool (*is_header_valid)(struct parse_reg_t *p_bin);

	bool (*is_project_valid)(struct parse_reg_t *p_bin, __u32 project_id);

	struct project_info_t *(*get_project_info)(struct parse_reg_t *p_bin, __u32 project_id);

	struct logo_table_t *(*get_logo_table_info)(struct parse_reg_t *p_bin, enum logo_table_type table_type);

	int (*print_logo_table_info)(struct parse_reg_t *p_bin, enum logo_table_type table_type);

	int (*print_project_info)(struct parse_reg_t *p_bin, __u32 project_id);

	int (*print_info)(struct parse_reg_t *p_bin);

	int (*get_reg)(struct parse_reg_t *p_bin, __u32 project_id,
		       enum logo_table_type table_type, struct reg_data_info_t *p_reg_info);

	int (*print_reg)(struct reg_data_info_t *p_reg_info);

	int (*write_reg)(struct reg_data_info_t *p_reg_info);

	int (*get_osd_buf_info)(struct parse_reg_t *p_bin, __u32 project_id,
		       struct osd_buf_info *p_info);

	int (*update_osd_buf_addr)(struct parse_reg_t *p_bin, __u32 project_id,
				   unsigned long addr);

	struct logo_head_t *p_header;

	__u16 project_number;
	struct project_info_t *p_project;

	__u16 table_type_number;
	struct logo_table_t *p_table;

	int (*destroy_parse_reg_t)(struct parse_reg_t *p_bin);
};

struct parse_reg_t *create_parse_reg_t(void *buf_addr);


#ifdef __cplusplus
}
#endif

#endif /*End of file*/
