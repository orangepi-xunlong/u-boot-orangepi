/*
 *  linux/include/asm-arm/proc-armv/system.h
 *
 *  Copyright (C) 1996 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __ASM_PROC_SYSTEM_H
#define __ASM_PROC_SYSTEM_H

/*
 * Save the current interrupt enable state & disable IRQs
 */

/*
 * Save the current interrupt enable state
 * and disable IRQs/FIQs
 */
void __attribute__((weak)) local_irq_save(unsigned int flag)
{
	;
}

/*
 * Enable IRQs
 */
void __attribute__((weak)) local_irq_enable(unsigned int flag)
{
	;
}

/*
 * Disable IRQs
 */
void __attribute__((weak)) local_irq_disable(unsigned int flag)
{
	;
}

/*
 * Enable FIQs
 */
void __attribute__((weak)) __stf(void)
{
	;
}

/*
 * Disable FIQs
 */
void __attribute__((weak)) __clf(void)
{
	;
}

/*
 * Save the current interrupt enable state.
 */
void __attribute__((weak)) local_save_flags(unsigned int flag)
{
	;
}

/*
 * restore saved IRQ & FIQ state
 */
void __attribute__((weak)) local_irq_restore(unsigned int flag)
{
	;
}

#if defined(CONFIG_CPU_SA1100) || defined(CONFIG_CPU_SA110) || \
	defined(CONFIG_ARM64)
/*
 * On the StrongARM, "swp" is terminally broken since it bypasses the
 * cache totally.  This means that the cache becomes inconsistent, and,
 * since we use normal loads/stores as well, this is really bad.
 * Typically, this causes oopsen in filp_close, but could have other,
 * more disasterous effects.  There are two work-arounds:
 *  1. Disable interrupts and emulate the atomic swap
 *  2. Clean the cache, perform atomic swap, flush the cache
 *
 * We choose (1) since its the "easiest" to achieve here and is not
 * dependent on the processor type.
 */
#define swp_is_buggy
#endif

#endif
