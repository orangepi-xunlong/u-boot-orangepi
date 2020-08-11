/*
 * sunxi_csi.c for csi parser v4l2 subdev
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "sunxi_csi.h"

#define IS_FLAG(x, y) (((x)&(y)) == y)

extern struct vin_core *vinc;
extern struct mipi_dev *mipi;
extern struct sensor_format_struct sensor_formats[];
struct csi_dev *csi;

static struct csi_format sunxi_csi_formats[] = {
	{
		.code = MEDIA_BUS_FMT_YUYV8_2X8,
		.seq = SEQ_YUYV,
		.infmt = FMT_YUV422,
		.data_width = 8,
	}, {
		.code = MEDIA_BUS_FMT_YVYU8_2X8,
		.seq = SEQ_YVYU,
		.infmt = FMT_YUV422,
		.data_width = 8,
	}, {
		.code = MEDIA_BUS_FMT_UYVY8_2X8,
		.seq = SEQ_UYVY,
		.infmt = FMT_YUV422,
		.data_width = 8,
	}, {
		.code = MEDIA_BUS_FMT_VYUY8_2X8,
		.seq = SEQ_VYUY,
		.infmt = FMT_YUV422,
		.data_width = 8,
	}, {
		.code = MEDIA_BUS_FMT_YUYV8_1X16,
		.seq = SEQ_YUYV,
		.infmt = FMT_YUV422,
		.data_width = 16,
	}, {
		.code = MEDIA_BUS_FMT_YVYU8_1X16,
		.seq = SEQ_YVYU,
		.infmt = FMT_YUV422,
		.data_width = 16,
	}, {
		.code = MEDIA_BUS_FMT_UYVY8_1X16,
		.seq = SEQ_UYVY,
		.infmt = FMT_YUV422,
		.data_width = 16,
	}, {
		.code = MEDIA_BUS_FMT_VYUY8_1X16,
		.seq = SEQ_VYUY,
		.infmt = FMT_YUV422,
		.data_width = 16,
	}, {
		.code = MEDIA_BUS_FMT_SBGGR8_1X8,
		.infmt = FMT_RAW,
		.data_width = 8,
	}, {
		.code = MEDIA_BUS_FMT_SGBRG8_1X8,
		.infmt = FMT_RAW,
		.data_width = 8,
	}, {
		.code = MEDIA_BUS_FMT_SGRBG8_1X8,
		.infmt = FMT_RAW,
		.data_width = 8,
	}, {
		.code = MEDIA_BUS_FMT_SRGGB8_1X8,
		.infmt = FMT_RAW,
		.data_width = 8,
	}, {
		.code = MEDIA_BUS_FMT_SBGGR10_1X10,
		.infmt = FMT_RAW,
		.data_width = 10,
	}, {
		.code = MEDIA_BUS_FMT_SGBRG10_1X10,
		.infmt = FMT_RAW,
		.data_width = 10,
	}, {
		.code = MEDIA_BUS_FMT_SGRBG10_1X10,
		.infmt = FMT_RAW,
		.data_width = 10,
	}, {
		.code = MEDIA_BUS_FMT_SRGGB10_1X10,
		.infmt = FMT_RAW,
		.data_width = 10,
	}, {
		.code = MEDIA_BUS_FMT_SBGGR12_1X12,
		.infmt = FMT_RAW,
		.data_width = 12,
	}, {
		.code = MEDIA_BUS_FMT_SGBRG12_1X12,
		.infmt = FMT_RAW,
		.data_width = 12,
	}, {
		.code = MEDIA_BUS_FMT_SGRBG12_1X12,
		.infmt = FMT_RAW,
		.data_width = 12,
	}, {
		.code = MEDIA_BUS_FMT_SRGGB12_1X12,
		.infmt = FMT_RAW,
		.data_width = 12,
	}
};


static int __csi_set_fmt_hw(void)
{
	struct prs_ncsi_bt656_header bt656_header;
	struct prs_mcsi_if_cfg mcsi_if;
	struct prs_cap_mode mode;
	int i;

	memset(&bt656_header, 0, sizeof(bt656_header));
	memset(&mcsi_if, 0, sizeof(mcsi_if));

	csi->ncsi_if.seq = csi->csi_fmt->seq;
	mcsi_if.seq = csi->csi_fmt->seq;

	switch (csi->csi_fmt->data_width) {
	case 8:
		csi->ncsi_if.dw = DW_8BIT;
		break;
	case 10:
		csi->ncsi_if.dw = DW_10BIT;
		break;
	case 12:
		csi->ncsi_if.dw = DW_12BIT;
		break;
	default:
		csi->ncsi_if.dw = DW_8BIT;
		break;
	}

	switch (sensor_formats->field) {
	case V4L2_FIELD_ANY:
	case V4L2_FIELD_NONE:
		csi->ncsi_if.type = PROGRESSED;
		csi->ncsi_if.mode = FRAME_MODE;
		mcsi_if.mode = FIELD_MODE;
		break;
	case V4L2_FIELD_TOP:
	case V4L2_FIELD_BOTTOM:
		csi->ncsi_if.type = INTERLACE;
		csi->ncsi_if.mode = FIELD_MODE;
		mcsi_if.mode = FIELD_MODE;
		break;
	case V4L2_FIELD_INTERLACED:
		csi->ncsi_if.type = INTERLACE;
		csi->ncsi_if.mode = FRAME_MODE;
		mcsi_if.mode = FRAME_MODE;
		break;
	default:
		csi->ncsi_if.type = PROGRESSED;
		csi->ncsi_if.mode = FRAME_MODE;
		mcsi_if.mode = FIELD_MODE;
		break;
	}

	switch (csi->bus_info.bus_if) {
	case V4L2_MBUS_PARALLEL:
		if (csi->csi_fmt->data_width == 16)
			csi->ncsi_if.intf = PRS_IF_INTLV_16BIT;
		else
			csi->ncsi_if.intf = PRS_IF_INTLV;
		csic_prs_mode(csi->id, PRS_NCSI);
		csic_prs_ncsi_if_cfg(csi->id, &csi->ncsi_if);
		csic_prs_ncsi_en(csi->id, 1);
		break;
	case V4L2_MBUS_BT656:
		if (csi->csi_fmt->data_width == 16) {
			if (csi->bus_info.ch_total_num == 1)
				csi->ncsi_if.intf = PRS_IF_BT1120_1CH;
			else if (csi->bus_info.ch_total_num == 2)
				csi->ncsi_if.intf = PRS_IF_BT1120_2CH;
			else if (csi->bus_info.ch_total_num == 4)
				csi->ncsi_if.intf = PRS_IF_BT1120_4CH;
		} else {
			if (csi->bus_info.ch_total_num == 1)
				csi->ncsi_if.intf = PRS_IF_BT656_1CH;
			else if (csi->bus_info.ch_total_num == 2)
				csi->ncsi_if.intf = PRS_IF_BT656_2CH;
			else if (csi->bus_info.ch_total_num == 4)
				csi->ncsi_if.intf = PRS_IF_BT656_4CH;
		}
		csic_prs_mode(csi->id, PRS_NCSI);
		bt656_header.ch0_id = 0;
		bt656_header.ch1_id = 1;
		bt656_header.ch2_id = 2;
		bt656_header.ch3_id = 3;
		csic_prs_ncsi_bt656_header_cfg(csi->id, &bt656_header);
		csic_prs_ncsi_if_cfg(csi->id, &csi->ncsi_if);
		csic_prs_ncsi_en(csi->id, 1);
		break;
	case V4L2_MBUS_CSI2:
		csic_prs_mode(csi->id, PRS_MCSI);
		csic_prs_mcsi_if_cfg(csi->id, &mcsi_if);
		csic_prs_mcsi_en(csi->id, 1);
		break;
	default:
		csic_prs_mode(csi->id, PRS_MCSI);
		csic_prs_mcsi_if_cfg(csi->id, &mcsi_if);
		csic_prs_mcsi_en(csi->id, 1);
		break;
	}

	if (csi->capture_mode == V4L2_MODE_IMAGE)
		mode.mode = SCAP;
	else
		mode.mode = VCAP;

	if (csi->out_size.hor_len != sensor_formats->width ||
		csi->out_size.ver_len != sensor_formats->height) {
		csi->out_size.hor_len = sensor_formats->width;
		csi->out_size.ver_len = sensor_formats->height;
		csi->out_size.hor_start = 0;
		csi->out_size.ver_start = 0;
	}

	if (sensor_formats->field == V4L2_FIELD_INTERLACED || sensor_formats->field == V4L2_FIELD_TOP ||
		sensor_formats->field == V4L2_FIELD_BOTTOM)
		csi->out_size.ver_len = csi->out_size.ver_len / 2;

	for (i = 0; i < csi->bus_info.ch_total_num; i++) {
		csic_prs_input_fmt_cfg(csi->id, i, csi->csi_fmt->infmt);
		csic_prs_output_size_cfg(csi->id, i, &csi->out_size);
	}

	csic_prs_capture_start(csi->id, csi->bus_info.ch_total_num, &mode);

	return 0;
}

int sunxi_csi_subdev_s_stream(int enable)
{
	csic_prs_pclk_en(csi->id, enable);
	if (enable) {
		csic_prs_enable(csi->id);
		csic_prs_disable(csi->id);
		csic_prs_enable(csi->id);
		__csi_set_fmt_hw();
	} else {
		switch (csi->bus_info.bus_if) {
		case V4L2_MBUS_PARALLEL:
		case V4L2_MBUS_BT656:
			csic_prs_ncsi_en(csi->id, 0);
			break;
		case V4L2_MBUS_CSI2:
			csic_prs_mcsi_en(csi->id, 0);
			break;
		default:
			return -1;
		}
		csic_prs_capture_stop(csi->id);
		csic_prs_disable(csi->id);
	}

	vin_log(VIN_LOG_FMT, "parser%d %s, %d*%d hoff: %d voff: %d code: %x field: %d\n",
		csi->id, enable ? "stream on" : "stream off",
		csi->out_size.hor_len, csi->out_size.ver_len,
		csi->out_size.hor_start, csi->out_size.ver_start,
		sensor_formats->mbus_code, sensor_formats->field);

	return 0;
}


static struct csi_format *__csi_find_format(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(sunxi_csi_formats); i++)
		if (sensor_formats->mbus_code == sunxi_csi_formats[i].code)
			return &sunxi_csi_formats[i];
	return NULL;
}

static struct csi_format *__csi_try_format(void)
{
	struct csi_format *csi_fmt;

	csi_fmt = __csi_find_format();
	if (csi_fmt == NULL)
		csi_fmt = &sunxi_csi_formats[0];

	sensor_formats->mbus_code = csi_fmt->code;
	return csi_fmt;
}

int sunxi_csi_s_mbus_config(const struct v4l2_mbus_config *cfg)
{
	if (cfg->type == V4L2_MBUS_CSI2 || cfg->type == V4L2_MBUS_SUBLVDS ||
		cfg->type == V4L2_MBUS_HISPI) {
		csi->bus_info.bus_if = V4L2_MBUS_CSI2;
		csi->bus_info.ch_total_num = 0;
		if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_CHANNEL_0))
			csi->bus_info.ch_total_num++;
		if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_CHANNEL_1))
			csi->bus_info.ch_total_num++;
		if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_CHANNEL_2))
			csi->bus_info.ch_total_num++;
		if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_CHANNEL_3))
			csi->bus_info.ch_total_num++;
	} else if (cfg->type == V4L2_MBUS_PARALLEL) {
		csi->bus_info.bus_if = V4L2_MBUS_PARALLEL;
		csi->bus_info.ch_total_num = 1;
		if (IS_FLAG(cfg->flags, V4L2_MBUS_MASTER)) {
			if (IS_FLAG(cfg->flags, V4L2_MBUS_HSYNC_ACTIVE_HIGH)) {
				csi->bus_info.bus_tmg.href_pol = ACTIVE_HIGH;
				csi->ncsi_if.href = REF_POSITIVE;
			} else {
				csi->bus_info.bus_tmg.href_pol = ACTIVE_LOW;
				csi->ncsi_if.href = REF_NEGATIVE;
			}
			if (IS_FLAG(cfg->flags, V4L2_MBUS_VSYNC_ACTIVE_HIGH)) {
				csi->bus_info.bus_tmg.vref_pol = ACTIVE_HIGH;
				csi->ncsi_if.vref = REF_POSITIVE;
			} else {
				csi->bus_info.bus_tmg.vref_pol = ACTIVE_LOW;
				csi->ncsi_if.vref = REF_NEGATIVE;
			}
			if (IS_FLAG(cfg->flags, V4L2_MBUS_PCLK_SAMPLE_RISING)) {
				csi->bus_info.bus_tmg.pclk_sample = RISING;
				csi->ncsi_if.clk = CLK_RISING;
			} else {
				csi->bus_info.bus_tmg.pclk_sample = FALLING;
				csi->ncsi_if.clk = CLK_FALLING;
			}
			if (IS_FLAG(cfg->flags, V4L2_MBUS_FIELD_EVEN_HIGH)) {
				csi->bus_info.bus_tmg.field_even_pol = ACTIVE_HIGH;
				csi->ncsi_if.field = FIELD_POS;
			} else {
				csi->bus_info.bus_tmg.field_even_pol = ACTIVE_LOW;
				csi->ncsi_if.field = FIELD_NEG;
			}
		} else {
			vin_err("Do not support V4L2_MBUS_SLAVE!\n");
			return -1;
		}
	} else if (cfg->type == V4L2_MBUS_BT656) {
		csi->bus_info.bus_if = V4L2_MBUS_BT656;
		csi->bus_info.ch_total_num = 0;
		if (IS_FLAG(cfg->flags, CSI_CH_0))
			csi->bus_info.ch_total_num++;
		if (IS_FLAG(cfg->flags, CSI_CH_1))
			csi->bus_info.ch_total_num++;
		if (IS_FLAG(cfg->flags, CSI_CH_2))
			csi->bus_info.ch_total_num++;
		if (IS_FLAG(cfg->flags, CSI_CH_3))
			csi->bus_info.ch_total_num++;
		if (csi->bus_info.ch_total_num == 4) {
			csi->arrange.column = 2;
			csi->arrange.row = 2;
		} else if (csi->bus_info.ch_total_num == 2) {
			csi->arrange.column = 2;
			csi->arrange.row = 1;
		} else {
			csi->bus_info.ch_total_num = 1;
			csi->arrange.column = 1;
			csi->arrange.row = 1;
		}
		if (IS_FLAG(cfg->flags, V4L2_MBUS_PCLK_SAMPLE_RISING)) {
			csi->bus_info.bus_tmg.pclk_sample = RISING;
			csi->ncsi_if.clk = CLK_RISING;
		} else {
			csi->bus_info.bus_tmg.pclk_sample = FALLING;
			csi->ncsi_if.clk = CLK_FALLING;
		}
	}
	vin_log(VIN_LOG_CSI, "csi%d total ch = %d\n", csi->id, csi->bus_info.ch_total_num);

	return 0;
}

int csi_probe(void)
{
	char sub_name[10];
	int ret = 0;

	sprintf(sub_name, "csi%d", vinc->csi_sel);

	csi = malloc(sizeof(struct csi_dev));
	if (!csi) {
		ret = -1;
		goto ekzalloc;
	}
	csi->id = vinc->csi_sel;
	csi->arrange.row = 1;
	csi->arrange.column = 1;
	csi->bus_info.ch_total_num = 1;

	csi->base = csi_getprop_regbase(sub_name, "reg", 0);
	if (!csi->base) {
		vin_err("Fail to get CSI base addr!\n");
		ret = -1;
		goto freedev;
	}
	vin_log(VIN_LOG_MD, "csi%d reg is 0x%lx\n", csi->id, csi->base);

	ret = csic_prs_set_base_addr(csi->id, csi->base);
	if (ret < 0)
		goto freedev;

	csi->csi_fmt = __csi_try_format();
	vin_log(VIN_LOG_CSI, "csi_fmt:code is 0x%x, data_width is %d\n",
			csi->csi_fmt->code, csi->csi_fmt->data_width);

	vin_log(VIN_LOG_CSI, "csi%d probe end!\n", csi->id);

	return 0;

freedev:
	free(csi);
ekzalloc:
	vin_log(VIN_LOG_CSI, "csi probe err!\n");
	return ret;
}

int csi_remove(void)
{
	free(csi);
	return 0;
}
