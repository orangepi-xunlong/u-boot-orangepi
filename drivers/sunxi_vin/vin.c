/*
 * vin.c
 *
 * Copyright (c) 2018 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zequn Zheng <zequnzhengi@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "vin.h"

unsigned int vin_log_mask;
struct vin_clk_total *vin_clk;
extern struct vin_core *vinc;
extern struct isp_dev *isp;
extern struct sensor_format_struct sensor_formats[];
struct tvd_dev dev;

extern long disp_ioctl(void *hd, unsigned int cmd, void *arg);

static struct disp_screen get_disp_screen(int w1, int h1, int w2, int h2)
{
	struct disp_screen screen;
	float r1, r2;
	r1 = (float)w1/(float)w2;
	r2 = (float)h1/(float)h2;
	if (r1 < r2) {
		screen.w = w2*r1;
		screen.h = h2*r1;
	} else {
		screen.w = w2*r2;
		screen.h = h2*r2;
	}

	screen.x = (w1 - screen.w)/2;
	screen.y = (h1 - screen.h)/2;

	return screen;
}

static int disp_init(int width, int height, unsigned int pixformat)
{
	unsigned int arg[6] = {0};
	int layer_id = 0;

	if (dev.layer_info.screen_id < 0)
		return 0;

	/* get current output type */
	/*
	arg[0] = dev.layer_info.screen_id;
	dev.layer_info.output_type = (enum disp_output_type)disp_ioctl(
	    NULL, DISP_GET_OUTPUT_TYPE, (void *)arg);
	if (dev.layer_info.output_type == DISP_OUTPUT_TYPE_NONE) {
		printf("the output type is DISP_OUTPUT_TYPE_NONE %d\n",
		       dev.layer_info.output_type);
		//return -1;
	}
	*/

	dev.layer_info.pixformat = pixformat;
	dev.layer_info.layer_config.channel = 0;
	dev.layer_info.layer_config.layer_id = layer_id;
	dev.layer_info.layer_config.info.zorder = 1;
	dev.layer_info.layer_config.info.alpha_mode = 1;
	dev.layer_info.layer_config.info.alpha_value = 0xff;
	dev.layer_info.width =
		disp_ioctl(NULL, DISP_GET_SCN_WIDTH, (void *)arg);
	dev.layer_info.height =
		disp_ioctl(NULL, DISP_GET_SCN_HEIGHT, (void *)arg);

	dev.layer_info.layer_config.info.mode = LAYER_MODE_BUFFER;

	if (dev.layer_info.pixformat == TVD_PL_YUV420)
		dev.layer_info.layer_config.info.fb.format =
			DISP_FORMAT_YUV420_SP_UVUV;
	else
		dev.layer_info.layer_config.info.fb.format =
			DISP_FORMAT_YUV422_SP_VUVU;

	if (dev.layer_info.full_screen == 0 && width < dev.layer_info.width &&
		height < dev.layer_info.height) {
		dev.layer_info.layer_config.info.screen_win.x =
			(dev.layer_info.width - width) / 2;
		dev.layer_info.layer_config.info.screen_win.y =
			(dev.layer_info.height - height) / 2;
		if (!dev.layer_info.layer_config.info.screen_win.width) {
			dev.layer_info.layer_config.info.screen_win.width = width;
			dev.layer_info.layer_config.info.screen_win.height =
				height;
		}
	} else {

		/*struct disp_screen screen;*/
		get_disp_screen(dev.layer_info.width, dev.layer_info.height,
				width, height);
		dev.layer_info.layer_config.info.screen_win.x = 0; /* screen.x; */
		dev.layer_info.layer_config.info.screen_win.y = 0; /* screen.y; */
		dev.layer_info.layer_config.info.screen_win.width =
			dev.layer_info.width;
		dev.layer_info.layer_config.info.screen_win.height =
			dev.layer_info.height;
	}
	return 0;
}

