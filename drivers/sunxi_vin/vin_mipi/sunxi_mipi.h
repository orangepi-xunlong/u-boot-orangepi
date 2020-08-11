/*
 * mipi subdev driver module
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _SUNXI_MIPI__H_
#define _SUNXI_MIPI__H_

#include "../vin.h"
#include "bsp_mipi_csi.h"
#include "combo_common.h"
#include "combo_rx/combo_rx_reg.h"
#include "combo_rx/combo_rx_reg_i.h"
#include "../utility/platform_cfg.h"

enum mipi_pad {
	MIPI_PAD_SINK,
	MIPI_PAD_SOURCE,
	MIPI_PAD_NUM,
};

struct combo_config {
	enum lvds_lane_num lvds_ln;
	enum mipi_lane_num mipi_ln;
	unsigned int lane_num;
	unsigned int total_rx_ch;
};

struct combo_format {
	u32 code;
	enum lvds_bit_width bit_width;
};

struct mipi_dev {
	unsigned int id;
	unsigned long base;
	char if_name[20];
	unsigned int if_type;
	unsigned int cmb_mode;
	unsigned int pyha_offset;
	unsigned int terminal_resistance;
	struct mipi_para csi2_cfg;
	struct combo_config cmb_cfg;
	struct combo_format *cmb_fmt;
	struct combo_sync_code sync_code;
	struct combo_lane_map lvds_map;
	struct combo_wdr_cfg wdr_cfg;
	struct mbus_framefmt_res res;
};

int mipi_probe(void);
int mipi_remove(void);
int sunxi_mipi_s_mbus_config(const struct v4l2_mbus_config *cfg, const struct mbus_framefmt_res *res);
int sunxi_mipi_subdev_s_stream(int enable);

#endif /*_SUNXI_MIPI__H_*/
