/*
 * Copyright (C) 2016 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SUNXI AXP21  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifndef __AXP2101_H__
#define __AXP2101_H__

//PMIC chip id reg03:bit7-6  bit3-
#define   AXP2101_CHIP_ID              (0x47)

#ifndef CONFIG_SYS_I2C_SUNXI
#define AXP2101_RUNTIME_ADDR			(0x2d)
#define AXP2101_DEVICE_ADDR			(0x3A3)
#else
#define AXP2101_DEVICE_ADDR			(0)
#ifndef CONFIG_AXP2101_SUNXI_I2C_SLAVE
#define AXP2101_RUNTIME_ADDR			CONFIG_SYS_SUNXI_R_I2C0_SLAVE
#else
#define AXP2101_RUNTIME_ADDR			CONFIG_AXP2101_SUNXI_I2C_SLAVE
#endif
#endif



/* define AXP21 REGISTER */
#define   AXP2101_COMM_STATUS0          	(0x00)
#define   AXP2101_MODE_CHGSTATUS      	(0x01)
#define   AXP2101_OTG_STATUS          	(0x02)
#define   AXP2101_VERSION         	 	(0x03)


#define   AXP2101_PWRON_STATUS           	(0x20)
#define   AXP2101_VBUS_VOL_SET         	(0x15)
#define   AXP2101_VBUS_CUR_SET          	(0x16)
#define   AXP2101_VOFF_THLD            	(0x24)
#define   AXP2101_OFF_CTL             	(0x10)
#define   AXP2101_CHARGE1             	(0x62)
#define   AXP2101_CHGLED_SET             	(0x69)

#define   AXP2101_BAT_AVERVOL_H6          (0x34)
#define   AXP2101_BAT_AVERVOL_L8          (0x35)

#define   AXP2101_FUEL_GAUGE_CTL       	(0x18)
#define   AXP2101_BAT_PERCEN_CAL			(0xA4)

#endif /* __AXP2101_REGS_H__ */


