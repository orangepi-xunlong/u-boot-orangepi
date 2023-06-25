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
#include <common.h>
#include <sunxi_spinand.h>
#include "spinand_ecc_op.h"
enum badblock_flag {
	FIRST_PAGE = 0,
	FIRST_TWO_PAGE,
	LAST_PAGE,
	LAST_TWO_PAGE,
};

struct __NandPhyInfoPar_t ext_spinand_idts[] = {
	/*-----------------------------XTX--------------------------*/
	{
		/* XT26G01C */
		.NandID = {0x0B, 0x11, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
			.DieCntPerChip = 1,
			.SectCntPerPage = 4,
			.PageCntPerBlk = 64,
			.BlkCntPerDie = 1024,
			.OperationOpt = NAND_DUAL_READ | NAND_QUAD_READ | NAND_MULTI_READ |
				NAND_MULTI_PROGRAM | NAND_MAX_BLK_ERASE_CNT,
			.AccessFreq = 100,
			.SpiMode = 0,
			.pagewithbadflag = FIRST_PAGE, /* the 1st page */
			.spi_nand_function = &spinand_function,
			.MultiPlaneBlockOffset = 1,
			.MaxEraseTimes = 65000,
			.EccType = BIT4_LIMIT5_TO_8_ERR15,
			.EccProtectedType = SIZE16_OFF0_LEN16,
			.ecc_status_shift = ECC_STATUS_SHIFT_4,
	},
	{
		/* XT26G02E */
		.NandID = {0x2c, 0x24, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
			.DieCntPerChip = 1,
			.SectCntPerPage = 4,
			.PageCntPerBlk = 64,
			.BlkCntPerDie = 2048,
			.OperationOpt = NAND_DUAL_READ | NAND_MULTI_READ |
				NAND_MULTI_PROGRAM | NAND_MAX_BLK_ERASE_CNT |
				NAND_TWO_PLANE_SELECT,
			.AccessFreq = 100,
			.SpiMode = 0,
			.pagewithbadflag = FIRST_PAGE,
			.spi_nand_function = &spinand_function,
			.MultiPlaneBlockOffset = 1,
			.MaxEraseTimes = 65000,
			.EccType = BIT3_LIMIT3_LIMIT_5_ERR2,
			.EccProtectedType = SIZE64_OFF32_LEN32,
			.ecc_status_shift = ECC_STATUS_SHIFT_4,
	},
	{
		/* XTX26G01A */
		.NandID		= {0x0B, 0xE1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
		.DieCntPerChip  = 1,
		.SectCntPerPage = 4,
		.PageCntPerBlk  = 64,
		.BlkCntPerDie	= 1024,
		.OperationOpt	= NAND_DUAL_READ | NAND_MULTI_READ |
				NAND_MULTI_PROGRAM | NAND_MAX_BLK_ERASE_CNT,
		.AccessFreq = 100,
		.SpiMode = 0,
		.pagewithbadflag = FIRST_PAGE,
		.spi_nand_function = &spinand_function,
		.MultiPlaneBlockOffset = 1,
		.MaxEraseTimes  = 50000,
		.ecc_status_shift = ECC_STATUS_SHIFT_2,
		.EccType	= BIT4_LIMIT5_TO_7_ERR8_LIMIT_12,
		.EccProtectedType = SIZE16_OFF8_LEN16,
	},
	{
		/* XTX26G02A */
		.NandID		= {0x0B, 0xE2, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
		.DieCntPerChip  = 1,
		.SectCntPerPage = 4,
		.PageCntPerBlk  = 64,
		.BlkCntPerDie	= 2048,
		.OperationOpt	= NAND_DUAL_READ | NAND_QUAD_READ | NAND_MULTI_READ |
				NAND_MULTI_PROGRAM | NAND_MAX_BLK_ERASE_CNT,
		.AccessFreq = 100,
		.SpiMode = 0,
		.pagewithbadflag = FIRST_PAGE,
		.spi_nand_function = &spinand_function,
		.MultiPlaneBlockOffset = 1,
		.MaxEraseTimes  = 50000,
		.EccType	= BIT4_LIMIT5_TO_7_ERR8_LIMIT_12,
		.EccProtectedType = SIZE16_OFF8_LEN16,
		.ecc_status_shift = ECC_STATUS_SHIFT_2,
	},

	/*--------------------------GigaDevice--------------------------*/
	{
		/* GD5F1GQ4UCYIG */
		.NandID		= {0xc8, 0xb1, 0x48, 0xff, 0xff, 0xff, 0xff, 0xff},
		.DieCntPerChip  = 1,
		.SectCntPerPage = 4,
		.PageCntPerBlk  = 64,
		.BlkCntPerDie	= 1024,
		.OperationOpt	= NAND_DUAL_READ | NAND_MULTI_PROGRAM |
				NAND_ONEDUMMY_AFTER_RANDOMREAD,
		.AccessFreq = 100,
		.SpiMode = 0,
		.pagewithbadflag = FIRST_PAGE, /* the 1st page */
		.spi_nand_function = &spinand_function,
		.MultiPlaneBlockOffset = 1,
		.MaxEraseTimes  = 50000,
		.EccType	= BIT3_LIMIT2_TO_6_ERR7,
		.EccProtectedType = SIZE16_OFF0_LEN16,
		.ecc_status_shift = ECC_STATUS_SHIFT_4,
	},
	{
		/* GD5F2GQ5UEXXG */
		.NandID		= {0xc8, 0x52, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
		.DieCntPerChip  = 1,
		.SectCntPerPage = 4,
		.PageCntPerBlk  = 64,
		.BlkCntPerDie	= 2048,
		.OperationOpt	= NAND_DUAL_READ | NAND_QUAD_READ | NAND_MULTI_PROGRAM |
				NAND_MULTI_READ | NAND_MAX_BLK_ERASE_CNT,
		.AccessFreq = 100,
		.SpiMode = 0,
		.pagewithbadflag = FIRST_PAGE, /* the 1st page */
		.spi_nand_function = &spinand_function,
		.MultiPlaneBlockOffset = 1,
		.MaxEraseTimes  = 50000,
		.EccType	= BIT2_LIMIT1_ERR2_LIMIT3,
		.EccProtectedType = SIZE16_OFF0_LEN16,
		.ecc_status_shift = ECC_STATUS_SHIFT_4,
	},
	/*--------------------------FM--------------------------*/
	{
		/* FM25S01A*/
		.NandID		= {0xa1, 0xe4, 0x48, 0xff, 0xff, 0xff, 0xff, 0xff},
		.DieCntPerChip  = 1,
		.SectCntPerPage = 4,
		.PageCntPerBlk  = 64,
		.BlkCntPerDie	= 1024,
		.OperationOpt	= NAND_DUAL_READ | NAND_QUAD_READ | NAND_MULTI_PROGRAM |
				NAND_MULTI_READ | NAND_MAX_BLK_ERASE_CNT,
		.AccessFreq = 100,
		.SpiMode = 0,
		.pagewithbadflag = FIRST_TWO_PAGE, /* the 1st page */
		.spi_nand_function = &spinand_function,
		.MultiPlaneBlockOffset = 1,
		.MaxEraseTimes  = 50000,
		.EccType	= BIT2_LIMIT1_ERR2,
		.EccProtectedType = SIZE16_OFF0_LEN16,
		.ecc_status_shift = ECC_STATUS_SHIFT_4,
	},
	{
		/* FM25S02A*/
		.NandID		= {0xa1, 0xe5, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
		.DieCntPerChip  = 1,
		.SectCntPerPage = 4,
		.PageCntPerBlk  = 64,
		.BlkCntPerDie	= 2048,
		.OperationOpt	= NAND_DUAL_READ | NAND_QUAD_READ | NAND_MULTI_PROGRAM |
				NAND_MULTI_READ | NAND_MAX_BLK_ERASE_CNT,
		.AccessFreq = 100,
		.SpiMode = 0,
		.pagewithbadflag = FIRST_TWO_PAGE, /* the 1st page */
		.spi_nand_function = &spinand_function,
		.MultiPlaneBlockOffset = 1,
		.MaxEraseTimes  = 50000,
		.EccType	= BIT2_LIMIT1_ERR2,
		.EccProtectedType = SIZE16_OFF0_LEN16,
		.ecc_status_shift = ECC_STATUS_SHIFT_4,
	},

	/*------------------------------MXIC-----------------------------*/
	{
		/* MX35LF1GE4AB */
		.NandID		= {0xc2, 0x12, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
		.DieCntPerChip  = 1,
		.SectCntPerPage = 4,
		.PageCntPerBlk  = 64,
		.BlkCntPerDie	= 1024,
		.OperationOpt	= NAND_DUAL_READ | NAND_QUAD_READ | NAND_MULTI_PROGRAM,
		.AccessFreq = 100,
		.SpiMode = 0,
		.pagewithbadflag = FIRST_TWO_PAGE, /* the 1st or 2nd page */
		.spi_nand_function = &spinand_function,
		.MultiPlaneBlockOffset = 1,
		.MaxEraseTimes  = 65000,
		.EccType	= BIT4_LIMIT3_TO_4_ERR15 | HAS_EXT_ECC_STATUS,
		/**
		 * MX35LF1GE4AB should use SIZE16_OFF4_LEN12, however, in order
		 * to compatibility with versions already sent to customers,
		 * which do not use general physical layout, we used
		 * SIZE16_OFF4_LEN4_OFF8 instead.
		 */
		.EccProtectedType = SIZE16_OFF4_LEN4_OFF8,
	},
	{
		/* WB */
		.NandID		= {0xef, 0xaa, 0x21, 0xff, 0xff, 0xff, 0xff, 0xff},
		.DieCntPerChip  = 1,
		.SectCntPerPage = 4,
		.PageCntPerBlk  = 64,
		.BlkCntPerDie	= 1024,
		.OperationOpt	= NAND_MULTI_PROGRAM | NAND_MULTI_READ | NAND_MAX_BLK_ERASE_CNT |
				NAND_DUAL_READ | NAND_EXT_INTERLEAVE | NAND_READ_RECLAIM,
		.AccessFreq = 100,
		.SpiMode = 0,
		.pagewithbadflag = FIRST_PAGE, /* the 1st page */
		.spi_nand_function = &spinand_function,
		.MultiPlaneBlockOffset = 1,
		.MaxEraseTimes  = 65000,
		.EccType	= BIT2_LIMIT1_ERR2,
		.ecc_status_shift = ECC_STATUS_SHIFT_4,
		.EccProtectedType = SIZE16_OFF4_LEN4_OFF8,
	},


	//------------------------------------------------------------------------------------------------------------------------
	{ {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 0, 	0,		 0, 	   0,	0x0000, 	0,	   0,    0,        0,       	  0},	 // NULL
};

struct match_id {
	int match_id_len;
	struct __NandPhyInfoPar_t *match_nand_manu;
};

__s32 seek_chip_from_ext_idts(u8 *chipid, struct __NandPhyInfoPar_t *nand_arch_info)
{
	__s32 i = 0, j = 0;
	int m = 0;
	struct __NandPhyInfoPar_t *tmpNandManu = NULL;

	struct match_id last_match = {
		.match_id_len = 0,
		.match_nand_manu = NULL,
	};

	for (i = 0; i < (sizeof(ext_spinand_idts) / sizeof(ext_spinand_idts[0])); i++) {
		tmpNandManu = &ext_spinand_idts[i];

		if (tmpNandManu->NandID[0] != chipid[0])
			continue;
		m = 1;
		for (j = 1; j < 6; j++) {
			if (tmpNandManu->NandID[j] == chipid[j] && tmpNandManu->NandID[j] != 0xff)
				m++;
			else
				break;
		}
		if (m >= 2 && m > last_match.match_id_len) {
			last_match.match_id_len = m;
			last_match.match_nand_manu = tmpNandManu;
		}
	}

	if (last_match.match_nand_manu != NULL) {
		memcpy(nand_arch_info, last_match.match_nand_manu, sizeof(struct __NandPhyInfoPar_t));
		return 0;
	}

	return -1;
}
