/*
 * Copyright (C) 2016 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SUNXI AXP21  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifndef __AXP81X_H__
#define __AXP81X_H__

//PMIC chip id reg03:bit7-6  bit3-
#define   AXP81X_CHIP_ID              (0x41)

#define AXP81X_DEVICE_ADDR			(0x3A3)
#ifndef CONFIG_SYS_SUNXI_R_I2C0_SLAVE
#define AXP81X_RUNTIME_ADDR			(0x2d)
#else
#ifndef CONFIG_AXP81X_SUNXI_I2C_SLAVE
#define AXP81X_RUNTIME_ADDR			CONFIG_SYS_SUNXI_R_I2C0_SLAVE
#else
#define AXP81X_RUNTIME_ADDR			CONFIG_AXP81X_SUNXI_I2C_SLAVE
#endif
#endif

#define AXP81X_POWER_KEY_STATUS AXP81X_INTSTS5
#define AXP81X_POWER_KEY_OFFSET 0x3

/*define AXP803 REGISTER */
#define   AXP81X_STATUS              			(0x00)
#define   AXP81X_MODE_CHGSTATUS      			(0x01)
#define   AXP81X_OTG_STATUS          			(0x02)
#define   AXP81X_VERSION         	   			(0x03)
#define   AXP81X_DATA_BUFFER0        			(0x04)
#define   AXP81X_DATA_BUFFER1        			(0x05)
#define   AXP81X_DATA_BUFFER2        			(0x06)
#define   AXP81X_DATA_BUFFER3        			(0x07)
#define   AXP81X_DATA_BUFFER4        			(0x08)
#define   AXP81X_DATA_BUFFER5        			(0x09)
#define   AXP81X_DATA_BUFFER6        			(0x0a)
#define   AXP81X_DATA_BUFFER7        			(0x0b)
#define   AXP81X_DATA_BUFFER8        			(0x0c)
#define   AXP81X_DATA_BUFFER9        			(0x0d)
#define   AXP81X_DATA_BUFFER10       			(0x0e)
#define   AXP81X_DATA_BUFFER11       			(0x0f)
#define   AXP81X_OUTPUT_CTL1     	   			(0x10)
#define   AXP81X_OUTPUT_CTL2     	   			(0x12)
#define   AXP81X_ALDO_CTL     	   			    (0x13)
#define   AXP81X_DLDO1_VOL                       (0x15)
#define   AXP81X_DLDO2_VOL                       (0x16)
#define   AXP81X_DLDO3_VOL                       (0x17)
#define   AXP81X_DLDO4_VOL                       (0x18)

#define   AXP81X_ELDO1_VOL                       (0x19)
#define   AXP81X_ELDO2_VOL                       (0x1A)
#define   AXP81X_ELDO3_VOL                       (0x1B)
#define   AXP81X_FLDO1_VOL                       (0x1C)
#define   AXP81X_FLDO2_VOL						(0x1D)
#define   AXP81X_DC1OUT_VOL                  	(0x20)
#define   AXP81X_DC2OUT_VOL          			(0x21)
#define   AXP81X_DC3OUT_VOL          			(0x22)
#define   AXP81X_DC4OUT_VOL          			(0x23)
#define   AXP81X_DC5OUT_VOL          			(0x24)
#define   AXP81X_DC6OUT_VOL          			(0x25)
#define   AXP81X_DC7OUT_VOL          			(0x26)


#define   AXP81X_DC23_DVM_CTL          			(0x27)
#define   AXP81X_ALDO1OUT_VOL                    (0x28)
#define   AXP81X_ALDO2OUT_VOL                    (0x29)
#define   AXP81X_ALDO3OUT_VOL                    (0x2A)

#define   AXP81X_VBUS_SET             			(0x30)
#define   AXP81X_VOFF_SET            			(0x31)
#define   AXP81X_OFF_CTL             			(0x32)
#define   AXP81X_CHARGE1             			(0x33)
#define   AXP81X_CHARGE2             			(0x34)
#define   AXP81X_CHARGE3             			(0x35)

#define   AXP81X_BAT_AVERVOL_H8                  (0x78)
#define   AXP81X_BAT_AVERVOL_L4                  (0x79)

#define   AXP81X_DCDC_MODESET        			(0x80)
#define   AXP81X_VOUT_MONITOR        			(0x81)
#define   AXP81X_ADC_EN             			    (0x82)
#define   AXP81X_ADC_SPEED_TS           			(0x84)
#define   AXP81X_ADC_SPEED      			        (0x85)
#define   AXP81X_TIMER_CTL           			(0x8A)
#define   AXP81X_HOTOVER_CTL         			(0x8F)
#define   AXP81X_GPIO0_CTL           			(0x90)
#define   AXP81X_GPIO0_VOL           			(0x91)
#define   AXP81X_GPIO1_CTL           			(0x92)
#define   AXP81X_GPIO1_VOL           			(0x93)
#define   AXP81X_GPIO012_SIGNAL      			(0x94)

#define   AXP81X_GPIO012_PDCTL       			(0x97)

#define   AXP81X_INTEN1              			(0x40)
#define   AXP81X_INTEN2              			(0x41)
#define   AXP81X_INTEN3              			(0x42)
#define   AXP81X_INTEN4              			(0x43)
#define   AXP81X_INTEN5              			(0x44)
#define   AXP81X_INTEN6              			(0x45)


#define   AXP81X_INTSTS1             			(0x48)
#define   AXP81X_INTSTS2             			(0x49)
#define   AXP81X_INTSTS3             			(0x4a)
#define   AXP81X_INTSTS4             			(0x4b)
#define   AXP81X_INTSTS5             			(0x4c)
#define   AXP81X_INTSTS6             			(0x4d)

#define   AXP81X_FUEL_GAUGE_CTL         			(0xB8)
#define   AXP81X_BAT_PERCEN_CAL					(0xB9)

#define   AXP81X_RDC1                			(0xBA)
#define   AXP81X_RDC0         					(0xBB)
#define   AXP81X_OCV1                			(0xBC)
#define   AXP81X_OCV0         					(0xBD)

#define   AXP81X_BAT_MAX_CAP1   					(0xE0)
#define   AXP81X_BAT_MAX_CAP0   					(0xE1)
#define   AXP81X_BAT_COULOMB_CNT1				(0xE2)
#define   AXP81X_BAT_COULOMB_CNT0				(0xE3)

#define   AXP81X_DCDC1_PWM_BIT				(0)
#define   AXP81X_DCDC2_PWM_BIT				(1)
#define   AXP81X_DCDC3_PWM_BIT				(2)
#define   AXP81X_DCDC4_PWM_BIT				(3)
#define   AXP81X_DCDC5_PWM_BIT				(4)
#define   AXP81X_DCDC6_PWM_BIT				(5)
#define   AXP81X_DCDC7_PWM_BIT				(6)

#endif /* __AXP81X_REGS_H__ */

