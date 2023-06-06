/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2020 Jernej Skrabec <jernej.skrabec@siol.net>
 */

enum axp313a_reg {
	AXP313A_CHIP_VERSION = 0x3,
	AXP313A_OUTPUT_CTRL1 = 0x10,
	AXP313A_DCDCD_VOLTAGE = 0x15,
	AXP313A_SHUTDOWN = 0x32,
};

#define AXP313A_CHIP_VERSION_MASK	0xcf

#define AXP313A_OUTPUT_CTRL1_DCDCD_EN	(1 << 3)

#define AXP313A_POWEROFF		(1 << 7)
