/*
 * Copyright (c) 2015-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch_helpers.h>
#include <endian.h>
#include <mmio.h>
#include <debug.h>
#include <assert.h>
#include <errno.h>
#include <bl_common.h>
#include <platform.h>
#include <platform_def.h>
#include <platform_common_def.h>
#include <delay_timer.h>
#include <cv_private.h>
#include <xlat_tables.h>
#include <cv_sd.h>
#include <cli.h>
#include <console.h>
#include <string.h>
#include <cv_efuse.h>
#include "cv_usb.h"
#include "cryptodma.h"
#include "cv_private.h"
#include "cv_security.h"

#include "mbedtls/sha256.h"

#define u32 uint32_t
#define u64 uint64_t
#define u8	uint8_t

#define MAP_DEVICE	MAP_REGION_FLAT(0, 0x100000000,		\
					MT_DEVICE | MT_RW | MT_SECURE)
#define MAP_DRAM	MAP_REGION_FLAT(0x100000000, 0x80000000,	\
					MT_MEMORY | MT_RW | MT_SECURE)
#define MAP_IO_DATA	MAP_REGION_FLAT(BM_IO_BUF_BASE, BM_IO_BUF_SIZE, \
					MT_DEVICE | MT_RW | MT_SECURE)

/*
 * Table of regions for various BL stages to map using the MMU.
 * This doesn't include TZRAM as the 'mem_layout' argument passed to
 * arm_configure_mmu_elx() will give the available subset of that,
 */
#ifdef IMAGE_BL1
static const mmap_region_t plat_bm_mmap[] = {
	MAP_DEVICE,
	MAP_DRAM,
	MAP_IO_DATA,
	{0},
};
#else
static const mmap_region_t plat_bm_mmap[] = {
	MAP_DEVICE,
	MAP_DRAM,
	{0},
	};
#endif
/*******************************************************************************
 * Macro generating the code for the function setting up the pagetables as per
 * the platform memory map & initialize the mmu, for the given exception level
 ******************************************************************************/

#define DEFINE_CONFIGURE_MMU_EL(_el)					\
	void bm_configure_mmu_el##_el(unsigned long total_base,	\
				   unsigned long total_size,		\
				   unsigned long ro_start,		\
				   unsigned long ro_limit,		\
				   unsigned long coh_start,		\
				   unsigned long coh_limit)		\
	{								\
		mmap_add_region(total_base, total_base,			\
				total_size,				\
				MT_MEMORY | MT_RW | MT_SECURE);		\
		mmap_add_region(ro_start, ro_start,			\
				ro_limit - ro_start,			\
				MT_MEMORY | MT_RO | MT_SECURE);		\
		mmap_add_region(coh_start, coh_start,			\
				coh_limit - coh_start,			\
				MT_DEVICE | MT_RW | MT_SECURE);		\
		mmap_add(plat_bm_mmap);				\
		init_xlat_tables();					\
									\
		enable_mmu_el##_el(0);					\
	}

/* Define EL1 and EL3 variants of the function initialising the MMU */
DEFINE_CONFIGURE_MMU_EL(1)
DEFINE_CONFIGURE_MMU_EL(3)

int keep_mmu_disabled;

uint32_t system_counter_freq = SYS_COUNTER_FREQ_IN_TICKS;

