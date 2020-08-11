/*
 * (C) Copyright 2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * zhouhuacai <zhouhuacai@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/key.h>
#include <asm/arch/ccmu.h>

__attribute__((section(".data")))
static uint32_t keyen_flag = 1;

__weak int sunxi_key_clock_open(void)
{
	return 0;
}

int sunxi_key_init(void)
{
	struct sunxi_lradc *sunxi_key_base = (struct sunxi_lradc *)SUNXI_KEYADC_BASE;
	uint reg_val = 0;

	sunxi_key_clock_open();

	reg_val = sunxi_key_base->ctrl;
	reg_val &= ~((7 << 1) | (0xffU << 24));
	reg_val |=  LRADC_HOLD_EN;
	reg_val |=  LRADC_EN;
	sunxi_key_base->ctrl = reg_val;

	/* disable all key irq */
	sunxi_key_base->intc = 0;
	sunxi_key_base->ints = 0x1f1f;

	return 0;
}

int sunxi_key_read(void)
{
	u32 ints;
	int key = -1;
	struct sunxi_lradc *sunxi_key_base = (struct sunxi_lradc *)SUNXI_KEYADC_BASE;

	if (!keyen_flag) {
		return -1;
	}

	ints = sunxi_key_base->ints;
	/* clear the pending data */
	sunxi_key_base->ints |= (ints & 0x1f);

	/* if there is already data pending,
	 read it */
	if ( ints & ADC0_KEYDOWN_PENDING) {
		if (ints & ADC0_DATA_PENDING) {
			key = sunxi_key_base->data0 & 0x3f;

			if (!key) {
				key = -1;
			}
		}
	} else if (ints & ADC0_DATA_PENDING) {
		key = sunxi_key_base->data0 & 0x3f;

		if (!key) {
			key = -1;
		}
	}

	if (key > 0) {
		printf("key pressed value=0x%x\n", key);
	}

	return key;
}