int disp_set_addr(int width, int height, unsigned int addr)

{
	unsigned int arg[6];
	int ret;

	if (dev.layer_info.pixformat == TVD_PL_YUV420) {
		dev.layer_info.layer_config.info.fb.size[0].width = width;
		dev.layer_info.layer_config.info.fb.size[0].height = height;
		dev.layer_info.layer_config.info.fb.size[1].width = width / 2;
		dev.layer_info.layer_config.info.fb.size[1].height = height / 2;
		dev.layer_info.layer_config.info.fb.size[2].width = width / 2;
		dev.layer_info.layer_config.info.fb.size[2].height = height / 2;
		dev.layer_info.layer_config.info.fb.crop.width =
			(unsigned long long)width << 32;
		dev.layer_info.layer_config.info.fb.crop.height =
			(unsigned long long)height << 32;
		dev.layer_info.layer_config.info.fb.addr[0] = addr;
		dev.layer_info.layer_config.info.fb.addr[1] =
			(dev.layer_info.layer_config.info.fb.addr[0] + width * height);
		dev.layer_info.layer_config.info.fb.addr[2] =
			dev.layer_info.layer_config.info.fb.addr[0] +
			width * height * 5 / 4;
		dev.layer_info.layer_config.info.fb.trd_right_addr[0] =
			(dev.layer_info.layer_config.info.fb.addr[0] +
			width * height * 3 / 2);
		dev.layer_info.layer_config.info.fb.trd_right_addr[1] =
			(dev.layer_info.layer_config.info.fb.addr[0] + width * height);
		dev.layer_info.layer_config.info.fb.trd_right_addr[2] =
			(dev.layer_info.layer_config.info.fb.addr[0] +
			width * height * 5 / 4);
	} else {
		dev.layer_info.layer_config.info.fb.size[0].width = width;
		dev.layer_info.layer_config.info.fb.size[0].height = height;
		dev.layer_info.layer_config.info.fb.size[1].width = width / 2;
		dev.layer_info.layer_config.info.fb.size[1].height = height;
		dev.layer_info.layer_config.info.fb.size[2].width = width / 2;
		dev.layer_info.layer_config.info.fb.size[2].height = height;
		dev.layer_info.layer_config.info.fb.crop.width =
			(unsigned long long)width << 32;
		dev.layer_info.layer_config.info.fb.crop.height =
			(unsigned long long)height << 32;
		dev.layer_info.layer_config.info.fb.addr[0] = addr;
		dev.layer_info.layer_config.info.fb.addr[1] =
			(dev.layer_info.layer_config.info.fb.addr[0] + width * height);
		dev.layer_info.layer_config.info.fb.addr[2] =
			dev.layer_info.layer_config.info.fb.addr[0] +
			width * height * 2 / 2;
		dev.layer_info.layer_config.info.fb.trd_right_addr[0] =
			(dev.layer_info.layer_config.info.fb.addr[0] +
			width * height * 2);
		dev.layer_info.layer_config.info.fb.trd_right_addr[1] =
			(dev.layer_info.layer_config.info.fb.addr[0] + width * height);
	}

	dev.layer_info.layer_config.enable = 1;

	arg[0] = dev.layer_info.screen_id;
	arg[1] = (int)&dev.layer_info.layer_config;
	arg[2] = 1;
	ret = disp_ioctl(NULL, DISP_LAYER_SET_CONFIG, (void *)arg);
	if (0 != ret)
		printf("disp_set_addr fail to set layer info\n");

	return 0;
}


