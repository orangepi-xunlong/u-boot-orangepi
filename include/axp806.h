/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2020 Jernej Skrabec <jernej.skrabec@siol.net>
 */

enum axp806_reg {
	AXP806_CHIP_VERSION = 0x3,
	AXP806_OUTPUT_CTRL1 = 0x10,
	AXP806_DCDCD_VOLTAGE = 0x15,
	AXP806_DCMOD_CTL2 = 0x1b,
	AXP806_SHUTDOWN = 0x32,
};

#define AXP806_CHIP_VERSION_MASK	0xcf

#define AXP806_OUTPUT_CTRL1_DCDCD_EN	(1 << 3)

#define AXP806_POWEROFF			(1 << 7)
