// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020 Allwinnertech Technology Corporation
 *
 */

#include <common.h>
#include <asm/cache.h>
#include <asm/csr.h>

#define L1_CACHE_BYTES CONFIG_SYS_CACHELINE_SIZE

void flush_dcache_all(void)
{
	;
}

void flush_dcache_range(unsigned long start, unsigned long end)
{
	register unsigned long i asm("a0") = start & ~(L1_CACHE_BYTES - 1);
	for (; i < end; i += L1_CACHE_BYTES)
		asm volatile(".long 0x0295000b");	/*dcache.cpa a0*/
	asm volatile(".long 0x01b0000b");		/*sync.is*/
}

void invalidate_dcache_range(unsigned long start, unsigned long end)
{
	register unsigned long i asm("a0") = start & ~(L1_CACHE_BYTES - 1);

	for (; i < end; i += L1_CACHE_BYTES)
		asm volatile ("dcache.ipa a0");

	asm volatile (".long 0x01b0000b");
	/* flush_dcache_all(); */
}

void icache_enable(void)
{
	;
}

void icache_disable(void)
{
	;
}

void dcache_enable(void)
{
	;
}

void dcache_disable(void)
{
	;
}

int icache_status(void)
{
	int ret = 0;

	return ret;
}

int dcache_status(void)
{
	int ret = 0;

	return ret;
}
