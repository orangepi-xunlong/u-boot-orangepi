/*
 * Copyright (C) 2016 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SUNXI AXP2202  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifndef __AXP2202_H__
#define __AXP2202_H__

//PMIC chip id reg03:bit7-6  bit3-
#define   AXP2202_CHIP_VER              (0x00)


#define AXP2202_DEVICE_ADDR			(0x3A3)
#ifndef CONFIG_SYS_SUNXI_R_I2C0_SLAVE
#define AXP2202_RUNTIME_ADDR			(0x2d)
#else
#define AXP2202_RUNTIME_ADDR			CONFIG_SYS_SUNXI_R_I2C0_SLAVE
#endif


/* define AXP21 REGISTER */
#define   AXP2202_COMM_STATUS0          	(0x00)
#define   AXP2202_MODE_CHGSTATUS      	(0x01)
#define   AXP2202_CHIP_ID         	 	(0x03)
#define   AXP2202_VERSION					(0x04)
#define   AXP2202_CURVE_CHECK            (0x0c)
#define   AXP2202_CHIP_ID_EXT				(0x0e)

#define   AXP2202_COMM_CFG	        	(0x10)
#define   AXP2202_VBUS_VOL_SET         	(0x16)
#define   AXP2202_VBUS_CUR_SET          	(0x17)
#define   AXP2202_RESET_CFG             (0x18)
#define   AXP2202_MODULE_EN           (0x19)
#define   AXP2202_PWRON_STATUS           	(0x20)
#define   AXP2202_VOFF_THLD            	(0x24)
#define   AXP2202_OFF_CTL             	(0x27)
#define   AXP2202_TS_CFG             	(0x50)
#define   AXP2202_CHARGE1             	(0x62)

#define   AXP2202_CHGLED_SET             	(0x70)

#define   AXP2202_ADC_CH_EN0             	(0xc0)
#define   AXP2202_BAT_AVERVOL_H6          (0xc4)
#define   AXP2202_BAT_AVERVOL_L8          (0xc5)

#define   AXP2202_FUEL_GAUGE_CTL       	(0x0B)
#define   AXP2202_BAT_PERCEN_CAL			(0xA4)
#define   AXP2202_GAUGE_CONFIG			(0xA2)
#endif /* __AXP2202_REGS_H__ */


