/*
 * (C) Copyright 2008 Texas Insturments
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * CPU specific code
 */

#include <common.h>
#include <command.h>
#include <asm/system.h>
#include <asm/cache.h>
#include <asm/armv7.h>
#include <linux/compiler.h>
#include <asm/io.h>
#include <asm/arch/timer.h>

void __weak cpu_cache_initialization(void){}

int cleanup_before_linux(void)
{
	/*
	 * this function is called just before we call linux
	 * it prepares the processor for linux
	 *
	 * we turn off caches etc ...
	 */
#ifndef CONFIG_SPL_BUILD
	disable_interrupts();
#endif

	/*
	 * Turn off I-cache and invalidate it
	 */
	icache_disable();
	invalidate_icache_all();
#ifdef CONFIG_ARM_A7
	/*
	 * turn off D-cache
	 * dcache_disable() in turn flushes the d-cache and disables MMU
	 */
	dcache_disable();
	v7_outer_cache_disable();

	/*
	 * After D-cache is flushed and before it is disabled there may
	 * be some new valid entries brought into the cache. We are sure
	 * that these lines are not dirty and will not affect our execution.
	 * (because unwinding the call-stack and setting a bit in CP15 SCTRL
	 * is all we did during this. We have not pushed anything on to the
	 * stack. Neither have we affected any static data)
	 * So just invalidate the entire d-cache again to avoid coherency
	 * problems for kernel
	 */
	invalidate_dcache_all();

	/*
	 * Some CPU need more cache attention before starting the kernel.
	 */
	cpu_cache_initialization();
#endif
	return 0;
}

void disable_smp(void);

int cleanup_before_powerdown(void)
{
#ifdef CONFIG_ARM_A7
	uint32_t reg;
#endif
	/*
	 * Turn off I-cache and invalidate it
	 */
	icache_disable();
	invalidate_icache_all();
#ifdef CONFIG_ARM_A7
	reg = get_cr();
	__msdelay(10);
	set_cr(reg & ~CR_C);

	__msdelay(10);
	flush_dcache_all();

	__asm volatile("clrex");
#endif
	return 0;

}