int plat_cryptodma_des(uintptr_t src, uintptr_t des, uint64_t len, unsigned char *key, unsigned char *iv, int algo)
{
	uint32_t data;
	uint32_t ts;

	__aligned(32) uint32_t dma_descriptor[22];

	INFO("plat_cryptodma_des, src=%p, des=%p, image_size=0x%x\n",
						(void *)src, (void *)des, (int)len);

	// Prepare descriptor
	memset(dma_descriptor, 0, sizeof(dma_descriptor));

	dma_descriptor[CRYPTODMA_CTRL] = DES_USE_DESCRIPTOR_IV | DES_USE_DESCRIPTOR_KEY | algo | 0xF;
	dma_descriptor[CRYPTODMA_CIPHER] = AES_KEY_MODE << 3 | CBC_ENABLE << 1 | DECRYPT_ENABLE;

	dma_descriptor[CRYPTODMA_SRC_ADDR_L] = (uint32_t)((uint64_t)src & 0xFFFFFFFF);
	dma_descriptor[CRYPTODMA_SRC_ADDR_H] = (uint32_t)((uint64_t)src >> 32);

	dma_descriptor[CRYPTODMA_DST_ADDR_L] = (uint32_t)((uint64_t)des & 0xFFFFFFFF);
	dma_descriptor[CRYPTODMA_DST_ADDR_H] = (uint32_t)((uint64_t)des >> 32);

	dma_descriptor[CRYPTODMA_DATA_AMOUNT_L] = (uint32_t)((uint64_t)len & 0xFFFFFFFF);
	dma_descriptor[CRYPTODMA_DATA_AMOUNT_H] = (uint32_t)((uint64_t)len >> 32);

	if (algo == DES_USE_BYPASS) {
		memset(&dma_descriptor[CRYPTODMA_KEY], 0, 16);
		memset(&dma_descriptor[CRYPTODMA_IV], 0, 16);
	} else {
	memcpy(&dma_descriptor[CRYPTODMA_KEY], key, 16);
	memcpy(&dma_descriptor[CRYPTODMA_IV], iv, 16);
	}

	// Set cryptodma control
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_INT_MASK, 0x3);
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DES_BASE_L, (uint32_t)((uint64_t)dma_descriptor & 0xFFFFFFFF));
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DES_BASE_H, (uint32_t)((uint64_t)dma_descriptor >> 32));

	data = mmio_read_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DMA_CTRL);

	flush_dcache_range((unsigned long)dma_descriptor,
			sizeof(dma_descriptor));

	// Clear interrupt
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_WR_INT, 0x1);
	// Trigger cryptodma engine
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DMA_CTRL,
				  DMA_WRITE_MAX_BURST << 24 |
				  DMA_READ_MAX_BURST << 16 |
				  DMA_DESCRIPTOR_MODE << 1 | DMA_ENABLE);

	ts = get_timer(0);
	do {
		data = mmio_read_32(SEC_CRYPTODMA_BASE + CRYPTODMA_WR_INT);
		INFO("INT status 0x%x\n", data);
		if (get_timer(ts) >= 1000) {
			ERROR("Decryption timeout\n");
			break;
		}
	} while (data == 0);

	return 0;
}

