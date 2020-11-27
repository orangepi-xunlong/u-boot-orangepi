/*
 * (C) Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __SUNXI_CRYPTO_H_
#define __SUNXI_CRYPTO_H_

int  sunxi_md5_calc(u8 *dst_addr, u32 dst_len,
					u8 *src_addr, u32 src_len);

int sunxi_create_rssk(u8 *rssk_buf, u32 rssk_byte);

#endif	/* __SUNXI_CRYPTO_H_ */
