/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include "asm/arch-sunxi/ce_2.3.h"
#include <common.h>
#include <asm/io.h>
#include <asm/arch/ce.h>
#include <memalign.h>
#include "ss_op.h"
#include <sunxi_board.h>

#define ALG_SHA256 (0x13)
#define ALG_SHA512 (0x15)
#define ALG_RSA	   (0x20)
#define ALG_MD5	   (0x10)
#define ALG_TRANG  (0x1C)

/*check if task_queue size is cache_line align*/
#define STATIC_CHECK(condition) extern u8 checkFailAt##__LINE__[-!(condition)];
STATIC_CHECK(sizeof(struct other_task_descriptor) ==
	     ALIGN(sizeof(struct other_task_descriptor), CACHE_LINE_SIZE))
STATIC_CHECK(sizeof(struct hash_task_descriptor) ==
	     ALIGN(sizeof(struct hash_task_descriptor), CACHE_LINE_SIZE))
#ifdef CE_DEBUG
unsigned long ce_task_addr_get(u8 *dst)
{
	unsigned long phy;
	if (dst[4]) {
		phy = (*((u64 *)dst)) & 0xffffffffff;
	} else {
		phy = *((u32 *)dst);
	}

	return phy;
}
#endif

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

int sunxi_md5_calc(u8 *dst_addr, u32 dst_len, u8 *src_addr, u32 src_len)
{
	u32 src_align_len					     = 0;
	u32 total_bit_len					     = 0;
	struct hash_task_descriptor task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	/* sha256  2word, sha512 4word*/
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_sign, CACHE_LINE_SIZE);

	memset(p_sign, 0, CACHE_LINE_SIZE);

	/* CE2.0 */
	src_align_len = ALIGN(src_len, 4);

	total_bit_len	     = src_len << 3;

	memset(&task0, 0, sizeof(task0));
	task0.ctrl = (CHANNEL_0 << CHN) | (0x1 << LPKG) | (0x0 << DLAV) |
		     (0x1 << IE);
	task0.cmd = (ALG_MD5);
	memcpy(task0.data_toal_len_addr, &total_bit_len, 4);
	memcpy(task0.sg[0].source_addr, &src_addr, 4);
	task0.sg[0].source_len = src_len;
	memcpy(task0.sg[0].dest_addr, &p_sign, 4);
	task0.sg[0].dest_len = dst_len;

	flush_cache((u32)&task0, sizeof(task0));
	flush_cache((u32)p_sign, CACHE_LINE_SIZE);
	flush_cache((u32)src_addr, src_align_len);

	ss_set_drq((u32)&task0);
	ss_irq_enable(CHANNEL_0);
	ss_ctrl_start(HASH_RBG_TRPE);
	ss_wait_finish(CHANNEL_0);
	ss_pending_clear(CHANNEL_0);
	ss_ctrl_stop();
	ss_irq_disable(CHANNEL_0);
	ss_set_drq(0);
	if (ss_check_err(CHANNEL_0)) {
		printf("SS %s fail 0x%x\n", __func__, ss_check_err(CHANNEL_0));
		return -1;
	}

	invalidate_dcache_range((ulong)p_sign, (ulong)p_sign + CACHE_LINE_SIZE);
	/*copy data*/
	memcpy(dst_addr, p_sign, dst_len);
	return 0;
}

