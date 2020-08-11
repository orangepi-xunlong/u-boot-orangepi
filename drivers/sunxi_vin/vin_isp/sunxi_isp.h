/*
 * lsunxi_isp.h
 *
 * Copyright (c) 2007-2017 Allwinnertech Co., Ltd.
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
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
#ifndef _SUNXI_ISP_H_
#define _SUNXI_ISP_H_
#include "../vin.h"
#include "../vin_video/vin_core.h"
#include "sun8iw12p1/sun8iw12p1_isp_reg_cfg.h"

#define min_t(type, x, y) ({			\
	type __min1 = (x);			\
	type __min2 = (y);			\
	__min1 < __min2 ? __min1 : __min2; })

#define max_t(type, x, y) ({			\
	type __max1 = (x);			\
	type __max2 = (y);			\
	__max1 > __max2 ? __max1 : __max2; })

#define clamp_t(type, val, lo, hi) min_t(type, max_t(type, val, lo), hi)

enum isp_pad {
	ISP_PAD_SINK,
	ISP_PAD_SOURCE_ST,
	ISP_PAD_SOURCE,
	ISP_PAD_NUM,
};

struct isp_pix_fmt {
	u32 mbus_code;
	enum isp_input_seq infmt;
	char *name;
	u32 fourcc;
	u32 color;
	u16 memplanes;
	u16 colplanes;
	u32 depth[VIDEO_MAX_PLANES];
	u16 mdataplanes;
	u16 flags;
};

struct isp_table_addr {
	void *isp_lsc_tbl_vaddr;
	void *isp_lsc_tbl_dma_addr;
	void *isp_gamma_tbl_vaddr;
	void *isp_gamma_tbl_dma_addr;
	void *isp_linear_tbl_vaddr;
	void *isp_linear_tbl_dma_addr;

	void *isp_drc_tbl_vaddr;
	void *isp_drc_tbl_dma_addr;
	void *isp_saturation_tbl_vaddr;
	void *isp_saturation_tbl_dma_addr;
};

struct isp_stat_to_user {
	/* v4l2 drivers fill */
	void *ae_buf;
	void *af_buf;
	void *awb_buf;
	void *hist_buf;
	void *afs_buf;
	void *pltm_buf;
};

struct vin_mm {
	size_t size;
	void *phy_addr;
};

struct isp_dev {
	int capture_mode;
	int use_isp;
	int irq;
	unsigned int is_empty;
	unsigned int load_flag;
	unsigned int f1_after_librun;/*fisrt frame after server run*/
	unsigned int have_init;
	unsigned int wdr_mode;
	unsigned int id;
	unsigned long base;
	/* char load_shadow[ISP_LOAD_REG_SIZE*3]; */
	/* char load_lut_tbl[ISP_TABLE_MAPPING1_SIZE*3]; */
	/* char load_drc_tbl[ISP_TABLE_MAPPING2_SIZE*3]; */
	struct vin_mm isp_stat;
	struct vin_mm isp_load;
	struct vin_mm isp_save;
	struct vin_mm isp_lut_tbl;
	struct vin_mm isp_drc_tbl;
	struct isp_debug_mode isp_dbg;
	struct isp_table_addr isp_tbl;
	struct isp_stat_to_user *stat_buf;
	struct isp_pix_fmt *isp_fmt;
	struct isp_size_settings isp_ob;
};

int isp_probe(void);
int isp_remove(void);
int sunxi_isp_subdev_s_stream(int enable);

#endif /*_SUNXI_ISP_H_*/