int plat_cryptodma_exec(uintptr_t src, uintptr_t dst, uint64_t len, spacc_exec_config *config)
{
	__attribute__((aligned(64))) uint32_t dma_descriptor[32] = { 0 };

	uint32_t status;
	u32 ts = 0;
	INFO("AES/0x%lx/0x%lx/0x%lx\n", src, dst, len);
	
	// Prepare descriptor
	dma_descriptor[CRYPTODMA_CTRL] =  DES_USE_DESCRIPTOR_IV | DES_USE_AES | 0xF|IV_OUT0_SELECT;
	if(config->otp==USE_OTP_KEY){
		dma_descriptor[CRYPTODMA_CTRL] |= OTP_KEY_SEL;
	}else{
		dma_descriptor[CRYPTODMA_CTRL] |= DES_USE_DESCRIPTOR_KEY;
		switch(config->key_mode){
			case AES_128BIT:
				memcpy(&dma_descriptor[CRYPTODMA_KEY],(void*)config->key, 16);
				break;
			case AES_192BIT:
				memcpy(&dma_descriptor[CRYPTODMA_KEY],(void*)config->key, 24);
				break;
			case AES_256BIT:
				memcpy(&dma_descriptor[CRYPTODMA_KEY],(void*)config->key, 32);
				break;
			default:
				break;
		}
	}
	dma_descriptor[CRYPTODMA_CIPHER]|=config->key_mode<<3;
	NOTICE("CONFIG->KEY_MODE:%d\n",config->key_mode);
	NOTICE("CONFIG->MODE:%d\n",config->mode);
	switch(config->mode){
		case AES_ECB:
			break;
		case AES_CBC:
			dma_descriptor[CRYPTODMA_CIPHER] |= CBC_ENABLE << 1;
			break;
		case AES_CTR:
			dma_descriptor[CRYPTODMA_CIPHER] |= 0x1 << 2;
			break;
		default:
			break;
	}
	memcpy(&dma_descriptor[CRYPTODMA_IV], (const void *)config->iv, 16);
	if (config->action == ENCRYPTION) {
		dma_descriptor[CRYPTODMA_CIPHER] |= 1;
	} 

	dma_descriptor[CRYPTODMA_SRC_ADDR_L] = (uint32_t)(src & 0xFFFFFFFF);
	dma_descriptor[CRYPTODMA_SRC_ADDR_H] = (uint32_t)(src >> 32);

	dma_descriptor[CRYPTODMA_DST_ADDR_L] = (uint32_t)(dst & 0xFFFFFFFF);
	dma_descriptor[CRYPTODMA_DST_ADDR_H] = (uint32_t)(dst >> 32);

	dma_descriptor[CRYPTODMA_DATA_AMOUNT_L] = (uint32_t)(len & 0xFFFFFFFF);
	dma_descriptor[CRYPTODMA_DATA_AMOUNT_H] = (uint32_t)(len >> 32);

	// Set cryptodma control
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_INT_MASK, 0x3);
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DES_BASE_L,
		      (uint32_t)((uint64_t)dma_descriptor & 0xFFFFFFFF));
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DES_BASE_H,
		      (uint32_t)((uint64_t)dma_descriptor >> 32));

	status = mmio_read_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DMA_CTRL);

	flush_dcache_range((unsigned long)dma_descriptor,
			   sizeof(dma_descriptor));
	flush_dcache_range((uintptr_t)src, len);
	flush_dcache_range((uintptr_t)dst, len);
	// Clear interrupt
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_WR_INT, 0x3);
	// Trigger cryptodma engine
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DMA_CTRL,
		      DMA_WRITE_MAX_BURST << 24 | DMA_READ_MAX_BURST << 16 |
			      DMA_DESCRIPTOR_MODE << 1 | DMA_ENABLE);
	NOTICE("dma_descriptor[CRYPTODMA_CTRL]:%x\n",
	       dma_descriptor[CRYPTODMA_CTRL]);
	NOTICE("dma_descriptor[CRYPTODMA_CIPHER]:%x\n",
	       dma_descriptor[CRYPTODMA_CIPHER]);
	NOTICE("SEC_CRYPTODMA_BASE[%x]:%x\n", SEC_CRYPTODMA_BASE + 0x14,
	       mmio_read_32(SEC_CRYPTODMA_BASE + 0x14));
	NOTICE("SEC_CRYPTODMA_BASE[CTRL]%x:%x\n",
	       SEC_CRYPTODMA_BASE + CRYPTODMA_DMA_CTRL,
	       mmio_read_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DMA_CTRL));

	do {
		status = mmio_read_32(SEC_CRYPTODMA_BASE + CRYPTODMA_WR_INT);
		INFO("INT status 0x%x\n", status);
		if (get_timer(ts) >= 300000) {
			ERROR("exec timeout\n");
			return -1;
		}
	} while (status == 0);

	

	return 0;
}

static inline void setup_src(u32 *dma_descriptor, uintptr_t src, u32 len)
{
	flush_dcache_range((unsigned long)src, len);

	dma_descriptor[CRYPTODMA_SRC_ADDR_L] = (uint32_t)((uint64_t)src & 0xFFFFFFFF);
	dma_descriptor[CRYPTODMA_SRC_ADDR_H] = (uint32_t)((uint64_t)src >> 32);

	dma_descriptor[CRYPTODMA_SRC_LEN] = len;
}

// some algo like sha256/sha1/sm3 needn't dst
#define setup_dst(dst)\
do {\
	dma_descriptor[CRYPTODMA_DST_ADDR_L] = (uint32_t)((uint64_t)(dst) & 0xFFFFFFFF);\
	dma_descriptor[CRYPTODMA_DST_ADDR_H] = (uint32_t)((uint64_t)(dst) >> 32);\
} while (0)

