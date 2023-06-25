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
#define   AXP2101_MODE_CHGSTATUS      	(0x01)
#define   AXP2101_OTG_STATUS          	(0x02)
#define   AXP2101_VERSION         	 	(0x03)
#define   AXP2101_DATA_BUFFER0        	(0x04)
#define   AXP2101_DATA_BUFFER1        	(0x05)
#define   AXP2101_DATA_BUFFER2       		(0x06)
#define   AXP2101_DATA_BUFFER3       		(0x07)
#define   AXP2101_OUTPUT_CTL0    	   		(0x80)
#define   AXP2101_OUTPUT_CTL1     	 	(0x81)
#define   AXP2101_OUTPUT_CTL2     		(0x90)
#define   AXP2101_OUTPUT_CTL3     		(0x91)

#define   AXP2101_DC1OUT_VOL				(0x82)
#define   AXP2101_DC2OUT_VOL          	(0x83)
#define   AXP2101_DC3OUT_VOL          	(0x84)
#define   AXP2101_DC4OUT_VOL          	(0x85)
#define   AXP2101_DC5OUT_VOL          	(0x86)
#define   AXP2101_ALDO1OUT_VOL			(0x92)
#define   AXP2101_ALDO2OUT_VOL			(0x93)
#define   AXP2101_ALDO3OUT_VOL			(0x94)
#define   AXP2101_ALDO4OUT_VOL			(0x95)
#define   AXP2101_BLDO1OUT_VOL			(0x96)
#define   AXP2101_BLDO2OUT_VOL			(0x97)
#define   AXP2101_CPUSLDO_VOL				(0x98)
#define   AXP2101_DLDO1OUT_VOL			(0x99)
#define   AXP2101_DLDO2OUT_VOL			(0x9A)

#define   AXP2101_PWRON_STATUS           	(0x20)
#define   AXP2101_VBUS_VOL_SET         	(0x15)
#define   AXP2101_VBUS_CUR_SET          	(0x16)
#define   AXP2101_OFF_CTL             	(0x10)
#define   AXP2101_CHARGE1             	(0x62)
#define   AXP2101_CHGLED_SET             	(0x69)

#define   AXP2101_INTEN0              	(0x40)
#define   AXP2101_INTEN1              	(0x41)
#define   AXP2101_INTEN2              	(0x42)

#define   AXP2101_INTSTS0             	(0x48)
#define   AXP2101_INTSTS1             	(0x49)
#define   AXP2101_INTSTS2             	(0x4a)



#endif /* __AXP2101_REGS_H__ */

