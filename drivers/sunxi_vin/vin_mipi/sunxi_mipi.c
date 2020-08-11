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
#include "sunxi_mipi.h"

struct mipi_dev *mipi;
extern struct vin_core *vinc;

#define IS_FLAG(x, y) (((x)&(y)) == y)
extern struct sensor_format_struct sensor_formats[];

#if 0
static struct combo_format sunxi_mipi_formats[] = {
	{
		.code = MEDIA_BUS_FMT_UYVY8_2X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_YUYV8_2X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_SBGGR8_1X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_SGBRG8_1X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_SGRBG8_1X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_SRGGB8_1X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_SBGGR10_1X10,
		.bit_width = RAW10,
	}, {
		.code = MEDIA_BUS_FMT_SGBRG10_1X10,
		.bit_width = RAW10,
	}, {
		.code = MEDIA_BUS_FMT_SGRBG10_1X10,
		.bit_width = RAW10,
	}, {
		.code = MEDIA_BUS_FMT_SRGGB10_1X10,
		.bit_width = RAW10,
	}, {
		.code = MEDIA_BUS_FMT_SBGGR12_1X12,
		.bit_width = RAW12,
	}, {
		.code = MEDIA_BUS_FMT_SGBRG12_1X12,
		.bit_width = RAW12,
	}, {
		.code = MEDIA_BUS_FMT_SGRBG12_1X12,
		.bit_width = RAW12,
	}, {
		.code = MEDIA_BUS_FMT_SRGGB12_1X12,
		.bit_width = RAW12,
	}, {
		.code = MEDIA_BUS_FMT_UYVY8_2X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_VYUY8_2X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_YUYV8_2X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_YVYU8_2X8,
		.bit_width = RAW8,
	}, {
		.code = MEDIA_BUS_FMT_UYVY10_2X10,
		.bit_width = RAW10,
	}, {
		.code = MEDIA_BUS_FMT_VYUY10_2X10,
		.bit_width = RAW10,
	}, {
		.code = MEDIA_BUS_FMT_YUYV10_2X10,
		.bit_width = RAW10,
	}, {
		.code = MEDIA_BUS_FMT_YVYU10_2X10,
		.bit_width = RAW10,
	}
};
#endif

void combo_rx_mipi_init(void)
{
	struct mipi_ctr mipi_ctr;
	struct mipi_lane_map mipi_map;

	memset(&mipi_ctr, 0, sizeof(mipi_ctr));
	mipi_ctr.mipi_lane_num = mipi->cmb_cfg.mipi_ln;
	mipi_ctr.mipi_msb_lsb_sel = 1;

	if (mipi->cmb_mode == MIPI_NORMAL_MODE) {
		mipi_ctr.mipi_wdr_mode_sel = 0;
	} else if (mipi->cmb_mode == MIPI_VC_WDR_MODE) {
		mipi_ctr.mipi_wdr_mode_sel = 0;
		mipi_ctr.mipi_open_multi_ch = 1;
		mipi_ctr.mipi_ch0_height = sensor_formats->height;
		mipi_ctr.mipi_ch1_height = sensor_formats->height;
		mipi_ctr.mipi_ch2_height = sensor_formats->height;
		mipi_ctr.mipi_ch3_height = sensor_formats->height;
	} else if (mipi->cmb_mode == MIPI_DOL_WDR_MODE) {
		mipi_ctr.mipi_wdr_mode_sel = 2;
	}

	mipi_map.mipi_lane0 = MIPI_IN_L0_USE_PAD_LANE0;
	mipi_map.mipi_lane1 = MIPI_IN_L1_USE_PAD_LANE1;
	mipi_map.mipi_lane2 = MIPI_IN_L2_USE_PAD_LANE2;
	mipi_map.mipi_lane3 = MIPI_IN_L3_USE_PAD_LANE3;

	cmb_rx_mode_sel(mipi->id, D_PHY);
	cmb_rx_app_pixel_out(mipi->id, TWO_PIXEL);
	cmb_rx_mipi_ctr(mipi->id, &mipi_ctr);
	cmb_rx_mipi_dphy_mapping(mipi->id, &mipi_map);
}