int trigger_cryptodma_work_and_wait_finish(u32 *dma_descriptor, int irq_enable)
{
	u32 data;
	u32 ts;

	// set exec dma descriptor addr
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DES_BASE_L, (uint32_t)((uint64_t)dma_descriptor & 0xFFFFFFFF));
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DES_BASE_H, (uint32_t)((uint64_t)dma_descriptor >> 32));

	// Set cryptodma dma control
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_INT_MASK, 0x7);

	// Clear interrupt
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_WR_INT, 0x7);

	// Trigger cryptodma engine
	mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DMA_CTRL,
		      DMA_WRITE_MAX_BURST << 24 | DMA_READ_MAX_BURST << 16 | DMA_DESCRIPTOR_MODE << 1 | DMA_ENABLE);

	ts = get_timer(0);
	do {
		data = mmio_read_32(SEC_CRYPTODMA_BASE + CRYPTODMA_WR_INT);
#if DEBUG
		ERROR("INT status 0x%x\n", data);
#endif
		if (get_timer(ts) >= 1000) {
			ERROR("exec timeout\n");
			return -1;
		}
	} while (data == 0);

	return 0;
}

int plat_cryptodma_base64(uintptr_t src, uint64_t len, uintptr_t dst, uint32_t customer_code, uint32_t action)
{
	__aligned(64) u32 dma_descriptor[32] = {0};
	int result_len;

	INFO("%s: src=0x%lx, len=0x%lx dst=0x%lx\n", __func__, src, len, dst);

	dma_descriptor[CRYPTODMA_CTRL] = DES_USE_BASE64 | 0xF;
	if (action == 1) {
		dma_descriptor[CRYPTODMA_CIPHER] = 0x1;
		result_len = (len + (3 - 1)) / 3 * 4;
	} else {
		result_len = (len / 4) * 3;
	}
	dma_descriptor[CRYPTODMA_DST_LEN] = result_len;

	setup_src(dma_descriptor, src, len);
	setup_dst(dst);
	flush_dcache_range((unsigned long)dma_descriptor, 32 * sizeof(uint32_t));

	if (customer_code)
		mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_BASE64_CUSTOMIZE_CODE, customer_code);

	if (trigger_cryptodma_work_and_wait_finish(dma_descriptor, 0))
		return -1;

	return result_len;
}

int plat_cryptodma_sha256_start(plat_cryptodma_sha256_t *ctx)
{
	INFO("%s:\n", __func__);

	memset(ctx, 0, sizeof(plat_cryptodma_sha256_t));

	ctx->state[0] = htobe32(0x6A09E667);
	ctx->state[1] = htobe32(0xBB67AE85);
	ctx->state[2] = htobe32(0x3C6EF372);
	ctx->state[3] = htobe32(0xA54FF53A);
	ctx->state[4] = htobe32(0x510E527F);
	ctx->state[5] = htobe32(0x9B05688C);
	ctx->state[6] = htobe32(0x1F83D9AB);
	ctx->state[7] = htobe32(0x5BE0CD19);

	return 0;
}

int plat_cryptodma_sha256_blocks(plat_cryptodma_sha256_t *ctx, const u8 *src, size_t len)
{
	__aligned(64) u32 dma_descriptor[32] = {0};
	u32 i;
	u32 result_size = 8;
	size_t alen;

	alen = len / 64 * 64;

	INFO("%s: S2 msg=%p, len=0x%lx alen=0x%lx\n", __func__, src, len, alen);
	if (alen != len) {
		ERROR("%s: len (%lu) must be multiple of 64 bytes\n", __func__, len);
		return -1;
	}

	ctx->total += len;

	dma_descriptor[CRYPTODMA_CTRL] = DES_USE_DESCRIPTOR_KEY | DES_USE_SHA | 0xF;
	dma_descriptor[CRYPTODMA_CIPHER] = (0x1 << 1);
	dma_descriptor[CRYPTODMA_CIPHER] |= 0x1;

	for (i = 0; i < result_size; i++)
		dma_descriptor[CRYPTODMA_KEY + i] = ctx->state[i];

	setup_src(dma_descriptor, (uintptr_t)src, len);
	flush_dcache_range((unsigned long)dma_descriptor, 32 * sizeof(uint32_t));

	if (trigger_cryptodma_work_and_wait_finish(dma_descriptor, 0))
		return -1;

	for (i = 0; i < result_size; i++)
		ctx->state[i] = mmio_read_32(SEC_CRYPTODMA_BASE + CRYPTODMA_SHA_PARA + i * 4);

	return 0;
}

