/*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CV_NAND_H
#define CV_NAND_H

#include <stdint.h>

//------------------------------------------------------------------------------
//  constant definitions:
//------------------------------------------------------------------------------

#define DESC_CONV(x) ((((((x[0] << 8) | x[1]) << 8) | x[2]) << 8) | x[3])

#define NAND_VECTOR_SIGNATURE DESC_CONV("NANV")
#define PRODUCTION_FW_SIGNATURE DESC_CONV("PTFM")

#define NAND_VECTOR_VERSION (0xF0000001)

#define NAND_BASE_DATA_BACKUP_COPY (2)
#define MAX_SCAN_VECTOR_BLOCK_COUNT (4) // NAND boot might use first block
#define NAND_FIP_DATA_BLOCK_COUNT (4)   // TODO : fip.bin / nk block size

#define NFC_RESET_BIT BIT(12)

//#define NAND_BASE_BLOCK_MAX_COUNT (32)
#define NAND_BASE_BLOCK_COUNT (24) // must aligned to 8
#define NAND_BASE_BLOCK_MAX_COUNT_ENTRY (NAND_BASE_BLOCK_COUNT / 8)

#define NFC_REG_CTRL_STATUS (0x500C0118)
#define NFC_REG_RNB_STATUS (0x500C1004)
#define NFC_REG_CTRL_STATUS_BUSY (0x100)
#define TOP_REG_NFC_DD_PINS (0x50010070)
#define TOP_REG_NFC_BOOT_REG (0x50010074)
#define TOP_REG_NFC_ID_VALUE (0x50010078)
#define TOP_REG_NFC_MISC (0x5001007C)
#define TOP_REG_NFC_REMAP39_TO_32 (0x50010064)

#define NAND_ID_NUM 8

#define NAND_PROBE_DELAY_TIME (100) //us
#define MAX_NAND_PROBE_DELAY_COUNT (10) // 10 * 100us = 1ms

//------------------------------------------------------------------------------
//  data type definitions: typedef, struct or class
//------------------------------------------------------------------------------

typedef struct nand_info
{
	uint8_t id[NAND_ID_NUM];
	uint16_t pageCnt;
	uint16_t pageSize; // Byte
	uint16_t blockCnt;
	uint8_t planeCnt;
	uint8_t dieCnt;
	uint8_t ceCnt;
	uint8_t chCnt;
	//TIMING_INFO_T timing;
} nand_info_t;

typedef struct _nand_base_vector_t
{
	uint32_t signature;
	uint32_t version;

	uint16_t fip_bin_blk[NAND_BASE_DATA_BACKUP_COPY][NAND_FIP_DATA_BLOCK_COUNT];

	uint16_t erase_count;		 // erase count for this block
	uint16_t rsvd_block_count;   // how many blocks reserved for nand_vect and fip.bin
	uint32_t nand_vector_length; // nand vector struct length, must less than a page

	uint16_t nand_vector_blk[NAND_BASE_DATA_BACKUP_COPY];
	uint8_t nand_base_block_usage[NAND_BASE_BLOCK_MAX_COUNT_ENTRY];

	nand_info_t nand;

	uint16_t factory_bad_block_count; // How many factory bad block
	uint16_t factory_bad_block_array_offset;

	uint16_t growing_bad_block_count; // How many growing bad block
	uint16_t growing_bad_block_array_offset;

	//uint16_t vector_buf[2048/2]; // 2K buf
} nand_base_vector_t;

typedef struct _nand_support_para_t
{
	uint8_t id[NAND_ID_NUM];

	uint32_t dd_reg_70_value;
	uint32_t dd_reg_74_value;

	uint16_t mainSize;
	uint16_t spareSize;
	uint16_t lunNumber;
	uint16_t pagesPerBlock;
	uint16_t blocksPerLun;
	uint16_t numberOfPlanes;
	uint16_t blockShift;
} nand_support_para_t;

#define MAX_OPEN_NAND_PB_COUNT 2

typedef struct
{
	uint32_t block_size;
	uint32_t nr_die;
	uint32_t nr_blk;
	uint32_t nand_size;

	uint32_t pg_sz;
	uint32_t spare_sz;
	uint32_t lun_num;
	uint32_t pages_per_block;
	uint32_t number_of_planes;
	uint32_t blocks_per_lun;
	uint32_t block_shift_bit;
} NAND_GEOMETRY_T;

//------------------------------------------------------------------------------
//  public function proto-type definitions:
//------------------------------------------------------------------------------
void nfc_init(uintptr_t regs_base, uintptr_t sdma_base);
int nfc_start(void *privateData);
int read_fip_bin_data(uint32_t fip_idx, void *buf, uint32_t offset, uint32_t len);
int scan_nand_vector(void);

#endif //CV_NFC_H
