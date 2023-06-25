
// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 */

#ifndef _SYS_PARTITION_H
#define _SYS_PARTITION_H

#include <common.h>
#include <sunxi_flash.h>
#include <private_uboot.h>
#include <sunxi_board.h>

int sunxi_probe_partition_map(void);
int sunxi_partition_get_partno_byname(const char *part_name);
int sunxi_partition_get_info(const char *part_name, disk_partition_t *info);
uint sunxi_partition_get_offset_byname(const char *part_name);
lbaint_t sunxi_partition_get_offset(int part_index);
int sunxi_partition_get_info_byname(const char *part_name, uint *part_offset,
				    uint *part_size);

#define GPT_SIGNATURE "EFI PART"

#endif