static int vin_md_get_clocks(void)
{
	int of_node = 0;
	int ret;
	unsigned long rate;
	int i;

	vin_clk = malloc(sizeof(struct vin_clk_total));

	vin_fdt_init();

	/* clk_init(); */
	of_node = vin_fdt_nodeoffset("vind");
	vin_log(VIN_LOG_MD, "%s:node is %d\n", __func__, of_node);

	ret = of_periph_clk_config_setup(of_node);
	if (ret) {
		vin_err("Fail to setup periph clk!\n");
		return -1;
	}
	vin_clk->clk[VIN_TOP_CLK].clock = of_clk_get(of_node, 0);
	if (vin_clk->clk[VIN_TOP_CLK].clock == NULL) {
		vin_err("Get top clk failed!\n");
		vin_clk->clk[VIN_TOP_CLK].clock = NULL;
		return -1;
	}

	vin_clk->clk[VIN_TOP_CLK_SRC].clock = of_clk_get(of_node, 1);
	if (vin_clk->clk[VIN_TOP_CLK_SRC].clock == NULL) {
		vin_err("Get top clk source failed!\n");
		vin_clk->clk[VIN_TOP_CLK_SRC].clock = NULL;
		return -1;
	}

	for (i = 0; i < VIN_MAX_CCI; i++) {
		vin_clk->mclk[i].mclk = of_clk_get(of_node, 3 * i + 2);
		if (vin_clk->mclk[i].mclk == NULL) {
			vin_err("Get mclk%d failed!\n", i);
			vin_clk->mclk[i].mclk = NULL;
			break;
		}

		/* vin_clk->mclk[3].clk_24m = clk_get(NULL, "hosc"); */
		vin_clk->mclk[i].clk_24m = of_clk_get(of_node, 3 * i + 3);
		if (vin_clk->mclk[i].clk_24m == NULL) {
			vin_err("Get mclk%d_24m failed!\n", i);
			vin_clk->mclk[i].clk_24m = NULL;
			break;
		}
		vin_clk->mclk[i].clk_pll = of_clk_get(of_node, 3 * i + 4);
		if (vin_clk->mclk[i].clk_pll == NULL) {
			vin_err("Get mclk%d_pll failed!\n", i);
			vin_clk->mclk[i].clk_pll = NULL;
			break;
		}
	}

	vin_clk->isp_clk.clock = of_clk_get(of_node, 3 * i + 2);
	if (vin_clk->isp_clk.clock == NULL) {
		vin_warn("Get isp clk failed!\n");
		vin_clk->isp_clk.clock = NULL;
	}

	vin_clk->mipi_clk[VIN_MIPI_CLK].clock = of_clk_get(of_node, 3 * i + 3);
	if (vin_clk->mipi_clk[VIN_MIPI_CLK].clock == NULL) {
		vin_warn("Get mipi clk failed!\n");
		vin_clk->mipi_clk[VIN_MIPI_CLK].clock = NULL;
	}

	if (vin_clk->mipi_clk[VIN_MIPI_CLK].clock != NULL) {
		vin_clk->mipi_clk[VIN_MIPI_CLK_SRC].clock = of_clk_get(of_node, 3 * i + 4);
		if (vin_clk->mipi_clk[VIN_MIPI_CLK_SRC].clock == NULL) {
			vin_warn("Get mipi clk source failed!\n");
			vin_clk->mipi_clk[VIN_MIPI_CLK_SRC].clock = NULL;
		}
	}

	if (vin_clk->mipi_clk[VIN_MIPI_CLK].clock &&
		vin_clk->mipi_clk[VIN_MIPI_CLK_SRC].clock) {
		if (clk_set_parent(vin_clk->mipi_clk[VIN_MIPI_CLK].clock,
		    vin_clk->mipi_clk[VIN_MIPI_CLK_SRC].clock)) {
			vin_err("set vin mipi clock source failed\n");
			return -1;
		}
		if (clk_set_rate(vin_clk->mipi_clk[VIN_MIPI_CLK].clock, 24*1000*1000)) {
			vin_err("set mipi clock rate error\n");
			return -1;
		}
	}

	if (clk_set_parent(vin_clk->clk[VIN_TOP_CLK].clock, vin_clk->clk[VIN_TOP_CLK_SRC].clock)) {
		vin_err("vin top clock set parent failed \n");
		return -1;
	}

	rate = vin_getprop_regbase("vind", "vind0_clk", 0);
	if (rate) {
		vin_log(VIN_LOG_MD, "vin get core clk = %ld\n", rate);
		vin_clk->clk[VIN_TOP_CLK].frequency = rate;

	} else {
		vin_err("vin failed to get core clk\n");
		vin_clk->clk[VIN_TOP_CLK].frequency = VIN_CLK_RATE;
	}

	return 0;

}

