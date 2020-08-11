#include <linux/linkage.h>
#include <asm/macro.h>
#include <config.h>

/*
* r0: fid
* r1: para1
* r2: para2
* r3: prar3
* r4: result array, will save r0-r3 after smc return
* note : in armv7 ,para0-3 pass use r0-r3,other para use sp
*/

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