int sunxi_hash_test(u8 *dst_addr, u32 dst_len, u8 *src_addr, u32 src_len, u32 sha_type)
{
	u32 total_bit_len					     = 0;
	struct hash_task_descriptor task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	/* sha256  2word, sha512 4word*/
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_sign, CACHE_LINE_SIZE);
	u32 md_size = dst_len;
	memset(p_sign, 0, CACHE_LINE_SIZE);

	memset((void *)&task0, 0x00, sizeof(task0));
	total_bit_len = src_len * 8;
	task0.ctrl    = (CHANNEL_0 << CHN) | (0x1 << LPKG) | (0x0 << DLAV) |
		     (0x1 << IE);
	task0.cmd = (sha_type << 0);
	memcpy(task0.data_toal_len_addr, &total_bit_len, 4);
	memcpy(task0.sg[0].source_addr, &src_addr, 4);
	task0.sg[0].source_len = src_len;
	memcpy(task0.sg[0].dest_addr, &p_sign, 4);
	task0.sg[0].dest_len = md_size;

	flush_cache(((u32)&task0), ALIGN(sizeof(task0), CACHE_LINE_SIZE));
	flush_cache((u32)p_sign, CACHE_LINE_SIZE);
	flush_cache(((u32)src_addr), ALIGN(src_len, CACHE_LINE_SIZE));

	ss_set_drq((u32)&task0);
	ss_irq_enable(CHANNEL_0);
	ss_ctrl_start(HASH_RBG_TRPE);
	ss_wait_finish(CHANNEL_0);
	ss_pending_clear(CHANNEL_0);
	ss_ctrl_stop();
	ss_irq_disable(CHANNEL_0);
	ss_set_drq(0);
	if (ss_check_err(CHANNEL_0)) {
		printf("SS %s fail 0x%x\n", __func__, ss_check_err(CHANNEL_0));
		return -1;
	}

	invalidate_dcache_range((ulong)p_sign,
				((ulong)p_sign) + CACHE_LINE_SIZE);
	/*copy data*/
	memcpy(dst_addr, p_sign, md_size);
	return 0;
}

int sunxi_sha_calc(u8 *dst_addr, u32 dst_len, u8 *src_addr, u32 src_len)
{
	u32 total_bit_len					     = 0;
	struct hash_task_descriptor task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	/* sha256  2word, sha512 4word*/
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_sign, CACHE_LINE_SIZE);
	u32 md_size = 32;
	memset(p_sign, 0, CACHE_LINE_SIZE);

	memset((void *)&task0, 0x00, sizeof(task0));
	total_bit_len = src_len * 8;
	task0.ctrl    = (CHANNEL_0 << CHN) | (0x1 << LPKG) | (0x0 << DLAV) |
		     (0x1 << IE);
	task0.cmd = (SUNXI_SHA256 << 0);
	memcpy(task0.data_toal_len_addr, &total_bit_len, 4);
	memcpy(task0.sg[0].source_addr, &src_addr, 4);
	task0.sg[0].source_len = src_len;
	memcpy(task0.sg[0].dest_addr, &p_sign, 4);
	task0.sg[0].dest_len = md_size;

	flush_cache(((u32)&task0), ALIGN(sizeof(task0), CACHE_LINE_SIZE));
	flush_cache((u32)p_sign, CACHE_LINE_SIZE);
	flush_cache(((u32)src_addr), ALIGN(src_len, CACHE_LINE_SIZE));

	ss_set_drq((u32)&task0);
	ss_irq_enable(CHANNEL_0);
	ss_ctrl_start(HASH_RBG_TRPE);
	ss_wait_finish(CHANNEL_0);
	ss_pending_clear(CHANNEL_0);
	ss_ctrl_stop();
	ss_irq_disable(CHANNEL_0);
	ss_set_drq(0);
	if (ss_check_err(CHANNEL_0)) {
		printf("SS %s fail 0x%x\n", __func__, ss_check_err(CHANNEL_0));
		return -1;
	}

	invalidate_dcache_range((ulong)p_sign,
				((ulong)p_sign) + CACHE_LINE_SIZE);
	/*copy data*/
	memcpy(dst_addr, p_sign, md_size);
	return 0;
}

