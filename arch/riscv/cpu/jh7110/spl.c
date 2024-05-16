// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022-2023 StarFive Technology Co., Ltd.
 * Author:	yanhong <yanhong.wang@starfivetech.com>
 *
 */

#include <dm.h>
#include <log.h>
#include <asm/csr.h>
#include <init.h>

#define CSR_U74_FEATURE_DISABLE	0x7c1

int spl_soc_init(void)
{
	int ret;
	struct udevice *dev;

	/*read memory size info from eeprom and
	 *init gd->ram_size variable
	 */
	dram_init();

	/* DDR init */
	ret = uclass_get_device(UCLASS_RAM, 0, &dev);
	if (ret) {
		debug("DRAM init failed: %d\n", ret);
		return ret;
	}

	/*flash init*/
	ret = uclass_get_device(UCLASS_SPI_FLASH, 0, &dev);
	if (ret) {
		debug("SPI init failed: %d\n", ret);
		return ret;
	}
	return 0;
}

void harts_early_init(void)
{
	/*
	 * Feature Disable CSR
	 *
	 * Clear feature disable CSR to '0' to turn on all features for
	 * each core. This operation must be in M-mode.
	 */
	if (CONFIG_IS_ENABLED(RISCV_MMODE))
		csr_write(CSR_U74_FEATURE_DISABLE, 0);

#ifdef CONFIG_SPL_BUILD

	/*clear L2 LIM  memory
	 * set __bss_end to 0x81e0000 region to zero
	 */
	__asm__ __volatile__ (
		"la t1, __bss_end\n"
		"li t2, 0x81e0000\n"
		"spl_clear_l2im:\n"
			"addi t1, t1, 8\n"
			"sd zero, 0(t1)\n"
			"blt t1, t2, spl_clear_l2im\n");
#endif
}
