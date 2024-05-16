/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2017 Theobroma Systems Design und Consulting GmbH
 */

#ifndef __SF_HDMI_H__
#define __SF_HDMI_H__

#include <clk.h>
#include <reset.h>

#define HDMI_STATUS				0xc8
#define m_HOTPLUG				(1 << 7)
#define m_MASK_INT_HOTPLUG		(1 << 5)
#define m_INT_HOTPLUG			(1 << 1)
#define v_MASK_INT_HOTPLUG(n)	((n & 0x1) << 5)

typedef struct
{
    /* TODO: add hdmi inno registers define */
} hdmi_regs_t;

typedef struct register_value {
	u16 reg;
	u8 value;
}reg_value_t;

struct sf_hdmi_priv {
	struct dw_hdmi hdmi;
	void *grf;
	void __iomem *base;

	struct clk  pclk;
	struct clk  sys_clk;
	struct clk  mclk;
	struct clk  bclk;
	struct clk  phy_clk;
	struct reset_ctl  tx_rst;
};

typedef enum {
	RES_1440_480I_60HZ = 0,
	RES_640_480P_60HZ,
	RES_720_480P_60HZ,
	RES_1280_720P_60HZ,
	RES_1920_1080P_60HZ,
	RES_3840_2160P_30HZ,
	RES_3840_2160P_60HZ,
}resolution_t;

typedef enum {
	VIC_1440x480i60 = 6,
	VIC_640x480p60 = 1,
	VIC_720x480p60 = 2,
	VIC_1280x720p60 = 4,
	VIC_1920x1080p60 = 16,
	VIC_3840x2160p30 = 95,
	VIC_3840x2160p60 = 97,
}vic_code_t;


#endif
