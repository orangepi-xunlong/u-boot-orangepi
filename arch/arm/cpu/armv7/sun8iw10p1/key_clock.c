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


#include <asm/io.h>
#include <asm/arch/key.h>
#include <asm/arch/ccmu.h>


int sunxi_key_clock_open(void)
{
	uint reg_val = 0,i=0;

	//reset
	reg_val = readl(CCMU_BUS_SOFT_RST_REG2);
	reg_val &= ~(1<<9);
	writel(reg_val, CCMU_BUS_SOFT_RST_REG2);
	for( i = 0; i < 100; i++ );
	reg_val |=  (1<<9);
	writel(reg_val, CCMU_BUS_SOFT_RST_REG2);

	//enable KEYADC gating
	reg_val = readl(CCMU_BUS_CLK_GATING_REG2);
	reg_val |= (1<<9);
	writel(reg_val, CCMU_BUS_CLK_GATING_REG2);

	return 0;
}

int sunxi_key_clock_close(void)
{
	uint reg_val = 0;

	//disable KEYADC gating
	reg_val = readl(CCMU_BUS_CLK_GATING_REG2);
	reg_val &= ~(1<<9);
	writel(reg_val, CCMU_BUS_CLK_GATING_REG2);

	return 0;
}
