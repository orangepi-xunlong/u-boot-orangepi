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

int sunxi_key_init(void)
{
	uint reg_val = 0;

	sunxi_key_clock_open();

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


int sunxi_key_read(void)
{

	u32 ints;
	int key = 0;
	u32 vin;

	ints = readl(GP_DATA_INTS);

	/* clear the pending status */
	writel((ints & 0x1), GP_DATA_INTS);

	/* if there is already data pending,read it .
	 * gpadc_data = Vin/Vref*4095, and Vref=1.8v.
	 * Vin should be 0~1.8v.
	 */
	if (ints & GPADC0_DATA_PENDING) {
		vin = readl(GP_CH0_DATA)*18/4095;
		key = vin > 16 ? 0 : (readl(GP_CH0_DATA)*63/4095);
	}

#ifdef CONFIG_ARCH_SUN50IW3P1
	/* Fix me: use lradc on perf board,
	 * so get the invalid val 0x17 by gpadc driver.
	*/
	if (key == 0x17) {
		int i = 0, tmp = 0;
		for (i = 0; i < 5; i++)
			tmp += readl(GP_CH0_DATA)*63/4095;
		tmp /= 5;
		if (tmp == key) {
			printf("get invalid gpadc(0x17) on perf\n");
			key = 0;
		}
	}
#endif
#ifdef CONFIG_ARCH_SUN8IW15P1
	/* Fix me: use lradc on perf board,
	 * so get the invalid val 0x17 by gpadc driver.
	*/
	if (key == 0x18) {
		int i = 0, tmp = 0;
		for (i = 0; i < 5; i++)
			tmp += readl(GP_CH0_DATA)*63/4095;
		tmp /= 5;
		if (tmp == key) {
			printf("get invalid gpadc(0x18) on perf\n");
			key = 0;
		}
	}
#endif

	if (key > 0)
		printf("key pressed value=0x%x\n", key);

	return key;
}


