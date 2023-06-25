/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/ce.h>
#include <memalign.h>
#include <sunxi_board.h>
#include "ss_op.h"

#define ALG_SHA256 (0x13)
#define ALG_SHA512 (0x15)
#define ALG_RSA (0x20)
#define ALG_MD5 (0x10)

/*check if task_queue size is cache_line align*/
#define STATIC_CHECK(condition) extern u8 checkFailAt##__LINE__[-!(condition)];
STATIC_CHECK(sizeof(task_queue) == ALIGN(sizeof(task_queue), CACHE_LINE_SIZE))

static u32 __aw_endian4(u32 data)
{
	u32 d1, d2, d3, d4;
	d1 = (data & 0xff) << 24;
	d2 = (data & 0xff00) << 8;
	d3 = (data & 0xff0000) >> 8;
	d4 = (data & 0xff000000) >> 24;

	return (d1 | d2 | d3 | d4);
}

#define PADDING_SHA 1
#define PADDING_MD5 0
static u32 __sha_padding(u32 data_size, u8 *text, u32 hash_mode)
{
	u32 i;
	u32 k, q;
	u32 size;
	u32 padding_buf[16];
	u8 *ptext;

	k = data_size / 64;
	q = data_size % 64;

	ptext = (u8 *)padding_buf;
	memset(padding_buf, 0, 16 * sizeof(u32));
	if (q == 0) {
		padding_buf[0] = 0x00000080;

		if (hash_mode) {
			padding_buf[14] = data_size >> 29;
			padding_buf[15] = data_size << 3;
			padding_buf[14] = __aw_endian4(padding_buf[14]);
			padding_buf[15] = __aw_endian4(padding_buf[15]);
		} else {
			padding_buf[14] = data_size << 3;
			padding_buf[15] = data_size >> 29;
		}

		for (i = 0; i < 64; i++) {
			text[k * 64 + i] = ptext[i];
		}
		size = (k + 1) * 64;
	} else if (q < 56) {
		for (i = 0; i < q; i++) {
			ptext[i] = text[k * 64 + i];
		}
		ptext[q] = 0x80;

		if (hash_mode) {
			padding_buf[14] = data_size >> 29;
			padding_buf[15] = data_size << 3;
			padding_buf[14] = __aw_endian4(padding_buf[14]);
			padding_buf[15] = __aw_endian4(padding_buf[15]);
		} else {
			padding_buf[14] = data_size << 3;
			padding_buf[15] = data_size >> 29;
		}

		for (i = 0; i < 64; i++) {
			text[k * 64 + i] = ptext[i];
		}
		size = (k + 1) * 64;
	} else {
		for (i = 0; i < q; i++) {
			ptext[i] = text[k * 64 + i];
		}
		ptext[q] = 0x80;
		for (i = 0; i < 64; i++) {
			text[k * 64 + i] = ptext[i];
		}

		//send last 512-bits text to SHA1/MD5
		for (i = 0; i < 16; i++) {
			padding_buf[i] = 0x0;
		}

		if (hash_mode) {
			padding_buf[14] = data_size >> 29;
			padding_buf[15] = data_size << 3;
			padding_buf[14] = __aw_endian4(padding_buf[14]);
			padding_buf[15] = __aw_endian4(padding_buf[15]);
		} else {
			padding_buf[14] = data_size << 3;
			padding_buf[15] = data_size >> 29;
		}

		for (i = 0; i < 64; i++) {
			text[(k + 1) * 64 + i] = ptext[i];
		}
		size = (k + 2) * 64;
	}

	return size;
}

static void __rsa_padding(u8 *dst_buf, u8 *src_buf, u32 data_len, u32 group_len)
{
	int i = 0;

	memset(dst_buf, 0, group_len);
	for (i = group_len - data_len; i < group_len; i++) {
		dst_buf[i] = src_buf[group_len - 1 - i];
	}
}

static void word_padding(u8 *dst_buf, u8 *src_buf, u32 size_byte)
{
	int i = 0;
	u32 *p_d = (u32 *)dst_buf;
	u32 *p_s = (u32 *)src_buf;
	u32 word_size = size_byte >> 2;

	memset(dst_buf, 0, size_byte);
	for (i = 0; i < word_size; i++) {
		p_d[i] = p_s[(word_size -1) - i];
	}
}

void sunxi_ss_open(void)
{
	ss_open();
}

void sunxi_ss_close(void)
{
	ss_close();
}

int sunxi_trng_gen(u8 *rng_buf, u32 rng_byte)
{
	task_queue task0 __aligned(64) = {0};
	/* sha256  2word, sha512 4word*/
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_sign, CACHE_LINE_SIZE);
	u32 align_shift = ss_get_addr_align();
	u32 word_len    = (rng_byte >> 2);

	memset(p_sign, 0, sizeof(p_sign));

	task0.task_id = CHANNEL_0;
	task0.common_ctl = (ALG_TRANG) | (0x1 << 31);
	task0.data_len = word_len;


	task0.source[0].addr	= 0;
	task0.source[0].length      = 0;
	task0.destination[0].addr   = (GET_LO32(p_sign) >> align_shift);
	task0.destination[0].length = word_len;
	task0.next_descriptor       = 0;

	flush_cache((ulong)(&task0), ALIGN(sizeof(task0), CACHE_LINE_SIZE));
	flush_cache((ulong)(p_sign), CACHE_LINE_SIZE);

	ss_set_drq(GET_LO32(&task0) >> align_shift);
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ALG_TRANG);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	ss_set_drq(0);
	if (ss_check_err()) {
		printf("SS %s fail 0x%x\n", __func__, ss_check_err());
		return -1;
	}

	invalidate_dcache_range((ulong)p_sign,
				((ulong)p_sign) + CACHE_LINE_SIZE);
	/*copy data*/
	memcpy(rng_buf, p_sign, rng_byte);
	return 0;
}

int sunxi_aes_with_hardware(uint8_t *dst_addr, uint8_t *src_addr, int len,
			    uint8_t *key, uint32_t key_len,
			    uint32_t symmetric_ctl, uint32_t c_ctl)
{
	uint8_t *src_align;
	uint8_t *dst_align;
	uint8_t __aligned(64) iv[16]   = { 0 };
	task_queue task0 __aligned(64) = { 0 };
	uint32_t cts_size, destination_len;
	uint32_t align_shift = ss_get_addr_align();
	ALLOC_CACHE_ALIGN_BUFFER(uint8_t, key_map, 64);
	src_align = src_addr;
	dst_align = dst_addr;

	memset(key_map, 0, 64);
	if (key) {
		memcpy(key_map, key, key_len);
	}
	memset(iv, 0x0, 16);
	memset((void *)&task0, 0x00, sizeof(task_queue));

	cts_size	= ALIGN(len, 4) / 4;
	destination_len = cts_size;

	task0.task_id		    = 0;
	task0.common_ctl	    = (1U << 31) | c_ctl;
	task0.symmetric_ctl	 = symmetric_ctl;
	task0.key_descriptor	= ((GET_LO32(key_map)) >> align_shift);
	task0.iv_descriptor	 = (GET_LO32(iv) >> align_shift);
	task0.data_len		    = cts_size;
	task0.source[0].addr	= ((GET_LO32(src_align)) >> align_shift);
	task0.source[0].length      = cts_size;
	task0.destination[0].addr   = ((GET_LO32(dst_align)) >> align_shift);
	task0.destination[0].length = destination_len;
	task0.next_descriptor       = 0;


	//flush&clean cache
	flush_cache((ulong)iv, ALIGN(sizeof(iv), CACHE_LINE_SIZE));
	flush_cache((ulong)&task0, sizeof(task_queue));
	flush_cache((ulong)key_map, ALIGN(sizeof(key_map), CACHE_LINE_SIZE));
	flush_cache((ulong)src_align, ALIGN(len, CACHE_LINE_SIZE));
	flush_cache((ulong)dst_align, ALIGN(len, CACHE_LINE_SIZE));

	ss_set_drq(GET_LO32(&task0) >> align_shift);
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ALG_AES);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	ss_set_drq(0);
	if (ss_check_err()) {
		printf("SS %s fail 0x%x\n", __func__, ss_check_err());
	}

	invalidate_dcache_range((ulong)dst_align,
				((ulong)dst_align) + CACHE_LINE_SIZE);
	memcpy(dst_addr, dst_align, len);

	return 0;
}

