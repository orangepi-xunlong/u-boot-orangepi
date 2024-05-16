// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020 SiFive, Inc
 *
 * Authors:
 *   Pragnesh Patel <pragnesh.patel@sifive.com>
 */

#include <common.h>
#include <asm/io.h>
#include <asm/global_data.h>


DECLARE_GLOBAL_DATA_PTR;

#if CONFIG_IS_ENABLED(STARFIVE_JH7110_L2CC_FLUSH)
#define L2_CACHE_FLUSH64	0x200
#define L2_CACHE_BASE_ADDR 	0x2010000

void flush_dcache_range(unsigned long start, unsigned long end)
{
	unsigned long line;
	volatile unsigned long *flush64;

	/* make sure the address is in the range */
	if(start > end ||
		start < CONFIG_STARFIVE_JH7110_L2CC_FLUSH_START ||
		end > (CONFIG_STARFIVE_JH7110_L2CC_FLUSH_START +
				CONFIG_STARFIVE_JH7110_L2CC_FLUSH_SIZE))
		return;

	/*In order to improve the performance, change base addr to a fixed value*/
	flush64 = (volatile unsigned long *)(L2_CACHE_BASE_ADDR + L2_CACHE_FLUSH64);

	/* memory barrier */
	mb();
	for (line = start; line < end; line += CONFIG_SYS_CACHELINE_SIZE) {
		(*flush64) = line;
		/* memory barrier */
		mb();
	}

	return;
}

void invalidate_dcache_range(unsigned long start, unsigned long end)
{
	flush_dcache_range(start,end);
}
#endif //SIFIVE_FU540_L2CC_FLUSH
