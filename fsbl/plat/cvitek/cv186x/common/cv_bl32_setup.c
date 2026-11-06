/*
 * Copyright (c) 2014-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <assert.h>
#include <platform.h>
#include <xlat_mmu_helpers.h>

void bl32_plat_enable_mmu(uint32_t flags)
{
	uint64_t v;

	enable_mmu_el1(flags);

	// Enable unaligned access
	__asm__ volatile("mrs %0, sctlr_el1" : "=r"(v));
	v &= ~(uint64_t)SCTLR_A_BIT;
	__asm__ volatile("msr sctlr_el1, %0\nisb" : : "r"(v));
}