int sunxi_md5_calc(u8 *dst_addr, u32 dst_len, u8 *src_addr, u32 src_len)
{
	u32 word_len = 0, src_align_len = 0;
	u32 total_bit_len			    = 0;
	task_queue task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	/* sha256  2word, sha512 4word*/
	ALLOC_CACHE_ALIGN_BUFFER(u32, total_package_len,
				 CACHE_LINE_SIZE / sizeof(u32));
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_sign, CACHE_LINE_SIZE);

	memset(p_sign, 0, sizeof(p_sign));

	if (ss_get_ver() < 2) {
		/* CE1.0 */
		src_align_len = __sha_padding(src_len, (u8 *)src_addr, PADDING_MD5);
		word_len      = src_align_len >> 2;

		task0.task_id    = 0;
		task0.common_ctl = (ALG_MD5) | (1U << 31);
		task0.data_len   = word_len;

	} else {
		/* CE2.0 */
		src_align_len = ALIGN(src_len, 4);
		word_len      = src_align_len >> 2;

		total_bit_len	= src_len << 3;
		total_package_len[0] = total_bit_len;
		total_package_len[1] = 0;

		task0.task_id	= 0;
		task0.common_ctl     = (ALG_MD5) | (1 << 15) | (1U << 31);
		task0.key_descriptor = GET_LO32(total_package_len);
		task0.data_len       = total_bit_len;
	}

	task0.source[0].addr	= GET_LO32(src_addr);
	task0.source[0].length      = word_len;
	task0.destination[0].addr   = GET_LO32(p_sign);
	task0.destination[0].length = dst_len >> 2;
	task0.next_descriptor       = 0;

	flush_cache((ulong)&task0, sizeof(task0));
	flush_cache((ulong)p_sign, CACHE_LINE_SIZE);
	flush_cache((ulong)src_addr, src_align_len);
	flush_cache((ulong)total_package_len, CACHE_LINE_SIZE);

	ss_set_drq(GET_LO32(&task0));
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ALG_MD5);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if (ss_check_err()) {
		printf("SS %s fail 0x%x\n", __func__, ss_check_err());
	}

	invalidate_dcache_range((ulong)p_sign, (ulong)p_sign + CACHE_LINE_SIZE);
	/*copy data*/
	memcpy(dst_addr, p_sign, dst_len);
	return 0;
}

int sunxi_sha_calc(u8 *dst_addr, u32 dst_len, u8 *src_addr, u32 src_len)
{
	u32 word_len = 0, src_align_len = 0;
	u32 total_bit_len			    = 0;
	u32 align_shift = 0;
	task_queue task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	/* sha256  2word, sha512 4word*/
	ALLOC_CACHE_ALIGN_BUFFER(u32, total_package_len,
				 CACHE_LINE_SIZE / sizeof(u32));
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_sign, CACHE_LINE_SIZE);

	memset(p_sign, 0, CACHE_LINE_SIZE);

	align_shift = ss_get_addr_align();

	if (ss_get_ver() < 2) {
		/* CE1.0 */
		src_align_len = __sha_padding(src_len, (u8 *)src_addr, PADDING_SHA);
		word_len      = src_align_len >> 2;

		task0.task_id    = 0;
		task0.common_ctl = (ALG_SHA256) | (1U << 31);
		task0.data_len   = word_len;

	} else {
		/* CE2.0 */
		src_align_len = ALIGN(src_len, 4);
		word_len      = src_align_len >> 2;

		total_bit_len	= src_len << 3;
		total_package_len[0] = total_bit_len;
		total_package_len[1] = 0;

		task0.task_id	= 0;
		task0.common_ctl     = (ALG_SHA256) | (1 << 15) | (1U << 31);
		task0.key_descriptor = (GET_LO32(total_package_len) >> align_shift);
		task0.data_len       = total_bit_len;
	}

	task0.source[0].addr	= (GET_LO32(src_addr) >> align_shift);
	task0.source[0].length      = word_len;
	task0.destination[0].addr   = (GET_LO32(p_sign) >> align_shift);
	task0.destination[0].length = 32 >> 2;
	task0.next_descriptor       = 0;

	flush_cache((ulong)&task0, sizeof(task0));
	flush_cache((ulong)p_sign, CACHE_LINE_SIZE);
	flush_cache((ulong)src_addr, src_align_len);
	flush_cache((ulong)total_package_len, CACHE_LINE_SIZE);

	ss_set_drq((GET_LO32(&task0) >> align_shift));
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ALG_SHA256);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if (ss_check_err()) {
		printf("SS %s fail 0x%x\n", __func__, ss_check_err());
	}

	invalidate_dcache_range((ulong)p_sign, (ulong)p_sign + CACHE_LINE_SIZE);
	//copy data
	memcpy(dst_addr, p_sign, 32);
	return 0;
}

