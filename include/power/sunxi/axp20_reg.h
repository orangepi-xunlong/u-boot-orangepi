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

#ifndef   __AXP209_REGS_H__
#define   __AXP209_REGS_H__


#define ABS(x) ((x > 0)?(x):(-(x)))


#define DCDC2_VALUE_FOR_BOOT    (1320)      //define dcdc2 to 1.32v for boot
#define AXP20_ADDR              (0x68>>1)

//define AXP20 REGISTER
#define   BOOT_POWER20_STATUS              			(0x00)
#define   BOOT_POWER20_MODE_CHGSTATUS      			(0x01)
#define   BOOT_POWER20_OTG_STATUS          			(0x02)
#define   BOOT_POWER20_VERSION         	   			(0x03)
#define   BOOT_POWER20_DATA_BUFFER0        			(0x04)
#define   BOOT_POWER20_DATA_BUFFER1        			(0x05)
#define   BOOT_POWER20_DATA_BUFFER2        			(0x06)
#define   BOOT_POWER20_DATA_BUFFER3        			(0x07)
#define   BOOT_POWER20_DATA_BUFFER4        			(0x08)
#define   BOOT_POWER20_DATA_BUFFER5        			(0x09)
#define   BOOT_POWER20_DATA_BUFFER6        			(0x0a)
#define   BOOT_POWER20_DATA_BUFFER7        			(0x0b)
#define   BOOT_POWER20_DATA_BUFFER8        			(0x0c)
#define   BOOT_POWER20_DATA_BUFFER9        			(0x0d)
#define   BOOT_POWER20_DATA_BUFFER10       			(0x0e)
#define   BOOT_POWER20_DATA_BUFFER11       			(0x0f)

#define   BOOT_POWER20_OUTPUT_CTL      	   			(0x12)
#define   BOOT_POWER20_DC2OUT_VOL          			(0x23)
#define   BOOT_POWER20_LDO3_DC2_DVM        			(0x25)
#define   BOOT_POWER20_DC3OUT_VOL          			(0x27)
#define   BOOT_POWER20_LDO24OUT_VOL        			(0x28)
#define   BOOT_POWER20_LDO3OUT_VOL         			(0x29)
#define   BOOT_POWER20_IPS_SET             			(0x30)
#define   BOOT_POWER20_VOFF_SET            			(0x31)
#define   BOOT_POWER20_OFF_CTL             			(0x32)
#define   BOOT_POWER20_CHARGE1             			(0x33)
#define   BOOT_POWER20_CHARGE2             			(0x34)
#define   BOOT_POWER20_BACKUP_CHG          			(0x35)
#define   BOOT_POWER20_POK_SET             			(0x36)
#define   BOOT_POWER20_DCDC_FREQSET        			(0x37)
#define   BOOT_POWER20_VLTF_CHGSET         			(0x38)
#define   BOOT_POWER20_VHTF_CHGSET         			(0x39)
#define   BOOT_POWER20_APS_WARNING1        			(0x3A)
#define   BOOT_POWER20_APS_WARNING2        			(0x3B)
#define   BOOT_POWER20_VLTF_DISCHGSET      			(0x3C)
#define   BOOT_POWER20_VHTF_DISCHGSET      			(0x3D)
#define   BOOT_POWER20_DCDC_MODESET        			(0x80)
#define   BOOT_POWER20_VOUT_MONITOR        			(0x81)
#define   BOOT_POWER20_ADC_EN1             			(0x82)
#define   BOOT_POWER20_ADC_EN2             			(0x83)
#define   BOOT_POWER20_ADC_SPEED           			(0x84)
#define   BOOT_POWER20_ADC_INPUTRANGE      			(0x85)
#define   BOOT_BOOT_POWER20_GPIO1ADC_VTH   			(0x86)
#define   BOOT_POWER20_GPIO1ADC_VTL        			(0x87)
#define   BOOT_POWER20_TIMER_CTL           			(0x8A)
#define   BOOT_POWER20_VBUS_DET_SRP        			(0x8B)
#define   BOOT_POWER20_HOTOVER_CTL         			(0x8F)
#define   BOOT_POWER20_GPIO0_CTL           			(0x90)
#define   BOOT_POWER20_GPIO0_VOL           			(0x91)
#define   BOOT_POWER20_GPIO1_CTL           			(0x92)
#define   BOOT_POWER20_GPIO2_CTL           			(0x93)
#define   BOOT_POWER20_GPIO012_SIGNAL      			(0x94)
#define   BOOT_POWER20_GPIO3_CTL           			(0x95)
#define   BOOT_POWER20_GPIO012_PDCTL       			(0x97)
#define   BOOT_POWER20_INTEN1              			(0x40)
#define   BOOT_POWER20_INTEN2              			(0x41)
#define   BOOT_POWER20_INTEN3              			(0x42)
#define   BOOT_POWER20_INTEN4              			(0x43)
#define   BOOT_POWER20_INTEN5              			(0x44)
#define   BOOT_POWER20_INTSTS1             			(0x48)
#define   BOOT_POWER20_INTSTS2             			(0x49)
#define   BOOT_POWER20_INTSTS3             			(0x4a)
#define   BOOT_POWER20_INTSTS4             			(0x4b)
#define   BOOT_POWER20_INTSTS5             			(0x4c)
#define   BOOT_POWER20_COULOMB_CTL         			(0xB8)
#define   BOOT_POWER20_COULOMB_CAL					(0xB9)
#define   BOOT_POWER20_RDC1                         (0xBA)
#define   BOOT_POWER20_RDC0                         (0xBB)
#define   BOOT_POWER20_OCV1                         (0xBC)
#define   BOOT_POWER20_OCV0                         (0xBD)