int plat_cryptodma_sha256_finish(plat_cryptodma_sha256_t *ctx, const void *msg, u64 rlen, u8 digest[32])
{
	mbedtls_sha256_context mt_ctx;

	u32 pad_len;
	u64 bits_len_be;
	u8 block[128] = { 0 };
	u32 out[8] = { 0 };
	u8 *p;
	int i;

	INFO("%s: msg=%p, rlen=0x%lx digest=%p\n", __func__, msg, rlen, digest);

	if (rlen >= 64) {
		ERROR("%s: rlen (%lu) must be lesser than 64 bytes\n", __func__,
		      rlen);
		return -1;
	}

	if (!msg || !rlen) {
		msg = "";
		rlen = 0;
	}

	ctx->total += rlen;
	bits_len_be = htobe64(ctx->total * 8);

	if (rlen < 56)
		pad_len = 55 - rlen;
	else
		pad_len = 119 - rlen;

	INFO("%s: pad_len=%u total=0x%lx\n", __func__, pad_len, ctx->total);

	p = block;

	// copy rest data
	memcpy(p, msg, rlen);
	p += rlen;
	*p++ = 0x80; // pad bit 1
	p += pad_len; // pad '\0' * pad_len
	memcpy(p, &bits_len_be, sizeof(uint64_t)); // append the length of bits in big-endian

	// SW impl. is faster for one block
	mbedtls_sha256_init(&mt_ctx);
	mbedtls_sha256_starts_ret(&mt_ctx, 0);

	for (i = 0; i < 8; i++)
		mt_ctx.state[i] = be32toh(ctx->state[i]);

	mbedtls_sha256_update_ret(&mt_ctx, block, (rlen < 56) ? 64 : 128);

	for (i = 0; i < 8; i++)
		out[i] = be32toh(mt_ctx.state[i]);

	memcpy(digest, out, 32);

	return 0;
}

int plat_cryptodma_sha256(const void *msg, uint64_t len, uint8_t digest[32])
{
	plat_cryptodma_sha256_t ctx;
	u64 alen = 0;

	plat_cryptodma_sha256_start(&ctx);

	if (len > 64) {
		alen = len / 64 * 64;
		plat_cryptodma_sha256_blocks(&ctx, msg, alen);
		plat_cryptodma_sha256_finish(&ctx, msg + alen, len - alen, digest);
	} else {
		plat_cryptodma_sha256_finish(&ctx, msg, len, digest);
	}

	return 0;
}

static struct {
	uint32_t start;
	uint32_t size;
	uint32_t wlock_shift;
	uint32_t rlock_shift;
} efuse_lock_map[] = {
	[EFUSE_LOCK_KPUB0] = {0xA8, 32, 0, 8},
	[EFUSE_LOCK_DEBUG_PW] = {0xC8, 16, 2, 10},
	[EFUSE_LOCK_LOADER_EK] = {0xD8, 16, 4, 12},
	[EFUSE_LOCK_DEVICE_EK] = {0xE8, 16, 6, 14}
};

static int plat_efuse_is_not_secret(uint32_t addr)
{
	uint32_t lock_value = mmio_read_32(SEC_EFUSE_SHADOW_REG + EFUSE_LOCK_ADDR);
	int i;

	if (addr == EFUSE_LOCK_ADDR)
		return 1;

	for (i = 0; i < ARRAY_SIZE(efuse_lock_map); i++) {
		uint32_t start = efuse_lock_map[i].start;
		uint32_t end = efuse_lock_map[i].start + efuse_lock_map[i].size;
		uint32_t lock = (lock_value >> efuse_lock_map[i].rlock_shift) & 0x3;

		if (addr >= start && addr < end && !lock)
			return 1;
	}

	if (addr < EFUSE_SECRET_START_OFFSET)
		return 1;

	if (addr >= EFUSE_SECRET_END_OFFSET)
		return 1;

	return 0;
}

int64_t plat_efuse_read_safe(uint32_t addr)
{
	if (addr >= EFUSE_SIZE)
		return -EFAULT;

	if (addr % 4 != 0)
		return -EFAULT;

	if (plat_efuse_is_not_secret(addr))
		return mmio_read_32(SEC_EFUSE_SHADOW_REG + addr);

	return -EFAULT;
}