s32 sm2_crypto_gen_cxy_kxy(struct sunxi_sm2_ctx_t *sm2_ctx)
{
	struct other_task_descriptor task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	u32 sm2_size_byte = sm2_ctx->sm2_size >> 3;
	u32 sm2_size_word = sm2_ctx->sm2_size >> 5;
	u32 mode = sm2_ctx->mode;
	int i;
	int case_size = sm2_size_byte;

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
		memcpy(task0.sg[0].source_addr, &p_k, 4);
		task0.sg[0].source_len = sm2_ctx->k_len;
		//p
		memcpy(task0.sg[1].source_addr, &p_p, 4);
		task0.sg[1].source_len = sm2_size_byte;
		//a
		memcpy(task0.sg[2].source_addr, &p_a, 4);
		task0.sg[2].source_len = sm2_size_byte;
		//gx
		memcpy(task0.sg[3].source_addr, &p_gx, 4);
		task0.sg[3].source_len = sm2_size_byte;
		//gy
		memcpy(task0.sg[4].source_addr, &p_gy, 4);
		task0.sg[4].source_len = sm2_size_byte;
		//e
		memcpy(task0.sg[5].source_addr, &p_h, 4);
		task0.sg[5].source_len = sm2_size_byte;
		//n
		memcpy(task0.sg[6].source_addr, &p_px, 4);
		task0.sg[6].source_len = sm2_size_byte;
		//d
		memcpy(task0.sg[7].source_addr, &p_py, 4);
		task0.sg[7].source_len = sm2_size_byte;

		//data_len
		for (i = 0; i < 8; i++) {
			task0.data_len += task0.sg[i].source_len;
		}

		//dest
		memcpy(task0.sg[0].dest_addr, &p_cx, 4);
		task0.sg[0].dest_len = sm2_size_byte;
		memcpy(task0.sg[1].dest_addr, &p_cy, 4);
		task0.sg[1].dest_len = sm2_size_byte;
		memcpy(task0.sg[2].dest_addr, &p_kx, 4);
		task0.sg[2].dest_len = sm2_size_byte;
		memcpy(task0.sg[3].dest_addr, &p_ky, 4);
		task0.sg[3].dest_len = sm2_size_byte;
		memcpy(task0.sg[4].dest_addr, &p_out, 4);
		task0.sg[4].dest_len = sm2_size_byte;
	} else if (mode == 0x1) {	//decrypt
		//dest
		memcpy(task0.sg[0].dest_addr, &p_out, 4);
		task0.sg[0].dest_len = sm2_size_byte;
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

#ifdef CE_DEBUG
	u64 phy_addr;
	pr_err("task->common_ctl = 0x%x\n", task0.common_ctl);
	pr_err("task->asymmetric_ctl = 0x%x\n", task0.asymmetric_ctl);
	pr_err("task->data_len = 0x%x\n", task0.data_len);
	for (i = 0; i < 8; i++) {
		phy_addr = (u64)ce_task_addr_get(task0.sg[i].source_addr);
		if (phy_addr) {
			pr_err("task->src[%d].addr = 0x%llx\n", i, phy_addr);
			pr_err("task->src[%d].len = 0x%x\n", i, task0.sg[i].source_len);
		}
	}
#endif

	ss_set_drq((ulong)&task0);
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ASYM_TRPE);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if (ss_check_err(task0.task_id)) {
		printf("SS %s fail 0x%x\n", __func__,
		       ss_check_err(task0.task_id));
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

	if ((*(ulong *)(p_out)) != 0x0) {
		pr_err("sm2: calu c1 error\n");
		return -1;
	}

	return 0;
}

