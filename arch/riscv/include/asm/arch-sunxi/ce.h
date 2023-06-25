/*
 * (C) Copyright 2018
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef _SUNXI_CE_H
#define _SUNXI_CE_H

#include <linux/types.h>
#include <config.h>
#include <asm/arch/cpu.h>

#if defined(CONFIG_SUNXI_CE_20)
#include "ce_2.0.h"
#elif defined(CONFIG_SUNXI_CE_10)
#include "ce_1.0.h"
#elif defined(CONFIG_SUNXI_CE_21)
#include "ce_2.1.h"
#elif defined(CONFIG_SUNXI_CE_23)
#include "ce_2.3.h"
#else
#error "Unsupported plat"
#endif

#ifndef __ASSEMBLY__
/*aes*/
/*enc_flag*/
#define AES_ENCRYPT	(0)
#define AES_DECRYPT	(1)

/*aes_mode*/
#define AES_MODE_ECB	(0)
#define AES_MODE_CBC	(1)

/*key_type*/
#define KEY_TYPE_SSK	(0)

struct aes_crypt_info_t {
	void *src_buf;
	u32 src_len;
	void *dst_buf;
	u32 dst_len;
	u8 aes_mode;
	u8 key_type;
	u8 enc_flag;
};

/*sm2*/
#define SM2_SIZE_BYTE	(32)

#define ECDH_USER_A			(0)
#define ECDH_USER_B			(1)

struct sunxi_sm2_ctx_t {
	u32 mode;
	u32 sm2_size;
	u32 k_len;
	void *k;
	void *n;
	void *p;
	void *a;
	void *b;
	void *gx;
	void *gy;
	void *px;
	void *py;
	void *d;
	void *r;
	void *s;
	void *h;
	void *m;
	u32 m_len;
	void *e;
	u32 e_len;
	void *cx;
	void *cy;
	void *kx;
	void *ky;
	void *ra;
	void *pxa;
	void *pya;
	void *rb;
	void *pxb;
	void *pyb;
	void *da;
	void *db;
	void *rax;
	void *ray;
	void *rbx;
	void *rby;
	void *out;
};

void sunxi_ss_open(void);
void sunxi_ss_close(void);
int  sunxi_sha_calc(u8 *dst_addr, u32 dst_len,
					u8 *src_addr, u32 src_len);
int sunxi_md5_calc(u8 *dst_addr, u32 dst_len, u8 *src_addr, u32 src_len);
int sunxi_hash_test(u8 *dst_addr, u32 dst_len,
					u8 *src_addr, u32 src_leni, u32 sha_type);

s32 sunxi_rsa_calc(u8 *n_addr,   u32 n_len,
				   u8 *e_addr,   u32 e_len,
				   u8 *dst_addr, u32 dst_len,
				   u8 *src_addr, u32 src_len);

int sunxi_do_aes_crypt(struct aes_crypt_info_t *aes_info);

s32 sunxi_sm2_sign_verify_test(struct sunxi_sm2_ctx_t *sm2_ctx);
s32 sm2_crypto_gen_cxy_kxy(struct sunxi_sm2_ctx_t *sm2_ctx);
s32 sm2_ecdh_gen_key_rx_ry(struct sunxi_sm2_ctx_t *sm2_ctx, int flag);
s32 sm2_ecdh_gen_ux_uy(struct sunxi_sm2_ctx_t *sm2_ctx);
#endif

#endif /* _SUNXI_CE_H */
