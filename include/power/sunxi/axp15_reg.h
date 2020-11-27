/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Young <guoyingyang@allwinnertech.com>
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

#ifndef   __AXP152_REGS_H__
#define   __AXP152_REGS_H__

#define   AXP15_ADDR              (0x60>>1)

//define AXP152 REGISTER
#define   BOOT_POWER15_MODE_CHGSTATUS      			(0x01)
#define   BOOT_POWER15_VERSION         	   			(0x03)
#define   BOOT_POWER15_OUTPUT_CTL     	   			(0x12)
#define   BOOT_POWER15_ALDO12_MODCTL     	   		(0x13)
#define   BOOT_POWER15_LDO0_VOL                     (0x15)
#define   BOOT_POWER15_DLDO2_VOL                    (0x16)
#define   BOOT_POWER15_DC2OUT_VOL          			(0x23)
#define   BOOT_POWER15_DC2OUT_DVM          			(0x25)
#define   BOOT_POWER15_DC1OUT_VOL                 	(0x26)
#define   BOOT_POWER15_DC3OUT_VOL                 	(0x27)
#define   BOOT_POWER15_ALDO12OUT_VOL                (0x28)
#define   BOOT_POWER15_DLDO1OUT_VOL					(0x29)
#define   BOOT_POWER15_DLDO2OUT_VOL					(0x2A)
#define   BOOT_POWER15_DC4OUT_VOL					(0x2B)
#define   BOOT_POWER15_VOFF_SET            			(0x31)
#define   BOOT_POWER15_OFF_CTL             			(0x32)
#define   BOOT_POWER15_POK_SET             			(0x36)
#define   BOOT_POWER15_DCDC_FREQSET        			(0x37)
#define   BOOT_POWER15_DCDC_MODESET        			(0x80)
#define   BOOT_POWER15_VOUT_MONITOR        			(0x81)
#define   BOOT_POWER15_TIMER_CTL           			(0x8A)
#define   BOOT_POWER15_HOTOVER_CTL         			(0x8F)

//gpio CTL
#define   BOOT_POWER15_GPIO0_CTL           			(0x90)
#define   BOOT_POWER15_GPIO1_CTL           			(0x91)
#define   BOOT_POWER15_GPIO2_CTL           			(0x92)
#define   BOOT_POWER15_GPIO3_CTL           			(0x93)
#define   BOOT_POWER15_GPIO2_LDO_MOD      			(0x96)
#define   BOOT_POWER15_GPIO0123_SIGNAL     			(0x97)
#define	  BOOT_POWER15_PWM0_FREQ_SET				(0X98)
#define	  BOOT_POWER15_PWM0_DUTY_CYCLES_SET1		(0X99)
#define	  BOOT_POWER15_PWM0_DUTY_CYCLES_SET2		(0X9A)
#define	  BOOT_POWER15_PWM1_FREQ_SET				(0X9B)
#define	  BOOT_POWER15_PWM1_DUTY_CYCLES_SET1		(0X9C)
#define	  BOOT_POWER15_PWM1_DUTY_CYCLES_SET2		(0X9D)

//int register
#define		BOOT_POWER15_INTEN1              		(0x40)
#define		BOOT_POWER15_INTEN2              		(0x41)
#define		BOOT_POWER15_INTEN3              		(0x42)
#define		BOOT_POWER15_INTSTS1             		(0x48)
#define		BOOT_POWER15_INTSTS2             		(0x49)
#define		BOOT_POWER15_INTSTS3             		(0x4a)


#endif /* __AXP152_REGS_H__ */