int sunxi_hash_test(u8 *dst_addr, u32 dst_len, u8 *src_addr, u32 src_len, u32 sha_type)
{
	u32 word_len = 0, src_align_len = 0;
	u32 total_bit_len			    = 0;
	u32 align_shift = 0;
	task_queue task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	/* sha256  2word, sha512 4word*/
	ALLOC_CACHE_ALIGN_BUFFER(u32, total_package_len,
				 CACHE_LINE_SIZE / sizeof(u32));
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_sign, CACHE_LINE_SIZE);

	memset(p_sign, 0, CACHE_LINE_SIZE);

	align_shift = ss_get_addr_align();

	if (ss_get_ver() < 2) {
		/* CE1.0 */
		src_align_len = __sha_padding(src_len, (u8 *)src_addr, PADDING_SHA);
		word_len      = src_align_len >> 2;

		task0.task_id    = 0;
		task0.common_ctl = (sha_type) | (1U << 31);
		task0.data_len   = word_len;

	} else {
		/* CE2.0 */
		src_align_len = ALIGN(src_len, 4);
		word_len      = src_align_len >> 2;

		total_bit_len	= src_len << 3;
		total_package_len[0] = total_bit_len;
		total_package_len[1] = 0;

		task0.task_id	= 0;
		task0.common_ctl     = (sha_type) | (1 << 15) | (1U << 31);
		task0.key_descriptor = (GET_LO32(total_package_len) >> align_shift);
		task0.data_len       = total_bit_len;
	}

	task0.source[0].addr	= (GET_LO32(src_addr) >> align_shift);
	task0.source[0].length      = word_len;
	task0.destination[0].addr   = (GET_LO32(p_sign) >> align_shift);
	task0.destination[0].length = 32 >> 2;
	task0.next_descriptor       = 0;

	flush_cache((ulong)&task0, sizeof(task0));
	flush_cache((ulong)p_sign, CACHE_LINE_SIZE);
	flush_cache((ulong)src_addr, src_align_len);
	flush_cache((ulong)total_package_len, CACHE_LINE_SIZE);

	ss_set_drq((GET_LO32(&task0) >> align_shift));
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ALG_SHA256);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if (ss_check_err()) {
		pr_err("SS %s fail 0x%x\n", __func__, ss_check_err());
	}

	invalidate_dcache_range((ulong)p_sign, (ulong)p_sign + CACHE_LINE_SIZE);
	//copy data
	memcpy(dst_addr, p_sign, 32);
	return 0;
}

s32 sm2_crypto_gen_cxy_kxy(struct sunxi_sm2_ctx_t *sm2_ctx)
{
	task_queue task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	u32 sm2_size_byte = sm2_ctx->sm2_size >> 3;
	u32 sm2_size_word = sm2_ctx->sm2_size >> 5;
	u32 mode = sm2_ctx->mode;
	u32 align_shift = ss_get_addr_align();
	int i;
	int case_size = sm2_size_byte;

	if (ss_get_ver() < 2) {
		pr_err("CE_1_0 don't support sm2\n");
		return 0;
	}

	if (case_size < CACHE_LINE_SIZE) {
		case_size = CACHE_LINE_SIZE;
	}

	ALLOC_CACHE_ALIGN_BUFFER(u8, p_k, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_p, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_a, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_gx, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_gy, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_px, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_py, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_h, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_cx, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_cy, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_kx, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_ky, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_out, case_size);

	/*big-endian to little-endian*/
	word_padding(p_k, sm2_ctx->k, sm2_size_byte);
	word_padding(p_p, sm2_ctx->p, sm2_size_byte);
	word_padding(p_a, sm2_ctx->a, sm2_size_byte);
	word_padding(p_gx, sm2_ctx->gx, sm2_size_byte);
	word_padding(p_gy, sm2_ctx->gy, sm2_size_byte);
	word_padding(p_h, sm2_ctx->h, sm2_size_byte);
	word_padding(p_px, sm2_ctx->px, sm2_size_byte);
	word_padding(p_py, sm2_ctx->py, sm2_size_byte);

	memset(p_cx, 0x00, case_size);
	memset(p_cy, 0x00, case_size);
	memset(p_kx, 0x00, case_size);
	memset(p_ky, 0x00, case_size);
	memset(p_out, 0x00, case_size);

	memset((void *)&task0, 0x00, sizeof(task0));
	task0.task_id	     = CHANNEL_0;
	task0.common_ctl     = (ALG_SM2 | (1U << 31));
	task0.asymmetric_ctl = (sm2_size_word) | (mode << 16);

	//encrypt
	if (mode == 0x0) {
		//k
		task0.source[0].addr = ((ulong)p_k >> align_shift);
		task0.source[0].length = (sm2_ctx->k_len >> 2);
		//p
		task0.source[1].addr = ((ulong)p_p >> align_shift);
		task0.source[1].length = sm2_size_word;
		//a
		task0.source[2].addr = ((ulong)p_a >> align_shift);
		task0.source[2].length = sm2_size_word;
		//gx
		task0.source[3].addr = ((ulong)p_gx >> align_shift);
		task0.source[3].length = sm2_size_word;
		//gy
		task0.source[4].addr = ((ulong)p_gy >> align_shift);
		task0.source[4].length = sm2_size_word;
		//h
		task0.source[5].addr = ((ulong)p_h >> align_shift);
		task0.source[5].length = sm2_size_word;
		//px
		task0.source[6].addr = ((ulong)p_px >> align_shift);
		task0.source[6].length = sm2_size_word;
		//py
		task0.source[7].addr = ((ulong)p_py >> align_shift);
		task0.source[7].length = sm2_size_word;

		//data_len
		for (i = 0; i < 8; i++) {
			task0.data_len += task0.source[i].length;
		}
		pr_err("data_len = %d\n", task0.data_len);

		//dest
		task0.destination[0].addr = ((ulong)p_cx >> align_shift);
		task0.destination[0].length = sm2_size_word;
		task0.destination[1].addr = ((ulong)p_cy >> align_shift);
		task0.destination[1].length = sm2_size_word;
		task0.destination[2].addr = ((ulong)p_kx >> align_shift);
		task0.destination[2].length = sm2_size_word;
		task0.destination[3].addr = ((ulong)p_ky >> align_shift);
		task0.destination[3].length = sm2_size_word;
		task0.destination[4].addr = ((ulong)p_out >> align_shift);
		task0.destination[4].length = sm2_size_word;
	} else if (mode == 0x1) {	//decrypt
		//dest
		task0.destination[0].addr = ((ulong)p_out >> align_shift);
		task0.destination[0].length = sm2_size_word;
	}

	flush_cache((ulong)&task0, sizeof(task0));
	flush_cache((ulong)p_k, case_size);
	flush_cache((ulong)p_p, case_size);
	flush_cache((ulong)p_a, case_size);
	flush_cache((ulong)p_gx, case_size);
	flush_cache((ulong)p_gy, case_size);
	flush_cache((ulong)p_h, case_size);
	flush_cache((ulong)p_px, case_size);
	flush_cache((ulong)p_py, case_size);
	flush_cache((ulong)p_cx, case_size);
	flush_cache((ulong)p_cy, case_size);
	flush_cache((ulong)p_kx, case_size);
	flush_cache((ulong)p_ky, case_size);
	flush_cache((ulong)p_out, case_size);

	ss_set_drq((GET_LO32(&task0) >> align_shift));
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ALG_SM2);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if (ss_check_err()) {
		printf("SS %s fail 0x%x\n", __func__, ss_check_err());
		return -1;
	}

	invalidate_dcache_range((ulong)p_cx, (ulong)p_cx + case_size);
	invalidate_dcache_range((ulong)p_cy, (ulong)p_cy + case_size);
	invalidate_dcache_range((ulong)p_kx, (ulong)p_kx + case_size);
	invalidate_dcache_range((ulong)p_ky, (ulong)p_ky + case_size);
	invalidate_dcache_range((ulong)p_out, (ulong)p_out + case_size);

	word_padding(sm2_ctx->cx, p_cx, sm2_size_byte);
	word_padding(sm2_ctx->cy, p_cy, sm2_size_byte);
	word_padding(sm2_ctx->kx, p_kx, sm2_size_byte);
	word_padding(sm2_ctx->ky, p_ky, sm2_size_byte);

	if ((*(u32 *)(p_out)) != 0x0) {
		pr_err("sm2: calu c1 error\n");
		return -1;
	}

	return 0;
}