uint32_t plat_bm_get_uart_clock(void)
{
	return PLAT_BM_BOOT_UART_CLK_IN_HZ;
}

void plat_bm_init_i2c(void)
{
	printf("fix me: plat_bm_init_i2c\n");
}

void plat_bm_deinit_i2c(void)
{
	printf("fix me: plat_bm_deinit_i2c\n");
}

void do_wdt_reset(const char *file, unsigned int line, const char *assertion)
{
	mdelay(5000);

	ERROR("WDT RST ASSERT: %s, <%d>, %s\n", file, line, assertion);
	plat_wait_debug_port();
	console_flush();

	mmio_setbits_32(TOP_BASE + 0x8, 0x4); // wdt reset enable
	mmio_write_32(WATCHDOG_BASE + 0x4, 0x20);	// Set timeout range reigster
	mmio_write_32(WATCHDOG_BASE + 0xc, 0x76);	// Counter restart register
	mmio_write_32(WATCHDOG_BASE, 0x13);		// Response mode and enable WDT

	while (1)
		;
}

void do_wdt_reset_direct(const char *file, unsigned int line, const char *assertion)
{
	console_flush();

	mmio_setbits_32(TOP_BASE + 0x8, 0x4); // wdt reset enable
	mmio_write_32(WATCHDOG_BASE + 0x4, 0x20);	// Set timeout range reigster
	mmio_write_32(WATCHDOG_BASE + 0xc, 0x76);	// Counter restart register
	mmio_write_32(WATCHDOG_BASE, 0x13);		// Response mode and enable WDT
}

int plat_ddr_firewall_set(int region_idx, uintptr_t start, uintptr_t end)
{
	uint32_t a;

	INFO("%s: region_idx=%d start=0x%lx end=0x%lx\n", __func__,
	       region_idx, start, end);

	mmio_write_32(SEC_DDR_FW_DIRTY_REGION, SEC_DDR_FW_DIRTY_REGION_DEFAULT >> SEC_DDR_FW_ADDR_SHIFT);

	start &= 0x7fffffff;
	end &= 0x7fffffff;

	// Set secure dram region_idx
	mmio_write_32(SEC_DDR_FW_SECREGION_START_0 + 4 * region_idx,
		      (uint32_t)(start >> SEC_DDR_FW_ADDR_SHIFT));
	mmio_write_32(SEC_DDR_FW_SECREGION_END_0 + 4 * region_idx,
		      (uint32_t)(end >> SEC_DDR_FW_ADDR_SHIFT));

	// Allow all AID
	mmio_write_32(SEC_DDR_FW_AID_CTRL0, SEC_DDR_FW_AID_CTRL_EN_ALL);
	mmio_write_32(SEC_DDR_FW_AID_CTRL1, SEC_DDR_FW_AID_CTRL_EN_ALL);
	mmio_write_32(SEC_DDR_FW_AID_CTRL2, SEC_DDR_FW_AID_CTRL_EN_ALL);
	mmio_write_32(SEC_DDR_FW_AID_CTRL3, SEC_DDR_FW_AID_CTRL_EN_ALL);

	// Enable secure region
	a = mmio_read_32(SEC_DDR_FW_CTRL0);
	a |= SEC_DDR_FW_CTRL0_EN;
	a |= 1 << (SEC_DDR_FW_CTRL0_SECREGION_EN_SHIFT + region_idx);
	a |= SEC_DDR_FW_CTRL0_AID_CTRL_REGION_ALL << SEC_DDR_FW_CTRL0_AID_CTRL_REGION_SHIFT;
	mmio_write_32(SEC_DDR_FW_CTRL0, a);

	return 0;
}

void bm_storage_boot_loader_version(uint32_t addr)
{
	int size_version, size_time;

	size_version = strlen(version_string);
	for (int i = 0; i < size_version; i++) {
		mmio_write_8(addr + i, version_string[i]);
	}

	mmio_write_8(addr + size_version, ' ');

	size_time = strlen(build_message);
	for (int i = 0; i < size_time; i++) {
		mmio_write_8(addr + size_version + 1 + i, build_message[i]);
	}

	mmio_write_8(addr + size_version + 1 + size_time, '\0');
}
