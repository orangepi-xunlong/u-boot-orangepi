/*
 * Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <asm_macros.S>
#include <cvsp.h>
#include <xlat_tables_defs.h>
#include <teesmc_opteed.h>
#include <teesmc_opteed_macros.h>
#include <optee_smc.h>
#include "../cvsp_private.h"


	.globl	bmsp_entrypoint
	.globl  bmsp_vector_table



	/* ---------------------------------------------
	 * Populate the params in x0-x7 from the pointer
	 * to the smc args structure in x0.
	 * ---------------------------------------------
	 */
	.macro restore_args_call_smc
	ldp	x6, x7, [x0, #BMSP_ARG6]
	ldp	x4, x5, [x0, #BMSP_ARG4]
	ldp	x2, x3, [x0, #BMSP_ARG2]
	ldp	x0, x1, [x0, #BMSP_ARG0]
	smc	#0
	.endm

	.macro	save_eret_context reg1 reg2
	mrs	\reg1, elr_el1
	mrs	\reg2, spsr_el1
	stp	\reg1, \reg2, [sp, #-0x10]!
	stp	x30, x18, [sp, #-0x10]!
	.endm

	.macro restore_eret_context reg1 reg2
	ldp	x30, x18, [sp], #0x10
	ldp	\reg1, \reg2, [sp], #0x10
	msr	elr_el1, \reg1
	msr	spsr_el1, \reg2
	.endm

	.section	.text, "ax"
	.align 3

func bmsp_entrypoint

	/* ---------------------------------------------
	 * Set the exception vector to something sane.
	 * ---------------------------------------------
	 */
	adr	x0, bmsp_exceptions
	msr	vbar_el1, x0
	isb

	/* ---------------------------------------------
	 * Enable the SError interrupt now that the
	 * exception vectors have been setup.
	 * ---------------------------------------------
	 */
	msr	daifclr, #DAIF_ABT_BIT

	/* ---------------------------------------------
	 * Enable the instruction cache, stack pointer
	 * and data access alignment checks
	 * ---------------------------------------------
	 */
	mov	x1, #(SCTLR_I_BIT | SCTLR_A_BIT | SCTLR_SA_BIT)
	mrs	x0, sctlr_el1
	orr	x0, x0, x1
	msr	sctlr_el1, x0
	isb

	/* ---------------------------------------------
	 * Invalidate the RW memory used by the BL32
	 * image. This includes the data and NOBITS
	 * sections. This is done to safeguard against
	 * possible corruption of this memory by dirty
	 * cache lines in a system cache as a result of
	 * use by an earlier boot loader stage.
	 * ---------------------------------------------
	 */
	adr	x0, __RW_START__
	adr	x1, __RW_END__
	sub	x1, x1, x0
	bl	inv_dcache_range

	/* ---------------------------------------------
	 * Zero out NOBITS sections. There are 2 of them:
	 *   - the .bss section;
	 *   - the coherent memory section.
	 * ---------------------------------------------
	 */
	ldr	x0, =__BSS_START__
	ldr	x1, =__BSS_SIZE__
	bl	zeromem

#if USE_COHERENT_MEM
	ldr	x0, =__COHERENT_RAM_START__
	ldr	x1, =__COHERENT_RAM_UNALIGNED_SIZE__
	bl	zeromem
#endif

	/* --------------------------------------------
	 * Allocate a stack whose memory will be marked
	 * as Normal-IS-WBWA when the MMU is enabled.
	 * There is no risk of reading stale stack
	 * memory after enabling the MMU as only the
	 * primary cpu is running at the moment.
	 * --------------------------------------------
	 */
	bl	plat_set_my_stack

	/* ---------------------------------------------
	 * Initialize the stack protector canary before
	 * any C code is called.
	 * ---------------------------------------------
	 */
#if STACK_PROTECTOR_ENABLED
	bl	update_stack_protector_canary
#endif

	/* ---------------------------------------------
	 * Perform early platform setup & platform
	 * specific early arch. setup e.g. mmu setup
	 * ---------------------------------------------
	 */
	bl	bmsp_early_platform_setup
	bl	bmsp_plat_arch_setup

	/* ---------------------------------------------
	 * Jump to main function.
	 * ---------------------------------------------
	 */
	bl	bmsp_main

	/* ---------------------------------------------
	 * Tell BMSPD that we are done initialising
	 * ---------------------------------------------
	 */
	mov	x1, x0
	mov	x0, #TEESMC_OPTEED_RETURN_ENTRY_DONE
	smc	#0

bmsp_entrypoint_panic:
	b	bmsp_entrypoint_panic
endfunc bmsp_entrypoint


	/* -------------------------------------------
	 * Table of entrypoint vectors provided to the
	 * BMSPD for the various entrypoints
	 * -------------------------------------------
	 */
func bmsp_vector_table
	b	bmsp_yield_smc_entry
	b	bmsp_fast_smc_entry
	b	bmsp_cpu_on_entry
	b	bmsp_cpu_off_entry
	b	bmsp_cpu_resume_entry
	b	bmsp_cpu_suspend_entry
	b	bmsp_sel1_intr_entry
	b	bmsp_system_off_entry
	b	bmsp_system_reset_entry
	b	bmsp_abort_yield_smc_entry
endfunc bmsp_vector_table

	/*---------------------------------------------
	 * This entrypoint is used by the BMSPD when this
	 * cpu is to be turned off through a CPU_OFF
	 * psci call to ask the BMSP to perform any
	 * bookeeping necessary. In the current
	 * implementation, the BMSPD expects the BMSP to
	 * re-initialise its state so nothing is done
	 * here except for acknowledging the request.
	 * ---------------------------------------------
	 */
func bmsp_cpu_off_entry
	bl	bmsp_cpu_off_main
	restore_args_call_smc
endfunc bmsp_cpu_off_entry

	/*---------------------------------------------
	 * This entrypoint is used by the BMSPD when the
	 * system is about to be switched off (through
	 * a SYSTEM_OFF psci call) to ask the BMSP to
	 * perform any necessary bookkeeping.
	 * ---------------------------------------------
	 */
func bmsp_system_off_entry
	bl	bmsp_system_off_main
	restore_args_call_smc
endfunc bmsp_system_off_entry

	/*---------------------------------------------
	 * This entrypoint is used by the BMSPD when the
	 * system is about to be reset (through a
	 * SYSTEM_RESET psci call) to ask the BMSP to
	 * perform any necessary bookkeeping.
	 * ---------------------------------------------
	 */
func bmsp_system_reset_entry
	bl	bmsp_system_reset_main
	restore_args_call_smc
endfunc bmsp_system_reset_entry

	/*---------------------------------------------
	 * This entrypoint is used by the BMSPD when this
	 * cpu is turned on using a CPU_ON psci call to
	 * ask the BMSP to initialise itself i.e. setup
	 * the mmu, stacks etc. Minimal architectural
	 * state will be initialised by the BMSPD when
	 * this function is entered i.e. Caches and MMU
	 * will be turned off, the execution state
	 * will be aarch64 and exceptions masked.
	 * ---------------------------------------------
	 */
func bmsp_cpu_on_entry
	/* ---------------------------------------------
	 * Set the exception vector to something sane.
	 * ---------------------------------------------
	 */
	adr	x0, bmsp_exceptions
	msr	vbar_el1, x0
	isb

	/* Enable the SError interrupt */
	msr	daifclr, #DAIF_ABT_BIT

	/* ---------------------------------------------
	 * Enable the instruction cache, stack pointer
	 * and data access alignment checks
	 * ---------------------------------------------
	 */
	mov	x1, #(SCTLR_I_BIT | SCTLR_A_BIT | SCTLR_SA_BIT)
	mrs	x0, sctlr_el1
	orr	x0, x0, x1
	msr	sctlr_el1, x0
	isb

	/* --------------------------------------------
	 * Give ourselves a stack whose memory will be
	 * marked as Normal-IS-WBWA when the MMU is
	 * enabled.
	 * --------------------------------------------
	 */
	bl	plat_set_my_stack

	/* --------------------------------------------
	 * Enable the MMU with the DCache disabled. It
	 * is safe to use stacks allocated in normal
	 * memory as a result. All memory accesses are
	 * marked nGnRnE when the MMU is disabled. So
	 * all the stack writes will make it to memory.
	 * All memory accesses are marked Non-cacheable
	 * when the MMU is enabled but D$ is disabled.
	 * So used stack memory is guaranteed to be
	 * visible immediately after the MMU is enabled
	 * Enabling the DCache at the same time as the
	 * MMU can lead to speculatively fetched and
	 * possibly stale stack memory being read from
	 * other caches. This can lead to coherency
	 * issues.
	 * --------------------------------------------
	 */
	mov	x0, #DISABLE_DCACHE
	bl	bl32_plat_enable_mmu

	/* ---------------------------------------------
	 * Enable the Data cache now that the MMU has
	 * been enabled. The stack has been unwound. It
	 * will be written first before being read. This
	 * will invalidate any stale cache lines resi-
	 * -dent in other caches. We assume that
	 * interconnect coherency has been enabled for
	 * this cluster by EL3 firmware.
	 * ---------------------------------------------
	 */
	mrs	x0, sctlr_el1
	orr	x0, x0, #SCTLR_C_BIT
	msr	sctlr_el1, x0
	isb

	/* ---------------------------------------------
	 * Enter C runtime to perform any remaining
	 * book keeping
	 * ---------------------------------------------
	 */
	bl	bmsp_cpu_on_main
	restore_args_call_smc

	/* Should never reach here */
bmsp_cpu_on_entry_panic:
	b	bmsp_cpu_on_entry_panic
endfunc bmsp_cpu_on_entry

	/*---------------------------------------------
	 * This entrypoint is used by the BMSPD when this
	 * cpu is to be suspended through a CPU_SUSPEND
	 * psci call to ask the BMSP to perform any
	 * bookeeping necessary. In the current
	 * implementation, the BMSPD saves and restores
	 * the EL1 state.
	 * ---------------------------------------------
	 */
func bmsp_cpu_suspend_entry
	bl	bmsp_cpu_suspend_main
	restore_args_call_smc
endfunc bmsp_cpu_suspend_entry

	/*-------------------------------------------------
	 * This entrypoint is used by the BMSPD to pass
	 * control for `synchronously` handling a S-EL1
	 * Interrupt which was triggered while executing
	 * in normal world. 'x0' contains a magic number
	 * which indicates this. BMSPD expects control to
	 * be handed back at the end of interrupt
	 * processing. This is done through an SMC.
	 * The handover agreement is:
	 *
	 * 1. PSTATE.DAIF are set upon entry. 'x1' has
	 *    the ELR_EL3 from the non-secure state.
	 * 2. BMSP has to preserve the callee saved
	 *    general purpose registers, SP_EL1/EL0 and
	 *    LR.
	 * 3. BMSP has to preserve the system and vfp
	 *    registers (if applicable).
	 * 4. BMSP can use 'x0-x18' to enable its C
	 *    runtime.
	 * 5. BMSP returns to BMSPD using an SMC with
	 *    'x0' = BMSP_HANDLED_S_EL1_INTR
	 * ------------------------------------------------
	 */
func	bmsp_sel1_intr_entry
#if DEBUG
	mov_imm	x2, BMSP_HANDLE_SEL1_INTR_AND_RETURN
	cmp	x0, x2
	b.ne	bmsp_sel1_int_entry_panic
#endif
	/*-------------------------------------------------
	 * Save any previous context needed to perform
	 * an exception return from S-EL1 e.g. context
	 * from a previous Non secure Interrupt.
	 * Update statistics and handle the S-EL1
	 * interrupt before returning to the BMSPD.
	 * IRQ/FIQs are not enabled since that will
	 * complicate the implementation. Execution
	 * will be transferred back to the normal world
	 * in any case. The handler can return 0
	 * if the interrupt was handled or BMSP_PREEMPTED
	 * if the expected interrupt was preempted
	 * by an interrupt that should be handled in EL3
	 * e.g. Group 0 interrupt in GICv3. In both
	 * the cases switch to EL3 using SMC with id
	 * BMSP_HANDLED_S_EL1_INTR. Any other return value
	 * from the handler will result in panic.
	 * ------------------------------------------------
	 */
	save_eret_context x2 x3
	bl	bmsp_update_sync_sel1_intr_stats
	bl	bmsp_common_int_handler
	/* Check if the S-EL1 interrupt has been handled */
	cbnz	x0, bmsp_sel1_intr_check_preemption
	b	bmsp_sel1_intr_return
bmsp_sel1_intr_check_preemption:
	/* Check if the S-EL1 interrupt has been preempted */
	ldr	x1, =OPTEE_SMC_RETURN_RPC_FOREIGN_INTR
	cmp	x0, x1
	b.ne	bmsp_sel1_int_entry_panic
bmsp_sel1_intr_return:
	ldr	x0, =TEESMC_OPTEED_RETURN_CALL_DONE
	restore_eret_context x2 x3
	smc	#0

	/* Should never reach here */
bmsp_sel1_int_entry_panic:
	no_ret	plat_panic_handler
endfunc bmsp_sel1_intr_entry

	/*---------------------------------------------
	 * This entrypoint is used by the BMSPD when this
	 * cpu resumes execution after an earlier
	 * CPU_SUSPEND psci call to ask the BMSP to
	 * restore its saved context. In the current
	 * implementation, the BMSPD saves and restores
	 * EL1 state so nothing is done here apart from
	 * acknowledging the request.
	 * ---------------------------------------------
	 */
func bmsp_cpu_resume_entry
	bl	bmsp_cpu_resume_main
	restore_args_call_smc

	/* Should never reach here */
	no_ret	plat_panic_handler
endfunc bmsp_cpu_resume_entry

	/*---------------------------------------------
	 * This entrypoint is used by the BMSPD to ask
	 * the BMSP to service a fast smc request.
	 * ---------------------------------------------
	 */
func bmsp_fast_smc_entry
	bl	bmsp_smc_handler
	restore_args_call_smc

	/* Should never reach here */
	no_ret	plat_panic_handler
endfunc bmsp_fast_smc_entry

	/*---------------------------------------------
	 * This entrypoint is used by the BMSPD to ask
	 * the BMSP to service a Yielding SMC request.
	 * We will enable preemption during execution
	 * of bmsp_smc_handler.
	 * ---------------------------------------------
	 */
func bmsp_yield_smc_entry
	# Disable IRQ(non-secure interrupt)
	msr	daifclr, #DAIF_FIQ_BIT
	bl	bmsp_smc_handler
	msr	daifset, #DAIF_FIQ_BIT | DAIF_IRQ_BIT
	restore_args_call_smc

	/* Should never reach here */
	no_ret	plat_panic_handler
endfunc bmsp_yield_smc_entry

	/*---------------------------------------------------------------------
	 * This entrypoint is used by the BMSPD to abort a pre-empted Yielding
	 * SMC. It could be on behalf of non-secure world or because a CPU
	 * suspend/CPU off request needs to abort the preempted SMC.
	 * --------------------------------------------------------------------
	 */
func bmsp_abort_yield_smc_entry

	/*
	 * Exceptions masking is already done by the BMSPD when entering this
	 * hook so there is no need to do it here.
	 */

	/* Reset the stack used by the pre-empted SMC */
	bl	plat_set_my_stack

	/*
	 * Allow some cleanup such as releasing locks.
	 */
	bl	bmsp_abort_smc_handler

	restore_args_call_smc

	/* Should never reach here */
	bl	plat_panic_handler
endfunc bmsp_abort_yield_smc_entry
