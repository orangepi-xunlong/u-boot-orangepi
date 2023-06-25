// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020 Allwinnertech Technology Corporation
 *
 */

/* CPU specific code */
#include <common.h>
#include <irq_func.h>
#include <asm/cache.h>

/*
 * cleanup_before_linux() is called just before we call linux
 * it prepares the processor for linux
 *
 * we disable interrupt and caches.
 */
int cleanup_before_linux(void)
{
	disable_interrupts();

	/* turn off I/D-cache */
	cache_flush();
	icache_disable();
	dcache_disable();

	return 0;
}
