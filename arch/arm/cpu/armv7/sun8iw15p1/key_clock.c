/*
 * (C) Copyright 2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */


#include <asm/io.h>
#include <asm/arch/key.h>
#include <asm/arch/ccmu.h>
#include <common.h>

int sunxi_key_clock_open(void)
{
	uint reg_val = 0;

	/* reset */
	reg_val = readl(CCMU_GPADC_BGR_REG);
	reg_val &= ~(1<<16);
	writel(reg_val, CCMU_GPADC_BGR_REG);
	udelay(2);
	reg_val |=  (1<<16);
	writel(reg_val, CCMU_GPADC_BGR_REG);

	/* enable KEYADC gating */
	reg_val = readl(CCMU_GPADC_BGR_REG);
	reg_val |= (1<<0);
	writel(reg_val, CCMU_GPADC_BGR_REG);

	return 0;
}

int sunxi_key_clock_close(void)
{
	uint reg_val = 0;

	/* disable KEYADC gating */
	reg_val = readl(CCMU_GPADC_BGR_REG);
	reg_val &= ~(1<<0);
	writel(reg_val, CCMU_GPADC_BGR_REG);

	return 0;
}

