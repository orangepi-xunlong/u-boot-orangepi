/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2017 Theobroma Systems Design und Consulting GmbH
 */

#ifndef __SOPH_DW_HDMI_H__
#define __SOPH_DW_HDMI_H__
#include "soph_vo.h"


/* HDMI_TX_PHY CFG*/

#define OPMODE_PLLCFG	0x06 // Mode of Operation and PLL  Dividers Control Register
#define PLLCURRCTRL		0x10 // PLL Current Control Register
#define PLLDIVCTRL		0x11 // PLL Dividers Control Register
#define TXTERM			0x19 // Transmission Termination Register
#define VLEVCTRL		0x0E // Voltage Level Control Register
#define CKSYMTXCTRL		0x09 // Clock Symbol and Transmitter Control Register

#define LT_1_65GBPS_TXTERM		0x0007
#define LT_1_65GBPS_VLEVCTRL	0x0160
#define LT_1_65GBPS_CKSYMTXCTRL 0x8d88

#define LT_3_40GBPS_TXTERM		0x0000
#define LT_3_40GBPS_VLEVCTRL	0x0120
#define LT_3_40GBPS_CKSYMTXCTRL 0x83F8

#define GT_3_40GBPS_TXTERM		0x0000
#define GT_3_40GBPS_VLEVCTRL	0x0140
#define GT_3_40GBPS_CKSYMTXCTRL 0x80F6

#define LT_1_65GBPS LT_1_65GBPS_TXTERM, LT_1_65GBPS_VLEVCTRL, LT_1_65GBPS_CKSYMTXCTRL
#define LT_3_40GBPS LT_3_40GBPS_TXTERM, LT_3_40GBPS_VLEVCTRL, LT_3_40GBPS_CKSYMTXCTRL
#define GT_3_40GBPS GT_3_40GBPS_TXTERM, GT_3_40GBPS_VLEVCTRL, GT_3_40GBPS_CKSYMTXCTRL

#define SE9_LT_1_65GBPS_TXTERM		0x0007
#define SE9_LT_1_65GBPS_VLEVCTRL	0x0120
#define SE9_LT_1_65GBPS_CKSYMTXCTRL 0x8d88

#define SE9_LT_3_40GBPS_TXTERM		0x0000
#define SE9_LT_3_40GBPS_VLEVCTRL	0x0120
#define SE9_LT_3_40GBPS_CKSYMTXCTRL 0x83F8

#define SE9_GT_3_40GBPS_TXTERM		0x0000
#define SE9_GT_3_40GBPS_VLEVCTRL	0x0080
#define SE9_GT_3_40GBPS_CKSYMTXCTRL 0x8FF4

#define SE9_LT_1_65GBPS SE9_LT_1_65GBPS_TXTERM, SE9_LT_1_65GBPS_VLEVCTRL, SE9_LT_1_65GBPS_CKSYMTXCTRL
#define SE9_LT_3_40GBPS SE9_LT_3_40GBPS_TXTERM, SE9_LT_3_40GBPS_VLEVCTRL, SE9_LT_3_40GBPS_CKSYMTXCTRL
#define SE9_GT_3_40GBPS SE9_GT_3_40GBPS_TXTERM, SE9_GT_3_40GBPS_VLEVCTRL, SE9_GT_3_40GBPS_CKSYMTXCTRL

struct soph_hdmi_driverdata {
	/* configuration */
	u8 i2c_clk_high;
	u8 i2c_clk_low;
	const char * const *regulator_names;
	u32 regulator_names_cnt;
	/* setters/getters */
	int (*set_input_vop)(struct udevice *dev);
	int (*clk_config)(struct udevice *dev);
};

struct soph_hdmi_priv {
	struct dw_hdmi hdmi;
	void *grf;
};

enum pixel_repetition_t {
	PIXEL_REPETITION_OFF = 0,
	PIXEL_REPETITION_1 = 1,
	PIXEL_REPETITION_2 = 2,
	PIXEL_REPETITION_3 = 3,
	PIXEL_REPETITION_4 = 4,
	PIXEL_REPETITION_5 = 5,
	PIXEL_REPETITION_6 = 6,
	PIXEL_REPETITION_7 = 7,
	PIXEL_REPETITION_8 = 8,
	PIXEL_REPETITION_9 = 9,
	PIXEL_REPETITION_10 = 10
};

enum operation_mode_t {
	HDMI_14 = 1,
	HDMI_20,
	MHL_24,
	MHL_PACKEDPIXEL
};

enum color_depth_t {
	COLOR_DEPTH_INVALID = 0,
	COLOR_DEPTH_8 = 8,
	COLOR_DEPTH_10 = 10,
	COLOR_DEPTH_12 = 12,
	COLOR_DEPTH_16 = 16
};

struct phy_config {
	enum pixel_repetition_t	pixel;
	enum color_depth_t	color;
	enum operation_mode_t	opmode;
	u16			oppllcfg;
	u16			pllcurrctrl;
	u16			pllgmpctrl;
	u16			txterm;
	u16			vlevctrl;
	u16			cksymtxctrl;
};

/**
 * soph_hdmi_read_edid() - read the attached HDMI/DVI monitor's EDID
 *
 * N.B.: The buffer should be large enough to hold 2 EDID blocks, as
 *       this function calls dw_hdmi_read_edid, which ignores buf_size
 *       argument and assumes that there's always enough space for 2
 *       EDID blocks.
 *
 * @dev:	device
 * @buf:	output buffer for the EDID
 * @buf_size:	number of bytes in the buffer
 * @return number of bytes read if OK, -ve if something went wrong
 */
int soph_hdmi_read_edid(struct udevice *dev, u8 *buf, int buf_size);

/**
 * soph_hdmi_of_to_plat() - common of_to_plat implementation
 *
 * @dev:	device
 * @return 0 if OK, -ve if something went wrong
 */
int soph_hdmi_of_to_plat(struct udevice *dev);

/**
 * soph_hdmi_probe() - common probe implementation
 *
 * Performs the following, common initialisation steps:
 * 1. checks for HPD (i.e. a HDMI monitor being attached)
 * 2. initialises the Designware HDMI core
 * 3. initialises the Designware HDMI PHY
 *
 * @dev:	device
 * @return 0 if OK, -ve if something went wrong
 */
int soph_hdmi_probe(struct udevice *dev);

int soph_dw_hdmi_init(struct soph_connector *conn, struct display_state *state);
void soph_dw_hdmi_deinit(struct soph_connector *conn, struct display_state *state);
int soph_dw_hdmi_prepare(struct soph_connector *conn, struct display_state *state);
int soph_dw_hdmi_enable(struct soph_connector *conn, struct display_state *state);
int soph_dw_hdmi_disable(struct soph_connector *conn, struct display_state *state);
int soph_dw_hdmi_get_timing(struct soph_connector *conn, struct display_state *state);
int soph_dw_hdmi_detect(struct soph_connector *conn, struct display_state *state);
int soph_dw_hdmi_get_edid(struct soph_connector *conn, struct display_state *state);

struct phy_config *phy316_get_configs(struct dw_hdmi *hdmi, unsigned long mpixelclock, u16 width, u16 height,
					enum color_depth_t color, enum pixel_repetition_t pixel);

#endif /* __SOPH_DW_HDMI_H__ */