s32 sm2_ecdh_gen_key_rx_ry(struct sunxi_sm2_ctx_t *sm2_ctx, int flag)
{
	task_queue task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	u32 sm2_size_byte = sm2_ctx->sm2_size >> 3;
	u32 sm2_size_word = sm2_ctx->sm2_size >> 5;
	u32 demod_rax[8] = {0x64CED1BD, 0xBC99D590, 0x049B434D, 0x0FD73428,
						0xCF608A5D, 0xB8FE5CE0, 0x7F150269, 0x40BAE40E};
	u32 demod_ray[8] = {0x376629C7, 0xAB21E7DB, 0x26092249, 0x9DDB118F,
						0x07CE8EAA, 0xE3E7720A, 0xFEF6A5CC, 0x062070C0};
	u32 demod_rbx[8] = {0xACC27688, 0xA6F7B706, 0x098BC91F, 0xF3AD1BFF,
						0x7DC2802C, 0xDB14CCCC, 0xDB0A9047, 0x1F9BD707};
	u32 demod_rby[8] = {0x2FEDAC04, 0x94B2FFC4, 0xD6853876, 0xC79B8F30,
						0x1C6573AD, 0x0AA50F39, 0xFC87181E, 0x1A1B46FE};
	int i, ret1, ret2;
	u32 align_shift = ss_get_addr_align();
	int cache_size = sm2_size_byte;

	if (sm2_size_byte < CACHE_LINE_SIZE) {
		cache_size = CACHE_LINE_SIZE;
	}

	ALLOC_CACHE_ALIGN_BUFFER(u8, p_p, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_a, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_ra, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_gx, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_gy, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_rax, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_ray, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_rbx, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_rby, cache_size);

	/*big-endian to little-endian*/
	word_padding(p_p, sm2_ctx->p, sm2_size_byte);
	word_padding(p_a, sm2_ctx->a, sm2_size_byte);
	word_padding(p_gx, sm2_ctx->gx, sm2_size_byte);
	word_padding(p_gy, sm2_ctx->gy, sm2_size_byte);
	if (flag == ECDH_USER_A) {
		word_padding(p_ra, sm2_ctx->ra, sm2_size_byte);
	} else {
		word_padding(p_ra, sm2_ctx->rb, sm2_size_byte);
	}

	memset((void *)&task0, 0x00, sizeof(task0));
	task0.task_id	     = CHANNEL_0;
	task0.common_ctl     = (0x21 | (1U << 31));
	task0.asymmetric_ctl = (sm2_size_word) | (2 << 16);

	//p
	task0.source[0].addr = ((ulong)p_p >> align_shift);
	task0.source[0].length = sm2_size_word;
	//ra
	task0.source[1].addr = ((ulong)p_ra >> align_shift);
	task0.source[1].length = sm2_size_word;
	//a
	task0.source[2].addr = ((ulong)p_a >> align_shift);
	task0.source[2].length = sm2_size_word;
	//gx
	task0.source[3].addr = ((ulong)p_gx >> align_shift);
	task0.source[3].length = sm2_size_word;
	//gy
	task0.source[4].addr = ((ulong)p_gy >> align_shift);
	task0.source[4].length = sm2_size_word;

	//data_len
	for (i = 0; i < 5; i++) {
		task0.data_len += task0.source[i].length;
	}

	//dest
	if (flag == ECDH_USER_A) {
		task0.destination[0].addr = ((ulong)p_rax >> align_shift);
		task0.destination[0].length = sm2_size_word;
		task0.destination[1].addr = ((ulong)p_ray >> align_shift);
		task0.destination[1].length = sm2_size_word;
	} else {
		task0.destination[0].addr = ((ulong)p_rbx >> align_shift);
		task0.destination[0].length = sm2_size_word;
		task0.destination[1].addr = ((ulong)p_rby >> align_shift);
		task0.destination[1].length = sm2_size_word;
	}

	flush_cache((ulong)&task0, sizeof(task0));
	flush_cache((ulong)p_p, cache_size);
	flush_cache((ulong)p_ra, cache_size);
	flush_cache((ulong)p_a, cache_size);
	flush_cache((ulong)p_gx, cache_size);
	flush_cache((ulong)p_gy, cache_size);
	flush_cache((ulong)p_rax, cache_size);
	flush_cache((ulong)p_ray, cache_size);
	flush_cache((ulong)p_rbx, cache_size);
	flush_cache((ulong)p_rby, cache_size);

	ss_set_drq((GET_LO32(&task0) >> align_shift));
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ALG_SM2);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if (ss_check_err()) {
		printf("SS %s fail 0x%x\n", __func__, ss_check_err());
		return -1;
	}

	if (flag == ECDH_USER_A) {
		invalidate_dcache_range((ulong)p_rax, (ulong)p_rax + cache_size);
		invalidate_dcache_range((ulong)p_ray, (ulong)p_ray + cache_size);
		word_padding(sm2_ctx->rax, p_rax, sm2_size_byte);
		word_padding(sm2_ctx->ray, p_ray, sm2_size_byte);
		ret1 = memcmp(demod_rax, sm2_ctx->rax, sm2_size_byte);
		ret2 = memcmp(demod_ray, sm2_ctx->ray, sm2_size_byte);
		if ((ret1 || ret2)) {
			pr_err(" compare rax ray fail\n");
			pr_err("rax(ce)\n");
			sunxi_dump((u8 *)(IOMEM_ADDR(sm2_ctx->rax)), sm2_size_byte);
			pr_err("rax(demod)\n");
			sunxi_dump((u8 *)(IOMEM_ADDR(demod_rax)), sm2_size_byte);
			pr_err("ray(ce)\n");
			sunxi_dump((u8 *)(IOMEM_ADDR(sm2_ctx->ray)), sm2_size_byte);
			pr_err("ray(demod)\n");
			sunxi_dump((u8 *)(IOMEM_ADDR(demod_ray)), sm2_size_byte);
			return -1;
		} else {
			pr_err(" compare rax ray sucess\n");
		}
	} else {
		invalidate_dcache_range((ulong)p_rbx, (ulong)p_rbx + cache_size);
		invalidate_dcache_range((ulong)p_rby, (ulong)p_rby + cache_size);
		word_padding(sm2_ctx->rbx, p_rbx, sm2_size_byte);
		word_padding(sm2_ctx->rby, p_rby, sm2_size_byte);
		ret1 = memcmp(demod_rbx, sm2_ctx->rbx, sm2_size_byte);
		ret2 = memcmp(demod_rby, sm2_ctx->rby, sm2_size_byte);
		if ((ret1 || ret2)) {
			pr_err(" compare rbx ray fail\n");
			pr_err("rbx(ce)\n");
			sunxi_dump((u8 *)(IOMEM_ADDR(sm2_ctx->rbx)), sm2_size_byte);
			pr_err("rbx(demod)\n");
			sunxi_dump((u8 *)(IOMEM_ADDR(demod_rbx)), sm2_size_byte);
			pr_err("rby(ce)\n");
			sunxi_dump((u8 *)(IOMEM_ADDR(sm2_ctx->rby)), sm2_size_byte);
			pr_err("rby(demod)\n");
			sunxi_dump((u8 *)(IOMEM_ADDR(demod_rby)), sm2_size_byte);
			return -1;
		} else {
			pr_err(" compare rbx rby sucess\n");
		}
	}

	return 0;
}