s32 sm2_ecdh_gen_key_rx_ry(struct sunxi_sm2_ctx_t *sm2_ctx, int flag)
{
	struct other_task_descriptor task0 __aligned(CACHE_LINE_SIZE) = { 0 };
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
	memcpy(task0.sg[0].source_addr, &p_p, 4);
	task0.sg[0].source_len = sm2_size_byte;
	//ra
	memcpy(task0.sg[1].source_addr, &p_ra, 4);
	task0.sg[1].source_len = sm2_size_byte;
	//a
	memcpy(task0.sg[2].source_addr, &p_a, 4);
	task0.sg[2].source_len = sm2_size_byte;
	//gx
	memcpy(task0.sg[3].source_addr, &p_gx, 4);
	task0.sg[3].source_len = sm2_size_byte;
	//gy
	memcpy(task0.sg[4].source_addr, &p_gy, 4);
	task0.sg[4].source_len = sm2_size_byte;

	//data_len
	for (i = 0; i < 5; i++) {
		task0.data_len += task0.sg[i].source_len;
	}

	//dest
	if (flag == ECDH_USER_A) {
		memcpy(task0.sg[0].dest_addr, &p_rax, 4);
		task0.sg[0].dest_len = sm2_size_byte;
		memcpy(task0.sg[1].dest_addr, &p_ray, 4);
		task0.sg[1].dest_len = sm2_size_byte;
	} else {
		memcpy(task0.sg[0].dest_addr, &p_rbx, 4);
		task0.sg[0].dest_len = sm2_size_byte;
		memcpy(task0.sg[1].dest_addr, &p_rby, 4);
		task0.sg[1].dest_len = sm2_size_byte;
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

#ifdef CE_DEBUG
	u64 phy_addr;
	pr_err("task->common_ctl = 0x%x\n", task0.common_ctl);
	pr_err("task->asymmetric_ctl = 0x%x\n", task0.asymmetric_ctl);
	pr_err("task->data_len = 0x%x\n", task0.data_len);
	for (i = 0; i < 8; i++) {
		phy_addr = (u64)ce_task_addr_get(task0.sg[i].source_addr);
		if (phy_addr) {
			pr_err("task->src[%d].addr = 0x%llx\n", i, phy_addr);
			pr_err("task->src[%d].len = 0x%x\n", i, task0.sg[i].source_len);
		}
	}
#endif

	ss_set_drq((u32)&task0);
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ASYM_TRPE);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if (ss_check_err(task0.task_id)) {
		printf("SS %s fail 0x%x\n", __func__,
		       ss_check_err(task0.task_id));
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
	struct other_task_descriptor task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	u32 sm2_size_byte = sm2_ctx->sm2_size >> 3;
	u32 sm2_size_word = sm2_ctx->sm2_size >> 5;
	u32 mode = sm2_ctx->mode;
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
	memcpy(task0.sg[0].source_addr, &data, 4);
	task0.sg[0].source_len = 12 * sm2_size_byte;
	//data_len
	task0.data_len = 12 * sm2_size_byte;

	//dest
	memcpy(task0.sg[0].dest_addr, &p_ux, 4);
	task0.sg[0].dest_len = sm2_size_byte;
	memcpy(task0.sg[1].dest_addr, &p_uy, 4);
	task0.sg[1].dest_len = sm2_size_byte;
	memcpy(task0.sg[2].dest_addr, &p_out, 4);
	task0.sg[2].dest_len = sm2_size_byte;

	flush_cache((ulong)data, task0.data_len);
	flush_cache((ulong)p_ux, cache_size);
	flush_cache((ulong)p_uy, cache_size);
	flush_cache((ulong)p_out, cache_size);

#ifdef CE_DEBUG
	int i;
	u64 phy_addr;
	pr_err("task->common_ctl = 0x%x\n", task0.common_ctl);
	pr_err("task->asymmetric_ctl = 0x%x\n", task0.asymmetric_ctl);
	pr_err("task->data_len = 0x%x\n", task0.data_len);
	for (i = 0; i < 8; i++) {
		phy_addr = (u64)ce_task_addr_get(task0.sg[i].source_addr);
		if (phy_addr) {
			pr_err("task->src[%d].addr = 0x%llx\n", i, phy_addr);
			pr_err("task->src[%d].len = 0x%x\n", i, task0.sg[i].source_len);
		}
	}
#endif

	ss_set_drq((u32)&task0);
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ASYM_TRPE);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if (ss_check_err(task0.task_id)) {
		printf("SS %s fail 0x%x\n", __func__,
		       ss_check_err(task0.task_id));
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
	struct other_task_descriptor task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	u32 sm2_size_byte = sm2_ctx->sm2_size >> 3;
	u32 sm2_size_word = sm2_ctx->sm2_size >> 5;
	u32 mode = sm2_ctx->mode;
	int i;
	u8 *p_data = NULL;
	int case_size = sm2_size_byte;

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
		memcpy(task0.sg[0].source_addr, &p_k, 4);
		task0.sg[0].source_len = sm2_ctx->k_len;
		//p
		memcpy(task0.sg[1].source_addr, &p_p, 4);
		task0.sg[1].source_len = sm2_size_byte;
		//a
		memcpy(task0.sg[2].source_addr, &p_a, 4);
		task0.sg[2].source_len = sm2_size_byte;
		//gx
		memcpy(task0.sg[3].source_addr, &p_gx, 4);
		task0.sg[3].source_len = sm2_size_byte;
		//gy
		memcpy(task0.sg[4].source_addr, &p_gy, 4);
		task0.sg[4].source_len = sm2_size_byte;
		//e
		memcpy(task0.sg[5].source_addr, &p_e, 4);
		task0.sg[5].source_len = sm2_size_byte;
		//n
		memcpy(task0.sg[6].source_addr, &p_n, 4);
		task0.sg[6].source_len = sm2_size_byte;
		//d
		memcpy(task0.sg[7].source_addr, &p_d, 4);
		task0.sg[7].source_len = sm2_size_byte;

		//data_len
		for (i = 0; i < 8; i++) {
			task0.data_len += task0.sg[i].source_len;
		}

		//dest
		memcpy(task0.sg[0].dest_addr, &p_r, 4);
		task0.sg[0].dest_len = sm2_size_byte;
		memcpy(task0.sg[1].dest_addr, &p_s, 4);
		task0.sg[1].dest_len = sm2_size_byte;
		memcpy(task0.sg[2].dest_addr, &p_out, 4);
		task0.sg[2].dest_len = sm2_size_byte;

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
		memcpy(task0.sg[0].source_addr, &data, 4);
		task0.sg[0].source_len = 12 * sm2_size_byte;
		//data_len
		task0.data_len = 12 * sm2_size_byte;

		//dest
		memcpy(task0.sg[0].dest_addr, &p_out, 4);
		task0.sg[0].dest_len = sm2_size_byte;

		flush_cache((ulong)p_out, case_size);
		flush_cache((ulong)data, task0.data_len);
	}

	flush_cache((ulong)&task0, sizeof(task0));

#ifdef CE_DEBUG
	u64 phy_addr;
	pr_err("task->common_ctl = 0x%x\n", task0.common_ctl);
	pr_err("task->asymmetric_ctl = 0x%x\n", task0.asymmetric_ctl);
	pr_err("task->data_len = 0x%x\n", task0.data_len);
	for (i = 0; i < 8; i++) {
		phy_addr = (u64)ce_task_addr_get(task0.sg[i].source_addr);
		if (phy_addr) {
			pr_err("task->src[%d].addr = 0x%llx\n", i, phy_addr);
			pr_err("task->src[%d].len = 0x%x\n", i, task0.sg[i].source_len);
		}
	}
#endif
	ss_set_drq((ulong)&task0);
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ASYM_TRPE);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if (ss_check_err(task0.task_id)) {
		printf("SS %s fail 0x%x\n", __func__,
		       ss_check_err(task0.task_id));
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
	struct other_task_descriptor task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_n, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_e, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_src, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_dst, TEMP_BUFF_LEN);

	memset(p_src, 0, mod_size_len_inbytes);
	memcpy(p_src, src_addr, src_len);
	memset(p_n, 0, mod_size_len_inbytes);
	memcpy(p_n, n_addr, n_len);
	memset(p_e, 0, mod_size_len_inbytes);
	memcpy(p_e, e_addr, e_len);

	memset((void *)&task0, 0x00, sizeof(task0));
	task0.task_id	     = CHANNEL_0;
	task0.common_ctl     = (ALG_RSA | (1U << 31));
	task0.asymmetric_ctl = (mod_bit_size >> 5);
	memcpy(task0.sg[0].source_addr, &p_e, 4);
	task0.sg[0].source_len = mod_size_len_inbytes;
	memcpy(task0.sg[1].source_addr, &p_n, 4);
	task0.sg[1].source_len = mod_size_len_inbytes;
	memcpy(task0.sg[2].source_addr, &p_src, 4);
	task0.sg[2].source_len = mod_size_len_inbytes;
	task0.data_len += task0.sg[0].source_len;
	task0.data_len += task0.sg[1].source_len;
	task0.data_len += task0.sg[2].source_len;

	memcpy(task0.sg[0].dest_addr, &p_dst, 4);
	task0.sg[0].dest_len = mod_size_len_inbytes;

	flush_cache((u32)&task0, sizeof(task0));
	flush_cache((u32)p_n, mod_size_len_inbytes);
	flush_cache((u32)p_e, mod_size_len_inbytes);
	flush_cache((u32)p_src, mod_size_len_inbytes);
	flush_cache((u32)p_dst, mod_size_len_inbytes);

	ss_set_drq((u32)&task0);
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ASYM_TRPE);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if (ss_check_err(task0.task_id)) {
		printf("SS %s fail 0x%x\n", __func__,
		       ss_check_err(task0.task_id));
		return -1;
	}

	invalidate_dcache_range((ulong)p_dst,
				(ulong)p_dst + mod_size_len_inbytes);
	memcpy(dst_addr, p_dst, mod_size_len_inbytes);

	return 0;
}

