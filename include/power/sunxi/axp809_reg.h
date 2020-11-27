/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef   __AXP809_REGS_H__
#define   __AXP809_REGS_H__

#define   AXP809_CHIP_ID              (0x12)

//define AXP809 REGISTER
#define   BOOT_POWER809_STATUS              			(0x00)
#define   BOOT_POWER809_MODE_CHGSTATUS      			(0x01)
#define   BOOT_POWER809_OTG_STATUS          			(0x02)
#define   BOOT_POWER809_VERSION         	   			(0x03)
#define   BOOT_POWER809_DATA_BUFFER0        			(0x04)
#define   BOOT_POWER809_DATA_BUFFER1        			(0x05)
#define   BOOT_POWER809_DATA_BUFFER2        			(0x06)
#define   BOOT_POWER809_DATA_BUFFER3        			(0x07)
#define   BOOT_POWER809_DATA_BUFFER4        			(0x08)
#define   BOOT_POWER809_DATA_BUFFER5        			(0x09)
#define   BOOT_POWER809_DATA_BUFFER6        			(0x0a)
#define   BOOT_POWER809_DATA_BUFFER7        			(0x0b)
#define   BOOT_POWER809_DATA_BUFFER8        			(0x0c)
#define   BOOT_POWER809_DATA_BUFFER9        			(0x0d)
#define   BOOT_POWER809_DATA_BUFFER10       			(0x0e)
#define   BOOT_POWER809_DATA_BUFFER11       			(0x0f)
#define   BOOT_POWER809_OUTPUT_CTL1     	   			(0x10)
#define   BOOT_POWER809_OUTPUT_CTL2     	   			(0x12)
#define   BOOT_POWER809_ALDO_CTL     	   			    (0x13)
#define   BOOT_POWER809_DLDO1_VOL                       (0x15)
#define   BOOT_POWER809_DLDO2_VOL                       (0x16)
#define   BOOT_POWER809_ELDO1_VOL                       (0x19)
#define   BOOT_POWER809_ELDO2_VOL                       (0x1A)
#define   BOOT_POWER809_ELDO3_VOL                       (0x1B)
#define   BOOT_POWER809_DC5LDO3_VOL                     (0x1C)
#define   BOOT_POWER809_DC1OUT_VOL                  	(0x21)
#define   BOOT_POWER809_DC2OUT_VOL          			(0x22)
#define   BOOT_POWER809_DC3OUT_VOL          			(0x23)
#define   BOOT_POWER809_DC4OUT_VOL          			(0x24)
#define   BOOT_POWER809_DC5OUT_VOL          			(0x25)
#define   BOOT_POWER809_DC23_DVM_CTL          			(0x27)
#define   BOOT_POWER809_ALDO1OUT_VOL                    (0x28)
#define   BOOT_POWER809_ALDO2OUT_VOL                    (0x29)
#define   BOOT_POWER809_ALDO3OUT_VOL                    (0x2A)

#define   BOOT_POWER809_VBUS_SET             			(0x30)
#define   BOOT_POWER809_VOFF_SET            			(0x31)
#define   BOOT_POWER809_OFF_CTL             			(0x32)
#define   BOOT_POWER809_CHARGE1             			(0x33)
#define   BOOT_POWER809_CHARGE2             			(0x34)
#define   BOOT_POWER809_CHARGE3             			(0x35)

#define   BOOT_POWER809_BAT_AVERVOL_H8                  (0x50)
#define   BOOT_POWER809_BAT_AVERVOL_L4                  (0x51)

#define   BOOT_POWER809_DCDC_MODESET        			(0x80)
#define   BOOT_POWER809_VOUT_MONITOR        			(0x81)
#define   BOOT_POWER809_ADC_EN             			    (0x82)
#define   BOOT_POWER809_ADC_SPEED           			(0x84)
#define   BOOT_POWER809_TS_ADC_SPEED      			    (0x85)
#define   BOOT_POWER809_TIMER_CTL           			(0x8A)
#define   BOOT_POWER809_HOTOVER_CTL         			(0x8F)
#define   BOOT_POWER809_GPIO0_CTL           			(0x90)
#define   BOOT_POWER809_GPIO0_VOL           			(0x91)
#define   BOOT_POWER809_GPIO1_CTL           			(0x92)
#define   BOOT_POWER809_GPIO1_VOL           			(0x93)
#define   BOOT_POWER809_GPIO012_SIGNAL      			(0x94)
#define   BOOT_POWER809_GPIO3_CTL           			(0x95)
#define   BOOT_POWER809_GPIO012_PDCTL       			(0x97)

#define   BOOT_POWER809_INTEN1              			(0x40)
#define   BOOT_POWER809_INTEN2              			(0x41)
#define   BOOT_POWER809_INTEN3              			(0x42)
#define   BOOT_POWER809_INTEN4              			(0x43)
#define   BOOT_POWER809_INTEN5              			(0x44)
#define   BOOT_POWER809_INTSTS1             			(0x48)
#define   BOOT_POWER809_INTSTS2             			(0x49)
#define   BOOT_POWER809_INTSTS3             			(0x4a)
#define   BOOT_POWER809_INTSTS4             			(0x4b)
#define   BOOT_POWER809_INTSTS5             			(0x4c)

#define   BOOT_POWER809_FUEL_GAUGE_CTL         			(0xB8)
#define   BOOT_POWER809_BAT_PERCEN_CAL					(0xB9)

#define   BOOT_POWER809_RDC1                			(0xBA)
#define   BOOT_POWER809_RDC0         					(0xBB)
#define   BOOT_POWER809_OCV1                			(0xBC)
#define   BOOT_POWER809_OCV0         					(0xBD)

#define   BOOT_POWER809_BAT_MAX_CAP1   					(0xE0)
#define   BOOT_POWER809_BAT_MAX_CAP0   					(0xE1)
#define   BOOT_POWER809_BAT_COULOMB_CNT1				(0xE2)
#define   BOOT_POWER809_BAT_COULOMB_CNT0				(0xE3)

#define   PMU_IRQ_STATU1                    (0x48)
#define   PMU_IRQ_STATU2                    (0x49)

#endif /* __AXP809_REGS_H__ */
