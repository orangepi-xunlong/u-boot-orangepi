/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 */
#ifndef __SUNXI_VERIFY_BOOT_INFO_H__
#define __SUNXI_VERIFY_BOOT_INFO_H__

typedef enum {
	KM_VERIFIED_BOOT_VERIFIED    = 0,
	KM_VERIFIED_BOOT_SELF_SIGNED = 1,
	KM_VERIFIED_BOOT_UNVERIFIED  = 2,
	KM_VERIFIED_BOOT_FAILED      = 3,
} keymaster_verified_boot_t;

typedef enum {
	SUNXI_VB_INFO_HASH = (1<<0),
	SUNXI_VB_INFO_KEY  = (1<<1),
	SUNXI_VB_INFO_LOCK = (1<<2),
	SUNXI_VB_INFO_BOOTSTATE  = (1<<3),
} verify_boot_info_mask_t;

void sunxi_set_verify_boot_blob(uint32_t mask,
		const uint8_t *data, size_t data_len);
void sunxi_set_verify_boot_number(uint32_t mask,
		uint32_t data);
int sunxi_keymaster_verify_boot_params_install(void);

#endif /* ifndef __SUNXI_VERIFY_BOOT_INFO_H__ */
