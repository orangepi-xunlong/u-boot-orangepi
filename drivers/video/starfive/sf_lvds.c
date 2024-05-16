// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2016 Rockchip Inc.
 */

#include <common.h>
#include <display.h>
#include <dm.h>
#include <edid.h>
#include <log.h>
#include <panel.h>
#include <regmap.h>
#include <syscon.h>
#include <asm/global_data.h>
#include <asm/gpio.h>
#include <asm/io.h>

#include <dt-bindings/clock/rk3288-cru.h>
#include <dt-bindings/video/rk3288.h>

DECLARE_GLOBAL_DATA_PTR;

/**
 * struct rk_lvds_priv - private rockchip lvds display driver info
 *
 * @reg: LVDS register address
 * @grf: GRF register
 * @panel: Panel device that is used in driver
 *
 * @output: Output mode, decided single or double channel,
 *		LVDS or LVTLL
 * @format: Data format that RGB data will packing as
 */
struct rk_lvds_priv {
	void __iomem *regs;
	struct udevice *panel;

	int output;
	int format;
};

int rk_lvds_enable(struct udevice *dev, int panel_bpp,
		   const struct display_timing *edid)
{
	return 0;
}

int rk_lvds_read_timing(struct udevice *dev, struct display_timing *timing)
{
	return 0;
}

static int rk_lvds_of_to_plat(struct udevice *dev)
{
	return 0;
}

int rk_lvds_probe(struct udevice *dev)
{

	return 0;
}

static const struct dm_display_ops lvds_rockchip_ops = {
	.read_timing = rk_lvds_read_timing,
	.enable = rk_lvds_enable,
};

static const struct udevice_id rockchip_lvds_ids[] = {
	{.compatible = "rockchip,rk3288-lvds"},
	{}
};

U_BOOT_DRIVER(lvds_rockchip) = {
	.name	= "lvds_rockchip",
	.id	= UCLASS_DISPLAY,
	.of_match = rockchip_lvds_ids,
	.ops	= &lvds_rockchip_ops,
	.of_to_plat	= rk_lvds_of_to_plat,
	.probe	= rk_lvds_probe,
	.priv_auto	= sizeof(struct rk_lvds_priv),
};