s32 sunxi_rsa_calc(u8 *n_addr, u32 n_len, u8 *e_addr, u32 e_len, u8 *dst_addr,
		   u32 dst_len, u8 *src_addr, u32 src_len)
{
	const u32 TEMP_BUFF_LEN = ((2048 >> 3) + CACHE_LINE_SIZE);

	u32 mod_bit_size	 = 2048;
	u32 mod_size_len_inbytes = mod_bit_size / 8;
	struct other_task_descriptor task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_n, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_e, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_src, TEMP_BUFF_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_dst, TEMP_BUFF_LEN);

	__rsa_padding(p_src, src_addr, src_len, mod_size_len_inbytes);
	__rsa_padding(p_n, n_addr, n_len, mod_size_len_inbytes);
	/*
	 * e usually 0x010001, so rsa padding, aka little-end ->
	 * big-end transfer is useless like, still do this rsa
	 * padding here, in case e is no longer 0x010001 some day
	 */
	memset(p_e, 0, mod_size_len_inbytes);
	__rsa_padding(p_e, e_addr, e_len, e_len);
	//memcpy(p_e, e_addr, e_len);

	memset((void *)&task0, 0x00, sizeof(task0));
	task0.task_id	     = CHANNEL_0;
	task0.common_ctl     = (ALG_RSA | (1U << 31));
	task0.asymmetric_ctl = (mod_bit_size >> 5);
	memcpy(task0.sg[0].source_addr, &p_e, 4);
	task0.sg[0].source_len = mod_size_len_inbytes;
	memcpy(task0.sg[1].source_addr, &p_n, 4);
	task0.sg[1].source_len = mod_size_len_inbytes;
	memcpy(task0.sg[2].source_addr, &p_src, 4);
	task0.sg[2].source_len = mod_size_len_inbytes;
	task0.data_len += task0.sg[0].source_len;
	task0.data_len += task0.sg[1].source_len;
	task0.data_len += task0.sg[2].source_len;

	memcpy(task0.sg[0].dest_addr, &p_dst, 4);
	task0.sg[0].dest_len = mod_size_len_inbytes;

	flush_cache((u32)&task0, sizeof(task0));
	flush_cache((u32)p_n, mod_size_len_inbytes);
	flush_cache((u32)p_e, mod_size_len_inbytes);
	flush_cache((u32)p_src, mod_size_len_inbytes);
	flush_cache((u32)p_dst, mod_size_len_inbytes);

	ss_set_drq((u32)&task0);
	ss_irq_enable(task0.task_id);
	ss_ctrl_start(ASYM_TRPE);
	ss_wait_finish(task0.task_id);
	ss_pending_clear(task0.task_id);
	ss_ctrl_stop();
	ss_irq_disable(task0.task_id);
	if (ss_check_err(task0.task_id)) {
		printf("SS %s fail 0x%x\n", __func__,
		       ss_check_err(task0.task_id));
		return -1;
	}

	invalidate_dcache_range((ulong)p_dst,
				(ulong)p_dst + mod_size_len_inbytes);
	__rsa_padding(dst_addr, p_dst, mod_bit_size / 64, mod_bit_size / 64);

	return 0;
}

