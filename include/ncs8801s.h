/*
 * include/ncs8801s.h
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 * Author: wanpeng <wanpeng@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef _NCS8801S_H_
#define _NCS8801S_H_
#include <common.h>
#include <i2c.h>
#include <sunxi_i2c.h>
#include <sys_config.h>


static inline int ncs8801s_i2c_readByte(unsigned char dev_addr_temp, unsigned char addr_temp)
{
	uint8_t dev_addr = dev_addr_temp;
	uint32_t addr = addr_temp;
	uint8_t buffer = 0x00;
	i2c_read(dev_addr, addr, 1, &buffer, 1);
	return buffer;
}

static inline int ncs8801s_i2c_writeByte(unsigned char dev_addr_temp, unsigned char addr_temp, unsigned char data)
{
	uint8_t dev_addr = dev_addr_temp;
	uint32_t addr = addr_temp;
	uint8_t buffer = data;
	return i2c_write(dev_addr, addr, 1, &buffer, 1);
}

#endif
