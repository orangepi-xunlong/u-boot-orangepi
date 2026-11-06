/*
 * Copyright (c) 2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PLAT_MACROS_S__
#define __PLAT_MACROS_S__

#include <arm_macros.S>
#include <platform_def.h>

	/* ---------------------------------------------
	 * The below required platform porting macro
	 * prints out relevant GIC and CCI registers
	 * whenever an unhandled exception is taken in
	 * BL31.
	 * Clobbers: x0 - x10, x16, x17, sp
	 * ---------------------------------------------
	 */
	.macro plat_crash_print_regs
	mov_imm	x17, PLAT_ARM_GICC_BASE
	mov_imm	x16, PLAT_ARM_GICD_BASE
	arm_print_gic_regs
	.endm

	.macro atf_state_set _regv, _rega, _val
		mov_imm \_regv, \_val
		mov_imm \_rega, ATF_STATE_REG
		str \_regv, [\_rega]
	.endm

#endif /* __PLAT_MACROS_S__ */
