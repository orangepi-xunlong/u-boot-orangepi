

#include <config.h>
#include "asm/mode.h"

.globl _start
_start: b	reset

reset:
	mrs r0, cpsr
	bic r0, r0, #ARMV7_MODE_MASK
	orr r0, r0, #ARMV7_SVC_MODE
	orr r0, r0, #( ARMV7_IRQ_MASK | ARMV7_FIQ_MASK )    @// After reset, ARM automaticly disables IRQ and FIQ, and runs in SVC mode.
	bic r0, r0, #(1<<9)                                 @// set little-endian
	msr cpsr_c, r0

	mrc	p15, 0, r0, c1, c0, 0
#ifdef CONFIG_ARCH_SUN8IW10P1
	orr	r0, r0, #0x00002000	@ set bits 13 (--V-)
#else
	bic	r0, r0, #0x00002000	@ clear bits 13 (--V-)
#endif
	bic	r0, r0, #0x00000007	@ clear bits 2:0 (-CAM)
	orr	r0, r0, #0x00000800	@ set bit 11 (Z---) BTB
	bic	r0, r0, #0x00001000	@ clear bit 12 (I) I-cache
	mcr	p15, 0, r0, c1, c0, 0

	ldr sp, =CONFIG_BOOT0_STACK_BOTTOM
	bl  clear_bss
	bl  cpu_init_s
	bl  main

clear_bss:
	ldr	r0, =__bss_start
	ldr	r1, =__bss_end

	mov	r2, #0		/* clear			    */

clbss_1:
	stmia   r0!, {r2}
	cmp r0, r1
	blt clbss_1

	mov pc, lr


