/*
 * sunxi_scaler.c for scaler and osd v4l2 subdev
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "../utility/platform_cfg.h"
#include "sunxi_scaler.h"
#include "../vin_video/vin_core.h"
#include "vipp_reg.h"

#define ALIGN_4B(x)	(((x) + (3)) & ~(3))
#define ALIGN_2B(x)	(((x) + (1)) & ~(1))

static LIST_HEAD(scaler_drv_list);

#define MIN_IN_WIDTH			192
#define MIN_IN_HEIGHT			128
#define MAX_IN_WIDTH			4224
#define MAX_IN_HEIGHT			4224

#define MIN_OUT_WIDTH			16
#define MIN_OUT_HEIGHT			10
#define MAX_OUT_WIDTH			4224
#define MAX_OUT_HEIGHT			4224

struct scaler_dev *scaler;
extern struct vin_core *vinc;
extern struct sensor_format_struct sensor_formats[];

static int __scaler_w_shift(int x_ratio, int y_ratio)
{
	int m, n;
	int sum_weight = 0;
	int weight_shift = -8;
	int xr = (x_ratio >> 8) + 1;
	int yr = (y_ratio >> 8) + 1;

	for (m = 0; m <= xr; m++) {
		for (n = 0; n <= yr; n++) {
			sum_weight += (y_ratio - abs((n << 8) - (yr << 7)))
				* (x_ratio - abs((m << 8) - (xr << 7)));
		}
	}
	sum_weight >>= 8;
	while (sum_weight != 0) {
		weight_shift++;
		sum_weight >>= 1;
	}
	return weight_shift;
}

static void __scaler_calc_ratios(struct v4l2_rect *input,
				struct scaler_para *para)
{
	unsigned int width;
	unsigned int height;
	unsigned int r_min;

	sensor_formats->o_width = clamp_t(u32, sensor_formats->o_width, MIN_IN_WIDTH, input->width);
	sensor_formats->o_height =
		clamp_t(u32, sensor_formats->o_height, MIN_IN_HEIGHT, input->height);

	para->xratio = 256 * input->width / sensor_formats->o_width;
	para->yratio = 256 * input->height / sensor_formats->o_height;
	para->xratio = clamp_t(u32, para->xratio, 256, 2048);
	para->yratio = clamp_t(u32, para->yratio, 256, 2048);

	r_min = min(para->xratio, para->yratio);

	width = ALIGN_4B(sensor_formats->o_width * r_min / 256);
	height = ALIGN_2B(sensor_formats->o_height * r_min / 256);
	para->xratio = 256 * width / sensor_formats->o_width;
	para->yratio = 256 * height / sensor_formats->o_height;
	para->xratio = clamp_t(u32, para->xratio, 256, 2048);
	para->yratio = clamp_t(u32, para->yratio, 256, 2048);
	para->width = sensor_formats->o_width;
	para->height = sensor_formats->o_height;
	vin_log(VIN_LOG_SCALER, "para: xr = %d, yr = %d, w = %d, h = %d\n",
		  para->xratio, para->yratio, para->width, para->height);
	/* Center the new crop rectangle.
	 * crop is before scaler
	 */
	input->left += (input->width - width) / 2;
	input->top += (input->height - height) / 2;
	input->left = ALIGN_4B(input->left);
	input->top = ALIGN_2B(input->top);
	input->width = width;
	input->height = height;

	vin_log(VIN_LOG_SCALER, "crop: left = %d, top = %d, w = %d, h = %d\n",
		input->left, input->top, input->width, input->height);
}

#ifdef VIPP_REG_MODE
static int sunxi_scaler_subdev_init(void)
{
	memset(scaler->vipp_reg.phy_addr, 0, VIPP_REG_SIZE);
	memset(scaler->osd_para.phy_addr, 0, OSD_PARA_SIZE);
	memset(scaler->osd_stat.phy_addr, 0, OSD_STAT_SIZE);
	vipp_set_reg_load_addr(scaler->id, (unsigned long)scaler->vipp_reg.phy_addr);
	vipp_set_osd_para_load_addr(scaler->id, (unsigned long)scaler->osd_para.phy_addr);
	vipp_set_osd_stat_load_addr(scaler->id, (unsigned long)scaler->osd_stat.phy_addr);
	vipp_set_osd_cv_update(scaler->id, NOT_UPDATED);
	vipp_set_osd_ov_update(scaler->id, NOT_UPDATED);
	vipp_set_para_ready(scaler->id, NOT_READY);
	return 0;
}
#endif