static int __vin_set_top_clk_rate(unsigned int rate)
{
	if (rate >= 300000000)
		vin_clk->clk[VIN_TOP_CLK_SRC].frequency = rate;
	else if (rate >= 150000000)
		vin_clk->clk[VIN_TOP_CLK_SRC].frequency = rate * 2;
	else if (rate >= 75000000)
		vin_clk->clk[VIN_TOP_CLK_SRC].frequency = rate * 4;
	else
		vin_clk->clk[VIN_TOP_CLK_SRC].frequency = VIN_CLK_RATE;

	if (clk_set_rate(vin_clk->clk[VIN_TOP_CLK_SRC].clock,
		vin_clk->clk[VIN_TOP_CLK_SRC].frequency)) {
		vin_err("set vin top clock source rate error\n");
		return -1;
	}

	if (clk_set_rate(vin_clk->clk[VIN_TOP_CLK].clock, rate)) {
		vin_err("set vin top clock rate error\n");
		return -1;
	}
	vin_log(VIN_LOG_POWER, "vin top clk get rate = %ld\n",
		clk_get_rate(vin_clk->clk[VIN_TOP_CLK].clock));

	return 0;
}

static int vin_md_clk_enable(void)
{
	if (vin_clk->clk[VIN_TOP_CLK].clock) {
		__vin_set_top_clk_rate(vin_clk->clk[VIN_TOP_CLK].frequency);
		clk_prepare_enable(vin_clk->clk[VIN_TOP_CLK].clock);
	}

	if (vin_clk->isp_clk.clock)
		clk_prepare_enable(vin_clk->isp_clk.clock);

	if (vin_clk->mipi_clk[VIN_MIPI_CLK].clock)
		clk_prepare_enable(vin_clk->mipi_clk[VIN_MIPI_CLK].clock);

	return 0;
}

static void vin_md_clk_disable(void)
{
	if (vin_clk->clk[VIN_TOP_CLK].clock)
		clk_disable(vin_clk->clk[VIN_TOP_CLK].clock);

	if (vin_clk->isp_clk.clock)
		clk_disable(vin_clk->isp_clk.clock);

	if (vin_clk->mipi_clk[VIN_MIPI_CLK].clock)
		clk_disable(vin_clk->mipi_clk[VIN_MIPI_CLK].clock);
}

static void vin_md_set_power(int on)
{
	if (on) {
		vin_md_clk_enable();
		udelay(120);
		csic_top_enable();
	} else {
		csic_top_disable();
		vin_md_clk_disable();
	}
}

static int vin_pipeline_set_mbus_config(void)
{
	struct v4l2_mbus_config mcfg;
	struct mbus_framefmt_res res;

	sensor_g_mbus_config(&mcfg, &res);

	/* s_mbus_config on all mipi and csi */
	if (vinc->mipi_sel != 0xff)
		sunxi_mipi_s_mbus_config(&mcfg, &res);

	sunxi_csi_s_mbus_config(&mcfg);

	return 0;
}

