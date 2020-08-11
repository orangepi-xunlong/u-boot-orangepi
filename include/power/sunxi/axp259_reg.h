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

#ifndef   __AXP259_REGS_H__
#define   __AXP259_REGS_H__

#define   AXP259_ADDR              (0x36)

#define   BOOT_POWER259_POWER_SOURCE_STATUS   (0x00)
#define   BOOT_POWER259_STARTUP_SOURCE        (0x02)
#define   BOOT_POWER259_IC_TYPE_NUM           (0x03)
#define   BOOT_POWER259_DATA_BUFFER0          (0x05)
#define   BOOT_POWER259_DATA_BUFFER1          (0x06)
#define   BOOT_POWER259_RESTART_POWOFF        (0x28)
#define   BOOT_POWER259_BAT_VOL               (0x78)
#define   BOOT_POWER259_BAT_PERCENTAGE        (0xB9)
#define   BOOT_POWER259_ADDR_EXTENSION        (0xff)

#endif /* __AXP152_REGS_H__ */

