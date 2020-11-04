/*
 * Copyright (C) 2019 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SUNXI AXP152  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifndef __AXP152_REGS_H__
#define __AXP152_REGS_H__

//PMIC chip id reg03:bit7-6  bit3-
#define   AXP152_CHIP_ID              (0x05)

#define AXP152_DEVICE_ADDR			(0x3A3)
#ifndef CONFIG_SYS_SUNXI_R_I2C0_SLAVE
#define AXP152_RUNTIME_ADDR			(0x2d)
#else
#ifndef CONFIG_AXP152_SUNXI_I2C_SLAVE
#define AXP152_RUNTIME_ADDR			CONFIG_SYS_SUNXI_R_I2C0_SLAVE
#else
#define AXP152_RUNTIME_ADDR			CONFIG_AXP152_SUNXI_I2C_SLAVE
#endif
#endif

/*#define   AXP15_ADDR              (0x60>>1)*/

//define AXP152 REGISTER
#define   AXP152_MODE_CHGSTATUS      			(0x01)
#define   AXP152_VERSION         	   			(0x03)
#define   AXP152_OUTPUT_CTL     	   			(0x12)
#define   AXP152_ALDO12_MODCTL     	   		(0x13)
#define   AXP152_LDO0_VOL                     (0x15)
#define   AXP152_DLDO2_VOL                    (0x16)
#define   AXP152_DC2OUT_VOL          			(0x23)
#define   AXP152_DC2OUT_DVM          			(0x25)
#define   AXP152_DC1OUT_VOL                 	(0x26)
#define   AXP152_DC3OUT_VOL                 	(0x27)
#define   AXP152_ALDO12OUT_VOL                (0x28)
#define   AXP152_DLDO1OUT_VOL					(0x29)
#define   AXP152_DLDO2OUT_VOL					(0x2A)
#define   AXP152_DC4OUT_VOL					(0x2B)
#define   AXP152_VOFF_SET            			(0x31)
#define   AXP152_OFF_CTL             			(0x32)
#define   AXP152_POK_SET             			(0x36)
#define   AXP152_DCDC_FREQSET        			(0x37)
#define   AXP152_DCDC_MODESET        			(0x80)
#define   AXP152_VOUT_MONITOR        			(0x81)
#define   AXP152_TIMER_CTL           			(0x8A)
#define   AXP152_HOTOVER_CTL         			(0x8F)

//gpio CTL
#define   AXP152_GPIO0_CTL           			(0x90)
#define   AXP152_GPIO1_CTL           			(0x91)
#define   AXP152_GPIO2_CTL           			(0x92)
#define   AXP152_GPIO3_CTL           			(0x93)
#define   AXP152_GPIO2_LDO_MOD      			(0x96)
#define   AXP152_GPIO0123_SIGNAL     			(0x97)
#define	  AXP152_PWM0_FREQ_SET				(0X98)
#define	  AXP152_PWM0_DUTY_CYCLES_SET1		(0X99)
#define	  AXP152_PWM0_DUTY_CYCLES_SET2		(0X9A)
#define	  AXP152_PWM1_FREQ_SET				(0X9B)
#define	  AXP152_PWM1_DUTY_CYCLES_SET1		(0X9C)
#define	  AXP152_PWM1_DUTY_CYCLES_SET2		(0X9D)

//int register
#define		AXP152_INTEN1              		(0x40)
#define		AXP152_INTEN2              		(0x41)
#define		AXP152_INTEN3              		(0x42)
#define		AXP152_INTSTS1             		(0x48)
#define		AXP152_INTSTS2             		(0x49)
#define		AXP152_INTSTS3             		(0x4a)


#endif /* __AXP152_REGS_H__ */