int sunxi_scaler_subdev_s_stream(int enable)
{
	struct vipp_scaler_config scaler_cfg;
	struct vipp_scaler_size scaler_size;
	struct vipp_crop crop;
	enum vipp_format out_fmt;

	switch (sensor_formats->fourcc) {
	case V4L2_PIX_FMT_SBGGR8:
	case V4L2_PIX_FMT_SGBRG8:
	case V4L2_PIX_FMT_SGRBG8:
	case V4L2_PIX_FMT_SRGGB8:
	case V4L2_PIX_FMT_SBGGR10:
	case V4L2_PIX_FMT_SGBRG10:
	case V4L2_PIX_FMT_SGRBG10:
	case V4L2_PIX_FMT_SRGGB10:
	case V4L2_PIX_FMT_SBGGR12:
	case V4L2_PIX_FMT_SGBRG12:
	case V4L2_PIX_FMT_SGRBG12:
	case V4L2_PIX_FMT_SRGGB12:
		vin_log(VIN_LOG_FMT, "%s output fmt is raw, return directly\n", __func__);
		return 0;
	default:
		break;
	}
	if (sensor_formats->field == V4L2_FIELD_INTERLACED || sensor_formats->field == V4L2_FIELD_TOP ||
	    sensor_formats->field == V4L2_FIELD_BOTTOM) {
		vin_log(VIN_LOG_SCALER, "Scaler not support field mode, return directly!\n");
		return 0;
	}

	if (enable) {
		crop.hor = scaler->crop.active.left;
		crop.ver = scaler->crop.active.top;
		crop.width = scaler->crop.active.width;
		crop.height = scaler->crop.active.height;
		vipp_set_crop(scaler->id, &crop);
		scaler_size.sc_width = scaler->para.width;
		scaler_size.sc_height = scaler->para.height;
		vipp_scaler_output_size(scaler->id, &scaler_size);

		switch (sensor_formats->fourcc) {
		case V4L2_PIX_FMT_YUV420:
		case V4L2_PIX_FMT_YUV420M:
		case V4L2_PIX_FMT_YVU420:
		case V4L2_PIX_FMT_YVU420M:
		case V4L2_PIX_FMT_NV21:
		case V4L2_PIX_FMT_NV21M:
		case V4L2_PIX_FMT_NV12:
		case V4L2_PIX_FMT_NV12M:
		case V4L2_PIX_FMT_FBC:
			out_fmt = YUV420;
			break;
		case V4L2_PIX_FMT_YUV422P:
		case V4L2_PIX_FMT_NV16:
		case V4L2_PIX_FMT_NV61:
		case V4L2_PIX_FMT_NV61M:
		case V4L2_PIX_FMT_NV16M:
			out_fmt = YUV422;
			break;
		default:
			out_fmt = YUV420;
			break;
		}
		if (scaler->is_osd_en)
			scaler_cfg.sc_out_fmt = YUV422;
		else
			scaler_cfg.sc_out_fmt = out_fmt;
		scaler_cfg.sc_x_ratio = scaler->para.xratio;
		scaler_cfg.sc_y_ratio = scaler->para.yratio;
		scaler_cfg.sc_w_shift = __scaler_w_shift(scaler->para.xratio, scaler->para.yratio);
		vipp_scaler_cfg(scaler->id, &scaler_cfg);
		vipp_output_fmt_cfg(scaler->id, out_fmt);
		vipp_scaler_en(scaler->id, 1);
		vipp_set_para_ready(scaler->id, HAS_READY);
		vipp_set_osd_ov_update(scaler->id, HAS_UPDATED);
		vipp_set_osd_cv_update(scaler->id, HAS_UPDATED);
		vipp_top_clk_en(scaler->id, enable);
		vipp_enable(scaler->id);
	} else {
		vipp_disable(scaler->id);
		vipp_top_clk_en(scaler->id, enable);
		vipp_scaler_en(scaler->id, 0);
		vipp_set_para_ready(scaler->id, NOT_READY);
		vipp_set_osd_ov_update(scaler->id, NOT_UPDATED);
		vipp_set_osd_cv_update(scaler->id, NOT_UPDATED);
	}
	vin_log(VIN_LOG_FMT, "vipp%d %s, %d*%d hoff: %d voff: %d xr: %d yr: %d\n",
		scaler->id, enable ? "stream on" : "stream off",
		scaler->para.width, scaler->para.height,
		scaler->crop.active.left, scaler->crop.active.top,
		scaler->para.xratio, scaler->para.yratio);

	return 0;
}

#ifdef VIPP_REG_MODE
static int scaler_resource_alloc(void)
{
	scaler->vipp_reg.size = VIPP_REG_SIZE + OSD_PARA_SIZE + OSD_STAT_SIZE;

	scaler->vipp_reg.phy_addr = malloc(scaler->vipp_reg.size);

	scaler->osd_para.phy_addr = scaler->vipp_reg.phy_addr + VIPP_REG_SIZE;
	scaler->osd_stat.phy_addr = scaler->osd_para.phy_addr + OSD_PARA_SIZE;

	return 0;
}

static void scaler_resource_free(void)
{
	free(scaler->vipp_reg.phy_addr);
}
#endif

int scaler_probe(void)
{
	char sub_name[10];
	int ret = 0;

	sprintf(sub_name, "vipp%d", vinc->vipp_sel);

	scaler = malloc(sizeof(struct scaler_dev));
	if (!scaler) {
		ret = -1;
		goto ekzalloc;
	}
	memset(scaler, 0, sizeof(struct scaler_dev));
	scaler->id = vinc->vipp_sel;

	scaler->base = csi_getprop_regbase(sub_name, "reg", 0);
	if (!scaler->base) {
		vin_err("Fail to get VIPP base addr!\n");
		ret = -1;
		goto freedev;
	}
	vin_log(VIN_LOG_MD, "vipp%d base reg is 0x%lx\n", scaler->id, scaler->base);

#ifdef VIPP_REG_MODE
	scaler_resource_alloc();
#endif
	vipp_set_base_addr(scaler->id, scaler->base);

#ifdef VIPP_REG_MODE
	vipp_map_reg_load_addr(scaler->id, (unsigned long)scaler->vipp_reg.phy_addr);
	vipp_map_osd_para_load_addr(scaler->id, (unsigned long)scaler->osd_para.phy_addr);

	sunxi_scaler_subdev_init();
#endif
	scaler->crop.active.width  = sensor_formats->width;
	scaler->crop.active.height = sensor_formats->height;
	__scaler_calc_ratios(&scaler->crop.active, &scaler->para);

	vin_log(VIN_LOG_SCALER, "scaler%d probe end\n", scaler->id);
	return 0;

freedev:
	free(scaler);
ekzalloc:
	vin_err("scaler%d probe err!\n", scaler->id);
	return ret;
}

int scaler_remove(void)
{
#ifdef VIPP_REG_MODE
	scaler_resource_free();
#endif
	free(scaler);
	return 0;
}
