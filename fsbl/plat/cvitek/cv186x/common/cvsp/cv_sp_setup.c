/*
 * Copyright (c) 2015-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <debug.h>
#include <bl_common.h>
#include <console.h>
#include <platform_def.h>
#include <platform_cvsp.h>
#include <cv_private.h>
#include <plat_arm.h>
#include <common_def.h>

#define BL32_END (unsigned long)(&__BL32_END__)

/* Weak definitions may be overridden in specific ARM standard platform */
#pragma weak bmsp_early_platform_setup
#pragma weak bmsp_platform_setup
#pragma weak bmsp_plat_arch_setup


/*******************************************************************************
 * Initialize the UART
 ******************************************************************************/
void bmsp_early_platform_setup(void)
{
	// uint32_t clk_in_hz = plat_bm_get_uart_clock();

	// /*
	//  * Initialize a different console than already in use to display
	//  * messages from BMSP
	//  */
	// console_init(PLAT_BM_BOOT_UART_BASE, clk_in_hz,
	// 		PLAT_BM_CONSOLE_BAUDRATE);
	console_init(PLAT_BOOT_UART_BASE, PLAT_BM_BOOT_UART_CLK_IN_HZ,
		     PLAT_BM_CONSOLE_BAUDRATE);
}

/*******************************************************************************
 * Perform platform specific setup placeholder
 ******************************************************************************/
void bmsp_platform_setup(void)
{
	plat_arm_gic_driver_init();
}

/*******************************************************************************
 * Perform the very early platform specific architectural setup here. At the
 * moment this is only intializes the MMU
 ******************************************************************************/
void bmsp_plat_arch_setup(void)
{
	uint64_t v;

	const uintptr_t bl32_start = (uintptr_t)(BL32_BASE);
	const uintptr_t bl32_end = (uintptr_t)(&__BL32_END__);

	const uintptr_t ro_start = (uintptr_t)(&__RO_START__);
	const uintptr_t ro_end = (uintptr_t)(&__RO_END__);

	VERBOSE("%s(): BL32 0x%lx 0x%lx\n", __func__, bl32_start, bl32_end);
	mmap_add_region(bl32_start, bl32_start, bl32_end - bl32_start,
			MT_MEMORY | MT_RW | MT_SECURE);

	VERBOSE("%s(): RO 0x%lx 0x%lx\n", __func__, ro_start, ro_end);
	mmap_add_region(ro_start, ro_start, ro_end - ro_start,
			MT_MEMORY | MT_CODE | MT_RO | MT_SECURE);

	VERBOSE("%s(): CO 0x%lx 0x%lx\n", __func__, BL_COHERENT_RAM_BASE,
		BL_COHERENT_RAM_END);
	mmap_add_region(BL_COHERENT_RAM_BASE, BL_COHERENT_RAM_BASE, BL_COHERENT_RAM_END - BL_COHERENT_RAM_BASE,
			MT_DEVICE | MT_RW | MT_SECURE);

	VERBOSE("%s(): DEVICE 0x%x 0x%x\n", __func__, 0, 0x80000000);
	mmap_add_region(0, 0, 0x80000000, MT_DEVICE | MT_RW | MT_SECURE);

	VERBOSE("%s(): DRAM 0x%lx 0x%lx\n", __func__, DRAM_BASE, 0x800000000);
	mmap_add_region(DRAM_BASE, DRAM_BASE, 0x800000000, MT_MEMORY | MT_RW | MT_NS);

	init_xlat_tables();

	enable_mmu_el1(0);

	// Enable unaligned access
	__asm__ volatile("mrs %0, sctlr_el1" : "=r"(v));
	v &= ~(uint64_t)SCTLR_A_BIT;
	__asm__ volatile("msr sctlr_el1, %0\nisb" : : "r"(v));
}