void combo_rx_sub_lvds_init(void)
{
	struct lvds_ctr lvds_ctr;

	memset(&lvds_ctr, 0, sizeof(lvds_ctr));
	lvds_ctr.lvds_bit_width = mipi->cmb_fmt->bit_width;
	lvds_ctr.lvds_lane_num = mipi->cmb_cfg.lvds_ln;

	if (mipi->cmb_mode == LVDS_NORMAL_MODE) {
		lvds_ctr.lvds_line_code_mode = 1;
	} else if (mipi->cmb_mode == LVDS_4CODE_WDR_MODE) {
		lvds_ctr.lvds_line_code_mode = mipi->wdr_cfg.line_code_mode;

		lvds_ctr.lvds_wdr_lbl_sel = 1;
		lvds_ctr.lvds_sync_code_line_cnt = mipi->wdr_cfg.line_cnt;

		lvds_ctr.lvds_code_mask = mipi->wdr_cfg.code_mask;
		lvds_ctr.lvds_wdr_fid_mode_sel = mipi->wdr_cfg.wdr_fid_mode_sel;
		lvds_ctr.lvds_wdr_fid_map_en = mipi->wdr_cfg.wdr_fid_map_en;
		lvds_ctr.lvds_wdr_fid0_map_sel = mipi->wdr_cfg.wdr_fid0_map_sel;
		lvds_ctr.lvds_wdr_fid1_map_sel = mipi->wdr_cfg.wdr_fid1_map_sel;
		lvds_ctr.lvds_wdr_fid2_map_sel = mipi->wdr_cfg.wdr_fid2_map_sel;
		lvds_ctr.lvds_wdr_fid3_map_sel = mipi->wdr_cfg.wdr_fid3_map_sel;

		lvds_ctr.lvds_wdr_en_multi_ch = mipi->wdr_cfg.wdr_en_multi_ch;
		lvds_ctr.lvds_wdr_ch0_height = mipi->wdr_cfg.wdr_ch0_height;
		lvds_ctr.lvds_wdr_ch1_height = mipi->wdr_cfg.wdr_ch1_height;
		lvds_ctr.lvds_wdr_ch2_height = mipi->wdr_cfg.wdr_ch2_height;
		lvds_ctr.lvds_wdr_ch3_height = mipi->wdr_cfg.wdr_ch3_height;
	} else if (mipi->cmb_mode == LVDS_5CODE_WDR_MODE) {
		lvds_ctr.lvds_line_code_mode = mipi->wdr_cfg.line_code_mode;
		lvds_ctr.lvds_wdr_lbl_sel = 2;
		lvds_ctr.lvds_sync_code_line_cnt = mipi->wdr_cfg.line_cnt;

		lvds_ctr.lvds_code_mask = mipi->wdr_cfg.code_mask;
	}

	cmb_rx_mode_sel(mipi->id, SUB_LVDS);
	cmb_rx_app_pixel_out(mipi->id, ONE_PIXEL);
	cmb_rx_lvds_ctr(mipi->id, &lvds_ctr);
	cmb_rx_lvds_mapping(mipi->id, &mipi->lvds_map);
	cmb_rx_lvds_sync_code(mipi->id, &mipi->sync_code);
}

void combo_rx_hispi_init(void)
{
	struct lvds_ctr lvds_ctr;
	struct hispi_ctr hispi_ctr;

	memset(&hispi_ctr, 0, sizeof(hispi_ctr));
	memset(&lvds_ctr, 0, sizeof(lvds_ctr));
	lvds_ctr.lvds_bit_width = mipi->cmb_fmt->bit_width;
	lvds_ctr.lvds_lane_num = mipi->cmb_cfg.lvds_ln;

	if (mipi->cmb_mode == HISPI_NORMAL_MODE) {
		lvds_ctr.lvds_line_code_mode = 0;
		lvds_ctr.lvds_pix_lsb = 1;

		hispi_ctr.hispi_normal = 1;
		hispi_ctr.hispi_trans_mode = PACKETIZED_SP;
	} else if (mipi->cmb_mode == HISPI_WDR_MODE) {
		lvds_ctr.lvds_line_code_mode = mipi->wdr_cfg.line_code_mode;

		lvds_ctr.lvds_wdr_lbl_sel = 1;

		lvds_ctr.lvds_pix_lsb = mipi->wdr_cfg.pix_lsb;
		lvds_ctr.lvds_sync_code_line_cnt = mipi->wdr_cfg.line_cnt;

		lvds_ctr.lvds_wdr_fid_mode_sel = mipi->wdr_cfg.wdr_fid_mode_sel;
		lvds_ctr.lvds_wdr_fid_map_en = mipi->wdr_cfg.wdr_fid_map_en;
		lvds_ctr.lvds_wdr_fid0_map_sel = mipi->wdr_cfg.wdr_fid0_map_sel;
		lvds_ctr.lvds_wdr_fid1_map_sel = mipi->wdr_cfg.wdr_fid1_map_sel;
		lvds_ctr.lvds_wdr_fid2_map_sel = mipi->wdr_cfg.wdr_fid2_map_sel;
		lvds_ctr.lvds_wdr_fid3_map_sel = mipi->wdr_cfg.wdr_fid3_map_sel;

		hispi_ctr.hispi_normal = 1;
		hispi_ctr.hispi_trans_mode = PACKETIZED_SP;
		hispi_ctr.hispi_wdr_en = 1;
		hispi_ctr.hispi_wdr_sof_fild = mipi->wdr_cfg.wdr_sof_fild;
		hispi_ctr.hispi_wdr_eof_fild = mipi->wdr_cfg.wdr_eof_fild;
		hispi_ctr.hispi_code_mask = mipi->wdr_cfg.code_mask;
	}

	cmb_rx_mode_sel(mipi->id, SUB_LVDS);
	cmb_rx_app_pixel_out(mipi->id, ONE_PIXEL);
	cmb_rx_lvds_ctr(mipi->id, &lvds_ctr);
	cmb_rx_lvds_mapping(mipi->id, &mipi->lvds_map);
	cmb_rx_lvds_sync_code(mipi->id, &mipi->sync_code);

	cmb_rx_hispi_ctr(mipi->id, &hispi_ctr);
}

