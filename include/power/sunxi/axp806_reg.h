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

#ifndef   __AXP806_REGS_H__
#define   __AXP806_REGS_H__

//#define   AXP806_ADDR              (0x10)
#define   AXP806_ADDR              (0x36)

//define AXP806 REGISTER
#define   BOOT_POWER806_STARUP_SRC             			(0x00)
#define   BOOT_POWER806_VERSION         	   			(0x03)
#define   BOOT_POWER806_DATA_BUFFER0        			(0x04)
#define   BOOT_POWER806_DATA_BUFFER1        			(0x05)
#define   BOOT_POWER806_DATA_BUFFER2        			(0x06)
#define   BOOT_POWER806_DATA_BUFFER3        			(0x07)

#define   BOOT_POWER806_OUTPUT_CTL1     	   			(0x10)
#define   BOOT_POWER806_OUTPUT_CTL2     	   			(0x11)

#define   BOOT_POWER806_DCAOUT_VOL                  	(0x12)
#define   BOOT_POWER806_DCBOUT_VOL          			(0x13)
#define   BOOT_POWER806_DCCOUT_VOL          			(0x14)
#define   BOOT_POWER806_DCDOUT_VOL          			(0x15)
#define   BOOT_POWER806_DCEOUT_VOL          			(0x16)
#define   BOOT_POWER806_ALDO1OUT_VOL                    (0x17)
#define   BOOT_POWER806_ALDO2OUT_VOL                    (0x18)
#define   BOOT_POWER806_ALDO3OUT_VOL                    (0x19)
#define   BOOT_POWER806_DCMOD_CTL1                  	(0x1A)
#define   BOOT_POWER806_DCMOD_CTL2                  	(0x1B)
#define   BOOT_POWER806_DCFREQ_SET                  	(0x1C)
#define   BOOT_POWER806_DCMONITOR_CTL                  	(0x1D)

#define   BOOT_POWER806_BLDO1OUT_VOL                    (0x20)
#define   BOOT_POWER806_BLDO2OUT_VOL                    (0x21)
#define   BOOT_POWER806_BLDO3OUT_VOL                    (0x22)
#define   BOOT_POWER806_BLDO4OUT_VOL                    (0x23)

#define   BOOT_POWER806_CLDO1OUT_VOL                    (0x24)
#define   BOOT_POWER806_CLDO2OUT_VOL                    (0x25)
#define   BOOT_POWER806_CLDO3OUT_VOL                    (0x26)

#define   BOOT_POWER806_VOFF_SETTING                    (0x31)
#define   BOOT_POWER806_POK_SETTING                     (0x36)

#define   BOOT_POWER806_INTEN1              			(0x40)
#define   BOOT_POWER806_INTEN2              			(0x41)
#define   BOOT_POWER806_INTSTS1             			(0x48)
#define   BOOT_POWER806_INTSTS2             			(0x49)

#endif /* __AXP806_REGS_H__ */