s32 sm2_ecdh_gen_ux_uy(struct sunxi_sm2_ctx_t *sm2_ctx)
{
	task_queue task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	u32 sm2_size_byte = sm2_ctx->sm2_size >> 3;
	u32 sm2_size_word = sm2_ctx->sm2_size >> 5;
	u32 mode = sm2_ctx->mode;
	u32 align_shift = ss_get_addr_align();
	u8 *p_data = NULL;
	int cache_size = sm2_size_byte;

	if (sm2_size_byte < CACHE_LINE_SIZE) {
		cache_size = CACHE_LINE_SIZE;
	}

	ALLOC_CACHE_ALIGN_BUFFER(u8, data, (12* sm2_size_byte));
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_n, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_h, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_p, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_a, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_ra, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_b, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_da, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_pxb, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_pyb, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_rax, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_rbx, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_rby, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_ux, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_uy, cache_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_out, cache_size);

	//-3-gen xu ||yu
	/*big-endian to little-endian*/
	word_padding(p_n, sm2_ctx->n, sm2_size_byte);
	word_padding(p_rax, sm2_ctx->rax, sm2_size_byte);
	word_padding(p_ra, sm2_ctx->ra, sm2_size_byte);
	word_padding(p_da, sm2_ctx->da, sm2_size_byte);
	word_padding(p_h, sm2_ctx->h, sm2_size_byte);
	word_padding(p_p, sm2_ctx->p, sm2_size_byte);
	word_padding(p_a, sm2_ctx->a, sm2_size_byte);
	word_padding(p_rbx, sm2_ctx->rbx, sm2_size_byte);
	word_padding(p_rby, sm2_ctx->rby, sm2_size_byte);
	word_padding(p_b, sm2_ctx->b, sm2_size_byte);
	word_padding(p_pxb, sm2_ctx->pxb, sm2_size_byte);
	word_padding(p_pyb, sm2_ctx->pyb, sm2_size_byte);

	memset((void *)&task0, 0x00, sizeof(task0));
	task0.task_id	     = CHANNEL_0;
	task0.common_ctl     = (ALG_SM2 | (1U << 31));
	task0.asymmetric_ctl = (sm2_size_word) | (mode << 16);

	memset((void *)data, 0x00, sizeof(data));
	p_data = data;
	//n
	memcpy(p_data, p_n, sm2_size_byte);
	p_data += sm2_size_byte;
	//rax
	memcpy(p_data, p_rax, sm2_size_byte);
	p_data += sm2_size_byte;
	//ra
	memcpy(p_data, p_ra, sm2_size_byte);
	p_data += sm2_size_byte;
	//da
	memcpy(p_data, p_da, sm2_size_byte);
	p_data += sm2_size_byte;
	//h
	memcpy(p_data, p_h, sm2_size_byte);
	p_data += sm2_size_byte;
	//p
	memcpy(p_data, p_p, sm2_size_byte);
	p_data += sm2_size_byte;
	//a
	memcpy(p_data, p_a, sm2_size_byte);
	p_data += sm2_size_byte;
	//rbx
	memcpy(p_data, p_rbx, sm2_size_byte);
	p_data += sm2_size_byte;
	//rby
	memcpy(p_data, p_rby, sm2_size_byte);
	p_data += sm2_size_byte;
	//b
	memcpy(p_data, p_b, sm2_size_byte);
	p_data += sm2_size_byte;
	//pxb
	memcpy(p_data, p_pxb, sm2_size_byte);
	p_data += sm2_size_byte;
	//pyb
	memcpy(p_data, p_pyb, sm2_size_byte);
	p_data += sm2_size_byte;

	//source
	task0.source[0].addr = ((ulong)data >> align_shift);
	task0.source[0].length = 12 * sm2_size_word;

	//data_len
	task0.data_len = 12 * sm2_size_word;

	//dest
	task0.destination[0].addr = ((ulong)p_ux >> align_shift);
	task0.destination[0].length = sm2_size_word;
	task0.destination[1].addr = ((ulong)p_uy >> align_shift);
	task0.destination[1].length = sm2_size_word;
	task0.destination[2].addr = ((ulong)p_out >> align_shift);
	task0.destination[2].length = sm2_size_word;

	flush_cache((ulong)data, task0.data_len);
	flush_cache((ulong)p_ux, cache_size);
	flush_cache((ulong)p_uy, cache_size);
	flush_cache((ulong)p_out, cache_size);

	ss_set_drq((GET_LO32(&task0) >> align_shift));
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ALG_SM2);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if (ss_check_err()) {
		printf("SS %s fail 0x%x\n", __func__, ss_check_err());
		return -1;
	}

	invalidate_dcache_range((ulong)p_ux, (ulong)p_ux + cache_size);
	invalidate_dcache_range((ulong)p_uy, (ulong)p_uy + cache_size);
	invalidate_dcache_range((ulong)p_out, (ulong)p_out + cache_size);

	if ((*(u32 *)(p_out)) != 1) {
		pr_err("%s fail\n", __func__);
		return -1;
	}

	return 0;
}

