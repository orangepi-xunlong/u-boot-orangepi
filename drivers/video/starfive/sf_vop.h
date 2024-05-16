/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2017 Theobroma Systems Design und Consulting GmbH
 */

#ifndef __RK_VOP_H__
#define __RK_VOP_H__
#include <clk.h>
#include <reset.h>

#define AQ_INTR_ACKNOWLEDGE				0x0010
#define AQ_INTR_ENBL					0x0014
#define DC_HW_REVISION					0x0024
#define DC_HW_CHIP_CID					0x0030

#define DC_REG_BASE						0x0800
#define DC_REG_RANGE					0x2000
#define DC_SEC_REG_OFFSET				0x100000

 //define power management i2c cmd(reg+data)
#define POWER_SW_0_REG                           (0x00+0x80)
#define POWER_SW_0_VDD18_HDMI                    0
#define POWER_SW_0_VDD18_MIPITX                  1
#define POWER_SW_0_VDD18_MIPIRX                  2
#define POWER_SW_0_VDD09_HDMI                    3
#define POWER_SW_0_VDD09_MIPITX                  4
#define POWER_SW_0_VDD09_MIPIRX                  5

enum vop_modes {
	VOP_MODE_EDP = 0,
	VOP_MODE_MIPI,
	VOP_MODE_HDMI,
	VOP_MODE_LVDS,
	VOP_MODE_DP,
};

struct sf_vop_priv {
	void __iomem * regs_hi;
	void __iomem * regs_low;
	struct udevice *conn_dev;
	struct display_timing timings;

	struct clk disp_axi;
	struct clk vout_src;
	struct clk top_vout_axi;
	struct clk top_vout_ahb;

	struct clk dc_pix0;
	struct clk dc_pix1;
	struct clk dc_axi;
	struct clk dc_core;
	struct clk dc_ahb;

	struct clk top_vout_lcd;
	struct clk hdmitx0_pixelclk;
	struct clk dc_pix_src;
	struct clk dc_pix0_out;
	struct clk dc_pix1_out;

	struct reset_ctl vout_resets;

//20221014
	struct reset_ctl dc8200_rst_axi;
	struct reset_ctl dc8200_rst_core;
	struct reset_ctl dc8200_rst_ahb;

	struct reset_ctl rst_vout_src;
	struct reset_ctl noc_disp;
	bool   mipi_logo;
	bool   hdmi_logo;
};

enum vop_features {
	VOP_FEATURE_OUTPUT_10BIT = (1 << 0),
};

struct rkvop_driverdata {
	/* configuration */
	u32 features;
	/* block-specific setters/getters */
	void (*set_pin_polarity)(struct udevice *, enum vop_modes, u32);
};

/**
 * rk_vop_probe() - common probe implementation
 *
 * Performs the rk_display_init on each port-subnode until finding a
 * working port (or returning an error if none of the ports could be
 * successfully initialised).
 *
 * @dev:	device
 * @return 0 if OK, -ve if something went wrong
 */
int rk_vop_probe(struct udevice *dev);

/**
 * rk_vop_bind() - common bind implementation
 *
 * Sets the plat->size field to the amount of memory to be reserved for
 * the framebuffer: this is always
 *     (32 BPP) x VIDEO_ROCKCHIP_MAX_XRES x VIDEO_ROCKCHIP_MAX_YRES
 *
 * @dev:	device
 * @return 0 (always OK)
 */
int rk_vop_bind(struct udevice *dev);

/**
 * rk_vop_probe_regulators() - probe (autoset + enable) regulators
 *
 * Probes a list of regulators by performing autoset and enable
 * operations on them.  The list of regulators is an array of string
 * pointers and any individual regulator-probe may fail without
 * counting as an error.
 *
 * @dev:	device
 * @names:	array of string-pointers to regulator names to probe
 * @cnt:	number of elements in the 'names' array
 */
void rk_vop_probe_regulators(struct udevice *dev,
			     const char * const *names, int cnt);

#endif
