// SPDX-License-Identifier: GPL-2.0+
/*
 * DRAM init helper functions
 *
 * (C) Copyright 2015 Hans de Goede <hdegoede@redhat.com>
 */

#include <common.h>
#include <time.h>
#include <asm/barriers.h>
#include <asm/io.h>
#include <asm/arch/dram.h>

/*
 * Wait up to 1s for value to be set in given part of reg.
 */
void mctl_await_completion(u32 *reg, u32 mask, u32 val)
{
	unsigned long tmo = timer_get_us() + 1000000;

	while ((readl(reg) & mask) != val) {
		if (timer_get_us() > tmo)
			panic("Timeout initialising DRAM\n");
	}
}

/*
 * Test if memory at offset offset matches memory at begin of DRAM
 *
 * Note: dsb() is not available on ARMv5 in Thumb mode
 */
#ifndef CONFIG_MACH_SUNIV
bool mctl_mem_matches(u32 offset)
{
	dsb();
	/* Try to write different values to RAM at two addresses */
	writel(0, CFG_SYS_SDRAM_BASE);
	writel(0xaa55aa55, (ulong)CFG_SYS_SDRAM_BASE + offset);
	dsb();
	/* Check if the same value is actually observed when reading back */
	return readl(CFG_SYS_SDRAM_BASE) ==
	       readl((ulong)CFG_SYS_SDRAM_BASE + offset);
}

/*
 * Test if memory at offset matches memory at top of DRAM
 */
bool mctl_mem_matches_top(ulong offset)
{
	static const unsigned value= 0xaa55aa55;

	/* Take last usable memory address */
	offset -= sizeof(value);
	dsb();
	/* Set zero at last usable memory address */
	writel(0, (ulong)CFG_SYS_SDRAM_BASE + offset);
	dsb();
	/* Set other value at last usable memory address */
	writel(value, (ulong)CFG_SYS_SDRAM_BASE + offset);
	dsb();
	/* Check if the same value is actually observed when reading back */
	return readl((ulong)CFG_SYS_SDRAM_BASE + offset) == value;
}

/*
 * Get memory address at offset of DRAM
 */
ulong mctl_mem_address(ulong offset)
{
	return (ulong)CFG_SYS_SDRAM_BASE + offset;
}
#endif
