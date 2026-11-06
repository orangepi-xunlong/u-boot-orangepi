/*
 * Copyright (c) 2015-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <asm_macros.S>
#include <assert_macros.S>
#include <platform_def.h>

	.globl	plat_my_core_pos
	.globl	plat_get_my_entrypoint
	.globl	platform_mem_init
	.globl	plat_bm_calc_core_pos
	.globl	plat_crash_console_init
	.globl	plat_crash_console_putc
	.globl  plat_secondary_cold_boot_setup
	.globl  plat_get_my_entrypoint
	.globl  plat_is_my_cpu_primary
	.globl  plat_non_primary_suspend
	.globl  plat_off_mmu
	.globl  plat_primary_suspend

func plat_my_core_pos
	mrs	x0, mpidr_el1
	b	plat_bm_calc_core_pos
endfunc plat_my_core_pos

/*
 *  unsigned int plat_bm_calc_core_pos(u_register_t mpidr);
 *  With this function: CorePos = (ClusterId * 4) + CoreId
 */
func plat_bm_calc_core_pos
	and	x1, x0, #MPIDR_CPU_MASK
	and	x0, x0, #MPIDR_CLUSTER_MASK
	add	x0, x1, x0, LSR #6
	ret
endfunc plat_bm_calc_core_pos

	/* -----------------------------------------------------
	 * unsigned int plat_is_my_cpu_primary (void);
	 *
	 * Find out whether the current cpu is the primary
	 * cpu.
	 * -----------------------------------------------------
	 */
func plat_is_my_cpu_primary
	mrs	x0, mpidr_el1
	and	x0, x0, #(MPIDR_CLUSTER_MASK | MPIDR_CPU_MASK)
	cmp	x0, #BM_PRIMARY_CPU
	cset	w0, eq
	ret
endfunc plat_is_my_cpu_primary

	/* -----------------------------------------------------
	 * void plat_secondary_cold_boot_setup (void);
	 *
	 * This function performs any platform specific actions
	 * needed for a secondary cpu after a cold reset e.g
	 * mark the cpu's presence, mechanism to place it in a
	 * holding pen etc.
	 * -----------------------------------------------------
	 */
func plat_secondary_cold_boot_setup
	/* Calculate address of our hold entry */
	bl	plat_my_core_pos
	mov_imm	x2, PLAT_BM_HOLD_BASE

	/* clean hold entry first, as reset won't clean SRAM */
	str	xzr, [x2, x0, lsl #3]

	/* Wait until we have a go */
poll_mailbox:
	ldr	x1, [x2, x0, lsl #3]
	cbz	x1, 1f
	mov_imm	x0, PLAT_BM_TRUSTED_MAILBOX_BASE
	ldr	x1, [x0]
	br	x1
1:
	wfe
	b	poll_mailbox
endfunc plat_secondary_cold_boot_setup

func plat_get_my_entrypoint
	/* TODO support warm boot */
	mov	x0, #0
	ret
endfunc plat_get_my_entrypoint

func platform_mem_init
	ret
endfunc platform_mem_init

	/* ---------------------------------------------
	 * int plat_crash_console_init(void)
	 * Function to initialize the crash console
	 * without a C Runtime to print crash report.
	 * Clobber list : x0, x1, x2
	 * ---------------------------------------------
	 */
func plat_crash_console_init
	bl	plat_bm_get_uart_clock
	mov x1, x0
	mov_imm	x0, PLAT_CRASH_UART_BASE
	mov_imm	x2, PLAT_BM_CONSOLE_BAUDRATE
	b	console_init
endfunc plat_crash_console_init

	/* ---------------------------------------------
	 * int plat_crash_console_putc(int c)
	 * Function to print a character on the crash
	 * console without a C Runtime.
	 * Clobber list : x1, x2
	 * ---------------------------------------------
	 */
func plat_crash_console_putc
	mov_imm	x1, PLAT_CRASH_UART_BASE
	b	console_putc
endfunc plat_crash_console_putc

func plat_off_mmu
	mrs x1, SCTLR_EL1
	bic x1, x1, SCTLR_M_BIT     /* MMU disable. */
	msr SCTLR_EL3, x1
	isb
	ret
endfunc plat_off_mmu

func plat_non_primary_suspend
	bl plat_off_mmu
	ldr x1, =ROM_BASE		/* Jump to Rom for waiting Mailbox */
	br x1
endfunc plat_non_primary_suspend
