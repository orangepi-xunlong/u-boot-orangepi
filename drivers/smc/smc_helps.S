#include <linux/linkage.h>
#include <asm/macro.h>
#include <config.h>

#define UNWIND(code...)         code

/*
* r0: fid
* r1: para1
* r2: para2
* r3: prar3
* r4: result array, will save r0-r3 after smc return
* note : in armv7 ,para0-3 pass use r0-r3,other para use sp
*/

#ifdef CONFIG_ARCH_SUN3IW1P1

ENTRY(__sunxi_smc_call)
ENDPROC(__sunxi_smc_call)

#else

ENTRY(__sunxi_smc_call)
#ifndef CONFIG_ARM64
.arch_extension sec
#endif
	stmfd	sp!, {r4-r12, lr}	@ save reg state
	smc #0
	ldmfd	sp!, {r4-r12, pc}	@ restore saved regs and return

ENDPROC(__sunxi_smc_call)


/* void tee_smc_call(struct smc_param *param); */
	.globl	tee_smc_call
ENTRY(tee_smc_call)
	push	{r4-r8, lr}
	mov	r8, r0
	ldm	r8, {r0-r7}
.arch_extension sec
	smc	#0
	stm	r8, {r0-r7}
	pop	{r4-r8, pc}
ENDPROC(tee_smc_call)



	/*
	 * Wrap c macros in asm macros to delay expansion until after the
	 * SMCCC asm macro is expanded.
	 */
	.macro SMCCC_SMC
	smc	#0
	.endm


	.macro SMCCC instr
UNWIND(	.fnstart)
	mov	r12, sp
	push	{r4-r7}
UNWIND(	.save	{r4-r7})
	ldm	r12, {r4-r7}
	\instr
	pop	{r4-r7}
	ldr	r12, [sp, #(4 * 4)]
	stm	r12, {r0-r3}
	bx	lr
UNWIND(	.fnend)
	.endm

/*
 * void smccc_smc(unsigned long a0, unsigned long a1, unsigned long a2,
 *		  unsigned long a3, unsigned long a4, unsigned long a5,
 *		  unsigned long a6, unsigned long a7, struct arm_smccc_res *res)
 */
ENTRY(arm_smccc_smc)
	SMCCC SMCCC_SMC
ENDPROC(arm_smccc_smc)



#endif
