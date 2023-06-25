/*
 * Copyright (C) 2016 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SUNXI AXP21  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifndef __AXP221_H__
#define __AXP221_H__

//PMIC chip id reg03:bit7-6  bit3-
#define   AXP221_CHIP_ID              (0x06)
#define   AXP221_CHIP_ID_EXT          (0x4a)

#define AXP221_DEVICE_ADDR			(0x3A3)

#ifndef CONFIG_SYS_SUNXI_R_I2C0_SLAVE
#define AXP221_RUNTIME_ADDR			(0x34)
#else
#ifndef CONFIG_AXP221_SUNXI_I2C_SLAVE
#define AXP221_RUNTIME_ADDR			CONFIG_SYS_SUNXI_R_I2C0_SLAVE
#else
#define AXP221_RUNTIME_ADDR			CONFIG_AXP221_SUNXI_I2C_SLAVE
#endif
#endif

#define AXP221_POWER_KEY_STATUS AXP221_INTSTS5
#define AXP221_POWER_KEY_OFFSET 0x5

/*define AXP221 REGISTER */
#define   AXP221_STATUS              			(0x00)
#define   AXP221_MODE_CHGSTATUS      			(0x01)
#define   AXP221_OTG_STATUS          			(0x02)
#define   AXP221_VERSION         	   			(0x03)
#define   AXP221_DATA_BUFFER0        			(0x04)
#define   AXP221_DATA_BUFFER1        			(0x05)
#define   AXP221_DATA_BUFFER2        			(0x06)
#define   AXP221_DATA_BUFFER3        			(0x07)
#define   AXP221_DATA_BUFFER4        			(0x08)
#define   AXP221_DATA_BUFFER5        			(0x09)
#define   AXP221_DATA_BUFFER6        			(0x0a)
#define   AXP221_DATA_BUFFER7        			(0x0b)
#define   AXP221_DATA_BUFFER8        			(0x0c)
#define   AXP221_DATA_BUFFER9        			(0x0d)
#define   AXP221_DATA_BUFFER10       			(0x0e)
#define   AXP221_DATA_BUFFER11       			(0x0f)
#define   AXP221_OUTPUT_CTL1     	   			(0x10)
#define   AXP221_OUTPUT_CTL2     	   			(0x12)
#define   AXP221_ALDO_CTL     	   			    (0x13)
#define   AXP221_DLDO1_VOL                       (0x15)
#define   AXP221_DLDO2_VOL                       (0x16)
#define   AXP221_DLDO3_VOL                       (0x17)
#define   AXP221_DLDO4_VOL                       (0x18)

#define   AXP221_ELDO1_VOL                       (0x19)
#define   AXP221_ELDO2_VOL                       (0x1A)
#define   AXP221_ELDO3_VOL                       (0x1B)
#define   AXP221_DC5LDO3_VOL                       (0x1C)
#define   AXP221_DC1OUT_VOL                  	(0x21)
#define   AXP221_DC2OUT_VOL          			(0x22)
#define   AXP221_DC3OUT_VOL          			(0x23)
#define   AXP221_DC4OUT_VOL          			(0x24)
#define   AXP221_DC5OUT_VOL          			(0x25)

#define   AXP221_ALDO1OUT_VOL                    (0x28)
#define   AXP221_ALDO2OUT_VOL                    (0x29)
#define   AXP221_ALDO3OUT_VOL                    (0x2A)

#define   AXP221_VBUS_SET             			(0x30)
#define   AXP221_VOFF_SET            			(0x31)
#define   AXP221_OFF_CTL             			(0x32)
#define   AXP221_CHARGE1             			(0x33)
#define   AXP221_CHARGE2             			(0x34)
#define   AXP221_CHARGE3             			(0x35)

#define   AXP221_BAT_AVERVOL_H8                  (0x78)
#define   AXP221_BAT_AVERVOL_L4                  (0x79)

#define   AXP221_DCDC_MODESET        			(0x80)
#define   AXP221_VOUT_MONITOR        			(0x81)
#define   AXP221_ADC_EN             			    (0x82)
#define   AXP221_ADC_SPEED_TS           			(0x84)
#define   AXP221_ADC_SPEED      			        (0x85)
#define   AXP221_TIMER_CTL           			(0x8A)
#define   AXP221_HOTOVER_CTL         			(0x8F)
#define   AXP221_GPIO0_CTL           			(0x90)
#define   AXP221_GPIO0_VOL           			(0x91)
#define   AXP221_GPIO1_CTL           			(0x92)
#define   AXP221_GPIO1_VOL           			(0x93)
#define   AXP221_GPIO012_SIGNAL      			(0x94)

#define   AXP221_GPIO012_PDCTL       			(0x97)

#define   AXP221_INTEN1              			(0x40)
#define   AXP221_INTEN2              			(0x41)
#define   AXP221_INTEN3              			(0x42)
#define   AXP221_INTEN4              			(0x43)
#define   AXP221_INTEN5              			(0x44)

#define   AXP221_INTSTS1             			(0x48)
#define   AXP221_INTSTS2             			(0x49)
#define   AXP221_INTSTS3             			(0x4a)
#define   AXP221_INTSTS4             			(0x4b)
#define   AXP221_INTSTS5             			(0x4c)

#define   AXP221_FUEL_GAUGE_CTL         			(0xB8)
#define   AXP221_BAT_PERCEN_CAL					(0xB9)

#define   AXP221_RDC1                			(0xBA)
#define   AXP221_RDC0         					(0xBB)
#define   AXP221_OCV1                			(0xBC)
#define   AXP221_OCV0         					(0xBD)

#define   AXP221_BAT_MAX_CAP1   					(0xE0)
#define   AXP221_BAT_MAX_CAP0   					(0xE1)
#define   AXP221_BAT_COULOMB_CNT1				(0xE2)
#define   AXP221_BAT_COULOMB_CNT0				(0xE3)

#define   AXP221_DCDC1_PWM_BIT				(0)
#define   AXP221_DCDC2_PWM_BIT				(1)
#define   AXP221_DCDC3_PWM_BIT				(2)
#define   AXP221_DCDC4_PWM_BIT				(3)
#define   AXP221_DCDC5_PWM_BIT				(4)

#endif /* __AXP221_REGS_H__ */