int sunxi_aes_with_hardware(uint8_t *dst_addr, uint8_t *src_addr, int len,
			    uint8_t *key, uint32_t key_len,
			    uint32_t symmetric_ctl, uint32_t c_ctl)
{
	uint8_t __aligned(64) iv[16]			 = { 0 };
	struct other_task_descriptor task0 __aligned(64) = { 0 };
	uint32_t cts_size, destination_len;
	ALLOC_CACHE_ALIGN_BUFFER(uint8_t, key_map, 64);

	memset(key_map, 0, 64);
	if (key) {
		memcpy(key_map, key, key_len);
	}
	memset((void *)&task0, 0x00, sizeof(task0));

	cts_size	= ALIGN(len, 4);
	destination_len = cts_size;

	task0.task_id	    = 0;
	task0.common_ctl    = (1U << 31) | c_ctl;
	task0.symmetric_ctl = symmetric_ctl;
	memcpy(task0.key_addr, &key_map, 4);
	memcpy(task0.iv_addr, &iv, 4);
	task0.data_len = len;
	memcpy(task0.sg[0].source_addr, &src_addr, 4);
	task0.sg[0].source_len = cts_size;
	memcpy(task0.sg[0].dest_addr, &dst_addr, 4);
	task0.sg[0].dest_len = destination_len;

	//flush&clean cache
	flush_cache((u32)iv, ALIGN(sizeof(iv), CACHE_LINE_SIZE));
	flush_cache((u32)&task0, sizeof(task0));
	flush_cache((u32)key_map, ALIGN(sizeof(key_map), CACHE_LINE_SIZE));
	flush_cache((u32)src_addr, ALIGN(len, CACHE_LINE_SIZE));
	flush_cache((u32)dst_addr, ALIGN(len, CACHE_LINE_SIZE));

	ss_set_drq(((u32)&task0));
	ss_irq_enable(CHANNEL_0);
	ss_ctrl_start(SYMM_TRPE);
	ss_wait_finish(CHANNEL_0);
	ss_pending_clear(CHANNEL_0);
	ss_ctrl_stop();
	ss_irq_disable(CHANNEL_0);
	ss_set_drq(0);
	if (ss_check_err(CHANNEL_0)) {
		printf("SS %s fail 0x%x\n", __func__, ss_check_err(CHANNEL_0));
		return -1;
	}

	invalidate_dcache_range((ulong)dst_addr, ((ulong)dst_addr) + len);

	return 0;
}

