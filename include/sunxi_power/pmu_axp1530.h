/*
 * Copyright (C) 2016 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SUNXI AXP1530  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifndef __AXP1530_H__
#define __AXP1530_H__

//PMIC chip id reg03:bit7-6  bit3-
#define   AXP1530_CHIP_ID              (0x48)
#define   AXP313A_CHIP_ID              (0x4B)
#define   AXP313B_CHIP_ID              (0x4C)

#define AXP1530_DEVICE_ADDR			(0x3A3)
#ifndef CONFIG_SYS_SUNXI_R_I2C0_SLAVE
#define AXP1530_RUNTIME_ADDR			(0x2d)
#else
#ifndef CONFIG_AXP1530_SUNXI_I2C_SLAVE
#define AXP1530_RUNTIME_ADDR			CONFIG_SYS_SUNXI_R_I2C0_SLAVE
#else
#define AXP1530_RUNTIME_ADDR                    CONFIG_AXP1530_SUNXI_I2C_SLAVE
#endif
#endif

/* define AXP1530 REGISTER */
#define	AXP1530_POWER_ON_SOURCE_INDIVATION			(0x00)
#define	AXP1530_POWER_OFF_SOURCE_INDIVATION			(0x01)
#define	AXP1530_VERSION								(0x03)
#define	AXP1530_OUTPUT_POWER_ON_OFF_CTL				(0x10)
#define AXP1530_DCDC_DVM_PWM_CTL					(0x12)
#define	AXP1530_DC1OUT_VOL							(0x13)
#define	AXP1530_DC2OUT_VOL          				(0x14)
#define	AXP1530_DC3OUT_VOL          				(0x15)
#define	AXP1530_ALDO1OUT_VOL						(0x16)
#define	AXP1530_DLDO1OUT_VOL						(0x17)
#define	AXP1530_POWER_DOMN_SEQUENCE					(0x1A)
#define	AXP1530_PWROK_VOFF_SERT						(0x1B)
#define AXP1530_POWER_WAKEUP_CTL					(0x1C)
#define AXP1530_OUTPUT_MONITOR_CONTROL				(0x1D)
#define	AXP1530_POK_SET								(0x1E)
#define	AXP1530_IRQ_ENABLE							(0x20)
#define	AXP1530_IRQ_STATUS							(0x21)
#define AXP1530_WRITE_LOCK							(0x70)
#define AXP1530_ERROR_MANAGEMENT					(0x71)
#define	AXP1530_DCDC1_2_POWER_ON_DEFAULT_SET		(0x80)
#define	AXP1530_DCDC3_ALDO1_POWER_ON_DEFAULT_SET	(0x81)


#endif /* __AXP1530_REGS_H__ */


