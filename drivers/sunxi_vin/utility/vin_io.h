/*
 * utility/vin_io.h
 *
 * Copyright (C) 2014 Allwinnertech Co., Ltd.
 * Copyright (C) 2015 Yang Feng
 *
 * Author: Yang Feng <yangfeng@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */
#ifndef _VFE_IO_H_
#define _VFE_IO_H_

#include <common.h>
#include <asm/io.h>

static inline u32 vin_reg_readl(unsigned long addr)
{
	return readl(addr);
}

static inline void vin_reg_writel(unsigned long addr, u32 reg_value)
{
	writel(reg_value, addr);
}

static inline void vin_reg_clr(unsigned long reg, u32 clr_bits)
{
	u32 v = vin_reg_readl(reg);
	vin_reg_writel(reg, v & ~clr_bits);
}

static inline void vin_reg_set(unsigned long reg, u32 set_bits)
{
	u32 v = vin_reg_readl(reg);
	vin_reg_writel(reg, v | set_bits);
}

/*
 * clr_bits for mask
 */
static inline
void vin_reg_clr_set(unsigned long reg, u32 clr_bits, u32 set_bits)
{
	u32 v = vin_reg_readl(reg);
	vin_reg_writel(reg, (v & ~clr_bits) | (set_bits & clr_bits));
}

#endif /*_VFE_IO_H_*/