int sunxi_trng_gen(u8 *rng_buf, u32 rng_byte)
{
	struct hash_task_descriptor task0 __aligned(CACHE_LINE_SIZE) = { 0 };
	/* sha256  2word, sha512 4word*/
	ALLOC_CACHE_ALIGN_BUFFER(u8, p_sign, CACHE_LINE_SIZE);
#define RNG_LEN 32
	memset(p_sign, 0, CACHE_LINE_SIZE);

	memset(&task0, 0, sizeof(task0));
	task0.ctrl = (CHANNEL_0 << CHN) | (0x1 << LPKG) | (0x0 << DLAV) |
		     (0x1 << IE);
	task0.cmd = (SUNXI_TRNG<<8)|(SUNXI_SHA256 << 0);
	memcpy(task0.sg[0].dest_addr, &p_sign, 4);
	task0.sg[0].dest_len = RNG_LEN;

	flush_cache(((u32)&task0), ALIGN(sizeof(task0), CACHE_LINE_SIZE));
	flush_cache((u32)p_sign, CACHE_LINE_SIZE);

	ss_set_drq((u32)&task0);
	ss_irq_enable(CHANNEL_0);
	ss_ctrl_start(HASH_RBG_TRPE);
	ss_wait_finish(CHANNEL_0);
	ss_pending_clear(CHANNEL_0);
	ss_ctrl_stop();
	ss_irq_disable(CHANNEL_0);
	ss_set_drq(0);
	if (ss_check_err(CHANNEL_0)) {
		printf("SS %s fail 0x%x\n", __func__, ss_check_err(CHANNEL_0));
		return -1;
	}

	invalidate_dcache_range((ulong)p_sign,
				((ulong)p_sign) + CACHE_LINE_SIZE);
	/*copy data*/
	memcpy(rng_buf, p_sign, RNG_LEN);
	return 0;
}

int sunxi_create_rssk(u8 *rssk_buf, u32 rssk_byte)
{
	return sunxi_trng_gen(rssk_buf, rssk_byte);
}

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