s32 sunxi_sm2_sign_verify_test(struct sunxi_sm2_ctx_t *sm2_ctx)
{
	task_queue task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	u32 sm2_size_byte = sm2_ctx->sm2_size >> 3;
	u32 sm2_size_word = sm2_ctx->sm2_size >> 5;
	u32 mode = sm2_ctx->mode;
	u32 align_shift = ss_get_addr_align();
	int i;
	u8 *p_data = NULL;
	int case_size = sm2_size_byte;

	if (ss_get_ver() < 2) {
		pr_err("CE_1_0 don't support sm2\n");
		return 0;
	}

	if (case_size < CACHE_LINE_SIZE) {
		case_size = CACHE_LINE_SIZE;
	}

	ALLOC_CACHE_ALIGN_BUFFER(u8, data, 512);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_k, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_p, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_a, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_gx, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_gy, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_e, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_n, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_px, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_py, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_d, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_r, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_s, case_size);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_out, case_size);

	memset(p_r, 0x0, case_size);
	memset(p_s, 0x0, case_size);
	memset(p_out, 0x0, case_size);
	/*big-endian to little-endian*/
	word_padding(p_k, sm2_ctx->k, sm2_size_byte);
	word_padding(p_p, sm2_ctx->p, sm2_size_byte);
	word_padding(p_a, sm2_ctx->a, sm2_size_byte);
	word_padding(p_gx, sm2_ctx->gx, sm2_size_byte);
	word_padding(p_gy, sm2_ctx->gy, sm2_size_byte);
	word_padding(p_n, sm2_ctx->n, sm2_size_byte);
	word_padding(p_px, sm2_ctx->px, sm2_size_byte);
	word_padding(p_py, sm2_ctx->py, sm2_size_byte);
	word_padding(p_d, sm2_ctx->d, sm2_size_byte);
	word_padding(p_e, sm2_ctx->e, sm2_size_byte);
	word_padding(p_r, sm2_ctx->r, sm2_size_byte);
	word_padding(p_s, sm2_ctx->s, sm2_size_byte);

	memset((void *)&task0, 0x00, sizeof(task0));
	task0.task_id	     = CHANNEL_0;
	task0.common_ctl     = (ALG_SM2 | (1U << 31));
	task0.asymmetric_ctl = (sm2_size_word) | (mode << 16);

	//signature
	if (mode == 0x2) {
		//k
		task0.source[0].addr = ((ulong)p_k >> align_shift);
		task0.source[0].length = (sm2_ctx->k_len >> 2);
		//p
		task0.source[1].addr = ((ulong)p_p >> align_shift);
		task0.source[1].length = sm2_size_word;
		//a
		task0.source[2].addr = ((ulong)p_a >> align_shift);
		task0.source[2].length = sm2_size_word;
		//gx
		task0.source[3].addr = ((ulong)p_gx >> align_shift);
		task0.source[3].length = sm2_size_word;
		//gy
		task0.source[4].addr = ((ulong)p_gy >> align_shift);
		task0.source[4].length = sm2_size_word;
		//e
		task0.source[5].addr = ((ulong)p_e >> align_shift);
		task0.source[5].length = sm2_size_word;
		//n
		task0.source[6].addr = ((ulong)p_n >> align_shift);
		task0.source[6].length = sm2_size_word;
		//d
		task0.source[7].addr = ((ulong)p_d >> align_shift);
		task0.source[7].length = sm2_size_word;

		//data_len
		for (i = 0; i < 8; i++) {
			task0.data_len += task0.source[i].length;
		}

		//dest
		task0.destination[0].addr = ((ulong)p_r >> align_shift);
		task0.destination[0].length = sm2_size_word;
		task0.destination[1].addr = ((ulong)p_s >> align_shift);
		task0.destination[1].length = sm2_size_word;
		task0.destination[2].addr = ((ulong)p_out >> align_shift);
		task0.destination[2].length = sm2_size_word;

		flush_cache((ulong)p_k, case_size);
		flush_cache((ulong)p_p, case_size);
		flush_cache((ulong)p_a, case_size);
		flush_cache((ulong)p_gx, case_size);
		flush_cache((ulong)p_gy, case_size);
		flush_cache((ulong)p_n, case_size);
		flush_cache((ulong)p_e, case_size);
		flush_cache((ulong)p_d, case_size);
		flush_cache((ulong)p_r, case_size);
		flush_cache((ulong)p_s, case_size);
		flush_cache((ulong)p_out, case_size);
	} else if (mode == 0x3) {	//verify
		memset(data, 0x0, sizeof(data));
		p_data = data;
		//n
		memcpy(p_data, p_n, sm2_size_byte);
		p_data += sm2_size_byte;
		//r
		memcpy(p_data, p_r, sm2_size_byte);
		p_data += sm2_size_byte;
		//s
		memcpy(p_data, p_s, sm2_size_byte);
		p_data += sm2_size_byte;
		//a
		memcpy(p_data, p_a, sm2_size_byte);
		p_data += sm2_size_byte;
		//gx
		memcpy(p_data, p_gx, sm2_size_byte);
		p_data += sm2_size_byte;
		//gy
		memcpy(p_data, p_gy, sm2_size_byte);
		p_data += sm2_size_byte;
		//p
		memcpy(p_data, p_p, sm2_size_byte);
		p_data += sm2_size_byte;
		//px
		memcpy(p_data, p_px, sm2_size_byte);
		p_data += sm2_size_byte;
		//py
		memcpy(p_data, p_py, sm2_size_byte);
		p_data += sm2_size_byte;
		//n
		memcpy(p_data, p_n, sm2_size_byte);
		p_data += sm2_size_byte;
		//e
		memcpy(p_data, p_e, sm2_size_byte);
		p_data += sm2_size_byte;
		//r
		memcpy(p_data, p_r, sm2_size_byte);
		p_data += sm2_size_byte;

		//source
		task0.source[0].addr = ((ulong)data >> align_shift);
		task0.source[0].length = 12 * sm2_size_word;

		//data_len
		task0.data_len = 12 * sm2_size_word;

		//dest
		task0.destination[0].addr = ((ulong)p_out >> align_shift);
		task0.destination[4].length = sm2_size_word;

		flush_cache((ulong)p_out, case_size);
		flush_cache((ulong)data, task0.data_len);
	}

	flush_cache((ulong)&task0, sizeof(task0));

	ss_set_drq((GET_LO32(&task0) >> align_shift));
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ALG_SM2);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if (ss_check_err()) {
		printf("SS %s fail 0x%x\n", __func__, ss_check_err());
		return -1;
	}

	if (mode == 0x2) {
		invalidate_dcache_range((ulong)p_r, (ulong)p_r + case_size);
		invalidate_dcache_range((ulong)p_s, (ulong)p_s + case_size);
		word_padding(sm2_ctx->r, p_r, sm2_size_byte);
		word_padding(sm2_ctx->s, p_s, sm2_size_byte);
	} else {
		invalidate_dcache_range((ulong)p_out, (ulong)p_out + case_size);
		if ((*(ulong *)(p_out)) != 2) {
			pr_err("signature verify fail");
			return -1;
		}
	}

	return 0;
}

s32 sunxi_normal_rsa(u8 *n_addr, u32 n_len, u8 *e_addr, u32 e_len, u8 *dst_addr,
		   u32 dst_len, u8 *src_addr, u32 src_len)
{
	const u32 TEMP_BUFF_LEN = ((2048 >> 3) + CACHE_LINE_SIZE);

	u32 mod_bit_size	 = 2048;
	u32 mod_size_len_inbytes = mod_bit_size / 8;
	u32 data_word_len	= mod_size_len_inbytes / 4;
	u32 align_shift = 0;

	task_queue task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_n, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_e, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_src, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_dst, TEMP_BUFF_LEN);

	__rsa_padding(p_src, src_addr, src_len, mod_size_len_inbytes);
	__rsa_padding(p_n, n_addr, n_len, mod_size_len_inbytes);
	memset(p_e, 0, mod_size_len_inbytes);
	memcpy(p_e, e_addr, e_len);

	align_shift = ss_get_addr_align();

	if (ss_get_ver() < 2) {
		/* CE1.0 */
		task0.task_id		    = 0;
		task0.common_ctl	    = (ALG_RSA | (1U << 31));
		task0.symmetric_ctl	 = 0;
		task0.asymmetric_ctl	= (2 << 28); /* rsa2048 */
		task0.key_descriptor	= GET_LO32(p_e);
		task0.iv_descriptor	 = GET_LO32(p_n);
		task0.ctr_descriptor	= 0;
		task0.data_len		    = data_word_len;
		task0.source[0].addr	= GET_LO32(p_src);
		task0.source[0].length      = data_word_len;
		task0.destination[0].addr   = GET_LO32(p_dst);
		task0.destination[0].length = data_word_len;
		task0.next_descriptor       = 0;
	} else {
		/* CE2.0 */
		task0.task_id	= 0;
		task0.common_ctl     = (ALG_RSA | (1U << 31));
		task0.symmetric_ctl  = 0;
		task0.asymmetric_ctl = (2048 / 32); /* rsa2048 */
		task0.ctr_descriptor = 0;

		task0.source[0].addr   = (GET_LO32(p_e) >> align_shift);
		task0.source[0].length = data_word_len;
		task0.source[1].addr   = (GET_LO32(p_n) >> align_shift);
		task0.source[1].length = data_word_len;
		task0.source[2].addr   = (GET_LO32(p_src) >> align_shift);
		task0.source[2].length = data_word_len;

		task0.data_len += task0.source[0].length;
		task0.data_len += task0.source[1].length;
		task0.data_len += task0.source[2].length;
		task0.data_len *= 4; /* byte len */

		task0.destination[0].addr   = (GET_LO32(p_dst) >> align_shift);
		task0.destination[0].length = data_word_len;
	}

	flush_cache((ulong)&task0, sizeof(task0));
	flush_cache((ulong)p_n, mod_size_len_inbytes);
	flush_cache((ulong)p_e, mod_size_len_inbytes);
	flush_cache((ulong)p_src, mod_size_len_inbytes);
	flush_cache((ulong)p_dst, mod_size_len_inbytes);

	ss_set_drq((GET_LO32(&task0) >> align_shift));
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ALG_RSA);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if (ss_check_err()) {
		printf("SS %s fail 0x%x\n", __func__, ss_check_err());
	}

	invalidate_dcache_range((ulong)p_dst,
				(ulong)p_dst + mod_size_len_inbytes);
	__rsa_padding(dst_addr, p_dst, mod_bit_size / 64, mod_bit_size / 64);

	return 0;
}

