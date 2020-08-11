/*
 * sunxi_csi.h for csi parser v4l2 subdev
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef _SUNXI_CSI_H_
#define _SUNXI_CSI_H_

#include "../vin.h"
#include "parser_reg.h"
#include "parser_reg_i.h"

enum csi_pad {
	CSI_PAD_SINK,
	CSI_PAD_SOURCE,
	CSI_PAD_NUM,
};

struct csi_format {
	unsigned int wd_align;
	u32 code;
	enum input_seq seq;
	enum prs_input_fmt infmt;
	unsigned int data_width;
};

struct csi_dev {
	unsigned int id;
	int irq;
	unsigned long base;
	struct bus_info bus_info;
	struct frame_arrange arrange;
	unsigned int capture_mode;
	struct prs_output_size out_size;
	struct csi_format *csi_fmt;
	struct prs_ncsi_if_cfg ncsi_if;
};

int csi_probe(void);
int csi_remove(void);
int sunxi_csi_s_mbus_config(const struct v4l2_mbus_config *cfg);
int sunxi_csi_subdev_s_stream(int enable);

#endif /*_SUNXI_CSI_H_*/
