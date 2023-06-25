/*
 *  * Copyright 2000-2009
 *   * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *    *
 *     * SPDX-License-Identifier:	GPL-2.0+
 *     */
#include <common.h>
#include <asm/io.h>
#include <physical_key.h>
#include <sys_config.h>
#include <fdt_support.h>
#include <console.h>
#include <asm/arch/clock.h>

int sunxi_gpadc_init(void)
{
	uint reg_val = 0;
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	/* reset */
	reg_val = readl(&ccm->gpadc_gate_reset);
	reg_val &= ~(1 << 16);
	writel(reg_val, ccm->gpadc_gate_reset);

	udelay(2);

	reg_val |= (1 << 16);
	writel(reg_val, ccm->gpadc_gate_reset);

	/* enable ADC gating */
	reg_val = readl(&ccm->gpadc_gate_reset);
	reg_val |= (1 << 0);
	writel(reg_val, ccm->gpadc_gate_reset);

	/*choose channel 0*/
	reg_val = readl(GP_CS_EN);
	reg_val |= 1;
	writel(reg_val, GP_CS_EN);

	/*choose continue work mode and enable ADC*/
	reg_val = readl(GP_CTRL);
	reg_val &= ~(1<<18);
	reg_val |= ((1<<19) | (1<<16));
	writel(reg_val, GP_CTRL);

	/* disable all key irq */
	writel(0, GP_DATA_INTC);
	writel(1, GP_DATA_INTS);

	return 0;

}


int sunxi_gpadc_read(int channel)
{
	u32 ints, i;
	int key = -1;
	u32 snum = 0;
	ints = readl(GP_DATA_INTS);
	/* clear the pending data */
	writel(readl(GP_DATA_INTS)|(ints & 0x1), GP_DATA_INTS);
	/* if there is already data pending, read it */
	if (ints & GPADC0_DATA_PENDING) {
		for (i = 0; i < 5; i++) {
			snum += readl(GP_CH0_DATA + (channel * 4));
			udelay(5);
		}
		key = snum / (i - 1);
		printf("adc value=0x%x\n", key);
	}
	return key;
}


