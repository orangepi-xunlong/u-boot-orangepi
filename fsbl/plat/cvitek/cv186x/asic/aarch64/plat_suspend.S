/*
 * Copyright (c) 2015-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <arch.h>
#include <asm_macros.S>
#include <platform_def.h>
#include <uart_16550.h>

	.globl bm_enter_suspend

	/* --------------------------------------------------------
	* int senduartc(int c)
	* Function to output a character over the console. It
	* returns the character printed on success or -1 on error.
	* In : w0 - character to be printed
	*      x1 - console base address
	* Out : return -1 on error else return character.
	* Clobber list : x2
	* --------------------------------------------------------
	*/
	.macro senduartc, char
		mov x0, #\char
		mov_imm x1, PLAT_BOOT_UART_BASE
		/* Prepend '\r' to '\n' */
		cmp w0, #0xA
		b.ne 2001f
		/* Check if the transmit FIFO is full */
	1002:
		ldr w2, [x1, #UARTLSR]
		and w2, w2, #(UARTLSR_TEMT | UARTLSR_THRE)
		cmp w2, #(UARTLSR_TEMT | UARTLSR_THRE)
		b.ne 1002b
		mov w2, #0xD                /* '\r' */
		str w2, [x1, #UARTTX]

		/* Check if the transmit FIFO is full */
	2001:   ldr w2, [x1, #UARTLSR]
		and w2, w2, #(UARTLSR_TEMT | UARTLSR_THRE)
		cmp w2, #(UARTLSR_TEMT | UARTLSR_THRE)
		b.ne 2001b
		str w0, [x1, #UARTTX]
	.endm

func bm_enter_suspend
	senduartc 'S'
	/*
	 * Backup PLAT_BM_TRUSTED_MAILBOX data to RTC SRAM
	 */
	ldr x1, =PLAT_BM_TRUSTED_MAILBOX_BASE
	ldr x0, =RTC_SRAM_TRUSTED_MAILBOX_BASE
	add x2, x1, #PLAT_BM_TRUSTED_MAILBOX_SIZE
copy_m:
	ldp w3, w4, [x1], #0x8
	stp w3, w4, [x0], #0x8
	cmp x1, x2
	blt copy_m

	senduartc 'U'
	/*
	 * TODO: Backup DDR training data to RTC SRAM
	 */

	/*
	 * copy execution code to Shared SRAM
	 */
	senduartc 'S'
	adr x1, suspend_enter
	ldr x0, =RTC_CORE_SRAM_BASE
	mov x5, x0
	adr x2, suspend_end
copy_s:
	ldp w3, w4, [x1], #0x8
	stp w3, w4, [x0], #0x8
	cmp x1, x2
	blt copy_s
	senduartc 'P'
	br x5
suspend_enter:
	senduartc 'E'
	senduartc 'N'
	senduartc 'D'
	senduartc '\n'

	isb
	dsb sy
	/* TODO: Enter DDR self refresh mode */

	isb
	dsb sy

	/* Send suspend request to RTC */
	/* enable power suspend wakeup source mask */
	mov_imm x0, REG_RTC_BASE
	mov_imm w1, 0x1F
	str w1, [x0, #RTC_EN_PWR_WAKEUP]

	/* enable suspend request */
	mov_imm x0, REG_RTC_BASE
	mov_imm w1, 0x01
	str w1, [x0, #RTC_EN_SUSPEND_REQ]

	/* set unlock key */
	mov_imm x0, REG_RTC_CTRL_BASE
	mov_imm w1, 0xAB18
	str w1, [x0, #RTC_CTRL0_UNLOCKKEY]

	/* trigger req_suspend[7] */
	mov_imm x0, REG_RTC_CTRL_BASE
	mov_imm w1, 0xFFFF0880
	str w1, [x0, #RTC_CTRL0]

	/* Jump to BL1, for FPGA test */
	mov_imm x1, ROM_BASE
	br x1
	/*ret*/
endfunc bm_enter_suspend

suspend_end:
	.word 0xff