void sunxi_vin_reset(int enable)
{
	struct prs_cap_mode mode = {.mode = VCAP};

	/*****************stop*******************/
	if (!enable) {
		csic_prs_capture_stop(vinc->csi_sel);
		csic_prs_disable(vinc->csi_sel);

		if (sensor_formats->fourcc == V4L2_PIX_FMT_FBC)
			csic_fbc_disable(vinc->vipp_sel);
		else
			csic_dma_disable(vinc->vipp_sel);
		vipp_disable(vinc->vipp_sel);
		vipp_top_clk_en(vinc->vipp_sel, 0);

		bsp_isp_enable(isp->id, 0);
		bsp_isp_capture_stop(isp->id);
	} else {
	/*****************start*******************/
		vipp_top_clk_en(vinc->vipp_sel, 1);
		vipp_enable(vinc->vipp_sel);
		if (sensor_formats->fourcc == V4L2_PIX_FMT_FBC)
			csic_fbc_enable(vinc->vipp_sel);
		else
			csic_dma_enable(vinc->vipp_sel);

		bsp_isp_enable(isp->id, 1);
		bsp_isp_capture_start(isp->id);

		csic_prs_enable(vinc->csi_sel);
		csic_prs_capture_start(vinc->csi_sel, 1, &mode);
	}
}

static void vin_s_stream(int enable)
{
	sensor_s_stream(enable);
	if (vinc->mipi_sel != 0xff)
		sunxi_mipi_subdev_s_stream(enable);
	sunxi_isp_subdev_s_stream(enable);
	sunxi_scaler_subdev_s_stream(enable);
	vin_subdev_s_stream(enable);
	sunxi_csi_subdev_s_stream(enable);

}

static void vin_probe(void)
{
	if (vinc->mipi_sel != 0xff)
		mipi_probe();
	csi_probe();
	vin_core_probe();
	isp_probe();
	scaler_probe();
}

void vin_free(void)
{
	if (vinc->mipi_sel != 0xff)
		mipi_remove();
	csi_remove();
	vin_core_remove();
	isp_remove();
	scaler_remove();
	sensor_list_free();
	free(vin_clk);
}

int csi_init(void)
{
	int used;
	int vind_used;
	unsigned long reg_base;
	int ret;
	unsigned int pixformat;

	pr_force("[VIN]CSI start!\n");

	pixformat = TVD_PL_YUV420;
	ret = disp_init(sensor_formats->o_width, sensor_formats->o_height, pixformat);
	if (ret)
		return -1;

	used = script_parser_fetch("vind0", "vind0_used", &vind_used, 1);
	if (used != 0) {
		vin_err("unable to get vind_used from [%s]\n", "vind0");
		return -1;
	}
	if (vind_used == 0) {
		vin_err("There is no open CSI\n");
		return -1;
	}

	ret = vin_md_get_clocks();
	if (ret)
		return -1;

	csi_fdt_init();

	reg_base = vin_getprop_regbase("vind", "reg", 0);
	csic_top_set_base_addr(reg_base);
	vin_log(VIN_LOG_MD, "reg is 0x%lx\n", reg_base);

	vin_md_set_power(PWR_ON);

	ret = vin_vinc_parser(0);
	if (ret != 0) {
		vin_err("parser vinc fail!\n");
		return -1;
	}

	ret = vin_sys_config_parser(vinc->rear_sensor);
	if (ret != 0) {
		vin_err("parser sys_config fail!\n");
		return -1;
	}
	sunxi_cci_init();

	vin_probe();

	vin_pipeline_set_mbus_config();

	csic_isp_input_select(vinc->isp_sel, 0, vinc->csi_sel, 0);
	csic_vipp_input_select(vinc->vipp_sel, vinc->isp_sel,  vinc->isp_tx_ch);

	/*
	ret = sensor_test_i2c();
	if (ret)
		return -1;
	*/

	sensor_power(PWR_ON);
	vin_s_stream(PWR_ON);

	pr_force("[VIN]GoodBye CSI!\n");
	return 0;
}

int vin_exit(void)
{
	/* sunxi_vin_reset(0); */
	/*sensor_power(PWR_OFF); */
	/*vin_md_set_power(PWR_OFF);*/
	/* vin_free(); */

	return 0;
}
