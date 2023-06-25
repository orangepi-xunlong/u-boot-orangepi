/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef _SS_H_
#define _SS_H_

#include <config.h>
#include <asm/arch/cpu.h>
#include <memalign.h>

#define SS_N_BASE       SUNXI_SS_BASE              /*non security */
#define SS_S_BASE       (SUNXI_SS_BASE+0x800)      /*security */

#define SS_TDQ          (SS_N_BASE + 0x00 + 0x800*ss_base_mode)
#define SS_ICR          (SS_N_BASE + 0x08 + 0x800*ss_base_mode)
#define SS_ISR          (SS_N_BASE + 0x0C + 0x800*ss_base_mode)
#define SS_TLR          (SS_N_BASE + 0x10 + 0x800*ss_base_mode)
#define SS_TSR          (SS_N_BASE + 0x14 + 0x800*ss_base_mode)
#define SS_ERR          (SS_N_BASE + 0x18 + 0x800*ss_base_mode)
#define SS_TPR          (SS_N_BASE + 0x1C + 0x800*ss_base_mode)
#define SS_VER          (SS_N_BASE + 0x90)

/*security */
#define SS_S_TDQ        (SS_S_BASE + 0x00)
#define SS_S_CTR        (SS_S_BASE + 0x04)
#define SS_S_ICR        (SS_S_BASE + 0x08)
#define SS_S_ISR        (SS_S_BASE + 0x0C)
#define SS_S_TLR        (SS_S_BASE + 0x10)
#define SS_S_TSR        (SS_S_BASE + 0x14)
#define SS_S_ERR        (SS_S_BASE + 0x18)
#define SS_S_TPR        (SS_S_BASE + 0x1C)

/*non security */
#define SS_N_TDQ        (SS_N_BASE + 0x00)
#define SS_N_ICR        (SS_N_BASE + 0x08)
#define SS_N_ISR        (SS_N_BASE + 0x0C)
#define SS_N_TLR        (SS_N_BASE + 0x10)
#define SS_N_TSR        (SS_N_BASE + 0x14)
#define SS_N_ERR        (SS_N_BASE + 0x18)
#define SS_N_TPR        (SS_N_BASE + 0x1C)

#define SHA1_160_MODE	0
#define SHA2_256_MODE	1

#define TRANG_BYTE_LEN (32)

/*alg type*/
#define ALG_AES		(0x0)
#define ALG_SM4		(0x3)
#define ALG_SHA256	(0x13)
#define ALG_SHA512	(0x15)
#define ALG_RSA		(0x20)
#define ALG_ECC		(0x21)
#define ALG_SM2		(0x22)
#define ALG_MD5		(0x10)
#define ALG_TRANG	(0x1C)


/*ctrl
*/
#define CHN  (0) /*channel id*/
#define IVE  (8)
#define LPKG (12) /*last package*/
#define DLAV (13) /*data length valid|━?┐?│?o：?：＜?????：1,D：：：＜a：|：：?a1,*/
#define IE   (16)

#define SUNXI_MD5    (0)
#define SUNXI_SHA1   (1)
#define SUNXI_SHA244 (2)
#define SUNXI_SHA256 (3)
#define SUNXI_SHA384 (4)
#define SUNXI_SHA512 (5)
#define SUNXI_SM3    (6)

#define SUNXI_TRNG   (2)

/*sm2 mode*/
#define SM2_MODE_ENC			(0)
#define SM2_MODE_DEC			(1)
#define SM2_MODE_SIGN			(2)
#define SM2_MODE_VERIFY			(3)
#define SM2_MODE_KEY_EXCHANGE	(4)

/*cmd
*/
#define HASH_SEL 0
#define HME 4
#define RGB_SEL 8
#define SUB_CMD 16

/*CE_TLR
*/
#define SYMM_TRPE 0
#define HASH_RBG_TRPE 1
#define ASYM_TRPE 2
#define RAES_TRPE 3

/*CE_ISR
*/
#define SUCCESS 0x1
#define FAIL 0x2
#define CLEAN 0x3

#define CHANNEL_0 0
#define CHANNEL_1 1
#define CHANNEL_2 2
#define CHANNEL_3 3

/* AES SETTINGS */
#define _SUNXI_AES_CFG (0)
#define SS_DIR_ENCRYPT 0
#define SS_DIR_DECRYPT 1
#define SS_KEY_SELECT_INPUT        (0)
#define SS_KEY_SELECT_SSK          (1)
#define SS_KEY_SELECT_HUK          (2)
#define SS_KEY_SELECT_RSSK         (3)

#define SS_AES_MODE_ECB         (0)
#define SS_AES_MODE_CBC         (1)
#define SS_AES_MODE_CTR         (2)
#define SS_AES_MODE_CTS         (3)
#define SS_AES_MODE_OFB         (4)
#define SS_AES_MODE_CFB         (5)
#define SS_AES_MODE_CBCMAC      (6)

#define SS_AES_KEY_128BIT       (0)
#define SS_AES_KEY_192BIT       (1)
#define SS_AES_KEY_256BIT       (2)

typedef struct sg {
	uint addr;
	uint length;
} sg;

typedef struct descriptor_queue {
	uint task_id;
	uint common_ctl;
	uint symmetric_ctl;
	uint asymmetric_ctl;
	uint key_descriptor;
	uint iv_descriptor;
	uint ctr_descriptor;
	uint data_len;
	sg source[8];
	sg destination[8];
	uint next_descriptor;
	uint reserved[3];
	/*make sure size is cache align*/
	uint padding[(ALIGN(176, CACHE_LINE_SIZE) - 176) / sizeof(uint)];
} task_queue;

#if defined(SHA256_MULTISTEP_PACKAGE) || defined(SHA512_MULTISTEP_PACKAGE)
int sunxi_hash_init(u8 *dst_addr, u8 *src_addr, u32 src_len, u32 total_len);
int sunxi_hash_update(u8 *dst_addr, u8 *src_addr, u32 src_len, u32 total_len);
int sunxi_hash_final(u8 *dst_addr, u8 *src_addr, u32 src_len, u32 total_len);
#endif

#endif    /*  #ifndef _SS_H_  */
