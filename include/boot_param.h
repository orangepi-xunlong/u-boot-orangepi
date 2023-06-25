/*
 * include/boot_param.h
 *
 * Copyright (c) 2021-2025 Allwinnertech Co., Ltd.
 * Author: lujianliang <lujianliang@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef __BOOT_PARAM_H
#define __BOOT_PARAM_H

#define BOOT_PARAM_MAGIC		"bootpara"
#define BOOT_PARAM_SIZE			4096
#define CHECK_SUM			0x5F0A6C39

struct sunxi_boot_parameter_header {
	u8 magic[8]; //bootpara
	u32 version; // describe the region version
	u32 check_sum;
	u32 length;
	u8 reserved[12];
};

//---total len 是4K
struct sunxi_boot_param_region {
	struct sunxi_boot_parameter_header header;//32
	char sdmmc_info[256];
	char nand_info[256];
	char spiflash_info[256];
	char ddr_info[512];
	u8 reserved[2784];// = 4096 - sdmmc_size - nand_size - spi_size - ddr_size - 32
};

#endif
