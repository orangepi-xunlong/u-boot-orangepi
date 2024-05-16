// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2015 Google, Inc
 * Copyright 2014 Rockchip Inc.
 */

#include <common.h>
#include <clk.h>
#include <display.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <edid.h>
#include <log.h>
#include <malloc.h>
#include <panel.h>
#include <regmap.h>
#include <reset.h>
#include <syscon.h>
#include <asm/gpio.h>
#include <asm/io.h>


#define MAX_CR_LOOP 5
#define MAX_EQ_LOOP 5
#define DP_LINK_STATUS_SIZE 6

#define DP_VOLTAGE_MAX         DP_TRAIN_VOLTAGE_SWING_1200
#define DP_PRE_EMPHASIS_MAX    DP_TRAIN_PRE_EMPHASIS_9_5

#define RK3288_GRF_SOC_CON6	0x025c
#define RK3288_GRF_SOC_CON12	0x0274
#define RK3399_GRF_SOC_CON20	0x6250
#define RK3399_GRF_SOC_CON25	0x6264

enum rockchip_dp_types {
	RK3288_DP = 0,
	RK3399_EDP
};

struct rockchip_dp_data {
	unsigned long reg_vop_big_little;
	unsigned long reg_vop_big_little_sel;
	unsigned long reg_ref_clk_sel;
	unsigned long ref_clk_sel_bit;
	enum rockchip_dp_types chip_type;
};

struct link_train {
	unsigned char revision;
	u8 link_rate;
	u8 lane_count;
};

struct rk_edp_priv {
	struct rk3288_edp *regs;
	void *grf;
	struct udevice *panel;
	struct link_train link_train;
	u8 train_set[4];
};


static int rk_edp_enable(struct udevice *dev, int panel_bpp,
			 const struct display_timing *edid)
{
	return 0;
}

static int rk_edp_read_edid(struct udevice *dev, u8 *buf, int buf_size)
{
	return 0;

}

static int rk_edp_of_to_plat(struct udevice *dev)
{
	return 0;
}

static int rk_edp_remove(struct udevice *dev)
{
	return 0;
}

static int rk_edp_probe(struct udevice *dev)
{
	return 0;
}

static const struct dm_display_ops dp_rockchip_ops = {
	.read_edid = rk_edp_read_edid,
	.enable = rk_edp_enable,
};

static const struct rockchip_dp_data rk3399_edp = {
	.reg_vop_big_little = RK3399_GRF_SOC_CON20,
	.reg_vop_big_little_sel = BIT(5),
	.reg_ref_clk_sel = RK3399_GRF_SOC_CON25,
	.ref_clk_sel_bit = BIT(11),
	.chip_type = RK3399_EDP,
};

static const struct rockchip_dp_data rk3288_dp = {
	.reg_vop_big_little = RK3288_GRF_SOC_CON6,
	.reg_vop_big_little_sel = BIT(5),
	.reg_ref_clk_sel = RK3288_GRF_SOC_CON12,
	.ref_clk_sel_bit = BIT(4),
	.chip_type = RK3288_DP,
};

static const struct udevice_id rockchip_dp_ids[] = {
	{ .compatible = "rockchip,rk3288-edp", .data = (ulong)&rk3288_dp },
	{ .compatible = "rockchip,rk3399-edp", .data = (ulong)&rk3399_edp },
	{ }
};

U_BOOT_DRIVER(dp_rockchip) = {
	.name	= "edp_rockchip",
	.id	= UCLASS_DISPLAY,
	.of_match = rockchip_dp_ids,
	.ops	= &dp_rockchip_ops,
	.of_to_plat	= rk_edp_of_to_plat,
	.probe	= rk_edp_probe,
	.remove	= rk_edp_remove,
	.priv_auto	= sizeof(struct rk_edp_priv),
};
