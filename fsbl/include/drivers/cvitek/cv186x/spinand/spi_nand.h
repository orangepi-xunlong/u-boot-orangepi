#ifndef SPI_NAND_H
#define SPI_NAND_H

#include "cv_spi_nand.h"

//------------------------------------------------------------------------------
//  constant definitions:
//------------------------------------------------------------------------------
#define NUM_SUPPORT_TOGGLE_SPI_NAND 4

#define DESC_CONV(x)	((((((x[0]<<8)|x[1])<<8)|x[2])<<8)|x[3])

#define SPI_NAND_VECTOR_SIGNATURE DESC_CONV("SPNV")

#define SPI_NAND_VECTOR_VERSION (0x18802001)

#define SPI_NAND_BASE_DATA_BACKUP_COPY (2)

#define SPI_NAND_FIP_DATA_BLOCK_COUNT (8)
#define SPI_NAND_FIP_DATA_BACKUP_COUNT (2)

#define SPI_NAND_FIP_RSVD_BLOCK_COUNT (4) // first 4 blocks are reserved for sys base vector
#define SPI_NAND_BASE_BLOCK_COUNT (32)		// must aligned to 8
#define SPI_NAND_BASE_BLOCK_MAX_COUNT_ENTRY (SPI_NAND_BASE_BLOCK_COUNT/8)

#define BIT(nr)			(1UL << (nr))

#define MAX_BLOCK_CNT			4096

//------------------------------------------------------------------------------
//  data type definitions: typedef, struct or class
//------------------------------------------------------------------------------
#define NUMBER_PATCH_SET 128
typedef struct _patch_data_t {
	uint32_t reg;
	uint32_t value;
} patch_data_t;

typedef struct _spi_nand_base_vector_t {
	uint32_t signature;
	uint32_t version;

	uint16_t spi_nand_vector_blks[SPI_NAND_BASE_DATA_BACKUP_COPY];

	uint16_t fip_bin_blk_cnt;
	uint16_t fip_bin_blks[SPI_NAND_BASE_DATA_BACKUP_COPY][SPI_NAND_FIP_DATA_BLOCK_COUNT];

	uint16_t erase_count; // erase count for sys base block
	uint16_t rsvd_block_count; // how many blocks reserved for spi_nand_vect and fip.bin
	uint32_t spi_nand_vector_length;  // spi_nand vector struct length, must less than a page

	uint8_t spi_nand_base_block_usage[SPI_NAND_BASE_BLOCK_MAX_COUNT_ENTRY]; // 32 bits => 1 DW

	spi_nand_info_t spi_nand_info;

	uint8_t factory_defect_buf[MAX_BLOCK_CNT/8]; // factory defect block table, up to 512 bytes

	uint32_t bld_size;
	uintptr_t bld_loading_to_addr;

	uint32_t valid_patch_num;
	struct _patch_data_t patch_data[NUMBER_PATCH_SET];
#ifdef NANDBOOT_V2
	uint16_t crc;
#endif
} spi_nand_base_vector_t;

//int spi_nand_check_write_vector(uint32_t bl1_img_sz, uint32_t bl2_img_sz);
void apply_spi_nand_patch(void);
int spi_nand_scan_vector(void);
int spi_nand_read_fip_bin(uint32_t fip_idx, void *buf, uint32_t offset, uint32_t len);
void spi_nand_load_bld(void *buf);

#endif //SPI_NAND_H
