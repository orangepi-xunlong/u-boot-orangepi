/*
 * Allwinner SoCs hdmi2.0 driver.
 *
 * Copyright (C) 2016 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef INCLUDE_HDMITX_DEV_H_
#define INCLUDE_HDMITX_DEV_H_
#include "../../config.h"

#if defined(__LINUX_PLAT__)
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sys_config.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/atomic.h>
#include <linux/kthread.h>
#include <linux/poll.h>
#include <linux/fcntl.h>
#else
#include "../../hdmi_boot.h"
#endif
#include "core_api.h"



/**
 * @short Main structures to instantiate the driver
 */
struct hdmi_tx_phy {
	int version;

	int generation;
	/** (@b user) Context status: closed (0), opened (<0) and configured
	 *  (>0) */
	int status;
};

/**
 * @short HDMI TX controller status information
 *
 * Initialize @b user fields (set status to zero).
 * After opening this data is for internal use only.
 */
struct hdmi_tx_ctrl {
	/** (@b user) Context status: closed (0), opened (<0) and
	 *  configured (>0) */
	u8 data_enable_polarity;
	u8 hspol;
	u8 vspol;

	u32 pixel_clock;
	u8 pixel_repetition;
	u32 tmds_clk;
	u8 color_resolution;
	u8 csc_on; /*csc_on=1,color space clock will be enabled*/
	u8 audio_on;
	u8 cec_on;
	u8 hdcp_on;
	u8 hdcp_encrypt_on;
	enum hdmi_hdcp_type hdcp_type;
	u8 hdmi_on;
	phy_access_t phy_access;
};

/**
 * @short Main structures to instantiate the driver
 */
typedef struct hdmi_tx_dev_api {
	char					device_name[20];

	/** SYNOPSYS DATA */
	struct hdmi_tx_ctrl	snps_hdmi_ctrl;

	hdcpParams_t			hdcp;
} hdmi_tx_dev_t;

#endif /* INCLUDE_HDMITX_DEV_H_ */