s32 sunxi_rsa_calc(u8 *n_addr, u32 n_len, u8 *e_addr, u32 e_len, u8 *dst_addr,
		   u32 dst_len, u8 *src_addr, u32 src_len)
{
	const u32 TEMP_BUFF_LEN = ((2048 >> 3) + CACHE_LINE_SIZE);

	u32 mod_bit_size	 = 2048;
	u32 mod_size_len_inbytes = mod_bit_size / 8;
	u32 data_word_len	= mod_size_len_inbytes / 4;
	u32 align_shift = 0;

	task_queue task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_n, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_e, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_src, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_dst, TEMP_BUFF_LEN);

	__rsa_padding(p_src, src_addr, src_len, mod_size_len_inbytes);
	__rsa_padding(p_n, n_addr, n_len, mod_size_len_inbytes);
	memset(p_e, 0, mod_size_len_inbytes);
	memcpy(p_e, e_addr, e_len);

	align_shift = ss_get_addr_align();

	if (ss_get_ver() < 2) {
		/* CE1.0 */
		task0.task_id		    = 0;
		task0.common_ctl	    = (ALG_RSA | (1U << 31));
		task0.symmetric_ctl	 = 0;
		task0.asymmetric_ctl	= (2 << 28); /* rsa2048 */
		task0.key_descriptor	= GET_LO32(p_e);
		task0.iv_descriptor	 = GET_LO32(p_n);
		task0.ctr_descriptor	= 0;
		task0.data_len		    = data_word_len;
		task0.source[0].addr	= GET_LO32(p_src);
		task0.source[0].length      = data_word_len;
		task0.destination[0].addr   = GET_LO32(p_dst);
		task0.destination[0].length = data_word_len;
		task0.next_descriptor       = 0;
	} else {
		/* CE2.0 */
		task0.task_id	= 0;
		task0.common_ctl     = (ALG_RSA | (1U << 31));
		task0.symmetric_ctl  = 0;
		task0.asymmetric_ctl = (2048 / 32); /* rsa2048 */
		task0.ctr_descriptor = 0;

		task0.source[0].addr   = (GET_LO32(p_e) >> align_shift);
		task0.source[0].length = data_word_len;
		task0.source[1].addr   = (GET_LO32(p_n) >> align_shift);
		task0.source[1].length = data_word_len;
		task0.source[2].addr   = (GET_LO32(p_src) >> align_shift);
		task0.source[2].length = data_word_len;

		task0.data_len += task0.source[0].length;
		task0.data_len += task0.source[1].length;
		task0.data_len += task0.source[2].length;
		task0.data_len *= 4; /* byte len */

		task0.destination[0].addr   = (GET_LO32(p_dst) >> align_shift);
		task0.destination[0].length = data_word_len;
	}

	flush_cache((ulong)&task0, sizeof(task0));
	flush_cache((ulong)p_n, mod_size_len_inbytes);
	flush_cache((ulong)p_e, mod_size_len_inbytes);
	flush_cache((ulong)p_src, mod_size_len_inbytes);
	flush_cache((ulong)p_dst, mod_size_len_inbytes);

	ss_set_drq((GET_LO32(&task0) >> align_shift));
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ALG_RSA);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if (ss_check_err()) {
		printf("SS %s fail 0x%x\n", __func__, ss_check_err());
	}

	invalidate_dcache_range((ulong)p_dst,
				(ulong)p_dst + mod_size_len_inbytes);
	__rsa_padding(dst_addr, p_dst, mod_bit_size / 64, mod_bit_size / 64);

	return 0;
}

int sunxi_create_rssk(u8 *rssk_buf, u32 rssk_byte)
{
	u32 total_bit_len			    = 0;
	u32 align_shift				    = 0;
	task_queue task0 __aligned(CACHE_LINE_SIZE) = { 0 };

	ALLOC_CACHE_ALIGN_BUFFER(u32, total_package_len,
				 CACHE_LINE_SIZE / sizeof(u32));
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_sign, CACHE_LINE_SIZE);

	memset(p_sign, 0, sizeof(p_sign));

	if (rssk_buf == NULL) {
		return -1;
	}

	align_shift = ss_get_addr_align();

	total_bit_len	= rssk_byte << 3;
	total_package_len[0] = total_bit_len;
	total_package_len[1] = 0;

	task0.task_id		    = 0;
	task0.common_ctl	    = (ALG_TRANG) | (0x1U << 31);
	task0.key_descriptor	= GET_LO32(total_package_len);
	task0.data_len		    = rssk_byte;

	task0.source[0].addr	= 0;
	task0.source[0].length      = 0;
	task0.destination[0].addr   = GET_LO32(p_sign) >> align_shift;
	task0.destination[0].length = (rssk_byte >> 2);
	task0.next_descriptor       = 0;

	flush_cache((ulong)&task0, sizeof(task0));
	flush_cache((ulong)p_sign, CACHE_LINE_SIZE);
	flush_cache((ulong)total_package_len, CACHE_LINE_SIZE);

	ss_set_drq((GET_LO32(&task0) >> align_shift));
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ALG_TRANG);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);

	if (ss_check_err()) {
		printf("RSSK %s fail 0x%x\n", __func__, ss_check_err());
	}

	invalidate_dcache_range((ulong)p_sign, (ulong)p_sign + CACHE_LINE_SIZE);
	/*copy data*/
	memcpy(rssk_buf, p_sign, rssk_byte);

	return 0;
}
#if defined(SHA256_MULTISTEP_PACKAGE) || defined(SHA512_MULTISTEP_PACKAGE)
/**************************************************************************
*function():
*	sunxi_hash_init(): used for the first package data;
*	sunxi_hash_final(): used for the last package data;
*	sunxi_hash_update(): used for other package data;
*
* Note: these functions just used for CE2.0 in hash_alg
*
**************************************************************************/
int sunxi_sha_process(u8 *dst_addr, u32 dst_len, u8 *src_addr, u32 src_len,
		      int iv_mode, int last_flag, u32 total_len)
{
	u32 word_len				    = 0;
	u32 src_align_len			    = 0;
	u32 total_bit_len			    = 0;
	phys_addr_t iv_addr				    = 0;
	u32 cur_bit_len				    = 0;
	int alg_hash				    = ALG_SHA256;
	task_queue task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	/* sha256  2word, sha512 4word*/
	ALLOC_CACHE_ALIGN_BUFFER(u32, total_package_len,
				 CACHE_LINE_SIZE / sizeof(u32));
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_sign, CACHE_LINE_SIZE);

	memset(p_sign, 0, sizeof(p_sign));

