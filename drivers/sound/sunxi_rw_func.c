/*
 * (C) Copyright 2014-2019
 * allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * caiyongheng <caiyongheng@allwinnertech.com>
 *
 * some simple description for this code
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 */

#include "sunxi_rw_func.h"
/*analog register*/
u32 read_prcm_wvalue(u32 addr, void __iomem *ADDA_PR_CFG_REG)
{
	u32 reg;

	reg = readl(ADDA_PR_CFG_REG);
	reg |= (0x1 << 28);
	writel(reg, ADDA_PR_CFG_REG);

	reg = readl(ADDA_PR_CFG_REG);
	reg &= ~(0x1 << 24);
	writel(reg, ADDA_PR_CFG_REG);

	reg = readl(ADDA_PR_CFG_REG);
	reg &= ~(ADDR_WIDTH << 16);
	reg |= (addr << 16);
	writel(reg, ADDA_PR_CFG_REG);

	reg = readl(ADDA_PR_CFG_REG);
	reg &= (0xff << 0);

	return reg;
}

void write_prcm_wvalue(u32 addr, u32 val, void __iomem *ADDA_PR_CFG_REG)
{
	u32 reg;

	reg = readl(ADDA_PR_CFG_REG);
	reg |= (0x1 << 28);
	writel(reg, ADDA_PR_CFG_REG);

	reg = readl(ADDA_PR_CFG_REG);
	reg &= ~(ADDR_WIDTH << 16);
	reg |= (addr << 16);
	writel(reg, ADDA_PR_CFG_REG);

	reg = readl(ADDA_PR_CFG_REG);
	reg &= ~(0xff << 8);
	reg |= (val << 8);
	writel(reg, ADDA_PR_CFG_REG);

	reg = readl(ADDA_PR_CFG_REG);
	reg |= (0x1 << 24);
	writel(reg, ADDA_PR_CFG_REG);

	reg = readl(ADDA_PR_CFG_REG);
	reg &= ~(0x1 << 24);
	writel(reg, ADDA_PR_CFG_REG);
}

/*digital*/
u32 codec_rdreg(void __iomem *address)
{
	return readl(address);
}

void codec_wrreg(void __iomem *address, u32 val)
{
	writel(val, address);
}

/**
* codec_wrreg_bits - update codec register bits
* @reg: codec register
* @mask: register mask
* @value: new value
*
* Writes new register value.
* Return 1 for change else 0.
*/
u32 codec_wrreg_prcm_bits(void __iomem *ADDA_PR_CFG_REG, u32 reg,
			  u32 mask, u32 value)
{
	u32 old, new;

	old	=	read_prcm_wvalue(reg, ADDA_PR_CFG_REG);
	new	=	(old & ~mask) | value;
	write_prcm_wvalue(reg, new, ADDA_PR_CFG_REG);

	return 0;
}

/**
* codec_wrreg_bits - update codec register bits
* @reg: codec register
* @mask: register mask
* @value: new value
*
* Writes new register value.
* Return 1 for change else 0.
*/
u32 codec_wrreg_bits(void __iomem *address, u32 mask, u32 value)
{
	u32 old, new;

	old	=	codec_rdreg(address);
	new	=	(old & ~mask) | value;
	codec_wrreg(address, new);

	return 0;
}

u32 codec_wr_control(void __iomem *reg, u32 mask, u32 shift, u32 val)
{
	u32 reg_val;

	reg_val = val << shift;
	mask = mask << shift;
	codec_wrreg_bits(reg, mask, reg_val);
	return 0;
}
