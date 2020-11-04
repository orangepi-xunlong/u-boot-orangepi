/*
 * (C) Copyright 2018 allwinnertech  <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef  __SUNXI_SPRITE_VERIFY_H__
#define  __SUNXI_SPRITE_VERIFY_H__

#include <common.h>

extern uint add_sum(void *buffer, uint length);

extern uint sunxi_sprite_part_rawdata_verify(uint base_start, long long base_bytes);

extern uint sunxi_sprite_part_sparsedata_verify(void);

extern uint sunxi_sprite_generate_checksum(void *buffer, uint length, uint src_sum);

extern int sunxi_sprite_verify_checksum(void *buffer, uint length, uint src_sum);

extern int sunxi_sprite_verify_dlmap(void *buffer);

extern int sunxi_sprite_verify_mbr(void *buffer);

extern int sunxi_sprite_read_mbr(void *buffer, uint mbr_copy);

extern int sunxi_sprite_verify_mbr_from_flash(u32 blocks, u32 mbr_copy);

#endif