void combo_rx_init(void)
{
	/*comnbo rx phya init*/
	cmb_rx_phya_config(mipi->id);

	if (mipi->terminal_resistance) {
		vin_log(VIN_LOG_MIPI, "open combo terminal resitance!\n");
		cmb_rx_te_auto_disable(mipi->id, 1);
		cmb_rx_phya_a_d0_en(mipi->id, 1);
		cmb_rx_phya_b_d0_en(mipi->id, 1);
		cmb_rx_phya_c_d0_en(mipi->id, 1);
		cmb_rx_phya_a_d1_en(mipi->id, 1);
		cmb_rx_phya_b_d1_en(mipi->id, 1);
		cmb_rx_phya_c_d1_en(mipi->id, 1);
		cmb_rx_phya_a_d2_en(mipi->id, 1);
		cmb_rx_phya_b_d2_en(mipi->id, 1);
		cmb_rx_phya_c_d2_en(mipi->id, 1);
		cmb_rx_phya_a_d3_en(mipi->id, 1);
		cmb_rx_phya_b_d3_en(mipi->id, 1);
		cmb_rx_phya_c_d3_en(mipi->id, 1);
		cmb_rx_phya_a_ck_en(mipi->id, 1);
		cmb_rx_phya_b_ck_en(mipi->id, 1);
		cmb_rx_phya_c_ck_en(mipi->id, 1);
	}
	cmb_rx_phya_singal_dly_en(mipi->id, 0);
	cmb_rx_phya_offset(mipi->id, mipi->pyha_offset);

	switch (mipi->if_type) {
	case V4L2_MBUS_PARALLEL:
		cmb_rx_mode_sel(mipi->id, CMOS);
		cmb_rx_app_pixel_out(mipi->id, ONE_PIXEL);
		break;
	case V4L2_MBUS_CSI2:
		cmb_rx_phya_ck_mode(mipi->id, 0);
		combo_rx_mipi_init();
		break;
	case V4L2_MBUS_SUBLVDS:
		cmb_rx_phya_ck_mode(mipi->id, 1);
		combo_rx_sub_lvds_init();
		break;
	case V4L2_MBUS_HISPI:
		cmb_rx_phya_ck_mode(mipi->id, 0);
		combo_rx_hispi_init();
		break;
	default:
		combo_rx_mipi_init();
		break;
	}

	cmb_rx_enable(mipi->id);
}

int sunxi_mipi_subdev_s_stream(int enable)
{
	mipi->cmb_mode = mipi->res.res_combo_mode & 0xf;
	mipi->terminal_resistance = mipi->res.res_combo_mode & CMB_TERMINAL_RES;
	mipi->pyha_offset = (mipi->res.res_combo_mode & 0x70) >> 4;

	if (enable)
		combo_rx_init();
	else
		cmb_rx_disable(mipi->id);

	vin_log(VIN_LOG_FMT, "%s%d %s, lane_num %d\n",
		mipi->if_name, mipi->id, enable ? "stream on" : "stream off",
		mipi->cmb_cfg.lane_num);

	return 0;
}

