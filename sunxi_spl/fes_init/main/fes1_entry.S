

#include <config.h>

.globl _start
_start: b	reset

reset:
	stmfd sp!, {lr}
	bl  clear_bss
	bl  main
	ldmfd sp!, {lr}
#ifndef CONFIG_ARCH_SUN8IW10P1
	mov pc, lr
#else
	ldr r0, =0x40200000
	mov r1, sp
	mov r2, #0x2000

memcpy_loop:
	ldmia   r1!, {r7-r10}
	stmia   r0!, {r7-r10}
	sub     r2, r2, #16
	cmp     r2, #0
	bne     memcpy_loop

	ldr     r0, =0x40200000
	mov     sp, r0
	mov     pc, lr
#endif


clear_bss:
	ldr	r0, =__bss_start
	ldr	r1, =__bss_end

	mov	r2, #0x00000000		/* clear			    */

clbss_l:
	str	r2, [r0]		/* clear loop...		    */
	add	r0, r0, #4
	cmp	r0, r1
	bne	clbss_l

	mov pc, lr
