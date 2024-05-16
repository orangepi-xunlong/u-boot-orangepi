// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018, Bin Meng <bmeng.cn@gmail.com>
 */

#include <common.h>
#include <fdtdec.h>
#include <init.h>
#include <linux/sizes.h>

DECLARE_GLOBAL_DATA_PTR;

__weak int board_ddr_size(void)
{
	return 0;
}

int dram_init(void)
{
	int ret;
	phys_size_t size;

	ret = fdtdec_setup_mem_size_base();
	if (ret)
		return ret;

	/* resize ddr size */
	size = board_ddr_size();
	if (size > 0)
		gd->ram_size = size << 30;

	return 0;
}

int dram_init_banksize(void)
{
	int ret;

	ret = fdtdec_setup_memory_banksize();
	if (ret)
		return ret;

	gd->bd->bi_dram[0].size = gd->ram_size;

	return 0;
}

ulong board_get_usable_ram_top(ulong total_size)
{
#ifdef CONFIG_64BIT
	/*
	 * Ensure that we run from first 4GB so that all
	 * addresses used by U-Boot are 32bit addresses.
	 *
	 * This in-turn ensures that 32bit DMA capable
	 * devices work fine because DMA mapping APIs will
	 * provide 32bit DMA addresses only.
	 */
	if (gd->ram_top > SZ_4G)
		return SZ_4G;
#endif
	return gd->ram_top;
}