#ifdef SHA512_MULTISTEP_PACKAGE
	alg_hash = ALG_SHA512;
#endif

	if (iv_mode == 1) {
		iv_addr		    = (phys_addr_t)dst_addr;
		task0.iv_descriptor = GET_LO32(iv_addr);
		flush_cache((phys_addr_t)iv_addr, dst_len);
	}

	/* CE2.0 */
	src_align_len = ALIGN(src_len, 4);

	if ((last_flag == 0) && (alg_hash == ALG_SHA256)) {
		cur_bit_len = (ALIGN(src_len, 64)) << 3;
	} else if ((last_flag == 0) && (alg_hash == ALG_SHA512)) {
		cur_bit_len = (ALIGN(src_len, 128)) << 3;
	} else {
		cur_bit_len = src_len << 3;
	}
	word_len	     = src_align_len >> 2;
	total_bit_len	= total_len << 3;
	total_package_len[0] = total_bit_len;
	total_package_len[1] = 0;

	task0.task_id = 0;
	task0.common_ctl =
		(alg_hash) | (last_flag << 15) | (iv_mode << 16) | (1U << 31);
	task0.key_descriptor = GET_LO32(total_package_len); /* total_len in bits */
	task0.data_len       = cur_bit_len; /* cur_data_len in bits */

	task0.source[0].addr	= GET_LO32(src_addr);
	task0.source[0].length      = word_len; /* cur_data_len in words */
	task0.destination[0].addr   = GET_LO32(p_sign);
	task0.destination[0].length = dst_len >> 2;
	task0.next_descriptor       = 0;

	flush_cache((ulong)&task0, sizeof(task0));
	flush_cache((ulong)p_sign, CACHE_LINE_SIZE);
	flush_cache((ulong)src_addr, src_align_len);
	flush_cache((ulong)total_package_len, CACHE_LINE_SIZE);

	ss_set_drq(GET_LO32(&task0);
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(alg_hash);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if (ss_check_err()) {
		printf("SS %s fail 0x%x\n", __func__, ss_check_err());
		return -1;
	}

	if (alg_hash == ALG_SHA512) {
		invalidate_dcache_range((ulong)p_sign,
					(ulong)p_sign + CACHE_LINE_SIZE * 2);
	} else {
		invalidate_dcache_range((ulong)p_sign,
					(ulong)p_sign + CACHE_LINE_SIZE);
	}
	/* copy data */
	memcpy(dst_addr, p_sign, dst_len);
	return 0;
}

int sunxi_hash_init(u8 *dst_addr, u8 *src_addr, u32 src_len, u32 total_len)
{
	u32 dst_len = 32;
	int ret     = 0;

#ifdef SHA512_MULTISTEP_PACKAGE
	dst_len = 64;
#endif
	ret = sunxi_sha_process(dst_addr, dst_len, src_addr, src_len, 0, 0,
				total_len);
	if (ret) {
		printf("sunxi hash init failed!\n");
		return -1;
	}
	return 0;
}

int sunxi_hash_update(u8 *dst_addr, u8 *src_addr, u32 src_len, u32 total_len)
{
	u32 dst_len = 32;
	int ret     = 0;

#ifdef SHA512_MULTISTEP_PACKAGE
	dst_len = 64;
#endif
	ret = sunxi_sha_process(dst_addr, dst_len, src_addr, src_len, 1, 0,
				total_len);
	if (ret) {
		printf("sunxi hash update failed!\n");
		return -1;
	}
	return 0;
}

int sunxi_hash_final(u8 *dst_addr, u8 *src_addr, u32 src_len, u32 total_len)
{
	u32 dst_len = 32;
	int ret     = 0;

#ifdef SHA512_MULTISTEP_PACKAGE
	dst_len = 64;
#endif
	ret = sunxi_sha_process(dst_addr, dst_len, src_addr, src_len, 1, 1,
				total_len);
	if (ret) {
		printf("sunxi hash final failed!\n");
		return -1;
	}
	return 0;
}
#endif

int sunxi_do_aes_crypt(struct aes_crypt_info_t *aes_info)
{
	int ret;
	u8 enc_flag;
	u8 aes_moe;
	u8 key_type;
	u8 key_size;
	u32 c_ctl = 0;
	u32 s_ctl = 0;

	if (aes_info->enc_flag == AES_ENCRYPT) {
		enc_flag = SS_DIR_ENCRYPT;
	} else {
		enc_flag = SS_DIR_DECRYPT;
	}

	if (aes_info->aes_mode == AES_MODE_ECB) {
		aes_moe = SS_AES_MODE_ECB;
	} else if (aes_info->aes_mode == SS_AES_MODE_CBC) {
		aes_moe = SS_AES_MODE_CBC;
	} else {
		pr_err("can't support aes mode\n");
		return -1;
	}

	if (aes_info->key_type == KEY_TYPE_SSK) {
		key_type = SS_KEY_SELECT_SSK;
		key_size = SS_AES_KEY_256BIT;
	} else {
		pr_err("can't support key type\n");
		return -2;
	}

	c_ctl = (enc_flag << 8);
	s_ctl = ((key_size << 0) | (aes_moe << 8) | (key_type << 20));
	ret = sunxi_aes_with_hardware((u8 *)IOMEM_ADDR(aes_info->dst_buf),
									(u8 *)IOMEM_ADDR(aes_info->src_buf),
									aes_info->src_len,
									NULL, 0, s_ctl, c_ctl);
	if (ret) {
		pr_err("sunxi_aes_with_hardware fail\n");
		return -3;
	}
	return 0;
}

#ifdef SUNXI_HASH_TEST
int do_sha256_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	u8 hash[32] = { 0 };
	phys_addr_t x1      = simple_strtol(argv[1], NULL, 16);
	u32 x2      = simple_strtol(argv[2], NULL, 16);
	if (argc < 3) {
		return -1;
	}
	printf("src = 0x%x, len = 0x%x\n", x1, x2);

	tick_printf("sha256 test start 0\n");
	sunxi_ss_open();
	sunxi_sha_calc(hash, 32, (u8 *)x1, x2);
	tick_printf("sha256 test end\n");
	sunxi_dump(hash, 32);

	return 0;
}

U_BOOT_CMD(sha256_test, 3, 0, do_sha256_test,
	   "do a sha256 test, arg1: src address, arg2: len(hex)", "NULL");
#endif
