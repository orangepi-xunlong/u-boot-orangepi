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
#else
#error "Unsupported plat"
#endif

#ifndef __ASSEMBLY__
void sunxi_ss_open(void);
void sunxi_ss_close(void);
int  sunxi_sha_calc(u8 *dst_addr, u32 dst_len,
					u8 *src_addr, u32 src_len);
int sunxi_md5_calc(u8 *dst_addr, u32 dst_len, u8 *src_addr, u32 src_len);

s32 sunxi_rsa_calc(u8 *n_addr,   u32 n_len,
				   u8 *e_addr,   u32 e_len,
				   u8 *dst_addr, u32 dst_len,
				   u8 *src_addr, u32 src_len);
#endif

#endif /* _SUNXI_CE_H */