int sunxi_mipi_s_mbus_config(const struct v4l2_mbus_config *cfg, const struct mbus_framefmt_res *res)
{
	if (cfg->type == V4L2_MBUS_CSI2) {
		mipi->if_type = V4L2_MBUS_CSI2;
		memcpy(mipi->if_name, "mipi", sizeof("mipi"));
		if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_4_LANE)) {
			mipi->csi2_cfg.lane_num = 4;
			mipi->cmb_cfg.lane_num = 4;
			mipi->cmb_cfg.mipi_ln = MIPI_4LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_3_LANE)) {
			mipi->csi2_cfg.lane_num = 3;
			mipi->cmb_cfg.lane_num = 3;
			mipi->cmb_cfg.mipi_ln = MIPI_3LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_2_LANE)) {
			mipi->csi2_cfg.lane_num = 2;
			mipi->cmb_cfg.lane_num = 2;
			mipi->cmb_cfg.mipi_ln = MIPI_2LANE;
		} else {
			mipi->cmb_cfg.lane_num = 1;
			mipi->csi2_cfg.lane_num = 1;
			mipi->cmb_cfg.mipi_ln = MIPI_1LANE;
		}
	} else if (cfg->type == V4L2_MBUS_SUBLVDS) {
		mipi->if_type = V4L2_MBUS_SUBLVDS;
		memcpy(mipi->if_name, "sublvds", sizeof("sublvds"));
		if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_12_LANE)) {
			mipi->cmb_cfg.lane_num = 12;
			mipi->cmb_cfg.lvds_ln = LVDS_12LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_10_LANE)) {
			mipi->cmb_cfg.lane_num = 10;
			mipi->cmb_cfg.lvds_ln = LVDS_10LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_8_LANE)) {
			mipi->cmb_cfg.lane_num = 8;
			mipi->cmb_cfg.lvds_ln = LVDS_8LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_4_LANE)) {
			mipi->cmb_cfg.lane_num = 4;
			mipi->cmb_cfg.lvds_ln = LVDS_4LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_2_LANE)) {
			mipi->cmb_cfg.lane_num = 2;
			mipi->cmb_cfg.lvds_ln = LVDS_2LANE;
		} else {
			mipi->cmb_cfg.lane_num = 8;
			mipi->cmb_cfg.lvds_ln = LVDS_8LANE;
		}
	} else if (cfg->type == V4L2_MBUS_HISPI) {
		mipi->if_type = V4L2_MBUS_HISPI;
		memcpy(mipi->if_name, "hispi", sizeof("hispi"));
		if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_12_LANE)) {
			mipi->cmb_cfg.lane_num = 12;
			mipi->cmb_cfg.lvds_ln = LVDS_12LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_10_LANE)) {
			mipi->cmb_cfg.lane_num = 10;
			mipi->cmb_cfg.lvds_ln = LVDS_10LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_8_LANE)) {
			mipi->cmb_cfg.lane_num = 8;
			mipi->cmb_cfg.lvds_ln = LVDS_8LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_4_LANE)) {
			mipi->cmb_cfg.lane_num = 4;
			mipi->cmb_cfg.lvds_ln = LVDS_4LANE;
		} else if (IS_FLAG(cfg->flags, V4L2_MBUS_SUBLVDS_2_LANE)) {
			mipi->cmb_cfg.lane_num = 2;
			mipi->cmb_cfg.lvds_ln = LVDS_2LANE;
		} else {
			mipi->cmb_cfg.lane_num = 4;
			mipi->cmb_cfg.lvds_ln = LVDS_4LANE;
		}
	} else {
		memcpy(mipi->if_name, "combo_parallel", sizeof("combo_parallel"));
		mipi->if_type = V4L2_MBUS_PARALLEL;
	}

	mipi->csi2_cfg.total_rx_ch = 0;
	if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_CHANNEL_0))
		mipi->csi2_cfg.total_rx_ch++;
	if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_CHANNEL_1))
		mipi->csi2_cfg.total_rx_ch++;
	if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_CHANNEL_2))
		mipi->csi2_cfg.total_rx_ch++;
	if (IS_FLAG(cfg->flags, V4L2_MBUS_CSI2_CHANNEL_3))
		mipi->csi2_cfg.total_rx_ch++;

	mipi->res = *res;

	return 0;
}

int mipi_probe(void)
{
	char sub_name[10];
	int ret = 0;

	if (vinc->mipi_sel == 0xff) {
		vin_warn("MIPI is no open!\n");
		return -1;
	}
	sprintf(sub_name, "mipi%d", vinc->mipi_sel);

	mipi = malloc(sizeof(struct mipi_dev));
	if (!mipi) {
		vin_err("Fail to get MIPI base addr!\n");
		ret = -1;
		goto ekzalloc;
	}
	mipi->id = vinc->mipi_sel;

	mipi->base = csi_getprop_regbase(sub_name, "reg", 0);
	if (!mipi->base) {
		ret = -1;
		goto freedev;
	}
	vin_log(VIN_LOG_MD, "mipi%d reg is 0x%lx\n", mipi->id, mipi->base);
	cmb_rx_set_base_addr(mipi->id, mipi->base);

	bsp_mipi_csi_set_base_addr(mipi->id, mipi->base);
	bsp_mipi_dphy_set_base_addr(mipi->id, mipi->base + 0x1000);

	vin_log(VIN_LOG_MIPI, "mipi%d probe end!\n", mipi->id);
	return 0;

freedev:
	free(mipi);
ekzalloc:
	vin_err("mipi probe err!\n");
	return ret;
}

int mipi_remove(void)
{
	free(mipi);
	return 0;
}


