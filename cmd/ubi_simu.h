/*
 * (C) Copyright 2007-2017
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * zhongguizhao <zhongguizhao@allwinnertech.com>
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
 * You should have received a copy of the GNU General Public License
 * along with this program.
 */
#ifndef  __UBI_SIMU_H__
#define  __UBI_SIMU_H__

#include <common.h>
#include <command.h>
#include <exports.h>
#include <memalign.h>
#include <mtd.h>
#include <nand.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/err.h>
#include <ubi_uboot.h>
#include <linux/errno.h>
#include <mtd/ubi-user.h>
#include <linux/crc32.h>

int ubi_simu_create_vol(char *volume, int64_t size, int dynamic, int vol_id);
int ubi_simu_volume_continue_write(char *volume, void *buf, size_t size);
int ubi_simu_volume_begin_write(char *volume, void *buf, size_t size, size_t full_size);
int ubi_simu_volume_write(char *volume, void *buf, size_t size);
struct ubi_device * ubi_simu_part(char *part_name, const char *vid_header_offset);
int sunxi_ubi_simu_volume_read(char *volume, loff_t offp, char *buf, size_t size);
#endif
