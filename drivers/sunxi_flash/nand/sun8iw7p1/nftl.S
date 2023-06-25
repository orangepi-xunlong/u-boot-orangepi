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
	ldr	r3, [r0, #88]
	blx	r3
	adds	r3, r0, #2
	itttt	eq
	ldreq	r2, [r4, #1968]
	ldreq	r3, [r2, #136]
	addeq	r3, r3, #1
	streq	r3, [r2, #136]
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
	ldrd	r3, r5, [r2, #96]
	adds	r3, r3, #1
	adc	r5, r5, #0
	strd	r3, r5, [r2, #96]
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
	cbz	r0, .L9
	ldr	r2, [r4, #1968]
	ldr	r3, [r2, #140]
	adds	r3, r3, #1
	str	r3, [r2, #140]
.L9:
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
	ldr	r4, .L18
	ldr	r5, .L18+4
.L16:
	ldr	r3, [r4, #8]
	cbnz	r3, .L17
	pop	{r3, r4, r5, pc}
.L17:
	ldr	r1, [r3]
	mov	r0, r5
	bl	NAND_Print_DBG
	ldr	r4, [r4, #8]
	b	.L16
.L19:
	.align	2
.L18:
	.word	.LANCHOR0
	.word	.LC5
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
	ldr	r3, .L23
	movs	r0, #0
.L21:
	ldr	r3, [r3, #8]
	cbnz	r3, .L22
	bx	lr
.L22:
	adds	r0, r0, #1
	b	.L21
.L24:
	.align	2
.L23:
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
	ldr	r3, .L28
	movs	r0, #0
.L26:
	ldr	r3, [r3, #8]
	cbnz	r3, .L27
	bx	lr
.L27:
	ldr	r2, [r3]
	add	r0, r0, r2
	b	.L26
.L29:
	.align	2
.L28:
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
	ldr	r3, .L31
	ldr	r3, [r3, #8]
	ldr	r0, [r3]
	bx	lr
.L32:
	.align	2
.L31:
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
	cbnz	r1, .L42
.L34:
	mov	r0, r2
	pop	{r4, r5, r6, pc}
.L37:
	ldr	r4, [r4, #8]
	subs	r0, r0, r1
	cbnz	r4, .L36
	mov	r1, r0
	ldr	r0, .L43
	bl	NAND_Print
	movs	r2, #1
	b	.L34
.L42:
	ldr	r1, .L43+4
	ldr	r4, [r1, #8]
.L36:
	ldr	r1, [r4]
	cmp	r1, r0
	bls	.L37
	ldr	r5, [r4, #40]
	mov	r1, r0
	mov	r0, r4
	mov	ip, r5
	pop	{r4, r5, r6, lr}
	bx	ip
.L44:
	.align	2
.L43:
	.word	.LC6
	.word	.LANCHOR0
	.size	nftl_read, .-nftl_read
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
	cbnz	r1, .L54
.L46:
	mov	r0, r2
	pop	{r4, r5, r6, pc}
.L49:
	ldr	r4, [r4, #8]
	subs	r0, r0, r1
	cbnz	r4, .L48
	mov	r1, r0
	ldr	r0, .L55
	bl	NAND_Print
	movs	r2, #1
	b	.L46
.L54:
	ldr	r1, .L55+4
	ldr	r4, [r1, #8]
.L48:
	ldr	r1, [r4]
	cmp	r1, r0
	bls	.L49
	ldr	r5, [r4, #44]
	mov	r1, r0
	mov	r0, r4
	mov	ip, r5
	pop	{r4, r5, r6, lr}
	bx	ip
.L56:
	.align	2
.L55:
	.word	.LC6
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
	cbnz	r1, .L66
.L58:
	mov	r0, r2
	pop	{r4, pc}
.L61:
	ldr	r3, [r3, #8]
	subs	r0, r0, r1
	cbnz	r3, .L60
	mov	r1, r0
	ldr	r0, .L67
	bl	NAND_Print
	movs	r2, #1
	b	.L58
.L66:
	ldr	r3, .L67+4
	ldr	r3, [r3, #8]
.L60:
	ldr	r1, [r3]
	cmp	r1, r0
	bls	.L61
	ldr	r4, [r3, #52]
	mov	r1, r0
	mov	r0, r3
	mov	r3, r4
	pop	{r4, lr}
	bx	r3
.L68:
	.align	2
.L67:
	.word	.LC6
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
	ldr	r3, .L72
	push	{r4, lr}
	ldr	r4, [r3, #8]
.L70:
	cbnz	r4, .L71
	mov	r0, r4
	pop	{r4, pc}
.L71:
	ldr	r3, [r4, #48]
	mov	r0, r4
	movw	r1, #65535
	blx	r3
	ldr	r4, [r4, #8]
	b	.L70
.L73:
	.align	2
.L72:
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
	bls	.L75
	mov	r1, r0
	ldr	r0, .L76
	bl	NAND_Print
.L75:
	mov	r0, r4
	pop	{r4, lr}
	b	NAND_Malloc
.L77:
	.align	2
.L76:
	.word	.LC7
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
	.section	.text.init_nrc.isra.0,"ax",%progbits
	.align	1
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	init_nrc.isra.0, %function
init_nrc.isra.0:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r4, r5, r6, r7, r8, lr}
	mov	r7, r0
	movs	r0, #16
	mov	r6, r1
	bl	NAND_Malloc
	mov	r4, r0
	cbnz	r0, .L80
	movw	r2, #1233
	ldr	r1, .L100
	ldr	r0, .L100+4
	bl	printf
.L99:
	mov	r0, #-1
	b	.L79
.L80:
	movs	r2, #16
	movs	r1, #0
	bl	NAND_Memset
	ldr	r3, [r7]
	ldrh	r5, [r3, #20]
	ldrh	r0, [r3, #26]
	adds	r0, r0, #64
	muls	r0, r5, r0
	bl	nftl_malloc
	str	r0, [r4, #8]
	cbnz	r0, .L82
	ldr	r1, .L100
	movw	r2, #1242
	ldr	r0, .L100+8
.L98:
.L83:
	bl	printf
	ldr	r0, [r4, #8]
	cbz	r0, .L86
	bl	nftl_free
.L86:
	ldr	r0, [r4, #12]
	cbz	r0, .L87
	bl	nftl_free
.L87:
	ldr	r0, [r4, #4]
	cbz	r0, .L88
	bl	nftl_free
.L88:
	mov	r0, r4
	bl	nftl_free
	movs	r3, #0
	str	r3, [r6]
	b	.L99
.L82:
	lsl	r8, r5, #2
	mov	r0, r8
	bl	nftl_malloc
	str	r0, [r4, #12]
	cbnz	r0, .L84
	movw	r2, #1247
	ldr	r1, .L100
	ldr	r0, .L100+12
	b	.L98
.L84:
	mov	r2, r8
	movs	r1, #255
	bl	NAND_Memset
	mov	r0, r5
	bl	nftl_malloc
	str	r0, [r4, #4]
	cbnz	r0, .L85
	movw	r2, #1253
	ldr	r1, .L100
	ldr	r0, .L100+16
	b	.L98
.L85:
	mov	r2, r5
	movs	r1, #0
	bl	NAND_Memset
	ldr	r3, [r7]
	mov	r2, r4
	str	r4, [r6]
	ldr	r0, .L100+20
	ldrh	r1, [r3, #20]
	ldr	r3, [r4, #4]
	str	r3, [sp, #4]
	ldr	r3, [r4, #12]
	str	r3, [sp]
	ldr	r3, [r4, #8]
	bl	NAND_Print
	movs	r0, #0
.L79:
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L101:
	.align	2
.L100:
	.word	.LANCHOR1
	.word	.LC8
	.word	.LC9
	.word	.LC10
	.word	.LC11
	.word	.LC12
	.size	init_nrc.isra.0, .-init_nrc.isra.0
	.section	.text.free_nrc,"ax",%progbits
	.align	1
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
	cbz	r0, .L103
	ldr	r0, [r0, #8]
	bl	nftl_free
	ldr	r0, [r4, #4]
	bl	nftl_free
	ldr	r0, [r4, #12]
	bl	nftl_free
	mov	r0, r4
	bl	nftl_free
.L103:
	movs	r0, #0
	pop	{r4, pc}
	.size	free_nrc, .-free_nrc
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
.L108:
	ldr	r3, [r0, #8]
	cbnz	r3, .L109
	str	r1, [r0, #8]
	bx	lr
.L109:
	mov	r0, r3
	b	.L108
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
.L111:
	ldr	r3, [r0, #8]
	cbz	r3, .L110
	ldr	r2, [r3, #8]
	cbnz	r2, .L114
	str	r2, [r0, #8]
.L110:
	mov	r0, r3
	bx	lr
.L114:
	mov	r0, r3
	b	.L111
	.size	del_last_nftl_blk, .-del_last_nftl_blk
	.section	.text.m5_readretry_init,"ax",%progbits
	.align	1
	.global	m5_readretry_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m5_readretry_init, %function
m5_readretry_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, #0
	push	{r4, lr}
	str	r3, [r0, #176]
	movs	r1, #15
	ldr	r3, [r0, #252]
	ldr	r4, .L118
	ldr	r0, .L118+4
	ldrh	r3, [r3, #42]
	strb	r3, [r4]
	ldr	r3, .L118+8
	strb	r1, [r3]
	bl	NAND_Print_DBG
	ldrb	r3, [r4]	@ zero_extendqisi2
	cmp	r3, #32
	ite	ne
	mvnne	r0, #99
	moveq	r0, #0
	pop	{r4, pc}
.L119:
	.align	2
.L118:
	.word	.LANCHOR2
	.word	.LC13
	.word	.LANCHOR3
	.size	m5_readretry_init, .-m5_readretry_init
	.section	.text.m5_vender_set_param,"ax",%progbits
	.align	1
	.global	m5_vender_set_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m5_vender_set_param, %function
m5_vender_set_param:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, lr}
	sub	sp, sp, #20
	str	r3, [sp, #8]
	movs	r3, #1
	mov	r4, r1
	strd	r1, r3, [sp]
	mov	r3, r2
	ldr	r1, .L121
	movs	r2, #0
	bl	set_cmd_with_nand_bus
	mov	r5, r0
	ldrb	r0, [r4, #3]	@ zero_extendqisi2
	ldrb	r3, [r4, #2]	@ zero_extendqisi2
	ldrb	r2, [r4, #1]	@ zero_extendqisi2
	str	r0, [sp]
	ldrb	r1, [r4]	@ zero_extendqisi2
	ldr	r0, .L121+4
	bl	NAND_Print_DBG
	mov	r0, r5
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, pc}
.L122:
	.align	2
.L121:
	.word	.LANCHOR4
	.word	.LC14
	.size	m5_vender_set_param, .-m5_vender_set_param
	.section	.text.m5_readretry_exit,"ax",%progbits
	.align	1
	.global	m5_readretry_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m5_readretry_exit, %function
m5_readretry_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #0
	bx	lr
	.size	m5_readretry_exit, .-m5_readretry_exit
	.section	.text.m5_set_readretry,"ax",%progbits
	.align	1
	.global	m5_set_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m5_set_readretry, %function
m5_set_readretry:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r4, r0
	bl	nand_enable_chip
	ldr	r0, [r4, #248]
	bl	ndfc_disable_randomize
	ldr	r0, [r4, #176]
	movs	r3, #4
	ldr	r1, .L129
	ldr	r2, .L129+4
	add	r1, r1, r0, lsl #2
	mov	r0, r4
	bl	m5_vender_set_param
	mov	r5, r0
	cbz	r0, .L125
	ldr	r1, [r4, #176]
	ldr	r0, .L129+8
	bl	NAND_Print
.L125:
	mov	r0, r4
	bl	nand_disable_chip
	mov	r0, r5
	pop	{r3, r4, r5, pc}
.L130:
	.align	2
.L129:
	.word	.LANCHOR6
	.word	.LANCHOR5
	.word	.LC15
	.size	m5_set_readretry, .-m5_set_readretry
	.section	.text.m5_read_page_end,"ax",%progbits
	.align	1
	.global	m5_read_page_end
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m5_read_page_end, %function
m5_read_page_end:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L141
	push	{r0, r1, r4, r5, r6, r7, r8, lr}
	mov	r6, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r5, r0
	mov	r0, r6
	bl	m0_read_page_end_not_retry
	adds	r3, r0, #2
	mov	r4, r0
	bne	.L131
	ldr	r8, .L141+12
	movs	r7, #1
	ldr	r0, .L141+4
	bl	NAND_Print_DBG
.L133:
	ldrb	r3, [r8]	@ zero_extendqisi2
	cmp	r3, r7
	ble	.L136
	str	r7, [r5, #176]
	mov	r0, r5
	bl	m5_set_readretry
	mov	r4, r0
	cbnz	r0, .L134
	mov	r0, r6
	bl	m0_read_page_start
	mov	r4, r0
	mov	r0, r6
	bl	m0_read_page_end_not_retry
	orrs	r4, r4, r0
	cmp	r4, #10
	beq	.L135
	cbnz	r4, .L134
.L135:
	ldr	r3, [r5, #176]
	movs	r4, #10
	ldr	r1, [r5, #248]
	ldr	r0, .L141+8
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
.L136:
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	m5_set_readretry
.L131:
	mov	r0, r4
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L134:
	adds	r7, r7, #1
	b	.L133
.L142:
	.align	2
.L141:
	.word	g_nsi
	.word	.LC16
	.word	.LC17
	.word	.LANCHOR3
	.size	m5_read_page_end, .-m5_read_page_end
	.section	.text.m5_special_init,"ax",%progbits
	.align	1
	.global	m5_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m5_special_init, %function
m5_special_init:
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
	cbnz	r4, .L146
	ldr	r0, .L148+12
	ldr	r3, .L148+16
	str	r0, [r3]
	ldr	r0, .L148+20
	bl	NAND_Print_DBG
.L143:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L145:
	mov	r0, r5
	bl	m5_readretry_init
	ldr	r5, [r5]
	orrs	r4, r4, r0
	b	.L144
.L146:
	ldr	r0, .L148+24
	bl	NAND_Print
	b	.L143
.L149:
	.align	2
.L148:
	.word	g_nsi
	.word	.LANCHOR2
	.word	.LANCHOR3
	.word	m5_read_page_end
	.word	function_read_page_end
	.word	.LC18
	.word	.LC19
	.size	m5_special_init, .-m5_special_init
	.section	.text.m5_special_exit,"ax",%progbits
	.align	1
	.global	m5_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m5_special_exit, %function
m5_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L153
	push	{r4, lr}
	ldr	r3, [r3]
	ldr	r4, [r3, #8]
.L151:
	cbnz	r4, .L152
	ldr	r0, .L153+4
	bl	NAND_Print_DBG
	mov	r0, r4
	pop	{r4, pc}
.L152:
	ldr	r4, [r4]
	b	.L151
.L154:
	.align	2
.L153:
	.word	g_nsi
	.word	.LC20
	.size	m5_special_exit, .-m5_special_exit
	.section	.text.m5_is_lsb_page,"ax",%progbits
	.align	1
	.global	m5_is_lsb_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m5_is_lsb_page, %function
m5_is_lsb_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	cbz	r0, .L157
	ldr	r3, .L159
	ldr	r3, [r3]
	ldr	r3, [r3, #8]
	ldr	r3, [r3, #252]
	ldr	r3, [r3, #16]
	subs	r3, r3, #1
	cmp	r3, r0
	beq	.L158
	and	r0, r0, #1
	bx	lr
.L157:
	movs	r0, #1
	bx	lr
.L158:
	movs	r0, #0
	bx	lr
.L160:
	.align	2
.L159:
	.word	g_nsi
	.size	m5_is_lsb_page, .-m5_is_lsb_page
	.section	.text.m0_special_init,"ax",%progbits
	.align	1
	.global	m0_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m0_special_init, %function
m0_special_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	ldr	r2, .L162
	ldr	r3, .L162+4
	ldr	r0, .L162+8
	str	r2, [r3]
	bl	NAND_Print_DBG
	movs	r0, #0
	pop	{r3, pc}
.L163:
	.align	2
.L162:
	.word	m0_read_page_end_not_retry
	.word	function_read_page_end
	.word	.LC21
	.size	m0_special_init, .-m0_special_init
	.section	.text.m0_special_exit,"ax",%progbits
	.align	1
	.global	m0_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m0_special_exit, %function
m0_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	ldr	r0, .L165
	bl	NAND_Print_DBG
	movs	r0, #0
	pop	{r3, pc}
.L166:
	.align	2
.L165:
	.word	.LC22
	.size	m0_special_exit, .-m0_special_exit
	.section	.text.m0_is_lsb_page,"ax",%progbits
	.align	1
	.global	m0_is_lsb_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m0_is_lsb_page, %function
m0_is_lsb_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #1
	bx	lr
	.size	m0_is_lsb_page, .-m0_is_lsb_page
	.section	.text.m3_vender_get_param,"ax",%progbits
	.align	1
	.global	m3_vender_get_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m3_vender_get_param, %function
m3_vender_get_param:
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
	ldr	r0, [r6, #248]
	bl	ndfc_disable_randomize
.L169:
	cmp	r4, r7
	bne	.L170
	mov	r0, r6
	bl	nand_disable_chip
	movs	r0, #0
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L170:
	mov	r3, r5
	mov	r2, r4
	str	r10, [sp]
	mov	r1, r8
	mov	r0, r6
	adds	r4, r4, #1
	bl	get_data_with_nand_bus_one_cmd
	adds	r5, r5, #1
	b	.L169
	.size	m3_vender_get_param, .-m3_vender_get_param
	.section	.text.m3_vender_set_param,"ax",%progbits
	.align	1
	.global	m3_vender_set_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m3_vender_set_param, %function
m3_vender_set_param:
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
	ldr	r0, [r6, #248]
	bl	ndfc_disable_randomize
.L172:
	cmp	r4, r7
	bne	.L173
	mov	r0, r6
	bl	nand_disable_chip
	ldrb	r0, [r5, #3]	@ zero_extendqisi2
	ldrb	r3, [r5, #2]	@ zero_extendqisi2
	ldrb	r2, [r5, #1]	@ zero_extendqisi2
	str	r0, [sp]
	ldrb	r1, [r5]	@ zero_extendqisi2
	ldr	r0, .L174
	bl	NAND_Print_DBG
	movs	r0, #0
	add	sp, sp, #24
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L173:
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
	b	.L172
.L175:
	.align	2
.L174:
	.word	.LC14
	.size	m3_vender_set_param, .-m3_vender_set_param
	.section	.text.m3_lsb_init,"ax",%progbits
	.align	1
	.global	m3_lsb_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m3_lsb_init, %function
m3_lsb_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, [r0, #252]
	movs	r0, #0
	ldr	r2, .L177
	ldr	r3, [r3, #40]
	lsrs	r1, r3, #16
	strb	r1, [r2]
	lsrs	r1, r3, #8
	ldr	r2, .L177+4
	strb	r1, [r2]
	ldr	r2, .L177+8
	strb	r3, [r2]
	movs	r2, #5
	ldr	r3, .L177+12
	strb	r2, [r3]
	movs	r2, #164
	ldr	r3, .L177+16
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
	ldr	r3, .L177+20
	strb	r2, [r3]
	strb	r2, [r3, #1]
	strb	r2, [r3, #2]
	strb	r2, [r3, #3]
	movs	r2, #1
	strb	r2, [r3, #4]
	bx	lr
.L178:
	.align	2
.L177:
	.word	.LANCHOR7
	.word	.LANCHOR8
	.word	.LANCHOR9
	.word	.LANCHOR10
	.word	.LANCHOR11
	.word	.LANCHOR12
	.size	m3_lsb_init, .-m3_lsb_init
	.section	.text.m3_lsb_enable,"ax",%progbits
	.align	1
	.global	m3_lsb_enable
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m3_lsb_enable, %function
m3_lsb_enable:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r4, r5, r6, lr}
	mov	r5, r0
	ldr	r4, .L182
	ldr	r2, .L182+4
	ldrb	r3, [r4]	@ zero_extendqisi2
	ldr	r1, .L182+8
	bl	m3_vender_get_param
	ldrb	r3, [r4]	@ zero_extendqisi2
	movs	r2, #0
	ldr	r0, .L182+8
	ldr	r4, .L182+12
.L180:
	cmp	r3, r2
	bhi	.L181
	ldr	r2, .L182+4
	mov	r1, sp
	mov	r0, r5
	bl	m3_vender_set_param
	movs	r0, #0
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, pc}
.L181:
	ldrb	r1, [r2, r0]	@ zero_extendqisi2
	ldrb	r6, [r2, r4]	@ zero_extendqisi2
	add	r1, r1, r6
	strb	r1, [sp, r2]
	adds	r2, r2, #1
	b	.L180
.L183:
	.align	2
.L182:
	.word	.LANCHOR10
	.word	.LANCHOR11
	.word	.LANCHOR13
	.word	.LANCHOR12
	.size	m3_lsb_enable, .-m3_lsb_enable
	.section	.text.m3_lsb_disable,"ax",%progbits
	.align	1
	.global	m3_lsb_disable
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m3_lsb_disable, %function
m3_lsb_disable:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	ldr	r3, .L185
	ldr	r2, .L185+4
	ldrb	r3, [r3]	@ zero_extendqisi2
	ldr	r1, .L185+8
	bl	m3_vender_set_param
	movs	r0, #0
	pop	{r3, pc}
.L186:
	.align	2
.L185:
	.word	.LANCHOR10
	.word	.LANCHOR11
	.word	.LANCHOR13
	.size	m3_lsb_disable, .-m3_lsb_disable
	.section	.text.m3_lsb_exit,"ax",%progbits
	.align	1
	.global	m3_lsb_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m3_lsb_exit, %function
m3_lsb_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	bl	nand_enable_chip
	ldr	r0, [r4, #248]
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
	ldr	r0, .L188
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	nand_disable_chip
	movs	r0, #0
	pop	{r4, pc}
.L189:
	.align	2
.L188:
	.word	.LC23
	.size	m3_lsb_exit, .-m3_lsb_exit
	.section	.text.m3_getdefaultparam,"ax",%progbits
	.align	1
	.global	m3_getdefaultparam
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m3_getdefaultparam, %function
m3_getdefaultparam:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	add	r4, r0, #48
	ldr	r5, .L193
	mov	r6, r1
	mov	r1, r4
	ldr	r2, .L193+4
	ldrb	r3, [r5]	@ zero_extendqisi2
	bl	m3_vender_get_param
	mov	r3, r6
.L191:
	ldrb	r1, [r5]	@ zero_extendqisi2
	subs	r2, r3, r6
	cmp	r1, r2
	bgt	.L192
	pop	{r4, r5, r6, pc}
.L192:
	ldrb	r2, [r4], #1	@ zero_extendqisi2
	strb	r2, [r3], #1
	b	.L191
.L194:
	.align	2
.L193:
	.word	.LANCHOR9
	.word	.LANCHOR14
	.size	m3_getdefaultparam, .-m3_getdefaultparam
	.section	.text.m3_setdefaultparam,"ax",%progbits
	.align	1
	.global	m3_setdefaultparam
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m3_setdefaultparam, %function
m3_setdefaultparam:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r4, r5, r6, r7, lr}
	add	r1, r0, #48
	ldr	r5, .L200
	movs	r2, #0
	ldrb	r3, [r5]	@ zero_extendqisi2
.L196:
	cmp	r3, r2
	bgt	.L197
	ldr	r7, .L200+4
	mov	r1, sp
	ldr	r2, .L200+8
	movs	r4, #0
	bl	m3_vender_set_param
	mov	r6, r0
	ldr	r0, .L200+12
	bl	NAND_Print_DBG
.L198:
	ldrb	r3, [r5]	@ zero_extendqisi2
	cmp	r3, r4
	bgt	.L199
	ldr	r0, .L200+16
	bl	NAND_Print_DBG
	mov	r0, r6
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L197:
	ldrb	r4, [r1], #1	@ zero_extendqisi2
	strb	r4, [sp, r2]
	adds	r2, r2, #1
	b	.L196
.L199:
	ldrb	r1, [sp, r4]	@ zero_extendqisi2
	mov	r0, r7
	bl	NAND_Print_DBG
	adds	r4, r4, #1
	b	.L198
.L201:
	.align	2
.L200:
	.word	.LANCHOR9
	.word	.LC25
	.word	.LANCHOR14
	.word	.LC24
	.word	.LC26
	.size	m3_setdefaultparam, .-m3_setdefaultparam
	.section	.text.m3_get_default_param,"ax",%progbits
	.align	1
	.global	m3_get_default_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m3_get_default_param, %function
m3_get_default_param:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	sub	sp, sp, #24
	add	r1, sp, #20
	mov	r4, r0
	bl	m3_getdefaultparam
	ldrb	r2, [sp, #23]	@ zero_extendqisi2
	ldr	r1, [r4, #248]
	ldrb	r3, [sp, #20]	@ zero_extendqisi2
	str	r2, [sp, #8]
	ldrb	r2, [sp, #22]	@ zero_extendqisi2
	ldr	r0, .L203
	str	r2, [sp, #4]
	ldrb	r2, [sp, #21]	@ zero_extendqisi2
	str	r2, [sp]
	ldr	r2, [r4, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	m3_setdefaultparam
	movs	r0, #0
	add	sp, sp, #24
	@ sp needed
	pop	{r4, pc}
.L204:
	.align	2
.L203:
	.word	.LC27
	.size	m3_get_default_param, .-m3_get_default_param
	.section	.text.m3_readretry_init,"ax",%progbits
	.align	1
	.global	m3_readretry_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m3_readretry_init, %function
m3_readretry_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	ldr	r3, [r0, #252]
	ldr	r4, .L220
	ldr	r2, [r3, #40]
	ldr	r3, .L220+4
	ubfx	r1, r2, #16, #8
	strb	r1, [r3]
	lsrs	r3, r2, #8
	strb	r3, [r4]
	ldr	r4, .L220+8
	strb	r2, [r4]
	cbnz	r1, .L206
	ldr	r4, .L220+12
	movs	r5, #172
	uxtb	r3, r3
	ldr	r6, .L220+16
	uxtb	r2, r2
	strb	r5, [r4]
	movs	r5, #173
	strb	r5, [r4, #1]
	movs	r5, #174
	strb	r5, [r4, #2]
	movs	r5, #175
	strb	r5, [r4, #3]
	adds	r3, r3, #1
	ldr	r5, .L220+20
.L207:
	cmp	r3, r1
	bhi	.L217
.L210:
	add	r2, r0, #48
	add	r1, r0, #176
	movs	r3, #0
	str	r3, [r0, #176]
.L216:
	strb	r3, [r2], #1
	cmp	r1, r2
	bne	.L216
	bl	m3_get_default_param
	movs	r0, #0
	b	.L205
.L208:
	ldrh	r7, [r5, r4, lsl #1]
	strh	r7, [r6, r4, lsl #1]	@ movhi
	adds	r4, r4, #1
.L209:
	cmp	r4, r2
	bcc	.L208
	adds	r1, r1, #1
	adds	r5, r5, #8
	adds	r6, r6, #8
	b	.L207
.L217:
	movs	r4, #0
	b	.L209
.L206:
	cmp	r1, #1
	bne	.L211
	ldr	r1, .L220+12
	movs	r4, #167
	uxtb	r3, r3
	ldr	r5, .L220+24
	ldr	r6, .L220+16
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
.L212:
	cmp	r3, r4
	bls	.L210
	movs	r1, #0
	b	.L214
.L213:
	ldrh	r7, [r5, r1, lsl #1]
	strh	r7, [r6, r1, lsl #1]	@ movhi
	adds	r1, r1, #1
.L214:
	cmp	r1, r2
	bcc	.L213
	adds	r4, r4, #1
	adds	r5, r5, #8
	adds	r6, r6, #8
	b	.L212
.L211:
	ldr	r0, .L220+28
	bl	NAND_Print
	mvn	r0, #53
.L205:
	pop	{r3, r4, r5, r6, r7, pc}
.L221:
	.align	2
.L220:
	.word	.LANCHOR8
	.word	.LANCHOR7
	.word	.LANCHOR9
	.word	.LANCHOR14
	.word	.LANCHOR16
	.word	.LANCHOR15
	.word	.LANCHOR17
	.word	.LC28
	.size	m3_readretry_init, .-m3_readretry_init
	.section	.text.m3_readretry_exit,"ax",%progbits
	.align	1
	.global	m3_readretry_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m3_readretry_exit, %function
m3_readretry_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	ldr	r0, .L223
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	m3_setdefaultparam
	movs	r0, #0
	str	r0, [r4, #176]
	pop	{r4, pc}
.L224:
	.align	2
.L223:
	.word	.LC29
	.size	m3_readretry_exit, .-m3_readretry_exit
	.section	.text.m3_set_readretry,"ax",%progbits
	.align	1
	.global	m3_set_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m3_set_readretry, %function
m3_set_readretry:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L234
	push	{r0, r1, r4, r5, r6, r7, r8, lr}
	ldr	r1, [r0, #176]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r1, r3
	bhi	.L233
	ldr	r4, .L234+4
	movs	r6, #0
	ldr	r7, .L234+8
	add	r5, sp, #4
	ldrb	r3, [r4]	@ zero_extendqisi2
	add	r7, r7, r1, lsl #3
.L227:
	cmp	r3, r6
	bhi	.L228
	subs	r2, r1, #2
	subs	r1, r1, #5
	cmp	r2, #4
	ldr	r8, .L234+24
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
	ldr	r2, .L234+12
	bl	m3_vender_set_param
	mov	r7, r0
	ldr	r0, .L234+16
	bl	NAND_Print_DBG
.L231:
	ldrb	r3, [r4]	@ zero_extendqisi2
	cmp	r3, r6
	bhi	.L232
	ldr	r0, .L234+20
	bl	NAND_Print_DBG
.L225:
	mov	r0, r7
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L228:
	adds	r2, r0, r6
	ldrh	ip, [r7, r6, lsl #1]
	ldrb	r2, [r2, #48]	@ zero_extendqisi2
	add	r2, r2, ip
	sxth	r2, r2
	usat	r2, #8, r2
	strb	r2, [r5, r6]
	adds	r6, r6, #1
	b	.L227
.L232:
	ldrb	r1, [r5, r6]	@ zero_extendqisi2
	mov	r0, r8
	bl	NAND_Print_DBG
	adds	r6, r6, #1
	b	.L231
.L233:
	mvn	r7, #52
	b	.L225
.L235:
	.align	2
.L234:
	.word	.LANCHOR8
	.word	.LANCHOR9
	.word	.LANCHOR16
	.word	.LANCHOR14
	.word	.LC30
	.word	.LC26
	.word	.LC31
	.size	m3_set_readretry, .-m3_set_readretry
	.section	.text.m3_read_page_end,"ax",%progbits
	.align	1
	.global	m3_read_page_end
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m3_read_page_end, %function
m3_read_page_end:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L249
	push	{r0, r1, r2, r4, r5, r6, r7, r8, r10, lr}
	mov	r6, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r5, r0
	mov	r0, r6
	bl	m0_read_page_end_not_retry
	adds	r3, r0, #2
	mov	r4, r0
	bne	.L236
	ldr	r7, .L249+4
	mov	r10, #0
	mov	r8, r10
	ldr	r0, .L249+8
	bl	NAND_Print_DBG
.L238:
	ldrb	r2, [r7]	@ zero_extendqisi2
	cmp	r2, r10
	ble	.L242
	ldr	r3, [r5, #176]
	mov	r0, r5
	adds	r3, r3, #1
	cmp	r3, r2
	it	hi
	movhi	r3, r8
	str	r3, [r5, #176]
	bl	m3_set_readretry
	mov	r4, r0
	cbz	r0, .L241
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	m3_set_readretry
.L242:
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	m3_set_readretry
.L236:
	mov	r0, r4
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L241:
	mov	r0, r6
	bl	m0_read_page_start
	mov	r4, r0
	mov	r0, r6
	bl	m0_read_page_end_not_retry
	orrs	r4, r4, r0
	cmp	r4, #10
	beq	.L243
	cbnz	r4, .L244
.L243:
	ldr	r3, [r5, #176]
	movs	r4, #10
	ldr	r1, [r5, #248]
	ldr	r0, .L249+12
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	b	.L242
.L244:
	add	r10, r10, #1
	b	.L238
.L250:
	.align	2
.L249:
	.word	g_nsi
	.word	.LANCHOR8
	.word	.LC32
	.word	.LC33
	.size	m3_read_page_end, .-m3_read_page_end
	.section	.text.m3_special_init,"ax",%progbits
	.align	1
	.global	m3_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m3_special_init, %function
m3_special_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r4, #0
	ldr	r3, .L256
	ldr	r3, [r3]
	ldr	r5, [r3, #8]
.L252:
	cbnz	r5, .L253
	ldr	r3, .L256+4
	ldrb	r1, [r3]	@ zero_extendqisi2
	ldr	r3, .L256+8
	ldrb	r2, [r3]	@ zero_extendqisi2
	ldr	r3, .L256+12
	ldrb	r3, [r3]	@ zero_extendqisi2
	cbnz	r4, .L254
	ldr	r0, .L256+16
	ldr	r5, .L256+20
	str	r5, [r0]
	ldr	r0, .L256+24
	bl	NAND_Print_DBG
.L251:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L253:
	mov	r0, r5
	bl	m3_readretry_init
	ldr	r5, [r5]
	orrs	r4, r4, r0
	b	.L252
.L254:
	ldr	r0, .L256+28
	bl	NAND_Print
	b	.L251
.L257:
	.align	2
.L256:
	.word	g_nsi
	.word	.LANCHOR7
	.word	.LANCHOR8
	.word	.LANCHOR9
	.word	function_read_page_end
	.word	m3_read_page_end
	.word	.LC34
	.word	.LC35
	.size	m3_special_init, .-m3_special_init
	.section	.text.m3_special_exit,"ax",%progbits
	.align	1
	.global	m3_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m3_special_exit, %function
m3_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L261
	push	{r4, lr}
	ldr	r3, [r3]
	ldr	r4, [r3, #8]
.L259:
	cbnz	r4, .L260
	ldr	r0, .L261+4
	bl	NAND_Print_DBG
	mov	r0, r4
	pop	{r4, pc}
.L260:
	mov	r0, r4
	bl	m3_readretry_exit
	ldr	r4, [r4]
	b	.L259
.L262:
	.align	2
.L261:
	.word	g_nsi
	.word	.LC36
	.size	m3_special_exit, .-m3_special_exit
	.section	.text.m7_activation_seq,"ax",%progbits
	.align	1
	.global	m7_activation_seq
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m7_activation_seq, %function
m7_activation_seq:
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	mov	r8, #9
	sub	sp, sp, #40
	mov	r6, r0
	mov	r2, r8
	add	r0, sp, #16
	ldr	r1, .L268
	movs	r4, #0
	bl	memcpy
	mov	r0, r6
	strd	r4, r4, [sp, #28]
	strb	r4, [sp, #36]
	add	r7, sp, #28
	bl	nand_enable_chip
	ldr	r0, [r6, #248]
	bl	ndfc_disable_randomize
	movs	r3, #2
	mov	r2, r4
	strd	r4, r3, [sp, #4]
	mov	r0, r6
	mov	r3, r4
	ldr	r1, .L268+4
	str	r4, [sp]
	bl	set_cmd_with_nand_bus
	movs	r3, #1
	mov	r5, r0
	strd	r7, r3, [sp]
	mov	r2, r4
	str	r8, [sp, #8]
	add	r1, sp, #16
	ldr	r3, .L268+8
	mov	r0, r6
	bl	set_cmd_with_nand_bus
	orrs	r5, r5, r0
	mov	r0, r6
	bl	nand_disable_chip
	cbz	r5, .L263
	mov	r1, r5
	ldr	r0, .L268+12
	bl	NAND_Print
.L263:
	mov	r0, r5
	add	sp, sp, #40
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L269:
	.align	2
.L268:
	.word	.LANCHOR18
	.word	.LANCHOR19
	.word	.LANCHOR20
	.word	.LC37
	.size	m7_activation_seq, .-m7_activation_seq
	.section	.text.m7_readretry_init,"ax",%progbits
	.align	1
	.global	m7_readretry_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m7_readretry_init, %function
m7_readretry_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r2, #0
	ldr	r3, [r0, #252]
	str	r2, [r0, #176]
	ldr	r4, [r3, #40]
	ldr	r2, .L274
	ldr	r3, .L274+4
	ubfx	r1, r4, #16, #8
	lsrs	r5, r4, #8
	strb	r1, [r2]
	subs	r1, r1, #48
	strb	r5, [r3]
	cmp	r1, #1
	ldr	r5, .L274+8
	strb	r4, [r5]
	mov	r5, r2
	mov	r4, r3
	bhi	.L271
	bl	m7_activation_seq
.L271:
	ldrb	r3, [r5]	@ zero_extendqisi2
	subs	r3, r3, #48
	cmp	r3, #2
	bhi	.L273
	ldrb	r1, [r4]	@ zero_extendqisi2
	ldr	r0, .L274+12
	bl	NAND_Print_DBG
	movs	r0, #0
.L270:
	pop	{r3, r4, r5, pc}
.L273:
	mvn	r0, #98
	b	.L270
.L275:
	.align	2
.L274:
	.word	.LANCHOR21
	.word	.LANCHOR22
	.word	.LANCHOR23
	.word	.LC38
	.size	m7_readretry_init, .-m7_readretry_init
	.section	.text.m7_readretry_exit,"ax",%progbits
	.align	1
	.global	m7_readretry_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m7_readretry_exit, %function
m7_readretry_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #0
	bx	lr
	.size	m7_readretry_exit, .-m7_readretry_exit
	.section	.text.m7_read_retry_clock_save,"ax",%progbits
	.align	1
	.global	m7_read_retry_clock_save
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m7_read_retry_clock_save, %function
m7_read_retry_clock_save:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	ldr	r3, .L278
	ldr	r0, [r0, #248]
	ldr	r3, [r3]
	ldr	r2, .L278+4
	ldr	r1, .L278+8
	ldr	r0, [r0, #8]
	blx	r3
	ldr	r0, [r4, #248]
	movs	r2, #20
	ldr	r3, .L278+12
	movs	r1, #10
	ldr	r0, [r0, #8]
	ldr	r3, [r3]
	blx	r3
	movs	r0, #0
	pop	{r4, pc}
.L279:
	.align	2
.L278:
	.word	NAND_GetClk
	.word	.LANCHOR24
	.word	.LANCHOR25
	.word	NAND_SetClk
	.size	m7_read_retry_clock_save, .-m7_read_retry_clock_save
	.section	.text.m7_read_retry_clock_recover,"ax",%progbits
	.align	1
	.global	m7_read_retry_clock_recover
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m7_read_retry_clock_recover, %function
m7_read_retry_clock_recover:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	ldr	r0, [r0, #248]
	ldr	r3, .L281
	ldr	r2, .L281+4
	ldr	r1, .L281+8
	ldr	r3, [r3]
	ldr	r2, [r2]
	ldr	r1, [r1]
	ldr	r0, [r0, #8]
	blx	r3
	movs	r0, #0
	pop	{r3, pc}
.L282:
	.align	2
.L281:
	.word	NAND_SetClk
	.word	.LANCHOR24
	.word	.LANCHOR25
	.size	m7_read_retry_clock_recover, .-m7_read_retry_clock_recover
	.section	.text.m7_end_one_read_retry,"ax",%progbits
	.align	1
	.global	m7_end_one_read_retry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m7_end_one_read_retry, %function
m7_end_one_read_retry:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L290
	push	{r4, r5, lr}
	mov	r4, r0
	sub	sp, sp, #20
	ldrb	r3, [r3]	@ zero_extendqisi2
	subs	r3, r3, #48
	cmp	r3, #1
	bhi	.L284
	bl	nand_enable_chip
	ldr	r0, [r4, #248]
	bl	ndfc_disable_randomize
	ldr	r0, [r4, #248]
	bl	ndfc_is_toogle_interface
	ldr	r5, .L290+4
	cbz	r0, .L285
	ldr	r0, [r4, #248]
	bl	ndfc_set_legacy_interface
	movs	r3, #1
	str	r3, [r5]
.L285:
	movs	r3, #0
	movs	r2, #1
	strd	r3, r2, [sp, #4]
	mov	r0, r4
	str	r3, [sp]
	mov	r2, r3
	ldr	r1, .L290+8
	bl	set_cmd_with_nand_bus
	ldr	r3, [r5]
	cmp	r3, #1
	bne	.L286
	ldr	r0, [r4, #248]
	bl	ndfc_set_toogle_interface
.L286:
	mov	r0, r4
	bl	nand_disable_chip
.L284:
	movs	r0, #0
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, pc}
.L291:
	.align	2
.L290:
	.word	.LANCHOR21
	.word	.LANCHOR26
	.word	.LANCHOR27
	.size	m7_end_one_read_retry, .-m7_end_one_read_retry
	.section	.text.m7_vender_set_param,"ax",%progbits
	.align	1
	.global	m7_vender_set_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m7_vender_set_param, %function
m7_vender_set_param:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, lr}
	mov	r6, r2
	ldr	r2, .L296
	sub	sp, sp, #20
	mov	r5, r0
	mov	r7, r1
	movs	r4, #0
	ldrb	r3, [r2]	@ zero_extendqisi2
	mov	r8, r2
	subs	r3, r3, #48
	cmp	r3, #1
	bhi	.L295
	movs	r3, #2
	mov	r2, r4
	strd	r4, r3, [sp, #4]
	mov	r3, r4
	ldr	r1, .L296+4
	str	r4, [sp]
	bl	set_cmd_with_nand_bus
	movs	r3, #1
	movs	r2, #3
	strd	r3, r2, [sp, #4]
	mov	r10, r0
	str	r7, [sp]
	mov	r3, r6
	mov	r2, r4
	ldr	r1, .L296+8
	mov	r0, r5
	bl	set_cmd_with_nand_bus
	orr	r0, r10, r0
.L293:
	ldrb	r3, [r8]	@ zero_extendqisi2
	cmp	r3, #50
	bne	.L292
	movs	r3, #4
	movs	r2, #1
	strd	r3, r2, [sp, #4]
	mov	r0, r5
	str	r7, [sp]
	mov	r3, r6
	ldr	r1, .L296+12
	bl	set_cmd_with_nand_bus
.L292:
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L295:
	mov	r0, r4
	b	.L293
.L297:
	.align	2
.L296:
	.word	.LANCHOR21
	.word	.LANCHOR19
	.word	.LANCHOR28
	.word	.LANCHOR29
	.size	m7_vender_set_param, .-m7_vender_set_param
	.section	.text.m7_set_readretry,"ax",%progbits
	.align	1
	.global	m7_set_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m7_set_readretry, %function
m7_set_readretry:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r4, r0
	mov	r6, r1
	ldr	r5, .L323
	bl	m7_read_retry_clock_save
	mov	r0, r4
	bl	nand_enable_chip
	ldr	r0, [r4, #248]
	bl	ndfc_disable_randomize
	ldr	r0, [r4, #248]
	bl	ndfc_is_toogle_interface
	cbz	r0, .L299
	ldr	r0, [r4, #248]
	bl	ndfc_set_legacy_interface
	movs	r3, #1
	str	r3, [r5]
.L299:
	ldr	r3, .L323+4
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #48
	bne	.L300
	cmp	r6, #255
	ldr	r3, [r4, #176]
	ldr	r2, .L323+8
	beq	.L301
	cbz	r6, .L302
	lsls	r1, r6, #31
	bpl	.L301
.L302:
	ldr	r1, .L323+12
	add	r3, r3, r3, lsl #1
	add	r1, r1, r3
	movs	r3, #16
.L320:
	strb	r3, [r2]
	movs	r3, #3
	ldr	r2, .L323+16
.L322:
	mov	r0, r4
	bl	m7_vender_set_param
	movs	r2, #0
	mov	r6, r0
	movs	r1, #182
.L321:
	mov	r0, r4
	bl	set_one_cmd
.L304:
	ldr	r3, [r5]
	cmp	r3, #1
	bne	.L306
	ldr	r0, [r4, #248]
	bl	ndfc_set_toogle_interface
.L306:
	mov	r0, r4
	bl	nand_disable_chip
	mov	r0, r4
	bl	m7_read_retry_clock_recover
	mov	r0, r6
	pop	{r4, r5, r6, pc}
.L301:
	ldr	r1, .L323+20
	add	r3, r3, r3, lsl #1
	add	r1, r1, r3
	movs	r3, #20
	b	.L320
.L300:
	cmp	r3, #49
	bne	.L305
	ldr	r1, [r4, #176]
	movs	r3, #3
	ldr	r0, .L323+24
	ldr	r2, .L323+28
	add	r1, r1, r1, lsl #1
	add	r1, r1, r0
	b	.L322
.L305:
	cmp	r3, #50
	bne	.L307
	ldr	r0, [r4, #176]
	movs	r3, #1
	ldr	r1, .L323+32
	ldr	r2, .L323+36
	add	r1, r1, r0, lsl #2
	mov	r0, r4
	bl	m7_vender_set_param
	movs	r2, #0
	mov	r6, r0
	movs	r1, #93
	b	.L321
.L307:
	movs	r6, #0
	b	.L304
.L324:
	.align	2
.L323:
	.word	.LANCHOR26
	.word	.LANCHOR21
	.word	.LANCHOR22
	.word	.LANCHOR30
	.word	.LANCHOR32
	.word	.LANCHOR31
	.word	.LANCHOR34
	.word	.LANCHOR33
	.word	.LANCHOR36
	.word	.LANCHOR35
	.size	m7_set_readretry, .-m7_set_readretry
	.section	.text.m7_exit_readretry,"ax",%progbits
	.align	1
	.global	m7_exit_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m7_exit_readretry, %function
m7_exit_readretry:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r4, r5, r6, lr}
	movs	r3, #0
	mov	r4, r0
	str	r3, [sp, #4]
	bl	nand_enable_chip
	ldr	r0, [r4, #248]
	bl	ndfc_disable_randomize
	ldr	r0, [r4, #248]
	bl	ndfc_is_toogle_interface
	ldr	r6, .L336
	cbz	r0, .L326
	ldr	r0, [r4, #248]
	bl	ndfc_set_legacy_interface
	movs	r3, #1
	str	r3, [r6]
.L326:
	ldr	r3, .L336+4
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #48
	bne	.L327
	ldr	r2, .L336+8
	movs	r3, #3
.L335:
	add	r1, sp, #4
	mov	r0, r4
	bl	m7_vender_set_param
	mov	r5, r0
.L328:
	ldr	r3, [r6]
	cmp	r3, #1
	bne	.L330
	ldr	r0, [r4, #248]
	bl	ndfc_set_toogle_interface
.L330:
	mov	r0, r4
	bl	nand_disable_chip
	mov	r0, r5
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, pc}
.L327:
	cmp	r3, #49
	bne	.L329
	movs	r3, #3
	ldr	r2, .L336+12
	b	.L335
.L329:
	cmp	r3, #50
	bne	.L331
	movs	r2, #1
	movs	r1, #255
	mov	r0, r4
	bl	set_one_cmd
	movs	r3, #1
	ldr	r2, .L336+16
	b	.L335
.L331:
	movs	r5, #0
	b	.L328
.L337:
	.align	2
.L336:
	.word	.LANCHOR26
	.word	.LANCHOR21
	.word	.LANCHOR32
	.word	.LANCHOR33
	.word	.LANCHOR35
	.size	m7_exit_readretry, .-m7_exit_readretry
	.section	.text.m7_special_init,"ax",%progbits
	.align	1
	.global	m7_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m7_special_init, %function
m7_special_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r4, #0
	ldr	r3, .L343
	ldr	r3, [r3]
	ldr	r5, [r3, #8]
.L339:
	cbnz	r5, .L340
	ldr	r3, .L343+4
	ldrb	r1, [r3]	@ zero_extendqisi2
	ldr	r3, .L343+8
	ldrb	r2, [r3]	@ zero_extendqisi2
	ldr	r3, .L343+12
	ldrb	r3, [r3]	@ zero_extendqisi2
	cbnz	r4, .L341
	ldr	r0, .L343+16
	ldr	r5, .L343+20
	str	r5, [r0]
	ldr	r0, .L343+24
	bl	NAND_Print_DBG
.L338:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L340:
	mov	r0, r5
	bl	m7_readretry_init
	ldr	r5, [r5]
	orrs	r4, r4, r0
	b	.L339
.L341:
	ldr	r0, .L343+28
	bl	NAND_Print
	b	.L338
.L344:
	.align	2
.L343:
	.word	g_nsi
	.word	.LANCHOR21
	.word	.LANCHOR22
	.word	.LANCHOR23
	.word	function_read_page_end
	.word	m7_read_page_end
	.word	.LC39
	.word	.LC40
	.size	m7_special_init, .-m7_special_init
	.section	.text.m7_special_exit,"ax",%progbits
	.align	1
	.global	m7_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m7_special_exit, %function
m7_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L348
	push	{r4, lr}
	ldr	r3, [r3]
	ldr	r4, [r3, #8]
.L346:
	cbnz	r4, .L347
	ldr	r0, .L348+4
	bl	NAND_Print_DBG
	mov	r0, r4
	pop	{r4, pc}
.L347:
	ldr	r4, [r4]
	b	.L346
.L349:
	.align	2
.L348:
	.word	g_nsi
	.word	.LC41
	.size	m7_special_exit, .-m7_special_exit
	.section	.text.m7_is_lsb_page,"ax",%progbits
	.align	1
	.global	m7_is_lsb_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m7_is_lsb_page, %function
m7_is_lsb_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	b	m5_is_lsb_page
	.size	m7_is_lsb_page, .-m7_is_lsb_page
	.section	.text.m6_readretry_init,"ax",%progbits
	.align	1
	.global	m6_readretry_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m6_readretry_init, %function
m6_readretry_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #252]
	ldr	r2, .L352
	push	{r4, lr}
	movs	r4, #0
	ldr	r3, [r3, #40]
	str	r4, [r0, #176]
	ldr	r0, .L352+4
	lsrs	r1, r3, #16
	strb	r1, [r2]
	lsrs	r1, r3, #8
	ldr	r2, .L352+8
	strb	r1, [r2]
	uxtb	r1, r1
	ldr	r2, .L352+12
	strb	r3, [r2]
	bl	NAND_Print_DBG
	mov	r0, r4
	pop	{r4, pc}
.L353:
	.align	2
.L352:
	.word	.LANCHOR37
	.word	.LC42
	.word	.LANCHOR38
	.word	.LANCHOR39
	.size	m6_readretry_init, .-m6_readretry_init
	.section	.text.m6_readretry_exit,"ax",%progbits
	.align	1
	.global	m6_readretry_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m6_readretry_exit, %function
m6_readretry_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #0
	bx	lr
	.size	m6_readretry_exit, .-m6_readretry_exit
	.section	.text.m6_vender_pre_condition,"ax",%progbits
	.align	1
	.global	m6_vender_pre_condition
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m6_vender_pre_condition, %function
m6_vender_pre_condition:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r3, r4, lr}
	movs	r3, #0
	movs	r2, #2
	str	r3, [sp]
	strd	r3, r2, [sp, #4]
	mov	r2, r3
	ldr	r1, .L360
	bl	set_cmd_with_nand_bus
	mov	r4, r0
	cbz	r0, .L355
	ldr	r0, .L360+4
	bl	NAND_Print
.L355:
	mov	r0, r4
	add	sp, sp, #16
	@ sp needed
	pop	{r4, pc}
.L361:
	.align	2
.L360:
	.word	.LANCHOR40
	.word	.LC43
	.size	m6_vender_pre_condition, .-m6_vender_pre_condition
	.section	.text.m6_read_retry_clock_save,"ax",%progbits
	.align	1
	.global	m6_read_retry_clock_save
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m6_read_retry_clock_save, %function
m6_read_retry_clock_save:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	ldr	r3, .L363
	ldr	r0, [r0, #248]
	ldr	r3, [r3]
	ldr	r2, .L363+4
	ldr	r1, .L363+8
	ldr	r0, [r0, #8]
	blx	r3
	ldr	r0, [r4, #248]
	movs	r2, #20
	ldr	r3, .L363+12
	movs	r1, #10
	ldr	r0, [r0, #8]
	ldr	r3, [r3]
	blx	r3
	movs	r0, #0
	pop	{r4, pc}
.L364:
	.align	2
.L363:
	.word	NAND_GetClk
	.word	.LANCHOR41
	.word	.LANCHOR42
	.word	NAND_SetClk
	.size	m6_read_retry_clock_save, .-m6_read_retry_clock_save
	.section	.text.m6_read_retry_clock_recover,"ax",%progbits
	.align	1
	.global	m6_read_retry_clock_recover
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m6_read_retry_clock_recover, %function
m6_read_retry_clock_recover:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	ldr	r0, [r0, #248]
	ldr	r3, .L366
	ldr	r2, .L366+4
	ldr	r1, .L366+8
	ldr	r3, [r3]
	ldr	r2, [r2]
	ldr	r1, [r1]
	ldr	r0, [r0, #8]
	blx	r3
	movs	r0, #0
	pop	{r3, pc}
.L367:
	.align	2
.L366:
	.word	NAND_SetClk
	.word	.LANCHOR41
	.word	.LANCHOR42
	.size	m6_read_retry_clock_recover, .-m6_read_retry_clock_recover
	.section	.text.m6_vender_set_param,"ax",%progbits
	.align	1
	.global	m6_vender_set_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m6_vender_set_param, %function
m6_vender_set_param:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r10, r3
	ldr	r7, .L377
	mov	r6, r0
	mov	r4, r1
	mov	r8, r2
	ldrb	r3, [r7]	@ zero_extendqisi2
	cmp	r3, #18
	bne	.L375
	mov	fp, #1
	mov	r3, r2
	strd	r1, fp, [sp]
	movs	r2, #0
	str	r10, [sp, #8]
	ldr	r1, .L377+4
	bl	set_cmd_with_nand_bus
	ldrb	r3, [r4]	@ zero_extendqisi2
	mov	r5, r0
	cbnz	r3, .L369
	ldrb	r3, [r4, #1]	@ zero_extendqisi2
	cbnz	r3, .L369
	ldrb	r3, [r4, #2]	@ zero_extendqisi2
	cbnz	r3, .L369
	ldrb	r3, [r4, #3]	@ zero_extendqisi2
	cbnz	r3, .L369
	ldr	r3, .L377+8
	ldr	r3, [r3]
	cbnz	r3, .L369
.L376:
	ldr	r3, .L377+12
	movs	r2, #1
	mov	r0, r6
	ldrb	r1, [r3]	@ zero_extendqisi2
	bl	set_one_cmd
	orrs	r5, r5, r0
.L368:
	mov	r0, r5
	add	sp, sp, #16
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L375:
	movs	r5, #0
.L369:
	ldrb	r3, [r7]	@ zero_extendqisi2
	cmp	r3, #17
	bne	.L371
	mov	fp, #1
	mov	r3, r8
	str	r10, [sp, #8]
	movs	r2, #0
	strd	r4, fp, [sp]
	mov	r0, r6
	ldr	r1, .L377+4
	bl	set_cmd_with_nand_bus
	ldrb	r3, [r4]	@ zero_extendqisi2
	orrs	r5, r5, r0
	cmp	r3, #0
	bne	.L372
	ldrb	r3, [r4, #1]	@ zero_extendqisi2
	cbnz	r3, .L371
	ldrb	r3, [r4, #2]	@ zero_extendqisi2
	cbnz	r3, .L371
	ldrb	r3, [r4, #3]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L376
.L371:
	ldrb	r3, [r7]	@ zero_extendqisi2
	cmp	r3, #16
	bne	.L373
	ldrb	r3, [r4]	@ zero_extendqisi2
	cmp	r3, #255
	bne	.L374
	ldrb	r3, [r4, #1]	@ zero_extendqisi2
	cmp	r3, #255
	bne	.L374
	ldrb	r3, [r4, #2]	@ zero_extendqisi2
	cmp	r3, #255
	bne	.L374
	ldrb	r3, [r4, #3]	@ zero_extendqisi2
	cmp	r3, #255
	beq	.L376
.L374:
	movs	r3, #1
	str	r10, [sp, #8]
	strd	r4, r3, [sp]
	movs	r2, #0
	mov	r3, r8
	ldr	r1, .L377+16
	mov	r0, r6
	bl	set_cmd_with_nand_bus
	orrs	r5, r5, r0
.L373:
	ldr	r7, .L377+20
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
	ldr	r0, .L377+24
	bl	NAND_Print_DBG
	b	.L368
.L372:
	cmp	r3, #2
	bne	.L371
	ldrb	r2, [r4, #1]	@ zero_extendqisi2
	cmp	r2, #0
	bne	.L371
	ldrb	r3, [r4, #2]	@ zero_extendqisi2
	cmp	r3, #126
	bne	.L371
	ldrb	r3, [r4, #3]	@ zero_extendqisi2
	cmp	r3, #124
	bne	.L371
	ldr	r3, .L377+28
	mov	r0, r6
	ldrb	r1, [r3]	@ zero_extendqisi2
	bl	set_one_cmd
	orrs	r5, r5, r0
	b	.L371
.L378:
	.align	2
.L377:
	.word	.LANCHOR37
	.word	.LANCHOR43
	.word	.LANCHOR44
	.word	.LANCHOR45
	.word	.LANCHOR47
	.word	.LANCHOR48
	.word	.LC44
	.word	.LANCHOR46
	.size	m6_vender_set_param, .-m6_vender_set_param
	.section	.text.m6_set_readretry,"ax",%progbits
	.align	1
	.global	m6_set_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m6_set_readretry, %function
m6_set_readretry:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r4, r0
	ldr	r6, .L392
	ldrb	r2, [r6]	@ zero_extendqisi2
	subs	r2, r2, #16
	cmp	r2, #2
	bhi	.L387
	bl	m6_read_retry_clock_save
	mov	r0, r4
	bl	nand_enable_chip
	ldr	r0, [r4, #248]
	bl	ndfc_disable_randomize
	ldr	r3, [r4, #176]
	cbnz	r3, .L381
	mov	r0, r4
	bl	m6_vender_pre_condition
.L381:
	ldr	r0, [r4, #248]
	bl	ndfc_is_toogle_interface
	ldr	r5, .L392+4
	cbz	r0, .L382
	ldr	r0, [r4, #248]
	bl	ndfc_set_legacy_interface
	movs	r3, #1
	str	r3, [r5]
.L382:
	ldrb	r2, [r6]	@ zero_extendqisi2
	ldr	r1, [r4, #176]
	cmp	r2, #17
	lsl	r3, r1, #2
	bne	.L383
	add	r3, r3, r1
	ldr	r1, .L392+8
.L391:
	ldr	r2, .L392+12
	add	r1, r1, r3
	movs	r3, #5
.L384:
	mov	r0, r4
	bl	m6_vender_set_param
	ldr	r3, [r5]
	mov	r6, r0
	cmp	r3, #1
	bne	.L386
	ldr	r0, [r4, #248]
	bl	ndfc_set_toogle_interface
.L386:
	mov	r0, r4
	bl	nand_disable_chip
	mov	r0, r4
	bl	m6_read_retry_clock_recover
.L379:
	mov	r0, r6
	pop	{r4, r5, r6, pc}
.L383:
	cmp	r2, #18
	bne	.L385
	add	r3, r3, r1
	ldr	r1, .L392+16
	b	.L391
.L385:
	ldr	r1, .L392+20
	ldr	r2, .L392+24
	add	r1, r1, r3
	movs	r3, #4
	b	.L384
.L387:
	mvn	r6, #88
	b	.L379
.L393:
	.align	2
.L392:
	.word	.LANCHOR37
	.word	.LANCHOR51
	.word	.LANCHOR52
	.word	.LANCHOR49
	.word	.LANCHOR53
	.word	.LANCHOR54
	.word	.LANCHOR50
	.size	m6_set_readretry, .-m6_set_readretry
	.section	.text.m6_exit_readretry,"ax",%progbits
	.align	1
	.global	m6_exit_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m6_exit_readretry, %function
m6_exit_readretry:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r4, r0
	ldr	r5, .L405
	ldrb	r2, [r5]	@ zero_extendqisi2
	subs	r2, r2, #16
	cmp	r2, #2
	bhi	.L399
	bl	nand_enable_chip
	ldr	r0, [r4, #248]
	bl	ndfc_disable_randomize
	ldr	r0, [r4, #248]
	bl	ndfc_is_toogle_interface
	ldr	r6, .L405+4
	cbz	r0, .L396
	ldr	r0, [r4, #248]
	bl	ndfc_set_legacy_interface
	movs	r3, #1
	str	r3, [r6]
.L396:
	ldrb	r3, [r5]	@ zero_extendqisi2
	cmp	r3, #17
	beq	.L400
	ldr	r1, .L405+8
	cmp	r3, #18
	ldr	r0, .L405+12
	it	eq
	moveq	r3, #5
	ldr	r2, .L405+16
	itt	ne
	movne	r3, #0
	movne	r2, r1
	ldr	r1, .L405+20
	it	ne
	movne	r1, r0
.L397:
	mov	r0, r4
	bl	m6_vender_set_param
	ldr	r3, [r6]
	mov	r5, r0
	cmp	r3, #1
	bne	.L398
	ldr	r0, [r4, #248]
	bl	ndfc_set_toogle_interface
.L398:
	mov	r0, r4
	bl	nand_disable_chip
.L394:
	mov	r0, r5
	pop	{r4, r5, r6, pc}
.L400:
	movs	r3, #5
	ldr	r2, .L405+16
	ldr	r1, .L405+24
	b	.L397
.L399:
	mvn	r5, #87
	b	.L394
.L406:
	.align	2
.L405:
	.word	.LANCHOR37
	.word	.LANCHOR51
	.word	.LANCHOR50
	.word	.LANCHOR54+24
	.word	.LANCHOR49
	.word	.LANCHOR53+50
	.word	.LANCHOR52+35
	.size	m6_exit_readretry, .-m6_exit_readretry
	.section	.text.m6_read_page_end,"ax",%progbits
	.align	1
	.global	m6_read_page_end
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m6_read_page_end, %function
m6_read_page_end:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L417
	push	{r0, r1, r2, r4, r5, r6, r7, r8, r10, lr}
	mov	r6, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r5, r0
	mov	r0, r6
	bl	m0_read_page_end_not_retry
	adds	r3, r0, #2
	mov	r4, r0
	bne	.L407
	ldr	r0, .L417+4
	movs	r7, #0
	bl	NAND_Print_DBG
	ldr	r3, .L417+8
	movs	r2, #1
	ldr	r10, .L417+16
	mov	r8, r3
	str	r2, [r3]
.L409:
	ldrb	r3, [r10]	@ zero_extendqisi2
	cmp	r3, r7
	ble	.L412
	str	r7, [r5, #176]
	mov	r0, r5
	bl	m6_set_readretry
	mov	r4, r0
	cbnz	r0, .L410
	mov	r0, r6
	bl	m0_read_page_start
	mov	r4, r0
	mov	r0, r6
	bl	m0_read_page_end_not_retry
	orrs	r4, r4, r0
	cmp	r4, #10
	beq	.L411
	cbnz	r4, .L410
.L411:
	ldr	r3, [r5, #176]
	movs	r4, #10
	ldr	r1, [r5, #248]
	ldr	r0, .L417+12
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
.L412:
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	str	r3, [r8]
	bl	m6_exit_readretry
.L407:
	mov	r0, r4
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L410:
	adds	r7, r7, #1
	b	.L409
.L418:
	.align	2
.L417:
	.word	g_nsi
	.word	.LC45
	.word	.LANCHOR44
	.word	.LC46
	.word	.LANCHOR38
	.size	m6_read_page_end, .-m6_read_page_end
	.section	.text.m6_special_init,"ax",%progbits
	.align	1
	.global	m6_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m6_special_init, %function
m6_special_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r4, #0
	ldr	r3, .L424
	ldr	r3, [r3]
	ldr	r5, [r3, #8]
.L420:
	cbnz	r5, .L421
	ldr	r3, .L424+4
	ldrb	r1, [r3]	@ zero_extendqisi2
	ldr	r3, .L424+8
	ldrb	r2, [r3]	@ zero_extendqisi2
	ldr	r3, .L424+12
	ldrb	r3, [r3]	@ zero_extendqisi2
	cbnz	r4, .L422
	ldr	r0, .L424+16
	ldr	r5, .L424+20
	str	r5, [r0]
	ldr	r0, .L424+24
	bl	NAND_Print_DBG
.L419:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L421:
	mov	r0, r5
	bl	m6_readretry_init
	ldr	r5, [r5]
	orrs	r4, r4, r0
	b	.L420
.L422:
	ldr	r0, .L424+28
	bl	NAND_Print
	b	.L419
.L425:
	.align	2
.L424:
	.word	g_nsi
	.word	.LANCHOR37
	.word	.LANCHOR38
	.word	.LANCHOR39
	.word	function_read_page_end
	.word	m6_read_page_end
	.word	.LC47
	.word	.LC48
	.size	m6_special_init, .-m6_special_init
	.section	.text.m6_special_exit,"ax",%progbits
	.align	1
	.global	m6_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m6_special_exit, %function
m6_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L429
	push	{r4, lr}
	ldr	r3, [r3]
	ldr	r4, [r3, #8]
.L427:
	cbnz	r4, .L428
	ldr	r0, .L429+4
	bl	NAND_Print_DBG
	mov	r0, r4
	pop	{r4, pc}
.L428:
	ldr	r4, [r4]
	b	.L427
.L430:
	.align	2
.L429:
	.word	g_nsi
	.word	.LC49
	.size	m6_special_exit, .-m6_special_exit
	.section	.text.m6_is_lsb_page,"ax",%progbits
	.align	1
	.global	m6_is_lsb_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m6_is_lsb_page, %function
m6_is_lsb_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	b	m5_is_lsb_page
	.size	m6_is_lsb_page, .-m6_is_lsb_page
	.section	.text.m1_write_page_FF,"ax",%progbits
	.align	1
	.global	m1_write_page_FF
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_write_page_FF, %function
m1_write_page_FF:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L451
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r10, r0
	sub	sp, sp, #24
	mov	r6, r1
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	ldr	r4, [r0, #248]
	mov	r5, r0
	ldr	r2, [r0, #20]
	ldr	r1, [r10, #4]
	ldr	r3, [r4, #12]
	add	r7, r4, #28
	ldr	ip, [r0, #24]
	cmp	r2, r3
	bcs	.L433
	cmp	r1, ip
	bcc	.L434
.L433:
	strd	r1, ip, [sp]
	ldr	r1, [r4, #8]
	mvn	r4, #107
	ldr	r0, .L451+4
	bl	NAND_Print
.L432:
	mov	r0, r4
	add	sp, sp, #24
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L434:
	bl	nand_read_chip_status_ready
	mov	r0, r5
	bl	nand_enable_chip
	ldr	r0, [r5, #248]
	bl	ndfc_disable_randomize
	ldr	r0, [r5, #248]
	bl	ndfc_disable_ecc
	mov	r0, r7
	bl	ndfc_clean_cmd_seq
	cmp	r6, #8
	beq	.L442
	cmp	r6, #16
	ite	eq
	moveq	r3, #640
	movne	r3, #0
	str	r3, [sp, #16]
	ite	eq
	moveq	r3, #18
	movne	r3, #0
.L448:
	str	r3, [sp, #8]
	add	r3, r4, #56
	str	r3, [sp, #12]
	mov	r8, #0
	ldr	r3, [sp, #8]
	mov	r6, r8
	subs	r3, r3, #1
	str	r3, [sp, #20]
.L440:
	lsl	fp, r6, #10
	ldr	r1, [r10, #16]
	cmp	r6, #0
	bne	.L437
	movs	r3, #1
	movs	r0, #128
	mov	r2, #1024
	strd	r3, r0, [r4, #40]
	str	r3, [r4, #48]
	str	r3, [r4, #88]
	strd	r3, r3, [r4, #80]
	str	r1, [r4, #96]
	str	r2, [r4, #92]
	str	r3, [r4, #76]
	ldrd	r1, r2, [r10, #4]
	str	r6, [r4, #28]
	ldr	r0, [r5, #36]
	bl	get_row_addr
	movs	r3, #5
	mov	r8, r0
	str	r3, [r4, #72]
	mov	r2, r0
	ldr	r3, [sp, #12]
	movs	r1, #2
	str	r3, [sp]
	movs	r3, #3
.L449:
	mov	r0, fp
	adds	r6, r6, #1
	bl	fill_cmd_addr
	mov	r1, r7
	ldr	r0, [r5, #248]
	bl	ndfc_execute_cmd
	ldr	r3, [sp, #8]
	cmp	r3, r6
	bne	.L440
	movs	r3, #0
	movs	r1, #16
	str	r3, [r4, #28]
	movs	r3, #1
	strd	r3, r1, [r4, #40]
	mov	r1, r7
	str	r3, [r4, #48]
	str	r3, [r4, #112]
	ldr	r0, [r5, #248]
	bl	ndfc_execute_cmd
	mov	r0, r5
	bl	nand_read_chip_status_ready
	mov	r4, r0
	cbz	r0, .L441
	ldr	r0, .L451+8
	bl	NAND_Print
.L441:
	mov	r0, r5
	bl	nand_disable_chip
	b	.L432
.L442:
	mov	r3, #832
	str	r3, [sp, #16]
	movs	r3, #9
	b	.L448
.L437:
	ldr	r3, [sp, #20]
	add	r1, r1, fp
	movs	r2, #133
	str	r1, [r4, #96]
	str	r2, [r4, #44]
	movs	r2, #1
	cmp	r3, r6
	str	r2, [r4, #40]
	ite	eq
	ldreq	r1, [sp, #16]
	movne	r1, #1024
	str	r2, [r4, #48]
	movs	r3, #0
	str	r2, [r4, #88]
	strd	r2, r2, [r4, #80]
	str	r2, [r4, #76]
	ldr	r2, [sp, #12]
	str	r1, [r4, #92]
	movs	r1, #2
	str	r3, [r4, #28]
	str	r1, [r4, #72]
	str	r2, [sp]
	mov	r2, r8
	b	.L449
.L452:
	.align	2
.L451:
	.word	g_nsi
	.word	.LC50
	.word	.LC51
	.size	m1_write_page_FF, .-m1_write_page_FF
	.section	.text.m1_vender_get_param,"ax",%progbits
	.align	1
	.global	m1_vender_get_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_vender_get_param, %function
m1_vender_get_param:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	b	m3_vender_get_param
	.size	m1_vender_get_param, .-m1_vender_get_param
	.section	.text.m1_vender_set_param,"ax",%progbits
	.align	1
	.global	m1_vender_set_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_vender_set_param, %function
m1_vender_set_param:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	b	m3_vender_set_param
	.size	m1_vender_set_param, .-m1_vender_set_param
	.section	.text.m1_major_check_byte,"ax",%progbits
	.align	1
	.global	m1_major_check_byte
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_major_check_byte, %function
m1_major_check_byte:
	@ args = 8, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r10, r0
	ldr	r8, [sp, #44]
	mov	r5, r2
	mov	fp, r3
	str	r1, [sp, #4]
	cmp	r2, r8, lsr #1
	bcs	.L474
	mov	r2, r8
	mov	r1, r5
	ldr	r0, .L477
	bl	NAND_Print
	mvn	r0, #106
	movs	r3, #255
	strb	r3, [r10]
.L455:
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L460:
	ldrb	lr, [r0], #1	@ zero_extendqisi2
	tst	lr, r1
	ite	ne
	addne	r2, r2, #1
	addeq	r3, r3, #1
.L471:
	cmp	ip, r0
	bne	.L460
	cmp	r5, r2
	bcs	.L461
.L476:
	orrs	r7, r7, r1
	uxtb	r7, r7
.L462:
	adds	r6, r6, #1
.L469:
	adds	r4, r4, #1
	cmp	r4, #8
	bne	.L456
	cmp	r6, #8
	iteet	ne
	movne	r3, #255
	strbeq	r7, [r10]
	moveq	r0, #0
	strbne	r3, [r10]
	it	ne
	mvnne	r0, #79
	b	.L455
.L467:
	ldrb	lr, [r0], #1	@ zero_extendqisi2
	tst	lr, r1
	ite	ne
	addne	r3, r3, #1
	addeq	r2, r2, #1
	b	.L464
.L474:
	movs	r6, #0
	mov	r7, r6
	mov	r4, r6
.L456:
	movs	r1, #1
	movs	r3, #0
	lsls	r1, r1, r4
	mov	r0, fp
	add	ip, r8, fp
	mov	r2, r3
	b	.L471
.L461:
	cmp	r5, r3
	bcc	.L462
	ldr	r0, [sp, #4]
	cmp	r0, #1
	bne	.L463
	ldr	r0, [sp, #40]
	movs	r3, #0
	mov	r2, r3
	add	ip, r8, r0
.L464:
	cmp	r0, ip
	bne	.L467
	cmp	r5, r2
	bcc	.L476
	cmp	r5, r3
	bcc	.L462
.L463:
	mov	r1, r4
	ldr	r0, .L477+4
	bl	NAND_Print
	mov	r1, r4
	ldr	r0, .L477+8
	bl	NAND_Print
	b	.L469
.L478:
	.align	2
.L477:
	.word	.LC52
	.word	.LC53
	.word	.LC54
	.size	m1_major_check_byte, .-m1_major_check_byte
	.section	.text.m1_get_read_retry_cfg,"ax",%progbits
	.align	1
	.global	m1_get_read_retry_cfg
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_get_read_retry_cfg, %function
m1_get_read_retry_cfg:
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
.L480:
	ldrb	r2, [r10, r3]	@ zero_extendqisi2
	strb	r2, [r4, r3]
	adds	r3, r3, #1
	cmp	r3, #8
	bne	.L480
	strd	r7, r3, [sp]
	movs	r2, #4
	mov	r3, r4
	movs	r1, #0
	mov	r0, fp
	bl	m1_major_check_byte
	subs	r6, r0, #0
	bge	.L481
	ldr	r0, .L502
.L501:
	bl	NAND_Print
.L479:
	mov	r0, r6
	add	sp, sp, #72
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L481:
	ldrb	r1, [fp]	@ zero_extendqisi2
	ldr	r0, .L502+4
	bl	NAND_Print_DBG
	add	r2, r10, #7
	movs	r3, #0
.L483:
	ldrb	r1, [r2, #1]!	@ zero_extendqisi2
	strb	r1, [r4, r3]
	adds	r3, r3, #1
	cmp	r3, #8
	bne	.L483
	strd	r7, r3, [sp]
	movs	r2, #4
	mov	r3, r4
	movs	r1, #0
	mov	r0, r5
	bl	m1_major_check_byte
	subs	r6, r0, #0
	bge	.L484
	ldr	r0, .L502+8
	b	.L501
.L484:
	ldrb	r1, [r5]	@ zero_extendqisi2
	ldr	r0, .L502+12
	bl	NAND_Print_DBG
	ldrb	r1, [fp]	@ zero_extendqisi2
	ldrb	r2, [r5]	@ zero_extendqisi2
	cmp	r1, #8
	bne	.L485
	cmp	r2, #4
	beq	.L492
.L485:
	ldr	r0, .L502+16
	mvn	r6, #80
	bl	NAND_Print
	b	.L479
.L488:
	adds	r5, r5, #1
	cmp	r5, #32
	bne	.L486
	movs	r6, #0
	b	.L489
.L492:
	movs	r5, #0
.L486:
	add	r2, r10, r5
	movs	r3, #0
.L487:
	ldrb	r1, [r2, #16]	@ zero_extendqisi2
	adds	r2, r2, #64
	strb	r1, [r4, r3]
	ldrb	r1, [r2, #-16]	@ zero_extendqisi2
	strb	r1, [r7, r3]
	adds	r3, r3, #1
	cmp	r3, #8
	bne	.L487
	strd	r7, r3, [sp]
	movs	r2, #4
	mov	r3, r4
	movs	r1, #1
	add	r0, r8, r5
	bl	m1_major_check_byte
	subs	r6, r0, #0
	bge	.L488
	mov	r1, r5
	ldr	r0, .L502+20
	bl	NAND_Print
.L489:
	ldr	r7, .L502+24
	movs	r4, #0
	ldr	r5, .L502+28
.L491:
	lsls	r3, r4, #29
	bne	.L490
	cbz	r4, .L490
	mov	r0, r7
	bl	NAND_Print_DBG
.L490:
	ldrb	r1, [r8, r4]	@ zero_extendqisi2
	mov	r0, r5
	adds	r4, r4, #1
	bl	NAND_Print_DBG
	cmp	r4, #32
	bne	.L491
	ldr	r0, .L502+24
	bl	NAND_Print_DBG
	b	.L479
.L503:
	.align	2
.L502:
	.word	.LC55
	.word	.LC56
	.word	.LC57
	.word	.LC58
	.word	.LC59
	.word	.LC60
	.word	.LC26
	.word	.LC61
	.size	m1_get_read_retry_cfg, .-m1_get_read_retry_cfg
	.section	.text.m1_read_otp_info_hynix,"ax",%progbits
	.align	1
	.global	m1_read_otp_info_hynix
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_read_otp_info_hynix, %function
m1_read_otp_info_hynix:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	mov	r4, r0
	sub	sp, sp, #32
	mov	r6, r1
	bl	nand_enable_chip
	ldr	r0, [r4, #248]
	bl	ndfc_disable_randomize
	ldr	r2, [r4, #20]
	ldr	r3, [r4, #248]
	ldr	r0, .L511
	add	r3, r3, r2, lsl #2
	ldr	r2, [r3, #1044]
	ldr	r1, [r3, #1012]
	bl	NAND_Print_DBG
	cbnz	r6, .L505
	mov	r0, r4
	bl	nand_disable_chip
	ldr	r0, .L511+4
	bl	NAND_Print
	mvn	r0, #78
.L504:
	add	sp, sp, #32
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L505:
	movs	r2, #1
	movs	r1, #255
	mov	r0, r4
	movs	r5, #1
	bl	set_one_cmd
	movs	r3, #54
	add	r7, sp, #20
	strb	r3, [sp, #16]
	movs	r3, #56
	strb	r3, [sp, #24]
	add	r1, sp, #16
	ldr	r3, .L511+8
	mov	r0, r4
	ldrb	r2, [r3]	@ zero_extendqisi2
	mov	r8, r3
	strd	r5, r5, [sp, #4]
	str	r7, [sp]
	cmp	r2, #6
	mov	r2, #0
	itt	eq
	moveq	r3, #14
	strbeq	r3, [sp, #24]
	movs	r3, #82
	strb	r3, [sp, #20]
	add	r3, sp, #24
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
	strb	r3, [sp, #16]
	mov	r3, #528
	str	r3, [sp]
	add	r1, sp, #16
	mov	r3, r6
	mov	r0, r4
	bl	get_data_with_nand_bus_one_cmd
	mov	r2, r5
	movs	r1, #255
	mov	r0, r4
	movs	r5, #0
	bl	set_one_cmd
	movs	r3, #54
	mov	r2, r5
	strb	r3, [sp, #16]
	movs	r3, #56
	strb	r3, [sp, #24]
	add	r1, sp, #16
	ldrb	r3, [r8]	@ zero_extendqisi2
	mov	r8, #1
	strd	r8, r8, [sp, #4]
	mov	r0, r4
	str	r7, [sp]
	cmp	r3, #6
	strb	r5, [sp, #20]
	itt	eq
	moveq	r3, #14
	strbeq	r3, [sp, #24]
	add	r3, sp, #24
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
	mov	r2, r8
	movs	r1, #48
	mov	r0, r4
	bl	set_one_cmd
	ldrb	r1, [r6]	@ zero_extendqisi2
	ldrb	r2, [r6, #1]	@ zero_extendqisi2
	cmp	r1, #8
	bne	.L509
	cmp	r2, #8
	beq	.L510
.L509:
	ldr	r0, .L511+12
	bl	NAND_Print
	mov	r0, r4
	bl	nand_disable_chip
	mvn	r0, #95
	b	.L504
.L510:
	mov	r0, r4
	bl	nand_disable_chip
	mov	r0, r5
	b	.L504
.L512:
	.align	2
.L511:
	.word	.LC62
	.word	.LC63
	.word	.LANCHOR55
	.word	.LC64
	.size	m1_read_otp_info_hynix, .-m1_read_otp_info_hynix
	.section	.text.m1_get_rr_value_otp_hynix,"ax",%progbits
	.align	1
	.global	m1_get_rr_value_otp_hynix
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_get_rr_value_otp_hynix, %function
m1_get_rr_value_otp_hynix:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r4, r5, r6, lr}
	mov	r4, r0
	mov	r0, #528
	bl	nand_get_temp_buf
	mov	r5, r0
	cbnz	r0, .L514
	mvn	r6, #77
	ldr	r0, .L518
	bl	NAND_Print
.L513:
	mov	r0, r6
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, pc}
.L514:
	mov	r1, r0
	mov	r0, r4
	bl	m1_read_otp_info_hynix
	cmp	r0, #0
	bge	.L516
	ldr	r3, [r4, #248]
	ldr	r2, [r4, #20]
	ldr	r0, .L518+4
	ldr	r1, [r3, #8]
	bl	NAND_Print
.L516:
	mov	r3, r5
	add	r2, r4, #48
	add	r1, sp, #7
	add	r0, sp, #6
	bl	m1_get_read_retry_cfg
	subs	r6, r0, #0
	bge	.L517
	ldr	r3, [r4, #248]
	ldr	r2, [r4, #20]
	ldr	r0, .L518+8
	ldr	r1, [r3, #8]
	bl	NAND_Print
.L517:
	mov	r1, #528
	mov	r0, r5
	bl	nand_free_temp_buf
	b	.L513
.L519:
	.align	2
.L518:
	.word	.LC65
	.word	.LC66
	.word	.LC67
	.size	m1_get_rr_value_otp_hynix, .-m1_get_rr_value_otp_hynix
	.section	.text.m1_saveotpvalue,"ax",%progbits
	.align	1
	.global	m1_saveotpvalue
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_saveotpvalue, %function
m1_saveotpvalue:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	subs	r3, r1, #1
	adds	r0, r0, #48
	adds	r1, r1, #31
.L521:
	ldrb	r2, [r3, #1]!	@ zero_extendqisi2
	cmp	r3, r1
	strb	r2, [r0], #1
	bne	.L521
	bx	lr
	.size	m1_saveotpvalue, .-m1_saveotpvalue
	.section	.text.m1_getotpparam,"ax",%progbits
	.align	1
	.global	m1_getotpparam
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_getotpparam, %function
m1_getotpparam:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	mov	r4, r0
	ldr	r7, .L530
	mov	r5, r1
	movs	r6, #0
.L524:
	mov	r1, r6
	mov	r0, r7
	bl	NAND_Print_DBG
	mov	r0, r4
	adds	r6, r6, #1
	bl	m1_get_rr_value_otp_hynix
	cmp	r0, #0
	bne	.L524
	mov	r0, r4
	mov	r1, r5
	adds	r4, r4, #32
.L525:
	mov	r2, r1
	movs	r3, #0
.L526:
	adds	r5, r0, r3
	adds	r3, r3, #1
	ldrb	r5, [r5, #48]	@ zero_extendqisi2
	cmp	r3, #4
	strb	r5, [r2], #1
	bne	.L526
	adds	r0, r0, #4
	adds	r1, r1, #4
	cmp	r0, r4
	bne	.L525
	movs	r0, #0
	pop	{r3, r4, r5, r6, r7, pc}
.L531:
	.align	2
.L530:
	.word	.LC68
	.size	m1_getotpparam, .-m1_getotpparam
	.section	.text.m1_setdefaultparam,"ax",%progbits
	.align	1
	.global	m1_setdefaultparam
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_setdefaultparam, %function
m1_setdefaultparam:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r4, r5, r6, r7, lr}
	add	r1, r0, #48
	ldr	r5, .L537
	movs	r2, #0
	ldrb	r3, [r5]	@ zero_extendqisi2
.L533:
	cmp	r3, r2
	bhi	.L534
	ldr	r7, .L537+4
	mov	r1, sp
	ldr	r2, .L537+8
	movs	r4, #0
	bl	m3_vender_set_param
	mov	r6, r0
	ldr	r0, .L537+12
	bl	NAND_Print_DBG
.L535:
	ldrb	r3, [r5]	@ zero_extendqisi2
	cmp	r3, r4
	bhi	.L536
	ldr	r0, .L537+16
	bl	NAND_Print_DBG
	mov	r0, r6
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L534:
	ldrb	r4, [r1], #1	@ zero_extendqisi2
	strb	r4, [sp, r2]
	adds	r2, r2, #1
	b	.L533
.L536:
	ldrb	r1, [sp, r4]	@ zero_extendqisi2
	mov	r0, r7
	bl	NAND_Print_DBG
	adds	r4, r4, #1
	b	.L535
.L538:
	.align	2
.L537:
	.word	.LANCHOR56
	.word	.LC25
	.word	.LANCHOR57
	.word	.LC24
	.word	.LC26
	.size	m1_setdefaultparam, .-m1_setdefaultparam
	.section	.text.m1_init_readretry_param,"ax",%progbits
	.align	1
	.global	m1_init_readretry_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_init_readretry_param, %function
m1_init_readretry_param:
	@ args = 0, pretend = 0, frame = 160
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #252]
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r4, r0
	sub	sp, sp, #176
	ldr	r0, [r3, #12]
	lsls	r0, r0, #9
	bl	nand_get_temp_buf
	mov	r5, r0
	cmp	r0, #0
	beq	.L558
	mov	r0, #18048
	bl	nand_get_temp_buf
	mov	r6, r0
	cbnz	r0, .L541
	ldr	r3, [r4, #252]
	mov	r0, r5
	ldr	r1, [r3, #12]
	lsls	r1, r1, #9
	bl	nand_free_temp_buf
	ldr	r0, .L569
	bl	NAND_Print
	mvn	r0, #75
.L539:
	add	sp, sp, #176
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L541:
	add	r2, r0, #17920
	subs	r3, r0, #1
	adds	r2, r2, #127
	movs	r1, #255
.L542:
	strb	r1, [r3, #1]!
	cmp	r2, r3
	bne	.L542
.L557:
	ldr	r8, .L569+32
	movs	r7, #8
.L548:
	ldr	r3, [r4, #16]
	add	r10, sp, #112
	add	r0, sp, #20
	str	r5, [sp, #36]
	str	r10, [sp, #40]
	strd	r3, r7, [sp, #20]
	movs	r3, #0
	str	r3, [sp, #28]
	movs	r3, #16
	str	r3, [sp, #44]
	ldr	r3, [r4, #28]
	str	r3, [sp, #32]
	bl	m0_read_page
	ldrb	r3, [sp, #115]	@ zero_extendqisi2
	mov	fp, r0
	ldr	r1, [r4, #248]
	ldr	r0, .L569+4
	str	r3, [sp, #12]
	ldrb	r3, [sp, #114]	@ zero_extendqisi2
	str	r3, [sp, #8]
	ldrb	r3, [sp, #113]	@ zero_extendqisi2
	str	r3, [sp, #4]
	ldrb	r3, [sp, #112]	@ zero_extendqisi2
	str	r3, [sp]
	ldrd	r2, r3, [sp, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	cmp	fp, #0
	blt	.L543
	ldrb	r3, [sp, #112]	@ zero_extendqisi2
	cbnz	r3, .L543
	ldrb	r3, [sp, #113]	@ zero_extendqisi2
	cmp	r3, #79
	bne	.L543
	ldrb	r3, [sp, #114]	@ zero_extendqisi2
	cmp	r3, #79
	bne	.L543
	ldrb	r3, [sp, #115]	@ zero_extendqisi2
	cmp	r3, #66
	bne	.L543
	subs	r3, r5, #1
	add	r0, r5, #31
	mov	r2, r3
.L545:
	ldrb	ip, [r2, #33]	@ zero_extendqisi2
	ldrb	r1, [r2, #1]!	@ zero_extendqisi2
	add	r1, r1, ip
	cmp	r1, #255
	beq	.L544
	mov	r0, r8
	bl	NAND_Print_DBG
.L543:
	adds	r7, r7, #1
	cmp	r7, #12
	bne	.L548
	ldr	r3, [r4, #248]
	add	r7, sp, #48
	ldr	r0, .L569+8
	ldr	r1, [r3, #8]
	bl	NAND_Print_DBG
	mov	r1, r7
	mov	r0, r4
	bl	m1_getotpparam
	mov	r0, r4
	bl	m1_setdefaultparam
	subs	r1, r5, #1
	movs	r3, #0
.L549:
	ldrb	r2, [r7, r3]	@ zero_extendqisi2
	adds	r3, r3, #1
	cmp	r3, #32
	strb	r2, [r1, #1]!
	mvn	r2, r2
	strb	r2, [r1, #32]
	bne	.L549
	ldr	fp, .L569+36
	mov	r8, #8
	ldr	r3, .L569+12
	str	r3, [sp, #112]
.L556:
	ldr	r3, [r4, #16]
	add	r0, sp, #20
	strd	r3, r8, [sp, #20]
	bl	m0_erase_block
	cmp	r0, #0
	bge	.L559
	ldrd	r1, r2, [sp, #20]
	ldr	r0, .L569+16
	bl	NAND_Print
.L553:
	add	r8, r8, #1
	cmp	r8, #12
	bne	.L556
	ldr	r3, [r4, #248]
	ldr	r2, [sp, #20]
	ldr	r0, .L569+20
	ldr	r1, [r3, #8]
	bl	NAND_Print_DBG
	b	.L557
.L544:
	cmp	r2, r0
	bne	.L545
	add	r2, r5, #63
.L547:
	ldrb	r1, [r3, #1]!	@ zero_extendqisi2
	cbnz	r1, .L546
	cmp	r3, r2
	bne	.L547
	b	.L543
.L559:
	movs	r7, #0
.L552:
	cmp	r7, #2
	str	r7, [sp, #28]
	ldr	r1, [r4, #28]
	beq	.L554
	movs	r3, #16
	add	r0, sp, #20
	strd	r5, r10, [sp, #36]
	str	r3, [sp, #44]
	str	r1, [sp, #32]
	bl	m0_write_page
	cmp	r0, #0
	bge	.L555
.L568:
	ldrd	r2, r3, [sp, #24]
	mov	r0, fp
	ldr	r1, [sp, #20]
	bl	NAND_Print
.L555:
	adds	r7, r7, #1
	cmp	r7, #3
	bne	.L552
	b	.L553
.L554:
	lsrs	r1, r1, #1
	add	r0, sp, #20
	strd	r6, r10, [sp, #36]
	bl	m1_write_page_FF
	cmp	r0, #0
	bge	.L553
	b	.L568
.L558:
	mvn	r0, #76
	b	.L539
.L546:
	ldr	r1, [r4, #248]
	movs	r7, #0
	ldrd	r2, r3, [sp, #20]
	ldr	r0, .L569+24
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	ldr	r10, .L569+40
	ldr	r3, [r4, #248]
	ldr	r8, .L569+44
	ldr	r2, [sp, #20]
	ldr	r1, [r3, #8]
	ldr	r0, .L569+28
	bl	NAND_Print_DBG
.L551:
	ldrb	r1, [r5, r7]	@ zero_extendqisi2
	mov	r0, r10
	bl	NAND_Print_DBG
	and	r3, r7, #7
	cmp	r3, #7
	bne	.L550
	mov	r0, r8
	bl	NAND_Print_DBG
.L550:
	adds	r7, r7, #1
	cmp	r7, #32
	bne	.L551
	mov	r1, r5
	mov	r0, r4
	bl	m1_saveotpvalue
	mov	r0, r4
	bl	m1_setdefaultparam
	mov	r1, #18048
	mov	r0, r6
	bl	nand_free_temp_buf
	ldr	r3, [r4, #252]
	mov	r0, r5
	ldr	r1, [r3, #12]
	lsls	r1, r1, #9
	bl	nand_free_temp_buf
	movs	r0, #0
	b	.L539
.L570:
	.align	2
.L569:
	.word	.LC69
	.word	.LC70
	.word	.LC72
	.word	1112493824
	.word	.LC74
	.word	.LC76
	.word	.LC77
	.word	.LC78
	.word	.LC71
	.word	.LC75
	.word	.LC73
	.word	.LC26
	.size	m1_init_readretry_param, .-m1_init_readretry_param
	.section	.text.m1_readretry_init,"ax",%progbits
	.align	1
	.global	m1_readretry_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_readretry_init, %function
m1_readretry_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #252]
	ldr	r2, .L579
	push	{r4, lr}
	ldr	r3, [r3, #40]
	ubfx	r1, r3, #16, #8
	strb	r1, [r2]
	lsrs	r4, r3, #8
	ldr	r2, .L579+4
	cmp	r1, #4
	strb	r4, [r2]
	ldr	r2, .L579+8
	strb	r3, [r2]
	bne	.L572
	ldr	r3, .L579+12
	movs	r2, #56
	strb	r2, [r3]
	movs	r2, #57
	strb	r2, [r3, #1]
	movs	r2, #58
	strb	r2, [r3, #2]
	movs	r2, #59
.L578:
	strb	r2, [r3, #3]
	add	r1, r0, #176
	add	r2, r0, #48
	movs	r3, #0
	str	r3, [r0, #176]
.L576:
	strb	r3, [r2], #1
	cmp	r2, r1
	bne	.L576
	bl	m1_init_readretry_param
	movs	r0, #0
	b	.L571
.L572:
	cmp	r1, #6
	bne	.L574
	ldr	r3, .L579+12
	movs	r2, #14
	strb	r2, [r3]
	movs	r2, #15
	strb	r2, [r3, #1]
	movs	r2, #16
	strb	r2, [r3, #2]
	movs	r2, #17
	b	.L578
.L574:
	ldr	r0, .L579+16
	bl	NAND_Print
	mvn	r0, #74
.L571:
	pop	{r4, pc}
.L580:
	.align	2
.L579:
	.word	.LANCHOR55
	.word	.LANCHOR58
	.word	.LANCHOR56
	.word	.LANCHOR57
	.word	.LC28
	.size	m1_readretry_init, .-m1_readretry_init
	.section	.text.m1_readretry_exit,"ax",%progbits
	.align	1
	.global	m1_readretry_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_readretry_exit, %function
m1_readretry_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	ldr	r0, .L582
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	m1_setdefaultparam
	movs	r0, #0
	str	r0, [r4, #176]
	pop	{r4, pc}
.L583:
	.align	2
.L582:
	.word	.LC79
	.size	m1_readretry_exit, .-m1_readretry_exit
	.section	.text.m1_set_readretry,"ax",%progbits
	.align	1
	.global	m1_set_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_set_readretry, %function
m1_set_readretry:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L589
	push	{r0, r1, r4, lr}
	ldr	r1, [r0, #176]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r1, r3
	bhi	.L588
	ldr	r3, .L589+4
	lsls	r1, r1, #2
	movs	r2, #0
	ldrb	r3, [r3]	@ zero_extendqisi2
.L586:
	cmp	r3, r2
	bhi	.L587
	ldr	r2, .L589+8
	mov	r1, sp
	bl	m3_vender_set_param
.L584:
	add	sp, sp, #8
	@ sp needed
	pop	{r4, pc}
.L587:
	adds	r4, r1, r2
	add	r4, r4, r0
	ldrb	r4, [r4, #48]	@ zero_extendqisi2
	strb	r4, [sp, r2]
	adds	r2, r2, #1
	b	.L586
.L588:
	mvn	r0, #73
	b	.L584
.L590:
	.align	2
.L589:
	.word	.LANCHOR58
	.word	.LANCHOR56
	.word	.LANCHOR57
	.size	m1_set_readretry, .-m1_set_readretry
	.section	.text.m1_read_page_end,"ax",%progbits
	.align	1
	.global	m1_read_page_end
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_read_page_end, %function
m1_read_page_end:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L604
	push	{r0, r1, r2, r4, r5, r6, r7, r8, r10, lr}
	mov	r6, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r5, r0
	mov	r0, r6
	bl	m0_read_page_end_not_retry
	adds	r3, r0, #2
	mov	r4, r0
	bne	.L591
	ldr	r7, .L604+4
	mov	r10, #0
	mov	r8, r10
	ldr	r0, .L604+8
	bl	NAND_Print_DBG
.L593:
	ldrb	r2, [r7]	@ zero_extendqisi2
	cmp	r2, r10
	ble	.L597
	ldr	r3, [r5, #176]
	mov	r0, r5
	adds	r3, r3, #1
	cmp	r3, r2
	it	hi
	movhi	r3, r8
	str	r3, [r5, #176]
	bl	m1_set_readretry
	mov	r4, r0
	cbz	r0, .L596
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	m1_set_readretry
.L597:
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	m1_set_readretry
.L591:
	mov	r0, r4
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L596:
	mov	r0, r6
	bl	m0_read_page_start
	mov	r4, r0
	mov	r0, r6
	bl	m0_read_page_end_not_retry
	orrs	r4, r4, r0
	cmp	r4, #10
	beq	.L598
	cbnz	r4, .L599
.L598:
	ldr	r3, [r5, #176]
	movs	r4, #10
	ldr	r1, [r5, #248]
	ldr	r0, .L604+12
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	b	.L597
.L599:
	add	r10, r10, #1
	b	.L593
.L605:
	.align	2
.L604:
	.word	g_nsi
	.word	.LANCHOR58
	.word	.LC80
	.word	.LC81
	.size	m1_read_page_end, .-m1_read_page_end
	.section	.text.m1_special_init,"ax",%progbits
	.align	1
	.global	m1_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_special_init, %function
m1_special_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r4, #0
	ldr	r3, .L611
	ldr	r3, [r3]
	ldr	r5, [r3, #8]
.L607:
	cbnz	r5, .L608
	ldr	r3, .L611+4
	ldrb	r1, [r3]	@ zero_extendqisi2
	ldr	r3, .L611+8
	ldrb	r2, [r3]	@ zero_extendqisi2
	ldr	r3, .L611+12
	ldrb	r3, [r3]	@ zero_extendqisi2
	cbnz	r4, .L609
	ldr	r0, .L611+16
	ldr	r5, .L611+20
	str	r5, [r0]
	ldr	r0, .L611+24
	bl	NAND_Print_DBG
.L606:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L608:
	mov	r0, r5
	bl	m1_readretry_init
	ldr	r5, [r5]
	orrs	r4, r4, r0
	b	.L607
.L609:
	ldr	r0, .L611+28
	bl	NAND_Print
	b	.L606
.L612:
	.align	2
.L611:
	.word	g_nsi
	.word	.LANCHOR55
	.word	.LANCHOR58
	.word	.LANCHOR56
	.word	function_read_page_end
	.word	m1_read_page_end
	.word	.LC82
	.word	.LC83
	.size	m1_special_init, .-m1_special_init
	.section	.text.m1_special_exit,"ax",%progbits
	.align	1
	.global	m1_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_special_exit, %function
m1_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L616
	push	{r4, lr}
	ldr	r3, [r3]
	ldr	r4, [r3, #8]
.L614:
	cbnz	r4, .L615
	ldr	r0, .L616+4
	bl	NAND_Print_DBG
	mov	r0, r4
	pop	{r4, pc}
.L615:
	mov	r0, r4
	bl	m1_readretry_exit
	ldr	r4, [r4]
	b	.L614
.L617:
	.align	2
.L616:
	.word	g_nsi
	.word	.LC84
	.size	m1_special_exit, .-m1_special_exit
	.section	.text.m1_is_lsb_page,"ax",%progbits
	.align	1
	.global	m1_is_lsb_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m1_is_lsb_page, %function
m1_is_lsb_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L622
	ldr	r3, [r3]
	ldr	r3, [r3, #8]
	ldr	r3, [r3, #32]
	cbz	r0, .L620
	subs	r3, r3, #1
	cmp	r3, r0
	beq	.L621
	and	r0, r0, #1
	bx	lr
.L620:
	movs	r0, #1
	bx	lr
.L621:
	movs	r0, #0
	bx	lr
.L623:
	.align	2
.L622:
	.word	g_nsi
	.size	m1_is_lsb_page, .-m1_is_lsb_page
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
	beq	.L626
	mov	r2, r6
	mov	r1, r5
	ldr	r0, .L627
	bl	NAND_Print
	mvn	r0, #71
.L624:
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L626:
	mov	r0, r4
	b	.L624
.L628:
	.align	2
.L627:
	.word	.LC85
	.size	micron_intel_set_feature, .-micron_intel_set_feature
	.section	.text.m4_readretry_init,"ax",%progbits
	.align	1
	.global	m4_readretry_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m4_readretry_init, %function
m4_readretry_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, r8, r10, lr}
	movs	r7, #0
	ldr	r3, [r0, #252]
	movs	r1, #137
	mov	r6, r0
	ldr	r10, .L635+24
	ldr	r3, [r3, #40]
	ldr	r5, .L635
	ldr	r8, .L635+28
	ldr	r4, .L635+4
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
	bne	.L630
	movs	r3, #12
	str	r8, [r5]
	strb	r3, [r4]
.L631:
	ldrb	r1, [r4]	@ zero_extendqisi2
	ldr	r0, .L635+8
	bl	NAND_Print_DBG
	movs	r0, #0
	b	.L629
.L630:
	cmp	r3, #64
	bne	.L632
	ldr	r3, .L635+12
	str	r3, [r5]
	movs	r3, #10
	strb	r3, [r4]
	b	.L631
.L632:
	cmp	r3, #80
	bne	.L633
	ldr	r3, .L635+16
	mov	r2, r7
	movs	r1, #147
	mov	r0, r6
	str	r3, [r5]
	movs	r3, #7
	strb	r3, [r4]
	bl	micron_intel_set_feature
	b	.L631
.L633:
	ldr	r0, .L635+20
	bl	NAND_Print
	mvn	r0, #70
.L629:
	pop	{r3, r4, r5, r6, r7, r8, r10, pc}
.L636:
	.align	2
.L635:
	.word	.LANCHOR59
	.word	.LANCHOR62
	.word	.LC87
	.word	.LANCHOR63
	.word	.LANCHOR64
	.word	.LC86
	.word	.LANCHOR61
	.word	.LANCHOR60
	.size	m4_readretry_init, .-m4_readretry_init
	.section	.text.m4_readretry_exit,"ax",%progbits
	.align	1
	.global	m4_readretry_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m4_readretry_exit, %function
m4_readretry_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L638
	movs	r0, #0
	strb	r0, [r3]
	ldr	r3, .L638+4
	strb	r0, [r3]
	bx	lr
.L639:
	.align	2
.L638:
	.word	.LANCHOR61
	.word	.LANCHOR62
	.size	m4_readretry_exit, .-m4_readretry_exit
	.section	.text.m4_set_readretry,"ax",%progbits
	.align	1
	.global	m4_set_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m4_set_readretry, %function
m4_set_readretry:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	mov	r4, r0
	bl	nand_enable_chip
	ldr	r0, [r4, #248]
	bl	ndfc_disable_randomize
	ldr	r3, .L651
	ldrb	r3, [r3]	@ zero_extendqisi2
	sub	r2, r3, #64
	cmp	r2, #1
	bls	.L641
	cmp	r3, #80
	bne	.L649
.L641:
	ldrb	r6, [r4, #176]	@ zero_extendqisi2
	ldr	r7, .L651+4
	cmp	r6, #255
	it	eq
	moveq	r6, #0
	cmp	r3, #80
	bne	.L644
	ldr	r3, [r7]
	ldrb	r3, [r3, r6]	@ zero_extendqisi2
	cbz	r3, .L645
.L647:
	ldr	r3, [r4, #176]
	cmp	r3, #255
	bne	.L644
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
	b	.L642
.L645:
	movs	r2, #1
	movs	r1, #147
	mov	r0, r4
	bl	micron_intel_set_feature
	mov	r5, r0
	cmp	r0, #0
	beq	.L647
.L642:
	ldr	r3, [r4, #176]
	mov	r0, r4
	cmp	r3, #255
	itt	eq
	moveq	r3, #0
	streq	r3, [r4, #176]
	bl	nand_disable_chip
	mov	r0, r5
	pop	{r3, r4, r5, r6, r7, pc}
.L644:
	ldr	r3, [r7]
	movs	r1, #137
	mov	r0, r4
	ldrb	r2, [r3, r6]	@ zero_extendqisi2
	bl	micron_intel_set_feature
	mov	r5, r0
	b	.L642
.L649:
	movs	r5, #0
	b	.L642
.L652:
	.align	2
.L651:
	.word	.LANCHOR61
	.word	.LANCHOR59
	.size	m4_set_readretry, .-m4_set_readretry
	.section	.text.m4_read_page_end,"ax",%progbits
	.align	1
	.global	m4_read_page_end
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m4_read_page_end, %function
m4_read_page_end:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L663
	push	{r0, r1, r4, r5, r6, r7, r8, lr}
	mov	r6, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r5, r0
	mov	r0, r6
	bl	m0_read_page_end_not_retry
	adds	r3, r0, #2
	mov	r4, r0
	bne	.L653
	ldr	r8, .L663+12
	movs	r7, #0
	ldr	r0, .L663+4
	bl	NAND_Print_DBG
.L655:
	ldrb	r3, [r8]	@ zero_extendqisi2
	cmp	r3, r7
	ble	.L658
	str	r7, [r5, #176]
	mov	r0, r5
	bl	m4_set_readretry
	mov	r4, r0
	cbnz	r0, .L656
	mov	r0, r6
	bl	m0_read_page_start
	mov	r4, r0
	mov	r0, r6
	bl	m0_read_page_end_not_retry
	orrs	r4, r4, r0
	cmp	r4, #10
	beq	.L657
	cbnz	r4, .L656
.L657:
	ldr	r3, [r5, #176]
	movs	r4, #10
	ldr	r1, [r5, #248]
	ldr	r0, .L663+8
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
.L658:
	movs	r3, #255
	mov	r0, r5
	str	r3, [r5, #176]
	bl	m4_set_readretry
.L653:
	mov	r0, r4
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L656:
	adds	r7, r7, #1
	b	.L655
.L664:
	.align	2
.L663:
	.word	g_nsi
	.word	.LC88
	.word	.LC89
	.word	.LANCHOR62
	.size	m4_read_page_end, .-m4_read_page_end
	.section	.text.m4_special_init,"ax",%progbits
	.align	1
	.global	m4_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m4_special_init, %function
m4_special_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r4, #0
	ldr	r3, .L670
	ldr	r3, [r3]
	ldr	r5, [r3, #8]
.L666:
	cbnz	r5, .L667
	ldr	r3, .L670+4
	ldrb	r1, [r3]	@ zero_extendqisi2
	ldr	r3, .L670+8
	ldrb	r2, [r3]	@ zero_extendqisi2
	cbnz	r4, .L668
	ldr	r0, .L670+12
	ldr	r3, .L670+16
	str	r0, [r3]
	ldr	r0, .L670+20
	bl	NAND_Print_DBG
.L665:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L667:
	mov	r0, r5
	bl	m4_readretry_init
	ldr	r5, [r5]
	orrs	r4, r4, r0
	b	.L666
.L668:
	ldr	r0, .L670+24
	bl	NAND_Print
	b	.L665
.L671:
	.align	2
.L670:
	.word	g_nsi
	.word	.LANCHOR61
	.word	.LANCHOR62
	.word	m4_read_page_end
	.word	function_read_page_end
	.word	.LC90
	.word	.LC91
	.size	m4_special_init, .-m4_special_init
	.section	.text.m4_special_exit,"ax",%progbits
	.align	1
	.global	m4_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m4_special_exit, %function
m4_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r5, #0
	ldr	r3, .L679
	ldr	r2, .L679+4
	ldr	r3, [r3]
	ldrb	r1, [r2]	@ zero_extendqisi2
	ldr	r0, [r3, #8]
	ldr	r3, .L679+8
	ldrb	r4, [r3]	@ zero_extendqisi2
.L673:
	cbnz	r0, .L674
	cbz	r5, .L675
	strb	r1, [r2]
	strb	r4, [r3]
.L675:
	ldr	r0, .L679+12
	bl	NAND_Print_DBG
	movs	r0, #0
	pop	{r3, r4, r5, pc}
.L674:
	movs	r1, #0
	ldr	r0, [r0]
	movs	r5, #1
	mov	r4, r1
	b	.L673
.L680:
	.align	2
.L679:
	.word	g_nsi
	.word	.LANCHOR61
	.word	.LANCHOR62
	.word	.LC92
	.size	m4_special_exit, .-m4_special_exit
	.section	.text.m4_0x41_is_lsb_page,"ax",%progbits
	.align	1
	.global	m4_0x41_is_lsb_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m4_0x41_is_lsb_page, %function
m4_0x41_is_lsb_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	subs	r3, r0, #2
	cmp	r3, #1
	bls	.L685
	ldr	r3, .L686
	ldr	r3, [r3]
	ldr	r3, [r3, #8]
	ldr	r3, [r3, #252]
	ldr	r3, [r3, #16]
	subs	r2, r3, #2
	cmp	r2, r0
	beq	.L685
	subs	r3, r3, #1
	cmp	r3, r0
	beq	.L685
	and	r0, r0, #3
	cmp	r0, #1
	ite	hi
	movhi	r0, #0
	movls	r0, #1
	bx	lr
.L685:
	movs	r0, #1
	bx	lr
.L687:
	.align	2
.L686:
	.word	g_nsi
	.size	m4_0x41_is_lsb_page, .-m4_0x41_is_lsb_page
	.section	.text.m4_0x42_is_lsb_page,"ax",%progbits
	.align	1
	.global	m4_0x42_is_lsb_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m4_0x42_is_lsb_page, %function
m4_0x42_is_lsb_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	cmp	r0, #5
	bls	.L692
	subs	r3, r0, #7
	cmp	r3, #1
	bls	.L692
	movw	r3, #509
	cmp	r0, r3
	beq	.L692
	cmp	r0, #6
	beq	.L695
	cmp	r0, #508
	beq	.L695
	movw	r3, #511
	cmp	r0, r3
	beq	.L695
	and	r0, r0, #3
	subs	r0, r0, #2
	cmp	r0, #1
	ite	hi
	movhi	r0, #0
	movls	r0, #1
	bx	lr
.L692:
	movs	r0, #1
	bx	lr
.L695:
	movs	r0, #0
	bx	lr
	.size	m4_0x42_is_lsb_page, .-m4_0x42_is_lsb_page
	.section	.text.m2_vender_get_param,"ax",%progbits
	.align	1
	.global	m2_vender_get_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_vender_get_param, %function
m2_vender_get_param:
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
	ldr	r0, [r6, #248]
	add	r8, sp, #16
	bl	ndfc_disable_randomize
	movs	r3, #55
	strb	r3, [r8, #-1]!
.L697:
	cmp	r4, r7
	bne	.L698
	mov	r0, r6
	bl	nand_disable_chip
	movs	r0, #0
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L698:
	mov	r3, r5
	mov	r2, r4
	str	r10, [sp]
	mov	r1, r8
	mov	r0, r6
	adds	r4, r4, #1
	bl	get_data_with_nand_bus_one_cmd
	adds	r5, r5, #1
	b	.L697
	.size	m2_vender_get_param, .-m2_vender_get_param
	.section	.text.m2_vender_set_param,"ax",%progbits
	.align	1
	.global	m2_vender_set_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_vender_set_param, %function
m2_vender_set_param:
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
	ldr	r0, [r6, #248]
	bl	ndfc_disable_randomize
.L700:
	cmp	r4, r7
	bne	.L701
	mov	r0, r6
	bl	nand_disable_chip
	movs	r0, #0
	add	sp, sp, #28
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L701:
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
	b	.L700
	.size	m2_vender_set_param, .-m2_vender_set_param
	.section	.text.m2_lsb_init,"ax",%progbits
	.align	1
	.global	m2_lsb_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_lsb_init, %function
m2_lsb_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, [r0, #252]
	ldr	r1, .L706
	ldr	r2, [r3, #40]
	ubfx	r3, r2, #16, #8
	strb	r3, [r1]
	lsrs	r0, r2, #8
	ldr	r1, .L706+4
	cmp	r3, #2
	strb	r0, [r1]
	ldr	r1, .L706+8
	strb	r2, [r1]
	bne	.L703
	ldr	r3, .L706+12
	movs	r2, #4
	strb	r2, [r3]
	movs	r2, #176
	ldr	r3, .L706+16
	strb	r2, [r3]
	movs	r2, #177
	strb	r2, [r3, #1]
	movs	r2, #160
	strb	r2, [r3, #2]
	movs	r2, #161
.L705:
	strb	r2, [r3, #3]
	movs	r2, #10
	ldr	r3, .L706+20
	strb	r2, [r3]
	strb	r2, [r3, #1]
	strb	r2, [r3, #2]
	strb	r2, [r3, #3]
.L704:
	movs	r0, #0
	bx	lr
.L703:
	cmp	r3, #3
	bne	.L704
	ldr	r3, .L706+12
	movs	r2, #4
	strb	r2, [r3]
	movs	r2, #160
	ldr	r3, .L706+16
	strb	r2, [r3]
	movs	r2, #161
	strb	r2, [r3, #1]
	movs	r2, #167
	strb	r2, [r3, #2]
	movs	r2, #168
	b	.L705
.L707:
	.align	2
.L706:
	.word	.LANCHOR65
	.word	.LANCHOR66
	.word	.LANCHOR67
	.word	.LANCHOR68
	.word	.LANCHOR69
	.word	.LANCHOR70
	.size	m2_lsb_init, .-m2_lsb_init
	.section	.text.m2_lsb_enable,"ax",%progbits
	.align	1
	.global	m2_lsb_enable
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_lsb_enable, %function
m2_lsb_enable:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r4, r5, r6, r7, lr}
	mov	r5, r0
	ldr	r4, .L711
	ldr	r2, .L711+4
	ldrb	r3, [r4]	@ zero_extendqisi2
	ldr	r1, .L711+8
	bl	m2_vender_get_param
	ldr	r6, .L711+12
	movs	r2, #0
	ldrb	r3, [r4]	@ zero_extendqisi2
	add	r1, sp, #4
	ldr	r4, .L711+8
.L709:
	cmp	r3, r2
	bhi	.L710
	ldr	r2, .L711+4
	mov	r0, r5
	bl	m2_vender_set_param
	movs	r0, #0
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L710:
	ldrb	r0, [r2, r4]	@ zero_extendqisi2
	ldrb	r7, [r2, r6]	@ zero_extendqisi2
	add	r0, r0, r7
	strb	r0, [r1, r2]
	adds	r2, r2, #1
	b	.L709
.L712:
	.align	2
.L711:
	.word	.LANCHOR68
	.word	.LANCHOR69
	.word	.LANCHOR71
	.word	.LANCHOR70
	.size	m2_lsb_enable, .-m2_lsb_enable
	.section	.text.m2_lsb_disable,"ax",%progbits
	.align	1
	.global	m2_lsb_disable
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_lsb_disable, %function
m2_lsb_disable:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	ldr	r3, .L714
	ldr	r2, .L714+4
	ldrb	r3, [r3]	@ zero_extendqisi2
	ldr	r1, .L714+8
	bl	m2_vender_set_param
	movs	r0, #0
	pop	{r3, pc}
.L715:
	.align	2
.L714:
	.word	.LANCHOR68
	.word	.LANCHOR69
	.word	.LANCHOR71
	.size	m2_lsb_disable, .-m2_lsb_disable
	.section	.text.m2_lsb_exit,"ax",%progbits
	.align	1
	.global	m2_lsb_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_lsb_exit, %function
m2_lsb_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	bl	nand_enable_chip
	ldr	r0, [r4, #248]
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
	ldr	r0, .L717
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	nand_disable_chip
	movs	r0, #0
	pop	{r4, pc}
.L718:
	.align	2
.L717:
	.word	.LC93
	.size	m2_lsb_exit, .-m2_lsb_exit
	.section	.text.m2_vender_get_param_otp_hynix,"ax",%progbits
	.align	1
	.global	m2_vender_get_param_otp_hynix
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_vender_get_param_otp_hynix, %function
m2_vender_get_param_otp_hynix:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, lr}
	movs	r3, #0
	sub	sp, sp, #36
	mov	r4, r0
	strh	r3, [sp, #28]	@ movhi
	ldr	r3, .L737
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #2
	bne	.L720
	movw	r3, #52479
	strh	r3, [sp, #24]	@ movhi
	movw	r3, #19776
	strh	r3, [sp, #28]	@ movhi
.L721:
	mov	r0, #1024
	movs	r6, #1
	bl	nand_get_temp_buf
	mov	r5, r0
	mov	r0, r4
	bl	nand_enable_chip
	ldr	r0, [r4, #248]
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
	bne	.L723
	cmp	r2, #8
	beq	.L732
.L723:
	mvn	r6, #95
	ldr	r0, .L737+4
	bl	NAND_Print
.L724:
	mov	r3, #1024
	movs	r2, #0
	str	r3, [sp]
	mov	r1, r2
	mov	r3, r5
	mov	r0, r4
	bl	get_data_with_nand_bus_one_cmd
	movs	r1, #0
	add	ip, r4, #112
.L725:
	add	r2, r5, r1, lsl #7
	add	r3, r4, #48
.L727:
	ldrb	r7, [r2], #1	@ zero_extendqisi2
	strb	r7, [r3], #1
	ldrb	r0, [r2, #63]	@ zero_extendqisi2
	add	r0, r0, r7
	cmp	r0, #255
	bne	.L726
	cmp	r3, ip
	bne	.L727
	ldr	r0, .L737+8
	bl	NAND_Print_DBG
	b	.L729
.L720:
	cmp	r3, #3
	bne	.L731
	movw	r3, #45230
	strh	r3, [sp, #24]	@ movhi
	movs	r3, #77
	strb	r3, [sp, #29]
	b	.L721
.L732:
	movs	r6, #0
	b	.L724
.L731:
	mvn	r6, #96
	b	.L719
.L726:
	adds	r1, r1, #1
	cmp	r1, #8
	bne	.L725
	mvn	r6, #94
	ldr	r0, .L737+12
	bl	NAND_Print_DBG
.L729:
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
.L719:
	mov	r0, r6
	add	sp, sp, #36
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L738:
	.align	2
.L737:
	.word	.LANCHOR65
	.word	.LC64
	.word	.LC95
	.word	.LC94
	.size	m2_vender_get_param_otp_hynix, .-m2_vender_get_param_otp_hynix
	.section	.text.m2_saveotpvalue,"ax",%progbits
	.align	1
	.global	m2_saveotpvalue
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_saveotpvalue, %function
m2_saveotpvalue:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	subs	r3, r1, #1
	adds	r0, r0, #48
	adds	r1, r1, #63
.L740:
	ldrb	r2, [r3, #1]!	@ zero_extendqisi2
	cmp	r3, r1
	strb	r2, [r0], #1
	bne	.L740
	bx	lr
	.size	m2_saveotpvalue, .-m2_saveotpvalue
	.section	.text.m2_getotpparam,"ax",%progbits
	.align	1
	.global	m2_getotpparam
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_getotpparam, %function
m2_getotpparam:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	mov	r4, r0
	ldr	r8, .L752+4
	mov	r5, r1
	movs	r6, #0
.L745:
	mov	r1, r6
	mov	r0, r8
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	m2_vender_get_param_otp_hynix
	mov	r7, r0
	cbnz	r0, .L743
	mov	r0, r4
	mov	r1, r5
	add	r2, r4, #64
.L744:
	movs	r3, #0
.L747:
	adds	r4, r0, r3
	ldrb	r4, [r4, #48]	@ zero_extendqisi2
	strb	r4, [r1, r3]
	adds	r3, r3, #1
	cmp	r3, #8
	bne	.L747
	adds	r0, r0, #8
	adds	r1, r1, #8
	cmp	r2, r0
	bne	.L744
.L742:
	mov	r0, r7
	pop	{r4, r5, r6, r7, r8, pc}
.L743:
	adds	r6, r6, #1
	cmp	r6, #5
	bne	.L745
	ldr	r0, .L752
	bl	NAND_Print
	b	.L742
.L753:
	.align	2
.L752:
	.word	.LC96
	.word	.LC68
	.size	m2_getotpparam, .-m2_getotpparam
	.section	.text.m2_setdefaultparam,"ax",%progbits
	.align	1
	.global	m2_setdefaultparam
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_setdefaultparam, %function
m2_setdefaultparam:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r4, r5, r6, r7, lr}
	add	r1, r0, #48
	ldr	r5, .L759
	movs	r2, #0
	ldrb	r3, [r5]	@ zero_extendqisi2
.L755:
	cmp	r3, r2
	bhi	.L756
	ldr	r7, .L759+4
	mov	r1, sp
	ldr	r2, .L759+8
	movs	r4, #0
	bl	m2_vender_set_param
	mov	r6, r0
	ldr	r0, .L759+12
	bl	NAND_Print_DBG
.L757:
	ldrb	r3, [r5]	@ zero_extendqisi2
	cmp	r3, r4
	bhi	.L758
	ldr	r0, .L759+16
	bl	NAND_Print_DBG
	mov	r0, r6
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L756:
	ldrb	r4, [r1], #1	@ zero_extendqisi2
	strb	r4, [sp, r2]
	adds	r2, r2, #1
	b	.L755
.L758:
	ldrb	r1, [sp, r4]	@ zero_extendqisi2
	mov	r0, r7
	bl	NAND_Print_DBG
	adds	r4, r4, #1
	b	.L757
.L760:
	.align	2
.L759:
	.word	.LANCHOR67
	.word	.LC25
	.word	.LANCHOR72
	.word	.LC24
	.word	.LC26
	.size	m2_setdefaultparam, .-m2_setdefaultparam
	.section	.text.m2_init_readretry_param,"ax",%progbits
	.align	1
	.global	m2_init_readretry_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_init_readretry_param, %function
m2_init_readretry_param:
	@ args = 0, pretend = 0, frame = 160
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #252]
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r4, r0
	sub	sp, sp, #176
	ldr	r0, [r3, #12]
	lsls	r0, r0, #9
	bl	nand_get_temp_buf
	mov	r5, r0
	cmp	r0, #0
	beq	.L785
	ldr	r8, .L787+28
.L762:
	ldr	r10, .L787+32
	movs	r6, #8
.L769:
	ldr	r3, [r4, #16]
	add	r7, sp, #112
	add	r0, sp, #20
	str	r5, [sp, #36]
	str	r7, [sp, #40]
	strd	r3, r6, [sp, #20]
	movs	r3, #0
	str	r3, [sp, #28]
	movs	r3, #16
	str	r3, [sp, #44]
	ldr	r3, [r4, #28]
	str	r3, [sp, #32]
	bl	m0_read_page
	ldrb	r3, [sp, #115]	@ zero_extendqisi2
	mov	fp, r0
	ldr	r1, [r4, #248]
	mov	r0, r8
	str	r3, [sp, #12]
	ldrb	r3, [sp, #114]	@ zero_extendqisi2
	str	r3, [sp, #8]
	ldrb	r3, [sp, #113]	@ zero_extendqisi2
	str	r3, [sp, #4]
	ldrb	r3, [sp, #112]	@ zero_extendqisi2
	str	r3, [sp]
	ldrd	r2, r3, [sp, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	cmp	fp, #0
	blt	.L764
	ldrb	r3, [sp, #112]	@ zero_extendqisi2
	cbnz	r3, .L764
	ldrb	r3, [sp, #113]	@ zero_extendqisi2
	cmp	r3, #79
	bne	.L764
	ldrb	r3, [sp, #114]	@ zero_extendqisi2
	cmp	r3, #79
	bne	.L764
	ldrb	r3, [sp, #115]	@ zero_extendqisi2
	cmp	r3, #66
	bne	.L764
	subs	r2, r5, #1
	add	r0, r5, #63
	mov	r3, r2
.L766:
	ldrb	ip, [r3, #65]	@ zero_extendqisi2
	ldrb	r1, [r3, #1]!	@ zero_extendqisi2
	add	r1, r1, ip
	cmp	r1, #255
	beq	.L765
	mov	r0, r10
	bl	NAND_Print_DBG
.L764:
	adds	r6, r6, #1
	cmp	r6, #12
	bne	.L769
	ldr	r3, [r4, #248]
	ldr	r0, .L787
	ldr	r1, [r3, #8]
	bl	NAND_Print_DBG
	add	r1, sp, #48
	mov	r0, r4
	bl	m2_getotpparam
	mov	r6, r0
	cbz	r0, .L786
.L770:
	ldr	r3, [r4, #252]
	mov	r0, r5
	ldr	r1, [r3, #12]
	lsls	r1, r1, #9
	bl	nand_free_temp_buf
	b	.L761
.L785:
	mvn	r6, #93
	ldr	r0, .L787+4
	bl	NAND_Print_DBG
.L761:
	mov	r0, r6
	add	sp, sp, #176
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L765:
	cmp	r3, r0
	bne	.L766
.L768:
	ldrb	r1, [r2, #1]!	@ zero_extendqisi2
	cmp	r1, #0
	bne	.L767
	cmp	r2, r3
	bne	.L768
	b	.L764
.L786:
	mov	r0, r4
	bl	m2_setdefaultparam
	add	r1, sp, #47
	subs	r3, r5, #1
	add	r0, r5, #63
.L774:
	ldrb	r2, [r1, #1]!	@ zero_extendqisi2
	strb	r2, [r3, #1]!
	cmp	r3, r0
	ldrb	r2, [r1]	@ zero_extendqisi2
	mvn	r2, r2
	strb	r2, [r3, #64]
	bne	.L774
	ldr	r3, .L787+8
	mov	r0, r4
	ldr	r10, .L787+36
	movs	r6, #8
	ldr	fp, .L787+40
	str	r3, [sp, #112]
	bl	m2_lsb_init
	mov	r0, r4
	bl	m2_lsb_enable
.L777:
	ldr	r3, [r4, #16]
	add	r0, sp, #20
	strd	r5, r7, [sp, #36]
	strd	r3, r6, [sp, #20]
	movs	r3, #0
	str	r3, [sp, #28]
	movs	r3, #16
	str	r3, [sp, #44]
	ldr	r3, [r4, #28]
	str	r3, [sp, #32]
	bl	m0_erase_block
	cmp	r0, #0
	bge	.L775
	ldrd	r1, r2, [sp, #20]
	mov	r0, fp
	bl	NAND_Print
.L776:
	adds	r6, r6, #1
	cmp	r6, #12
	bne	.L777
	mov	r0, r4
	bl	m2_lsb_disable
	mov	r0, r4
	bl	m2_lsb_exit
	ldr	r3, [r4, #248]
	ldr	r0, .L787+12
	ldr	r1, [r3, #8]
	bl	NAND_Print_DBG
	b	.L762
.L775:
	add	r0, sp, #20
	bl	m0_write_page
	cmp	r0, #0
	bge	.L776
	ldrd	r2, r3, [sp, #24]
	mov	r0, r10
	ldr	r1, [sp, #20]
	bl	NAND_Print
	b	.L776
.L767:
	ldr	r1, [r4, #248]
	movs	r6, #0
	ldrd	r2, r3, [sp, #20]
	ldr	r0, .L787+16
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	ldr	r8, .L787+44
	ldr	r3, [r4, #248]
	ldr	r7, .L787+20
	ldr	r2, [sp, #20]
	ldr	r1, [r3, #8]
	ldr	r0, .L787+24
	bl	NAND_Print_DBG
.L773:
	ldrb	r1, [r5, r6]	@ zero_extendqisi2
	mov	r0, r8
	bl	NAND_Print_DBG
	and	r3, r6, #7
	cmp	r3, #7
	bne	.L772
	mov	r0, r7
	bl	NAND_Print_DBG
.L772:
	adds	r6, r6, #1
	cmp	r6, #64
	bne	.L773
	mov	r1, r5
	mov	r0, r4
	bl	m2_saveotpvalue
	mov	r0, r4
	bl	m2_setdefaultparam
	mov	r6, r0
	b	.L770
.L788:
	.align	2
.L787:
	.word	.LC72
	.word	.LC97
	.word	1112493824
	.word	.LC98
	.word	.LC77
	.word	.LC26
	.word	.LC78
	.word	.LC70
	.word	.LC71
	.word	.LC75
	.word	.LC74
	.word	.LC73
	.size	m2_init_readretry_param, .-m2_init_readretry_param
	.section	.text.m2_readretry_init,"ax",%progbits
	.align	1
	.global	m2_readretry_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_readretry_init, %function
m2_readretry_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #252]
	ldr	r2, .L798
	push	{r4, lr}
	ldr	r3, [r3, #40]
	ubfx	r1, r3, #16, #8
	strb	r1, [r2]
	lsrs	r4, r3, #8
	ldr	r2, .L798+4
	cmp	r1, #2
	strb	r4, [r2]
	ldr	r2, .L798+8
	strb	r3, [r2]
	bne	.L790
	ldr	r3, .L798+12
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
.L797:
	strb	r2, [r3, #7]
	add	r1, r0, #176
	add	r2, r0, #48
	movs	r3, #0
	str	r3, [r0, #176]
.L794:
	strb	r3, [r2], #1
	cmp	r2, r1
	bne	.L794
	pop	{r4, lr}
	b	m2_init_readretry_param
.L790:
	cmp	r1, #3
	bne	.L792
	ldr	r3, .L798+12
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
	b	.L797
.L792:
	ldr	r0, .L798+16
	bl	NAND_Print
	mvn	r0, #92
	pop	{r4, pc}
.L799:
	.align	2
.L798:
	.word	.LANCHOR65
	.word	.LANCHOR66
	.word	.LANCHOR67
	.word	.LANCHOR72
	.word	.LC28
	.size	m2_readretry_init, .-m2_readretry_init
	.section	.text.m2_readretry_exit,"ax",%progbits
	.align	1
	.global	m2_readretry_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_readretry_exit, %function
m2_readretry_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	ldr	r0, .L801
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	m2_setdefaultparam
	movs	r0, #0
	str	r0, [r4, #176]
	pop	{r4, pc}
.L802:
	.align	2
.L801:
	.word	.LC99
	.size	m2_readretry_exit, .-m2_readretry_exit
	.section	.text.m2_set_readretry,"ax",%progbits
	.align	1
	.global	m2_set_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_set_readretry, %function
m2_set_readretry:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L810
	push	{r0, r1, r2, r4, r5, r6, r7, lr}
	ldr	r1, [r0, #176]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r1, r3
	bhi	.L809
	ldr	r5, .L810+4
	lsls	r1, r1, #3
	movs	r2, #0
	ldrb	r3, [r5]	@ zero_extendqisi2
.L805:
	cmp	r3, r2
	bhi	.L806
	ldr	r7, .L810+8
	mov	r1, sp
	ldr	r2, .L810+12
	movs	r4, #0
	bl	m2_vender_set_param
	mov	r6, r0
	ldr	r0, .L810+16
	bl	NAND_Print_DBG
.L807:
	ldrb	r3, [r5]	@ zero_extendqisi2
	cmp	r3, r4
	bhi	.L808
	ldr	r0, .L810+20
	bl	NAND_Print_DBG
.L803:
	mov	r0, r6
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L806:
	adds	r4, r1, r2
	add	r4, r4, r0
	ldrb	r4, [r4, #48]	@ zero_extendqisi2
	strb	r4, [sp, r2]
	adds	r2, r2, #1
	b	.L805
.L808:
	ldrb	r1, [sp, r4]	@ zero_extendqisi2
	mov	r0, r7
	bl	NAND_Print
	adds	r4, r4, #1
	b	.L807
.L809:
	mvn	r6, #91
	b	.L803
.L811:
	.align	2
.L810:
	.word	.LANCHOR66
	.word	.LANCHOR67
	.word	.LC31
	.word	.LANCHOR72
	.word	.LC100
	.word	.LC26
	.size	m2_set_readretry, .-m2_set_readretry
	.section	.text.m2_special_init,"ax",%progbits
	.align	1
	.global	m2_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_special_init, %function
m2_special_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r4, #0
	ldr	r3, .L817
	ldr	r3, [r3]
	ldr	r5, [r3, #8]
.L813:
	cbnz	r5, .L814
	ldr	r3, .L817+4
	ldrb	r1, [r3]	@ zero_extendqisi2
	ldr	r3, .L817+8
	ldrb	r2, [r3]	@ zero_extendqisi2
	ldr	r3, .L817+12
	ldrb	r3, [r3]	@ zero_extendqisi2
	cbnz	r4, .L815
	ldr	r0, .L817+16
	ldr	r5, .L817+20
	str	r5, [r0]
	ldr	r0, .L817+24
	bl	NAND_Print_DBG
.L812:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L814:
	mov	r0, r5
	bl	m2_readretry_init
	ldr	r5, [r5]
	orrs	r4, r4, r0
	b	.L813
.L815:
	ldr	r0, .L817+28
	bl	NAND_Print
	b	.L812
.L818:
	.align	2
.L817:
	.word	g_nsi
	.word	.LANCHOR65
	.word	.LANCHOR66
	.word	.LANCHOR67
	.word	function_read_page_end
	.word	m2_read_page_end
	.word	.LC101
	.word	.LC102
	.size	m2_special_init, .-m2_special_init
	.section	.text.m2_special_exit,"ax",%progbits
	.align	1
	.global	m2_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_special_exit, %function
m2_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L822
	push	{r4, lr}
	ldr	r3, [r3]
	ldr	r4, [r3, #8]
.L820:
	cbnz	r4, .L821
	ldr	r0, .L822+4
	bl	NAND_Print_DBG
	mov	r0, r4
	pop	{r4, pc}
.L821:
	mov	r0, r4
	bl	m2_readretry_exit
	ldr	r4, [r4]
	b	.L820
.L823:
	.align	2
.L822:
	.word	g_nsi
	.word	.LC103
	.size	m2_special_exit, .-m2_special_exit
	.section	.text.m2_is_lsb_page,"ax",%progbits
	.align	1
	.global	m2_is_lsb_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_is_lsb_page, %function
m2_is_lsb_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	cmp	r0, #1
	bls	.L826
	ldr	r3, .L829
	ldr	r3, [r3]
	ldr	r3, [r3, #8]
	ldr	r3, [r3, #252]
	ldr	r3, [r3, #16]
	subs	r2, r3, #2
	cmp	r2, r0
	beq	.L828
	subs	r3, r3, #1
	cmp	r3, r0
	beq	.L828
	and	r0, r0, #3
	subs	r0, r0, #2
	cmp	r0, #1
	ite	hi
	movhi	r0, #0
	movls	r0, #1
	bx	lr
.L826:
	movs	r0, #1
	bx	lr
.L828:
	movs	r0, #0
	bx	lr
.L830:
	.align	2
.L829:
	.word	g_nsi
	.size	m2_is_lsb_page, .-m2_is_lsb_page
	.section	.text.m3_is_lsb_page,"ax",%progbits
	.align	1
	.global	m3_is_lsb_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m3_is_lsb_page, %function
m3_is_lsb_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	b	m2_is_lsb_page
	.size	m3_is_lsb_page, .-m3_is_lsb_page
	.section	.text.m4_0x40_is_lsb_page,"ax",%progbits
	.align	1
	.global	m4_0x40_is_lsb_page
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m4_0x40_is_lsb_page, %function
m4_0x40_is_lsb_page:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	b	m2_is_lsb_page
	.size	m4_0x40_is_lsb_page, .-m4_0x40_is_lsb_page
	.section	.text.m9_dsp_on,"ax",%progbits
	.align	1
	.global	m9_dsp_on
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m9_dsp_on, %function
m9_dsp_on:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L834
	push	{r4, lr}
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r4, r0
	bl	nand_enable_chip
	ldr	r0, [r4, #248]
	bl	ndfc_disable_randomize
	movs	r2, #0
	movs	r1, #38
	mov	r0, r4
	bl	set_one_cmd
	mov	r0, r4
	bl	nand_disable_chip
	movs	r0, #0
	pop	{r4, pc}
.L835:
	.align	2
.L834:
	.word	g_nsi
	.size	m9_dsp_on, .-m9_dsp_on
	.section	.text.m9_cmd25,"ax",%progbits
	.align	1
	.global	m9_cmd25
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m9_cmd25, %function
m9_cmd25:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L837
	push	{r4, lr}
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r4, r0
	bl	nand_enable_chip
	ldr	r0, [r4, #248]
	bl	ndfc_disable_randomize
	movs	r2, #0
	movs	r1, #37
	mov	r0, r4
	bl	set_one_cmd
	mov	r0, r4
	bl	nand_disable_chip
	movs	r0, #0
	pop	{r4, pc}
.L838:
	.align	2
.L837:
	.word	g_nsi
	.size	m9_cmd25, .-m9_cmd25
	.section	.text.m9_set_lmflgfix_next,"ax",%progbits
	.align	1
	.global	m9_set_lmflgfix_next
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m9_set_lmflgfix_next, %function
m9_set_lmflgfix_next:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L844
	push	{r4, r5, r6, r7, r8, lr}
	mov	r5, r1
	sub	sp, sp, #24
	ldr	r1, [r0]
	add	r7, sp, #22
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r4, r0
	bl	nand_enable_chip
	ldr	r0, [r4, #248]
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
	ldr	r3, .L844+4
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #52
	bne	.L840
	movs	r3, #34
.L843:
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
.L840:
	cmp	r3, #53
	ite	eq
	moveq	r3, #37
	movne	r3, #35
	b	.L843
.L845:
	.align	2
.L844:
	.word	g_nsi
	.word	.LANCHOR73
	.size	m9_set_lmflgfix_next, .-m9_set_lmflgfix_next
	.section	.text.m9_readretry_init,"ax",%progbits
	.align	1
	.global	m9_readretry_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m9_readretry_init, %function
m9_readretry_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, [r0, #252]
	ldr	r2, .L849
	push	{r4, lr}
	movs	r4, #0
	ldr	r3, [r3, #40]
	str	r4, [r0, #176]
	ubfx	r1, r3, #16, #8
	ldr	r0, .L849+4
	strb	r1, [r2]
	lsrs	r2, r3, #8
	strb	r2, [r0]
	ldr	r0, .L849+8
	strb	r3, [r0]
	sub	r3, r1, #50
	cmp	r3, #3
	bls	.L847
	ldr	r0, .L849+12
	bl	NAND_Print
	mvn	r0, #98
.L846:
	pop	{r4, pc}
.L847:
	uxtb	r1, r2
	ldr	r0, .L849+16
	bl	NAND_Print
	mov	r0, r4
	b	.L846
.L850:
	.align	2
.L849:
	.word	.LANCHOR73
	.word	.LANCHOR74
	.word	.LANCHOR75
	.word	.LC104
	.word	.LC105
	.size	m9_readretry_init, .-m9_readretry_init
	.section	.text.m9_readretry_exit,"ax",%progbits
	.align	1
	.global	m9_readretry_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m9_readretry_exit, %function
m9_readretry_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #0
	bx	lr
	.size	m9_readretry_exit, .-m9_readretry_exit
	.section	.text.m9_read_retry_clock_save,"ax",%progbits
	.align	1
	.global	m9_read_retry_clock_save
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m9_read_retry_clock_save, %function
m9_read_retry_clock_save:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	mov	r4, r0
	ldr	r3, .L853
	ldr	r0, [r0, #248]
	ldr	r3, [r3]
	ldr	r2, .L853+4
	ldr	r1, .L853+8
	ldr	r0, [r0, #8]
	blx	r3
	ldr	r0, [r4, #248]
	movs	r2, #20
	ldr	r3, .L853+12
	movs	r1, #10
	ldr	r0, [r0, #8]
	ldr	r3, [r3]
	blx	r3
	movs	r0, #0
	pop	{r4, pc}
.L854:
	.align	2
.L853:
	.word	NAND_GetClk
	.word	.LANCHOR76
	.word	.LANCHOR77
	.word	NAND_SetClk
	.size	m9_read_retry_clock_save, .-m9_read_retry_clock_save
	.section	.text.m9_read_retry_clock_recover,"ax",%progbits
	.align	1
	.global	m9_read_retry_clock_recover
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m9_read_retry_clock_recover, %function
m9_read_retry_clock_recover:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	ldr	r0, [r0, #248]
	ldr	r3, .L856
	ldr	r2, .L856+4
	ldr	r1, .L856+8
	ldr	r3, [r3]
	ldr	r2, [r2]
	ldr	r1, [r1]
	ldr	r0, [r0, #8]
	blx	r3
	movs	r0, #0
	pop	{r3, pc}
.L857:
	.align	2
.L856:
	.word	NAND_SetClk
	.word	.LANCHOR76
	.word	.LANCHOR77
	.size	m9_read_retry_clock_recover, .-m9_read_retry_clock_recover
	.section	.text.m9_vender_set_param,"ax",%progbits
	.align	1
	.global	m9_vender_set_param
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m9_vender_set_param, %function
m9_vender_set_param:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r3, r4, lr}
	movs	r4, #1
	movs	r3, #4
	str	r1, [sp]
	strd	r3, r4, [sp, #4]
	mov	r3, r2
	ldr	r1, .L859
	mov	r2, r4
	bl	set_cmd_with_nand_bus
	add	sp, sp, #16
	@ sp needed
	pop	{r4, pc}
.L860:
	.align	2
.L859:
	.word	.LANCHOR78
	.size	m9_vender_set_param, .-m9_vender_set_param
	.section	.text.m9_set_readretry,"ax",%progbits
	.align	1
	.global	m9_set_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m9_set_readretry, %function
m9_set_readretry:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	mov	r4, r0
	bl	nand_enable_chip
	ldr	r0, [r4, #248]
	bl	ndfc_disable_randomize
	ldr	r3, .L866
	ldr	r1, [r4, #176]
	ldrb	r3, [r3]	@ zero_extendqisi2
	cmp	r3, #52
	bne	.L862
	ldr	r3, .L866+4
	add	r1, r3, r1, lsl #2
.L863:
	movs	r3, #1
	ldr	r2, .L866+8
	mov	r0, r4
	bl	m9_vender_set_param
	movs	r2, #0
	mov	r5, r0
	movs	r1, #93
	mov	r0, r4
	bl	set_one_cmd
	mov	r0, r4
	bl	nand_disable_chip
	mov	r0, r5
	pop	{r3, r4, r5, pc}
.L862:
	cmp	r3, #53
	lsl	r1, r1, #2
	ite	eq
	ldreq	r3, .L866+12
	ldrne	r3, .L866+16
	add	r1, r1, r3
	b	.L863
.L867:
	.align	2
.L866:
	.word	.LANCHOR73
	.word	.LANCHOR79
	.word	.LANCHOR82
	.word	.LANCHOR80
	.word	.LANCHOR81
	.size	m9_set_readretry, .-m9_set_readretry
	.section	.text.m9_exit_readretry,"ax",%progbits
	.align	1
	.global	m9_exit_readretry
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m9_exit_readretry, %function
m9_exit_readretry:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r4, r5, lr}
	movs	r3, #0
	mov	r4, r0
	add	r5, sp, #8
	str	r3, [r5, #-4]!
	bl	nand_enable_chip
	ldr	r0, [r4, #248]
	bl	ndfc_disable_randomize
	movs	r2, #1
	movs	r1, #255
	mov	r0, r4
	bl	set_one_cmd
	mov	r1, r5
	movs	r3, #1
	ldr	r2, .L869
	mov	r0, r4
	bl	m9_vender_set_param
	mov	r5, r0
	mov	r0, r4
	bl	nand_disable_chip
	mov	r0, r5
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, pc}
.L870:
	.align	2
.L869:
	.word	.LANCHOR82
	.size	m9_exit_readretry, .-m9_exit_readretry
	.section	.text.m9_special_init,"ax",%progbits
	.align	1
	.global	m9_special_init
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m9_special_init, %function
m9_special_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, lr}
	movs	r4, #0
	ldr	r3, .L877
	ldr	r3, [r3]
	ldr	r5, [r3, #8]
.L872:
	cbnz	r5, .L873
	ldr	r3, .L877+4
	ldrb	r1, [r3]	@ zero_extendqisi2
	ldr	r3, .L877+8
	ldrb	r2, [r3]	@ zero_extendqisi2
	ldr	r3, .L877+12
	ldrb	r3, [r3]	@ zero_extendqisi2
	cbnz	r4, .L874
	ldr	r0, .L877+16
	ldr	r5, .L877+20
	str	r5, [r0]
	ldr	r0, .L877+24
.L876:
	bl	NAND_Print
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L873:
	mov	r0, r5
	bl	m9_readretry_init
	ldr	r5, [r5]
	orrs	r4, r4, r0
	b	.L872
.L874:
	ldr	r0, .L877+28
	b	.L876
.L878:
	.align	2
.L877:
	.word	g_nsi
	.word	.LANCHOR73
	.word	.LANCHOR74
	.word	.LANCHOR75
	.word	function_read_page_end
	.word	m9_read_page_end
	.word	.LC106
	.word	.LC107
	.size	m9_special_init, .-m9_special_init
	.section	.text.m9_special_exit,"ax",%progbits
	.align	1
	.global	m9_special_exit
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m9_special_exit, %function
m9_special_exit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L890
	push	{r4, r5, r6, lr}
	movs	r6, #0
	ldr	r3, [r3]
	ldr	r4, [r3, #8]
.L880:
	cbnz	r4, .L883
	ldr	r0, .L890+4
	mov	r5, r4
	bl	NAND_Print
	b	.L879
.L883:
	ldr	r3, [r4, #220]
	cbz	r3, .L881
	str	r6, [r4, #196]
	mov	r0, r4
	bl	_setup_nand_toggle_vendor_specific_feature
	mov	r5, r0
	cbz	r0, .L881
	ldr	r0, .L890+8
	bl	NAND_Print
.L879:
	mov	r0, r5
	pop	{r4, r5, r6, pc}
.L881:
	ldr	r4, [r4]
	b	.L880
.L891:
	.align	2
.L890:
	.word	g_nsi
	.word	.LC109
	.word	.LC108
	.size	m9_special_exit, .-m9_special_exit
	.section	.text.m9_check_bad_block_first_burn,"ax",%progbits
	.align	1
	.global	m9_check_bad_block_first_burn
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m9_check_bad_block_first_burn, %function
m9_check_bad_block_first_burn:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L896
	push	{r0, r1, r2, r4, r5, r6, r7, r8, r10, lr}
	mov	r8, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	ldr	r4, [r0, #248]
	mov	r5, r0
	ldr	r2, [r0, #20]
	ldr	r1, [r8, #4]
	ldr	r3, [r4, #12]
	add	r10, r4, #28
	ldr	r0, [r0, #24]
	cmp	r2, r3
	bcs	.L893
	cmp	r1, r0
	bcc	.L894
.L893:
	strd	r1, r0, [sp]
	ldr	r1, [r4, #8]
	mvn	r4, #9
	ldr	r0, .L896+4
	bl	NAND_Print
.L892:
	mov	r0, r4
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L894:
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
	strd	r6, r3, [r4, #100]
	str	r7, [r4, #28]
	strd	r6, r7, [r4, #48]
	strd	r6, r7, [r4, #108]
	ldr	r0, [r5, #36]
	bl	get_row_addr
	movs	r3, #5
	mov	r2, r0
	str	r3, [r4, #132]
	add	r3, r4, #116
	str	r3, [sp]
	movs	r1, #2
	movs	r3, #3
	mov	r0, r7
	bl	fill_cmd_addr
	movs	r3, #16
	strd	r6, r6, [r4, #168]
	strd	r6, r3, [r4, #160]
	mov	r1, r10
	ldr	r0, [r5, #248]
	bl	ndfc_execute_cmd
	mov	r0, r5
	bl	nand_read_chip_status_ready
	mov	r4, r0
	mov	r0, r5
	bl	nand_disable_chip
	b	.L892
.L897:
	.align	2
.L896:
	.word	g_nsi
	.word	.LC50
	.size	m9_check_bad_block_first_burn, .-m9_check_bad_block_first_burn
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
.L899:
	ldr	r3, [r0, #20]
	cbnz	r3, .L901
	str	r1, [r0, #20]
	str	r3, [r1, #20]
	b	.L903
.L901:
	ldrh	r2, [r3, #6]
	ldrh	r4, [r1, #6]
	cmp	r4, r2
	bhi	.L902
	str	r1, [r3, #24]
	str	r3, [r1, #20]
	str	r1, [r0, #20]
.L903:
	str	r0, [r1, #24]
	movs	r0, #0
	pop	{r4, pc}
.L902:
	mov	r0, r3
	b	.L899
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
	cbz	r0, .L904
	ldr	r2, [r0, #20]
	str	r2, [r3, #172]
	ldr	r2, [r0, #20]
	str	r2, [r3, #172]
	ldr	r2, [r0, #20]
	cbz	r2, .L904
	adds	r3, r3, #152
	str	r3, [r2, #24]
.L904:
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
.L912:
	cbnz	r0, .L915
	bx	lr
.L915:
	cmp	r0, r1
	ldr	r3, [r0, #20]
	bne	.L916
	ldr	r2, [r0, #24]
	str	r3, [r2, #20]
	ldr	r3, [r0, #20]
	cbz	r3, .L913
	str	r2, [r3, #24]
.L913:
	movs	r3, #0
	strd	r3, r3, [r0, #20]
	bx	lr
.L916:
	mov	r0, r3
	b	.L912
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
.L921:
	cbnz	r0, .L923
	bx	lr
.L923:
	cmp	r0, r1
	beq	.L924
	ldr	r0, [r0, #20]
	b	.L921
.L924:
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
	cbz	r3, .L925
	ldr	r0, .L930
	bl	NAND_Print
	ldr	r5, .L930+4
	ldr	r4, [r4, #172]
	ldr	r6, .L930+8
.L927:
	cbnz	r4, .L928
.L925:
	pop	{r4, r5, r6, pc}
.L928:
	ldrh	r2, [r4, #6]
	mov	r0, r5
	ldrh	r1, [r4]
	bl	NAND_Print
	ldr	r1, [r4, #8]
	mov	r0, r6
	bl	NAND_Print
	ldrh	r1, [r4, #2]
	ldr	r0, .L930+12
	bl	NAND_Print
	ldr	r4, [r4, #20]
	b	.L927
.L931:
	.align	2
.L930:
	.word	.LC110
	.word	.LC111
	.word	.LC112
	.word	.LC113
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
.L933:
	ldr	r3, [r0, #12]
	cbnz	r3, .L935
	str	r1, [r0, #12]
	str	r3, [r1, #12]
	b	.L937
.L935:
	ldrh	r2, [r3, #4]
	ldrh	r4, [r1, #4]
	cmp	r4, r2
	bcc	.L936
	str	r1, [r3, #16]
	str	r3, [r1, #12]
	str	r1, [r0, #12]
.L937:
	str	r0, [r1, #16]
	movs	r0, #0
	pop	{r4, pc}
.L936:
	mov	r0, r3
	b	.L933
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
	cbz	r0, .L938
	ldr	r2, [r0, #12]
	str	r2, [r3, #200]
	ldr	r2, [r0, #12]
	cbz	r2, .L938
	adds	r3, r3, #188
	str	r3, [r2, #16]
.L938:
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
.L946:
	cbnz	r0, .L949
	bx	lr
.L949:
	cmp	r0, r1
	ldr	r3, [r0, #12]
	bne	.L950
	ldr	r2, [r0, #16]
	str	r3, [r2, #12]
	ldr	r3, [r0, #12]
	cbz	r3, .L947
	str	r2, [r3, #16]
.L947:
	movs	r3, #0
	strd	r3, r3, [r0, #12]
	bx	lr
.L950:
	mov	r0, r3
	b	.L946
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
.L955:
	cbnz	r0, .L957
	bx	lr
.L957:
	cmp	r0, r1
	beq	.L958
	ldr	r0, [r0, #12]
	b	.L955
.L958:
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
	cbz	r3, .L959
	ldr	r0, .L964
	bl	NAND_Print
	ldr	r5, .L964+4
	ldr	r4, [r4, #200]
	ldr	r6, .L964+8
.L961:
	cbnz	r4, .L962
.L959:
	pop	{r4, r5, r6, pc}
.L962:
	ldrh	r2, [r4, #6]
	mov	r0, r5
	ldrh	r1, [r4]
	bl	NAND_Print
	ldrh	r2, [r4, #4]
	mov	r0, r6
	ldr	r1, [r4, #8]
	bl	NAND_Print
	ldrh	r1, [r4, #2]
	ldr	r0, .L964+12
	bl	NAND_Print
	ldr	r4, [r4, #12]
	b	.L961
.L965:
	.align	2
.L964:
	.word	.LC114
	.word	.LC111
	.word	.LC115
	.word	.LC113
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
.L967:
	ldr	r3, [r0, #28]
	cbnz	r3, .L969
	str	r1, [r0, #28]
	str	r3, [r1, #28]
	b	.L971
.L969:
	ldr	r4, [r3, #8]
	ldr	r2, [r1, #8]
	cmp	r4, r2
	blt	.L970
	str	r1, [r3, #32]
	str	r3, [r1, #28]
	str	r1, [r0, #28]
.L971:
	str	r0, [r1, #32]
	movs	r0, #0
	pop	{r4, pc}
.L970:
	mov	r0, r3
	b	.L967
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
	cbz	r2, .L972
	ldr	r2, [r2, #28]
	str	r2, [r3, #252]
.L972:
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
.L978:
	cbnz	r0, .L981
	bx	lr
.L981:
	cmp	r0, r1
	ldr	r3, [r0, #28]
	bne	.L982
	ldr	r2, [r0, #32]
	str	r3, [r2, #28]
	ldr	r3, [r0, #28]
	cbz	r3, .L979
	str	r2, [r3, #32]
.L979:
	movs	r3, #0
	strd	r3, r3, [r0, #28]
	bx	lr
.L982:
	mov	r0, r3
	b	.L978
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
	cbz	r3, .L986
	ldr	r0, .L991
	bl	NAND_Print
	ldr	r5, .L991+4
	ldr	r4, [r4, #252]
	ldr	r6, .L991+8
.L988:
	cbnz	r4, .L989
.L986:
	pop	{r4, r5, r6, pc}
.L989:
	ldrh	r1, [r4]
	mov	r0, r5
	bl	NAND_Print
	ldr	r1, [r4, #8]
	mov	r0, r6
	bl	NAND_Print
	ldr	r4, [r4, #28]
	b	.L988
.L992:
	.align	2
.L991:
	.word	.LC116
	.word	.LC117
	.word	.LC118
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
	cbnz	r0, .L994
	ldrh	r3, [r5, #22]
	adds	r3, r3, #1
	strh	r3, [r5, #22]	@ movhi
.L993:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L994:
	ldr	r0, .L996
	bl	NAND_Print
	b	.L993
.L997:
	.align	2
.L996:
	.word	.LC119
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
	cbz	r0, .L998
	movs	r3, #0
	strd	r3, r3, [r0, #20]
	ldrh	r3, [r1, #22]
	cbz	r3, .L998
	subs	r3, r3, #1
	strh	r3, [r1, #22]	@ movhi
.L998:
	pop	{r3, pc}
	.size	out_phy_block_from_free_list, .-out_phy_block_from_free_list
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
.L1006:
	cbnz	r3, .L1008
	cbz	r2, .L1010
	ldrh	r0, [r2, #6]
.L1009:
	pop	{r4, pc}
.L1008:
	ldrh	r4, [r3, #6]
	cmp	r4, r1
	bcs	.L1007
	cmp	r4, r0
	itt	cs
	movcs	r0, r4
	movcs	r2, r3
.L1007:
	ldr	r3, [r3, #20]
	b	.L1006
.L1010:
	movw	r0, #65535
	b	.L1009
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
.L1012:
	cbnz	r3, .L1014
	pop	{r4, pc}
.L1014:
	ldrh	r4, [r3, #6]
	cmp	r4, r1
	bcs	.L1013
	cmp	r4, r2
	itt	cs
	movcs	r2, r4
	movcs	r0, r3
.L1013:
	ldr	r3, [r3, #20]
	b	.L1012
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
.L1016:
	cbnz	r3, .L1018
	pop	{r4, pc}
.L1018:
	ldrh	r4, [r3, #6]
	cmp	r4, r1
	bcs	.L1017
	cmp	r4, r2
	itt	cs
	movcs	r2, r4
	movcs	r0, r3
.L1017:
	ldr	r3, [r3, #12]
	b	.L1016
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
.L1020:
	cbnz	r3, .L1022
	pop	{r4, r5, r6, pc}
.L1022:
	ldrh	r4, [r3, #6]
	cmp	r4, r1
	bcs	.L1021
	ldr	r6, [r3, #8]
	cmp	r6, r2
	bcs	.L1021
	cmp	r4, r5
	itt	ls
	movls	r5, r4
	movls	r0, r3
.L1021:
	ldr	r3, [r3, #12]
	b	.L1020
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
.L1024:
	cbnz	r3, .L1026
	bx	lr
.L1026:
	ldr	r1, [r3, #8]
	cmp	r1, r2
	itt	cs
	movcs	r0, r3
	movcs	r2, r1
	ldr	r3, [r3, #12]
	b	.L1024
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
.L1028:
	cbnz	r3, .L1030
	bx	lr
.L1030:
	ldr	r1, [r3, #8]
	cmp	r1, r2
	itt	ls
	movls	r0, r3
	movls	r2, r1
	ldr	r3, [r3, #12]
	b	.L1028
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
	cbz	r0, .L1031
	movs	r3, #0
	strd	r3, r3, [r0, #20]
	ldrh	r3, [r4, #22]
	cbz	r3, .L1031
	subs	r3, r3, #1
	strh	r3, [r4, #22]	@ movhi
.L1031:
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
.L1039:
	cbnz	r3, .L1041
	cbz	r2, .L1038
	mov	r1, r2
	pop	{r4, r5}
	b	out_phy_block_from_free_list_by_block
.L1041:
	ldrh	r5, [r3, #6]
	cmp	r5, r1
	bcs	.L1040
	cmp	r5, r4
	itt	cs
	movcs	r4, r5
	movcs	r2, r3
.L1040:
	ldr	r3, [r3, #20]
	b	.L1039
.L1038:
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
	cbz	r0, .L1043
	ldr	r0, .L1048
	bl	NAND_Print
.L1043:
	mov	r0, r4
	pop	{r4, pc}
.L1049:
	.align	2
.L1048:
	.word	.LC120
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
	cbz	r0, .L1050
	movs	r3, #0
	strd	r3, r3, [r0, #12]
.L1050:
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
	cbz	r0, .L1055
	movs	r3, #0
	strd	r3, r3, [r0, #12]
.L1055:
	pop	{r3, pc}
	.size	out_phy_block_from_invalid_page_list_by_block, .-out_phy_block_from_invalid_page_list_by_block
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
	cbnz	r1, .L1061
	ldr	r1, [r4, #16]
	cbnz	r1, .L1061
	cmp	r2, r3
	strh	r2, [r4, #4]	@ movhi
	bls	.L1063
	ldrh	r1, [r4]
	ldr	r0, .L1075
	bl	NAND_Print
.L1063:
	movs	r0, #0
	pop	{r3, r4, r5, pc}
.L1061:
	cmp	r2, r3
	strh	r2, [r4, #4]	@ movhi
	bls	.L1064
	ldr	r0, .L1075+4
	bl	NAND_Print
.L1064:
	ldr	r3, [r4, #16]
	adds	r5, r5, #188
	cmp	r3, r5
	beq	.L1063
	ldrh	r1, [r4, #4]
.L1066:
	ldrh	r2, [r3, #4]
	cmp	r2, r1
	bcs	.L1063
	ldr	r2, [r3, #16]
	str	r2, [r4, #16]
	ldr	r2, [r3, #16]
	str	r4, [r2, #12]
	ldr	r2, [r4, #12]
	str	r2, [r3, #12]
	ldr	r2, [r4, #12]
	cbz	r2, .L1067
	str	r3, [r2, #16]
.L1067:
	str	r3, [r4, #12]
	str	r4, [r3, #16]
	ldr	r3, [r4, #16]
	ldr	r2, [r3, #16]
	cmp	r2, #0
	bne	.L1066
	b	.L1063
.L1076:
	.align	2
.L1075:
	.word	.LC121
	.word	.LC122
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
	push	{r4, lr}
	mov	r4, r0
.L1078:
	ldr	r3, [r4, #200]
	cbnz	r3, .L1080
.L1079:
	movs	r0, #0
	pop	{r4, pc}
.L1080:
	ldr	r1, [r4, #4]
	ldrh	r2, [r3, #4]
	ldrh	r3, [r1, #20]
	cmp	r2, r3
	bne	.L1079
	mov	r0, r4
	bl	out_phy_block_from_invalid_page_list
	mov	r1, r0
	mov	r0, r4
	bl	put_phy_block_to_free_list
	b	.L1078
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
	ldr	r7, .L1088
	mov	r2, r1
	mov	r5, r0
	mov	r6, r1
	ldr	r8, .L1088+16
	mov	r1, r0
	ldr	r0, .L1088+4
	bl	printf
.L1082:
	cmp	r6, r4
	bne	.L1086
	ldr	r0, .L1088+8
	pop	{r4, r5, r6, r7, r8, lr}
	b	printf
.L1086:
	lsls	r2, r4, #28
	bne	.L1083
	ldrb	r2, [r5]	@ zero_extendqisi2
	mov	r1, r4
	mov	r0, r7
	bl	printf
.L1083:
	adds	r4, r4, #1
	ldrb	r1, [r5], #1	@ zero_extendqisi2
	lsls	r3, r4, #28
	ite	eq
	ldreq	r0, .L1088+12
	movne	r0, r8
	bl	printf
	b	.L1082
.L1089:
	.align	2
.L1088:
	.word	.LC124
	.word	.LC123
	.word	.LC26
	.word	.LC125
	.word	.LC61
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
	ldr	r0, .L1097
	bl	NAND_Print_DBG
	cmp	r6, r5
	bcc	.L1091
	ldr	r0, .L1097+4
	bl	NAND_Print
.L1096:
	movs	r0, #1
.L1090:
	pop	{r4, r5, r6, pc}
.L1091:
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
	bcs	.L1093
	mov	r2, r6
	mov	r1, r5
	ldr	r0, .L1097+8
	bl	NAND_Print
	b	.L1096
.L1093:
	ldr	r0, .L1097+12
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
	b	.L1090
.L1098:
	.align	2
.L1097:
	.word	.LC126
	.word	.LC127
	.word	.LC128
	.word	-858993459
	.size	init_zone_after_first_scan, .-init_zone_after_first_scan
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
	ldr	r3, [r0, #1976]
	blx	r3
	ldrh	r3, [r7, #6]
	strh	r5, [r7, #4]	@ movhi
	adds	r3, r3, #1
	strh	r3, [r7, #6]	@ movhi
	cmp	r0, #0
	beq	.L1104
	ldr	r0, .L1109
	bl	NAND_Print
	ldr	r3, [r4, #1996]
	add	r1, sp, #8
	mov	r0, r4
	blx	r3
	movs	r0, #1
.L1099:
	add	sp, sp, #24
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1103:
	uxth	r3, r6
	mov	r2, r7
	mov	r1, r10
	ldr	r5, [r4, #1988]
	str	r3, [sp]
	mov	r0, r4
	ldr	r3, [r4, #284]
	blx	r5
	cbz	r0, .L1102
	mov	r0, fp
	bl	NAND_Print
.L1102:
	adds	r6, r6, #1
.L1100:
	cmp	r8, r6
	bhi	.L1103
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
	ldr	r3, [r4, #1996]
	blx	r3
	cmp	r8, #0
	bne	.L1105
	str	r7, [r4, #292]
	mov	r0, r8
	strh	r8, [r4, #320]	@ movhi
	b	.L1099
.L1104:
	mov	r6, r0
	ldr	fp, .L1109+4
	b	.L1100
.L1105:
	mov	r0, r5
	b	.L1099
.L1110:
	.align	2
.L1109:
	.word	.LC129
	.word	.LC130
	.size	do_write_error_in_build_list, .-do_write_error_in_build_list
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
.L1112:
	subs	r4, r3, r0
	cmp	r2, r4
	bgt	.L1114
	movs	r0, #0
.L1111:
	pop	{r4, pc}
.L1114:
	ldrb	r4, [r3], #1	@ zero_extendqisi2
	cmp	r4, r1
	beq	.L1112
	movs	r0, #1
	b	.L1111
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
.L1118:
	cmp	r5, r8
	bcc	.L1121
.L1117:
	mov	r0, r6
	add	sp, sp, #16
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L1121:
	ldrh	r3, [r4]
	mov	r1, sp
	mov	r0, r7
	strh	r3, [sp, #2]	@ movhi
	ldr	r3, [r7, #1992]
	blx	r3
	cmp	r0, #159
	bne	.L1119
	subs	r6, r6, #1
	beq	.L1117
.L1119:
	mov	r1, r7
	mov	r0, r4
	bl	is_last_phy_block
	cmp	r0, #1
	beq	.L1117
	adds	r4, r4, #36
	adds	r5, r5, #1
	b	.L1118
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
.L1131:
	ldr	r3, [r5, #4]
	ldrh	r2, [r3, #20]
	cmp	r2, r4
	bls	.L1133
	ldrh	r3, [r3, #28]
	add	r1, sp, #16
	ldrh	r2, [r6]
	mov	r0, r5
	strh	r4, [sp, #16]	@ movhi
	strh	r3, [sp, #20]	@ movhi
	strh	r2, [sp, #18]	@ movhi
	str	r7, [sp, #24]
	str	sp, [sp, #28]
	ldr	r3, [r5, #1980]
	blx	r3
	cbnz	r0, .L1132
	ldrb	r3, [sp, #14]	@ zero_extendqisi2
	cmp	r3, #99
	bhi	.L1132
	ldr	r2, [r5, #1968]
	ldr	r1, [r2, #172]
	cmp	r1, r3
	bhi	.L1132
	str	r3, [r2, #172]
.L1133:
	movs	r0, #0
	add	sp, sp, #36
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L1132:
	adds	r4, r4, #1
	b	.L1131
	.size	check_cross_talk, .-check_cross_talk
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
	sub	sp, sp, #36
	mov	r0, #4096
	mov	r6, r1
	mov	r7, r2
	bl	NAND_Malloc
	mov	r4, r0
	cbnz	r0, .L1136
	ldr	r1, .L1137
	ldr	r0, .L1137+4
	bl	NAND_Print
.L1136:
	add	r3, sp, #16
	mov	r2, r7
	strd	r4, r3, [sp]
	mov	r1, r6
	movs	r3, #8
	mov	r0, r5
	bl	PageRead
	mov	r5, r0
	ldrb	r0, [sp, #20]	@ zero_extendqisi2
	ldrb	r3, [sp, #18]	@ zero_extendqisi2
	ldrb	r2, [sp, #17]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [sp, #19]	@ zero_extendqisi2
	ldrb	r1, [sp, #16]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L1137+8
	bl	NAND_Print
	ldrb	r0, [sp, #25]	@ zero_extendqisi2
	ldrb	r3, [sp, #23]	@ zero_extendqisi2
	ldrb	r2, [sp, #22]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [sp, #24]	@ zero_extendqisi2
	ldrb	r1, [sp, #21]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L1137+12
	bl	NAND_Print
	ldrb	r0, [r4, #4]	@ zero_extendqisi2
	ldrb	r3, [r4, #2]	@ zero_extendqisi2
	ldrb	r2, [r4, #1]	@ zero_extendqisi2
	ldrb	r1, [r4]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [r4, #3]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L1137+16
	bl	NAND_Print
	ldrb	r0, [r4, #10]	@ zero_extendqisi2
	ldrb	r3, [r4, #7]	@ zero_extendqisi2
	ldrb	r2, [r4, #6]	@ zero_extendqisi2
	ldrb	r1, [r4, #5]	@ zero_extendqisi2
	str	r0, [sp, #8]
	ldrb	r0, [r4, #9]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [r4, #8]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L1137+20
	bl	NAND_Print
	mov	r0, r4
	bl	nftl_free
	mov	r0, r5
	add	sp, sp, #36
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L1138:
	.align	2
.L1137:
	.word	.LANCHOR83
	.word	.LC131
	.word	.LC132
	.word	.LC133
	.word	.LC134
	.word	.LC135
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
	sub	sp, sp, #52
	mov	r0, #4096
	mov	r7, r1
	mov	r6, r2
	bl	NAND_Malloc
	mov	r4, r0
	cbnz	r0, .L1140
	ldr	r1, .L1141
	ldr	r0, .L1141+4
	bl	NAND_Print
.L1140:
	ldr	r3, [r5, #4]
	movs	r1, #36
	muls	r1, r7, r1
	mov	r0, r5
	ldr	r3, [r3, #32]
	ldrh	r3, [r3, r1]
	add	r1, sp, #16
	strh	r6, [sp, #16]	@ movhi
	str	r4, [sp, #24]
	strh	r3, [sp, #18]	@ movhi
	movs	r3, #8
	strh	r3, [sp, #20]	@ movhi
	add	r3, sp, #32
	str	r3, [sp, #28]
	ldr	r3, [r5, #1980]
	blx	r3
	mov	r5, r0
	ldrb	r0, [sp, #36]	@ zero_extendqisi2
	ldrb	r3, [sp, #34]	@ zero_extendqisi2
	ldrb	r2, [sp, #33]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [sp, #35]	@ zero_extendqisi2
	ldrb	r1, [sp, #32]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L1141+8
	bl	NAND_Print
	ldrb	r0, [sp, #41]	@ zero_extendqisi2
	ldrb	r3, [sp, #39]	@ zero_extendqisi2
	ldrb	r2, [sp, #38]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [sp, #40]	@ zero_extendqisi2
	ldrb	r1, [sp, #37]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L1141+12
	bl	NAND_Print
	ldrb	r0, [r4, #4]	@ zero_extendqisi2
	ldrb	r3, [r4, #2]	@ zero_extendqisi2
	ldrb	r2, [r4, #1]	@ zero_extendqisi2
	ldrb	r1, [r4]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [r4, #3]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L1141+16
	bl	NAND_Print
	ldrb	r0, [r4, #10]	@ zero_extendqisi2
	ldrb	r3, [r4, #7]	@ zero_extendqisi2
	ldrb	r2, [r4, #6]	@ zero_extendqisi2
	ldrb	r1, [r4, #5]	@ zero_extendqisi2
	str	r0, [sp, #8]
	ldrb	r0, [r4, #9]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [r4, #8]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L1141+20
	bl	NAND_Print
	mov	r0, r4
	bl	nftl_free
	mov	r0, r5
	add	sp, sp, #52
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L1142:
	.align	2
.L1141:
	.word	.LANCHOR84
	.word	.LC131
	.word	.LC132
	.word	.LC133
	.word	.LC134
	.word	.LC135
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
	cbnz	r0, .L1144
	ldr	r1, .L1145
	ldr	r0, .L1145+4
	bl	NAND_Print
.L1144:
	ldr	r3, [r5, #4]
	movs	r2, #36
	muls	r6, r2, r6
	mov	r0, r4
	mov	r2, #4096
	movs	r1, #165
	ldr	r8, [r3, #32]
	bl	NAND_Memset
	ldr	r2, .L1145+8
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
	ldr	r3, [r5, #1984]
	blx	r3
	mov	r5, r0
	mov	r0, r4
	bl	nftl_free
	mov	r0, r5
	add	sp, sp, #32
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L1146:
	.align	2
.L1145:
	.word	.LANCHOR85
	.word	.LC136
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
	cbnz	r0, .L1148
	ldr	r1, .L1149
	ldr	r0, .L1149+4
	bl	NAND_Print
.L1148:
	mov	r0, r4
	mov	r2, #4096
	movs	r1, #165
	bl	NAND_Memset
	ldr	r2, .L1149+8
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
	bl	PageWrite
	mov	r5, r0
	mov	r0, r4
	bl	nftl_free
	mov	r0, r5
	add	sp, sp, #28
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L1150:
	.align	2
.L1149:
	.word	.LANCHOR86
	.word	.LC136
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
	b	BlockErase
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
.L1155:
	mov	r0, r5
	bl	out_phy_block_from_free_list
	cbnz	r0, .L1158
	mov	r1, r0
	mov	r4, #-1
.L1154:
	mov	r0, r5
	bl	out_phy_block_from_invalid_page_list
	cbz	r0, .L1153
	strh	r1, [r0, #4]	@ movhi
	strh	r1, [r0, #2]	@ movhi
	strd	r4, r1, [r0, #8]
	strd	r1, r1, [r0, #16]
	strd	r1, r1, [r0, #24]
	str	r1, [r0, #32]
	b	.L1154
.L1158:
	strh	r4, [r0, #4]	@ movhi
	strh	r4, [r0, #2]	@ movhi
	strd	r6, r4, [r0, #8]
	strd	r4, r4, [r0, #16]
	strd	r4, r4, [r0, #24]
	str	r4, [r0, #32]
	b	.L1155
.L1153:
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
	ldr	fp, .L1168+4
	mov	r2, r0
	ldrb	r1, [r4, #16]	@ zero_extendqisi2
	lsrs	r7, r6, #20
	ldr	r0, .L1168
	add	r10, r8, r7, lsl #2
	bl	NAND_Print
.L1160:
	cmp	r8, r10
	bne	.L1161
	ubfx	r6, r6, #0, #20
	cbz	r6, .L1162
	add	r7, r4, r7, lsl #2
	movs	r1, #255
	mov	r2, r6
	ldr	r0, [r7, #52]
	bl	NAND_Memset
	mov	r1, r6
	ldr	r0, .L1168+4
	bl	NAND_Print
.L1162:
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
.L1163:
	strh	r3, [r5, #396]	@ movhi
	adds	r3, r3, #1
	cmp	r3, #10
	strh	r2, [r5, #398]	@ movhi
	strd	r2, r2, [r5, #400]
	add	r5, r5, #16
	str	r2, [r5, #392]
	bne	.L1163
	movs	r3, #255
	strh	r2, [r4, #394]	@ movhi
	strd	r3, r2, [r4, #556]
	movs	r0, #0
	strd	r2, r2, [r4, #564]
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1161:
	ldr	r0, [r8], #4
	mov	r2, #1048576
	movs	r1, #255
	bl	NAND_Memset
	mov	r1, #1048576
	mov	r0, fp
	bl	NAND_Print
	b	.L1160
.L1169:
	.align	2
.L1168:
	.word	.LC137
	.word	.LC138
	.size	nand_clean_zone_table2, .-nand_clean_zone_table2
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
.L1171:
	cmp	r3, r4
	bhi	.L1172
	pop	{r4, r5, r6, pc}
.L1172:
	subs	r5, r6, r4
	lsls	r5, r5, #3
	lsr	r5, r1, r5
	strb	r5, [r2, r4]
	adds	r4, r4, #1
	b	.L1171
	.size	set_spare_data, .-set_spare_data
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
.L1174:
	cmp	r2, r3
	bhi	.L1175
	mov	r0, r4
	pop	{r4, pc}
.L1175:
	ldrb	r0, [r1, r3]	@ zero_extendqisi2
	adds	r3, r3, #1
	orr	r4, r0, r4, lsl #8
	b	.L1174
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
	ldr	r3, [r4, #1984]
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
	ldr	r3, [r4, #1984]
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
	ldr	r3, [r4, #1984]
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
	ldr	r3, [r4, #1984]
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
	ldr	r0, .L1182
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
	ldr	r3, [r5, #1976]
	blx	r3
	mov	r7, r0
	cbz	r0, .L1180
	ldrh	r1, [r4]
	ldr	r0, .L1182+4
	bl	NAND_Print
	ldr	r3, [r5, #1996]
	mov	r1, sp
	mov	r0, r5
	blx	r3
	movs	r3, #255
	strh	r6, [r4, #4]	@ movhi
	strh	r3, [r4, #2]	@ movhi
	ldrh	r3, [r5, #20]
	adds	r3, r3, #1
	strh	r3, [r5, #20]	@ movhi
.L1179:
	mov	r0, r7
	add	sp, sp, #16
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L1180:
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
	ldrh	r3, [r3, #20]
	strh	r7, [r4, #2]	@ movhi
	strh	r3, [r4, #4]	@ movhi
	b	.L1179
.L1183:
	.align	2
.L1182:
	.word	.LC139
	.word	.LC140
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
	cbnz	r0, .L1185
	ldr	r0, .L1186
	bl	NAND_Print
.L1185:
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
	ldr	r0, .L1186+4
	bl	NAND_Print
	movs	r0, #0
	pop	{r3, r4, r5, pc}
.L1187:
	.align	2
.L1186:
	.word	.LC141
	.word	.LC142
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
	ldr	r0, .L1205
	bl	NAND_Print_DBG
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
	ldr	r3, [r4, #1976]
	blx	r3
	mov	r6, r0
	cbz	r0, .L1190
.L1202:
	ldrh	r1, [r5]
	ldr	r0, .L1205+4
	bl	NAND_Print
	b	.L1191
.L1190:
	ldrh	r3, [r5, #6]
	movs	r1, #165
	ldr	r0, [r4, #284]
	mov	r8, r6
	adds	r3, r3, #1
	strh	r3, [r5, #6]	@ movhi
	ldr	r3, [r4, #4]
	ldrh	r2, [r3, #26]
	bl	memset
	movs	r2, #15
	movs	r1, #165
	mov	r0, r7
	bl	memset
.L1193:
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
	cbz	r0, .L1192
	ldrh	r2, [r5]
	mov	r3, r8
	ldr	r1, .L1205+8
	ldr	r0, .L1205+12
.L1203:
	bl	NAND_Print
.L1191:
	ldr	r3, [r4, #1996]
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
	b	.L1189
.L1192:
	add	r8, r8, #1
	cmp	r8, #3
	bne	.L1193
	mov	r8, r0
.L1197:
	ldr	r3, [r4, #4]
	movs	r1, #0
	ldr	r0, [r4, #284]
	ldrh	r2, [r3, #26]
	bl	memset
	movs	r2, #15
	movs	r1, #0
	mov	r0, r7
	bl	memset
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
	ldr	r3, [r4, #1980]
	blx	r3
	mov	r6, r0
	cbz	r0, .L1194
	mov	r3, r8
	ldrh	r2, [r5]
	ldr	r1, .L1205+8
	ldr	r0, .L1205+16
	b	.L1203
.L1194:
	ldr	r10, [r4, #4]
	movs	r1, #165
	ldr	r0, [r4, #284]
	ldrh	r2, [r10, #26]
	bl	nftl_check_pattern
	mov	r6, r0
	cbz	r0, .L1195
	ldrh	r2, [r5]
	movs	r3, #165
	ldr	r1, .L1205+8
	ldr	r0, .L1205+20
	str	r3, [sp]
	mov	r3, r8
.L1204:
	bl	NAND_Print
	b	.L1191
.L1195:
	movs	r2, #15
	movs	r1, #165
	mov	r0, r7
	bl	nftl_check_pattern
	mov	r6, r0
	cbz	r0, .L1196
	movs	r3, #165
	ldrh	r2, [r5]
	str	r3, [sp]
	mov	r3, r8
	ldr	r1, .L1205+8
	ldr	r0, .L1205+24
	b	.L1204
.L1196:
	add	r8, r8, #1
	cmp	r8, #3
	bne	.L1197
	ldrh	r3, [r10, #28]
	add	r1, sp, #24
	ldrh	r2, [r5]
	strh	r0, [sp, #24]	@ movhi
	strh	r3, [sp, #28]	@ movhi
	strd	r0, r0, [sp, #32]
	mov	r0, r4
	strh	r2, [sp, #26]	@ movhi
	ldr	r3, [r4, #1976]
	blx	r3
	mov	r6, r0
	cmp	r0, #0
	bne	.L1202
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
.L1189:
	mov	r0, r6
	add	sp, sp, #44
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L1206:
	.align	2
.L1205:
	.word	.LC139
	.word	.LC140
	.word	.LANCHOR87
	.word	.LC143
	.word	.LC144
	.word	.LC145
	.word	.LC146
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
	ldr	r0, .L1236
	mov	r10, r1
	bl	NAND_Print_DBG
	ldr	r3, [r4, #24]
	ldr	r5, [r4, #40]
	cmp	r3, #0
	beq	.L1231
	ldr	r6, [r4, #4]
	ldr	r0, [r4, #32]
	ldrb	r1, [r6, #8]	@ zero_extendqisi2
	add	r0, r0, r3
	bl	__aeabi_uidiv
	ldrh	r1, [r6, #20]
	bl	__aeabi_uidiv
	mov	r8, r0
.L1211:
	movs	r7, #0
	strh	r7, [r4, #18]	@ movhi
.L1212:
	ldrh	r3, [r4, #18]
	cmp	r3, r8
	bcs	.L1222
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
	ldr	r3, [r4, #1992]
	blx	r3
	cmp	r0, #1
	bne	.L1213
	ldr	r3, [r4, #1976]
	add	r1, sp, #16
	mov	r0, r4
	blx	r3
	mov	r6, r0
	cbz	r0, .L1214
	movs	r3, #255
	add	r1, sp, #16
	strh	r3, [r5, #2]	@ movhi
	mov	r0, r4
	ldr	r3, [r4, #1996]
	blx	r3
.L1215:
	movw	r3, #65535
	str	r7, [r5, #20]
	strh	r3, [r5, #4]	@ movhi
	mov	r1, r4
	strd	r7, r7, [r5, #12]
	mov	r0, r5
	str	r5, [r4, #44]
	bl	is_last_phy_block
	cmp	r0, #1
	bne	.L1218
.L1222:
	ldrh	r1, [r4, #18]
	cmp	r1, #1
	bhi	.L1219
	ldr	r0, .L1236+4
	bl	NAND_Print
	movs	r0, #1
	b	.L1210
.L1231:
	mov	r8, #-1
	b	.L1211
.L1214:
	ldrh	r3, [r4, #18]
	adds	r3, r3, #1
	uxth	r3, r3
	cmp	r3, #1
	strh	r3, [r4, #18]	@ movhi
	strh	r0, [r5, #2]	@ movhi
	strh	r0, [r5, #6]	@ movhi
	bne	.L1215
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
.L1216:
	ldr	r3, [r4, #4]
	ldrh	r3, [r3, #20]
	cmp	r3, r6
	bhi	.L1217
	add	r3, r5, #36
	str	r3, [r4, #40]
	b	.L1215
.L1217:
	ldr	r3, [r4, #1984]
	add	r1, sp, #16
	mov	r0, r4
	adds	r6, r6, #1
	blx	r3
	ldrh	r3, [sp, #16]
	adds	r3, r3, #1
	strh	r3, [sp, #16]	@ movhi
	b	.L1216
.L1213:
	movs	r3, #255
	strh	r3, [r5, #2]	@ movhi
	b	.L1215
.L1218:
	adds	r5, r5, #36
	b	.L1212
.L1219:
	cmp	r8, #-1
	add	r1, r1, #-1
	strh	r1, [r4, #18]	@ movhi
	bne	.L1223
.L1225:
	movs	r0, #0
.L1210:
	add	sp, sp, #36
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L1223:
	mov	r1, r4
	mov	r0, r5
	bl	is_last_phy_block
	cmp	r0, #1
	beq	.L1225
	movs	r7, #0
.L1226:
	ldr	r2, [r4, #4]
	ldrh	r3, [r5]
	ldrh	r1, [r2, #24]
	cmp	r1, r3
	bls	.L1225
	ldrh	r1, [r2, #28]
	mov	r0, r4
	ldr	r2, [r4, #284]
	strh	r3, [sp, #18]	@ movhi
	strh	r1, [sp, #20]	@ movhi
	add	r1, sp, #16
	strh	r7, [sp, #16]	@ movhi
	str	r2, [sp, #24]
	str	sp, [sp, #28]
	ldr	r3, [r4, #1992]
	blx	r3
	cmp	r0, #1
	bne	.L1227
	ldr	r3, [r4, #1976]
	add	r1, sp, #16
	mov	r0, r4
	blx	r3
	mov	r6, r0
	cbz	r0, .L1228
	movs	r3, #255
	add	r1, sp, #16
	strh	r3, [r5, #2]	@ movhi
	mov	r0, r4
	ldr	r3, [r4, #1996]
	blx	r3
.L1229:
	adds	r5, r5, #36
	b	.L1226
.L1228:
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
	ldr	r3, [r4, #1984]
	blx	r3
	b	.L1225
.L1227:
	movs	r3, #255
	strh	r3, [r5, #2]	@ movhi
	b	.L1229
.L1237:
	.align	2
.L1236:
	.word	.LC147
	.word	.LC148
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
	ldr	r2, [r0, #4]
	push	{r4, r5, lr}
	sub	sp, sp, #36
	ldrh	r4, [r1]
	movs	r5, #0
	ldrh	r1, [r2, #28]
	ldr	r2, [r0, #284]
	strh	r4, [sp, #2]	@ movhi
	add	r4, sp, #16
	strh	r1, [sp, #4]	@ movhi
	mov	r1, sp
	str	r2, [sp, #8]
	ldr	r3, [r0, #1980]
	strh	r5, [sp]	@ movhi
	str	r4, [sp, #12]
	blx	r3
	mov	r0, r4
	bl	get_block_used_count_from_oob
	add	sp, sp, #36
	@ sp needed
	pop	{r4, r5, pc}
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
	ldr	r3, [r0, #1980]
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
	ldr	r3, [r4, #1984]
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
	bne	.L1248
	ldrb	r0, [r0, #2]	@ zero_extendqisi2
	sub	r3, r0, #238
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	bx	lr
.L1248:
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
	beq	.L1259
	ldr	r7, [r0, #4]
	ldr	r0, [r0, #32]
	ldrb	r1, [r7, #8]	@ zero_extendqisi2
	add	r0, r0, r3
	bl	__aeabi_uidiv
	ldrh	r1, [r7, #20]
	bl	__aeabi_uidiv
	mov	r7, r0
.L1250:
	movs	r3, #0
	mov	r8, r3
	strh	r3, [r4, #18]	@ movhi
.L1251:
	ldrh	r3, [r4, #18]
	cmp	r3, r7
	bcs	.L1266
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
	ldr	r3, [r4, #1992]
	blx	r3
	cmp	r0, #1
	bne	.L1252
	ldr	r3, [r4, #1980]
	add	r1, sp, #16
	mov	r0, r4
	blx	r3
	mov	r2, r0
	mov	r0, sp
	bl	is_ftl_start_flag_page
	cmp	r0, #1
	beq	.L1253
.L1266:
	movs	r0, #1
.L1249:
	add	sp, sp, #32
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L1259:
	mov	r7, #-1
	b	.L1250
.L1253:
	cbz	r2, .L1255
	cmp	r2, #10
	bne	.L1266
.L1255:
	movs	r2, #4
	movs	r1, #3
	mov	r0, sp
	bl	get_spare_data
	cmp	r0, r6
	beq	.L1257
	ldr	r0, .L1267
	bl	NAND_Print
	b	.L1266
.L1257:
	adds	r5, r5, #36
	movs	r0, #0
	str	r5, [r4, #40]
	b	.L1249
.L1252:
	movs	r3, #255
	mov	r1, r4
	strh	r3, [r5, #2]	@ movhi
	mov	r0, r5
	ldrh	r3, [r4, #18]
	adds	r3, r3, #1
	strh	r3, [r4, #18]	@ movhi
	bl	is_last_phy_block
	cmp	r0, #1
	beq	.L1266
	adds	r5, r5, #36
	b	.L1251
.L1268:
	.align	2
.L1267:
	.word	.LC149
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
	bne	.L1271
	ldrb	r0, [r0, #2]	@ zero_extendqisi2
	sub	r3, r0, #204
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	bx	lr
.L1271:
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
.L1276:
	ldrb	r4, [r2, #1]!	@ zero_extendqisi2
	cmp	r4, #255
	it	eq
	addeq	r3, r3, #1
	cmp	r2, r1
	bne	.L1276
	cmp	r3, #8
	bls	.L1278
	ldrb	r0, [r0]	@ zero_extendqisi2
	sub	r3, r0, #255
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
.L1274:
	pop	{r4, pc}
.L1278:
	movs	r0, #0
	b	.L1274
	.size	is_nouse_page, .-is_nouse_page
	.section	.text.m7_read_page_end,"ax",%progbits
	.align	1
	.global	m7_read_page_end
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m7_read_page_end, %function
m7_read_page_end:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L1291
	push	{r0, r1, r2, r4, r5, r6, r7, r8, r10, lr}
	mov	r6, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r5, r0
	mov	r0, r6
	bl	m0_read_page_end_not_retry
	adds	r3, r0, #2
	mov	r4, r0
	bne	.L1280
	ldr	r8, .L1291+12
	movs	r7, #0
	ldr	r10, .L1291+16
	ldr	r0, .L1291+4
	bl	NAND_Print_DBG
.L1282:
	ldrb	r3, [r8]	@ zero_extendqisi2
	cmp	r3, r7
	ble	.L1286
	str	r7, [r5, #176]
	mov	r0, r5
	ldrh	r1, [r6, #8]
	bl	m7_set_readretry
	mov	r4, r0
	cbnz	r0, .L1283
	mov	r0, r6
	bl	m0_read_page_start
	mov	r4, r0
	mov	r0, r6
	bl	m0_read_page_end_not_retry
	orrs	r4, r4, r0
	mov	r0, r5
	bl	m7_end_one_read_retry
	ldr	r0, [r6, #20]
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L1284
	ldr	r3, [r5, #176]
	mvn	r4, #1
	ldr	r1, [r5, #248]
	mov	r0, r10
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
.L1283:
	adds	r7, r7, #1
	b	.L1282
.L1284:
	cmp	r4, #10
	beq	.L1285
	cmp	r4, #0
	bne	.L1283
.L1285:
	ldr	r3, [r5, #176]
	movs	r4, #10
	ldr	r1, [r5, #248]
	ldr	r0, .L1291+8
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
.L1286:
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	m7_exit_readretry
.L1280:
	mov	r0, r4
	add	sp, sp, #12
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L1292:
	.align	2
.L1291:
	.word	g_nsi
	.word	.LC150
	.word	.LC152
	.word	.LANCHOR22
	.word	.LC151
	.size	m7_read_page_end, .-m7_read_page_end
	.section	.text.m2_read_page_end,"ax",%progbits
	.align	1
	.global	m2_read_page_end
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m2_read_page_end, %function
m2_read_page_end:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L1309
	push	{r0, r1, r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r6, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r5, r0
	mov	r0, r6
	bl	m0_read_page_end_not_retry
	adds	r3, r0, #2
	mov	r4, r0
	bne	.L1293
	ldr	r8, .L1309+16
	movs	r7, #0
	ldr	r10, .L1309+20
	ldr	r0, .L1309+4
	bl	NAND_Print_DBG
.L1295:
	ldrb	r2, [r8]	@ zero_extendqisi2
	cmp	r2, r7
	ble	.L1299
	ldr	r3, [r5, #176]
	mov	r0, r5
	adds	r3, r3, #1
	cmp	r3, r2
	it	hi
	movhi	r3, #0
	str	r3, [r5, #176]
	bl	m2_set_readretry
	mov	r4, r0
	cbz	r0, .L1298
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	m2_set_readretry
.L1299:
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	m2_set_readretry
.L1293:
	mov	r0, r4
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1298:
	mov	r0, r6
	bl	m0_read_page_start
	mov	r4, r0
	mov	r0, r6
	bl	m0_read_page_end_not_retry
	ldr	fp, [r6, #20]
	orrs	r4, r4, r0
	mov	r0, fp
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L1300
	ldr	r3, [r5, #176]
	mvn	r4, #1
	ldr	r1, [r5, #248]
	mov	r0, r10
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print
.L1301:
	adds	r7, r7, #1
	b	.L1295
.L1300:
	cmp	r4, #10
	bne	.L1302
.L1303:
	ldrb	r0, [fp, #4]	@ zero_extendqisi2
	movs	r4, #10
	ldrb	r3, [fp, #3]	@ zero_extendqisi2
	ldrb	r2, [fp, #2]	@ zero_extendqisi2
	ldrb	r1, [fp, #1]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L1309+8
	bl	NAND_Print_DBG
	ldr	r3, [r5, #176]
	ldr	r1, [r5, #248]
	ldr	r0, .L1309+12
	str	r3, [sp, #4]
	ldr	r3, [r6, #8]
	str	r3, [sp]
	ldr	r3, [r6, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print
	b	.L1299
.L1302:
	cmp	r4, #0
	bne	.L1301
	b	.L1303
.L1310:
	.align	2
.L1309:
	.word	g_nsi
	.word	.LC153
	.word	.LC154
	.word	.LC155
	.word	.LANCHOR66
	.word	.LC151
	.size	m2_read_page_end, .-m2_read_page_end
	.section	.text.m9_read_page_end,"ax",%progbits
	.align	1
	.global	m9_read_page_end
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	m9_read_page_end, %function
m9_read_page_end:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L1359
	push	{r0, r1, r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r4, r0
	ldr	r1, [r0]
	ldr	r0, [r3]
	bl	nci_get_from_nsi
	mov	r5, r0
	mov	r0, r4
	bl	m0_read_page_end_not_retry
	adds	r3, r0, #2
	mov	r6, r0
	bne	.L1311
	ldr	r8, .L1359+28
	movs	r7, #0
	ldr	r0, .L1359+4
	bl	NAND_Print
	mov	r10, r8
.L1313:
	ldrb	r3, [r8]	@ zero_extendqisi2
	cmp	r3, r7
	bgt	.L1319
	ldr	r7, .L1359+8
	ldrb	r3, [r7]	@ zero_extendqisi2
	cmp	r3, #51
	beq	.L1320
	ldr	fp, .L1359+12
	mov	r8, #0
.L1321:
	ldrb	r3, [r10]	@ zero_extendqisi2
	cmp	r3, r8
	bgt	.L1327
	ldr	fp, .L1359+12
	mov	r8, #0
.L1328:
	ldrb	r3, [r10]	@ zero_extendqisi2
	cmp	r3, r8
	bgt	.L1334
.L1320:
	ldr	fp, .L1359+12
	mov	r8, #0
.L1335:
	ldrb	r3, [r10]	@ zero_extendqisi2
	cmp	r3, r8
	bgt	.L1349
.L1350:
	movs	r3, #0
	mov	r0, r5
	str	r3, [r5, #176]
	bl	m9_exit_readretry
.L1311:
	mov	r0, r6
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1319:
	str	r7, [r5, #176]
	mov	r0, r5
	ldrh	r1, [r4, #8]
	bl	m9_set_readretry
	mov	r6, r0
	cbz	r0, .L1314
	ldr	r0, .L1359+12
	bl	NAND_Print_DBG
.L1315:
	adds	r7, r7, #1
	b	.L1313
.L1314:
	mov	r0, r4
	bl	m0_read_page_start
	mov	r6, r0
	mov	r0, r4
	bl	m0_read_page_end_not_retry
	orrs	r6, r6, r0
	ldr	r0, [r4, #20]
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L1316
	ldr	r3, [r5, #176]
	mvn	r6, #1
	ldr	r1, [r5, #248]
	ldr	r0, .L1359+16
	str	r3, [sp, #4]
	ldr	r3, [r4, #8]
	str	r3, [sp]
	ldr	r3, [r4, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	b	.L1315
.L1316:
	cmp	r6, #10
	beq	.L1347
	cmp	r6, #0
	bne	.L1315
.L1348:
	ldr	r3, [r5, #176]
	movs	r6, #10
	ldr	r1, [r5, #248]
	ldr	r0, .L1359+20
	str	r3, [sp, #4]
	ldr	r3, [r4, #8]
	str	r3, [sp]
	ldr	r3, [r4, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	b	.L1350
.L1327:
	str	r8, [r5, #176]
	mov	r0, r5
	ldrh	r1, [r4, #8]
	bl	m9_set_readretry
	mov	r6, r0
	cbz	r0, .L1322
	mov	r0, fp
	bl	NAND_Print_DBG
.L1323:
	add	r8, r8, #1
	b	.L1321
.L1322:
	mov	r0, r4
	bl	m9_dsp_on
	mov	r0, r4
	bl	m0_read_page_start
	mov	r6, r0
	mov	r0, r4
	bl	m0_read_page_end_not_retry
	orrs	r6, r6, r0
	ldr	r0, [r4, #20]
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L1324
	ldr	r3, [r5, #176]
	mvn	r6, #1
	ldr	r1, [r5, #248]
	ldr	r0, .L1359+16
	str	r3, [sp, #4]
	ldr	r3, [r4, #8]
	str	r3, [sp]
	ldr	r3, [r4, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	b	.L1323
.L1324:
	cmp	r6, #10
	beq	.L1347
	cmp	r6, #0
	bne	.L1323
	b	.L1348
.L1334:
	str	r8, [r5, #176]
	mov	r0, r5
	ldrh	r1, [r4, #8]
	bl	m9_set_readretry
	mov	r6, r0
	cbz	r0, .L1329
	mov	r0, fp
	bl	NAND_Print_DBG
.L1330:
	add	r8, r8, #1
	b	.L1328
.L1329:
	mov	r0, r4
	bl	m9_cmd25
	mov	r0, r4
	bl	m0_read_page_start
	mov	r6, r0
	mov	r0, r4
	bl	m0_read_page_end_not_retry
	orrs	r6, r6, r0
	ldr	r0, [r4, #20]
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L1331
	ldr	r3, [r5, #176]
	mvn	r6, #1
	ldr	r1, [r5, #248]
	ldr	r0, .L1359+16
	str	r3, [sp, #4]
	ldr	r3, [r4, #8]
	str	r3, [sp]
	ldr	r3, [r4, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	b	.L1330
.L1331:
	cmp	r6, #10
	beq	.L1347
	cmp	r6, #0
	bne	.L1330
	b	.L1348
.L1349:
	str	r8, [r5, #176]
	mov	r0, r5
	ldrh	r1, [r4, #8]
	bl	m9_set_readretry
	mov	r6, r0
	cbz	r0, .L1336
	mov	r0, fp
	bl	NAND_Print_DBG
.L1337:
	add	r8, r8, #1
	b	.L1335
.L1336:
	ldrb	r3, [r7]	@ zero_extendqisi2
	cmp	r3, #52
	bne	.L1338
	movs	r1, #198
.L1357:
	mov	r0, r4
	bl	m9_set_lmflgfix_next
	mov	r0, r4
	bl	m9_cmd25
	ldrb	r3, [r7]	@ zero_extendqisi2
	cmp	r3, #50
	beq	.L1341
	subs	r3, r3, #52
	cmp	r3, #1
	bhi	.L1342
.L1341:
	mov	r0, r4
	bl	m9_dsp_on
.L1342:
	mov	r0, r4
	bl	m0_read_page_start
	mov	r6, r0
	mov	r0, r4
	bl	m0_read_page_end_not_retry
	ldrb	r3, [r7]	@ zero_extendqisi2
	orrs	r6, r6, r0
	cmp	r3, #52
	bne	.L1343
	movs	r1, #194
.L1358:
	mov	r0, r4
	bl	m9_set_lmflgfix_next
	ldr	r0, [r4, #20]
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L1346
	ldr	r3, [r5, #176]
	mvn	r6, #1
	ldr	r1, [r5, #248]
	ldr	r0, .L1359+16
	str	r3, [sp, #4]
	ldr	r3, [r4, #8]
	str	r3, [sp]
	ldr	r3, [r4, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	b	.L1337
.L1338:
	cmp	r3, #53
	ite	eq
	moveq	r1, #26
	movne	r1, #192
	b	.L1357
.L1343:
	cmp	r3, #53
	ite	eq
	moveq	r1, #10
	movne	r1, #64
	b	.L1358
.L1346:
	cmp	r6, #10
	beq	.L1347
	cmp	r6, #0
	bne	.L1337
	b	.L1348
.L1347:
	ldr	r3, [r5, #176]
	ldr	r1, [r5, #248]
	ldr	r0, .L1359+24
	str	r3, [sp, #4]
	ldr	r3, [r4, #8]
	str	r3, [sp]
	ldr	r3, [r4, #4]
	ldr	r2, [r5, #20]
	ldr	r1, [r1, #8]
	bl	NAND_Print_DBG
	b	.L1350
.L1360:
	.align	2
.L1359:
	.word	g_nsi
	.word	.LC156
	.word	.LANCHOR73
	.word	.LC157
	.word	.LC151
	.word	.LC158
	.word	.LC159
	.word	.LANCHOR74
	.size	m9_read_page_end, .-m9_read_page_end
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
	ldr	r0, .L1393
	mov	r4, r1
	bl	NAND_Print
	mov	r0, #4096
	bl	NAND_Malloc
	mov	r5, r0
	cbnz	r0, .L1362
	ldr	r0, .L1393+4
	bl	NAND_Print
.L1362:
	ldr	r7, .L1393+4
	movs	r6, #0
.L1363:
	ldr	r2, [r10, #4]
	ldrh	r1, [r2, #20]
	cmp	r1, r6
	bgt	.L1365
	movs	r6, #0
	mov	r7, r6
.L1366:
	ldr	r2, [r10, #4]
	uxth	r8, r7
	ldrh	ip, [r4]
	ldr	r1, [r10, #284]
	ldrh	r0, [r2, #28]
	ldrh	r2, [r2, #20]
	cmp	r2, r7
	ble	.L1378
	strh	ip, [sp, #18]	@ movhi
	strh	r0, [sp, #20]	@ movhi
	mov	r0, r10
	str	r1, [sp, #24]
	add	r1, sp, #16
	ldr	r2, [r10, #1980]
	strh	r8, [sp, #16]	@ movhi
	str	sp, [sp, #28]
	blx	r2
	mov	fp, r0
	mov	r0, sp
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L1367
.L1378:
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
	ldr	r2, [r10, #1976]
	blx	r2
	cmp	r0, #0
	bne	.L1368
.L1369:
	movs	r7, #0
.L1381:
	cmp	r7, r6
	ldr	r2, [r10, #4]
	beq	.L1380
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
	ldr	r2, [r10, #1984]
	blx	r2
	b	.L1381
.L1365:
	ldrh	r0, [r2, #26]
	bl	nftl_malloc
	str	r0, [r5, r6, lsl #2]
	cbnz	r0, .L1364
	mov	r0, r7
	bl	NAND_Print
.L1364:
	adds	r6, r6, #1
	b	.L1363
.L1367:
	mov	r0, sp
	bl	is_ftl_logic_page_data
	cmp	r0, #1
	bne	.L1370
	cmp	fp, #0
	beq	.L1371
	cmp	fp, #10
	bne	.L1372
.L1371:
	ldr	r2, [r10, #4]
	adds	r6, r6, #1
	ldr	r1, [r10, #284]
	ldr	r0, [r5, r7, lsl #2]
	ldrh	r2, [r2, #26]
	bl	NAND_Memcpy
.L1372:
	adds	r7, r7, #1
	b	.L1366
.L1370:
	ldrb	r1, [sp, #1]	@ zero_extendqisi2
	cmp	r1, #170
	bne	.L1373
	cmp	fp, #0
	bne	.L1391
.L1374:
	ldr	r2, [r10, #4]
	adds	r6, r6, #1
	ldr	r1, [r10, #284]
	ldr	r0, [r5, r7, lsl #2]
	ldrh	r2, [r2, #26]
	bl	NAND_Memcpy
	b	.L1376
.L1373:
	ldr	r0, .L1393+8
.L1392:
	bl	NAND_Print
	b	.L1372
.L1391:
	cmp	fp, #10
	beq	.L1374
.L1376:
	ldr	r0, .L1393+12
	b	.L1392
.L1368:
	ldr	r2, [r10, #1996]
	add	r1, sp, #16
	mov	r0, r10
	blx	r2
	mov	r0, r10
	bl	out_phy_block_from_free_list
	mov	r4, r0
	cbnz	r0, .L1379
	ldr	r0, .L1393+16
	bl	NAND_Print
.L1380:
	movs	r6, #0
.L1383:
	ldr	r2, [r10, #4]
	ldrh	r2, [r2, #20]
	cmp	r2, r6
	bgt	.L1384
	mov	r0, r5
	bl	nftl_free
	ldr	r0, .L1393+20
	bl	NAND_Print
	mov	r0, r4
	add	sp, sp, #32
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1379:
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
	ldr	r2, [r10, #1976]
	blx	r2
	b	.L1369
.L1384:
	ldr	r0, [r5, r6, lsl #2]
	adds	r6, r6, #1
	bl	nftl_free
	b	.L1383
.L1394:
	.align	2
.L1393:
	.word	.LC160
	.word	.LC161
	.word	.LC162
	.word	.LC163
	.word	.LC164
	.word	.LC165
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
	ldr	r3, .L1397
	subs	r3, r0, r3
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	pop	{r3, pc}
.L1398:
	.align	2
.L1397:
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
	ldr	r3, .L1400
	lsls	r0, r0, #16
	subs	r3, r0, r3
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	pop	{r3, pc}
.L1401:
	.align	2
.L1400:
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
	ldr	r3, .L1403
	subs	r3, r0, r3
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	pop	{r3, pc}
.L1404:
	.align	2
.L1403:
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
	bhi	.L1406
	ldr	r1, .L1408
	ldr	r0, .L1408+4
	bl	NAND_Print
	movs	r0, #0
.L1405:
	pop	{r3, pc}
.L1406:
	lsrs	r1, r1, #18
	add	r0, r0, r1, lsl #2
	rsb	r1, r1, r1, lsl #14
	ldr	r0, [r0, #52]
	add	r2, r2, r1, lsl #18
	add	r0, r0, r2, lsl #2
	b	.L1405
.L1409:
	.align	2
.L1408:
	.word	.LANCHOR88
	.word	.LC166
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
.L1411:
	cmp	r5, r10
	bne	.L1421
	movs	r0, #0
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1421:
	ldr	r3, [fp, r5, lsl #2]
	ldr	r2, [r6, #28]
	cmp	r2, r3
	bls	.L1412
	mov	r1, r3
	mov	r0, r6
	bl	get_logic_page_map
	mov	r7, r0
	cbnz	r0, .L1413
	ldr	r0, .L1425
	bl	NAND_Print_DBG
	ldrh	r3, [r4, #4]
	adds	r3, r3, #1
	strh	r3, [r4, #4]	@ movhi
.L1414:
	adds	r5, r5, #1
	b	.L1411
.L1413:
	ldrh	r3, [r0, #2]
	movw	r2, #65535
	cmp	r3, r2
	beq	.L1416
	ldr	r2, [r6, #4]
	mov	r8, #36
	mul	r8, r8, r3
	ldr	r2, [r2, #32]
	add	r3, r2, r8
	cmp	r4, r3
	beq	.L1417
	mov	r1, r3
	mov	r0, r6
	str	r2, [sp, #4]
	str	r3, [sp]
	bl	phy_block_from_invalid_page_incr
	ldrd	r3, r2, [sp]
.L1418:
	ldr	r0, [r6, #4]
	ldrh	r1, [r3, #4]
	ldrh	r3, [r0, #20]
	cmp	r1, r3
	bls	.L1416
	ldrh	r1, [r2, r8]
	ldr	r0, .L1425+4
	bl	NAND_Print
.L1416:
	ldrh	r3, [r7]
	bfi	r3, r5, #0, #12
	strh	r3, [r7]	@ movhi
	ldrh	r3, [r4]
	strh	r3, [r7, #2]	@ movhi
	b	.L1414
.L1417:
	ldrh	r1, [r4, #4]
	adds	r1, r1, #1
	strh	r1, [r4, #4]	@ movhi
	b	.L1418
.L1412:
	adds	r2, r3, #1
	beq	.L1420
	mov	r2, r5
	ldrh	r1, [r4]
	ldr	r0, .L1425+8
	bl	NAND_Print
.L1420:
	ldrh	r3, [r4, #4]
	ldr	r2, [r6, #4]
	adds	r3, r3, #1
	ldrh	r2, [r2, #20]
	uxth	r3, r3
	strh	r3, [r4, #4]	@ movhi
	cmp	r2, r3
	bcs	.L1414
	ldr	r0, .L1425+12
	bl	NAND_Print
	b	.L1414
.L1426:
	.align	2
.L1425:
	.word	.LC167
	.word	.LC168
	.word	.LC169
	.word	.LC170
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
.L1428:
	str	r1, [r3], #4
	cmp	r3, r2
	bne	.L1428
	ldr	r8, .L1441+4
	movs	r5, #0
.L1432:
	cmp	r4, r8
	bls	.L1429
	mov	r0, #1048576
	bl	NAND_Malloc
	str	r0, [r7], #4
	cbnz	r0, .L1430
.L1439:
	mov	r1, r5
.L1440:
	ldr	r0, .L1441
	bl	NAND_Print
	mvn	r0, #1
	b	.L1427
.L1430:
	mov	r2, #1048576
	movs	r1, #255
	adds	r5, r5, #1
	sub	r4, r4, #262144
	bl	NAND_Memset
	cmp	r5, #25
	bne	.L1432
	cbz	r4, .L1435
	mov	r1, r4
	b	.L1440
.L1429:
	lsls	r4, r4, #2
	mov	r0, r4
	bl	nftl_malloc
	add	r3, r6, r5, lsl #2
	str	r0, [r3, #52]
	cmp	r0, #0
	beq	.L1439
	mov	r2, r4
	movs	r1, #255
	bl	NAND_Memset
	movs	r0, #0
.L1427:
	pop	{r4, r5, r6, r7, r8, pc}
.L1435:
	mov	r0, r4
	b	.L1427
.L1442:
	.align	2
.L1441:
	.word	.LC171
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
	cbnz	r0, .L1444
.L1446:
	ldr	r0, .L1457
.L1456:
	mvn	r5, #1
	bl	NAND_Print
.L1443:
	mov	r0, r5
	pop	{r3, r4, r5, r6, r7, pc}
.L1444:
	ldr	r3, [r4, #4]
	ldrh	r0, [r3, #26]
	bl	nftl_malloc
	str	r0, [r4, #288]
	cmp	r0, #0
	beq	.L1446
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
	cbz	r0, .L1447
	ldr	r0, .L1457+4
	b	.L1456
.L1447:
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
	cbnz	r0, .L1448
	ldr	r0, .L1457+8
	b	.L1456
.L1448:
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
	cbnz	r0, .L1449
	ldr	r0, .L1457+12
	b	.L1456
.L1449:
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
	cbnz	r0, .L1450
	ldr	r0, .L1457+16
	b	.L1456
.L1450:
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
	ldr	r3, .L1457+20
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
.L1451:
	strh	r1, [r2, #396]	@ movhi
	adds	r1, r1, #1
	cmp	r1, #10
	strh	r3, [r2, #398]	@ movhi
	strd	r3, r3, [r2, #400]
	add	r2, r2, #16
	str	r3, [r2, #392]
	bne	.L1451
	movs	r2, #255
	strh	r3, [r4, #394]	@ movhi
	strd	r2, r3, [r4, #556]
	strd	r3, r3, [r4, #564]
	b	.L1443
.L1458:
	.align	2
.L1457:
	.word	.LC172
	.word	.LC173
	.word	.LC174
	.word	.LC175
	.word	.LC176
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
.L1461:
	ldr	r0, [r5], #4
	cbz	r0, .L1460
	bl	nftl_free
.L1460:
	cmp	r5, r4
	bne	.L1461
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
	cbnz	r0, .L1469
	ldr	r0, .L1473
	bl	NAND_Print
	movs	r0, #1
.L1468:
	pop	{r4, r5, r6, pc}
.L1469:
	mov	r2, r5
	str	r5, [r4, #1960]
	str	r5, [r4, #1964]
	mov	r3, r4
	movs	r6, #255
	mov	r5, #-1
	mov	r1, r2
.L1471:
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
	bne	.L1471
	movs	r0, #0
	b	.L1468
.L1474:
	.align	2
.L1473:
	.word	.LC161
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
.L1477:
	cbnz	r0, .L1479
.L1476:
	bx	lr
.L1479:
	ldr	r3, [r0, #4]
	cmp	r3, r1
	beq	.L1476
	ldr	r0, [r0, #12]
	b	.L1477
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
.L1483:
	mul	r2, r1, r3
	adds	r4, r0, r2
	ldrh	r4, [r4, #10]
	cmp	r4, #255
	bne	.L1481
	adds	r2, r2, #8
	add	r0, r0, r2
.L1480:
	pop	{r4, pc}
.L1481:
	adds	r3, r3, #1
	cmp	r3, #16
	bne	.L1483
	movs	r0, #0
	b	.L1480
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
.L1486:
	ldr	r3, [r2, #12]
	cbnz	r3, .L1487
	str	r1, [r2, #12]
	strd	r3, r2, [r1, #12]
	movs	r2, #1
	strh	r2, [r1, #2]	@ movhi
	ldrh	r2, [r0, #2]
	adds	r2, r2, #1
	strh	r2, [r0, #2]	@ movhi
	mov	r0, r3
	bx	lr
.L1487:
	mov	r2, r3
	b	.L1486
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
	cbz	r3, .L1489
	str	r2, [r3, #16]
.L1489:
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
.L1496:
	lsls	r2, r3, #4
	adds	r1, r0, r2
	ldrh	r1, [r1, #6]
	cbnz	r1, .L1494
	adds	r2, r2, #4
	add	r0, r0, r2
	bx	lr
.L1494:
	adds	r3, r3, #1
	cmp	r3, #10
	bne	.L1496
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
.L1499:
	cbnz	r0, .L1501
.L1498:
	bx	lr
.L1501:
	ldr	r3, [r0, #4]
	cmp	r3, r1
	beq	.L1498
	ldr	r0, [r0, #8]
	b	.L1499
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
.L1503:
	ldr	r3, [r2, #8]
	cbnz	r3, .L1504
	str	r1, [r2, #8]
	strd	r3, r2, [r1, #8]
	ldrh	r2, [r0]
	adds	r2, r2, #1
	strh	r2, [r0]	@ movhi
	mov	r0, r3
	bx	lr
.L1504:
	mov	r2, r3
	b	.L1503
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
	cbz	r3, .L1506
	str	r2, [r3, #12]
.L1506:
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
	cbnz	r0, .L1513
	mov	r0, r5
	bl	get_empty_gc_node
	mov	r1, r0
	cbz	r0, .L1513
	str	r7, [r0, #4]
	strh	r6, [r0, #2]	@ movhi
	strd	r4, r4, [r0, #8]
	mov	r0, r5
	bl	add_to_gc_list_tail
	mov	r0, r4
.L1510:
	pop	{r3, r4, r5, r6, r7, pc}
.L1513:
	movs	r0, #1
	b	.L1510
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
	ldr	r1, .L1567
	add	r6, sp, #88
	bl	memcpy
	movs	r3, #47
	ldr	r1, .L1567+4
	strh	r3, [r6, #-32]!	@ movhi
	mov	r0, r6
	bl	nand_fdt_get_string
	cbz	r0, .L1515
	movs	r2, #3
	add	r1, sp, #72
	bl	NAND_Memcmp
	cbnz	r0, .L1515
	mov	r7, r0
	strh	r0, [r5, #18]	@ movhi
.L1516:
	ldr	r3, [r5, #4]
	mov	r10, #0
	ldrh	r2, [r4]
	ldrh	r3, [r3, #24]
	cmp	r3, r2
	bls	.L1560
	strh	r2, [sp, #74]	@ movhi
	add	r1, sp, #72
	strh	r10, [sp, #72]	@ movhi
	mov	r0, r5
	strh	r10, [sp, #76]	@ movhi
	strd	r10, r6, [sp, #80]
	ldr	r2, [r5, #1992]
	blx	r2
	cmp	r0, #1
	bne	.L1518
	ldr	r2, [r5, #1980]
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
	bne	.L1519
	ldrh	r1, [r4]
	ldr	r0, .L1567+8
	bl	NAND_Print
.L1560:
	mov	r0, r10
	b	.L1514
.L1515:
	mov	r0, #-1
.L1514:
	add	sp, sp, #88
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1519:
	mov	r0, r6
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L1520
	ldrh	r2, [r5, #18]
	mov	r1, r4
	mov	r0, r5
	adds	r2, r2, #1
	strh	r2, [r5, #18]	@ movhi
	strh	r10, [r4, #2]	@ movhi
	strh	r10, [r4, #6]	@ movhi
	strh	r10, [r4, #4]	@ movhi
	bl	put_phy_block_to_free_list
.L1521:
	adds	r4, r4, #36
	b	.L1516
.L1520:
	mov	r0, r6
	bl	is_ftl_logic_page_data
	cmp	r0, #1
	beq	.L1522
	mov	r0, r6
	bl	is_function_info_page
	cmp	r0, #1
	bne	.L1523
.L1522:
	cmp	r8, #0
	beq	.L1524
	cmp	r8, #10
	bne	.L1523
.L1524:
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
	bne	.L1521
	ldr	r3, [r5, #1972]
	ldrh	r3, [r3, #4]
	cmp	r3, #0
	beq	.L1521
	mov	r2, r10
.L1556:
	mov	r1, r4
	mov	r0, r5
	bl	add_prio_gc
	ldr	r0, .L1567+12
	b	.L1558
.L1523:
	mov	r0, r6
	bl	is_ftl_start_flag_page
	cmp	r0, #1
	bne	.L1525
	ldr	r0, .L1567+16
.L1558:
	bl	NAND_Print_DBG
	b	.L1521
.L1525:
	mov	r0, r6
	bl	is_fill_page
	cmp	r0, #1
	bne	.L1526
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
	ldr	r0, .L1567+20
	bl	NAND_Print_DBG
	b	.L1521
.L1526:
	ldrb	r0, [sp, #62]	@ zero_extendqisi2
	ldrb	r3, [sp, #58]	@ zero_extendqisi2
	ldrb	r2, [sp, #57]	@ zero_extendqisi2
	str	r0, [sp, #12]
	ldrb	r0, [sp, #61]	@ zero_extendqisi2
	ldrb	r1, [sp, #56]	@ zero_extendqisi2
	str	r0, [sp, #8]
	ldrb	r0, [sp, #60]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [sp, #59]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L1567+24
	bl	NAND_Print_DBG
	uxth	r2, r7
	adds	r3, r2, #1
	strh	r3, [r4, #6]	@ movhi
	cmp	r8, #0
	beq	.L1527
	cmp	r8, #10
	bne	.L1528
.L1527:
	ldrb	r3, [sp, #56]	@ zero_extendqisi2
	cmp	r3, #255
	beq	.L1529
	ldrh	r1, [r4]
	ldr	r0, .L1567+28
	bl	NAND_Print_DBG
	movs	r3, #255
	strh	r3, [r4, #2]	@ movhi
.L1559:
	ldrh	r3, [r5, #20]
	adds	r3, r3, #1
	strh	r3, [r5, #20]	@ movhi
	b	.L1521
.L1529:
	adds	r2, r2, #2
.L1566:
	uxth	r2, r2
	mov	r1, r4
	mov	r0, r5
	bl	erase_block
.L1562:
	cmp	r0, #0
	bne	.L1521
	ldrh	r3, [r5, #18]
	adds	r3, r3, #1
	strh	r3, [r5, #18]	@ movhi
	b	.L1521
.L1528:
	ldrh	r1, [r4]
	ldr	r0, .L1567+32
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
	ldr	r3, [r5, #1980]
	blx	r3
	ldrh	r8, [r4]
	mov	r10, r0
	cbz	r0, .L1530
	cmp	r0, #10
	bne	.L1531
.L1530:
	mov	r0, r6
	bl	is_phy_mapping_page
	cmp	r0, #1
	mov	fp, r0
	bne	.L1532
	mov	r1, r8
	ldr	r0, .L1567+36
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
	bne	.L1521
	ldr	r3, [r5, #1972]
	ldrh	r3, [r3, #4]
	cmp	r3, #0
	beq	.L1521
	mov	r2, fp
	b	.L1556
.L1532:
	mov	r0, r6
	bl	is_nouse_page
	cmp	r0, #1
	mov	r1, r8
	ite	eq
	ldreq	r0, .L1567+40
	ldrne	r0, .L1567+44
	bl	NAND_Print
	ldrh	r2, [r4, #6]
	adds	r2, r2, #1
	b	.L1566
.L1531:
	mov	r1, r8
	ldr	r0, .L1567+48
	bl	NAND_Print
	ldrb	r0, [sp, #71]	@ zero_extendqisi2
	ldrb	r1, [sp, #56]	@ zero_extendqisi2
	ldrb	r3, [sp, #58]	@ zero_extendqisi2
	str	r0, [sp, #48]
	ldrb	r0, [sp, #70]	@ zero_extendqisi2
	ldrb	r2, [sp, #57]	@ zero_extendqisi2
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
	ldr	r0, .L1567+52
	bl	NAND_Print
	mov	r1, r4
	mov	r0, r5
	bl	torture_block_and_mark
	b	.L1562
.L1568:
	.align	2
.L1567:
	.word	.LANCHOR18+9
	.word	.LANCHOR89
	.word	.LC177
	.word	.LC178
	.word	.LC179
	.word	.LC180
	.word	.LC181
	.word	.LC182
	.word	.LC183
	.word	.LC184
	.word	.LC185
	.word	.LC186
	.word	.LC187
	.word	.LC188
.L1518:
	ldrh	r1, [r4]
	ldr	r0, .L1569
	bl	NAND_Print_DBG
	movs	r2, #255
	strh	r10, [r4, #6]	@ movhi
	strh	r2, [r4, #2]	@ movhi
	b	.L1559
.L1570:
	.align	2
.L1569:
	.word	.LC189
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
	cbz	r3, .L1572
.L1574:
	movs	r0, #1
.L1571:
	pop	{r3, r4, r5, r6, r7, r8, r10, pc}
.L1572:
	ldr	r3, [r0, #200]
	cmp	r3, #0
	beq	.L1574
	ldr	r3, [r0, #172]
	cmp	r3, #0
	beq	.L1574
	ldr	r3, [r0, #292]
	ldrh	r7, [r0, #18]
	ldr	r8, [r3, #8]
	cmp	r8, r7
	blt	.L1574
	ldr	r3, [r0, #4]
	ldrh	r5, [r3, #16]
	mov	r1, r5
	bl	get_block_max_erase_time_from_free_list
	cmp	r0, #0
	beq	.L1574
	ldrh	r6, [r0, #6]
	ldr	r0, .L1592
	umull	r0, r1, r5, r0
	ubfx	r10, r1, #2, #16
	cmp	r6, r10
	bcc	.L1574
	sub	r2, r8, r7
	mov	r1, r5
	mov	r0, r4
	bl	get_block_min_erase_time_from_invalid_list
	mov	r7, r0
	cmp	r0, #0
	beq	.L1574
	ldrh	r8, [r0, #6]
	cmp	r6, r8
	bls	.L1574
	lsls	r2, r6, #1
	adds	r3, r2, r6
	cmp	r3, r5, lsl #1
	ble	.L1575
	ldr	r2, .L1592+4
.L1591:
	umull	r2, r3, r5, r2
	lsrs	r3, r3, #3
	strh	r3, [r4, #574]	@ movhi
.L1576:
	ldrh	r2, [r4, #574]
	sub	r3, r6, r8
	cmp	r3, r2
	blt	.L1574
	mov	r2, r8
	mov	r1, r6
	ldr	r0, .L1592+8
	bl	NAND_Print
	ldrh	r1, [r7]
	ldr	r0, .L1592+12
	bl	NAND_Print
	ldrh	r1, [r4, #18]
	ldr	r0, .L1592+16
	bl	NAND_Print
	str	r7, [r4, #580]
	movs	r0, #0
	b	.L1571
.L1575:
	cmp	r5, r2
	bge	.L1577
	ldr	r2, .L1592
	b	.L1591
.L1577:
	cmp	r5, r3
	bge	.L1578
	movs	r1, #7
	mov	r0, r5
	bl	__aeabi_uidiv
	strh	r0, [r4, #574]	@ movhi
	b	.L1576
.L1578:
	strh	r10, [r4, #574]	@ movhi
	b	.L1576
.L1593:
	.align	2
.L1592:
	.word	-858993459
	.word	-1431655765
	.word	.LC190
	.word	.LC191
	.word	.LC192
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
	beq	.L1595
	cmp	r2, #2
	it	ne
	movne	r2, #3
.L1595:
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
.L1602:
	ldr	r3, [r3, #12]
	cbnz	r3, .L1603
	bx	lr
.L1603:
	ldrh	r2, [r3, #4]
	add	r0, r0, r2
	b	.L1602
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
	bpl	.L1605
	ldr	r3, [r0, #324]
	cmp	r1, r3
	beq	.L1607
.L1605:
	ldr	r0, [r0, #292]
	subs	r3, r0, r1
	rsbs	r0, r3, #0
	adcs	r0, r0, r3
	bx	lr
.L1607:
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
	bls	.L1610
	ldr	r3, [r0, #368]
	cmp	r3, #19
	bls	.L1610
	ldr	r2, [r0, #1968]
	ldr	r3, [r2, #184]
	adds	r3, r3, #1
	str	r3, [r2, #184]
	bl	get_block_min_count_from_invalid_list
	movs	r2, #5
	mov	r1, r0
	mov	r0, r4
	bl	add_prio_gc
	cbnz	r0, .L1612
	ldr	r3, [r4, #372]
	adds	r3, r3, #4
	str	r3, [r4, #372]
.L1612:
	ldr	r3, [r4, #4]
	ldr	r2, [r4, #372]
	ldrh	r3, [r3, #20]
	cmp	r2, r3
	itt	cs
	movcs	r3, #0
	strdcs	r3, r3, [r4, #368]
.L1610:
	movs	r0, #0
	pop	{r4, pc}
	.size	static_wear_leveling, .-static_wear_leveling
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
.L1614:
	ldr	r3, [r4, #4]
	ldrh	r0, [r3, #26]
	ldrh	r3, [r4, #584]
	cmp	r3, r5
	bhi	.L1617
	bl	nftl_malloc
	str	r0, [r4, #1616]
	cbz	r0, .L1618
	movs	r0, #0
	b	.L1613
.L1617:
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
	cbnz	r0, .L1615
.L1618:
	ldr	r0, .L1622
	bl	NAND_Print
	movs	r0, #1
.L1613:
	pop	{r4, r5, r6, r7, r8, pc}
.L1615:
	adds	r5, r5, #1
	b	.L1614
.L1623:
	.align	2
.L1622:
	.word	.LC161
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
.L1625:
	ldrh	r3, [r5, #584]
	cmp	r3, r4
	bgt	.L1626
	ldr	r0, [r5, #1616]
	bl	nftl_free
	movs	r0, #0
	pop	{r3, r4, r5, pc}
.L1626:
	add	r3, r5, r4, lsl #5
	adds	r4, r4, #1
	ldr	r0, [r3, #604]
	bl	nftl_free
	b	.L1625
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
	ldr	r5, .L1632
	ldr	r0, .L1632
	bl	del_last_nftl_blk
	mov	r4, r0
.L1630:
	mov	r0, r4
	cbnz	r4, .L1631
	pop	{r3, r4, r5, pc}
.L1631:
	bl	nftl_exit
	mov	r0, r4
	bl	nftl_free
	mov	r0, r5
	bl	del_last_nftl_blk
	mov	r4, r0
	b	.L1630
.L1633:
	.align	2
.L1632:
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
.L1635:
	cmp	r1, r3
	add	r0, r0, #32
	bhi	.L1636
	movs	r0, #0
	pop	{r4, r5, pc}
.L1636:
	strh	r3, [r0, #-32]	@ movhi
	adds	r3, r3, #1
	strh	r4, [r0, #-30]	@ movhi
	str	r5, [r0, #-24]
	strd	r2, r2, [r0, #-16]
	strd	r2, r2, [r0, #-8]
	strh	r2, [r0, #-28]	@ movhi
	strh	r2, [r0, #-26]	@ movhi
	b	.L1635
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
.L1638:
	cbnz	r0, .L1640
.L1637:
	bx	lr
.L1640:
	ldr	r3, [r0, #8]
	cmp	r3, r1
	beq	.L1637
	ldr	r0, [r0, #16]
	b	.L1638
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
.L1642:
	cbnz	r0, .L1644
.L1641:
	bx	lr
.L1644:
	ldr	r3, [r0, #8]
	cmp	r3, r1
	beq	.L1641
	ldr	r0, [r0, #24]
	b	.L1642
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
.L1646:
	ldr	r4, [r3, #16]
	cbnz	r4, .L1649
	str	r1, [r3, #16]
	strd	r4, r3, [r1, #16]
	strh	r2, [r1, #2]	@ movhi
	cbnz	r2, .L1647
	ldrh	r3, [r0, #2]
	adds	r3, r3, #1
	strh	r3, [r0, #2]	@ movhi
.L1648:
	movs	r0, #0
	pop	{r4, pc}
.L1649:
	mov	r3, r4
	b	.L1646
.L1647:
	ldrh	r3, [r0, #6]
	adds	r3, r3, #1
	strh	r3, [r0, #6]	@ movhi
	b	.L1648
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
.L1651:
	ldr	r3, [r2, #24]
	cbnz	r3, .L1652
	str	r1, [r2, #24]
	strd	r3, r2, [r1, #24]
	movs	r2, #1
	strh	r2, [r1, #2]	@ movhi
	ldrh	r2, [r0, #4]
	adds	r2, r2, #1
	strh	r2, [r0, #4]	@ movhi
	mov	r0, r3
	bx	lr
.L1652:
	mov	r2, r3
	b	.L1651
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
	cbz	r3, .L1654
	str	r2, [r3, #20]
.L1654:
	ldrh	r3, [r1, #2]
	cbnz	r3, .L1655
	ldrh	r3, [r0, #2]
	subs	r3, r3, #1
	strh	r3, [r0, #2]	@ movhi
.L1656:
	movs	r3, #255
	mov	r0, r1
	strh	r3, [r1, #2]	@ movhi
	movs	r3, #0
	strd	r3, r3, [r1, #16]
	bx	lr
.L1655:
	ldrh	r3, [r0, #6]
	subs	r3, r3, #1
	strh	r3, [r0, #6]	@ movhi
	b	.L1656
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
	cbz	r3, .L1661
	str	r2, [r3, #28]
.L1661:
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
.L1666:
	uxth	r3, r1
	cmp	r2, r3
	bhi	.L1668
	ldrh	r3, [r0, #6]
	ldr	r1, [r0, #984]
	cmp	r3, #9
	bhi	.L1669
.L1670:
	cbz	r1, .L1665
	ldrh	r3, [r1, #2]
	cbz	r3, .L1677
	ldr	r1, [r1, #16]
	b	.L1670
.L1668:
	adds	r3, r1, #1
	add	r4, r0, r3, lsl #5
	ldrh	r4, [r4, #-22]
	cmp	r4, #255
	bne	.L1675
	add	r1, r0, r1, lsl #5
	adds	r1, r1, #8
.L1665:
	mov	r0, r1
	pop	{r4}
	bx	lr
.L1675:
	mov	r1, r3
	b	.L1666
.L1671:
	ldr	r1, [r1, #16]
.L1669:
	cmp	r1, #0
	beq	.L1665
	ldrh	r3, [r1, #2]
	cmp	r3, #2
	bne	.L1671
.L1677:
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
	cbnz	r3, .L1679
	ldr	r0, .L1689
	bl	NAND_Print
.L1687:
	mov	fp, #0
.L1678:
	mov	r0, fp
	add	sp, sp, #16
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1679:
	lsrs	r3, r2, #8
	uxtb	r2, r2
	lsl	r8, r3, #9
	ldr	r3, [r0, #1972]
	lsls	r5, r2, #9
	ldrh	r3, [r3]
	str	r3, [sp, #12]
	cbz	r3, .L1681
	ldr	r6, [r0, #604]
	ldr	r3, [r0, #2004]
	mov	r2, r6
	blx	r3
	mov	r2, r5
	mov	fp, r0
	add	r1, r6, r8
	mov	r0, r7
	bl	NAND_Memcpy
	cmp	fp, #10
	bne	.L1678
	ldr	r2, [r4, #1968]
	mov	r0, r4
	ldr	r1, [sp, #4]
	ldr	r3, [r2, #120]
	adds	r3, r3, #1
	str	r3, [r2, #120]
	mov	r2, r6
	ldr	r3, [r4, #2000]
	blx	r3
	b	.L1687
.L1681:
	add	r10, r0, #584
	mov	r0, r10
	bl	search_from_cache_write_list
	mov	fp, r0
	cbz	r0, .L1682
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
	bcc	.L1683
	add	r2, r8, r5
	cmp	r3, r2
	bcc	.L1683
.L1688:
	mov	r2, r5
	mov	r1, r10
	mov	r0, r7
	bl	NAND_Memcpy
	b	.L1687
.L1683:
	ldr	r3, [r4, #2004]
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
	b	.L1688
.L1682:
	ldr	r1, [sp, #4]
	mov	r0, r10
	bl	search_from_cache_read_list
	mov	r6, r0
	cbz	r0, .L1684
	ldrh	r4, [r0, #2]
	cbnz	r4, .L1685
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
	b	.L1687
.L1685:
	ldr	r1, [r0, #12]
	mov	r2, r5
	mov	r0, r7
	add	r1, r1, r8
	bl	NAND_Memcpy
	b	.L1678
.L1684:
	mov	r0, r10
	bl	get_empty_cahce_node
	cbnz	r0, .L1686
	ldrh	r3, [r4, #590]
	mov	fp, #1
	ldrh	r2, [r4, #586]
	ldrh	r1, [r4, #588]
	ldr	r0, .L1689+4
	bl	NAND_Print
	b	.L1678
.L1686:
	ldr	r2, [sp, #4]
	ldr	r1, [sp, #4]
	ldr	r3, [r4, #2004]
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
	bne	.L1678
	ldr	r2, [r4, #1968]
	mov	r0, r4
	mov	fp, r6
	ldr	r3, [r2, #120]
	adds	r3, r3, #1
	str	r3, [r2, #120]
	ldr	r3, [r4, #2000]
	ldrd	r1, r2, [sp, #4]
	blx	r3
	b	.L1678
.L1690:
	.align	2
.L1689:
	.word	.LC193
	.word	.LC194
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
	cbz	r3, .L1692
	ldr	r1, [r1, #24]
	add	r3, r3, r2
	cmp	r3, r1
	bls	.L1693
	movs	r4, #1
	ldr	r0, .L1706
	bl	NAND_Print
.L1692:
	mov	r0, r4
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1693:
	ldr	r1, .L1706+4
	ldr	r3, .L1706+8
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
	beq	.L1692
.L1697:
	ldr	r3, [r6, #4]
	add	fp, fp, #1
	add	r10, r10, r5, lsl #9
	ldrb	r3, [r3, #8]	@ zero_extendqisi2
	cmp	r7, r3
	bhi	.L1698
	mov	r3, r10
	uxth	r2, r7
	mov	r1, fp
	mov	r0, r6
	blx	r8
	orrs	r4, r4, r0
	b	.L1692
.L1698:
	uxth	r5, r3
	subs	r7, r7, r3
	mov	r1, fp
	mov	r3, r10
	mov	r2, r5
	mov	r0, r6
	blx	r8
	orrs	r4, r4, r0
	b	.L1697
.L1707:
	.align	2
.L1706:
	.word	.LC195
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
	bne	.L1709
	ldr	r0, .L1710
	bl	NAND_Print
.L1709:
	ldr	r1, [r4, #1968]
	movs	r0, #0
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
.L1711:
	.align	2
.L1710:
	.word	.LC196
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
	cbz	r0, .L1714
	ldr	r1, [r0, #8]
	mov	r2, #512
	mov	r0, r5
	bl	NAND_Memcpy
	movs	r0, #0
.L1713:
	pop	{r4, r5, r6, pc}
.L1714:
	mov	r3, r2
	mov	r0, r4
	movs	r2, #1
	bl	__nand_read
	cmp	r0, #0
	beq	.L1713
	ldr	r0, .L1719
	bl	NAND_Print
	movs	r0, #1
	b	.L1713
.L1720:
	.align	2
.L1719:
	.word	.LC197
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
.L1722:
	cmp	r4, r7
	bne	.L1723
	mov	r0, r5
	pop	{r4, r5, r6, r7, r8, pc}
.L1723:
	mov	r2, r6
	mov	r1, r4
	mov	r0, r8
	adds	r4, r4, #1
	bl	nand_sector_cache_read
	add	r6, r6, #512
	orrs	r5, r5, r0
	b	.L1722
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
	bne	.L1725
	ldr	r0, .L1726
	bl	NAND_Print
.L1725:
	ldr	r1, [r4, #1968]
	movs	r0, #1
	ldrd	r2, [r1, #80]
	adds	fp, r2, r5
	mov	r2, r6
	adc	ip, r3, #0
	mov	r3, r5
	strd	fp, [r1, #80]
	mov	r1, r4
	str	r7, [sp]
	bl	nand_op
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, fp, pc}
.L1727:
	.align	2
.L1726:
	.word	.LC198
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
	cbnz	r0, .L1730
	ldr	r1, .L1733
	ldr	r0, .L1733+4
	bl	NAND_Print
.L1730:
	movs	r5, #0
	mov	r2, #512
	movs	r1, #170
	mov	r0, r4
	bl	NAND_Memset
.L1731:
	mov	r1, r5
	mov	r3, r4
	movs	r2, #1
	mov	r0, r6
	adds	r5, r5, #1
	bl	__nand_write
	cmp	r5, #38912
	bne	.L1731
	mov	r0, r4
	bl	nftl_free
	pop	{r4, r5, r6, pc}
.L1734:
	.align	2
.L1733:
	.word	.LANCHOR90
	.word	.LC199
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
	cbz	r0, .L1736
	mov	r1, r0
	mov	r0, r5
	bl	del_from_sector_cache_write_list
.L1744:
	mov	r2, #512
	mov	r1, r7
	ldr	r0, [r4, #8]
	bl	NAND_Memcpy
	mov	r1, r4
	mov	r0, r5
	bl	add_to_sector_cache_write_list_tail
	movs	r0, #0
	b	.L1735
.L1736:
	ldrh	r3, [r6, #1622]
	cmp	r3, #7
	bls	.L1738
	ldr	r1, [r6, #1960]
	mov	r0, r5
	bl	del_from_sector_cache_write_list
	movs	r2, #1
	mov	r4, r0
	ldr	r3, [r0, #8]
	ldr	r1, [r0, #4]
	mov	r0, r6
	bl	__nand_write
	cbz	r0, .L1739
	ldr	r0, .L1745
.L1743:
	bl	NAND_Print
	movs	r0, #1
.L1735:
	pop	{r4, r5, r6, r7, r8, pc}
.L1738:
	mov	r0, r5
	bl	get_empty_sector_cahce_node
	mov	r4, r0
	cbnz	r0, .L1739
	ldr	r0, .L1745+4
	b	.L1743
.L1739:
	str	r8, [r4, #4]
	b	.L1744
.L1746:
	.align	2
.L1745:
	.word	.LC200
	.word	.LC201
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
.L1748:
	cmp	r4, r7
	bne	.L1749
	mov	r0, r5
	pop	{r4, r5, r6, r7, r8, pc}
.L1749:
	mov	r2, r6
	mov	r1, r4
	mov	r0, r8
	adds	r4, r4, #1
	bl	nand_sector_cache_write
	add	r6, r6, #512
	orrs	r5, r5, r0
	b	.L1748
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
.L1751:
	ldr	r1, [r5, #1960]
	cbz	r1, .L1752
	mov	r0, r7
	bl	del_from_sector_cache_write_list
	movs	r2, #1
	ldr	r3, [r0, #8]
	ldr	r1, [r0, #4]
	mov	r0, r5
	bl	__nand_write
	subs	r6, r6, #1
	mov	r4, r0
	bne	.L1751
.L1752:
	cbz	r4, .L1750
	ldr	r0, .L1761
	bl	NAND_Print
.L1750:
	mov	r0, r4
	pop	{r3, r4, r5, r6, r7, pc}
.L1762:
	.align	2
.L1761:
	.word	.LC202
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
	ldr	r1, .L1790
	bl	memcpy
	movs	r2, #8
	ldr	r1, .L1790+4
	add	r0, sp, r2
	bl	memcpy
	ldr	r1, [r4, #1608]
	cbnz	r1, .L1764
	ldr	r0, .L1790+8
	bl	NAND_Print
.L1765:
	movs	r0, #0
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, pc}
.L1764:
	add	r7, r4, #584
	mov	r0, r7
	bl	del_from_cache_write_list
	mov	r5, r0
	cmp	r0, #0
	beq	.L1765
	ldr	r3, [r4, #4]
	ldrh	r2, [r0, #6]
	ldrd	r1, r6, [r0, #8]
	ldrb	r3, [r3, #8]	@ zero_extendqisi2
	cmp	r2, r3
	beq	.L1767
	ldr	r3, [r4, #2004]
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
	bne	.L1767
	ldrh	r2, [r4, #590]
	ldr	r1, [r5, #8]
	ldr	r0, .L1790+12
	bl	NAND_Print
.L1767:
	movs	r3, #0
	mov	r2, r6
	strh	r3, [r5, #4]	@ movhi
	mov	r0, r4
	ldr	r3, [r4, #4]
	ldr	r1, [r5, #8]
	ldrb	r3, [r3, #8]	@ zero_extendqisi2
	strh	r3, [r5, #6]	@ movhi
	ldr	r3, [r4, #2000]
	blx	r3
	cbz	r0, .L1769
	ldr	r0, .L1790+16
	bl	NAND_Print
.L1769:
	ldr	r10, [r4, #4]
	movs	r2, #8
	mov	r1, sp
	mov	r0, r10
	bl	strncmp
	tst	r0, #255
	bne	.L1770
.L1773:
	cmp	r8, #1
	bne	.L1774
	ldrh	r1, [r4, #320]
	ldr	r0, .L1790+20
	bl	printf
	ldrh	r0, [r4, #320]
	cbz	r0, .L1774
	subs	r0, r0, #1
	bl	nand_phy_get_page_type
	cmp	r0, #2
	bne	.L1774
	ldr	r0, .L1790+24
	bl	printf
	ldr	r3, [r4, #2000]
	mov	r2, r6
	ldr	r1, [r5, #8]
	mov	r0, r4
	blx	r3
	cbz	r0, .L1774
	ldr	r0, .L1790+16
	bl	NAND_Print
	b	.L1774
.L1770:
	movs	r2, #8
	mov	r0, r10
	add	r1, sp, r2
	bl	strncmp
	tst	r0, #255
	beq	.L1773
.L1774:
	movs	r2, #2
	mov	r1, r5
	mov	r0, r7
	bl	add_to_cache_read_list_tail
	b	.L1765
.L1791:
	.align	2
.L1790:
	.word	.LANCHOR18+14
	.word	.LANCHOR18+22
	.word	.LC203
	.word	.LC204
	.word	.LC205
	.word	.LC206
	.word	.LC207
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
	mov	r7, r1
	mov	r5, r3
	cbnz	r3, .L1793
	ldr	r0, .L1831
	bl	NAND_Print
.L1829:
	movs	r0, #0
	b	.L1792
.L1793:
	lsls	r3, r2, #1
	lsl	r10, r2, #9
	and	r3, r3, #65024
	uxth	r10, r10
	str	r3, [sp, #4]
	ldr	r3, [r0, #1972]
	ldrh	r6, [r3]
	cbz	r6, .L1795
	ldr	r6, [r0, #604]
	ldr	r3, [r0, #2004]
	mov	r2, r6
	blx	r3
	ldr	r3, [sp, #4]
	mov	r2, r10
	mov	r1, r5
	add	r3, r3, r6
	mov	r0, r3
	bl	NAND_Memcpy
	ldr	r3, [r4, #2000]
	mov	r2, r6
	mov	r1, r7
	mov	r0, r4
	add	sp, sp, #16
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, lr}
	bx	r3
.L1795:
	add	r6, r0, #584
	ldrh	r0, [r3, #2]
	cbnz	r0, .L1796
	ldr	r0, [r4, #4]
	ldrb	r8, [r0, #8]	@ zero_extendqisi2
	cmp	r8, r2
	bne	.L1796
	ldrh	r3, [r3, #26]
	cbnz	r3, .L1796
	mov	r0, r6
	bl	search_from_cache_write_list
	mov	r1, r0
	cbz	r0, .L1797
	mov	r0, r6
	bl	del_from_cache_write_list
.L1797:
	mov	r1, r7
	mov	r0, r6
	bl	search_from_cache_read_list
	mov	r1, r0
	cbz	r0, .L1798
	mov	r0, r6
	bl	del_from_cache_read_list
.L1798:
	ldr	r3, [r4, #2000]
	mov	r2, r5
	mov	r1, r7
	mov	r0, r4
	blx	r3
	cmp	r0, #0
	beq	.L1829
	mov	r1, r8
	ldr	r0, .L1831+4
	bl	NAND_Print
.L1828:
	movs	r0, #1
.L1792:
	add	sp, sp, #16
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1796:
	mov	r1, r7
	mov	r0, r6
	bl	search_from_cache_write_list
	mov	r8, r0
	cmp	r0, #0
	beq	.L1800
	mov	r1, r0
	mov	r0, r6
	bl	del_from_cache_write_list
	ldr	r3, [r8, #12]
	ldr	r2, [r4, #4]
	str	r3, [sp, #8]
	ldrb	r2, [r2, #8]	@ zero_extendqisi2
	ldrh	r3, [r8, #6]
	cmp	r2, r3
	beq	.L1802
	lsl	fp, r3, #9
	ldr	r2, [sp, #4]
	uxth	fp, fp
	cmp	r2, fp
	bne	.L1803
	add	r3, r3, r10, lsr #9
	strh	r3, [r8, #6]	@ movhi
.L1802:
	ldrd	r4, r3, [sp, #4]
	mov	r2, r10
	mov	r1, r5
	add	r3, r3, r4
	mov	r0, r3
	bl	NAND_Memcpy
	mov	r1, r8
	mov	r0, r6
	bl	add_to_cache_write_list_tail
	mov	r1, r7
	mov	r0, r6
	bl	search_from_cache_read_list
	mov	r1, r0
	cmp	r0, #0
	beq	.L1829
	mov	r0, r6
	bl	del_from_cache_read_list
	b	.L1829
.L1803:
	ldr	r3, [r4, #2004]
	mov	r1, r7
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
	strh	r2, [r8, #6]	@ movhi
	bne	.L1802
	mov	r1, r7
	ldr	r0, .L1831+8
	bl	NAND_Print
	b	.L1802
.L1800:
	mov	r0, r6
	bl	search_from_cache_read_list
	mov	fp, r0
	cbz	r0, .L1806
	ldr	r3, [sp, #4]
	mov	r2, r10
	ldr	r0, [r0, #12]
	mov	r1, r5
	add	r0, r0, r3
	bl	NAND_Memcpy
	ldr	r3, [r4, #4]
	mov	r1, fp
	strh	r8, [fp, #4]	@ movhi
	mov	r0, r6
	ldrb	r3, [r3, #8]	@ zero_extendqisi2
	strh	r3, [fp, #6]	@ movhi
	bl	del_from_cache_read_list
	mov	r0, r6
	bl	add_to_cache_write_list_tail
	ldr	r3, [r4, #1972]
	ldrh	r2, [r4, #588]
	ldrh	r3, [r3, #24]
	cmp	r2, r3
	bcc	.L1829
	mov	r1, r8
	mov	r0, r4
	bl	flush_write_cache_to_nand
	cmp	r0, #0
	beq	.L1829
	ldr	r0, .L1831+12
.L1830:
	bl	NAND_Print
	b	.L1828
.L1807:
	movs	r1, #0
	mov	r0, r4
	bl	flush_write_cache_to_nand
	cbz	r0, .L1806
	ldr	r0, .L1831+16
	b	.L1830
.L1806:
	ldr	r3, [r4, #1972]
	ldrh	r2, [r4, #588]
	ldrh	r3, [r3, #24]
	cmp	r2, r3
	bcs	.L1807
	mov	r0, r6
	bl	get_empty_cahce_node
	mov	r8, r0
	cbnz	r0, .L1808
	ldrh	r3, [r4, #590]
	ldrh	r2, [r4, #586]
	ldrh	r1, [r4, #588]
	ldr	r0, .L1831+20
	bl	NAND_Print
	b	.L1828
.L1808:
	ldr	r3, [sp, #4]
	str	r7, [r0, #8]
	ldr	fp, [r0, #12]
	cbnz	r3, .L1809
	ldrh	r3, [sp, #4]
	strh	r3, [r0, #4]	@ movhi
	lsr	r3, r10, #9
.L1827:
	strh	r3, [r8, #6]	@ movhi
	mov	r1, r5
	ldr	r3, [sp, #4]
	mov	r2, r10
	add	r3, r3, fp
	mov	r0, r3
	bl	NAND_Memcpy
	mov	r1, r8
	mov	r0, r6
	bl	add_to_cache_write_list_tail
	b	.L1829
.L1809:
	ldr	r3, [r4, #2004]
	mov	r2, fp
	mov	r1, r7
	mov	r0, r4
	blx	r3
	cmp	r0, #10
	bne	.L1811
	mov	r1, r7
	ldr	r0, .L1831+24
	bl	NAND_Print
.L1811:
	movs	r3, #0
	strh	r3, [r8, #4]	@ movhi
	ldr	r3, [r4, #4]
	ldrb	r3, [r3, #8]	@ zero_extendqisi2
	b	.L1827
.L1832:
	.align	2
.L1831:
	.word	.LC208
	.word	.LC209
	.word	.LC210
	.word	.LC211
	.word	.LC212
	.word	.LC194
	.word	.LC213
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
.L1834:
	ldr	r3, [r6, #1608]
	cbz	r3, .L1835
	adds	r4, r4, #1
	movs	r1, #1
	mov	r0, r6
	bl	flush_write_cache_to_nand
	cmp	r7, r4
	orr	r5, r5, r0
	bne	.L1834
.L1835:
	cbz	r5, .L1837
	ldr	r0, .L1848
	bl	NAND_Print
.L1837:
	cbz	r4, .L1833
	ldr	r3, [r6, #356]
	cbz	r3, .L1833
	mov	r2, r4
	ldrb	r1, [r6, #16]	@ zero_extendqisi2
	ldr	r0, .L1848+4
	bl	NAND_Print
.L1833:
	mov	r0, r5
	pop	{r3, r4, r5, r6, r7, pc}
.L1849:
	.align	2
.L1848:
	.word	.LC202
	.word	.LC214
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
	bls	.L1853
	ldr	r0, .L1874
	bl	NAND_Print
	movs	r0, #1
.L1852:
	pop	{r4, r5, r6, r7, r8, pc}
.L1853:
	ldr	r1, [r6, #1968]
	ldrd	r2, [r1, #104]
	adds	r7, r2, r4
	adc	r8, r3, #0
	ldr	r3, [r6, #4]
	strd	r7, [r1, #104]
	ldrb	r7, [r3, #8]	@ zero_extendqisi2
	mov	r1, r7
	bl	__aeabi_uidivmod
	mov	r5, r0
	cbz	r1, .L1855
	subs	r1, r7, r1
	adds	r5, r5, #1
	uxth	r1, r1
	cmp	r1, r4
	bhi	.L1863
	subs	r4, r4, r1
.L1856:
	mov	r1, r7
	mov	r0, r4
	bl	__aeabi_uidiv
	cbz	r0, .L1857
	add	r7, r6, #584
	adds	r4, r5, r0
.L1862:
	ldr	r3, [r6, #1972]
	ldrh	r3, [r3]
	cbnz	r3, .L1859
	mov	r1, r5
	mov	r0, r7
	bl	search_from_cache_write_list
	mov	r1, r0
	cbz	r0, .L1860
	mov	r0, r7
	bl	del_from_cache_write_list
.L1860:
	mov	r1, r5
	mov	r0, r7
	bl	search_from_cache_read_list
	mov	r1, r0
	cbz	r0, .L1859
	mov	r0, r7
	bl	del_from_cache_read_list
.L1859:
	mov	r1, r5
	ldr	r3, [r6, #2008]
	mov	r0, r6
	adds	r5, r5, #1
	blx	r3
	ldr	r3, [r6, #4]
	movs	r1, #0
	ldr	ip, [r6, #1968]
	ldrb	r0, [r3, #8]	@ zero_extendqisi2
	ldrd	r3, r2, [ip, #112]
	adds	r3, r3, r0
	adc	r2, r1, r2
	cmp	r5, r4
	strd	r3, r2, [ip, #112]
	bne	.L1862
.L1857:
	movs	r0, #0
	b	.L1852
.L1855:
	cmp	r7, r4
	it	hi
	movhi	r4, #0
	b	.L1856
.L1863:
	movs	r4, #0
	b	.L1856
.L1875:
	.align	2
.L1874:
	.word	.LC215
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
	bne	.L1877
	ldr	r0, .L1878
	bl	NAND_Print
.L1877:
	mov	r2, r6
	mov	r1, r5
	mov	r0, r4
	pop	{r4, r5, r6, lr}
	b	nand_discard
.L1879:
	.align	2
.L1878:
	.word	.LC216
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
	ldr	r7, .L1887
	movs	r4, #0
	ldr	r8, .L1887+4
	ldrb	r1, [r3, #8]	@ zero_extendqisi2
	ldr	r0, [r0, #24]
	bl	__aeabi_uidiv
	mov	r6, r0
.L1883:
	cmp	r4, r6
	bne	.L1886
	movs	r0, #0
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L1886:
	mov	r1, r4
	mov	r0, r5
	bl	get_logic_page_map
	cbnz	r0, .L1884
	mov	r0, r8
	bl	NAND_Print_DBG
.L1885:
	adds	r4, r4, #1
	b	.L1883
.L1884:
	ldrh	r2, [r0, #2]
	movw	r3, #65535
	cmp	r2, r3
	beq	.L1885
	ldrb	r1, [r0, #1]	@ zero_extendqisi2
	ldrh	r3, [r0]
	mov	r0, r7
	ubfx	r1, r1, #4, #1
	str	r1, [sp]
	ubfx	r3, r3, #0, #12
	mov	r1, r4
	bl	NAND_Print
	b	.L1885
.L1888:
	.align	2
.L1887:
	.word	.LC218
	.word	.LC217
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
.L1890:
	cbnz	r3, .L1891
	bx	lr
.L1891:
	mov	r0, r3
	ldr	r3, [r3, #8]
	b	.L1890
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
.L1893:
	cbnz	r3, .L1894
	ldr	r3, [r2, #12]
	ldr	r3, [r3, #1968]
	ldr	r3, [r3, #176]
.L1895:
	cbnz	r0, .L1897
.L1892:
	bx	lr
.L1894:
	mov	r2, r3
	ldr	r3, [r3, #8]
	b	.L1893
.L1897:
	ldr	r2, [r0, #12]
	ldrb	r2, [r2, #16]	@ zero_extendqisi2
	cmp	r2, r3
	beq	.L1892
	ldr	r0, [r0, #8]
	b	.L1895
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
.L1899:
	cbnz	r3, .L1900
	ldr	r3, [r0, #12]
	ldr	r3, [r3, #1968]
	ldr	r0, [r3, #180]
	bx	lr
.L1900:
	mov	r0, r3
	ldr	r3, [r3, #8]
	b	.L1899
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
.L1902:
	cbnz	r0, .L1903
	bx	lr
.L1903:
	ldr	r2, [r0, #12]
	ldr	r0, [r0, #8]
	ldr	r3, [r2, #364]
	clz	r3, r3
	lsrs	r3, r3, #5
	str	r3, [r2, #364]
	b	.L1902
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
.L1905:
	cbnz	r3, .L1906
	ldr	r1, [r1, #8]
	ldr	r2, [r4, #12]
	ldr	r2, [r2, #1968]
	cbnz	r1, .L1907
	str	r3, [r2, #176]
	bl	clean_read_reclaim_complete_flag
.L1908:
	ldr	r3, [r4, #12]
	movs	r0, #0
	ldr	r3, [r3, #1968]
	str	r0, [r3, #180]
	pop	{r3, r4, r5, pc}
.L1906:
	mov	r4, r3
	ldr	r3, [r3, #8]
	b	.L1905
.L1907:
	ldrb	r3, [r5, #16]	@ zero_extendqisi2
	adds	r3, r3, #1
	str	r3, [r2, #176]
	b	.L1908
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
.L1910:
	cbnz	r0, .L1911
	bx	lr
.L1911:
	ldr	r3, [r0, #12]
	ldr	r0, [r0, #8]
	ldr	r3, [r3, #1968]
	strd	r1, r2, [r3, #176]
	b	.L1910
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
	ldr	r8, .L1918
	mov	r6, r2
	mov	r4, r1
	movw	r7, #65535
.L1913:
	cmp	r4, r6
	bcc	.L1917
	mov	r4, #-1
	b	.L1912
.L1917:
	mov	r1, r4
	mov	r0, r5
	bl	get_logic_page_map
	cbnz	r0, .L1914
	mov	r0, r8
	bl	NAND_Print_DBG
.L1915:
	adds	r4, r4, #1
	b	.L1913
.L1914:
	ldrh	r3, [r0, #2]
	cmp	r3, r7
	beq	.L1915
	ldrb	r3, [r0, #1]	@ zero_extendqisi2
	ldr	r2, [r5, #364]
	ubfx	r1, r3, #4, #1
	cmp	r1, r2
	beq	.L1915
	bfi	r3, r2, #4, #1
	strb	r3, [r0, #1]
.L1912:
	mov	r0, r4
	pop	{r4, r5, r6, r7, r8, pc}
.L1919:
	.align	2
.L1918:
	.word	.LC217
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
	bne	.L1921
	mov	r1, r4
	mov	r0, r6
	pop	{r3, r4, r5, r6, r7, lr}
	b	set_nftl_read_claim_complete
.L1921:
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
	beq	.L1925
	ldr	r4, [r0, #1968]
	ldr	r0, .L1927
	ldr	r1, [r4, #64]
	bl	NAND_Print_DBG
	ldrd	r2, [r4, #72]
	ldr	r0, .L1927+4
	bl	NAND_Print_DBG
	ldrd	r2, [r4, #80]
	ldr	r0, .L1927+8
	bl	NAND_Print_DBG
	ldrd	r2, [r4, #88]
	ldr	r0, .L1927+12
	bl	NAND_Print_DBG
	ldrd	r2, [r4, #96]
	ldr	r0, .L1927+16
	bl	NAND_Print_DBG
	ldrd	r2, [r4, #104]
	ldr	r0, .L1927+20
	bl	NAND_Print_DBG
	ldrd	r2, [r4, #112]
	ldr	r0, .L1927+24
	bl	NAND_Print_DBG
	ldr	r1, [r4, #120]
	ldr	r0, .L1927+28
	bl	NAND_Print_DBG
	ldr	r1, [r4, #124]
	ldr	r0, .L1927+32
	bl	NAND_Print_DBG
	ldr	r1, [r4, #128]
	ldr	r0, .L1927+36
	bl	NAND_Print_DBG
	ldr	r1, [r4, #132]
	ldr	r0, .L1927+40
	bl	NAND_Print_DBG
	ldr	r1, [r4, #184]
	ldr	r0, .L1927+44
	bl	NAND_Print_DBG
	ldr	r1, [r4, #136]
	ldr	r0, .L1927+48
	bl	NAND_Print_DBG
	ldr	r1, [r4, #140]
	ldr	r0, .L1927+52
	bl	NAND_Print_DBG
	ldr	r1, [r4, #144]
	ldr	r0, .L1927+56
	bl	NAND_Print_DBG
	ldr	r1, [r4, #148]
	ldr	r0, .L1927+60
	bl	NAND_Print_DBG
	ldr	r1, [r4, #152]
	ldr	r0, .L1927+64
	bl	NAND_Print_DBG
	ldr	r1, [r4, #156]
	ldr	r0, .L1927+68
	bl	NAND_Print_DBG
	ldr	r1, [r4, #160]
	ldr	r0, .L1927+72
	bl	NAND_Print_DBG
	ldr	r1, [r4, #164]
	ldr	r0, .L1927+76
	bl	NAND_Print_DBG
	ldr	r1, [r4, #168]
	ldr	r0, .L1927+80
	bl	NAND_Print_DBG
	ldr	r1, [r4, #172]
	ldr	r0, .L1927+84
	bl	NAND_Print_DBG
	ldr	r1, [r4, #176]
	ldr	r0, .L1927+88
	bl	NAND_Print_DBG
	ldr	r1, [r4, #180]
	ldr	r0, .L1927+92
	pop	{r4, lr}
	b	NAND_Print_DBG
.L1925:
	pop	{r4, pc}
.L1928:
	.align	2
.L1927:
	.word	.LC219
	.word	.LC220
	.word	.LC221
	.word	.LC222
	.word	.LC223
	.word	.LC224
	.word	.LC225
	.word	.LC226
	.word	.LC227
	.word	.LC228
	.word	.LC229
	.word	.LC230
	.word	.LC231
	.word	.LC232
	.word	.LC233
	.word	.LC234
	.word	.LC235
	.word	.LC236
	.word	.LC237
	.word	.LC238
	.word	.LC239
	.word	.LC240
	.word	.LC241
	.word	.LC242
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
	mov	r2, #328
	mov	r4, r0
	sub	sp, sp, #32
	mov	r5, r1
	movs	r1, #0
	ldr	r0, [r0, #1968]
	bl	NAND_Memset
	ldr	r3, [r4, #1968]
	ldr	r6, .L1943
	ldr	r2, [r3, #144]
	str	r6, [r3, #64]
	adds	r2, r2, #1
	str	r2, [r3, #144]
	cmp	r5, #0
	beq	.L1930
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
	ldr	r3, [r4, #1980]
	blx	r3
	mov	r5, r0
	mov	r0, sp
	bl	is_phy_mapping_page
	cmp	r0, #1
	bne	.L1932
	cbz	r5, .L1933
	cmp	r5, #10
	bne	.L1932
.L1933:
	ldr	r3, [r4, #308]
	ldr	r1, [r4, #284]
	add	r1, r1, r3
	ldr	r3, [r1, #64]
	cmp	r3, r6
	bne	.L1932
	mov	r2, #328
	ldr	r0, [r4, #1968]
	bl	NAND_Memcpy
	ldr	r3, [r4, #1968]
	ldr	r2, [r3, #144]
	adds	r2, r2, #1
	str	r2, [r3, #144]
	ldr	r2, [r3, #168]
	adds	r2, r2, #1
	itt	eq
	moveq	r2, #0
	streq	r2, [r3, #168]
	ldr	r2, [r3, #176]
	adds	r2, r2, #1
	itt	eq
	moveq	r2, #0
	streq	r2, [r3, #176]
	ldr	r2, [r3, #180]
	adds	r2, r2, #1
	itt	eq
	moveq	r2, #0
	streq	r2, [r3, #180]
.L1932:
	mov	r0, r4
	bl	print_smart
.L1930:
	movs	r0, #0
	add	sp, sp, #32
	@ sp needed
	pop	{r4, r5, r6, pc}
.L1944:
	.align	2
.L1943:
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
	beq	.L1945
	ldr	r0, .L1951
	bl	NAND_Print_DBG
	ldr	r3, [r4, #4]
	ldr	r0, .L1951+4
	ldrh	r1, [r3, #24]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #4]
	ldr	r0, .L1951+8
	ldrh	r1, [r3, #26]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #4]
	ldr	r0, .L1951+12
	ldrh	r1, [r3, #20]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #4]
	ldr	r0, .L1951+16
	ldrh	r1, [r3, #16]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #4]
	ldr	r0, .L1951+20
	ldrh	r1, [r3, #30]
	bl	NAND_Print_DBG
	ldr	r1, [r4, #356]
	ldr	r0, .L1951+24
	bl	NAND_Print_DBG
	ldrb	r1, [r4, #16]	@ zero_extendqisi2
	ldr	r0, .L1951+28
	bl	NAND_Print_DBG
	ldrb	r1, [r4, #17]	@ zero_extendqisi2
	ldr	r0, .L1951+32
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #18]
	ldr	r0, .L1951+36
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #20]
	ldr	r0, .L1951+40
	bl	NAND_Print_DBG
	ldr	r1, [r4, #24]
	ldr	r0, .L1951+44
	bl	NAND_Print_DBG
	ldr	r1, [r4, #32]
	ldr	r0, .L1951+48
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #22]
	ldr	r0, .L1951+52
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #380]
	ldr	r0, .L1951+56
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #382]
	ldr	r0, .L1951+60
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #388]
	ldr	r0, .L1951+64
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #384]
	ldr	r0, .L1951+68
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #394]
	ldr	r0, .L1951+72
	bl	NAND_Print_DBG
	ldr	r3, [r4, #40]
	ldr	r0, .L1951+76
	ldrh	r1, [r3]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #44]
	ldr	r0, .L1951+80
	ldrh	r1, [r3]
	bl	NAND_Print_DBG
	add	r3, r4, #272
	ldm	r3, {r0, r1, r2}
	add	r3, r4, #260
	stm	sp, {r0, r1, r2}
	ldr	r0, .L1951+84
	ldm	r3, {r1, r2, r3}
	bl	NAND_Print_DBG
	add	r3, r4, #308
	ldm	r3, {r0, r1, r2}
	add	r3, r4, #296
	stm	sp, {r0, r1, r2}
	ldr	r0, .L1951+88
	ldm	r3, {r1, r2, r3}
	bl	NAND_Print_DBG
	ldr	r1, [r4, #292]
	ldr	r0, .L1951+92
	bl	NAND_Print_DBG
	ldr	r3, [r4, #292]
	ldr	r0, .L1951+96
	ldrh	r1, [r3]
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #320]
	ldr	r0, .L1951+100
	bl	NAND_Print_DBG
	ldr	r3, [r4, #292]
	ldr	r0, .L1951+104
	ldr	r1, [r3, #8]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #48]
	ldr	r0, .L1951+108
	ldrh	r2, [r3, #6]
	ldrh	r1, [r3]
	bl	NAND_Print_DBG
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r3, r3, #31
	bpl	.L1947
	add	r3, r4, #340
	ldm	r3, {r0, r1, r2}
	add	r3, r4, #328
	stm	sp, {r0, r1, r2}
	ldr	r0, .L1951+112
	ldm	r3, {r1, r2, r3}
	bl	NAND_Print_DBG
	ldr	r1, [r4, #324]
	ldr	r0, .L1951+116
	bl	NAND_Print_DBG
	ldr	r3, [r4, #324]
	ldr	r0, .L1951+120
	ldrh	r1, [r3]
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #352]
	ldr	r0, .L1951+124
	bl	NAND_Print_DBG
	ldr	r3, [r4, #324]
	ldr	r0, .L1951+128
	ldr	r1, [r3, #8]
	bl	NAND_Print_DBG
.L1947:
	ldr	r1, [r4, #360]
	ldr	r0, .L1951+132
	bl	NAND_Print_DBG
	ldr	r1, [r4, #284]
	ldr	r0, .L1951+136
	bl	NAND_Print_DBG
	ldr	r1, [r4, #376]
	ldr	r0, .L1951+140
	bl	NAND_Print_DBG
	ldrh	r1, [r4, #584]
	ldr	r0, .L1951+144
	bl	NAND_Print_DBG
	ldr	r1, [r4, #368]
	ldr	r0, .L1951+148
	bl	NAND_Print_DBG
	ldr	r1, [r4, #372]
	ldr	r0, .L1951+152
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L1951+156
	ldrh	r1, [r3]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L1951+160
	ldrh	r1, [r3, #2]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L1951+164
	ldrh	r1, [r3, #4]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L1951+168
	ldrh	r1, [r3, #6]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L1951+172
	ldrh	r1, [r3, #10]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L1951+176
	ldrh	r1, [r3, #12]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L1951+180
	ldrh	r1, [r3, #14]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L1951+184
	ldrh	r1, [r3, #16]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L1951+188
	ldrh	r1, [r3, #18]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L1951+192
	ldrh	r1, [r3, #20]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L1951+196
	ldrh	r1, [r3, #22]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L1951+200
	ldrh	r1, [r3, #24]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L1951+204
	ldrh	r1, [r3, #26]
	bl	NAND_Print_DBG
	ldr	r3, [r4, #1972]
	ldr	r0, .L1951+208
	ldr	r1, [r3, #28]
	bl	NAND_Print_DBG
	mov	r0, r4
	add	sp, sp, #16
	@ sp needed
	pop	{r4, lr}
	b	print_smart
.L1945:
	add	sp, sp, #16
	@ sp needed
	pop	{r4, pc}
.L1952:
	.align	2
.L1951:
	.word	.LC243
	.word	.LC244
	.word	.LC245
	.word	.LC246
	.word	.LC247
	.word	.LC248
	.word	.LC249
	.word	.LC250
	.word	.LC251
	.word	.LC252
	.word	.LC253
	.word	.LC254
	.word	.LC255
	.word	.LC256
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
	ldr	r4, .L1957
.L1955:
	cmp	r2, r1
	bne	.L1956
	pop	{r4, pc}
.L1956:
	ldrb	r3, [r1], #1	@ zero_extendqisi2
	eors	r3, r3, r0
	uxtb	r3, r3
	ldrh	r3, [r4, r3, lsl #1]
	eor	r0, r3, r0, lsr #8
	b	.L1955
.L1958:
	.align	2
.L1957:
	.word	.LANCHOR91
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
	cbnz	r0, .L1964
	ldr	r3, [r6, #4]
	mov	r0, r5
	ldrb	r4, [r3, #8]	@ zero_extendqisi2
	bl	is_phy_mapping_page
	cbnz	r0, .L1961
	lsls	r4, r4, #9
.L1962:
	mov	r2, r4
	mov	r1, r7
	movw	r0, #65535
	bl	nand_crc16
	ldrb	r3, [r5, #12]	@ zero_extendqisi2
	mov	r1, r0
	ldrb	r2, [r5, #13]	@ zero_extendqisi2
	orr	r2, r2, r3, lsl #8
	cmp	r0, r2
	beq	.L1964
	mov	r3, r4
	ldr	r0, .L1965
	bl	NAND_Print
	movs	r0, #1
.L1959:
	pop	{r3, r4, r5, r6, r7, pc}
.L1961:
	ldr	r4, [r6, #308]
	b	.L1962
.L1964:
	movs	r0, #0
	b	.L1959
.L1966:
	.align	2
.L1965:
	.word	.LC296
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
	push	{r3, r4, r5, r6, r7, lr}
	mov	r5, r0
	ldrb	r3, [r2]	@ zero_extendqisi2
	mov	r6, r1
	mov	r4, r2
	cmp	r3, #255
	beq	.L1968
	ldr	r0, .L1982
.L1981:
	ldr	r6, .L1982+4
	movs	r5, #0
	ldr	r7, .L1982+8
	bl	NAND_Print
	movs	r3, #1
	movw	r2, #2812
	ldr	r1, .L1982+12
	ldr	r0, .L1982+16
	bl	NAND_Print
	ldr	r0, .L1982+20
	bl	NAND_Print
.L1976:
	ldrb	r1, [r4, r5]	@ zero_extendqisi2
	mov	r0, r6
	bl	NAND_Print
	cmp	r5, #15
	bne	.L1975
	mov	r0, r7
	bl	NAND_Print
.L1975:
	adds	r5, r5, #1
	cmp	r5, #16
	bne	.L1976
	ldr	r0, .L1982+8
	bl	NAND_Print
	movs	r0, #1
	b	.L1972
.L1968:
	bl	is_physic_info_enable_crc
	cbz	r0, .L1970
	mov	r2, r4
	mov	r1, r6
	mov	r0, r5
	bl	crc_wrong
	cbnz	r0, .L1971
.L1974:
	movs	r0, #0
.L1972:
	pop	{r3, r4, r5, r6, r7, pc}
.L1971:
	ldr	r0, .L1982+24
	b	.L1981
.L1970:
	ldrb	r3, [r4, #11]	@ zero_extendqisi2
	cmp	r3, #165
	bne	.L1973
	ldrb	r3, [r4, #12]	@ zero_extendqisi2
	cmp	r3, #165
	bne	.L1973
	ldrb	r3, [r4, #13]	@ zero_extendqisi2
	cmp	r3, #165
	beq	.L1974
.L1973:
	ldr	r0, .L1982+28
	b	.L1981
.L1983:
	.align	2
.L1982:
	.word	.LC297
	.word	.LC302
	.word	.LC26
	.word	.LANCHOR92
	.word	.LC300
	.word	.LC301
	.word	.LC298
	.word	.LC299
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
	mov	r7, r2
	ldr	r10, .L2018+20
	movs	r4, #0
	sub	sp, sp, #32
	mov	r5, r0
	ldr	fp, .L2018+24
	mov	r8, r1
	ldr	r2, [r0, #308]
	movs	r1, #255
	mov	r0, r7
	bl	NAND_Memset
.L1985:
	ldr	r3, [r5, #4]
	ldrh	r2, [r3, #20]
	cmp	r2, r4
	ble	.L1988
	ldrh	r2, [r3, #28]
	mov	r0, r5
	ldrh	r1, [r8]
	ldr	r3, [r5, #284]
	strh	r2, [sp, #20]	@ movhi
	strh	r1, [sp, #18]	@ movhi
	add	r1, sp, #16
	str	r3, [sp, #24]
	strh	r4, [sp, #16]	@ movhi
	ldr	r3, [r5, #1980]
	str	sp, [sp, #28]
	blx	r3
	mov	r6, r0
	mov	r0, sp
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L1986
	cbnz	r6, .L1987
.L1989:
	mov	r1, r4
	ldr	r0, .L2018
	bl	NAND_Print
.L1988:
	mov	r0, r4
	b	.L1984
.L1987:
	cmp	r6, #10
	beq	.L1989
.L1986:
	mov	r0, sp
	bl	is_ftl_logic_page_data
	cmp	r0, #1
	bne	.L1990
	cbz	r6, .L1991
	cmp	r6, #10
	bne	.L1992
.L1991:
	mov	r2, sp
	ldr	r1, [r5, #284]
	mov	r0, r5
	bl	nand_power_off_ecc_error
	cbz	r0, .L1993
	ldr	r0, .L2018+4
	bl	NAND_Print
.L1992:
	mov	r2, r4
	ldrh	r1, [r8]
	ldr	r0, .L2018+8
	bl	NAND_Print
	movw	r0, #65535
.L1984:
	add	sp, sp, #32
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L1993:
	mov	r0, sp
	bl	get_logic_page_from_oob
	str	r0, [r7, r4, lsl #2]
	mov	r3, r0
	ldr	r2, [r5, #28]
	cmp	r0, r2
	bcc	.L1994
	mov	r2, r4
	ldrh	r1, [r8]
	ldr	r0, .L2018+12
	bl	NAND_Print
	mov	r3, #-1
	str	r3, [r7, r4, lsl #2]
.L1994:
	cbz	r6, .L1999
	cmp	r6, #10
	bne	.L1992
.L1999:
	adds	r4, r4, #1
	b	.L1985
.L1990:
	ldrb	r3, [sp, #1]	@ zero_extendqisi2
	cmp	r3, #170
	bne	.L1995
	ldr	r0, .L2018+16
	mov	r1, r4
.L2017:
	bl	NAND_Print
	b	.L1994
.L1995:
	mov	r0, sp
	bl	is_power_down_info_page
	cmp	r0, #1
	bne	.L1996
	cbz	r6, .L1997
	cmp	r6, #10
	bne	.L1992
.L1997:
	mov	r0, sp
	bl	get_special_data_from_oob
	mov	r1, r4
	str	r0, [r7, r4, lsl #2]
	mov	r0, fp
	b	.L2017
.L1996:
	mov	r1, r4
	mov	r0, r10
	b	.L2017
.L2019:
	.align	2
.L2018:
	.word	.LC303
	.word	.LC304
	.word	.LC309
	.word	.LC305
	.word	.LC306
	.word	.LC308
	.word	.LC307
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
	mov	r6, r1
	mov	r0, r10
	movs	r1, #255
	bl	NAND_Memset
.L2021:
	ldr	r3, [r5, #4]
	ldrh	r2, [r3, #20]
	cmp	r2, r4
	bgt	.L2030
	movs	r0, #0
	add	sp, sp, #40
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2030:
	ldrh	r2, [r3, #28]
	mov	fp, #0
	ldrh	r1, [r6]
	add	r7, sp, #8
	ldr	r3, [r5, #284]
	strh	r4, [sp, #24]	@ movhi
	strh	r1, [sp, #26]	@ movhi
	strh	r2, [sp, #28]	@ movhi
	str	r3, [sp, #32]
	str	r7, [sp, #36]
.L2024:
	ldr	r3, [r5, #1980]
	add	r1, sp, #24
	mov	r0, r5
	blx	r3
	subs	r8, r0, #0
	bge	.L2022
	mov	r3, r4
	ldrh	r2, [r6]
	mov	r1, r8
	ldr	r0, .L2041
	bl	NAND_Print
	cmp	fp, #0
	beq	.L2031
	ldrh	r2, [r6]
	mov	r3, r4
	str	fp, [sp]
	mov	r1, r8
	ldr	r0, .L2041+4
	bl	NAND_Print
.L2022:
	mov	r0, r7
	bl	is_ftl_logic_page_data
	cmp	r0, #1
	bne	.L2025
	cmp	r8, #0
	beq	.L2026
	cmp	r8, #10
	bne	.L2027
.L2026:
	mov	r2, r7
	ldr	r1, [r5, #284]
	mov	r0, r5
	bl	nand_power_off_ecc_error
	cbz	r0, .L2028
	ldr	r0, .L2041+8
	bl	NAND_Print
.L2027:
	adds	r4, r4, #1
	b	.L2021
.L2031:
	mov	fp, #1
	b	.L2024
.L2028:
	mov	r0, r7
	bl	get_logic_page_from_oob
	str	r0, [r10, r4, lsl #2]
	mov	r3, r0
	ldr	r2, [r5, #28]
	cmp	r0, r2
	bcc	.L2027
	mov	r2, r4
	ldrh	r1, [r6]
	ldr	r0, .L2041+12
	bl	NAND_Print
	mov	r3, #-1
	str	r3, [r10, r4, lsl #2]
	b	.L2027
.L2025:
	mov	r0, r7
	bl	is_nouse_page
	cmp	r0, #1
	beq	.L2027
	mov	r2, r4
	ldrh	r1, [r6]
	ldr	r0, .L2041+16
	bl	NAND_Print
	b	.L2027
.L2042:
	.align	2
.L2041:
	.word	.LC310
	.word	.LC311
	.word	.LC312
	.word	.LC305
	.word	.LC313
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
	sub	sp, sp, #48
	movs	r4, #0
	str	r1, [sp, #8]
	str	r4, [r2]
.L2044:
	ldr	r3, [r6, #4]
	ldrh	r2, [r3, #20]
	cmp	r2, r4
	bhi	.L2053
.L2043:
	mov	r0, r4
	add	sp, sp, #48
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2053:
	cmp	r5, #0
	beq	.L2045
	ldr	r2, [r5, #4]
	ldrb	r2, [r2, r4]	@ zero_extendqisi2
	cmp	r2, #0
	bne	.L2045
	ldr	r2, [r5, #8]
	ldrh	r7, [r3, #26]
	ldrb	r8, [r2, r4]	@ zero_extendqisi2
	add	r7, r7, r8
.L2046:
	ldr	r2, [sp, #8]
	add	r1, sp, #32
	ldrh	r3, [r3, #28]
	mov	r0, r6
	strh	r4, [sp, #32]	@ movhi
	ldrh	r2, [r2]
	strh	r3, [sp, #36]	@ movhi
	strd	r8, r7, [sp, #40]
	strh	r2, [sp, #34]	@ movhi
	ldr	r3, [r6, #1980]
	blx	r3
	mov	r10, r0
	cbz	r0, .L2047
	cmp	r0, #10
	beq	.L2047
	ldr	r3, [sp, #8]
	mov	r2, r4
	ldr	r0, .L2089
	ldrh	r1, [r3]
	bl	NAND_Print
	ldr	r3, [fp]
	adds	r3, r3, #1
	str	r3, [fp]
.L2047:
	mov	r0, r7
	bl	is_nouse_page
	cmp	r0, #1
	beq	.L2062
	adds	r3, r4, #1
	cmp	r10, #0
	beq	.L2049
	cmp	r10, #10
	bne	.L2050
.L2049:
	cmp	r8, #0
	beq	.L2050
	mov	r2, r7
	mov	r1, r8
	mov	r0, r6
	str	r3, [sp, #12]
	bl	nand_power_off_ecc_error
	ldr	r3, [sp, #12]
	cbz	r0, .L2052
	ldr	r0, .L2089+4
	bl	NAND_Print
	ldr	r3, [sp, #12]
.L2050:
	mov	r4, r3
	b	.L2044
.L2045:
	ldrb	r2, [r3, #8]	@ zero_extendqisi2
	cmp	r2, #8
	ittee	eq
	ldreq	r8, [r6, #284]
	addeq	r7, sp, #16
	addne	r7, sp, #16
	movne	r8, #0
	b	.L2046
.L2052:
	ldr	r2, [r5, #4]
	movs	r1, #1
	strb	r1, [r2, r4]
	ldr	r2, [r5]
	cmp	r2, r3
	it	cc
	strcc	r3, [r5]
	b	.L2050
.L2059:
	cmp	r5, #0
	beq	.L2064
	ldr	r2, [r5, #4]
	ldrb	r2, [r2, r8]	@ zero_extendqisi2
	cmp	r2, #0
	bne	.L2064
	ldr	r2, [r5, #8]
	ldrh	r7, [r3, #26]
	ldrb	r10, [r2, r8]	@ zero_extendqisi2
	add	r7, r7, r10
.L2054:
	ldr	r2, [sp, #8]
	add	r1, sp, #32
	ldrh	r3, [r3, #28]
	mov	r0, r6
	strh	r8, [sp, #32]	@ movhi
	ldrh	r2, [r2]
	strh	r3, [sp, #36]	@ movhi
	strd	r10, r7, [sp, #40]
	strh	r2, [sp, #34]	@ movhi
	ldr	r3, [r6, #1980]
	blx	r3
	mov	fp, r0
	mov	r0, r7
	bl	is_nouse_page
	cmp	r0, #1
	beq	.L2056
	ldr	r3, [sp, #8]
	mov	r2, r8
	ldr	r0, .L2089+8
	ldrh	r1, [r3]
	bl	NAND_Print
	ldrb	r0, [r7, #4]	@ zero_extendqisi2
	ldrb	r3, [r7, #2]	@ zero_extendqisi2
	ldrb	r2, [r7, #1]	@ zero_extendqisi2
	ldrb	r1, [r7]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [r7, #3]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L2089+12
	bl	NAND_Print
	ldrb	r0, [r7, #9]	@ zero_extendqisi2
	ldrb	r3, [r7, #7]	@ zero_extendqisi2
	ldrb	r2, [r7, #6]	@ zero_extendqisi2
	ldrb	r1, [r7, #5]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [r7, #8]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L2089+16
	bl	NAND_Print
	cmp	fp, #0
	beq	.L2057
	cmp	fp, #10
	bne	.L2056
.L2057:
	cmp	r10, #0
	beq	.L2056
	ldr	r2, [r5, #4]
	movs	r1, #1
	add	r3, r8, #1
	strb	r1, [r2, r8]
	ldr	r2, [r5]
	cmp	r2, r3
	it	cc
	strcc	r3, [r5]
.L2056:
	add	r8, r8, #1
.L2048:
	ldr	r3, [r6, #4]
	ldrh	r2, [r3, #20]
	cmp	r2, r8
	bhi	.L2059
	b	.L2043
.L2064:
	add	r7, sp, #16
	mov	r10, #0
	b	.L2054
.L2062:
	mov	r8, r4
	b	.L2048
.L2090:
	.align	2
.L2089:
	.word	.LC314
	.word	.LC315
	.word	.LC316
	.word	.LC132
	.word	.LC133
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
	mov	r10, r0
	mov	r8, r3
	mov	fp, r3
	mov	r4, #-1
	mov	r5, r3
	sub	sp, sp, #48
	str	r3, [r2]
	str	r1, [sp, #4]
.L2092:
	ldr	r2, [r10, #4]
	ldrh	r3, [r2, #20]
	cmp	r3, r5
	bhi	.L2100
	adds	r2, r4, #1
	bne	.L2098
	mov	r4, r3
.L2101:
	mov	r0, r4
	add	sp, sp, #48
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2100:
	cbz	r6, .L2106
	ldr	r3, [r6, #4]
	ldrb	r3, [r3, r5]	@ zero_extendqisi2
	cbnz	r3, .L2106
	ldrh	r7, [r2, #26]
	ldr	r3, [r6, #8]
	add	r1, r7, #64
	mla	r3, r5, r1, r3
	add	r7, r7, r3
.L2093:
	ldr	r1, [sp, #4]
	mov	r0, r10
	ldrh	r2, [r2, #28]
	str	r3, [sp, #40]
	ldrh	r1, [r1]
	strh	r2, [sp, #36]	@ movhi
	strh	r5, [sp, #32]	@ movhi
	strh	r1, [sp, #34]	@ movhi
	add	r1, sp, #32
	str	r7, [sp, #44]
	ldr	r2, [r10, #1980]
	str	r3, [sp, #8]
	blx	r2
	ldr	r3, [sp, #8]
	cbz	r0, .L2094
	cmp	r0, #10
	beq	.L2094
	ldr	r3, [sp, #4]
	cmp	r4, #-1
	it	eq
	moveq	r4, r5
	mov	r2, r5
	ldr	r0, .L2119
	ldrh	r1, [r3]
	bl	NAND_Print
.L2095:
	adds	r5, r5, #1
	b	.L2092
.L2106:
	add	r7, sp, #16
	movs	r3, #0
	b	.L2093
.L2094:
	mov	r0, r7
	str	r3, [sp, #8]
	bl	is_nouse_page
	cmp	r0, #1
	ldr	r3, [sp, #8]
	bne	.L2096
	add	r8, r8, #1
	cmp	r4, #-1
	it	eq
	moveq	r4, r5
	cmp	r8, #3
	bne	.L2095
.L2098:
	cmp	fp, r4
	beq	.L2101
	ldr	r1, [sp, #4]
	mov	r3, r4
	mov	r2, fp
	ldr	r0, .L2119+4
	ldrh	r1, [r1]
	bl	NAND_Print
	ldr	r2, [r10, #1968]
	ldr	r3, [r2, #172]
	adds	r3, r3, #1
	str	r3, [r2, #172]
	movs	r3, #0
.L2102:
	ldr	r2, [r6]
	cmp	r2, r3
	bls	.L2101
	ldr	r2, [r6, #4]
	ldrb	r2, [r2, r3]	@ zero_extendqisi2
	cbz	r2, .L2103
	ldr	r2, [r10, #4]
	ldr	r1, [r6, #8]
	ldrh	r0, [r2, #26]
	add	r1, r1, r0
	add	r2, r0, #64
	mla	r2, r3, r2, r1
	ldr	r1, [r10, #1968]
	ldr	r1, [r1, #172]
	strb	r1, [r2, #14]
.L2103:
	adds	r3, r3, #1
	b	.L2102
.L2096:
	mov	r2, r7
	mov	r1, r3
	mov	r0, r10
	bl	nand_power_off_ecc_error
	cbz	r0, .L2099
	ldr	r0, .L2119+8
	bl	NAND_Print
	b	.L2095
.L2099:
	adds	r1, r4, #1
	add	fp, fp, #1
	bne	.L2095
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
	b	.L2095
.L2120:
	.align	2
.L2119:
	.word	.LC317
	.word	.LC319
	.word	.LC318
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
	ldr	fp, .L2143+4
	sub	sp, sp, #24
	mov	r7, r1
	mov	r10, #0
.L2122:
	mov	r0, r4
	bl	out_phy_block_from_free_list
	mov	r5, r0
	cbnz	r0, .L2123
.L2127:
	ldr	r0, .L2143
	bl	NAND_Print
	movs	r0, #1
.L2121:
	add	sp, sp, #24
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2123:
	ldr	r3, [r4, #4]
	add	r1, sp, #8
	ldrh	r2, [r0]
	mov	r0, r4
	strh	r10, [sp, #8]	@ movhi
	ldrh	r3, [r3, #28]
	strh	r2, [sp, #10]	@ movhi
	strd	r10, r10, [sp, #16]
	strh	r3, [sp, #12]	@ movhi
	ldr	r3, [r4, #1976]
	blx	r3
	ldrh	r3, [r5, #6]
	strh	r10, [r5, #4]	@ movhi
	adds	r3, r3, #1
	strh	r3, [r5, #6]	@ movhi
	cbz	r0, .L2125
	ldr	r3, [r4, #1996]
	add	r1, sp, #8
	mov	r0, r4
	blx	r3
	mov	r0, fp
	bl	NAND_Print
	ldrh	r2, [r4, #380]
	ldrh	r3, [r4, #22]
	cmp	r3, r2, lsr #2
	bhi	.L2122
	b	.L2127
.L2125:
	ldr	r3, [r7]
	mov	r8, r0
	ldr	r3, [r3, #8]
	adds	r3, r3, #1
	str	r3, [r5, #8]
.L2128:
	ldrh	r3, [r7, #28]
	ldr	r1, [r7]
	cmp	r3, r8
	bhi	.L2131
.L2129:
	ldrh	r3, [r1, #4]
	movs	r6, #0
	ldrh	r2, [r1]
	mov	r0, r4
	strh	r6, [sp, #8]	@ movhi
	add	r1, sp, #8
	strh	r3, [r5, #4]	@ movhi
	ldr	r3, [r4, #4]
	strh	r2, [sp, #10]	@ movhi
	strd	r6, r6, [sp, #16]
	ldrh	r3, [r3, #28]
	strh	r3, [sp, #12]	@ movhi
	ldr	r3, [r4, #1996]
	blx	r3
	ldr	r3, [r7]
	ldrh	r3, [r3, #2]
	strh	r3, [r5, #2]	@ movhi
	str	r5, [r7]
.L2132:
	ldrh	r3, [r7, #28]
	cmp	r3, r6
	bhi	.L2136
	movs	r0, #0
	b	.L2121
.L2131:
	ldr	r3, [r7, #12]
	ldr	r3, [r3, r8, lsl #2]
	adds	r3, r3, #1
	beq	.L2129
	uxth	r3, r8
	mov	r2, r5
	mov	r0, r4
	ldr	r6, [r4, #1988]
	str	r3, [sp]
	ldr	r3, [r4, #284]
	blx	r6
	cbz	r0, .L2130
	ldr	r3, [r4, #4]
	add	r1, sp, #8
	ldrh	r2, [r5]
	mov	r0, r4
	strh	r10, [sp, #8]	@ movhi
	ldrh	r3, [r3, #28]
	strh	r2, [sp, #10]	@ movhi
	strd	r10, r10, [sp, #16]
	strh	r3, [sp, #12]	@ movhi
	ldr	r3, [r4, #1996]
	blx	r3
	b	.L2122
.L2130:
	add	r8, r8, #1
	b	.L2128
.L2136:
	ldr	r3, [r7, #12]
	ldr	r1, [r3, r6, lsl #2]
	adds	r3, r1, #1
	beq	.L2134
	mov	r0, r4
	bl	get_logic_page_map
	cbz	r0, .L2134
	ldr	r3, [r7]
	ldrh	r3, [r3]
	strh	r3, [r0, #2]	@ movhi
	ldrh	r3, [r0]
	bfi	r3, r6, #0, #12
	strh	r3, [r0]	@ movhi
.L2134:
	adds	r6, r6, #1
	b	.L2132
.L2144:
	.align	2
.L2143:
	.word	.LC320
	.word	.LC321
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
	mov	r5, r1
	mov	fp, r2
	movs	r1, #165
	movs	r2, #16
	mov	r0, r7
	str	r3, [sp, #12]
	ldr	r6, [sp, #80]
	bl	NAND_Memset
	bl	is_physic_info_enable_crc
	cbz	r0, .L2146
	ldr	r3, [r4, #4]
	mov	r1, fp
	movw	r0, #65535
	ldrb	r2, [r3, #8]	@ zero_extendqisi2
	ldr	r3, .L2166
	cmp	r6, r3
	itet	ne
	lslne	r2, r2, #9
	ldrheq	r2, [r4, #308]
	uxthne	r2, r2
	bl	nand_crc16
	lsrs	r3, r0, #8
	strb	r0, [sp, #29]
	strb	r3, [sp, #28]
.L2146:
	mov	r8, #3
.L2155:
	ldr	r0, [r5]
	ldr	lr, [r4, #4]
	ldr	r3, .L2166
	ldrh	r1, [r0]
	ldrh	r2, [r5, #28]
	ldrh	ip, [lr, #28]
	cmp	r6, r3
	strh	r1, [sp, #34]	@ movhi
	strh	r2, [sp, #32]	@ movhi
	strh	ip, [sp, #36]	@ movhi
	strd	fp, r7, [sp, #40]
	bne	.L2149
	ldrh	r3, [r0, #6]
	mov	r2, r6
	str	r3, [sp]
	ldr	r3, [r0, #8]
.L2165:
	mov	r1, r7
	mov	r0, r4
	bl	set_oob_special_page
	b	.L2150
.L2149:
	cmp	r6, #192
	bne	.L2151
	ldrh	r3, [lr, #20]
	subs	r3, r3, #1
	cmp	r2, r3
	bne	.L2152
	ldr	r0, .L2166+4
	bl	NAND_Print
.L2152:
	ldr	r3, [r5]
	mov	r1, r7
	mov	r0, r4
	ldrh	r2, [r3, #6]
	str	r2, [sp]
	ldr	r2, [sp, #12]
	ldr	r3, [r3, #8]
	bl	set_oob_logic_page
.L2150:
	ldr	r3, [r5]
	mov	r10, #1
	add	r1, sp, #32
	mov	r0, r4
	strh	r10, [r3, #2]	@ movhi
	ldr	r3, [r4, #1984]
	blx	r3
	cbz	r0, .L2145
	subs	r8, r8, #1
	beq	.L2156
	ldrh	r1, [sp, #34]
	ldr	r0, .L2166+8
	bl	NAND_Print
	mov	r1, r5
	mov	r0, r4
	bl	do_write_error
	cmp	r0, #0
	beq	.L2155
	ldr	r0, .L2166+12
	bl	NAND_Print
	mov	r0, r10
.L2145:
	add	sp, sp, #48
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2151:
	ldr	r3, .L2166+16
	cmp	r6, r3
	bne	.L2150
	ldrh	r3, [lr, #20]
	subs	r3, r3, #1
	cmp	r2, r3
	bne	.L2153
	ldr	r0, .L2166+20
	bl	NAND_Print
.L2153:
	ldr	r3, [r5]
	ldrh	r2, [r3, #6]
	str	r2, [sp]
	ldr	r2, .L2166+16
	ldr	r3, [r3, #8]
	b	.L2165
.L2156:
	mov	r0, r8
	b	.L2145
.L2167:
	.align	2
.L2166:
	.word	-1431633921
	.word	.LC322
	.word	.LC324
	.word	.LC325
	.word	-1441713357
	.word	.LC323
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
	bne	.L2169
	mov	r1, r3
	bl	get_logic_page_map
	mov	r10, r0
	cbnz	r0, .L2170
.L2176:
	movs	r7, #1
	ldr	r0, .L2192
	bl	NAND_Print_DBG
.L2168:
	mov	r0, r7
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2170:
	ldrh	r3, [r0, #2]
	movw	r2, #65535
	cmp	r3, r2
	beq	.L2184
	ldr	r2, [r8, #4]
	movs	r5, #36
	ldr	r1, [r2, #32]
	mla	r5, r5, r3, r1
	ldr	r3, [r4]
	cmp	r3, r5
	bne	.L2183
	ldrh	r3, [r5, #4]
	ldrh	r2, [r2, #20]
	adds	r3, r3, #1
	uxth	r3, r3
	cmp	r2, r3
	strh	r3, [r5, #4]	@ movhi
	bcs	.L2184
	ldrh	r1, [r4, #28]
	ldr	r0, .L2192+4
	bl	NAND_Print
.L2184:
	movs	r6, #0
	b	.L2172
.L2183:
	movs	r6, #1
.L2172:
	movs	r3, #192
	ldr	r2, [sp, #40]
	str	r3, [sp]
	mov	r1, r4
	mov	r3, fp
	mov	r0, r8
	bl	write_phy_data_to_current_block
	mov	r7, r0
	cbz	r0, .L2173
	ldr	r0, .L2192+8
.L2191:
	bl	NAND_Print
	b	.L2168
.L2173:
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
.L2174:
	cmp	r6, #0
	beq	.L2179
	mov	r1, r5
	mov	r0, r8
	bl	phy_block_from_invalid_page_incr
	b	.L2179
.L2169:
	ldr	r6, .L2192+12
	cmp	r2, r6
	bne	.L2175
	mov	r1, r3
	bl	get_logic_page_map
	mov	r7, r0
	cmp	r0, #0
	beq	.L2176
	ldrh	r3, [r0, #2]
	movw	r2, #65535
	cmp	r3, r2
	beq	.L2187
	ldr	r2, [r8, #4]
	movs	r5, #36
	ldr	r1, [r2, #32]
	mla	r5, r5, r3, r1
	ldr	r3, [r4]
	cmp	r3, r5
	bne	.L2186
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
	bcs	.L2187
	ldrh	r1, [r4, #28]
	ldr	r0, .L2192+4
	bl	NAND_Print
.L2187:
	movs	r6, #0
	b	.L2177
.L2186:
	movs	r6, #1
.L2177:
	movw	r3, #65535
	movw	r2, #4095
	strh	r3, [r7, #2]	@ movhi
	ldrh	r3, [r7]
	bfi	r3, r2, #0, #12
	strh	r3, [r7]	@ movhi
	ubfx	r3, r3, #8, #8
	orr	r3, r3, #16
	strb	r3, [r7, #1]
	b	.L2174
.L2175:
	ldr	r5, .L2192+16
	cmp	r2, r5
	bne	.L2179
	ldr	r6, [r1]
	ldrh	r5, [r6, #4]
	adds	r5, r5, #1
	strh	r5, [r6, #4]	@ movhi
	str	r2, [sp]
	ldr	r2, [sp, #40]
	bl	write_phy_data_to_current_block
	mov	r7, r0
	cbz	r0, .L2180
	ldr	r0, .L2192+20
	b	.L2191
.L2180:
	ldrh	r3, [r4, #28]
	adds	r3, r3, #1
	strh	r3, [r4, #28]	@ movhi
.L2179:
	mov	r0, r8
	movs	r7, #0
	bl	adjust_invaild_list
	b	.L2168
.L2193:
	.align	2
.L2192:
	.word	.LC167
	.word	.LC326
	.word	.LC327
	.word	-1441722095
	.word	-1441713357
	.word	.LC328
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
	cbz	r0, .L2195
.L2198:
	movs	r5, #1
	ldr	r0, .L2200
	bl	NAND_Print
.L2194:
	mov	r0, r5
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, pc}
.L2195:
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r3, r3, #31
	bpl	.L2194
	str	r8, [sp]
	mov	r3, r7
	mov	r2, r6
	add	r1, r4, #324
	mov	r0, r4
	bl	write_logic_data_to_current_block
	cmp	r0, #0
	beq	.L2194
	b	.L2198
.L2201:
	.align	2
.L2200:
	.word	.LC329
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
	ldr	r1, .L2203
	b	write_data_to_page
.L2204:
	.align	2
.L2203:
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
	cbz	r3, .L2206
	ldr	r3, [r6, #4]
	mov	r0, r6
	ldrh	r1, [r3, #16]
	bl	out_phy_block_from_free_list_by_erase_time
	str	r0, [r4]
	cbz	r0, .L2207
	ldrh	r2, [r0, #6]
	ldrh	r1, [r0]
	ldr	r0, .L2214
	bl	NAND_Print
.L2207:
	ldr	r1, [r4]
	cbnz	r1, .L2208
	ldr	r0, .L2214+4
	bl	NAND_Print
	movs	r0, #1
.L2205:
	pop	{r3, r4, r5, r6, r7, pc}
.L2206:
	mov	r0, r6
	bl	out_phy_block_from_free_list
	str	r0, [r4]
	b	.L2207
.L2208:
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
	b	.L2205
.L2215:
	.align	2
.L2214:
	.word	.LC330
	.word	.LC331
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
	ldr	r3, [r0, #1976]
	blx	r3
	ldrh	r3, [r5, #6]
	ldr	r2, [r4, #376]
	strh	r6, [r5, #4]	@ movhi
	adds	r3, r3, #1
	uxth	r3, r3
	cmp	r3, r2
	strh	r3, [r5, #6]	@ movhi
	bls	.L2217
	ldr	r2, [r4, #4]
	ldrh	r2, [r2, #16]
	cmp	r2, r3
	it	hi
	strhi	r3, [r4, #376]
.L2217:
	str	r7, [r5, #8]
	cbz	r0, .L2216
	ldr	r3, [r4, #1996]
	mov	r1, sp
	mov	r0, r4
	blx	r3
	ldr	r0, .L2222
	bl	NAND_Print
	movs	r0, #1
.L2216:
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L2223:
	.align	2
.L2222:
	.word	.LC332
	.size	new_block_init_for_write, .-new_block_init_for_write
	.section	.text.cross_talk_rebuild_current_block,"ax",%progbits
	.align	1
	.global	cross_talk_rebuild_current_block
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	cross_talk_rebuild_current_block, %function
cross_talk_rebuild_current_block:
	@ args = 0, pretend = 0, frame = 56
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	sub	sp, sp, #72
	mov	r6, r1
	mov	r8, #0
	add	r1, sp, #72
	mov	r5, r0
	str	r8, [r1, #-36]!
	adds	r0, r0, #4
	mov	r4, r2
	bl	init_nrc.isra.0
	cbz	r0, .L2225
	ldr	r0, [sp, #36]
	mov	r4, r8
	bl	free_nrc
	ldr	r0, .L2244
	bl	NAND_Print
.L2226:
	mov	r0, r4
	add	sp, sp, #72
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2225:
	ldr	r3, [sp, #36]
	add	r2, sp, #28
	mov	r1, r6
	mov	r0, r5
	bl	get_used_page_num
	mov	r10, r0
	cbnz	r4, .L2227
	ldr	r3, [r6, #8]
	mov	r2, r0
	ldrh	r1, [r6]
	ldr	r0, .L2244+4
	bl	NAND_Print
	ldr	r0, [sp, #36]
	ldr	r3, [r0]
	cmp	r3, #0
	bne	.L2228
	bl	free_nrc
	ldr	r0, .L2244+8
	bl	NAND_Print
	ldrh	r2, [r6, #6]
	mov	r1, r6
	mov	r0, r5
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
	b	.L2226
.L2227:
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
	ldr	r0, .L2244+12
	str	r3, [sp]
	ldr	r3, [r6, #8]
	bl	NAND_Print
	ldr	r0, [sp, #36]
	ldr	r3, [r0]
	cbnz	r3, .L2229
	str	r3, [sp, #20]
	bl	free_nrc
	ldr	r0, .L2244+16
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
	b	.L2226
.L2229:
	ldrh	r8, [r4, #6]
	cmp	r10, fp
	ldrh	r7, [r6, #6]
	ittt	ls
	movls	r2, r8
	movls	r8, r7
	movls	r7, r2
	ldr	r2, [sp, #28]
	cbnz	r2, .L2231
	ldr	r2, [sp, #32]
	cbz	r2, .L2232
.L2231:
	add	r8, r8, #20
	adds	r7, r7, #20
.L2232:
	cmp	r10, fp
	add	r8, r8, #1
	uxth	r8, r8
	mov	r2, fp
	mov	r1, r10
	bcs	.L2233
	ldr	r0, .L2244+20
	bl	NAND_Print
	mov	r1, r6
	mov	r6, r4
	mov	r2, r8
	mov	r0, r5
	bl	erase_block
.L2228:
	ldr	r8, .L2244+36
.L2236:
	mov	r0, r5
	bl	out_phy_block_from_free_list
	mov	r4, r0
	cbnz	r0, .L2234
	ldr	r0, .L2244+24
	bl	NAND_Print
	b	.L2226
.L2233:
	ldr	r0, .L2244+28
	bl	NAND_Print
	mov	r2, r8
	mov	r1, r4
	mov	r0, r5
	bl	erase_block
	b	.L2228
.L2234:
	ldr	r2, [r6, #8]
	mov	r1, r0
	adds	r2, r2, #1
	str	r2, [r0, #8]
	mov	r0, r5
	bl	new_block_init_for_write
	cmp	r0, #0
	beq	.L2240
	mov	r0, r8
	bl	NAND_Print
	b	.L2236
.L2239:
	ldr	r1, [r3, #4]
	ldrb	r1, [r1, r8]	@ zero_extendqisi2
	cmp	r1, #0
	beq	.L2237
	ldr	r3, [r3, #8]
	movs	r2, #16
	mov	r0, fp
	ldrb	r1, [r3, r8]	@ zero_extendqisi2
	ldr	r3, [r5, #4]
	ldrh	r3, [r3, #26]
	add	r1, r1, r3
	bl	NAND_Memcpy
	ldr	r3, [r5, #4]
	mov	r0, fp
	ldrh	r1, [r4]
	ldrh	r2, [r3, #28]
	ldr	r3, [sp, #36]
	ldr	r3, [r3, #8]
	ldrb	r3, [r3, r8]	@ zero_extendqisi2
	strh	r10, [sp, #56]	@ movhi
	add	r10, r10, #1
	strh	r1, [sp, #58]	@ movhi
	strh	r2, [sp, #60]	@ movhi
	strd	r3, fp, [sp, #64]
	bl	get_special_data_from_oob
	ldrh	r3, [r4, #6]
	mov	r2, r0
	mov	r1, fp
	mov	r0, r5
	str	r3, [sp]
	ldr	r3, [r4, #8]
	bl	set_oob_special_page
	ldr	r3, [r5, #1984]
	add	r1, sp, #56
	mov	r0, r5
	blx	r3
.L2238:
	add	r8, r8, #1
.L2235:
	ldr	r3, [sp, #36]
	ldr	r2, [r3]
	cmp	r2, r8
	bhi	.L2239
	adds	r2, r7, #1
	mov	r1, r6
	uxth	r2, r2
	mov	r0, r5
	bl	erase_block
	ldr	r0, [sp, #36]
	bl	free_nrc
	b	.L2226
.L2237:
	mov	r1, r8
	ldr	r0, .L2244+32
	bl	NAND_Print
	b	.L2238
.L2240:
	mov	r10, r0
	mov	r8, r0
	add	fp, sp, #40
	b	.L2235
.L2245:
	.align	2
.L2244:
	.word	.LC333
	.word	.LC334
	.word	.LC335
	.word	.LC336
	.word	.LC337
	.word	.LC338
	.word	.LC340
	.word	.LC339
	.word	.LC342
	.word	.LC341
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
	add	fp, r0, #4
	sub	sp, sp, #72
	mov	r5, r0
	mov	r6, r1
	movs	r7, #0
	add	r1, sp, #32
	mov	r0, fp
	mov	r4, r2
	strd	r7, r7, [sp, #32]
	bl	init_nrc.isra.0
	cbz	r0, .L2247
	ldr	r0, .L2280
	bl	NAND_Print
	ldr	r0, [sp, #32]
	bl	free_nrc
.L2278:
	movs	r4, #0
	b	.L2248
.L2247:
	bl	get_timer_masked
	add	r2, sp, #24
	mov	r1, r6
	mov	r7, r0
	ldr	r3, [sp, #32]
	mov	r0, r5
	bl	get_used_page_num_no_crosstalk
	mov	r8, r0
	bl	get_timer_masked
	mov	r1, r8
	subs	r2, r0, r7
	ldr	r0, .L2280+4
	bl	printf
	cbnz	r4, .L2249
	ldr	r3, [r6, #8]
	mov	r2, r8
	ldrh	r1, [r6]
	ldr	r0, .L2280+8
	bl	printf
	ldr	r7, [sp, #32]
	ldr	r3, [r7]
	cbz	r3, .L2277
	ldr	r8, .L2280+52
.L2250:
	mov	r0, r5
	bl	out_phy_block_from_free_list
	mov	r4, r0
	cbnz	r0, .L2251
.L2259:
	ldr	r0, .L2280+12
.L2279:
	bl	NAND_Print
	b	.L2278
.L2277:
	mov	r0, r7
	bl	free_nrc
	ldr	r0, .L2280+16
	bl	NAND_Print
	ldrh	r2, [r6, #6]
	mov	r1, r6
	mov	r0, r5
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
.L2248:
	mov	r0, r4
	add	sp, sp, #72
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2251:
	ldr	r2, [r6, #8]
	mov	r1, r0
	str	r2, [r0, #8]
	mov	r0, r5
	bl	new_block_init_for_write
	cmp	r0, #0
	beq	.L2252
	mov	r0, r8
	bl	NAND_Print
	b	.L2250
.L2249:
	add	r1, sp, #36
	mov	r0, fp
	bl	init_nrc.isra.0
	cbz	r0, .L2253
	ldr	r0, [sp, #32]
	bl	free_nrc
	ldr	r0, [sp, #36]
	bl	free_nrc
	ldr	r0, .L2280+20
	b	.L2279
.L2253:
	bl	get_timer_masked
	ldr	r3, [sp, #36]
	mov	r7, r0
	add	r2, sp, #28
	mov	r1, r4
	mov	r0, r5
	bl	get_used_page_num_no_crosstalk
	mov	r10, r0
	bl	get_timer_masked
	mov	r1, r10
	subs	r2, r0, r7
	ldr	r0, .L2280+4
	bl	printf
	ldr	r3, [r4, #8]
	mov	r2, r8
	ldrh	r1, [r6]
	ldr	r0, .L2280+24
	strd	r10, r3, [sp, #4]
	ldrh	r3, [r4]
	str	r3, [sp]
	ldr	r3, [r6, #8]
	bl	NAND_Print
	ldr	r7, [sp, #36]
	ldr	fp, [r7]
	cmp	fp, #0
	bne	.L2254
	ldr	r0, [sp, #32]
	bl	free_nrc
	ldr	r0, [sp, #36]
	bl	free_nrc
	ldr	r0, .L2280+28
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
	b	.L2278
.L2254:
	cmp	r8, r10
	bcs	.L2255
	mov	r3, fp
	mov	r2, r10
	mov	r1, r8
	ldr	r0, .L2280+32
	bl	NAND_Print
	mov	r3, r6
	mov	r6, r4
	mov	r4, r3
.L2256:
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
	ldr	r3, [r5, #1976]
	blx	r3
	cbz	r0, .L2257
	ldrh	r1, [r4]
	ldr	r0, .L2280+36
	bl	NAND_Print
	ldr	r3, [r5, #1996]
	add	r1, sp, #40
	mov	r0, r5
	blx	r3
	movs	r3, #255
	strh	r8, [r6, #4]	@ movhi
	strh	r3, [r6, #2]	@ movhi
	ldr	r8, .L2280+52
	ldrh	r3, [r5, #20]
	adds	r3, r3, #1
	strh	r3, [r5, #20]	@ movhi
.L2258:
	mov	r0, r5
	bl	out_phy_block_from_free_list
	mov	r4, r0
	cmp	r0, #0
	beq	.L2259
	ldr	r2, [r0, #8]
	mov	r1, r0
	mov	r0, r5
	bl	new_block_init_for_write
	cbz	r0, .L2257
	mov	r0, r8
	bl	NAND_Print
	b	.L2258
.L2255:
	ldr	r7, [sp, #32]
	mov	r2, r10
	mov	r1, r8
	ldr	r0, .L2280+40
	ldr	r3, [r7]
	bl	NAND_Print
	b	.L2256
.L2257:
	ldr	r3, [r6, #8]
	str	r3, [r4, #8]
.L2252:
	mov	r10, #0
	bl	get_timer_masked
	mov	r8, r10
	mov	fp, r0
.L2260:
	ldr	r2, [r7]
	cmp	r2, r8
	bhi	.L2263
	bl	get_timer_masked
	ldr	r1, [r7]
	sub	r2, r0, fp
	ldr	r0, .L2280+44
	bl	printf
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
	b	.L2248
.L2263:
	ldr	r3, [r7, #4]
	ldrb	r3, [r3, r8]	@ zero_extendqisi2
	cbz	r3, .L2261
	ldr	r0, [r5, #4]
	ldr	r3, [r7, #8]
	ldrh	ip, [r4]
	ldrh	r1, [r0, #26]
	ldrh	r0, [r0, #28]
	strh	ip, [sp, #58]	@ movhi
	add	r2, r1, #64
	strh	r10, [sp, #56]	@ movhi
	mul	r2, r8, r2
	strh	r0, [sp, #60]	@ movhi
	add	r10, r10, #1
	add	r1, r1, r2
	add	r1, r1, r3
	add	r3, r3, r2
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
	ldr	r3, [r5, #1984]
	add	r1, sp, #56
	mov	r0, r5
	blx	r3
.L2262:
	add	r8, r8, #1
	b	.L2260
.L2261:
	mov	r1, r8
	ldr	r0, .L2280+48
	bl	NAND_Print
	b	.L2262
.L2281:
	.align	2
.L2280:
	.word	.LC343
	.word	.LC344
	.word	.LC345
	.word	.LC340
	.word	.LC346
	.word	.LC337
	.word	.LC347
	.word	.LC348
	.word	.LC338
	.word	.LC349
	.word	.LC339
	.word	.LC350
	.word	.LC342
	.word	.LC341
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
	@ args = 0, pretend = 0, frame = 56
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r10, fp, lr}
	mov	r4, r0
	sub	sp, sp, #72
	bl	get_timer_masked
	ldr	r3, [r4, #4]
	str	r0, [sp, #32]
	ldr	r0, [r4, #24]
	ldrb	r1, [r3, #8]	@ zero_extendqisi2
	bl	__aeabi_uidiv
	ldr	r3, [r4, #252]
	mov	r2, #-1
	str	r0, [r4, #28]
	str	r2, [sp, #24]
	mov	r10, r3
.L2283:
	cmp	r10, #0
	bne	.L2284
	cbz	r3, .L2285
	ldr	r2, [r4, #1968]
	ldr	r3, [r3, #8]
	str	r3, [r2, #164]
.L2285:
	bl	get_timer_masked
	ldr	r6, [r4, #252]
	movs	r5, #0
	mov	r7, r5
	mov	r8, r5
	mov	r3, #-1
	str	r0, [sp, #36]
	str	r5, [sp, #28]
	str	r3, [sp, #20]
.L2286:
	cmp	r6, #0
	bne	.L2299
	bl	get_timer_masked
	ldr	r3, [sp, #36]
	subs	r1, r0, r3
	ldr	r0, .L2396
	bl	printf
	mov	r1, r10
	mov	r0, r4
	bl	init_smart_info
	ldr	r6, [r4, #252]
.L2300:
	cmp	r6, #0
	bne	.L2302
	mov	r0, r4
	bl	adjust_invaild_list
	cmp	r8, #0
	beq	.L2303
	cbz	r7, .L2303
	ldr	r1, [r8, #8]
	ldr	r2, [r7, #8]
	adds	r3, r1, #1
	cmp	r3, r2
	beq	.L2303
	ldr	r0, .L2396+4
	bl	NAND_Print
.L2303:
	movs	r3, #0
	str	r3, [r4, #292]
	cmp	r7, #0
	bne	.L2304
	cmp	r5, #0
	bne	.L2305
	ldr	r0, .L2396+8
	bl	NAND_Print_DBG
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r0, r3, #31
	bpl	.L2306
	cmp	r8, #0
	beq	.L2307
.L2391:
	movs	r2, #3
	mov	r1, r8
	mov	r0, r4
	bl	add_prio_gc
	b	.L2307
.L2284:
	ldr	r2, [r10, #8]
	ldr	r10, [r10, #28]
	str	r2, [sp, #24]
	ldr	r1, [sp, #24]
	ldr	r2, [r4, #1968]
	str	r1, [r2, #160]
	b	.L2283
.L2299:
	ldr	r3, [r4, #4]
	ldr	r2, [r4, #284]
	ldrh	r0, [r6]
	ldrh	r1, [r3, #28]
	ldrh	r3, [r3, #20]
	str	r2, [sp, #64]
	add	r2, sp, #40
	str	r2, [sp, #68]
	ldr	r2, .L2396+12
	subs	r3, r3, #1
	strh	r3, [sp, #56]	@ movhi
	movs	r3, #1
	strh	r0, [sp, #58]	@ movhi
	mov	r0, r4
	str	r3, [r2]
	strh	r1, [sp, #60]	@ movhi
	add	r1, sp, #56
	ldr	r3, [r4, #1980]
	blx	r3
	ldr	r2, .L2396+12
	movs	r3, #0
	mov	fp, r0
	str	r3, [r2]
	bl	is_physic_info_enable_crc
	add	r2, sp, #40
	cbz	r0, .L2287
	ldr	r1, [r4, #284]
	mov	r0, r4
	bl	crc_wrong
	cmp	r0, #0
	it	ne
	mvnne	fp, #1
.L2287:
	ldr	r3, [r6, #8]
	ldr	r1, [r4, #376]
	ldr	r2, [r4, #1968]
	str	r3, [sp, #20]
	ldrh	r3, [r6, #6]
	cmp	r3, r1
	bls	.L2288
	movw	r1, #49999
	cmp	r3, r1
	itt	ls
	strls	r3, [r4, #376]
	strls	r3, [r2, #152]
.L2288:
	ldr	r1, [r2, #156]
	add	r0, sp, #40
	cmp	r3, r1
	it	cc
	strcc	r3, [r2, #156]
	bl	is_phy_mapping_page
	cmp	r0, #1
	bne	.L2290
	cmp	fp, #0
	beq	.L2291
	cmp	fp, #10
	bne	.L2290
.L2291:
	ldrh	r3, [r6, #4]
	mov	r10, r6
	ldr	r2, [r4, #284]
	mov	r1, r6
	mov	r0, r4
	adds	r3, r3, #1
	strh	r3, [r6, #4]	@ movhi
	ldr	r3, [r4, #4]
	ldrh	r3, [r3, #20]
	subs	r3, r3, #1
	bl	recover_logic_page_mapping
.L2388:
	mov	r8, r6
	b	.L2292
.L2290:
	add	r0, sp, #40
	bl	is_nouse_page
	cmp	r0, #1
	bne	.L2293
	ldrh	r1, [sp, #58]
	ldr	r0, .L2396+16
	bl	NAND_Print
	cbz	r7, .L2337
	ldrh	r1, [r7]
	cbnz	r5, .L2294
	ldrh	r2, [r6]
	ldr	r0, .L2396+20
	bl	NAND_Print
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r2, r3, #31
	bmi	.L2339
	ldr	r3, [r7, #8]
	ldr	r0, [r6, #8]
	cmp	r3, r0
	beq	.L2339
	ldrh	r2, [r6]
	ldrh	r1, [r7]
	str	r0, [sp]
	ldr	r0, .L2396+24
	bl	NAND_Print
	ldr	r2, [r4, #304]
	mov	r1, r7
	mov	r0, r4
	bl	recover_phy_page_mapping
	ldr	r3, [r4, #4]
	mov	r1, r7
	ldr	r2, [r4, #304]
	mov	r0, r4
	ldrh	r3, [r3, #20]
	subs	r3, r3, #1
	bl	recover_logic_page_mapping
	movs	r2, #3
	mov	r1, r7
	mov	r0, r4
	bl	add_prio_gc
.L2337:
	mov	r7, r6
	b	.L2292
.L2294:
	ldr	r2, [r7, #8]
	ldr	r0, .L2396+28
	bl	NAND_Print
	ldr	r2, [r5, #8]
	ldrh	r1, [r5]
	ldr	r0, .L2396+32
	bl	NAND_Print
	ldr	r2, [sp, #20]
	ldrh	r1, [r6]
	ldr	r0, .L2396+36
	bl	NAND_Print
	mov	r1, r7
	mov	r7, r5
	movs	r2, #3
	mov	r0, r4
	bl	add_prio_gc
.L2339:
	mov	r5, r6
	b	.L2292
.L2293:
	ldrb	r0, [sp, #46]	@ zero_extendqisi2
	ldrb	r2, [sp, #41]	@ zero_extendqisi2
	ldrb	r1, [sp, #40]	@ zero_extendqisi2
	str	r0, [sp, #12]
	ldrb	r0, [sp, #45]	@ zero_extendqisi2
	ldrb	r3, [sp, #42]	@ zero_extendqisi2
	str	r0, [sp, #8]
	ldrb	r0, [sp, #44]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [sp, #43]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L2396+40
	bl	NAND_Print
	ldrh	r2, [r6]
	ldrb	r1, [r4, #16]	@ zero_extendqisi2
	cmp	fp, #0
	beq	.L2295
	cmp	fp, #10
	bne	.L2296
.L2295:
	ldr	r0, .L2396+44
.L2387:
	bl	NAND_Print
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r3, r3, #31
	bpl	.L2298
	ldrd	r2, r3, [sp, #20]
	cmp	r3, r2
	bne	.L2298
	ldrh	r2, [r6, #6]
	mov	r1, r6
	mov	r0, r4
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
	str	r6, [sp, #28]
.L2292:
	ldr	r6, [r6, #28]
	b	.L2286
.L2296:
	ldr	r0, .L2396+48
	b	.L2387
.L2298:
	movs	r3, #170
	ldr	r2, [r4, #304]
	strh	r3, [r6, #2]	@ movhi
	mov	r1, r6
	ldrh	r3, [r6, #4]
	mov	r0, r4
	adds	r3, r3, #1
	strh	r3, [r6, #4]	@ movhi
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
	b	.L2388
.L2302:
	cmp	r6, r7
	beq	.L2301
	cmp	r6, r5
	beq	.L2301
	ldr	r3, [sp, #28]
	cmp	r6, r3
	beq	.L2301
	mov	r1, r6
	mov	r0, r4
	bl	put_phy_block_to_invalid_page_list
.L2301:
	ldr	r6, [r6, #28]
	b	.L2300
.L2397:
	.align	2
.L2396:
	.word	.LC360
	.word	.LC361
	.word	.LC362
	.word	.LANCHOR93
	.word	.LC351
	.word	.LC352
	.word	.LC353
	.word	.LC354
	.word	.LC355
	.word	.LC356
	.word	.LC357
	.word	.LC358
	.word	.LC359
.L2306:
	cmp	r8, #0
	beq	.L2307
	ldrh	r3, [r8, #2]
	cmp	r3, #170
	beq	.L2391
.L2307:
	bl	get_timer_masked
	mov	r6, r0
	cmp	r5, #0
	beq	.L2315
	ldr	r3, [r4, #292]
	cmp	r5, r3
	beq	.L2316
	movw	r2, #655
	ldr	r1, .L2398
	ldr	r0, .L2398+4
	bl	printf
.L2316:
	ldr	r3, [r4, #292]
	ldrh	r2, [r4, #320]
	ldr	r0, .L2398+8
	ldrh	r1, [r3]
	bl	NAND_Print_DBG
	ldrh	r3, [r4, #320]
	mov	r0, r4
	ldr	r2, [r4, #304]
	ldr	r1, [r4, #292]
	bl	recover_logic_page_mapping
	ldr	r3, [r4, #292]
	ldrh	r2, [r4, #320]
	ldrh	r3, [r3, #4]
	cmp	r2, r3
	bcs	.L2317
	ldr	r0, .L2398+12
	bl	NAND_Print
.L2317:
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r2, r3, #31
	bpl	.L2393
	ldr	r8, .L2398+88
	ldr	r10, .L2398+92
.L2318:
	ldr	r7, [r4, #324]
	cmp	r7, #0
	beq	.L2320
.L2329:
	ldr	r2, [r4, #336]
	mov	r0, r4
	ldr	r1, [r4, #324]
	bl	recover_phy_page_mapping
	uxth	r3, r0
	movw	r2, #65535
	cmp	r3, r2
	strh	r3, [r4, #352]	@ movhi
	bne	.L2386
	ldr	r0, .L2398+16
.L2392:
	bl	NAND_Print
	ldr	r1, [r4, #324]
	mov	r0, r4
	ldrh	r2, [r1, #6]
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
	movs	r3, #0
	str	r3, [r4, #324]
	b	.L2318
.L2304:
	cmp	r5, #0
	bne	.L2305
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r1, r3, #31
	bpl	.L2308
	ldr	r2, [r7, #8]
	ldrh	r1, [r7]
	ldr	r0, .L2398+20
	bl	NAND_Print_DBG
	cmp	r8, #0
	beq	.L2309
	ldrh	r3, [r8, #2]
	cmp	r3, #170
	bne	.L2309
	movs	r2, #3
	mov	r1, r8
	mov	r0, r4
	bl	add_prio_gc
.L2309:
	movs	r2, #0
.L2390:
	mov	r1, r7
	mov	r0, r4
	bl	cross_talk_rebuild_current_block
	mov	r5, r0
	cmp	r0, #0
	bne	.L2307
.L2395:
	movs	r0, #1
.L2282:
	add	sp, sp, #72
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2308:
	cmp	r8, #0
	beq	.L2311
	ldrh	r3, [r8, #2]
	cmp	r3, #170
	bne	.L2311
	movs	r2, #3
	mov	r1, r8
	mov	r0, r4
	bl	add_prio_gc
.L2311:
	bl	get_timer_masked
	mov	r1, r7
	mov	r6, r0
	movs	r2, #0
	mov	r0, r4
	bl	no_cross_talk_rebuild_current_block
	mov	r5, r0
	bl	get_timer_masked
	subs	r1, r0, r6
	ldr	r0, .L2398+24
	bl	printf
	b	.L2307
.L2305:
	ldrb	r6, [r4, #17]	@ zero_extendqisi2
	ands	r6, r6, #1
	beq	.L2312
	mov	r2, r5
	b	.L2390
.L2312:
	ldr	r2, [r7, #8]
	ldr	r3, [r5, #8]
	cmp	r2, r3
	bne	.L2314
	ldrh	r1, [r7]
	ldr	r0, .L2398+28
	bl	NAND_Print_DBG
	ldr	r2, [r5, #8]
	ldrh	r1, [r5]
	ldr	r0, .L2398+32
	bl	NAND_Print_DBG
	mov	r2, r5
	mov	r1, r7
	mov	r0, r4
	bl	no_cross_talk_rebuild_current_block
	mov	r5, r0
	b	.L2307
.L2314:
	ldr	r0, .L2398+36
	bl	NAND_Print
	ldr	r2, [r8, #8]
	ldrh	r1, [r8]
	ldr	r0, .L2398+40
	bl	NAND_Print
	ldr	r2, [r7, #8]
	ldrh	r1, [r7]
	ldr	r0, .L2398+44
	bl	NAND_Print
	ldr	r2, [r5, #8]
	ldrh	r1, [r5]
	ldr	r0, .L2398+48
	bl	NAND_Print
	ldr	r2, [r4, #304]
	mov	r1, r7
	mov	r0, r4
	bl	recover_block_phy_page_mapping
	ldr	r3, [r4, #4]
	mov	r1, r7
	ldr	r2, [r4, #304]
	mov	r0, r4
	ldrh	r3, [r3, #20]
	subs	r3, r3, #1
	bl	recover_logic_page_mapping
	movs	r2, #3
	mov	r1, r7
	mov	r0, r4
	bl	add_prio_gc
	mov	r1, r5
	ldr	r2, [r4, #304]
	mov	r0, r4
	bl	recover_block_phy_page_mapping
	ldr	r3, [r4, #4]
	mov	r1, r5
	ldr	r2, [r4, #304]
	mov	r0, r4
	ldrh	r3, [r3, #20]
	subs	r3, r3, #1
	bl	recover_logic_page_mapping
	mov	r1, r5
	movs	r2, #3
	mov	r0, r4
	mov	r5, r6
	bl	add_prio_gc
	b	.L2307
.L2320:
	mov	r0, r4
	bl	out_phy_block_from_free_list
	mov	r1, r0
	str	r0, [r4, #324]
	cbnz	r0, .L2323
.L2394:
	movs	r5, #1
	b	.L2319
.L2323:
	ldr	r3, [r4, #292]
	strh	r7, [r4, #352]	@ movhi
	ldr	r2, [r3, #8]
	adds	r2, r2, #1
	str	r2, [r0, #8]
	mov	r0, r4
	bl	new_block_init_for_write
	cbz	r0, .L2324
	mov	r0, r10
	bl	NAND_Print
	str	r7, [r4, #324]
	b	.L2318
.L2324:
	ldrh	fp, [r4, #320]
	mov	r5, r0
.L2326:
	cmp	fp, r5
	ldr	r2, [r4, #324]
	bls	.L2329
	uxth	r3, r5
	ldr	r7, [r4, #1988]
	ldr	r1, [r4, #292]
	mov	r0, r4
	str	r3, [sp]
	ldr	r3, [r4, #284]
	blx	r7
	cbz	r0, .L2327
	mov	r0, r8
	bl	NAND_Print
.L2327:
	adds	r5, r5, #1
	b	.L2326
.L2386:
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
	bls	.L2330
	ldr	r0, .L2398+52
	bl	NAND_Print
	ldrh	r2, [r4, #320]
	ldrh	r1, [r4, #352]
	ldr	r0, .L2398+56
	bl	NAND_Print
.L2393:
	movs	r5, #0
.L2319:
	bl	get_timer_masked
	subs	r1, r0, r6
	ldr	r0, .L2398+60
	bl	printf
	cmp	r5, #0
	beq	.L2335
	ldr	r0, .L2398+64
	bl	NAND_Print
	b	.L2395
.L2330:
	bcs	.L2393
	ldr	r0, .L2398+68
	b	.L2392
.L2315:
	ldr	r0, .L2398+72
	bl	NAND_Print_DBG
	ldr	r3, [r4, #172]
	cbz	r3, .L2332
	mov	r0, r4
	bl	out_phy_block_from_free_list
	str	r0, [r4, #292]
	cmp	r0, #0
	beq	.L2394
	ldr	r3, [sp, #20]
	strh	r5, [r4, #320]	@ movhi
	adds	r3, r3, #1
	str	r3, [r0, #8]
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r3, r3, #31
	bpl	.L2334
	mov	r0, r4
	bl	out_phy_block_from_free_list
	str	r0, [r4, #324]
	cmp	r0, #0
	beq	.L2394
	ldr	r3, [r4, #292]
	strh	r5, [r4, #352]	@ movhi
	ldr	r3, [r3, #8]
	adds	r3, r3, #1
	str	r3, [r0, #8]
	b	.L2319
.L2334:
	str	r5, [r4, #324]
	b	.L2319
.L2332:
	ldr	r0, .L2398+76
	bl	NAND_Print
	mov	r0, r4
	bl	print_free_list
	mov	r0, r4
	bl	print_block_invalid_list
	b	.L2394
.L2335:
	mov	r0, r4
	bl	adjust_invaild_list
	ldr	r3, [r4, #292]
	ldrh	r1, [r4, #22]
	ldr	r0, .L2398+80
	str	r3, [r4, #48]
	bl	NAND_Print_DBG
	bl	get_timer_masked
	ldr	r3, [sp, #32]
	subs	r1, r0, r3
	ldr	r0, .L2398+84
	bl	NAND_Print
	mov	r0, r5
	b	.L2282
.L2399:
	.align	2
.L2398:
	.word	.LANCHOR94
	.word	.LC371
	.word	.LC372
	.word	.LC373
	.word	.LC376
	.word	.LC363
	.word	.LC364
	.word	.LC365
	.word	.LC366
	.word	.LC367
	.word	.LC368
	.word	.LC369
	.word	.LC370
	.word	.LC377
	.word	.LC378
	.word	.LC382
	.word	.LC383
	.word	.LC379
	.word	.LC380
	.word	.LC381
	.word	.LC384
	.word	.LC385
	.word	.LC375
	.word	.LC374
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
	ldr	r5, .L2420
	sub	sp, sp, #48
	mov	r8, r1
.L2401:
	mov	r0, r4
	bl	out_phy_block_from_free_list
	mov	r7, r0
	cbnz	r0, .L2402
	ldr	r0, .L2420+4
	bl	NAND_Print
.L2400:
	mov	r0, r7
	add	sp, sp, #48
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2402:
	ldr	r2, [r8, #8]
	mov	r1, r0
	str	r2, [r0, #8]
	mov	r0, r4
	bl	new_block_init_for_write
	mov	r6, r0
	cbz	r0, .L2404
	mov	r0, r5
	bl	NAND_Print
	b	.L2401
.L2404:
	ldr	fp, .L2420+32
	mov	r5, r6
	ldrh	r1, [r8]
	ldr	r0, .L2420+8
	bl	NAND_Print
.L2405:
	ldr	r3, [r4, #4]
	ldrh	r2, [r3, #20]
	cmp	r2, r5
	bgt	.L2412
	ldrh	r2, [r8, #6]
	mov	r1, r8
	mov	r0, r4
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
	mov	r2, r6
	mov	r1, r5
	ldr	r0, .L2420+12
	bl	NAND_Print
	b	.L2400
.L2412:
	ldrh	r2, [r3, #28]
	mov	r0, r4
	ldrh	r1, [r8]
	ldr	r3, [r4, #284]
	strh	r2, [sp, #20]	@ movhi
	strh	r1, [sp, #18]	@ movhi
	add	r1, sp, #16
	str	r3, [sp, #24]
	strh	r5, [sp, #16]	@ movhi
	ldr	r3, [r4, #1980]
	str	sp, [sp, #28]
	blx	r3
	mov	r10, r0
	mov	r0, sp
	bl	is_ftl_logic_page_data
	cmp	r0, #1
	bne	.L2406
	cmp	r10, #0
	beq	.L2407
	cmp	r10, #10
	bne	.L2408
.L2407:
	mov	r2, sp
	ldr	r1, [r4, #284]
	mov	r0, r4
	bl	nand_power_off_ecc_error
	cbz	r0, .L2409
	ldr	r0, .L2420+16
	bl	NAND_Print
.L2410:
	adds	r5, r5, #1
	b	.L2405
.L2409:
	mov	r2, r10
	mov	r1, r5
	ldr	r0, .L2420+20
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
	ldr	r3, [r4, #1984]
	blx	r3
	b	.L2410
.L2408:
	ldr	r0, .L2420+24
	mov	r1, r5
.L2419:
	bl	NAND_Print
	b	.L2410
.L2406:
	ldrb	r3, [sp, #1]	@ zero_extendqisi2
	cmp	r3, #170
	bne	.L2411
	mov	r1, r5
	mov	r0, fp
	b	.L2419
.L2411:
	mov	r0, sp
	bl	is_nouse_page
	cmp	r0, #1
	beq	.L2410
	mov	r1, r5
	ldr	r0, .L2420+28
	b	.L2419
.L2421:
	.align	2
.L2420:
	.word	.LC379
	.word	.LC164
	.word	.LC386
	.word	.LC392
	.word	.LC387
	.word	.LC388
	.word	.LC389
	.word	.LC391
	.word	.LC390
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
.L2423:
	ldrh	r3, [r4, #320]
	cbz	r3, .L2426
.L2430:
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r0, r3, #31
	bpl	.L2428
	add	r5, r4, #324
.L2427:
	ldrh	r3, [r4, #352]
	cbnz	r3, .L2428
	ldr	r1, [r4, #324]
	mov	r0, r4
	ldr	r2, [r1, #8]
	bl	new_block_init_for_write
	cbnz	r0, .L2432
.L2428:
	movs	r0, #0
.L2422:
	pop	{r3, r4, r5, pc}
.L2426:
	ldr	r1, [r4, #292]
	mov	r0, r4
	ldr	r2, [r1, #8]
	bl	new_block_init_for_write
	cmp	r0, #0
	beq	.L2430
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
	beq	.L2423
	ldr	r0, .L2445
.L2444:
	bl	NAND_Print
	movs	r0, #1
	b	.L2422
.L2432:
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
	beq	.L2427
	ldr	r0, .L2445+4
	b	.L2444
.L2446:
	.align	2
.L2445:
	.word	.LC393
	.word	.LC394
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
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r4, r5, r6, r7, r8, r10, fp, lr}
	movs	r4, #0
	ldr	fp, .L2456+4
	mov	r8, r0
	mov	r10, r1
	mov	r7, r2
	mov	r6, r3
	mov	r5, r4
.L2448:
	cmp	r6, r5
	bhi	.L2453
	ldrh	r2, [r7, #4]
	cmp	r2, r4
	beq	.L2455
	mov	r3, r6
	mov	r1, r4
	ldr	r0, .L2456
	bl	NAND_Print
	movs	r0, #1
.L2447:
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2453:
	ldr	r3, [r10, #8]
	ldr	r1, [r3, r5, lsl #2]
	adds	r3, r1, #1
	beq	.L2449
	mov	r0, r8
	bl	get_logic_page_map
	mov	r3, r0
	cbnz	r0, .L2450
	str	r0, [sp, #4]
	mov	r0, fp
	bl	NAND_Print
	ldr	r3, [sp, #4]
.L2450:
	ldrh	r1, [r3, #2]
	ldrh	r2, [r7]
	cmp	r1, r2
	bne	.L2449
	ldrh	r3, [r3]
	ubfx	r3, r3, #0, #12
	cmp	r3, r5
	beq	.L2452
.L2449:
	adds	r4, r4, #1
.L2452:
	adds	r5, r5, #1
	b	.L2448
.L2455:
	movs	r0, #0
	b	.L2447
.L2457:
	.align	2
.L2456:
	.word	.LC396
	.word	.LC395
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
	bne	.L2459
	ldr	r1, [r0, #312]
	mov	r2, #328
	ldr	r0, [r0, #344]
	bl	NAND_Memcpy
.L2459:
	ldr	r3, .L2462
	mov	r1, r5
	ldr	r2, [r5, #4]
	mov	r0, r4
	str	r3, [sp]
	movs	r3, #0
	bl	write_phy_data_to_current_block
	mov	r6, r0
	cbz	r0, .L2460
	ldr	r0, .L2462+4
	bl	NAND_Print
.L2458:
	mov	r0, r6
	add	sp, sp, #8
	@ sp needed
	pop	{r4, r5, r6, pc}
.L2460:
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
	bne	.L2458
	ldr	r1, [r5]
	mov	r0, r4
	bl	put_phy_block_to_invalid_page_list
	mov	r0, r4
	bl	adjust_invaild_list
	b	.L2458
.L2463:
	.align	2
.L2462:
	.word	-1431633921
	.word	.LC397
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
	ldr	r3, .L2465
	str	r3, [r0, #1976]
	ldr	r3, .L2465+4
	str	r3, [r0, #1980]
	ldr	r3, .L2465+8
	str	r3, [r0, #1984]
	ldr	r3, .L2465+12
	str	r3, [r0, #1992]
	ldr	r3, .L2465+16
	str	r3, [r0, #1996]
	ldr	r3, .L2465+20
	str	r3, [r0, #1988]
	ldr	r3, .L2465+24
	str	r3, [r0, #2004]
	ldr	r3, .L2465+28
	str	r3, [r0, #2000]
	ldr	r3, .L2465+32
	str	r3, [r0, #2008]
	bx	lr
.L2466:
	.align	2
.L2465:
	.word	_nftl_nand_erase_superblk
	.word	_nftl_nand_read_page
	.word	_nftl_nand_write_page
	.word	_nftl_nand_is_blk_good
	.word	_nftl_nand_mark_bad_blk
	.word	_nftl_nand_copy_page
	.word	_nftl_nand_read_logic_page
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
	bne	.L2476
	mov	r0, r4
	bl	write_phy_page_map_to_current_block
	cbz	r0, .L2470
	ldr	r0, .L2487
.L2486:
	bl	NAND_Print
.L2468:
	mov	r0, r5
	pop	{r4, r5, r6, pc}
.L2470:
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r2, r3, #31
	bpl	.L2472
	add	r1, r4, #324
	mov	r0, r4
	bl	write_phy_page_map_to_current_block
	cbz	r0, .L2472
	ldr	r0, .L2487
	bl	NAND_Print
.L2472:
	mov	r1, r6
	mov	r0, r4
	bl	get_new_current_block
	cbz	r0, .L2474
	ldr	r0, .L2487+4
	b	.L2486
.L2474:
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r3, r3, #31
	bpl	.L2475
	add	r1, r4, #324
	mov	r0, r4
	bl	get_new_current_block
	cbz	r0, .L2475
	ldr	r0, .L2487+8
	b	.L2486
.L2475:
	ldr	r3, [r4, #368]
	adds	r3, r3, #1
	str	r3, [r4, #368]
.L2476:
	movs	r5, #0
	b	.L2468
.L2488:
	.align	2
.L2487:
	.word	.LC398
	.word	.LC399
	.word	.LC400
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
	bne	.L2490
	mov	r0, r5
	bl	do_after_write
	cbz	r0, .L2490
	ldr	r0, .L2501
	bl	NAND_Print
.L2489:
	mov	r0, r4
	pop	{r4, r5, r6, r7, r8, pc}
.L2490:
	mov	r0, r5
	bl	do_before_write
	cbz	r0, .L2492
	ldr	r0, .L2501+4
.L2500:
	bl	NAND_Print
	movs	r4, #1
	b	.L2489
.L2492:
	mov	r3, r8
	mov	r2, r7
	mov	r1, r6
	mov	r0, r5
	bl	write_data_to_page
	cbz	r0, .L2493
	ldr	r0, .L2501+8
	b	.L2500
.L2493:
	mov	r0, r5
	bl	do_after_write
	mov	r4, r0
	cmp	r0, #0
	beq	.L2489
	ldr	r0, .L2501+12
	b	.L2500
.L2502:
	.align	2
.L2501:
	.word	.LC401
	.word	.LC402
	.word	.LC403
	.word	.LC404
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
	sub	sp, sp, #48
	mov	r5, r1
	add	r6, sp, #16
	mov	r4, r0
	movs	r1, #90
	str	r2, [sp, #12]
	mov	r0, r6
	movs	r2, #16
	bl	NAND_Memset
	cbnz	r5, .L2505
	mov	r0, r4
	bl	out_phy_block_from_invalid_page_list
.L2544:
	mov	r5, r0
	cbnz	r0, .L2508
	ldr	r0, .L2545
	bl	NAND_Print
	movs	r0, #8
.L2504:
	add	sp, sp, #48
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2505:
	mov	r1, r5
	mov	r0, r4
	bl	is_current_blcok
	cmp	r0, #1
	beq	.L2525
	mov	r0, r4
	bl	out_phy_block_from_invalid_page_list_by_block
	b	.L2544
.L2508:
	ldr	r2, [r4, #1968]
	ldrh	r0, [r0]
	ldr	r3, [r2, #124]
	strh	r0, [sp, #34]	@ movhi
	mov	r0, r4
	adds	r3, r3, #1
	str	r3, [r2, #124]
	ldr	r3, [r4, #4]
	ldr	r2, [r4, #284]
	ldrh	r1, [r3, #28]
	ldrh	r3, [r3, #20]
	strd	r2, r6, [sp, #40]
	strh	r1, [sp, #36]	@ movhi
	add	r1, sp, #32
	subs	r3, r3, #1
	strh	r3, [sp, #32]	@ movhi
	ldr	r3, [r4, #1980]
	blx	r3
	mov	r8, r0
	mov	r0, r6
	bl	is_phy_mapping_page
	cmp	r0, #1
	mov	r7, r0
	beq	.L2509
	ldr	r0, .L2545+4
	movs	r7, #1
	bl	NAND_Print
	ldr	r3, [r4, #4]
	ldrh	r1, [r5]
	ldr	r0, .L2545+8
	ldrh	r2, [r3, #20]
	ldrh	r3, [r3, #28]
	subs	r2, r2, #1
	str	r3, [sp]
	movs	r3, #0
	bl	NAND_Print
	ldrb	r0, [sp, #20]	@ zero_extendqisi2
	ldrb	r3, [sp, #19]	@ zero_extendqisi2
	ldrb	r2, [sp, #18]	@ zero_extendqisi2
	ldrb	r1, [sp, #17]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L2545+12
	bl	NAND_Print
	ldrb	r0, [sp, #25]	@ zero_extendqisi2
	ldrb	r3, [sp, #23]	@ zero_extendqisi2
	ldrb	r2, [sp, #22]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [sp, #24]	@ zero_extendqisi2
	ldrb	r1, [sp, #21]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L2545+16
	bl	NAND_Print
	ldrh	r2, [r5]
	ldrb	r1, [r4, #16]	@ zero_extendqisi2
	ldr	r0, .L2545+20
	bl	NAND_Print
	ldr	r2, [r4, #268]
	mov	r1, r5
	mov	r0, r4
	bl	recover_block_phy_page_mapping
.L2510:
	ldr	r3, [r4, #4]
	mov	r2, r5
	add	r1, r4, #260
	mov	r0, r4
	ldrh	r3, [r3, #20]
	bl	check_invalid_page_right
	cbz	r0, .L2512
	movs	r7, #1
	movw	r6, #65535
	ldr	r0, .L2545+24
	bl	NAND_Print
.L2513:
	mov	r8, #0
	b	.L2519
.L2509:
	cmp	r8, #0
	beq	.L2511
	cmp	r8, #10
	beq	.L2511
	ldrh	r1, [r5]
	ldr	r0, .L2545+28
	bl	NAND_Print
	ldr	r2, [r4, #268]
	mov	r1, r5
	mov	r0, r4
	bl	recover_block_phy_page_mapping
	b	.L2510
.L2511:
	ldr	r2, [r4, #272]
	movs	r7, #0
	ldr	r1, [r4, #284]
	ldr	r0, [r4, #268]
	bl	NAND_Memcpy
	b	.L2510
.L2512:
	ldr	r3, [sp, #12]
	movw	r6, #65535
	cmp	r7, #0
	it	eq
	moveq	r6, r3
	b	.L2513
.L2520:
	ldr	r3, [r4, #268]
	ldr	r10, [r3, r8, lsl #2]
	cmp	r10, #-1
	beq	.L2515
	mov	r1, r10
	mov	r0, r4
	bl	get_logic_page_map
	ldrh	r2, [r5]
	mov	fp, r0
	ldrh	r3, [r0, #2]
	cmp	r2, r3
	bne	.L2515
	ldrh	r3, [r0]
	ubfx	r3, r3, #0, #12
	cmp	r3, r8
	str	r3, [sp, #8]
	bne	.L2515
	ldr	r1, [r4, #1968]
	ldr	r0, [r4, #4]
	strh	r2, [sp, #34]	@ movhi
	add	r2, sp, #16
	ldr	r3, [r1, #128]
	str	r2, [sp, #44]
	strh	r8, [sp, #32]	@ movhi
	adds	r3, r3, #1
	str	r3, [r1, #128]
	ldrh	r1, [r0, #28]
	mov	r0, r4
	ldr	r3, [r4, #288]
	strh	r1, [sp, #36]	@ movhi
	add	r1, sp, #32
	str	r3, [sp, #40]
	ldr	r3, [r4, #1980]
	blx	r3
	add	r2, sp, #16
	mov	r0, r2
	bl	get_logic_page_from_oob
	cmp	r10, r0
	mov	r3, r0
	beq	.L2516
	ldrh	r1, [r5]
	mov	r2, r8
	str	r10, [sp]
	movs	r7, #1
	ldr	r0, .L2545+32
	movw	r6, #65535
	bl	NAND_Print
	ldrb	r0, [sp, #20]	@ zero_extendqisi2
	ldrb	r3, [sp, #18]	@ zero_extendqisi2
	ldrb	r2, [sp, #17]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [sp, #19]	@ zero_extendqisi2
	ldrb	r1, [sp, #16]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L2545+36
	bl	NAND_Print
	ldrb	r0, [sp, #25]	@ zero_extendqisi2
	ldrb	r3, [sp, #23]	@ zero_extendqisi2
	ldrb	r2, [sp, #22]	@ zero_extendqisi2
	str	r0, [sp, #4]
	ldrb	r0, [sp, #24]	@ zero_extendqisi2
	ldrb	r1, [sp, #21]	@ zero_extendqisi2
	str	r0, [sp]
	ldr	r0, .L2545+40
	bl	NAND_Print
	ldrh	r3, [fp]
	movw	r2, #4095
	strh	r6, [fp, #2]	@ movhi
	bfi	r3, r2, #0, #12
	strh	r3, [fp]	@ movhi
.L2517:
	subs	r6, r6, #1
	uxth	r6, r6
	cbz	r6, .L2518
.L2515:
	add	r8, r8, #1
.L2519:
	ldr	r3, [r4, #4]
	ldrh	r3, [r3, #20]
	subs	r3, r3, #1
	cmp	r3, r8
	bhi	.L2520
	str	r8, [sp, #8]
.L2518:
	ldr	r3, [r4, #4]
	ldr	r1, [sp, #8]
	ldrh	r3, [r3, #20]
	subs	r2, r3, #1
	cmp	r2, r1
	ldr	r1, [sp, #12]
	itt	eq
	moveq	r2, #0
	strheq	r2, [r4, #386]	@ movhi
	ldrh	r2, [r5, #4]
	cbz	r1, .L2522
	cbz	r7, .L2523
	cmp	r3, r2
	bne	.L2523
.L2524:
	ldr	r0, .L2545+44
	bl	NAND_Print
	ldrh	r2, [r5, #6]
	mov	r1, r5
	mov	r0, r4
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
.L2525:
	movs	r0, #0
	b	.L2504
.L2516:
	ldr	r2, [r4, #288]
	mov	r1, r10
	mov	r0, r4
	bl	nand_write_logic_page_no_gc
	cmp	r0, #0
	beq	.L2517
	ldr	r0, .L2545+48
	bl	NAND_Print
	movs	r0, #1
	b	.L2504
.L2522:
	cmp	r7, #0
	bne	.L2524
.L2523:
	mov	r1, r5
	mov	r0, r4
	bl	put_phy_block_to_invalid_page_list
	mov	r0, r4
	bl	adjust_invaild_list
	b	.L2525
.L2546:
	.align	2
.L2545:
	.word	.LC405
	.word	.LC406
	.word	.LC407
	.word	.LC408
	.word	.LC409
	.word	.LC410
	.word	.LC412
	.word	.LC411
	.word	.LC413
	.word	.LC414
	.word	.LC415
	.word	.LC417
	.word	.LC416
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
	beq	.L2560
	add	r4, r0, #392
	mov	r0, r4
	bl	del_from_gc_list
	ldr	r7, [r0, #4]
	mov	r5, r0
	mov	r0, r6
	mov	r1, r7
	bl	is_current_blcok
	cmp	r0, #1
	bne	.L2549
	mov	r1, r5
	mov	r0, r4
	bl	add_to_gc_list_tail
.L2561:
	movs	r4, #0
.L2547:
	mov	r0, r4
	pop	{r3, r4, r5, r6, r7, pc}
.L2549:
	cbnz	r7, .L2550
	ldr	r0, .L2564
	bl	NAND_Print
	strh	r7, [r5, #2]	@ movhi
	strh	r7, [r6, #394]	@ movhi
	str	r7, [r5, #4]
	b	.L2561
.L2550:
	mov	r0, r6
	bl	is_block_in_invalid_list
	ldrh	r2, [r5, #2]
	mov	r4, r0
	cbnz	r0, .L2551
	ldrh	r1, [r7]
	ldr	r0, .L2564+4
.L2562:
	bl	NAND_Print
	b	.L2563
.L2551:
	cmp	r2, #5
	strh	r2, [r6, #394]	@ movhi
	bne	.L2552
	movs	r4, #0
	movs	r2, #4
	mov	r0, r6
	bl	garbage_collect_first
.L2563:
	strh	r4, [r5, #2]	@ movhi
	strh	r4, [r6, #394]	@ movhi
	str	r4, [r5, #4]
	b	.L2547
.L2552:
	movw	r2, #65535
	mov	r0, r6
	bl	garbage_collect_first
	ldr	r3, [r5, #4]
	mov	r4, r0
	cbz	r0, .L2553
	ldrh	r1, [r3]
	movs	r4, #1
	ldrh	r2, [r5, #2]
	ldr	r0, .L2564+8
	bl	NAND_Print
	movs	r3, #0
	strh	r3, [r5, #2]	@ movhi
	strh	r3, [r6, #394]	@ movhi
	str	r3, [r5, #4]
	b	.L2547
.L2553:
	mov	r1, r3
	mov	r0, r6
	bl	is_block_in_free_list
	cbnz	r0, .L2554
	ldrh	r2, [r5, #2]
	ldrh	r1, [r3]
	ldr	r0, .L2564+12
	b	.L2562
.L2554:
	mov	r0, r6
	bl	out_phy_block_from_free_list_by_block
	mov	r1, r0
	cbnz	r0, .L2555
	ldr	r3, [r5, #4]
	ldrh	r2, [r5, #2]
	ldr	r0, .L2564+16
	ldrh	r1, [r3]
	b	.L2562
.L2555:
	ldrh	r2, [r0, #6]
	mov	r0, r6
	adds	r2, r2, #1
	uxth	r2, r2
	bl	erase_block
	movs	r3, #0
	strh	r3, [r5, #2]	@ movhi
	strh	r3, [r6, #394]	@ movhi
	str	r3, [r5, #4]
	b	.L2547
.L2560:
	mov	r4, r1
	b	.L2547
.L2565:
	.align	2
.L2564:
	.word	.LC418
	.word	.LC419
	.word	.LC420
	.word	.LC421
	.word	.LC422
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
.L2567:
	mov	r0, r6
	bl	do_prio_gc
	subs	r5, r5, #1
	orr	r4, r4, r0
	bne	.L2567
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
	cbnz	r0, .L2570
	movs	r6, #1
	ldr	r0, .L2574
	bl	NAND_Print_DBG
.L2569:
	mov	r0, r6
	add	sp, sp, #36
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L2570:
	ldrh	r2, [r0, #2]
	movw	r1, #65535
	ldr	r3, [r4, #4]
	cmp	r2, r1
	bne	.L2572
	mov	r0, r6
	ldrh	r2, [r3, #26]
	movs	r1, #255
	movs	r6, #0
	bl	NAND_Memset
	b	.L2569
.L2572:
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
	ldr	r3, [r4, #1980]
	blx	r3
	adds	r3, r0, #2
	mov	r6, r0
	bne	.L2573
	ldr	r3, [r4, #4]
	movs	r0, #36
	ldrh	r1, [r5, #2]
	movs	r2, #1
	ldr	r3, [r3, #32]
	mla	r1, r0, r1, r3
	mov	r0, r4
	bl	add_prio_gc
	mov	r1, r7
	ldr	r0, .L2574+4
	bl	NAND_Print
.L2573:
	mov	r0, r4
	bl	do_prio_gc
	b	.L2569
.L2575:
	.align	2
.L2574:
	.word	.LC395
	.word	.LC423
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
	cbnz	r3, .L2577
.L2579:
	movs	r4, #8
.L2576:
	mov	r0, r4
	pop	{r4, pc}
.L2577:
	ldrh	r3, [r3, #4]
	cmp	r3, r1
	bls	.L2579
	movw	r2, #65535
	movs	r1, #0
	bl	garbage_collect_first
	cmp	r0, #8
	mov	r4, r0
	beq	.L2579
	cbnz	r0, .L2580
	ldr	r0, .L2584
	bl	NAND_Print
	b	.L2576
.L2580:
	ldr	r0, .L2584+4
	movs	r4, #1
	bl	NAND_Print
	b	.L2576
.L2585:
	.align	2
.L2584:
	.word	.LC424
	.word	.LC425
	.size	gc_one, .-gc_one
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
.L2587:
	mov	r1, r5
	mov	r0, r4
	bl	gc_one
	cmp	r0, #8
	bne	.L2587
	ldr	r0, .L2589
	bl	NAND_Print
	movs	r0, #0
	pop	{r3, r4, r5, pc}
.L2590:
	.align	2
.L2589:
	.word	.LC426
	.size	gc_all, .-gc_all
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
	mov	r6, r0
	ldr	r3, [r0, #4]
	movs	r4, #0
	ldr	r7, .L2602
	ldrh	r5, [r3, #20]
	muls	r5, r1, r5
.L2592:
	cmp	r4, r5
	blt	.L2595
.L2593:
	movs	r0, #0
	pop	{r3, r4, r5, r6, r7, pc}
.L2595:
	ldr	r3, [r6, #200]
	cmp	r3, #0
	beq	.L2593
	ldrh	r3, [r3, #4]
	movs	r1, #1
	mov	r0, r6
	add	r4, r4, r3
	subs	r4, r4, #1
	bl	gc_one
	cmp	r0, #0
	beq	.L2592
	mov	r0, r7
	bl	NAND_Print
	b	.L2592
.L2603:
	.align	2
.L2602:
	.word	.LC427
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
	push	{r3, r4, r5, r6, r7, lr}
	mov	r4, r0
	ldrh	r1, [r0, #22]
	ldrh	r3, [r0, #380]
	cmp	r3, r1
	bhi	.L2605
	ldr	r0, .L2616
.L2615:
	bl	NAND_Print
	movs	r0, #0
	pop	{r3, r4, r5, r6, r7, pc}
.L2605:
	ldr	r3, [r0, #4]
	movs	r7, #0
	ldrh	r6, [r3, #20]
	subs	r6, r6, #10
	uxth	r6, r6
.L2607:
	ldrh	r2, [r4, #22]
	uxth	r5, r7
	ldrh	r3, [r4, #380]
	cmp	r2, r3
	bcs	.L2610
	mov	r1, r6
	mov	r0, r4
	bl	gc_one
	cmp	r0, #8
	add	r7, r7, #1
	bne	.L2607
.L2610:
	ldr	r3, [r4, #4]
	ldr	r7, .L2616+4
	ldrh	r6, [r3, #20]
	lsrs	r6, r6, #3
.L2608:
	ldrh	r1, [r4, #22]
	cmp	r1, #4
	bhi	.L2613
	mov	r0, r7
	bl	NAND_Print
	mov	r1, r6
	mov	r0, r4
	bl	gc_one
	cmp	r0, #8
	bne	.L2611
.L2613:
	mov	r1, r5
	ldr	r0, .L2616+8
	b	.L2615
.L2611:
	adds	r5, r5, #1
	uxth	r5, r5
	b	.L2608
.L2617:
	.align	2
.L2616:
	.word	.LC428
	.word	.LC429
	.word	.LC430
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
	ldr	r0, .L2623
	bl	NAND_Print_DBG
	mov	r0, r4
	bl	first_scan_all_blocks
	ldrh	r1, [r4, #18]
	mov	r0, r4
	bl	init_zone_after_first_scan
	cbz	r0, .L2619
	ldr	r0, .L2623+4
.L2622:
	movs	r5, #1
	bl	NAND_Print
.L2618:
	mov	r0, r5
	pop	{r3, r4, r5, pc}
.L2619:
	mov	r0, r4
	bl	second_scan_all_blocks
	mov	r5, r0
	cbz	r0, .L2621
	ldr	r0, .L2623+8
	b	.L2622
.L2621:
	mov	r0, r4
	bl	power_on_gc
	mov	r0, r4
	bl	print_nftl_zone
	ldr	r0, .L2623+12
	bl	NAND_Print
	b	.L2618
.L2624:
	.align	2
.L2623:
	.word	.LC431
	.word	.LC432
	.word	.LC433
	.word	.LC434
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
	ldr	r0, .L2626
	bl	NAND_Print
	ldr	r5, .L2626+4
	mov	r0, r4
	bl	build_zone_list
	ldr	r0, .L2626+8
	bl	NAND_Print
	mov	r0, r4
	bl	nftl_cache_init
	mov	r0, r5
	bl	NAND_Print
	mov	r0, r4
	bl	print_block_invalid_list
	mov	r0, r4
	bl	print_free_list
	mov	r0, r5
	bl	NAND_Print
	ldr	r0, .L2626+12
	pop	{r3, r4, r5, lr}
	b	NAND_Print
.L2627:
	.align	2
.L2626:
	.word	.LC435
	.word	.LC437
	.word	.LC436
	.word	.LC438
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
	ldr	r4, .L2629
	mov	r5, r0
	bl	__nand_flush_write_cache
	mov	r0, r4
	bl	NAND_Print
	mov	r0, r5
	bl	print_nftl_zone
	mov	r0, r4
	bl	NAND_Print
	mov	r0, r5
	bl	nand_clean_zone_table
	mov	r0, r4
	bl	NAND_Print
	mov	r0, r5
	bl	nand_find_zone_table
	mov	r0, r5
	bl	write_data_to_nand
	mov	r0, r4
	bl	NAND_Print
	movs	r0, #0
	pop	{r3, r4, r5, pc}
.L2630:
	.align	2
.L2629:
	.word	.LC439
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
	ldr	r3, [sp, #92]
	mov	r6, r2
	ldr	r2, [sp, #76]
	ldr	r7, [sp, #88]
	lsr	r10, r3, #9
	ldr	r3, [sp, #72]
	strd	r0, r1, [sp, #28]
	movw	r0, #2012
	lsrs	r7, r7, #9
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
	str	r0, [r6]
	cmp	r0, #0
	beq	.L2632
	lsl	r3, r10, #9
	uxth	r3, r3
	str	r3, [sp, #16]
	movs	r3, #2
.L2662:
.L2633:
	movw	r2, #2012
	movs	r1, #0
	mov	r0, r4
	str	r3, [sp, #4]
	bl	NAND_Memset
	ldr	r3, [sp, #28]
	movs	r0, #36
	str	r3, [r4]
	ldr	r3, [sp, #32]
	str	r3, [r4, #1972]
	bl	NAND_Malloc
	str	r0, [r4, #4]
	cmp	r0, #0
	beq	.L2632
	ldr	r3, .L2664
	movs	r2, #8
	ldr	r3, [r3]
	ldr	r1, [r3, #8]
	add	r1, r1, r2
	bl	memcpy
	ldr	r5, [r4, #4]
	mov	r1, r7
	ldrh	r3, [sp, #16]
	ldr	r0, [sp, #24]
	strh	r3, [r5, #26]	@ movhi
	ldr	r3, [sp, #96]
	strb	r3, [r5, #10]
	bl	__aeabi_uidiv
	ldr	r2, [r4, #1972]
	mov	r8, r0
	ldrh	r2, [r2, #12]
	cmp	r2, r0
	bcs	.L2643
	uxth	r3, r0
	mov	r1, r10
	mov	r0, r7
	str	r3, [sp, #8]
	mov	fp, #36
	ldrh	r3, [sp, #8]
	strh	r3, [r5, #24]	@ movhi
	bl	__aeabi_uidiv
	uxtb	r2, r10
	add	r3, r8, #1
	strh	r0, [r5, #20]	@ movhi
	strb	r2, [r5, #8]
	strh	r2, [r5, #28]	@ movhi
	movw	r2, #3000
	strh	r2, [r5, #16]	@ movhi
	mul	r2, fp, r3
	str	r3, [sp, #12]
	mov	r0, r2
	str	r2, [sp, #36]
	bl	nftl_malloc
	ldr	r3, [r4, #4]
	str	r0, [r5, #32]
	ldr	r0, [r3, #32]
	cbz	r0, .L2632
	ldr	r2, [sp, #36]
	movs	r1, #255
	subs	r2, r2, #36
	bl	NAND_Memset
	movs	r1, #0
	mov	ip, #-1
	mov	r2, r1
.L2635:
	ldr	r3, [sp, #12]
	cmp	r3, r1
	bhi	.L2636
	ldr	r0, [r6]
	bl	nftl_ops_init
	mov	r1, r7
	ldr	r0, [sp, #20]
	bl	__aeabi_uidiv
	ldr	r3, [sp, #8]
	subs	r0, r3, r0
	ldr	r3, [r4, #1972]
	uxth	r2, r0
	ldrh	r3, [r3, #12]
	strh	r2, [r4, #22]	@ movhi
	cmp	r3, r2
	bls	.L2637
	mov	r1, r2
	ldr	r0, .L2664+4
	bl	NAND_Print
	b	.L2663
.L2632:
	ldr	r0, .L2664+8
	bl	NAND_Print
.L2663:
	mvn	r5, #1
.L2631:
	mov	r0, r5
	add	sp, sp, #40
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r10, fp, pc}
.L2636:
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
	b	.L2635
.L2637:
	mov	r1, r8
	ldr	r0, .L2664+12
	bl	NAND_Print_DBG
	ldrh	r3, [r4, #22]
	movs	r1, #0
	ldr	r0, [r6]
	sub	r2, r8, r3
	muls	r2, r7, r2
	muls	r3, r7, r3
	bl	zone_param_init
	mov	r5, r0
	cbz	r0, .L2638
	ldr	r0, .L2664+16
	bl	NAND_Print
	b	.L2631
.L2638:
	ldr	r3, [sp, #104]
	ldr	r2, [r4, #1972]
	strb	r3, [r4, #16]
	ldrh	r3, [r2, #26]
	cbz	r3, .L2639
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	orr	r3, r3, #1
	strb	r3, [r4, #17]
.L2639:
	ldrh	r3, [r2, #10]
	cbz	r3, .L2640
	mov	r1, r8
	ldr	r0, [r6]
	bl	is_no_use_device
	cmp	r0, #1
	bne	.L2640
	mov	r1, r8
	ldr	r0, [r6]
	bl	build_zone_list_first
	mov	r8, r0
	cbz	r0, .L2640
	ldr	r0, .L2664+20
	bl	NAND_Print
.L2642:
	mov	r5, r8
	b	.L2631
.L2640:
	ldr	r0, [r6]
	bl	build_zone_list
	mov	r8, r0
	cbz	r0, .L2641
	ldr	r0, .L2664+24
	bl	NAND_Print
	ldr	r3, [sp, #4]
	cmp	r3, #1
	beq	.L2642
	ldr	r0, .L2664+28
	bl	NAND_Print
	movs	r3, #1
	b	.L2662
.L2641:
	ldr	r0, [r6]
	bl	nftl_cache_init
	b	.L2631
.L2643:
	mov	r5, #-1
	b	.L2631
.L2665:
	.align	2
.L2664:
	.word	g_nsi
	.word	.LC440
	.word	.LC161
	.word	.LC441
	.word	.LC442
	.word	.LC443
	.word	.LC444
	.word	.LC445
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
	ldr	r1, .L2687
	add	r0, sp, #44
	bl	memcpy
	add	r0, sp, #56
	movs	r3, #47
	strh	r3, [r0, #-16]!	@ movhi
	ldr	r1, .L2687+4
	bl	nand_fdt_get_string
	cbz	r0, .L2679
	movs	r2, #9
	add	r1, sp, #44
	bl	NAND_Memcmp
	mov	r7, r0
	cbnz	r0, .L2679
	ldr	r3, .L2687+8
	ldr	r2, .L2687+12
	ldr	r1, .L2687+16
	str	r3, [sp, #4]
	ldr	r3, .L2687+20
	ldr	r0, .L2687+24
	str	r3, [sp]
	ldr	r3, .L2687+28
	bl	NAND_Print
	ldrh	r3, [r5, #34]
	cmp	r3, #15
	bhi	.L2683
.L2679:
	mov	r6, #-1
	b	.L2666
.L2683:
	movs	r0, #32
	bl	NAND_Malloc
	mov	r1, r0
	str	r0, [r4, #16]
	cbnz	r0, .L2670
	movs	r6, #1
	ldr	r0, .L2687+32
	bl	NAND_Print
.L2666:
	mov	r0, r6
	add	sp, sp, #60
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L2670:
	str	r7, [r0, #8]
	mov	r2, #65536
	ldr	r7, [r5, #80]
	mov	r3, #65537
	strd	r2, r3, [r0]
	ldr	r2, [r7, #36]
	ldrh	r3, [r2, #2478]
	strh	r3, [r0, #12]	@ movhi
	ldrh	r3, [r2, #2480]
	strh	r3, [r0, #16]	@ movhi
	movs	r3, #8
	strh	r3, [r0, #14]	@ movhi
	movs	r3, #2
	strh	r3, [r0, #18]	@ movhi
	movs	r0, #3
	ldr	r3, .L2687+36
	strh	r0, [r1, #20]	@ movhi
	str	r3, [r1, #28]
	ldrh	r3, [r2, #2474]
	cmp	r3, #0
	bne	.L2671
.L2684:
	movs	r3, #30
	strh	r0, [r1, #24]	@ movhi
	strh	r3, [r1, #22]	@ movhi
.L2672:
	ldrh	r3, [r7, #2]
	strh	r3, [r1, #26]	@ movhi
	cmp	r3, #0
	beq	.L2675
	movs	r3, #3
.L2686:
	strh	r3, [r1, #24]	@ movhi
	ldrh	r3, [r1, #22]
	ldr	r2, [r2, #2492]
	ldr	r0, .L2687+40
	cmp	r3, #30
	itt	hi
	movhi	r3, #30
	strhhi	r3, [r1, #22]	@ movhi
	movs	r3, #1
	strh	r3, [r1, #24]	@ movhi
	sub	r3, r2, #86016
	sub	r3, r3, #384
	cmp	r3, r0
	mov	r3, #0
	it	ls
	strls	r2, [r1, #28]
	mov	r0, r4
	strd	r6, r3, [sp, #32]
	str	r5, [sp, #28]
	ldrh	r3, [r5, #34]
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
	bne	.L2666
	ldr	r3, [r5, #80]
	ldr	r0, .L2687+44
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
	ldr	r3, [r3, #24]
	str	r6, [r4, #8]
	str	r3, [r4]
	ldr	r3, .L2687+48
	str	r3, [r4, #40]
	ldr	r3, .L2687+52
	str	r3, [r4, #44]
	ldr	r3, .L2687+56
	str	r3, [r4, #52]
	ldr	r3, .L2687+60
	str	r3, [r4, #48]
	ldr	r3, .L2687+64
	str	r3, [r4, #56]
	ldr	r3, .L2687+68
	str	r3, [r4, #60]
	ldr	r3, .L2687+72
	str	r3, [r4, #64]
	ldr	r3, .L2687+76
	str	r3, [r4, #68]
	b	.L2666
.L2671:
	cmp	r3, #255
	bne	.L2673
	movs	r3, #1
	strh	r3, [r1]	@ movhi
	movs	r3, #50
	strh	r3, [r1, #22]	@ movhi
	movs	r3, #40
.L2685:
	strh	r3, [r1, #24]	@ movhi
	b	.L2672
.L2673:
	cmp	r3, #10
	bls	.L2674
	movs	r3, #10
	strh	r3, [r2, #2474]	@ movhi
	b	.L2684
.L2674:
	add	r3, r3, r3, lsl #2
	add	r0, r3, r3, lsl #2
	lsls	r3, r3, #3
	lsls	r0, r0, #1
	strh	r0, [r1, #22]	@ movhi
	b	.L2685
.L2675:
	movs	r3, #1
	b	.L2686
.L2688:
	.align	2
.L2687:
	.word	.LANCHOR18+30
	.word	.LANCHOR95
	.word	.LC450
	.word	.LC447
	.word	.LC448
	.word	.LC451
	.word	.LC449
	.word	.LC446
	.word	.LC161
	.word	1296000
	.word	7692600
	.word	.LC452
	.word	_nand_read
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
	movs	r0, #72
	bl	NAND_Malloc
	mov	r4, r0
	cbnz	r0, .L2690
	ldr	r0, .L2693
	bl	NAND_Print
.L2689:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L2690:
	mov	r0, r5
	bl	build_nand_partition
	ldrh	r1, [r5]
	str	r0, [r4, #4]
	mov	r0, r4
	bl	nftl_initialize
	cbz	r0, .L2692
	ldr	r0, .L2693+4
	movs	r4, #0
	bl	NAND_Print
	b	.L2689
.L2692:
	mov	r1, r4
	ldr	r0, .L2693+8
	bl	add_nftl_blk_list
	ldr	r0, .L2693+12
	bl	NAND_Print_DBG
	b	.L2689
.L2694:
	.align	2
.L2693:
	.word	.LC161
	.word	.LC453
	.word	.LANCHOR0
	.word	.LC454
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
	ldr	r3, .L2700
	str	r2, [r3, #8]
.L2696:
	mov	r0, r4
	cbz	r4, .L2695
	bl	nftl_add
	cbnz	r0, .L2697
	ldr	r0, .L2700+4
	bl	NAND_Print
	movs	r0, #1
.L2695:
	pop	{r4, pc}
.L2697:
	ldr	r4, [r4, #48]
	b	.L2696
.L2701:
	.align	2
.L2700:
	.word	.LANCHOR0
	.word	.LC455
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
	cbnz	r1, .L2704
	ldr	r3, .L2708
	str	r1, [r3, #8]
.L2704:
	cbnz	r4, .L2707
	mov	r0, r4
	b	.L2702
.L2707:
	ldrh	r3, [r4]
	cmp	r3, r5
	bne	.L2705
	mov	r0, r4
	bl	nftl_add
	cbnz	r0, .L2705
	ldr	r0, .L2708+4
	bl	NAND_Print
	movs	r0, #1
.L2702:
	pop	{r3, r4, r5, pc}
.L2705:
	ldr	r4, [r4, #48]
	b	.L2704
.L2709:
	.align	2
.L2708:
	.word	.LANCHOR0
	.word	.LC455
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
	bcs	.L2711
	ldrh	r2, [r0, #384]
	cbz	r2, .L2711
	ldr	r3, [r0, #4]
	ldrh	r1, [r3, #20]
	subs	r1, r1, #3
	uxth	r1, r1
	bl	gc_one
.L2711:
	movs	r0, #0
	pop	{r3, pc}
	.size	all_time_gc, .-all_time_gc
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
	cbz	r0, .L2717
.L2734:
	movs	r0, #0
	b	.L2715
.L2717:
	ldrh	r3, [r4, #382]
	cmp	r3, r1
	bls	.L2719
	ldrh	r3, [r4, #386]
	cbnz	r3, .L2721
	ldr	r3, [r4, #200]
	cbnz	r3, .L2722
	ldr	r0, .L2736
	bl	NAND_Print
.L2735:
	movs	r0, #1
.L2715:
	pop	{r4, pc}
.L2722:
	ldrh	r2, [r3, #4]
	ldr	r3, [r4, #4]
	ldrh	r3, [r3, #20]
	cmp	r2, r3, lsr #1
	bcc	.L2723
	movs	r3, #4
.L2733:
	strh	r3, [r4, #388]	@ movhi
	ldr	r3, [r4, #1972]
	ldrh	r2, [r4, #22]
	ldrh	r3, [r3, #16]
	cmp	r2, r3
	itt	ls
	movwls	r3, #65535
	strhls	r3, [r4, #388]	@ movhi
.L2721:
	ldrh	r2, [r4, #388]
	movw	r3, #65535
	cmp	r2, r3
	beq	.L2728
	movs	r1, #0
	mov	r0, r4
	bl	garbage_collect_first
	cmp	r0, #0
	beq	.L2734
	ldrh	r1, [r4, #22]
	ldr	r0, .L2736+4
	bl	NAND_Print
	b	.L2735
.L2723:
	cmp	r2, r3, lsr #2
	bcc	.L2725
	movs	r3, #8
	b	.L2733
.L2725:
	cmp	r2, r3, lsr #3
	bcc	.L2726
	movs	r3, #16
	b	.L2733
.L2726:
	ldr	r0, .L2736+8
	bl	NAND_Print
	movw	r3, #65535
	b	.L2733
.L2728:
	movs	r1, #1
	mov	r0, r4
	bl	gc_win_block
	b	.L2734
.L2719:
	movs	r3, #1
	strh	r0, [r4, #388]	@ movhi
	str	r3, [r4, #384]
	b	.L2715
.L2737:
	.align	2
.L2736:
	.word	.LC456
	.word	.LC458
	.word	.LC457
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
	push	{r3, r4, r5, lr}
	mov	r5, r0
	bl	nand_write_logic_page_no_gc
	mov	r4, r0
	cbz	r0, .L2739
	movs	r4, #1
	ldr	r0, .L2741
	bl	NAND_Print
.L2738:
	mov	r0, r4
	pop	{r3, r4, r5, pc}
.L2739:
	mov	r0, r5
	bl	garbage_collect
	mov	r0, r5
	bl	all_time_gc
	mov	r0, r5
	bl	do_prio_gc
	b	.L2738
.L2742:
	.align	2
.L2741:
	.word	.LC459
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
	cbz	r0, .L2745
	mov	r0, r4
	bl	write_phy_page_map_to_current_block
	cbz	r0, .L2746
	ldr	r0, .L2774
.L2772:
	bl	NAND_Print
	movs	r0, #1
	b	.L2744
.L2746:
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r1, r3, #31
	bpl	.L2749
	add	r1, r4, #324
	mov	r0, r4
	bl	write_phy_page_map_to_current_block
	cbz	r0, .L2749
	ldr	r0, .L2774
	bl	NAND_Print
.L2749:
	mov	r1, r5
	mov	r0, r4
	bl	get_new_current_block
	cbz	r0, .L2751
	ldr	r0, .L2774+4
	b	.L2772
.L2751:
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r2, r3, #31
	bmi	.L2752
.L2753:
	movs	r0, #0
.L2744:
	pop	{r3, r4, r5, pc}
.L2752:
	add	r1, r4, #324
	mov	r0, r4
	bl	get_new_current_block
	cmp	r0, #0
	beq	.L2753
	ldr	r0, .L2774+8
	b	.L2772
.L2745:
	ldrh	r3, [r4, #320]
	cmp	r3, #0
	beq	.L2753
	mov	r0, r4
	bl	write_phy_page_map_to_current_block
	cbz	r0, .L2754
	ldr	r0, .L2774
	bl	NAND_Print
.L2754:
	ldrb	r3, [r4, #17]	@ zero_extendqisi2
	lsls	r3, r3, #31
	bmi	.L2755
.L2773:
	movs	r0, #8
	b	.L2744
.L2755:
	add	r1, r4, #324
	mov	r0, r4
	bl	write_phy_page_map_to_current_block
	cmp	r0, #0
	beq	.L2773
	ldr	r0, .L2774
	bl	NAND_Print
	b	.L2773
.L2775:
	.align	2
.L2774:
	.word	.LC398
	.word	.LC399
	.word	.LC400
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
.L2777:
	ldr	r3, [r5, #4]
	ldrh	r3, [r3, #20]
	cmp	r3, r4
	bgt	.L2779
	movs	r0, #1
.L2776:
	pop	{r3, r4, r5, pc}
.L2779:
	mov	r0, r5
	bl	fill_no_use_data_to_page
	cmp	r0, #0
	beq	.L2776
	adds	r4, r4, #1
	b	.L2777
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
	cbz	r5, .L2789
	mov	r1, r5
	bl	is_current_blcok
	cmp	r0, #1
	bne	.L2785
	movs	r3, #0
	str	r3, [r4, #580]
.L2789:
	movs	r5, #2
	b	.L2783
.L2785:
	mov	r0, r4
	bl	is_block_in_invalid_list
	ldrh	r1, [r5]
	mov	r6, r0
	cbnz	r0, .L2786
	ldr	r0, .L2790
	bl	NAND_Print
	str	r6, [r4, #580]
	b	.L2789
.L2786:
	ldrh	r2, [r5, #6]
	movs	r5, #1
	ldr	r0, .L2790+4
	bl	NAND_Print
	ldr	r2, [r4, #1968]
	mov	r0, r4
	strh	r5, [r4, #578]	@ movhi
	ldr	r3, [r2, #132]
	add	r3, r3, r5
	str	r3, [r2, #132]
	bl	fill_no_use_data_to_current_block
	movw	r2, #65535
	ldr	r1, [r4, #580]
	mov	r0, r4
	bl	garbage_collect_first
	ldr	r2, [r4, #580]
	mov	r3, r0
	cbz	r0, .L2787
	ldrh	r1, [r2]
	ldr	r0, .L2790+8
	bl	NAND_Print
	movs	r3, #0
	str	r3, [r4, #580]
.L2783:
	mov	r0, r5
	pop	{r4, r5, r6, pc}
.L2787:
	strh	r0, [r4, #578]	@ movhi
	mov	r1, r2
	mov	r0, r4
	bl	is_block_in_free_list
	mov	r5, r0
	cbnz	r0, .L2788
	ldrh	r1, [r2]
	ldr	r0, .L2790+12
	bl	NAND_Print
	str	r5, [r4, #580]
	b	.L2783
.L2788:
	str	r3, [r4, #580]
	mov	r5, r3
	b	.L2783
.L2791:
	.align	2
.L2790:
	.word	.LC460
	.word	.LC461
	.word	.LC462
	.word	.LC463
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
	cbnz	r3, .L2793
.L2795:
	movs	r0, #0
.L2792:
	pop	{r4, pc}
.L2793:
	ldrh	r3, [r0, #384]
	cbz	r3, .L2796
	bl	check_static_wear_leveling
	cmp	r0, #0
	bne	.L2795
	mov	r0, r4
	pop	{r4, lr}
	b	done_static_wear_leveling
.L2796:
	movs	r0, #1
	b	.L2792
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
.L2798:
	cmp	r4, r5
	blt	.L2800
	movs	r0, #1
.L2797:
	pop	{r4, r5, r6, pc}
.L2800:
	mov	r0, r6
	bl	fill_no_use_data_to_page
	cmp	r0, #0
	beq	.L2797
	adds	r4, r4, #1
	b	.L2798
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
	ldr	r3, .L2807
	push	{r4, lr}
	ldr	r4, [r3, #8]
.L2805:
	cbnz	r4, .L2806
	mov	r0, r4
	pop	{r4, pc}
.L2806:
	ldr	r0, [r4, #12]
	movs	r1, #8
	bl	fill_no_use_page_to_current_block
	ldr	r4, [r4, #8]
	b	.L2805
.L2808:
	.align	2
.L2807:
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
	.global	read_mapping_page
	.global	m9_1z_8g
	.global	m9_1z_16g
	.global	m9_1y
	.global	m9_read_retry_reg_adr_1y
	.global	m9_read_retry_cmd_1y
	.global	m9_read_retry_exit_cmd_1y
	.global	m9_read_retry_enable_cmd_1y
	.global	m9_read_retry_reg_cnt
	.global	m9_read_retry_cycle
	.global	m9_read_retry_mode
	.global	m2_lsb_mode_reg_num
	.global	m2_lsb_mode_val
	.global	m2_lsb_mode_default_val
	.global	m2_lsb_mode_reg_adr
	.global	m2_read_retry_reg_adr_2
	.global	m2_read_retry_reg_adr_1
	.global	m2_read_retry_reg_adr
	.global	m2_read_retry_reg_num
	.global	m2_read_retry_cycle
	.global	m2_read_retry_mode
	.global	m4_p1_3
	.global	m4_p1_2
	.global	m4_p1_1
	.global	m4_read_retry_cycle
	.global	m4_read_retry_mode
	.global	m1_read_retry_reg_adr
	.global	m1_read_retry_reg_num
	.global	m1_read_retry_cycle
	.global	m1_read_retry_mode
	.global	m6_read_retry_exit_cmd
	.global	m6_read_retry_last_cmd
	.global	m6_p1_3
	.global	m6_p1_2
	.global	m6_read_retry_addr_2
	.global	m6_read_retry_cmd_2
	.global	m6_p1_1
	.global	m6_read_retry_addr_1
	.global	m6_read_retry_cmd_1
	.global	m6_read_retry_end_cmd
	.global	m6_read_retry_start_cmd
	.global	m6_read_retry_reg_cnt
	.global	m6_read_retry_cycle
	.global	m6_read_retry_mode
	.global	m7_1y
	.global	m7_read_retry_reg_adr_1y
	.global	m7_read_retry_cmd_1y
	.global	m7_read_retry_exit_cmd_1y
	.global	m7_read_retry_enable_cmd_1y
	.global	m7_24
	.global	m7_read_retry_reg_adr_24
	.global	m7_read_retry_cmd_24
	.global	m7_19_high
	.global	m7_19_low
	.global	m7_read_retry_reg_adr_19
	.global	m7_read_retry_cmd_19
	.global	m7_read_retry_disable_cmd_19
	.global	m7_read_retry_enable_cmd_19
	.global	m7_acti_start_addr
	.global	m7_acti_start_cmd
	.global	m7_read_retry_reg_cnt
	.global	m7_read_retry_cycle
	.global	m7_read_retry_mode
	.global	para1
	.global	para0
	.global	m3_lsb_mode_reg_num
	.global	m3_lsb_mode_val
	.global	m3_lsb_mode_default_val
	.global	m3_lsb_mode_reg_adr
	.global	m3_read_retry_val
	.global	m3_read_retry_reg_adr
	.global	m3_read_retry_reg_num
	.global	m3_read_retry_cycle
	.global	m3_read_retry_mode
	.global	m5_p1
	.global	m5_read_retry_cmd
	.global	m5_read_retry_addr
	.global	m5_read_retry_cycle
	.global	m5_read_retry_mode
	.global	nftl_blk_head
	.global	mod
	.global	com
	.section	.rodata
	.set	.LANCHOR18,. + 0
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
	.section	.bss.m1_read_retry_cycle,"aw",%nobits
	.set	.LANCHOR58,. + 0
	.type	m1_read_retry_cycle, %object
	.size	m1_read_retry_cycle, 1
m1_read_retry_cycle:
	.space	1
	.section	.bss.m1_read_retry_mode,"aw",%nobits
	.set	.LANCHOR55,. + 0
	.type	m1_read_retry_mode, %object
	.size	m1_read_retry_mode, 1
m1_read_retry_mode:
	.space	1
	.section	.bss.m1_read_retry_reg_adr,"aw",%nobits
	.set	.LANCHOR57,. + 0
	.type	m1_read_retry_reg_adr, %object
	.size	m1_read_retry_reg_adr, 4
m1_read_retry_reg_adr:
	.space	4
	.section	.bss.m1_read_retry_reg_num,"aw",%nobits
	.set	.LANCHOR56,. + 0
	.type	m1_read_retry_reg_num, %object
	.size	m1_read_retry_reg_num, 1
m1_read_retry_reg_num:
	.space	1
	.section	.bss.m2_lsb_mode_default_val,"aw",%nobits
	.set	.LANCHOR71,. + 0
	.type	m2_lsb_mode_default_val, %object
	.size	m2_lsb_mode_default_val, 4
m2_lsb_mode_default_val:
	.space	4
	.section	.bss.m2_lsb_mode_reg_adr,"aw",%nobits
	.set	.LANCHOR69,. + 0
	.type	m2_lsb_mode_reg_adr, %object
	.size	m2_lsb_mode_reg_adr, 4
m2_lsb_mode_reg_adr:
	.space	4
	.section	.bss.m2_lsb_mode_reg_num,"aw",%nobits
	.set	.LANCHOR68,. + 0
	.type	m2_lsb_mode_reg_num, %object
	.size	m2_lsb_mode_reg_num, 1
m2_lsb_mode_reg_num:
	.space	1
	.section	.bss.m2_lsb_mode_val,"aw",%nobits
	.set	.LANCHOR70,. + 0
	.type	m2_lsb_mode_val, %object
	.size	m2_lsb_mode_val, 4
m2_lsb_mode_val:
	.space	4
	.section	.bss.m2_read_retry_cycle,"aw",%nobits
	.set	.LANCHOR66,. + 0
	.type	m2_read_retry_cycle, %object
	.size	m2_read_retry_cycle, 1
m2_read_retry_cycle:
	.space	1
	.section	.bss.m2_read_retry_mode,"aw",%nobits
	.set	.LANCHOR65,. + 0
	.type	m2_read_retry_mode, %object
	.size	m2_read_retry_mode, 1
m2_read_retry_mode:
	.space	1
	.section	.bss.m2_read_retry_reg_adr,"aw",%nobits
	.set	.LANCHOR72,. + 0
	.type	m2_read_retry_reg_adr, %object
	.size	m2_read_retry_reg_adr, 8
m2_read_retry_reg_adr:
	.space	8
	.section	.bss.m2_read_retry_reg_num,"aw",%nobits
	.set	.LANCHOR67,. + 0
	.type	m2_read_retry_reg_num, %object
	.size	m2_read_retry_reg_num, 1
m2_read_retry_reg_num:
	.space	1
	.section	.bss.m3_lsb_mode_default_val,"aw",%nobits
	.set	.LANCHOR13,. + 0
	.type	m3_lsb_mode_default_val, %object
	.size	m3_lsb_mode_default_val, 5
m3_lsb_mode_default_val:
	.space	5
	.section	.bss.m3_lsb_mode_reg_adr,"aw",%nobits
	.set	.LANCHOR11,. + 0
	.type	m3_lsb_mode_reg_adr, %object
	.size	m3_lsb_mode_reg_adr, 5
m3_lsb_mode_reg_adr:
	.space	5
	.section	.bss.m3_lsb_mode_reg_num,"aw",%nobits
	.set	.LANCHOR10,. + 0
	.type	m3_lsb_mode_reg_num, %object
	.size	m3_lsb_mode_reg_num, 1
m3_lsb_mode_reg_num:
	.space	1
	.section	.bss.m3_lsb_mode_val,"aw",%nobits
	.set	.LANCHOR12,. + 0
	.type	m3_lsb_mode_val, %object
	.size	m3_lsb_mode_val, 5
m3_lsb_mode_val:
	.space	5
	.section	.bss.m3_read_retry_cycle,"aw",%nobits
	.set	.LANCHOR8,. + 0
	.type	m3_read_retry_cycle, %object
	.size	m3_read_retry_cycle, 1
m3_read_retry_cycle:
	.space	1
	.section	.bss.m3_read_retry_mode,"aw",%nobits
	.set	.LANCHOR7,. + 0
	.type	m3_read_retry_mode, %object
	.size	m3_read_retry_mode, 1
m3_read_retry_mode:
	.space	1
	.section	.bss.m3_read_retry_reg_adr,"aw",%nobits
	.set	.LANCHOR14,. + 0
	.type	m3_read_retry_reg_adr, %object
	.size	m3_read_retry_reg_adr, 4
m3_read_retry_reg_adr:
	.space	4
	.section	.bss.m3_read_retry_reg_num,"aw",%nobits
	.set	.LANCHOR9,. + 0
	.type	m3_read_retry_reg_num, %object
	.size	m3_read_retry_reg_num, 1
m3_read_retry_reg_num:
	.space	1
	.section	.bss.m3_read_retry_val,"aw",%nobits
	.align	1
	.set	.LANCHOR16,. + 0
	.type	m3_read_retry_val, %object
	.size	m3_read_retry_val, 56
m3_read_retry_val:
	.space	56
	.section	.bss.m4_p1,"aw",%nobits
	.align	2
	.set	.LANCHOR59,. + 0
	.type	m4_p1, %object
	.size	m4_p1, 4
m4_p1:
	.space	4
	.section	.bss.m4_read_retry_cycle,"aw",%nobits
	.set	.LANCHOR62,. + 0
	.type	m4_read_retry_cycle, %object
	.size	m4_read_retry_cycle, 1
m4_read_retry_cycle:
	.space	1
	.section	.bss.m4_read_retry_mode,"aw",%nobits
	.set	.LANCHOR61,. + 0
	.type	m4_read_retry_mode, %object
	.size	m4_read_retry_mode, 1
m4_read_retry_mode:
	.space	1
	.section	.bss.m5_read_retry_cycle,"aw",%nobits
	.set	.LANCHOR3,. + 0
	.type	m5_read_retry_cycle, %object
	.size	m5_read_retry_cycle, 1
m5_read_retry_cycle:
	.space	1
	.section	.bss.m5_read_retry_mode,"aw",%nobits
	.set	.LANCHOR2,. + 0
	.type	m5_read_retry_mode, %object
	.size	m5_read_retry_mode, 1
m5_read_retry_mode:
	.space	1
	.section	.bss.m6_read_retry_cycle,"aw",%nobits
	.set	.LANCHOR38,. + 0
	.type	m6_read_retry_cycle, %object
	.size	m6_read_retry_cycle, 1
m6_read_retry_cycle:
	.space	1
	.section	.bss.m6_read_retry_mode,"aw",%nobits
	.set	.LANCHOR37,. + 0
	.type	m6_read_retry_mode, %object
	.size	m6_read_retry_mode, 1
m6_read_retry_mode:
	.space	1
	.section	.bss.m6_read_retry_reg_cnt,"aw",%nobits
	.set	.LANCHOR39,. + 0
	.type	m6_read_retry_reg_cnt, %object
	.size	m6_read_retry_reg_cnt, 1
m6_read_retry_reg_cnt:
	.space	1
	.section	.bss.m6_retry_flag,"aw",%nobits
	.align	2
	.set	.LANCHOR44,. + 0
	.type	m6_retry_flag, %object
	.size	m6_retry_flag, 4
m6_retry_flag:
	.space	4
	.section	.bss.m6_sclk0_bak,"aw",%nobits
	.align	2
	.set	.LANCHOR42,. + 0
	.type	m6_sclk0_bak, %object
	.size	m6_sclk0_bak, 4
m6_sclk0_bak:
	.space	4
	.section	.bss.m6_sclk1_bak,"aw",%nobits
	.align	2
	.set	.LANCHOR41,. + 0
	.type	m6_sclk1_bak, %object
	.size	m6_sclk1_bak, 4
m6_sclk1_bak:
	.space	4
	.section	.bss.m6_toggle_mode_flag,"aw",%nobits
	.align	2
	.set	.LANCHOR51,. + 0
	.type	m6_toggle_mode_flag, %object
	.size	m6_toggle_mode_flag, 4
m6_toggle_mode_flag:
	.space	4
	.section	.bss.m7_read_retry_cycle,"aw",%nobits
	.set	.LANCHOR22,. + 0
	.type	m7_read_retry_cycle, %object
	.size	m7_read_retry_cycle, 1
m7_read_retry_cycle:
	.space	1
	.section	.bss.m7_read_retry_mode,"aw",%nobits
	.set	.LANCHOR21,. + 0
	.type	m7_read_retry_mode, %object
	.size	m7_read_retry_mode, 1
m7_read_retry_mode:
	.space	1
	.section	.bss.m7_read_retry_reg_cnt,"aw",%nobits
	.set	.LANCHOR23,. + 0
	.type	m7_read_retry_reg_cnt, %object
	.size	m7_read_retry_reg_cnt, 1
m7_read_retry_reg_cnt:
	.space	1
	.section	.bss.m7_sclk0_bak,"aw",%nobits
	.align	2
	.set	.LANCHOR25,. + 0
	.type	m7_sclk0_bak, %object
	.size	m7_sclk0_bak, 4
m7_sclk0_bak:
	.space	4
	.section	.bss.m7_sclk1_bak,"aw",%nobits
	.align	2
	.set	.LANCHOR24,. + 0
	.type	m7_sclk1_bak, %object
	.size	m7_sclk1_bak, 4
m7_sclk1_bak:
	.space	4
	.section	.bss.m7_toggle_mode_flag,"aw",%nobits
	.align	2
	.set	.LANCHOR26,. + 0
	.type	m7_toggle_mode_flag, %object
	.size	m7_toggle_mode_flag, 4
m7_toggle_mode_flag:
	.space	4
	.section	.bss.m9_read_retry_cycle,"aw",%nobits
	.set	.LANCHOR74,. + 0
	.type	m9_read_retry_cycle, %object
	.size	m9_read_retry_cycle, 1
m9_read_retry_cycle:
	.space	1
	.section	.bss.m9_read_retry_mode,"aw",%nobits
	.set	.LANCHOR73,. + 0
	.type	m9_read_retry_mode, %object
	.size	m9_read_retry_mode, 1
m9_read_retry_mode:
	.space	1
	.section	.bss.m9_read_retry_reg_cnt,"aw",%nobits
	.set	.LANCHOR75,. + 0
	.type	m9_read_retry_reg_cnt, %object
	.size	m9_read_retry_reg_cnt, 1
m9_read_retry_reg_cnt:
	.space	1
	.section	.bss.m9_sclk0_bak,"aw",%nobits
	.align	2
	.set	.LANCHOR77,. + 0
	.type	m9_sclk0_bak, %object
	.size	m9_sclk0_bak, 4
m9_sclk0_bak:
	.space	4
	.section	.bss.m9_sclk1_bak,"aw",%nobits
	.align	2
	.set	.LANCHOR76,. + 0
	.type	m9_sclk1_bak, %object
	.size	m9_sclk1_bak, 4
m9_sclk1_bak:
	.space	4
	.section	.bss.nftl_blk_head,"aw",%nobits
	.align	2
	.set	.LANCHOR0,. + 0
	.type	nftl_blk_head, %object
	.size	nftl_blk_head, 72
nftl_blk_head:
	.space	72
	.section	.bss.read_mapping_page,"aw",%nobits
	.align	2
	.set	.LANCHOR93,. + 0
	.type	read_mapping_page, %object
	.size	read_mapping_page, 4
read_mapping_page:
	.space	4
	.section	.data.m2_read_retry_reg_adr_1,"aw",%progbits
	.type	m2_read_retry_reg_adr_1, %object
	.size	m2_read_retry_reg_adr_1, 8
m2_read_retry_reg_adr_1:
	.byte	-52
	.byte	-65
	.byte	-86
	.byte	-85
	.byte	-51
	.byte	-83
	.byte	-82
	.byte	-81
	.section	.data.m2_read_retry_reg_adr_2,"aw",%progbits
	.type	m2_read_retry_reg_adr_2, %object
	.size	m2_read_retry_reg_adr_2, 8
m2_read_retry_reg_adr_2:
	.byte	-80
	.byte	-79
	.byte	-78
	.byte	-77
	.byte	-76
	.byte	-75
	.byte	-74
	.byte	-73
	.section	.data.m4_p1_1,"aw",%progbits
	.set	.LANCHOR63,. + 0
	.type	m4_p1_1, %object
	.size	m4_p1_1, 10
m4_p1_1:
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
	.section	.data.m4_p1_2,"aw",%progbits
	.set	.LANCHOR60,. + 0
	.type	m4_p1_2, %object
	.size	m4_p1_2, 12
m4_p1_2:
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
	.section	.data.m4_p1_3,"aw",%progbits
	.set	.LANCHOR64,. + 0
	.type	m4_p1_3, %object
	.size	m4_p1_3, 7
m4_p1_3:
	.byte	1
	.byte	2
	.byte	3
	.byte	0
	.byte	1
	.byte	2
	.byte	3
	.section	.data.m5_p1,"aw",%progbits
	.set	.LANCHOR6,. + 0
	.type	m5_p1, %object
	.size	m5_p1, 60
m5_p1:
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
	.section	.data.m5_read_retry_addr,"aw",%progbits
	.set	.LANCHOR5,. + 0
	.type	m5_read_retry_addr, %object
	.size	m5_read_retry_addr, 4
m5_read_retry_addr:
	.byte	-89
	.byte	-92
	.byte	-91
	.byte	-90
	.section	.data.m5_read_retry_cmd,"aw",%progbits
	.set	.LANCHOR4,. + 0
	.type	m5_read_retry_cmd, %object
	.size	m5_read_retry_cmd, 4
m5_read_retry_cmd:
	.byte	-95
	.byte	-95
	.byte	-95
	.byte	-95
	.section	.data.m6_p1_1,"aw",%progbits
	.set	.LANCHOR54,. + 0
	.type	m6_p1_1, %object
	.size	m6_p1_1, 28
m6_p1_1:
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
	.section	.data.m6_p1_2,"aw",%progbits
	.set	.LANCHOR52,. + 0
	.type	m6_p1_2, %object
	.size	m6_p1_2, 40
m6_p1_2:
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
	.section	.data.m6_p1_3,"aw",%progbits
	.set	.LANCHOR53,. + 0
	.type	m6_p1_3, %object
	.size	m6_p1_3, 55
m6_p1_3:
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
	.section	.data.m6_read_retry_addr_1,"aw",%progbits
	.set	.LANCHOR50,. + 0
	.type	m6_read_retry_addr_1, %object
	.size	m6_read_retry_addr_1, 4
m6_read_retry_addr_1:
	.byte	4
	.byte	5
	.byte	6
	.byte	7
	.section	.data.m6_read_retry_addr_2,"aw",%progbits
	.set	.LANCHOR49,. + 0
	.type	m6_read_retry_addr_2, %object
	.size	m6_read_retry_addr_2, 5
m6_read_retry_addr_2:
	.byte	4
	.byte	5
	.byte	6
	.byte	7
	.byte	13
	.section	.data.m6_read_retry_cmd_1,"aw",%progbits
	.set	.LANCHOR47,. + 0
	.type	m6_read_retry_cmd_1, %object
	.size	m6_read_retry_cmd_1, 4
m6_read_retry_cmd_1:
	.byte	85
	.byte	85
	.byte	85
	.byte	85
	.section	.data.m6_read_retry_cmd_2,"aw",%progbits
	.set	.LANCHOR43,. + 0
	.type	m6_read_retry_cmd_2, %object
	.size	m6_read_retry_cmd_2, 5
m6_read_retry_cmd_2:
	.byte	85
	.byte	85
	.byte	85
	.byte	85
	.byte	85
	.section	.data.m6_read_retry_end_cmd,"aw",%progbits
	.set	.LANCHOR48,. + 0
	.type	m6_read_retry_end_cmd, %object
	.size	m6_read_retry_end_cmd, 2
m6_read_retry_end_cmd:
	.byte	38
	.byte	93
	.section	.data.m6_read_retry_exit_cmd,"aw",%progbits
	.set	.LANCHOR45,. + 0
	.type	m6_read_retry_exit_cmd, %object
	.size	m6_read_retry_exit_cmd, 1
m6_read_retry_exit_cmd:
	.byte	-1
	.section	.data.m6_read_retry_last_cmd,"aw",%progbits
	.set	.LANCHOR46,. + 0
	.type	m6_read_retry_last_cmd, %object
	.size	m6_read_retry_last_cmd, 1
m6_read_retry_last_cmd:
	.byte	-77
	.section	.data.m6_read_retry_start_cmd,"aw",%progbits
	.set	.LANCHOR40,. + 0
	.type	m6_read_retry_start_cmd, %object
	.size	m6_read_retry_start_cmd, 2
m6_read_retry_start_cmd:
	.byte	92
	.byte	-59
	.section	.data.m7_19_high,"aw",%progbits
	.set	.LANCHOR31,. + 0
	.type	m7_19_high, %object
	.size	m7_19_high, 60
m7_19_high:
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
	.section	.data.m7_19_low,"aw",%progbits
	.set	.LANCHOR30,. + 0
	.type	m7_19_low, %object
	.size	m7_19_low, 48
m7_19_low:
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
	.section	.data.m7_1y,"aw",%progbits
	.set	.LANCHOR36,. + 0
	.type	m7_1y, %object
	.size	m7_1y, 76
m7_1y:
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
	.section	.data.m7_24,"aw",%progbits
	.set	.LANCHOR34,. + 0
	.type	m7_24, %object
	.size	m7_24, 27
m7_24:
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
	.section	.data.m7_acti_start_addr,"aw",%progbits
	.set	.LANCHOR20,. + 0
	.type	m7_acti_start_addr, %object
	.size	m7_acti_start_addr, 9
m7_acti_start_addr:
	.byte	4
	.byte	5
	.byte	6
	.byte	7
	.byte	8
	.byte	9
	.byte	10
	.byte	11
	.byte	12
	.section	.data.m7_acti_start_cmd,"aw",%progbits
	.set	.LANCHOR19,. + 0
	.type	m7_acti_start_cmd, %object
	.size	m7_acti_start_cmd, 2
m7_acti_start_cmd:
	.byte	59
	.byte	-71
	.section	.data.m7_read_retry_cmd_19,"aw",%progbits
	.set	.LANCHOR28,. + 0
	.type	m7_read_retry_cmd_19, %object
	.size	m7_read_retry_cmd_19, 3
m7_read_retry_cmd_19:
	.byte	83
	.byte	83
	.byte	83
	.section	.data.m7_read_retry_cmd_1y,"aw",%progbits
	.set	.LANCHOR29,. + 0
	.type	m7_read_retry_cmd_1y, %object
	.size	m7_read_retry_cmd_1y, 1
m7_read_retry_cmd_1y:
	.byte	-17
	.section	.data.m7_read_retry_cmd_24,"aw",%progbits
	.type	m7_read_retry_cmd_24, %object
	.size	m7_read_retry_cmd_24, 3
m7_read_retry_cmd_24:
	.byte	83
	.byte	83
	.byte	83
	.section	.data.m7_read_retry_disable_cmd_19,"aw",%progbits
	.set	.LANCHOR27,. + 0
	.type	m7_read_retry_disable_cmd_19, %object
	.size	m7_read_retry_disable_cmd_19, 1
m7_read_retry_disable_cmd_19:
	.byte	-42
	.section	.data.m7_read_retry_enable_cmd_19,"aw",%progbits
	.type	m7_read_retry_enable_cmd_19, %object
	.size	m7_read_retry_enable_cmd_19, 1
m7_read_retry_enable_cmd_19:
	.byte	-74
	.section	.data.m7_read_retry_enable_cmd_1y,"aw",%progbits
	.type	m7_read_retry_enable_cmd_1y, %object
	.size	m7_read_retry_enable_cmd_1y, 1
m7_read_retry_enable_cmd_1y:
	.byte	93
	.section	.data.m7_read_retry_exit_cmd_1y,"aw",%progbits
	.type	m7_read_retry_exit_cmd_1y, %object
	.size	m7_read_retry_exit_cmd_1y, 1
m7_read_retry_exit_cmd_1y:
	.byte	-1
	.section	.data.m7_read_retry_reg_adr_19,"aw",%progbits
	.set	.LANCHOR32,. + 0
	.type	m7_read_retry_reg_adr_19, %object
	.size	m7_read_retry_reg_adr_19, 9
m7_read_retry_reg_adr_19:
	.byte	4
	.byte	5
	.byte	7
	.space	6
	.section	.data.m7_read_retry_reg_adr_1y,"aw",%progbits
	.set	.LANCHOR35,. + 0
	.type	m7_read_retry_reg_adr_1y, %object
	.size	m7_read_retry_reg_adr_1y, 1
m7_read_retry_reg_adr_1y:
	.byte	17
	.section	.data.m7_read_retry_reg_adr_24,"aw",%progbits
	.set	.LANCHOR33,. + 0
	.type	m7_read_retry_reg_adr_24, %object
	.size	m7_read_retry_reg_adr_24, 9
m7_read_retry_reg_adr_24:
	.byte	4
	.byte	5
	.byte	7
	.space	6
	.section	.data.m9_1y,"aw",%progbits
	.set	.LANCHOR81,. + 0
	.type	m9_1y, %object
	.size	m9_1y, 124
m9_1y:
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
	.section	.data.m9_1z_16g,"aw",%progbits
	.set	.LANCHOR79,. + 0
	.type	m9_1z_16g, %object
	.size	m9_1z_16g, 128
m9_1z_16g:
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
	.section	.data.m9_1z_8g,"aw",%progbits
	.set	.LANCHOR80,. + 0
	.type	m9_1z_8g, %object
	.size	m9_1z_8g, 124
m9_1z_8g:
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
	.section	.data.m9_read_retry_cmd_1y,"aw",%progbits
	.set	.LANCHOR78,. + 0
	.type	m9_read_retry_cmd_1y, %object
	.size	m9_read_retry_cmd_1y, 1
m9_read_retry_cmd_1y:
	.byte	-17
	.section	.data.m9_read_retry_enable_cmd_1y,"aw",%progbits
	.type	m9_read_retry_enable_cmd_1y, %object
	.size	m9_read_retry_enable_cmd_1y, 1
m9_read_retry_enable_cmd_1y:
	.byte	93
	.section	.data.m9_read_retry_exit_cmd_1y,"aw",%progbits
	.type	m9_read_retry_exit_cmd_1y, %object
	.size	m9_read_retry_exit_cmd_1y, 1
m9_read_retry_exit_cmd_1y:
	.byte	-1
	.section	.data.m9_read_retry_reg_adr_1y,"aw",%progbits
	.set	.LANCHOR82,. + 0
	.type	m9_read_retry_reg_adr_1y, %object
	.size	m9_read_retry_reg_adr_1y, 1
m9_read_retry_reg_adr_1y:
	.byte	17
	.section	.rodata.__FUNCTION__.8503,"a",%progbits
	.set	.LANCHOR92,. + 0
	.type	__FUNCTION__.8503, %object
	.size	__FUNCTION__.8503, 25
__FUNCTION__.8503:
	.ascii	"nand_power_off_ecc_error\000"
	.section	.rodata.__func__.8016,"a",%progbits
	.set	.LANCHOR94,. + 0
	.type	__func__.8016, %object
	.size	__func__.8016, 23
__func__.8016:
	.ascii	"get_current_used_block\000"
	.section	.rodata.__func__.8118,"a",%progbits
	.set	.LANCHOR1,. + 0
	.type	__func__.8118, %object
	.size	__func__.8118, 9
__func__.8118:
	.ascii	"init_nrc\000"
	.section	.rodata.__func__.8218,"a",%progbits
	.set	.LANCHOR87,. + 0
	.type	__func__.8218, %object
	.size	__func__.8218, 23
__func__.8218:
	.ascii	"torture_block_and_mark\000"
	.section	.rodata.__func__.8375,"a",%progbits
	.set	.LANCHOR83,. + 0
	.type	__func__.8375, %object
	.size	__func__.8375, 18
__func__.8375:
	.ascii	"nand_dbg_phy_read\000"
	.section	.rodata.__func__.8388,"a",%progbits
	.set	.LANCHOR84,. + 0
	.type	__func__.8388, %object
	.size	__func__.8388, 23
__func__.8388:
	.ascii	"nand_dbg_zone_phy_read\000"
	.section	.rodata.__func__.8399,"a",%progbits
	.set	.LANCHOR85,. + 0
	.type	__func__.8399, %object
	.size	__func__.8399, 24
__func__.8399:
	.ascii	"nand_dbg_zone_phy_write\000"
	.section	.rodata.__func__.8408,"a",%progbits
	.set	.LANCHOR86,. + 0
	.type	__func__.8408, %object
	.size	__func__.8408, 19
__func__.8408:
	.ascii	"nand_dbg_phy_write\000"
	.section	.rodata.__func__.8477,"a",%progbits
	.set	.LANCHOR90,. + 0
	.type	__func__.8477, %object
	.size	__func__.8477, 19
__func__.8477:
	.ascii	"write_data_to_nand\000"
	.section	.rodata.__func__.8664,"a",%progbits
	.set	.LANCHOR88,. + 0
	.type	__func__.8664, %object
	.size	__func__.8664, 19
__func__.8664:
	.ascii	"get_logic_page_map\000"
	.section	.rodata.__nand_discard.str1.1,"aMS",%progbits,1
.LC216:
	.ascii	"[NE]dicard sector:0x%x,len:0x%x\012\000"
	.section	.rodata.__nand_flush_sector_write_cache.str1.1,"aMS",%progbits,1
.LC202:
	.ascii	"[NE]nand_flush_write_cache write error\012\000"
	.section	.rodata.__nand_flush_write_cache.str1.1,"aMS",%progbits,1
.LC214:
	.ascii	"[ND]flush cache %d %d!\012\000"
	.section	.rodata.__nand_read.str1.1,"aMS",%progbits,1
.LC196:
	.ascii	"[NE]read sector:0x%x,len:0x%x\012\000"
	.section	.rodata.__nand_write.str1.1,"aMS",%progbits,1
.LC198:
	.ascii	"[NE]write sector:0x%x,len:0x%x\012\000"
	.section	.rodata._nftl_nand_read_logic_page.str1.1,"aMS",%progbits,1
.LC423:
	.ascii	"[NE]read a page: %d READ_RECLAIM!\012\000"
	.section	.rodata._nftl_nand_write_logic_page.str1.1,"aMS",%progbits,1
.LC459:
	.ascii	"[NE]error nand_write_logic_page_no_gc\012\000"
	.section	.rodata.adjust_free_list.str1.1,"aMS",%progbits,1
.LC141:
	.ascii	"[NE] adjust free list fail!!\012\000"
.LC142:
	.ascii	"[NE] adjust free list : %d %d!!\012\000"
	.section	.rodata.block_last_page_ecc_error.str1.1,"aMS",%progbits,1
.LC160:
	.ascii	"[NE]do block_last_page_ecc_error!!\012\000"
.LC161:
	.ascii	"[NE]====no memory!!!!!=====\012\000"
.LC162:
	.ascii	"[NE]current_block_ecc_error unkown page data!!\012\000"
.LC163:
	.ascii	"[NE]current_block_ecc_error invalid page data!!\012"
	.ascii	"\000"
.LC164:
	.ascii	"[NE]current_block_ecc_error no free block!!!\012\000"
.LC165:
	.ascii	"[NE]do block_last_page_ecc_error end!!\012\000"
	.section	.rodata.build_zone_list.str1.1,"aMS",%progbits,1
.LC431:
	.ascii	"[ND]first\012\000"
.LC432:
	.ascii	"[NE]init_zone_after_first_scan error\012\000"
.LC433:
	.ascii	"[NE]second_scan_all_blocks error\012\000"
.LC434:
	.ascii	"[ND]nftl ok!\012\000"
	.section	.rodata.build_zone_list_first.str1.1,"aMS",%progbits,1
.LC147:
	.ascii	"[ND]build_zone_list_first.\012\000"
.LC148:
	.ascii	"[NE]no block %d\012\000"
	.section	.rodata.check_invalid_page_right.str1.1,"aMS",%progbits,1
.LC395:
	.ascii	"[NE]logic_page_map_temp NULL!!\012\000"
.LC396:
	.ascii	"[NE]invaild_page num error:[%d] [%d]  used_page:[%d"
	.ascii	"]!!\012\000"
	.section	.rodata.check_static_wear_leveling.str1.1,"aMS",%progbits,1
.LC190:
	.ascii	"[ND]swl: static WL need to do(%d : %d)\012\000"
.LC191:
	.ascii	"[ND]swl: min_ec_blk_no = %d,\012\000"
.LC192:
	.ascii	"[ND]swl: zone->blocks = %d)\012\000"
	.section	.rodata.com,"a",%progbits
	.set	.LANCHOR95,. + 0
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
.LC296:
	.ascii	"exception case wb %04x %04x, len: %d\012\000"
	.section	.rodata.cross_talk_rebuild_current_block.str1.1,"aMS",%progbits,1
.LC333:
	.ascii	"[ND]cross_talk_rebuild_current_block fail 1\012\000"
.LC334:
	.ascii	"[ND]cross_talk_rebuild_current_block!! %d,%d,%d!\012"
	.ascii	"\000"
.LC335:
	.ascii	"[ND]cross_talk_rebuild_current_block fail 2\012\000"
.LC336:
	.ascii	"[ND]cross_talk_rebuild_current_block!! %d,%d,%d,%d,"
	.ascii	"%d,%d!\012\000"
.LC337:
	.ascii	"[ND]cross_talk_rebuild_current_block fail 3\012\000"
.LC338:
	.ascii	"[ND]corss talk rebuild 1 %d %d %d!\012\000"
.LC339:
	.ascii	"[ND]corss talk rebuild 2 %d %d %d!\012\000"
.LC340:
	.ascii	"[NE] get free block fail!\012\000"
.LC341:
	.ascii	"[NE]something is error here 14!\012\000"
.LC342:
	.ascii	"[NE]page lost %d %d\012\000"
	.section	.rodata.current_block_ecc_error.str1.1,"aMS",%progbits,1
.LC386:
	.ascii	"[NE]do current_block_ecc_error %d!!\012\000"
.LC387:
	.ascii	"[NE]slc power off ecc error 2!!\012\000"
.LC388:
	.ascii	"[NE]current_block_ecc ok page %d %d!!\012\000"
.LC389:
	.ascii	"[NE]current_block_ecc_error page %d!!\012\000"
.LC390:
	.ascii	"[NE]current_block_ecc_error invalid page data %d!!\012"
	.ascii	"\000"
.LC391:
	.ascii	"[NE]current_block_ecc_error unkown page data %d!!\012"
	.ascii	"\000"
.LC392:
	.ascii	"[NE]do current_block_ecc_error end %d %d!!\012\000"
	.section	.rodata.do_after_write.str1.1,"aMS",%progbits,1
.LC398:
	.ascii	"[NE]write_phy_page_map_to_current_block error!\012\000"
.LC399:
	.ascii	"[NE]get_new_current_block error1!\012\000"
.LC400:
	.ascii	"[NE]get_new_current_block error2!\012\000"
	.section	.rodata.do_before_write.str1.1,"aMS",%progbits,1
.LC393:
	.ascii	"[NE]get_new_current_block error3!\012\000"
.LC394:
	.ascii	"[NE]get_new_current_block error4!\012\000"
	.section	.rodata.do_prio_gc.str1.1,"aMS",%progbits,1
.LC418:
	.ascii	"[ND]no block to prio gc\012\000"
.LC419:
	.ascii	"[ND]no need gc:%d, type:%d\012\000"
.LC420:
	.ascii	"[NE]do_prio_gc garbage_collect_first fail:%d, type:"
	.ascii	"%d\012\000"
.LC421:
	.ascii	"[ND]prio gc fail1:%d, type:%d\012\000"
.LC422:
	.ascii	"[ND]prio gc fail2:%d, type:%d\012\000"
	.section	.rodata.do_write_error.str1.1,"aMS",%progbits,1
.LC320:
	.ascii	"[NE]no free block\012\000"
.LC321:
	.ascii	"[NE]do_write_error erase error!\012\000"
	.section	.rodata.do_write_error_in_build_list.str1.1,"aMS",%progbits,1
.LC129:
	.ascii	"[NE]NAND EVENT:do_write_error erase error1!\012\000"
.LC130:
	.ascii	"[NE]NAND EVENT:do_write_error erase error2!\012\000"
	.section	.rodata.done_static_wear_leveling.str1.1,"aMS",%progbits,1
.LC460:
	.ascii	"[ND]swl: the block(%d) for static WL is not in inva"
	.ascii	"lid_list\012\000"
.LC461:
	.ascii	"[ND]swl: done_static_wear_leveling(min_ec) block:%d"
	.ascii	", erase_count:%d\012\000"
.LC462:
	.ascii	"[NE]swl: garbage_collect_first fail:%d\012\000"
.LC463:
	.ascii	"[NE]swl: last->check fail:%d\012\000"
	.section	.rodata.dump_buf.str1.1,"aMS",%progbits,1
.LC123:
	.ascii	" s %p %d\012\000"
.LC124:
	.ascii	"%08x: \000"
.LC125:
	.ascii	"%02x\012\000"
	.section	.rodata.erase_block.str1.1,"aMS",%progbits,1
.LC139:
	.ascii	"[NE]erase_block block %d!\012\000"
.LC140:
	.ascii	"[NE]erase_block block fail %d!\012\000"
	.section	.rodata.first_scan_all_blocks.str1.1,"aMS",%progbits,1
.LC177:
	.ascii	"[NE] ftl end block! %d!\012\000"
.LC178:
	.ascii	"[NE]READ_RECLAIM!\012\000"
.LC179:
	.ascii	"[NE]do nothing\012\000"
.LC180:
	.ascii	"[NE]fill block %d\012\000"
.LC181:
	.ascii	"[NE]unkown page 0 spare data %x,%x,%x,%x,%x,%x,%x!!"
	.ascii	"\012\000"
.LC182:
	.ascii	"[ND]  bad block: %d\012\000"
.LC183:
	.ascii	"[NE]first page ecc error ! %d!\012\000"
.LC184:
	.ascii	"[NE]last page ok ! %d!\012\000"
.LC185:
	.ascii	"[NE]last page blank ! %d!\012\000"
.LC186:
	.ascii	"[NE]last page unkown ! %d!\012\000"
.LC187:
	.ascii	"[NE]first page last page ecc error ! %d!\012\000"
.LC188:
	.ascii	"last page spare data: %02x %02x %02x %02x %02x %02x"
	.ascii	" %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\000"
.LC189:
	.ascii	"[ND]bad block: %d\012\000"
	.section	.rodata.flush_write_cache_to_nand.str1.1,"aMS",%progbits,1
.LC203:
	.ascii	"[NE]flush write cache to nand no cache!\012\000"
.LC204:
	.ascii	"[NE]ECC_LIMIT happened 10! page:%d cache: %d!\012\000"
.LC205:
	.ascii	"[NE]flush write cache to nand error!\012\000"
.LC206:
	.ascii	"fwctn %d\012\000"
.LC207:
	.ascii	"fwctn w+\012\000"
	.section	.rodata.garbage_collect.str1.1,"aMS",%progbits,1
.LC456:
	.ascii	"[NE]garbage_collect phy_block null\012\000"
.LC457:
	.ascii	"[ND]garbage too frequency! %d:%d\012\000"
.LC458:
	.ascii	"[NE]garbage_collect_first wrong! %d\012\000"
	.section	.rodata.garbage_collect_first.str1.1,"aMS",%progbits,1
.LC405:
	.ascii	"[NE]garbage_collect_first none!!\012\000"
.LC406:
	.ascii	"[NE]garbage_collect_first wrong!!\012\000"
.LC407:
	.ascii	"[NE]garbage_collect_first cannot find PHY_MAPPING_P"
	.ascii	"AGE %d %d %d %d!!\012\000"
.LC408:
	.ascii	"[NE]garbage_collect_first cannot find PHY_MAPPING_P"
	.ascii	"AGE %x %x %x %x !!\012\000"
.LC409:
	.ascii	"[NE]garbage_collect_first cannot find PHY_MAPPING_P"
	.ascii	"AGE %x %x %x %x %x!!\012\000"
.LC410:
	.ascii	"[NE]garbage_collect_first rebuild %d,%d!!\012\000"
.LC411:
	.ascii	"[NE]garbage_collect_first PHY_MAPPING_PAGE ecc err "
	.ascii	"block:%d !!\012\000"
.LC412:
	.ascii	"[NE]garbage_collect_first invaild_page num error!!\012"
	.ascii	"\000"
.LC413:
	.ascii	"[NE]garbage_collect_first read a logic page error %"
	.ascii	"d %d %d %d!!\012\000"
.LC414:
	.ascii	"[NE]%x %x %x %x %x !!\012\000"
.LC415:
	.ascii	"[NE]%x %x %x %x %x!!\012\000"
.LC416:
	.ascii	"[NE]nand_write_logic_page_no_gc gc fail!!\012\000"
.LC417:
	.ascii	"[NE]erase ecc error block\012\000"
	.section	.rodata.gc_all.str1.1,"aMS",%progbits,1
.LC426:
	.ascii	"[NE]gc all end\012\000"
	.section	.rodata.gc_one.str1.1,"aMS",%progbits,1
.LC424:
	.ascii	"[NE]gc_one ok!\012\000"
.LC425:
	.ascii	"[NE]gc_one error!\012\000"
	.section	.rodata.gc_win_block.str1.1,"aMS",%progbits,1
.LC427:
	.ascii	"[NE] gc_win_block gc_one error \012\000"
	.section	.rodata.get_logic_page_map.str1.1,"aMS",%progbits,1
.LC166:
	.ascii	"%s:logic_page:0x%x >= total_pages:0x%x\012\000"
	.section	.rodata.get_new_current_block.str1.1,"aMS",%progbits,1
.LC330:
	.ascii	"[ND]swl: new_cur_blk->alloc ok(max_ec): block_no = "
	.ascii	"%d; ec = %d; \012\000"
.LC331:
	.ascii	"[NE]no free block!!!!\012\000"
	.section	.rodata.get_used_page_num.str1.1,"aMS",%progbits,1
.LC314:
	.ascii	"[NE]get_used_page_num ecc error block:%d page:%d!\012"
	.ascii	"\000"
.LC315:
	.ascii	"[NE]slc power off ecc error 4!!\012\000"
.LC316:
	.ascii	"[NE]get_used_page_num used %d %d!\012\000"
	.section	.rodata.get_used_page_num_no_crosstalk.str1.1,"aMS",%progbits,1
.LC317:
	.ascii	"[NE]gupnnc ecc err b:%d p:%d!\012\000"
.LC318:
	.ascii	"[NE]gupnnc power off ecc err!!\012\000"
.LC319:
	.ascii	"[NE]cross talk block:%d total_good_num %d good_page"
	.ascii	"_num: %d!\012\000"
	.section	.rodata.init_nrc.isra.0.str1.1,"aMS",%progbits,1
.LC8:
	.ascii	"%s %d : fail 1\012\000"
.LC9:
	.ascii	"%s %d : fail 2\012\000"
.LC10:
	.ascii	"%s %d : fail 3\012\000"
.LC11:
	.ascii	"%s %d : fail 4\012\000"
.LC12:
	.ascii	"pages_per_blk: %d, %p, %p, %p, %p\012\000"
	.section	.rodata.init_zone_after_first_scan.str1.1,"aMS",%progbits,1
.LC126:
	.ascii	"[ND]before second %d %d.\012\000"
.LC127:
	.ascii	"[NE]this zone not enough data block!!\012\000"
.LC128:
	.ascii	"[NE]this zone not enough free block %d , %d!!\012\000"
	.section	.rodata.is_no_use_device.str1.1,"aMS",%progbits,1
.LC149:
	.ascii	"[NE]mtd size changed !\012\000"
	.section	.rodata.m0_special_exit.str1.1,"aMS",%progbits,1
.LC22:
	.ascii	" m0 special exit \012\000"
	.section	.rodata.m0_special_init.str1.1,"aMS",%progbits,1
.LC21:
	.ascii	" m0 special init \012\000"
	.section	.rodata.m1_get_read_retry_cfg.str1.1,"aMS",%progbits,1
.LC55:
	.ascii	"_get_read_retry_parameters, get rr count failed!\012"
	.ascii	"\000"
.LC56:
	.ascii	"rr cnt: %d\012\000"
.LC57:
	.ascii	"_get_read_retry_parameters, get rr reg count failed"
	.ascii	"!\012\000"
.LC58:
	.ascii	"rr reg cnt: %d\012\000"
.LC59:
	.ascii	"read retry value from otp error: rr_cnt %d rr_reg_c"
	.ascii	"nt %d!\012\000"
.LC60:
	.ascii	"_get_read_retry_parameters, get the %d-th byte of r"
	.ascii	"r table failed!\012\000"
.LC61:
	.ascii	"%02x \000"
	.section	.rodata.m1_get_rr_value_otp_hynix.str1.1,"aMS",%progbits,1
.LC65:
	.ascii	"otp_info_hynix_16nm : allocate memory fail\012\000"
.LC66:
	.ascii	"m1 CH %d chip %d get otp info failed!\012\000"
.LC67:
	.ascii	"m1 CH %d chip %d get read retry cfg from otp info f"
	.ascii	"ailed!\012\000"
	.section	.rodata.m1_getotpparam.str1.1,"aMS",%progbits,1
.LC68:
	.ascii	"_vender_get_param_otp_hynix time %d!\012\000"
	.section	.rodata.m1_init_readretry_param.str1.1,"aMS",%progbits,1
.LC69:
	.ascii	"[PHY_GetDefaultParam]:data_ff_buf malloc fail\012\000"
.LC70:
	.ascii	"nctrl: %d, chip %d, block %d, page 0, oob: 0x%x, 0x"
	.ascii	"%x, 0x%x, 0x%x\012\000"
.LC71:
	.ascii	"otp data check error!\012\000"
.LC72:
	.ascii	"[PHY_DBG] ch %d, can't get right otp value from nan"
	.ascii	"d otp blocks, then use otp command\012\000"
.LC73:
	.ascii	"0x%x \000"
.LC74:
	.ascii	"erase chip %d, block %d error\012\000"
.LC75:
	.ascii	"write chip %d, block %d, page %d error\012\000"
.LC76:
	.ascii	"[PHY_DBG] repair ch %d chip %d otp value end\012\000"
.LC77:
	.ascii	"find good otp value in ch: %d, chip %d, block %d \012"
	.ascii	"\000"
.LC78:
	.ascii	"ch %d, chip %d Read Retry value Table from nand otp"
	.ascii	" block:\012\000"
	.section	.rodata.m1_major_check_byte.str1.1,"aMS",%progbits,1
.LC52:
	.ascii	"_major_check_byte, wrong input para, level %d, len "
	.ascii	"%d\012\000"
.LC53:
	.ascii	"%d:  '1'-'0' : %d - %d\012\000"
.LC54:
	.ascii	"get bit %d failed!\012\000"
	.section	.rodata.m1_read_otp_info_hynix.str1.1,"aMS",%progbits,1
.LC62:
	.ascii	"start get read retry param from: ce %d, rb %d...\012"
	.ascii	"\000"
.LC63:
	.ascii	"invalid buffer for otp info!\012\000"
.LC64:
	.ascii	"hynix OTP RegCount value error: 0x%x, 0x%x \012\000"
	.section	.rodata.m1_read_page_end.str1.1,"aMS",%progbits,1
.LC80:
	.ascii	"m1 retry!\012\000"
.LC81:
	.ascii	"m1 ReadRetry ok! ch =%d, chip = %d  block = %d, pag"
	.ascii	"e = %d, RetryCount = %d  \012\000"
	.section	.rodata.m1_readretry_exit.str1.1,"aMS",%progbits,1
.LC79:
	.ascii	"m1_readretry_exit. \012\000"
	.section	.rodata.m1_special_exit.str1.1,"aMS",%progbits,1
.LC84:
	.ascii	" m1_special_exit \012\000"
	.section	.rodata.m1_special_init.str1.1,"aMS",%progbits,1
.LC82:
	.ascii	" m1_special_init m1_read_retry_mode: %d m1_read_ret"
	.ascii	"ry_cycle:%d m1_read_retry_reg_num:%d\012\000"
.LC83:
	.ascii	" m1_special_init error m1_read_retry_mode: %d m1_re"
	.ascii	"ad_retry_cycle:%d m1_read_retry_reg_num:%d\012\000"
	.section	.rodata.m1_write_page_FF.str1.1,"aMS",%progbits,1
.LC50:
	.ascii	"fatal err -0, wrong input parameter, ch: %d  chip: "
	.ascii	"%d/%d  block: %d/%d \012\000"
.LC51:
	.ascii	"m1 write page FF wrong\012\000"
	.section	.rodata.m2_getotpparam.str1.1,"aMS",%progbits,1
.LC96:
	.ascii	"_vender_get_param_otp_hynix error!\012\000"
	.section	.rodata.m2_init_readretry_param.str1.1,"aMS",%progbits,1
.LC97:
	.ascii	"m2 get default param error !\012\000"
.LC98:
	.ascii	"[PHY_DBG] repair ch %d otp value end\012\000"
	.section	.rodata.m2_lsb_exit.str1.1,"aMS",%progbits,1
.LC93:
	.ascii	"m2_lsb_exit\012\000"
	.section	.rodata.m2_read_page_end.str1.1,"aMS",%progbits,1
.LC153:
	.ascii	"m2 retry!\012\000"
.LC154:
	.ascii	"sdata:0x%x %x %x %x \000"
.LC155:
	.ascii	"m2 ReadRetry ok! ch =%d, chip = %d  block = %d, pag"
	.ascii	"e = %d, RetryCount = %d  \012\000"
	.section	.rodata.m2_readretry_exit.str1.1,"aMS",%progbits,1
.LC99:
	.ascii	"m2_readretry_exit. \012\000"
	.section	.rodata.m2_set_readretry.str1.1,"aMS",%progbits,1
.LC100:
	.ascii	"retry para:\000"
	.section	.rodata.m2_special_exit.str1.1,"aMS",%progbits,1
.LC103:
	.ascii	" m2_special_exit \012\000"
	.section	.rodata.m2_special_init.str1.1,"aMS",%progbits,1
.LC101:
	.ascii	" m2_special_init m2_read_retry_mode: %d m2_read_ret"
	.ascii	"ry_cycle:%d m2_read_retry_reg_num:%d\012\000"
.LC102:
	.ascii	" m2_special_init error m2_read_retry_mode: %d m2_re"
	.ascii	"ad_retry_cycle:%d m2_read_retry_reg_num:%d\012\000"
	.section	.rodata.m2_vender_get_param_otp_hynix.str1.1,"aMS",%progbits,1
.LC94:
	.ascii	"otp copy failed!\012\000"
.LC95:
	.ascii	"otp copy %d is ok!\012\000"
	.section	.rodata.m3_get_default_param.str1.1,"aMS",%progbits,1
.LC27:
	.ascii	"m3_get_default_param: ch: %d, chip: %d, value: 0x%x"
	.ascii	" 0x%x 0x%x 0x%x \012\000"
	.section	.rodata.m3_lsb_exit.str1.1,"aMS",%progbits,1
.LC23:
	.ascii	"m3_lsb_exit\012\000"
	.section	.rodata.m3_read_page_end.str1.1,"aMS",%progbits,1
.LC32:
	.ascii	"m0 retry!\012\000"
.LC33:
	.ascii	"m3 ReadRetry ok! ch =%d, chip = %d  block = %d, pag"
	.ascii	"e = %d, RetryCount = %d  \012\000"
	.section	.rodata.m3_readretry_exit.str1.1,"aMS",%progbits,1
.LC29:
	.ascii	"m3_readretry_exit. \012\000"
	.section	.rodata.m3_readretry_init.str1.1,"aMS",%progbits,1
.LC28:
	.ascii	"NFC_ReadRetryInit, unknown read retry mode 0x%x\012"
	.ascii	"\000"
	.section	.rodata.m3_set_readretry.str1.1,"aMS",%progbits,1
.LC30:
	.ascii	"m3 retry param:\000"
.LC31:
	.ascii	" %x \000"
	.section	.rodata.m3_setdefaultparam.str1.1,"aMS",%progbits,1
.LC24:
	.ascii	"set retry default value: \000"
.LC25:
	.ascii	" %x\000"
.LC26:
	.ascii	"\012\000"
	.section	.rodata.m3_special_exit.str1.1,"aMS",%progbits,1
.LC36:
	.ascii	" m3_special_exit \012\000"
	.section	.rodata.m3_special_init.str1.1,"aMS",%progbits,1
.LC34:
	.ascii	" m3_special_init m3_read_retry_mode: %d m3_read_ret"
	.ascii	"ry_cycle:%d m3_read_retry_reg_num:%d\012\000"
.LC35:
	.ascii	" m3_special_init error m3_read_retry_mode: %d m3_re"
	.ascii	"ad_retry_cycle:%d m3_read_retry_reg_num:%d\012\000"
	.section	.rodata.m4_read_page_end.str1.1,"aMS",%progbits,1
.LC88:
	.ascii	"m4 retry!\012\000"
.LC89:
	.ascii	"m4 ReadRetry ok! ch =%d, chip = %d  block = %d, pag"
	.ascii	"e = %d, RetryCount = %d  \012\000"
	.section	.rodata.m4_readretry_init.str1.1,"aMS",%progbits,1
.LC86:
	.ascii	"micron read retry cannot support!\012\000"
.LC87:
	.ascii	"micron read retry conut: %d !\012\000"
	.section	.rodata.m4_special_exit.str1.1,"aMS",%progbits,1
.LC92:
	.ascii	" m4_special_exit \012\000"
	.section	.rodata.m4_special_init.str1.1,"aMS",%progbits,1
.LC90:
	.ascii	" m4_special_init m4_read_retry_mode:%d m4_read_retr"
	.ascii	"y_cycle:%d \012\000"
.LC91:
	.ascii	" m4_special_init error m4_read_retry_mode:%d m4_rea"
	.ascii	"d_retry_cycle:%d \012\000"
	.section	.rodata.m5_read_page_end.str1.1,"aMS",%progbits,1
.LC16:
	.ascii	"m5 retry!\012\000"
.LC17:
	.ascii	"m5 ReadRetry ok! ch =%d, chip = %d  block = %d, pag"
	.ascii	"e = %d, RetryCount = %d  \012\000"
	.section	.rodata.m5_readretry_init.str1.1,"aMS",%progbits,1
.LC13:
	.ascii	"samsung read retry conut: %d !\012\000"
	.section	.rodata.m5_set_readretry.str1.1,"aMS",%progbits,1
.LC15:
	.ascii	"m5 set readretry error ! %x \012\000"
	.section	.rodata.m5_special_exit.str1.1,"aMS",%progbits,1
.LC20:
	.ascii	" m5_special_exit \012\000"
	.section	.rodata.m5_special_init.str1.1,"aMS",%progbits,1
.LC18:
	.ascii	" m5_special_init m5_read_retry_mode:%d m5_read_retr"
	.ascii	"y_cycle :%d \012\000"
.LC19:
	.ascii	" m5_special_init error m5_read_retry_mode:%d m5_rea"
	.ascii	"d_retry_cycle :%d \012\000"
	.section	.rodata.m5_vender_set_param.str1.1,"aMS",%progbits,1
.LC14:
	.ascii	"rr value %x %x %x %x!\012\000"
	.section	.rodata.m6_read_page_end.str1.1,"aMS",%progbits,1
.LC45:
	.ascii	"m6 retry!\012\000"
.LC46:
	.ascii	"m6 ReadRetry ok! ch =%d, chip = %d  block = %d, pag"
	.ascii	"e = %d, RetryCount = %d  \012\000"
	.section	.rodata.m6_readretry_init.str1.1,"aMS",%progbits,1
.LC42:
	.ascii	"toshiba read retry conut: %d !\012\000"
	.section	.rodata.m6_special_exit.str1.1,"aMS",%progbits,1
.LC49:
	.ascii	" m6_special_exit \012\000"
	.section	.rodata.m6_special_init.str1.1,"aMS",%progbits,1
.LC47:
	.ascii	" m6_special_init m6_read_retry_mode :%d m6_read_ret"
	.ascii	"ry_cycle :%d m6_read_retry_reg_cnt :%d \012\000"
.LC48:
	.ascii	" m6_special_init error m6_read_retry_mode :%d m6_re"
	.ascii	"ad_retry_cycle :%d m6_read_retry_reg_cnt :%d \012\000"
	.section	.rodata.m6_vender_pre_condition.str1.1,"aMS",%progbits,1
.LC43:
	.ascii	"toshiba vender_pre_condition error!\012\000"
	.section	.rodata.m6_vender_set_param.str1.1,"aMS",%progbits,1
.LC44:
	.ascii	"m6 rr value 3 %x %x %x %x!\012\000"
	.section	.rodata.m7_activation_seq.str1.1,"aMS",%progbits,1
.LC37:
	.ascii	"m7 activation seq fail 0x%x\012\000"
	.section	.rodata.m7_read_page_end.str1.1,"aMS",%progbits,1
.LC150:
	.ascii	"m7 retry!\012\000"
.LC151:
	.ascii	"retry spare all 0xff! ch =%d, chip = %d  block = %d"
	.ascii	", page = %d, RetryCount = %d  \012\000"
.LC152:
	.ascii	"m7 ReadRetry ok! ch =%d, chip = %d  block = %d, pag"
	.ascii	"e = %d, RetryCount = %d  \012\000"
	.section	.rodata.m7_readretry_init.str1.1,"aMS",%progbits,1
.LC38:
	.ascii	"sandisk read retry conut: %d !\012\000"
	.section	.rodata.m7_special_exit.str1.1,"aMS",%progbits,1
.LC41:
	.ascii	" m7_special_exit \012\000"
	.section	.rodata.m7_special_init.str1.1,"aMS",%progbits,1
.LC39:
	.ascii	" m7_special_init m7_read_retry_mode:%d m7_read_retr"
	.ascii	"y_cycle :%d m7_read_retry_reg_cnt %d \012\000"
.LC40:
	.ascii	" m7_special_init error m7_read_retry_mode:%d m7_rea"
	.ascii	"d_retry_cycle :%d m7_read_retry_reg_cnt %d \012\000"
	.section	.rodata.m9_read_page_end.str1.1,"aMS",%progbits,1
.LC156:
	.ascii	"m9 retry!\012\000"
.LC157:
	.ascii	"m9 set readretry error\012\000"
.LC158:
	.ascii	"m9 ReadRetry ok! ch =%d, chip = %d  block = %d, pag"
	.ascii	"e = %d, RetryCount = %d  \012\000"
.LC159:
	.ascii	"m9 ReadRetry ok with ecc limit! ch =%d, chip = %d  "
	.ascii	"block = %d, page = %d, RetryCount = %d  \012\000"
	.section	.rodata.m9_readretry_init.str1.1,"aMS",%progbits,1
.LC104:
	.ascii	"m9 retry mode err : %d !\012\000"
.LC105:
	.ascii	"sandisk A19 read retry conut: %d !\012\000"
	.section	.rodata.m9_special_exit.str1.1,"aMS",%progbits,1
.LC108:
	.ascii	"_setup_nand_toggle_vendor_specific_feature() failed"
	.ascii	"!\012\000"
.LC109:
	.ascii	" m9_special_exit \012\000"
	.section	.rodata.m9_special_init.str1.1,"aMS",%progbits,1
.LC106:
	.ascii	" m9_special_init m9_read_retry_mode:%d m9_read_retr"
	.ascii	"y_cycle :%d m9_read_retry_reg_cnt %d \012\000"
.LC107:
	.ascii	" m9_special_init error m9_read_retry_mode:%d m9_rea"
	.ascii	"d_retry_cycle :%d m9_read_retry_reg_cnt %d \012\000"
	.section	.rodata.malloc_logic_page_map.str1.1,"aMS",%progbits,1
.LC171:
	.ascii	"[NE] zone_param_init malloc_logic_page_map error %d"
	.ascii	"\012\000"
	.section	.rodata.micron_intel_set_feature.str1.1,"aMS",%progbits,1
.LC85:
	.ascii	"set feature(addr %d) p0: %d readretry Configuration"
	.ascii	" failed!\012\000"
	.section	.rodata.mod,"a",%progbits
	.set	.LANCHOR89,. + 0
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
.LC193:
	.ascii	"[NE]nand_cache_read error  NULL!\012\000"
.LC194:
	.ascii	"[NE]error1 node %d,%d,%d!\012\000"
	.section	.rodata.nand_cache_write.str1.1,"aMS",%progbits,1
.LC208:
	.ascii	"[NE]nand_cache_write error  NULL!\012\000"
.LC209:
	.ascii	"[NE]error1 bitmap %x!\012\000"
.LC210:
	.ascii	"[NE]ECC_LIMIT happened 12! page:%d!\012\000"
.LC211:
	.ascii	"[NE]error1 flush write cache to nand\012\000"
.LC212:
	.ascii	"[NE]error2 flush write cache to nand\012\000"
.LC213:
	.ascii	"[NE]ECC_LIMIT happened 11! page:%d!\012\000"
	.section	.rodata.nand_check_table.str1.1,"aMS",%progbits,1
.LC439:
	.ascii	"[ND]==========================\012\000"
	.section	.rodata.nand_clean_zone_table2.str1.1,"aMS",%progbits,1
.LC137:
	.ascii	"zoneNO:%d total_pages: %d\012\000"
.LC138:
	.ascii	"MEMSET bytes: %d\012\000"
	.section	.rodata.nand_crc16_table,"a",%progbits
	.align	1
	.set	.LANCHOR91,. + 0
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
.LC131:
	.ascii	"[NE]%s:malloc 4096 fail!\012\000"
.LC132:
	.ascii	"[NE]%x %x %x %x %x \000"
.LC133:
	.ascii	"%x %x %x %x %x!!\012\000"
.LC134:
	.ascii	"buf:%x %x %x %x %x \000"
.LC135:
	.ascii	"%x %x %x %x %x %x!!\012\000"
	.section	.rodata.nand_dbg_zone_phy_write.str1.1,"aMS",%progbits,1
.LC136:
	.ascii	"[NE]%s:malloc 8192 fail!\012\000"
	.section	.rodata.nand_discard.str1.1,"aMS",%progbits,1
.LC215:
	.ascii	"[NE]nand_discard paramter error!\012\000"
	.section	.rodata.nand_find_zone_table.str1.1,"aMS",%progbits,1
.LC435:
	.ascii	"build_zone_list start\012\000"
.LC436:
	.ascii	"nftl_cache_init start\012\000"
.LC437:
	.ascii	"=================================2================="
	.ascii	"======================\012\000"
.LC438:
	.ascii	"nand_find_zone_table end\012\000"
	.section	.rodata.nand_op.str1.1,"aMS",%progbits,1
.LC195:
	.ascii	"[NE]paramter error!\012\000"
	.section	.rodata.nand_power_off_ecc_error.str1.1,"aMS",%progbits,1
.LC297:
	.ascii	"exception case bad block\012\000"
.LC298:
	.ascii	"exception case wbc\012\000"
.LC299:
	.ascii	"exception case gd\012\000"
.LC300:
	.ascii	"%s %d, flag = %d\012\000"
.LC301:
	.ascii	"OOB :\000"
.LC302:
	.ascii	"%.2x \000"
	.section	.rodata.nand_sector_cache_read.str1.1,"aMS",%progbits,1
.LC197:
	.ascii	"[NE]nand_sector_cache_read 1!\012\000"
	.section	.rodata.nand_sector_cache_write.str1.1,"aMS",%progbits,1
.LC200:
	.ascii	"[NE]nand_sector_cache_write 1!\012\000"
.LC201:
	.ascii	"[NE]nand_sector_cache_write 2!\012\000"
	.section	.rodata.nand_write_data_page.str1.1,"aMS",%progbits,1
.LC401:
	.ascii	"[NE]do after write error1!\012\000"
.LC402:
	.ascii	"[NE]do_before_write error!\012\000"
.LC403:
	.ascii	"[NE]write data to page error!\012\000"
.LC404:
	.ascii	"[NE]do after write error2!\012\000"
	.section	.rodata.new_block_init_for_write.str1.1,"aMS",%progbits,1
.LC332:
	.ascii	"[NE]new_block_init_for_write erase error!\012\000"
	.section	.rodata.nftl_add.str1.1,"aMS",%progbits,1
.LC453:
	.ascii	"[NE]nftl_initialize failed\012\000"
.LC454:
	.ascii	"[ND]nftl_add ok\012\000"
	.section	.rodata.nftl_build_all.str1.1,"aMS",%progbits,1
.LC455:
	.ascii	"[NE]nftl_build_all error!\012\000"
	.section	.rodata.nftl_initialize.str1.1,"aMS",%progbits,1
.LC446:
	.ascii	"000\000"
.LC447:
	.ascii	"7\000"
.LC448:
	.ascii	"3\000"
.LC449:
	.ascii	"sun8iw7 nftl version %s.%s.%s  %s-%s\012\000"
.LC450:
	.ascii	"09:56\000"
.LC451:
	.ascii	"2021.05.24\000"
.LC452:
	.ascii	"[ND]max_erase_times = %i\012\000"
	.section	.rodata.nftl_malloc.str1.1,"aMS",%progbits,1
.LC7:
	.ascii	"[NE]malloc size too large %d!\012\000"
	.section	.rodata.nftl_read.str1.1,"aMS",%progbits,1
.LC6:
	.ascii	"[NE]parameter error %d,%d !\012\000"
	.section	.rodata.nftl_start.str1.1,"aMS",%progbits,1
.LC440:
	.ascii	"[NE]not enough blocks %d!\012\000"
.LC441:
	.ascii	"[ND]nftl start:%d,%d\012\000"
.LC442:
	.ascii	"[NE] zone_param_init fail!\012\000"
.LC443:
	.ascii	"[NE] build_zone_list_first fail!\012\000"
.LC444:
	.ascii	"[NE] build_zone_list fail!\012\000"
.LC445:
	.ascii	"try again\012\000"
	.section	.rodata.no_cross_talk_rebuild_current_block.str1.1,"aMS",%progbits,1
.LC343:
	.ascii	"[NE]cross_talk_rebuild_current_block fail 1\012\000"
.LC344:
	.ascii	"chking %d pages %dms\012\000"
.LC345:
	.ascii	"[ND]nctrcb1 %d,%d,%d!\012\000"
.LC346:
	.ascii	"[ND]no_cross_talk_rebuild_current_block fail 2\012\000"
.LC347:
	.ascii	"[ND]nctrcb2 %d,%d,%d,%d,%d,%d!\012\000"
.LC348:
	.ascii	"[ND]no_cross_talk_rebuild_current_block fail 4\012\000"
.LC349:
	.ascii	"[NE]no_cross_talk_rebuild_current_block erase block"
	.ascii	" fail %d!\012\000"
.LC350:
	.ascii	"copying %d  pages %dms\012\000"
	.section	.rodata.para0,"a",%progbits
	.align	1
	.set	.LANCHOR15,. + 0
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
	.set	.LANCHOR17,. + 0
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
.LC121:
	.ascii	"[NE]invalid_page_count more than pages per blk 0 %d"
	.ascii	" %d!!\012\000"
.LC122:
	.ascii	"[NE]invalid_page_count more than pages per blk 1!!\012"
	.ascii	"\000"
	.section	.rodata.power_on_gc.str1.1,"aMS",%progbits,1
.LC428:
	.ascii	"[NE]not power on gc %d!\012\000"
.LC429:
	.ascii	"[NE]emergency power on gc %d !\012\000"
.LC430:
	.ascii	"[NE]power on gc %d!\012\000"
	.section	.rodata.print_all_nftl_blk.str1.1,"aMS",%progbits,1
.LC5:
	.ascii	"[ND]cap: 0x%08x.\012\000"
	.section	.rodata.print_block_count_list.str1.1,"aMS",%progbits,1
.LC116:
	.ascii	"[NE]block_used_count list: \012\000"
.LC117:
	.ascii	"[NE]block NO: %d \000"
.LC118:
	.ascii	"[NE]block_used_count: %d \012\000"
	.section	.rodata.print_block_invalid_list.str1.1,"aMS",%progbits,1
.LC114:
	.ascii	"[NE]invalid_page_count list: \012\000"
.LC115:
	.ascii	"count:%4d; invalid:%3d;\000"
	.section	.rodata.print_free_list.str1.1,"aMS",%progbits,1
.LC110:
	.ascii	"[NE]free list: \012\000"
.LC111:
	.ascii	"[NE]block NO:%4d; erase:%3d; \000"
.LC112:
	.ascii	"count:%4d; \000"
.LC113:
	.ascii	"info:0x%x\012\000"
	.section	.rodata.print_logic_page_map.str1.1,"aMS",%progbits,1
.LC217:
	.ascii	"[NE]page_map NULL!!\012\000"
.LC218:
	.ascii	"%4d:<%4d %3d><%d>\012\000"
	.section	.rodata.print_nftl_zone.str1.1,"aMS",%progbits,1
.LC243:
	.ascii	"[ND]nand3.x \012\000"
.LC244:
	.ascii	"[ND]zone->nand_chip->blk_per_chip: %d \012\000"
.LC245:
	.ascii	"[ND]zone->nand_chip->bytes_per_page: %d \012\000"
.LC246:
	.ascii	"[ND]zone->nand_chip->pages_per_blk: %d \012\000"
.LC247:
	.ascii	"[ND]zone->nand_chip->max_erase_times: %d \012\000"
.LC248:
	.ascii	"[ND]zone->nand_chip->support_read_reclaim: %d \012\000"
.LC249:
	.ascii	"[ND]zone->test: %d \012\000"
.LC250:
	.ascii	"[ND]zone->zone_no: %d \012\000"
.LC251:
	.ascii	"[ND]zone->zone_attr: %d \012\000"
.LC252:
	.ascii	"[ND]zone->blocks: %d \012\000"
.LC253:
	.ascii	"[ND]zone->bad_block: %d \012\000"
.LC254:
	.ascii	"[ND]zone->logic_cap_in_sects: %d \012\000"
.LC255:
	.ascii	"[ND]zone->backup_cap_in_sects: %d \012\000"
.LC256:
	.ascii	"[ND]zone->free_block_num: %d \012\000"
.LC257:
	.ascii	"[ND]zone->gc_strategy.start_gc_free_blocks: %d \012"
	.ascii	"\000"
.LC258:
	.ascii	"[ND]zone->gc_strategy.stop_gc_free_blocks: %d \012\000"
.LC259:
	.ascii	"[ND]zone->gc_strategy.gc_page: %d \012\000"
.LC260:
	.ascii	"[ND]zone->gc_strategy.process: %d \012\000"
.LC261:
	.ascii	"[ND]zone->prio_gc.prio_type : %d \012\000"
.LC262:
	.ascii	"[ND]zone->zone_start_phy_block->phy_block.Block_NO:"
	.ascii	" %d \012\000"
.LC263:
	.ascii	"[ND]zone->zone_end_phy_block->phy_block.Block_NO: %"
	.ascii	"d \012\000"
.LC264:
	.ascii	"[ND]zone->zone_phy_page_map_for_gc: %x \012\000"
.LC265:
	.ascii	"[ND]zone->current_block.user_info: %x \012\000"
.LC266:
	.ascii	"[ND]zone->current_block.block_info: %x \012\000"
.LC267:
	.ascii	"[ND]zone->current_block.block_info->phy_block.Block"
	.ascii	"_NO: %d \012\000"
.LC268:
	.ascii	"[ND]zone->current_block.page_used: %d \012\000"
.LC269:
	.ascii	"[ND]zone->current_block.block_info->block_used_coun"
	.ascii	"t: %d \012\000"
.LC270:
	.ascii	"[NE] zone->recv_free_block: %d %d!!\012\000"
.LC271:
	.ascii	"[ND]zone->assist_block.user_info: %x \012\000"
.LC272:
	.ascii	"[ND]zone->assist_block.block_info: %x \012\000"
.LC273:
	.ascii	"[ND]zone->assist_block.block_info->phy_block.Block_"
	.ascii	"NO: %d \012\000"
.LC274:
	.ascii	"[ND]zone->assist_block.page_used: %d \012\000"
.LC275:
	.ascii	"[ND]zone->assist_block.block_info->block_used_count"
	.ascii	": %d \012\000"
.LC276:
	.ascii	"[ND]zone->read_reclaim_complete: %x \012\000"
.LC277:
	.ascii	"[ND]zone->temp_page_buf: %x \012\000"
.LC278:
	.ascii	"[ND]zone->max_erase_num: %d \012\000"
.LC279:
	.ascii	"[ND]zone->cache.cache_totals: %x \012\000"
.LC280:
	.ascii	"[ND]zone->wl_gc_count: %d \012\000"
.LC281:
	.ascii	"[ND]zone->wl_gc_page_num: %d \012\000"
.LC282:
	.ascii	"[ND]zone->cfg->nftl_dont_use_cache: %x \012\000"
.LC283:
	.ascii	"[ND]zone->cfg->nftl_use_cache_sort: %x \012\000"
.LC284:
	.ascii	"[ND]zone->cfg->nftl_support_gc_read_reclaim: %x \012"
	.ascii	"\000"
.LC285:
	.ascii	"[ND]zone->cfg->nftl_support_wear_leveling: %x \012\000"
.LC286:
	.ascii	"[ND]zone->cfg->nftl_need_erase: %x \012\000"
.LC287:
	.ascii	"[ND]zone->cfg->nftl_min_free_block_num: %x \012\000"
.LC288:
	.ascii	"[ND]zone->cfg->nftl_gc_threshold_free_block_num: %x"
	.ascii	" \012\000"
.LC289:
	.ascii	"[ND]zone->cfg->nftl_min_free_block: %x \012\000"
.LC290:
	.ascii	"[ND]zone->cfg->nftl_gc_threshold_ratio_numerator: %"
	.ascii	"x \012\000"
.LC291:
	.ascii	"[ND]zone->cfg->nftl_gc_threshold_ratio_denominator:"
	.ascii	" %x \012\000"
.LC292:
	.ascii	"[ND]zone->cfg->nftl_max_cache_num: %d \012\000"
.LC293:
	.ascii	"[ND]zone->cfg->nftl_max_cache_write_num: %d \012\000"
.LC294:
	.ascii	"[ND]zone->cfg->nftl_cross_talk: %x \012\000"
.LC295:
	.ascii	"[ND]zone->cfg->nftl_read_claim_interval: %d \012\000"
	.section	.rodata.print_smart.str1.1,"aMS",%progbits,1
.LC219:
	.ascii	"[NE]smart->version:%x;\012\000"
.LC220:
	.ascii	"[NE]smart->total_recv_read_sectors:%lld;\012\000"
.LC221:
	.ascii	"[NE]smart->total_recv_write_sectors:%lld;\012\000"
.LC222:
	.ascii	"[NE]smart->total_real_read_pages:%lld;\012\000"
.LC223:
	.ascii	"[NE]smart->total_real_write_pages:%lld;\012\000"
.LC224:
	.ascii	"[NE]smart->total_recv_discard_sectors:%lld;\012\000"
.LC225:
	.ascii	"[NE]smart->total_real_discard_sectors:%lld;\012\000"
.LC226:
	.ascii	"[NE]smart->total_recv_read_claim_pages:%d;\012\000"
.LC227:
	.ascii	"[NE]smart->total_gc_times:%d;\012\000"
.LC228:
	.ascii	"[NE]smart->total_gc_pages:%d;\012\000"
.LC229:
	.ascii	"[NE]smart->total_wl_times:%d;\012\000"
.LC230:
	.ascii	"[NE]smart->total_wl_gc_times:%d;\012\000"
.LC231:
	.ascii	"[NE]smart->total_real_read_error_pages:%d;\012\000"
.LC232:
	.ascii	"[NE]smart->total_real_write_error_pages:%d;\012\000"
.LC233:
	.ascii	"[NE]smart->total_normal_power_cycles:%x;\012\000"
.LC234:
	.ascii	"[NE]smart->total_unusual_power_cycles:%x;\012\000"
.LC235:
	.ascii	"[NE]smart->max_block_erase_times:%d;\012\000"
.LC236:
	.ascii	"[NE]smart->min_block_erase_times:%d;\012\000"
.LC237:
	.ascii	"[NE]smart->max_block_used_counter:%d;\012\000"
.LC238:
	.ascii	"[NE]smart->min_block_used_counter:%d;\012\000"
.LC239:
	.ascii	"[NE]smart->read_reclaim_utc:%d;\012\000"
.LC240:
	.ascii	"[NE]smart->cross_talk_times:%d;\012\000"
.LC241:
	.ascii	"[NE]smart->read_reclaim_zone_no:%d;\012\000"
.LC242:
	.ascii	"[NE]smart->read_reclaim_page_no:%d;\012\000"
	.section	.rodata.put_phy_block_to_free_list.str1.1,"aMS",%progbits,1
.LC119:
	.ascii	"[NE]add free_list error !\012\000"
	.section	.rodata.put_phy_block_to_invalid_page_list.str1.1,"aMS",%progbits,1
.LC120:
	.ascii	"[NE]add invalid_list error !\012\000"
	.section	.rodata.recover_block_phy_page_mapping.str1.1,"aMS",%progbits,1
.LC310:
	.ascii	"[NE]ret:%d read fail %d %d,read one more time!!\012"
	.ascii	"\000"
.LC311:
	.ascii	"[NE]ret:%d read fail %d %d,%d times,don't read more"
	.ascii	"!!\012\000"
.LC312:
	.ascii	"[NE]slc power off ecc error 3!!\012\000"
.LC313:
	.ascii	"[NE]recover_block_phy_page_mapping error page %d %d"
	.ascii	"!!\012\000"
	.section	.rodata.recover_logic_page_mapping.str1.1,"aMS",%progbits,1
.LC167:
	.ascii	"[NE]logic_page_map NULL!!\012\000"
.LC168:
	.ascii	"[NE]invalid_page_count more than pages_per_blk! :%d"
	.ascii	"\012\000"
.LC169:
	.ascii	"[NE]inv P b:%d p:%d,lp %x!!\012\000"
.LC170:
	.ascii	"[NE]build invalid_page_count more than pages_per_bl"
	.ascii	"k!!\012\000"
	.section	.rodata.recover_phy_page_mapping.str1.1,"aMS",%progbits,1
.LC303:
	.ascii	" rppm nouse page %d!!\012\000"
.LC304:
	.ascii	"[NE]slc power off ecc error 1!!\012\000"
.LC305:
	.ascii	"[NE]invalid logic_page block:%d page:%d,logic_page "
	.ascii	"0x%x!!\012\000"
.LC306:
	.ascii	" rppm invalid page %d!!\012\000"
.LC307:
	.ascii	"[NE]power_down page %d!!\012\000"
.LC308:
	.ascii	"[NE]recover_phy_page_mapping unkown page data %d!!\012"
	.ascii	"\000"
.LC309:
	.ascii	"[NE]recover_phy_page_mapping ecc error block:%d;pag"
	.ascii	"e:%d!!\012\000"
	.section	.rodata.second_scan_all_blocks.str1.1,"aMS",%progbits,1
.LC351:
	.ascii	"[NE]current used block :%d\012\000"
.LC352:
	.ascii	"[NE]NAND_EVENT: 2 current used block found %d,%d!\012"
	.ascii	"\000"
.LC353:
	.ascii	"[NE]no crosstalk:2 block used count diff %d,%d %d,%"
	.ascii	"d !\012\000"
.LC354:
	.ascii	"[NE]NAND_EVENT: muti current used block found1 %d %"
	.ascii	"d!\012\000"
.LC355:
	.ascii	"[NE]NAND_EVENT: muti current used block found2 %d %"
	.ascii	"d!\012\000"
.LC356:
	.ascii	"[NE]NAND_EVENT: muti current used block found3 %d %"
	.ascii	"d!\012\000"
.LC357:
	.ascii	"[NE]NAND_EVENT: last page spare data %x,%x,%x,%x,%x"
	.ascii	",%x,%x!!\012\000"
.LC358:
	.ascii	"[NE]last page unkown ecc ok %d,%d!!\012\000"
.LC359:
	.ascii	"[NE]last page ecc error %d,%d!!\012\000"
.LC360:
	.ascii	" iter took %dms\012\000"
.LC361:
	.ascii	"[ND]something strange %d,%d!!\012\000"
.LC362:
	.ascii	"[ND]all block full!!\012\000"
.LC363:
	.ascii	"[ND]corss talk rebuild 0 %d %d!!\012\000"
.LC364:
	.ascii	"nctrcb took %dms\012\000"
.LC365:
	.ascii	"[NE]muti current used block 1st %d %d!\012\000"
.LC366:
	.ascii	"[NE]muti current used block 2nd %d %d!\012\000"
.LC367:
	.ascii	"[NE]something is error here 7!\012\000"
.LC368:
	.ascii	"[NE]p_phy_block_info_last %d %d!\012\000"
.LC369:
	.ascii	"[NE]muti current used block 1 %d %d!\012\000"
.LC370:
	.ascii	"[NE]muti current used block 2 %d %d!\012\000"
.LC371:
	.ascii	"%s %d : \012\000"
.LC372:
	.ascii	"[ND]recover %d %d\012\000"
.LC373:
	.ascii	"[NE]error!!\012\000"
.LC374:
	.ascii	"[NE]new_block_init_for_write error here!\012\000"
.LC375:
	.ascii	"[NE]something is error here 10!\012\000"
.LC376:
	.ascii	"[NE]something is error here 11!\012\000"
.LC377:
	.ascii	"[NE]something is error here 12 %d,%d!\012\000"
.LC378:
	.ascii	"[NE]something is error here 17 %d,%d!\012\000"
.LC379:
	.ascii	"[NE]something is error here 16!\012\000"
.LC380:
	.ascii	"[ND]get a new free block\012\000"
.LC381:
	.ascii	"[NE]no free block to use!\012\000"
.LC382:
	.ascii	"gcub took %dms\012\000"
.LC383:
	.ascii	"[NE]get_current_used_block fail!\012\000"
.LC384:
	.ascii	"[ND]free block nums is %d !\012\000"
.LC385:
	.ascii	" ssab %dms\012\000"
	.section	.rodata.torture_block_and_mark.str1.1,"aMS",%progbits,1
.LC143:
	.ascii	"%s write b@%d p@%d err\012\000"
.LC144:
	.ascii	"%s read b@%d p@%d err\012\000"
.LC145:
	.ascii	"%s b@%d p@%d patt %02x checking main data fail\012\000"
.LC146:
	.ascii	"%s b@%d p@%d patt %02x checking spare data fail\012"
	.ascii	"\000"
	.section	.rodata.write_data_to_nand.str1.1,"aMS",%progbits,1
.LC199:
	.ascii	"[NE]%s:malloc 512 fail!\012\000"
	.section	.rodata.write_data_to_page.str1.1,"aMS",%progbits,1
.LC329:
	.ascii	"[NE]write logic data to current block error\012\000"
	.section	.rodata.write_logic_data_to_current_block.str1.1,"aMS",%progbits,1
.LC326:
	.ascii	"[NE]invalid_page_count more than pages_per_blk at w"
	.ascii	"rite logic data to current block:%d!!\012\000"
.LC327:
	.ascii	"[NE]write_phy_data_to_current_block error2\012\000"
.LC328:
	.ascii	"[NE]write phy data to current block error2\012\000"
	.section	.rodata.write_phy_data_to_current_block.str1.1,"aMS",%progbits,1
.LC322:
	.ascii	"[NE]last block can not used to logic page %d!!\012\000"
.LC323:
	.ascii	"[NE]last block can not used to power down page %d!!"
	.ascii	"\012\000"
.LC324:
	.ascii	"[NE]write_phy_data_to_current_block write error %d!"
	.ascii	"\012\000"
.LC325:
	.ascii	"[NE]do_write_error error\012\000"
	.section	.rodata.write_phy_page_map_to_current_block.str1.1,"aMS",%progbits,1
.LC397:
	.ascii	"[NE]write_phy_data_to_current_block error1\012\000"
	.section	.rodata.zone_param_init.str1.1,"aMS",%progbits,1
.LC172:
	.ascii	"[NE] zone_param_init error1\012\000"
.LC173:
	.ascii	"[NE] zone_param_init error2\012\000"
.LC174:
	.ascii	"[NE] zone_param_init error3\012\000"
.LC175:
	.ascii	"[NE] zone_param_init error4\012\000"
.LC176:
	.ascii	"[NE] zone_param_init error5\012\000"
	.ident	"GCC: (Linaro GCC 7.2-2017.11) 7.2.1 20171011"
	.section	.note.GNU-stack,"",%progbits
