
/*
 * sunxi_scaler.h for scaler and osd v4l2 subdev
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#ifndef _SUNXI_SCALER_H_
#define _SUNXI_SCALER_H_
#include "../vin.h"
#include "vipp_reg.h"

/* #define VIPP_REG_MODE */

enum scaler_pad {
	SCALER_PAD_SINK,
	SCALER_PAD_SOURCE,
	SCALER_PAD_NUM,
};

struct scaler_para {
	u32 xratio;
	u32 yratio;
	u32 w_shift;
	u32 width;
	u32 height;
};

struct vin_mmm {
	size_t size;
	void *phy_addr;
};

struct v4l2_rect {
	__s32   left;
	__s32   top;
	__u32   width;
	__u32   height;
};

struct scaler_dev {
	unsigned int is_empty;
	unsigned int is_osd_en;
	unsigned int id;
	unsigned long base;
#ifdef VIPP_REG_MODE
	struct vin_mmm vipp_reg;
	struct vin_mmm osd_para;
	struct vin_mmm osd_stat;
#endif
	struct {
		struct v4l2_rect active;
	} crop;
	struct scaler_para para;
};

int scaler_probe(void);
int scaler_remove(void);
int sunxi_scaler_subdev_s_stream(int enable);

#endif /*_SUNXI_SCALER_H_*/