//adc data register
#define   BOOT_POWER20_BAT_AVERVOL_H8          		(0x78)
#define   BOOT_POWER20_BAT_AVERVOL_L4          		(0x79)
#define   BOOT_POWER20_BAT_AVERCHGCUR_H8       		(0x7A)
#define   BOOT_POWER20_BAT_AVERCHGCUR_L4       		(0x7B)
#define   BOOT_POWER20_ACIN_VOL_H8             		(0x56)
#define   BOOT_POWER20_ACIN_VOL_L4             		(0x57)
#define   BOOT_POWER20_ACIN_CUR_H8             		(0x58)
#define   BOOT_POWER20_ACIN_CUR_L4             		(0x59)
#define   BOOT_POWER20_VBUS_VOL_H8             		(0x5A)
#define   BOOT_POWER20_VBUS_VOL_L4             		(0x5B)
#define   BOOT_POWER20_VBUS_CUR_H8             		(0x5C)
#define   BOOT_POWER20_VBUS_CUR_L4             		(0x5D)
#define   BOOT_POWER20_BAT_AVERDISCHGCUR_H8    		(0x7C)
#define   BOOT_POWER20_BAT_AVERDISCHGCUR_L5    		(0x7D)
#define   BOOT_POWER20_APS_AVERVOL_H8          		(0x7E)
#define   BOOT_POWER20_APS_AVERVOL_L4          		(0x7F)
#define   BOOT_POWER20_BAT_CHGCOULOMB3         		(0xB0)
#define   BOOT_POWER20_BAT_CHGCOULOMB2         		(0xB1)
#define   BOOT_POWER20_BAT_CHGCOULOMB1         		(0xB2)
#define   BOOT_POWER20_BAT_CHGCOULOMB0         		(0xB3)
#define   BOOT_POWER20_BAT_DISCHGCOULOMB3      		(0xB4)
#define   BOOT_POWER20_BAT_DISCHGCOULOMB2      		(0xB5)
#define   BOOT_POWER20_BAT_DISCHGCOULOMB1      		(0xB6)
#define   BOOT_POWER20_BAT_DISCHGCOULOMB0      		(0xB7)
#define   BOOT_POWER20_BAT_POWERH8             		(0x70)
#define   BOOT_POWER20_BAT_POWERM8             		(0x71)
#define   BOOT_POWER20_BAT_POWERL8             		(0x72)


#endif /* __AXP209_REGS_H__ */
