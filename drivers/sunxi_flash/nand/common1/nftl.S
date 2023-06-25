/*
 * Copyright 2000-2019
 * SPDX-License-Identifier: GPL-2.0
 */
	.arch armv7-a
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 4
	.eabi_attribute 34, 0
	.eabi_attribute 18, 2
	.file	"nftl.c"
	.section	.text._nand_sector_read,"ax",%progbits
	.align	1
	.global	_nand_sector_read
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nand_sector_read, %function
_nand_sector_read:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #0
	bx	lr
	.size	_nand_sector_read, .-_nand_sector_read
	.section	.text._nand_flush_sector_write_cache,"ax",%progbits
	.align	1
	.global	_nand_flush_sector_write_cache
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nand_flush_sector_write_cache, %function
_nand_flush_sector_write_cache:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #0
	bx	lr
	.size	_nand_flush_sector_write_cache, .-_nand_flush_sector_write_cache
	.section	.text._shutdown_op,"ax",%progbits
	.align	1
	.global	_shutdown_op
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_shutdown_op, %function
_shutdown_op:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #0
	bx	lr
	.size	_shutdown_op, .-_shutdown_op
	.section	.text._nftl_nand_erase_superblk,"ax",%progbits
	.align	1
	.global	_nftl_nand_erase_superblk
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nftl_nand_erase_superblk, %function
_nftl_nand_erase_superblk:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r3, r4, lr}
	ldr	r3, [r0]
	ldr	r0, [r3, #4]
	ldrh	r3, [r1, #2]
	strh	r3, [sp, #2]	@ movhi
	ldrh	r3, [r1]
	strh	r3, [sp]	@ movhi
	ldrh	r3, [r1, #4]
	strh	r3, [sp, #4]	@ movhi
	ldr	r3, [r1, #8]
	str	r3, [sp, #8]
	ldr	r3, [r1, #12]
	mov	r1, sp
	str	r3, [sp, #12]
	ldr	r3, [r0, #84]
	blx	r3
	add	sp, sp, #20
	@ sp needed
	ldr	pc, [sp], #4
	.size	_nftl_nand_erase_superblk, .-_nftl_nand_erase_superblk
	.section	.text._nftl_nand_is_blk_good,"ax",%progbits
	.align	1
	.global	_nftl_nand_is_blk_good
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nftl_nand_is_blk_good, %function
_nftl_nand_is_blk_good:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r3, r4, lr}
	ldr	r3, [r0]
	ldr	r0, [r3, #4]
	ldrh	r3, [r1, #2]
	strh	r3, [sp, #2]	@ movhi
	ldrh	r3, [r1]
	strh	r3, [sp]	@ movhi
	ldrh	r3, [r1, #4]
	strh	r3, [sp, #4]	@ movhi
	ldr	r3, [r1, #8]
	str	r3, [sp, #8]
	ldr	r3, [r1, #12]
	mov	r1, sp
	str	r3, [sp, #12]
	ldr	r3, [r0, #96]
	blx	r3
	add	sp, sp, #20
	@ sp needed
	ldr	pc, [sp], #4
	.size	_nftl_nand_is_blk_good, .-_nftl_nand_is_blk_good
	.section	.text._nftl_nand_mark_bad_blk,"ax",%progbits
	.align	1
	.global	_nftl_nand_mark_bad_blk
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nftl_nand_mark_bad_blk, %function
_nftl_nand_mark_bad_blk:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r3, r4, lr}
	ldr	r3, [r0]
	ldr	r0, [r3, #4]
	ldrh	r3, [r1, #2]
	strh	r3, [sp, #2]	@ movhi
	ldrh	r3, [r1]
	strh	r3, [sp]	@ movhi
	ldrh	r3, [r1, #4]
	strh	r3, [sp, #4]	@ movhi
	ldr	r3, [r1, #8]
	str	r3, [sp, #8]
	ldr	r3, [r1, #12]
	mov	r1, sp
	str	r3, [sp, #12]
	ldr	r3, [r0, #100]
	blx	r3
	add	sp, sp, #20
	@ sp needed
	ldr	pc, [sp], #4
	.size	_nftl_nand_mark_bad_blk, .-_nftl_nand_mark_bad_blk
	.section	.text.dumphex,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	dumphex, %function
dumphex:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	mov	r6, r1
	ldr	r7, .L10
	sub	sp, sp, #56
	mov	r4, r0
	movs	r5, #0
	ldr	r8, .L10+8
.L8:
	cmp	r5, r6
	add	r4, r4, #16
	bcc	.L9
	ldr	r0, .L10+4
	add	sp, sp, #56
	@ sp needed
	pop	{r4, r5, r6, r7, r8, lr}
	b	NAND_Print
.L9:
	mov	r1, r5
	mov	r0, r7
	bl	NAND_Print
	ldrb	r0, [r4, #-1]	@ zero_extendqisi2
	adds	r5, r5, #16
	ldrb	r3, [r4, #-14]	@ zero_extendqisi2
	ldrb	r2, [r4, #-15]	@ zero_extendqisi2
	ldrb	r1, [r4, #-16]	@ zero_extendqisi2
	str	r0, [sp, #48]
	ldrb	r0, [r4, #-2]	@ zero_extendqisi2
	str	r0, [sp, #44]
	ldrb	r0, [r4, #-3]	@ zero_extendqisi2
	str	r0, [sp, #40]
	ldrb	r0, [r4, #-4]	@ zero_extendqisi2
	str	r0, [sp, #36]
	ldrb	r0, [r4, #-5]	@ zero_extendqisi2
	str	r0, [sp, #32]
	ldrb	r0, [r4, #-6]	@ zero_extendqisi2
	str	r0, [sp, #28]
	ldrb	r0, [r4, #-7]	@ zero_extendqisi2
	str	r0, [sp, #24]
	ldrb	r0, [r4, #-8]	@ zero_extendqisi2
	str	r0, [sp, #20]
	ldrb	r0, [r4, #-9]	@ zero_extendqisi2
	str	r0, [sp, #16]
	ldrb	r0, [r4, #-10]	@ zero_extendqisi2
	str	r0, [sp, #12]
	ldrb	r0, [r4, #-11]	@ zero_extendqisi2
	str	r0, [sp, #8]
	ldrb	r0, [r4, #-12]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [r4, #-13]	@ zero_extendqisi2
	str	r0, [sp]
	mov	r0, r8
	bl	NAND_Print
	b	.L8
.L11:
	.align	2
.L10:
	.word	.LC5
	.word	.LC7
	.word	.LC6
	.size	dumphex, .-dumphex
	.section	.text._nftl_nand_read_page,"ax",%progbits
	.align	1
	.global	_nftl_nand_read_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nftl_nand_read_page, %function
_nftl_nand_read_page:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, lr}
	mov	r4, r0
	ldr	r2, [r4, #1968]
	sub	sp, sp, #20
	ldr	r3, [r0]
	ldr	r0, [r3, #4]
	ldrd	r3, r5, [r2, #80]
	adds	r3, r3, #1
	adc	r5, r5, #0
	strd	r3, r5, [r2, #80]
	ldrh	r3, [r1, #2]
	strh	r3, [sp, #2]	@ movhi
	ldrh	r3, [r1]
	strh	r3, [sp]	@ movhi
	ldrh	r3, [r1, #4]
	strh	r3, [sp, #4]	@ movhi
	ldr	r3, [r1, #8]
	str	r3, [sp, #8]
	ldr	r3, [r1, #12]
	mov	r1, sp
	str	r3, [sp, #12]
	ldr	r3, [r0, #88]
	blx	r3
	adds	r3, r0, #2
	itttt	eq
	ldreq	r2, [r4, #1968]
	ldreq	r3, [r2, #132]
	addeq	r3, r3, #1
	streq	r3, [r2, #132]
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, pc}
	.size	_nftl_nand_read_page, .-_nftl_nand_read_page
	.section	.text._nftl_nand_write_page,"ax",%progbits
	.align	1
	.global	_nftl_nand_write_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nftl_nand_write_page, %function
_nftl_nand_write_page:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, lr}
	mov	r4, r0
	ldr	r3, [r0]
	sub	sp, sp, #20
	ldr	r2, [r4, #1968]
	ldr	r0, [r3, #4]
	ldrd	r3, r5, [r2, #88]
	adds	r3, r3, #1
	adc	r5, r5, #0
	strd	r3, r5, [r2, #88]
	ldrh	r3, [r1, #2]
	strh	r3, [sp, #2]	@ movhi
	ldrh	r3, [r1]
	strh	r3, [sp]	@ movhi
	ldrh	r3, [r1, #4]
	strh	r3, [sp, #4]	@ movhi
	ldr	r3, [r1, #8]
	str	r3, [sp, #8]
	ldr	r3, [r1, #12]
	mov	r1, sp
	str	r3, [sp, #12]
	ldr	r3, [r0, #92]
	blx	r3
	cbz	r0, .L14
	ldr	r2, [r4, #1968]
	ldr	r3, [r2, #136]
	adds	r3, r3, #1
	str	r3, [r2, #136]
.L14:
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, pc}
	.size	_nftl_nand_write_page, .-_nftl_nand_write_page
	.section	.text._nand_sector_write,"ax",%progbits
	.align	1
	.global	_nand_sector_write
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nand_sector_write, %function
_nand_sector_write:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #0
	bx	lr
	.size	_nand_sector_write, .-_nand_sector_write
	.section	.text.print_all_nftl_blk,"ax",%progbits
	.align	1
	.global	print_all_nftl_blk
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	print_all_nftl_blk, %function
print_all_nftl_blk:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	ldr	r4, .L23
	ldr	r5, .L23+4
.L21:
	ldr	r3, [r4, #8]
	cbnz	r3, .L22
	pop	{r3, r4, r5, pc}
.L22:
	ldr	r1, [r3]
	mov	r0, r5
	bl	NAND_Print_DBG
	ldr	r4, [r4, #8]
	b	.L21
.L24:
	.align	2
.L23:
	.word	.LANCHOR0
	.word	.LC8
	.size	print_all_nftl_blk, .-print_all_nftl_blk
	.section	.text.get_nftl_num,"ax",%progbits
	.align	1
	.global	get_nftl_num
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_nftl_num, %function
get_nftl_num:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L28
	movs	r0, #0
.L26:
	ldr	r3, [r3, #8]
	cbnz	r3, .L27
	bx	lr
.L27:
	adds	r0, r0, #1
	b	.L26
.L29:
	.align	2
.L28:
	.word	.LANCHOR0
	.size	get_nftl_num, .-get_nftl_num
	.section	.text.get_nftl_cap,"ax",%progbits
	.align	1
	.global	get_nftl_cap
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_nftl_cap, %function
get_nftl_cap:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L33
	movs	r0, #0
.L31:
	ldr	r3, [r3, #8]
	cbnz	r3, .L32
	bx	lr
.L32:
	ldr	r2, [r3]
	add	r0, r0, r2
	b	.L31
.L34:
	.align	2
.L33:
	.word	.LANCHOR0
	.size	get_nftl_cap, .-get_nftl_cap
	.section	.text.get_first_nftl_cap,"ax",%progbits
	.align	1
	.global	get_first_nftl_cap
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_first_nftl_cap, %function
get_first_nftl_cap:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L36
	ldr	r3, [r3, #8]
	ldr	r0, [r3]
	bx	lr
.L37:
	.align	2
.L36:
	.word	.LANCHOR0
	.size	get_first_nftl_cap, .-get_first_nftl_cap
	.section	.text.nftl_read,"ax",%progbits
	.align	1
	.global	nftl_read
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_read, %function
nftl_read:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	mov	r3, r2
	push	{r4, r5, r6, lr}
	mov	r2, r1
	cbnz	r1, .L47
.L39:
	mov	r0, r2
	pop	{r4, r5, r6, pc}
.L42:
	ldr	r4, [r4, #8]
	subs	r0, r0, r1
	cbnz	r4, .L41
	mov	r1, r0
	ldr	r0, .L48
	bl	NAND_Print
	movs	r2, #1
	b	.L39
.L47:
	ldr	r1, .L48+4
	ldr	r4, [r1, #8]
.L41:
	ldr	r1, [r4]
	cmp	r1, r0
	bls	.L42
	ldr	r5, [r4, #44]
	mov	r1, r0
	mov	r0, r4
	mov	ip, r5
	pop	{r4, r5, r6, lr}
	bx	ip
.L49:
	.align	2
.L48:
	.word	.LC9
	.word	.LANCHOR0
	.size	nftl_read, .-nftl_read
	.section	.text.nftl_read_history,"ax",%progbits
	.align	1
	.global	nftl_read_history
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_read_history, %function
nftl_read_history:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	mov	r6, r0
	mov	r8, r2
	mov	r5, r1
	cbz	r1, .L51
	ldr	r3, .L58
	ldr	r0, .L58+4
	ldr	r4, [r3, #8]
	ldr	r1, [r4]
	bl	NAND_Print
.L52:
	ldr	r1, [r4]
	cmp	r1, r6
	bls	.L53
	ldr	r7, [r4, #40]
	mov	r3, r8
	mov	r2, r5
	mov	r1, r6
	mov	r0, r4
	mov	ip, r7
	pop	{r4, r5, r6, r7, r8, lr}
	bx	ip
.L53:
	ldr	r4, [r4, #8]
	subs	r6, r6, r1
	cmp	r4, #0
	bne	.L52
	mov	r2, r5
	movs	r5, #1
	mov	r1, r6
	ldr	r0, .L58+8
	bl	NAND_Print
.L51:
	mov	r0, r5
	pop	{r4, r5, r6, r7, r8, pc}
.L59:
	.align	2
.L58:
	.word	.LANCHOR0
	.word	.LC10
	.word	.LC9
	.size	nftl_read_history, .-nftl_read_history
	.section	.text.nftl_write,"ax",%progbits
	.align	1
	.global	nftl_write
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_write, %function
nftl_write:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	mov	r3, r2
	push	{r4, r5, r6, lr}
	mov	r2, r1
	cbnz	r1, .L69
.L61:
	mov	r0, r2
	pop	{r4, r5, r6, pc}
.L64:
	ldr	r4, [r4, #8]
	subs	r0, r0, r1
	cbnz	r4, .L63
	mov	r1, r0
	ldr	r0, .L70
	bl	NAND_Print
	movs	r2, #1
	b	.L61
.L69:
	ldr	r1, .L70+4
	ldr	r4, [r1, #8]
.L63:
	ldr	r1, [r4]
	cmp	r1, r0
	bls	.L64
	ldr	r5, [r4, #48]
	mov	r1, r0
	mov	r0, r4
	mov	ip, r5
	pop	{r4, r5, r6, lr}
	bx	ip
.L71:
	.align	2
.L70:
	.word	.LC9
	.word	.LANCHOR0
	.size	nftl_write, .-nftl_write
	.section	.text.nftl_discard,"ax",%progbits
	.align	1
	.global	nftl_discard
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_discard, %function
nftl_discard:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r2, r1
	cbnz	r1, .L81
.L73:
	mov	r0, r2
	pop	{r4, pc}
.L76:
	ldr	r3, [r3, #8]
	subs	r0, r0, r1
	cbnz	r3, .L75
	mov	r1, r0
	ldr	r0, .L82
	bl	NAND_Print
	movs	r2, #1
	b	.L73
.L81:
	ldr	r3, .L82+4
	ldr	r3, [r3, #8]
.L75:
	ldr	r1, [r3]
	cmp	r1, r0
	bls	.L76
	ldr	r4, [r3, #56]
	mov	r1, r0
	mov	r0, r3
	mov	r3, r4
	pop	{r4, lr}
	bx	r3
.L83:
	.align	2
.L82:
	.word	.LC9
	.word	.LANCHOR0
	.size	nftl_discard, .-nftl_discard
	.section	.text.nftl_flush_write_cache,"ax",%progbits
	.align	1
	.global	nftl_flush_write_cache
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_flush_write_cache, %function
nftl_flush_write_cache:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L87
	push	{r4, lr}
	ldr	r4, [r3, #8]
.L85:
	cbnz	r4, .L86
	mov	r0, r4
	pop	{r4, pc}
.L86:
	ldr	r3, [r4, #52]
	mov	r0, r4
	movw	r1, #65535
	blx	r3
	ldr	r4, [r4, #8]
	b	.L85
.L88:
	.align	2
.L87:
	.word	.LANCHOR0
	.size	nftl_flush_write_cache, .-nftl_flush_write_cache
	.section	.text.nftl_malloc,"ax",%progbits
	.align	1
	.global	nftl_malloc
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_malloc, %function
nftl_malloc:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	cmp	r0, #1572864
	push	{r4, lr}
	mov	r4, r0
	bls	.L90
	mov	r1, r0
	ldr	r0, .L91
	bl	NAND_Print
.L90:
	mov	r0, r4
	pop	{r4, lr}
	b	NAND_Malloc
.L92:
	.align	2
.L91:
	.word	.LC11
	.size	nftl_malloc, .-nftl_malloc
	.section	.text.nftl_free,"ax",%progbits
	.align	1
	.global	nftl_free
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_free, %function
nftl_free:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r1, #0
	b	NAND_Free
	.size	nftl_free, .-nftl_free
	.section	.text.add_nftl_blk_list,"ax",%progbits
	.align	1
	.global	add_nftl_blk_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	add_nftl_blk_list, %function
add_nftl_blk_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r3, #0
	str	r3, [r1, #8]
.L95:
	ldr	r3, [r0, #8]
	cbnz	r3, .L96
	str	r1, [r0, #8]
	bx	lr
.L96:
	mov	r0, r3
	b	.L95
	.size	add_nftl_blk_list, .-add_nftl_blk_list
	.section	.text.del_last_nftl_blk,"ax",%progbits
	.align	1
	.global	del_last_nftl_blk
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	del_last_nftl_blk, %function
del_last_nftl_blk:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
.L98:
	ldr	r3, [r0, #8]
	cbz	r3, .L97
	ldr	r2, [r3, #8]
	cbnz	r2, .L101
	str	r2, [r0, #8]
.L97:
	mov	r0, r3
	bx	lr
.L101:
	mov	r0, r3
	b	.L98
	.size	del_last_nftl_blk, .-del_last_nftl_blk
	.section	.text.nftl_support_boot_check_crc_from_dts,"ax",%progbits
	.align	1
	.global	nftl_support_boot_check_crc_from_dts
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_support_boot_check_crc_from_dts, %function
nftl_support_boot_check_crc_from_dts:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	bl	nand_get_support_boot_check_crc
	adds	r0, r0, #0
	it	ne
	movne	r0, #1
	pop	{r3, pc}
	.size	nftl_support_boot_check_crc_from_dts, .-nftl_support_boot_check_crc_from_dts
	.section	.text._nftl_nand_read_history_page,"ax",%progbits
	.align	1
	.global	_nftl_nand_read_history_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nftl_nand_read_history_page, %function
_nftl_nand_read_history_page:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, lr}
	mov	r4, r0
	ldr	r2, [r4, #1968]
	sub	sp, sp, #20
	ldr	r3, [r0]
	ldr	r0, [r3, #4]
	ldrd	r3, r5, [r2, #80]
	adds	r3, r3, #1
	adc	r5, r5, #0
	strd	r3, r5, [r2, #80]
	ldrh	r3, [r1, #2]
	strh	r3, [sp, #2]	@ movhi
	ldrh	r3, [r1]
	strh	r3, [sp]	@ movhi
	ldrh	r3, [r1, #4]
	strh	r3, [sp, #4]	@ movhi
	ldr	r3, [r1, #8]
	str	r3, [sp, #8]
	ldr	r3, [r1, #12]
	mov	r1, sp
	str	r3, [sp, #12]
	ldr	r3, [r0, #88]
	blx	r3
	adds	r3, r0, #2
	itttt	eq
	ldreq	r2, [r4, #1968]
	ldreq	r3, [r2, #132]
	addeq	r3, r3, #1
	streq	r3, [r2, #132]
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, pc}
	.size	_nftl_nand_read_history_page, .-_nftl_nand_read_history_page
	.section	.text.sandisk_A19_dsp_on,"ax",%progbits
	.align	1
	.global	sandisk_A19_dsp_on
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_A19_dsp_on, %function
sandisk_A19_dsp_on:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L106
	push	{r4, lr}
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r4, r0
	bl	nand_enable_chip
	ldr	r0, [r4, #268]
	bl	ndfc_disable_randomize
	movs	r2, #0
	movs	r1, #38
	mov	r0, r4
	bl	set_one_cmd
	mov	r0, r4
	bl	nand_disable_chip
	movs	r0, #0
	pop	{r4, pc}
.L107:
	.align	2
.L106:
	.word	g_nsi
	.size	sandisk_A19_dsp_on, .-sandisk_A19_dsp_on
	.section	.text.sandisk_A19_cmd25,"ax",%progbits
	.align	1
	.global	sandisk_A19_cmd25
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_A19_cmd25, %function
sandisk_A19_cmd25:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L109
	push	{r4, lr}
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r4, r0
	bl	nand_enable_chip
	ldr	r0, [r4, #268]
	bl	ndfc_disable_randomize
	movs	r2, #0
	movs	r1, #37
	mov	r0, r4
	bl	set_one_cmd
	mov	r0, r4
	bl	nand_disable_chip
	movs	r0, #0
	pop	{r4, pc}
.L110:
	.align	2
.L109:
	.word	g_nsi
	.size	sandisk_A19_cmd25, .-sandisk_A19_cmd25
	.section	.text.sandisk_A19_set_lmflgfix_next,"ax",%progbits
	.align	1
	.global	sandisk_A19_set_lmflgfix_next
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_A19_set_lmflgfix_next, %function
sandisk_A19_set_lmflgfix_next:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L116
	push	{r4, r5, r6, r7, r8, lr}
	mov	r5, r1
	sub	sp, sp, #24
	ldr	r1, [r0]
	add	r7, sp, #22
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r4, r0
	bl	nand_enable_chip
	ldr	r0, [r4, #268]
	bl	ndfc_disable_randomize
	movs	r2, #0
	movs	r1, #92
	mov	r0, r4
	bl	set_one_cmd
	movs	r2, #0
	movs	r1, #197
	mov	r0, r4
	bl	set_one_cmd
	movs	r3, #0
	movs	r2, #1
	strb	r3, [sp, #21]
	movs	r3, #85
	strb	r3, [sp, #23]
	add	r1, sp, #23
	add	r3, sp, #21
	strd	r2, r2, [sp, #4]
	str	r7, [sp]
	mov	r0, r4
	strb	r2, [sp, #22]
	bl	set_cmd_with_nand_bus
	ldr	r3, .L116+4
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #52
	bne	.L112
	movs	r3, #34
.L115:
	strb	r5, [sp, #22]
	movs	r5, #1
	strb	r3, [sp, #21]
	mov	r8, #85
	add	r3, sp, #21
	mov	r2, r5
	add	r1, sp, #23
	strd	r5, r5, [sp, #4]
	str	r7, [sp]
	mov	r0, r4
	movs	r6, #0
	strb	r8, [sp, #23]
	bl	set_cmd_with_nand_bus
	add	r3, sp, #21
	mov	r2, r5
	add	r1, sp, #23
	strd	r5, r5, [sp, #4]
	str	r7, [sp]
	mov	r0, r4
	strb	r6, [sp, #21]
	strb	r6, [sp, #22]
	strb	r8, [sp, #23]
	bl	set_cmd_with_nand_bus
	mov	r0, r4
	bl	nand_disable_chip
	mov	r0, r6
	add	sp, sp, #24
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L112:
	cmp	r3, #53
	ite	eq
	moveq	r3, #37
	movne	r3, #35
	b	.L115
.L117:
	.align	2
.L116:
	.word	g_nsi
	.word	.LANCHOR1
	.size	sandisk_A19_set_lmflgfix_next, .-sandisk_A19_set_lmflgfix_next
	.section	.text.sandisk_A19_readretry_init,"ax",%progbits
	.align	1
	.global	sandisk_A19_readretry_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_A19_readretry_init, %function
sandisk_A19_readretry_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #272]
	ldr	r2, .L121
	push	{r4, lr}
	movs	r4, #0
	ldr	r3, [r3, #52]
	str	r4, [r0, #176]
	ubfx	r1, r3, #16, #8
	ldr	r0, .L121+4
	strb	r1, [r2]
	lsrs	r2, r3, #8
	strb	r2, [r0]
	ldr	r0, .L121+8
	strb	r3, [r0]
	sub	r3, r1, #50
	cmp	r3, #3
	bls	.L119
	ldr	r0, .L121+12
	bl	NAND_Print
	mvn	r0, #98
.L118:
	pop	{r4, pc}
.L119:
	uxtb	r1, r2
	ldr	r0, .L121+16
	bl	NAND_Print_DBG
	mov	r0, r4
	b	.L118
.L122:
	.align	2
.L121:
	.word	.LANCHOR1
	.word	.LANCHOR2
	.word	.LANCHOR3
	.word	.LC12
	.word	.LC13
	.size	sandisk_A19_readretry_init, .-sandisk_A19_readretry_init
	.section	.text.sandisk_A19_readretry_exit,"ax",%progbits
	.align	1
	.global	sandisk_A19_readretry_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_A19_readretry_exit, %function
sandisk_A19_readretry_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #0
	bx	lr
	.size	sandisk_A19_readretry_exit, .-sandisk_A19_readretry_exit
	.section	.text.sandisk_A19_read_retry_clock_save,"ax",%progbits
	.align	1
	.global	sandisk_A19_read_retry_clock_save
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_A19_read_retry_clock_save, %function
sandisk_A19_read_retry_clock_save:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	ldr	r3, .L125
	ldr	r0, [r0, #268]
	ldr	r3, [r3]
	ldr	r2, .L125+4
	ldr	r1, .L125+8
	ldr	r0, [r0, #8]
	blx	r3
	ldr	r0, [r4, #268]
	movs	r2, #20
	ldr	r3, .L125+12
	movs	r1, #10
	ldr	r0, [r0, #8]
	ldr	r3, [r3]
	blx	r3
	movs	r0, #0
	pop	{r4, pc}
.L126:
	.align	2
.L125:
	.word	NAND_GetClk
	.word	.LANCHOR4
	.word	.LANCHOR5
	.word	NAND_SetClk
	.size	sandisk_A19_read_retry_clock_save, .-sandisk_A19_read_retry_clock_save
	.section	.text.sandisk_A19_read_retry_clock_recover,"ax",%progbits
	.align	1
	.global	sandisk_A19_read_retry_clock_recover
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_A19_read_retry_clock_recover, %function
sandisk_A19_read_retry_clock_recover:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	ldr	r0, [r0, #268]
	ldr	r3, .L128
	ldr	r2, .L128+4
	ldr	r1, .L128+8
	ldr	r3, [r3]
	ldr	r2, [r2]
	ldr	r1, [r1]
	ldr	r0, [r0, #8]
	blx	r3
	movs	r0, #0
	pop	{r3, pc}
.L129:
	.align	2
.L128:
	.word	NAND_SetClk
	.word	.LANCHOR4
	.word	.LANCHOR5
	.size	sandisk_A19_read_retry_clock_recover, .-sandisk_A19_read_retry_clock_recover
	.section	.text.sandisk_A19_vender_set_param,"ax",%progbits
	.align	1
	.global	sandisk_A19_vender_set_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_A19_vender_set_param, %function
sandisk_A19_vender_set_param:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r3, r4, lr}
	movs	r4, #1
	movs	r3, #4
	str	r1, [sp]
	strd	r3, r4, [sp, #4]
	mov	r3, r2
	ldr	r1, .L131
	mov	r2, r4
	bl	set_cmd_with_nand_bus
	add	sp, sp, #16
	@ sp needed
	pop	{r4, pc}
.L132:
	.align	2
.L131:
	.word	.LANCHOR6
	.size	sandisk_A19_vender_set_param, .-sandisk_A19_vender_set_param
	.section	.text.sandisk_A19_set_readretry,"ax",%progbits
	.align	1
	.global	sandisk_A19_set_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_A19_set_readretry, %function
sandisk_A19_set_readretry:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r4, r0
	bl	nand_enable_chip
	ldr	r0, [r4, #268]
	bl	ndfc_disable_randomize
	ldr	r3, .L138
	ldr	r1, [r4, #176]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #52
	bne	.L134
	ldr	r3, .L138+4
	add	r1, r3, r1, lsl #2
.L135:
	movs	r3, #1
	ldr	r2, .L138+8
	mov	r0, r4
	bl	sandisk_A19_vender_set_param
	movs	r2, #0
	mov	r5, r0
	movs	r1, #93
	mov	r0, r4
	bl	set_one_cmd
	mov	r0, r4
	bl	nand_disable_chip
	mov	r0, r5
	pop	{r3, r4, r5, pc}
.L134:
	cmp	r3, #53
	lsl	r1, r1, #2
	ite	eq
	ldreq	r3, .L138+12
	ldrne	r3, .L138+16
	add	r1, r1, r3
	b	.L135
.L139:
	.align	2
.L138:
	.word	.LANCHOR1
	.word	.LANCHOR7
	.word	.LANCHOR10
	.word	.LANCHOR8
	.word	.LANCHOR9
	.size	sandisk_A19_set_readretry, .-sandisk_A19_set_readretry
	.section	.text.sandisk_A19_exit_readretry,"ax",%progbits
	.align	1
	.global	sandisk_A19_exit_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_A19_exit_readretry, %function
sandisk_A19_exit_readretry:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r4, r5, lr}
	movs	r3, #0
	mov	r4, r0
	add	r5, sp, #8
	str	r3, [r5, #-4]!
	bl	nand_enable_chip
	ldr	r0, [r4, #268]
	bl	ndfc_disable_randomize
	movs	r2, #1
	movs	r1, #255
	mov	r0, r4
	bl	set_one_cmd
	mov	r1, r5
	movs	r3, #1
	ldr	r2, .L141
	mov	r0, r4
	bl	sandisk_A19_vender_set_param
	mov	r5, r0
	mov	r0, r4
	bl	nand_disable_chip
	mov	r0, r5
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, pc}
.L142:
	.align	2
.L141:
	.word	.LANCHOR10
	.size	sandisk_A19_exit_readretry, .-sandisk_A19_exit_readretry
	.section	.text.sandisk_A19_special_init,"ax",%progbits
	.align	1
	.global	sandisk_A19_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_A19_special_init, %function
sandisk_A19_special_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r4, #0
	ldr	r3, .L148
	ldr	r3, [r3]
	ldr	r5, [r3, #8]
.L144:
	cbnz	r5, .L145
	ldr	r3, .L148+4
	ldrb	r1, [r3]	@ zero_extendqisi2
	ldr	r3, .L148+8
	ldrb	r2, [r3]	@ zero_extendqisi2
	ldr	r3, .L148+12
	ldrb	r3, [r3]	@ zero_extendqisi2
	cbnz	r4, .L146
	ldr	r0, .L148+16
	ldr	r5, .L148+20
	str	r5, [r0]
	ldr	r0, .L148+24
	bl	NAND_Print_DBG
.L143:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L145:
	mov	r0, r5
	bl	sandisk_A19_readretry_init
	ldr	r5, [r5]
	orrs	r4, r4, r0
	b	.L144
.L146:
	ldr	r0, .L148+28
	bl	NAND_Print
	b	.L143
.L149:
	.align	2
.L148:
	.word	g_nsi
	.word	.LANCHOR1
	.word	.LANCHOR2
	.word	.LANCHOR3
	.word	df_read_page_end
	.word	sandisk_A19_read_page_end
	.word	.LC14
	.word	.LC15
	.size	sandisk_A19_special_init, .-sandisk_A19_special_init
	.section	.text.sandisk_A19_special_exit,"ax",%progbits
	.align	1
	.global	sandisk_A19_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_A19_special_exit, %function
sandisk_A19_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L161
	push	{r4, r5, r6, lr}
	movs	r6, #0
	ldr	r3, [r3]
	ldr	r4, [r3, #8]
.L151:
	cbnz	r4, .L154
	ldr	r0, .L161+4
	mov	r5, r4
	bl	NAND_Print_DBG
	b	.L150
.L154:
	ldrb	r3, [r4, #262]	@ zero_extendqisi2
	cbz	r3, .L152
	str	r6, [r4, #196]
	mov	r0, r4
	bl	sandisk_toggle_vendor_specific_setting
	mov	r5, r0
	cbz	r0, .L152
	ldr	r1, .L161+8
	ldr	r0, .L161+12
	bl	NAND_Print
.L150:
	mov	r0, r5
	pop	{r4, r5, r6, pc}
.L152:
	ldr	r4, [r4]
	b	.L151
.L162:
	.align	2
.L161:
	.word	g_nsi
	.word	.LC17
	.word	.LANCHOR11
	.word	.LC16
	.size	sandisk_A19_special_exit, .-sandisk_A19_special_exit
	.section	.text.sandisk_A19_check_bad_block_first_burn,"ax",%progbits
	.align	1
	.global	sandisk_A19_check_bad_block_first_burn
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_A19_check_bad_block_first_burn, %function
sandisk_A19_check_bad_block_first_burn:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L167
	push	{r0, r1, r2, r4, r5, r6, r7, r8, r10, lr}
	mov	r8, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	ldr	r4, [r0, #268]
	mov	r5, r0
	ldr	r2, [r0, #20]
	ldr	r1, [r8, #4]
	ldr	r3, [r4, #12]
	add	r10, r4, #32
	ldr	r0, [r0, #24]
	cmp	r2, r3
	bcs	.L164
	cmp	r1, r0
	bcc	.L165
.L164:
	strd	r1, r0, [sp]
	ldr	r1, [r4, #8]
	mvn	r4, #9
	ldr	r0, .L167+4
	bl	NAND_Print
.L163:
	mov	r0, r4
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L165:
	mov	r0, r4
	movs	r6, #1
	bl	ndfc_disable_randomize
	mov	r0, r5
	movs	r7, #0
	bl	nand_read_chip_status_ready
	mov	r0, r5
	bl	nand_enable_chip
	mov	r0, r10
	bl	ndfc_clean_cmd_seq
	movs	r3, #162
	ldrd	r1, r2, [r8, #4]
	strd	r6, r3, [r4, #40]
	movs	r3, #128
	strd	r6, r3, [r4, #104]
	str	r7, [r4, #32]
	strd	r6, r7, [r4, #48]
	strd	r6, r7, [r4, #112]
	ldr	r0, [r5, #36]
	bl	get_row_addr
	movs	r3, #5
	mov	r2, r0
	str	r3, [r4, #136]
	add	r3, r4, #120
	str	r3, [sp]
	movs	r1, #2
	movs	r3, #3
	mov	r0, r7
	bl	fill_cmd_addr
	movs	r3, #16
	strd	r6, r6, [r4, #176]
	strd	r6, r3, [r4, #168]
	mov	r1, r10
	ldr	r0, [r5, #268]
	bl	ndfc_execute_cmd
	mov	r0, r5
	bl	nand_read_chip_status_ready
	mov	r4, r0
	mov	r0, r5
	bl	nand_disable_chip
	b	.L163
.L168:
	.align	2
.L167:
	.word	g_nsi
	.word	.LC18
	.size	sandisk_A19_check_bad_block_first_burn, .-sandisk_A19_check_bad_block_first_burn
	.section	.text.hynix20nm_vender_set_param,"ax",%progbits
	.align	1
	.global	hynix20nm_vender_set_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix20nm_vender_set_param, %function
hynix20nm_vender_set_param:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, lr}
	mov	r4, r2
	sub	sp, sp, #28
	mov	r7, r3
	mov	r6, r0
	movs	r3, #54
	add	r8, sp, #24
	add	r7, r7, r4
	mov	r10, #1
	mov	r5, r1
	strb	r3, [r8, #-1]!
	bl	nand_enable_chip
	ldr	r0, [r6, #268]
	bl	ndfc_disable_randomize
.L170:
	cmp	r4, r7
	bne	.L171
	mov	r0, r6
	bl	nand_disable_chip
	movs	r0, #0
	add	sp, sp, #28
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L171:
	mov	r3, r4
	str	r5, [sp]
	movs	r2, #0
	mov	r1, r8
	strd	r10, r10, [sp, #4]
	mov	r0, r6
	bl	set_cmd_with_nand_bus
	movs	r2, #0
	movs	r1, #22
	mov	r0, r6
	adds	r4, r4, #1
	bl	set_one_cmd
	adds	r5, r5, #1
	b	.L170
	.size	hynix20nm_vender_set_param, .-hynix20nm_vender_set_param
	.section	.text.hynix20nm_set_readretry,"ax",%progbits
	.align	1
	.global	hynix20nm_set_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix20nm_set_readretry, %function
hynix20nm_set_readretry:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L179
	push	{r0, r1, r2, r4, r5, r6, r7, lr}
	ldr	r1, [r0, #176]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r1, r3
	bhi	.L178
	ldr	r5, .L179+4
	lsls	r1, r1, #3
	movs	r2, #0
	ldrb	r3, [r5]	@ zero_extendqisi2
.L174:
	cmp	r3, r2
	bhi	.L175
	ldr	r7, .L179+8
	mov	r1, sp
	ldr	r2, .L179+12
	movs	r4, #0
	bl	hynix20nm_vender_set_param
	mov	r6, r0
	ldr	r0, .L179+16
	bl	NAND_Print_DBG
.L176:
	ldrb	r3, [r5]	@ zero_extendqisi2
	cmp	r3, r4
	bhi	.L177
	ldr	r0, .L179+20
	bl	NAND_Print_DBG
.L172:
	mov	r0, r6
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L175:
	adds	r4, r1, r2
	add	r4, r4, r0
	ldrb	r4, [r4, #48]	@ zero_extendqisi2
	strb	r4, [sp, r2]
	adds	r2, r2, #1
	b	.L174
.L177:
	ldrb	r1, [sp, r4]	@ zero_extendqisi2
	mov	r0, r7
	bl	NAND_Print_DBG
	adds	r4, r4, #1
	b	.L176
.L178:
	mvn	r6, #91
	b	.L172
.L180:
	.align	2
.L179:
	.word	.LANCHOR12
	.word	.LANCHOR13
	.word	.LC20
	.word	.LANCHOR14
	.word	.LC19
	.word	.LC7
	.size	hynix20nm_set_readretry, .-hynix20nm_set_readretry
	.section	.text.hynix20nm_vender_get_param,"ax",%progbits
	.align	1
	.global	hynix20nm_vender_get_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix20nm_vender_get_param, %function
hynix20nm_vender_get_param:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, lr}
	mov	r7, r3
	sub	sp, sp, #20
	mov	r4, r2
	mov	r6, r0
	mov	r5, r1
	add	r7, r7, r4
	mov	r10, #1
	bl	nand_enable_chip
	ldr	r0, [r6, #268]
	add	r8, sp, #16
	bl	ndfc_disable_randomize
	movs	r3, #55
	strb	r3, [r8, #-1]!
.L182:
	cmp	r4, r7
	bne	.L183
	mov	r0, r6
	bl	nand_disable_chip
	movs	r0, #0
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L183:
	mov	r3, r5
	mov	r2, r4
	str	r10, [sp]
	mov	r1, r8
	mov	r0, r6
	adds	r4, r4, #1
	bl	get_data_with_nand_bus_one_cmd
	adds	r5, r5, #1
	b	.L182
	.size	hynix20nm_vender_get_param, .-hynix20nm_vender_get_param
	.section	.text.hynix20nm_lsb_init,"ax",%progbits
	.align	1
	.global	hynix20nm_lsb_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix20nm_lsb_init, %function
hynix20nm_lsb_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, [r0, #272]
	ldr	r1, .L188
	ldr	r2, [r3, #52]
	ubfx	r3, r2, #16, #8
	strb	r3, [r1]
	lsrs	r0, r2, #8
	ldr	r1, .L188+4
	cmp	r3, #2
	strb	r0, [r1]
	ldr	r1, .L188+8
	strb	r2, [r1]
	bne	.L185
	ldr	r3, .L188+12
	movs	r2, #4
	strb	r2, [r3]
	movs	r2, #176
	ldr	r3, .L188+16
	strb	r2, [r3]
	movs	r2, #177
	strb	r2, [r3, #1]
	movs	r2, #160
	strb	r2, [r3, #2]
	movs	r2, #161
.L187:
	strb	r2, [r3, #3]
	movs	r2, #10
	ldr	r3, .L188+20
	strb	r2, [r3]
	strb	r2, [r3, #1]
	strb	r2, [r3, #2]
	strb	r2, [r3, #3]
.L186:
	movs	r0, #0
	bx	lr
.L185:
	cmp	r3, #3
	bne	.L186
	ldr	r3, .L188+12
	movs	r2, #4
	strb	r2, [r3]
	movs	r2, #160
	ldr	r3, .L188+16
	strb	r2, [r3]
	movs	r2, #161
	strb	r2, [r3, #1]
	movs	r2, #167
	strb	r2, [r3, #2]
	movs	r2, #168
	b	.L187
.L189:
	.align	2
.L188:
	.word	.LANCHOR15
	.word	.LANCHOR12
	.word	.LANCHOR13
	.word	.LANCHOR16
	.word	.LANCHOR17
	.word	.LANCHOR18
	.size	hynix20nm_lsb_init, .-hynix20nm_lsb_init
	.section	.text.hynix20nm_lsb_enable,"ax",%progbits
	.align	1
	.global	hynix20nm_lsb_enable
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix20nm_lsb_enable, %function
hynix20nm_lsb_enable:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r4, r5, r6, r7, lr}
	mov	r5, r0
	ldr	r4, .L193
	ldr	r2, .L193+4
	ldrb	r3, [r4]	@ zero_extendqisi2
	ldr	r1, .L193+8
	bl	hynix20nm_vender_get_param
	ldr	r6, .L193+12
	movs	r2, #0
	ldrb	r3, [r4]	@ zero_extendqisi2
	add	r1, sp, #4
	ldr	r4, .L193+8
.L191:
	cmp	r3, r2
	bhi	.L192
	ldr	r2, .L193+4
	mov	r0, r5
	bl	hynix20nm_vender_set_param
	movs	r0, #0
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L192:
	ldrb	r0, [r2, r4]	@ zero_extendqisi2
	ldrb	r7, [r2, r6]	@ zero_extendqisi2
	add	r0, r0, r7
	strb	r0, [r1, r2]
	adds	r2, r2, #1
	b	.L191
.L194:
	.align	2
.L193:
	.word	.LANCHOR16
	.word	.LANCHOR17
	.word	.LANCHOR19
	.word	.LANCHOR18
	.size	hynix20nm_lsb_enable, .-hynix20nm_lsb_enable
	.section	.text.hynix20nm_lsb_disable,"ax",%progbits
	.align	1
	.global	hynix20nm_lsb_disable
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix20nm_lsb_disable, %function
hynix20nm_lsb_disable:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	ldr	r3, .L196
	ldr	r2, .L196+4
	ldrb	r3, [r3]	@ zero_extendqisi2
	ldr	r1, .L196+8
	bl	hynix20nm_vender_set_param
	movs	r0, #0
	pop	{r3, pc}
.L197:
	.align	2
.L196:
	.word	.LANCHOR16
	.word	.LANCHOR17
	.word	.LANCHOR19
	.size	hynix20nm_lsb_disable, .-hynix20nm_lsb_disable
	.section	.text.hynix20nm_lsb_exit,"ax",%progbits
	.align	1
	.global	hynix20nm_lsb_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix20nm_lsb_exit, %function
hynix20nm_lsb_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	bl	nand_enable_chip
	ldr	r0, [r4, #268]
	bl	ndfc_disable_randomize
	movs	r2, #0
	mov	r0, r4
	mov	r1, r2
	bl	set_one_cmd
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	movs	r2, #1
	movs	r1, #48
	mov	r0, r4
	bl	set_one_cmd
	ldr	r0, .L199
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	nand_disable_chip
	movs	r0, #0
	pop	{r4, pc}
.L200:
	.align	2
.L199:
	.word	.LC21
	.size	hynix20nm_lsb_exit, .-hynix20nm_lsb_exit
	.section	.text.hynix20nm_vender_get_param_otp_hynix,"ax",%progbits
	.align	1
	.global	hynix20nm_vender_get_param_otp_hynix
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix20nm_vender_get_param_otp_hynix, %function
hynix20nm_vender_get_param_otp_hynix:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, lr}
	movs	r3, #0
	sub	sp, sp, #36
	mov	r4, r0
	strh	r3, [sp, #28]	@ movhi
	ldr	r3, .L219
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #2
	bne	.L202
	movw	r3, #52479
	strh	r3, [sp, #24]	@ movhi
	movw	r3, #19776
	strh	r3, [sp, #28]	@ movhi
.L203:
	mov	r0, #1024
	movs	r6, #1
	bl	nand_get_temp_buf
	mov	r5, r0
	mov	r0, r4
	bl	nand_enable_chip
	ldr	r0, [r4, #268]
	bl	ndfc_disable_randomize
	movs	r2, #1
	movs	r1, #255
	mov	r0, r4
	bl	set_one_cmd
	movs	r3, #54
	movs	r2, #0
	strb	r3, [sp, #20]
	add	r3, sp, #28
	str	r3, [sp]
	add	r1, sp, #20
	add	r3, sp, #24
	strd	r6, r6, [sp, #4]
	mov	r0, r4
	bl	set_cmd_with_nand_bus
	movs	r2, #0
	add	r3, sp, #29
	mov	r1, r2
	str	r3, [sp]
	strd	r6, r6, [sp, #4]
	add	r3, sp, #25
	mov	r0, r4
	bl	set_cmd_with_nand_bus
	movs	r2, #0
	movs	r1, #22
	mov	r0, r4
	bl	set_one_cmd
	movs	r2, #0
	movs	r1, #23
	mov	r0, r4
	bl	set_one_cmd
	movs	r2, #0
	movs	r1, #4
	mov	r0, r4
	bl	set_one_cmd
	movs	r2, #0
	movs	r1, #25
	mov	r0, r4
	bl	set_one_cmd
	movs	r2, #0
	mov	r0, r4
	mov	r1, r2
	bl	set_one_cmd
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	movs	r1, #2
	mov	r0, r4
	bl	set_one_addr
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	movs	r3, #48
	movs	r2, #0
	strb	r3, [sp, #20]
	movs	r3, #2
	str	r3, [sp]
	add	r1, sp, #20
	mov	r3, r5
	mov	r0, r4
	bl	get_data_with_nand_bus_one_cmd
	ldrb	r1, [r5]	@ zero_extendqisi2
	ldrb	r2, [r5, #1]	@ zero_extendqisi2
	cmp	r1, #8
	bne	.L205
	cmp	r2, #8
	beq	.L214
.L205:
	mvn	r6, #95
	ldr	r0, .L219+4
	bl	NAND_Print
.L206:
	mov	r3, #1024
	movs	r2, #0
	str	r3, [sp]
	mov	r1, r2
	mov	r3, r5
	mov	r0, r4
	bl	get_data_with_nand_bus_one_cmd
	movs	r1, #0
	add	ip, r4, #112
.L207:
	add	r2, r5, r1, lsl #7
	add	r3, r4, #48
.L209:
	ldrb	r7, [r2], #1	@ zero_extendqisi2
	strb	r7, [r3], #1
	ldrb	r0, [r2, #63]	@ zero_extendqisi2
	add	r0, r0, r7
	cmp	r0, #255
	bne	.L208
	cmp	r3, ip
	bne	.L209
	ldr	r0, .L219+8
	bl	NAND_Print_DBG
	b	.L211
.L202:
	cmp	r3, #3
	bne	.L213
	movw	r3, #45230
	strh	r3, [sp, #24]	@ movhi
	movs	r3, #77
	strb	r3, [sp, #29]
	b	.L203
.L214:
	movs	r6, #0
	b	.L206
.L213:
	mvn	r6, #96
	b	.L201
.L208:
	adds	r1, r1, #1
	cmp	r1, #8
	bne	.L207
	mvn	r6, #94
	ldr	r0, .L219+12
	bl	NAND_Print_DBG
.L211:
	movs	r2, #1
	movs	r1, #255
	mov	r0, r4
	bl	set_one_cmd
	movs	r1, #56
	movs	r2, #1
	mov	r0, r4
	bl	set_one_cmd
	mov	r0, r4
	bl	nand_disable_chip
	mov	r1, #1024
	mov	r0, r5
	bl	nand_free_temp_buf
.L201:
	mov	r0, r6
	add	sp, sp, #36
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L220:
	.align	2
.L219:
	.word	.LANCHOR15
	.word	.LC22
	.word	.LC24
	.word	.LC23
	.size	hynix20nm_vender_get_param_otp_hynix, .-hynix20nm_vender_get_param_otp_hynix
	.section	.text.hynix20nm_saveotpvalue,"ax",%progbits
	.align	1
	.global	hynix20nm_saveotpvalue
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix20nm_saveotpvalue, %function
hynix20nm_saveotpvalue:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	subs	r3, r1, #1
	adds	r0, r0, #48
	adds	r1, r1, #63
.L222:
	ldrb	r2, [r3, #1]!	@ zero_extendqisi2
	cmp	r3, r1
	strb	r2, [r0], #1
	bne	.L222
	bx	lr
	.size	hynix20nm_saveotpvalue, .-hynix20nm_saveotpvalue
	.section	.text.hynix20nm_getotpparam,"ax",%progbits
	.align	1
	.global	hynix20nm_getotpparam
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix20nm_getotpparam, %function
hynix20nm_getotpparam:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	mov	r4, r0
	ldr	r8, .L234+4
	mov	r5, r1
	movs	r6, #0
.L227:
	mov	r1, r6
	mov	r0, r8
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	hynix20nm_vender_get_param_otp_hynix
	mov	r7, r0
	cbnz	r0, .L225
	mov	r0, r4
	mov	r1, r5
	add	r2, r4, #64
.L226:
	movs	r3, #0
.L229:
	adds	r4, r0, r3
	ldrb	r4, [r4, #48]	@ zero_extendqisi2
	strb	r4, [r1, r3]
	adds	r3, r3, #1
	cmp	r3, #8
	bne	.L229
	adds	r0, r0, #8
	adds	r1, r1, #8
	cmp	r2, r0
	bne	.L226
.L224:
	mov	r0, r7
	pop	{r4, r5, r6, r7, r8, pc}
.L225:
	adds	r6, r6, #1
	cmp	r6, #5
	bne	.L227
	ldr	r0, .L234
	bl	NAND_Print
	b	.L224
.L235:
	.align	2
.L234:
	.word	.LC26
	.word	.LC25
	.size	hynix20nm_getotpparam, .-hynix20nm_getotpparam
	.section	.text.hynix20nm_set_default_param,"ax",%progbits
	.align	1
	.global	hynix20nm_set_default_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix20nm_set_default_param, %function
hynix20nm_set_default_param:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r4, r5, r6, r7, lr}
	add	r1, r0, #48
	ldr	r5, .L241
	movs	r2, #0
	ldrb	r3, [r5]	@ zero_extendqisi2
.L237:
	cmp	r3, r2
	bhi	.L238
	ldr	r7, .L241+4
	mov	r1, sp
	ldr	r2, .L241+8
	movs	r4, #0
	bl	hynix20nm_vender_set_param
	mov	r6, r0
	ldr	r0, .L241+12
	bl	NAND_Print_DBG
.L239:
	ldrb	r3, [r5]	@ zero_extendqisi2
	cmp	r3, r4
	bhi	.L240
	ldr	r0, .L241+16
	bl	NAND_Print_DBG
	mov	r0, r6
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L238:
	ldrb	r4, [r1], #1	@ zero_extendqisi2
	strb	r4, [sp, r2]
	adds	r2, r2, #1
	b	.L237
.L240:
	ldrb	r1, [sp, r4]	@ zero_extendqisi2
	mov	r0, r7
	bl	NAND_Print_DBG
	adds	r4, r4, #1
	b	.L239
.L242:
	.align	2
.L241:
	.word	.LANCHOR13
	.word	.LC28
	.word	.LANCHOR14
	.word	.LC27
	.word	.LC7
	.size	hynix20nm_set_default_param, .-hynix20nm_set_default_param
	.section	.text.hynix20nm_readretry_exit,"ax",%progbits
	.align	1
	.global	hynix20nm_readretry_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix20nm_readretry_exit, %function
hynix20nm_readretry_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	ldr	r0, .L244
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	hynix20nm_set_default_param
	movs	r0, #0
	str	r0, [r4, #176]
	pop	{r4, pc}
.L245:
	.align	2
.L244:
	.word	.LC29
	.size	hynix20nm_readretry_exit, .-hynix20nm_readretry_exit
	.section	.text.hynix20nm_special_exit,"ax",%progbits
	.align	1
	.global	hynix20nm_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix20nm_special_exit, %function
hynix20nm_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L249
	push	{r4, lr}
	ldr	r3, [r3]
	ldr	r4, [r3, #8]
.L247:
	cbnz	r4, .L248
	ldr	r0, .L249+4
	bl	NAND_Print_DBG
	mov	r0, r4
	pop	{r4, pc}
.L248:
	mov	r0, r4
	bl	hynix20nm_readretry_exit
	ldr	r4, [r4]
	b	.L247
.L250:
	.align	2
.L249:
	.word	g_nsi
	.word	.LC30
	.size	hynix20nm_special_exit, .-hynix20nm_special_exit
	.section	.text.hynix26nm_vender_set_param,"ax",%progbits
	.align	1
	.global	hynix26nm_vender_set_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix26nm_vender_set_param, %function
hynix26nm_vender_set_param:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r5, r1
	sub	sp, sp, #24
	mov	r4, r2
	mov	r7, r3
	mov	r6, r0
	movs	r3, #54
	add	r10, sp, #24
	mov	r8, r5
	add	r7, r7, r4
	mov	fp, #1
	strb	r3, [r10, #-1]!
	bl	nand_enable_chip
	ldr	r0, [r6, #268]
	bl	ndfc_disable_randomize
.L252:
	cmp	r4, r7
	bne	.L253
	mov	r0, r6
	bl	nand_disable_chip
	ldrb	r0, [r5, #3]	@ zero_extendqisi2
	ldrb	r3, [r5, #2]	@ zero_extendqisi2
	ldrb	r2, [r5, #1]	@ zero_extendqisi2
	str	r0, [sp]
	ldrb	r1, [r5]	@ zero_extendqisi2
	ldr	r0, .L254
	bl	NAND_Print_DBG
	movs	r0, #0
	add	sp, sp, #24
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L253:
	mov	r3, r4
	str	r8, [sp]
	movs	r2, #0
	mov	r1, r10
	strd	fp, fp, [sp, #4]
	mov	r0, r6
	bl	set_cmd_with_nand_bus
	movs	r2, #0
	movs	r1, #22
	mov	r0, r6
	adds	r4, r4, #1
	bl	set_one_cmd
	add	r8, r8, #1
	b	.L252
.L255:
	.align	2
.L254:
	.word	.LC31
	.size	hynix26nm_vender_set_param, .-hynix26nm_vender_set_param
	.section	.text.hynix26nm_set_readretry,"ax",%progbits
	.align	1
	.global	hynix26nm_set_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix26nm_set_readretry, %function
hynix26nm_set_readretry:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L265
	push	{r0, r1, r4, r5, r6, r7, r8, lr}
	ldr	r1, [r0, #176]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r1, r3
	bhi	.L264
	ldr	r4, .L265+4
	movs	r6, #0
	ldr	r7, .L265+8
	add	r5, sp, #4
	ldrb	r3, [r4]	@ zero_extendqisi2
	add	r7, r7, r1, lsl #3
.L258:
	cmp	r3, r6
	bhi	.L259
	subs	r2, r1, #2
	subs	r1, r1, #5
	cmp	r2, #4
	ldr	r8, .L265+24
	it	ls
	movls	r2, #0
	mov	r6, #0
	it	ls
	strbls	r2, [sp, #4]
	cmp	r1, #1
	it	ls
	movls	r2, #0
	mov	r1, r5
	it	ls
	strbls	r2, [sp, #5]
	ldr	r2, .L265+12
	bl	hynix26nm_vender_set_param
	mov	r7, r0
	ldr	r0, .L265+16
	bl	NAND_Print_DBG
.L262:
	ldrb	r3, [r4]	@ zero_extendqisi2
	cmp	r3, r6
	bhi	.L263
	ldr	r0, .L265+20
	bl	NAND_Print_DBG
.L256:
	mov	r0, r7
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L259:
	adds	r2, r0, r6
	ldrh	ip, [r7, r6, lsl #1]
	ldrb	r2, [r2, #48]	@ zero_extendqisi2
	add	r2, r2, ip
	sxth	r2, r2
	usat	r2, #8, r2
	strb	r2, [r5, r6]
	adds	r6, r6, #1
	b	.L258
.L263:
	ldrb	r1, [r5, r6]	@ zero_extendqisi2
	mov	r0, r8
	bl	NAND_Print_DBG
	adds	r6, r6, #1
	b	.L262
.L264:
	mvn	r7, #52
	b	.L256
.L266:
	.align	2
.L265:
	.word	.LANCHOR20
	.word	.LANCHOR21
	.word	.LANCHOR22
	.word	.LANCHOR23
	.word	.LC32
	.word	.LC7
	.word	.LC20
	.size	hynix26nm_set_readretry, .-hynix26nm_set_readretry
	.section	.text.hynix26nm_read_page_end,"ax",%progbits
	.align	1
	.global	hynix26nm_read_page_end
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix26nm_read_page_end, %function
hynix26nm_read_page_end:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L280
	push	{r0, r1, r2, r4, r5, r6, r7, r8, r10, lr}
	mov	r6, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r5, r0
	mov	r0, r6
	bl	generic_read_page_end_not_retry
	adds	r3, r0, #2
	mov	r4, r0
	bne	.L267
	ldr	r7, .L280+4
	mov	r10, #0
	mov	r8, r10
	ldr	r0, .L280+8
	bl	NAND_Print_DBG
.L269:
	ldrb	r2, [r7]	@ zero_extendqisi2
	cmp	r2, r10
	ble	.L273
	ldr	r3, [r5, #176]
	mov	r0, r5
	adds	r3, r3, #1
	cmp	r3, r2
	it	hi
	movhi	r3, r8
	str	r3, [r5, #176]
	bl	hynix26nm_set_readretry
	mov	r4, r0
	cbz	r0, .L272
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	hynix26nm_set_readretry
.L273:
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	hynix26nm_set_readretry
.L267:
	mov	r0, r4
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L272:
	mov	r0, r6
	bl	generic_read_page_start
	mov	r4, r0
	mov	r0, r6
	bl	generic_read_page_end_not_retry
	orrs	r4, r4, r0
	cmp	r4, #10
	beq	.L274
	cbnz	r4, .L275
.L274:
	ldr	r3, [r5, #176]
	movs	r4, #10
	ldr	r1, [r5, #268]
	ldr	r0, .L280+12
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	b	.L273
.L275:
	add	r10, r10, #1
	b	.L269
.L281:
	.align	2
.L280:
	.word	g_nsi
	.word	.LANCHOR20
	.word	.LC33
	.word	.LC34
	.size	hynix26nm_read_page_end, .-hynix26nm_read_page_end
	.section	.text.hynix26nm_vender_get_param,"ax",%progbits
	.align	1
	.global	hynix26nm_vender_get_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix26nm_vender_get_param, %function
hynix26nm_vender_get_param:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, lr}
	mov	r4, r2
	sub	sp, sp, #20
	mov	r7, r3
	mov	r6, r0
	movs	r3, #55
	add	r8, sp, #16
	add	r7, r7, r4
	mov	r10, #1
	mov	r5, r1
	strb	r3, [r8, #-1]!
	bl	nand_enable_chip
	ldr	r0, [r6, #268]
	bl	ndfc_disable_randomize
.L283:
	cmp	r4, r7
	bne	.L284
	mov	r0, r6
	bl	nand_disable_chip
	movs	r0, #0
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L284:
	mov	r3, r5
	mov	r2, r4
	str	r10, [sp]
	mov	r1, r8
	mov	r0, r6
	adds	r4, r4, #1
	bl	get_data_with_nand_bus_one_cmd
	adds	r5, r5, #1
	b	.L283
	.size	hynix26nm_vender_get_param, .-hynix26nm_vender_get_param
	.section	.text.hynix26nm_lsb_init,"ax",%progbits
	.align	1
	.global	hynix26nm_lsb_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix26nm_lsb_init, %function
hynix26nm_lsb_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, [r0, #272]
	movs	r0, #0
	ldr	r2, .L286
	ldr	r3, [r3, #52]
	lsrs	r1, r3, #16
	strb	r1, [r2]
	lsrs	r1, r3, #8
	ldr	r2, .L286+4
	strb	r1, [r2]
	ldr	r2, .L286+8
	strb	r3, [r2]
	movs	r2, #5
	ldr	r3, .L286+12
	strb	r2, [r3]
	movs	r2, #164
	ldr	r3, .L286+16
	strb	r2, [r3]
	movs	r2, #165
	strb	r2, [r3, #1]
	movs	r2, #176
	strb	r2, [r3, #2]
	movs	r2, #177
	strb	r2, [r3, #3]
	movs	r2, #201
	strb	r2, [r3, #4]
	movs	r2, #37
	ldr	r3, .L286+20
	strb	r2, [r3]
	strb	r2, [r3, #1]
	strb	r2, [r3, #2]
	strb	r2, [r3, #3]
	movs	r2, #1
	strb	r2, [r3, #4]
	bx	lr
.L287:
	.align	2
.L286:
	.word	.LANCHOR24
	.word	.LANCHOR20
	.word	.LANCHOR21
	.word	.LANCHOR25
	.word	.LANCHOR26
	.word	.LANCHOR27
	.size	hynix26nm_lsb_init, .-hynix26nm_lsb_init
	.section	.text.hynix26nm_lsb_enable,"ax",%progbits
	.align	1
	.global	hynix26nm_lsb_enable
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix26nm_lsb_enable, %function
hynix26nm_lsb_enable:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r4, r5, r6, lr}
	mov	r5, r0
	ldr	r4, .L291
	ldr	r2, .L291+4
	ldrb	r3, [r4]	@ zero_extendqisi2
	ldr	r1, .L291+8
	bl	hynix26nm_vender_get_param
	ldrb	r3, [r4]	@ zero_extendqisi2
	movs	r2, #0
	ldr	r0, .L291+8
	ldr	r4, .L291+12
.L289:
	cmp	r3, r2
	bhi	.L290
	ldr	r2, .L291+4
	mov	r1, sp
	mov	r0, r5
	bl	hynix26nm_vender_set_param
	movs	r0, #0
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, pc}
.L290:
	ldrb	r1, [r2, r0]	@ zero_extendqisi2
	ldrb	r6, [r2, r4]	@ zero_extendqisi2
	add	r1, r1, r6
	strb	r1, [sp, r2]
	adds	r2, r2, #1
	b	.L289
.L292:
	.align	2
.L291:
	.word	.LANCHOR25
	.word	.LANCHOR26
	.word	.LANCHOR28
	.word	.LANCHOR27
	.size	hynix26nm_lsb_enable, .-hynix26nm_lsb_enable
	.section	.text.hynix26nm_lsb_disable,"ax",%progbits
	.align	1
	.global	hynix26nm_lsb_disable
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix26nm_lsb_disable, %function
hynix26nm_lsb_disable:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	ldr	r3, .L294
	ldr	r2, .L294+4
	ldrb	r3, [r3]	@ zero_extendqisi2
	ldr	r1, .L294+8
	bl	hynix26nm_vender_set_param
	movs	r0, #0
	pop	{r3, pc}
.L295:
	.align	2
.L294:
	.word	.LANCHOR25
	.word	.LANCHOR26
	.word	.LANCHOR28
	.size	hynix26nm_lsb_disable, .-hynix26nm_lsb_disable
	.section	.text.hynix26nm_lsb_exit,"ax",%progbits
	.align	1
	.global	hynix26nm_lsb_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix26nm_lsb_exit, %function
hynix26nm_lsb_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	bl	nand_enable_chip
	ldr	r0, [r4, #268]
	bl	ndfc_disable_randomize
	movs	r2, #0
	mov	r0, r4
	mov	r1, r2
	bl	set_one_cmd
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	movs	r2, #1
	movs	r1, #48
	mov	r0, r4
	bl	set_one_cmd
	ldr	r0, .L297
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	nand_disable_chip
	movs	r0, #0
	pop	{r4, pc}
.L298:
	.align	2
.L297:
	.word	.LC35
	.size	hynix26nm_lsb_exit, .-hynix26nm_lsb_exit
	.section	.text.hynix26nm_getdefaultparam,"ax",%progbits
	.align	1
	.global	hynix26nm_getdefaultparam
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix26nm_getdefaultparam, %function
hynix26nm_getdefaultparam:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	add	r4, r0, #48
	ldr	r5, .L302
	mov	r6, r1
	mov	r1, r4
	ldr	r2, .L302+4
	ldrb	r3, [r5]	@ zero_extendqisi2
	bl	hynix26nm_vender_get_param
	mov	r3, r6
.L300:
	ldrb	r1, [r5]	@ zero_extendqisi2
	subs	r2, r3, r6
	cmp	r1, r2
	bgt	.L301
	pop	{r4, r5, r6, pc}
.L301:
	ldrb	r2, [r4], #1	@ zero_extendqisi2
	strb	r2, [r3], #1
	b	.L300
.L303:
	.align	2
.L302:
	.word	.LANCHOR21
	.word	.LANCHOR23
	.size	hynix26nm_getdefaultparam, .-hynix26nm_getdefaultparam
	.section	.text.hynix26nm_setdefaultparam,"ax",%progbits
	.align	1
	.global	hynix26nm_setdefaultparam
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix26nm_setdefaultparam, %function
hynix26nm_setdefaultparam:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r4, r5, r6, r7, lr}
	add	r1, r0, #48
	ldr	r5, .L309
	movs	r2, #0
	ldrb	r3, [r5]	@ zero_extendqisi2
.L305:
	cmp	r3, r2
	bgt	.L306
	ldr	r7, .L309+4
	mov	r1, sp
	ldr	r2, .L309+8
	movs	r4, #0
	bl	hynix26nm_vender_set_param
	mov	r6, r0
	ldr	r0, .L309+12
	bl	NAND_Print_DBG
.L307:
	ldrb	r3, [r5]	@ zero_extendqisi2
	cmp	r3, r4
	bgt	.L308
	ldr	r0, .L309+16
	bl	NAND_Print_DBG
	mov	r0, r6
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L306:
	ldrb	r4, [r1], #1	@ zero_extendqisi2
	strb	r4, [sp, r2]
	adds	r2, r2, #1
	b	.L305
.L308:
	ldrb	r1, [sp, r4]	@ zero_extendqisi2
	mov	r0, r7
	bl	NAND_Print_DBG
	adds	r4, r4, #1
	b	.L307
.L310:
	.align	2
.L309:
	.word	.LANCHOR21
	.word	.LC28
	.word	.LANCHOR23
	.word	.LC27
	.word	.LC7
	.size	hynix26nm_setdefaultparam, .-hynix26nm_setdefaultparam
	.section	.text.hynix26nm_get_default_param,"ax",%progbits
	.align	1
	.global	hynix26nm_get_default_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix26nm_get_default_param, %function
hynix26nm_get_default_param:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	sub	sp, sp, #24
	add	r1, sp, #20
	mov	r4, r0
	bl	hynix26nm_getdefaultparam
	ldrb	r2, [sp, #23]	@ zero_extendqisi2
	ldr	r1, [r4, #268]
	ldrb	r3, [sp, #20]	@ zero_extendqisi2
	str	r2, [sp, #8]
	ldrb	r2, [sp, #22]	@ zero_extendqisi2
	ldr	r0, .L312
	str	r2, [sp, #4]
	ldrb	r2, [sp, #21]	@ zero_extendqisi2
	str	r2, [sp]
	ldr	r2, [r4, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	hynix26nm_setdefaultparam
	movs	r0, #0
	add	sp, sp, #24
	@ sp needed
	pop	{r4, pc}
.L313:
	.align	2
.L312:
	.word	.LC36
	.size	hynix26nm_get_default_param, .-hynix26nm_get_default_param
	.section	.text.hynix26nm_readretry_init,"ax",%progbits
	.align	1
	.global	hynix26nm_readretry_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix26nm_readretry_init, %function
hynix26nm_readretry_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	ldr	r3, [r0, #272]
	ldr	r4, .L329
	ldr	r2, [r3, #52]
	ldr	r3, .L329+4
	ubfx	r1, r2, #16, #8
	strb	r1, [r3]
	lsrs	r3, r2, #8
	strb	r3, [r4]
	ldr	r4, .L329+8
	strb	r2, [r4]
	cbnz	r1, .L315
	ldr	r4, .L329+12
	movs	r5, #172
	uxtb	r3, r3
	ldr	r6, .L329+16
	uxtb	r2, r2
	strb	r5, [r4]
	movs	r5, #173
	strb	r5, [r4, #1]
	movs	r5, #174
	strb	r5, [r4, #2]
	movs	r5, #175
	strb	r5, [r4, #3]
	adds	r3, r3, #1
	ldr	r5, .L329+20
.L316:
	cmp	r3, r1
	bhi	.L326
.L319:
	add	r2, r0, #48
	add	r1, r0, #176
	movs	r3, #0
	str	r3, [r0, #176]
.L325:
	strb	r3, [r2], #1
	cmp	r1, r2
	bne	.L325
	bl	hynix26nm_get_default_param
	movs	r0, #0
	b	.L314
.L317:
	ldrh	r7, [r5, r4, lsl #1]
	strh	r7, [r6, r4, lsl #1]	@ movhi
	adds	r4, r4, #1
.L318:
	cmp	r4, r2
	bcc	.L317
	adds	r1, r1, #1
	adds	r5, r5, #8
	adds	r6, r6, #8
	b	.L316
.L326:
	movs	r4, #0
	b	.L318
.L315:
	cmp	r1, #1
	bne	.L320
	ldr	r1, .L329+12
	movs	r4, #167
	uxtb	r3, r3
	ldr	r5, .L329+24
	ldr	r6, .L329+16
	uxtb	r2, r2
	strb	r4, [r1]
	movs	r4, #173
	strb	r4, [r1, #1]
	movs	r4, #174
	strb	r4, [r1, #2]
	movs	r4, #175
	strb	r4, [r1, #3]
	adds	r3, r3, #1
	movs	r4, #0
.L321:
	cmp	r3, r4
	bls	.L319
	movs	r1, #0
	b	.L323
.L322:
	ldrh	r7, [r5, r1, lsl #1]
	strh	r7, [r6, r1, lsl #1]	@ movhi
	adds	r1, r1, #1
.L323:
	cmp	r1, r2
	bcc	.L322
	adds	r4, r4, #1
	adds	r5, r5, #8
	adds	r6, r6, #8
	b	.L321
.L320:
	ldr	r0, .L329+28
	bl	NAND_Print
	mvn	r0, #53
.L314:
	pop	{r3, r4, r5, r6, r7, pc}
.L330:
	.align	2
.L329:
	.word	.LANCHOR20
	.word	.LANCHOR24
	.word	.LANCHOR21
	.word	.LANCHOR23
	.word	.LANCHOR22
	.word	.LANCHOR29
	.word	.LANCHOR30
	.word	.LC37
	.size	hynix26nm_readretry_init, .-hynix26nm_readretry_init
	.section	.text.hynix26nm_readretry_exit,"ax",%progbits
	.align	1
	.global	hynix26nm_readretry_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix26nm_readretry_exit, %function
hynix26nm_readretry_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	ldr	r0, .L332
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	hynix26nm_setdefaultparam
	movs	r0, #0
	str	r0, [r4, #176]
	pop	{r4, pc}
.L333:
	.align	2
.L332:
	.word	.LC38
	.size	hynix26nm_readretry_exit, .-hynix26nm_readretry_exit
	.section	.text.hynix26nm_special_init,"ax",%progbits
	.align	1
	.global	hynix26nm_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix26nm_special_init, %function
hynix26nm_special_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r4, #0
	ldr	r3, .L339
	ldr	r3, [r3]
	ldr	r5, [r3, #8]
.L335:
	cbnz	r5, .L336
	ldr	r3, .L339+4
	ldrb	r1, [r3]	@ zero_extendqisi2
	ldr	r3, .L339+8
	ldrb	r2, [r3]	@ zero_extendqisi2
	ldr	r3, .L339+12
	ldrb	r3, [r3]	@ zero_extendqisi2
	cbnz	r4, .L337
	ldr	r0, .L339+16
	ldr	r5, .L339+20
	str	r5, [r0]
	ldr	r0, .L339+24
	bl	NAND_Print_DBG
.L334:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L336:
	mov	r0, r5
	bl	hynix26nm_readretry_init
	ldr	r5, [r5]
	orrs	r4, r4, r0
	b	.L335
.L337:
	ldr	r0, .L339+28
	bl	NAND_Print
	b	.L334
.L340:
	.align	2
.L339:
	.word	g_nsi
	.word	.LANCHOR24
	.word	.LANCHOR20
	.word	.LANCHOR21
	.word	df_read_page_end
	.word	hynix26nm_read_page_end
	.word	.LC39
	.word	.LC40
	.size	hynix26nm_special_init, .-hynix26nm_special_init
	.section	.text.hynix26nm_special_exit,"ax",%progbits
	.align	1
	.global	hynix26nm_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix26nm_special_exit, %function
hynix26nm_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L344
	push	{r4, lr}
	ldr	r3, [r3]
	ldr	r4, [r3, #8]
.L342:
	cbnz	r4, .L343
	ldr	r0, .L344+4
	bl	NAND_Print_DBG
	mov	r0, r4
	pop	{r4, pc}
.L343:
	mov	r0, r4
	bl	hynix26nm_readretry_exit
	ldr	r4, [r4]
	b	.L342
.L345:
	.align	2
.L344:
	.word	g_nsi
	.word	.LC41
	.size	hynix26nm_special_exit, .-hynix26nm_special_exit
	.section	.text.toshiba_read_retry_clock_save,"ax",%progbits
	.align	1
	.global	toshiba_read_retry_clock_save
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	toshiba_read_retry_clock_save, %function
toshiba_read_retry_clock_save:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	ldr	r3, .L347
	ldr	r0, [r0, #268]
	ldr	r3, [r3]
	ldr	r2, .L347+4
	ldr	r1, .L347+8
	ldr	r0, [r0, #8]
	blx	r3
	ldr	r0, [r4, #268]
	movs	r2, #20
	ldr	r3, .L347+12
	movs	r1, #10
	ldr	r0, [r0, #8]
	ldr	r3, [r3]
	blx	r3
	movs	r0, #0
	pop	{r4, pc}
.L348:
	.align	2
.L347:
	.word	NAND_GetClk
	.word	.LANCHOR31
	.word	.LANCHOR32
	.word	NAND_SetClk
	.size	toshiba_read_retry_clock_save, .-toshiba_read_retry_clock_save
	.section	.text.toshiba_read_retry_clock_recover,"ax",%progbits
	.align	1
	.global	toshiba_read_retry_clock_recover
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	toshiba_read_retry_clock_recover, %function
toshiba_read_retry_clock_recover:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	ldr	r0, [r0, #268]
	ldr	r3, .L350
	ldr	r2, .L350+4
	ldr	r1, .L350+8
	ldr	r3, [r3]
	ldr	r2, [r2]
	ldr	r1, [r1]
	ldr	r0, [r0, #8]
	blx	r3
	movs	r0, #0
	pop	{r3, pc}
.L351:
	.align	2
.L350:
	.word	NAND_SetClk
	.word	.LANCHOR31
	.word	.LANCHOR32
	.size	toshiba_read_retry_clock_recover, .-toshiba_read_retry_clock_recover
	.section	.text.toshiba_vender_set_param,"ax",%progbits
	.align	1
	.global	toshiba_vender_set_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	toshiba_vender_set_param, %function
toshiba_vender_set_param:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r10, r3
	ldr	r7, .L361
	mov	r6, r0
	mov	r4, r1
	mov	r8, r2
	ldrb	r3, [r7]	@ zero_extendqisi2
	cmp	r3, #18
	bne	.L359
	mov	fp, #1
	mov	r3, r2
	strd	r1, fp, [sp]
	movs	r2, #0
	str	r10, [sp, #8]
	ldr	r1, .L361+4
	bl	set_cmd_with_nand_bus
	ldrb	r3, [r4]	@ zero_extendqisi2
	mov	r5, r0
	cbnz	r3, .L353
	ldrb	r3, [r4, #1]	@ zero_extendqisi2
	cbnz	r3, .L353
	ldrb	r3, [r4, #2]	@ zero_extendqisi2
	cbnz	r3, .L353
	ldrb	r3, [r4, #3]	@ zero_extendqisi2
	cbnz	r3, .L353
	ldr	r3, .L361+8
	ldr	r3, [r3]
	cbnz	r3, .L353
.L360:
	ldr	r3, .L361+12
	movs	r2, #1
	mov	r0, r6
	ldrb	r1, [r3]	@ zero_extendqisi2
	bl	set_one_cmd
	orrs	r5, r5, r0
.L352:
	mov	r0, r5
	add	sp, sp, #16
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L359:
	movs	r5, #0
.L353:
	ldrb	r3, [r7]	@ zero_extendqisi2
	cmp	r3, #17
	bne	.L355
	mov	fp, #1
	mov	r3, r8
	str	r10, [sp, #8]
	movs	r2, #0
	strd	r4, fp, [sp]
	mov	r0, r6
	ldr	r1, .L361+4
	bl	set_cmd_with_nand_bus
	ldrb	r3, [r4]	@ zero_extendqisi2
	orrs	r5, r5, r0
	cmp	r3, #0
	bne	.L356
	ldrb	r3, [r4, #1]	@ zero_extendqisi2
	cbnz	r3, .L355
	ldrb	r3, [r4, #2]	@ zero_extendqisi2
	cbnz	r3, .L355
	ldrb	r3, [r4, #3]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L360
.L355:
	ldrb	r3, [r7]	@ zero_extendqisi2
	cmp	r3, #16
	bne	.L357
	ldrb	r3, [r4]	@ zero_extendqisi2
	cmp	r3, #255
	bne	.L358
	ldrb	r3, [r4, #1]	@ zero_extendqisi2
	cmp	r3, #255
	bne	.L358
	ldrb	r3, [r4, #2]	@ zero_extendqisi2
	cmp	r3, #255
	bne	.L358
	ldrb	r3, [r4, #3]	@ zero_extendqisi2
	cmp	r3, #255
	beq	.L360
.L358:
	movs	r3, #1
	str	r10, [sp, #8]
	strd	r4, r3, [sp]
	movs	r2, #0
	mov	r3, r8
	ldr	r1, .L361+16
	mov	r0, r6
	bl	set_cmd_with_nand_bus
	orrs	r5, r5, r0
.L357:
	ldr	r7, .L361+20
	movs	r2, #0
	mov	r0, r6
	ldrb	r1, [r7]	@ zero_extendqisi2
	bl	set_one_cmd
	ldrb	r1, [r7, #1]	@ zero_extendqisi2
	orrs	r5, r5, r0
	movs	r2, #0
	mov	r0, r6
	bl	set_one_cmd
	orrs	r5, r5, r0
	ldrb	r0, [r4, #3]	@ zero_extendqisi2
	ldrb	r3, [r4, #2]	@ zero_extendqisi2
	ldrb	r2, [r4, #1]	@ zero_extendqisi2
	str	r0, [sp]
	ldrb	r1, [r4]	@ zero_extendqisi2
	ldr	r0, .L361+24
	bl	NAND_Print_DBG
	b	.L352
.L356:
	cmp	r3, #2
	bne	.L355
	ldrb	r2, [r4, #1]	@ zero_extendqisi2
	cmp	r2, #0
	bne	.L355
	ldrb	r3, [r4, #2]	@ zero_extendqisi2
	cmp	r3, #126
	bne	.L355
	ldrb	r3, [r4, #3]	@ zero_extendqisi2
	cmp	r3, #124
	bne	.L355
	ldr	r3, .L361+28
	mov	r0, r6
	ldrb	r1, [r3]	@ zero_extendqisi2
	bl	set_one_cmd
	orrs	r5, r5, r0
	b	.L355
.L362:
	.align	2
.L361:
	.word	.LANCHOR33
	.word	.LANCHOR34
	.word	.LANCHOR35
	.word	.LANCHOR36
	.word	.LANCHOR38
	.word	.LANCHOR39
	.word	.LC42
	.word	.LANCHOR37
	.size	toshiba_vender_set_param, .-toshiba_vender_set_param
	.section	.text.toshiba_vender_pre_condition,"ax",%progbits
	.align	1
	.global	toshiba_vender_pre_condition
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	toshiba_vender_pre_condition, %function
toshiba_vender_pre_condition:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r3, r4, lr}
	movs	r3, #0
	movs	r2, #2
	str	r3, [sp]
	strd	r3, r2, [sp, #4]
	mov	r2, r3
	ldr	r1, .L368
	bl	set_cmd_with_nand_bus
	mov	r4, r0
	cbz	r0, .L363
	ldr	r0, .L368+4
	bl	NAND_Print
.L363:
	mov	r0, r4
	add	sp, sp, #16
	@ sp needed
	pop	{r4, pc}
.L369:
	.align	2
.L368:
	.word	.LANCHOR40
	.word	.LC43
	.size	toshiba_vender_pre_condition, .-toshiba_vender_pre_condition
	.section	.text.toshiba_set_readretry,"ax",%progbits
	.align	1
	.global	toshiba_set_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	toshiba_set_readretry, %function
toshiba_set_readretry:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r4, r0
	ldr	r6, .L383
	ldrb	r2, [r6]	@ zero_extendqisi2
	subs	r2, r2, #16
	cmp	r2, #2
	bhi	.L378
	bl	toshiba_read_retry_clock_save
	mov	r0, r4
	bl	nand_enable_chip
	ldr	r0, [r4, #268]
	bl	ndfc_disable_randomize
	ldr	r3, [r4, #176]
	cbnz	r3, .L372
	mov	r0, r4
	bl	toshiba_vender_pre_condition
.L372:
	ldr	r0, [r4, #268]
	bl	ndfc_is_toggle_interface
	ldr	r5, .L383+4
	cbz	r0, .L373
	ldr	r0, [r4, #268]
	bl	ndfc_set_legacy_interface
	movs	r3, #1
	str	r3, [r5]
.L373:
	ldrb	r2, [r6]	@ zero_extendqisi2
	ldr	r1, [r4, #176]
	cmp	r2, #17
	lsl	r3, r1, #2
	bne	.L374
	add	r3, r3, r1
	ldr	r1, .L383+8
.L382:
	ldr	r2, .L383+12
	add	r1, r1, r3
	movs	r3, #5
.L375:
	mov	r0, r4
	bl	toshiba_vender_set_param
	ldr	r3, [r5]
	mov	r6, r0
	cmp	r3, #1
	bne	.L377
	ldr	r0, [r4, #268]
	bl	ndfc_set_toggle_interface
.L377:
	mov	r0, r4
	bl	nand_disable_chip
	mov	r0, r4
	bl	toshiba_read_retry_clock_recover
.L370:
	mov	r0, r6
	pop	{r4, r5, r6, pc}
.L374:
	cmp	r2, #18
	bne	.L376
	add	r3, r3, r1
	ldr	r1, .L383+16
	b	.L382
.L376:
	ldr	r1, .L383+20
	ldr	r2, .L383+24
	add	r1, r1, r3
	movs	r3, #4
	b	.L375
.L378:
	mvn	r6, #88
	b	.L370
.L384:
	.align	2
.L383:
	.word	.LANCHOR33
	.word	.LANCHOR43
	.word	.LANCHOR44
	.word	.LANCHOR41
	.word	.LANCHOR45
	.word	.LANCHOR46
	.word	.LANCHOR42
	.size	toshiba_set_readretry, .-toshiba_set_readretry
	.section	.text.toshiba_readretry_init,"ax",%progbits
	.align	1
	.global	toshiba_readretry_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	toshiba_readretry_init, %function
toshiba_readretry_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #272]
	ldr	r2, .L386
	push	{r4, lr}
	movs	r4, #0
	ldr	r3, [r3, #52]
	str	r4, [r0, #176]
	ldr	r0, .L386+4
	lsrs	r1, r3, #16
	strb	r1, [r2]
	lsrs	r1, r3, #8
	ldr	r2, .L386+8
	strb	r1, [r2]
	uxtb	r1, r1
	ldr	r2, .L386+12
	strb	r3, [r2]
	bl	NAND_Print_DBG
	mov	r0, r4
	pop	{r4, pc}
.L387:
	.align	2
.L386:
	.word	.LANCHOR33
	.word	.LC44
	.word	.LANCHOR47
	.word	.LANCHOR48
	.size	toshiba_readretry_init, .-toshiba_readretry_init
	.section	.text.toshiba_readretry_exit,"ax",%progbits
	.align	1
	.global	toshiba_readretry_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	toshiba_readretry_exit, %function
toshiba_readretry_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #0
	bx	lr
	.size	toshiba_readretry_exit, .-toshiba_readretry_exit
	.section	.text.toshiba_exit_readretry,"ax",%progbits
	.align	1
	.global	toshiba_exit_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	toshiba_exit_readretry, %function
toshiba_exit_readretry:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r4, r0
	ldr	r5, .L400
	ldrb	r2, [r5]	@ zero_extendqisi2
	subs	r2, r2, #16
	cmp	r2, #2
	bhi	.L394
	bl	nand_enable_chip
	ldr	r0, [r4, #268]
	bl	ndfc_disable_randomize
	ldr	r0, [r4, #268]
	bl	ndfc_is_toggle_interface
	ldr	r6, .L400+4
	cbz	r0, .L391
	ldr	r0, [r4, #268]
	bl	ndfc_set_legacy_interface
	movs	r3, #1
	str	r3, [r6]
.L391:
	ldrb	r3, [r5]	@ zero_extendqisi2
	cmp	r3, #17
	beq	.L395
	ldr	r1, .L400+8
	cmp	r3, #18
	ldr	r0, .L400+12
	it	eq
	moveq	r3, #5
	ldr	r2, .L400+16
	itt	ne
	movne	r3, #0
	movne	r2, r1
	ldr	r1, .L400+20
	it	ne
	movne	r1, r0
.L392:
	mov	r0, r4
	bl	toshiba_vender_set_param
	ldr	r3, [r6]
	mov	r5, r0
	cmp	r3, #1
	bne	.L393
	ldr	r0, [r4, #268]
	bl	ndfc_set_toggle_interface
.L393:
	mov	r0, r4
	bl	nand_disable_chip
.L389:
	mov	r0, r5
	pop	{r4, r5, r6, pc}
.L395:
	movs	r3, #5
	ldr	r2, .L400+16
	ldr	r1, .L400+24
	b	.L392
.L394:
	mvn	r5, #87
	b	.L389
.L401:
	.align	2
.L400:
	.word	.LANCHOR33
	.word	.LANCHOR43
	.word	.LANCHOR42
	.word	.LANCHOR46+24
	.word	.LANCHOR41
	.word	.LANCHOR45+50
	.word	.LANCHOR44+35
	.size	toshiba_exit_readretry, .-toshiba_exit_readretry
	.section	.text.toshiba_read_page_end,"ax",%progbits
	.align	1
	.global	toshiba_read_page_end
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	toshiba_read_page_end, %function
toshiba_read_page_end:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L412
	push	{r0, r1, r2, r4, r5, r6, r7, r8, r10, lr}
	mov	r6, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r5, r0
	mov	r0, r6
	bl	generic_read_page_end_not_retry
	adds	r3, r0, #2
	mov	r4, r0
	bne	.L402
	ldr	r0, .L412+4
	movs	r7, #0
	bl	NAND_Print_DBG
	ldr	r3, .L412+8
	movs	r2, #1
	ldr	r10, .L412+16
	mov	r8, r3
	str	r2, [r3]
.L404:
	ldrb	r3, [r10]	@ zero_extendqisi2
	cmp	r3, r7
	ble	.L407
	str	r7, [r5, #176]
	mov	r0, r5
	bl	toshiba_set_readretry
	mov	r4, r0
	cbnz	r0, .L405
	mov	r0, r6
	bl	generic_read_page_start
	mov	r4, r0
	mov	r0, r6
	bl	generic_read_page_end_not_retry
	orrs	r4, r4, r0
	cmp	r4, #10
	beq	.L406
	cbnz	r4, .L405
.L406:
	ldr	r3, [r5, #176]
	movs	r4, #10
	ldr	r1, [r5, #268]
	ldr	r0, .L412+12
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
.L407:
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	str	r3, [r8]
	bl	toshiba_exit_readretry
.L402:
	mov	r0, r4
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L405:
	adds	r7, r7, #1
	b	.L404
.L413:
	.align	2
.L412:
	.word	g_nsi
	.word	.LC45
	.word	.LANCHOR35
	.word	.LC46
	.word	.LANCHOR47
	.size	toshiba_read_page_end, .-toshiba_read_page_end
	.section	.text.toshiba_special_init,"ax",%progbits
	.align	1
	.global	toshiba_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	toshiba_special_init, %function
toshiba_special_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r4, #0
	ldr	r3, .L419
	ldr	r3, [r3]
	ldr	r5, [r3, #8]
.L415:
	cbnz	r5, .L416
	ldr	r3, .L419+4
	ldrb	r1, [r3]	@ zero_extendqisi2
	ldr	r3, .L419+8
	ldrb	r2, [r3]	@ zero_extendqisi2
	ldr	r3, .L419+12
	ldrb	r3, [r3]	@ zero_extendqisi2
	cbnz	r4, .L417
	ldr	r0, .L419+16
	ldr	r5, .L419+20
	str	r5, [r0]
	ldr	r0, .L419+24
	bl	NAND_Print_DBG
.L414:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L416:
	mov	r0, r5
	bl	toshiba_readretry_init
	ldr	r5, [r5]
	orrs	r4, r4, r0
	b	.L415
.L417:
	ldr	r0, .L419+28
	bl	NAND_Print
	b	.L414
.L420:
	.align	2
.L419:
	.word	g_nsi
	.word	.LANCHOR33
	.word	.LANCHOR47
	.word	.LANCHOR48
	.word	df_read_page_end
	.word	toshiba_read_page_end
	.word	.LC47
	.word	.LC48
	.size	toshiba_special_init, .-toshiba_special_init
	.section	.text.toshiba_special_exit,"ax",%progbits
	.align	1
	.global	toshiba_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	toshiba_special_exit, %function
toshiba_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L424
	push	{r4, lr}
	ldr	r3, [r3]
	ldr	r4, [r3, #8]
.L422:
	cbnz	r4, .L423
	ldr	r0, .L424+4
	bl	NAND_Print_DBG
	mov	r0, r4
	pop	{r4, pc}
.L423:
	ldr	r4, [r4]
	b	.L422
.L425:
	.align	2
.L424:
	.word	g_nsi
	.word	.LC49
	.size	toshiba_special_exit, .-toshiba_special_exit
	.section	.text.micron_intel_set_feature,"ax",%progbits
	.align	1
	.global	micron_intel_set_feature
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	micron_intel_set_feature, %function
micron_intel_set_feature:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, lr}
	sub	sp, sp, #20
	movs	r4, #0
	mov	r7, r0
	mov	r6, r2
	mov	r5, r1
	strb	r1, [sp, #7]
	add	r1, sp, #7
	strb	r2, [sp, #8]
	add	r2, sp, #8
	strb	r4, [sp, #9]
	strb	r4, [sp, #10]
	strb	r4, [sp, #11]
	bl	nand_set_feature
	add	r2, sp, #12
	add	r1, sp, #7
	mov	r0, r7
	bl	nand_get_feature
	ldrb	r2, [sp, #12]	@ zero_extendqisi2
	ldrb	r3, [sp, #8]	@ zero_extendqisi2
	cmp	r2, r3
	beq	.L428
	mov	r2, r6
	mov	r1, r5
	ldr	r0, .L429
	bl	NAND_Print
	mvn	r0, #71
.L426:
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L428:
	mov	r0, r4
	b	.L426
.L430:
	.align	2
.L429:
	.word	.LC50
	.size	micron_intel_set_feature, .-micron_intel_set_feature
	.section	.text.micron_set_readretry,"ax",%progbits
	.align	1
	.global	micron_set_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	micron_set_readretry, %function
micron_set_readretry:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	mov	r4, r0
	bl	nand_enable_chip
	ldr	r0, [r4, #268]
	bl	ndfc_disable_randomize
	ldr	r3, .L442
	ldrb	r3, [r3]	@ zero_extendqisi2
	sub	r2, r3, #64
	cmp	r2, #2
	bls	.L432
	cmp	r3, #80
	bne	.L440
.L432:
	ldrb	r6, [r4, #176]	@ zero_extendqisi2
	ldr	r7, .L442+4
	cmp	r6, #255
	it	eq
	moveq	r6, #0
	cmp	r3, #80
	bne	.L435
	ldr	r3, [r7]
	ldrb	r3, [r3, r6]	@ zero_extendqisi2
	cbz	r3, .L436
.L438:
	ldr	r3, [r4, #176]
	cmp	r3, #255
	bne	.L435
	movs	r2, #0
	movs	r1, #137
	mov	r0, r4
	bl	micron_intel_set_feature
	movs	r2, #0
	mov	r5, r0
	movs	r1, #147
	mov	r0, r4
	bl	micron_intel_set_feature
	orrs	r5, r5, r0
	b	.L433
.L436:
	movs	r2, #1
	movs	r1, #147
	mov	r0, r4
	bl	micron_intel_set_feature
	mov	r5, r0
	cmp	r0, #0
	beq	.L438
.L433:
	ldr	r3, [r4, #176]
	mov	r0, r4
	cmp	r3, #255
	itt	eq
	moveq	r3, #0
	streq	r3, [r4, #176]
	bl	nand_disable_chip
	mov	r0, r5
	pop	{r3, r4, r5, r6, r7, pc}
.L435:
	ldr	r3, [r7]
	movs	r1, #137
	mov	r0, r4
	ldrb	r2, [r3, r6]	@ zero_extendqisi2
	bl	micron_intel_set_feature
	mov	r5, r0
	b	.L433
.L440:
	movs	r5, #0
	b	.L433
.L443:
	.align	2
.L442:
	.word	.LANCHOR49
	.word	.LANCHOR50
	.size	micron_set_readretry, .-micron_set_readretry
	.section	.text.micron_read_page_end,"ax",%progbits
	.align	1
	.global	micron_read_page_end
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	micron_read_page_end, %function
micron_read_page_end:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L454
	push	{r0, r1, r4, r5, r6, r7, r8, lr}
	mov	r6, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r5, r0
	mov	r0, r6
	bl	generic_read_page_end_not_retry
	adds	r3, r0, #2
	mov	r4, r0
	bne	.L444
	ldr	r8, .L454+12
	movs	r7, #0
	ldr	r0, .L454+4
	bl	NAND_Print_DBG
.L446:
	ldrb	r3, [r8]	@ zero_extendqisi2
	cmp	r3, r7
	ble	.L449
	str	r7, [r5, #176]
	mov	r0, r5
	bl	micron_set_readretry
	mov	r4, r0
	cbnz	r0, .L447
	mov	r0, r6
	bl	generic_read_page_start
	mov	r4, r0
	mov	r0, r6
	bl	generic_read_page_end_not_retry
	orrs	r4, r4, r0
	cmp	r4, #10
	beq	.L448
	cbnz	r4, .L447
.L448:
	ldr	r3, [r5, #176]
	movs	r4, #10
	ldr	r1, [r5, #268]
	ldr	r0, .L454+8
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
.L449:
	movs	r3, #255
	mov	r0, r5
	str	r3, [r5, #176]
	bl	micron_set_readretry
.L444:
	mov	r0, r4
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L447:
	adds	r7, r7, #1
	b	.L446
.L455:
	.align	2
.L454:
	.word	g_nsi
	.word	.LC51
	.word	.LC52
	.word	.LANCHOR51
	.size	micron_read_page_end, .-micron_read_page_end
	.section	.text.micron_readretry_init,"ax",%progbits
	.align	1
	.global	micron_readretry_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	micron_readretry_init, %function
micron_readretry_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, r8, r10, lr}
	movs	r7, #0
	ldr	r3, [r0, #272]
	movs	r1, #137
	mov	r6, r0
	ldr	r10, .L464+28
	ldr	r3, [r3, #52]
	ldr	r5, .L464
	ldr	r8, .L464+32
	ldr	r4, .L464+4
	lsrs	r2, r3, #16
	lsrs	r3, r3, #8
	str	r7, [r0, #176]
	strb	r2, [r10]
	mov	r2, r7
	strb	r3, [r4]
	str	r8, [r5]
	bl	micron_intel_set_feature
	ldrb	r3, [r10]	@ zero_extendqisi2
	cmp	r3, #65
	bne	.L457
	movs	r3, #12
	str	r8, [r5]
	strb	r3, [r4]
.L458:
	ldrb	r1, [r4]	@ zero_extendqisi2
	ldr	r0, .L464+8
	bl	NAND_Print_DBG
	movs	r0, #0
	b	.L456
.L457:
	cmp	r3, #64
	bne	.L459
	ldr	r3, .L464+12
	str	r3, [r5]
	movs	r3, #10
.L463:
	strb	r3, [r4]
	b	.L458
.L459:
	cmp	r3, #80
	bne	.L460
	ldr	r3, .L464+16
	mov	r2, r7
	movs	r1, #147
	mov	r0, r6
	str	r3, [r5]
	movs	r3, #7
	strb	r3, [r4]
	bl	micron_intel_set_feature
	b	.L458
.L460:
	cmp	r3, #66
	bne	.L461
	ldr	r3, .L464+20
	str	r3, [r5]
	movs	r3, #18
	b	.L463
.L461:
	ldr	r0, .L464+24
	bl	NAND_Print
	mvn	r0, #70
.L456:
	pop	{r3, r4, r5, r6, r7, r8, r10, pc}
.L465:
	.align	2
.L464:
	.word	.LANCHOR50
	.word	.LANCHOR51
	.word	.LC54
	.word	.LANCHOR53
	.word	.LANCHOR54
	.word	.LANCHOR55
	.word	.LC53
	.word	.LANCHOR49
	.word	.LANCHOR52
	.size	micron_readretry_init, .-micron_readretry_init
	.section	.text.micron_readretry_exit,"ax",%progbits
	.align	1
	.global	micron_readretry_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	micron_readretry_exit, %function
micron_readretry_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L467
	movs	r0, #0
	strb	r0, [r3]
	ldr	r3, .L467+4
	strb	r0, [r3]
	bx	lr
.L468:
	.align	2
.L467:
	.word	.LANCHOR49
	.word	.LANCHOR51
	.size	micron_readretry_exit, .-micron_readretry_exit
	.section	.text.micron_special_init,"ax",%progbits
	.align	1
	.global	micron_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	micron_special_init, %function
micron_special_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r4, #0
	ldr	r3, .L474
	ldr	r3, [r3]
	ldr	r5, [r3, #8]
.L470:
	cbnz	r5, .L471
	ldr	r3, .L474+4
	ldrb	r1, [r3]	@ zero_extendqisi2
	ldr	r3, .L474+8
	ldrb	r2, [r3]	@ zero_extendqisi2
	cbnz	r4, .L472
	ldr	r0, .L474+12
	ldr	r3, .L474+16
	str	r0, [r3]
	ldr	r0, .L474+20
	bl	NAND_Print_DBG
.L469:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L471:
	mov	r0, r5
	bl	micron_readretry_init
	ldr	r5, [r5]
	orrs	r4, r4, r0
	b	.L470
.L472:
	ldr	r0, .L474+24
	bl	NAND_Print
	b	.L469
.L475:
	.align	2
.L474:
	.word	g_nsi
	.word	.LANCHOR49
	.word	.LANCHOR51
	.word	micron_read_page_end
	.word	df_read_page_end
	.word	.LC55
	.word	.LC56
	.size	micron_special_init, .-micron_special_init
	.section	.text.micron_special_exit,"ax",%progbits
	.align	1
	.global	micron_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	micron_special_exit, %function
micron_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r5, #0
	ldr	r3, .L483
	ldr	r2, .L483+4
	ldr	r3, [r3]
	ldrb	r1, [r2]	@ zero_extendqisi2
	ldr	r0, [r3, #8]
	ldr	r3, .L483+8
	ldrb	r4, [r3]	@ zero_extendqisi2
.L477:
	cbnz	r0, .L478
	cbz	r5, .L479
	strb	r1, [r2]
	strb	r4, [r3]
.L479:
	ldr	r0, .L483+12
	bl	NAND_Print_DBG
	movs	r0, #0
	pop	{r3, r4, r5, pc}
.L478:
	movs	r1, #0
	ldr	r0, [r0]
	movs	r5, #1
	mov	r4, r1
	b	.L477
.L484:
	.align	2
.L483:
	.word	g_nsi
	.word	.LANCHOR49
	.word	.LANCHOR51
	.word	.LC57
	.size	micron_special_exit, .-micron_special_exit
	.section	.text.hynix16nm_vender_set_param,"ax",%progbits
	.align	1
	.global	hynix16nm_vender_set_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_vender_set_param, %function
hynix16nm_vender_set_param:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	b	hynix26nm_vender_set_param
	.size	hynix16nm_vender_set_param, .-hynix16nm_vender_set_param
	.section	.text.hynix16nm_set_readretry,"ax",%progbits
	.align	1
	.global	hynix16nm_set_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_set_readretry, %function
hynix16nm_set_readretry:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L491
	push	{r0, r1, r4, lr}
	ldr	r1, [r0, #176]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r1, r3
	bhi	.L490
	ldr	r3, .L491+4
	lsls	r1, r1, #2
	movs	r2, #0
	ldrb	r3, [r3]	@ zero_extendqisi2
.L488:
	cmp	r3, r2
	bhi	.L489
	ldr	r2, .L491+8
	mov	r1, sp
	bl	hynix26nm_vender_set_param
.L486:
	add	sp, sp, #8
	@ sp needed
	pop	{r4, pc}
.L489:
	adds	r4, r1, r2
	add	r4, r4, r0
	ldrb	r4, [r4, #48]	@ zero_extendqisi2
	strb	r4, [sp, r2]
	adds	r2, r2, #1
	b	.L488
.L490:
	mvn	r0, #73
	b	.L486
.L492:
	.align	2
.L491:
	.word	.LANCHOR56
	.word	.LANCHOR57
	.word	.LANCHOR58
	.size	hynix16nm_set_readretry, .-hynix16nm_set_readretry
	.section	.text.hynix16nm_read_page_end,"ax",%progbits
	.align	1
	.global	hynix16nm_read_page_end
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_read_page_end, %function
hynix16nm_read_page_end:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L506
	push	{r0, r1, r2, r4, r5, r6, r7, r8, r10, lr}
	mov	r6, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r5, r0
	mov	r0, r6
	bl	generic_read_page_end_not_retry
	adds	r3, r0, #2
	mov	r4, r0
	bne	.L493
	ldr	r7, .L506+4
	mov	r10, #0
	mov	r8, r10
	ldr	r0, .L506+8
	bl	NAND_Print_DBG
.L495:
	ldrb	r2, [r7]	@ zero_extendqisi2
	cmp	r2, r10
	ble	.L499
	ldr	r3, [r5, #176]
	mov	r0, r5
	adds	r3, r3, #1
	cmp	r3, r2
	it	hi
	movhi	r3, r8
	str	r3, [r5, #176]
	bl	hynix16nm_set_readretry
	mov	r4, r0
	cbz	r0, .L498
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	hynix16nm_set_readretry
.L499:
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	hynix16nm_set_readretry
.L493:
	mov	r0, r4
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L498:
	mov	r0, r6
	bl	generic_read_page_start
	mov	r4, r0
	mov	r0, r6
	bl	generic_read_page_end_not_retry
	orrs	r4, r4, r0
	cmp	r4, #10
	beq	.L500
	cbnz	r4, .L501
.L500:
	ldr	r3, [r5, #176]
	movs	r4, #10
	ldr	r1, [r5, #268]
	ldr	r0, .L506+12
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	b	.L499
.L501:
	add	r10, r10, #1
	b	.L495
.L507:
	.align	2
.L506:
	.word	g_nsi
	.word	.LANCHOR56
	.word	.LC58
	.word	.LC59
	.size	hynix16nm_read_page_end, .-hynix16nm_read_page_end
	.section	.text.hynix16nm_get_special_info,"ax",%progbits
	.align	1
	.global	hynix16nm_get_special_info
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_get_special_info, %function
hynix16nm_get_special_info:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r4, r0
	ldr	r0, .L521
	mov	r8, r1
	mov	r7, r2
	mov	r10, r3
	bl	NAND_Print_DBG
	mov	r0, sp
	bl	physic_info_get_offset
	cbz	r0, .L515
	ldr	r0, .L521+4
	bl	NAND_Print
	movs	r0, #1
.L508:
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L515:
	ldr	fp, .L521+8
	movs	r5, #4
	add	r6, r4, #9728
.L509:
	mov	r1, r5
	mov	r0, fp
	bl	NAND_Print_DBG
	ldrb	r3, [r6]	@ zero_extendqisi2
	cmp	r3, #165
	beq	.L511
	mov	r3, r4
	add	r2, sp, #4
	ldr	r1, [sp]
	mov	r0, r5
	bl	physic_info_get_one_copy
.L511:
	ldrb	r3, [r6]	@ zero_extendqisi2
	cmp	r3, #165
	bne	.L512
	add	r2, r4, #9728
	add	r1, r4, #10752
	adds	r2, r2, #2
	movs	r3, #0
.L513:
	ldrb	r0, [r2], #1	@ zero_extendqisi2
	cmp	r2, r1
	add	r3, r3, r0
	uxtb	r3, r3
	bne	.L513
	movw	r2, #9729
	ldrb	r2, [r4, r2]	@ zero_extendqisi2
	cmp	r2, r3
	bne	.L514
	mla	r1, r7, r10, r4
	mov	r0, r8
	mov	r2, r7
	add	r1, r1, #9728
	adds	r1, r1, #2
	bl	NAND_Memcpy
	movs	r0, #0
	b	.L508
.L514:
	movs	r3, #0
	strb	r3, [r6]
.L512:
	adds	r5, r5, #1
	cmp	r5, #51
	bne	.L509
	mov	r0, #-1
	b	.L508
.L522:
	.align	2
.L521:
	.word	.LC60
	.word	.LC61
	.word	.LC62
	.size	hynix16nm_get_special_info, .-hynix16nm_get_special_info
	.section	.text.hynix20nm_init_readretry_param,"ax",%progbits
	.align	1
	.global	hynix20nm_init_readretry_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix20nm_init_readretry_param, %function
hynix20nm_init_readretry_param:
	@ args = 0, pretend = 0, frame = 64
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #272]
	push	{r4, r5, r6, r7, r8, lr}
	mov	r4, r0
	sub	sp, sp, #64
	ldr	r0, [r3, #16]
	lsls	r0, r0, #9
	bl	nand_get_temp_buf
	mov	r6, r0
	cbnz	r0, .L524
	mvn	r5, #93
	ldr	r0, .L532
	bl	NAND_Print_DBG
.L523:
	mov	r0, r5
	add	sp, sp, #64
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L524:
	mov	r0, #32768
	bl	nand_get_temp_buf
	cbnz	r0, .L526
	ldr	r3, [r4, #272]
	mov	r0, r6
	mvn	r5, #75
	ldr	r1, [r3, #16]
	lsls	r1, r1, #9
	bl	nand_free_temp_buf
	ldr	r0, .L532+4
	bl	NAND_Print
	b	.L523
.L526:
	ldr	r3, [r4, #16]
	movs	r2, #64
	mov	r1, r6
	bl	hynix16nm_get_special_info
	ldr	r3, [r4, #268]
	mov	r5, r0
	cbnz	r0, .L527
	ldr	r7, .L532+8
	ldr	r8, .L532+24
	ldr	r2, [r4, #16]
	ldr	r1, [r3, #8]
	ldr	r0, .L532+12
	bl	NAND_Print_DBG
.L529:
	ldrb	r1, [r6, r5]	@ zero_extendqisi2
	mov	r0, r7
	bl	NAND_Print_DBG
	and	r3, r5, #7
	cmp	r3, #7
	bne	.L528
	mov	r0, r8
	bl	NAND_Print_DBG
.L528:
	adds	r5, r5, #1
	cmp	r5, #64
	bne	.L529
	mov	r1, r6
	mov	r0, r4
	bl	hynix20nm_saveotpvalue
	mov	r0, r4
	bl	hynix20nm_set_default_param
	mov	r5, r0
.L530:
	ldr	r3, [r4, #272]
	mov	r0, r6
	ldr	r1, [r3, #16]
	lsls	r1, r1, #9
	bl	nand_free_temp_buf
	b	.L523
.L527:
	ldr	r1, [r3, #8]
	ldr	r0, .L532+16
	bl	NAND_Print_DBG
	mov	r1, sp
	mov	r0, r4
	bl	hynix20nm_getotpparam
	mov	r5, r0
	cmp	r0, #0
	bne	.L530
	mov	r0, r4
	bl	hynix20nm_set_default_param
	ldr	r3, [r4, #268]
	ldr	r0, .L532+20
	ldr	r1, [r3, #8]
	bl	NAND_Print_DBG
	b	.L530
.L533:
	.align	2
.L532:
	.word	.LC63
	.word	.LC64
	.word	.LC66
	.word	.LC65
	.word	.LC67
	.word	.LC68
	.word	.LC7
	.size	hynix20nm_init_readretry_param, .-hynix20nm_init_readretry_param
	.section	.text.hynix20nm_readretry_init,"ax",%progbits
	.align	1
	.global	hynix20nm_readretry_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix20nm_readretry_init, %function
hynix20nm_readretry_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #272]
	ldr	r2, .L543
	push	{r4, lr}
	ldr	r3, [r3, #52]
	ubfx	r1, r3, #16, #8
	strb	r1, [r2]
	lsrs	r4, r3, #8
	ldr	r2, .L543+4
	cmp	r1, #2
	strb	r4, [r2]
	ldr	r2, .L543+8
	strb	r3, [r2]
	bne	.L535
	ldr	r3, .L543+12
	movs	r2, #204
	strb	r2, [r3]
	movs	r2, #191
	strb	r2, [r3, #1]
	movs	r2, #170
	strb	r2, [r3, #2]
	movs	r2, #171
	strb	r2, [r3, #3]
	movs	r2, #205
	strb	r2, [r3, #4]
	movs	r2, #173
	strb	r2, [r3, #5]
	movs	r2, #174
	strb	r2, [r3, #6]
	movs	r2, #175
.L542:
	strb	r2, [r3, #7]
	add	r1, r0, #176
	add	r2, r0, #48
	movs	r3, #0
	str	r3, [r0, #176]
.L539:
	strb	r3, [r2], #1
	cmp	r2, r1
	bne	.L539
	pop	{r4, lr}
	b	hynix20nm_init_readretry_param
.L535:
	cmp	r1, #3
	bne	.L537
	ldr	r3, .L543+12
	movs	r2, #176
	strb	r2, [r3]
	movs	r2, #177
	strb	r2, [r3, #1]
	movs	r2, #178
	strb	r2, [r3, #2]
	movs	r2, #179
	strb	r2, [r3, #3]
	movs	r2, #180
	strb	r2, [r3, #4]
	movs	r2, #181
	strb	r2, [r3, #5]
	movs	r2, #182
	strb	r2, [r3, #6]
	movs	r2, #183
	b	.L542
.L537:
	ldr	r0, .L543+16
	bl	NAND_Print
	mvn	r0, #92
	pop	{r4, pc}
.L544:
	.align	2
.L543:
	.word	.LANCHOR15
	.word	.LANCHOR12
	.word	.LANCHOR13
	.word	.LANCHOR14
	.word	.LC37
	.size	hynix20nm_readretry_init, .-hynix20nm_readretry_init
	.section	.text.hynix20nm_special_init,"ax",%progbits
	.align	1
	.global	hynix20nm_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix20nm_special_init, %function
hynix20nm_special_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r4, #0
	ldr	r3, .L550
	ldr	r3, [r3]
	ldr	r5, [r3, #8]
.L546:
	cbnz	r5, .L547
	ldr	r3, .L550+4
	ldrb	r1, [r3]	@ zero_extendqisi2
	ldr	r3, .L550+8
	ldrb	r2, [r3]	@ zero_extendqisi2
	ldr	r3, .L550+12
	ldrb	r3, [r3]	@ zero_extendqisi2
	cbnz	r4, .L548
	ldr	r0, .L550+16
	ldr	r5, .L550+20
	str	r5, [r0]
	ldr	r0, .L550+24
	bl	NAND_Print_DBG
.L545:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L547:
	mov	r0, r5
	bl	hynix20nm_readretry_init
	ldr	r5, [r5]
	orrs	r4, r4, r0
	b	.L546
.L548:
	ldr	r0, .L550+28
	bl	NAND_Print
	b	.L545
.L551:
	.align	2
.L550:
	.word	g_nsi
	.word	.LANCHOR15
	.word	.LANCHOR12
	.word	.LANCHOR13
	.word	df_read_page_end
	.word	hynix20nm_read_page_end
	.word	.LC69
	.word	.LC70
	.size	hynix20nm_special_init, .-hynix20nm_special_init
	.section	.text.hynix16nm_write_page_FF,"ax",%progbits
	.align	1
	.global	hynix16nm_write_page_FF
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_write_page_FF, %function
hynix16nm_write_page_FF:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L571
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r10, r0
	sub	sp, sp, #24
	mov	r6, r1
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	ldr	r4, [r0, #268]
	mov	r5, r0
	ldr	r2, [r0, #20]
	ldr	r1, [r10, #4]
	ldr	r3, [r4, #12]
	add	r7, r4, #32
	ldr	ip, [r0, #24]
	cmp	r2, r3
	bcs	.L553
	cmp	r1, ip
	bcc	.L554
.L553:
	strd	r1, ip, [sp]
	ldr	r1, [r4, #8]
	mvn	r4, #107
	ldr	r0, .L571+4
	bl	NAND_Print
.L552:
	mov	r0, r4
	add	sp, sp, #24
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L554:
	bl	nand_read_chip_status_ready
	mov	r0, r5
	bl	nand_enable_chip
	ldr	r0, [r5, #268]
	bl	ndfc_disable_randomize
	ldr	r0, [r5, #268]
	bl	ndfc_disable_ecc
	mov	r0, r7
	bl	ndfc_clean_cmd_seq
	cmp	r6, #8
	beq	.L563
	cmp	r6, #16
	beq	.L564
	movs	r3, #0
	str	r3, [sp, #12]
.L568:
	str	r3, [sp, #8]
	add	r3, r4, #56
	str	r3, [sp, #16]
	mov	r8, #0
	ldr	r3, [sp, #8]
	mov	r6, r8
	subs	r3, r3, #1
	str	r3, [sp, #20]
.L557:
	ldr	r3, [sp, #8]
	lsl	fp, r6, #10
	cmp	r6, r3
	bne	.L561
	movs	r3, #0
	movs	r1, #16
	str	r3, [r4, #32]
	movs	r3, #1
	strd	r3, r1, [r4, #40]
	mov	r1, r7
	str	r3, [r4, #48]
	str	r3, [r4, #116]
	ldr	r0, [r5, #268]
	bl	ndfc_execute_cmd
	mov	r0, r5
	bl	nand_read_chip_status_ready
	mov	r4, r0
	cbz	r0, .L562
	ldr	r0, .L571+8
	bl	NAND_Print
.L562:
	mov	r0, r5
	bl	nand_disable_chip
	b	.L552
.L563:
	mov	r3, #832
	str	r3, [sp, #12]
	movs	r3, #9
	b	.L568
.L564:
	mov	r3, #640
	str	r3, [sp, #12]
	movs	r3, #18
	b	.L568
.L561:
	ldr	r1, [r10, #16]
	cbnz	r6, .L558
	movs	r3, #1
	movs	r0, #128
	mov	r2, #1024
	strd	r3, r0, [r4, #40]
	str	r3, [r4, #48]
	str	r3, [r4, #88]
	strd	r3, r3, [r4, #80]
	str	r1, [r4, #100]
	str	r2, [r4, #92]
	str	r3, [r4, #76]
	ldrd	r1, r2, [r10, #4]
	str	r6, [r4, #32]
	ldr	r0, [r5, #36]
	bl	get_row_addr
	movs	r3, #5
	mov	r8, r0
	str	r3, [r4, #72]
	mov	r2, r0
	ldr	r3, [sp, #16]
	movs	r1, #2
	str	r3, [sp]
	movs	r3, #3
.L569:
	mov	r0, fp
	adds	r6, r6, #1
	bl	fill_cmd_addr
	mov	r1, r7
	ldr	r0, [r5, #268]
	bl	ndfc_execute_cmd
	b	.L557
.L558:
	ldr	r3, [sp, #20]
	add	r1, r1, fp
	movs	r2, #133
	str	r1, [r4, #100]
	str	r2, [r4, #44]
	movs	r2, #1
	cmp	r3, r6
	str	r2, [r4, #40]
	ite	eq
	ldreq	r1, [sp, #12]
	movne	r1, #1024
	str	r2, [r4, #48]
	movs	r3, #0
	str	r2, [r4, #88]
	strd	r2, r2, [r4, #80]
	str	r2, [r4, #76]
	ldr	r2, [sp, #16]
	str	r1, [r4, #92]
	movs	r1, #2
	str	r3, [r4, #32]
	str	r1, [r4, #72]
	str	r2, [sp]
	mov	r2, r8
	b	.L569
.L572:
	.align	2
.L571:
	.word	g_nsi
	.word	.LC18
	.word	.LC71
	.size	hynix16nm_write_page_FF, .-hynix16nm_write_page_FF
	.section	.text.hynix16nm_vender_get_param,"ax",%progbits
	.align	1
	.global	hynix16nm_vender_get_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_vender_get_param, %function
hynix16nm_vender_get_param:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	b	hynix26nm_vender_get_param
	.size	hynix16nm_vender_get_param, .-hynix16nm_vender_get_param
	.section	.text.hynix16nm_major_check_byte,"ax",%progbits
	.align	1
	.global	hynix16nm_major_check_byte
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_major_check_byte, %function
hynix16nm_major_check_byte:
	@ args = 8, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r10, r0
	ldr	r8, [sp, #44]
	mov	r5, r2
	mov	fp, r3
	str	r1, [sp, #4]
	cmp	r2, r8, lsr #1
	bcs	.L593
	mov	r2, r8
	mov	r1, r5
	ldr	r0, .L596
	bl	NAND_Print
	mvn	r0, #106
	movs	r3, #255
	strb	r3, [r10]
.L574:
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L579:
	ldrb	lr, [r0], #1	@ zero_extendqisi2
	tst	lr, r1
	ite	ne
	addne	r2, r2, #1
	addeq	r3, r3, #1
.L590:
	cmp	ip, r0
	bne	.L579
	cmp	r5, r2
	bcs	.L580
.L595:
	orrs	r7, r7, r1
	uxtb	r7, r7
.L581:
	adds	r6, r6, #1
.L588:
	adds	r4, r4, #1
	cmp	r4, #8
	bne	.L575
	cmp	r6, #8
	iteet	ne
	movne	r3, #255
	strbeq	r7, [r10]
	moveq	r0, #0
	strbne	r3, [r10]
	it	ne
	mvnne	r0, #79
	b	.L574
.L586:
	ldrb	lr, [r0], #1	@ zero_extendqisi2
	tst	lr, r1
	ite	ne
	addne	r3, r3, #1
	addeq	r2, r2, #1
	b	.L583
.L593:
	movs	r6, #0
	mov	r7, r6
	mov	r4, r6
.L575:
	movs	r1, #1
	movs	r3, #0
	lsls	r1, r1, r4
	mov	r0, fp
	add	ip, r8, fp
	mov	r2, r3
	b	.L590
.L580:
	cmp	r5, r3
	bcc	.L581
	ldr	r0, [sp, #4]
	cmp	r0, #1
	bne	.L582
	ldr	r0, [sp, #40]
	movs	r3, #0
	mov	r2, r3
	add	ip, r8, r0
.L583:
	cmp	r0, ip
	bne	.L586
	cmp	r5, r2
	bcc	.L595
	cmp	r5, r3
	bcc	.L581
.L582:
	mov	r1, r4
	ldr	r0, .L596+4
	bl	NAND_Print
	mov	r1, r4
	ldr	r0, .L596+8
	bl	NAND_Print
	b	.L588
.L597:
	.align	2
.L596:
	.word	.LC72
	.word	.LC73
	.word	.LC74
	.size	hynix16nm_major_check_byte, .-hynix16nm_major_check_byte
	.section	.text.hynix16nm_get_read_retry_cfg,"ax",%progbits
	.align	1
	.global	hynix16nm_get_read_retry_cfg
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_get_read_retry_cfg, %function
hynix16nm_get_read_retry_cfg:
	@ args = 0, pretend = 0, frame = 64
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	sub	sp, sp, #72
	add	r4, sp, #8
	mov	fp, r0
	mov	r5, r1
	mov	r8, r2
	movs	r1, #0
	movs	r2, #32
	mov	r0, r4
	add	r7, sp, #40
	mov	r10, r3
	bl	memset
	movs	r2, #32
	movs	r1, #0
	mov	r0, r7
	bl	memset
	movs	r3, #0
.L599:
	ldrb	r2, [r10, r3]	@ zero_extendqisi2
	strb	r2, [r4, r3]
	adds	r3, r3, #1
	cmp	r3, #8
	bne	.L599
	strd	r7, r3, [sp]
	movs	r2, #4
	mov	r3, r4
	movs	r1, #0
	mov	r0, fp
	bl	hynix16nm_major_check_byte
	subs	r6, r0, #0
	bge	.L600
	ldr	r0, .L621
.L620:
	bl	NAND_Print
.L598:
	mov	r0, r6
	add	sp, sp, #72
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L600:
	ldrb	r1, [fp]	@ zero_extendqisi2
	ldr	r0, .L621+4
	bl	NAND_Print_DBG
	add	r2, r10, #7
	movs	r3, #0
.L602:
	ldrb	r1, [r2, #1]!	@ zero_extendqisi2
	strb	r1, [r4, r3]
	adds	r3, r3, #1
	cmp	r3, #8
	bne	.L602
	strd	r7, r3, [sp]
	movs	r2, #4
	mov	r3, r4
	movs	r1, #0
	mov	r0, r5
	bl	hynix16nm_major_check_byte
	subs	r6, r0, #0
	bge	.L603
	ldr	r0, .L621+8
	b	.L620
.L603:
	ldrb	r1, [r5]	@ zero_extendqisi2
	ldr	r0, .L621+12
	bl	NAND_Print_DBG
	ldrb	r1, [fp]	@ zero_extendqisi2
	ldrb	r2, [r5]	@ zero_extendqisi2
	cmp	r1, #8
	bne	.L604
	cmp	r2, #4
	beq	.L611
.L604:
	ldr	r0, .L621+16
	mvn	r6, #80
	bl	NAND_Print
	b	.L598
.L607:
	adds	r5, r5, #1
	cmp	r5, #32
	bne	.L605
	movs	r6, #0
	b	.L608
.L611:
	movs	r5, #0
.L605:
	add	r2, r10, r5
	movs	r3, #0
.L606:
	ldrb	r1, [r2, #16]	@ zero_extendqisi2
	adds	r2, r2, #64
	strb	r1, [r4, r3]
	ldrb	r1, [r2, #-16]	@ zero_extendqisi2
	strb	r1, [r7, r3]
	adds	r3, r3, #1
	cmp	r3, #8
	bne	.L606
	strd	r7, r3, [sp]
	movs	r2, #4
	mov	r3, r4
	movs	r1, #1
	add	r0, r8, r5
	bl	hynix16nm_major_check_byte
	subs	r6, r0, #0
	bge	.L607
	mov	r1, r5
	ldr	r0, .L621+20
	bl	NAND_Print
.L608:
	ldr	r7, .L621+24
	movs	r4, #0
	ldr	r5, .L621+28
.L610:
	lsls	r3, r4, #29
	bne	.L609
	cbz	r4, .L609
	mov	r0, r7
	bl	NAND_Print_DBG
.L609:
	ldrb	r1, [r8, r4]	@ zero_extendqisi2
	mov	r0, r5
	adds	r4, r4, #1
	bl	NAND_Print_DBG
	cmp	r4, #32
	bne	.L610
	ldr	r0, .L621+24
	bl	NAND_Print_DBG
	b	.L598
.L622:
	.align	2
.L621:
	.word	.LC75
	.word	.LC76
	.word	.LC77
	.word	.LC78
	.word	.LC79
	.word	.LC80
	.word	.LC7
	.word	.LC81
	.size	hynix16nm_get_read_retry_cfg, .-hynix16nm_get_read_retry_cfg
	.section	.text.hynix16nm_read_otp_info_hynix,"ax",%progbits
	.align	1
	.global	hynix16nm_read_otp_info_hynix
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_read_otp_info_hynix, %function
hynix16nm_read_otp_info_hynix:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r4, r0
	sub	sp, sp, #32
	mov	r8, r1
	bl	nand_enable_chip
	ldr	r0, [r4, #268]
	bl	ndfc_disable_randomize
	ldr	r2, [r4, #20]
	ldr	r3, [r4, #268]
	ldr	r0, .L628
	add	r3, r3, r2, lsl #2
	ldr	r2, [r3, #1148]
	ldr	r1, [r3, #1116]
	bl	NAND_Print_DBG
	cmp	r8, #0
	bne	.L624
	mov	r0, r4
	bl	nand_disable_chip
	ldr	r0, .L628+4
	bl	NAND_Print
	mvn	r0, #78
.L623:
	add	sp, sp, #32
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L624:
	movs	r2, #1
	movs	r1, #255
	mov	r0, r4
	movs	r6, #1
	bl	set_one_cmd
	add	r7, sp, #20
	movs	r3, #82
	movs	r5, #56
	mov	fp, #54
	strb	r3, [sp, #20]
	movs	r2, #0
	add	r3, sp, #24
	add	r1, sp, #16
	strd	r6, r6, [sp, #4]
	mov	r0, r4
	str	r7, [sp]
	mov	r10, #48
	strb	r5, [sp, #24]
	strb	fp, [sp, #16]
	bl	set_cmd_with_nand_bus
	movs	r2, #0
	movs	r1, #22
	mov	r0, r4
	bl	set_one_cmd
	movs	r2, #0
	movs	r1, #23
	mov	r0, r4
	bl	set_one_cmd
	movs	r2, #0
	movs	r1, #4
	mov	r0, r4
	bl	set_one_cmd
	movs	r2, #0
	movs	r1, #25
	mov	r0, r4
	bl	set_one_cmd
	movs	r2, #0
	mov	r0, r4
	mov	r1, r2
	bl	set_one_cmd
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	movs	r1, #2
	mov	r0, r4
	bl	set_one_addr
	movs	r1, #0
	mov	r0, r4
	bl	set_one_addr
	mov	r3, #528
	movs	r2, #0
	str	r3, [sp]
	add	r1, sp, #16
	mov	r3, r8
	mov	r0, r4
	strb	r10, [sp, #16]
	bl	get_data_with_nand_bus_one_cmd
	mov	r2, r6
	movs	r1, #255
	mov	r0, r4
	bl	set_one_cmd
	strb	r5, [sp, #24]
	movs	r5, #0
	add	r3, sp, #24
	mov	r2, r5
	add	r1, sp, #16
	strd	r6, r6, [sp, #4]
	str	r7, [sp]
	mov	r0, r4
	strb	fp, [sp, #16]
	strb	r5, [sp, #20]
	bl	set_cmd_with_nand_bus
	mov	r2, r5
	movs	r1, #22
	mov	r0, r4
	bl	set_one_cmd
	mov	r2, r5
	mov	r1, r5
	mov	r0, r4
	bl	set_one_cmd
	mov	r1, r5
	mov	r0, r4
	bl	set_one_addr
	mov	r1, r5
	mov	r0, r4
	bl	set_one_addr
	mov	r1, r5
	mov	r0, r4
	bl	set_one_addr
	mov	r1, r5
	mov	r0, r4
	bl	set_one_addr
	mov	r1, r5
	mov	r0, r4
	bl	set_one_addr
	mov	r2, r6
	mov	r1, r10
	mov	r0, r4
	bl	set_one_cmd
	ldrb	r1, [r8]	@ zero_extendqisi2
	ldrb	r2, [r8, #1]	@ zero_extendqisi2
	cmp	r1, #8
	bne	.L626
	cmp	r2, #8
	beq	.L627
.L626:
	ldr	r0, .L628+8
	bl	NAND_Print
	mov	r0, r4
	bl	nand_disable_chip
	mvn	r0, #95
	b	.L623
.L627:
	mov	r0, r4
	bl	nand_disable_chip
	mov	r0, r5
	b	.L623
.L629:
	.align	2
.L628:
	.word	.LC82
	.word	.LC83
	.word	.LC22
	.size	hynix16nm_read_otp_info_hynix, .-hynix16nm_read_otp_info_hynix
	.section	.text.hynix16nm_get_rr_value_otp_hynix,"ax",%progbits
	.align	1
	.global	hynix16nm_get_rr_value_otp_hynix
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_get_rr_value_otp_hynix, %function
hynix16nm_get_rr_value_otp_hynix:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r4, r5, r6, lr}
	mov	r4, r0
	mov	r0, #528
	bl	nand_get_temp_buf
	mov	r5, r0
	cbnz	r0, .L631
	mvn	r6, #77
	ldr	r0, .L635
	bl	NAND_Print
.L630:
	mov	r0, r6
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, pc}
.L631:
	mov	r1, r0
	mov	r0, r4
	bl	hynix16nm_read_otp_info_hynix
	cmp	r0, #0
	bge	.L633
	ldr	r3, [r4, #268]
	ldr	r2, [r4, #20]
	ldr	r0, .L635+4
	ldr	r1, [r3, #8]
	bl	NAND_Print
.L633:
	mov	r3, r5
	add	r2, r4, #48
	add	r1, sp, #7
	add	r0, sp, #6
	bl	hynix16nm_get_read_retry_cfg
	subs	r6, r0, #0
	bge	.L634
	ldr	r3, [r4, #268]
	ldr	r2, [r4, #20]
	ldr	r0, .L635+8
	ldr	r1, [r3, #8]
	bl	NAND_Print
.L634:
	mov	r1, #528
	mov	r0, r5
	bl	nand_free_temp_buf
	b	.L630
.L636:
	.align	2
.L635:
	.word	.LC84
	.word	.LC85
	.word	.LC86
	.size	hynix16nm_get_rr_value_otp_hynix, .-hynix16nm_get_rr_value_otp_hynix
	.section	.text.hynix16nm_save_otp_value,"ax",%progbits
	.align	1
	.global	hynix16nm_save_otp_value
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_save_otp_value, %function
hynix16nm_save_otp_value:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	subs	r3, r1, #1
	adds	r0, r0, #48
	adds	r1, r1, #31
.L638:
	ldrb	r2, [r3, #1]!	@ zero_extendqisi2
	cmp	r3, r1
	strb	r2, [r0], #1
	bne	.L638
	bx	lr
	.size	hynix16nm_save_otp_value, .-hynix16nm_save_otp_value
	.section	.text.hynix16nm_get_otp_param,"ax",%progbits
	.align	1
	.global	hynix16nm_get_otp_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_get_otp_param, %function
hynix16nm_get_otp_param:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	mov	r4, r0
	ldr	r7, .L647
	mov	r5, r1
	movs	r6, #0
.L641:
	mov	r1, r6
	mov	r0, r7
	bl	NAND_Print_DBG
	mov	r0, r4
	adds	r6, r6, #1
	bl	hynix16nm_get_rr_value_otp_hynix
	cmp	r0, #0
	bne	.L641
	mov	r0, r4
	mov	r1, r5
	adds	r4, r4, #32
.L642:
	mov	r2, r1
	movs	r3, #0
.L643:
	adds	r5, r0, r3
	adds	r3, r3, #1
	ldrb	r5, [r5, #48]	@ zero_extendqisi2
	cmp	r3, #4
	strb	r5, [r2], #1
	bne	.L643
	adds	r0, r0, #4
	adds	r1, r1, #4
	cmp	r0, r4
	bne	.L642
	movs	r0, #0
	pop	{r3, r4, r5, r6, r7, pc}
.L648:
	.align	2
.L647:
	.word	.LC25
	.size	hynix16nm_get_otp_param, .-hynix16nm_get_otp_param
	.section	.text.hynix16nm_set_default_param,"ax",%progbits
	.align	1
	.global	hynix16nm_set_default_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_set_default_param, %function
hynix16nm_set_default_param:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r4, r5, r6, r7, lr}
	add	r1, r0, #48
	ldr	r5, .L654
	movs	r2, #0
	ldrb	r3, [r5]	@ zero_extendqisi2
.L650:
	cmp	r3, r2
	bhi	.L651
	ldr	r7, .L654+4
	mov	r1, sp
	ldr	r2, .L654+8
	movs	r4, #0
	bl	hynix26nm_vender_set_param
	mov	r6, r0
	ldr	r0, .L654+12
	bl	NAND_Print_DBG
.L652:
	ldrb	r3, [r5]	@ zero_extendqisi2
	cmp	r3, r4
	bhi	.L653
	ldr	r0, .L654+16
	bl	NAND_Print_DBG
	mov	r0, r6
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L651:
	ldrb	r4, [r1], #1	@ zero_extendqisi2
	strb	r4, [sp, r2]
	adds	r2, r2, #1
	b	.L650
.L653:
	ldrb	r1, [sp, r4]	@ zero_extendqisi2
	mov	r0, r7
	bl	NAND_Print_DBG
	adds	r4, r4, #1
	b	.L652
.L655:
	.align	2
.L654:
	.word	.LANCHOR57
	.word	.LC28
	.word	.LANCHOR58
	.word	.LC27
	.word	.LC7
	.size	hynix16nm_set_default_param, .-hynix16nm_set_default_param
	.section	.text.hynix16nm_init_readretry_param,"ax",%progbits
	.align	1
	.global	hynix16nm_init_readretry_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_init_readretry_param, %function
hynix16nm_init_readretry_param:
	@ args = 0, pretend = 0, frame = 64
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #272]
	push	{r4, r5, r6, r7, r8, r10, lr}
	mov	r4, r0
	sub	sp, sp, #68
	ldr	r0, [r3, #16]
	lsls	r0, r0, #9
	bl	nand_get_temp_buf
	mov	r6, r0
	cmp	r0, #0
	beq	.L663
	mov	r0, #32768
	bl	nand_get_temp_buf
	mov	r7, r0
	cbnz	r0, .L658
	ldr	r3, [r4, #272]
	mov	r0, r6
	ldr	r1, [r3, #16]
	lsls	r1, r1, #9
	bl	nand_free_temp_buf
	ldr	r0, .L665
	bl	NAND_Print
	mvn	r0, #75
.L656:
	add	sp, sp, #68
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L658:
	ldr	r3, [r4, #16]
	movs	r2, #32
	mov	r1, r6
	bl	hynix16nm_get_special_info
	ldr	r3, [r4, #268]
	mov	r5, r0
	cbnz	r0, .L659
	ldr	r8, .L665+16
	mov	r2, r0
	ldr	r10, .L665+20
	ldr	r1, [r3, #8]
	ldr	r0, .L665+4
	bl	NAND_Print_DBG
.L661:
	ldrb	r1, [r6, r5]	@ zero_extendqisi2
	mov	r0, r8
	bl	NAND_Print_DBG
	and	r3, r5, #7
	cmp	r3, #7
	bne	.L660
	mov	r0, r10
	bl	NAND_Print_DBG
.L660:
	adds	r5, r5, #1
	cmp	r5, #32
	bne	.L661
	mov	r0, r4
	mov	r1, r6
	bl	hynix16nm_save_otp_value
	mov	r0, r4
	bl	hynix16nm_set_default_param
.L662:
	mov	r1, #32768
	mov	r0, r7
	bl	nand_free_temp_buf
	ldr	r3, [r4, #272]
	mov	r0, r6
	ldr	r1, [r3, #16]
	lsls	r1, r1, #9
	bl	nand_free_temp_buf
	movs	r0, #0
	b	.L656
.L659:
	ldr	r1, [r3, #8]
	ldr	r0, .L665+8
	bl	NAND_Print_DBG
	mov	r1, sp
	mov	r0, r4
	bl	hynix16nm_get_otp_param
	mov	r0, r4
	bl	hynix16nm_set_default_param
	ldr	r3, [r4, #268]
	movs	r2, #0
	ldr	r0, .L665+12
	ldr	r1, [r3, #8]
	bl	NAND_Print_DBG
	b	.L662
.L663:
	mvn	r0, #76
	b	.L656
.L666:
	.align	2
.L665:
	.word	.LC64
	.word	.LC87
	.word	.LC67
	.word	.LC88
	.word	.LC66
	.word	.LC7
	.size	hynix16nm_init_readretry_param, .-hynix16nm_init_readretry_param
	.section	.text.hynix16nm_readretry_init,"ax",%progbits
	.align	1
	.global	hynix16nm_readretry_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_readretry_init, %function
hynix16nm_readretry_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #272]
	ldr	r2, .L672
	push	{r4, lr}
	ldr	r3, [r3, #52]
	ubfx	r1, r3, #16, #8
	strb	r1, [r2]
	lsrs	r4, r3, #8
	ldr	r2, .L672+4
	cmp	r1, #4
	strb	r4, [r2]
	ldr	r2, .L672+8
	strb	r3, [r2]
	bne	.L668
	ldr	r3, .L672+12
	movs	r2, #56
	add	r1, r0, #176
	strb	r2, [r3]
	movs	r2, #57
	strb	r2, [r3, #1]
	movs	r2, #58
	strb	r2, [r3, #2]
	movs	r2, #59
	strb	r2, [r3, #3]
	add	r2, r0, #48
	movs	r3, #0
	str	r3, [r0, #176]
.L669:
	strb	r3, [r2], #1
	cmp	r2, r1
	bne	.L669
	bl	hynix16nm_init_readretry_param
	movs	r0, #0
.L667:
	pop	{r4, pc}
.L668:
	ldr	r0, .L672+16
	bl	NAND_Print
	mvn	r0, #74
	b	.L667
.L673:
	.align	2
.L672:
	.word	.LANCHOR59
	.word	.LANCHOR56
	.word	.LANCHOR57
	.word	.LANCHOR58
	.word	.LC37
	.size	hynix16nm_readretry_init, .-hynix16nm_readretry_init
	.section	.text.hynix16nm_readretry_exit,"ax",%progbits
	.align	1
	.global	hynix16nm_readretry_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_readretry_exit, %function
hynix16nm_readretry_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	ldr	r0, .L675
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	hynix16nm_set_default_param
	movs	r0, #0
	str	r0, [r4, #176]
	pop	{r4, pc}
.L676:
	.align	2
.L675:
	.word	.LC89
	.size	hynix16nm_readretry_exit, .-hynix16nm_readretry_exit
	.section	.text.hynix16nm_special_init,"ax",%progbits
	.align	1
	.global	hynix16nm_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_special_init, %function
hynix16nm_special_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r4, #0
	ldr	r3, .L682
	ldr	r3, [r3]
	ldr	r5, [r3, #8]
.L678:
	cbnz	r5, .L679
	ldr	r3, .L682+4
	ldrb	r1, [r3]	@ zero_extendqisi2
	ldr	r3, .L682+8
	ldrb	r2, [r3]	@ zero_extendqisi2
	ldr	r3, .L682+12
	ldrb	r3, [r3]	@ zero_extendqisi2
	cbnz	r4, .L680
	ldr	r0, .L682+16
	ldr	r5, .L682+20
	str	r5, [r0]
	ldr	r0, .L682+24
	bl	NAND_Print_DBG
.L677:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L679:
	mov	r0, r5
	bl	hynix16nm_readretry_init
	ldr	r5, [r5]
	orrs	r4, r4, r0
	b	.L678
.L680:
	ldr	r0, .L682+28
	bl	NAND_Print
	b	.L677
.L683:
	.align	2
.L682:
	.word	g_nsi
	.word	.LANCHOR59
	.word	.LANCHOR56
	.word	.LANCHOR57
	.word	df_read_page_end
	.word	hynix16nm_read_page_end
	.word	.LC90
	.word	.LC91
	.size	hynix16nm_special_init, .-hynix16nm_special_init
	.section	.text.hynix16nm_special_exit,"ax",%progbits
	.align	1
	.global	hynix16nm_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix16nm_special_exit, %function
hynix16nm_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L687
	push	{r4, lr}
	ldr	r3, [r3]
	ldr	r4, [r3, #8]
.L685:
	cbnz	r4, .L686
	ldr	r0, .L687+4
	bl	NAND_Print_DBG
	mov	r0, r4
	pop	{r4, pc}
.L686:
	mov	r0, r4
	bl	hynix16nm_readretry_exit
	ldr	r4, [r4]
	b	.L685
.L688:
	.align	2
.L687:
	.word	g_nsi
	.word	.LC92
	.size	hynix16nm_special_exit, .-hynix16nm_special_exit
	.section	.text.sandisk_read_retry_clock_save,"ax",%progbits
	.align	1
	.global	sandisk_read_retry_clock_save
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_read_retry_clock_save, %function
sandisk_read_retry_clock_save:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	ldr	r3, .L690
	ldr	r0, [r0, #268]
	ldr	r3, [r3]
	ldr	r2, .L690+4
	ldr	r1, .L690+8
	ldr	r0, [r0, #8]
	blx	r3
	ldr	r0, [r4, #268]
	movs	r2, #20
	ldr	r3, .L690+12
	movs	r1, #10
	ldr	r0, [r0, #8]
	ldr	r3, [r3]
	blx	r3
	movs	r0, #0
	pop	{r4, pc}
.L691:
	.align	2
.L690:
	.word	NAND_GetClk
	.word	.LANCHOR60
	.word	.LANCHOR61
	.word	NAND_SetClk
	.size	sandisk_read_retry_clock_save, .-sandisk_read_retry_clock_save
	.section	.text.sandisk_vender_set_param,"ax",%progbits
	.align	1
	.global	sandisk_vender_set_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_vender_set_param, %function
sandisk_vender_set_param:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, lr}
	mov	r6, r2
	ldr	r2, .L696
	sub	sp, sp, #20
	mov	r5, r0
	mov	r7, r1
	movs	r4, #0
	ldrb	r3, [r2]	@ zero_extendqisi2
	mov	r8, r2
	subs	r3, r3, #48
	cmp	r3, #1
	bhi	.L695
	movs	r3, #2
	mov	r2, r4
	strd	r4, r3, [sp, #4]
	mov	r3, r4
	ldr	r1, .L696+4
	str	r4, [sp]
	bl	set_cmd_with_nand_bus
	movs	r3, #1
	movs	r2, #3
	strd	r3, r2, [sp, #4]
	mov	r10, r0
	str	r7, [sp]
	mov	r3, r6
	mov	r2, r4
	ldr	r1, .L696+8
	mov	r0, r5
	bl	set_cmd_with_nand_bus
	orr	r0, r10, r0
.L693:
	ldrb	r3, [r8]	@ zero_extendqisi2
	cmp	r3, #50
	bne	.L692
	movs	r3, #4
	movs	r2, #1
	strd	r3, r2, [sp, #4]
	mov	r0, r5
	str	r7, [sp]
	mov	r3, r6
	ldr	r1, .L696+12
	bl	set_cmd_with_nand_bus
.L692:
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L695:
	mov	r0, r4
	b	.L693
.L697:
	.align	2
.L696:
	.word	.LANCHOR62
	.word	.LANCHOR63
	.word	.LANCHOR64
	.word	.LANCHOR65
	.size	sandisk_vender_set_param, .-sandisk_vender_set_param
	.section	.text.sandisk_read_retry_clock_recover,"ax",%progbits
	.align	1
	.global	sandisk_read_retry_clock_recover
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_read_retry_clock_recover, %function
sandisk_read_retry_clock_recover:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	ldr	r0, [r0, #268]
	ldr	r3, .L699
	ldr	r2, .L699+4
	ldr	r1, .L699+8
	ldr	r3, [r3]
	ldr	r2, [r2]
	ldr	r1, [r1]
	ldr	r0, [r0, #8]
	blx	r3
	movs	r0, #0
	pop	{r3, pc}
.L700:
	.align	2
.L699:
	.word	NAND_SetClk
	.word	.LANCHOR60
	.word	.LANCHOR61
	.size	sandisk_read_retry_clock_recover, .-sandisk_read_retry_clock_recover
	.section	.text.sandisk_end_one_read_retry,"ax",%progbits
	.align	1
	.global	sandisk_end_one_read_retry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_end_one_read_retry, %function
sandisk_end_one_read_retry:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L708
	push	{r4, r5, lr}
	mov	r4, r0
	sub	sp, sp, #20
	ldrb	r3, [r3]	@ zero_extendqisi2
	subs	r3, r3, #48
	cmp	r3, #1
	bhi	.L702
	bl	nand_enable_chip
	ldr	r0, [r4, #268]
	bl	ndfc_disable_randomize
	ldr	r0, [r4, #268]
	bl	ndfc_is_toggle_interface
	ldr	r5, .L708+4
	cbz	r0, .L703
	ldr	r0, [r4, #268]
	bl	ndfc_set_legacy_interface
	movs	r3, #1
	str	r3, [r5]
.L703:
	movs	r3, #0
	movs	r2, #1
	strd	r3, r2, [sp, #4]
	mov	r0, r4
	str	r3, [sp]
	mov	r2, r3
	ldr	r1, .L708+8
	bl	set_cmd_with_nand_bus
	ldr	r3, [r5]
	cmp	r3, #1
	bne	.L704
	ldr	r0, [r4, #268]
	bl	ndfc_set_toggle_interface
.L704:
	mov	r0, r4
	bl	nand_disable_chip
.L702:
	movs	r0, #0
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, pc}
.L709:
	.align	2
.L708:
	.word	.LANCHOR62
	.word	.LANCHOR66
	.word	.LANCHOR67
	.size	sandisk_end_one_read_retry, .-sandisk_end_one_read_retry
	.section	.text.sandisk_set_readretry,"ax",%progbits
	.align	1
	.global	sandisk_set_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_set_readretry, %function
sandisk_set_readretry:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r4, r0
	mov	r6, r1
	ldr	r5, .L735
	bl	sandisk_read_retry_clock_save
	mov	r0, r4
	bl	nand_enable_chip
	ldr	r0, [r4, #268]
	bl	ndfc_disable_randomize
	ldr	r0, [r4, #268]
	bl	ndfc_is_toggle_interface
	cbz	r0, .L711
	ldr	r0, [r4, #268]
	bl	ndfc_set_legacy_interface
	movs	r3, #1
	str	r3, [r5]
.L711:
	ldr	r3, .L735+4
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #48
	bne	.L712
	cmp	r6, #255
	ldr	r3, [r4, #176]
	ldr	r2, .L735+8
	beq	.L713
	cbz	r6, .L714
	lsls	r1, r6, #31
	bpl	.L713
.L714:
	ldr	r1, .L735+12
	add	r3, r3, r3, lsl #1
	add	r1, r1, r3
	movs	r3, #16
.L732:
	strb	r3, [r2]
	movs	r3, #3
	ldr	r2, .L735+16
.L734:
	mov	r0, r4
	bl	sandisk_vender_set_param
	movs	r2, #0
	mov	r6, r0
	movs	r1, #182
.L733:
	mov	r0, r4
	bl	set_one_cmd
.L716:
	ldr	r3, [r5]
	cmp	r3, #1
	bne	.L718
	ldr	r0, [r4, #268]
	bl	ndfc_set_toggle_interface
.L718:
	mov	r0, r4
	bl	nand_disable_chip
	mov	r0, r4
	bl	sandisk_read_retry_clock_recover
	mov	r0, r6
	pop	{r4, r5, r6, pc}
.L713:
	ldr	r1, .L735+20
	add	r3, r3, r3, lsl #1
	add	r1, r1, r3
	movs	r3, #20
	b	.L732
.L712:
	cmp	r3, #49
	bne	.L717
	ldr	r1, [r4, #176]
	movs	r3, #3
	ldr	r0, .L735+24
	ldr	r2, .L735+28
	add	r1, r1, r1, lsl #1
	add	r1, r1, r0
	b	.L734
.L717:
	cmp	r3, #50
	bne	.L719
	ldr	r0, [r4, #176]
	movs	r3, #1
	ldr	r1, .L735+32
	ldr	r2, .L735+36
	add	r1, r1, r0, lsl #2
	mov	r0, r4
	bl	sandisk_vender_set_param
	movs	r2, #0
	mov	r6, r0
	movs	r1, #93
	b	.L733
.L719:
	movs	r6, #0
	b	.L716
.L736:
	.align	2
.L735:
	.word	.LANCHOR66
	.word	.LANCHOR62
	.word	.LANCHOR69
	.word	.LANCHOR68
	.word	.LANCHOR71
	.word	.LANCHOR70
	.word	.LANCHOR73
	.word	.LANCHOR72
	.word	.LANCHOR75
	.word	.LANCHOR74
	.size	sandisk_set_readretry, .-sandisk_set_readretry
	.section	.text.sandisk_activation_seq,"ax",%progbits
	.align	1
	.global	sandisk_activation_seq
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_activation_seq, %function
sandisk_activation_seq:
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	mov	r8, #9
	sub	sp, sp, #40
	mov	r6, r0
	mov	r2, r8
	add	r0, sp, #16
	ldr	r1, .L742
	movs	r4, #0
	bl	memcpy
	mov	r0, r6
	strd	r4, r4, [sp, #28]
	strb	r4, [sp, #36]
	add	r7, sp, #28
	bl	nand_enable_chip
	ldr	r0, [r6, #268]
	bl	ndfc_disable_randomize
	movs	r3, #2
	mov	r2, r4
	strd	r4, r3, [sp, #4]
	mov	r0, r6
	mov	r3, r4
	ldr	r1, .L742+4
	str	r4, [sp]
	bl	set_cmd_with_nand_bus
	movs	r3, #1
	mov	r5, r0
	strd	r7, r3, [sp]
	mov	r2, r4
	str	r8, [sp, #8]
	add	r1, sp, #16
	ldr	r3, .L742+8
	mov	r0, r6
	bl	set_cmd_with_nand_bus
	orrs	r5, r5, r0
	mov	r0, r6
	bl	nand_disable_chip
	cbz	r5, .L737
	mov	r1, r5
	ldr	r0, .L742+12
	bl	NAND_Print
.L737:
	mov	r0, r5
	add	sp, sp, #40
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L743:
	.align	2
.L742:
	.word	.LANCHOR76
	.word	.LANCHOR63
	.word	.LANCHOR77
	.word	.LC93
	.size	sandisk_activation_seq, .-sandisk_activation_seq
	.section	.text.sandisk_readretry_init,"ax",%progbits
	.align	1
	.global	sandisk_readretry_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_readretry_init, %function
sandisk_readretry_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r2, #0
	ldr	r3, [r0, #272]
	str	r2, [r0, #176]
	ldr	r4, [r3, #52]
	ldr	r2, .L748
	ldr	r3, .L748+4
	ubfx	r1, r4, #16, #8
	lsrs	r5, r4, #8
	strb	r1, [r2]
	subs	r1, r1, #48
	strb	r5, [r3]
	cmp	r1, #1
	ldr	r5, .L748+8
	strb	r4, [r5]
	mov	r5, r2
	mov	r4, r3
	bhi	.L745
	bl	sandisk_activation_seq
.L745:
	ldrb	r3, [r5]	@ zero_extendqisi2
	subs	r3, r3, #48
	cmp	r3, #2
	bhi	.L747
	ldrb	r1, [r4]	@ zero_extendqisi2
	ldr	r0, .L748+12
	bl	NAND_Print_DBG
	movs	r0, #0
.L744:
	pop	{r3, r4, r5, pc}
.L747:
	mvn	r0, #98
	b	.L744
.L749:
	.align	2
.L748:
	.word	.LANCHOR62
	.word	.LANCHOR69
	.word	.LANCHOR78
	.word	.LC94
	.size	sandisk_readretry_init, .-sandisk_readretry_init
	.section	.text.sandisk_readretry_exit,"ax",%progbits
	.align	1
	.global	sandisk_readretry_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_readretry_exit, %function
sandisk_readretry_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #0
	bx	lr
	.size	sandisk_readretry_exit, .-sandisk_readretry_exit
	.section	.text.sandisk_exit_readretry,"ax",%progbits
	.align	1
	.global	sandisk_exit_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_exit_readretry, %function
sandisk_exit_readretry:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r4, r5, r6, lr}
	movs	r3, #0
	mov	r4, r0
	str	r3, [sp, #4]
	bl	nand_enable_chip
	ldr	r0, [r4, #268]
	bl	ndfc_disable_randomize
	ldr	r0, [r4, #268]
	bl	ndfc_is_toggle_interface
	ldr	r6, .L762
	cbz	r0, .L752
	ldr	r0, [r4, #268]
	bl	ndfc_set_legacy_interface
	movs	r3, #1
	str	r3, [r6]
.L752:
	ldr	r3, .L762+4
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #48
	bne	.L753
	ldr	r2, .L762+8
	movs	r3, #3
.L761:
	add	r1, sp, #4
	mov	r0, r4
	bl	sandisk_vender_set_param
	mov	r5, r0
.L754:
	ldr	r3, [r6]
	cmp	r3, #1
	bne	.L756
	ldr	r0, [r4, #268]
	bl	ndfc_set_toggle_interface
.L756:
	mov	r0, r4
	bl	nand_disable_chip
	mov	r0, r5
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, pc}
.L753:
	cmp	r3, #49
	bne	.L755
	movs	r3, #3
	ldr	r2, .L762+12
	b	.L761
.L755:
	cmp	r3, #50
	bne	.L757
	movs	r2, #1
	movs	r1, #255
	mov	r0, r4
	bl	set_one_cmd
	movs	r3, #1
	ldr	r2, .L762+16
	b	.L761
.L757:
	movs	r5, #0
	b	.L754
.L763:
	.align	2
.L762:
	.word	.LANCHOR66
	.word	.LANCHOR62
	.word	.LANCHOR71
	.word	.LANCHOR72
	.word	.LANCHOR74
	.size	sandisk_exit_readretry, .-sandisk_exit_readretry
	.section	.text.sandisk_special_init,"ax",%progbits
	.align	1
	.global	sandisk_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_special_init, %function
sandisk_special_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r4, #0
	ldr	r3, .L769
	ldr	r3, [r3]
	ldr	r5, [r3, #8]
.L765:
	cbnz	r5, .L766
	ldr	r3, .L769+4
	ldrb	r1, [r3]	@ zero_extendqisi2
	ldr	r3, .L769+8
	ldrb	r2, [r3]	@ zero_extendqisi2
	ldr	r3, .L769+12
	ldrb	r3, [r3]	@ zero_extendqisi2
	cbnz	r4, .L767
	ldr	r0, .L769+16
	ldr	r5, .L769+20
	str	r5, [r0]
	ldr	r0, .L769+24
	bl	NAND_Print_DBG
.L764:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L766:
	mov	r0, r5
	bl	sandisk_readretry_init
	ldr	r5, [r5]
	orrs	r4, r4, r0
	b	.L765
.L767:
	ldr	r0, .L769+28
	bl	NAND_Print
	b	.L764
.L770:
	.align	2
.L769:
	.word	g_nsi
	.word	.LANCHOR62
	.word	.LANCHOR69
	.word	.LANCHOR78
	.word	df_read_page_end
	.word	sandisk_read_page_end
	.word	.LC95
	.word	.LC96
	.size	sandisk_special_init, .-sandisk_special_init
	.section	.text.sandisk_special_exit,"ax",%progbits
	.align	1
	.global	sandisk_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_special_exit, %function
sandisk_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L774
	push	{r4, lr}
	ldr	r3, [r3]
	ldr	r4, [r3, #8]
.L772:
	cbnz	r4, .L773
	ldr	r0, .L774+4
	bl	NAND_Print_DBG
	mov	r0, r4
	pop	{r4, pc}
.L773:
	ldr	r4, [r4]
	b	.L772
.L775:
	.align	2
.L774:
	.word	g_nsi
	.word	.LC97
	.size	sandisk_special_exit, .-sandisk_special_exit
	.section	.text.samsung_vender_set_param,"ax",%progbits
	.align	1
	.global	samsung_vender_set_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	samsung_vender_set_param, %function
samsung_vender_set_param:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, lr}
	sub	sp, sp, #20
	str	r3, [sp, #8]
	movs	r3, #1
	mov	r4, r1
	strd	r1, r3, [sp]
	mov	r3, r2
	ldr	r1, .L777
	movs	r2, #0
	bl	set_cmd_with_nand_bus
	mov	r5, r0
	ldrb	r0, [r4, #3]	@ zero_extendqisi2
	ldrb	r3, [r4, #2]	@ zero_extendqisi2
	ldrb	r2, [r4, #1]	@ zero_extendqisi2
	str	r0, [sp]
	ldrb	r1, [r4]	@ zero_extendqisi2
	ldr	r0, .L777+4
	bl	NAND_Print_DBG
	mov	r0, r5
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, pc}
.L778:
	.align	2
.L777:
	.word	.LANCHOR79
	.word	.LC31
	.size	samsung_vender_set_param, .-samsung_vender_set_param
	.section	.text.samsung_readretry_init,"ax",%progbits
	.align	1
	.global	samsung_readretry_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	samsung_readretry_init, %function
samsung_readretry_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r2, [r0, #272]
	movs	r3, #0
	push	{r4, lr}
	ldr	r4, .L785
	ldrh	r2, [r2, #54]
	ldr	r1, [r0, #8]
	str	r3, [r0, #176]
	strb	r2, [r4]
	ldr	r2, .L785+4
	ldr	r3, .L785+8
	cmp	r1, r2
	bne	.L780
	ldrh	r1, [r0, #12]
	movw	r2, #51876
	cmp	r1, r2
	bne	.L780
	movs	r2, #255
.L784:
	strb	r2, [r3]
	ldr	r0, .L785+12
	ldrb	r1, [r3]	@ zero_extendqisi2
	bl	NAND_Print_DBG
	ldrb	r3, [r4]	@ zero_extendqisi2
	cmp	r3, #32
	ite	ne
	mvnne	r0, #99
	moveq	r0, #0
	pop	{r4, pc}
.L780:
	movs	r2, #15
	b	.L784
.L786:
	.align	2
.L785:
	.word	.LANCHOR80
	.word	-1013653780
	.word	.LANCHOR81
	.word	.LC98
	.size	samsung_readretry_init, .-samsung_readretry_init
	.section	.text.samsung_readretry_exit,"ax",%progbits
	.align	1
	.global	samsung_readretry_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	samsung_readretry_exit, %function
samsung_readretry_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #0
	bx	lr
	.size	samsung_readretry_exit, .-samsung_readretry_exit
	.section	.text.samsung_set_readretry_K9GCGD8U0F,"ax",%progbits
	.align	1
	.global	samsung_set_readretry_K9GCGD8U0F
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	samsung_set_readretry_K9GCGD8U0F, %function
samsung_set_readretry_K9GCGD8U0F:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, lr}
	movs	r3, #0
	sub	sp, sp, #20
	mov	r4, r0
	strb	r3, [sp, #7]
	strd	r3, r3, [sp, #8]
	bl	nand_enable_chip
	ldr	r0, [r4, #268]
	bl	ndfc_disable_randomize
	ldrb	r3, [r4, #176]	@ zero_extendqisi2
	add	r2, sp, #8
	add	r1, sp, #7
	mov	r0, r4
	strb	r3, [sp, #8]
	strb	r3, [sp, #9]
	strb	r3, [sp, #10]
	strb	r3, [sp, #11]
	mov	r3, #-1
	str	r3, [sp, #12]
	bl	nand_set_read_retry_K9GCGD8U0F
	add	r2, sp, #12
	add	r1, sp, #7
	mov	r0, r4
	bl	nand_get_read_retry_K9GCGD8U0F
	ldrb	r2, [sp, #12]	@ zero_extendqisi2
	mov	r5, r0
	ldrb	r3, [sp, #8]	@ zero_extendqisi2
	cmp	r2, r3
	bne	.L789
	ldrb	r2, [sp, #13]	@ zero_extendqisi2
	ldrb	r3, [sp, #9]	@ zero_extendqisi2
	cmp	r2, r3
	bne	.L789
	ldrb	r2, [sp, #14]	@ zero_extendqisi2
	ldrb	r3, [sp, #10]	@ zero_extendqisi2
	cmp	r2, r3
	bne	.L789
	ldrb	r2, [sp, #15]	@ zero_extendqisi2
	ldrb	r3, [sp, #11]	@ zero_extendqisi2
	cmp	r2, r3
	beq	.L790
.L789:
	mov	r5, #-1
	ldr	r1, .L792
	ldr	r0, .L792+4
	bl	NAND_Print
.L790:
	ldr	r3, [r4, #176]
	mov	r0, r4
	cmp	r3, #255
	itt	eq
	moveq	r3, #0
	streq	r3, [r4, #176]
	bl	nand_disable_chip
	mov	r0, r5
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, pc}
.L793:
	.align	2
.L792:
	.word	.LANCHOR82
	.word	.LC99
	.size	samsung_set_readretry_K9GCGD8U0F, .-samsung_set_readretry_K9GCGD8U0F
	.section	.text.samsung_set_readretry,"ax",%progbits
	.align	1
	.global	samsung_set_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	samsung_set_readretry, %function
samsung_set_readretry:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r4, r0
	bl	nand_enable_chip
	ldr	r0, [r4, #268]
	bl	ndfc_disable_randomize
	ldr	r2, [r4, #8]
	ldr	r3, .L804
	cmp	r2, r3
	bne	.L795
	ldrh	r2, [r4, #12]
	movw	r3, #51876
	cmp	r2, r3
	bne	.L795
	mov	r0, r4
	bl	samsung_set_readretry_K9GCGD8U0F
	mov	r5, r0
	cbz	r0, .L796
	ldr	r1, [r4, #176]
	ldr	r0, .L804+4
.L803:
	bl	NAND_Print
.L796:
	mov	r0, r4
	bl	nand_disable_chip
	mov	r0, r5
	pop	{r3, r4, r5, pc}
.L795:
	ldr	r0, [r4, #176]
	movs	r3, #4
	ldr	r1, .L804+8
	ldr	r2, .L804+12
	add	r1, r1, r0, lsl #2
	mov	r0, r4
	bl	samsung_vender_set_param
	mov	r5, r0
	cmp	r0, #0
	beq	.L796
	ldr	r1, [r4, #176]
	ldr	r0, .L804+16
	b	.L803
.L805:
	.align	2
.L804:
	.word	-1013653780
	.word	.LC100
	.word	.LANCHOR84
	.word	.LANCHOR83
	.word	.LC101
	.size	samsung_set_readretry, .-samsung_set_readretry
	.section	.text.samsung_read_page_end,"ax",%progbits
	.align	1
	.global	samsung_read_page_end
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	samsung_read_page_end, %function
samsung_read_page_end:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L818
	push	{r0, r1, r2, r4, r5, r6, r7, r8, r10, lr}
	mov	r6, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r5, r0
	mov	r0, r6
	bl	generic_read_page_end_not_retry
	adds	r3, r0, #2
	mov	r4, r0
	bne	.L806
	ldr	r8, .L818+12
	movs	r7, #1
	ldr	r10, .L818+16
	ldr	r0, .L818+4
	bl	NAND_Print_DBG
.L808:
	ldrb	r3, [r8]	@ zero_extendqisi2
	cmp	r3, r7
	ble	.L813
	ldr	r3, [r5, #8]
	cmp	r3, r10
	bne	.L809
	ldrh	r2, [r5, #12]
	movw	r3, #51876
	cmp	r2, r3
	bne	.L809
	rsb	r3, r7, #256
	str	r3, [r5, #176]
.L810:
	mov	r0, r5
	bl	samsung_set_readretry
	mov	r4, r0
	cbnz	r0, .L811
	mov	r0, r6
	bl	generic_read_page_start
	mov	r4, r0
	mov	r0, r6
	bl	generic_read_page_end_not_retry
	orrs	r4, r4, r0
	cmp	r4, #10
	beq	.L812
	cbnz	r4, .L811
.L812:
	ldr	r3, [r5, #176]
	movs	r4, #10
	ldr	r1, [r5, #268]
	ldr	r0, .L818+8
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
.L813:
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	samsung_set_readretry
.L806:
	mov	r0, r4
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L809:
	str	r7, [r5, #176]
	b	.L810
.L811:
	adds	r7, r7, #1
	b	.L808
.L819:
	.align	2
.L818:
	.word	g_nsi
	.word	.LC102
	.word	.LC103
	.word	.LANCHOR81
	.word	-1013653780
	.size	samsung_read_page_end, .-samsung_read_page_end
	.section	.text.samsung_special_init,"ax",%progbits
	.align	1
	.global	samsung_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	samsung_special_init, %function
samsung_special_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r4, #0
	ldr	r3, .L825
	ldr	r3, [r3]
	ldr	r5, [r3, #8]
.L821:
	cbnz	r5, .L822
	ldr	r3, .L825+4
	ldrb	r1, [r3]	@ zero_extendqisi2
	ldr	r3, .L825+8
	ldrb	r2, [r3]	@ zero_extendqisi2
	cbnz	r4, .L823
	ldr	r0, .L825+12
	ldr	r3, .L825+16
	str	r0, [r3]
	ldr	r0, .L825+20
	bl	NAND_Print_DBG
.L820:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L822:
	mov	r0, r5
	bl	samsung_readretry_init
	ldr	r5, [r5]
	orrs	r4, r4, r0
	b	.L821
.L823:
	ldr	r0, .L825+24
	bl	NAND_Print
	b	.L820
.L826:
	.align	2
.L825:
	.word	g_nsi
	.word	.LANCHOR80
	.word	.LANCHOR81
	.word	samsung_read_page_end
	.word	df_read_page_end
	.word	.LC104
	.word	.LC105
	.size	samsung_special_init, .-samsung_special_init
	.section	.text.samsung_special_exit,"ax",%progbits
	.align	1
	.global	samsung_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	samsung_special_exit, %function
samsung_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L830
	push	{r4, lr}
	ldr	r3, [r3]
	ldr	r4, [r3, #8]
.L828:
	cbnz	r4, .L829
	ldr	r0, .L830+4
	bl	NAND_Print_DBG
	mov	r0, r4
	pop	{r4, pc}
.L829:
	ldr	r4, [r4]
	b	.L828
.L831:
	.align	2
.L830:
	.word	g_nsi
	.word	.LC106
	.size	samsung_special_exit, .-samsung_special_exit
	.section	.text.generic_special_init,"ax",%progbits
	.align	1
	.global	generic_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	generic_special_init, %function
generic_special_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	ldr	r2, .L833
	ldr	r3, .L833+4
	ldr	r0, .L833+8
	str	r2, [r3]
	bl	NAND_Print_DBG
	movs	r0, #0
	pop	{r3, pc}
.L834:
	.align	2
.L833:
	.word	generic_read_page_end_not_retry
	.word	df_read_page_end
	.word	.LC107
	.size	generic_special_init, .-generic_special_init
	.section	.text.generic_special_exit,"ax",%progbits
	.align	1
	.global	generic_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	generic_special_exit, %function
generic_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	ldr	r0, .L836
	bl	NAND_Print_DBG
	movs	r0, #0
	pop	{r3, pc}
.L837:
	.align	2
.L836:
	.word	.LC108
	.size	generic_special_exit, .-generic_special_exit
	.section	.text.is_on_panic,"ax",%progbits
	.align	1
	.global	is_on_panic
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_on_panic, %function
is_on_panic:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L839
	ldr	r0, [r3]
	adds	r0, r0, #0
	it	ne
	movne	r0, #1
	bx	lr
.L840:
	.align	2
.L839:
	.word	.LANCHOR85
	.size	is_on_panic, .-is_on_panic
	.section	.text.panic_mark_enable,"ax",%progbits
	.align	1
	.global	panic_mark_enable
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	panic_mark_enable, %function
panic_mark_enable:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L842
	movs	r2, #1
	str	r2, [r3]
	bx	lr
.L843:
	.align	2
.L842:
	.word	.LANCHOR86
	.size	panic_mark_enable, .-panic_mark_enable
	.section	.text.is_panic_enable,"ax",%progbits
	.align	1
	.global	is_panic_enable
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_panic_enable, %function
is_panic_enable:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L845
	ldr	r0, [r3]
	adds	r0, r0, #0
	it	ne
	movne	r0, #1
	bx	lr
.L846:
	.align	2
.L845:
	.word	.LANCHOR86
	.size	is_panic_enable, .-is_panic_enable
	.section	.text.panic_init_part,"ax",%progbits
	.align	1
	.global	panic_init_part
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	panic_init_part, %function
panic_init_part:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r3, r2
	cbz	r0, .L851
	cbz	r2, .L851
	ldr	r5, .L852
	ldr	r4, [r5, #8]
	cbnz	r4, .L851
	str	r2, [r5, #8]
	ldr	r2, .L852+4
	strd	r0, r1, [r5]
	movs	r5, #1
	str	r5, [r2]
	mov	r2, r1
	mov	r1, r0
	ldr	r0, .L852+8
	bl	NAND_Print
	mov	r0, r4
.L847:
	pop	{r3, r4, r5, pc}
.L851:
	mov	r0, #-1
	b	.L847
.L853:
	.align	2
.L852:
	.word	.LANCHOR87
	.word	.LANCHOR86
	.word	.LC109
	.size	panic_init_part, .-panic_init_part
	.section	.text.panic_read,"ax",%progbits
	.align	1
	.global	panic_read
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	panic_read, %function
panic_read:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r8, r3
	ldr	r3, .L860
	mov	r6, r0
	mov	r5, r2
	ldr	r3, [r3]
	cbz	r3, .L858
	ldr	r3, .L860+4
	movs	r2, #1
	mov	r4, r1
	sub	fp, r8, r1
	str	r2, [r3]
	bic	r3, r5, #32640
	bic	r3, r3, #127
	add	r10, r3, r1
.L856:
	cmp	r4, r10
	add	r3, fp, r4
	bne	.L857
	ubfx	r2, r5, #0, #15
	bic	r5, r5, #32640
	bic	r5, r5, #127
	add	r3, r8, r5
	cbz	r2, .L859
	ldr	r5, [r6, #44]
	mov	r1, r4
	mov	r0, r6
	blx	r5
	movs	r0, #0
.L854:
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L857:
	mov	r1, r4
	ldr	r7, [r6, #44]
	mov	r2, #32768
	mov	r0, r6
	blx	r7
	add	r4, r4, #32768
	b	.L856
.L858:
	mov	r0, #-1
	b	.L854
.L859:
	mov	r0, r2
	b	.L854
.L861:
	.align	2
.L860:
	.word	.LANCHOR86
	.word	.LANCHOR85
	.size	panic_read, .-panic_read
	.section	.text.is_panic_data_page,"ax",%progbits
	.align	1
	.global	is_panic_data_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_panic_data_page, %function
is_panic_data_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldrb	r3, [r0, #1]	@ zero_extendqisi2
	cmp	r3, #170
	bne	.L864
	ldrb	r0, [r0, #2]	@ zero_extendqisi2
	sub	r3, r0, #99
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	bx	lr
.L864:
	movs	r0, #0
	bx	lr
	.size	is_panic_data_page, .-is_panic_data_page
	.section	.text.is_panic_dummy_page,"ax",%progbits
	.align	1
	.global	is_panic_dummy_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_panic_dummy_page, %function
is_panic_dummy_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldrb	r3, [r0, #1]	@ zero_extendqisi2
	cmp	r3, #170
	bne	.L867
	ldrb	r0, [r0, #2]	@ zero_extendqisi2
	sub	r3, r0, #98
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	bx	lr
.L867:
	movs	r0, #0
	bx	lr
	.size	is_panic_dummy_page, .-is_panic_dummy_page
	.section	.text.panic_recover_info,"ax",%progbits
	.align	1
	.global	panic_recover_info
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	panic_recover_info, %function
panic_recover_info:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r5, r1
	ldrb	r4, [r0, #4]	@ zero_extendqisi2
	cmp	r4, #2
	bhi	.L869
	ldrh	r1, [r1]
	ldr	r0, .L871
	bl	NAND_Print
	ldr	r3, .L871+4
	movs	r0, #0
	str	r5, [r3, r4, lsl #2]
.L868:
	pop	{r3, r4, r5, pc}
.L869:
	mov	r1, r4
	ldr	r0, .L871+8
	bl	NAND_Print
	mov	r0, #-1
	b	.L868
.L872:
	.align	2
.L871:
	.word	.LC110
	.word	.LANCHOR88
	.word	.LC111
	.size	panic_recover_info, .-panic_recover_info
	.section	.text.add_free_list,"ax",%progbits
	.align	1
	.global	add_free_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	add_free_list, %function
add_free_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	adds	r0, r0, #152
	push	{r4, lr}
.L874:
	ldr	r3, [r0, #20]
	cbnz	r3, .L876
	str	r1, [r0, #20]
	str	r3, [r1, #20]
	b	.L878
.L876:
	ldrh	r2, [r3, #6]
	ldrh	r4, [r1, #6]
	cmp	r4, r2
	bhi	.L877
	str	r1, [r3, #24]
	str	r3, [r1, #20]
	str	r1, [r0, #20]
.L878:
	str	r0, [r1, #24]
	movs	r0, #0
	pop	{r4, pc}
.L877:
	mov	r0, r3
	b	.L874
	.size	add_free_list, .-add_free_list
	.section	.text.del_free_list,"ax",%progbits
	.align	1
	.global	del_free_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	del_free_list, %function
del_free_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	mov	r3, r0
	ldr	r0, [r0, #172]
	cbz	r0, .L879
	ldr	r2, [r0, #20]
	str	r2, [r3, #172]
	ldr	r2, [r0, #20]
	str	r2, [r3, #172]
	ldr	r2, [r0, #20]
	cbz	r2, .L879
	adds	r3, r3, #152
	str	r3, [r2, #24]
.L879:
	bx	lr
	.size	del_free_list, .-del_free_list
	.section	.text.del_free_list_by_block,"ax",%progbits
	.align	1
	.global	del_free_list_by_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	del_free_list_by_block, %function
del_free_list_by_block:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, [r0, #172]
.L887:
	cbnz	r0, .L890
	bx	lr
.L890:
	cmp	r0, r1
	ldr	r3, [r0, #20]
	bne	.L891
	ldr	r2, [r0, #24]
	str	r3, [r2, #20]
	ldr	r3, [r0, #20]
	cbz	r3, .L888
	str	r2, [r3, #24]
.L888:
	movs	r3, #0
	strd	r3, r3, [r0, #20]
	bx	lr
.L891:
	mov	r0, r3
	b	.L887
	.size	del_free_list_by_block, .-del_free_list_by_block
	.section	.text.is_block_in_free_list,"ax",%progbits
	.align	1
	.global	is_block_in_free_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_block_in_free_list, %function
is_block_in_free_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, [r0, #172]
.L896:
	cbnz	r0, .L898
	bx	lr
.L898:
	cmp	r0, r1
	beq	.L899
	ldr	r0, [r0, #20]
	b	.L896
.L899:
	movs	r0, #1
	bx	lr
	.size	is_block_in_free_list, .-is_block_in_free_list
	.section	.text.print_free_list,"ax",%progbits
	.align	1
	.global	print_free_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	print_free_list, %function
print_free_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #356]
	push	{r4, r5, r6, lr}
	mov	r4, r0
	cbz	r3, .L900
	ldr	r0, .L905
	bl	NAND_Print_DBG
	ldr	r5, .L905+4
	ldr	r4, [r4, #172]
	ldr	r6, .L905+8
.L902:
	cbnz	r4, .L903
.L900:
	pop	{r4, r5, r6, pc}
.L903:
	ldrh	r2, [r4, #6]
	mov	r0, r5
	ldrh	r1, [r4]
	bl	NAND_Print_DBG
	ldr	r1, [r4, #8]
	mov	r0, r6
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #2]
	ldr	r0, .L905+12
	bl	NAND_Print_DBG
	ldr	r4, [r4, #20]
	b	.L902
.L906:
	.align	2
.L905:
	.word	.LC112
	.word	.LC113
	.word	.LC114
	.word	.LC115
	.size	print_free_list, .-print_free_list
	.section	.text.add_block_invalid_list,"ax",%progbits
	.align	1
	.global	add_block_invalid_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	add_block_invalid_list, %function
add_block_invalid_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	adds	r0, r0, #188
	push	{r4, lr}
.L908:
	ldr	r3, [r0, #12]
	cbnz	r3, .L910
	str	r1, [r0, #12]
	str	r3, [r1, #12]
	b	.L912
.L910:
	ldrh	r2, [r3, #4]
	ldrh	r4, [r1, #4]
	cmp	r4, r2
	bcc	.L911
	str	r1, [r3, #16]
	str	r3, [r1, #12]
	str	r1, [r0, #12]
.L912:
	str	r0, [r1, #16]
	movs	r0, #0
	pop	{r4, pc}
.L911:
	mov	r0, r3
	b	.L908
	.size	add_block_invalid_list, .-add_block_invalid_list
	.section	.text.del_block_invalid_list,"ax",%progbits
	.align	1
	.global	del_block_invalid_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	del_block_invalid_list, %function
del_block_invalid_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	mov	r3, r0
	ldr	r0, [r0, #200]
	cbz	r0, .L913
	ldr	r2, [r0, #12]
	str	r2, [r3, #200]
	ldr	r2, [r0, #12]
	cbz	r2, .L913
	adds	r3, r3, #188
	str	r3, [r2, #16]
.L913:
	bx	lr
	.size	del_block_invalid_list, .-del_block_invalid_list
	.section	.text.del_block_invalid_list_by_block,"ax",%progbits
	.align	1
	.global	del_block_invalid_list_by_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	del_block_invalid_list_by_block, %function
del_block_invalid_list_by_block:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, [r0, #200]
.L921:
	cbnz	r0, .L924
	bx	lr
.L924:
	cmp	r0, r1
	ldr	r3, [r0, #12]
	bne	.L925
	ldr	r2, [r0, #16]
	str	r3, [r2, #12]
	ldr	r3, [r0, #12]
	cbz	r3, .L922
	str	r2, [r3, #16]
.L922:
	movs	r3, #0
	strd	r3, r3, [r0, #12]
	bx	lr
.L925:
	mov	r0, r3
	b	.L921
	.size	del_block_invalid_list_by_block, .-del_block_invalid_list_by_block
	.section	.text.is_block_in_invalid_list,"ax",%progbits
	.align	1
	.global	is_block_in_invalid_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_block_in_invalid_list, %function
is_block_in_invalid_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, [r0, #200]
.L930:
	cbnz	r0, .L932
	bx	lr
.L932:
	cmp	r0, r1
	beq	.L933
	ldr	r0, [r0, #12]
	b	.L930
.L933:
	movs	r0, #1
	bx	lr
	.size	is_block_in_invalid_list, .-is_block_in_invalid_list
	.section	.text.print_block_invalid_list,"ax",%progbits
	.align	1
	.global	print_block_invalid_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	print_block_invalid_list, %function
print_block_invalid_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #356]
	push	{r4, r5, r6, lr}
	mov	r4, r0
	cbz	r3, .L934
	ldr	r0, .L939
	bl	NAND_Print_DBG
	ldr	r5, .L939+4
	ldr	r4, [r4, #200]
	ldr	r6, .L939+8
.L936:
	cbnz	r4, .L937
.L934:
	pop	{r4, r5, r6, pc}
.L937:
	ldrh	r2, [r4, #6]
	mov	r0, r5
	ldrh	r1, [r4]
	bl	NAND_Print_DBG
	ldrh	r2, [r4, #4]
	mov	r0, r6
	ldr	r1, [r4, #8]
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #2]
	ldr	r0, .L939+12
	bl	NAND_Print_DBG
	ldr	r4, [r4, #12]
	b	.L936
.L940:
	.align	2
.L939:
	.word	.LC116
	.word	.LC113
	.word	.LC117
	.word	.LC115
	.size	print_block_invalid_list, .-print_block_invalid_list
	.section	.text.add_block_count_list,"ax",%progbits
	.align	1
	.global	add_block_count_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	add_block_count_list, %function
add_block_count_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	adds	r0, r0, #224
	push	{r4, lr}
.L942:
	ldr	r3, [r0, #28]
	cbnz	r3, .L944
	str	r1, [r0, #28]
	str	r3, [r1, #28]
	b	.L946
.L944:
	ldr	r4, [r3, #8]
	ldr	r2, [r1, #8]
	cmp	r4, r2
	blt	.L945
	str	r1, [r3, #32]
	str	r3, [r1, #28]
	str	r1, [r0, #28]
.L946:
	str	r0, [r1, #32]
	movs	r0, #0
	pop	{r4, pc}
.L945:
	mov	r0, r3
	b	.L942
	.size	add_block_count_list, .-add_block_count_list
	.section	.text.del_block_count_list,"ax",%progbits
	.align	1
	.global	del_block_count_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	del_block_count_list, %function
del_block_count_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	mov	r3, r0
	adds	r0, r0, #224
	ldr	r2, [r3, #252]
	cbz	r2, .L947
	ldr	r2, [r2, #28]
	str	r2, [r3, #252]
.L947:
	bx	lr
	.size	del_block_count_list, .-del_block_count_list
	.section	.text.del_block_count_by_block,"ax",%progbits
	.align	1
	.global	del_block_count_by_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	del_block_count_by_block, %function
del_block_count_by_block:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, [r0, #252]
.L953:
	cbnz	r0, .L956
	bx	lr
.L956:
	cmp	r0, r1
	ldr	r3, [r0, #28]
	bne	.L957
	ldr	r2, [r0, #32]
	str	r3, [r2, #28]
	ldr	r3, [r0, #28]
	cbz	r3, .L954
	str	r2, [r3, #32]
.L954:
	movs	r3, #0
	strd	r3, r3, [r0, #28]
	bx	lr
.L957:
	mov	r0, r3
	b	.L953
	.size	del_block_count_by_block, .-del_block_count_by_block
	.section	.text.print_block_count_list,"ax",%progbits
	.align	1
	.global	print_block_count_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	print_block_count_list, %function
print_block_count_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #356]
	push	{r4, r5, r6, lr}
	mov	r4, r0
	cbz	r3, .L961
	ldr	r0, .L966
	bl	NAND_Print_DBG
	ldr	r5, .L966+4
	ldr	r4, [r4, #252]
	ldr	r6, .L966+8
.L963:
	cbnz	r4, .L964
.L961:
	pop	{r4, r5, r6, pc}
.L964:
	ldrh	r1, [r4]
	mov	r0, r5
	bl	NAND_Print_DBG
	ldr	r1, [r4, #8]
	mov	r0, r6
	bl	NAND_Print_DBG
	ldr	r4, [r4, #28]
	b	.L963
.L967:
	.align	2
.L966:
	.word	.LC118
	.word	.LC119
	.word	.LC120
	.size	print_block_count_list, .-print_block_count_list
	.section	.text.put_phy_block_to_free_list,"ax",%progbits
	.align	1
	.global	put_phy_block_to_free_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	put_phy_block_to_free_list, %function
put_phy_block_to_free_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r2, #0
	push	{r3, r4, r5, lr}
	strd	r2, r2, [r1, #20]
	mov	r5, r0
	bl	add_free_list
	mov	r4, r0
	cbnz	r0, .L969
	ldrh	r3, [r5, #22]
	adds	r3, r3, #1
	strh	r3, [r5, #22]	@ movhi
.L968:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L969:
	ldr	r0, .L971
	bl	NAND_Print_DBG
	b	.L968
.L972:
	.align	2
.L971:
	.word	.LC121
	.size	put_phy_block_to_free_list, .-put_phy_block_to_free_list
	.section	.text.out_phy_block_from_free_list,"ax",%progbits
	.align	1
	.global	out_phy_block_from_free_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	out_phy_block_from_free_list, %function
out_phy_block_from_free_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	mov	r1, r0
	bl	del_free_list
	cbz	r0, .L973
	movs	r3, #0
	strd	r3, r3, [r0, #20]
	ldrh	r3, [r1, #22]
	cbz	r3, .L973
	subs	r3, r3, #1
	strh	r3, [r1, #22]	@ movhi
.L973:
	pop	{r3, pc}
	.size	out_phy_block_from_free_list, .-out_phy_block_from_free_list
	.section	.text.panic_backup_block,"ax",%progbits
	.align	1
	.global	panic_backup_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	panic_backup_block, %function
panic_backup_block:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L991
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r8, r0
	ldr	r0, [r3]
	cbz	r0, .L980
	ldr	r6, .L991+4
	movs	r5, #0
	ldr	fp, .L991+16
.L984:
	ldr	r4, [r6, r5, lsl #2]
	lsl	r10, r5, #2
	cbz	r4, .L982
	ldrh	r1, [r4]
	ldr	r0, .L991+8
	bl	NAND_Print
	movs	r0, #0
.L980:
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L982:
	mov	r0, r8
	bl	out_phy_block_from_free_list
	mov	r7, r0
	cbz	r0, .L983
	ldrh	r1, [r0]
	adds	r5, r5, #1
	mov	r0, fp
	bl	NAND_Print
	cmp	r5, #3
	str	r7, [r10, r6]
	bne	.L984
	ldr	r3, [r8, #4]
	mov	r0, r4
	ldrh	r3, [r3, #20]
	add	r3, r3, r3, lsl #1
	str	r3, [r6, #12]
	b	.L980
.L983:
	ldr	r0, .L991+12
	bl	NAND_Print
	mov	r0, #-1
	b	.L980
.L992:
	.align	2
.L991:
	.word	.LANCHOR86
	.word	.LANCHOR88
	.word	.LC122
	.word	.LC124
	.word	.LC123
	.size	panic_backup_block, .-panic_backup_block
	.section	.text.get_max_erase_time_from_free_list,"ax",%progbits
	.align	1
	.global	get_max_erase_time_from_free_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_max_erase_time_from_free_list, %function
get_max_erase_time_from_free_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #172]
	movs	r0, #0
	mov	r2, r0
	push	{r4, lr}
.L994:
	cbnz	r3, .L996
	cbz	r2, .L998
	ldrh	r0, [r2, #6]
.L997:
	pop	{r4, pc}
.L996:
	ldrh	r4, [r3, #6]
	cmp	r4, r1
	bcs	.L995
	cmp	r4, r0
	itt	cs
	movcs	r0, r4
	movcs	r2, r3
.L995:
	ldr	r3, [r3, #20]
	b	.L994
.L998:
	movw	r0, #65535
	b	.L997
	.size	get_max_erase_time_from_free_list, .-get_max_erase_time_from_free_list
	.section	.text.get_block_max_erase_time_from_free_list,"ax",%progbits
	.align	1
	.global	get_block_max_erase_time_from_free_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_block_max_erase_time_from_free_list, %function
get_block_max_erase_time_from_free_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r2, #0
	ldr	r3, [r0, #172]
	mov	r0, r2
	push	{r4, lr}
.L1000:
	cbnz	r3, .L1002
	pop	{r4, pc}
.L1002:
	ldrh	r4, [r3, #6]
	cmp	r4, r1
	bcs	.L1001
	cmp	r4, r2
	itt	cs
	movcs	r2, r4
	movcs	r0, r3
.L1001:
	ldr	r3, [r3, #20]
	b	.L1000
	.size	get_block_max_erase_time_from_free_list, .-get_block_max_erase_time_from_free_list
	.section	.text.get_block_max_erase_time_from_invalid_list,"ax",%progbits
	.align	1
	.global	get_block_max_erase_time_from_invalid_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_block_max_erase_time_from_invalid_list, %function
get_block_max_erase_time_from_invalid_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r2, #0
	ldr	r3, [r0, #200]
	mov	r0, r2
	push	{r4, lr}
.L1004:
	cbnz	r3, .L1006
	pop	{r4, pc}
.L1006:
	ldrh	r4, [r3, #6]
	cmp	r4, r1
	bcs	.L1005
	cmp	r4, r2
	itt	cs
	movcs	r2, r4
	movcs	r0, r3
.L1005:
	ldr	r3, [r3, #12]
	b	.L1004
	.size	get_block_max_erase_time_from_invalid_list, .-get_block_max_erase_time_from_invalid_list
	.section	.text.get_block_min_erase_time_from_invalid_list,"ax",%progbits
	.align	1
	.global	get_block_min_erase_time_from_invalid_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_block_min_erase_time_from_invalid_list, %function
get_block_min_erase_time_from_invalid_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #4]
	push	{r4, r5, r6, lr}
	ldrh	r5, [r3, #16]
	ldr	r3, [r0, #200]
	movs	r0, #0
.L1008:
	cbnz	r3, .L1010
	pop	{r4, r5, r6, pc}
.L1010:
	ldrh	r4, [r3, #6]
	cmp	r4, r1
	bcs	.L1009
	ldr	r6, [r3, #8]
	cmp	r6, r2
	bcs	.L1009
	cmp	r4, r5
	itt	ls
	movls	r5, r4
	movls	r0, r3
.L1009:
	ldr	r3, [r3, #12]
	b	.L1008
	.size	get_block_min_erase_time_from_invalid_list, .-get_block_min_erase_time_from_invalid_list
	.section	.text.get_block_max_count_from_invalid_list,"ax",%progbits
	.align	1
	.global	get_block_max_count_from_invalid_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_block_max_count_from_invalid_list, %function
get_block_max_count_from_invalid_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r2, #0
	ldr	r3, [r0, #200]
	mov	r0, r2
.L1012:
	cbnz	r3, .L1014
	bx	lr
.L1014:
	ldr	r1, [r3, #8]
	cmp	r1, r2
	itt	cs
	movcs	r0, r3
	movcs	r2, r1
	ldr	r3, [r3, #12]
	b	.L1012
	.size	get_block_max_count_from_invalid_list, .-get_block_max_count_from_invalid_list
	.section	.text.get_block_min_count_from_invalid_list,"ax",%progbits
	.align	1
	.global	get_block_min_count_from_invalid_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_block_min_count_from_invalid_list, %function
get_block_min_count_from_invalid_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, [r0, #200]
	mov	r2, #-1
	movs	r0, #0
.L1016:
	cbnz	r3, .L1018
	bx	lr
.L1018:
	ldr	r1, [r3, #8]
	cmp	r1, r2
	itt	ls
	movls	r0, r3
	movls	r2, r1
	ldr	r3, [r3, #12]
	b	.L1016
	.size	get_block_min_count_from_invalid_list, .-get_block_min_count_from_invalid_list
	.section	.text.out_phy_block_from_free_list_by_block,"ax",%progbits
	.align	1
	.global	out_phy_block_from_free_list_by_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	out_phy_block_from_free_list_by_block, %function
out_phy_block_from_free_list_by_block:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	bl	del_free_list_by_block
	cbz	r0, .L1019
	movs	r3, #0
	strd	r3, r3, [r0, #20]
	ldrh	r3, [r4, #22]
	cbz	r3, .L1019
	subs	r3, r3, #1
	strh	r3, [r4, #22]	@ movhi
.L1019:
	pop	{r4, pc}
	.size	out_phy_block_from_free_list_by_block, .-out_phy_block_from_free_list_by_block
	.section	.text.out_phy_block_from_free_list_by_erase_time,"ax",%progbits
	.align	1
	.global	out_phy_block_from_free_list_by_erase_time
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	out_phy_block_from_free_list_by_erase_time, %function
out_phy_block_from_free_list_by_erase_time:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, [r0, #172]
	push	{r4, r5}
	movs	r4, #0
	mov	r2, r4
.L1027:
	cbnz	r3, .L1029
	cbz	r2, .L1026
	mov	r1, r2
	pop	{r4, r5}
	b	out_phy_block_from_free_list_by_block
.L1029:
	ldrh	r5, [r3, #6]
	cmp	r5, r1
	bcs	.L1028
	cmp	r5, r4
	itt	cs
	movcs	r4, r5
	movcs	r2, r3
.L1028:
	ldr	r3, [r3, #20]
	b	.L1027
.L1026:
	mov	r0, r2
	pop	{r4, r5}
	bx	lr
	.size	out_phy_block_from_free_list_by_erase_time, .-out_phy_block_from_free_list_by_erase_time
	.section	.text.put_phy_block_to_invalid_page_list,"ax",%progbits
	.align	1
	.global	put_phy_block_to_invalid_page_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	put_phy_block_to_invalid_page_list, %function
put_phy_block_to_invalid_page_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r2, #0
	push	{r4, lr}
	strd	r2, r2, [r1, #12]
	bl	add_block_invalid_list
	mov	r4, r0
	cbz	r0, .L1031
	ldr	r0, .L1036
	bl	NAND_Print_DBG
.L1031:
	mov	r0, r4
	pop	{r4, pc}
.L1037:
	.align	2
.L1036:
	.word	.LC125
	.size	put_phy_block_to_invalid_page_list, .-put_phy_block_to_invalid_page_list
	.section	.text.out_phy_block_from_invalid_page_list,"ax",%progbits
	.align	1
	.global	out_phy_block_from_invalid_page_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	out_phy_block_from_invalid_page_list, %function
out_phy_block_from_invalid_page_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	bl	del_block_invalid_list
	cbz	r0, .L1038
	movs	r3, #0
	strd	r3, r3, [r0, #12]
.L1038:
	pop	{r3, pc}
	.size	out_phy_block_from_invalid_page_list, .-out_phy_block_from_invalid_page_list
	.section	.text.out_phy_block_from_invalid_page_list_by_block,"ax",%progbits
	.align	1
	.global	out_phy_block_from_invalid_page_list_by_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	out_phy_block_from_invalid_page_list_by_block, %function
out_phy_block_from_invalid_page_list_by_block:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	bl	del_block_invalid_list_by_block
	cbz	r0, .L1043
	movs	r3, #0
	strd	r3, r3, [r0, #12]
.L1043:
	pop	{r3, pc}
	.size	out_phy_block_from_invalid_page_list_by_block, .-out_phy_block_from_invalid_page_list_by_block
	.section	.text.out_phy_block_from_used_list_by_block,"ax",%progbits
	.align	1
	.global	out_phy_block_from_used_list_by_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	out_phy_block_from_used_list_by_block, %function
out_phy_block_from_used_list_by_block:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	b	del_block_count_by_block
	.size	out_phy_block_from_used_list_by_block, .-out_phy_block_from_used_list_by_block
	.section	.text.phy_block_from_invalid_page_incr,"ax",%progbits
	.align	1
	.global	phy_block_from_invalid_page_incr
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	phy_block_from_invalid_page_incr, %function
phy_block_from_invalid_page_incr:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r4, r1
	ldrh	r2, [r1, #4]
	mov	r5, r0
	ldr	r1, [r1, #12]
	ldr	r3, [r0, #4]
	adds	r2, r2, #1
	uxth	r2, r2
	ldrh	r3, [r3, #20]
	cbnz	r1, .L1050
	ldr	r1, [r4, #16]
	cbnz	r1, .L1050
	cmp	r2, r3
	strh	r2, [r4, #4]	@ movhi
	bls	.L1052
	ldrh	r1, [r4]
	ldr	r0, .L1064
	bl	NAND_Print_DBG
.L1052:
	movs	r0, #0
	pop	{r3, r4, r5, pc}
.L1050:
	cmp	r2, r3
	strh	r2, [r4, #4]	@ movhi
	bls	.L1053
	ldr	r0, .L1064+4
	bl	NAND_Print_DBG
.L1053:
	ldr	r3, [r4, #16]
	adds	r5, r5, #188
	cmp	r3, r5
	beq	.L1052
	ldrh	r1, [r4, #4]
.L1055:
	ldrh	r2, [r3, #4]
	cmp	r2, r1
	bcs	.L1052
	ldr	r2, [r3, #16]
	str	r2, [r4, #16]
	ldr	r2, [r3, #16]
	str	r4, [r2, #12]
	ldr	r2, [r4, #12]
	str	r2, [r3, #12]
	ldr	r2, [r4, #12]
	cbz	r2, .L1056
	str	r3, [r2, #16]
.L1056:
	str	r3, [r4, #12]
	str	r4, [r3, #16]
	ldr	r3, [r4, #16]
	ldr	r2, [r3, #16]
	cmp	r2, #0
	bne	.L1055
	b	.L1052
.L1065:
	.align	2
.L1064:
	.word	.LC126
	.word	.LC127
	.size	phy_block_from_invalid_page_incr, .-phy_block_from_invalid_page_incr
	.section	.text.adjust_invaild_list,"ax",%progbits
	.align	1
	.global	adjust_invaild_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	adjust_invaild_list, %function
adjust_invaild_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r4, r0
.L1067:
	ldr	r3, [r4, #200]
	cbnz	r3, .L1069
.L1068:
	movs	r0, #0
	pop	{r3, r4, r5, pc}
.L1069:
	ldr	r1, [r4, #4]
	ldrh	r2, [r3, #4]
	ldrh	r3, [r1, #20]
	cmp	r2, r3
	bne	.L1068
	mov	r0, r4
	bl	out_phy_block_from_invalid_page_list
	mov	r5, r0
	mov	r1, r0
	mov	r0, r4
	bl	put_phy_block_to_free_list
	mov	r1, r5
	mov	r0, r4
	bl	out_phy_block_from_used_list_by_block
	b	.L1067
	.size	adjust_invaild_list, .-adjust_invaild_list
	.section	.text.dump_buf,"ax",%progbits
	.align	1
	.global	dump_buf
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	dump_buf, %function
dump_buf:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	movs	r4, #0
	ldr	r7, .L1075
	mov	r6, r0
	mov	r5, r1
	ldr	r8, .L1075+8
	ldr	r0, .L1075+4
	bl	NAND_Print
.L1071:
	cmp	r4, r5
	bne	.L1074
	ldr	r0, .L1075+4
	pop	{r4, r5, r6, r7, r8, lr}
	b	NAND_Print
.L1074:
	lsls	r2, r4, #28
	bne	.L1072
	mov	r1, r4
	mov	r0, r7
	bl	NAND_Print
.L1072:
	ldrb	r1, [r6, r4]	@ zero_extendqisi2
	mov	r0, r8
	adds	r4, r4, #1
	bl	NAND_Print
	lsls	r3, r4, #28
	bne	.L1071
	ldr	r0, .L1075+4
	bl	NAND_Print
	b	.L1071
.L1076:
	.align	2
.L1075:
	.word	.LC5
	.word	.LC7
	.word	.LC81
	.size	dump_buf, .-dump_buf
	.global	__aeabi_uidiv
	.section	.text.init_zone_after_first_scan,"ax",%progbits
	.align	1
	.global	init_zone_after_first_scan
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	init_zone_after_first_scan, %function
init_zone_after_first_scan:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r5, r1
	ldr	r6, [r0, #4]
	mov	r4, r0
	ldr	r0, [r0, #24]
	ldrb	r1, [r6, #8]	@ zero_extendqisi2
	bl	__aeabi_uidiv
	ldrh	r1, [r6, #20]
	bl	__aeabi_uidiv
	mov	r1, r5
	mov	r6, r0
	mov	r2, r0
	ldr	r0, .L1084
	bl	NAND_Print_DBG
	cmp	r6, r5
	bcc	.L1078
	ldr	r0, .L1084+4
	mov	r2, r6
	mov	r1, r5
.L1083:
	bl	NAND_Print
	movs	r0, #1
.L1077:
	pop	{r4, r5, r6, pc}
.L1078:
	ldr	r1, [r4, #4]
	subs	r2, r5, r6
	ldrb	r0, [r1, #8]	@ zero_extendqisi2
	ldrh	r3, [r1, #20]
	muls	r3, r0, r3
	muls	r3, r2, r3
	str	r3, [r4, #32]
	ldr	r3, [r4, #1972]
	ldrh	r1, [r3, #12]
	subs	r1, r1, #1
	cmp	r2, r1
	bcs	.L1080
	mov	r2, r6
	mov	r1, r5
	ldr	r0, .L1084+8
	b	.L1083
.L1080:
	ldr	r0, .L1084+12
	umull	r0, r1, r2, r0
	ubfx	r0, r1, #2, #16
	ldrh	r1, [r3, #14]
	strh	r0, [r4, #380]	@ movhi
	cmp	r0, r1
	ldrh	r0, [r3, #18]
	it	cc
	strhcc	r1, [r4, #380]	@ movhi
	ldrh	r1, [r3, #20]
	muls	r0, r2, r0
	bl	__aeabi_uidiv
	ldrh	r3, [r4, #380]
	strh	r0, [r4, #382]	@ movhi
	uxth	r0, r0
	adds	r2, r3, #1
	cmp	r2, r0
	mov	r0, #0
	itt	ge
	addge	r3, r3, #2
	strhge	r3, [r4, #382]	@ movhi
	movs	r3, #1
	str	r3, [r4, #384]
	b	.L1077
.L1085:
	.align	2
.L1084:
	.word	.LC128
	.word	.LC129
	.word	.LC130
	.word	-858993459
	.size	init_zone_after_first_scan, .-init_zone_after_first_scan
	.section	.text.check_bit_nums,"ax",%progbits
	.align	1
	.global	check_bit_nums
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	check_bit_nums, %function
check_bit_nums:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	movs	r2, #0
	mov	r4, r0
.L1087:
	subs	r3, r4, r0
	cmp	r3, r1
	blt	.L1090
	mov	r0, r2
	pop	{r4, r5, r6, pc}
.L1090:
	ldrb	r6, [r4], #1	@ zero_extendqisi2
	movs	r5, #8
	movs	r3, #1
.L1089:
	tst	r6, r3
	lsl	r3, r3, #1
	it	ne
	addne	r2, r2, #1
	subs	r5, r5, #1
	uxtb	r3, r3
	bne	.L1089
	b	.L1087
	.size	check_bit_nums, .-check_bit_nums
	.section	.text.is_all_bit_ff,"ax",%progbits
	.align	1
	.global	is_all_bit_ff
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_all_bit_ff, %function
is_all_bit_ff:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	rsb	r4, r2, r1, lsl #3
	bl	check_bit_nums
	cmp	r4, r0
	ite	gt
	movgt	r0, #0
	movle	r0, #1
	pop	{r4, pc}
	.size	is_all_bit_ff, .-is_all_bit_ff
	.section	.text.is_all_byte_ff,"ax",%progbits
	.align	1
	.global	is_all_byte_ff
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_all_byte_ff, %function
is_all_byte_ff:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, lr}
	subs	r2, r1, r2
	mov	r4, r0
	movs	r3, #0
.L1097:
	subs	r5, r4, r0
	cmp	r1, r5
	bgt	.L1099
	cmp	r3, r2
	ite	lt
	movlt	r0, #0
	movge	r0, #1
	pop	{r4, r5, pc}
.L1099:
	ldrb	r5, [r4], #1	@ zero_extendqisi2
	cmp	r5, #255
	it	eq
	addeq	r3, r3, #1
	b	.L1097
	.size	is_all_byte_ff, .-is_all_byte_ff
	.section	.text.do_write_error_in_build_list,"ax",%progbits
	.align	1
	.global	do_write_error_in_build_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	do_write_error_in_build_list, %function
do_write_error_in_build_list:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r8, r3
	ldr	r3, [r0, #4]
	mov	r7, r2
	sub	sp, sp, #24
	movs	r5, #0
	ldrh	r2, [r2]
	mov	r10, r1
	ldrh	r3, [r3, #28]
	add	r1, sp, #8
	strh	r5, [sp, #8]	@ movhi
	mov	r4, r0
	strh	r2, [sp, #10]	@ movhi
	strh	r3, [sp, #12]	@ movhi
	strd	r5, r5, [sp, #16]
	ldr	r3, [r0, #1980]
	blx	r3
	ldrh	r3, [r7, #6]
	strh	r5, [r7, #4]	@ movhi
	adds	r3, r3, #1
	strh	r3, [r7, #6]	@ movhi
	cmp	r0, #0
	beq	.L1105
	ldr	r0, .L1110
	bl	NAND_Print
	ldr	r3, [r4, #2000]
	add	r1, sp, #8
	mov	r0, r4
	blx	r3
	movs	r0, #1
.L1100:
	add	sp, sp, #24
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1104:
	uxth	r3, r6
	mov	r2, r7
	mov	r1, r10
	ldr	r5, [r4, #1992]
	str	r3, [sp]
	mov	r0, r4
	ldr	r3, [r4, #284]
	blx	r5
	cbz	r0, .L1103
	mov	r0, fp
	bl	NAND_Print
.L1103:
	adds	r6, r6, #1
.L1101:
	cmp	r8, r6
	bhi	.L1104
	ldr	r3, [r4, #4]
	movs	r5, #0
	ldrh	r2, [r10]
	add	r1, sp, #8
	strh	r5, [sp, #8]	@ movhi
	mov	r0, r4
	ldrh	r3, [r3, #28]
	strh	r2, [sp, #10]	@ movhi
	strd	r5, r5, [sp, #16]
	strh	r3, [sp, #12]	@ movhi
	ldr	r3, [r4, #2000]
	blx	r3
	cmp	r8, #0
	bne	.L1106
	str	r7, [r4, #292]
	mov	r0, r8
	strh	r8, [r4, #320]	@ movhi
	b	.L1100
.L1105:
	mov	r6, r0
	ldr	fp, .L1110+4
	b	.L1101
.L1106:
	mov	r0, r5
	b	.L1100
.L1111:
	.align	2
.L1110:
	.word	.LC131
	.word	.LC132
	.size	do_write_error_in_build_list, .-do_write_error_in_build_list
	.section	.text.init_nrc,"ax",%progbits
	.align	1
	.global	init_nrc
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	init_nrc, %function
init_nrc:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	mov	r5, r0
	movs	r0, #16
	mov	r6, r1
	bl	NAND_Malloc
	mov	r4, r0
	cbnz	r0, .L1113
	mov	r2, #1400
	ldr	r1, .L1133
	ldr	r0, .L1133+4
	bl	NAND_Print
.L1132:
	mov	r0, #-1
	b	.L1112
.L1113:
	movs	r2, #16
	movs	r1, #0
	bl	NAND_Memset
	ldr	r3, [r5, #4]
	ldrh	r5, [r3, #20]
	ldrh	r0, [r3, #26]
	adds	r0, r0, #64
	muls	r0, r5, r0
	bl	nftl_malloc
	str	r0, [r4, #8]
	cbnz	r0, .L1115
	ldr	r1, .L1133
	movw	r2, #1409
	ldr	r0, .L1133+8
.L1131:
.L1116:
	bl	NAND_Print
	ldr	r0, [r4, #8]
	cbz	r0, .L1119
	bl	nftl_free
.L1119:
	ldr	r0, [r4, #12]
	cbz	r0, .L1120
	bl	nftl_free
.L1120:
	ldr	r0, [r4, #4]
	cbz	r0, .L1121
	bl	nftl_free
.L1121:
	mov	r0, r4
	bl	nftl_free
	movs	r3, #0
	str	r3, [r6]
	b	.L1132
.L1115:
	lsls	r7, r5, #2
	mov	r0, r7
	bl	nftl_malloc
	str	r0, [r4, #12]
	cbnz	r0, .L1117
	movw	r2, #1414
	ldr	r1, .L1133
	ldr	r0, .L1133+12
	b	.L1131
.L1117:
	mov	r2, r7
	movs	r1, #255
	bl	NAND_Memset
	mov	r0, r5
	bl	nftl_malloc
	str	r0, [r4, #4]
	cbnz	r0, .L1118
	movw	r2, #1420
	ldr	r1, .L1133
	ldr	r0, .L1133+16
	b	.L1131
.L1118:
	mov	r2, r5
	movs	r1, #0
	bl	NAND_Memset
	movs	r0, #0
	str	r4, [r6]
.L1112:
	pop	{r3, r4, r5, r6, r7, pc}
.L1134:
	.align	2
.L1133:
	.word	.LANCHOR89
	.word	.LC133
	.word	.LC134
	.word	.LC135
	.word	.LC136
	.size	init_nrc, .-init_nrc
	.section	.text.free_nrc,"ax",%progbits
	.align	1
	.global	free_nrc
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	free_nrc, %function
free_nrc:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	cbz	r0, .L1136
	ldr	r0, [r0, #8]
	bl	nftl_free
	ldr	r0, [r4, #4]
	bl	nftl_free
	ldr	r0, [r4, #12]
	bl	nftl_free
	mov	r0, r4
	bl	nftl_free
.L1136:
	movs	r0, #0
	pop	{r4, pc}
	.size	free_nrc, .-free_nrc
	.section	.text.nftl_check_pattern,"ax",%progbits
	.align	1
	.global	nftl_check_pattern
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_check_pattern, %function
nftl_check_pattern:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	mov	r3, r0
	push	{r4, lr}
.L1141:
	subs	r4, r3, r0
	cmp	r2, r4
	bgt	.L1143
	movs	r0, #0
.L1140:
	pop	{r4, pc}
.L1143:
	ldrb	r4, [r3], #1	@ zero_extendqisi2
	cmp	r4, r1
	beq	.L1141
	movs	r0, #1
	b	.L1140
	.size	nftl_check_pattern, .-nftl_check_pattern
	.section	.text.is_last_phy_block,"ax",%progbits
	.align	1
	.global	is_last_phy_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_last_phy_block, %function
is_last_phy_block:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, [r1, #4]
	ldrh	r0, [r0]
	ldrh	r3, [r3, #24]
	subs	r3, r3, #1
	subs	r3, r3, r0
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	bx	lr
	.size	is_last_phy_block, .-is_last_phy_block
	.section	.text.get_vaild_blocks,"ax",%progbits
	.align	1
	.global	get_vaild_blocks
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_vaild_blocks, %function
get_vaild_blocks:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r3, r4, r5, r6, r7, r8, lr}
	movs	r4, #36
	ldr	r3, [r0, #4]
	sub	r8, r2, r1
	mov	r7, r0
	mov	r5, r1
	mov	r6, r8
	ldr	r3, [r3, #32]
	mla	r4, r4, r1, r3
.L1147:
	cmp	r5, r8
	bcc	.L1150
.L1146:
	mov	r0, r6
	add	sp, sp, #16
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L1150:
	ldrh	r3, [r4]
	mov	r1, sp
	mov	r0, r7
	strh	r3, [sp, #2]	@ movhi
	ldr	r3, [r7, #1996]
	blx	r3
	cmp	r0, #159
	bne	.L1148
	subs	r6, r6, #1
	beq	.L1146
.L1148:
	mov	r1, r7
	mov	r0, r4
	bl	is_last_phy_block
	cmp	r0, #1
	beq	.L1146
	adds	r4, r4, #36
	adds	r5, r5, #1
	b	.L1147
	.size	get_vaild_blocks, .-get_vaild_blocks
	.section	.text.is_phy_block_valid,"ax",%progbits
	.align	1
	.global	is_phy_block_valid
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_phy_block_valid, %function
is_phy_block_valid:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, [r1, #4]
	ldrh	r0, [r0]
	ldrh	r3, [r3, #24]
	cmp	r0, r3
	ite	cs
	movcs	r0, #0
	movcc	r0, #1
	bx	lr
	.size	is_phy_block_valid, .-is_phy_block_valid
	.section	.text.get_phy_block_addr,"ax",%progbits
	.align	1
	.global	get_phy_block_addr
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_phy_block_addr, %function
get_phy_block_addr:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, [r0, #4]
	movs	r0, #36
	ldr	r3, [r3, #32]
	mla	r0, r0, r1, r3
	bx	lr
	.size	get_phy_block_addr, .-get_phy_block_addr
	.section	.text.check_cross_talk,"ax",%progbits
	.align	1
	.global	check_cross_talk
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	check_cross_talk, %function
check_cross_talk:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, lr}
	movs	r4, #0
	sub	sp, sp, #36
	mov	r5, r0
	mov	r6, r1
	mov	r7, r4
.L1160:
	ldr	r3, [r5, #4]
	ldrh	r2, [r3, #20]
	cmp	r2, r4
	bls	.L1162
	ldrh	r3, [r3, #28]
	add	r1, sp, #16
	ldrh	r2, [r6]
	mov	r0, r5
	strh	r4, [sp, #16]	@ movhi
	strh	r3, [sp, #20]	@ movhi
	strh	r2, [sp, #18]	@ movhi
	str	r7, [sp, #24]
	str	sp, [sp, #28]
	ldr	r3, [r5, #1984]
	blx	r3
	cbnz	r0, .L1161
	ldrb	r3, [sp, #14]	@ zero_extendqisi2
	cmp	r3, #99
	bhi	.L1161
	ldr	r2, [r5, #1968]
	ldr	r1, [r2, #168]
	cmp	r1, r3
	bhi	.L1161
	str	r3, [r2, #168]
.L1162:
	movs	r0, #0
	add	sp, sp, #36
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L1161:
	adds	r4, r4, #1
	b	.L1160
	.size	check_cross_talk, .-check_cross_talk
	.section	.text.nftl_get_bad_block_cnt,"ax",%progbits
	.align	1
	.global	nftl_get_bad_block_cnt
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_get_bad_block_cnt, %function
nftl_get_bad_block_cnt:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, [r0, #12]
	ldrh	r0, [r3, #20]
	bx	lr
	.size	nftl_get_bad_block_cnt, .-nftl_get_bad_block_cnt
	.section	.text.nand_dbg_phy_read,"ax",%progbits
	.align	1
	.global	nand_dbg_phy_read
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_dbg_phy_read, %function
nand_dbg_phy_read:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, lr}
	mov	r5, r0
	sub	sp, sp, #76
	mov	r0, #4096
	mov	r6, r1
	mov	r7, r2
	bl	NAND_Malloc
	mov	r4, r0
	cbnz	r0, .L1166
	ldr	r1, .L1167
	ldr	r0, .L1167+4
	bl	NAND_Print
.L1166:
	add	r3, sp, #56
	mov	r2, r7
	strd	r4, r3, [sp]
	mov	r1, r6
	movs	r3, #8
	mov	r0, r5
	bl	page_read
	mov	r5, r0
	ldrb	r0, [sp, #71]	@ zero_extendqisi2
	ldrb	r3, [sp, #58]	@ zero_extendqisi2
	ldrb	r2, [sp, #57]	@ zero_extendqisi2
	str	r0, [sp, #48]
	ldrb	r0, [sp, #70]	@ zero_extendqisi2
	ldrb	r1, [sp, #56]	@ zero_extendqisi2
	str	r0, [sp, #44]
	ldrb	r0, [sp, #69]	@ zero_extendqisi2
	str	r0, [sp, #40]
	ldrb	r0, [sp, #68]	@ zero_extendqisi2
	str	r0, [sp, #36]
	ldrb	r0, [sp, #67]	@ zero_extendqisi2
	str	r0, [sp, #32]
	ldrb	r0, [sp, #66]	@ zero_extendqisi2
	str	r0, [sp, #28]
	ldrb	r0, [sp, #65]	@ zero_extendqisi2
	str	r0, [sp, #24]
	ldrb	r0, [sp, #64]	@ zero_extendqisi2
	str	r0, [sp, #20]
	ldrb	r0, [sp, #63]	@ zero_extendqisi2
	str	r0, [sp, #16]
	ldrb	r0, [sp, #62]	@ zero_extendqisi2
	str	r0, [sp, #12]
	ldrb	r0, [sp, #61]	@ zero_extendqisi2
	str	r0, [sp, #8]
	ldrb	r0, [sp, #60]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [sp, #59]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L1167+8
	bl	NAND_Print_DBG
	ldrb	r0, [r4, #10]	@ zero_extendqisi2
	ldrb	r3, [r4, #2]	@ zero_extendqisi2
	ldrb	r2, [r4, #1]	@ zero_extendqisi2
	ldrb	r1, [r4]	@ zero_extendqisi2
	str	r0, [sp, #28]
	ldrb	r0, [r4, #9]	@ zero_extendqisi2
	str	r0, [sp, #24]
	ldrb	r0, [r4, #8]	@ zero_extendqisi2
	str	r0, [sp, #20]
	ldrb	r0, [r4, #7]	@ zero_extendqisi2
	str	r0, [sp, #16]
	ldrb	r0, [r4, #6]	@ zero_extendqisi2
	str	r0, [sp, #12]
	ldrb	r0, [r4, #5]	@ zero_extendqisi2
	str	r0, [sp, #8]
	ldrb	r0, [r4, #4]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [r4, #3]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L1167+12
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	nftl_free
	mov	r0, r5
	add	sp, sp, #76
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L1168:
	.align	2
.L1167:
	.word	.LANCHOR90
	.word	.LC137
	.word	.LC138
	.word	.LC139
	.size	nand_dbg_phy_read, .-nand_dbg_phy_read
	.section	.text.nand_dbg_zone_phy_read,"ax",%progbits
	.align	1
	.global	nand_dbg_zone_phy_read
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_dbg_zone_phy_read, %function
nand_dbg_zone_phy_read:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, lr}
	mov	r5, r0
	sub	sp, sp, #92
	mov	r0, #4096
	mov	r7, r1
	mov	r6, r2
	bl	NAND_Malloc
	mov	r4, r0
	cbnz	r0, .L1170
	ldr	r1, .L1171
	ldr	r0, .L1171+4
	bl	NAND_Print
.L1170:
	ldr	r3, [r5, #4]
	movs	r1, #36
	muls	r1, r7, r1
	mov	r0, r5
	ldr	r3, [r3, #32]
	ldrh	r3, [r3, r1]
	add	r1, sp, #56
	strh	r6, [sp, #56]	@ movhi
	str	r4, [sp, #64]
	strh	r3, [sp, #58]	@ movhi
	movs	r3, #8
	strh	r3, [sp, #60]	@ movhi
	add	r3, sp, #72
	str	r3, [sp, #68]
	ldr	r3, [r5, #1984]
	blx	r3
	mov	r5, r0
	ldrb	r0, [sp, #87]	@ zero_extendqisi2
	ldrb	r3, [sp, #74]	@ zero_extendqisi2
	ldrb	r2, [sp, #73]	@ zero_extendqisi2
	str	r0, [sp, #48]
	ldrb	r0, [sp, #86]	@ zero_extendqisi2
	ldrb	r1, [sp, #72]	@ zero_extendqisi2
	str	r0, [sp, #44]
	ldrb	r0, [sp, #85]	@ zero_extendqisi2
	str	r0, [sp, #40]
	ldrb	r0, [sp, #84]	@ zero_extendqisi2
	str	r0, [sp, #36]
	ldrb	r0, [sp, #83]	@ zero_extendqisi2
	str	r0, [sp, #32]
	ldrb	r0, [sp, #82]	@ zero_extendqisi2
	str	r0, [sp, #28]
	ldrb	r0, [sp, #81]	@ zero_extendqisi2
	str	r0, [sp, #24]
	ldrb	r0, [sp, #80]	@ zero_extendqisi2
	str	r0, [sp, #20]
	ldrb	r0, [sp, #79]	@ zero_extendqisi2
	str	r0, [sp, #16]
	ldrb	r0, [sp, #78]	@ zero_extendqisi2
	str	r0, [sp, #12]
	ldrb	r0, [sp, #77]	@ zero_extendqisi2
	str	r0, [sp, #8]
	ldrb	r0, [sp, #76]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [sp, #75]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L1171+8
	bl	NAND_Print_DBG
	ldrb	r0, [r4, #10]	@ zero_extendqisi2
	ldrb	r3, [r4, #2]	@ zero_extendqisi2
	ldrb	r2, [r4, #1]	@ zero_extendqisi2
	ldrb	r1, [r4]	@ zero_extendqisi2
	str	r0, [sp, #28]
	ldrb	r0, [r4, #9]	@ zero_extendqisi2
	str	r0, [sp, #24]
	ldrb	r0, [r4, #8]	@ zero_extendqisi2
	str	r0, [sp, #20]
	ldrb	r0, [r4, #7]	@ zero_extendqisi2
	str	r0, [sp, #16]
	ldrb	r0, [r4, #6]	@ zero_extendqisi2
	str	r0, [sp, #12]
	ldrb	r0, [r4, #5]	@ zero_extendqisi2
	str	r0, [sp, #8]
	ldrb	r0, [r4, #4]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [r4, #3]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L1171+12
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	nftl_free
	mov	r0, r5
	add	sp, sp, #92
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L1172:
	.align	2
.L1171:
	.word	.LANCHOR91
	.word	.LC137
	.word	.LC138
	.word	.LC139
	.size	nand_dbg_zone_phy_read, .-nand_dbg_zone_phy_read
	.section	.text.nand_dbg_zone_phy_write,"ax",%progbits
	.align	1
	.global	nand_dbg_zone_phy_write
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_dbg_zone_phy_write, %function
nand_dbg_zone_phy_write:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	mov	r5, r0
	sub	sp, sp, #32
	mov	r0, #8192
	mov	r6, r1
	mov	r7, r2
	bl	NAND_Malloc
	mov	r4, r0
	cbnz	r0, .L1174
	ldr	r1, .L1175
	ldr	r0, .L1175+4
	bl	NAND_Print
.L1174:
	ldr	r3, [r5, #4]
	movs	r2, #36
	muls	r6, r2, r6
	mov	r0, r4
	mov	r2, #4096
	movs	r1, #165
	ldr	r8, [r3, #32]
	bl	NAND_Memset
	ldr	r2, .L1175+8
	mov	r3, #-1
	strh	r3, [sp, #24]	@ movhi
	mov	r1, sp
	mov	r0, r5
	strd	r3, r2, [sp, #16]
	movw	r3, #65535
	strb	r3, [sp, #26]
	ldr	r3, [r5, #4]
	ldrh	r2, [r8, r6]
	strh	r7, [sp]	@ movhi
	ldrh	r3, [r3, #28]
	strh	r2, [sp, #2]	@ movhi
	str	r4, [sp, #8]
	strh	r3, [sp, #4]	@ movhi
	add	r3, sp, #16
	str	r3, [sp, #12]
	ldr	r3, [r5, #1988]
	blx	r3
	mov	r5, r0
	mov	r0, r4
	bl	nftl_free
	mov	r0, r5
	add	sp, sp, #32
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L1176:
	.align	2
.L1175:
	.word	.LANCHOR92
	.word	.LC140
	.word	-5921281
	.size	nand_dbg_zone_phy_write, .-nand_dbg_zone_phy_write
	.section	.text.nand_dbg_phy_write,"ax",%progbits
	.align	1
	.global	nand_dbg_phy_write
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_dbg_phy_write, %function
nand_dbg_phy_write:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, lr}
	mov	r5, r0
	sub	sp, sp, #28
	mov	r0, #8192
	mov	r6, r1
	mov	r7, r2
	bl	NAND_Malloc
	mov	r4, r0
	cbnz	r0, .L1178
	ldr	r1, .L1179
	ldr	r0, .L1179+4
	bl	NAND_Print
.L1178:
	mov	r0, r4
	mov	r2, #4096
	movs	r1, #165
	bl	NAND_Memset
	ldr	r2, .L1179+8
	mov	r3, #-1
	strh	r3, [sp, #16]	@ movhi
	mov	r1, r6
	mov	r0, r5
	strd	r3, r2, [sp, #8]
	movw	r3, #65535
	strb	r3, [sp, #18]
	add	r3, sp, #8
	strd	r4, r3, [sp]
	mov	r2, r7
	movs	r3, #8
	bl	page_write
	mov	r5, r0
	mov	r0, r4
	bl	nftl_free
	mov	r0, r5
	add	sp, sp, #28
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L1180:
	.align	2
.L1179:
	.word	.LANCHOR93
	.word	.LC140
	.word	-5921281
	.size	nand_dbg_phy_write, .-nand_dbg_phy_write
	.section	.text.nand_dbg_phy_erase,"ax",%progbits
	.align	1
	.global	nand_dbg_phy_erase
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_dbg_phy_erase, %function
nand_dbg_phy_erase:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	b	block_erase
	.size	nand_dbg_phy_erase, .-nand_dbg_phy_erase
	.section	.text.nand_dbg_single_phy_erase,"ax",%progbits
	.align	1
	.global	nand_dbg_single_phy_erase
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_dbg_single_phy_erase, %function
nand_dbg_single_phy_erase:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	b	nand_physic_erase_block
	.size	nand_dbg_single_phy_erase, .-nand_dbg_single_phy_erase
	.section	.text.clean_block_list,"ax",%progbits
	.align	1
	.global	clean_block_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	clean_block_list, %function
clean_block_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r5, r0
	movs	r4, #0
	mov	r6, #-1
.L1185:
	mov	r0, r5
	bl	out_phy_block_from_free_list
	cbnz	r0, .L1188
	mov	r1, r0
	mov	r4, #-1
.L1184:
	mov	r0, r5
	bl	out_phy_block_from_invalid_page_list
	cbz	r0, .L1183
	strh	r1, [r0, #4]	@ movhi
	strh	r1, [r0, #2]	@ movhi
	strd	r4, r1, [r0, #8]
	strd	r1, r1, [r0, #16]
	strd	r1, r1, [r0, #24]
	str	r1, [r0, #32]
	b	.L1184
.L1188:
	strh	r4, [r0, #4]	@ movhi
	strh	r4, [r0, #2]	@ movhi
	strd	r6, r4, [r0, #8]
	strd	r4, r4, [r0, #16]
	strd	r4, r4, [r0, #24]
	str	r4, [r0, #32]
	b	.L1185
.L1183:
	pop	{r4, r5, r6, pc}
	.size	clean_block_list, .-clean_block_list
	.section	.text.nand_clean_zone_table2,"ax",%progbits
	.align	1
	.global	nand_clean_zone_table2
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_clean_zone_table2, %function
nand_clean_zone_table2:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #4]
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r4, r0
	ldr	r0, [r0, #24]
	add	r8, r4, #52
	mov	r5, r4
	ldrb	r1, [r3, #8]	@ zero_extendqisi2
	bl	__aeabi_uidiv
	lsls	r6, r0, #2
	ldr	fp, .L1198+4
	mov	r2, r0
	ldrb	r1, [r4, #16]	@ zero_extendqisi2
	lsrs	r7, r6, #20
	ldr	r0, .L1198
	add	r10, r8, r7, lsl #2
	bl	NAND_Print_DBG
.L1190:
	cmp	r8, r10
	bne	.L1191
	ubfx	r6, r6, #0, #20
	cbz	r6, .L1192
	add	r7, r4, r7, lsl #2
	movs	r1, #255
	mov	r2, r6
	ldr	r0, [r7, #52]
	bl	NAND_Memset
	mov	r1, r6
	ldr	r0, .L1198+4
	bl	NAND_Print_DBG
.L1192:
	movs	r3, #0
	movw	r2, #65535
	strh	r2, [r4, #320]	@ movhi
	strh	r2, [r4, #352]	@ movhi
	mov	r2, r3
	str	r3, [r4, #292]
	str	r3, [r4, #324]
	strd	r3, r3, [r4, #172]
	strd	r3, r3, [r4, #164]
	strd	r3, r3, [r4, #180]
	strd	r3, r3, [r4, #208]
	strd	r3, r3, [r4, #200]
	strd	r3, r3, [r4, #216]
	strd	r3, r3, [r4, #244]
	strd	r3, r3, [r4, #236]
	strd	r3, r3, [r4, #252]
	str	r3, [r4, #20]
	strh	r3, [r4, #392]	@ movhi
.L1193:
	strh	r3, [r5, #396]	@ movhi
	adds	r3, r3, #1
	cmp	r3, #10
	strh	r2, [r5, #398]	@ movhi
	strd	r2, r2, [r5, #400]
	add	r5, r5, #16
	str	r2, [r5, #392]
	bne	.L1193
	movs	r3, #255
	strh	r2, [r4, #394]	@ movhi
	strd	r3, r2, [r4, #556]
	movs	r0, #0
	strd	r2, r2, [r4, #564]
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1191:
	ldr	r0, [r8], #4
	mov	r2, #1048576
	movs	r1, #255
	bl	NAND_Memset
	mov	r1, #1048576
	mov	r0, fp
	bl	NAND_Print_DBG
	b	.L1190
.L1199:
	.align	2
.L1198:
	.word	.LC141
	.word	.LC142
	.size	nand_clean_zone_table2, .-nand_clean_zone_table2
	.section	.text.nftl_clean_logic_page_map,"ax",%progbits
	.align	1
	.global	nftl_clean_logic_page_map
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_clean_logic_page_map, %function
nftl_clean_logic_page_map:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, r8, r10, lr}
	mov	r4, r0
	ldr	r3, [r0, #4]
	add	r6, r4, #52
	ldr	r0, [r0, #24]
	ldrb	r1, [r3, #8]	@ zero_extendqisi2
	bl	__aeabi_uidiv
	lsls	r5, r0, #2
	ldr	r10, .L1204+4
	mov	r2, r0
	ldrb	r1, [r4, #16]	@ zero_extendqisi2
	lsrs	r7, r5, #20
	ldr	r0, .L1204
	add	r8, r6, r7, lsl #2
	bl	NAND_Print_DBG
.L1201:
	cmp	r6, r8
	bne	.L1202
	ubfx	r5, r5, #0, #20
	cbz	r5, .L1200
	add	r4, r4, r7, lsl #2
	mov	r2, r5
	movs	r1, #255
	ldr	r0, [r4, #52]
	bl	NAND_Memset
	mov	r1, r5
	ldr	r0, .L1204+4
	pop	{r3, r4, r5, r6, r7, r8, r10, lr}
	b	NAND_Print_DBG
.L1202:
	ldr	r0, [r6], #4
	mov	r2, #1048576
	movs	r1, #255
	bl	NAND_Memset
	mov	r1, #1048576
	mov	r0, r10
	bl	NAND_Print_DBG
	b	.L1201
.L1200:
	pop	{r3, r4, r5, r6, r7, r8, r10, pc}
.L1205:
	.align	2
.L1204:
	.word	.LC141
	.word	.LC142
	.size	nftl_clean_logic_page_map, .-nftl_clean_logic_page_map
	.section	.text.nftl_clean_used_list,"ax",%progbits
	.align	1
	.global	nftl_clean_used_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_clean_used_list, %function
nftl_clean_used_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r1, [r0, #4]
	movs	r3, #0
	push	{r4, r5, r6, lr}
	movs	r6, #36
	mov	r4, r3
	ldrh	r5, [r1, #24]
.L1207:
	cmp	r5, r3
	bgt	.L1208
	movs	r3, #0
	strd	r3, r3, [r0, #244]
	strd	r3, r3, [r0, #236]
	strd	r3, r3, [r0, #252]
	pop	{r4, r5, r6, pc}
.L1208:
	ldr	r2, [r1, #32]
	mla	r2, r6, r3, r2
	adds	r3, r3, #1
	strd	r4, r4, [r2, #28]
	b	.L1207
	.size	nftl_clean_used_list, .-nftl_clean_used_list
	.section	.text.nftl_clean_invalid_list,"ax",%progbits
	.align	1
	.global	nftl_clean_invalid_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_clean_invalid_list, %function
nftl_clean_invalid_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r1, [r0, #4]
	movs	r3, #0
	push	{r4, r5, r6, lr}
	movs	r6, #36
	mov	r4, r3
	ldrh	r5, [r1, #24]
.L1210:
	cmp	r5, r3
	bgt	.L1211
	movs	r3, #0
	strd	r3, r3, [r0, #208]
	strd	r3, r3, [r0, #200]
	strd	r3, r3, [r0, #216]
	pop	{r4, r5, r6, pc}
.L1211:
	ldr	r2, [r1, #32]
	mla	r2, r6, r3, r2
	adds	r3, r3, #1
	strd	r4, r4, [r2, #12]
	b	.L1210
	.size	nftl_clean_invalid_list, .-nftl_clean_invalid_list
	.section	.text.clean_zone,"ax",%progbits
	.align	1
	.global	clean_zone
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	clean_zone, %function
clean_zone:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r3, r4, r5, r6, r7, r8, lr}
	mov	r4, r0
	cbz	r0, .L1213
	ldrb	r6, [r0, #1976]	@ zero_extendqisi2
	cmp	r6, #0
	beq	.L1214
.L1213:
	ldr	r5, [r4, #172]
	movs	r6, #0
	ldr	r7, .L1242
	ldr	r8, .L1242+4
.L1215:
	cmp	r5, #0
	bne	.L1217
	ldr	r5, [r4, #252]
	ldr	r7, .L1242
	ldr	r8, .L1242+12
.L1218:
	cmp	r5, #0
	bne	.L1220
	ldr	r3, [r4, #292]
	cbz	r3, .L1221
	ldrh	r2, [r3]
	mov	r1, sp
	ldr	r3, [r4, #4]
	mov	r0, r4
	strh	r5, [sp]	@ movhi
	strh	r2, [sp, #2]	@ movhi
	ldrh	r3, [r3, #28]
	strd	r5, r5, [sp, #8]
	strh	r3, [sp, #4]	@ movhi
	ldr	r3, [r4, #1980]
	blx	r3
	cbz	r0, .L1222
	ldrh	r2, [r5]
	ldr	r1, .L1242
	ldr	r0, .L1242+4
	bl	NAND_Print
	ldr	r3, [r4, #2000]
	mov	r1, sp
	mov	r0, r4
	blx	r3
	ldrh	r3, [r4, #20]
	adds	r3, r3, #1
	strh	r3, [r4, #20]	@ movhi
.L1222:
	movs	r1, #0
	movs	r6, #1
	ldrh	r3, [r1, #6]
	mov	r0, r4
	strh	r1, [r1, #2]	@ movhi
	strh	r1, [r1, #4]	@ movhi
	adds	r3, r3, #1
	strh	r3, [r1, #6]	@ movhi
	bl	put_phy_block_to_free_list
.L1221:
	mov	r0, r4
	bl	nftl_clean_logic_page_map
	mov	r0, r4
	bl	nftl_clean_used_list
	bl	nftl_clean_invalid_list
	bl	out_phy_block_from_free_list
	str	r0, [r4, #292]
	cbnz	r0, .L1223
	ldr	r0, .L1242+8
	bl	NAND_Print
.L1223:
	ldr	r2, [r4, #292]
	movs	r3, #0
	strh	r3, [r4, #320]	@ movhi
	ldr	r3, [r2, #8]
	adds	r3, r3, #1
	str	r3, [r2, #8]
.L1214:
	mov	r0, r6
	add	sp, sp, #16
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L1217:
	ldr	r3, [r4, #4]
	movs	r6, #0
	ldrh	r2, [r5]
	mov	r1, sp
	strh	r6, [sp]	@ movhi
	mov	r0, r4
	ldrh	r3, [r3, #28]
	strh	r2, [sp, #2]	@ movhi
	strd	r6, r6, [sp, #8]
	strh	r3, [sp, #4]	@ movhi
	ldr	r3, [r4, #1980]
	blx	r3
	cbz	r0, .L1216
	ldrh	r2, [r5]
	mov	r1, r7
	mov	r0, r8
	bl	NAND_Print
	ldr	r3, [r4, #2000]
	mov	r1, sp
	mov	r0, r4
	blx	r3
	movs	r3, #255
	strh	r6, [r5, #4]	@ movhi
	strh	r3, [r5, #2]	@ movhi
	mov	r1, r5
	ldrh	r3, [r4, #20]
	mov	r0, r4
	adds	r3, r3, #1
	strh	r3, [r4, #20]	@ movhi
	bl	out_phy_block_from_free_list_by_block
.L1216:
	ldrh	r3, [r5, #6]
	movs	r6, #1
	adds	r3, r3, #1
	strh	r3, [r5, #6]	@ movhi
	ldr	r5, [r5, #20]
	b	.L1215
.L1220:
	ldr	r3, [r4, #4]
	mov	r0, r4
	ldrh	r1, [r5]
	ldrh	r2, [r3, #28]
	movs	r3, #0
	strh	r3, [sp]	@ movhi
	strh	r1, [sp, #2]	@ movhi
	mov	r1, sp
	strd	r3, r3, [sp, #8]
	strh	r2, [sp, #4]	@ movhi
	ldr	r3, [r4, #1980]
	blx	r3
	cbz	r0, .L1219
	ldrh	r2, [r5]
	mov	r1, r7
	mov	r0, r8
	bl	NAND_Print
	ldr	r3, [r4, #2000]
	mov	r1, sp
	mov	r0, r4
	blx	r3
	ldrh	r3, [r4, #20]
	adds	r3, r3, #1
	strh	r3, [r4, #20]	@ movhi
.L1219:
	ldrh	r3, [r5, #6]
	movs	r2, #0
	strh	r2, [r5, #2]	@ movhi
	mov	r1, r5
	strh	r2, [r5, #4]	@ movhi
	mov	r0, r4
	adds	r3, r3, #1
	movs	r6, #1
	strh	r3, [r5, #6]	@ movhi
	bl	put_phy_block_to_free_list
	ldr	r5, [r5, #28]
	b	.L1218
.L1243:
	.align	2
.L1242:
	.word	.LANCHOR94
	.word	.LC143
	.word	.LC145
	.word	.LC144
	.size	clean_zone, .-clean_zone
	.section	.text.nftl_clean_zone,"ax",%progbits
	.align	1
	.global	nftl_clean_zone
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_clean_zone, %function
nftl_clean_zone:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L1246
	ldr	r0, [r3, #8]
	cbz	r0, .L1245
	ldr	r0, [r0, #12]
	b	clean_zone
.L1245:
	bx	lr
.L1247:
	.align	2
.L1246:
	.word	.LANCHOR0
	.size	nftl_clean_zone, .-nftl_clean_zone
	.section	.text.set_spare_data,"ax",%progbits
	.align	1
	.global	set_spare_data
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	set_spare_data, %function
set_spare_data:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	add	r6, r3, #536870912
	movs	r4, #0
	add	r2, r2, r0
	subs	r6, r6, #1
.L1249:
	cmp	r3, r4
	bhi	.L1250
	pop	{r4, r5, r6, pc}
.L1250:
	subs	r5, r6, r4
	lsls	r5, r5, #3
	lsr	r5, r1, r5
	strb	r5, [r2, r4]
	adds	r4, r4, #1
	b	.L1249
	.size	set_spare_data, .-set_spare_data
	.section	.text.panic_write,"ax",%progbits
	.align	1
	.global	panic_write
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	panic_write, %function
panic_write:
	@ args = 0, pretend = 0, frame = 48
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	sub	sp, sp, #56
	str	r3, [sp, #16]
	mov	fp, r2
	ldr	r3, .L1264
	str	r1, [sp, #12]
	ldr	r3, [r3]
	cbz	r3, .L1262
	ldr	r8, [r0, #12]
	movs	r2, #1
	ldr	r3, .L1264+4
	add	r0, fp, #-1
	ldr	r7, .L1264+8
	ldr	r5, [r8, #4]
	str	r2, [r3]
	ldrb	r1, [r5, #8]	@ zero_extendqisi2
	add	r0, r0, r1
	bl	__aeabi_uidiv
	ldrh	r1, [r5, #20]
	mov	r10, r0
	subs	r0, r1, #1
	add	r0, r0, r10
	bl	__aeabi_uidiv
	mov	r3, r10
	str	r0, [sp]
	mov	r2, fp
	ldr	r1, [sp, #12]
	ldr	r0, .L1264+12
	bl	NAND_Print
	ldr	r3, [r7, #12]
	subs	r3, r3, #3
	cmp	r3, r10
	bcs	.L1253
	ldrb	r1, [r5, #8]	@ zero_extendqisi2
	ldr	r0, .L1264+16
	muls	r1, r3, r1
	bl	NAND_Print
.L1262:
	mov	r0, #-1
	b	.L1251
.L1253:
	movs	r2, #16
	movs	r1, #165
	add	r0, sp, #24
	mov	r4, r7
	bl	NAND_Memset
	ldr	r3, [r7, #16]
	ldr	r6, [r7, r3, lsl #2]
.L1254:
	ldr	r3, [r7, #20]
	cmp	r10, #0
	bne	.L1257
	ldr	fp, .L1264+24
	movs	r7, #3
	add	r10, sp, #24
	movs	r3, #98
	strb	r3, [sp, #26]
.L1261:
	ldr	r2, [r4, #20]
	cmp	r2, #0
	bne	.L1258
.L1259:
	movs	r0, #0
.L1251:
	add	sp, sp, #56
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1257:
	ldrh	r2, [r5, #20]
	cmp	r3, r2
	bcc	.L1255
	ldr	r3, [r7, #16]
	adds	r3, r3, #1
	str	r3, [r7, #16]
	ldr	r6, [r7, r3, lsl #2]
	movs	r3, #0
	str	r3, [r7, #20]
.L1255:
	ldr	r3, [r4, #20]
	ldrh	r1, [r6]
	cbnz	r3, .L1256
	ldrh	r2, [r5, #28]
	mov	r0, r8
	strh	r3, [sp, #40]	@ movhi
	strh	r1, [sp, #42]	@ movhi
	add	r1, sp, #40
	strh	r2, [sp, #44]	@ movhi
	strd	r3, r3, [sp, #48]
	ldr	r2, [r8, #1980]
	str	r3, [sp, #20]
	blx	r2
	ldrh	r2, [r6, #6]
	ldr	r3, [sp, #20]
	adds	r2, r2, #1
	strh	r2, [r6, #6]	@ movhi
	strh	r3, [r6, #4]	@ movhi
.L1256:
	ldr	r2, [r4, #20]
	add	r10, r10, #-1
	ldrh	r1, [r6]
	ldr	r0, .L1264+20
	bl	NAND_Print
	movw	r3, #43775
	movs	r2, #5
	strh	r3, [sp, #24]	@ movhi
	movs	r3, #99
	strb	r3, [sp, #26]
	add	r0, sp, #24
	ldrb	r3, [r8, #16]	@ zero_extendqisi2
	strb	r3, [sp, #27]
	ldr	r3, [r4, #16]
	strb	r3, [sp, #28]
	movs	r3, #2
	ldrh	r1, [r6, #6]
	bl	set_spare_data
	movs	r3, #4
	movs	r2, #7
	ldr	r1, [sp, #12]
	add	r0, sp, #24
	bl	set_spare_data
	ldrb	r1, [r5, #8]	@ zero_extendqisi2
	movs	r3, #4
	movs	r2, #11
	add	r0, sp, #24
	cmp	r1, fp
	it	cs
	movcs	r1, fp
	bl	set_spare_data
	ldrh	r3, [r5, #28]
	mov	r0, r8
	ldrh	r2, [r6]
	ldr	r1, [r4, #20]
	strh	r3, [sp, #44]	@ movhi
	ldr	r3, [sp, #16]
	strh	r1, [sp, #40]	@ movhi
	add	r1, sp, #40
	strh	r2, [sp, #42]	@ movhi
	str	r3, [sp, #48]
	add	r3, sp, #24
	str	r3, [sp, #52]
	ldr	r3, [r8, #1988]
	blx	r3
	ldr	r2, [sp, #16]
	ldrh	r3, [r5, #26]
	adds	r3, r2, r3
	ldr	r2, [sp, #12]
	str	r3, [sp, #16]
	ldrb	r3, [r5, #8]	@ zero_extendqisi2
	cmp	r3, fp
	it	cs
	movcs	r3, fp
	add	r2, r2, r3
	sub	fp, fp, r3
	ldr	r3, [r4, #12]
	str	r2, [sp, #12]
	subs	r3, r3, #1
	str	r3, [r4, #12]
	ldr	r3, [r4, #20]
	adds	r3, r3, #1
	str	r3, [r4, #20]
	b	.L1254
.L1258:
	ldrh	r3, [r5, #20]
	cmp	r2, r3
	bcs	.L1259
	ldrh	r1, [r6]
	mov	r0, fp
	bl	NAND_Print
	ldrh	r3, [r5, #28]
	mov	r0, r8
	ldrh	r2, [r6]
	ldr	r1, [r4, #20]
	strh	r3, [sp, #44]	@ movhi
	ldr	r3, [sp, #16]
	strh	r1, [sp, #40]	@ movhi
	add	r1, sp, #40
	strh	r2, [sp, #42]	@ movhi
	strd	r3, r10, [sp, #48]
	ldr	r3, [r8, #1988]
	blx	r3
	ldr	r2, [r4, #12]
	ldr	r3, [r4, #20]
	subs	r2, r2, #1
	str	r2, [r4, #12]
	adds	r3, r3, #1
	ldrh	r2, [r5, #20]
	str	r3, [r4, #20]
	cmp	r3, r2
	bcc	.L1260
	ldr	r3, [r4, #16]
	adds	r3, r3, #1
	str	r3, [r4, #16]
	ldr	r6, [r4, r3, lsl #2]
	movs	r3, #0
	str	r3, [r4, #20]
.L1260:
	subs	r7, r7, #1
	bne	.L1261
	b	.L1259
.L1265:
	.align	2
.L1264:
	.word	.LANCHOR86
	.word	.LANCHOR85
	.word	.LANCHOR88
	.word	.LC146
	.word	.LC147
	.word	.LC148
	.word	.LC149
	.size	panic_write, .-panic_write
	.section	.text.get_spare_data,"ax",%progbits
	.align	1
	.global	get_spare_data
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_spare_data, %function
get_spare_data:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, #0
	push	{r4, lr}
	add	r1, r1, r0
	mov	r4, r3
.L1267:
	cmp	r2, r3
	bhi	.L1268
	mov	r0, r4
	pop	{r4, pc}
.L1268:
	ldrb	r0, [r1, r3]	@ zero_extendqisi2
	adds	r3, r3, #1
	orr	r4, r0, r4, lsl #8
	b	.L1267
	.size	get_spare_data, .-get_spare_data
	.section	.text.set_oob_logic_page,"ax",%progbits
	.align	1
	.global	set_oob_logic_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	set_oob_logic_page, %function
set_oob_logic_page:
	@ args = 4, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r4, r1
	mov	r5, r3
	movs	r3, #255
	mov	r1, r2
	mov	r0, r4
	strb	r3, [r4]
	movs	r2, #1
	movs	r3, #4
	bl	set_spare_data
	movs	r3, #2
	movs	r2, #5
	ldrh	r1, [sp, #16]
	bl	set_spare_data
	movs	r3, #4
	movs	r2, #7
	mov	r1, r5
	bl	set_spare_data
	ldrb	r3, [r4, #1]	@ zero_extendqisi2
	orn	r3, r3, #63
	strb	r3, [r4, #1]
	pop	{r3, r4, r5, pc}
	.size	set_oob_logic_page, .-set_oob_logic_page
	.section	.text.set_oob_special_page,"ax",%progbits
	.align	1
	.global	set_oob_special_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	set_oob_special_page, %function
set_oob_special_page:
	@ args = 4, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r4, r1
	ldrh	r6, [sp, #16]
	mov	r5, r3
	movs	r3, #255
	mov	r1, r2
	strb	r3, [r4]
	mov	r0, r4
	movs	r3, #4
	movs	r2, #1
	bl	set_spare_data
	mov	r1, r6
	movs	r3, #2
	movs	r2, #5
	bl	set_spare_data
	mov	r1, r5
	movs	r3, #4
	movs	r2, #7
	pop	{r4, r5, r6, lr}
	b	set_spare_data
	.size	set_oob_special_page, .-set_oob_special_page
	.section	.text.mark_free_block,"ax",%progbits
	.align	1
	.global	mark_free_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	mark_free_block, %function
mark_free_block:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, lr}
	sub	sp, sp, #44
	add	r6, sp, #24
	mov	r4, r0
	mov	r5, r1
	mov	r0, r6
	movs	r2, #16
	movs	r1, #165
	bl	NAND_Memset
	ldr	r3, [r4, #4]
	movs	r7, #0
	ldrh	r1, [r5]
	mov	r0, r4
	strh	r7, [sp, #8]	@ movhi
	ldrh	r2, [r3, #28]
	ldr	r3, [r4, #284]
	strh	r1, [sp, #10]	@ movhi
	mov	r1, r6
	strh	r2, [sp, #12]	@ movhi
	mov	r2, #1431655765
	strd	r3, r6, [sp, #16]
	ldrh	r3, [r5, #6]
	str	r3, [sp]
	ldr	r3, [r5, #8]
	bl	set_oob_special_page
	ldr	r3, [r4, #1988]
	add	r1, sp, #8
	mov	r0, r4
	blx	r3
	ldr	r3, [r4, #4]
	movs	r0, #1
	ldrh	r1, [r5]
	strh	r0, [sp, #8]	@ movhi
	mov	r0, r4
	ldrh	r2, [r3, #28]
	ldr	r3, [r4, #284]
	strh	r1, [sp, #10]	@ movhi
	mov	r1, r6
	strh	r2, [sp, #12]	@ movhi
	mov	r2, #1431655765
	strd	r3, r6, [sp, #16]
	ldrh	r3, [r5, #6]
	str	r3, [sp]
	ldr	r3, [r5, #8]
	bl	set_oob_special_page
	ldr	r3, [r4, #1988]
	add	r1, sp, #8
	mov	r0, r4
	blx	r3
	ldr	r3, [r4, #4]
	ldrh	r0, [r5]
	ldr	r2, [r4, #284]
	ldrh	r1, [r3, #28]
	ldrh	r3, [r3, #20]
	strh	r0, [sp, #10]	@ movhi
	mov	r0, r4
	strh	r1, [sp, #12]	@ movhi
	mov	r1, r6
	subs	r3, r3, #2
	strd	r2, r6, [sp, #16]
	strh	r3, [sp, #8]	@ movhi
	mov	r2, #1431655765
	ldrh	r3, [r5, #6]
	str	r3, [sp]
	ldr	r3, [r5, #8]
	bl	set_oob_special_page
	ldr	r3, [r4, #1988]
	add	r1, sp, #8
	mov	r0, r4
	blx	r3
	ldr	r3, [r4, #4]
	ldrh	r0, [r5]
	ldr	r2, [r4, #284]
	ldrh	r1, [r3, #28]
	ldrh	r3, [r3, #20]
	strh	r0, [sp, #10]	@ movhi
	mov	r0, r4
	strh	r1, [sp, #12]	@ movhi
	mov	r1, r6
	subs	r3, r3, #1
	strd	r2, r6, [sp, #16]
	strh	r3, [sp, #8]	@ movhi
	mov	r2, #1431655765
	ldrh	r3, [r5, #6]
	str	r3, [sp]
	ldr	r3, [r5, #8]
	bl	set_oob_special_page
	ldr	r3, [r4, #1988]
	add	r1, sp, #8
	mov	r0, r4
	blx	r3
	mov	r0, r7
	add	sp, sp, #44
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
	.size	mark_free_block, .-mark_free_block
	.section	.text.erase_block,"ax",%progbits
	.align	1
	.global	erase_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	erase_block, %function
erase_block:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r3, r4, r5, r6, r7, r8, lr}
	mov	r5, r0
	mov	r4, r1
	ldr	r0, .L1275
	mov	r8, r2
	movs	r6, #0
	ldrh	r1, [r1]
	bl	NAND_Print
	ldr	r3, [r5, #4]
	mov	r1, sp
	ldrh	r2, [r4]
	mov	r0, r5
	strh	r6, [sp]	@ movhi
	ldrh	r3, [r3, #28]
	strh	r2, [sp, #2]	@ movhi
	strd	r6, r6, [sp, #8]
	strh	r3, [sp, #4]	@ movhi
	ldr	r3, [r5, #1980]
	blx	r3
	mov	r7, r0
	cbz	r0, .L1273
	ldrh	r1, [r4]
	ldr	r0, .L1275+4
	bl	NAND_Print
	ldr	r3, [r5, #2000]
	mov	r1, sp
	mov	r0, r5
	blx	r3
	movs	r3, #255
	strh	r6, [r4, #4]	@ movhi
	strh	r3, [r4, #2]	@ movhi
	ldrh	r3, [r5, #20]
	adds	r3, r3, #1
	strh	r3, [r5, #20]	@ movhi
.L1272:
	mov	r0, r7
	add	sp, sp, #16
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L1273:
	mov	r3, #-1
	mov	r1, r4
	str	r3, [r4, #8]
	mov	r0, r5
	strh	r8, [r4, #6]	@ movhi
	bl	put_phy_block_to_free_list
	mov	r1, r4
	mov	r0, r5
	bl	mark_free_block
	ldr	r3, [r5, #4]
	mov	r1, r4
	mov	r0, r5
	ldrh	r3, [r3, #20]
	strh	r7, [r4, #2]	@ movhi
	strh	r3, [r4, #4]	@ movhi
	bl	out_phy_block_from_used_list_by_block
	mov	r0, r5
	bl	out_phy_block_from_invalid_page_list_by_block
	b	.L1272
.L1276:
	.align	2
.L1275:
	.word	.LC150
	.word	.LC151
	.size	erase_block, .-erase_block
	.section	.text.adjust_free_list,"ax",%progbits
	.align	1
	.global	adjust_free_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	adjust_free_list, %function
adjust_free_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r5, r0
	bl	out_phy_block_from_free_list
	mov	r4, r0
	cbnz	r0, .L1278
	ldr	r0, .L1279
	bl	NAND_Print
.L1278:
	ldrh	r2, [r4, #6]
	mov	r1, r4
	mov	r0, r5
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
	mov	r1, r4
	mov	r0, r5
	bl	out_phy_block_from_free_list_by_block
	str	r0, [r5, #48]
	ldrh	r2, [r0, #6]
	ldrh	r1, [r0]
	ldr	r0, .L1279+4
	bl	NAND_Print
	movs	r0, #0
	pop	{r3, r4, r5, pc}
.L1280:
	.align	2
.L1279:
	.word	.LC152
	.word	.LC153
	.size	adjust_free_list, .-adjust_free_list
	.section	.text.nand_dbg_zone_erase,"ax",%progbits
	.align	1
	.global	nand_dbg_zone_erase
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_dbg_zone_erase, %function
nand_dbg_zone_erase:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r5, r2
	ldr	r3, [r0, #4]
	movs	r2, #36
	mov	r4, r0
	ldr	r3, [r3, #32]
	mla	r1, r2, r1, r3
	bl	out_phy_block_from_free_list_by_block
	mov	r2, r5
	mov	r1, r0
	mov	r0, r4
	pop	{r3, r4, r5, lr}
	b	erase_block
	.size	nand_dbg_zone_erase, .-nand_dbg_zone_erase
	.section	.text.panic_recover_data,"ax",%progbits
	.align	1
	.global	panic_recover_data
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	panic_recover_data, %function
panic_recover_data:
	@ args = 0, pretend = 0, frame = 40
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	sub	sp, sp, #40
	ldr	r4, .L1301
	ldr	r5, [r4]
	cbnz	r5, .L1283
	ldr	r0, .L1301+4
	bl	NAND_Print
.L1300:
	movs	r0, #0
	b	.L1282
.L1283:
	ldr	r6, [r0, #12]
	ldr	r0, .L1301+8
	ldr	r7, [r6, #4]
	bl	NAND_Print
	ldrh	r0, [r7, #26]
	bl	NAND_Malloc
	mov	fp, r0
	cbnz	r0, .L1285
	mov	r2, #308
	ldr	r1, .L1301+12
	ldr	r0, .L1301+16
	bl	NAND_Print
	mov	r0, #-1
.L1282:
	add	sp, sp, #40
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1285:
	ldrh	r2, [r7, #26]
	movs	r1, #0
	bl	NAND_Memset
	movs	r3, #0
.L1299:
	str	r3, [r4, #16]
	ldr	r3, [r4, #16]
	cmp	r3, #2
	bhi	.L1293
	ldr	r3, [r4, r3, lsl #2]
	str	r3, [sp]
	movs	r3, #0
.L1298:
	str	r3, [r4, #20]
	ldrh	r2, [r7, #20]
	ldr	r3, [r4, #20]
	cmp	r3, r2
	bcc	.L1294
	ldr	r3, [r4, #16]
	adds	r3, r3, #1
	b	.L1299
.L1294:
	ldr	r2, [sp]
	add	r5, sp, #24
	mov	r0, r6
	ldrh	r1, [r2]
	ldrh	r2, [r7, #28]
	strh	r3, [sp, #8]	@ movhi
	strh	r1, [sp, #10]	@ movhi
	add	r1, sp, #8
	ldr	r3, [r6, #1984]
	strh	r2, [sp, #12]	@ movhi
	str	fp, [sp, #16]
	str	r5, [sp, #20]
	blx	r3
	mov	r0, r5
	bl	is_panic_dummy_page
	ldr	r3, [sp]
	ldrh	r8, [r3]
	cbz	r0, .L1288
	ldr	r2, [r4, #20]
	mov	r1, r8
	ldr	r0, .L1301+20
	bl	NAND_Print
.L1289:
	ldr	r3, [r4, #20]
	adds	r3, r3, #1
	b	.L1298
.L1288:
	mov	r0, r5
	bl	is_panic_data_page
	ldr	r10, [r4, #20]
	cbnz	r0, .L1290
	ldr	r3, [r4, #16]
	mov	r2, r10
	mov	r1, r8
	cbnz	r3, .L1291
	cmp	r10, #0
	bne	.L1291
	ldr	r0, .L1301+24
	bl	NAND_Print
.L1292:
	movs	r1, #0
	mov	r0, fp
	bl	NAND_Free
	b	.L1300
.L1291:
	ldr	r0, .L1301+28
	bl	NAND_Print
.L1293:
	ldr	r3, [r4, #16]
	cmp	r3, #0
	blt	.L1292
	ldr	r1, [r4, r3, lsl #2]
	mov	r0, r6
	ldrh	r2, [r1, #6]
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
	ldr	r3, [r4, #16]
	subs	r3, r3, #1
	str	r3, [r4, #16]
	b	.L1293
.L1290:
	movs	r2, #4
	movs	r1, #7
	mov	r0, r5
	bl	get_spare_data
	movs	r2, #4
	str	r0, [sp, #4]
	movs	r1, #11
	mov	r0, r5
	bl	get_spare_data
	mov	r2, r10
	mov	r5, r0
	mov	r1, r8
	ldr	r3, [sp, #4]
	ldr	r0, .L1301+32
	bl	NAND_Print
	mov	r2, fp
	mov	r1, r5
	ldr	r0, [sp, #4]
	bl	nftl_write
	b	.L1289
.L1302:
	.align	2
.L1301:
	.word	.LANCHOR88
	.word	.LC154
	.word	.LC155
	.word	.LANCHOR95
	.word	.LC156
	.word	.LC157
	.word	.LC158
	.word	.LC159
	.word	.LC160
	.size	panic_recover_data, .-panic_recover_data
	.section	.text.torture_block_and_mark,"ax",%progbits
	.align	1
	.global	torture_block_and_mark
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	torture_block_and_mark, %function
torture_block_and_mark:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, lr}
	sub	sp, sp, #44
	add	r7, sp, #8
	mov	r4, r0
	mov	r5, r1
	movs	r2, #15
	movs	r1, #0
	mov	r0, r7
	bl	memset
	ldrh	r1, [r5]
	ldr	r0, .L1319
	bl	NAND_Print
	ldr	r3, [r4, #4]
	mov	r0, r4
	ldrh	r1, [r5]
	ldrh	r2, [r3, #28]
	movs	r3, #0
	strh	r3, [sp, #24]	@ movhi
	strh	r1, [sp, #26]	@ movhi
	add	r1, sp, #24
	strd	r3, r3, [sp, #32]
	strh	r2, [sp, #28]	@ movhi
	ldr	r3, [r4, #1980]
	blx	r3
	mov	r6, r0
	cbz	r0, .L1304
	ldrh	r1, [r5]
	ldr	r0, .L1319+4
.L1316:
	bl	NAND_Print
	b	.L1305
.L1304:
	ldrh	r3, [r5, #6]
	movs	r1, #165
	ldr	r0, [r4, #284]
	mov	r8, r6
	adds	r3, r3, #1
	strh	r3, [r5, #6]	@ movhi
	ldr	r3, [r4, #4]
	ldrh	r2, [r3, #26]
	bl	NAND_Memset
	movs	r2, #15
	movs	r1, #165
	mov	r0, r7
	bl	NAND_Memset
.L1307:
	ldr	r3, [r4, #4]
	mov	r0, r4
	ldrh	r1, [r5]
	strh	r8, [sp, #24]	@ movhi
	ldrh	r2, [r3, #28]
	ldr	r3, [r4, #284]
	strh	r1, [sp, #26]	@ movhi
	add	r1, sp, #24
	strh	r2, [sp, #28]	@ movhi
	strd	r3, r7, [sp, #32]
	ldr	r3, [r4, #1988]
	blx	r3
	mov	r6, r0
	cbz	r0, .L1306
	ldrh	r2, [r5]
	mov	r3, r8
	ldr	r1, .L1319+8
	ldr	r0, .L1319+12
.L1317:
	bl	NAND_Print
.L1305:
	ldr	r3, [r4, #2000]
	add	r1, sp, #24
	mov	r0, r4
	blx	r3
	movs	r3, #0
	strh	r3, [r5, #4]	@ movhi
	movs	r3, #255
	strh	r3, [r5, #2]	@ movhi
	ldrh	r3, [r4, #20]
	adds	r3, r3, #1
	strh	r3, [r4, #20]	@ movhi
	b	.L1303
.L1306:
	add	r8, r8, #1
	cmp	r8, #3
	bne	.L1307
	mov	r8, r0
.L1311:
	ldr	r3, [r4, #4]
	movs	r1, #0
	ldr	r0, [r4, #284]
	ldrh	r2, [r3, #26]
	bl	NAND_Memset
	movs	r2, #15
	movs	r1, #0
	mov	r0, r7
	bl	NAND_Memset
	ldr	r3, [r4, #4]
	mov	r0, r4
	ldrh	r1, [r5]
	strh	r8, [sp, #24]	@ movhi
	ldrh	r2, [r3, #28]
	ldr	r3, [r4, #284]
	strh	r1, [sp, #26]	@ movhi
	add	r1, sp, #24
	strh	r2, [sp, #28]	@ movhi
	strd	r3, r7, [sp, #32]
	ldr	r3, [r4, #1984]
	blx	r3
	mov	r6, r0
	cbz	r0, .L1308
	mov	r3, r8
	ldrh	r2, [r5]
	ldr	r1, .L1319+8
	ldr	r0, .L1319+16
	b	.L1317
.L1308:
	ldr	r10, [r4, #4]
	movs	r1, #165
	ldr	r0, [r4, #284]
	ldrh	r2, [r10, #26]
	bl	nftl_check_pattern
	mov	r6, r0
	cbz	r0, .L1309
	ldrh	r2, [r5]
	movs	r3, #165
	ldr	r1, .L1319+8
	ldr	r0, .L1319+20
	str	r3, [sp]
	mov	r3, r8
.L1318:
	bl	NAND_Print
	b	.L1305
.L1309:
	movs	r2, #15
	movs	r1, #165
	mov	r0, r7
	bl	nftl_check_pattern
	mov	r6, r0
	cbz	r0, .L1310
	movs	r3, #165
	ldrh	r2, [r5]
	str	r3, [sp]
	mov	r3, r8
	ldr	r1, .L1319+8
	ldr	r0, .L1319+24
	b	.L1318
.L1310:
	add	r8, r8, #1
	cmp	r8, #3
	bne	.L1311
	ldrh	r3, [r10, #28]
	add	r1, sp, #24
	ldrh	r2, [r5]
	strh	r0, [sp, #24]	@ movhi
	strh	r3, [sp, #28]	@ movhi
	strd	r0, r0, [sp, #32]
	mov	r0, r4
	strh	r2, [sp, #26]	@ movhi
	ldr	r3, [r4, #1980]
	blx	r3
	mov	r6, r0
	cbz	r0, .L1312
	ldrh	r1, [r5]
	ldr	r0, .L1319+28
	b	.L1316
.L1312:
	ldrh	r3, [r5, #6]
	mov	r1, r5
	mov	r0, r4
	adds	r3, r3, #1
	strh	r3, [r5, #6]	@ movhi
	mov	r3, #-1
	str	r3, [r5, #8]
	bl	put_phy_block_to_free_list
	mov	r1, r5
	mov	r0, r4
	bl	mark_free_block
	ldr	r3, [r4, #4]
	ldrh	r3, [r3, #20]
	strh	r6, [r5, #2]	@ movhi
	strh	r3, [r5, #4]	@ movhi
.L1303:
	mov	r0, r6
	add	sp, sp, #44
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L1320:
	.align	2
.L1319:
	.word	.LC161
	.word	.LC151
	.word	.LANCHOR96
	.word	.LC162
	.word	.LC163
	.word	.LC164
	.word	.LC165
	.word	.LC166
	.size	torture_block_and_mark, .-torture_block_and_mark
	.section	.text.set_no_use_page,"ax",%progbits
	.align	1
	.global	set_no_use_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	set_no_use_page, %function
set_no_use_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, #255
	push	{r4, lr}
	mov	r4, r1
	strb	r3, [r1]
	movs	r3, #1
	movs	r1, #170
	mov	r2, r3
	mov	r0, r4
	bl	set_spare_data
	movs	r3, #1
	movs	r2, #2
	movs	r1, #187
	pop	{r4, lr}
	b	set_spare_data
	.size	set_no_use_page, .-set_no_use_page
	.section	.text.set_start_block_flag,"ax",%progbits
	.align	1
	.global	set_start_block_flag
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	set_start_block_flag, %function
set_start_block_flag:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, #255
	push	{r4, r5, r6, lr}
	mov	r4, r1
	strb	r3, [r1]
	movs	r3, #1
	mov	r5, r2
	mov	r0, r4
	mov	r2, r3
	movs	r1, #170
	bl	set_spare_data
	movs	r3, #1
	movs	r2, #2
	movs	r1, #238
	bl	set_spare_data
	mov	r1, r5
	movs	r3, #4
	movs	r2, #3
	pop	{r4, r5, r6, lr}
	b	set_spare_data
	.size	set_start_block_flag, .-set_start_block_flag
	.section	.text.set_end_block_flag,"ax",%progbits
	.align	1
	.global	set_end_block_flag
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	set_end_block_flag, %function
set_end_block_flag:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, #255
	push	{r4, lr}
	mov	r4, r1
	strb	r3, [r1]
	movs	r3, #1
	movs	r1, #170
	mov	r2, r3
	mov	r0, r4
	bl	set_spare_data
	movs	r3, #1
	movs	r2, #2
	movs	r1, #204
	pop	{r4, lr}
	b	set_spare_data
	.size	set_end_block_flag, .-set_end_block_flag
	.section	.text.build_zone_list_first,"ax",%progbits
	.align	1
	.global	build_zone_list_first
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	build_zone_list_first, %function
build_zone_list_first:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, lr}
	mov	r4, r0
	sub	sp, sp, #36
	ldr	r0, .L1350
	mov	r10, r1
	bl	NAND_Print_DBG
	ldr	r3, [r4, #24]
	ldr	r5, [r4, #40]
	cmp	r3, #0
	beq	.L1345
	ldr	r6, [r4, #4]
	ldr	r0, [r4, #32]
	ldrb	r1, [r6, #8]	@ zero_extendqisi2
	add	r0, r0, r3
	bl	__aeabi_uidiv
	ldrh	r1, [r6, #20]
	bl	__aeabi_uidiv
	mov	r8, r0
.L1325:
	movs	r7, #0
	strh	r7, [r4, #18]	@ movhi
.L1326:
	ldrh	r3, [r4, #18]
	cmp	r3, r8
	bcs	.L1336
	ldr	r3, [r4, #4]
	mov	r0, r4
	ldrh	r1, [r5]
	str	r5, [r4, #44]
	ldrh	r2, [r3, #28]
	ldr	r3, [r4, #284]
	strh	r1, [sp, #18]	@ movhi
	add	r1, sp, #16
	strh	r7, [sp, #16]	@ movhi
	str	r3, [sp, #24]
	strh	r2, [sp, #20]	@ movhi
	str	sp, [sp, #28]
	ldr	r3, [r4, #1996]
	blx	r3
	cmp	r0, #1
	bne	.L1327
	ldr	r3, [r4, #1980]
	add	r1, sp, #16
	mov	r0, r4
	blx	r3
	mov	r6, r0
	cbz	r0, .L1328
	movs	r3, #255
	add	r1, sp, #16
	strh	r3, [r5, #2]	@ movhi
	mov	r0, r4
	ldr	r3, [r4, #2000]
	blx	r3
.L1329:
	movw	r3, #65535
	str	r7, [r5, #20]
	strh	r3, [r5, #4]	@ movhi
	mov	r1, r4
	strd	r7, r7, [r5, #12]
	mov	r0, r5
	str	r5, [r4, #44]
	bl	is_last_phy_block
	cmp	r0, #1
	bne	.L1332
.L1336:
	ldrh	r1, [r4, #18]
	cmp	r1, #1
	bhi	.L1333
	ldr	r0, .L1350+4
	bl	NAND_Print
	movs	r0, #1
	b	.L1324
.L1345:
	mov	r8, #-1
	b	.L1325
.L1328:
	ldrh	r3, [r4, #18]
	adds	r3, r3, #1
	uxth	r3, r3
	cmp	r3, #1
	strh	r3, [r4, #18]	@ movhi
	strh	r0, [r5, #2]	@ movhi
	strh	r0, [r5, #6]	@ movhi
	bne	.L1329
	movs	r2, #16
	movs	r1, #255
	mov	r0, sp
	bl	NAND_Memset
	mov	r2, r10
	mov	r1, sp
	mov	r0, r4
	bl	set_start_block_flag
	ldr	r3, [r4, #4]
	ldrh	r1, [r5]
	strh	r6, [sp, #16]	@ movhi
	ldrh	r2, [r3, #28]
	ldr	r3, [r4, #284]
	strh	r1, [sp, #18]	@ movhi
	strh	r2, [sp, #20]	@ movhi
	str	r3, [sp, #24]
	str	sp, [sp, #28]
.L1330:
	ldr	r3, [r4, #4]
	ldrh	r3, [r3, #20]
	cmp	r3, r6
	bhi	.L1331
	add	r3, r5, #36
	str	r3, [r4, #40]
	b	.L1329
.L1331:
	ldr	r3, [r4, #1988]
	add	r1, sp, #16
	mov	r0, r4
	adds	r6, r6, #1
	blx	r3
	ldrh	r3, [sp, #16]
	adds	r3, r3, #1
	strh	r3, [sp, #16]	@ movhi
	b	.L1330
.L1327:
	movs	r3, #255
	strh	r3, [r5, #2]	@ movhi
	b	.L1329
.L1332:
	adds	r5, r5, #36
	b	.L1326
.L1333:
	cmp	r8, #-1
	add	r1, r1, #-1
	strh	r1, [r4, #18]	@ movhi
	bne	.L1337
.L1339:
	movs	r0, #0
.L1324:
	add	sp, sp, #36
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L1337:
	mov	r1, r4
	mov	r0, r5
	bl	is_last_phy_block
	cmp	r0, #1
	beq	.L1339
	movs	r7, #0
.L1340:
	ldr	r2, [r4, #4]
	ldrh	r3, [r5]
	ldrh	r1, [r2, #24]
	cmp	r1, r3
	bls	.L1339
	ldrh	r1, [r2, #28]
	mov	r0, r4
	ldr	r2, [r4, #284]
	strh	r3, [sp, #18]	@ movhi
	strh	r1, [sp, #20]	@ movhi
	add	r1, sp, #16
	strh	r7, [sp, #16]	@ movhi
	str	r2, [sp, #24]
	str	sp, [sp, #28]
	ldr	r3, [r4, #1996]
	blx	r3
	cmp	r0, #1
	bne	.L1341
	ldr	r3, [r4, #1980]
	add	r1, sp, #16
	mov	r0, r4
	blx	r3
	mov	r6, r0
	cbz	r0, .L1342
	movs	r3, #255
	add	r1, sp, #16
	strh	r3, [r5, #2]	@ movhi
	mov	r0, r4
	ldr	r3, [r4, #2000]
	blx	r3
.L1343:
	adds	r5, r5, #36
	b	.L1340
.L1342:
	movs	r2, #16
	mov	r0, sp
	movs	r1, #255
	bl	NAND_Memset
	mov	r1, sp
	mov	r0, r4
	bl	set_end_block_flag
	ldr	r3, [r4, #4]
	mov	r0, r4
	ldrh	r1, [r5]
	strh	r6, [sp, #16]	@ movhi
	ldrh	r2, [r3, #28]
	ldr	r3, [r4, #284]
	strh	r1, [sp, #18]	@ movhi
	add	r1, sp, #16
	strh	r2, [sp, #20]	@ movhi
	str	r3, [sp, #24]
	str	sp, [sp, #28]
	ldr	r3, [r4, #1988]
	blx	r3
	b	.L1339
.L1341:
	movs	r3, #255
	strh	r3, [r5, #2]	@ movhi
	b	.L1343
.L1351:
	.align	2
.L1350:
	.word	.LC167
	.word	.LC168
	.size	build_zone_list_first, .-build_zone_list_first
	.section	.text.get_bad_flag_from_oob,"ax",%progbits
	.align	1
	.global	get_bad_flag_from_oob
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_bad_flag_from_oob, %function
get_bad_flag_from_oob:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	movs	r2, #4
	movs	r1, #1
	bl	get_spare_data
	adds	r0, r0, #0
	it	ne
	movne	r0, #1
	pop	{r3, pc}
	.size	get_bad_flag_from_oob, .-get_bad_flag_from_oob
	.section	.text.get_block_used_count_from_oob,"ax",%progbits
	.align	1
	.global	get_block_used_count_from_oob
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_block_used_count_from_oob, %function
get_block_used_count_from_oob:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r2, #4
	movs	r1, #7
	b	get_spare_data
	.size	get_block_used_count_from_oob, .-get_block_used_count_from_oob
	.section	.text.get_used_block_count,"ax",%progbits
	.align	1
	.global	get_used_block_count
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_used_block_count, %function
get_used_block_count:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	ldrh	r2, [r1]
	movs	r3, #0
	push	{r4, lr}
	sub	sp, sp, #32
	add	r4, sp, #16
	strh	r3, [sp]	@ movhi
	mov	r1, sp
	strh	r2, [sp, #2]	@ movhi
	strh	r3, [sp, #4]	@ movhi
	str	r3, [sp, #8]
	str	r4, [sp, #12]
	ldr	r3, [r0, #1984]
	blx	r3
	mov	r0, r4
	bl	get_block_used_count_from_oob
	add	sp, sp, #32
	@ sp needed
	pop	{r4, pc}
	.size	get_used_block_count, .-get_used_block_count
	.section	.text.get_logic_page_from_oob,"ax",%progbits
	.align	1
	.global	get_logic_page_from_oob
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_logic_page_from_oob, %function
get_logic_page_from_oob:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	movs	r2, #4
	movs	r1, #1
	bl	get_spare_data
	bic	r0, r0, #-268435456
	pop	{r3, pc}
	.size	get_logic_page_from_oob, .-get_logic_page_from_oob
	.section	.text._nftl_nand_read_logic_history_page,"ax",%progbits
	.align	1
	.global	_nftl_nand_read_logic_history_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nftl_nand_read_logic_history_page, %function
_nftl_nand_read_logic_history_page:
	@ args = 0, pretend = 0, frame = 56
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	sub	sp, sp, #56
	movs	r2, #16
	mov	r5, r0
	add	r0, sp, #24
	str	r1, [sp, #16]
	movs	r1, #0
	bl	memset
	ldr	r3, [r5, #4]
	ldrb	r6, [r3, #8]	@ zero_extendqisi2
	lsls	r6, r6, #9
	mov	r0, r6
	bl	nftl_malloc
	mov	r8, r0
	mov	r0, r6
	bl	nftl_malloc
	mov	fp, r0
	cmp	r8, #0
	bne	.L1357
.L1374:
	ldr	r1, .L1375
	ldr	r0, .L1375+4
	bl	NAND_Print
	movs	r0, #1
.L1356:
	add	sp, sp, #56
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1357:
	cbnz	r0, .L1359
	mov	r0, r8
	bl	nftl_free
	b	.L1374
.L1359:
	ldrh	r2, [r5, #18]
	movs	r1, #0
	ldrh	r3, [r5, #20]
	mov	r0, r8
	mov	r10, #0
	mov	r7, r10
	adds	r3, r2, r3
	mov	r2, r6
	str	r3, [sp, #8]
	bl	NAND_Memset
	mov	r2, r6
	movs	r1, #0
	mov	r0, fp
	bl	NAND_Memset
	movs	r2, #16
	movs	r1, #0
	add	r0, sp, #24
	bl	NAND_Memset
	lsrs	r3, r6, #9
	str	r3, [sp, #20]
.L1360:
	ldr	r3, [sp, #8]
	cmp	r3, r7
	bne	.L1368
	mov	r1, r3
	ldr	r0, .L1375+8
	bl	NAND_Print
	mov	r0, r8
	bl	nftl_free
	mov	r0, fp
	bl	nftl_free
	movs	r0, #0
	b	.L1356
.L1368:
	mov	r1, r7
	ldr	r0, .L1375+12
	bl	NAND_Print
	uxth	r3, r7
	movs	r4, #0
	strh	r4, [sp, #40]	@ movhi
	add	r1, sp, #40
	str	r3, [sp, #4]
	mov	r0, r5
	ldrh	r3, [sp, #4]
	strh	r4, [sp, #44]	@ movhi
	str	r4, [sp, #48]
	strh	r3, [sp, #42]	@ movhi
	add	r3, sp, #24
	str	r3, [sp, #52]
	ldr	r3, [r5, #1996]
	blx	r3
	cmp	r0, #1
	beq	.L1361
	mov	r1, r7
	ldr	r0, .L1375+16
	bl	NAND_Print
.L1362:
	adds	r7, r7, #1
	b	.L1360
.L1367:
	movs	r2, #16
	movs	r1, #0
	add	r0, sp, #24
	bl	NAND_Memset
	uxth	r3, r4
	add	r1, sp, #40
	mov	r0, r5
	str	r3, [sp, #12]
	ldrh	r3, [sp, #12]
	strh	r3, [sp, #40]	@ movhi
	ldrh	r3, [sp, #4]
	strh	r3, [sp, #42]	@ movhi
	movs	r3, #0
	strh	r3, [sp, #44]	@ movhi
	str	r3, [sp, #48]
	add	r3, sp, #24
	str	r3, [sp, #52]
	ldr	r3, [r5, #1984]
	blx	r3
	adds	r0, r0, #2
	bne	.L1363
	ldr	r0, .L1375+20
	mov	r2, r4
	mov	r1, r7
.L1373:
	bl	NAND_Print
.L1364:
	adds	r4, r4, #1
.L1361:
	ldr	r3, [r5, #4]
	ldrh	r3, [r3, #20]
	cmp	r3, r4
	bgt	.L1367
	b	.L1362
.L1363:
	add	r0, sp, #24
	bl	get_logic_page_from_oob
	ldr	r3, [sp, #16]
	cmp	r0, r3
	bne	.L1364
	ldrh	r3, [sp, #12]
	add	r1, sp, #40
	str	r8, [sp, #48]
	mov	r0, r5
	strh	r3, [sp, #40]	@ movhi
	ldrh	r3, [sp, #4]
	strh	r3, [sp, #42]	@ movhi
	ldrh	r3, [sp, #20]
	strh	r3, [sp, #44]	@ movhi
	add	r3, sp, #24
	str	r3, [sp, #52]
	ldr	r3, [r5, #1984]
	blx	r3
	adds	r0, r0, #2
	bne	.L1365
	mov	r2, r4
	mov	r1, r7
	ldr	r0, .L1375+24
	b	.L1373
.L1365:
	add	r3, r10, #1
	str	r3, [sp, #12]
	cmp	r10, #0
	beq	.L1366
	mov	r2, r10
	ldr	r1, [sp, #16]
	ldr	r0, .L1375+28
	bl	NAND_Print
	mov	r1, r6
	mov	r0, fp
	bl	dumphex
	ldr	r0, .L1375+32
	bl	NAND_Print
	movs	r1, #16
	add	r0, sp, #24
	bl	dumphex
.L1366:
	mov	r2, r6
	mov	r1, r8
	mov	r0, fp
	bl	NAND_Memcpy
	ldr	r10, [sp, #12]
	b	.L1364
.L1376:
	.align	2
.L1375:
	.word	.LANCHOR97
	.word	.LC169
	.word	.LC176
	.word	.LC170
	.word	.LC171
	.word	.LC172
	.word	.LC173
	.word	.LC174
	.word	.LC175
	.size	_nftl_nand_read_logic_history_page, .-_nftl_nand_read_logic_history_page
	.section	.text._nand_read_history,"ax",%progbits
	.align	1
	.global	_nand_read_history
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nand_read_history, %function
_nand_read_history:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r6, r3
	ldr	r4, [r0, #12]
	add	r2, r2, r1
	mov	r5, r1
	ldr	r3, [r4, #24]
	cmp	r2, r3
	bhi	.L1381
	ldr	r3, [r4, #4]
	mov	r0, r5
	ldrb	r1, [r3, #8]	@ zero_extendqisi2
	bl	__aeabi_uidiv
	mov	r2, r6
	mov	r1, r0
	mov	r0, r4
	pop	{r4, r5, r6, lr}
	b	_nftl_nand_read_logic_history_page
.L1381:
	ldr	r0, .L1382
	bl	NAND_Print
	movs	r0, #1
	pop	{r4, r5, r6, pc}
.L1383:
	.align	2
.L1382:
	.word	.LC177
	.size	_nand_read_history, .-_nand_read_history
	.section	.text.get_special_data_from_oob,"ax",%progbits
	.align	1
	.global	get_special_data_from_oob
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_special_data_from_oob, %function
get_special_data_from_oob:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r2, #4
	movs	r1, #1
	b	get_spare_data
	.size	get_special_data_from_oob, .-get_special_data_from_oob
	.section	.text._nftl_nand_copy_page,"ax",%progbits
	.align	1
	.global	_nftl_nand_copy_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nftl_nand_copy_page, %function
_nftl_nand_copy_page:
	@ args = 4, pretend = 0, frame = 48
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	sub	sp, sp, #56
	mov	r8, r2
	ldrh	r2, [sp, #80]
	mov	r6, r1
	add	r5, sp, #8
	ldrh	r1, [r1]
	mov	r4, r0
	strh	r2, [sp, #24]	@ movhi
	strh	r2, [sp, #40]	@ movhi
	ldr	r2, [r0, #4]
	strh	r1, [sp, #26]	@ movhi
	ldrh	r1, [r8]
	ldrh	r2, [r2, #28]
	str	r3, [sp, #32]
	strh	r1, [sp, #42]	@ movhi
	add	r1, sp, #24
	strh	r2, [sp, #28]	@ movhi
	strh	r2, [sp, #44]	@ movhi
	str	r3, [sp, #48]
	str	r5, [sp, #36]
	ldr	r3, [r0, #1984]
	str	r5, [sp, #52]
	blx	r3
	cmp	r0, #10
	ite	ne
	movne	r7, r0
	moveq	r7, #0
	mov	r0, r5
	bl	get_special_data_from_oob
	ldrh	r3, [r8, #6]
	mov	r2, r0
	mov	r1, r5
	mov	r0, r4
	adds	r3, r3, #1
	uxth	r3, r3
	str	r3, [sp]
	ldr	r3, [r6, #8]
	bl	set_oob_special_page
	ldr	r3, [r4, #1988]
	add	r1, sp, #40
	mov	r0, r4
	blx	r3
	orr	r0, r7, r0, lsl #8
	add	sp, sp, #56
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
	.size	_nftl_nand_copy_page, .-_nftl_nand_copy_page
	.section	.text.get_erase_count_from_oob,"ax",%progbits
	.align	1
	.global	get_erase_count_from_oob
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_erase_count_from_oob, %function
get_erase_count_from_oob:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	movs	r2, #2
	movs	r1, #5
	bl	get_spare_data
	uxth	r0, r0
	pop	{r3, pc}
	.size	get_erase_count_from_oob, .-get_erase_count_from_oob
	.section	.text.is_ftl_start_flag_page,"ax",%progbits
	.align	1
	.global	is_ftl_start_flag_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_ftl_start_flag_page, %function
is_ftl_start_flag_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldrb	r3, [r0, #1]	@ zero_extendqisi2
	cmp	r3, #170
	bne	.L1390
	ldrb	r0, [r0, #2]	@ zero_extendqisi2
	sub	r3, r0, #238
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	bx	lr
.L1390:
	movs	r0, #0
	bx	lr
	.size	is_ftl_start_flag_page, .-is_ftl_start_flag_page
	.section	.text.is_no_use_device,"ax",%progbits
	.align	1
	.global	is_no_use_device
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_no_use_device, %function
is_no_use_device:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #24]
	push	{r4, r5, r6, r7, r8, lr}
	mov	r4, r0
	sub	sp, sp, #32
	mov	r6, r1
	ldr	r5, [r0, #40]
	cmp	r3, #0
	beq	.L1401
	ldr	r7, [r0, #4]
	ldr	r0, [r0, #32]
	ldrb	r1, [r7, #8]	@ zero_extendqisi2
	add	r0, r0, r3
	bl	__aeabi_uidiv
	ldrh	r1, [r7, #20]
	bl	__aeabi_uidiv
	mov	r7, r0
.L1392:
	movs	r3, #0
	mov	r8, r3
	strh	r3, [r4, #18]	@ movhi
.L1393:
	ldrh	r3, [r4, #18]
	cmp	r3, r7
	bcs	.L1408
	ldr	r3, [r4, #4]
	mov	r0, r4
	ldrh	r1, [r5]
	str	r5, [r4, #44]
	ldrh	r2, [r3, #28]
	ldr	r3, [r4, #284]
	strh	r1, [sp, #18]	@ movhi
	add	r1, sp, #16
	strh	r8, [sp, #16]	@ movhi
	str	r3, [sp, #24]
	strh	r2, [sp, #20]	@ movhi
	str	sp, [sp, #28]
	ldr	r3, [r4, #1996]
	blx	r3
	cmp	r0, #1
	bne	.L1394
	ldr	r3, [r4, #1984]
	add	r1, sp, #16
	mov	r0, r4
	blx	r3
	mov	r2, r0
	mov	r0, sp
	bl	is_ftl_start_flag_page
	cmp	r0, #1
	beq	.L1395
.L1408:
	movs	r0, #1
.L1391:
	add	sp, sp, #32
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L1401:
	mov	r7, #-1
	b	.L1392
.L1395:
	cbz	r2, .L1397
	cmp	r2, #10
	bne	.L1408
.L1397:
	movs	r2, #4
	movs	r1, #3
	mov	r0, sp
	bl	get_spare_data
	cmp	r0, r6
	beq	.L1399
	ldr	r0, .L1409
	bl	NAND_Print
	b	.L1408
.L1399:
	adds	r5, r5, #36
	movs	r0, #0
	str	r5, [r4, #40]
	b	.L1391
.L1394:
	movs	r3, #255
	mov	r1, r4
	strh	r3, [r5, #2]	@ movhi
	mov	r0, r5
	ldrh	r3, [r4, #18]
	adds	r3, r3, #1
	strh	r3, [r4, #18]	@ movhi
	bl	is_last_phy_block
	cmp	r0, #1
	beq	.L1408
	adds	r5, r5, #36
	b	.L1393
.L1410:
	.align	2
.L1409:
	.word	.LC178
	.size	is_no_use_device, .-is_no_use_device
	.section	.text.is_ftl_end_flag_page,"ax",%progbits
	.align	1
	.global	is_ftl_end_flag_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_ftl_end_flag_page, %function
is_ftl_end_flag_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldrb	r3, [r0, #1]	@ zero_extendqisi2
	cmp	r3, #170
	bne	.L1413
	ldrb	r0, [r0, #2]	@ zero_extendqisi2
	sub	r3, r0, #204
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	bx	lr
.L1413:
	movs	r0, #0
	bx	lr
	.size	is_ftl_end_flag_page, .-is_ftl_end_flag_page
	.section	.text.is_ftl_special_data,"ax",%progbits
	.align	1
	.global	is_ftl_special_data
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_ftl_special_data, %function
is_ftl_special_data:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldrb	r0, [r0, #1]	@ zero_extendqisi2
	sub	r3, r0, #170
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	bx	lr
	.size	is_ftl_special_data, .-is_ftl_special_data
	.section	.text.is_ftl_logic_page_data,"ax",%progbits
	.align	1
	.global	is_ftl_logic_page_data
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_ftl_logic_page_data, %function
is_ftl_logic_page_data:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldrb	r0, [r0, #1]	@ zero_extendqisi2
	and	r0, r0, #240
	sub	r3, r0, #192
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	bx	lr
	.size	is_ftl_logic_page_data, .-is_ftl_logic_page_data
	.section	.text.is_nouse_page,"ax",%progbits
	.align	1
	.global	is_nouse_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_nouse_page, %function
is_nouse_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	subs	r2, r0, #1
	add	r1, r0, #9
	movs	r3, #0
	push	{r4, lr}
.L1418:
	ldrb	r4, [r2, #1]!	@ zero_extendqisi2
	cmp	r4, #255
	it	eq
	addeq	r3, r3, #1
	cmp	r2, r1
	bne	.L1418
	cmp	r3, #8
	bls	.L1420
	ldrb	r0, [r0]	@ zero_extendqisi2
	sub	r3, r0, #255
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
.L1416:
	pop	{r4, pc}
.L1420:
	movs	r0, #0
	b	.L1416
	.size	is_nouse_page, .-is_nouse_page
	.section	.text.sandisk_A19_read_page_end,"ax",%progbits
	.align	1
	.global	sandisk_A19_read_page_end
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_A19_read_page_end, %function
sandisk_A19_read_page_end:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L1470
	push	{r0, r1, r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r4, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r5, r0
	mov	r0, r4
	bl	generic_read_page_end_not_retry
	adds	r3, r0, #2
	mov	r6, r0
	bne	.L1422
	ldr	r8, .L1470+28
	movs	r7, #0
	ldr	r0, .L1470+4
	bl	NAND_Print_DBG
	mov	r10, r8
.L1424:
	ldrb	r3, [r8]	@ zero_extendqisi2
	cmp	r3, r7
	bgt	.L1430
	ldr	r7, .L1470+8
	ldrb	r3, [r7]	@ zero_extendqisi2
	cmp	r3, #51
	beq	.L1431
	ldr	fp, .L1470+12
	mov	r8, #0
.L1432:
	ldrb	r3, [r10]	@ zero_extendqisi2
	cmp	r3, r8
	bgt	.L1438
	ldr	fp, .L1470+12
	mov	r8, #0
.L1439:
	ldrb	r3, [r10]	@ zero_extendqisi2
	cmp	r3, r8
	bgt	.L1445
.L1431:
	ldr	fp, .L1470+12
	mov	r8, #0
.L1446:
	ldrb	r3, [r10]	@ zero_extendqisi2
	cmp	r3, r8
	bgt	.L1460
.L1461:
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	sandisk_A19_exit_readretry
.L1422:
	mov	r0, r6
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1430:
	str	r7, [r5, #176]
	mov	r0, r5
	ldrh	r1, [r4, #8]
	bl	sandisk_A19_set_readretry
	mov	r6, r0
	cbz	r0, .L1425
	ldr	r0, .L1470+12
	bl	NAND_Print_DBG
.L1426:
	adds	r7, r7, #1
	b	.L1424
.L1425:
	mov	r0, r4
	bl	generic_read_page_start
	mov	r6, r0
	mov	r0, r4
	bl	generic_read_page_end_not_retry
	orrs	r6, r6, r0
	ldr	r0, [r4, #20]
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L1427
	ldr	r3, [r5, #176]
	mvn	r6, #1
	ldr	r1, [r5, #268]
	ldr	r0, .L1470+16
	str	r3, [sp, #4]
	ldr	r3, [r4, #8]
	str	r3, [sp]
	ldr	r3, [r4, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	b	.L1426
.L1427:
	cmp	r6, #10
	beq	.L1458
	cmp	r6, #0
	bne	.L1426
.L1459:
	ldr	r3, [r5, #176]
	movs	r6, #10
	ldr	r1, [r5, #268]
	ldr	r0, .L1470+20
	str	r3, [sp, #4]
	ldr	r3, [r4, #8]
	str	r3, [sp]
	ldr	r3, [r4, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	b	.L1461
.L1438:
	str	r8, [r5, #176]
	mov	r0, r5
	ldrh	r1, [r4, #8]
	bl	sandisk_A19_set_readretry
	mov	r6, r0
	cbz	r0, .L1433
	mov	r0, fp
	bl	NAND_Print_DBG
.L1434:
	add	r8, r8, #1
	b	.L1432
.L1433:
	mov	r0, r4
	bl	sandisk_A19_dsp_on
	mov	r0, r4
	bl	generic_read_page_start
	mov	r6, r0
	mov	r0, r4
	bl	generic_read_page_end_not_retry
	orrs	r6, r6, r0
	ldr	r0, [r4, #20]
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L1435
	ldr	r3, [r5, #176]
	mvn	r6, #1
	ldr	r1, [r5, #268]
	ldr	r0, .L1470+16
	str	r3, [sp, #4]
	ldr	r3, [r4, #8]
	str	r3, [sp]
	ldr	r3, [r4, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	b	.L1434
.L1435:
	cmp	r6, #10
	beq	.L1458
	cmp	r6, #0
	bne	.L1434
	b	.L1459
.L1445:
	str	r8, [r5, #176]
	mov	r0, r5
	ldrh	r1, [r4, #8]
	bl	sandisk_A19_set_readretry
	mov	r6, r0
	cbz	r0, .L1440
	mov	r0, fp
	bl	NAND_Print_DBG
.L1441:
	add	r8, r8, #1
	b	.L1439
.L1440:
	mov	r0, r4
	bl	sandisk_A19_cmd25
	mov	r0, r4
	bl	generic_read_page_start
	mov	r6, r0
	mov	r0, r4
	bl	generic_read_page_end_not_retry
	orrs	r6, r6, r0
	ldr	r0, [r4, #20]
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L1442
	ldr	r3, [r5, #176]
	mvn	r6, #1
	ldr	r1, [r5, #268]
	ldr	r0, .L1470+16
	str	r3, [sp, #4]
	ldr	r3, [r4, #8]
	str	r3, [sp]
	ldr	r3, [r4, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	b	.L1441
.L1442:
	cmp	r6, #10
	beq	.L1458
	cmp	r6, #0
	bne	.L1441
	b	.L1459
.L1460:
	str	r8, [r5, #176]
	mov	r0, r5
	ldrh	r1, [r4, #8]
	bl	sandisk_A19_set_readretry
	mov	r6, r0
	cbz	r0, .L1447
	mov	r0, fp
	bl	NAND_Print_DBG
.L1448:
	add	r8, r8, #1
	b	.L1446
.L1447:
	ldrb	r3, [r7]	@ zero_extendqisi2
	cmp	r3, #52
	bne	.L1449
	movs	r1, #198
.L1468:
	mov	r0, r4
	bl	sandisk_A19_set_lmflgfix_next
	mov	r0, r4
	bl	sandisk_A19_cmd25
	ldrb	r3, [r7]	@ zero_extendqisi2
	cmp	r3, #50
	beq	.L1452
	subs	r3, r3, #52
	cmp	r3, #1
	bhi	.L1453
.L1452:
	mov	r0, r4
	bl	sandisk_A19_dsp_on
.L1453:
	mov	r0, r4
	bl	generic_read_page_start
	mov	r6, r0
	mov	r0, r4
	bl	generic_read_page_end_not_retry
	ldrb	r3, [r7]	@ zero_extendqisi2
	orrs	r6, r6, r0
	cmp	r3, #52
	bne	.L1454
	movs	r1, #194
.L1469:
	mov	r0, r4
	bl	sandisk_A19_set_lmflgfix_next
	ldr	r0, [r4, #20]
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L1457
	ldr	r3, [r5, #176]
	mvn	r6, #1
	ldr	r1, [r5, #268]
	ldr	r0, .L1470+16
	str	r3, [sp, #4]
	ldr	r3, [r4, #8]
	str	r3, [sp]
	ldr	r3, [r4, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	b	.L1448
.L1449:
	cmp	r3, #53
	ite	eq
	moveq	r1, #26
	movne	r1, #192
	b	.L1468
.L1454:
	cmp	r3, #53
	ite	eq
	moveq	r1, #10
	movne	r1, #64
	b	.L1469
.L1457:
	cmp	r6, #10
	beq	.L1458
	cmp	r6, #0
	bne	.L1448
	b	.L1459
.L1458:
	ldr	r3, [r5, #176]
	ldr	r1, [r5, #268]
	ldr	r0, .L1470+24
	str	r3, [sp, #4]
	ldr	r3, [r4, #8]
	str	r3, [sp]
	ldr	r3, [r4, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	b	.L1461
.L1471:
	.align	2
.L1470:
	.word	g_nsi
	.word	.LC179
	.word	.LANCHOR1
	.word	.LC180
	.word	.LC181
	.word	.LC182
	.word	.LC183
	.word	.LANCHOR2
	.size	sandisk_A19_read_page_end, .-sandisk_A19_read_page_end
	.section	.text.hynix20nm_read_page_end,"ax",%progbits
	.align	1
	.global	hynix20nm_read_page_end
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	hynix20nm_read_page_end, %function
hynix20nm_read_page_end:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L1488
	push	{r0, r1, r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r6, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r5, r0
	mov	r0, r6
	bl	generic_read_page_end_not_retry
	adds	r3, r0, #2
	mov	r4, r0
	bne	.L1472
	ldr	r8, .L1488+16
	movs	r7, #0
	ldr	r10, .L1488+20
	ldr	r0, .L1488+4
	bl	NAND_Print_DBG
.L1474:
	ldrb	r2, [r8]	@ zero_extendqisi2
	cmp	r2, r7
	ble	.L1478
	ldr	r3, [r5, #176]
	mov	r0, r5
	adds	r3, r3, #1
	cmp	r3, r2
	it	hi
	movhi	r3, #0
	str	r3, [r5, #176]
	bl	hynix20nm_set_readretry
	mov	r4, r0
	cbz	r0, .L1477
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	hynix20nm_set_readretry
.L1478:
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	hynix20nm_set_readretry
.L1472:
	mov	r0, r4
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1477:
	mov	r0, r6
	bl	generic_read_page_start
	mov	r4, r0
	mov	r0, r6
	bl	generic_read_page_end_not_retry
	ldr	fp, [r6, #20]
	orrs	r4, r4, r0
	mov	r0, fp
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L1479
	ldr	r3, [r5, #176]
	mvn	r4, #1
	ldr	r1, [r5, #268]
	mov	r0, r10
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
.L1480:
	adds	r7, r7, #1
	b	.L1474
.L1479:
	cmp	r4, #10
	bne	.L1481
.L1482:
	ldrb	r0, [fp, #4]	@ zero_extendqisi2
	movs	r4, #10
	ldrb	r3, [fp, #3]	@ zero_extendqisi2
	ldrb	r2, [fp, #2]	@ zero_extendqisi2
	ldrb	r1, [fp, #1]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L1488+8
	bl	NAND_Print_DBG
	ldr	r3, [r5, #176]
	ldr	r1, [r5, #268]
	ldr	r0, .L1488+12
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	b	.L1478
.L1481:
	cmp	r4, #0
	bne	.L1480
	b	.L1482
.L1489:
	.align	2
.L1488:
	.word	g_nsi
	.word	.LC184
	.word	.LC185
	.word	.LC186
	.word	.LANCHOR12
	.word	.LC181
	.size	hynix20nm_read_page_end, .-hynix20nm_read_page_end
	.section	.text.sandisk_read_page_end,"ax",%progbits
	.align	1
	.global	sandisk_read_page_end
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	sandisk_read_page_end, %function
sandisk_read_page_end:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L1501
	push	{r0, r1, r2, r4, r5, r6, r7, r8, r10, lr}
	mov	r6, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r5, r0
	mov	r0, r6
	bl	generic_read_page_end_not_retry
	adds	r3, r0, #2
	mov	r4, r0
	bne	.L1490
	ldr	r8, .L1501+12
	movs	r7, #0
	ldr	r10, .L1501+16
	ldr	r0, .L1501+4
	bl	NAND_Print_DBG
.L1492:
	ldrb	r3, [r8]	@ zero_extendqisi2
	cmp	r3, r7
	ble	.L1496
	str	r7, [r5, #176]
	mov	r0, r5
	ldrh	r1, [r6, #8]
	bl	sandisk_set_readretry
	mov	r4, r0
	cbnz	r0, .L1493
	mov	r0, r6
	bl	generic_read_page_start
	mov	r4, r0
	mov	r0, r6
	bl	generic_read_page_end_not_retry
	orrs	r4, r4, r0
	mov	r0, r5
	bl	sandisk_end_one_read_retry
	ldr	r0, [r6, #20]
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L1494
	ldr	r3, [r5, #176]
	mvn	r4, #1
	ldr	r1, [r5, #268]
	mov	r0, r10
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
.L1493:
	adds	r7, r7, #1
	b	.L1492
.L1494:
	cmp	r4, #10
	beq	.L1495
	cmp	r4, #0
	bne	.L1493
.L1495:
	ldr	r3, [r5, #176]
	movs	r4, #10
	ldr	r1, [r5, #268]
	ldr	r0, .L1501+8
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
.L1496:
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	sandisk_exit_readretry
.L1490:
	mov	r0, r4
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L1502:
	.align	2
.L1501:
	.word	g_nsi
	.word	.LC187
	.word	.LC188
	.word	.LANCHOR69
	.word	.LC181
	.size	sandisk_read_page_end, .-sandisk_read_page_end
	.section	.text.block_last_page_ecc_error,"ax",%progbits
	.align	1
	.global	block_last_page_ecc_error
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	block_last_page_ecc_error, %function
block_last_page_ecc_error:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r10, r0
	sub	sp, sp, #32
	ldr	r0, .L1535
	mov	r4, r1
	bl	NAND_Print
	mov	r0, #4096
	bl	NAND_Malloc
	mov	r5, r0
	cbnz	r0, .L1504
	ldr	r1, .L1535+4
	ldr	r0, .L1535+8
	bl	NAND_Print
.L1504:
	ldr	r7, .L1535+4
	movs	r6, #0
	ldr	r8, .L1535+28
.L1505:
	ldr	r2, [r10, #4]
	ldrh	r1, [r2, #20]
	cmp	r1, r6
	bgt	.L1507
	movs	r6, #0
	mov	r7, r6
.L1508:
	ldr	r2, [r10, #4]
	uxth	r8, r7
	ldrh	ip, [r4]
	ldr	r1, [r10, #284]
	ldrh	r0, [r2, #28]
	ldrh	r2, [r2, #20]
	cmp	r2, r7
	ble	.L1520
	strh	ip, [sp, #18]	@ movhi
	strh	r0, [sp, #20]	@ movhi
	mov	r0, r10
	str	r1, [sp, #24]
	add	r1, sp, #16
	ldr	r2, [r10, #1984]
	strh	r8, [sp, #16]	@ movhi
	str	sp, [sp, #28]
	blx	r2
	mov	fp, r0
	mov	r0, sp
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L1509
.L1520:
	ldr	r2, [r10, #4]
	ldrh	r0, [r4]
	strh	r8, [sp, #16]	@ movhi
	ldrh	r1, [r2, #28]
	ldr	r2, [r10, #284]
	strh	r0, [sp, #18]	@ movhi
	mov	r0, r10
	strh	r1, [sp, #20]	@ movhi
	add	r1, sp, #16
	str	r2, [sp, #24]
	str	sp, [sp, #28]
	ldr	r2, [r10, #1980]
	blx	r2
	cmp	r0, #0
	bne	.L1510
.L1511:
	movs	r7, #0
.L1523:
	cmp	r7, r6
	ldr	r2, [r10, #4]
	beq	.L1522
	ldrh	r0, [r4]
	ldrh	r1, [r2, #28]
	ldr	r2, [r5, r7, lsl #2]
	strh	r7, [sp, #16]	@ movhi
	adds	r7, r7, #1
	strh	r0, [sp, #18]	@ movhi
	mov	r0, r10
	strh	r1, [sp, #20]	@ movhi
	add	r1, sp, #16
	str	r2, [sp, #24]
	str	sp, [sp, #28]
	ldr	r2, [r10, #1988]
	blx	r2
	b	.L1523
.L1507:
	ldrh	r0, [r2, #26]
	bl	nftl_malloc
	str	r0, [r5, r6, lsl #2]
	cbnz	r0, .L1506
	mov	r1, r7
	mov	r0, r8
	bl	NAND_Print
.L1506:
	adds	r6, r6, #1
	b	.L1505
.L1509:
	mov	r0, sp
	bl	is_ftl_logic_page_data
	cmp	r0, #1
	bne	.L1512
	cmp	fp, #0
	beq	.L1513
	cmp	fp, #10
	bne	.L1514
.L1513:
	ldr	r2, [r10, #4]
	adds	r6, r6, #1
	ldr	r1, [r10, #284]
	ldr	r0, [r5, r7, lsl #2]
	ldrh	r2, [r2, #26]
	bl	NAND_Memcpy
.L1514:
	adds	r7, r7, #1
	b	.L1508
.L1512:
	ldrb	r1, [sp, #1]	@ zero_extendqisi2
	cmp	r1, #170
	bne	.L1515
	cmp	fp, #0
	bne	.L1533
.L1516:
	ldr	r2, [r10, #4]
	adds	r6, r6, #1
	ldr	r1, [r10, #284]
	ldr	r0, [r5, r7, lsl #2]
	ldrh	r2, [r2, #26]
	bl	NAND_Memcpy
	b	.L1518
.L1515:
	ldr	r0, .L1535+12
.L1534:
	bl	NAND_Print
	b	.L1514
.L1533:
	cmp	fp, #10
	beq	.L1516
.L1518:
	ldr	r0, .L1535+16
	b	.L1534
.L1510:
	ldr	r2, [r10, #2000]
	add	r1, sp, #16
	mov	r0, r10
	blx	r2
	mov	r0, r10
	bl	out_phy_block_from_free_list
	mov	r4, r0
	cbnz	r0, .L1521
	ldr	r0, .L1535+20
	bl	NAND_Print
.L1522:
	movs	r6, #0
.L1525:
	ldr	r2, [r10, #4]
	ldrh	r2, [r2, #20]
	cmp	r2, r6
	bgt	.L1526
	mov	r0, r5
	bl	nftl_free
	ldr	r0, .L1535+24
	bl	NAND_Print
	mov	r0, r4
	add	sp, sp, #32
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1521:
	ldr	r2, [r10, #4]
	ldrh	r0, [r0]
	strh	r8, [sp, #16]	@ movhi
	ldrh	r1, [r2, #28]
	ldr	r2, [r10, #284]
	strh	r0, [sp, #18]	@ movhi
	mov	r0, r10
	strh	r1, [sp, #20]	@ movhi
	add	r1, sp, #16
	str	r2, [sp, #24]
	str	sp, [sp, #28]
	ldr	r2, [r10, #1980]
	blx	r2
	b	.L1511
.L1526:
	ldr	r0, [r5, r6, lsl #2]
	adds	r6, r6, #1
	bl	nftl_free
	b	.L1525
.L1536:
	.align	2
.L1535:
	.word	.LC189
	.word	.LANCHOR98
	.word	.LC190
	.word	.LC192
	.word	.LC193
	.word	.LC194
	.word	.LC195
	.word	.LC191
	.size	block_last_page_ecc_error, .-block_last_page_ecc_error
	.section	.text.is_fill_page,"ax",%progbits
	.align	1
	.global	is_fill_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_fill_page, %function
is_fill_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	bl	get_special_data_from_oob
	sub	r3, r0, #1431655765
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	pop	{r3, pc}
	.size	is_fill_page, .-is_fill_page
	.section	.text.is_phy_mapping_page,"ax",%progbits
	.align	1
	.global	is_phy_mapping_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_phy_mapping_page, %function
is_phy_mapping_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	bl	get_special_data_from_oob
	ldr	r3, .L1539
	subs	r3, r0, r3
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	pop	{r3, pc}
.L1540:
	.align	2
.L1539:
	.word	-1431633921
	.size	is_phy_mapping_page, .-is_phy_mapping_page
	.section	.text.is_function_info_page,"ax",%progbits
	.align	1
	.global	is_function_info_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_function_info_page, %function
is_function_info_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	bl	get_special_data_from_oob
	lsrs	r0, r0, #16
	ldr	r3, .L1542
	lsls	r0, r0, #16
	subs	r3, r0, r3
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	pop	{r3, pc}
.L1543:
	.align	2
.L1542:
	.word	-1441726464
	.size	is_function_info_page, .-is_function_info_page
	.section	.text.is_power_down_info_page,"ax",%progbits
	.align	1
	.global	is_power_down_info_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_power_down_info_page, %function
is_power_down_info_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	bl	get_special_data_from_oob
	ldr	r3, .L1545
	subs	r3, r0, r3
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	pop	{r3, pc}
.L1546:
	.align	2
.L1545:
	.word	-1441713357
	.size	is_power_down_info_page, .-is_power_down_info_page
	.section	.text.get_logic_page_map,"ax",%progbits
	.align	1
	.global	get_logic_page_map
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_logic_page_map, %function
get_logic_page_map:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	mov	r2, r1
	ldr	r3, [r0, #28]
	cmp	r3, r1
	bhi	.L1548
	ldr	r1, .L1550
	ldr	r0, .L1550+4
	bl	NAND_Print
	movs	r0, #0
.L1547:
	pop	{r3, pc}
.L1548:
	lsrs	r1, r1, #18
	add	r0, r0, r1, lsl #2
	rsb	r1, r1, r1, lsl #14
	ldr	r0, [r0, #52]
	add	r2, r2, r1, lsl #18
	add	r0, r0, r2, lsl #2
	b	.L1547
.L1551:
	.align	2
.L1550:
	.word	.LANCHOR99
	.word	.LC196
	.size	get_logic_page_map, .-get_logic_page_map
	.section	.text.recover_logic_page_mapping,"ax",%progbits
	.align	1
	.global	recover_logic_page_mapping
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	recover_logic_page_mapping, %function
recover_logic_page_mapping:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r6, r0
	mov	r4, r1
	mov	fp, r2
	mov	r10, r3
	movs	r5, #0
.L1553:
	cmp	r5, r10
	bne	.L1563
	movs	r0, #0
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1563:
	ldr	r3, [fp, r5, lsl #2]
	ldr	r2, [r6, #28]
	cmp	r2, r3
	bls	.L1554
	mov	r1, r3
	mov	r0, r6
	bl	get_logic_page_map
	mov	r7, r0
	cbnz	r0, .L1555
	ldr	r0, .L1567
	bl	NAND_Print
	ldrh	r3, [r4, #4]
	adds	r3, r3, #1
	strh	r3, [r4, #4]	@ movhi
.L1556:
	adds	r5, r5, #1
	b	.L1553
.L1555:
	ldrh	r3, [r0, #2]
	movw	r2, #65535
	cmp	r3, r2
	beq	.L1558
	ldr	r2, [r6, #4]
	mov	r8, #36
	mul	r8, r8, r3
	ldr	r2, [r2, #32]
	add	r3, r2, r8
	cmp	r4, r3
	beq	.L1559
	mov	r1, r3
	mov	r0, r6
	str	r2, [sp, #4]
	str	r3, [sp]
	bl	phy_block_from_invalid_page_incr
	ldrd	r3, r2, [sp]
.L1560:
	ldr	r0, [r6, #4]
	ldrh	r1, [r3, #4]
	ldrh	r3, [r0, #20]
	cmp	r1, r3
	bls	.L1558
	ldrh	r1, [r2, r8]
	ldr	r0, .L1567+4
	bl	NAND_Print
.L1558:
	ldrh	r3, [r7]
	bfi	r3, r5, #0, #12
	strh	r3, [r7]	@ movhi
	ldrh	r3, [r4]
	strh	r3, [r7, #2]	@ movhi
	b	.L1556
.L1559:
	ldrh	r1, [r4, #4]
	adds	r1, r1, #1
	strh	r1, [r4, #4]	@ movhi
	b	.L1560
.L1554:
	adds	r2, r3, #1
	beq	.L1562
	mov	r2, r5
	ldrh	r1, [r4]
	ldr	r0, .L1567+8
	bl	NAND_Print
.L1562:
	ldrh	r3, [r4, #4]
	ldr	r2, [r6, #4]
	adds	r3, r3, #1
	ldrh	r2, [r2, #20]
	uxth	r3, r3
	strh	r3, [r4, #4]	@ movhi
	cmp	r2, r3
	bcs	.L1556
	ldr	r0, .L1567+12
	bl	NAND_Print
	b	.L1556
.L1568:
	.align	2
.L1567:
	.word	.LC197
	.word	.LC198
	.word	.LC199
	.word	.LC200
	.size	recover_logic_page_mapping, .-recover_logic_page_mapping
	.section	.text.malloc_logic_page_map,"ax",%progbits
	.align	1
	.global	malloc_logic_page_map
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	malloc_logic_page_map, %function
malloc_logic_page_map:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	add	r7, r0, #52
	mov	r4, r1
	mov	r6, r0
	add	r2, r0, #152
	mov	r3, r7
	movs	r1, #0
.L1570:
	str	r1, [r3], #4
	cmp	r3, r2
	bne	.L1570
	ldr	r8, .L1582+16
	movs	r5, #0
	mov	r2, #262144
	mov	r1, r4
	ldr	r0, .L1582
	bl	NAND_Print
.L1574:
	cmp	r4, r8
	bls	.L1571
	mov	r0, #1048576
	bl	NAND_Malloc
	str	r0, [r7], #4
	cbnz	r0, .L1572
	ldr	r0, .L1582+4
	mov	r1, r5
.L1581:
	bl	NAND_Print
	mvn	r0, #1
.L1569:
	pop	{r4, r5, r6, r7, r8, pc}
.L1572:
	mov	r2, #1048576
	movs	r1, #255
	adds	r5, r5, #1
	sub	r4, r4, #262144
	bl	NAND_Memset
	cmp	r5, #25
	bne	.L1574
	cbz	r4, .L1577
	mov	r1, r4
	ldr	r0, .L1582+8
	b	.L1581
.L1571:
	lsls	r4, r4, #2
	mov	r0, r4
	bl	nftl_malloc
	add	r3, r6, r5, lsl #2
	str	r0, [r3, #52]
	cbnz	r0, .L1576
	mov	r1, r5
	ldr	r0, .L1582+12
	b	.L1581
.L1576:
	mov	r2, r4
	movs	r1, #255
	bl	NAND_Memset
	movs	r0, #0
	b	.L1569
.L1577:
	mov	r0, r4
	b	.L1569
.L1583:
	.align	2
.L1582:
	.word	.LC201
	.word	.LC202
	.word	.LC204
	.word	.LC203
	.word	262143
	.size	malloc_logic_page_map, .-malloc_logic_page_map
	.section	.text.zone_param_init,"ax",%progbits
	.align	1
	.global	zone_param_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	zone_param_init, %function
zone_param_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	mov	r7, r3
	ldr	r3, [r0, #4]
	mov	r4, r0
	mov	r5, r1
	mov	r6, r2
	ldrh	r0, [r3, #26]
	bl	nftl_malloc
	str	r0, [r4, #284]
	cbnz	r0, .L1585
	ldr	r1, .L1596
	ldr	r0, .L1596+4
.L1594:
	bl	NAND_Print
	b	.L1595
.L1585:
	ldr	r3, [r4, #4]
	ldrh	r0, [r3, #26]
	bl	nftl_malloc
	str	r0, [r4, #288]
	cbnz	r0, .L1587
	ldr	r1, .L1596
	ldr	r0, .L1596+8
	b	.L1594
.L1587:
	ldr	r2, [r4, #4]
	movs	r1, #36
	strh	r5, [r4, #36]	@ movhi
	mov	r0, r6
	str	r6, [r4, #24]
	ldr	r3, [r2, #32]
	str	r7, [r4, #32]
	mla	r5, r1, r5, r3
	movs	r3, #0
	strh	r3, [r4, #18]	@ movhi
	str	r5, [r4, #40]
	strh	r3, [r4, #20]	@ movhi
	strh	r3, [r4, #22]	@ movhi
	str	r3, [r4, #44]
	ldrb	r1, [r2, #8]	@ zero_extendqisi2
	bl	__aeabi_uidiv
	mov	r1, r0
	mov	r0, r4
	bl	malloc_logic_page_map
	mov	r5, r0
	cbz	r0, .L1588
	ldr	r0, .L1596+12
	bl	NAND_Print
.L1595:
	mvn	r5, #1
.L1584:
	mov	r0, r5
	pop	{r3, r4, r5, r6, r7, pc}
.L1588:
	ldr	r3, [r4, #4]
	ldrh	r0, [r3, #20]
	mov	r3, #320
	str	r3, [r4, #316]
	lsls	r0, r0, #2
	str	r0, [r4, #308]
	add	r0, r0, r3
	str	r0, [r4, #300]
	bl	nftl_malloc
	mov	r3, r0
	str	r0, [r4, #296]
	cbnz	r0, .L1589
	ldr	r1, .L1596
	ldr	r0, .L1596+16
	b	.L1594
.L1589:
	ldr	r2, [r4, #308]
	movs	r1, #255
	str	r0, [r4, #304]
	add	r3, r3, r2
	ldr	r2, [r4, #300]
	str	r3, [r4, #312]
	str	r3, [r4, #1968]
	bl	NAND_Memset
	ldr	r3, [r4, #308]
	ldr	r0, [r4, #300]
	str	r3, [r4, #340]
	str	r0, [r4, #332]
	bl	nftl_malloc
	mov	r3, r0
	str	r0, [r4, #328]
	cbnz	r0, .L1590
	ldr	r1, .L1596
	ldr	r0, .L1596+20
	b	.L1594
.L1590:
	ldr	r2, [r4, #340]
	movs	r1, #255
	str	r0, [r4, #336]
	add	r3, r3, r2
	ldr	r2, [r4, #332]
	str	r3, [r4, #344]
	bl	NAND_Memset
	ldr	r3, [r4, #308]
	ldr	r0, [r4, #300]
	str	r3, [r4, #272]
	ldr	r3, [r4, #316]
	str	r0, [r4, #264]
	str	r3, [r4, #280]
	bl	nftl_malloc
	str	r0, [r4, #260]
	cbnz	r0, .L1591
	ldr	r1, .L1596
	ldr	r0, .L1596+24
	b	.L1594
.L1591:
	ldr	r3, [r4, #272]
	mov	r1, r5
	str	r0, [r4, #268]
	str	r5, [r4, #356]
	add	r0, r0, r3
	ldr	r3, [r4, #4]
	str	r0, [r4, #276]
	str	r5, [r4, #376]
	ldrh	r2, [r3, #16]
	strd	r5, r5, [r4, #360]
	strd	r5, r5, [r4, #368]
	strd	r5, r5, [r4, #576]
	lsrs	r3, r2, #1
	str	r5, [r4, #292]
	str	r5, [r4, #324]
	strh	r3, [r4, #572]	@ movhi
	ldr	r3, .L1596+28
	strd	r5, r5, [r4, #172]
	strd	r5, r5, [r4, #164]
	strd	r5, r5, [r4, #180]
	umull	r2, r3, r2, r3
	mov	r2, r4
	strd	r5, r5, [r4, #208]
	strd	r5, r5, [r4, #200]
	strd	r5, r5, [r4, #216]
	lsrs	r3, r3, #2
	strh	r3, [r4, #574]	@ movhi
	movw	r3, #65535
	strh	r3, [r4, #320]	@ movhi
	strh	r3, [r4, #352]	@ movhi
	mov	r3, r5
	strd	r5, r5, [r4, #244]
	strd	r5, r5, [r4, #236]
	strd	r5, r5, [r4, #252]
	strh	r5, [r4, #392]	@ movhi
.L1592:
	strh	r1, [r2, #396]	@ movhi
	adds	r1, r1, #1
	cmp	r1, #10
	strh	r3, [r2, #398]	@ movhi
	strd	r3, r3, [r2, #400]
	add	r2, r2, #16
	str	r3, [r2, #392]
	bne	.L1592
	movs	r2, #255
	strh	r3, [r4, #394]	@ movhi
	strd	r2, r3, [r4, #556]
	strd	r3, r3, [r4, #564]
	b	.L1584
.L1597:
	.align	2
.L1596:
	.word	.LANCHOR100
	.word	.LC205
	.word	.LC206
	.word	.LC207
	.word	.LC208
	.word	.LC209
	.word	.LC210
	.word	-1431655765
	.size	zone_param_init, .-zone_param_init
	.section	.text.free_logic_page_map,"ax",%progbits
	.align	1
	.global	free_logic_page_map
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	free_logic_page_map, %function
free_logic_page_map:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	add	r5, r0, #52
	add	r4, r0, #152
.L1600:
	ldr	r0, [r5], #4
	cbz	r0, .L1599
	bl	nftl_free
.L1599:
	cmp	r5, r4
	bne	.L1600
	movs	r0, #0
	pop	{r3, r4, r5, pc}
	.size	free_logic_page_map, .-free_logic_page_map
	.section	.text.zone_param_exit,"ax",%progbits
	.align	1
	.global	zone_param_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	zone_param_exit, %function
zone_param_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	bl	free_logic_page_map
	ldr	r0, [r4, #260]
	bl	nftl_free
	ldr	r0, [r4, #296]
	bl	nftl_free
	ldr	r0, [r4, #328]
	bl	nftl_free
	ldr	r0, [r4, #284]
	bl	nftl_free
	ldr	r0, [r4, #288]
	bl	nftl_free
	movs	r0, #0
	pop	{r4, pc}
	.size	zone_param_exit, .-zone_param_exit
	.section	.text.set_physic_op_par,"ax",%progbits
	.align	1
	.global	set_physic_op_par
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	set_physic_op_par, %function
set_physic_op_par:
	@ args = 8, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	strh	r3, [r0, #4]	@ movhi
	ldr	r3, [sp]
	strh	r2, [r0]	@ movhi
	strh	r1, [r0, #2]	@ movhi
	str	r3, [r0, #8]
	ldr	r3, [sp, #4]
	str	r3, [r0, #12]
	bx	lr
	.size	set_physic_op_par, .-set_physic_op_par
	.section	.text.nftl_sector_cache_init,"ax",%progbits
	.align	1
	.global	nftl_sector_cache_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_sector_cache_init, %function
nftl_sector_cache_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	movs	r3, #16
	movs	r5, #0
	mov	r4, r0
	str	r3, [r0, #1620]
	str	r5, [r0, #1624]
	mov	r0, #8192
	bl	NAND_Malloc
	str	r0, [r4, #1624]
	cbnz	r0, .L1608
	ldr	r1, .L1612
	ldr	r0, .L1612+4
	bl	NAND_Print
	movs	r0, #1
.L1607:
	pop	{r4, r5, r6, pc}
.L1608:
	mov	r2, r5
	str	r5, [r4, #1960]
	str	r5, [r4, #1964]
	mov	r3, r4
	movs	r6, #255
	mov	r5, #-1
	mov	r1, r2
.L1610:
	strh	r2, [r3, #1628]	@ movhi
	add	r4, r0, r2, lsl #9
	adds	r2, r2, #1
	strh	r6, [r3, #1630]	@ movhi
	cmp	r2, #16
	str	r5, [r3, #1632]
	str	r1, [r3, #1640]
	add	r3, r3, #20
	str	r1, [r3, #1624]
	str	r4, [r3, #1616]
	bne	.L1610
	movs	r0, #0
	b	.L1607
.L1613:
	.align	2
.L1612:
	.word	.LANCHOR101
	.word	.LC211
	.size	nftl_sector_cache_init, .-nftl_sector_cache_init
	.section	.text.nftl_sector_cache_exit,"ax",%progbits
	.align	1
	.global	nftl_sector_cache_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_sector_cache_exit, %function
nftl_sector_cache_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	ldr	r0, [r0, #1624]
	bl	nftl_free
	movs	r0, #0
	pop	{r3, pc}
	.size	nftl_sector_cache_exit, .-nftl_sector_cache_exit
	.section	.text.search_from_sector_cache_write_list,"ax",%progbits
	.align	1
	.global	search_from_sector_cache_write_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	search_from_sector_cache_write_list, %function
search_from_sector_cache_write_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, [r0, #340]
.L1616:
	cbnz	r0, .L1618
.L1615:
	bx	lr
.L1618:
	ldr	r3, [r0, #4]
	cmp	r3, r1
	beq	.L1615
	ldr	r0, [r0, #12]
	b	.L1616
	.size	search_from_sector_cache_write_list, .-search_from_sector_cache_write_list
	.section	.text.get_empty_sector_cahce_node,"ax",%progbits
	.align	1
	.global	get_empty_sector_cahce_node
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_empty_sector_cahce_node, %function
get_empty_sector_cahce_node:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, #0
	movs	r1, #20
	push	{r4, lr}
.L1622:
	mul	r2, r1, r3
	adds	r4, r0, r2
	ldrh	r4, [r4, #10]
	cmp	r4, #255
	bne	.L1620
	adds	r2, r2, #8
	add	r0, r0, r2
.L1619:
	pop	{r4, pc}
.L1620:
	adds	r3, r3, #1
	cmp	r3, #16
	bne	.L1622
	movs	r0, #0
	b	.L1619
	.size	get_empty_sector_cahce_node, .-get_empty_sector_cahce_node
	.section	.text.add_to_sector_cache_write_list_tail,"ax",%progbits
	.align	1
	.global	add_to_sector_cache_write_list_tail
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	add_to_sector_cache_write_list_tail, %function
add_to_sector_cache_write_list_tail:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	add	r2, r0, #328
.L1625:
	ldr	r3, [r2, #12]
	cbnz	r3, .L1626
	str	r1, [r2, #12]
	strd	r3, r2, [r1, #12]
	movs	r2, #1
	strh	r2, [r1, #2]	@ movhi
	ldrh	r2, [r0, #2]
	adds	r2, r2, #1
	strh	r2, [r0, #2]	@ movhi
	mov	r0, r3
	bx	lr
.L1626:
	mov	r2, r3
	b	.L1625
	.size	add_to_sector_cache_write_list_tail, .-add_to_sector_cache_write_list_tail
	.section	.text.del_from_sector_cache_write_list,"ax",%progbits
	.align	1
	.global	del_from_sector_cache_write_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	del_from_sector_cache_write_list, %function
del_from_sector_cache_write_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldrd	r3, r2, [r1, #12]
	str	r3, [r2, #12]
	ldr	r3, [r1, #12]
	cbz	r3, .L1628
	str	r2, [r3, #16]
.L1628:
	movs	r3, #0
	strd	r3, r3, [r1, #12]
	movs	r3, #255
	strh	r3, [r1, #2]	@ movhi
	ldrh	r3, [r0, #2]
	subs	r3, r3, #1
	strh	r3, [r0, #2]	@ movhi
	mov	r0, r1
	bx	lr
	.size	del_from_sector_cache_write_list, .-del_from_sector_cache_write_list
	.section	.text.get_empty_gc_node,"ax",%progbits
	.align	1
	.global	get_empty_gc_node
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_empty_gc_node, %function
get_empty_gc_node:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r3, #0
.L1635:
	lsls	r2, r3, #4
	adds	r1, r0, r2
	ldrh	r1, [r1, #6]
	cbnz	r1, .L1633
	adds	r2, r2, #4
	add	r0, r0, r2
	bx	lr
.L1633:
	adds	r3, r3, #1
	cmp	r3, #10
	bne	.L1635
	movs	r0, #0
	bx	lr
	.size	get_empty_gc_node, .-get_empty_gc_node
	.section	.text.search_gc_node,"ax",%progbits
	.align	1
	.global	search_gc_node
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	search_gc_node, %function
search_gc_node:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, [r0, #172]
.L1638:
	cbnz	r0, .L1640
.L1637:
	bx	lr
.L1640:
	ldr	r3, [r0, #4]
	cmp	r3, r1
	beq	.L1637
	ldr	r0, [r0, #8]
	b	.L1638
	.size	search_gc_node, .-search_gc_node
	.section	.text.add_to_gc_list_tail,"ax",%progbits
	.align	1
	.global	add_to_gc_list_tail
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	add_to_gc_list_tail, %function
add_to_gc_list_tail:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	add	r2, r0, #164
.L1642:
	ldr	r3, [r2, #8]
	cbnz	r3, .L1643
	str	r1, [r2, #8]
	strd	r3, r2, [r1, #8]
	ldrh	r2, [r0]
	adds	r2, r2, #1
	strh	r2, [r0]	@ movhi
	mov	r0, r3
	bx	lr
.L1643:
	mov	r2, r3
	b	.L1642
	.size	add_to_gc_list_tail, .-add_to_gc_list_tail
	.section	.text.del_from_gc_list,"ax",%progbits
	.align	1
	.global	del_from_gc_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	del_from_gc_list, %function
del_from_gc_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldrd	r3, r2, [r1, #8]
	str	r3, [r2, #8]
	ldr	r3, [r1, #8]
	cbz	r3, .L1645
	str	r2, [r3, #12]
.L1645:
	movs	r3, #0
	strd	r3, r3, [r1, #8]
	ldrh	r3, [r0]
	subs	r3, r3, #1
	strh	r3, [r0]	@ movhi
	mov	r0, r1
	bx	lr
	.size	del_from_gc_list, .-del_from_gc_list
	.section	.text.add_prio_gc,"ax",%progbits
	.align	1
	.global	add_prio_gc
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	add_prio_gc, %function
add_prio_gc:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	add	r5, r0, #392
	mov	r0, r5
	mov	r7, r1
	mov	r6, r2
	bl	search_gc_node
	mov	r4, r0
	cbnz	r0, .L1652
	mov	r0, r5
	bl	get_empty_gc_node
	mov	r1, r0
	cbz	r0, .L1652
	str	r7, [r0, #4]
	strh	r6, [r0, #2]	@ movhi
	strd	r4, r4, [r0, #8]
	mov	r0, r5
	bl	add_to_gc_list_tail
	mov	r0, r4
.L1649:
	pop	{r3, r4, r5, r6, r7, pc}
.L1652:
	movs	r0, #1
	b	.L1649
	.size	add_prio_gc, .-add_prio_gc
	.section	.text.first_scan_all_blocks,"ax",%progbits
	.align	1
	.global	first_scan_all_blocks
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	first_scan_all_blocks, %function
first_scan_all_blocks:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	sub	sp, sp, #88
	ldr	r4, [r0, #40]
	mov	r5, r0
	movs	r2, #5
	add	r0, sp, #72
	ldr	r1, .L1707
	add	r6, sp, #88
	bl	memcpy
	movs	r3, #47
	ldr	r1, .L1707+4
	strh	r3, [r6, #-32]!	@ movhi
	mov	r0, r6
	bl	nand_fdt_get_string
	cmp	r0, #0
	beq	.L1654
	movs	r2, #3
	add	r1, sp, #72
	bl	NAND_Memcmp
	mov	r7, r0
	cbnz	r0, .L1654
	ldr	r0, .L1707+8
	bl	NAND_Print_DBG
	strh	r7, [r5, #18]	@ movhi
.L1655:
	ldr	r3, [r5, #4]
	mov	r10, #0
	ldrh	r2, [r4]
	ldrh	r3, [r3, #24]
	cmp	r3, r2
	bls	.L1700
	strh	r2, [sp, #74]	@ movhi
	add	r1, sp, #72
	strh	r10, [sp, #72]	@ movhi
	mov	r0, r5
	strh	r10, [sp, #76]	@ movhi
	strd	r10, r6, [sp, #80]
	ldr	r2, [r5, #1996]
	blx	r2
	cmp	r0, #1
	bne	.L1657
	ldr	r2, [r5, #1984]
	add	r1, sp, #72
	mov	r0, r5
	blx	r2
	str	r4, [r5, #44]
	mov	r8, r0
	mov	r0, r6
	bl	get_block_used_count_from_oob
	mov	fp, r0
	mov	r0, r6
	bl	is_ftl_end_flag_page
	cmp	r0, #1
	bne	.L1658
	ldrh	r1, [r4]
	ldr	r0, .L1707+12
	bl	NAND_Print
.L1700:
	mov	r0, r10
	b	.L1653
.L1654:
	mov	r0, #-1
.L1653:
	add	sp, sp, #88
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1658:
	mov	r0, r6
	bl	is_panic_data_page
	cmp	r0, #1
	bne	.L1659
	ldrb	r2, [sp, #59]	@ zero_extendqisi2
	ldrb	r3, [r5, #16]	@ zero_extendqisi2
	cmp	r2, r3
	bne	.L1660
	mov	r1, r4
	mov	r0, r6
	bl	panic_recover_info
.L1660:
	adds	r4, r4, #36
	b	.L1655
.L1659:
	mov	r0, r6
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L1661
	ldrh	r2, [r5, #18]
	mov	r1, r4
	mov	r0, r5
	adds	r2, r2, #1
	strh	r2, [r5, #18]	@ movhi
	strh	r10, [r4, #2]	@ movhi
	strh	r10, [r4, #6]	@ movhi
	strh	r10, [r4, #4]	@ movhi
	bl	put_phy_block_to_free_list
	b	.L1660
.L1661:
	mov	r0, r6
	bl	is_ftl_logic_page_data
	cmp	r0, #1
	beq	.L1662
	mov	r0, r6
	bl	is_function_info_page
	cmp	r0, #1
	bne	.L1663
.L1662:
	cmp	r8, #0
	beq	.L1664
	cmp	r8, #10
	bne	.L1663
.L1664:
	ldrh	r3, [r5, #18]
	mov	r10, #1
	mov	r0, r6
	adds	r3, r3, #1
	strh	r3, [r5, #18]	@ movhi
	strh	r10, [r4, #2]	@ movhi
	bl	get_erase_count_from_oob
	movs	r3, #0
	strh	r0, [r4, #6]	@ movhi
	mov	r7, r0
	strh	r3, [r4, #4]	@ movhi
	str	fp, [r4, #8]
	mov	r1, r4
	strd	r3, r3, [r4, #28]
	mov	r0, r5
	bl	add_block_count_list
	cmp	r8, #10
	bne	.L1660
	ldr	r3, [r5, #1972]
	ldrh	r3, [r3, #4]
	cmp	r3, #0
	beq	.L1660
	mov	r2, r10
	mov	r0, r5
	bl	add_prio_gc
	ldr	r0, .L1707+16
	bl	NAND_Print_DBG
	b	.L1660
.L1663:
	mov	r0, r6
	bl	is_ftl_start_flag_page
	cmp	r0, #1
	bne	.L1665
	ldr	r0, .L1707+20
.L1699:
	bl	NAND_Print
	b	.L1660
.L1665:
	mov	r0, r6
	bl	is_fill_page
	cmp	r0, #1
	bne	.L1666
	ldrh	r3, [r5, #18]
	mov	r0, r6
	adds	r3, r3, #1
	strh	r3, [r5, #18]	@ movhi
	movs	r3, #0
	strh	r3, [r4, #2]	@ movhi
	bl	get_erase_count_from_oob
	ldr	r3, [r5, #4]
	mov	r1, r4
	strh	r0, [r4, #6]	@ movhi
	mov	r7, r0
	mov	r0, r5
	ldrh	r3, [r3, #20]
	strh	r3, [r4, #4]	@ movhi
	bl	put_phy_block_to_free_list
	ldrh	r1, [r4]
	ldr	r0, .L1707+24
	bl	NAND_Print_DBG
	b	.L1660
.L1666:
	ldrb	r0, [sp, #71]	@ zero_extendqisi2
	ldrh	r1, [r4]
	ldrb	r3, [sp, #57]	@ zero_extendqisi2
	str	r0, [sp, #52]
	ldrb	r0, [sp, #70]	@ zero_extendqisi2
	ldrb	r2, [sp, #56]	@ zero_extendqisi2
	str	r0, [sp, #48]
	ldrb	r0, [sp, #69]	@ zero_extendqisi2
	str	r0, [sp, #44]
	ldrb	r0, [sp, #68]	@ zero_extendqisi2
	str	r0, [sp, #40]
	ldrb	r0, [sp, #67]	@ zero_extendqisi2
	str	r0, [sp, #36]
	ldrb	r0, [sp, #66]	@ zero_extendqisi2
	str	r0, [sp, #32]
	ldrb	r0, [sp, #65]	@ zero_extendqisi2
	str	r0, [sp, #28]
	ldrb	r0, [sp, #64]	@ zero_extendqisi2
	str	r0, [sp, #24]
	ldrb	r0, [sp, #63]	@ zero_extendqisi2
	str	r0, [sp, #20]
	ldrb	r0, [sp, #62]	@ zero_extendqisi2
	str	r0, [sp, #16]
	ldrb	r0, [sp, #61]	@ zero_extendqisi2
	str	r0, [sp, #12]
	ldrb	r0, [sp, #60]	@ zero_extendqisi2
	str	r0, [sp, #8]
	ldrb	r0, [sp, #59]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [sp, #58]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L1707+28
	bl	NAND_Print
	uxth	r2, r7
	adds	r3, r2, #1
	strh	r3, [r4, #6]	@ movhi
	cmp	r8, #0
	beq	.L1667
	cmp	r8, #10
	bne	.L1668
.L1667:
	ldrb	r3, [sp, #56]	@ zero_extendqisi2
	cmp	r3, #255
	beq	.L1669
	ldrh	r1, [r4]
	ldr	r0, .L1707+32
	bl	NAND_Print
	movs	r3, #255
	strh	r3, [r4, #2]	@ movhi
.L1698:
	ldrh	r3, [r5, #20]
	adds	r3, r3, #1
	strh	r3, [r5, #20]	@ movhi
	b	.L1660
.L1669:
	adds	r2, r2, #2
.L1705:
	uxth	r2, r2
	mov	r1, r4
	mov	r0, r5
	bl	erase_block
.L1702:
	cmp	r0, #0
	bne	.L1660
	ldrh	r3, [r5, #18]
	adds	r3, r3, #1
	strh	r3, [r5, #18]	@ movhi
	b	.L1660
.L1668:
	ldrh	r1, [r4]
	ldr	r0, .L1707+36
	bl	NAND_Print
	ldr	r3, [r5, #4]
	add	r1, sp, #72
	ldrh	r2, [r4]
	mov	r0, r5
	ldrh	r3, [r3, #20]
	strh	r2, [sp, #74]	@ movhi
	subs	r3, r3, #1
	strh	r3, [sp, #72]	@ movhi
	movs	r3, #0
	strh	r3, [sp, #76]	@ movhi
	strd	r3, r6, [sp, #80]
	ldr	r3, [r5, #1984]
	blx	r3
	ldrh	r8, [r4]
	mov	r10, r0
	cbz	r0, .L1670
	cmp	r0, #10
	bne	.L1671
.L1670:
	mov	r0, r6
	bl	is_phy_mapping_page
	cmp	r0, #1
	mov	fp, r0
	bne	.L1672
	mov	r1, r8
	ldr	r0, .L1707+40
	bl	NAND_Print
	mov	r0, r6
	bl	get_block_used_count_from_oob
	ldrh	r2, [r5, #18]
	mov	r8, r0
	mov	r0, r6
	adds	r2, r2, #1
	strh	r2, [r5, #18]	@ movhi
	strh	fp, [r4, #2]	@ movhi
	bl	get_erase_count_from_oob
	movs	r2, #0
	strh	r0, [r4, #6]	@ movhi
	mov	r7, r0
	strh	r2, [r4, #4]	@ movhi
	str	r8, [r4, #8]
	mov	r1, r4
	strd	r2, r2, [r4, #28]
	mov	r0, r5
	bl	add_block_count_list
	cmp	r10, #10
	bne	.L1660
	ldr	r3, [r5, #1972]
	ldrh	r3, [r3, #4]
	cmp	r3, #0
	beq	.L1660
	mov	r2, fp
	mov	r0, r5
	bl	add_prio_gc
	ldr	r0, .L1707+44
	b	.L1699
.L1672:
	mov	r0, r6
	bl	is_nouse_page
	cmp	r0, #1
	mov	r1, r8
	ite	eq
	ldreq	r0, .L1707+48
	ldrne	r0, .L1707+52
	bl	NAND_Print
	ldrh	r2, [r4, #6]
	adds	r2, r2, #1
	b	.L1705
.L1708:
	.align	2
.L1707:
	.word	.LANCHOR76+9
	.word	.LANCHOR102
	.word	.LC212
	.word	.LC213
	.word	.LC214
	.word	.LC215
	.word	.LC216
	.word	.LC217
	.word	.LC218
	.word	.LC219
	.word	.LC220
	.word	.LC221
	.word	.LC222
	.word	.LC223
.L1671:
	mov	r1, r8
	ldr	r0, .L1709
	bl	NAND_Print
	ldrb	r0, [sp, #71]	@ zero_extendqisi2
	ldrh	r1, [r4]
	ldrb	r3, [sp, #57]	@ zero_extendqisi2
	str	r0, [sp, #52]
	ldrb	r0, [sp, #70]	@ zero_extendqisi2
	ldrb	r2, [sp, #56]	@ zero_extendqisi2
	str	r0, [sp, #48]
	ldrb	r0, [sp, #69]	@ zero_extendqisi2
	str	r0, [sp, #44]
	ldrb	r0, [sp, #68]	@ zero_extendqisi2
	str	r0, [sp, #40]
	ldrb	r0, [sp, #67]	@ zero_extendqisi2
	str	r0, [sp, #36]
	ldrb	r0, [sp, #66]	@ zero_extendqisi2
	str	r0, [sp, #32]
	ldrb	r0, [sp, #65]	@ zero_extendqisi2
	str	r0, [sp, #28]
	ldrb	r0, [sp, #64]	@ zero_extendqisi2
	str	r0, [sp, #24]
	ldrb	r0, [sp, #63]	@ zero_extendqisi2
	str	r0, [sp, #20]
	ldrb	r0, [sp, #62]	@ zero_extendqisi2
	str	r0, [sp, #16]
	ldrb	r0, [sp, #61]	@ zero_extendqisi2
	str	r0, [sp, #12]
	ldrb	r0, [sp, #60]	@ zero_extendqisi2
	str	r0, [sp, #8]
	ldrb	r0, [sp, #59]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [sp, #58]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L1709+4
	bl	NAND_Print
	mov	r1, r4
	mov	r0, r5
	bl	torture_block_and_mark
	b	.L1702
.L1657:
	movs	r2, #255
	strh	r10, [r4, #6]	@ movhi
	strh	r2, [r4, #2]	@ movhi
	b	.L1698
.L1710:
	.align	2
.L1709:
	.word	.LC224
	.word	.LC225
	.size	first_scan_all_blocks, .-first_scan_all_blocks
	.section	.text.check_static_wear_leveling,"ax",%progbits
	.align	1
	.global	check_static_wear_leveling
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	check_static_wear_leveling, %function
check_static_wear_leveling:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, r8, r10, lr}
	mov	r4, r0
	ldr	r3, [r0, #580]
	cbz	r3, .L1712
.L1714:
	movs	r0, #1
.L1711:
	pop	{r3, r4, r5, r6, r7, r8, r10, pc}
.L1712:
	ldr	r3, [r0, #200]
	cmp	r3, #0
	beq	.L1714
	ldr	r3, [r0, #172]
	cmp	r3, #0
	beq	.L1714
	ldr	r3, [r0, #292]
	ldrh	r7, [r0, #18]
	ldr	r8, [r3, #8]
	cmp	r8, r7
	blt	.L1714
	ldr	r3, [r0, #4]
	ldrh	r5, [r3, #16]
	mov	r1, r5
	bl	get_block_max_erase_time_from_free_list
	cmp	r0, #0
	beq	.L1714
	ldrh	r6, [r0, #6]
	ldr	r0, .L1732
	umull	r0, r1, r5, r0
	ubfx	r10, r1, #2, #16
	cmp	r6, r10
	bcc	.L1714
	sub	r2, r8, r7
	mov	r1, r5
	mov	r0, r4
	bl	get_block_min_erase_time_from_invalid_list
	mov	r7, r0
	cmp	r0, #0
	beq	.L1714
	ldrh	r8, [r0, #6]
	cmp	r6, r8
	bls	.L1714
	lsls	r2, r6, #1
	adds	r3, r2, r6
	cmp	r3, r5, lsl #1
	ble	.L1715
	ldr	r2, .L1732+4
.L1731:
	umull	r2, r3, r5, r2
	lsrs	r3, r3, #3
	strh	r3, [r4, #574]	@ movhi
.L1716:
	ldrh	r2, [r4, #574]
	sub	r3, r6, r8
	cmp	r3, r2
	blt	.L1714
	mov	r2, r8
	mov	r1, r6
	ldr	r0, .L1732+8
	bl	NAND_Print_DBG
	ldrh	r1, [r7]
	ldr	r0, .L1732+12
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #18]
	ldr	r0, .L1732+16
	bl	NAND_Print_DBG
	str	r7, [r4, #580]
	movs	r0, #0
	b	.L1711
.L1715:
	cmp	r5, r2
	bge	.L1717
	ldr	r2, .L1732
	b	.L1731
.L1717:
	cmp	r5, r3
	bge	.L1718
	movs	r1, #7
	mov	r0, r5
	bl	__aeabi_uidiv
	strh	r0, [r4, #574]	@ movhi
	b	.L1716
.L1718:
	strh	r10, [r4, #574]	@ movhi
	b	.L1716
.L1733:
	.align	2
.L1732:
	.word	-858993459
	.word	-1431655765
	.word	.LC226
	.word	.LC227
	.word	.LC228
	.size	check_static_wear_leveling, .-check_static_wear_leveling
	.section	.text.prio_gc_one,"ax",%progbits
	.align	1
	.global	prio_gc_one
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	prio_gc_one, %function
prio_gc_one:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #4]
	cmp	r2, #1
	push	{r4, lr}
	ldr	r4, [r3, #32]
	mov	r3, #36
	mla	r1, r3, r1, r4
	beq	.L1735
	cmp	r2, #2
	it	ne
	movne	r2, #3
.L1735:
	uxth	r2, r2
	bl	add_prio_gc
	movs	r0, #0
	pop	{r4, pc}
	.size	prio_gc_one, .-prio_gc_one
	.section	.text.get_total_invalid_page,"ax",%progbits
	.align	1
	.global	get_total_invalid_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_total_invalid_page, %function
get_total_invalid_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	add	r3, r0, #188
	movs	r0, #0
.L1742:
	ldr	r3, [r3, #12]
	cbnz	r3, .L1743
	bx	lr
.L1743:
	ldrh	r2, [r3, #4]
	add	r0, r0, r2
	b	.L1742
	.size	get_total_invalid_page, .-get_total_invalid_page
	.section	.text.is_current_blcok,"ax",%progbits
	.align	1
	.global	is_current_blcok
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	is_current_blcok, %function
is_current_blcok:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldrb	r3, [r0, #17]	@ zero_extendqisi2
	lsls	r2, r3, #31
	bpl	.L1745
	ldr	r3, [r0, #324]
	cmp	r1, r3
	beq	.L1747
.L1745:
	ldr	r0, [r0, #292]
	subs	r3, r0, r1
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	bx	lr
.L1747:
	movs	r0, #1
	bx	lr
	.size	is_current_blcok, .-is_current_blcok
	.section	.text.static_wear_leveling,"ax",%progbits
	.align	1
	.global	static_wear_leveling
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	static_wear_leveling, %function
static_wear_leveling:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldrh	r2, [r0, #22]
	ldrh	r3, [r0, #380]
	push	{r4, lr}
	mov	r4, r0
	cmp	r2, r3
	bls	.L1750
	ldr	r3, [r0, #368]
	cmp	r3, #19
	bls	.L1750
	ldr	r2, [r0, #1968]
	ldr	r3, [r2, #180]
	adds	r3, r3, #1
	str	r3, [r2, #180]
	bl	get_block_min_count_from_invalid_list
	movs	r2, #5
	mov	r1, r0
	mov	r0, r4
	bl	add_prio_gc
	cbnz	r0, .L1752
	ldr	r3, [r4, #372]
	adds	r3, r3, #4
	str	r3, [r4, #372]
.L1752:
	ldr	r3, [r4, #4]
	ldr	r2, [r4, #372]
	ldrh	r3, [r3, #20]
	cmp	r2, r3
	itt	cs
	movcs	r3, #0
	strdcs	r3, r3, [r4, #368]
.L1750:
	movs	r0, #0
	pop	{r4, pc}
	.size	static_wear_leveling, .-static_wear_leveling
	.section	.text.nftl_get_gc_info,"ax",%progbits
	.align	1
	.global	nftl_get_gc_info
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_get_gc_info, %function
nftl_get_gc_info:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r8, r2
	movs	r2, #0
	mov	r5, r0
	mov	r7, r1
	sub	sp, sp, #32
	ldr	ip, [r0, #4]
	mov	r6, r2
	ldr	r4, [r0, #200]
	mov	r1, r2
	mov	r0, r2
.L1754:
	cbnz	r4, .L1756
	ldr	ip, [r5, #1968]
	mov	fp, #10
	ldrh	r3, [r5, #22]
	strd	r1, r0, [sp, #16]
	mov	r1, r8
	strd	r6, r2, [sp, #24]
	mov	r0, r7
	ldr	r2, [ip, #124]
	ldr	r10, .L1767+4
	str	r2, [sp, #12]
	ldr	r2, [ip, #120]
	str	r2, [sp, #8]
	ldrh	r2, [r5, #382]
	str	r2, [sp, #4]
	ldrh	r2, [r5, #380]
	str	r2, [sp]
	ldr	r2, .L1767
	bl	NAND_Snprintf
	ldr	r5, [r5, #200]
	mov	r6, r0
.L1757:
	sub	r1, r8, r6
	adds	r0, r7, r6
	cmp	r1, #1
	bne	.L1760
.L1758:
	movs	r3, #10
	strb	r3, [r0, #-1]
	mov	r0, r6
	add	sp, sp, #32
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1756:
	ldrh	r3, [r4, #4]
	adds	r0, r0, #1
	ldrh	lr, [ip, #20]
	ldr	r4, [r4, #12]
	add	r2, r2, r3
	cmp	r3, lr, lsr #1
	itt	hi
	addhi	r6, r6, r3
	addhi	r1, r1, #1
	b	.L1754
.L1760:
	cmp	r5, #0
	beq	.L1758
	ldrh	r2, [r5, #4]
	cmp	r2, #0
	beq	.L1758
	ldrh	r3, [r5]
	adds	r4, r4, #1
	str	r2, [sp]
	mov	r2, r10
	bl	NAND_Snprintf
	lsls	r3, r4, #29
	add	r6, r6, r0
	itt	eq
	addeq	r3, r7, r6
	strbeq	fp, [r3, #-1]
	ldr	r5, [r5, #12]
	b	.L1757
.L1768:
	.align	2
.L1767:
	.word	.LC229
	.word	.LC230
	.size	nftl_get_gc_info, .-nftl_get_gc_info
	.section	.text.nftl_cache_init,"ax",%progbits
	.align	1
	.global	nftl_cache_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_cache_init, %function
nftl_cache_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #1972]
	push	{r4, r5, r6, r7, r8, lr}
	mov	r4, r0
	mov	r6, r0
	movs	r7, #255
	mov	r8, #-1
	movs	r5, #0
	ldrh	r3, [r3, #22]
	strh	r5, [r0, #586]	@ movhi
	strh	r5, [r0, #588]	@ movhi
	strh	r3, [r0, #584]	@ movhi
	strh	r5, [r0, #590]	@ movhi
	str	r5, [r0, #1568]
	str	r5, [r0, #1572]
	str	r5, [r0, #1576]
	str	r5, [r0, #1580]
	str	r5, [r0, #1600]
	str	r5, [r0, #1604]
	str	r5, [r0, #1608]
	str	r5, [r0, #1612]
.L1770:
	ldr	r3, [r4, #4]
	ldrh	r0, [r3, #26]
	ldrh	r3, [r4, #584]
	cmp	r3, r5
	bhi	.L1773
	bl	nftl_malloc
	str	r0, [r4, #1616]
	cbnz	r0, .L1774
	ldr	r1, .L1776
	ldr	r0, .L1776+4
	b	.L1775
.L1773:
	movs	r3, #0
	strh	r5, [r6, #592]	@ movhi
	strh	r7, [r6, #594]	@ movhi
	str	r8, [r6, #600]
	strd	r3, r3, [r6, #608]
	strd	r3, r3, [r6, #616]
	adds	r6, r6, #32
	strh	r3, [r6, #564]	@ movhi
	strh	r3, [r6, #566]	@ movhi
	bl	nftl_malloc
	str	r0, [r6, #572]
	cbnz	r0, .L1771
	ldr	r1, .L1776
	ldr	r0, .L1776+8
.L1775:
	bl	NAND_Print
	movs	r0, #1
.L1769:
	pop	{r4, r5, r6, r7, r8, pc}
.L1771:
	adds	r5, r5, #1
	b	.L1770
.L1774:
	movs	r0, #0
	b	.L1769
.L1777:
	.align	2
.L1776:
	.word	.LANCHOR103
	.word	.LC232
	.word	.LC231
	.size	nftl_cache_init, .-nftl_cache_init
	.section	.text.nftl_cache_exit,"ax",%progbits
	.align	1
	.global	nftl_cache_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_cache_exit, %function
nftl_cache_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r5, r0
	movs	r4, #0
.L1779:
	ldrh	r3, [r5, #584]
	cmp	r3, r4
	bgt	.L1780
	ldr	r0, [r5, #1616]
	bl	nftl_free
	movs	r0, #0
	pop	{r3, r4, r5, pc}
.L1780:
	add	r3, r5, r4, lsl #5
	adds	r4, r4, #1
	ldr	r0, [r3, #604]
	bl	nftl_free
	b	.L1779
	.size	nftl_cache_exit, .-nftl_cache_exit
	.section	.text.nftl_zone_release,"ax",%progbits
	.align	1
	.global	nftl_zone_release
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_zone_release, %function
nftl_zone_release:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	bl	nftl_cache_exit
	mov	r0, r4
	bl	zone_param_exit
	ldr	r3, [r4, #4]
	ldr	r0, [r3, #32]
	bl	nftl_free
	ldr	r0, [r4, #4]
	bl	nftl_free
	mov	r0, r4
	pop	{r4, lr}
	b	nftl_free
	.size	nftl_zone_release, .-nftl_zone_release
	.section	.text.nftl_exit,"ax",%progbits
	.align	1
	.global	nftl_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_exit, %function
nftl_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	ldr	r0, [r0, #4]
	bl	free_nand_partition
	ldr	r0, [r4, #12]
	bl	nftl_zone_release
	ldr	r0, [r4, #16]
	bl	nftl_free
	movs	r0, #0
	pop	{r4, pc}
	.size	nftl_exit, .-nftl_exit
	.section	.text.nand_ftl_exit,"ax",%progbits
	.align	1
	.global	nand_ftl_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_ftl_exit, %function
nand_ftl_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	ldr	r5, .L1786
	ldr	r0, .L1786
	bl	del_last_nftl_blk
	mov	r4, r0
.L1784:
	mov	r0, r4
	cbnz	r4, .L1785
	pop	{r3, r4, r5, pc}
.L1785:
	bl	nftl_exit
	mov	r0, r4
	bl	nftl_free
	mov	r0, r5
	bl	del_last_nftl_blk
	mov	r4, r0
	b	.L1784
.L1787:
	.align	2
.L1786:
	.word	.LANCHOR0
	.size	nand_ftl_exit, .-nand_ftl_exit
	.section	.text.nftl_cache_clean,"ax",%progbits
	.align	1
	.global	nftl_cache_clean
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_cache_clean, %function
nftl_cache_clean:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #1972]
	add	r0, r0, #592
	push	{r4, r5, lr}
	movs	r4, #255
	mov	r5, #-1
	ldrh	r1, [r3, #22]
	movs	r3, #0
	strh	r3, [r0, #-6]	@ movhi
	mov	r2, r3
	strh	r3, [r0, #-4]	@ movhi
	strh	r1, [r0, #-8]	@ movhi
	strh	r3, [r0, #-2]	@ movhi
	str	r3, [r0, #976]
	str	r3, [r0, #980]
	str	r3, [r0, #984]
	str	r3, [r0, #988]
	str	r3, [r0, #1008]
	str	r3, [r0, #1012]
	str	r3, [r0, #1016]
	str	r3, [r0, #1020]
.L1789:
	cmp	r1, r3
	add	r0, r0, #32
	bhi	.L1790
	movs	r0, #0
	pop	{r4, r5, pc}
.L1790:
	strh	r3, [r0, #-32]	@ movhi
	adds	r3, r3, #1
	strh	r4, [r0, #-30]	@ movhi
	str	r5, [r0, #-24]
	strd	r2, r2, [r0, #-16]
	strd	r2, r2, [r0, #-8]
	strh	r2, [r0, #-28]	@ movhi
	strh	r2, [r0, #-26]	@ movhi
	b	.L1789
	.size	nftl_cache_clean, .-nftl_cache_clean
	.section	.text.search_from_cache_read_list,"ax",%progbits
	.align	1
	.global	search_from_cache_read_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	search_from_cache_read_list, %function
search_from_cache_read_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, [r0, #984]
.L1792:
	cbnz	r0, .L1794
.L1791:
	bx	lr
.L1794:
	ldr	r3, [r0, #8]
	cmp	r3, r1
	beq	.L1791
	ldr	r0, [r0, #16]
	b	.L1792
	.size	search_from_cache_read_list, .-search_from_cache_read_list
	.section	.text.search_from_cache_write_list,"ax",%progbits
	.align	1
	.global	search_from_cache_write_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	search_from_cache_write_list, %function
search_from_cache_write_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, [r0, #1024]
.L1796:
	cbnz	r0, .L1798
.L1795:
	bx	lr
.L1798:
	ldr	r3, [r0, #8]
	cmp	r3, r1
	beq	.L1795
	ldr	r0, [r0, #24]
	b	.L1796
	.size	search_from_cache_write_list, .-search_from_cache_write_list
	.section	.text.add_to_cache_read_list_tail,"ax",%progbits
	.align	1
	.global	add_to_cache_read_list_tail
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	add_to_cache_read_list_tail, %function
add_to_cache_read_list_tail:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	add	r3, r0, #968
	push	{r4, lr}
.L1800:
	ldr	r4, [r3, #16]
	cbnz	r4, .L1803
	str	r1, [r3, #16]
	strd	r4, r3, [r1, #16]
	strh	r2, [r1, #2]	@ movhi
	cbnz	r2, .L1801
	ldrh	r3, [r0, #2]
	adds	r3, r3, #1
	strh	r3, [r0, #2]	@ movhi
.L1802:
	movs	r0, #0
	pop	{r4, pc}
.L1803:
	mov	r3, r4
	b	.L1800
.L1801:
	ldrh	r3, [r0, #6]
	adds	r3, r3, #1
	strh	r3, [r0, #6]	@ movhi
	b	.L1802
	.size	add_to_cache_read_list_tail, .-add_to_cache_read_list_tail
	.section	.text.add_to_cache_write_list_tail,"ax",%progbits
	.align	1
	.global	add_to_cache_write_list_tail
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	add_to_cache_write_list_tail, %function
add_to_cache_write_list_tail:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	add	r2, r0, #1000
.L1805:
	ldr	r3, [r2, #24]
	cbnz	r3, .L1806
	str	r1, [r2, #24]
	strd	r3, r2, [r1, #24]
	movs	r2, #1
	strh	r2, [r1, #2]	@ movhi
	ldrh	r2, [r0, #4]
	adds	r2, r2, #1
	strh	r2, [r0, #4]	@ movhi
	mov	r0, r3
	bx	lr
.L1806:
	mov	r2, r3
	b	.L1805
	.size	add_to_cache_write_list_tail, .-add_to_cache_write_list_tail
	.section	.text.del_from_cache_read_list,"ax",%progbits
	.align	1
	.global	del_from_cache_read_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	del_from_cache_read_list, %function
del_from_cache_read_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldrd	r3, r2, [r1, #16]
	str	r3, [r2, #16]
	ldr	r3, [r1, #16]
	cbz	r3, .L1808
	str	r2, [r3, #20]
.L1808:
	ldrh	r3, [r1, #2]
	cbnz	r3, .L1809
	ldrh	r3, [r0, #2]
	subs	r3, r3, #1
	strh	r3, [r0, #2]	@ movhi
.L1810:
	movs	r3, #255
	mov	r0, r1
	strh	r3, [r1, #2]	@ movhi
	movs	r3, #0
	strd	r3, r3, [r1, #16]
	bx	lr
.L1809:
	ldrh	r3, [r0, #6]
	subs	r3, r3, #1
	strh	r3, [r0, #6]	@ movhi
	b	.L1810
	.size	del_from_cache_read_list, .-del_from_cache_read_list
	.section	.text.del_from_cache_write_list,"ax",%progbits
	.align	1
	.global	del_from_cache_write_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	del_from_cache_write_list, %function
del_from_cache_write_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldrd	r3, r2, [r1, #24]
	str	r3, [r2, #24]
	ldr	r3, [r1, #24]
	cbz	r3, .L1815
	str	r2, [r3, #28]
.L1815:
	movs	r3, #0
	strd	r3, r3, [r1, #24]
	movs	r3, #255
	strh	r3, [r1, #2]	@ movhi
	ldrh	r3, [r0, #4]
	subs	r3, r3, #1
	strh	r3, [r0, #4]	@ movhi
	mov	r0, r1
	bx	lr
	.size	del_from_cache_write_list, .-del_from_cache_write_list
	.section	.text.get_empty_cahce_node,"ax",%progbits
	.align	1
	.global	get_empty_cahce_node
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_empty_cahce_node, %function
get_empty_cahce_node:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldrh	r2, [r0]
	movs	r1, #0
	push	{r4}
.L1820:
	uxth	r3, r1
	cmp	r2, r3
	bhi	.L1822
	ldrh	r3, [r0, #6]
	ldr	r1, [r0, #984]
	cmp	r3, #9
	bhi	.L1823
.L1824:
	cbz	r1, .L1819
	ldrh	r3, [r1, #2]
	cbz	r3, .L1831
	ldr	r1, [r1, #16]
	b	.L1824
.L1822:
	adds	r3, r1, #1
	add	r4, r0, r3, lsl #5
	ldrh	r4, [r4, #-22]
	cmp	r4, #255
	bne	.L1829
	add	r1, r0, r1, lsl #5
	adds	r1, r1, #8
.L1819:
	mov	r0, r1
	pop	{r4}
	bx	lr
.L1829:
	mov	r1, r3
	b	.L1820
.L1825:
	ldr	r1, [r1, #16]
.L1823:
	cmp	r1, #0
	beq	.L1819
	ldrh	r3, [r1, #2]
	cmp	r3, #2
	bne	.L1825
.L1831:
	pop	{r4}
	b	del_from_cache_read_list
	.size	get_empty_cahce_node, .-get_empty_cahce_node
	.section	.text.nand_cache_read,"ax",%progbits
	.align	1
	.global	nand_cache_read
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_cache_read, %function
nand_cache_read:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r4, r0
	str	r1, [sp, #4]
	mov	r7, r3
	cbnz	r3, .L1833
	ldr	r0, .L1843
	bl	NAND_Print
.L1841:
	mov	fp, #0
.L1832:
	mov	r0, fp
	add	sp, sp, #16
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1833:
	lsrs	r3, r2, #8
	uxtb	r2, r2
	lsl	r8, r3, #9
	ldr	r3, [r0, #1972]
	lsls	r5, r2, #9
	ldrh	r3, [r3]
	str	r3, [sp, #12]
	cbz	r3, .L1835
	ldr	r6, [r0, #604]
	ldr	r3, [r0, #2008]
	mov	r2, r6
	blx	r3
	mov	r2, r5
	mov	fp, r0
	add	r1, r6, r8
	mov	r0, r7
	bl	NAND_Memcpy
	cmp	fp, #10
	bne	.L1832
	ldr	r2, [r4, #1968]
	mov	r0, r4
	ldr	r1, [sp, #4]
	ldr	r3, [r2, #116]
	adds	r3, r3, #1
	str	r3, [r2, #116]
	mov	r2, r6
	ldr	r3, [r4, #2004]
	blx	r3
	b	.L1841
.L1835:
	add	r10, r0, #584
	mov	r0, r10
	bl	search_from_cache_write_list
	mov	fp, r0
	cbz	r0, .L1836
	ldr	r3, [r0, #12]
	ldrh	r6, [r0, #6]
	str	r3, [sp, #8]
	ldrh	r3, [r0, #4]
	lsls	r6, r6, #9
	ldr	r2, [sp, #8]
	uxth	r6, r6
	lsls	r3, r3, #9
	add	r10, r2, r8
	uxth	r3, r3
	add	r3, r3, r6
	cmp	r3, r8
	bcc	.L1837
	add	r2, r8, r5
	cmp	r3, r2
	bcc	.L1837
.L1842:
	mov	r2, r5
	mov	r1, r10
	mov	r0, r7
	bl	NAND_Memcpy
	b	.L1841
.L1837:
	ldr	r3, [r4, #2008]
	mov	r0, r4
	ldr	r2, [r4, #1616]
	ldr	r1, [sp, #4]
	blx	r3
	ldr	r3, [r4, #4]
	ldr	r1, [r4, #1616]
	ldrh	r2, [r3, #26]
	ldr	r3, [sp, #8]
	add	r1, r1, r6
	subs	r2, r2, r6
	add	r3, r3, r6
	mov	r0, r3
	bl	NAND_Memcpy
	ldr	r3, [r4, #4]
	ldrb	r3, [r3, #8]	@ zero_extendqisi2
	strh	r3, [fp, #6]	@ movhi
	b	.L1842
.L1836:
	ldr	r1, [sp, #4]
	mov	r0, r10
	bl	search_from_cache_read_list
	mov	r6, r0
	cbz	r0, .L1838
	ldrh	r4, [r0, #2]
	cbnz	r4, .L1839
	mov	r1, r0
	mov	r0, r10
	bl	del_from_cache_read_list
	ldr	r1, [r6, #12]
	mov	r2, r5
	mov	r0, r7
	add	r1, r1, r8
	bl	NAND_Memcpy
	mov	r2, r4
	mov	r1, r6
	mov	r0, r10
	bl	add_to_cache_read_list_tail
	b	.L1841
.L1839:
	ldr	r1, [r0, #12]
	mov	r2, r5
	mov	r0, r7
	add	r1, r1, r8
	bl	NAND_Memcpy
	b	.L1832
.L1838:
	mov	r0, r10
	bl	get_empty_cahce_node
	cbnz	r0, .L1840
	ldrh	r3, [r4, #590]
	mov	fp, #1
	ldrh	r2, [r4, #586]
	ldrh	r1, [r4, #588]
	ldr	r0, .L1843+4
	bl	NAND_Print
	b	.L1832
.L1840:
	ldr	r2, [sp, #4]
	ldr	r1, [sp, #4]
	ldr	r3, [r4, #2008]
	str	r2, [r0, #8]
	ldr	r2, [r0, #12]
	strd	r2, r0, [sp, #8]
	mov	r0, r4
	blx	r3
	ldr	r3, [sp, #12]
	mov	fp, r0
	ldr	r2, [r4, #4]
	mov	r0, r10
	strh	r6, [r3, #4]	@ movhi
	mov	r1, r3
	ldrb	r2, [r2, #8]	@ zero_extendqisi2
	strh	r2, [r3, #6]	@ movhi
	mov	r2, r6
	bl	add_to_cache_read_list_tail
	ldr	r3, [sp, #8]
	mov	r2, r5
	mov	r0, r7
	add	r1, r3, r8
	bl	NAND_Memcpy
	cmp	fp, #10
	bne	.L1832
	ldr	r2, [r4, #1968]
	mov	r0, r4
	mov	fp, r6
	ldr	r3, [r2, #116]
	adds	r3, r3, #1
	str	r3, [r2, #116]
	ldr	r3, [r4, #2004]
	ldrd	r1, r2, [sp, #4]
	blx	r3
	b	.L1832
.L1844:
	.align	2
.L1843:
	.word	.LC233
	.word	.LC234
	.size	nand_cache_read, .-nand_cache_read
	.global	__aeabi_uidivmod
	.section	.text.nand_op,"ax",%progbits
	.align	1
	.global	nand_op
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_op, %function
nand_op:
	@ args = 4, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r6, r1
	ldr	r10, [sp, #32]
	mov	r4, r3
	cbz	r3, .L1846
	ldr	r1, [r1, #24]
	add	r3, r3, r2
	cmp	r3, r1
	bls	.L1847
	movs	r4, #1
	ldr	r0, .L1860
	bl	NAND_Print
.L1846:
	mov	r0, r4
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1847:
	ldr	r1, .L1860+4
	ldr	r3, .L1860+8
	cmp	r0, #0
	ite	ne
	movne	r8, r1
	moveq	r8, r3
	ldr	r3, [r6, #4]
	mov	r0, r2
	ldrb	r5, [r3, #8]	@ zero_extendqisi2
	mov	r1, r5
	bl	__aeabi_uidivmod
	subs	r5, r5, r1
	mov	fp, r0
	uxth	r5, r5
	mov	r3, r10
	cmp	r4, r5
	itet	ls
	uxthls	r5, r4
	subhi	r7, r4, r5
	movls	r7, #0
	orr	r2, r5, r1, lsl #8
	mov	r1, r0
	mov	r0, r6
	uxth	r2, r2
	blx	r8
	mov	r4, r0
	cmp	r7, #0
	beq	.L1846
.L1851:
	ldr	r3, [r6, #4]
	add	fp, fp, #1
	add	r10, r10, r5, lsl #9
	ldrb	r3, [r3, #8]	@ zero_extendqisi2
	cmp	r7, r3
	bhi	.L1852
	mov	r3, r10
	uxth	r2, r7
	mov	r1, fp
	mov	r0, r6
	blx	r8
	orrs	r4, r4, r0
	b	.L1846
.L1852:
	uxth	r5, r3
	subs	r7, r7, r3
	mov	r1, fp
	mov	r3, r10
	mov	r2, r5
	mov	r0, r6
	blx	r8
	orrs	r4, r4, r0
	b	.L1851
.L1861:
	.align	2
.L1860:
	.word	.LC177
	.word	nand_cache_write
	.word	nand_cache_read
	.size	nand_op, .-nand_op
	.section	.text.__nand_read,"ax",%progbits
	.align	1
	.global	__nand_read
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	__nand_read, %function
__nand_read:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r4, r5, r6, r7, fp, lr}
	mov	r7, r3
	ldr	r3, [r0, #356]
	mov	r4, r0
	mov	r6, r1
	mov	r5, r2
	cmp	r3, #3
	bne	.L1863
	ldr	r0, .L1864
	bl	NAND_Print_DBG
.L1863:
	ldr	r1, [r4, #1968]
	movs	r0, #0
	ldrd	r2, [r1, #64]
	adds	fp, r2, r5
	mov	r2, r6
	adc	ip, r3, #0
	mov	r3, r5
	strd	fp, [r1, #64]
	mov	r1, r4
	str	r7, [sp]
	bl	nand_op
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, fp, pc}
.L1865:
	.align	2
.L1864:
	.word	.LC235
	.size	__nand_read, .-__nand_read
	.section	.text._nand_read,"ax",%progbits
	.align	1
	.global	_nand_read
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nand_read, %function
_nand_read:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, [r0, #12]
	b	__nand_read
	.size	_nand_read, .-_nand_read
	.section	.text.nand_sector_cache_read,"ax",%progbits
	.align	1
	.global	nand_sector_cache_read
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_sector_cache_read, %function
nand_sector_cache_read:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r4, r0
	addw	r0, r0, #1620
	mov	r5, r2
	bl	search_from_sector_cache_write_list
	cbz	r0, .L1868
	ldr	r1, [r0, #8]
	mov	r2, #512
	mov	r0, r5
	bl	NAND_Memcpy
	movs	r0, #0
.L1867:
	pop	{r4, r5, r6, pc}
.L1868:
	mov	r3, r2
	mov	r0, r4
	movs	r2, #1
	bl	__nand_read
	cmp	r0, #0
	beq	.L1867
	ldr	r0, .L1873
	bl	NAND_Print
	movs	r0, #1
	b	.L1867
.L1874:
	.align	2
.L1873:
	.word	.LC236
	.size	nand_sector_cache_read, .-nand_sector_cache_read
	.section	.text.__nand_sector_read,"ax",%progbits
	.align	1
	.global	__nand_sector_read
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	__nand_sector_read, %function
__nand_sector_read:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	mov	r8, r0
	mov	r4, r1
	mov	r6, r3
	adds	r7, r1, r2
	movs	r5, #0
.L1876:
	cmp	r4, r7
	bne	.L1877
	mov	r0, r5
	pop	{r4, r5, r6, r7, r8, pc}
.L1877:
	mov	r2, r6
	mov	r1, r4
	mov	r0, r8
	adds	r4, r4, #1
	bl	nand_sector_cache_read
	add	r6, r6, #512
	orrs	r5, r5, r0
	b	.L1876
	.size	__nand_sector_read, .-__nand_sector_read
	.section	.text.__nand_write,"ax",%progbits
	.align	1
	.global	__nand_write
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	__nand_write, %function
__nand_write:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r4, r5, r6, r7, fp, lr}
	mov	r7, r3
	ldr	r3, [r0, #356]
	mov	r4, r0
	mov	r6, r1
	mov	r5, r2
	cmp	r3, #3
	bne	.L1879
	ldr	r0, .L1880
	bl	NAND_Print_DBG
.L1879:
	ldr	r1, [r4, #1968]
	movs	r0, #1
	ldrd	r2, [r1, #72]
	adds	fp, r2, r5
	mov	r2, r6
	adc	ip, r3, #0
	mov	r3, r5
	strd	fp, [r1, #72]
	mov	r1, r4
	str	r7, [sp]
	bl	nand_op
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, fp, pc}
.L1881:
	.align	2
.L1880:
	.word	.LC237
	.size	__nand_write, .-__nand_write
	.section	.text._nand_write,"ax",%progbits
	.align	1
	.global	_nand_write
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nand_write, %function
_nand_write:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, [r0, #12]
	b	__nand_write
	.size	_nand_write, .-_nand_write
	.section	.text.write_data_to_nand,"ax",%progbits
	.align	1
	.global	write_data_to_nand
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	write_data_to_nand, %function
write_data_to_nand:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r6, r0
	mov	r0, #512
	bl	NAND_Malloc
	mov	r4, r0
	cbnz	r0, .L1884
	ldr	r1, .L1887
	ldr	r0, .L1887+4
	bl	NAND_Print
.L1884:
	movs	r5, #0
	mov	r2, #512
	movs	r1, #170
	mov	r0, r4
	bl	NAND_Memset
.L1885:
	mov	r1, r5
	mov	r3, r4
	movs	r2, #1
	mov	r0, r6
	adds	r5, r5, #1
	bl	__nand_write
	cmp	r5, #38912
	bne	.L1885
	mov	r0, r4
	bl	nftl_free
	movs	r0, #0
	pop	{r4, r5, r6, pc}
.L1888:
	.align	2
.L1887:
	.word	.LANCHOR104
	.word	.LC238
	.size	write_data_to_nand, .-write_data_to_nand
	.section	.text.nand_sector_cache_write,"ax",%progbits
	.align	1
	.global	nand_sector_cache_write
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_sector_cache_write, %function
nand_sector_cache_write:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	addw	r5, r0, #1620
	mov	r6, r0
	mov	r0, r5
	mov	r8, r1
	mov	r7, r2
	bl	search_from_sector_cache_write_list
	mov	r4, r0
	cbz	r0, .L1890
	mov	r1, r0
	mov	r0, r5
	bl	del_from_sector_cache_write_list
.L1898:
	mov	r2, #512
	mov	r1, r7
	ldr	r0, [r4, #8]
	bl	NAND_Memcpy
	mov	r1, r4
	mov	r0, r5
	bl	add_to_sector_cache_write_list_tail
	movs	r0, #0
	b	.L1889
.L1890:
	ldrh	r3, [r6, #1622]
	cmp	r3, #7
	bls	.L1892
	ldr	r1, [r6, #1960]
	mov	r0, r5
	bl	del_from_sector_cache_write_list
	movs	r2, #1
	mov	r4, r0
	ldr	r3, [r0, #8]
	ldr	r1, [r0, #4]
	mov	r0, r6
	bl	__nand_write
	cbz	r0, .L1893
	ldr	r0, .L1899
.L1897:
	bl	NAND_Print
	movs	r0, #1
.L1889:
	pop	{r4, r5, r6, r7, r8, pc}
.L1892:
	mov	r0, r5
	bl	get_empty_sector_cahce_node
	mov	r4, r0
	cbnz	r0, .L1893
	ldr	r0, .L1899+4
	b	.L1897
.L1893:
	str	r8, [r4, #4]
	b	.L1898
.L1900:
	.align	2
.L1899:
	.word	.LC239
	.word	.LC240
	.size	nand_sector_cache_write, .-nand_sector_cache_write
	.section	.text.__nand_sector_write,"ax",%progbits
	.align	1
	.global	__nand_sector_write
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	__nand_sector_write, %function
__nand_sector_write:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	mov	r8, r0
	mov	r4, r1
	mov	r6, r3
	adds	r7, r1, r2
	movs	r5, #0
.L1902:
	cmp	r4, r7
	bne	.L1903
	mov	r0, r5
	pop	{r4, r5, r6, r7, r8, pc}
.L1903:
	mov	r2, r6
	mov	r1, r4
	mov	r0, r8
	adds	r4, r4, #1
	bl	nand_sector_cache_write
	add	r6, r6, #512
	orrs	r5, r5, r0
	b	.L1902
	.size	__nand_sector_write, .-__nand_sector_write
	.section	.text.__nand_flush_sector_write_cache,"ax",%progbits
	.align	1
	.global	__nand_flush_sector_write_cache
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	__nand_flush_sector_write_cache, %function
__nand_flush_sector_write_cache:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	mov	r5, r0
	mov	r6, r1
	addw	r7, r0, #1620
	movs	r4, #0
.L1905:
	ldr	r1, [r5, #1960]
	cbz	r1, .L1906
	mov	r0, r7
	bl	del_from_sector_cache_write_list
	movs	r2, #1
	ldr	r3, [r0, #8]
	ldr	r1, [r0, #4]
	mov	r0, r5
	bl	__nand_write
	subs	r6, r6, #1
	mov	r4, r0
	bne	.L1905
.L1906:
	cbz	r4, .L1904
	ldr	r0, .L1915
	bl	NAND_Print
.L1904:
	mov	r0, r4
	pop	{r3, r4, r5, r6, r7, pc}
.L1916:
	.align	2
.L1915:
	.word	.LC241
	.size	__nand_flush_sector_write_cache, .-__nand_flush_sector_write_cache
	.section	.text.flush_write_cache_to_nand,"ax",%progbits
	.align	1
	.global	flush_write_cache_to_nand
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	flush_write_cache_to_nand, %function
flush_write_cache_to_nand:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, lr}
	sub	sp, sp, #20
	mov	r4, r0
	movs	r2, #8
	mov	r8, r1
	mov	r0, sp
	ldr	r1, .L1944
	bl	memcpy
	movs	r2, #8
	ldr	r1, .L1944+4
	add	r0, sp, r2
	bl	memcpy
	ldr	r1, [r4, #1608]
	cbnz	r1, .L1918
	ldr	r0, .L1944+8
	bl	NAND_Print
.L1919:
	movs	r0, #0
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L1918:
	add	r7, r4, #584
	mov	r0, r7
	bl	del_from_cache_write_list
	mov	r5, r0
	cmp	r0, #0
	beq	.L1919
	ldr	r3, [r4, #4]
	ldrh	r2, [r0, #6]
	ldrd	r1, r6, [r0, #8]
	ldrb	r3, [r3, #8]	@ zero_extendqisi2
	cmp	r2, r3
	beq	.L1921
	ldr	r3, [r4, #2008]
	mov	r0, r4
	ldr	r2, [r4, #1616]
	blx	r3
	mov	r10, r0
	ldrh	r0, [r5, #6]
	ldr	r3, [r4, #4]
	ldr	r1, [r4, #1616]
	lsls	r0, r0, #9
	ldrh	r2, [r3, #26]
	uxth	r0, r0
	subs	r2, r2, r0
	add	r1, r1, r0
	add	r0, r0, r6
	bl	NAND_Memcpy
	cmp	r10, #10
	bne	.L1921
	ldrh	r2, [r4, #590]
	ldr	r1, [r5, #8]
	ldr	r0, .L1944+12
	bl	NAND_Print
.L1921:
	movs	r3, #0
	mov	r2, r6
	strh	r3, [r5, #4]	@ movhi
	mov	r0, r4
	ldr	r3, [r4, #4]
	ldr	r1, [r5, #8]
	ldrb	r3, [r3, #8]	@ zero_extendqisi2
	strh	r3, [r5, #6]	@ movhi
	ldr	r3, [r4, #2004]
	blx	r3
	cbz	r0, .L1923
	ldr	r0, .L1944+16
	bl	NAND_Print
.L1923:
	ldr	r10, [r4, #4]
	movs	r2, #8
	mov	r1, sp
	mov	r0, r10
	bl	NAND_Memcmp
	tst	r0, #255
	bne	.L1924
.L1927:
	cmp	r8, #1
	bne	.L1928
	ldrh	r1, [r4, #320]
	ldr	r0, .L1944+20
	bl	NAND_Print
	ldrh	r0, [r4, #320]
	cbz	r0, .L1928
	subs	r0, r0, #1
	bl	nand_phy_get_page_type
	cmp	r0, #2
	bne	.L1928
	ldr	r0, .L1944+24
	bl	NAND_Print
	ldr	r3, [r4, #2004]
	mov	r2, r6
	ldr	r1, [r5, #8]
	mov	r0, r4
	blx	r3
	cbz	r0, .L1928
	ldr	r0, .L1944+16
	bl	NAND_Print
	b	.L1928
.L1924:
	movs	r2, #8
	mov	r0, r10
	add	r1, sp, r2
	bl	NAND_Memcmp
	tst	r0, #255
	beq	.L1927
.L1928:
	movs	r2, #2
	mov	r1, r5
	mov	r0, r7
	bl	add_to_cache_read_list_tail
	b	.L1919
.L1945:
	.align	2
.L1944:
	.word	.LANCHOR76+14
	.word	.LANCHOR76+22
	.word	.LC242
	.word	.LC243
	.word	.LC244
	.word	.LC245
	.word	.LC246
	.size	flush_write_cache_to_nand, .-flush_write_cache_to_nand
	.section	.text.nand_cache_write,"ax",%progbits
	.align	1
	.global	nand_cache_write
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_cache_write, %function
nand_cache_write:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r4, r0
	mov	r6, r1
	str	r3, [sp, #4]
	cbnz	r3, .L1947
.L1985:
	movs	r0, #0
	b	.L1946
.L1947:
	lsls	r3, r2, #1
	lsl	r8, r2, #9
	and	r10, r3, #65024
	ldr	r3, [r0, #1972]
	uxth	r8, r8
	ldrh	r5, [r3]
	cbz	r5, .L1949
	ldr	r5, [r0, #604]
	ldr	r3, [r0, #2008]
	mov	r2, r5
	blx	r3
	mov	r2, r8
	ldr	r1, [sp, #4]
	add	r0, r5, r10
	bl	NAND_Memcpy
	ldr	r3, [r4, #2004]
	mov	r2, r5
	mov	r1, r6
	mov	r0, r4
	add	sp, sp, #16
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, lr}
	bx	r3
.L1949:
	add	r5, r0, #584
	ldrh	r0, [r3, #2]
	cbnz	r0, .L1950
	ldr	r0, [r4, #4]
	ldrb	r7, [r0, #8]	@ zero_extendqisi2
	cmp	r7, r2
	bne	.L1950
	ldrh	r3, [r3, #26]
	cbnz	r3, .L1950
	mov	r0, r5
	bl	search_from_cache_write_list
	mov	r1, r0
	cbz	r0, .L1951
	mov	r0, r5
	bl	del_from_cache_write_list
.L1951:
	mov	r1, r6
	mov	r0, r5
	bl	search_from_cache_read_list
	mov	r1, r0
	cbz	r0, .L1952
	mov	r0, r5
	bl	del_from_cache_read_list
.L1952:
	ldr	r3, [r4, #2004]
	mov	r1, r6
	ldr	r2, [sp, #4]
	mov	r0, r4
	blx	r3
	cmp	r0, #0
	beq	.L1985
	mov	r1, r7
	ldr	r0, .L1987
	bl	NAND_Print
.L1984:
	movs	r0, #1
.L1946:
	add	sp, sp, #16
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1950:
	mov	r1, r6
	mov	r0, r5
	bl	search_from_cache_write_list
	mov	r7, r0
	cmp	r0, #0
	beq	.L1954
	mov	r1, r0
	mov	r0, r5
	bl	del_from_cache_write_list
	ldr	r3, [r7, #12]
	ldr	r2, [r4, #4]
	str	r3, [sp, #8]
	ldrb	r2, [r2, #8]	@ zero_extendqisi2
	ldrh	r3, [r7, #6]
	cmp	r2, r3
	beq	.L1956
	lsl	fp, r3, #9
	uxth	fp, fp
	cmp	r10, fp
	bne	.L1957
	add	r3, r3, r8, lsr #9
	strh	r3, [r7, #6]	@ movhi
.L1956:
	ldrd	r1, r3, [sp, #4]
	mov	r2, r8
	add	r3, r3, r10
	mov	r0, r3
	bl	NAND_Memcpy
	mov	r1, r7
	mov	r0, r5
	bl	add_to_cache_write_list_tail
	mov	r1, r6
	mov	r0, r5
	bl	search_from_cache_read_list
	mov	r1, r0
	cmp	r0, #0
	beq	.L1985
	mov	r0, r5
	bl	del_from_cache_read_list
	b	.L1985
.L1957:
	ldr	r3, [r4, #2008]
	mov	r1, r6
	ldr	r2, [r4, #1616]
	mov	r0, r4
	blx	r3
	ldr	r2, [r4, #4]
	ldr	r3, [sp, #8]
	ldr	r1, [r4, #1616]
	ldrh	r2, [r2, #26]
	str	r0, [sp, #12]
	add	r0, r3, fp
	add	r1, r1, fp
	sub	r2, r2, fp
	bl	NAND_Memcpy
	ldr	r2, [r4, #4]
	ldr	r3, [sp, #12]
	ldrb	r2, [r2, #8]	@ zero_extendqisi2
	cmp	r3, #10
	strh	r2, [r7, #6]	@ movhi
	bne	.L1956
	mov	r1, r6
	ldr	r0, .L1987+4
	bl	NAND_Print
	b	.L1956
.L1954:
	mov	r0, r5
	bl	search_from_cache_read_list
	mov	fp, r0
	cbz	r0, .L1959
	ldr	r0, [r0, #12]
	mov	r2, r8
	ldr	r1, [sp, #4]
	add	r0, r0, r10
	bl	NAND_Memcpy
	ldr	r3, [r4, #4]
	mov	r1, fp
	strh	r7, [fp, #4]	@ movhi
	mov	r0, r5
	ldrb	r3, [r3, #8]	@ zero_extendqisi2
	strh	r3, [fp, #6]	@ movhi
	bl	del_from_cache_read_list
	mov	r0, r5
	bl	add_to_cache_write_list_tail
	ldr	r3, [r4, #1972]
	ldrh	r2, [r4, #588]
	ldrh	r3, [r3, #24]
	cmp	r2, r3
	bcc	.L1985
	mov	r1, r7
	mov	r0, r4
	bl	flush_write_cache_to_nand
	cmp	r0, #0
	beq	.L1985
	ldr	r0, .L1987+8
.L1986:
	bl	NAND_Print
	b	.L1984
.L1960:
	movs	r1, #0
	mov	r0, r4
	bl	flush_write_cache_to_nand
	cbz	r0, .L1959
	ldr	r0, .L1987+12
	b	.L1986
.L1959:
	ldr	r3, [r4, #1972]
	ldrh	r2, [r4, #588]
	ldrh	r3, [r3, #24]
	cmp	r2, r3
	bcs	.L1960
	mov	r0, r5
	bl	get_empty_cahce_node
	mov	r7, r0
	cbnz	r0, .L1961
	ldrh	r3, [r4, #590]
	ldrh	r2, [r4, #586]
	ldrh	r1, [r4, #588]
	ldr	r0, .L1987+16
	bl	NAND_Print
	b	.L1984
.L1961:
	str	r6, [r0, #8]
	ldr	fp, [r0, #12]
	cmp	r10, #0
	bne	.L1962
	lsr	r3, r8, #9
	strh	r10, [r0, #4]	@ movhi
.L1983:
	ldr	r1, [sp, #4]
	add	r0, fp, r10
	strh	r3, [r7, #6]	@ movhi
	mov	r2, r8
	bl	NAND_Memcpy
	mov	r1, r7
	mov	r0, r5
	bl	add_to_cache_write_list_tail
	b	.L1985
.L1962:
	ldr	r3, [r4, #2008]
	mov	r2, fp
	mov	r1, r6
	mov	r0, r4
	blx	r3
	cmp	r0, #10
	bne	.L1964
	mov	r1, r6
	ldr	r0, .L1987+20
	bl	NAND_Print
.L1964:
	movs	r3, #0
	strh	r3, [r7, #4]	@ movhi
	ldr	r3, [r4, #4]
	ldrb	r3, [r3, #8]	@ zero_extendqisi2
	b	.L1983
.L1988:
	.align	2
.L1987:
	.word	.LC247
	.word	.LC248
	.word	.LC249
	.word	.LC250
	.word	.LC234
	.word	.LC251
	.size	nand_cache_write, .-nand_cache_write
	.section	.text.__nand_flush_write_cache,"ax",%progbits
	.align	1
	.global	__nand_flush_write_cache
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	__nand_flush_write_cache, %function
__nand_flush_write_cache:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	movs	r4, #0
	mov	r6, r0
	mov	r7, r1
	mov	r5, r4
.L1990:
	ldr	r3, [r6, #1608]
	cbz	r3, .L1991
	adds	r4, r4, #1
	movs	r1, #1
	mov	r0, r6
	bl	flush_write_cache_to_nand
	cmp	r7, r4
	orr	r5, r5, r0
	bne	.L1990
.L1991:
	cbz	r5, .L1993
	ldr	r0, .L2004
	bl	NAND_Print
.L1993:
	cbz	r4, .L1989
	ldr	r3, [r6, #356]
	cbz	r3, .L1989
	mov	r2, r4
	ldrb	r1, [r6, #16]	@ zero_extendqisi2
	ldr	r0, .L2004+4
	bl	NAND_Print_DBG
.L1989:
	mov	r0, r5
	pop	{r3, r4, r5, r6, r7, pc}
.L2005:
	.align	2
.L2004:
	.word	.LC241
	.word	.LC252
	.size	__nand_flush_write_cache, .-__nand_flush_write_cache
	.section	.text._nand_flush_write_cache,"ax",%progbits
	.align	1
	.global	_nand_flush_write_cache
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nand_flush_write_cache, %function
_nand_flush_write_cache:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, [r0, #12]
	b	__nand_flush_write_cache
	.size	_nand_flush_write_cache, .-_nand_flush_write_cache
	.section	.text.nand_clean_zone_table,"ax",%progbits
	.align	1
	.global	nand_clean_zone_table
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_clean_zone_table, %function
nand_clean_zone_table:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r1, #1000
	mov	r4, r0
	bl	__nand_flush_write_cache
	mov	r0, r4
	bl	clean_block_list
	mov	r0, r4
	bl	nand_clean_zone_table2
	mov	r0, r4
	bl	nftl_cache_exit
	movs	r0, #0
	pop	{r4, pc}
	.size	nand_clean_zone_table, .-nand_clean_zone_table
	.section	.text.nand_discard,"ax",%progbits
	.align	1
	.global	nand_discard
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_discard, %function
nand_discard:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	mov	r6, r0
	ldr	r3, [r6, #24]
	mov	r4, r2
	adds	r2, r1, r2
	mov	r0, r1
	cmp	r2, r3
	bls	.L2009
	ldr	r0, .L2030
	bl	NAND_Print
	movs	r0, #1
.L2008:
	pop	{r4, r5, r6, r7, r8, pc}
.L2009:
	ldr	r1, [r6, #1968]
	ldrd	r2, [r1, #96]
	adds	r7, r2, r4
	adc	r8, r3, #0
	ldr	r3, [r6, #4]
	strd	r7, [r1, #96]
	ldrb	r7, [r3, #8]	@ zero_extendqisi2
	mov	r1, r7
	bl	__aeabi_uidivmod
	mov	r5, r0
	cbz	r1, .L2011
	subs	r1, r7, r1
	adds	r5, r5, #1
	uxth	r1, r1
	cmp	r1, r4
	bhi	.L2019
	subs	r4, r4, r1
.L2012:
	mov	r1, r7
	mov	r0, r4
	bl	__aeabi_uidiv
	cbz	r0, .L2013
	add	r7, r6, #584
	adds	r4, r5, r0
.L2018:
	ldr	r3, [r6, #1972]
	ldrh	r3, [r3]
	cbnz	r3, .L2015
	mov	r1, r5
	mov	r0, r7
	bl	search_from_cache_write_list
	mov	r1, r0
	cbz	r0, .L2016
	mov	r0, r7
	bl	del_from_cache_write_list
.L2016:
	mov	r1, r5
	mov	r0, r7
	bl	search_from_cache_read_list
	mov	r1, r0
	cbz	r0, .L2015
	mov	r0, r7
	bl	del_from_cache_read_list
.L2015:
	mov	r1, r5
	ldr	r3, [r6, #2016]
	mov	r0, r6
	adds	r5, r5, #1
	blx	r3
	ldr	r3, [r6, #4]
	movs	r1, #0
	ldr	ip, [r6, #1968]
	ldrb	r0, [r3, #8]	@ zero_extendqisi2
	ldrd	r3, r2, [ip, #104]
	adds	r3, r3, r0
	adc	r2, r1, r2
	cmp	r5, r4
	strd	r3, r2, [ip, #104]
	bne	.L2018
.L2013:
	movs	r0, #0
	b	.L2008
.L2011:
	cmp	r7, r4
	it	hi
	movhi	r4, #0
	b	.L2012
.L2019:
	movs	r4, #0
	b	.L2012
.L2031:
	.align	2
.L2030:
	.word	.LC253
	.size	nand_discard, .-nand_discard
	.section	.text.__nand_discard,"ax",%progbits
	.align	1
	.global	__nand_discard
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	__nand_discard, %function
__nand_discard:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #356]
	push	{r4, r5, r6, lr}
	mov	r4, r0
	mov	r5, r1
	mov	r6, r2
	cmp	r3, #3
	bne	.L2033
	ldr	r0, .L2034
	bl	NAND_Print_DBG
.L2033:
	mov	r2, r6
	mov	r1, r5
	mov	r0, r4
	pop	{r4, r5, r6, lr}
	b	nand_discard
.L2035:
	.align	2
.L2034:
	.word	.LC254
	.size	__nand_discard, .-__nand_discard
	.section	.text._nand_discard,"ax",%progbits
	.align	1
	.global	_nand_discard
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nand_discard, %function
_nand_discard:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, [r0, #12]
	b	__nand_discard
	.size	_nand_discard, .-_nand_discard
	.section	.text.__shutdown_op,"ax",%progbits
	.align	1
	.global	__shutdown_op
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	__shutdown_op, %function
__shutdown_op:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #0
	bx	lr
	.size	__shutdown_op, .-__shutdown_op
	.section	.text.print_logic_page_map,"ax",%progbits
	.align	1
	.global	print_logic_page_map
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	print_logic_page_map, %function
print_logic_page_map:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #4]
	push	{r0, r1, r4, r5, r6, r7, r8, lr}
	mov	r5, r0
	ldr	r7, .L2043
	movs	r4, #0
	ldr	r8, .L2043+4
	ldrb	r1, [r3, #8]	@ zero_extendqisi2
	ldr	r0, [r0, #24]
	bl	__aeabi_uidiv
	mov	r6, r0
.L2039:
	cmp	r4, r6
	bne	.L2042
	movs	r0, #0
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L2042:
	mov	r1, r4
	mov	r0, r5
	bl	get_logic_page_map
	cbnz	r0, .L2040
	mov	r0, r8
	bl	NAND_Print
.L2041:
	adds	r4, r4, #1
	b	.L2039
.L2040:
	ldrh	r2, [r0, #2]
	movw	r3, #65535
	cmp	r2, r3
	beq	.L2041
	ldrb	r1, [r0, #1]	@ zero_extendqisi2
	ldrh	r3, [r0]
	mov	r0, r7
	ubfx	r1, r1, #4, #1
	str	r1, [sp]
	ubfx	r3, r3, #0, #12
	mov	r1, r4
	bl	NAND_Print_DBG
	b	.L2041
.L2044:
	.align	2
.L2043:
	.word	.LC256
	.word	.LC255
	.size	print_logic_page_map, .-print_logic_page_map
	.section	.text.get_last_nftl,"ax",%progbits
	.align	1
	.global	get_last_nftl
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_last_nftl, %function
get_last_nftl:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	mov	r3, r0
.L2046:
	cbnz	r3, .L2047
	bx	lr
.L2047:
	mov	r0, r3
	ldr	r3, [r3, #8]
	b	.L2046
	.size	get_last_nftl, .-get_last_nftl
	.section	.text.get_nftl_need_read_claim,"ax",%progbits
	.align	1
	.global	get_nftl_need_read_claim
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_nftl_need_read_claim, %function
get_nftl_need_read_claim:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	mov	r2, r0
	mov	r3, r0
.L2049:
	cbnz	r3, .L2050
	ldr	r3, [r2, #12]
	ldr	r3, [r3, #1968]
	ldr	r3, [r3, #172]
.L2051:
	cbnz	r0, .L2053
.L2048:
	bx	lr
.L2050:
	mov	r2, r3
	ldr	r3, [r3, #8]
	b	.L2049
.L2053:
	ldr	r2, [r0, #12]
	ldrb	r2, [r2, #16]	@ zero_extendqisi2
	cmp	r2, r3
	beq	.L2048
	ldr	r0, [r0, #8]
	b	.L2051
	.size	get_nftl_need_read_claim, .-get_nftl_need_read_claim
	.section	.text.get_nftl_read_claim_page_no,"ax",%progbits
	.align	1
	.global	get_nftl_read_claim_page_no
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_nftl_read_claim_page_no, %function
get_nftl_read_claim_page_no:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	mov	r3, r0
.L2055:
	cbnz	r3, .L2056
	ldr	r3, [r0, #12]
	ldr	r3, [r3, #1968]
	ldr	r0, [r3, #176]
	bx	lr
.L2056:
	mov	r0, r3
	ldr	r3, [r3, #8]
	b	.L2055
	.size	get_nftl_read_claim_page_no, .-get_nftl_read_claim_page_no
	.section	.text.clean_read_reclaim_complete_flag,"ax",%progbits
	.align	1
	.global	clean_read_reclaim_complete_flag
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	clean_read_reclaim_complete_flag, %function
clean_read_reclaim_complete_flag:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
.L2058:
	cbnz	r0, .L2059
	bx	lr
.L2059:
	ldr	r2, [r0, #12]
	ldr	r0, [r0, #8]
	ldr	r3, [r2, #364]
	clz	r3, r3
	lsrs	r3, r3, #5
	str	r3, [r2, #364]
	b	.L2058
	.size	clean_read_reclaim_complete_flag, .-clean_read_reclaim_complete_flag
	.section	.text.set_nftl_read_claim_complete,"ax",%progbits
	.align	1
	.global	set_nftl_read_claim_complete
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	set_nftl_read_claim_complete, %function
set_nftl_read_claim_complete:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r3, #1
	ldr	r5, [r1, #12]
	mov	r4, r0
	str	r3, [r5, #360]
	mov	r3, r0
.L2061:
	cbnz	r3, .L2062
	ldr	r1, [r1, #8]
	ldr	r2, [r4, #12]
	ldr	r2, [r2, #1968]
	cbnz	r1, .L2063
	str	r3, [r2, #172]
	bl	clean_read_reclaim_complete_flag
.L2064:
	ldr	r3, [r4, #12]
	movs	r0, #0
	ldr	r3, [r3, #1968]
	str	r0, [r3, #176]
	pop	{r3, r4, r5, pc}
.L2062:
	mov	r4, r3
	ldr	r3, [r3, #8]
	b	.L2061
.L2063:
	ldrb	r3, [r5, #16]	@ zero_extendqisi2
	adds	r3, r3, #1
	str	r3, [r2, #172]
	b	.L2064
	.size	set_nftl_read_claim_complete, .-set_nftl_read_claim_complete
	.section	.text.set_nftl_read_claim_flag,"ax",%progbits
	.align	1
	.global	set_nftl_read_claim_flag
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	set_nftl_read_claim_flag, %function
set_nftl_read_claim_flag:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
.L2066:
	cbnz	r0, .L2067
	bx	lr
.L2067:
	ldr	r3, [r0, #12]
	ldr	r0, [r0, #8]
	ldr	r3, [r3, #1968]
	strd	r1, r2, [r3, #172]
	b	.L2066
	.size	set_nftl_read_claim_flag, .-set_nftl_read_claim_flag
	.section	.text.get_cold_logic_page,"ax",%progbits
	.align	1
	.global	get_cold_logic_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_cold_logic_page, %function
get_cold_logic_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	mov	r5, r0
	ldr	r8, .L2074
	mov	r6, r2
	mov	r4, r1
	movw	r7, #65535
.L2069:
	cmp	r4, r6
	bcc	.L2073
	mov	r4, #-1
	b	.L2068
.L2073:
	mov	r1, r4
	mov	r0, r5
	bl	get_logic_page_map
	cbnz	r0, .L2070
	mov	r0, r8
	bl	NAND_Print
.L2071:
	adds	r4, r4, #1
	b	.L2069
.L2070:
	ldrh	r3, [r0, #2]
	cmp	r3, r7
	beq	.L2071
	ldrb	r3, [r0, #1]	@ zero_extendqisi2
	ldr	r2, [r5, #364]
	ubfx	r1, r3, #4, #1
	cmp	r1, r2
	beq	.L2071
	bfi	r3, r2, #4, #1
	strb	r3, [r0, #1]
.L2068:
	mov	r0, r4
	pop	{r4, r5, r6, r7, r8, pc}
.L2075:
	.align	2
.L2074:
	.word	.LC255
	.size	get_cold_logic_page, .-get_cold_logic_page
	.section	.text.read_reclaim,"ax",%progbits
	.align	1
	.global	read_reclaim
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	read_reclaim, %function
read_reclaim:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	mov	r4, r1
	mov	r7, r2
	mov	r6, r0
	bl	get_nftl_read_claim_page_no
	ldr	r5, [r1, #12]
	mov	r1, r0
	ldr	r2, [r5, #28]
	mov	r0, r5
	bl	get_cold_logic_page
	adds	r3, r0, #1
	mov	r5, r0
	bne	.L2077
	mov	r1, r4
	mov	r0, r6
	pop	{r3, r4, r5, r6, r7, lr}
	b	set_nftl_read_claim_complete
.L2077:
	ldr	r0, [r4, #12]
	mov	r3, r7
	mov	r1, r5
	ldr	r2, [r0, #4]
	ldrb	r2, [r2, #8]	@ zero_extendqisi2
	bl	nand_cache_read
	ldr	r3, [r4, #12]
	mov	r2, r5
	mov	r0, r6
	ldrb	r1, [r3, #16]	@ zero_extendqisi2
	bl	set_nftl_read_claim_flag
	movs	r0, #0
	pop	{r3, r4, r5, r6, r7, pc}
	.size	read_reclaim, .-read_reclaim
	.section	.text.nftl_get_boot_cnt,"ax",%progbits
	.align	1
	.global	nftl_get_boot_cnt
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_get_boot_cnt, %function
nftl_get_boot_cnt:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, [r0, #1968]
	ldr	r0, [r3, #140]
	bx	lr
	.size	nftl_get_boot_cnt, .-nftl_get_boot_cnt
	.section	.text.nftl_get_zone_cap,"ax",%progbits
	.align	1
	.global	nftl_get_zone_cap
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_get_zone_cap, %function
nftl_get_zone_cap:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, [r0, #24]
	bx	lr
	.size	nftl_get_zone_cap, .-nftl_get_zone_cap
	.section	.text.nftl_get_zone_priv,"ax",%progbits
	.align	1
	.global	nftl_get_zone_priv
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_get_zone_priv, %function
nftl_get_zone_priv:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r0, [r0]
	bx	lr
	.size	nftl_get_zone_priv, .-nftl_get_zone_priv
	.section	.text.nftl_get_zone_write_cache_nums,"ax",%progbits
	.align	1
	.global	nftl_get_zone_write_cache_nums
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_get_zone_write_cache_nums, %function
nftl_get_zone_write_cache_nums:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldrh	r0, [r0, #588]
	bx	lr
	.size	nftl_get_zone_write_cache_nums, .-nftl_get_zone_write_cache_nums
	.section	.text.print_smart,"ax",%progbits
	.align	1
	.global	print_smart
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	print_smart, %function
print_smart:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #356]
	push	{r4, lr}
	cmp	r3, #0
	beq	.L2082
	ldr	r4, [r0, #1968]
	ldr	r0, .L2084
	ldr	r1, [r4, #112]
	bl	NAND_Print_DBG
	ldrd	r2, [r4, #64]
	ldr	r0, .L2084+4
	bl	NAND_Print_DBG
	ldrd	r2, [r4, #72]
	ldr	r0, .L2084+8
	bl	NAND_Print_DBG
	ldrd	r2, [r4, #80]
	ldr	r0, .L2084+12
	bl	NAND_Print_DBG
	ldrd	r2, [r4, #88]
	ldr	r0, .L2084+16
	bl	NAND_Print_DBG
	ldrd	r2, [r4, #96]
	ldr	r0, .L2084+20
	bl	NAND_Print_DBG
	ldrd	r2, [r4, #104]
	ldr	r0, .L2084+24
	bl	NAND_Print_DBG
	ldr	r1, [r4, #116]
	ldr	r0, .L2084+28
	bl	NAND_Print_DBG
	ldr	r1, [r4, #120]
	ldr	r0, .L2084+32
	bl	NAND_Print_DBG
	ldr	r1, [r4, #124]
	ldr	r0, .L2084+36
	bl	NAND_Print_DBG
	ldr	r1, [r4, #128]
	ldr	r0, .L2084+40
	bl	NAND_Print_DBG
	ldr	r1, [r4, #180]
	ldr	r0, .L2084+44
	bl	NAND_Print_DBG
	ldr	r1, [r4, #132]
	ldr	r0, .L2084+48
	bl	NAND_Print_DBG
	ldr	r1, [r4, #136]
	ldr	r0, .L2084+52
	bl	NAND_Print_DBG
	ldr	r1, [r4, #140]
	ldr	r0, .L2084+56
	bl	NAND_Print_DBG
	ldr	r1, [r4, #144]
	ldr	r0, .L2084+60
	bl	NAND_Print_DBG
	ldr	r1, [r4, #148]
	ldr	r0, .L2084+64
	bl	NAND_Print_DBG
	ldr	r1, [r4, #152]
	ldr	r0, .L2084+68
	bl	NAND_Print_DBG
	ldr	r1, [r4, #156]
	ldr	r0, .L2084+72
	bl	NAND_Print_DBG
	ldr	r1, [r4, #160]
	ldr	r0, .L2084+76
	bl	NAND_Print_DBG
	ldr	r1, [r4, #164]
	ldr	r0, .L2084+80
	bl	NAND_Print_DBG
	ldr	r1, [r4, #168]
	ldr	r0, .L2084+84
	bl	NAND_Print_DBG
	ldr	r1, [r4, #172]
	ldr	r0, .L2084+88
	bl	NAND_Print_DBG
	ldr	r1, [r4, #176]
	ldr	r0, .L2084+92
	pop	{r4, lr}
	b	NAND_Print_DBG
.L2082:
	pop	{r4, pc}
.L2085:
	.align	2
.L2084:
	.word	.LC257
	.word	.LC258
	.word	.LC259
	.word	.LC260
	.word	.LC261
	.word	.LC262
	.word	.LC263
	.word	.LC264
	.word	.LC265
	.word	.LC266
	.word	.LC267
	.word	.LC268
	.word	.LC269
	.word	.LC270
	.word	.LC271
	.word	.LC272
	.word	.LC273
	.word	.LC274
	.word	.LC275
	.word	.LC276
	.word	.LC277
	.word	.LC278
	.word	.LC279
	.word	.LC280
	.size	print_smart, .-print_smart
	.section	.text.init_smart_info,"ax",%progbits
	.align	1
	.global	init_smart_info
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	init_smart_info, %function
init_smart_info:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r2, #320
	mov	r4, r0
	sub	sp, sp, #32
	mov	r5, r1
	movs	r1, #0
	ldr	r0, [r0, #1968]
	bl	NAND_Memset
	ldr	r3, [r4, #1968]
	ldr	r6, .L2100
	ldr	r2, [r3, #140]
	str	r6, [r3, #112]
	adds	r2, r2, #1
	str	r2, [r3, #140]
	cmp	r5, #0
	beq	.L2087
	ldr	r3, [r4, #4]
	ldrh	r0, [r5]
	ldr	r2, [r4, #284]
	ldrh	r1, [r3, #28]
	ldrh	r3, [r3, #20]
	strh	r0, [sp, #18]	@ movhi
	mov	r0, r4
	strh	r1, [sp, #20]	@ movhi
	add	r1, sp, #16
	subs	r3, r3, #1
	str	r2, [sp, #24]
	strh	r3, [sp, #16]	@ movhi
	str	sp, [sp, #28]
	ldr	r3, [r4, #1984]
	blx	r3
	mov	r5, r0
	mov	r0, sp
	bl	is_phy_mapping_page
	cmp	r0, #1
	bne	.L2089
	cbz	r5, .L2090
	cmp	r5, #10
	bne	.L2089
.L2090:
	ldr	r3, [r4, #308]
	ldr	r1, [r4, #284]
	add	r1, r1, r3
	ldr	r3, [r1, #112]
	cmp	r3, r6
	bne	.L2089
	mov	r2, #320
	ldr	r0, [r4, #1968]
	bl	NAND_Memcpy
	ldr	r3, [r4, #1968]
	ldr	r2, [r3, #140]
	adds	r2, r2, #1
	str	r2, [r3, #140]
	ldr	r2, [r3, #164]
	adds	r2, r2, #1
	itt	eq
	moveq	r2, #0
	streq	r2, [r3, #164]
	ldr	r2, [r3, #172]
	adds	r2, r2, #1
	itt	eq
	moveq	r2, #0
	streq	r2, [r3, #172]
	ldr	r2, [r3, #176]
	adds	r2, r2, #1
	itt	eq
	moveq	r2, #0
	streq	r2, [r3, #176]
.L2089:
	mov	r0, r4
	bl	print_smart
.L2087:
	movs	r0, #0
	add	sp, sp, #32
	@ sp needed
	pop	{r4, r5, r6, pc}
.L2101:
	.align	2
.L2100:
	.word	-1431655935
	.size	init_smart_info, .-init_smart_info
	.section	.text.print_nftl_zone,"ax",%progbits
	.align	1
	.global	print_nftl_zone
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	print_nftl_zone, %function
print_nftl_zone:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r3, r4, lr}
	mov	r4, r0
	ldr	r3, [r0, #356]
	cmp	r3, #0
	beq	.L2102
	ldr	r0, .L2108
	bl	NAND_Print_DBG
	ldr	r3, [r4, #4]
	ldr	r0, .L2108+4
	ldrh	r1, [r3, #24]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #4]
	ldr	r0, .L2108+8
	ldrh	r1, [r3, #26]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #4]
	ldr	r0, .L2108+12
	ldrh	r1, [r3, #20]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #4]
	ldr	r0, .L2108+16
	ldrh	r1, [r3, #16]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #4]
	ldr	r0, .L2108+20
	ldrh	r1, [r3, #30]
	bl	NAND_Print_DBG
	ldr	r1, [r4, #356]
	ldr	r0, .L2108+24
	bl	NAND_Print_DBG
	ldrb	r1, [r4, #16]	@ zero_extendqisi2
	ldr	r0, .L2108+28
	bl	NAND_Print_DBG
	ldrb	r1, [r4, #17]	@ zero_extendqisi2
	ldr	r0, .L2108+32
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #18]
	ldr	r0, .L2108+36
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #20]
	ldr	r0, .L2108+40
	bl	NAND_Print_DBG
	ldr	r1, [r4, #24]
	ldr	r0, .L2108+44
	bl	NAND_Print_DBG
	ldr	r1, [r4, #32]
	ldr	r0, .L2108+48
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #22]
	ldr	r0, .L2108+52
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #380]
	ldr	r0, .L2108+56
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #382]
	ldr	r0, .L2108+60
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #388]
	ldr	r0, .L2108+64
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #384]
	ldr	r0, .L2108+68
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #394]
	ldr	r0, .L2108+72
	bl	NAND_Print_DBG
	ldr	r3, [r4, #40]
	ldr	r0, .L2108+76
	ldrh	r1, [r3]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #44]
	ldr	r0, .L2108+80
	ldrh	r1, [r3]
	bl	NAND_Print_DBG
	add	r3, r4, #272
	ldm	r3, {r0, r1, r2}
	add	r3, r4, #260
	stm	sp, {r0, r1, r2}
	ldr	r0, .L2108+84
	ldm	r3, {r1, r2, r3}
	bl	NAND_Print_DBG
	add	r3, r4, #308
	ldm	r3, {r0, r1, r2}
	add	r3, r4, #296
	stm	sp, {r0, r1, r2}
	ldr	r0, .L2108+88
	ldm	r3, {r1, r2, r3}
	bl	NAND_Print_DBG
	ldr	r1, [r4, #292]
	ldr	r0, .L2108+92
	bl	NAND_Print_DBG
	ldr	r3, [r4, #292]
	ldr	r0, .L2108+96
	ldrh	r1, [r3]
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #320]
	ldr	r0, .L2108+100
	bl	NAND_Print_DBG
	ldr	r3, [r4, #292]
	ldr	r0, .L2108+104
	ldr	r1, [r3, #8]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #48]
	ldr	r0, .L2108+108
	ldrh	r2, [r3, #6]
	ldrh	r1, [r3]
	bl	NAND_Print_DBG
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r3, r3, #31
	bpl	.L2104
	add	r3, r4, #340
	ldm	r3, {r0, r1, r2}
	add	r3, r4, #328
	stm	sp, {r0, r1, r2}
	ldr	r0, .L2108+112
	ldm	r3, {r1, r2, r3}
	bl	NAND_Print_DBG
	ldr	r1, [r4, #324]
	ldr	r0, .L2108+116
	bl	NAND_Print_DBG
	ldr	r3, [r4, #324]
	ldr	r0, .L2108+120
	ldrh	r1, [r3]
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #352]
	ldr	r0, .L2108+124
	bl	NAND_Print_DBG
	ldr	r3, [r4, #324]
	ldr	r0, .L2108+128
	ldr	r1, [r3, #8]
	bl	NAND_Print_DBG
.L2104:
	ldr	r1, [r4, #360]
	ldr	r0, .L2108+132
	bl	NAND_Print_DBG
	ldr	r1, [r4, #284]
	ldr	r0, .L2108+136
	bl	NAND_Print_DBG
	ldr	r1, [r4, #376]
	ldr	r0, .L2108+140
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #584]
	ldr	r0, .L2108+144
	bl	NAND_Print_DBG
	ldr	r1, [r4, #368]
	ldr	r0, .L2108+148
	bl	NAND_Print_DBG
	ldr	r1, [r4, #372]
	ldr	r0, .L2108+152
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L2108+156
	ldrh	r1, [r3]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L2108+160
	ldrh	r1, [r3, #2]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L2108+164
	ldrh	r1, [r3, #4]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L2108+168
	ldrh	r1, [r3, #6]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L2108+172
	ldrh	r1, [r3, #10]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L2108+176
	ldrh	r1, [r3, #12]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L2108+180
	ldrh	r1, [r3, #14]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L2108+184
	ldrh	r1, [r3, #16]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L2108+188
	ldrh	r1, [r3, #18]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L2108+192
	ldrh	r1, [r3, #20]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L2108+196
	ldrh	r1, [r3, #22]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L2108+200
	ldrh	r1, [r3, #24]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L2108+204
	ldrh	r1, [r3, #26]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L2108+208
	ldr	r1, [r3, #32]
	bl	NAND_Print_DBG
	mov	r0, r4
	add	sp, sp, #16
	@ sp needed
	pop	{r4, lr}
	b	print_smart
.L2102:
	add	sp, sp, #16
	@ sp needed
	pop	{r4, pc}
.L2109:
	.align	2
.L2108:
	.word	.LC281
	.word	.LC282
	.word	.LC283
	.word	.LC284
	.word	.LC285
	.word	.LC286
	.word	.LC287
	.word	.LC288
	.word	.LC289
	.word	.LC290
	.word	.LC291
	.word	.LC292
	.word	.LC293
	.word	.LC294
	.word	.LC295
	.word	.LC296
	.word	.LC297
	.word	.LC298
	.word	.LC299
	.word	.LC300
	.word	.LC301
	.word	.LC302
	.word	.LC303
	.word	.LC304
	.word	.LC305
	.word	.LC306
	.word	.LC307
	.word	.LC308
	.word	.LC309
	.word	.LC310
	.word	.LC311
	.word	.LC312
	.word	.LC313
	.word	.LC314
	.word	.LC315
	.word	.LC316
	.word	.LC317
	.word	.LC318
	.word	.LC319
	.word	.LC320
	.word	.LC321
	.word	.LC322
	.word	.LC323
	.word	.LC324
	.word	.LC325
	.word	.LC326
	.word	.LC327
	.word	.LC328
	.word	.LC329
	.word	.LC330
	.word	.LC331
	.word	.LC332
	.word	.LC333
	.size	print_nftl_zone, .-print_nftl_zone
	.section	.text.nftl_set_zone_test,"ax",%progbits
	.align	1
	.global	nftl_set_zone_test
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_set_zone_test, %function
nftl_set_zone_test:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	str	r1, [r0, #356]
	b	print_nftl_zone
	.size	nftl_set_zone_test, .-nftl_set_zone_test
	.section	.text.nand_crc16,"ax",%progbits
	.align	1
	.global	nand_crc16
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_crc16, %function
nand_crc16:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	add	r2, r2, r1
	ldr	r4, .L2114
.L2112:
	cmp	r2, r1
	bne	.L2113
	pop	{r4, pc}
.L2113:
	ldrb	r3, [r1], #1	@ zero_extendqisi2
	eors	r3, r3, r0
	uxtb	r3, r3
	ldrh	r3, [r4, r3, lsl #1]
	eor	r0, r3, r0, lsr #8
	b	.L2112
.L2115:
	.align	2
.L2114:
	.word	.LANCHOR105
	.size	nand_crc16, .-nand_crc16
	.section	.text.crc_wrong,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	crc_wrong, %function
crc_wrong:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	mov	r6, r0
	mov	r0, r2
	mov	r7, r1
	mov	r5, r2
	bl	is_nouse_page
	cbnz	r0, .L2121
	ldr	r3, [r6, #4]
	mov	r0, r5
	ldrb	r4, [r3, #8]	@ zero_extendqisi2
	bl	is_phy_mapping_page
	cbnz	r0, .L2118
	lsls	r4, r4, #9
.L2119:
	mov	r2, r4
	mov	r1, r7
	movw	r0, #65535
	bl	nand_crc16
	ldrb	r3, [r5, #12]	@ zero_extendqisi2
	mov	r1, r0
	ldrb	r2, [r5, #13]	@ zero_extendqisi2
	orr	r2, r2, r3, lsl #8
	cmp	r0, r2
	beq	.L2121
	mov	r3, r4
	ldr	r0, .L2122
	bl	NAND_Print
	movs	r0, #1
.L2116:
	pop	{r3, r4, r5, r6, r7, pc}
.L2118:
	ldr	r4, [r6, #308]
	b	.L2119
.L2121:
	movs	r0, #0
	b	.L2116
.L2123:
	.align	2
.L2122:
	.word	.LC334
	.size	crc_wrong, .-crc_wrong
	.section	.text.nand_power_off_ecc_error,"ax",%progbits
	.align	1
	.global	nand_power_off_ecc_error
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_power_off_ecc_error, %function
nand_power_off_ecc_error:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, lr}
	mov	r4, r2
	ldrb	r5, [r2]	@ zero_extendqisi2
	sub	sp, sp, #60
	cmp	r5, #255
	beq	.L2125
	ldr	r0, .L2136
.L2135:
	bl	NAND_Print
	movs	r3, #1
	movw	r2, #3134
	ldr	r1, .L2136+4
	ldr	r0, .L2136+8
	bl	NAND_Print
	ldr	r0, .L2136+12
	bl	NAND_Print
	ldrb	r0, [r4, #15]	@ zero_extendqisi2
	ldrb	r3, [r4, #2]	@ zero_extendqisi2
	ldrb	r2, [r4, #1]	@ zero_extendqisi2
	str	r0, [sp, #48]
	ldrb	r0, [r4, #14]	@ zero_extendqisi2
	ldrb	r1, [r4]	@ zero_extendqisi2
	str	r0, [sp, #44]
	ldrb	r0, [r4, #13]	@ zero_extendqisi2
	str	r0, [sp, #40]
	ldrb	r0, [r4, #12]	@ zero_extendqisi2
	str	r0, [sp, #36]
	ldrb	r0, [r4, #11]	@ zero_extendqisi2
	str	r0, [sp, #32]
	ldrb	r0, [r4, #10]	@ zero_extendqisi2
	str	r0, [sp, #28]
	ldrb	r0, [r4, #9]	@ zero_extendqisi2
	str	r0, [sp, #24]
	ldrb	r0, [r4, #8]	@ zero_extendqisi2
	str	r0, [sp, #20]
	ldrb	r0, [r4, #7]	@ zero_extendqisi2
	str	r0, [sp, #16]
	ldrb	r0, [r4, #6]	@ zero_extendqisi2
	str	r0, [sp, #12]
	ldrb	r0, [r4, #5]	@ zero_extendqisi2
	str	r0, [sp, #8]
	ldrb	r0, [r4, #4]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [r4, #3]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L2136+16
	bl	NAND_Print
	movs	r0, #1
	b	.L2129
.L2125:
	ldr	r3, [r0, #1972]
	ldrh	r3, [r3, #28]
	cbz	r3, .L2127
	bl	crc_wrong
	cbnz	r0, .L2128
.L2131:
	movs	r0, #0
.L2129:
	add	sp, sp, #60
	@ sp needed
	pop	{r4, r5, pc}
.L2128:
	ldr	r0, .L2136+20
	b	.L2135
.L2127:
	ldrb	r3, [r2, #11]	@ zero_extendqisi2
	cmp	r3, #165
	bne	.L2130
	ldrb	r3, [r2, #15]	@ zero_extendqisi2
	cmp	r3, #165
	beq	.L2131
.L2130:
	ldr	r0, .L2136+24
	b	.L2135
.L2137:
	.align	2
.L2136:
	.word	.LC335
	.word	.LANCHOR106
	.word	.LC338
	.word	.LC339
	.word	.LC340
	.word	.LC336
	.word	.LC337
	.size	nand_power_off_ecc_error, .-nand_power_off_ecc_error
	.section	.text.recover_phy_page_mapping,"ax",%progbits
	.align	1
	.global	recover_phy_page_mapping
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	recover_phy_page_mapping, %function
recover_phy_page_mapping:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r8, r2
	ldr	r10, .L2172+20
	movs	r4, #0
	sub	sp, sp, #32
	mov	r5, r0
	ldr	fp, .L2172+24
	mov	r7, r1
	ldr	r2, [r0, #308]
	movs	r1, #255
	mov	r0, r8
	bl	NAND_Memset
.L2139:
	ldr	r3, [r5, #4]
	ldrh	r2, [r3, #20]
	cmp	r2, r4
	ble	.L2142
	ldrh	r2, [r3, #28]
	mov	r0, r5
	ldrh	r1, [r7]
	ldr	r3, [r5, #284]
	strh	r2, [sp, #20]	@ movhi
	strh	r1, [sp, #18]	@ movhi
	add	r1, sp, #16
	str	r3, [sp, #24]
	strh	r4, [sp, #16]	@ movhi
	ldr	r3, [r5, #1984]
	str	sp, [sp, #28]
	blx	r3
	mov	r6, r0
	mov	r0, sp
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L2140
	cbnz	r6, .L2141
.L2143:
	mov	r2, r4
	ldrh	r1, [r7]
	ldr	r0, .L2172
	bl	NAND_Print
.L2142:
	mov	r0, r4
	b	.L2138
.L2141:
	cmp	r6, #10
	beq	.L2143
.L2140:
	mov	r0, sp
	bl	is_ftl_logic_page_data
	cmp	r0, #1
	bne	.L2144
	cbz	r6, .L2145
	cmp	r6, #10
	bne	.L2146
.L2145:
	mov	r2, sp
	ldr	r1, [r5, #284]
	mov	r0, r5
	bl	nand_power_off_ecc_error
	cbz	r0, .L2147
	mov	r2, r4
	ldrh	r1, [r7]
	ldr	r0, .L2172+4
	bl	NAND_Print
.L2146:
	mov	r2, r4
	ldrh	r1, [r7]
	ldr	r0, .L2172+8
	bl	NAND_Print
	movw	r0, #65535
.L2138:
	add	sp, sp, #32
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2147:
	mov	r0, sp
	bl	get_logic_page_from_oob
	str	r0, [r8, r4, lsl #2]
	mov	r3, r0
	ldr	r2, [r5, #28]
	cmp	r0, r2
	bcc	.L2148
	mov	r2, r4
	ldrh	r1, [r7]
	ldr	r0, .L2172+12
	bl	NAND_Print
	mov	r3, #-1
	str	r3, [r8, r4, lsl #2]
.L2148:
	cbz	r6, .L2153
	cmp	r6, #10
	bne	.L2146
.L2153:
	adds	r4, r4, #1
	b	.L2139
.L2144:
	ldrb	r3, [sp, #1]	@ zero_extendqisi2
	cmp	r3, #170
	bne	.L2149
	ldr	r0, .L2172+16
	mov	r1, r4
.L2171:
	bl	NAND_Print
	b	.L2148
.L2149:
	mov	r0, sp
	bl	is_power_down_info_page
	cmp	r0, #1
	bne	.L2150
	cbz	r6, .L2151
	cmp	r6, #10
	bne	.L2146
.L2151:
	mov	r0, sp
	bl	get_special_data_from_oob
	mov	r1, r4
	str	r0, [r8, r4, lsl #2]
	mov	r0, fp
	b	.L2171
.L2150:
	mov	r1, r4
	mov	r0, r10
	b	.L2171
.L2173:
	.align	2
.L2172:
	.word	.LC341
	.word	.LC342
	.word	.LC347
	.word	.LC343
	.word	.LC344
	.word	.LC346
	.word	.LC345
	.size	recover_phy_page_mapping, .-recover_phy_page_mapping
	.section	.text.recover_block_phy_page_mapping,"ax",%progbits
	.align	1
	.global	recover_block_phy_page_mapping
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	recover_block_phy_page_mapping, %function
recover_block_phy_page_mapping:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r10, r2
	movs	r4, #0
	ldr	r2, [r0, #308]
	sub	sp, sp, #40
	mov	r5, r0
	mov	r7, r1
	mov	r0, r10
	movs	r1, #255
	bl	NAND_Memset
.L2175:
	ldr	r2, [r5, #4]
	ldrh	r3, [r2, #20]
	subs	r3, r3, #1
	cmp	r3, r4
	bgt	.L2186
	movs	r0, #0
	add	sp, sp, #40
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2186:
	ldr	r3, [r5, #1972]
	uxth	r0, r4
	ldrh	r1, [r7]
	add	r6, sp, #8
	ldrh	r3, [r3, #28]
	cmp	r3, #0
	beq	.L2176
	ldr	r3, [r5, #284]
	ldrh	r2, [r2, #28]
	strh	r0, [sp, #24]	@ movhi
	strh	r1, [sp, #26]	@ movhi
	strh	r2, [sp, #28]	@ movhi
.L2197:
	mov	fp, #0
	strd	r3, r6, [sp, #32]
.L2180:
	ldr	r3, [r5, #1984]
	add	r1, sp, #24
	mov	r0, r5
	blx	r3
	subs	r8, r0, #0
	bge	.L2178
	mov	r3, r4
	ldrh	r2, [r7]
	mov	r1, r8
	ldr	r0, .L2198
	bl	NAND_Print
	cmp	fp, #0
	beq	.L2187
	ldrh	r2, [r7]
	mov	r3, r4
	str	fp, [sp]
	mov	r1, r8
	ldr	r0, .L2198+4
	bl	NAND_Print
.L2178:
	mov	r0, r6
	bl	is_ftl_logic_page_data
	cmp	r0, #1
	bne	.L2181
	cmp	r8, #0
	beq	.L2182
	cmp	r8, #10
	bne	.L2183
.L2182:
	mov	r2, r6
	ldr	r1, [r5, #284]
	mov	r0, r5
	bl	nand_power_off_ecc_error
	cbz	r0, .L2184
	ldr	r0, .L2198+8
	bl	NAND_Print
.L2183:
	adds	r4, r4, #1
	b	.L2175
.L2176:
	strh	r0, [sp, #24]	@ movhi
	strh	r1, [sp, #26]	@ movhi
	strh	r3, [sp, #28]	@ movhi
	b	.L2197
.L2187:
	mov	fp, #1
	b	.L2180
.L2184:
	mov	r0, r6
	bl	get_logic_page_from_oob
	str	r0, [r10, r4, lsl #2]
	mov	r3, r0
	ldr	r2, [r5, #28]
	cmp	r0, r2
	bcc	.L2183
	mov	r2, r4
	ldrh	r1, [r7]
	ldr	r0, .L2198+12
	bl	NAND_Print
	mov	r3, #-1
	str	r3, [r10, r4, lsl #2]
	b	.L2183
.L2181:
	mov	r0, r6
	bl	is_nouse_page
	cmp	r0, #1
	beq	.L2183
	mov	r2, r4
	ldrh	r1, [r7]
	ldr	r0, .L2198+16
	bl	NAND_Print
	b	.L2183
.L2199:
	.align	2
.L2198:
	.word	.LC348
	.word	.LC349
	.word	.LC350
	.word	.LC343
	.word	.LC351
	.size	recover_block_phy_page_mapping, .-recover_block_phy_page_mapping
	.section	.text.get_used_page_num,"ax",%progbits
	.align	1
	.global	get_used_page_num
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_used_page_num, %function
get_used_page_num:
	@ args = 0, pretend = 0, frame = 40
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r6, r0
	mov	fp, r2
	mov	r5, r3
	sub	sp, sp, #104
	movs	r4, #0
	str	r1, [sp, #64]
	str	r4, [r2]
.L2201:
	ldr	r3, [r6, #4]
	ldrh	r2, [r3, #20]
	cmp	r2, r4
	bhi	.L2212
.L2200:
	mov	r0, r4
	add	sp, sp, #104
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2212:
	cmp	r5, #0
	beq	.L2202
	ldr	r2, [r5, #4]
	ldrb	r2, [r2, r4]	@ zero_extendqisi2
	cmp	r2, #0
	bne	.L2202
	ldrh	r7, [r3, #26]
	ldr	r2, [r5, #8]
	add	r8, r7, #64
	mla	r8, r4, r8, r2
	add	r7, r7, r8
.L2203:
	ldr	r2, [sp, #64]
	uxth	r0, r4
	ldrh	r1, [r2]
	ldr	r2, [r6, #1972]
	ldrh	r2, [r2, #28]
	cmp	r2, #0
	beq	.L2204
	ldrh	r3, [r3, #28]
	strh	r0, [sp, #88]	@ movhi
	strh	r1, [sp, #90]	@ movhi
	strh	r3, [sp, #92]	@ movhi
	str	r8, [sp, #96]
.L2250:
	str	r7, [sp, #100]
	add	r1, sp, #88
	ldr	r3, [r6, #1984]
	mov	r0, r6
	blx	r3
	mov	r10, r0
	cbz	r0, .L2206
	cmp	r0, #10
	beq	.L2206
	ldr	r3, [sp, #64]
	mov	r2, r4
	ldr	r0, .L2252
	ldrh	r1, [r3]
	bl	NAND_Print
	ldr	r3, [fp]
	adds	r3, r3, #1
	str	r3, [fp]
.L2206:
	mov	r0, r7
	bl	is_nouse_page
	cmp	r0, #1
	beq	.L2223
	adds	r3, r4, #1
	cmp	r10, #0
	beq	.L2208
	cmp	r10, #10
	bne	.L2209
.L2208:
	cmp	r8, #0
	beq	.L2209
	mov	r2, r7
	mov	r1, r8
	mov	r0, r6
	str	r3, [sp, #68]
	bl	nand_power_off_ecc_error
	ldr	r3, [sp, #68]
	cbz	r0, .L2211
	ldr	r0, .L2252+4
	bl	NAND_Print
	ldr	r3, [sp, #68]
.L2209:
	mov	r4, r3
	b	.L2201
.L2202:
	ldrb	r2, [r3, #8]	@ zero_extendqisi2
	cmp	r2, #8
	ittee	eq
	ldreq	r8, [r6, #284]
	addeq	r7, sp, #72
	addne	r7, sp, #72
	movne	r8, #0
	b	.L2203
.L2204:
	strh	r0, [sp, #88]	@ movhi
	strh	r1, [sp, #90]	@ movhi
	strh	r2, [sp, #92]	@ movhi
	str	r2, [sp, #96]
	b	.L2250
.L2211:
	ldr	r2, [r5, #4]
	movs	r1, #1
	strb	r1, [r2, r4]
	ldr	r2, [r5]
	cmp	r2, r3
	it	cc
	strcc	r3, [r5]
	b	.L2209
.L2220:
	cmp	r5, #0
	beq	.L2225
	ldr	r2, [r5, #4]
	ldrb	r2, [r2, r7]	@ zero_extendqisi2
	cmp	r2, #0
	bne	.L2225
	ldrh	r2, [r3, #26]
	ldr	r1, [r5, #8]
	add	r8, r2, #64
	mla	r8, r7, r8, r1
	add	fp, r8, r2
.L2213:
	ldr	r1, [r6, #1972]
	uxth	ip, r7
	ldr	r2, [sp, #64]
	ldrh	r1, [r1, #28]
	ldrh	r0, [r2]
	cmp	r1, #0
	beq	.L2214
	ldrh	r3, [r3, #28]
	strh	ip, [sp, #88]	@ movhi
	strh	r0, [sp, #90]	@ movhi
	strh	r3, [sp, #92]	@ movhi
	str	r8, [sp, #96]
.L2251:
	ldr	r3, [r6, #1984]
	add	r1, sp, #88
	str	fp, [sp, #100]
	mov	r0, r6
	blx	r3
	str	r0, [sp, #68]
	mov	r0, fp
	bl	is_nouse_page
	cmp	r0, #1
	add	r10, r7, #1
	beq	.L2217
	ldrb	r0, [fp, #15]	@ zero_extendqisi2
	ldr	r2, [sp, #64]
	ldrb	r3, [fp]	@ zero_extendqisi2
	ldrh	r1, [r2]
	str	r0, [sp, #56]
	ldrb	r0, [fp, #14]	@ zero_extendqisi2
	str	r0, [sp, #52]
	ldrb	r0, [fp, #13]	@ zero_extendqisi2
	str	r0, [sp, #48]
	ldrb	r0, [fp, #12]	@ zero_extendqisi2
	str	r0, [sp, #44]
	ldrb	r0, [fp, #11]	@ zero_extendqisi2
	str	r0, [sp, #40]
	ldrb	r0, [fp, #10]	@ zero_extendqisi2
	str	r0, [sp, #36]
	ldrb	r0, [fp, #9]	@ zero_extendqisi2
	str	r0, [sp, #32]
	ldrb	r0, [fp, #8]	@ zero_extendqisi2
	str	r0, [sp, #28]
	ldrb	r0, [fp, #7]	@ zero_extendqisi2
	str	r0, [sp, #24]
	ldrb	r0, [fp, #6]	@ zero_extendqisi2
	str	r0, [sp, #20]
	ldrb	r0, [fp, #5]	@ zero_extendqisi2
	str	r0, [sp, #16]
	ldrb	r0, [fp, #4]	@ zero_extendqisi2
	str	r0, [sp, #12]
	ldrb	r0, [fp, #3]	@ zero_extendqisi2
	str	r0, [sp, #8]
	ldrb	r0, [fp, #2]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r2, [fp, #1]	@ zero_extendqisi2
	ldr	r0, .L2252+8
	str	r2, [sp]
	mov	r2, r7
	bl	NAND_Print
	ldr	r3, [sp, #68]
	cbz	r3, .L2218
	cmp	r3, #10
	bne	.L2217
.L2218:
	cmp	r8, #0
	beq	.L2217
	ldr	r3, [r5, #4]
	movs	r2, #1
	strb	r2, [r3, r7]
	ldr	r3, [r5]
	cmp	r3, r10
	it	cc
	strcc	r10, [r5]
.L2217:
	mov	r7, r10
.L2207:
	ldr	r3, [r6, #4]
	ldrh	r2, [r3, #20]
	cmp	r2, r7
	bhi	.L2220
	b	.L2200
.L2225:
	add	fp, sp, #72
	mov	r8, #0
	b	.L2213
.L2214:
	strh	ip, [sp, #88]	@ movhi
	strh	r0, [sp, #90]	@ movhi
	strh	r1, [sp, #92]	@ movhi
	str	r1, [sp, #96]
	b	.L2251
.L2223:
	mov	r7, r4
	b	.L2207
.L2253:
	.align	2
.L2252:
	.word	.LC352
	.word	.LC353
	.word	.LC354
	.size	get_used_page_num, .-get_used_page_num
	.section	.text.get_used_page_num_no_crosstalk,"ax",%progbits
	.align	1
	.global	get_used_page_num_no_crosstalk
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_used_page_num_no_crosstalk, %function
get_used_page_num_no_crosstalk:
	@ args = 0, pretend = 0, frame = 48
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r6, r3
	movs	r3, #0
	mov	r8, r0
	mov	r10, r3
	mov	fp, r3
	mov	r4, #-1
	mov	r5, r3
	sub	sp, sp, #48
	str	r3, [r2]
	str	r1, [sp, #4]
.L2255:
	ldr	r2, [r8, #4]
	ldrh	r3, [r2, #20]
	cmp	r3, r5
	bhi	.L2264
	adds	r2, r4, #1
	bne	.L2262
	mov	r4, r3
.L2265:
	mov	r0, r4
	add	sp, sp, #48
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2264:
	cbz	r6, .L2256
	ldr	r3, [r6, #4]
	ldrb	r3, [r3, r5]	@ zero_extendqisi2
	cbnz	r3, .L2256
	ldrh	r7, [r2, #26]
	ldr	r3, [r6, #8]
	add	r1, r7, #64
	mla	r3, r5, r1, r3
	add	r7, r7, r3
.L2257:
	ldr	r1, [sp, #4]
	mov	r0, r8
	ldrh	r2, [r2, #28]
	str	r3, [sp, #40]
	ldrh	r1, [r1]
	strh	r2, [sp, #36]	@ movhi
	strh	r5, [sp, #32]	@ movhi
	strh	r1, [sp, #34]	@ movhi
	add	r1, sp, #32
	str	r7, [sp, #44]
	ldr	r2, [r8, #1984]
	str	r3, [sp, #8]
	blx	r2
	ldr	r3, [sp, #8]
	cbz	r0, .L2258
	cmp	r0, #10
	beq	.L2258
	ldr	r3, [sp, #4]
	cmp	r4, #-1
	it	eq
	moveq	r4, r5
	mov	r2, r5
	ldr	r0, .L2285
	ldrh	r1, [r3]
	bl	NAND_Print
.L2259:
	adds	r5, r5, #1
	b	.L2255
.L2256:
	ldrb	r3, [r2, #8]	@ zero_extendqisi2
	cmp	r3, #4
	ittee	eq
	ldreq	r3, [r8, #284]
	addeq	r7, sp, #16
	addne	r7, sp, #16
	movne	r3, #0
	b	.L2257
.L2258:
	mov	r0, r7
	str	r3, [sp, #8]
	bl	is_nouse_page
	cmp	r0, #1
	ldr	r3, [sp, #8]
	bne	.L2260
	add	r10, r10, #1
	cmp	r4, #-1
	it	eq
	moveq	r4, r5
	cmp	r10, #3
	bne	.L2259
.L2262:
	cmp	fp, r4
	beq	.L2265
	ldr	r1, [sp, #4]
	mov	r3, r4
	mov	r2, fp
	ldr	r0, .L2285+4
	ldrh	r1, [r1]
	bl	NAND_Print
	ldr	r2, [r8, #1968]
	ldr	r3, [r2, #168]
	adds	r3, r3, #1
	str	r3, [r2, #168]
	movs	r3, #0
.L2266:
	ldr	r2, [r6]
	cmp	r2, r3
	bls	.L2265
	ldr	r2, [r6, #4]
	ldrb	r2, [r2, r3]	@ zero_extendqisi2
	cbz	r2, .L2267
	ldr	r2, [r8, #4]
	ldr	r1, [r6, #8]
	ldrh	r0, [r2, #26]
	add	r1, r1, r0
	add	r2, r0, #64
	mla	r2, r3, r2, r1
	ldr	r1, [r8, #1968]
	ldr	r1, [r1, #168]
	strb	r1, [r2, #14]
.L2267:
	adds	r3, r3, #1
	b	.L2266
.L2260:
	mov	r2, r7
	mov	r1, r3
	mov	r0, r8
	bl	nand_power_off_ecc_error
	cbz	r0, .L2263
	ldr	r0, .L2285+8
	bl	NAND_Print
	b	.L2259
.L2263:
	adds	r1, r4, #1
	add	fp, fp, #1
	bne	.L2259
	ldr	r2, [r6, #4]
	movs	r1, #1
	adds	r3, r5, #1
	mov	r0, r7
	str	r3, [sp, #12]
	strb	r1, [r2, r5]
	ldr	r2, [r6, #12]
	str	r2, [sp, #8]
	bl	get_logic_page_from_oob
	ldr	r2, [sp, #8]
	ldr	r3, [sp, #12]
	str	r0, [r2, r5, lsl #2]
	ldr	r2, [r6]
	cmp	r2, r3
	it	cc
	strcc	r3, [r6]
	b	.L2259
.L2286:
	.align	2
.L2285:
	.word	.LC355
	.word	.LC357
	.word	.LC356
	.size	get_used_page_num_no_crosstalk, .-get_used_page_num_no_crosstalk
	.section	.text.do_write_error,"ax",%progbits
	.align	1
	.global	do_write_error
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	do_write_error, %function
do_write_error:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r4, r0
	ldr	r10, .L2309+4
	sub	sp, sp, #24
	mov	r6, r1
	ldr	fp, .L2309+8
.L2288:
	mov	r0, r4
	bl	out_phy_block_from_free_list
	mov	r5, r0
	cbnz	r0, .L2289
.L2293:
	ldr	r0, .L2309
	bl	NAND_Print
	movs	r0, #1
.L2287:
	add	sp, sp, #24
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2289:
	ldr	r3, [r4, #4]
	movs	r7, #0
	ldrh	r2, [r0]
	add	r1, sp, #8
	strh	r7, [sp, #8]	@ movhi
	mov	r0, r4
	ldrh	r3, [r3, #28]
	strh	r2, [sp, #10]	@ movhi
	strd	r7, r7, [sp, #16]
	strh	r3, [sp, #12]	@ movhi
	ldr	r3, [r4, #1980]
	blx	r3
	ldrh	r3, [r5, #6]
	strh	r7, [r5, #4]	@ movhi
	adds	r3, r3, #1
	strh	r3, [r5, #6]	@ movhi
	cbz	r0, .L2291
	ldr	r3, [r4, #2000]
	add	r1, sp, #8
	mov	r0, r4
	blx	r3
	ldrh	r1, [r5]
	mov	r0, fp
	bl	NAND_Print
	ldrh	r2, [r4, #380]
	ldrh	r3, [r4, #22]
	cmp	r3, r2, lsr #2
	bhi	.L2288
	b	.L2293
.L2291:
	ldr	r3, [r6]
	mov	r8, r0
	ldr	r3, [r3, #8]
	adds	r3, r3, #1
	str	r3, [r5, #8]
.L2294:
	ldrh	r3, [r6, #28]
	ldr	r1, [r6]
	cmp	r3, r8
	bhi	.L2297
.L2295:
	ldrh	r3, [r1, #4]
	movs	r7, #0
	ldrh	r2, [r1]
	mov	r0, r4
	strh	r7, [sp, #8]	@ movhi
	strh	r3, [r5, #4]	@ movhi
	ldr	r3, [r4, #4]
	strh	r2, [sp, #10]	@ movhi
	ldrh	r2, [r1, #6]
	ldrh	r3, [r3, #28]
	strd	r7, r7, [sp, #16]
	adds	r2, r2, #1
	uxth	r2, r2
	strh	r3, [sp, #12]	@ movhi
	bl	erase_block
	ldr	r3, [r6]
	ldrh	r3, [r3, #2]
	strh	r3, [r5, #2]	@ movhi
	str	r5, [r6]
.L2298:
	ldrh	r3, [r6, #28]
	cmp	r3, r7
	bhi	.L2302
	movs	r0, #0
	b	.L2287
.L2297:
	ldr	r3, [r6, #12]
	ldr	r3, [r3, r8, lsl #2]
	adds	r3, r3, #1
	beq	.L2295
	uxth	r3, r8
	ldr	r7, [r4, #1992]
	mov	r2, r5
	str	r3, [sp]
	mov	r0, r4
	ldr	r3, [r4, #284]
	blx	r7
	cbz	r0, .L2296
	ldrh	r1, [r5]
	mov	r0, r10
	bl	NAND_Print
	ldr	r3, [r4, #4]
	mov	r0, r4
	ldrh	r1, [r5]
	ldrh	r2, [r3, #28]
	movs	r3, #0
	strh	r3, [sp, #8]	@ movhi
	strh	r1, [sp, #10]	@ movhi
	add	r1, sp, #8
	strd	r3, r3, [sp, #16]
	strh	r2, [sp, #12]	@ movhi
	ldr	r3, [r4, #2000]
	blx	r3
	b	.L2288
.L2296:
	add	r8, r8, #1
	b	.L2294
.L2302:
	ldr	r3, [r6, #12]
	ldr	r1, [r3, r7, lsl #2]
	adds	r3, r1, #1
	beq	.L2300
	mov	r0, r4
	bl	get_logic_page_map
	cbz	r0, .L2300
	ldr	r3, [r6]
	ldrh	r3, [r3]
	strh	r3, [r0, #2]	@ movhi
	ldrh	r3, [r0]
	bfi	r3, r7, #0, #12
	strh	r3, [r0]	@ movhi
.L2300:
	adds	r7, r7, #1
	b	.L2298
.L2310:
	.align	2
.L2309:
	.word	.LC358
	.word	.LC360
	.word	.LC359
	.size	do_write_error, .-do_write_error
	.section	.text.write_phy_data_to_current_block,"ax",%progbits
	.align	1
	.global	write_phy_data_to_current_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	write_phy_data_to_current_block, %function
write_phy_data_to_current_block:
	@ args = 4, pretend = 0, frame = 40
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	sub	sp, sp, #48
	add	r7, sp, #16
	mov	r4, r0
	mov	fp, r2
	mov	r0, r7
	movs	r2, #16
	mov	r5, r1
	movs	r1, #165
	str	r3, [sp, #12]
	ldr	r6, [sp, #80]
	mov	r8, #3
	bl	NAND_Memset
	ldr	r3, [r4, #4]
	mov	r1, fp
	movw	r0, #65535
	ldrb	r2, [r3, #8]	@ zero_extendqisi2
	ldr	r3, .L2328
	cmp	r6, r3
	itet	ne
	lslne	r2, r2, #9
	ldrheq	r2, [r4, #308]
	uxthne	r2, r2
	bl	nand_crc16
	lsrs	r3, r0, #8
	strb	r0, [sp, #29]
	strb	r3, [sp, #28]
.L2314:
	ldr	r0, [r5]
	ldr	lr, [r4, #4]
	ldr	r3, .L2328
	ldrh	r1, [r0]
	ldrh	r2, [r5, #28]
	ldrh	ip, [lr, #28]
	cmp	r6, r3
	strh	r1, [sp, #34]	@ movhi
	strh	r2, [sp, #32]	@ movhi
	strh	ip, [sp, #36]	@ movhi
	strd	fp, r7, [sp, #40]
	bne	.L2315
	ldrh	r3, [r0, #6]
	mov	r2, r6
	str	r3, [sp]
	ldr	r3, [r0, #8]
.L2327:
	mov	r1, r7
	mov	r0, r4
	bl	set_oob_special_page
	b	.L2316
.L2315:
	cmp	r6, #192
	bne	.L2317
	ldrh	r3, [lr, #20]
	subs	r3, r3, #1
	cmp	r2, r3
	bne	.L2318
	ldr	r0, .L2328+4
	bl	NAND_Print_DBG
.L2318:
	ldr	r3, [r5]
	mov	r1, r7
	mov	r0, r4
	ldrh	r2, [r3, #6]
	str	r2, [sp]
	ldr	r2, [sp, #12]
	ldr	r3, [r3, #8]
	bl	set_oob_logic_page
.L2316:
	ldr	r3, [r5]
	mov	r10, #1
	add	r1, sp, #32
	mov	r0, r4
	strh	r10, [r3, #2]	@ movhi
	ldr	r3, [r4, #1988]
	blx	r3
	cbz	r0, .L2311
	subs	r8, r8, #1
	beq	.L2321
	ldrh	r1, [sp, #34]
	ldr	r0, .L2328+8
	bl	NAND_Print_DBG
	mov	r1, r5
	mov	r0, r4
	bl	do_write_error
	cmp	r0, #0
	beq	.L2314
	ldr	r0, .L2328+12
	bl	NAND_Print
	mov	r0, r10
.L2311:
	add	sp, sp, #48
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2317:
	ldr	r3, .L2328+16
	cmp	r6, r3
	bne	.L2316
	ldrh	r3, [lr, #20]
	subs	r3, r3, #1
	cmp	r2, r3
	bne	.L2319
	ldr	r0, .L2328+20
	bl	NAND_Print_DBG
.L2319:
	ldr	r3, [r5]
	ldrh	r2, [r3, #6]
	str	r2, [sp]
	ldr	r2, .L2328+16
	ldr	r3, [r3, #8]
	b	.L2327
.L2321:
	mov	r0, r8
	b	.L2311
.L2329:
	.align	2
.L2328:
	.word	-1431633921
	.word	.LC361
	.word	.LC363
	.word	.LC364
	.word	-1441713357
	.word	.LC362
	.size	write_phy_data_to_current_block, .-write_phy_data_to_current_block
	.section	.text.write_logic_data_to_current_block,"ax",%progbits
	.align	1
	.global	write_logic_data_to_current_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	write_logic_data_to_current_block, %function
write_logic_data_to_current_block:
	@ args = 4, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	cmp	r2, #192
	push	{r0, r1, r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r8, r0
	mov	r4, r1
	mov	fp, r3
	bne	.L2331
	mov	r1, r3
	bl	get_logic_page_map
	mov	r10, r0
	cbnz	r0, .L2332
.L2338:
	movs	r7, #1
	ldr	r0, .L2354
	bl	NAND_Print_DBG
.L2330:
	mov	r0, r7
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2332:
	ldrh	r3, [r0, #2]
	movw	r2, #65535
	cmp	r3, r2
	beq	.L2346
	ldr	r2, [r8, #4]
	movs	r5, #36
	ldr	r1, [r2, #32]
	mla	r5, r5, r3, r1
	ldr	r3, [r4]
	cmp	r3, r5
	bne	.L2345
	ldrh	r3, [r5, #4]
	ldrh	r2, [r2, #20]
	adds	r3, r3, #1
	uxth	r3, r3
	cmp	r2, r3
	strh	r3, [r5, #4]	@ movhi
	bcs	.L2346
	ldrh	r1, [r4, #28]
	ldr	r0, .L2354+4
	bl	NAND_Print_DBG
.L2346:
	movs	r6, #0
	b	.L2334
.L2345:
	movs	r6, #1
.L2334:
	movs	r3, #192
	ldr	r2, [sp, #40]
	str	r3, [sp]
	mov	r1, r4
	mov	r3, fp
	mov	r0, r8
	bl	write_phy_data_to_current_block
	mov	r7, r0
	cbz	r0, .L2335
	ldr	r0, .L2354+8
.L2353:
	bl	NAND_Print
	b	.L2330
.L2335:
	ldr	r3, [r4]
	ldrh	r1, [r4, #28]
	ldr	r2, [r8, #364]
	ldrh	r3, [r3]
	ubfx	r0, r1, #0, #12
	adds	r1, r1, #1
	clz	r2, r2
	strh	r3, [r10, #2]	@ movhi
	lsrs	r2, r2, #5
	ldrh	r3, [r10]
	bfi	r3, r0, #0, #12
	strh	r3, [r10]	@ movhi
	ubfx	r3, r3, #8, #8
	bfi	r3, r2, #4, #1
	strb	r3, [r10, #1]
	ldr	r3, [r4, #12]
	str	fp, [r3, r0, lsl #2]
	strh	r1, [r4, #28]	@ movhi
.L2336:
	cmp	r6, #0
	beq	.L2341
	mov	r1, r5
	mov	r0, r8
	bl	phy_block_from_invalid_page_incr
	b	.L2341
.L2331:
	ldr	r6, .L2354+12
	cmp	r2, r6
	bne	.L2337
	mov	r1, r3
	bl	get_logic_page_map
	mov	r7, r0
	cmp	r0, #0
	beq	.L2338
	ldrh	r3, [r0, #2]
	movw	r2, #65535
	cmp	r3, r2
	beq	.L2349
	ldr	r2, [r8, #4]
	movs	r5, #36
	ldr	r1, [r2, #32]
	mla	r5, r5, r3, r1
	ldr	r3, [r4]
	cmp	r3, r5
	bne	.L2348
	ldrh	r3, [r0]
	mov	r0, #-1
	ldr	r1, [r4, #12]
	ldrh	r2, [r2, #20]
	ubfx	r3, r3, #0, #12
	str	r0, [r1, r3, lsl #2]
	ldrh	r3, [r5, #4]
	adds	r3, r3, #1
	uxth	r3, r3
	cmp	r2, r3
	strh	r3, [r5, #4]	@ movhi
	bcs	.L2349
	ldrh	r1, [r4, #28]
	ldr	r0, .L2354+16
	bl	NAND_Print
.L2349:
	movs	r6, #0
	b	.L2339
.L2348:
	movs	r6, #1
.L2339:
	movw	r3, #65535
	movw	r2, #4095
	strh	r3, [r7, #2]	@ movhi
	ldrh	r3, [r7]
	bfi	r3, r2, #0, #12
	strh	r3, [r7]	@ movhi
	ubfx	r3, r3, #8, #8
	orr	r3, r3, #16
	strb	r3, [r7, #1]
	b	.L2336
.L2337:
	ldr	r5, .L2354+20
	cmp	r2, r5
	bne	.L2341
	ldr	r6, [r1]
	ldrh	r5, [r6, #4]
	adds	r5, r5, #1
	strh	r5, [r6, #4]	@ movhi
	str	r2, [sp]
	ldr	r2, [sp, #40]
	bl	write_phy_data_to_current_block
	mov	r7, r0
	cbz	r0, .L2342
	ldr	r0, .L2354+24
	b	.L2353
.L2342:
	ldrh	r3, [r4, #28]
	adds	r3, r3, #1
	strh	r3, [r4, #28]	@ movhi
.L2341:
	mov	r0, r8
	movs	r7, #0
	bl	adjust_invaild_list
	b	.L2330
.L2355:
	.align	2
.L2354:
	.word	.LC365
	.word	.LC366
	.word	.LC367
	.word	-1441722095
	.word	.LC368
	.word	-1441713357
	.word	.LC369
	.size	write_logic_data_to_current_block, .-write_logic_data_to_current_block
	.section	.text.write_data_to_page,"ax",%progbits
	.align	1
	.global	write_data_to_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	write_data_to_page, %function
write_data_to_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r4, r5, r6, r7, r8, lr}
	mov	r6, r1
	str	r3, [sp]
	mov	r7, r2
	mov	r8, r3
	mov	r3, r2
	mov	r2, r1
	add	r1, r0, #292
	mov	r4, r0
	bl	write_logic_data_to_current_block
	mov	r5, r0
	cbz	r0, .L2357
.L2360:
	movs	r5, #1
	ldr	r0, .L2362
	bl	NAND_Print
.L2356:
	mov	r0, r5
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L2357:
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r3, r3, #31
	bpl	.L2356
	str	r8, [sp]
	mov	r3, r7
	mov	r2, r6
	add	r1, r4, #324
	mov	r0, r4
	bl	write_logic_data_to_current_block
	cmp	r0, #0
	beq	.L2356
	b	.L2360
.L2363:
	.align	2
.L2362:
	.word	.LC370
	.size	write_data_to_page, .-write_data_to_page
	.section	.text._nftl_discard_logic_page,"ax",%progbits
	.align	1
	.global	_nftl_discard_logic_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nftl_discard_logic_page, %function
_nftl_discard_logic_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	mov	r2, r1
	movs	r3, #0
	ldr	r1, .L2365
	b	write_data_to_page
.L2366:
	.align	2
.L2365:
	.word	-1441722095
	.size	_nftl_discard_logic_page, .-_nftl_discard_logic_page
	.section	.text.get_new_current_block,"ax",%progbits
	.align	1
	.global	get_new_current_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_new_current_block, %function
get_new_current_block:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	mov	r4, r1
	ldr	r3, [r1]
	mov	r6, r0
	ldr	r2, [r1, #16]
	movs	r1, #255
	ldr	r0, [r4, #12]
	ldr	r5, [r3, #8]
	bl	NAND_Memset
	ldrh	r3, [r6, #578]
	cbz	r3, .L2368
	ldr	r3, [r6, #4]
	mov	r0, r6
	ldrh	r1, [r3, #16]
	bl	out_phy_block_from_free_list_by_erase_time
	str	r0, [r4]
	cbz	r0, .L2369
	ldrh	r2, [r0, #6]
	ldrh	r1, [r0]
	ldr	r0, .L2376
	bl	NAND_Print_DBG
.L2369:
	ldr	r1, [r4]
	cbnz	r1, .L2370
	ldr	r0, .L2376+4
	bl	NAND_Print
	movs	r0, #1
.L2367:
	pop	{r3, r4, r5, r6, r7, pc}
.L2368:
	mov	r0, r6
	bl	out_phy_block_from_free_list
	str	r0, [r4]
	b	.L2369
.L2370:
	adds	r2, r5, #1
	movs	r7, #0
	strh	r7, [r4, #28]	@ movhi
	str	r2, [r1, #8]
	ldrb	r3, [r6, #17]	@ zero_extendqisi2
	ands	r0, r3, #1
	ittt	ne
	addne	r2, r5, #2
	movne	r0, r7
	strne	r2, [r1, #8]
	b	.L2367
.L2377:
	.align	2
.L2376:
	.word	.LC371
	.word	.LC372
	.size	get_new_current_block, .-get_new_current_block
	.section	.text.new_block_init_for_write,"ax",%progbits
	.align	1
	.global	new_block_init_for_write
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	new_block_init_for_write, %function
new_block_init_for_write:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #4]
	push	{r4, r5, r6, r7, lr}
	mov	r7, r2
	ldrh	r2, [r1]
	sub	sp, sp, #20
	movs	r6, #0
	mov	r5, r1
	ldrh	r3, [r3, #28]
	mov	r1, sp
	strh	r2, [sp, #2]	@ movhi
	mov	r4, r0
	strh	r6, [sp]	@ movhi
	strh	r3, [sp, #4]	@ movhi
	strd	r6, r6, [sp, #8]
	ldr	r3, [r0, #1980]
	blx	r3
	ldrh	r3, [r5, #6]
	ldr	r2, [r4, #376]
	strh	r6, [r5, #4]	@ movhi
	adds	r3, r3, #1
	uxth	r3, r3
	cmp	r3, r2
	strh	r3, [r5, #6]	@ movhi
	bls	.L2379
	ldr	r2, [r4, #4]
	ldrh	r2, [r2, #16]
	cmp	r2, r3
	it	hi
	strhi	r3, [r4, #376]
.L2379:
	str	r7, [r5, #8]
	cbz	r0, .L2378
	ldr	r3, [r4, #2000]
	mov	r1, sp
	mov	r0, r4
	blx	r3
	ldr	r0, .L2384
	bl	NAND_Print
	movs	r0, #1
.L2378:
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L2385:
	.align	2
.L2384:
	.word	.LC373
	.size	new_block_init_for_write, .-new_block_init_for_write
	.section	.text.get_current_used_block,"ax",%progbits
	.align	1
	.global	get_current_used_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	get_current_used_block, %function
get_current_used_block:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r4, r5, r6, r7, r8, r10, lr}
	mov	r4, r0
	mov	r5, r2
	mov	r6, r1
	cmp	r1, #0
	beq	.L2387
	ldr	r3, [r0, #292]
	cmp	r3, r1
	beq	.L2388
	movw	r2, #1014
	ldr	r1, .L2421
	ldr	r0, .L2421+4
	bl	NAND_Print
.L2388:
	ldr	r3, [r4, #292]
	ldrh	r2, [r4, #320]
	ldr	r0, .L2421+8
	ldrh	r1, [r3]
	bl	NAND_Print_DBG
	ldrh	r3, [r4, #320]
	mov	r0, r4
	ldr	r2, [r4, #304]
	ldr	r1, [r4, #292]
	bl	recover_logic_page_mapping
	ldr	r3, [r4, #292]
	ldrh	r1, [r4, #320]
	ldrh	r2, [r3, #4]
	cmp	r1, r2
	bcs	.L2389
	ldr	r0, .L2421+12
	bl	NAND_Print
.L2389:
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r3, r3, #31
	bpl	.L2419
	ldr	r6, .L2421+16
	ldr	r8, .L2421+44
.L2390:
	ldr	r7, [r4, #324]
	cbz	r7, .L2392
.L2401:
	ldr	r2, [r4, #336]
	mov	r0, r4
	ldr	r1, [r4, #324]
	bl	recover_phy_page_mapping
	uxth	r3, r0
	movw	r2, #65535
	cmp	r3, r2
	strh	r3, [r4, #352]	@ movhi
	bne	.L2417
	ldr	r0, .L2421+20
.L2418:
	bl	NAND_Print
	ldr	r1, [r4, #324]
	mov	r0, r4
	ldrh	r2, [r1, #6]
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
	movs	r3, #0
	str	r3, [r4, #324]
	b	.L2390
.L2392:
	mov	r0, r4
	bl	out_phy_block_from_free_list
	mov	r1, r0
	str	r0, [r4, #324]
	cbnz	r0, .L2395
.L2420:
	movs	r0, #1
	b	.L2386
.L2395:
	ldr	r3, [r4, #292]
	strh	r7, [r4, #352]	@ movhi
	ldr	r2, [r3, #8]
	adds	r2, r2, #1
	str	r2, [r0, #8]
	mov	r0, r4
	bl	new_block_init_for_write
	cbz	r0, .L2396
	mov	r0, r8
	bl	NAND_Print
	str	r7, [r4, #324]
	b	.L2390
.L2396:
	ldrh	r10, [r4, #320]
	mov	r5, r0
.L2398:
	cmp	r5, r10
	ldr	r2, [r4, #324]
	bcs	.L2401
	uxth	r3, r5
	ldr	r7, [r4, #1992]
	ldr	r1, [r4, #292]
	mov	r0, r4
	str	r3, [sp]
	ldr	r3, [r4, #284]
	blx	r7
	cbz	r0, .L2399
	mov	r0, r6
	bl	NAND_Print
.L2399:
	adds	r5, r5, #1
	b	.L2398
.L2417:
	ldr	r3, [r4, #292]
	ldr	r1, [r4, #324]
	ldr	r2, [r3, #8]
	uxth	r3, r0
	mov	r0, r4
	adds	r2, r2, #1
	str	r2, [r1, #8]
	ldr	r2, [r4, #336]
	bl	recover_logic_page_mapping
	ldrh	r1, [r4, #352]
	ldrh	r2, [r4, #320]
	cmp	r1, r2
	bls	.L2402
	ldr	r0, .L2421+24
	bl	NAND_Print
	ldrh	r2, [r4, #320]
	ldrh	r1, [r4, #352]
	ldr	r0, .L2421+28
	bl	NAND_Print
.L2419:
	movs	r0, #0
.L2386:
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L2402:
	bcs	.L2419
	ldr	r0, .L2421+32
	b	.L2418
.L2387:
	ldr	r0, .L2421+36
	bl	NAND_Print_DBG
	ldr	r3, [r4, #172]
	cbz	r3, .L2404
	mov	r0, r4
	bl	out_phy_block_from_free_list
	str	r0, [r4, #292]
	cmp	r0, #0
	beq	.L2420
	adds	r5, r5, #1
	strh	r6, [r4, #320]	@ movhi
	str	r5, [r0, #8]
	ldrb	r0, [r4, #17]	@ zero_extendqisi2
	ands	r0, r0, #1
	beq	.L2406
	mov	r0, r4
	bl	out_phy_block_from_free_list
	str	r0, [r4, #324]
	cmp	r0, #0
	beq	.L2420
	ldr	r3, [r4, #292]
	strh	r6, [r4, #352]	@ movhi
	ldr	r3, [r3, #8]
	adds	r3, r3, #1
	str	r3, [r0, #8]
	b	.L2419
.L2406:
	str	r0, [r4, #324]
	b	.L2386
.L2404:
	ldr	r0, .L2421+40
	bl	NAND_Print
	mov	r0, r4
	bl	print_free_list
	mov	r0, r4
	bl	print_block_invalid_list
	b	.L2420
.L2422:
	.align	2
.L2421:
	.word	.LANCHOR107
	.word	.LC374
	.word	.LC375
	.word	.LC376
	.word	.LC378
	.word	.LC379
	.word	.LC380
	.word	.LC381
	.word	.LC382
	.word	.LC383
	.word	.LC384
	.word	.LC377
	.size	get_current_used_block, .-get_current_used_block
	.section	.text.cross_talk_rebuild_current_block,"ax",%progbits
	.align	1
	.global	cross_talk_rebuild_current_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	cross_talk_rebuild_current_block, %function
cross_talk_rebuild_current_block:
	@ args = 0, pretend = 0, frame = 40
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	sub	sp, sp, #56
	mov	r6, r1
	movs	r7, #0
	add	r1, sp, #56
	mov	r5, r0
	str	r7, [r1, #-20]!
	mov	r4, r2
	bl	init_nrc
	cbz	r0, .L2424
	ldr	r0, [sp, #36]
	mov	r4, r7
	bl	free_nrc
	ldr	r0, .L2444
	bl	NAND_Print
.L2425:
	mov	r0, r4
	add	sp, sp, #56
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2424:
	ldr	r3, [sp, #36]
	add	r2, sp, #28
	mov	r1, r6
	mov	r0, r5
	bl	get_used_page_num
	mov	r10, r0
	cbnz	r4, .L2426
	ldr	r3, [r6, #8]
	mov	r2, r0
	ldrh	r1, [r6]
	ldr	r0, .L2444+4
	bl	NAND_Print
	ldr	r0, [sp, #36]
	ldr	r3, [r0]
	cmp	r3, #0
	bne	.L2439
	bl	free_nrc
	ldr	r0, .L2444+8
	bl	NAND_Print
	ldrh	r2, [r6, #6]
	mov	r1, r6
	mov	r0, r5
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
	b	.L2425
.L2426:
	ldr	r3, [sp, #36]
	add	r2, sp, #32
	mov	r1, r4
	mov	r0, r5
	bl	get_used_page_num
	ldr	r3, [r4, #8]
	mov	fp, r0
	ldrh	r1, [r6]
	mov	r2, r10
	strd	r0, r3, [sp, #4]
	ldrh	r3, [r4]
	ldr	r0, .L2444+12
	str	r3, [sp]
	ldr	r3, [r6, #8]
	bl	NAND_Print
	ldr	r0, [sp, #36]
	ldr	r3, [r0]
	cbnz	r3, .L2428
	str	r3, [sp, #20]
	bl	free_nrc
	ldr	r0, .L2444+16
	bl	NAND_Print
	ldrh	r2, [r6, #6]
	mov	r1, r6
	mov	r0, r5
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
	ldrh	r2, [r4, #6]
	mov	r1, r4
	mov	r0, r5
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
	ldr	r3, [sp, #20]
	mov	r4, r3
	b	.L2425
.L2428:
	ldrh	r7, [r4, #6]
	cmp	r10, fp
	ldrh	r8, [r6, #6]
	ittt	ls
	movls	r2, r7
	movls	r7, r8
	movls	r8, r2
	ldr	r2, [sp, #28]
	cbnz	r2, .L2430
	ldr	r2, [sp, #32]
	cbz	r2, .L2431
.L2430:
	adds	r7, r7, #20
	add	r8, r8, #20
.L2431:
	cmp	r10, fp
	add	r7, r7, #1
	uxth	r7, r7
	mov	r2, fp
	mov	r1, r10
	bcs	.L2432
	ldr	r0, .L2444+20
	bl	NAND_Print
	mov	r1, r6
	mov	r6, r4
	mov	r2, r7
	mov	r0, r5
	bl	erase_block
.L2427:
	ldr	r7, .L2444+24
.L2435:
	mov	r0, r5
	bl	out_phy_block_from_free_list
	mov	r4, r0
	cbnz	r0, .L2433
	ldr	r0, .L2444+28
	bl	NAND_Print
	b	.L2425
.L2432:
	ldr	r0, .L2444+32
	bl	NAND_Print
	mov	r2, r7
	mov	r1, r4
	mov	r0, r5
	bl	erase_block
	b	.L2427
.L2439:
	mov	r8, r4
	b	.L2427
.L2433:
	ldr	r2, [r6, #8]
	mov	r1, r0
	adds	r2, r2, #1
	str	r2, [r0, #8]
	mov	r0, r5
	bl	new_block_init_for_write
	cmp	r0, #0
	beq	.L2440
	mov	r0, r7
	bl	NAND_Print
	b	.L2435
.L2438:
	ldr	r3, [r0, #4]
	ldrb	r3, [r3, r10]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L2436
	ldr	r2, [r5, #4]
	ldr	r0, [r0, #8]
	ldrh	r1, [r2, #26]
	ldrh	r2, [r2, #28]
	add	r3, r1, #64
	mla	r3, r10, r3, r0
	ldrh	r0, [r4]
	strh	r7, [sp, #40]	@ movhi
	strh	r2, [sp, #44]	@ movhi
	adds	r7, r7, #1
	strh	r0, [sp, #42]	@ movhi
	add	r1, r1, r3
	mov	r0, r1
	strd	r3, r1, [sp, #48]
	str	r1, [sp, #20]
	bl	get_special_data_from_oob
	ldrh	r3, [r4, #6]
	mov	r2, r0
	ldr	r1, [sp, #20]
	mov	r0, r5
	str	r3, [sp]
	ldr	r3, [r4, #8]
	bl	set_oob_special_page
	ldr	r3, [r5, #1988]
	add	r1, sp, #40
	mov	r0, r5
	blx	r3
.L2437:
	add	r10, r10, #1
.L2434:
	ldr	r0, [sp, #36]
	ldr	r2, [r0]
	cmp	r2, r10
	bhi	.L2438
	add	r2, r8, #1
	mov	r1, r6
	uxth	r2, r2
	mov	r0, r5
	bl	erase_block
	ldr	r0, [sp, #36]
	bl	free_nrc
	b	.L2425
.L2436:
	mov	r1, r10
	mov	r0, fp
	bl	NAND_Print
	b	.L2437
.L2440:
	mov	r7, r0
	mov	r10, r0
	ldr	fp, .L2444+36
	b	.L2434
.L2445:
	.align	2
.L2444:
	.word	.LC385
	.word	.LC386
	.word	.LC387
	.word	.LC388
	.word	.LC389
	.word	.LC390
	.word	.LC393
	.word	.LC392
	.word	.LC391
	.word	.LC394
	.size	cross_talk_rebuild_current_block, .-cross_talk_rebuild_current_block
	.section	.text.no_cross_talk_rebuild_current_block,"ax",%progbits
	.align	1
	.global	no_cross_talk_rebuild_current_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	no_cross_talk_rebuild_current_block, %function
no_cross_talk_rebuild_current_block:
	@ args = 0, pretend = 0, frame = 56
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	sub	sp, sp, #72
	mov	r6, r1
	movs	r7, #0
	add	r1, sp, #32
	mov	r5, r0
	mov	r4, r2
	strd	r7, r7, [sp, #32]
	bl	init_nrc
	cbz	r0, .L2447
	ldr	r0, .L2480
	bl	NAND_Print
	ldr	r0, [sp, #32]
	bl	free_nrc
.L2478:
	movs	r4, #0
	b	.L2448
.L2447:
	ldr	r3, [sp, #32]
	add	r2, sp, #24
	mov	r1, r6
	mov	r0, r5
	bl	get_used_page_num_no_crosstalk
	mov	r8, r0
	cbnz	r4, .L2449
	ldr	r3, [r6, #8]
	mov	r2, r0
	ldrh	r1, [r6]
	ldr	r0, .L2480+4
	bl	NAND_Print
	ldr	r7, [sp, #32]
	ldr	r3, [r7]
	cbz	r3, .L2477
	ldr	r8, .L2480+40
.L2450:
	mov	r0, r5
	bl	out_phy_block_from_free_list
	mov	r4, r0
	cbnz	r0, .L2451
.L2459:
	ldr	r0, .L2480+8
.L2479:
	bl	NAND_Print
	b	.L2478
.L2477:
	mov	r0, r7
	bl	free_nrc
	ldr	r0, .L2480+12
	bl	NAND_Print
	ldrh	r2, [r6, #6]
	mov	r1, r6
	mov	r0, r5
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
.L2448:
	mov	r0, r4
	add	sp, sp, #72
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2451:
	ldr	r2, [r6, #8]
	mov	r1, r0
	str	r2, [r0, #8]
	mov	r0, r5
	bl	new_block_init_for_write
	cmp	r0, #0
	beq	.L2452
	mov	r0, r8
	bl	NAND_Print
	b	.L2450
.L2449:
	add	r1, sp, #36
	mov	r0, r5
	bl	init_nrc
	cbz	r0, .L2453
	ldr	r0, [sp, #32]
	bl	free_nrc
	ldr	r0, [sp, #36]
	bl	free_nrc
	ldr	r0, .L2480+16
	b	.L2479
.L2453:
	ldr	r3, [sp, #36]
	add	r2, sp, #28
	mov	r1, r4
	mov	r0, r5
	bl	get_used_page_num_no_crosstalk
	ldr	r3, [r4, #8]
	mov	r10, r0
	ldrh	r1, [r6]
	mov	r2, r8
	strd	r0, r3, [sp, #4]
	ldrh	r3, [r4]
	ldr	r0, .L2480+20
	str	r3, [sp]
	ldr	r3, [r6, #8]
	bl	NAND_Print
	ldr	r7, [sp, #36]
	ldr	fp, [r7]
	cmp	fp, #0
	bne	.L2454
	ldr	r0, [sp, #32]
	bl	free_nrc
	ldr	r0, [sp, #36]
	bl	free_nrc
	ldr	r0, .L2480+24
	bl	NAND_Print
	ldrh	r2, [r6, #6]
	mov	r1, r6
	mov	r0, r5
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
	ldrh	r2, [r4, #6]
	mov	r1, r4
	mov	r0, r5
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
	b	.L2478
.L2454:
	cmp	r8, r10
	bcs	.L2455
	mov	r3, fp
	mov	r2, r10
	mov	r1, r8
	ldr	r0, .L2480+28
	bl	NAND_Print
	mov	r3, r6
	mov	r6, r4
	mov	r4, r3
.L2456:
	ldr	r3, [r5, #4]
	mov	r8, #0
	ldrh	r2, [r4]
	add	r1, sp, #40
	strh	r8, [sp, #40]	@ movhi
	mov	r0, r5
	ldrh	r3, [r3, #28]
	strh	r2, [sp, #42]	@ movhi
	strd	r8, r8, [sp, #48]
	strh	r3, [sp, #44]	@ movhi
	ldr	r3, [r5, #1980]
	blx	r3
	cbz	r0, .L2457
	ldrh	r1, [r4]
	ldr	r0, .L2480+32
	bl	NAND_Print
	ldr	r3, [r5, #2000]
	add	r1, sp, #40
	mov	r0, r5
	blx	r3
	movs	r3, #255
	strh	r8, [r6, #4]	@ movhi
	strh	r3, [r6, #2]	@ movhi
	ldr	r8, .L2480+40
	ldrh	r3, [r5, #20]
	adds	r3, r3, #1
	strh	r3, [r5, #20]	@ movhi
.L2458:
	mov	r0, r5
	bl	out_phy_block_from_free_list
	mov	r4, r0
	cmp	r0, #0
	beq	.L2459
	ldr	r2, [r0, #8]
	mov	r1, r0
	mov	r0, r5
	bl	new_block_init_for_write
	cbz	r0, .L2457
	mov	r0, r8
	bl	NAND_Print
	b	.L2458
.L2455:
	ldr	r7, [sp, #32]
	mov	r2, r10
	mov	r1, r8
	ldr	r0, .L2480+36
	ldr	r3, [r7]
	bl	NAND_Print
	b	.L2456
.L2457:
	ldr	r3, [r6, #8]
	str	r3, [r4, #8]
.L2452:
	ldr	fp, .L2480+44
	mov	r10, #0
	mov	r8, r10
.L2460:
	ldr	r2, [r7]
	cmp	r2, r8
	bhi	.L2463
	ldrh	r2, [r6, #6]
	mov	r1, r6
	mov	r0, r5
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
	ldr	r2, [r7]
	ldr	r1, [r7, #12]
	ldr	r0, [r5, #304]
	strh	r2, [r5, #320]	@ movhi
	str	r4, [r5, #292]
	lsls	r2, r2, #2
	bl	NAND_Memcpy
	ldr	r0, [sp, #32]
	bl	free_nrc
	ldr	r0, [sp, #36]
	bl	free_nrc
	b	.L2448
.L2463:
	ldr	r3, [r7, #4]
	ldrb	r3, [r3, r8]	@ zero_extendqisi2
	cbz	r3, .L2461
	ldr	r2, [r5, #4]
	ldr	r0, [r7, #8]
	ldrh	r1, [r2, #26]
	ldrh	r2, [r2, #28]
	add	r3, r1, #64
	mla	r3, r8, r3, r0
	ldrh	r0, [r4]
	strh	r10, [sp, #56]	@ movhi
	strh	r2, [sp, #60]	@ movhi
	add	r10, r10, #1
	strh	r0, [sp, #58]	@ movhi
	add	r1, r1, r3
	mov	r0, r1
	strd	r3, r1, [sp, #64]
	str	r1, [sp, #20]
	bl	get_special_data_from_oob
	ldrh	r3, [r4, #6]
	mov	r2, r0
	ldr	r1, [sp, #20]
	mov	r0, r5
	str	r3, [sp]
	ldr	r3, [r4, #8]
	bl	set_oob_special_page
	ldr	r3, [r5, #1988]
	add	r1, sp, #56
	mov	r0, r5
	blx	r3
.L2462:
	add	r8, r8, #1
	b	.L2460
.L2461:
	mov	r1, r8
	mov	r0, fp
	bl	NAND_Print
	b	.L2462
.L2481:
	.align	2
.L2480:
	.word	.LC385
	.word	.LC395
	.word	.LC392
	.word	.LC396
	.word	.LC389
	.word	.LC397
	.word	.LC398
	.word	.LC390
	.word	.LC399
	.word	.LC391
	.word	.LC393
	.word	.LC394
	.size	no_cross_talk_rebuild_current_block, .-no_cross_talk_rebuild_current_block
	.section	.text.second_scan_all_blocks,"ax",%progbits
	.align	1
	.global	second_scan_all_blocks
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	second_scan_all_blocks, %function
second_scan_all_blocks:
	@ args = 0, pretend = 0, frame = 48
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L2565
	ldr	r2, .L2565+4
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r4, r0
	ldr	r3, [r3]
	sub	sp, sp, #104
	ldr	r3, [r3, #752]
	cmp	r3, r2
	beq	.L2483
	subs	r3, r3, #1
	cmp	r3, #1
	bhi	.L2518
.L2483:
	ldr	r3, [r4, #4]
	ldr	r0, [r4, #24]
	ldrb	r1, [r3, #8]	@ zero_extendqisi2
	bl	__aeabi_uidiv
	ldr	r5, [r4, #252]
	mov	r3, #-1
	str	r0, [r4, #28]
	str	r3, [sp, #64]
	mov	r10, r5
.L2485:
	cmp	r10, #0
	bne	.L2486
	cmp	r5, #0
	beq	.L2519
	ldr	r3, [r4, #1968]
	mov	r7, r10
	ldr	r2, [r5, #8]
	mov	r6, r10
	mov	r8, r10
	str	r10, [sp, #68]
	str	r2, [r3, #160]
.L2502:
	ldr	r2, [r4, #308]
	lsrs	r3, r2, #9
	ubfx	r2, r2, #0, #9
	cbz	r2, .L2489
	adds	r3, r3, #1
.L2489:
	ldr	r2, [r4, #4]
	ldrh	r0, [r5]
	ldr	r1, [r4, #284]
	ldrh	r2, [r2, #20]
	strh	r3, [sp, #92]	@ movhi
	add	r3, sp, #72
	strh	r0, [sp, #90]	@ movhi
	mov	r0, r4
	subs	r2, r2, #1
	str	r1, [sp, #96]
	str	r3, [sp, #100]
	add	r1, sp, #88
	ldr	r3, [r4, #1984]
	strh	r2, [sp, #88]	@ movhi
	blx	r3
	ldr	r3, [r4, #1972]
	mov	fp, r0
	ldrh	r3, [r3, #28]
	cbz	r3, .L2490
	add	r2, sp, #72
	ldr	r1, [r4, #284]
	mov	r0, r4
	bl	crc_wrong
	cmp	r0, #0
	it	ne
	mvnne	fp, #1
.L2490:
	ldr	r3, [r5, #8]
	ldr	r1, [r4, #376]
	ldr	r2, [r4, #1968]
	str	r3, [sp, #60]
	ldrh	r3, [r5, #6]
	cmp	r3, r1
	bls	.L2491
	movw	r1, #49999
	cmp	r3, r1
	itt	ls
	strls	r3, [r4, #376]
	strls	r3, [r2, #148]
.L2491:
	ldr	r1, [r2, #152]
	add	r0, sp, #72
	cmp	r3, r1
	it	cc
	strcc	r3, [r2, #152]
	bl	is_phy_mapping_page
	cmp	r0, #1
	bne	.L2493
	cmp	fp, #0
	beq	.L2494
	cmp	fp, #10
	bne	.L2493
.L2494:
	ldrh	r3, [r5, #4]
	mov	r10, r5
	ldr	r2, [r4, #284]
	mov	r1, r5
	mov	r0, r4
	adds	r3, r3, #1
	strh	r3, [r5, #4]	@ movhi
	ldr	r3, [r4, #4]
	ldrh	r3, [r3, #20]
	subs	r3, r3, #1
	bl	recover_logic_page_mapping
.L2559:
	mov	r8, r5
	b	.L2495
.L2486:
	ldr	r3, [r10, #8]
	ldr	r10, [r10, #28]
	str	r3, [sp, #64]
	ldr	r2, [sp, #64]
	ldr	r3, [r4, #1968]
	str	r2, [r3, #156]
	b	.L2485
.L2493:
	add	r0, sp, #72
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L2496
	cbz	r6, .L2521
	ldrh	r1, [r6]
	cbnz	r7, .L2497
	ldrh	r2, [r5]
	ldr	r0, .L2565+8
	bl	NAND_Print
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r0, r3, #31
	bmi	.L2523
	ldr	r3, [r6, #8]
	ldr	r0, [r5, #8]
	cmp	r3, r0
	beq	.L2523
	ldrh	r2, [r5]
	ldrh	r1, [r6]
	str	r0, [sp]
	ldr	r0, .L2565+12
	bl	NAND_Print
	ldr	r2, [r4, #304]
	mov	r1, r6
	mov	r0, r4
	bl	recover_phy_page_mapping
	ldr	r3, [r4, #4]
	mov	r1, r6
	ldr	r2, [r4, #304]
	mov	r0, r4
	ldrh	r3, [r3, #20]
	subs	r3, r3, #1
	bl	recover_logic_page_mapping
	movs	r2, #3
	mov	r1, r6
	mov	r0, r4
	bl	add_prio_gc
.L2521:
	mov	r6, r5
	b	.L2495
.L2497:
	ldr	r2, [r6, #8]
	ldr	r0, .L2565+16
	bl	NAND_Print
	ldr	r2, [r7, #8]
	ldrh	r1, [r7]
	ldr	r0, .L2565+20
	bl	NAND_Print
	ldr	r2, [sp, #60]
	ldrh	r1, [r5]
	ldr	r0, .L2565+24
	bl	NAND_Print
	mov	r1, r6
	mov	r6, r7
	movs	r2, #3
	mov	r0, r4
	bl	add_prio_gc
.L2523:
	mov	r7, r5
	b	.L2495
.L2496:
	ldrb	r0, [sp, #87]	@ zero_extendqisi2
	ldrb	r2, [sp, #73]	@ zero_extendqisi2
	ldrb	r1, [sp, #72]	@ zero_extendqisi2
	str	r0, [sp, #48]
	ldrb	r0, [sp, #86]	@ zero_extendqisi2
	ldrb	r3, [sp, #74]	@ zero_extendqisi2
	str	r0, [sp, #44]
	ldrb	r0, [sp, #85]	@ zero_extendqisi2
	str	r0, [sp, #40]
	ldrb	r0, [sp, #84]	@ zero_extendqisi2
	str	r0, [sp, #36]
	ldrb	r0, [sp, #83]	@ zero_extendqisi2
	str	r0, [sp, #32]
	ldrb	r0, [sp, #82]	@ zero_extendqisi2
	str	r0, [sp, #28]
	ldrb	r0, [sp, #81]	@ zero_extendqisi2
	str	r0, [sp, #24]
	ldrb	r0, [sp, #80]	@ zero_extendqisi2
	str	r0, [sp, #20]
	ldrb	r0, [sp, #79]	@ zero_extendqisi2
	str	r0, [sp, #16]
	ldrb	r0, [sp, #78]	@ zero_extendqisi2
	str	r0, [sp, #12]
	ldrb	r0, [sp, #77]	@ zero_extendqisi2
	str	r0, [sp, #8]
	ldrb	r0, [sp, #76]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [sp, #75]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L2565+28
	bl	NAND_Print
	ldrh	r2, [r5]
	ldrb	r1, [r4, #16]	@ zero_extendqisi2
	cmp	fp, #0
	beq	.L2498
	cmp	fp, #10
	bne	.L2499
.L2498:
	ldr	r0, .L2565+32
.L2558:
	bl	NAND_Print
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r1, r3, #31
	bpl	.L2501
	ldrd	r2, r3, [sp, #60]
	cmp	r3, r2
	bne	.L2501
	ldrh	r2, [r5, #6]
	mov	r1, r5
	mov	r0, r4
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
	str	r5, [sp, #68]
.L2495:
	ldr	r5, [r5, #28]
	cmp	r5, #0
	bne	.L2502
.L2487:
	mov	r1, r10
	mov	r0, r4
	bl	init_smart_info
	ldr	r5, [r4, #252]
.L2503:
	cmp	r5, #0
	bne	.L2505
	mov	r0, r4
	bl	adjust_invaild_list
	cmp	r8, #0
	beq	.L2506
	cbz	r6, .L2506
	ldr	r1, [r8, #8]
	ldr	r2, [r6, #8]
	adds	r3, r1, #1
	cmp	r3, r2
	beq	.L2506
	ldr	r0, .L2565+36
	bl	NAND_Print
.L2506:
	movs	r3, #0
	str	r3, [r4, #292]
	cmp	r6, #0
	bne	.L2507
	cmp	r7, #0
	bne	.L2508
	ldr	r0, .L2565+40
	bl	NAND_Print
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r2, r3, #31
	bpl	.L2509
	cmp	r8, #0
	beq	.L2510
.L2563:
	movs	r2, #3
	mov	r1, r8
	mov	r0, r4
	bl	add_prio_gc
	b	.L2510
.L2499:
	ldr	r0, .L2565+44
	b	.L2558
.L2501:
	movs	r3, #170
	ldr	r2, [r4, #304]
	strh	r3, [r5, #2]	@ movhi
	mov	r1, r5
	ldrh	r3, [r5, #4]
	mov	r0, r4
	adds	r3, r3, #1
	strh	r3, [r5, #4]	@ movhi
	bl	recover_block_phy_page_mapping
	ldr	r3, [r4, #4]
	mov	r1, r5
	ldr	r2, [r4, #304]
	mov	r0, r4
	ldrh	r3, [r3, #20]
	subs	r3, r3, #1
	bl	recover_logic_page_mapping
	movs	r2, #3
	mov	r1, r5
	mov	r0, r4
	bl	add_prio_gc
	b	.L2559
.L2566:
	.align	2
.L2565:
	.word	g_nreg_base
	.word	196609
	.word	.LC400
	.word	.LC401
	.word	.LC402
	.word	.LC403
	.word	.LC404
	.word	.LC405
	.word	.LC406
	.word	.LC408
	.word	.LC409
	.word	.LC407
.L2519:
	mov	r3, #-1
	mov	r7, r10
	mov	r6, r10
	mov	r8, r10
	str	r10, [sp, #68]
	str	r3, [sp, #60]
	b	.L2487
.L2505:
	cmp	r6, r5
	beq	.L2504
	cmp	r7, r5
	beq	.L2504
	ldr	r3, [sp, #68]
	cmp	r3, r5
	beq	.L2504
	mov	r1, r5
	mov	r0, r4
	bl	put_phy_block_to_invalid_page_list
.L2504:
	ldr	r5, [r5, #28]
	b	.L2503
.L2509:
	cmp	r8, #0
	beq	.L2510
	ldrh	r3, [r8, #2]
	cmp	r3, #170
	beq	.L2563
.L2510:
	ldr	r2, [sp, #60]
	mov	r1, r7
	mov	r0, r4
	bl	get_current_used_block
	mov	r5, r0
	cmp	r0, #0
	beq	.L2517
	ldr	r0, .L2567
	bl	NAND_Print
	b	.L2564
.L2507:
	cbnz	r7, .L2508
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r3, r3, #31
	bpl	.L2511
	ldr	r2, [r6, #8]
	ldrh	r1, [r6]
	ldr	r0, .L2567+4
	bl	NAND_Print_DBG
	cmp	r8, #0
	beq	.L2512
	ldrh	r3, [r8, #2]
	cmp	r3, #170
	bne	.L2512
	movs	r2, #3
	mov	r1, r8
	mov	r0, r4
	bl	add_prio_gc
.L2512:
	movs	r2, #0
.L2561:
	mov	r1, r6
	mov	r0, r4
	bl	cross_talk_rebuild_current_block
	mov	r7, r0
	cmp	r0, #0
	bne	.L2510
.L2564:
	movs	r5, #1
.L2482:
	mov	r0, r5
	add	sp, sp, #104
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2511:
	cmp	r8, #0
	beq	.L2513
	ldrh	r3, [r8, #2]
	cmp	r3, #170
	bne	.L2513
	movs	r2, #3
	mov	r1, r8
	mov	r0, r4
	bl	add_prio_gc
.L2513:
	movs	r2, #0
.L2562:
	mov	r1, r6
	mov	r0, r4
	bl	no_cross_talk_rebuild_current_block
	mov	r7, r0
	b	.L2510
.L2508:
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	ands	r5, r3, #1
	beq	.L2514
	mov	r2, r7
	b	.L2561
.L2514:
	ldr	r2, [r6, #8]
	ldr	r3, [r7, #8]
	cmp	r2, r3
	bne	.L2516
	ldrh	r1, [r6]
	ldr	r0, .L2567+8
	bl	NAND_Print
	ldr	r2, [r7, #8]
	ldrh	r1, [r7]
	ldr	r0, .L2567+12
	bl	NAND_Print_DBG
	mov	r2, r7
	b	.L2562
.L2516:
	ldr	r0, .L2567+16
	bl	NAND_Print
	ldr	r2, [r8, #8]
	ldrh	r1, [r8]
	ldr	r0, .L2567+20
	bl	NAND_Print
	ldr	r2, [r6, #8]
	ldrh	r1, [r6]
	ldr	r0, .L2567+8
	bl	NAND_Print
	ldr	r2, [r7, #8]
	ldrh	r1, [r7]
	ldr	r0, .L2567+12
	bl	NAND_Print_DBG
	ldr	r2, [r4, #304]
	mov	r1, r6
	mov	r0, r4
	bl	recover_block_phy_page_mapping
	ldr	r3, [r4, #4]
	mov	r1, r6
	ldr	r2, [r4, #304]
	mov	r0, r4
	ldrh	r3, [r3, #20]
	subs	r3, r3, #1
	bl	recover_logic_page_mapping
	movs	r2, #3
	mov	r1, r6
	mov	r0, r4
	bl	add_prio_gc
	mov	r1, r7
	ldr	r2, [r4, #304]
	mov	r0, r4
	bl	recover_block_phy_page_mapping
	ldr	r3, [r4, #4]
	mov	r1, r7
	ldr	r2, [r4, #304]
	mov	r0, r4
	ldrh	r3, [r3, #20]
	subs	r3, r3, #1
	bl	recover_logic_page_mapping
	mov	r1, r7
	movs	r2, #3
	mov	r0, r4
	mov	r7, r5
	bl	add_prio_gc
	b	.L2510
.L2517:
	mov	r0, r4
	bl	adjust_invaild_list
	ldr	r2, [r4, #1968]
	ldr	r3, [r4, #292]
	ldrh	r1, [r4, #22]
	ldr	r0, .L2567+24
	str	r3, [r4, #48]
	ldr	r3, [r2, #160]
	str	r3, [sp, #4]
	ldr	r3, [r2, #156]
	str	r3, [sp]
	ldrd	r2, r3, [r2, #148]
	bl	NAND_Print_DBG
	b	.L2482
.L2518:
	mov	r5, #-1
	b	.L2482
.L2568:
	.align	2
.L2567:
	.word	.LC415
	.word	.LC410
	.word	.LC411
	.word	.LC412
	.word	.LC413
	.word	.LC414
	.word	.LC416
	.size	second_scan_all_blocks, .-second_scan_all_blocks
	.section	.text.current_block_ecc_error,"ax",%progbits
	.align	1
	.global	current_block_ecc_error
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	current_block_ecc_error, %function
current_block_ecc_error:
	@ args = 0, pretend = 0, frame = 48
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r4, r0
	ldr	r5, .L2589
	sub	sp, sp, #48
	mov	r8, r1
.L2570:
	mov	r0, r4
	bl	out_phy_block_from_free_list
	mov	r7, r0
	cbnz	r0, .L2571
	ldr	r0, .L2589+4
	bl	NAND_Print
.L2569:
	mov	r0, r7
	add	sp, sp, #48
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2571:
	ldr	r2, [r8, #8]
	mov	r1, r0
	str	r2, [r0, #8]
	mov	r0, r4
	bl	new_block_init_for_write
	mov	r6, r0
	cbz	r0, .L2573
	mov	r0, r5
	bl	NAND_Print
	b	.L2570
.L2573:
	ldr	fp, .L2589+32
	mov	r5, r6
	ldrh	r1, [r8]
	ldr	r0, .L2589+8
	bl	NAND_Print
.L2574:
	ldr	r3, [r4, #4]
	ldrh	r2, [r3, #20]
	cmp	r2, r5
	bgt	.L2581
	ldrh	r2, [r8, #6]
	mov	r1, r8
	mov	r0, r4
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
	mov	r2, r6
	mov	r1, r5
	ldr	r0, .L2589+12
	bl	NAND_Print_DBG
	b	.L2569
.L2581:
	ldrh	r2, [r3, #28]
	mov	r0, r4
	ldrh	r1, [r8]
	ldr	r3, [r4, #284]
	strh	r2, [sp, #20]	@ movhi
	strh	r1, [sp, #18]	@ movhi
	add	r1, sp, #16
	str	r3, [sp, #24]
	strh	r5, [sp, #16]	@ movhi
	ldr	r3, [r4, #1984]
	str	sp, [sp, #28]
	blx	r3
	mov	r10, r0
	mov	r0, sp
	bl	is_ftl_logic_page_data
	cmp	r0, #1
	bne	.L2575
	cmp	r10, #0
	beq	.L2576
	cmp	r10, #10
	bne	.L2577
.L2576:
	mov	r2, sp
	ldr	r1, [r4, #284]
	mov	r0, r4
	bl	nand_power_off_ecc_error
	cbz	r0, .L2578
	ldr	r0, .L2589+16
	bl	NAND_Print
.L2579:
	adds	r5, r5, #1
	b	.L2574
.L2578:
	mov	r2, r10
	mov	r1, r5
	ldr	r0, .L2589+20
	bl	NAND_Print
	ldr	r3, [r4, #4]
	mov	r0, r4
	ldrh	r1, [r7]
	strh	r6, [sp, #32]	@ movhi
	adds	r6, r6, #1
	ldrh	r2, [r3, #28]
	ldr	r3, [r4, #284]
	strh	r1, [sp, #34]	@ movhi
	add	r1, sp, #32
	strh	r2, [sp, #36]	@ movhi
	str	r3, [sp, #40]
	str	sp, [sp, #44]
	ldr	r3, [r4, #1988]
	blx	r3
	b	.L2579
.L2577:
	ldr	r0, .L2589+24
	mov	r1, r5
.L2588:
	bl	NAND_Print
	b	.L2579
.L2575:
	ldrb	r3, [sp, #1]	@ zero_extendqisi2
	cmp	r3, #170
	bne	.L2580
	mov	r1, r5
	mov	r0, fp
	b	.L2588
.L2580:
	mov	r0, sp
	bl	is_nouse_page
	cmp	r0, #1
	beq	.L2579
	mov	r1, r5
	ldr	r0, .L2589+28
	b	.L2588
.L2590:
	.align	2
.L2589:
	.word	.LC382
	.word	.LC194
	.word	.LC417
	.word	.LC423
	.word	.LC418
	.word	.LC419
	.word	.LC420
	.word	.LC422
	.word	.LC421
	.size	current_block_ecc_error, .-current_block_ecc_error
	.section	.text.do_before_write,"ax",%progbits
	.align	1
	.global	do_before_write
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	do_before_write, %function
do_before_write:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r4, r0
	add	r5, r0, #292
.L2592:
	ldrh	r3, [r4, #320]
	cbz	r3, .L2595
.L2599:
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r0, r3, #31
	bpl	.L2597
	add	r5, r4, #324
.L2596:
	ldrh	r3, [r4, #352]
	cbnz	r3, .L2597
	ldr	r1, [r4, #324]
	mov	r0, r4
	ldr	r2, [r1, #8]
	bl	new_block_init_for_write
	cbnz	r0, .L2601
.L2597:
	movs	r0, #0
.L2591:
	pop	{r3, r4, r5, pc}
.L2595:
	ldr	r1, [r4, #292]
	mov	r0, r4
	ldr	r2, [r1, #8]
	bl	new_block_init_for_write
	cmp	r0, #0
	beq	.L2599
	ldr	r2, [r4, #292]
	mov	r0, r4
	ldr	r3, [r2, #8]
	subs	r1, r3, #1
	str	r1, [r2, #8]
	ldrb	r1, [r4, #17]	@ zero_extendqisi2
	lsls	r1, r1, #31
	mov	r1, r5
	itt	mi
	submi	r3, r3, #2
	strmi	r3, [r2, #8]
	bl	get_new_current_block
	cmp	r0, #0
	beq	.L2592
	ldr	r0, .L2614
.L2613:
	bl	NAND_Print
	movs	r0, #1
	b	.L2591
.L2601:
	ldr	r2, [r4, #324]
	mov	r0, r4
	ldr	r3, [r2, #8]
	subs	r1, r3, #1
	str	r1, [r2, #8]
	ldrb	r1, [r4, #17]	@ zero_extendqisi2
	lsls	r1, r1, #31
	mov	r1, r5
	itt	mi
	submi	r3, r3, #2
	strmi	r3, [r2, #8]
	bl	get_new_current_block
	cmp	r0, #0
	beq	.L2596
	ldr	r0, .L2614+4
	b	.L2613
.L2615:
	.align	2
.L2614:
	.word	.LC424
	.word	.LC425
	.size	do_before_write, .-do_before_write
	.section	.text.check_invalid_page_right,"ax",%progbits
	.align	1
	.global	check_invalid_page_right
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	check_invalid_page_right, %function
check_invalid_page_right:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	movs	r4, #0
	ldr	fp, .L2625+4
	mov	r8, r0
	mov	r10, r1
	mov	r7, r2
	mov	r6, r3
	mov	r5, r4
.L2617:
	cmp	r6, r5
	bhi	.L2622
	ldrh	r2, [r7, #4]
	cmp	r2, r4
	beq	.L2624
	mov	r3, r6
	mov	r1, r4
	ldr	r0, .L2625
	bl	NAND_Print
	movs	r0, #1
.L2616:
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2622:
	ldr	r3, [r10, #8]
	ldr	r1, [r3, r5, lsl #2]
	adds	r3, r1, #1
	beq	.L2618
	mov	r0, r8
	bl	get_logic_page_map
	cbnz	r0, .L2619
	mov	r0, fp
	bl	NAND_Print_DBG
.L2618:
	adds	r4, r4, #1
	b	.L2620
.L2619:
	ldrh	r2, [r0, #2]
	ldrh	r3, [r7]
	cmp	r2, r3
	bne	.L2618
	ldrh	r3, [r0]
	ubfx	r3, r3, #0, #12
	cmp	r3, r5
	bne	.L2618
.L2620:
	adds	r5, r5, #1
	b	.L2617
.L2624:
	movs	r0, #0
	b	.L2616
.L2626:
	.align	2
.L2625:
	.word	.LC427
	.word	.LC426
	.size	check_invalid_page_right, .-check_invalid_page_right
	.section	.text.write_phy_page_map_to_current_block,"ax",%progbits
	.align	1
	.global	write_phy_page_map_to_current_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	write_phy_page_map_to_current_block, %function
write_phy_page_map_to_current_block:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	add	r3, r0, #324
	push	{r0, r1, r4, r5, r6, lr}
	cmp	r1, r3
	mov	r4, r0
	mov	r5, r1
	bne	.L2628
	ldr	r1, [r0, #312]
	mov	r2, #320
	ldr	r0, [r0, #344]
	bl	NAND_Memcpy
.L2628:
	ldr	r3, .L2631
	mov	r1, r5
	ldr	r2, [r5, #4]
	mov	r0, r4
	str	r3, [sp]
	movs	r3, #0
	bl	write_phy_data_to_current_block
	mov	r6, r0
	cbz	r0, .L2629
	ldr	r0, .L2631+4
	bl	NAND_Print
.L2627:
	mov	r0, r6
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, pc}
.L2629:
	mov	r1, r5
	ldrh	r3, [r5, #28]
	ldr	r2, [r1], #4
	adds	r3, r3, #1
	ldrh	r0, [r2, #4]
	uxth	r3, r3
	strh	r3, [r5, #28]	@ movhi
	adds	r0, r0, #1
	strh	r0, [r2, #4]	@ movhi
	mov	r0, r4
	bl	check_invalid_page_right
	ldr	r3, [r4, #4]
	ldrh	r2, [r5, #28]
	ldrh	r3, [r3, #20]
	cmp	r2, r3
	bne	.L2627
	ldr	r1, [r5]
	mov	r0, r4
	bl	put_phy_block_to_invalid_page_list
	mov	r0, r4
	bl	adjust_invaild_list
	b	.L2627
.L2632:
	.align	2
.L2631:
	.word	-1431633921
	.word	.LC428
	.size	write_phy_page_map_to_current_block, .-write_phy_page_map_to_current_block
	.section	.text.nftl_ops_init,"ax",%progbits
	.align	1
	.global	nftl_ops_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_ops_init, %function
nftl_ops_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L2634
	str	r3, [r0, #1980]
	ldr	r3, .L2634+4
	str	r3, [r0, #1984]
	ldr	r3, .L2634+8
	str	r3, [r0, #1988]
	ldr	r3, .L2634+12
	str	r3, [r0, #1996]
	ldr	r3, .L2634+16
	str	r3, [r0, #2000]
	ldr	r3, .L2634+20
	str	r3, [r0, #1992]
	ldr	r3, .L2634+24
	str	r3, [r0, #2008]
	ldr	r3, .L2634+28
	str	r3, [r0, #2012]
	ldr	r3, .L2634+32
	str	r3, [r0, #2004]
	ldr	r3, .L2634+36
	str	r3, [r0, #2016]
	bx	lr
.L2635:
	.align	2
.L2634:
	.word	_nftl_nand_erase_superblk
	.word	_nftl_nand_read_page
	.word	_nftl_nand_write_page
	.word	_nftl_nand_is_blk_good
	.word	_nftl_nand_mark_bad_blk
	.word	_nftl_nand_copy_page
	.word	_nftl_nand_read_logic_page
	.word	_nftl_nand_read_logic_history_page
	.word	_nftl_nand_write_logic_page
	.word	_nftl_discard_logic_page
	.size	nftl_ops_init, .-nftl_ops_init
	.section	.text.current_block_at_last_page,"ax",%progbits
	.align	1
	.global	current_block_at_last_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	current_block_at_last_page, %function
current_block_at_last_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, [r0, #4]
	ldrh	r0, [r1, #28]
	ldrh	r3, [r3, #20]
	subs	r3, r3, #1
	subs	r3, r3, r0
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	bx	lr
	.size	current_block_at_last_page, .-current_block_at_last_page
	.section	.text.do_after_write,"ax",%progbits
	.align	1
	.global	do_after_write
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	do_after_write, %function
do_after_write:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	add	r6, r0, #292
	mov	r1, r6
	mov	r4, r0
	bl	current_block_at_last_page
	cmp	r0, #1
	mov	r5, r0
	bne	.L2645
	mov	r0, r4
	bl	write_phy_page_map_to_current_block
	cbz	r0, .L2639
	ldr	r0, .L2656
.L2655:
	bl	NAND_Print
.L2637:
	mov	r0, r5
	pop	{r4, r5, r6, pc}
.L2639:
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r2, r3, #31
	bpl	.L2641
	add	r1, r4, #324
	mov	r0, r4
	bl	write_phy_page_map_to_current_block
	cbz	r0, .L2641
	ldr	r0, .L2656
	bl	NAND_Print
.L2641:
	mov	r1, r6
	mov	r0, r4
	bl	get_new_current_block
	cbz	r0, .L2643
	ldr	r0, .L2656+4
	b	.L2655
.L2643:
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r3, r3, #31
	bpl	.L2644
	add	r1, r4, #324
	mov	r0, r4
	bl	get_new_current_block
	cbz	r0, .L2644
	ldr	r0, .L2656+8
	b	.L2655
.L2644:
	ldr	r3, [r4, #368]
	adds	r3, r3, #1
	str	r3, [r4, #368]
.L2645:
	movs	r5, #0
	b	.L2637
.L2657:
	.align	2
.L2656:
	.word	.LC429
	.word	.LC430
	.word	.LC431
	.size	do_after_write, .-do_after_write
	.section	.text.nand_write_data_page,"ax",%progbits
	.align	1
	.global	nand_write_data_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_write_data_page, %function
nand_write_data_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	mov	r6, r1
	add	r1, r0, #292
	mov	r5, r0
	mov	r7, r2
	mov	r8, r3
	bl	current_block_at_last_page
	cmp	r0, #1
	mov	r4, r0
	bne	.L2659
	mov	r0, r5
	bl	do_after_write
	cbz	r0, .L2659
	ldr	r0, .L2670
	bl	NAND_Print
.L2658:
	mov	r0, r4
	pop	{r4, r5, r6, r7, r8, pc}
.L2659:
	mov	r0, r5
	bl	do_before_write
	cbz	r0, .L2661
	ldr	r0, .L2670+4
.L2669:
	bl	NAND_Print
	movs	r4, #1
	b	.L2658
.L2661:
	mov	r3, r8
	mov	r2, r7
	mov	r1, r6
	mov	r0, r5
	bl	write_data_to_page
	cbz	r0, .L2662
	ldr	r0, .L2670+8
	b	.L2669
.L2662:
	mov	r0, r5
	bl	do_after_write
	mov	r4, r0
	cmp	r0, #0
	beq	.L2658
	ldr	r0, .L2670+12
	b	.L2669
.L2671:
	.align	2
.L2670:
	.word	.LC432
	.word	.LC433
	.word	.LC434
	.word	.LC435
	.size	nand_write_data_page, .-nand_write_data_page
	.section	.text.nand_write_logic_page_no_gc,"ax",%progbits
	.align	1
	.global	nand_write_logic_page_no_gc
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_write_logic_page_no_gc, %function
nand_write_logic_page_no_gc:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	mov	r3, r2
	mov	r2, r1
	movs	r1, #192
	b	nand_write_data_page
	.size	nand_write_logic_page_no_gc, .-nand_write_logic_page_no_gc
	.section	.text.garbage_collect_first,"ax",%progbits
	.align	1
	.global	garbage_collect_first
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	garbage_collect_first, %function
garbage_collect_first:
	@ args = 0, pretend = 0, frame = 40
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	sub	sp, sp, #96
	mov	r5, r1
	add	r7, sp, #64
	mov	r4, r0
	mov	r6, r2
	movs	r1, #90
	movs	r2, #16
	mov	r0, r7
	bl	NAND_Memset
	cbnz	r5, .L2674
	mov	r0, r4
	bl	out_phy_block_from_invalid_page_list
.L2714:
	mov	r5, r0
	cbnz	r0, .L2677
	ldr	r0, .L2715
	bl	NAND_Print
	movs	r0, #8
.L2673:
	add	sp, sp, #96
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2674:
	mov	r1, r5
	mov	r0, r4
	bl	is_current_blcok
	cmp	r0, #1
	beq	.L2692
	mov	r0, r4
	bl	out_phy_block_from_invalid_page_list_by_block
	b	.L2714
.L2677:
	ldr	r2, [r4, #1968]
	ldrh	r0, [r0]
	ldr	r3, [r2, #120]
	strh	r0, [sp, #82]	@ movhi
	mov	r0, r4
	adds	r3, r3, #1
	str	r3, [r2, #120]
	ldr	r3, [r4, #4]
	ldr	r2, [r4, #284]
	ldrh	r1, [r3, #28]
	ldrh	r3, [r3, #20]
	strd	r2, r7, [sp, #88]
	strh	r1, [sp, #84]	@ movhi
	add	r1, sp, #80
	subs	r3, r3, #1
	strh	r3, [sp, #80]	@ movhi
	ldr	r3, [r4, #1984]
	blx	r3
	mov	r10, r0
	mov	r0, r7
	bl	is_phy_mapping_page
	cmp	r0, #1
	mov	r8, r0
	beq	.L2678
	ldr	r0, .L2715+4
	mov	r8, #1
	bl	NAND_Print_DBG
	ldr	r3, [r4, #4]
	ldrh	r1, [r5]
	ldr	r0, .L2715+8
	ldrh	r2, [r3, #20]
	ldrh	r3, [r3, #28]
	subs	r2, r2, #1
	str	r3, [sp]
	movs	r3, #0
	bl	NAND_Print_DBG
	ldrb	r0, [sp, #79]	@ zero_extendqisi2
	ldrb	r3, [sp, #67]	@ zero_extendqisi2
	ldrb	r2, [sp, #66]	@ zero_extendqisi2
	str	r0, [sp, #44]
	ldrb	r0, [sp, #78]	@ zero_extendqisi2
	ldrb	r1, [sp, #65]	@ zero_extendqisi2
	str	r0, [sp, #40]
	ldrb	r0, [sp, #77]	@ zero_extendqisi2
	str	r0, [sp, #36]
	ldrb	r0, [sp, #76]	@ zero_extendqisi2
	str	r0, [sp, #32]
	ldrb	r0, [sp, #75]	@ zero_extendqisi2
	str	r0, [sp, #28]
	ldrb	r0, [sp, #74]	@ zero_extendqisi2
	str	r0, [sp, #24]
	ldrb	r0, [sp, #73]	@ zero_extendqisi2
	str	r0, [sp, #20]
	ldrb	r0, [sp, #72]	@ zero_extendqisi2
	str	r0, [sp, #16]
	ldrb	r0, [sp, #71]	@ zero_extendqisi2
	str	r0, [sp, #12]
	ldrb	r0, [sp, #70]	@ zero_extendqisi2
	str	r0, [sp, #8]
	ldrb	r0, [sp, #69]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [sp, #68]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L2715+12
	bl	NAND_Print_DBG
	ldrh	r2, [r5]
	ldrb	r1, [r4, #16]	@ zero_extendqisi2
	ldr	r0, .L2715+16
	bl	NAND_Print_DBG
	ldr	r2, [r4, #268]
	mov	r1, r5
	mov	r0, r4
	bl	recover_block_phy_page_mapping
.L2679:
	ldr	r3, [r4, #4]
	mov	r2, r5
	add	r1, r4, #260
	mov	r0, r4
	ldrh	r3, [r3, #20]
	bl	check_invalid_page_right
	cmp	r0, #0
	beq	.L2681
	mov	r8, #1
	movw	r6, #65535
	ldr	r0, .L2715+20
	bl	NAND_Print
.L2683:
	mov	r10, #0
.L2682:
	ldr	r3, [r4, #4]
	ldrh	r3, [r3, #20]
	subs	r3, r3, #1
	cmp	r3, r10
	bhi	.L2690
.L2689:
	ldr	r3, [r4, #4]
	ldrh	r3, [r3, #20]
	subs	r3, r3, #1
	cmp	r3, r10
	bne	.L2691
	movs	r6, #0
	strh	r6, [r4, #386]	@ movhi
	cmp	r8, #0
	beq	.L2691
	ldr	r0, .L2715+24
	bl	NAND_Print
	ldrh	r2, [r5, #6]
	mov	r1, r5
	mov	r0, r4
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
.L2692:
	movs	r0, #0
	b	.L2673
.L2678:
	cmp	r10, #0
	beq	.L2680
	cmp	r10, #10
	beq	.L2680
	ldrh	r1, [r5]
	ldr	r0, .L2715+28
	bl	NAND_Print_DBG
	ldr	r2, [r4, #268]
	mov	r1, r5
	mov	r0, r4
	bl	recover_block_phy_page_mapping
	b	.L2679
.L2680:
	ldr	r2, [r4, #272]
	mov	r8, #0
	ldr	r1, [r4, #284]
	ldr	r0, [r4, #268]
	bl	NAND_Memcpy
	b	.L2679
.L2681:
	movw	r3, #65535
	cmp	r8, #0
	it	ne
	movne	r6, r3
	b	.L2683
.L2690:
	ldr	r3, [r4, #268]
	ldr	r3, [r3, r10, lsl #2]
	adds	r2, r3, #1
	beq	.L2684
	mov	r1, r3
	mov	r0, r4
	str	r3, [sp, #60]
	bl	get_logic_page_map
	mov	fp, r0
	cmp	r0, #0
	beq	.L2684
	ldrh	r2, [r0, #2]
	ldrh	r1, [r5]
	cmp	r1, r2
	bne	.L2684
	ldrh	r2, [r0]
	ubfx	r2, r2, #0, #12
	cmp	r2, r10
	bne	.L2684
	ldr	r0, [r4, #1968]
	ldr	ip, [r4, #4]
	strh	r1, [sp, #82]	@ movhi
	add	r1, sp, #80
	ldr	r2, [r0, #124]
	strh	r10, [sp, #80]	@ movhi
	adds	r2, r2, #1
	str	r2, [r0, #124]
	ldrh	r0, [ip, #28]
	ldr	r2, [r4, #288]
	strh	r0, [sp, #84]	@ movhi
	mov	r0, r4
	strd	r2, r7, [sp, #88]
	ldr	r2, [r4, #1984]
	blx	r2
	str	r0, [sp, #56]
	mov	r0, r7
	bl	get_logic_page_from_oob
	ldrd	r2, r3, [sp, #56]
	cbz	r2, .L2685
	cmp	r2, #10
	bne	.L2686
.L2685:
	cmp	r3, r0
	beq	.L2687
	ldrh	r1, [r5]
	mov	r2, r10
	str	r3, [sp]
	mov	r3, r0
	ldr	r0, .L2715+32
	mov	r8, #1
	bl	NAND_Print_DBG
	ldrb	r0, [sp, #79]	@ zero_extendqisi2
	movw	r6, #65535
	ldrb	r3, [sp, #66]	@ zero_extendqisi2
	ldrb	r2, [sp, #65]	@ zero_extendqisi2
	str	r0, [sp, #48]
	ldrb	r0, [sp, #78]	@ zero_extendqisi2
	ldrb	r1, [sp, #64]	@ zero_extendqisi2
	str	r0, [sp, #44]
	ldrb	r0, [sp, #77]	@ zero_extendqisi2
	str	r0, [sp, #40]
	ldrb	r0, [sp, #76]	@ zero_extendqisi2
	str	r0, [sp, #36]
	ldrb	r0, [sp, #75]	@ zero_extendqisi2
	str	r0, [sp, #32]
	ldrb	r0, [sp, #74]	@ zero_extendqisi2
	str	r0, [sp, #28]
	ldrb	r0, [sp, #73]	@ zero_extendqisi2
	str	r0, [sp, #24]
	ldrb	r0, [sp, #72]	@ zero_extendqisi2
	str	r0, [sp, #20]
	ldrb	r0, [sp, #71]	@ zero_extendqisi2
	str	r0, [sp, #16]
	ldrb	r0, [sp, #70]	@ zero_extendqisi2
	str	r0, [sp, #12]
	ldrb	r0, [sp, #69]	@ zero_extendqisi2
	str	r0, [sp, #8]
	ldrb	r0, [sp, #68]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [sp, #67]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L2715+36
	bl	NAND_Print
	ldrh	r3, [fp]
	movw	r2, #4095
	strh	r6, [fp, #2]	@ movhi
	bfi	r3, r2, #0, #12
	strh	r3, [fp]	@ movhi
	ldrh	r3, [r5, #4]
	adds	r3, r3, #1
	strh	r3, [r5, #4]	@ movhi
.L2688:
	subs	r6, r6, #1
	uxth	r6, r6
	cmp	r6, #0
	beq	.L2689
.L2684:
	add	r10, r10, #1
	b	.L2682
.L2687:
	ldr	r2, [r4, #288]
	mov	r1, r3
	mov	r0, r4
	bl	nand_write_logic_page_no_gc
	cmp	r0, #0
	beq	.L2688
	ldr	r0, .L2715+40
	bl	NAND_Print
	movs	r0, #1
	b	.L2673
.L2686:
	ldrh	r3, [r5, #4]
	mov	r8, #1
	movw	r6, #65535
	adds	r3, r3, #1
	strh	r3, [r5, #4]	@ movhi
	b	.L2688
.L2691:
	mov	r1, r5
	mov	r0, r4
	bl	put_phy_block_to_invalid_page_list
	mov	r0, r4
	bl	adjust_invaild_list
	b	.L2692
.L2716:
	.align	2
.L2715:
	.word	.LC436
	.word	.LC437
	.word	.LC438
	.word	.LC439
	.word	.LC440
	.word	.LC442
	.word	.LC446
	.word	.LC441
	.word	.LC443
	.word	.LC444
	.word	.LC445
	.size	garbage_collect_first, .-garbage_collect_first
	.section	.text.do_prio_gc,"ax",%progbits
	.align	1
	.global	do_prio_gc
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	do_prio_gc, %function
do_prio_gc:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r1, [r0, #564]
	push	{r3, r4, r5, r6, r7, lr}
	mov	r6, r0
	cmp	r1, #0
	beq	.L2730
	add	r4, r0, #392
	mov	r0, r4
	bl	del_from_gc_list
	ldr	r7, [r0, #4]
	mov	r5, r0
	mov	r0, r6
	mov	r1, r7
	bl	is_current_blcok
	cmp	r0, #1
	bne	.L2719
	mov	r1, r5
	mov	r0, r4
	bl	add_to_gc_list_tail
.L2731:
	movs	r4, #0
.L2717:
	mov	r0, r4
	pop	{r3, r4, r5, r6, r7, pc}
.L2719:
	cbnz	r7, .L2720
	ldr	r0, .L2734
	bl	NAND_Print_DBG
	strh	r7, [r5, #2]	@ movhi
	strh	r7, [r6, #394]	@ movhi
	str	r7, [r5, #4]
	b	.L2731
.L2720:
	mov	r0, r6
	bl	is_block_in_invalid_list
	ldrh	r2, [r5, #2]
	mov	r4, r0
	cbnz	r0, .L2721
	ldrh	r1, [r7]
	ldr	r0, .L2734+4
.L2732:
	bl	NAND_Print_DBG
	b	.L2733
.L2721:
	cmp	r2, #5
	strh	r2, [r6, #394]	@ movhi
	bne	.L2722
	movs	r4, #0
	movs	r2, #4
	mov	r0, r6
	bl	garbage_collect_first
.L2733:
	strh	r4, [r5, #2]	@ movhi
	strh	r4, [r6, #394]	@ movhi
	str	r4, [r5, #4]
	b	.L2717
.L2722:
	movw	r2, #65535
	mov	r0, r6
	bl	garbage_collect_first
	ldr	r3, [r5, #4]
	mov	r4, r0
	cbz	r0, .L2723
	ldrh	r1, [r3]
	movs	r4, #1
	ldrh	r2, [r5, #2]
	ldr	r0, .L2734+8
	bl	NAND_Print
	movs	r3, #0
	strh	r3, [r5, #2]	@ movhi
	strh	r3, [r6, #394]	@ movhi
	str	r3, [r5, #4]
	b	.L2717
.L2723:
	mov	r1, r3
	mov	r0, r6
	bl	is_block_in_free_list
	cbnz	r0, .L2724
	ldrh	r2, [r5, #2]
	ldrh	r1, [r3]
	ldr	r0, .L2734+12
	b	.L2732
.L2724:
	mov	r0, r6
	bl	out_phy_block_from_free_list_by_block
	mov	r1, r0
	cbnz	r0, .L2725
	ldr	r3, [r5, #4]
	ldrh	r2, [r5, #2]
	ldr	r0, .L2734+16
	ldrh	r1, [r3]
	b	.L2732
.L2725:
	ldrh	r2, [r0, #6]
	mov	r0, r6
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
	movs	r3, #0
	strh	r3, [r5, #2]	@ movhi
	strh	r3, [r6, #394]	@ movhi
	str	r3, [r5, #4]
	b	.L2717
.L2730:
	mov	r4, r1
	b	.L2717
.L2735:
	.align	2
.L2734:
	.word	.LC447
	.word	.LC448
	.word	.LC449
	.word	.LC450
	.word	.LC451
	.size	do_prio_gc, .-do_prio_gc
	.section	.text.prio_gc_all,"ax",%progbits
	.align	1
	.global	prio_gc_all
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	prio_gc_all, %function
prio_gc_all:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r6, r0
	movs	r5, #10
	movs	r4, #0
.L2737:
	mov	r0, r6
	bl	do_prio_gc
	subs	r5, r5, #1
	orr	r4, r4, r0
	bne	.L2737
	mov	r0, r4
	pop	{r4, r5, r6, pc}
	.size	prio_gc_all, .-prio_gc_all
	.section	.text._nftl_nand_read_logic_page,"ax",%progbits
	.align	1
	.global	_nftl_nand_read_logic_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nftl_nand_read_logic_page, %function
_nftl_nand_read_logic_page:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, lr}
	sub	sp, sp, #36
	mov	r4, r0
	mov	r7, r1
	mov	r6, r2
	bl	get_logic_page_map
	mov	r5, r0
	cbnz	r0, .L2740
	movs	r6, #1
	ldr	r0, .L2744
	bl	NAND_Print_DBG
.L2739:
	mov	r0, r6
	add	sp, sp, #36
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L2740:
	ldrh	r2, [r0, #2]
	movw	r1, #65535
	ldr	r3, [r4, #4]
	cmp	r2, r1
	bne	.L2742
	mov	r0, r6
	ldrh	r2, [r3, #26]
	movs	r1, #255
	movs	r6, #0
	bl	NAND_Memset
	b	.L2739
.L2742:
	ldrb	r1, [r0, #1]	@ zero_extendqisi2
	ldr	r0, [r4, #364]
	bfi	r1, r0, #4, #1
	mov	r0, r4
	strb	r1, [r5, #1]
	ldrh	r1, [r3, #28]
	ldrh	r3, [r5]
	str	r6, [sp, #24]
	strh	r1, [sp, #20]	@ movhi
	add	r1, sp, #16
	ubfx	r3, r3, #0, #12
	strh	r2, [sp, #18]	@ movhi
	strh	r3, [sp, #16]	@ movhi
	str	sp, [sp, #28]
	ldr	r3, [r4, #1984]
	blx	r3
	adds	r3, r0, #2
	mov	r6, r0
	bne	.L2743
	ldr	r3, [r4, #4]
	movs	r0, #36
	ldrh	r1, [r5, #2]
	movs	r2, #1
	ldr	r3, [r3, #32]
	mla	r1, r0, r1, r3
	mov	r0, r4
	bl	add_prio_gc
	mov	r1, r7
	ldr	r0, .L2744+4
	bl	NAND_Print
.L2743:
	mov	r0, r4
	bl	do_prio_gc
	b	.L2739
.L2745:
	.align	2
.L2744:
	.word	.LC426
	.word	.LC452
	.size	_nftl_nand_read_logic_page, .-_nftl_nand_read_logic_page
	.section	.text.gc_one,"ax",%progbits
	.align	1
	.global	gc_one
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	gc_one, %function
gc_one:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #200]
	push	{r4, lr}
	cbnz	r3, .L2747
.L2749:
	movs	r4, #8
.L2746:
	mov	r0, r4
	pop	{r4, pc}
.L2747:
	ldrh	r3, [r3, #4]
	cmp	r3, r1
	bls	.L2749
	movw	r2, #65535
	movs	r1, #0
	bl	garbage_collect_first
	cmp	r0, #8
	mov	r4, r0
	beq	.L2749
	cbnz	r0, .L2750
	ldr	r0, .L2754
	bl	NAND_Print_DBG
	b	.L2746
.L2750:
	ldr	r0, .L2754+4
	movs	r4, #1
	bl	NAND_Print
	b	.L2746
.L2755:
	.align	2
.L2754:
	.word	.LC453
	.word	.LC454
	.size	gc_one, .-gc_one
	.section	.text.gc_all_cnt,"ax",%progbits
	.align	1
	.global	gc_all_cnt
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	gc_all_cnt, %function
gc_all_cnt:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r5, r0
	mov	r6, r1
	mov	r4, r2
.L2757:
	cbz	r4, .L2761
	mov	r1, r6
	mov	r0, r5
	bl	gc_one
	cmp	r0, #8
	add	r4, r4, #-1
	bne	.L2757
.L2761:
	movs	r0, #0
	pop	{r4, r5, r6, pc}
	.size	gc_all_cnt, .-gc_all_cnt
	.section	.text.gc_all_base_on_free_blks,"ax",%progbits
	.align	1
	.global	gc_all_base_on_free_blks
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	gc_all_base_on_free_blks, %function
gc_all_base_on_free_blks:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldrh	r3, [r0, #22]
	ldrh	r1, [r0, #380]
	push	{r4, r5, r6, lr}
	mov	r5, r0
	cmp	r3, r1
	bcc	.L2762
	ldrh	r2, [r0, #382]
	add	r2, r2, r2, lsl #1
	cmp	r2, r3
	bcc	.L2762
	ldr	r6, .L2767
	ldrb	r0, [r6, #10]	@ zero_extendqisi2
	ldrh	r4, [r6, #8]
	muls	r4, r0, r4
	ldr	r0, [r6, #4]
	muls	r0, r4, r0
	cmp	r0, #264192
	bcs	.L2762
	ldr	r0, [r5, #4]
	subs	r3, r3, r1
	subs	r1, r2, r1
	ldrh	r4, [r0, #20]
	lsrs	r4, r4, #1
	subs	r0, r4, #3
	muls	r0, r3, r0
	bl	__aeabi_uidiv
	adds	r1, r4, r0
	movs	r2, #3
	mov	r0, r5
	uxth	r1, r1
	pop	{r4, r5, r6, lr}
	b	gc_all_cnt
.L2762:
	movs	r0, #0
	pop	{r4, r5, r6, pc}
.L2768:
	.align	2
.L2767:
	.word	.LANCHOR108
	.size	gc_all_base_on_free_blks, .-gc_all_base_on_free_blks
	.section	.text.gc_all,"ax",%progbits
	.align	1
	.global	gc_all
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	gc_all, %function
gc_all:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r4, r0
	mov	r5, r1
.L2770:
	mov	r1, r5
	mov	r0, r4
	bl	gc_one
	cmp	r0, #0
	beq	.L2770
	movs	r0, #0
	pop	{r3, r4, r5, pc}
	.size	gc_all, .-gc_all
	.section	.text.gc_all_enhance,"ax",%progbits
	.align	1
	.global	gc_all_enhance
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	gc_all_enhance, %function
gc_all_enhance:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldrh	r1, [r0, #22]
	ldrh	r2, [r0, #380]
	push	{r4}
	cmp	r2, r1
	bhi	.L2773
	ldrh	r2, [r0, #382]
	add	r2, r2, r2, lsl #1
	cmp	r2, r1
	bcc	.L2773
	ldr	r4, .L2778
	ldrb	r2, [r4, #10]	@ zero_extendqisi2
	ldrh	r1, [r4, #8]
	muls	r1, r2, r1
	ldr	r2, [r4, #4]
	muls	r2, r1, r2
	cmp	r2, #264192
	bcs	.L2773
	ldr	r3, [r0, #4]
	pop	{r4}
	ldrh	r1, [r3, #20]
	lsrs	r1, r1, #1
	b	gc_all
.L2773:
	movs	r0, #0
	pop	{r4}
	bx	lr
.L2779:
	.align	2
.L2778:
	.word	.LANCHOR108
	.size	gc_all_enhance, .-gc_all_enhance
	.section	.text.gc_win_block,"ax",%progbits
	.align	1
	.global	gc_win_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	gc_win_block, %function
gc_win_block:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	mov	r7, r0
	ldr	r3, [r0, #4]
	movs	r5, #0
	ldrh	r6, [r3, #20]
	muls	r6, r1, r6
.L2781:
	cmp	r5, r6
	blt	.L2783
	movs	r4, #0
	b	.L2780
.L2783:
	ldr	r4, [r7, #200]
	cbz	r4, .L2780
	ldrh	r3, [r4, #4]
	movs	r1, #1
	mov	r0, r7
	add	r5, r5, r3
	subs	r5, r5, #1
	bl	gc_one
	mov	r4, r0
	cmp	r0, #0
	beq	.L2781
	cmp	r0, #1
	bne	.L2780
	ldr	r0, .L2787
	bl	NAND_Print_DBG
.L2780:
	mov	r0, r4
	pop	{r3, r4, r5, r6, r7, pc}
.L2788:
	.align	2
.L2787:
	.word	.LC455
	.size	gc_win_block, .-gc_win_block
	.section	.text.power_on_gc,"ax",%progbits
	.align	1
	.global	power_on_gc
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	power_on_gc, %function
power_on_gc:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, lr}
	mov	r4, r0
	ldrh	r6, [r0, #22]
	sub	sp, sp, #20
	ldrh	r7, [r0, #380]
	ldr	r3, [r0, #4]
	cmp	r6, r7
	ldrh	r5, [r3, #20]
	bcc	.L2790
	ldrb	r8, [r3, #8]	@ zero_extendqisi2
	ldrh	r10, [r0, #382]
	ldr	r0, [r0, #32]
	mov	r1, r8
	bl	__aeabi_uidiv
	mov	r1, r5
	bl	__aeabi_uidiv
	mov	r1, r8
	str	r0, [sp, #8]
	ldr	r0, [r4, #24]
	bl	__aeabi_uidiv
	mov	r1, r5
	bl	__aeabi_uidiv
	ldrh	r3, [r4, #18]
	mov	r2, r7
	str	r0, [sp, #4]
	mov	r1, r6
	ldr	r0, .L2800
	str	r3, [sp]
	mov	r3, r10
	bl	NAND_Print_DBG
.L2791:
	movs	r0, #0
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L2790:
	subs	r5, r5, #10
	movs	r7, #0
	uxth	r5, r5
.L2792:
	ldrh	r2, [r4, #22]
	uxth	r6, r7
	ldrh	r3, [r4, #380]
	cmp	r2, r3
	bcs	.L2795
	mov	r1, r5
	mov	r0, r4
	bl	gc_one
	cmp	r0, #8
	add	r7, r7, #1
	bne	.L2792
.L2795:
	ldr	r3, [r4, #4]
	ldr	r7, .L2800+4
	ldrh	r5, [r3, #20]
	lsrs	r5, r5, #3
.L2793:
	ldrh	r1, [r4, #22]
	cmp	r1, #4
	bhi	.L2798
	mov	r0, r7
	bl	NAND_Print_DBG
	mov	r1, r5
	mov	r0, r4
	bl	gc_one
	cmp	r0, #8
	bne	.L2796
.L2798:
	mov	r1, r6
	ldr	r0, .L2800+8
	bl	NAND_Print_DBG
	b	.L2791
.L2796:
	adds	r6, r6, #1
	uxth	r6, r6
	b	.L2793
.L2801:
	.align	2
.L2800:
	.word	.LC456
	.word	.LC457
	.word	.LC458
	.size	power_on_gc, .-power_on_gc
	.section	.text.build_zone_list,"ax",%progbits
	.align	1
	.global	build_zone_list
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	build_zone_list, %function
build_zone_list:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r4, r0
	ldr	r0, .L2807
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	first_scan_all_blocks
	ldrh	r1, [r4, #18]
	mov	r0, r4
	bl	init_zone_after_first_scan
	cbz	r0, .L2803
	ldr	r0, .L2807+4
.L2806:
	movs	r5, #1
	bl	NAND_Print
.L2802:
	mov	r0, r5
	pop	{r3, r4, r5, pc}
.L2803:
	mov	r0, r4
	bl	second_scan_all_blocks
	mov	r5, r0
	cbz	r0, .L2805
	ldr	r0, .L2807+8
	b	.L2806
.L2805:
	mov	r0, r4
	bl	power_on_gc
	ldr	r0, .L2807+12
	bl	NAND_Print
	mov	r0, r4
	bl	panic_backup_block
	b	.L2802
.L2808:
	.align	2
.L2807:
	.word	.LC459
	.word	.LC460
	.word	.LC461
	.word	.LC462
	.size	build_zone_list, .-build_zone_list
	.section	.text.nand_find_zone_table,"ax",%progbits
	.align	1
	.global	nand_find_zone_table
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_find_zone_table, %function
nand_find_zone_table:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r4, r0
	bl	clean_block_list
	mov	r0, r4
	bl	nand_clean_zone_table2
	ldr	r0, .L2810
	bl	NAND_Print_DBG
	ldr	r5, .L2810+4
	mov	r0, r4
	bl	build_zone_list
	ldr	r0, .L2810+8
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	nftl_cache_init
	mov	r0, r5
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	print_block_invalid_list
	mov	r0, r4
	bl	print_free_list
	mov	r0, r5
	bl	NAND_Print_DBG
	ldr	r0, .L2810+12
	bl	NAND_Print_DBG
	movs	r0, #0
	pop	{r3, r4, r5, pc}
.L2811:
	.align	2
.L2810:
	.word	.LC463
	.word	.LC465
	.word	.LC464
	.word	.LC466
	.size	nand_find_zone_table, .-nand_find_zone_table
	.section	.text.nand_check_table,"ax",%progbits
	.align	1
	.global	nand_check_table
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_check_table, %function
nand_check_table:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r1, #1000
	ldr	r4, .L2813
	mov	r5, r0
	bl	__nand_flush_write_cache
	mov	r0, r4
	bl	NAND_Print_DBG
	mov	r0, r5
	bl	print_nftl_zone
	mov	r0, r4
	bl	NAND_Print_DBG
	mov	r0, r5
	bl	nand_clean_zone_table
	mov	r0, r4
	bl	NAND_Print_DBG
	mov	r0, r5
	bl	nand_find_zone_table
	mov	r0, r5
	bl	write_data_to_nand
	mov	r0, r4
	bl	NAND_Print_DBG
	movs	r0, #0
	pop	{r3, r4, r5, pc}
.L2814:
	.align	2
.L2813:
	.word	.LC467
	.size	nand_check_table, .-nand_check_table
	.global	__aeabi_idiv
	.section	.text.nftl_start,"ax",%progbits
	.align	1
	.global	nftl_start
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_start, %function
nftl_start:
	@ args = 40, pretend = 0, frame = 40
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	sub	sp, sp, #40
	ldr	r3, [sp, #88]
	mov	r7, r2
	ldr	r2, [sp, #76]
	strd	r0, r1, [sp, #28]
	movw	r0, #2020
	lsr	r8, r3, #9
	ldr	r3, [sp, #92]
	lsrs	r3, r3, #9
	str	r3, [sp]
	ldr	r3, [sp, #72]
	lsrs	r3, r3, #9
	orr	r3, r3, r2, lsl #23
	ldr	r2, [sp, #84]
	str	r3, [sp, #20]
	ldr	r3, [sp, #80]
	lsrs	r3, r3, #9
	orr	r3, r3, r2, lsl #23
	str	r3, [sp, #24]
	bl	NAND_Malloc
	mov	r4, r0
	str	r0, [r7]
	cbz	r0, .L2816
	ldr	r3, [sp]
	ldr	r10, .L2840+44
	lsls	r3, r3, #9
	uxth	r3, r3
	str	r3, [sp, #12]
	movs	r3, #2
	str	r3, [sp, #8]
.L2817:
	movw	r2, #2020
	movs	r1, #0
	mov	r0, r4
	bl	NAND_Memset
	ldr	r3, [sp, #28]
	movs	r0, #44
	str	r3, [r4]
	ldr	r3, [sp, #32]
	str	r3, [r4, #1972]
	bl	NAND_Malloc
	str	r0, [r4, #4]
	cbnz	r0, .L2819
	ldr	r1, .L2840
	ldr	r0, .L2840+4
	b	.L2839
.L2816:
	ldr	r1, .L2840
	ldr	r0, .L2840+8
.L2839:
	mvn	r5, #1
	bl	NAND_Print
.L2815:
	mov	r0, r5
	add	sp, sp, #40
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2819:
	movs	r2, #4
	movs	r1, #0
	bl	NAND_Memset
	ldr	r3, [r10]
	movs	r2, #8
	ldr	r0, [r4, #4]
	ldr	r1, [r3, #8]
	add	r1, r1, r2
	bl	NAND_Memcpy
	ldr	r5, [r4, #4]
	mov	r1, r8
	ldrh	r3, [sp, #12]
	ldr	r0, [sp, #24]
	strh	r3, [r5, #26]	@ movhi
	ldr	r3, [sp, #96]
	strb	r3, [r5, #10]
	bl	__aeabi_uidiv
	ldr	r2, [r4, #1972]
	mov	r6, r0
	ldrh	r2, [r2, #12]
	cmp	r2, r0
	bcc	.L2820
	mov	r1, r0
	ldr	r0, .L2840+12
	bl	NAND_Print
	mov	r5, #-1
	b	.L2815
.L2820:
	uxth	r3, r0
	ldr	r1, [sp]
	mov	r0, r8
	str	r3, [sp, #4]
	mov	fp, #36
	ldrh	r3, [sp, #4]
	strh	r3, [r5, #24]	@ movhi
	bl	__aeabi_uidiv
	ldrb	r2, [sp]	@ zero_extendqisi2
	adds	r3, r6, #1
	strh	r0, [r5, #20]	@ movhi
	str	r3, [sp, #16]
	strb	r2, [r5, #8]
	strh	r2, [r5, #28]	@ movhi
	movw	r2, #3000
	strh	r2, [r5, #16]	@ movhi
	ldr	r2, [r10]
	ldr	r2, [r2, #8]
	ldr	r1, [r2, #248]
	strb	r1, [r5, #36]
	ldr	r2, [r2, #252]
	str	r2, [r5, #40]
	mul	r2, fp, r3
	mov	r0, r2
	str	r2, [sp, #36]
	bl	nftl_malloc
	ldr	r3, [r4, #4]
	str	r0, [r5, #32]
	ldr	r2, [sp, #36]
	ldr	r0, [r3, #32]
	cbnz	r0, .L2821
	ldr	r1, .L2840
	ldr	r0, .L2840+16
	b	.L2839
.L2821:
	subs	r2, r2, #36
	movs	r1, #255
	bl	NAND_Memset
	movs	r1, #0
	mov	ip, #-1
	mov	r2, r1
.L2822:
	ldr	r3, [sp, #16]
	cmp	r3, r1
	bhi	.L2823
	ldr	r0, [r7]
	bl	nftl_ops_init
	mov	r1, r8
	ldr	r0, [sp, #20]
	bl	__aeabi_uidiv
	ldr	r3, [sp, #4]
	subs	r0, r3, r0
	ldr	r3, [r4, #1972]
	uxth	r2, r0
	ldrh	r3, [r3, #12]
	strh	r2, [r4, #22]	@ movhi
	cmp	r3, r2
	bls	.L2824
	mov	r1, r2
	ldr	r0, .L2840+20
	b	.L2839
.L2823:
	ldr	r3, [r4, #4]
	mul	r0, fp, r1
	ldr	lr, [r3, #32]
	add	r3, lr, r0
	strh	r1, [lr, r0]	@ movhi
	strh	r2, [r3, #4]	@ movhi
	adds	r1, r1, #1
	strh	r2, [r3, #2]	@ movhi
	str	ip, [r3, #8]
	strh	r2, [r3, #6]	@ movhi
	strd	r2, r2, [r3, #12]
	strd	r2, r2, [r3, #20]
	strd	r2, r2, [r3, #28]
	b	.L2822
.L2824:
	mov	r1, r6
	ldr	r0, .L2840+24
	bl	NAND_Print_DBG
	ldrh	r3, [r4, #22]
	movs	r1, #0
	ldr	r0, [r7]
	subs	r2, r6, r3
	mul	r2, r8, r2
	mul	r3, r8, r3
	bl	zone_param_init
	mov	r5, r0
	cbz	r0, .L2825
	ldr	r0, .L2840+28
	bl	NAND_Print
	b	.L2815
.L2825:
	ldr	r3, [sp, #104]
	ldr	r2, [r4, #1972]
	strb	r3, [r4, #16]
	ldrh	r3, [r2, #26]
	cbz	r3, .L2826
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	orr	r3, r3, #1
	strb	r3, [r4, #17]
.L2826:
	ldrh	r3, [r2, #10]
	cbz	r3, .L2827
	mov	r1, r6
	ldr	r0, [r7]
	bl	is_no_use_device
	cmp	r0, #1
	bne	.L2827
	mov	r1, r6
	ldr	r0, [r7]
	bl	build_zone_list_first
	mov	r6, r0
	cbz	r0, .L2827
	ldr	r0, .L2840+32
	bl	NAND_Print
.L2829:
	mov	r5, r6
	b	.L2815
.L2827:
	ldr	r0, [r7]
	bl	build_zone_list
	mov	r6, r0
	cbz	r0, .L2828
	ldr	r0, .L2840+36
	bl	NAND_Print
	ldr	r3, [sp, #8]
	cmp	r3, #1
	beq	.L2829
	ldr	r0, .L2840+40
	bl	NAND_Print
	movs	r3, #1
	str	r3, [sp, #8]
	b	.L2817
.L2828:
	ldr	r0, [r7]
	bl	nftl_cache_init
	b	.L2815
.L2841:
	.align	2
.L2840:
	.word	.LANCHOR109
	.word	.LC469
	.word	.LC468
	.word	.LC470
	.word	.LC471
	.word	.LC472
	.word	.LC473
	.word	.LC474
	.word	.LC475
	.word	.LC476
	.word	.LC477
	.word	g_nsi
	.size	nftl_start, .-nftl_start
	.section	.text.nftl_initialize,"ax",%progbits
	.align	1
	.global	nftl_initialize
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_initialize, %function
nftl_initialize:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, lr}
	sub	sp, sp, #60
	ldr	r5, [r0, #4]
	mov	r4, r0
	mov	r6, r1
	movs	r2, #9
	ldr	r1, .L2864
	add	r0, sp, #44
	bl	memcpy
	add	r0, sp, #56
	movs	r3, #47
	strh	r3, [r0, #-16]!	@ movhi
	ldr	r1, .L2864+4
	bl	nand_fdt_get_string
	cbz	r0, .L2856
	movs	r2, #9
	add	r1, sp, #44
	bl	NAND_Memcmp
	mov	r7, r0
	cbnz	r0, .L2856
	ldr	r3, .L2864+8
	ldr	r2, .L2864+12
	ldr	r1, .L2864+16
	str	r3, [sp, #4]
	ldr	r3, .L2864+20
	ldr	r0, .L2864+24
	str	r3, [sp]
	ldr	r3, .L2864+28
	bl	NAND_Print
	ldr	r0, .L2864+32
	bl	nftl_get_nand_phyinfo
	ldrh	r3, [r5, #34]
	cmp	r3, #15
	bhi	.L2860
.L2856:
	mov	r6, #-1
	b	.L2842
.L2860:
	movs	r0, #36
	bl	NAND_Malloc
	str	r0, [r4, #16]
	cbnz	r0, .L2846
	movs	r6, #1
	ldr	r1, .L2864+36
	ldr	r0, .L2864+40
	bl	NAND_Print
.L2842:
	mov	r0, r6
	add	sp, sp, #60
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L2846:
	str	r7, [r0, #8]
	mov	r2, #65536
	ldr	r7, [r5, #80]
	mov	r3, #65537
	strd	r2, r3, [r0]
	movs	r1, #3
	ldr	r2, [r7, #36]
	ldrh	r3, [r2, #2478]
	strh	r3, [r0, #12]	@ movhi
	ldrh	r3, [r2, #2480]
	strh	r1, [r0, #20]	@ movhi
	strh	r3, [r0, #16]	@ movhi
	movs	r3, #8
	strh	r3, [r0, #14]	@ movhi
	movs	r3, #2
	strh	r3, [r0, #18]	@ movhi
	ldr	r3, .L2864+44
	str	r3, [r0, #32]
	ldrh	r3, [r2, #2474]
	cmp	r3, #0
	bne	.L2847
.L2861:
	movs	r3, #30
	strh	r1, [r0, #24]	@ movhi
	strh	r3, [r0, #22]	@ movhi
.L2848:
	ldrh	r3, [r7, #2]
	strh	r3, [r0, #26]	@ movhi
	cmp	r3, #0
	beq	.L2851
	movs	r3, #3
.L2863:
	ldr	r1, [r2, #2492]
	strh	r3, [r0, #24]	@ movhi
	ldrh	r3, [r0, #22]
	ldr	r7, .L2864+48
	sub	r2, r1, #86016
	sub	r2, r2, #384
	cmp	r3, #30
	itt	hi
	movhi	r3, #30
	strhhi	r3, [r0, #22]	@ movhi
	movs	r3, #1
	cmp	r2, r7
	strh	r3, [r0, #24]	@ movhi
	it	ls
	strls	r1, [r0, #32]
	strh	r3, [r0, #28]	@ movhi
	bl	nftl_support_boot_check_crc_from_dts
	cbnz	r0, .L2855
	ldr	r3, [r4, #16]
	strh	r0, [r3, #28]	@ movhi
.L2855:
	movs	r3, #0
	str	r5, [sp, #28]
	strd	r6, r3, [sp, #32]
	mov	r0, r4
	ldrh	r3, [r5, #34]
	ldr	r1, [r4, #16]
	str	r3, [sp, #24]
	ldrh	r3, [r5, #40]
	str	r3, [sp, #20]
	ldr	r3, [r5, #44]
	str	r3, [sp, #16]
	ldrd	r2, [r5, #72]
	strd	r2, [sp, #8]
	ldrd	r2, [r5, #64]
	strd	r2, [sp]
	add	r2, r4, #12
	bl	nftl_start
	mov	r6, r0
	cmp	r0, #0
	bne	.L2842
	ldr	r3, [r5, #80]
	ldr	r0, .L2864+52
	ldr	r2, [r3, #36]
	ldr	r3, [r4, #12]
	ldr	r1, [r2, #2484]
	ldr	r3, [r3, #4]
	ldr	r2, [r2, #2488]
	strh	r1, [r3, #16]	@ movhi
	uxth	r1, r1
	strh	r2, [r3, #30]	@ movhi
	bl	NAND_Print_DBG
	ldr	r3, [r4, #12]
	ldr	r2, [r3, #24]
	str	r6, [r4, #8]
	str	r2, [r4]
	ldr	r2, .L2864+56
	str	r2, [r4, #44]
	ldr	r2, .L2864+60
	str	r2, [r4, #40]
	ldr	r2, .L2864+64
	str	r2, [r4, #48]
	ldr	r2, .L2864+68
	str	r2, [r4, #56]
	ldr	r2, .L2864+72
	str	r2, [r4, #52]
	ldr	r2, .L2864+76
	str	r2, [r4, #60]
	ldr	r2, .L2864+80
	str	r2, [r4, #64]
	ldr	r2, .L2864+84
	str	r2, [r4, #68]
	ldr	r2, .L2864+88
	str	r2, [r4, #72]
	movs	r2, #1
	strb	r2, [r3, #1976]
	b	.L2842
.L2847:
	cmp	r3, #255
	bne	.L2849
	movs	r3, #1
	strh	r3, [r0]	@ movhi
	movs	r3, #50
	strh	r3, [r0, #22]	@ movhi
	movs	r3, #40
.L2862:
	strh	r3, [r0, #24]	@ movhi
	b	.L2848
.L2849:
	cmp	r3, #10
	bls	.L2850
	movs	r3, #10
	strh	r3, [r2, #2474]	@ movhi
	b	.L2861
.L2850:
	add	r3, r3, r3, lsl #2
	add	r1, r3, r3, lsl #2
	lsls	r3, r3, #3
	lsls	r1, r1, #1
	strh	r1, [r0, #22]	@ movhi
	b	.L2862
.L2851:
	movs	r3, #1
	b	.L2863
.L2865:
	.align	2
.L2864:
	.word	.LANCHOR76+30
	.word	.LANCHOR110
	.word	.LC482
	.word	.LC479
	.word	.LC480
	.word	.LC483
	.word	.LC481
	.word	.LC478
	.word	.LANCHOR108
	.word	.LANCHOR111
	.word	.LC484
	.word	1296000
	.word	7692600
	.word	.LC485
	.word	_nand_read
	.word	_nand_read_history
	.word	_nand_write
	.word	_nand_discard
	.word	_nand_flush_write_cache
	.word	_shutdown_op
	.word	_nand_sector_read
	.word	_nand_sector_write
	.word	_nand_flush_sector_write_cache
	.size	nftl_initialize, .-nftl_initialize
	.section	.text.nftl_add,"ax",%progbits
	.align	1
	.global	nftl_add
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_add, %function
nftl_add:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r5, r0
	movs	r0, #76
	bl	NAND_Malloc
	mov	r4, r0
	cbnz	r0, .L2867
	ldr	r1, .L2870
	ldr	r0, .L2870+4
	bl	NAND_Print
.L2866:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L2867:
	mov	r0, r5
	bl	build_nand_partition
	ldrh	r1, [r5]
	str	r0, [r4, #4]
	mov	r0, r4
	bl	nftl_initialize
	cbz	r0, .L2869
	ldr	r0, .L2870+8
	movs	r4, #0
	bl	NAND_Print
	b	.L2866
.L2869:
	mov	r1, r4
	ldr	r0, .L2870+12
	bl	add_nftl_blk_list
	mov	r0, r4
	bl	panic_recover_data
	ldr	r0, .L2870+16
	bl	NAND_Print_DBG
	b	.L2866
.L2871:
	.align	2
.L2870:
	.word	.LANCHOR112
	.word	.LC486
	.word	.LC487
	.word	.LANCHOR0
	.word	.LC488
	.size	nftl_add, .-nftl_add
	.section	.text.nftl_build_all,"ax",%progbits
	.align	1
	.global	nftl_build_all
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_build_all, %function
nftl_build_all:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	movs	r2, #0
	ldr	r4, [r0, #56]
	ldr	r3, .L2877
	str	r2, [r3, #8]
.L2873:
	mov	r0, r4
	cbz	r4, .L2872
	bl	nftl_add
	cbnz	r0, .L2874
	ldr	r0, .L2877+4
	bl	NAND_Print
	movs	r0, #1
.L2872:
	pop	{r4, pc}
.L2874:
	ldr	r4, [r4, #48]
	b	.L2873
.L2878:
	.align	2
.L2877:
	.word	.LANCHOR0
	.word	.LC489
	.size	nftl_build_all, .-nftl_build_all
	.section	.text.nftl_build_one,"ax",%progbits
	.align	1
	.global	nftl_build_one
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_build_one, %function
nftl_build_one:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r5, r1
	ldr	r4, [r0, #56]
	cbnz	r1, .L2881
	ldr	r3, .L2885
	str	r1, [r3, #8]
.L2881:
	cbnz	r4, .L2884
	mov	r0, r4
	b	.L2879
.L2884:
	ldrh	r3, [r4]
	cmp	r3, r5
	bne	.L2882
	mov	r0, r4
	bl	nftl_add
	cbnz	r0, .L2882
	ldr	r0, .L2885+4
	bl	NAND_Print
	movs	r0, #1
.L2879:
	pop	{r3, r4, r5, pc}
.L2882:
	ldr	r4, [r4, #48]
	b	.L2881
.L2886:
	.align	2
.L2885:
	.word	.LANCHOR0
	.word	.LC489
	.size	nftl_build_one, .-nftl_build_one
	.section	.text.all_time_gc,"ax",%progbits
	.align	1
	.global	all_time_gc
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	all_time_gc, %function
all_time_gc:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldrh	r1, [r0, #22]
	ldrh	r2, [r0, #382]
	push	{r3, lr}
	cmp	r1, r2
	bcs	.L2888
	ldrh	r2, [r0, #384]
	cbz	r2, .L2888
	ldr	r3, [r0, #4]
	ldrh	r1, [r3, #20]
	subs	r1, r1, #3
	uxth	r1, r1
	bl	gc_one
.L2888:
	movs	r0, #0
	pop	{r3, pc}
	.size	all_time_gc, .-all_time_gc
	.section	.text.shutdown_on_gc,"ax",%progbits
	.align	1
	.global	shutdown_on_gc
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	shutdown_on_gc, %function
shutdown_on_gc:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldrh	r2, [r0, #382]
	push	{r4, r5, lr}
	sub	sp, sp, #20
	ldrh	r3, [r0, #384]
	mov	r4, r0
	str	r2, [sp, #8]
	ldrh	r2, [r0, #380]
	ldr	r5, .L2900
	ldr	r1, .L2900+4
	str	r2, [sp, #4]
	ldrh	r2, [r0, #22]
	str	r2, [sp]
	ldr	r2, [r0, #200]
	ldr	r0, .L2900+8
	bl	NAND_Print_DBG
.L2893:
	ldrh	r1, [r4, #22]
	ldrh	r3, [r4, #382]
.L2896:
	cmp	r1, r3
	bhi	.L2895
	ldr	r2, [r4, #200]
	cbnz	r2, .L2894
	ldrh	r2, [r4, #380]
	ldr	r0, .L2900+12
	bl	NAND_Print
.L2895:
	movs	r0, #0
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, pc}
.L2894:
	ldrh	r2, [r4, #384]
	cmp	r2, #0
	beq	.L2896
	movs	r1, #0
	mov	r0, r4
	bl	gc_one
	ldrh	r3, [r4, #382]
	mov	r0, r5
	ldrh	r2, [r4, #380]
	ldrh	r1, [r4, #22]
	bl	NAND_Print_DBG
	b	.L2893
.L2901:
	.align	2
.L2900:
	.word	.LC492
	.word	.LANCHOR113
	.word	.LC490
	.word	.LC491
	.size	shutdown_on_gc, .-shutdown_on_gc
	.section	.text.garbage_collect,"ax",%progbits
	.align	1
	.global	garbage_collect
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	garbage_collect, %function
garbage_collect:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldrh	r3, [r0, #380]
	ldrh	r1, [r0, #22]
	push	{r4, lr}
	mov	r4, r0
	cmp	r3, r1
	itt	cs
	movcs	r3, #0
	strhcs	r3, [r0, #384]	@ movhi
	ldrh	r0, [r0, #384]
	cbz	r0, .L2904
.L2921:
	movs	r0, #0
	b	.L2902
.L2904:
	ldrh	r3, [r4, #382]
	cmp	r3, r1
	bls	.L2906
	ldrh	r3, [r4, #386]
	cbnz	r3, .L2908
	ldr	r3, [r4, #200]
	cbnz	r3, .L2909
	ldr	r0, .L2923
	bl	NAND_Print
.L2922:
	movs	r0, #1
.L2902:
	pop	{r4, pc}
.L2909:
	ldrh	r2, [r3, #4]
	ldr	r3, [r4, #4]
	ldrh	r3, [r3, #20]
	cmp	r2, r3, lsr #1
	bcc	.L2910
	movs	r3, #4
.L2920:
	strh	r3, [r4, #388]	@ movhi
	ldr	r3, [r4, #1972]
	ldrh	r2, [r4, #22]
	ldrh	r3, [r3, #16]
	cmp	r2, r3
	itt	ls
	movwls	r3, #65535
	strhls	r3, [r4, #388]	@ movhi
.L2908:
	ldrh	r2, [r4, #388]
	movw	r3, #65535
	cmp	r2, r3
	beq	.L2915
	movs	r1, #0
	mov	r0, r4
	bl	garbage_collect_first
	cmp	r0, #0
	beq	.L2921
	ldrh	r1, [r4, #22]
	ldr	r0, .L2923+4
	bl	NAND_Print
	b	.L2922
.L2910:
	cmp	r2, r3, lsr #2
	bcc	.L2912
	movs	r3, #8
	b	.L2920
.L2912:
	cmp	r2, r3, lsr #3
	bcc	.L2913
	movs	r3, #16
	b	.L2920
.L2913:
	ldr	r0, .L2923+8
	bl	NAND_Print_DBG
	movw	r3, #65535
	b	.L2920
.L2915:
	movs	r1, #1
	mov	r0, r4
	bl	gc_win_block
	b	.L2921
.L2906:
	movs	r3, #1
	strh	r0, [r4, #388]	@ movhi
	str	r3, [r4, #384]
	b	.L2902
.L2924:
	.align	2
.L2923:
	.word	.LC493
	.word	.LC495
	.word	.LC494
	.size	garbage_collect, .-garbage_collect
	.section	.text._nftl_nand_write_logic_page,"ax",%progbits
	.align	1
	.global	_nftl_nand_write_logic_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	_nftl_nand_write_logic_page, %function
_nftl_nand_write_logic_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r5, r0
	mov	r6, r1
	bl	nand_write_logic_page_no_gc
	mov	r4, r0
	cbz	r0, .L2926
	movs	r4, #1
	mov	r1, r6
	ldr	r0, .L2928
	bl	NAND_Print
.L2925:
	mov	r0, r4
	pop	{r4, r5, r6, pc}
.L2926:
	mov	r0, r5
	bl	garbage_collect
	mov	r0, r5
	bl	all_time_gc
	mov	r0, r5
	bl	do_prio_gc
	b	.L2925
.L2929:
	.align	2
.L2928:
	.word	.LC496
	.size	_nftl_nand_write_logic_page, .-_nftl_nand_write_logic_page
	.section	.text.current_block_at_first_page,"ax",%progbits
	.align	1
	.global	current_block_at_first_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	current_block_at_first_page, %function
current_block_at_first_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldrh	r0, [r1, #28]
	clz	r0, r0
	lsrs	r0, r0, #5
	bx	lr
	.size	current_block_at_first_page, .-current_block_at_first_page
	.section	.text.fill_no_use_data_to_page,"ax",%progbits
	.align	1
	.global	fill_no_use_data_to_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	fill_no_use_data_to_page, %function
fill_no_use_data_to_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	add	r5, r0, #292
	mov	r1, r5
	mov	r4, r0
	bl	current_block_at_last_page
	cbz	r0, .L2932
	mov	r0, r4
	bl	write_phy_page_map_to_current_block
	cbz	r0, .L2933
	ldr	r0, .L2961
.L2959:
	bl	NAND_Print
	movs	r0, #1
	b	.L2931
.L2933:
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r1, r3, #31
	bpl	.L2936
	add	r1, r4, #324
	mov	r0, r4
	bl	write_phy_page_map_to_current_block
	cbz	r0, .L2936
	ldr	r0, .L2961
	bl	NAND_Print
.L2936:
	mov	r1, r5
	mov	r0, r4
	bl	get_new_current_block
	cbz	r0, .L2938
	ldr	r0, .L2961+4
	b	.L2959
.L2938:
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r2, r3, #31
	bmi	.L2939
.L2940:
	movs	r0, #0
.L2931:
	pop	{r3, r4, r5, pc}
.L2939:
	add	r1, r4, #324
	mov	r0, r4
	bl	get_new_current_block
	cmp	r0, #0
	beq	.L2940
	ldr	r0, .L2961+8
	b	.L2959
.L2932:
	ldrh	r3, [r4, #320]
	cmp	r3, #0
	beq	.L2940
	mov	r0, r4
	bl	write_phy_page_map_to_current_block
	cbz	r0, .L2941
	ldr	r0, .L2961
	bl	NAND_Print
.L2941:
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r3, r3, #31
	bmi	.L2942
.L2960:
	movs	r0, #8
	b	.L2931
.L2942:
	add	r1, r4, #324
	mov	r0, r4
	bl	write_phy_page_map_to_current_block
	cmp	r0, #0
	beq	.L2960
	ldr	r0, .L2961
	bl	NAND_Print
	b	.L2960
.L2962:
	.align	2
.L2961:
	.word	.LC429
	.word	.LC430
	.word	.LC431
	.size	fill_no_use_data_to_page, .-fill_no_use_data_to_page
	.section	.text.fill_no_use_data_to_current_block,"ax",%progbits
	.align	1
	.global	fill_no_use_data_to_current_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	fill_no_use_data_to_current_block, %function
fill_no_use_data_to_current_block:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r5, r0
	movs	r4, #0
.L2964:
	ldr	r3, [r5, #4]
	ldrh	r3, [r3, #20]
	cmp	r3, r4
	bgt	.L2966
	movs	r0, #1
.L2963:
	pop	{r3, r4, r5, pc}
.L2966:
	mov	r0, r5
	bl	fill_no_use_data_to_page
	cmp	r0, #0
	beq	.L2963
	adds	r4, r4, #1
	b	.L2964
	.size	fill_no_use_data_to_current_block, .-fill_no_use_data_to_current_block
	.section	.text.done_static_wear_leveling,"ax",%progbits
	.align	1
	.global	done_static_wear_leveling
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	done_static_wear_leveling, %function
done_static_wear_leveling:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r4, r0
	ldr	r5, [r0, #580]
	cbz	r5, .L2976
	mov	r1, r5
	bl	is_current_blcok
	cmp	r0, #1
	bne	.L2972
	movs	r3, #0
	str	r3, [r4, #580]
.L2976:
	movs	r5, #2
	b	.L2970
.L2972:
	mov	r0, r4
	bl	is_block_in_invalid_list
	ldrh	r1, [r5]
	mov	r6, r0
	cbnz	r0, .L2973
	ldr	r0, .L2977
	bl	NAND_Print_DBG
	str	r6, [r4, #580]
	b	.L2976
.L2973:
	ldrh	r2, [r5, #6]
	movs	r5, #1
	ldr	r0, .L2977+4
	bl	NAND_Print_DBG
	ldr	r2, [r4, #1968]
	mov	r0, r4
	strh	r5, [r4, #578]	@ movhi
	ldr	r3, [r2, #128]
	add	r3, r3, r5
	str	r3, [r2, #128]
	bl	fill_no_use_data_to_current_block
	movw	r2, #65535
	ldr	r1, [r4, #580]
	mov	r0, r4
	bl	garbage_collect_first
	ldr	r2, [r4, #580]
	mov	r3, r0
	cbz	r0, .L2974
	ldrh	r1, [r2]
	ldr	r0, .L2977+8
	bl	NAND_Print
	movs	r3, #0
	str	r3, [r4, #580]
.L2970:
	mov	r0, r5
	pop	{r4, r5, r6, pc}
.L2974:
	strh	r0, [r4, #578]	@ movhi
	mov	r1, r2
	mov	r0, r4
	bl	is_block_in_free_list
	mov	r5, r0
	cbnz	r0, .L2975
	ldrh	r1, [r2]
	ldr	r0, .L2977+12
	bl	NAND_Print
	str	r5, [r4, #580]
	b	.L2970
.L2975:
	str	r3, [r4, #580]
	mov	r5, r3
	b	.L2970
.L2978:
	.align	2
.L2977:
	.word	.LC497
	.word	.LC498
	.word	.LC499
	.word	.LC500
	.size	done_static_wear_leveling, .-done_static_wear_leveling
	.section	.text.do_static_wear_leveling,"ax",%progbits
	.align	1
	.global	do_static_wear_leveling
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	do_static_wear_leveling, %function
do_static_wear_leveling:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #1972]
	push	{r4, lr}
	mov	r4, r0
	ldrh	r3, [r3, #6]
	cbnz	r3, .L2980
.L2982:
	movs	r0, #0
.L2979:
	pop	{r4, pc}
.L2980:
	ldrh	r3, [r0, #384]
	cbz	r3, .L2983
	bl	check_static_wear_leveling
	cmp	r0, #0
	bne	.L2982
	mov	r0, r4
	pop	{r4, lr}
	b	done_static_wear_leveling
.L2983:
	movs	r0, #1
	b	.L2979
	.size	do_static_wear_leveling, .-do_static_wear_leveling
	.section	.text.fill_no_use_page_to_current_block,"ax",%progbits
	.align	1
	.global	fill_no_use_page_to_current_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	fill_no_use_page_to_current_block, %function
fill_no_use_page_to_current_block:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r6, r0
	mov	r5, r1
	movs	r4, #0
.L2985:
	cmp	r4, r5
	blt	.L2987
	movs	r0, #1
.L2984:
	pop	{r4, r5, r6, pc}
.L2987:
	mov	r0, r6
	bl	fill_no_use_data_to_page
	cmp	r0, #0
	beq	.L2984
	adds	r4, r4, #1
	b	.L2985
	.size	fill_no_use_page_to_current_block, .-fill_no_use_page_to_current_block
	.section	.text.nftl_write_end,"ax",%progbits
	.align	1
	.global	nftl_write_end
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nftl_write_end, %function
nftl_write_end:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	ldr	r3, .L2999
	ldr	r4, [r3, #8]
	ldr	r3, [r4, #12]
	ldr	r3, [r3, #4]
	ldrb	r2, [r3, #36]	@ zero_extendqisi2
	cbz	r2, .L2998
	ldr	r5, [r3, #40]
	cmp	r5, #0
	it	eq
	moveq	r5, #8
.L2995:
	cbnz	r4, .L2996
.L2998:
	movs	r0, #0
	pop	{r3, r4, r5, pc}
.L2996:
	ldr	r0, [r4, #12]
	mov	r1, r5
	bl	fill_no_use_page_to_current_block
	ldr	r4, [r4, #8]
	b	.L2995
.L3000:
	.align	2
.L2999:
	.word	.LANCHOR0
	.size	nftl_write_end, .-nftl_write_end
	.section	.text.nand_before_shutdown,"ax",%progbits
	.align	1
	.global	nand_before_shutdown
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	nand_before_shutdown, %function
nand_before_shutdown:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r1, #8
	b	fill_no_use_page_to_current_block
	.size	nand_before_shutdown, .-nand_before_shutdown
	.global	samsung_p1
	.global	samsung_read_retry_cmd
	.global	samsung_read_retry_addr
	.global	samsung_read_retry_cycle
	.global	samsung_read_retry_mode
	.global	sandisk_1y
	.global	sandisk_read_retry_reg_adr_1y
	.global	sandisk_read_retry_cmd_1y
	.global	sandisk_read_retry_exit_cmd_1y
	.global	sandisk_read_retry_enable_cmd_1y
	.global	sandisk_24
	.global	sandisk_read_retry_reg_adr_24
	.global	sandisk_read_retry_cmd_24
	.global	sandisk_19_high
	.global	sandisk_19_low
	.global	sandisk_read_retry_reg_adr_19
	.global	sandisk_read_retry_cmd_19
	.global	sandisk_read_retry_disable_cmd_19
	.global	sandisk_read_retry_enable_cmd_19
	.global	sandisk_acti_start_addr
	.global	sandisk_acti_start_cmd
	.global	sandisk_read_retry_reg_cnt
	.global	sandisk_read_retry_cycle
	.global	sandisk_read_retry_mode
	.global	hynix16nm_read_retry_reg_adr
	.global	hynix16nm_read_retry_reg_num
	.global	hynix16nm_read_retry_cycle
	.global	hynix16nm_read_retry_mode
	.global	micron_p1_4
	.global	micron_p1_3
	.global	micron_p1_2
	.global	micron_p1_1
	.global	micron_read_retry_cycle
	.global	micron_read_retry_mode
	.global	toshiba_read_retry_exit_cmd
	.global	toshiba_read_retry_last_cmd
	.global	toshiba_p1_3
	.global	toshiba_p1_2
	.global	toshiba_read_retry_addr_2
	.global	toshiba_read_retry_cmd_2
	.global	toshiba_p1_1
	.global	toshiba_read_retry_addr_1
	.global	toshiba_read_retry_cmd_1
	.global	toshiba_read_retry_end_cmd
	.global	toshiba_read_retry_start_cmd
	.global	toshiba_read_retry_reg_cnt
	.global	toshiba_read_retry_cycle
	.global	toshiba_read_retry_mode
	.global	para1
	.global	para0
	.global	hynix26nm_lsb_mode_reg_num
	.global	hynix26nm_lsb_mode_val
	.global	hynix26nm_lsb_mode_default_val
	.global	hynix26nm_lsb_mode_reg_adr
	.global	hynix26nm_read_retry_val
	.global	hynix26nm_read_retry_reg_adr
	.global	hynix26nm_read_retry_reg_num
	.global	hynix26nm_read_retry_cycle
	.global	hynix26nm_read_retry_mode
	.global	hynix20nm_lsb_mode_reg_num
	.global	hynix20nm_lsb_mode_val
	.global	hynix20nm_lsb_mode_default_val
	.global	hynix20nm_lsb_mode_reg_adr
	.global	hynix20nm_read_retry_reg_adr_2
	.global	hynix20nm_read_retry_reg_adr_1
	.global	hynix20nm_read_retry_reg_adr
	.global	hynix20nm_read_retry_reg_num
	.global	hynix20nm_read_retry_cycle
	.global	hynix20nm_read_retry_mode
	.global	sandisk_A19_1z_8g
	.global	sandisk_A19_1z_16g
	.global	sandisk_A19_1y
	.global	sandisk_A19_read_retry_reg_adr_1y
	.global	sandisk_A19_read_retry_cmd_1y
	.global	sandisk_A19_read_retry_exit_cmd_1y
	.global	sandisk_A19_read_retry_enable_cmd_1y
	.global	sandisk_A19_read_retry_reg_cnt
	.global	sandisk_A19_read_retry_cycle
	.global	sandisk_A19_read_retry_mode
	.global	phyinfo
	.global	nftl_blk_head
	.global	mod
	.global	com
	.section	.rodata
	.set	.LANCHOR76,. + 0
.LC1:
	.byte	83
	.byte	83
	.byte	83
	.byte	83
	.byte	83
	.byte	83
	.byte	83
	.byte	83
	.byte	83
.LC2:
	.byte	115
	.byte	117
	.byte	110
	.byte	120
	.byte	105
.LC3:
	.byte	44
	.byte	100
	.byte	68
	.byte	50
	.byte	-91
	.byte	-1
	.byte	-1
	.byte	-1
.LC4:
	.byte	44
	.byte	-124
	.byte	68
	.byte	50
	.byte	-86
	.byte	4
	.byte	0
	.byte	0
.LC0:
	.byte	97
	.byte	108
	.byte	108
	.byte	119
	.byte	105
	.byte	110
	.byte	110
	.byte	101
	.byte	114
	.section	.bss.hynix16nm_read_retry_cycle,"aw",%nobits
	.set	.LANCHOR56,. + 0
	.type	hynix16nm_read_retry_cycle, %object
	.size	hynix16nm_read_retry_cycle, 1
hynix16nm_read_retry_cycle:
	.space	1
	.section	.bss.hynix16nm_read_retry_mode,"aw",%nobits
	.set	.LANCHOR59,. + 0
	.type	hynix16nm_read_retry_mode, %object
	.size	hynix16nm_read_retry_mode, 1
hynix16nm_read_retry_mode:
	.space	1
	.section	.bss.hynix16nm_read_retry_reg_adr,"aw",%nobits
	.set	.LANCHOR58,. + 0
	.type	hynix16nm_read_retry_reg_adr, %object
	.size	hynix16nm_read_retry_reg_adr, 4
hynix16nm_read_retry_reg_adr:
	.space	4
	.section	.bss.hynix16nm_read_retry_reg_num,"aw",%nobits
	.set	.LANCHOR57,. + 0
	.type	hynix16nm_read_retry_reg_num, %object
	.size	hynix16nm_read_retry_reg_num, 1
hynix16nm_read_retry_reg_num:
	.space	1
	.section	.bss.hynix20nm_lsb_mode_default_val,"aw",%nobits
	.set	.LANCHOR19,. + 0
	.type	hynix20nm_lsb_mode_default_val, %object
	.size	hynix20nm_lsb_mode_default_val, 4
hynix20nm_lsb_mode_default_val:
	.space	4
	.section	.bss.hynix20nm_lsb_mode_reg_adr,"aw",%nobits
	.set	.LANCHOR17,. + 0
	.type	hynix20nm_lsb_mode_reg_adr, %object
	.size	hynix20nm_lsb_mode_reg_adr, 4
hynix20nm_lsb_mode_reg_adr:
	.space	4
	.section	.bss.hynix20nm_lsb_mode_reg_num,"aw",%nobits
	.set	.LANCHOR16,. + 0
	.type	hynix20nm_lsb_mode_reg_num, %object
	.size	hynix20nm_lsb_mode_reg_num, 1
hynix20nm_lsb_mode_reg_num:
	.space	1
	.section	.bss.hynix20nm_lsb_mode_val,"aw",%nobits
	.set	.LANCHOR18,. + 0
	.type	hynix20nm_lsb_mode_val, %object
	.size	hynix20nm_lsb_mode_val, 4
hynix20nm_lsb_mode_val:
	.space	4
	.section	.bss.hynix20nm_read_retry_cycle,"aw",%nobits
	.set	.LANCHOR12,. + 0
	.type	hynix20nm_read_retry_cycle, %object
	.size	hynix20nm_read_retry_cycle, 1
hynix20nm_read_retry_cycle:
	.space	1
	.section	.bss.hynix20nm_read_retry_mode,"aw",%nobits
	.set	.LANCHOR15,. + 0
	.type	hynix20nm_read_retry_mode, %object
	.size	hynix20nm_read_retry_mode, 1
hynix20nm_read_retry_mode:
	.space	1
	.section	.bss.hynix20nm_read_retry_reg_adr,"aw",%nobits
	.set	.LANCHOR14,. + 0
	.type	hynix20nm_read_retry_reg_adr, %object
	.size	hynix20nm_read_retry_reg_adr, 8
hynix20nm_read_retry_reg_adr:
	.space	8
	.section	.bss.hynix20nm_read_retry_reg_num,"aw",%nobits
	.set	.LANCHOR13,. + 0
	.type	hynix20nm_read_retry_reg_num, %object
	.size	hynix20nm_read_retry_reg_num, 1
hynix20nm_read_retry_reg_num:
	.space	1
	.section	.bss.hynix26nm_lsb_mode_default_val,"aw",%nobits
	.set	.LANCHOR28,. + 0
	.type	hynix26nm_lsb_mode_default_val, %object
	.size	hynix26nm_lsb_mode_default_val, 5
hynix26nm_lsb_mode_default_val:
	.space	5
	.section	.bss.hynix26nm_lsb_mode_reg_adr,"aw",%nobits
	.set	.LANCHOR26,. + 0
	.type	hynix26nm_lsb_mode_reg_adr, %object
	.size	hynix26nm_lsb_mode_reg_adr, 5
hynix26nm_lsb_mode_reg_adr:
	.space	5
	.section	.bss.hynix26nm_lsb_mode_reg_num,"aw",%nobits
	.set	.LANCHOR25,. + 0
	.type	hynix26nm_lsb_mode_reg_num, %object
	.size	hynix26nm_lsb_mode_reg_num, 1
hynix26nm_lsb_mode_reg_num:
	.space	1
	.section	.bss.hynix26nm_lsb_mode_val,"aw",%nobits
	.set	.LANCHOR27,. + 0
	.type	hynix26nm_lsb_mode_val, %object
	.size	hynix26nm_lsb_mode_val, 5
hynix26nm_lsb_mode_val:
	.space	5
	.section	.bss.hynix26nm_read_retry_cycle,"aw",%nobits
	.set	.LANCHOR20,. + 0
	.type	hynix26nm_read_retry_cycle, %object
	.size	hynix26nm_read_retry_cycle, 1
hynix26nm_read_retry_cycle:
	.space	1
	.section	.bss.hynix26nm_read_retry_mode,"aw",%nobits
	.set	.LANCHOR24,. + 0
	.type	hynix26nm_read_retry_mode, %object
	.size	hynix26nm_read_retry_mode, 1
hynix26nm_read_retry_mode:
	.space	1
	.section	.bss.hynix26nm_read_retry_reg_adr,"aw",%nobits
	.set	.LANCHOR23,. + 0
	.type	hynix26nm_read_retry_reg_adr, %object
	.size	hynix26nm_read_retry_reg_adr, 4
hynix26nm_read_retry_reg_adr:
	.space	4
	.section	.bss.hynix26nm_read_retry_reg_num,"aw",%nobits
	.set	.LANCHOR21,. + 0
	.type	hynix26nm_read_retry_reg_num, %object
	.size	hynix26nm_read_retry_reg_num, 1
hynix26nm_read_retry_reg_num:
	.space	1
	.section	.bss.hynix26nm_read_retry_val,"aw",%nobits
	.align	1
	.set	.LANCHOR22,. + 0
	.type	hynix26nm_read_retry_val, %object
	.size	hynix26nm_read_retry_val, 56
hynix26nm_read_retry_val:
	.space	56
	.section	.bss.micron_p1,"aw",%nobits
	.align	2
	.set	.LANCHOR50,. + 0
	.type	micron_p1, %object
	.size	micron_p1, 4
micron_p1:
	.space	4
	.section	.bss.micron_read_retry_cycle,"aw",%nobits
	.set	.LANCHOR51,. + 0
	.type	micron_read_retry_cycle, %object
	.size	micron_read_retry_cycle, 1
micron_read_retry_cycle:
	.space	1
	.section	.bss.micron_read_retry_mode,"aw",%nobits
	.set	.LANCHOR49,. + 0
	.type	micron_read_retry_mode, %object
	.size	micron_read_retry_mode, 1
micron_read_retry_mode:
	.space	1
	.section	.bss.nand_on_panic,"aw",%nobits
	.align	2
	.set	.LANCHOR85,. + 0
	.type	nand_on_panic, %object
	.size	nand_on_panic, 4
nand_on_panic:
	.space	4
	.section	.bss.nftl_blk_head,"aw",%nobits
	.align	2
	.set	.LANCHOR0,. + 0
	.type	nftl_blk_head, %object
	.size	nftl_blk_head, 76
nftl_blk_head:
	.space	76
	.section	.bss.panic_enable,"aw",%nobits
	.align	2
	.set	.LANCHOR86,. + 0
	.type	panic_enable, %object
	.size	panic_enable, 4
panic_enable:
	.space	4
	.section	.bss.panic_info,"aw",%nobits
	.align	2
	.set	.LANCHOR88,. + 0
	.type	panic_info, %object
	.size	panic_info, 24
panic_info:
	.space	24
	.section	.bss.panic_part,"aw",%nobits
	.align	2
	.set	.LANCHOR87,. + 0
	.type	panic_part, %object
	.size	panic_part, 12
panic_part:
	.space	12
	.section	.bss.phyinfo,"aw",%nobits
	.align	2
	.set	.LANCHOR108,. + 0
	.type	phyinfo, %object
	.size	phyinfo, 20
phyinfo:
	.space	20
	.section	.bss.samsung_read_retry_cycle,"aw",%nobits
	.set	.LANCHOR81,. + 0
	.type	samsung_read_retry_cycle, %object
	.size	samsung_read_retry_cycle, 1
samsung_read_retry_cycle:
	.space	1
	.section	.bss.samsung_read_retry_mode,"aw",%nobits
	.set	.LANCHOR80,. + 0
	.type	samsung_read_retry_mode, %object
	.size	samsung_read_retry_mode, 1
samsung_read_retry_mode:
	.space	1
	.section	.bss.sandisk_A19_read_retry_cycle,"aw",%nobits
	.set	.LANCHOR2,. + 0
	.type	sandisk_A19_read_retry_cycle, %object
	.size	sandisk_A19_read_retry_cycle, 1
sandisk_A19_read_retry_cycle:
	.space	1
	.section	.bss.sandisk_A19_read_retry_mode,"aw",%nobits
	.set	.LANCHOR1,. + 0
	.type	sandisk_A19_read_retry_mode, %object
	.size	sandisk_A19_read_retry_mode, 1
sandisk_A19_read_retry_mode:
	.space	1
	.section	.bss.sandisk_A19_read_retry_reg_cnt,"aw",%nobits
	.set	.LANCHOR3,. + 0
	.type	sandisk_A19_read_retry_reg_cnt, %object
	.size	sandisk_A19_read_retry_reg_cnt, 1
sandisk_A19_read_retry_reg_cnt:
	.space	1
	.section	.bss.sandisk_A19_sclk0_bak,"aw",%nobits
	.align	2
	.set	.LANCHOR5,. + 0
	.type	sandisk_A19_sclk0_bak, %object
	.size	sandisk_A19_sclk0_bak, 4
sandisk_A19_sclk0_bak:
	.space	4
	.section	.bss.sandisk_A19_sclk1_bak,"aw",%nobits
	.align	2
	.set	.LANCHOR4,. + 0
	.type	sandisk_A19_sclk1_bak, %object
	.size	sandisk_A19_sclk1_bak, 4
sandisk_A19_sclk1_bak:
	.space	4
	.section	.bss.sandisk_read_retry_cycle,"aw",%nobits
	.set	.LANCHOR69,. + 0
	.type	sandisk_read_retry_cycle, %object
	.size	sandisk_read_retry_cycle, 1
sandisk_read_retry_cycle:
	.space	1
	.section	.bss.sandisk_read_retry_mode,"aw",%nobits
	.set	.LANCHOR62,. + 0
	.type	sandisk_read_retry_mode, %object
	.size	sandisk_read_retry_mode, 1
sandisk_read_retry_mode:
	.space	1
	.section	.bss.sandisk_read_retry_reg_cnt,"aw",%nobits
	.set	.LANCHOR78,. + 0
	.type	sandisk_read_retry_reg_cnt, %object
	.size	sandisk_read_retry_reg_cnt, 1
sandisk_read_retry_reg_cnt:
	.space	1
	.section	.bss.sandisk_sclk0_bak,"aw",%nobits
	.align	2
	.set	.LANCHOR61,. + 0
	.type	sandisk_sclk0_bak, %object
	.size	sandisk_sclk0_bak, 4
sandisk_sclk0_bak:
	.space	4
	.section	.bss.sandisk_sclk1_bak,"aw",%nobits
	.align	2
	.set	.LANCHOR60,. + 0
	.type	sandisk_sclk1_bak, %object
	.size	sandisk_sclk1_bak, 4
sandisk_sclk1_bak:
	.space	4
	.section	.bss.sandisk_toggle_mode_flag,"aw",%nobits
	.align	2
	.set	.LANCHOR66,. + 0
	.type	sandisk_toggle_mode_flag, %object
	.size	sandisk_toggle_mode_flag, 4
sandisk_toggle_mode_flag:
	.space	4
	.section	.bss.toshiba_read_retry_cycle,"aw",%nobits
	.set	.LANCHOR47,. + 0
	.type	toshiba_read_retry_cycle, %object
	.size	toshiba_read_retry_cycle, 1
toshiba_read_retry_cycle:
	.space	1
	.section	.bss.toshiba_read_retry_mode,"aw",%nobits
	.set	.LANCHOR33,. + 0
	.type	toshiba_read_retry_mode, %object
	.size	toshiba_read_retry_mode, 1
toshiba_read_retry_mode:
	.space	1
	.section	.bss.toshiba_read_retry_reg_cnt,"aw",%nobits
	.set	.LANCHOR48,. + 0
	.type	toshiba_read_retry_reg_cnt, %object
	.size	toshiba_read_retry_reg_cnt, 1
toshiba_read_retry_reg_cnt:
	.space	1
	.section	.bss.toshiba_retry_flag,"aw",%nobits
	.align	2
	.set	.LANCHOR35,. + 0
	.type	toshiba_retry_flag, %object
	.size	toshiba_retry_flag, 4
toshiba_retry_flag:
	.space	4
	.section	.bss.toshiba_sclk0_bak,"aw",%nobits
	.align	2
	.set	.LANCHOR32,. + 0
	.type	toshiba_sclk0_bak, %object
	.size	toshiba_sclk0_bak, 4
toshiba_sclk0_bak:
	.space	4
	.section	.bss.toshiba_sclk1_bak,"aw",%nobits
	.align	2
	.set	.LANCHOR31,. + 0
	.type	toshiba_sclk1_bak, %object
	.size	toshiba_sclk1_bak, 4
toshiba_sclk1_bak:
	.space	4
	.section	.bss.toshiba_toggle_mode_flag,"aw",%nobits
	.align	2
	.set	.LANCHOR43,. + 0
	.type	toshiba_toggle_mode_flag, %object
	.size	toshiba_toggle_mode_flag, 4
toshiba_toggle_mode_flag:
	.space	4
	.section	.data.hynix20nm_read_retry_reg_adr_1,"aw",%progbits
	.type	hynix20nm_read_retry_reg_adr_1, %object
	.size	hynix20nm_read_retry_reg_adr_1, 8
hynix20nm_read_retry_reg_adr_1:
	.byte	-52
	.byte	-65
	.byte	-86
	.byte	-85
	.byte	-51
	.byte	-83
	.byte	-82
	.byte	-81
	.section	.data.hynix20nm_read_retry_reg_adr_2,"aw",%progbits
	.type	hynix20nm_read_retry_reg_adr_2, %object
	.size	hynix20nm_read_retry_reg_adr_2, 8
hynix20nm_read_retry_reg_adr_2:
	.byte	-80
	.byte	-79
	.byte	-78
	.byte	-77
	.byte	-76
	.byte	-75
	.byte	-74
	.byte	-73
	.section	.data.micron_p1_1,"aw",%progbits
	.set	.LANCHOR53,. + 0
	.type	micron_p1_1, %object
	.size	micron_p1_1, 10
micron_p1_1:
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	2
	.byte	3
	.byte	4
	.byte	5
	.byte	6
	.byte	7
	.section	.data.micron_p1_2,"aw",%progbits
	.set	.LANCHOR52,. + 0
	.type	micron_p1_2, %object
	.size	micron_p1_2, 12
micron_p1_2:
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	2
	.byte	3
	.byte	4
	.byte	5
	.byte	6
	.byte	7
	.byte	8
	.byte	12
	.section	.data.micron_p1_3,"aw",%progbits
	.set	.LANCHOR54,. + 0
	.type	micron_p1_3, %object
	.size	micron_p1_3, 7
micron_p1_3:
	.byte	1
	.byte	2
	.byte	3
	.byte	0
	.byte	1
	.byte	2
	.byte	3
	.section	.data.micron_p1_4,"aw",%progbits
	.set	.LANCHOR55,. + 0
	.type	micron_p1_4, %object
	.size	micron_p1_4, 18
micron_p1_4:
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	2
	.byte	3
	.byte	4
	.byte	5
	.byte	6
	.byte	7
	.byte	8
	.byte	9
	.byte	10
	.byte	11
	.byte	12
	.byte	13
	.byte	14
	.byte	15
	.section	.data.samsung_p1,"aw",%progbits
	.set	.LANCHOR84,. + 0
	.type	samsung_p1, %object
	.size	samsung_p1, 60
samsung_p1:
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	5
	.byte	10
	.byte	0
	.byte	0
	.byte	40
	.byte	0
	.byte	-20
	.byte	-40
	.byte	-19
	.byte	-11
	.byte	-19
	.byte	-26
	.byte	10
	.byte	15
	.byte	5
	.byte	0
	.byte	15
	.byte	10
	.byte	-5
	.byte	-20
	.byte	-24
	.byte	-17
	.byte	-24
	.byte	-36
	.byte	-15
	.byte	-5
	.byte	-2
	.byte	-16
	.byte	10
	.byte	0
	.byte	-5
	.byte	-20
	.byte	-48
	.byte	-30
	.byte	-48
	.byte	-62
	.byte	20
	.byte	15
	.byte	-5
	.byte	-20
	.byte	-24
	.byte	-5
	.byte	-24
	.byte	-36
	.byte	30
	.byte	20
	.byte	-5
	.byte	-20
	.byte	-5
	.byte	-1
	.byte	-5
	.byte	-8
	.byte	7
	.byte	12
	.byte	2
	.byte	0
	.section	.data.samsung_read_retry_addr,"aw",%progbits
	.set	.LANCHOR83,. + 0
	.type	samsung_read_retry_addr, %object
	.size	samsung_read_retry_addr, 4
samsung_read_retry_addr:
	.byte	-89
	.byte	-92
	.byte	-91
	.byte	-90
	.section	.data.samsung_read_retry_cmd,"aw",%progbits
	.set	.LANCHOR79,. + 0
	.type	samsung_read_retry_cmd, %object
	.size	samsung_read_retry_cmd, 4
samsung_read_retry_cmd:
	.byte	-95
	.byte	-95
	.byte	-95
	.byte	-95
	.section	.data.sandisk_19_high,"aw",%progbits
	.set	.LANCHOR70,. + 0
	.type	sandisk_19_high, %object
	.size	sandisk_19_high, 60
sandisk_19_high:
	.byte	0
	.byte	-16
	.byte	0
	.byte	15
	.byte	-32
	.byte	0
	.byte	15
	.byte	-48
	.byte	0
	.byte	14
	.byte	-32
	.byte	0
	.byte	14
	.byte	-48
	.byte	0
	.byte	13
	.byte	-16
	.byte	0
	.byte	13
	.byte	-32
	.byte	0
	.byte	13
	.byte	-48
	.byte	0
	.byte	1
	.byte	16
	.byte	0
	.byte	2
	.byte	32
	.byte	0
	.byte	2
	.byte	16
	.byte	0
	.byte	3
	.byte	32
	.byte	0
	.byte	15
	.byte	0
	.byte	0
	.byte	14
	.byte	-16
	.byte	0
	.byte	13
	.byte	-64
	.byte	0
	.byte	15
	.byte	-16
	.byte	0
	.byte	1
	.byte	0
	.byte	0
	.byte	2
	.byte	0
	.byte	0
	.byte	13
	.byte	-80
	.byte	0
	.byte	12
	.byte	-96
	.byte	0
	.section	.data.sandisk_19_low,"aw",%progbits
	.set	.LANCHOR68,. + 0
	.type	sandisk_19_low, %object
	.size	sandisk_19_low, 48
sandisk_19_low:
	.byte	-16
	.byte	0
	.byte	-16
	.byte	-32
	.byte	0
	.byte	-32
	.byte	-48
	.byte	0
	.byte	-48
	.byte	16
	.byte	0
	.byte	16
	.byte	32
	.byte	0
	.byte	32
	.byte	48
	.byte	0
	.byte	48
	.byte	-64
	.byte	0
	.byte	-48
	.byte	0
	.byte	0
	.byte	16
	.byte	0
	.byte	0
	.byte	32
	.byte	16
	.byte	0
	.byte	32
	.byte	-80
	.byte	0
	.byte	-48
	.byte	-96
	.byte	0
	.byte	-48
	.byte	-112
	.byte	0
	.byte	-48
	.byte	-80
	.byte	0
	.byte	-64
	.byte	-96
	.byte	0
	.byte	-64
	.byte	-112
	.byte	0
	.byte	-64
	.section	.data.sandisk_1y,"aw",%progbits
	.set	.LANCHOR75,. + 0
	.type	sandisk_1y, %object
	.size	sandisk_1y, 76
sandisk_1y:
	.byte	4
	.byte	0
	.byte	124
	.byte	124
	.byte	8
	.byte	0
	.byte	4
	.byte	4
	.byte	124
	.byte	124
	.byte	0
	.byte	124
	.byte	124
	.byte	124
	.byte	0
	.byte	0
	.byte	12
	.byte	0
	.byte	120
	.byte	120
	.byte	16
	.byte	0
	.byte	8
	.byte	8
	.byte	124
	.byte	124
	.byte	120
	.byte	120
	.byte	124
	.byte	124
	.byte	4
	.byte	4
	.byte	124
	.byte	124
	.byte	120
	.byte	116
	.byte	20
	.byte	4
	.byte	4
	.byte	0
	.byte	120
	.byte	120
	.byte	0
	.byte	124
	.byte	120
	.byte	120
	.byte	120
	.byte	120
	.byte	120
	.byte	120
	.byte	120
	.byte	116
	.byte	120
	.byte	120
	.byte	4
	.byte	0
	.byte	120
	.byte	116
	.byte	120
	.byte	116
	.byte	120
	.byte	116
	.byte	116
	.byte	112
	.byte	120
	.byte	116
	.byte	120
	.byte	120
	.byte	120
	.byte	112
	.byte	120
	.byte	116
	.byte	120
	.byte	112
	.byte	112
	.byte	112
	.section	.data.sandisk_24,"aw",%progbits
	.set	.LANCHOR73,. + 0
	.type	sandisk_24, %object
	.size	sandisk_24, 27
sandisk_24:
	.byte	0
	.byte	-16
	.byte	0
	.byte	0
	.byte	-32
	.byte	0
	.byte	-1
	.byte	-16
	.byte	-16
	.byte	-18
	.byte	-32
	.byte	-32
	.byte	-34
	.byte	-48
	.byte	-48
	.byte	-51
	.byte	-64
	.byte	-64
	.byte	1
	.byte	0
	.byte	0
	.byte	2
	.byte	0
	.byte	0
	.byte	3
	.byte	0
	.byte	0
	.section	.data.sandisk_A19_1y,"aw",%progbits
	.set	.LANCHOR9,. + 0
	.type	sandisk_A19_1y, %object
	.size	sandisk_A19_1y, 124
sandisk_A19_1y:
	.byte	124
	.byte	0
	.byte	0
	.byte	124
	.byte	4
	.byte	0
	.byte	124
	.byte	120
	.byte	120
	.byte	0
	.byte	120
	.byte	116
	.byte	8
	.byte	124
	.byte	0
	.byte	124
	.byte	0
	.byte	124
	.byte	124
	.byte	120
	.byte	124
	.byte	124
	.byte	120
	.byte	116
	.byte	0
	.byte	124
	.byte	116
	.byte	112
	.byte	0
	.byte	120
	.byte	0
	.byte	124
	.byte	0
	.byte	120
	.byte	124
	.byte	120
	.byte	0
	.byte	120
	.byte	120
	.byte	116
	.byte	0
	.byte	120
	.byte	116
	.byte	112
	.byte	0
	.byte	120
	.byte	112
	.byte	108
	.byte	0
	.byte	4
	.byte	4
	.byte	0
	.byte	0
	.byte	4
	.byte	0
	.byte	124
	.byte	12
	.byte	4
	.byte	124
	.byte	120
	.byte	12
	.byte	4
	.byte	120
	.byte	116
	.byte	16
	.byte	8
	.byte	0
	.byte	124
	.byte	16
	.byte	8
	.byte	4
	.byte	0
	.byte	120
	.byte	116
	.byte	120
	.byte	116
	.byte	120
	.byte	116
	.byte	116
	.byte	112
	.byte	120
	.byte	116
	.byte	112
	.byte	108
	.byte	120
	.byte	116
	.byte	108
	.byte	104
	.byte	120
	.byte	112
	.byte	120
	.byte	116
	.byte	120
	.byte	112
	.byte	116
	.byte	112
	.byte	120
	.byte	112
	.byte	108
	.byte	104
	.byte	120
	.byte	112
	.byte	112
	.byte	108
	.byte	120
	.byte	108
	.byte	112
	.byte	108
	.byte	120
	.byte	108
	.byte	108
	.byte	104
	.byte	120
	.byte	108
	.byte	104
	.byte	100
	.byte	116
	.byte	104
	.byte	108
	.byte	104
	.byte	116
	.byte	104
	.byte	104
	.byte	100
	.section	.data.sandisk_A19_1z_16g,"aw",%progbits
	.set	.LANCHOR7,. + 0
	.type	sandisk_A19_1z_16g, %object
	.size	sandisk_A19_1z_16g, 128
sandisk_A19_1z_16g:
	.byte	124
	.byte	0
	.byte	4
	.byte	0
	.byte	4
	.byte	0
	.byte	0
	.byte	124
	.byte	120
	.byte	0
	.byte	8
	.byte	4
	.byte	8
	.byte	0
	.byte	124
	.byte	120
	.byte	124
	.byte	124
	.byte	4
	.byte	0
	.byte	0
	.byte	124
	.byte	0
	.byte	124
	.byte	120
	.byte	124
	.byte	8
	.byte	4
	.byte	4
	.byte	124
	.byte	116
	.byte	120
	.byte	4
	.byte	4
	.byte	4
	.byte	0
	.byte	8
	.byte	4
	.byte	0
	.byte	124
	.byte	0
	.byte	4
	.byte	8
	.byte	4
	.byte	12
	.byte	4
	.byte	124
	.byte	120
	.byte	124
	.byte	4
	.byte	12
	.byte	8
	.byte	120
	.byte	120
	.byte	4
	.byte	0
	.byte	124
	.byte	120
	.byte	0
	.byte	124
	.byte	116
	.byte	120
	.byte	112
	.byte	120
	.byte	8
	.byte	8
	.byte	4
	.byte	0
	.byte	4
	.byte	8
	.byte	8
	.byte	4
	.byte	12
	.byte	8
	.byte	12
	.byte	8
	.byte	0
	.byte	0
	.byte	12
	.byte	8
	.byte	4
	.byte	0
	.byte	120
	.byte	116
	.byte	124
	.byte	124
	.byte	120
	.byte	116
	.byte	120
	.byte	120
	.byte	120
	.byte	116
	.byte	8
	.byte	8
	.byte	0
	.byte	124
	.byte	116
	.byte	116
	.byte	0
	.byte	124
	.byte	120
	.byte	116
	.byte	124
	.byte	120
	.byte	112
	.byte	116
	.byte	116
	.byte	116
	.byte	124
	.byte	116
	.byte	112
	.byte	112
	.byte	12
	.byte	12
	.byte	8
	.byte	4
	.byte	16
	.byte	12
	.byte	12
	.byte	8
	.byte	112
	.byte	112
	.byte	112
	.byte	116
	.byte	116
	.byte	112
	.byte	112
	.byte	112
	.section	.data.sandisk_A19_1z_8g,"aw",%progbits
	.set	.LANCHOR8,. + 0
	.type	sandisk_A19_1z_8g, %object
	.size	sandisk_A19_1z_8g, 124
sandisk_A19_1z_8g:
	.byte	4
	.byte	4
	.byte	120
	.byte	120
	.byte	4
	.byte	4
	.byte	124
	.byte	116
	.byte	0
	.byte	0
	.byte	4
	.byte	120
	.byte	4
	.byte	4
	.byte	124
	.byte	124
	.byte	0
	.byte	0
	.byte	0
	.byte	124
	.byte	0
	.byte	0
	.byte	0
	.byte	116
	.byte	8
	.byte	8
	.byte	4
	.byte	120
	.byte	124
	.byte	124
	.byte	4
	.byte	120
	.byte	124
	.byte	124
	.byte	0
	.byte	124
	.byte	4
	.byte	4
	.byte	124
	.byte	112
	.byte	124
	.byte	124
	.byte	116
	.byte	116
	.byte	0
	.byte	0
	.byte	120
	.byte	112
	.byte	12
	.byte	12
	.byte	8
	.byte	120
	.byte	120
	.byte	120
	.byte	124
	.byte	124
	.byte	4
	.byte	4
	.byte	8
	.byte	4
	.byte	120
	.byte	120
	.byte	8
	.byte	120
	.byte	124
	.byte	124
	.byte	120
	.byte	112
	.byte	120
	.byte	120
	.byte	112
	.byte	108
	.byte	0
	.byte	0
	.byte	116
	.byte	108
	.byte	8
	.byte	8
	.byte	0
	.byte	116
	.byte	124
	.byte	124
	.byte	120
	.byte	108
	.byte	0
	.byte	0
	.byte	4
	.byte	4
	.byte	116
	.byte	116
	.byte	116
	.byte	108
	.byte	120
	.byte	120
	.byte	124
	.byte	112
	.byte	12
	.byte	12
	.byte	0
	.byte	116
	.byte	4
	.byte	4
	.byte	12
	.byte	8
	.byte	120
	.byte	120
	.byte	124
	.byte	116
	.byte	120
	.byte	120
	.byte	112
	.byte	104
	.byte	8
	.byte	8
	.byte	0
	.byte	112
	.byte	16
	.byte	16
	.byte	12
	.byte	120
	.byte	0
	.byte	0
	.byte	12
	.byte	8
	.section	.data.sandisk_A19_read_retry_cmd_1y,"aw",%progbits
	.set	.LANCHOR6,. + 0
	.type	sandisk_A19_read_retry_cmd_1y, %object
	.size	sandisk_A19_read_retry_cmd_1y, 1
sandisk_A19_read_retry_cmd_1y:
	.byte	-17
	.section	.data.sandisk_A19_read_retry_enable_cmd_1y,"aw",%progbits
	.type	sandisk_A19_read_retry_enable_cmd_1y, %object
	.size	sandisk_A19_read_retry_enable_cmd_1y, 1
sandisk_A19_read_retry_enable_cmd_1y:
	.byte	93
	.section	.data.sandisk_A19_read_retry_exit_cmd_1y,"aw",%progbits
	.type	sandisk_A19_read_retry_exit_cmd_1y, %object
	.size	sandisk_A19_read_retry_exit_cmd_1y, 1
sandisk_A19_read_retry_exit_cmd_1y:
	.byte	-1
	.section	.data.sandisk_A19_read_retry_reg_adr_1y,"aw",%progbits
	.set	.LANCHOR10,. + 0
	.type	sandisk_A19_read_retry_reg_adr_1y, %object
	.size	sandisk_A19_read_retry_reg_adr_1y, 1
sandisk_A19_read_retry_reg_adr_1y:
	.byte	17
	.section	.data.sandisk_acti_start_addr,"aw",%progbits
	.set	.LANCHOR77,. + 0
	.type	sandisk_acti_start_addr, %object
	.size	sandisk_acti_start_addr, 9
sandisk_acti_start_addr:
	.byte	4
	.byte	5
	.byte	6
	.byte	7
	.byte	8
	.byte	9
	.byte	10
	.byte	11
	.byte	12
	.section	.data.sandisk_acti_start_cmd,"aw",%progbits
	.set	.LANCHOR63,. + 0
	.type	sandisk_acti_start_cmd, %object
	.size	sandisk_acti_start_cmd, 2
sandisk_acti_start_cmd:
	.byte	59
	.byte	-71
	.section	.data.sandisk_read_retry_cmd_19,"aw",%progbits
	.set	.LANCHOR64,. + 0
	.type	sandisk_read_retry_cmd_19, %object
	.size	sandisk_read_retry_cmd_19, 3
sandisk_read_retry_cmd_19:
	.byte	83
	.byte	83
	.byte	83
	.section	.data.sandisk_read_retry_cmd_1y,"aw",%progbits
	.set	.LANCHOR65,. + 0
	.type	sandisk_read_retry_cmd_1y, %object
	.size	sandisk_read_retry_cmd_1y, 1
sandisk_read_retry_cmd_1y:
	.byte	-17
	.section	.data.sandisk_read_retry_cmd_24,"aw",%progbits
	.type	sandisk_read_retry_cmd_24, %object
	.size	sandisk_read_retry_cmd_24, 3
sandisk_read_retry_cmd_24:
	.byte	83
	.byte	83
	.byte	83
	.section	.data.sandisk_read_retry_disable_cmd_19,"aw",%progbits
	.set	.LANCHOR67,. + 0
	.type	sandisk_read_retry_disable_cmd_19, %object
	.size	sandisk_read_retry_disable_cmd_19, 1
sandisk_read_retry_disable_cmd_19:
	.byte	-42
	.section	.data.sandisk_read_retry_enable_cmd_19,"aw",%progbits
	.type	sandisk_read_retry_enable_cmd_19, %object
	.size	sandisk_read_retry_enable_cmd_19, 1
sandisk_read_retry_enable_cmd_19:
	.byte	-74
	.section	.data.sandisk_read_retry_enable_cmd_1y,"aw",%progbits
	.type	sandisk_read_retry_enable_cmd_1y, %object
	.size	sandisk_read_retry_enable_cmd_1y, 1
sandisk_read_retry_enable_cmd_1y:
	.byte	93
	.section	.data.sandisk_read_retry_exit_cmd_1y,"aw",%progbits
	.type	sandisk_read_retry_exit_cmd_1y, %object
	.size	sandisk_read_retry_exit_cmd_1y, 1
sandisk_read_retry_exit_cmd_1y:
	.byte	-1
	.section	.data.sandisk_read_retry_reg_adr_19,"aw",%progbits
	.set	.LANCHOR71,. + 0
	.type	sandisk_read_retry_reg_adr_19, %object
	.size	sandisk_read_retry_reg_adr_19, 9
sandisk_read_retry_reg_adr_19:
	.byte	4
	.byte	5
	.byte	7
	.space	6
	.section	.data.sandisk_read_retry_reg_adr_1y,"aw",%progbits
	.set	.LANCHOR74,. + 0
	.type	sandisk_read_retry_reg_adr_1y, %object
	.size	sandisk_read_retry_reg_adr_1y, 1
sandisk_read_retry_reg_adr_1y:
	.byte	17
	.section	.data.sandisk_read_retry_reg_adr_24,"aw",%progbits
	.set	.LANCHOR72,. + 0
	.type	sandisk_read_retry_reg_adr_24, %object
	.size	sandisk_read_retry_reg_adr_24, 9
sandisk_read_retry_reg_adr_24:
	.byte	4
	.byte	5
	.byte	7
	.space	6
	.section	.data.toshiba_p1_1,"aw",%progbits
	.set	.LANCHOR46,. + 0
	.type	toshiba_p1_1, %object
	.size	toshiba_p1_1, 28
toshiba_p1_1:
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	4
	.byte	4
	.byte	4
	.byte	4
	.byte	124
	.byte	124
	.byte	124
	.byte	124
	.byte	120
	.byte	120
	.byte	120
	.byte	120
	.byte	116
	.byte	116
	.byte	116
	.byte	116
	.byte	8
	.byte	8
	.byte	8
	.byte	8
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.section	.data.toshiba_p1_2,"aw",%progbits
	.set	.LANCHOR44,. + 0
	.type	toshiba_p1_2, %object
	.size	toshiba_p1_2, 40
toshiba_p1_2:
	.byte	4
	.byte	4
	.byte	124
	.byte	126
	.byte	0
	.byte	0
	.byte	124
	.byte	120
	.byte	120
	.byte	0
	.byte	124
	.byte	118
	.byte	116
	.byte	114
	.byte	0
	.byte	8
	.byte	8
	.byte	0
	.byte	0
	.byte	0
	.byte	11
	.byte	126
	.byte	118
	.byte	116
	.byte	0
	.byte	16
	.byte	118
	.byte	114
	.byte	112
	.byte	0
	.byte	2
	.byte	0
	.byte	126
	.byte	124
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.section	.data.toshiba_p1_3,"aw",%progbits
	.set	.LANCHOR45,. + 0
	.type	toshiba_p1_3, %object
	.size	toshiba_p1_3, 55
toshiba_p1_3:
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	2
	.byte	4
	.byte	2
	.byte	0
	.byte	0
	.byte	124
	.byte	0
	.byte	124
	.byte	124
	.byte	0
	.byte	122
	.byte	0
	.byte	122
	.byte	122
	.byte	0
	.byte	120
	.byte	2
	.byte	120
	.byte	122
	.byte	0
	.byte	126
	.byte	4
	.byte	126
	.byte	122
	.byte	0
	.byte	118
	.byte	4
	.byte	118
	.byte	120
	.byte	0
	.byte	4
	.byte	4
	.byte	4
	.byte	118
	.byte	0
	.byte	6
	.byte	10
	.byte	6
	.byte	2
	.byte	0
	.byte	116
	.byte	124
	.byte	116
	.byte	118
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.section	.data.toshiba_read_retry_addr_1,"aw",%progbits
	.set	.LANCHOR42,. + 0
	.type	toshiba_read_retry_addr_1, %object
	.size	toshiba_read_retry_addr_1, 4
toshiba_read_retry_addr_1:
	.byte	4
	.byte	5
	.byte	6
	.byte	7
	.section	.data.toshiba_read_retry_addr_2,"aw",%progbits
	.set	.LANCHOR41,. + 0
	.type	toshiba_read_retry_addr_2, %object
	.size	toshiba_read_retry_addr_2, 5
toshiba_read_retry_addr_2:
	.byte	4
	.byte	5
	.byte	6
	.byte	7
	.byte	13
	.section	.data.toshiba_read_retry_cmd_1,"aw",%progbits
	.set	.LANCHOR38,. + 0
	.type	toshiba_read_retry_cmd_1, %object
	.size	toshiba_read_retry_cmd_1, 4
toshiba_read_retry_cmd_1:
	.byte	85
	.byte	85
	.byte	85
	.byte	85
	.section	.data.toshiba_read_retry_cmd_2,"aw",%progbits
	.set	.LANCHOR34,. + 0
	.type	toshiba_read_retry_cmd_2, %object
	.size	toshiba_read_retry_cmd_2, 5
toshiba_read_retry_cmd_2:
	.byte	85
	.byte	85
	.byte	85
	.byte	85
	.byte	85
	.section	.data.toshiba_read_retry_end_cmd,"aw",%progbits
	.set	.LANCHOR39,. + 0
	.type	toshiba_read_retry_end_cmd, %object
	.size	toshiba_read_retry_end_cmd, 2
toshiba_read_retry_end_cmd:
	.byte	38
	.byte	93
	.section	.data.toshiba_read_retry_exit_cmd,"aw",%progbits
	.set	.LANCHOR36,. + 0
	.type	toshiba_read_retry_exit_cmd, %object
	.size	toshiba_read_retry_exit_cmd, 1
toshiba_read_retry_exit_cmd:
	.byte	-1
	.section	.data.toshiba_read_retry_last_cmd,"aw",%progbits
	.set	.LANCHOR37,. + 0
	.type	toshiba_read_retry_last_cmd, %object
	.size	toshiba_read_retry_last_cmd, 1
toshiba_read_retry_last_cmd:
	.byte	-77
	.section	.data.toshiba_read_retry_start_cmd,"aw",%progbits
	.set	.LANCHOR40,. + 0
	.type	toshiba_read_retry_start_cmd, %object
	.size	toshiba_read_retry_start_cmd, 2
toshiba_read_retry_start_cmd:
	.byte	92
	.byte	-59
	.section	.rodata.__FUNCTION__.8562,"a",%progbits
	.set	.LANCHOR106,. + 0
	.type	__FUNCTION__.8562, %object
	.size	__FUNCTION__.8562, 25
__FUNCTION__.8562:
	.ascii	"nand_power_off_ecc_error\000"
	.section	.rodata.__func__.6081,"a",%progbits
	.set	.LANCHOR112,. + 0
	.type	__func__.6081, %object
	.size	__func__.6081, 9
__func__.6081:
	.ascii	"nftl_add\000"
	.section	.rodata.__func__.6248,"a",%progbits
	.set	.LANCHOR111,. + 0
	.type	__func__.6248, %object
	.size	__func__.6248, 16
__func__.6248:
	.ascii	"nftl_initialize\000"
	.section	.rodata.__func__.6486,"a",%progbits
	.set	.LANCHOR11,. + 0
	.type	__func__.6486, %object
	.size	__func__.6486, 25
__func__.6486:
	.ascii	"sandisk_A19_special_exit\000"
	.section	.rodata.__func__.7578,"a",%progbits
	.set	.LANCHOR82,. + 0
	.type	__func__.7578, %object
	.size	__func__.7578, 33
__func__.7578:
	.ascii	"samsung_set_readretry_K9GCGD8U0F\000"
	.section	.rodata.__func__.7690,"a",%progbits
	.set	.LANCHOR95,. + 0
	.type	__func__.7690, %object
	.size	__func__.7690, 19
__func__.7690:
	.ascii	"panic_recover_data\000"
	.section	.rodata.__func__.7975,"a",%progbits
	.set	.LANCHOR100,. + 0
	.type	__func__.7975, %object
	.size	__func__.7975, 16
__func__.7975:
	.ascii	"zone_param_init\000"
	.section	.rodata.__func__.8123,"a",%progbits
	.set	.LANCHOR107,. + 0
	.type	__func__.8123, %object
	.size	__func__.8123, 23
__func__.8123:
	.ascii	"get_current_used_block\000"
	.section	.rodata.__func__.8189,"a",%progbits
	.set	.LANCHOR89,. + 0
	.type	__func__.8189, %object
	.size	__func__.8189, 9
__func__.8189:
	.ascii	"init_nrc\000"
	.section	.rodata.__func__.8274,"a",%progbits
	.set	.LANCHOR96,. + 0
	.type	__func__.8274, %object
	.size	__func__.8274, 23
__func__.8274:
	.ascii	"torture_block_and_mark\000"
	.section	.rodata.__func__.8397,"a",%progbits
	.set	.LANCHOR98,. + 0
	.type	__func__.8397, %object
	.size	__func__.8397, 26
__func__.8397:
	.ascii	"block_last_page_ecc_error\000"
	.section	.rodata.__func__.8429,"a",%progbits
	.set	.LANCHOR90,. + 0
	.type	__func__.8429, %object
	.size	__func__.8429, 18
__func__.8429:
	.ascii	"nand_dbg_phy_read\000"
	.section	.rodata.__func__.8440,"a",%progbits
	.set	.LANCHOR91,. + 0
	.type	__func__.8440, %object
	.size	__func__.8440, 23
__func__.8440:
	.ascii	"nand_dbg_zone_phy_read\000"
	.section	.rodata.__func__.8451,"a",%progbits
	.set	.LANCHOR92,. + 0
	.type	__func__.8451, %object
	.size	__func__.8451, 24
__func__.8451:
	.ascii	"nand_dbg_zone_phy_write\000"
	.section	.rodata.__func__.8460,"a",%progbits
	.set	.LANCHOR93,. + 0
	.type	__func__.8460, %object
	.size	__func__.8460, 19
__func__.8460:
	.ascii	"nand_dbg_phy_write\000"
	.section	.rodata.__func__.8538,"a",%progbits
	.set	.LANCHOR104,. + 0
	.type	__func__.8538, %object
	.size	__func__.8538, 19
__func__.8538:
	.ascii	"write_data_to_nand\000"
	.section	.rodata.__func__.8570,"a",%progbits
	.set	.LANCHOR94,. + 0
	.type	__func__.8570, %object
	.size	__func__.8570, 11
__func__.8570:
	.ascii	"clean_zone\000"
	.section	.rodata.__func__.8602,"a",%progbits
	.set	.LANCHOR109,. + 0
	.type	__func__.8602, %object
	.size	__func__.8602, 11
__func__.8602:
	.ascii	"nftl_start\000"
	.section	.rodata.__func__.8736,"a",%progbits
	.set	.LANCHOR99,. + 0
	.type	__func__.8736, %object
	.size	__func__.8736, 19
__func__.8736:
	.ascii	"get_logic_page_map\000"
	.section	.rodata.__func__.8774,"a",%progbits
	.set	.LANCHOR101,. + 0
	.type	__func__.8774, %object
	.size	__func__.8774, 23
__func__.8774:
	.ascii	"nftl_sector_cache_init\000"
	.section	.rodata.__func__.9047,"a",%progbits
	.set	.LANCHOR113,. + 0
	.type	__func__.9047, %object
	.size	__func__.9047, 15
__func__.9047:
	.ascii	"shutdown_on_gc\000"
	.section	.rodata.__func__.9076,"a",%progbits
	.set	.LANCHOR103,. + 0
	.type	__func__.9076, %object
	.size	__func__.9076, 16
__func__.9076:
	.ascii	"nftl_cache_init\000"
	.section	.rodata.__func__.9407,"a",%progbits
	.set	.LANCHOR97,. + 0
	.type	__func__.9407, %object
	.size	__func__.9407, 35
__func__.9407:
	.ascii	"_nftl_nand_read_logic_history_page\000"
	.section	.rodata.__nand_discard.str1.1,"aMS",%progbits,1
.LC254:
	.ascii	"[ND]dicard sector:0x%x,len:0x%x\012\000"
	.section	.rodata.__nand_flush_sector_write_cache.str1.1,"aMS",%progbits,1
.LC241:
	.ascii	"[NE]nand_flush_write_cache write error\012\000"
	.section	.rodata.__nand_flush_write_cache.str1.1,"aMS",%progbits,1
.LC252:
	.ascii	"[ND]flush cache %d %d!\012\000"
	.section	.rodata.__nand_read.str1.1,"aMS",%progbits,1
.LC235:
	.ascii	"[ND]read sector:0x%x,len:0x%x\012\000"
	.section	.rodata.__nand_write.str1.1,"aMS",%progbits,1
.LC237:
	.ascii	"[ND]write sector:0x%x,len:0x%x\012\000"
	.section	.rodata._nand_read_history.str1.1,"aMS",%progbits,1
.LC177:
	.ascii	"[NE]paramter error. beyond reality capacity\012\000"
	.section	.rodata._nftl_nand_read_logic_history_page.str1.1,"aMS",%progbits,1
.LC169:
	.ascii	"[NE]%s malloc buffer fail\012\000"
.LC170:
	.ascii	"scan super block@%d\012\000"
.LC171:
	.ascii	"block@%d is bad\012\000"
.LC172:
	.ascii	"[NE]read sb@%u sp@%u err, maybe read a bad block\012"
	.ascii	"\000"
.LC173:
	.ascii	"[NE]read lb@%u lp@%u err\012\000"
.LC174:
	.ascii	"page@%u history%d data:\012\000"
.LC175:
	.ascii	"oob:\012\000"
.LC176:
	.ascii	"scan end total super blocks:%d\012\000"
	.section	.rodata._nftl_nand_read_logic_page.str1.1,"aMS",%progbits,1
.LC452:
	.ascii	"[NE]read a page: %d READ_RECLAIM!\012\000"
	.section	.rodata._nftl_nand_write_logic_page.str1.1,"aMS",%progbits,1
.LC496:
	.ascii	"[NE]error nand_write_logic_page_no_gc page_no:%u\012"
	.ascii	"\000"
	.section	.rodata.adjust_free_list.str1.1,"aMS",%progbits,1
.LC152:
	.ascii	"[NE]adjust free list fail!!\012\000"
.LC153:
	.ascii	"[NI]adjust free list : %d %d!!\012\000"
	.section	.rodata.block_last_page_ecc_error.str1.1,"aMS",%progbits,1
.LC189:
	.ascii	"[NE]do block_last_page_ecc_error!!\012\000"
.LC190:
	.ascii	"[NE]%s:malloc fail 1!\012\000"
.LC191:
	.ascii	"[NE]%s:malloc fail 2!\012\000"
.LC192:
	.ascii	"[NE]current_block_ecc_error unkown page data!!\012\000"
.LC193:
	.ascii	"[NE]current_block_ecc_error invalid page data!!\012"
	.ascii	"\000"
.LC194:
	.ascii	"[NE]current_block_ecc_error no free block!!!\012\000"
.LC195:
	.ascii	"[NE]do block_last_page_ecc_error end!!\012\000"
	.section	.rodata.build_zone_list.str1.1,"aMS",%progbits,1
.LC459:
	.ascii	"[ND]first\012\000"
.LC460:
	.ascii	"[NE]init_zone_after_first_scan error\012\000"
.LC461:
	.ascii	"[NE]second_scan_all_blocks error\012\000"
.LC462:
	.ascii	"[NI]nftl ok!\012\000"
	.section	.rodata.build_zone_list_first.str1.1,"aMS",%progbits,1
.LC167:
	.ascii	"[ND]build_zone_list_first.\012\000"
.LC168:
	.ascii	"[NE]no block %d\012\000"
	.section	.rodata.check_invalid_page_right.str1.1,"aMS",%progbits,1
.LC426:
	.ascii	"[ND]logic_page_map_temp NULL!!\012\000"
.LC427:
	.ascii	"[NE]invaild_page num error:[%d] [%d]  used_page:[%d"
	.ascii	"]!!\012\000"
	.section	.rodata.check_static_wear_leveling.str1.1,"aMS",%progbits,1
.LC226:
	.ascii	"[ND]swl: static WL need to do(%d : %d)\012\000"
.LC227:
	.ascii	"[ND]swl: min_ec_blk_no = %d,\012\000"
.LC228:
	.ascii	"[ND]swl: zone->blocks = %d)\012\000"
	.section	.rodata.clean_zone.str1.1,"aMS",%progbits,1
.LC143:
	.ascii	"[NE]%s erase fb@%d fail\012\000"
.LC144:
	.ascii	"[NE]%s erase ub@%u fail\012\000"
.LC145:
	.ascii	"[NE]not current block\012\000"
	.section	.rodata.com,"a",%progbits
	.set	.LANCHOR110,. + 0
	.type	com, %object
	.size	com, 11
com:
	.byte	99
	.byte	111
	.byte	109
	.byte	112
	.byte	97
	.byte	116
	.byte	105
	.byte	98
	.byte	108
	.byte	101
	.byte	0
	.section	.rodata.crc_wrong.str1.1,"aMS",%progbits,1
.LC334:
	.ascii	"[NE]exception case wb %04x %04x, len: %d\012\000"
	.section	.rodata.cross_talk_rebuild_current_block.str1.1,"aMS",%progbits,1
.LC385:
	.ascii	"[NE]cross_talk_rebuild_current_block fail 1\012\000"
.LC386:
	.ascii	"[NE]cross_talk_rebuild_current_block!! %d,%d,%d!\012"
	.ascii	"\000"
.LC387:
	.ascii	"[NE]cross_talk_rebuild_current_block fail 2\012\000"
.LC388:
	.ascii	"[NE]cross_talk_rebuild_current_block!! %d,%d,%d,%d,"
	.ascii	"%d,%d!\012\000"
.LC389:
	.ascii	"[NE]cross_talk_rebuild_current_block fail 3\012\000"
.LC390:
	.ascii	"[NE]corss talk rebuild 1 %d %d %d!\012\000"
.LC391:
	.ascii	"[NE]corss talk rebuild 2 %d %d %d!\012\000"
.LC392:
	.ascii	"[NE]get free block fail!\012\000"
.LC393:
	.ascii	"[NE]something is error here 14!\012\000"
.LC394:
	.ascii	"[NE]page lost %d %d\012\000"
	.section	.rodata.current_block_ecc_error.str1.1,"aMS",%progbits,1
.LC417:
	.ascii	"[NE]do current_block_ecc_error %d!!\012\000"
.LC418:
	.ascii	"[NE]power off ecc error 2.\012\000"
.LC419:
	.ascii	"[NE]current_block_ecc ok page %d %d!!\012\000"
.LC420:
	.ascii	"[NE]current_block_ecc_error page %d!!\012\000"
.LC421:
	.ascii	"[NE]current_block_ecc_error invalid page data %d!!\012"
	.ascii	"\000"
.LC422:
	.ascii	"[NE]current_block_ecc_error unkown page data %d!!\012"
	.ascii	"\000"
.LC423:
	.ascii	"[ND]do current_block_ecc_error end %d %d!!\012\000"
	.section	.rodata.do_after_write.str1.1,"aMS",%progbits,1
.LC429:
	.ascii	"[NE]write_phy_page_map_to_current_block error!\012\000"
.LC430:
	.ascii	"[NE]get_new_current_block error1!\012\000"
.LC431:
	.ascii	"[NE]get_new_current_block error2!\012\000"
	.section	.rodata.do_before_write.str1.1,"aMS",%progbits,1
.LC424:
	.ascii	"[NE]get_new_current_block error3!\012\000"
.LC425:
	.ascii	"[NE]get_new_current_block error4!\012\000"
	.section	.rodata.do_prio_gc.str1.1,"aMS",%progbits,1
.LC447:
	.ascii	"[ND]no block to prio gc\012\000"
.LC448:
	.ascii	"[ND]no need gc:%d, type:%d\012\000"
.LC449:
	.ascii	"[NE]do_prio_gc garbage_collect_first fail:%d, type:"
	.ascii	"%d\012\000"
.LC450:
	.ascii	"[ND]prio gc fail1:%d, type:%d\012\000"
.LC451:
	.ascii	"[ND]prio gc fail2:%d, type:%d\012\000"
	.section	.rodata.do_write_error.str1.1,"aMS",%progbits,1
.LC358:
	.ascii	"[NE]no free block\012\000"
.LC359:
	.ascii	"[NE]do_write_error erase error sb:%d!\012\000"
.LC360:
	.ascii	"[NE]do_write_error sb@%d nftl_nand_copy_page error!"
	.ascii	"\012\000"
	.section	.rodata.do_write_error_in_build_list.str1.1,"aMS",%progbits,1
.LC131:
	.ascii	"[NE]NAND EVENT:do_write_error erase error1!\012\000"
.LC132:
	.ascii	"[NE]NAND EVENT:do_write_error erase error2!\012\000"
	.section	.rodata.done_static_wear_leveling.str1.1,"aMS",%progbits,1
.LC497:
	.ascii	"[ND]swl: the block(%d) for static WL is not in inva"
	.ascii	"lid_list\012\000"
.LC498:
	.ascii	"[ND]swl: done_static_wear_leveling(min_ec) block:%d"
	.ascii	", erase_count:%d\012\000"
.LC499:
	.ascii	"[NE]swl: garbage_collect_first fail:%d\012\000"
.LC500:
	.ascii	"[NE]swl: last->check fail:%d\012\000"
	.section	.rodata.dumphex.str1.1,"aMS",%progbits,1
.LC5:
	.ascii	"%08x: \000"
.LC6:
	.ascii	"%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %"
	.ascii	"02x %02x %02x %02x %02x %02x\012\000"
.LC7:
	.ascii	"\012\000"
	.section	.rodata.erase_block.str1.1,"aMS",%progbits,1
.LC150:
	.ascii	"[NI]erase_block block %d!\012\000"
.LC151:
	.ascii	"[NE]erase_block block fail %d!\012\000"
	.section	.rodata.first_scan_all_blocks.str1.1,"aMS",%progbits,1
.LC212:
	.ascii	"[ND]fsab\012\000"
.LC213:
	.ascii	"[NI]ftl end block! %d!\012\000"
.LC214:
	.ascii	"[ND]READ_RECLAIM!\012\000"
.LC215:
	.ascii	"[NI]start page , do nothing\012\000"
.LC216:
	.ascii	"[ND]fill block %d\012\000"
.LC217:
	.ascii	"[NI]b@%d p@0 unknow spare data:%x,%x,%x,%x,%x,%x,%x"
	.ascii	",%x,%x,%x,%x,%x,%x,%x,%x,%x.\012\000"
.LC218:
	.ascii	"[NE]nftl bad block: %d\012\000"
.LC219:
	.ascii	"[NE]nftl b@%d p@0 ecc error\012\000"
.LC220:
	.ascii	"[NE]b@%d last page ok !\012\000"
.LC221:
	.ascii	"[NI]READ_RECLAIM!\012\000"
.LC222:
	.ascii	"[NE]last page blank ! %d!\012\000"
.LC223:
	.ascii	"[NE]last page unkown ! %d!\012\000"
.LC224:
	.ascii	"[NE]b@%d first page last page ecc error\012\000"
.LC225:
	.ascii	"[NE]b@%d last page spare data %x,%x,%x,%x,%x,%x,%x,"
	.ascii	"%x,%x,%x,%x,%x,%x,%x,%x,%x\012\000"
	.section	.rodata.flush_write_cache_to_nand.str1.1,"aMS",%progbits,1
.LC242:
	.ascii	"[NE]flush write cache to nand no cache!\012\000"
.LC243:
	.ascii	"[NE]ECC_LIMIT happened 10! page:%d cache: %d!\012\000"
.LC244:
	.ascii	"[NE]flush write cache to nand error!\012\000"
.LC245:
	.ascii	"[NE]fwctn %d\012\000"
.LC246:
	.ascii	"[NE]fwctn w+\012\000"
	.section	.rodata.garbage_collect.str1.1,"aMS",%progbits,1
.LC493:
	.ascii	"[NE]garbage_collect phy_block null\012\000"
.LC494:
	.ascii	"[ND]garbage too frequency! %d:%d\012\000"
.LC495:
	.ascii	"[NE]garbage_collect_first wrong fbn:%d\012\000"
	.section	.rodata.garbage_collect_first.str1.1,"aMS",%progbits,1
.LC436:
	.ascii	"[NI]garbage_collect_first none!!\012\000"
.LC437:
	.ascii	"[ND]garbage_collect_first wrong!!\012\000"
.LC438:
	.ascii	"[ND]garbage_collect_first cannot find PHY_MAPPING_P"
	.ascii	"AGE%d %d %d %d!!\012\000"
.LC439:
	.ascii	"[ND]garbage_collect_first cannot find PHY_MAPPING_P"
	.ascii	"AGE%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x."
	.ascii	"\012\000"
.LC440:
	.ascii	"[ND][NE]garbage_collect_first rebuild %d,%d!!\012\000"
.LC441:
	.ascii	"[ND]garbage_collect_first PHY_MAPPING_PAGEecc err b"
	.ascii	"lock:%d !!\012\000"
.LC442:
	.ascii	"[NE]garbage_collect_first invaild_page num error!!\012"
	.ascii	"\000"
.LC443:
	.ascii	"[ND]garbage_collect_first read a logic page error%d"
	.ascii	" %d %d %d spare data:\012\000"
.LC444:
	.ascii	"%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\012"
	.ascii	"\000"
.LC445:
	.ascii	"[NE]nand_write_logic_page_no_gc gc fail!!\012\000"
.LC446:
	.ascii	"[NE]erase ecc error block\012\000"
	.section	.rodata.gc_one.str1.1,"aMS",%progbits,1
.LC453:
	.ascii	"[ND]gc_one ok!\012\000"
.LC454:
	.ascii	"[NE]gc_one error!\012\000"
	.section	.rodata.gc_win_block.str1.1,"aMS",%progbits,1
.LC455:
	.ascii	"[ND][NE] gc_win_block gc_one error \012\000"
	.section	.rodata.generic_special_exit.str1.1,"aMS",%progbits,1
.LC108:
	.ascii	" generic special exit \012\000"
	.section	.rodata.generic_special_init.str1.1,"aMS",%progbits,1
.LC107:
	.ascii	" generic special init \012\000"
	.section	.rodata.get_current_used_block.str1.1,"aMS",%progbits,1
.LC374:
	.ascii	"%s %d:\012\000"
.LC375:
	.ascii	"[ND]recover %d %d\012\000"
.LC376:
	.ascii	"[NE]page used:%d invalid page count:%d error\012\000"
.LC377:
	.ascii	"[NE]new_block_init_for_write error here!\012\000"
.LC378:
	.ascii	"[NE]something is error here 10!\012\000"
.LC379:
	.ascii	"[NE]something is error here 11!\012\000"
.LC380:
	.ascii	"[NE]assist block page used:%d current block page us"
	.ascii	"ed:%d something is error here 12.\012\000"
.LC381:
	.ascii	"[NE]assist block page used:%d current block page us"
	.ascii	"ed:%d something is error here 17.\012\000"
.LC382:
	.ascii	"[NE]something is error here 16!\012\000"
.LC383:
	.ascii	"[ND]get a new free block\012\000"
.LC384:
	.ascii	"[NE]no free block to use!\012\000"
	.section	.rodata.get_logic_page_map.str1.1,"aMS",%progbits,1
.LC196:
	.ascii	"[NE]%s:logic_page:0x%x >= total_pages:0x%x\012\000"
	.section	.rodata.get_new_current_block.str1.1,"aMS",%progbits,1
.LC371:
	.ascii	"[ND]swl: new_cur_blk->alloc ok(max_ec): block_no = "
	.ascii	"%d; ec = %d; \012\000"
.LC372:
	.ascii	"[NE]no free block!!!!\012\000"
	.section	.rodata.get_used_page_num.str1.1,"aMS",%progbits,1
.LC352:
	.ascii	"[NE]get_used_page_num ecc error block:%d page:%d!\012"
	.ascii	"\000"
.LC353:
	.ascii	"[NE]power off ecc error 4!!\012\000"
.LC354:
	.ascii	"[NE]sb@%d sp@%d get_used_page_num used spare data %"
	.ascii	"x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\012\000"
	.section	.rodata.get_used_page_num_no_crosstalk.str1.1,"aMS",%progbits,1
.LC355:
	.ascii	"[NE]gupnnc ecc err b:%d p:%d.\012\000"
.LC356:
	.ascii	"[NE]gupnnc power off ecc err.\012\000"
.LC357:
	.ascii	"[NE]cross talk block:%d total_good_num %d good_page"
	.ascii	"_num: %d!\012\000"
	.section	.rodata.hynix16nm_get_read_retry_cfg.str1.1,"aMS",%progbits,1
.LC75:
	.ascii	"_get_read_retry_parameters, get rr count failed!\012"
	.ascii	"\000"
.LC76:
	.ascii	"rr cnt: %d\012\000"
.LC77:
	.ascii	"_get_read_retry_parameters, get rr reg count failed"
	.ascii	"!\012\000"
.LC78:
	.ascii	"rr reg cnt: %d\012\000"
.LC79:
	.ascii	"read retry value from otp error: rr_cnt %d rr_reg_c"
	.ascii	"nt %d!\012\000"
.LC80:
	.ascii	"_get_read_retry_parameters, get the %d-th byte of r"
	.ascii	"r table failed!\012\000"
.LC81:
	.ascii	"%02x \000"
	.section	.rodata.hynix16nm_get_rr_value_otp_hynix.str1.1,"aMS",%progbits,1
.LC84:
	.ascii	"otp_info_hynix_16nm : allocate memory fail\012\000"
.LC85:
	.ascii	"m1 CH %d chip %d get otp info failed!\012\000"
.LC86:
	.ascii	"m1 CH %d chip %d get read retry cfg from otp info f"
	.ascii	"ailed!\012\000"
	.section	.rodata.hynix16nm_get_special_info.str1.1,"aMS",%progbits,1
.LC60:
	.ascii	"get_hynix_special_info start!!\012\000"
.LC61:
	.ascii	"hynix_special_info:can't find uboot head\012\000"
.LC62:
	.ascii	"physic info start_block %d\012\000"
	.section	.rodata.hynix16nm_init_readretry_param.str1.1,"aMS",%progbits,1
.LC87:
	.ascii	"ch %d, chip %d Read Retry value Table from uboot\012"
	.ascii	"\000"
.LC88:
	.ascii	"[PHY_DBG] repair ch %d chip %d otp value end\012\000"
	.section	.rodata.hynix16nm_major_check_byte.str1.1,"aMS",%progbits,1
.LC72:
	.ascii	"_major_check_byte, wrong input para, level %d, len "
	.ascii	"%d\012\000"
.LC73:
	.ascii	"%d:  '1'-'0' : %d - %d\012\000"
.LC74:
	.ascii	"get bit %d failed!\012\000"
	.section	.rodata.hynix16nm_read_otp_info_hynix.str1.1,"aMS",%progbits,1
.LC82:
	.ascii	"start get read retry param from: ce %d, rb %d...\012"
	.ascii	"\000"
.LC83:
	.ascii	"invalid buffer for otp info!\012\000"
	.section	.rodata.hynix16nm_read_page_end.str1.1,"aMS",%progbits,1
.LC58:
	.ascii	"m1 retry!\012\000"
.LC59:
	.ascii	"m1 ReadRetry ok! ch =%d, chip = %d  block = %d, pag"
	.ascii	"e = %d, RetryCount = %d  \012\000"
	.section	.rodata.hynix16nm_readretry_exit.str1.1,"aMS",%progbits,1
.LC89:
	.ascii	"hynix16nm_readretry_exit. \012\000"
	.section	.rodata.hynix16nm_special_exit.str1.1,"aMS",%progbits,1
.LC92:
	.ascii	" hynix16nm_special_exit \012\000"
	.section	.rodata.hynix16nm_special_init.str1.1,"aMS",%progbits,1
.LC90:
	.ascii	" hynix16nm_special_init hynix16nm_read_retry_mode: "
	.ascii	"%d hynix16nm_read_retry_cycle:%d hynix16nm_read_ret"
	.ascii	"ry_reg_num:%d\012\000"
.LC91:
	.ascii	" hynix16nm_special_init error hynix16nm_read_retry_"
	.ascii	"mode: %d hynix16nm_read_retry_cycle:%d hynix16nm_re"
	.ascii	"ad_retry_reg_num:%d\012\000"
	.section	.rodata.hynix16nm_write_page_FF.str1.1,"aMS",%progbits,1
.LC71:
	.ascii	"m1 write page FF wrong\012\000"
	.section	.rodata.hynix20nm_getotpparam.str1.1,"aMS",%progbits,1
.LC25:
	.ascii	"_vender_get_param_otp_hynix time %d!\012\000"
.LC26:
	.ascii	"_vender_get_param_otp_hynix error!\012\000"
	.section	.rodata.hynix20nm_init_readretry_param.str1.1,"aMS",%progbits,1
.LC63:
	.ascii	"hynix20nm get default param error !\012\000"
.LC64:
	.ascii	"[PHY_GetDefaultParam]:readbuf malloc fail\012\000"
.LC65:
	.ascii	"ch %d, chip %d Read Retry value Table from uboot:\012"
	.ascii	"\000"
.LC66:
	.ascii	"0x%x \000"
.LC67:
	.ascii	"[PHY_DBG] ch %d, can't get right otp value from nan"
	.ascii	"d otp blocks, then use otp command\012\000"
.LC68:
	.ascii	"[PHY_DBG] repair ch %d otp value end\012\000"
	.section	.rodata.hynix20nm_lsb_exit.str1.1,"aMS",%progbits,1
.LC21:
	.ascii	"hynix20nm_lsb_exit\012\000"
	.section	.rodata.hynix20nm_read_page_end.str1.1,"aMS",%progbits,1
.LC184:
	.ascii	"hynix20nm retry!\012\000"
.LC185:
	.ascii	"sdata:0x%x %x %x %x \000"
.LC186:
	.ascii	"hynix20nm ReadRetry ok! ch =%d, chip = %d  block = "
	.ascii	"%d, page = %d, RetryCount = %d  \012\000"
	.section	.rodata.hynix20nm_readretry_exit.str1.1,"aMS",%progbits,1
.LC29:
	.ascii	"hynix20nm_readretry_exit. \012\000"
	.section	.rodata.hynix20nm_set_default_param.str1.1,"aMS",%progbits,1
.LC27:
	.ascii	"set retry default value: \000"
.LC28:
	.ascii	" %x\000"
	.section	.rodata.hynix20nm_set_readretry.str1.1,"aMS",%progbits,1
.LC19:
	.ascii	"retry para:\000"
.LC20:
	.ascii	" %x \000"
	.section	.rodata.hynix20nm_special_exit.str1.1,"aMS",%progbits,1
.LC30:
	.ascii	" hynix20nm_special_exit \012\000"
	.section	.rodata.hynix20nm_special_init.str1.1,"aMS",%progbits,1
.LC69:
	.ascii	" hynix20nm_special_init hynix20nm_read_retry_mode: "
	.ascii	"%d hynix20nm_read_retry_cycle:%d hynix20nm_read_ret"
	.ascii	"ry_reg_num:%d\012\000"
.LC70:
	.ascii	" hynix20nm_special_init error hynix20nm_read_retry_"
	.ascii	"mode: %d hynix20nm_read_retry_cycle:%d hynix20nm_re"
	.ascii	"ad_retry_reg_num:%d\012\000"
	.section	.rodata.hynix20nm_vender_get_param_otp_hynix.str1.1,"aMS",%progbits,1
.LC22:
	.ascii	"hynix OTP RegCount value error: 0x%x, 0x%x \012\000"
.LC23:
	.ascii	"otp copy failed!\012\000"
.LC24:
	.ascii	"otp copy %d is ok!\012\000"
	.section	.rodata.hynix26nm_get_default_param.str1.1,"aMS",%progbits,1
.LC36:
	.ascii	"hynix26nm_get_default_param: ch: %d, chip: %d, valu"
	.ascii	"e: 0x%x 0x%x 0x%x 0x%x \012\000"
	.section	.rodata.hynix26nm_lsb_exit.str1.1,"aMS",%progbits,1
.LC35:
	.ascii	"hynix26nm_lsb_exit\012\000"
	.section	.rodata.hynix26nm_read_page_end.str1.1,"aMS",%progbits,1
.LC33:
	.ascii	"m0 retry!\012\000"
.LC34:
	.ascii	"hynix26nm ReadRetry ok! ch =%d, chip = %d  block = "
	.ascii	"%d, page = %d, RetryCount = %d  \012\000"
	.section	.rodata.hynix26nm_readretry_exit.str1.1,"aMS",%progbits,1
.LC38:
	.ascii	"hynix26nm_readretry_exit. \012\000"
	.section	.rodata.hynix26nm_readretry_init.str1.1,"aMS",%progbits,1
.LC37:
	.ascii	"NFC_ReadRetryInit, unknown read retry mode 0x%x\012"
	.ascii	"\000"
	.section	.rodata.hynix26nm_set_readretry.str1.1,"aMS",%progbits,1
.LC32:
	.ascii	"hynix26nm retry param:\000"
	.section	.rodata.hynix26nm_special_exit.str1.1,"aMS",%progbits,1
.LC41:
	.ascii	" hynix26nm_special_exit \012\000"
	.section	.rodata.hynix26nm_special_init.str1.1,"aMS",%progbits,1
.LC39:
	.ascii	" hynix26nm_special_init hynix26nm_read_retry_mode: "
	.ascii	"%d hynix26nm_read_retry_cycle:%d hynix26nm_read_ret"
	.ascii	"ry_reg_num:%d\012\000"
.LC40:
	.ascii	" hynix26nm_special_init error hynix26nm_read_retry_"
	.ascii	"mode: %d hynix26nm_read_retry_cycle:%d hynix26nm_re"
	.ascii	"ad_retry_reg_num:%d\012\000"
	.section	.rodata.hynix26nm_vender_set_param.str1.1,"aMS",%progbits,1
.LC31:
	.ascii	"rr value %x %x %x %x!\012\000"
	.section	.rodata.init_nrc.str1.1,"aMS",%progbits,1
.LC133:
	.ascii	"%s %d:fail 1\012\000"
.LC134:
	.ascii	"%s %d:fail 2\012\000"
.LC135:
	.ascii	"%s %d:fail 3\012\000"
.LC136:
	.ascii	"%s %d:fail 4\012\000"
	.section	.rodata.init_zone_after_first_scan.str1.1,"aMS",%progbits,1
.LC128:
	.ascii	"[ND]before second %d %d.\012\000"
.LC129:
	.ascii	"[NE]only have blocks:%u,logic cap need blocks:%u th"
	.ascii	"is zone not enough data block!!\012\000"
.LC130:
	.ascii	"[NE]this zone not enough free block %d , %d!!\012\000"
	.section	.rodata.is_no_use_device.str1.1,"aMS",%progbits,1
.LC178:
	.ascii	"[NE]mtd size changed !\012\000"
	.section	.rodata.malloc_logic_page_map.str1.1,"aMS",%progbits,1
.LC201:
	.ascii	"[NE]malloc logic page map: total_pages@%u len@%u\012"
	.ascii	"\000"
.LC202:
	.ascii	"[NE]%s:malloc fail for logic_page_map %d\012\000"
.LC203:
	.ascii	"[NE]%s: malloc fail for logic_page_map %d\012\000"
.LC204:
	.ascii	"[NE]zone_param_init malloc_logic_page_map error %d\012"
	.ascii	"\000"
	.section	.rodata.micron_intel_set_feature.str1.1,"aMS",%progbits,1
.LC50:
	.ascii	"set feature(addr %d) p0: %d readretry Configuration"
	.ascii	" failed!\012\000"
	.section	.rodata.micron_read_page_end.str1.1,"aMS",%progbits,1
.LC51:
	.ascii	"micron retry!\012\000"
.LC52:
	.ascii	"micron ReadRetry ok! ch =%d, chip = %d  block = %d,"
	.ascii	" page = %d, RetryCount = %d  \012\000"
	.section	.rodata.micron_readretry_init.str1.1,"aMS",%progbits,1
.LC53:
	.ascii	"micron read retry cannot support!\012\000"
.LC54:
	.ascii	"micron read retry conut: %d !\012\000"
	.section	.rodata.micron_special_exit.str1.1,"aMS",%progbits,1
.LC57:
	.ascii	" micron_special_exit \012\000"
	.section	.rodata.micron_special_init.str1.1,"aMS",%progbits,1
.LC55:
	.ascii	" micron_special_init micron_read_retry_mode:%d micr"
	.ascii	"on_read_retry_cycle:%d \012\000"
.LC56:
	.ascii	" micron_special_init error micron_read_retry_mode:%"
	.ascii	"d micron_read_retry_cycle:%d \012\000"
	.section	.rodata.mod,"a",%progbits
	.set	.LANCHOR102,. + 0
	.type	mod, %object
	.size	mod, 6
mod:
	.byte	109
	.byte	111
	.byte	100
	.byte	101
	.byte	108
	.byte	0
	.section	.rodata.nand_cache_read.str1.1,"aMS",%progbits,1
.LC233:
	.ascii	"[NI]nand_cache_read error buf is  NULL!\012\000"
.LC234:
	.ascii	"[NE]error1 node %d,%d,%d!\012\000"
	.section	.rodata.nand_cache_write.str1.1,"aMS",%progbits,1
.LC247:
	.ascii	"[NE]error1 bitmap %x!\012\000"
.LC248:
	.ascii	"[NE]ECC_LIMIT happened 12! page:%d!\012\000"
.LC249:
	.ascii	"[NE]error1 flush write cache to nand\012\000"
.LC250:
	.ascii	"[NE]error2 flush write cache to nand\012\000"
.LC251:
	.ascii	"[NI]ECC_LIMIT happened 11! page:%d!\012\000"
	.section	.rodata.nand_check_table.str1.1,"aMS",%progbits,1
.LC467:
	.ascii	"[ND]==========================\012\000"
	.section	.rodata.nand_clean_zone_table2.str1.1,"aMS",%progbits,1
.LC141:
	.ascii	"[ND]zoneNO:%d total_pages: %d\012\000"
.LC142:
	.ascii	"[ND]NAND_Memset bytes: %d\012\000"
	.section	.rodata.nand_crc16_table,"a",%progbits
	.align	1
	.set	.LANCHOR105,. + 0
	.type	nand_crc16_table, %object
	.size	nand_crc16_table, 512
nand_crc16_table:
	.short	0
	.short	-16191
	.short	-15999
	.short	320
	.short	-15615
	.short	960
	.short	640
	.short	-15807
	.short	-14847
	.short	1728
	.short	1920
	.short	-14527
	.short	1280
	.short	-14911
	.short	-15231
	.short	1088
	.short	-13311
	.short	3264
	.short	3456
	.short	-12991
	.short	3840
	.short	-12351
	.short	-12671
	.short	3648
	.short	2560
	.short	-13631
	.short	-13439
	.short	2880
	.short	-14079
	.short	2496
	.short	2176
	.short	-14271
	.short	-10239
	.short	6336
	.short	6528
	.short	-9919
	.short	6912
	.short	-9279
	.short	-9599
	.short	6720
	.short	7680
	.short	-8511
	.short	-8319
	.short	8000
	.short	-8959
	.short	7616
	.short	7296
	.short	-9151
	.short	5120
	.short	-11071
	.short	-10879
	.short	5440
	.short	-10495
	.short	6080
	.short	5760
	.short	-10687
	.short	-11775
	.short	4800
	.short	4992
	.short	-11455
	.short	4352
	.short	-11839
	.short	-12159
	.short	4160
	.short	-4095
	.short	12480
	.short	12672
	.short	-3775
	.short	13056
	.short	-3135
	.short	-3455
	.short	12864
	.short	13824
	.short	-2367
	.short	-2175
	.short	14144
	.short	-2815
	.short	13760
	.short	13440
	.short	-3007
	.short	15360
	.short	-831
	.short	-639
	.short	15680
	.short	-255
	.short	16320
	.short	16000
	.short	-447
	.short	-1535
	.short	15040
	.short	15232
	.short	-1215
	.short	14592
	.short	-1599
	.short	-1919
	.short	14400
	.short	10240
	.short	-5951
	.short	-5759
	.short	10560
	.short	-5375
	.short	11200
	.short	10880
	.short	-5567
	.short	-4607
	.short	11968
	.short	12160
	.short	-4287
	.short	11520
	.short	-4671
	.short	-4991
	.short	11328
	.short	-7167
	.short	9408
	.short	9600
	.short	-6847
	.short	9984
	.short	-6207
	.short	-6527
	.short	9792
	.short	8704
	.short	-7487
	.short	-7295
	.short	9024
	.short	-7935
	.short	8640
	.short	8320
	.short	-8127
	.short	-24575
	.short	24768
	.short	24960
	.short	-24255
	.short	25344
	.short	-23615
	.short	-23935
	.short	25152
	.short	26112
	.short	-22847
	.short	-22655
	.short	26432
	.short	-23295
	.short	26048
	.short	25728
	.short	-23487
	.short	27648
	.short	-21311
	.short	-21119
	.short	27968
	.short	-20735
	.short	28608
	.short	28288
	.short	-20927
	.short	-22015
	.short	27328
	.short	27520
	.short	-21695
	.short	26880
	.short	-22079
	.short	-22399
	.short	26688
	.short	30720
	.short	-18239
	.short	-18047
	.short	31040
	.short	-17663
	.short	31680
	.short	31360
	.short	-17855
	.short	-16895
	.short	32448
	.short	32640
	.short	-16575
	.short	32000
	.short	-16959
	.short	-17279
	.short	31808
	.short	-19455
	.short	29888
	.short	30080
	.short	-19135
	.short	30464
	.short	-18495
	.short	-18815
	.short	30272
	.short	29184
	.short	-19775
	.short	-19583
	.short	29504
	.short	-20223
	.short	29120
	.short	28800
	.short	-20415
	.short	20480
	.short	-28479
	.short	-28287
	.short	20800
	.short	-27903
	.short	21440
	.short	21120
	.short	-28095
	.short	-27135
	.short	22208
	.short	22400
	.short	-26815
	.short	21760
	.short	-27199
	.short	-27519
	.short	21568
	.short	-25599
	.short	23744
	.short	23936
	.short	-25279
	.short	24320
	.short	-24639
	.short	-24959
	.short	24128
	.short	23040
	.short	-25919
	.short	-25727
	.short	23360
	.short	-26367
	.short	22976
	.short	22656
	.short	-26559
	.short	-30719
	.short	18624
	.short	18816
	.short	-30399
	.short	19200
	.short	-29759
	.short	-30079
	.short	19008
	.short	19968
	.short	-28991
	.short	-28799
	.short	20288
	.short	-29439
	.short	19904
	.short	19584
	.short	-29631
	.short	17408
	.short	-31551
	.short	-31359
	.short	17728
	.short	-30975
	.short	18368
	.short	18048
	.short	-31167
	.short	-32255
	.short	17088
	.short	17280
	.short	-31935
	.short	16640
	.short	-32319
	.short	-32639
	.short	16448
	.section	.rodata.nand_dbg_phy_read.str1.1,"aMS",%progbits,1
.LC137:
	.ascii	"[NE]%s:malloc 4096 fail!\012\000"
.LC138:
	.ascii	"[ND]spare: %x %x %x %x %x %x %x %x %x %x %x %x %x %"
	.ascii	"x %x %x\000"
.LC139:
	.ascii	"[ND]buf:%x %x %x %x %x %x %x %x %x %x %x.\012\000"
	.section	.rodata.nand_dbg_zone_phy_write.str1.1,"aMS",%progbits,1
.LC140:
	.ascii	"[NE]%s:malloc 8192 fail!\012\000"
	.section	.rodata.nand_discard.str1.1,"aMS",%progbits,1
.LC253:
	.ascii	"[NE]nand_discard paramter error!\012\000"
	.section	.rodata.nand_find_zone_table.str1.1,"aMS",%progbits,1
.LC463:
	.ascii	"[ND]build_zone_list start\012\000"
.LC464:
	.ascii	"[ND]nftl_cache_init start\012\000"
.LC465:
	.ascii	"[ND]=================================2============="
	.ascii	"==========================\012\000"
.LC466:
	.ascii	"[ND]nand_find_zone_table end\012\000"
	.section	.rodata.nand_power_off_ecc_error.str1.1,"aMS",%progbits,1
.LC335:
	.ascii	"[NE]exception case bad block\012\000"
.LC336:
	.ascii	"[NE]exception case wbc\012\000"
.LC337:
	.ascii	"[NE]exception case gd\012\000"
.LC338:
	.ascii	"[NE]%s %d, flag = %d\012\000"
.LC339:
	.ascii	"[NE]OOB :\000"
.LC340:
	.ascii	"%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %"
	.ascii	".2x %.2x %.2x %.2x %.2x %.2x\012\000"
	.section	.rodata.nand_sector_cache_read.str1.1,"aMS",%progbits,1
.LC236:
	.ascii	"[NE]nand_sector_cache_read 1!\012\000"
	.section	.rodata.nand_sector_cache_write.str1.1,"aMS",%progbits,1
.LC239:
	.ascii	"[NE]nand_sector_cache_write 1!\012\000"
.LC240:
	.ascii	"[NE]nand_sector_cache_write 2!\012\000"
	.section	.rodata.nand_write_data_page.str1.1,"aMS",%progbits,1
.LC432:
	.ascii	"[NE]do after write error1!\012\000"
.LC433:
	.ascii	"[NE]do_before_write error!\012\000"
.LC434:
	.ascii	"[NE]write data to page error!\012\000"
.LC435:
	.ascii	"[NE]do after write error2!\012\000"
	.section	.rodata.new_block_init_for_write.str1.1,"aMS",%progbits,1
.LC373:
	.ascii	"[NE]new_block_init_for_write erase error!\012\000"
	.section	.rodata.nftl_add.str1.1,"aMS",%progbits,1
.LC486:
	.ascii	"[NE]%s:malloc fail for nftl_blk\012\000"
.LC487:
	.ascii	"[NE]nftl_initialize failed\012\000"
.LC488:
	.ascii	"[ND]nftl_add ok\012\000"
	.section	.rodata.nftl_build_all.str1.1,"aMS",%progbits,1
.LC489:
	.ascii	"[NE]nftl_build_all error!\012\000"
	.section	.rodata.nftl_cache_init.str1.1,"aMS",%progbits,1
.LC231:
	.ascii	"[NE]%s:malloc fail for cache_node!\012\000"
.LC232:
	.ascii	"[NE]%s:malloc fail for cache_page_buf!\012\000"
	.section	.rodata.nftl_get_gc_info.str1.1,"aMS",%progbits,1
.LC229:
	.ascii	"%d %d %d %d %d %d/%d %d/%d\012\000"
.LC230:
	.ascii	"%d:%d \000"
	.section	.rodata.nftl_initialize.str1.1,"aMS",%progbits,1
.LC478:
	.ascii	"002\000"
.LC479:
	.ascii	"0\000"
.LC480:
	.ascii	"5\000"
.LC481:
	.ascii	"sunxi nftl version %s.%s.%s  %s-%s\012\000"
.LC482:
	.ascii	"15:11\000"
.LC483:
	.ascii	"2021.08.05\000"
.LC484:
	.ascii	"[NE]%s:malloc fail for nftl_cfg\012\000"
.LC485:
	.ascii	"[ND]max_erase_times = %i\012\000"
	.section	.rodata.nftl_malloc.str1.1,"aMS",%progbits,1
.LC11:
	.ascii	"[NI]malloc size is large %d!\012\000"
	.section	.rodata.nftl_read.str1.1,"aMS",%progbits,1
.LC9:
	.ascii	"[NE]parameter error %d,%d !\012\000"
	.section	.rodata.nftl_read_history.str1.1,"aMS",%progbits,1
.LC10:
	.ascii	"awa1543 nftl_logic_size:%u\012\000"
	.section	.rodata.nftl_sector_cache_init.str1.1,"aMS",%progbits,1
.LC211:
	.ascii	"[NE]%s:malloc fail for cache_buf!\012\000"
	.section	.rodata.nftl_start.str1.1,"aMS",%progbits,1
.LC468:
	.ascii	"[NE]%s: malloc fail for zone\012\000"
.LC469:
	.ascii	"[NE]%s: malloc fail for nand_chip\012\000"
.LC470:
	.ascii	"[NE]size@%u less than min free block nums@%u\012\000"
.LC471:
	.ascii	"[NE]%s: malloc fail for nand_block_info\012\000"
.LC472:
	.ascii	"[NE]not enough blocks %d!\012\000"
.LC473:
	.ascii	"[ND]nftl start:%d,%d\012\000"
.LC474:
	.ascii	"[NE]zone_param_init fail!\012\000"
.LC475:
	.ascii	"[NE]build_zone_list_first fail!\012\000"
.LC476:
	.ascii	"[NE]build_zone_list fail!\012\000"
.LC477:
	.ascii	"[NI]try again\012\000"
	.section	.rodata.no_cross_talk_rebuild_current_block.str1.1,"aMS",%progbits,1
.LC395:
	.ascii	"[NI]nctrcb1 %d,%d,%d!\012\000"
.LC396:
	.ascii	"[NE]no_cross_talk_rebuild_current_block fail 2\012\000"
.LC397:
	.ascii	"[NI]nctrcb2 %d,%d,%d,%d,%d,%d!\012\000"
.LC398:
	.ascii	"[NE]no_cross_talk_rebuild_current_block fail 4\012\000"
.LC399:
	.ascii	"[NE]no_cross_talk_rebuild_current_block erase block"
	.ascii	" fail %d!\012\000"
	.section	.rodata.panic_backup_block.str1.1,"aMS",%progbits,1
.LC122:
	.ascii	"[ND] reserved panic block before %d!\012\000"
.LC123:
	.ascii	"[NE] reserved panic block %d!\012\000"
.LC124:
	.ascii	"[NE] no reserved panic block!!! quit!\012\000"
	.section	.rodata.panic_init_part.str1.1,"aMS",%progbits,1
.LC109:
	.ascii	"[ND] panic part %s off %u size %u\012\000"
	.section	.rodata.panic_recover_data.str1.1,"aMS",%progbits,1
.LC154:
	.ascii	"[ND] not find panic data, no need to recovery!\012\000"
.LC155:
	.ascii	"[ND] recovery panic data start!\012\000"
.LC156:
	.ascii	"[NE] %s(%d): malloc failed\012\000"
.LC157:
	.ascii	"[ND] ignore blk %d page %d as dummy page\012\000"
.LC158:
	.ascii	"[ND] exit recovery panic block\012\000"
.LC159:
	.ascii	"[ND] invalid blk %d page %d, finish recovery\012\000"
.LC160:
	.ascii	"[ND] move blk %d page %d to logic addr %d\012\000"
	.section	.rodata.panic_recover_info.str1.1,"aMS",%progbits,1
.LC110:
	.ascii	"[ND] find panic block : %d\012\000"
.LC111:
	.ascii	"[NE] invalid panic block index %d\012\000"
	.section	.rodata.panic_write.str1.1,"aMS",%progbits,1
.LC146:
	.ascii	"[ND] write panic data : logic addr: %d, sectors %d,"
	.ascii	" pages %d, blocks %d\012\000"
.LC147:
	.ascii	"[NE] too much data for panic-write, %d sectors left"
	.ascii	" over\012\000"
.LC148:
	.ascii	"[ND] write panic data block: %d page %d !\012\000"
.LC149:
	.ascii	"[ND] write dummy data block: %d page %d !\012\000"
	.section	.rodata.para0,"a",%progbits
	.align	1
	.set	.LANCHOR29,. + 0
	.type	para0, %object
	.size	para0, 56
para0:
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	6
	.short	10
	.short	6
	.short	0
	.short	-3
	.short	-7
	.short	-8
	.short	0
	.short	-6
	.short	-13
	.short	-15
	.short	0
	.short	-11
	.short	-20
	.short	-23
	.short	0
	.short	0
	.short	-26
	.short	-30
	.short	0
	.short	0
	.short	-32
	.short	-37
	.section	.rodata.para1,"a",%progbits
	.align	1
	.set	.LANCHOR30,. + 0
	.type	para1, %object
	.size	para1, 56
para1:
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	6
	.short	10
	.short	6
	.short	0
	.short	-3
	.short	-7
	.short	-8
	.short	0
	.short	-6
	.short	-13
	.short	-15
	.short	0
	.short	-9
	.short	-20
	.short	-23
	.short	0
	.short	0
	.short	-26
	.short	-30
	.short	0
	.short	0
	.short	-32
	.short	-37
	.section	.rodata.phy_block_from_invalid_page_incr.str1.1,"aMS",%progbits,1
.LC126:
	.ascii	"[ND]invalid_page_count more than pages per blk 0 %d"
	.ascii	" %d!!\012\000"
.LC127:
	.ascii	"[ND]invalid_page_count more than pages per blk 1!!\012"
	.ascii	"\000"
	.section	.rodata.power_on_gc.str1.1,"aMS",%progbits,1
.LC456:
	.ascii	"[ND]not power on gc [%d, %d, %d], [%d %d %d]!\012\000"
.LC457:
	.ascii	"[ND]emergency power on gc %d !\012\000"
.LC458:
	.ascii	"[ND]power on gc %d!\012\000"
	.section	.rodata.print_all_nftl_blk.str1.1,"aMS",%progbits,1
.LC8:
	.ascii	"[ND]cap: 0x%08x.\012\000"
	.section	.rodata.print_block_count_list.str1.1,"aMS",%progbits,1
.LC118:
	.ascii	"[ND]block_used_count list: \012\000"
.LC119:
	.ascii	"[ND]block NO: %d \000"
.LC120:
	.ascii	"[ND]block_used_count: %d \012\000"
	.section	.rodata.print_block_invalid_list.str1.1,"aMS",%progbits,1
.LC116:
	.ascii	"[ND]invalid_page_count list: \012\000"
.LC117:
	.ascii	"[ND]count:%4d; invalid:%3d;\000"
	.section	.rodata.print_free_list.str1.1,"aMS",%progbits,1
.LC112:
	.ascii	"[ND]free list: \012\000"
.LC113:
	.ascii	"[ND]block NO:%4d; erase:%3d; \000"
.LC114:
	.ascii	"[ND]count:%4d; \000"
.LC115:
	.ascii	"[ND]info:0x%x\012\000"
	.section	.rodata.print_logic_page_map.str1.1,"aMS",%progbits,1
.LC255:
	.ascii	"[NI]page_map NULL!!\012\000"
.LC256:
	.ascii	"[ND]%4d:<%4d %3d><%d>\012\000"
	.section	.rodata.print_nftl_zone.str1.1,"aMS",%progbits,1
.LC281:
	.ascii	"[ND]nand3.x \012\000"
.LC282:
	.ascii	"[ND]zone->nand_chip->blk_per_chip: %d \012\000"
.LC283:
	.ascii	"[ND]zone->nand_chip->bytes_per_page: %d \012\000"
.LC284:
	.ascii	"[ND]zone->nand_chip->pages_per_blk: %d \012\000"
.LC285:
	.ascii	"[ND]zone->nand_chip->max_erase_times: %d \012\000"
.LC286:
	.ascii	"[ND]zone->nand_chip->support_read_reclaim: %d \012\000"
.LC287:
	.ascii	"[ND]zone->test: %d \012\000"
.LC288:
	.ascii	"[ND]zone->zone_no: %d \012\000"
.LC289:
	.ascii	"[ND]zone->zone_attr: %d \012\000"
.LC290:
	.ascii	"[ND]zone->blocks: %d \012\000"
.LC291:
	.ascii	"[ND]zone->bad_block: %d \012\000"
.LC292:
	.ascii	"[ND]zone->logic_cap_in_sects: %d \012\000"
.LC293:
	.ascii	"[ND]zone->backup_cap_in_sects: %d \012\000"
.LC294:
	.ascii	"[ND]zone->free_block_num: %d \012\000"
.LC295:
	.ascii	"[ND]zone->gc_strategy.start_gc_free_blocks: %d \012"
	.ascii	"\000"
.LC296:
	.ascii	"[ND]zone->gc_strategy.stop_gc_free_blocks: %d \012\000"
.LC297:
	.ascii	"[ND]zone->gc_strategy.gc_page: %d \012\000"
.LC298:
	.ascii	"[ND]zone->gc_strategy.process: %d \012\000"
.LC299:
	.ascii	"[ND]zone->prio_gc.prio_type : %d \012\000"
.LC300:
	.ascii	"[ND]zone->zone_start_phy_block->phy_block.Block_NO:"
	.ascii	" %d \012\000"
.LC301:
	.ascii	"[ND]zone->zone_end_phy_block->phy_block.Block_NO: %"
	.ascii	"d \012\000"
.LC302:
	.ascii	"[ND]zone->zone_phy_page_map_for_gc: %x \012\000"
.LC303:
	.ascii	"[ND]zone->current_block.user_info: %x \012\000"
.LC304:
	.ascii	"[ND]zone->current_block.block_info: %x \012\000"
.LC305:
	.ascii	"[ND]zone->current_block.block_info->phy_block.Block"
	.ascii	"_NO: %d \012\000"
.LC306:
	.ascii	"[ND]zone->current_block.page_used: %d \012\000"
.LC307:
	.ascii	"[ND]zone->current_block.block_info->block_used_coun"
	.ascii	"t: %d \012\000"
.LC308:
	.ascii	"[ND]zone->recv_free_block: %d %d!!\012\000"
.LC309:
	.ascii	"[ND]zone->assist_block.user_info: %x \012\000"
.LC310:
	.ascii	"[ND]zone->assist_block.block_info: %x \012\000"
.LC311:
	.ascii	"[ND]zone->assist_block.block_info->phy_block.Block_"
	.ascii	"NO: %d \012\000"
.LC312:
	.ascii	"[ND]zone->assist_block.page_used: %d \012\000"
.LC313:
	.ascii	"[ND]zone->assist_block.block_info->block_used_count"
	.ascii	": %d \012\000"
.LC314:
	.ascii	"[ND]zone->read_reclaim_complete: %x \012\000"
.LC315:
	.ascii	"[ND]zone->temp_page_buf: %x \012\000"
.LC316:
	.ascii	"[ND]zone->max_erase_num: %d \012\000"
.LC317:
	.ascii	"[ND]zone->cache.cache_totals: %x \012\000"
.LC318:
	.ascii	"[ND]zone->wl_gc_count: %d \012\000"
.LC319:
	.ascii	"[ND]zone->wl_gc_page_num: %d \012\000"
.LC320:
	.ascii	"[ND]zone->cfg->nftl_dont_use_cache: %x \012\000"
.LC321:
	.ascii	"[ND]zone->cfg->nftl_use_cache_sort: %x \012\000"
.LC322:
	.ascii	"[ND]zone->cfg->nftl_support_gc_read_reclaim: %x \012"
	.ascii	"\000"
.LC323:
	.ascii	"[ND]zone->cfg->nftl_support_wear_leveling: %x \012\000"
.LC324:
	.ascii	"[ND]zone->cfg->nftl_need_erase: %x \012\000"
.LC325:
	.ascii	"[ND]zone->cfg->nftl_min_free_block_num: %x \012\000"
.LC326:
	.ascii	"[ND]zone->cfg->nftl_gc_threshold_free_block_num: %x"
	.ascii	" \012\000"
.LC327:
	.ascii	"[ND]zone->cfg->nftl_min_free_block: %x \012\000"
.LC328:
	.ascii	"[ND]zone->cfg->nftl_gc_threshold_ratio_numerator: %"
	.ascii	"x \012\000"
.LC329:
	.ascii	"[ND]zone->cfg->nftl_gc_threshold_ratio_denominator:"
	.ascii	" %x \012\000"
.LC330:
	.ascii	"[ND]zone->cfg->nftl_max_cache_num: %d \012\000"
.LC331:
	.ascii	"[ND]zone->cfg->nftl_max_cache_write_num: %d \012\000"
.LC332:
	.ascii	"[ND]zone->cfg->nftl_cross_talk: %x \012\000"
.LC333:
	.ascii	"[ND]zone->cfg->nftl_read_claim_interval: %d \012\000"
	.section	.rodata.print_smart.str1.1,"aMS",%progbits,1
.LC257:
	.ascii	"[ND]smart->version:%x;\012\000"
.LC258:
	.ascii	"[ND]smart->total_recv_read_sectors:%lld;\012\000"
.LC259:
	.ascii	"[ND]smart->total_recv_write_sectors:%lld;\012\000"
.LC260:
	.ascii	"[ND]smart->total_real_read_pages:%lld;\012\000"
.LC261:
	.ascii	"[ND]smart->total_real_write_pages:%lld;\012\000"
.LC262:
	.ascii	"[ND]smart->total_recv_discard_sectors:%lld;\012\000"
.LC263:
	.ascii	"[ND]smart->total_real_discard_sectors:%lld;\012\000"
.LC264:
	.ascii	"[ND]smart->total_recv_read_claim_pages:%d;\012\000"
.LC265:
	.ascii	"[ND]smart->total_gc_times:%d;\012\000"
.LC266:
	.ascii	"[ND]smart->total_gc_pages:%d;\012\000"
.LC267:
	.ascii	"[ND]smart->total_wl_times:%d;\012\000"
.LC268:
	.ascii	"[ND]smart->total_wl_gc_times:%d;\012\000"
.LC269:
	.ascii	"[ND]smart->total_real_read_error_pages:%d;\012\000"
.LC270:
	.ascii	"[ND]smart->total_real_write_error_pages:%d;\012\000"
.LC271:
	.ascii	"[ND]smart->total_normal_power_cycles:%x;\012\000"
.LC272:
	.ascii	"[ND]smart->total_unusual_power_cycles:%x;\012\000"
.LC273:
	.ascii	"[ND]smart->max_block_erase_times:%d;\012\000"
.LC274:
	.ascii	"[ND]smart->min_block_erase_times:%d;\012\000"
.LC275:
	.ascii	"[ND]smart->max_block_used_counter:%d;\012\000"
.LC276:
	.ascii	"[ND]smart->min_block_used_counter:%d;\012\000"
.LC277:
	.ascii	"[ND]smart->read_reclaim_utc:%d;\012\000"
.LC278:
	.ascii	"[ND]smart->cross_talk_times:%d;\012\000"
.LC279:
	.ascii	"[ND]smart->read_reclaim_zone_no:%d;\012\000"
.LC280:
	.ascii	"[ND]smart->read_reclaim_page_no:%d;\012\000"
	.section	.rodata.put_phy_block_to_free_list.str1.1,"aMS",%progbits,1
.LC121:
	.ascii	"[ND]free_list error !\012\000"
	.section	.rodata.put_phy_block_to_invalid_page_list.str1.1,"aMS",%progbits,1
.LC125:
	.ascii	"[ND]add invalid_list error !\012\000"
	.section	.rodata.recover_block_phy_page_mapping.str1.1,"aMS",%progbits,1
.LC348:
	.ascii	"[NE]ret:%d read fail %d %d,read one more time!!\012"
	.ascii	"\000"
.LC349:
	.ascii	"[NE]ret:%d read fail %d %d,%d times,don't read more"
	.ascii	"!!\012\000"
.LC350:
	.ascii	"[NE]power off ecc error 3!!\012\000"
.LC351:
	.ascii	"[NE]recover sb@%d sp@%d phy page mapping error.\012"
	.ascii	"\000"
	.section	.rodata.recover_logic_page_mapping.str1.1,"aMS",%progbits,1
.LC197:
	.ascii	"[NI]logic_page_map is NULL\012\000"
.LC198:
	.ascii	"[NE]invalid_page_count more than pages_per_blk :%d\012"
	.ascii	"\000"
.LC199:
	.ascii	"[NE]inv P b:%d p:%d,lp %x\012\000"
.LC200:
	.ascii	"[NE]build invalid_page_count more than pages_per_bl"
	.ascii	"k\012\000"
	.section	.rodata.recover_phy_page_mapping.str1.1,"aMS",%progbits,1
.LC341:
	.ascii	"[NE]sb@%d sp@%d recover_phy_page_mapping no used pa"
	.ascii	"ge\012\000"
.LC342:
	.ascii	"[NE]sb@%d sp@%d power off ecc error1.\012\000"
.LC343:
	.ascii	"[NE]invalid logic_page block:%d page:%d,logic_page "
	.ascii	"0x%x.\012\000"
.LC344:
	.ascii	"[NE]rppm invalid page %d!!\012\000"
.LC345:
	.ascii	"[NI]power_down page %d!!\012\000"
.LC346:
	.ascii	"[NE]recover_phy_page_mapping unkown page data %d!!\012"
	.ascii	"\000"
.LC347:
	.ascii	"[NE]recover_phy_page_mapping ecc error block:%d;pag"
	.ascii	"e:%d.\012\000"
	.section	.rodata.samsung_read_page_end.str1.1,"aMS",%progbits,1
.LC102:
	.ascii	"samsung retry!\012\000"
.LC103:
	.ascii	"samsung ReadRetry ok! ch =%d, chip = %d  block = %d"
	.ascii	", page = %d, RetryCount = %d  \012\000"
	.section	.rodata.samsung_readretry_init.str1.1,"aMS",%progbits,1
.LC98:
	.ascii	"samsung read retry conut: %d !\012\000"
	.section	.rodata.samsung_set_readretry.str1.1,"aMS",%progbits,1
.LC100:
	.ascii	"m5 set readretry error ! %x \012\000"
.LC101:
	.ascii	"samsung set readretry error ! %x \012\000"
	.section	.rodata.samsung_set_readretry_K9GCGD8U0F.str1.1,"aMS",%progbits,1
.LC99:
	.ascii	"%s feature set fail\012\000"
	.section	.rodata.samsung_special_exit.str1.1,"aMS",%progbits,1
.LC106:
	.ascii	" samsung_special_exit \012\000"
	.section	.rodata.samsung_special_init.str1.1,"aMS",%progbits,1
.LC104:
	.ascii	" samsung_special_init samsung_read_retry_mode:%d sa"
	.ascii	"msung_read_retry_cycle :%d \012\000"
.LC105:
	.ascii	" samsung_special_init error samsung_read_retry_mode"
	.ascii	":%d samsung_read_retry_cycle :%d \012\000"
	.section	.rodata.sandisk_A19_check_bad_block_first_burn.str1.1,"aMS",%progbits,1
.LC18:
	.ascii	"fatal err -0, wrong input parameter, ch: %d  chip: "
	.ascii	"%d/%d  block: %d/%d \012\000"
	.section	.rodata.sandisk_A19_read_page_end.str1.1,"aMS",%progbits,1
.LC179:
	.ascii	"sandisk_A19 retry!\012\000"
.LC180:
	.ascii	"sandisk_A19 set readretry error\012\000"
.LC181:
	.ascii	"retry spare all 0xff! ch =%d, chip = %d  block = %d"
	.ascii	", page = %d, RetryCount = %d  \012\000"
.LC182:
	.ascii	"sandisk_A19 ReadRetry ok! ch =%d, chip = %d  block "
	.ascii	"= %d, page = %d, RetryCount = %d  \012\000"
.LC183:
	.ascii	"sandisk_A19 ReadRetry ok with ecc limit! ch =%d, ch"
	.ascii	"ip = %d  block = %d, page = %d, RetryCount = %d  \012"
	.ascii	"\000"
	.section	.rodata.sandisk_A19_readretry_init.str1.1,"aMS",%progbits,1
.LC12:
	.ascii	"sandisk_A19 retry mode err : %d !\012\000"
.LC13:
	.ascii	"sandisk A19 read retry conut: %d !\012\000"
	.section	.rodata.sandisk_A19_special_exit.str1.1,"aMS",%progbits,1
.LC16:
	.ascii	"%s sandisk toggle vendor specific setting failed!\012"
	.ascii	"\000"
.LC17:
	.ascii	" sandisk_A19_special_exit \012\000"
	.section	.rodata.sandisk_A19_special_init.str1.1,"aMS",%progbits,1
.LC14:
	.ascii	" sandisk_A19_special_init sandisk_A19_read_retry_mo"
	.ascii	"de:%d sandisk_A19_read_retry_cycle :%d sandisk_A19_"
	.ascii	"read_retry_reg_cnt %d \012\000"
.LC15:
	.ascii	" sandisk_A19_special_init error sandisk_A19_read_re"
	.ascii	"try_mode:%d sandisk_A19_read_retry_cycle :%d sandis"
	.ascii	"k_A19_read_retry_reg_cnt %d \012\000"
	.section	.rodata.sandisk_activation_seq.str1.1,"aMS",%progbits,1
.LC93:
	.ascii	"sandisk activation seq fail 0x%x\012\000"
	.section	.rodata.sandisk_read_page_end.str1.1,"aMS",%progbits,1
.LC187:
	.ascii	"sandisk retry!\012\000"
.LC188:
	.ascii	"sandisk ReadRetry ok! ch =%d, chip = %d  block = %d"
	.ascii	", page = %d, RetryCount = %d  \012\000"
	.section	.rodata.sandisk_readretry_init.str1.1,"aMS",%progbits,1
.LC94:
	.ascii	"sandisk read retry conut: %d !\012\000"
	.section	.rodata.sandisk_special_exit.str1.1,"aMS",%progbits,1
.LC97:
	.ascii	" sandisk_special_exit \012\000"
	.section	.rodata.sandisk_special_init.str1.1,"aMS",%progbits,1
.LC95:
	.ascii	" sandisk_special_init sandisk_read_retry_mode:%d sa"
	.ascii	"ndisk_read_retry_cycle :%d sandisk_read_retry_reg_c"
	.ascii	"nt %d \012\000"
.LC96:
	.ascii	" sandisk_special_init error sandisk_read_retry_mode"
	.ascii	":%d sandisk_read_retry_cycle :%d sandisk_read_retry"
	.ascii	"_reg_cnt %d \012\000"
	.section	.rodata.second_scan_all_blocks.str1.1,"aMS",%progbits,1
.LC400:
	.ascii	"[NI]NAND_EVENT: 2 current used block found %d,%d!\012"
	.ascii	"\000"
.LC401:
	.ascii	"[NE]no crosstalk:2 block used count diff %d,%d %d,%"
	.ascii	"d !\012\000"
.LC402:
	.ascii	"[NE]NAND_EVENT: muti current used block found1 %d %"
	.ascii	"d!\012\000"
.LC403:
	.ascii	"[NE]NAND_EVENT: muti current used block found2 %d %"
	.ascii	"d!\012\000"
.LC404:
	.ascii	"[NE]NAND_EVENT: muti current used block found3 %d %"
	.ascii	"d!\012\000"
.LC405:
	.ascii	"[NE]NAND_EVENT: last page spare data %x,%x,%x,%x,%x"
	.ascii	",%x,%x %x %x %x %x %x %x %x %x %x!!\012\000"
.LC406:
	.ascii	"[NI]zone@%d b@%d last page unkown ecc ok.\012\000"
.LC407:
	.ascii	"[NE]zone@%d b@%d last page ecc error.\012\000"
.LC408:
	.ascii	"[NE]something strange %d,%d!!\012\000"
.LC409:
	.ascii	"[NI]all block full!!\012\000"
.LC410:
	.ascii	"[ND]corss talk rebuild 0 %d %d!!\012\000"
.LC411:
	.ascii	"[NE]1st current block:%d 1st current block uc:%d mu"
	.ascii	"ti current used block 1st.\012\000"
.LC412:
	.ascii	"[ND]2nd current block:%d 2nd current block uc:%d mu"
	.ascii	"ti current used block 2nd.\012\000"
.LC413:
	.ascii	"[NE]something is error here 7!\012\000"
.LC414:
	.ascii	"[NE]p_phy_block_info_last %d %d.\012\000"
.LC415:
	.ascii	"[NE]get_current_used_block fail.\012\000"
.LC416:
	.ascii	"[ND]free block nums is %d, aec: %d, iec: %d, a_buc:"
	.ascii	" %d, i_buc: %d!\012\000"
	.section	.rodata.shutdown_on_gc.str1.1,"aMS",%progbits,1
.LC490:
	.ascii	"[ND]%s %p %d %d %d %\012\000"
.LC491:
	.ascii	"[NI]shutdown_on_gc end! [%d %d %d]\012\000"
.LC492:
	.ascii	"[ND]shutdown_on_gc one ok! [%d %d %d]\012\000"
	.section	.rodata.torture_block_and_mark.str1.1,"aMS",%progbits,1
.LC161:
	.ascii	"[NI]torture b@%d.\012\000"
.LC162:
	.ascii	"[NE]%s write b@%d p@%d err\012\000"
.LC163:
	.ascii	"[NE]%s read b@%d p@%d err\012\000"
.LC164:
	.ascii	"[NE]%s b@%d p@%d patt %02x checking main data fail\012"
	.ascii	"\000"
.LC165:
	.ascii	"[NE]%s b@%d p@%d patt %02x checking spare data fail"
	.ascii	"\012\000"
.LC166:
	.ascii	"[NE][NE]erase_block block fail %d!\012\000"
	.section	.rodata.toshiba_read_page_end.str1.1,"aMS",%progbits,1
.LC45:
	.ascii	"toshiba retry!\012\000"
.LC46:
	.ascii	"toshiba ReadRetry ok! ch =%d, chip = %d  block = %d"
	.ascii	", page = %d, RetryCount = %d  \012\000"
	.section	.rodata.toshiba_readretry_init.str1.1,"aMS",%progbits,1
.LC44:
	.ascii	"toshiba read retry conut: %d !\012\000"
	.section	.rodata.toshiba_special_exit.str1.1,"aMS",%progbits,1
.LC49:
	.ascii	" toshiba_special_exit \012\000"
	.section	.rodata.toshiba_special_init.str1.1,"aMS",%progbits,1
.LC47:
	.ascii	" toshiba_special_init toshiba_read_retry_mode :%d t"
	.ascii	"oshiba_read_retry_cycle :%d toshiba_read_retry_reg_"
	.ascii	"cnt :%d \012\000"
.LC48:
	.ascii	" toshiba_special_init error toshiba_read_retry_mode"
	.ascii	" :%d toshiba_read_retry_cycle :%d toshiba_read_retr"
	.ascii	"y_reg_cnt :%d \012\000"
	.section	.rodata.toshiba_vender_pre_condition.str1.1,"aMS",%progbits,1
.LC43:
	.ascii	"toshiba vender_pre_condition error!\012\000"
	.section	.rodata.toshiba_vender_set_param.str1.1,"aMS",%progbits,1
.LC42:
	.ascii	"toshiba rr value 3 %x %x %x %x!\012\000"
	.section	.rodata.write_data_to_nand.str1.1,"aMS",%progbits,1
.LC238:
	.ascii	"[NE][NE]%s:malloc 512 fail!\012\000"
	.section	.rodata.write_data_to_page.str1.1,"aMS",%progbits,1
.LC370:
	.ascii	"[NE]write logic data to current block error\012\000"
	.section	.rodata.write_logic_data_to_current_block.str1.1,"aMS",%progbits,1
.LC365:
	.ascii	"[ND]logic_page_map NULL!!\012\000"
.LC366:
	.ascii	"[ND]invalid_page_count more than pages_per_blk at w"
	.ascii	"rite logic data to current block:%d!!\012\000"
.LC367:
	.ascii	"[NE]write_phy_data_to_current_block error2\012\000"
.LC368:
	.ascii	"[NE]invalid_page_count more than pages_per_blk at w"
	.ascii	"rite logic data to current block:%d!!\012\000"
.LC369:
	.ascii	"[NE]write phy data to current block error2\012\000"
	.section	.rodata.write_phy_data_to_current_block.str1.1,"aMS",%progbits,1
.LC361:
	.ascii	"[ND]last block can not used to logic page %d!!\012\000"
.LC362:
	.ascii	"[ND]last block can not used to power down page %d!!"
	.ascii	"\012\000"
.LC363:
	.ascii	"[ND]write_phy_data_to_current_block write error %d!"
	.ascii	"\012\000"
.LC364:
	.ascii	"[NE]do_write_error error\012\000"
	.section	.rodata.write_phy_page_map_to_current_block.str1.1,"aMS",%progbits,1
.LC428:
	.ascii	"[NE]write_phy_data_to_current_block error1\012\000"
	.section	.rodata.zone_param_init.str1.1,"aMS",%progbits,1
.LC205:
	.ascii	"[NE]%s:malloc fail for temp_page_buf\012\000"
.LC206:
	.ascii	"[NE]%s:malloc fail for logic_page_buf\012\000"
.LC207:
	.ascii	"[NE]zone_param_init error2\012\000"
.LC208:
	.ascii	"[NE]%s:malloc fail for current_block.user_info.buf\012"
	.ascii	"\000"
.LC209:
	.ascii	"[NE]%s:malloc fail for assist_block.user_info.buf\012"
	.ascii	"\000"
.LC210:
	.ascii	"[NE]%s:malloc fail for zone_phy_page_map_for_gc.buf"
	.ascii	"\012\000"
	.ident	"GCC: (Linaro GCC 7.2-2017.11) 7.2.1 20171011"
	.section	.note.GNU-stack,"",%progbits
