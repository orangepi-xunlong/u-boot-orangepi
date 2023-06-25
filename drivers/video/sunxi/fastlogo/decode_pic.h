/*
 * decode_pic.h
 *
 * Copyright (C) 2021 tracyone
 *
 * Contacts: tracyone <tracyone@live.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _DECODE_PIC_H
#define _DECODE_PIC_H

#ifdef __cplusplus
extern "C" {
#endif
#include <linux/types.h>
#include <linux/stddef.h>
#include <linux/printk.h>
#include "load_file.h"

//#define FASTLOGO_DEBUG
struct raw_pic_t;

enum decode_type {
	BMP_DECODE_TYPE,
	JPEG_DECODE_TYPE,
	BYPASS_DECODE_TYPE,
	INVALID_DECODE_TYPE,
};

/**
 * decode out argument
 */
struct decode_out_arg {
	enum decode_type type;
	__u32 width;
	__u32 height;
	__u32 bpp;
	__u32 stride; /*16 byte align*/
};

/**
 * raw_pic_t
 */
struct raw_pic_t {
	__u32 width;
	__u32 height;
	__u32 bpp;
	__u32 stride;
	__u32 file_size;
	void *addr;
	int (*print_info)(struct raw_pic_t *p_raw);
	int (*free_raw_pic)(struct raw_pic_t *p_raw);
};

struct raw_pic_t *decode_pic(struct file_info_t *p_in_file,
			     struct decode_out_arg *p_out_arg);

int decode_pic2(struct file_info_t *p_in_file, struct raw_pic_t *p_pic,
		enum decode_type type);

#ifdef __cplusplus
}
#endif

#endif /*End of file*/
