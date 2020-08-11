#include <linux/linkage.h>
#include <asm/assembler.h>

/*
 * void *memcpy_align_16(void *dest, const void *src, size_t n);
 *
 * note: the size must be 16 bytes align.
 *
 */
 
ENTRY(memcpy_align16)
	stmfd	sp!, {r4-r10, lr}
	mov	r4,r1                   /* r4 is the source address */
	mov	r5,r0                   /* r5 is the dest address */

	mov     r10,r1
	add	r10,r10,r2              /* r10 is the end address */

copy_loop:
	ldmia	r4!, {r6-r9}		/* copy from source address [r4]    */
	stmia	r5!, {r6-r9}		/* copy to   target address [r5]    */
	cmp	r4, r10			/* until source end address [r10]    */
	blo	copy_loop

	ldmfd	sp!, {r4-r10, pc}
ENDPROC(memcpy_align16)


