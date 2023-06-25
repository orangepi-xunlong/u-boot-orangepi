/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __SPINAND_H
#define __SPINAND_H

#include <linux/mtd/aw-ubi.h>
#include <linux/mtd/aw-spinand.h>

#define AW_MTD_SPINAND_VER_MAIN		0x01
#define AW_MTD_SPINAND_VER_SUB		0x08
#define AW_MTD_SPINAND_VER_DATE		0x20220106


#define spinand_to_mtd(spinand) (&spinand->mtd)
#define mtd_to_spinand(mtd) ((struct aw_spinand *)(mtd->priv))
#define spinand_to_chip(spinand) (&spinand->chip)


/*
 * @ChipCnt:
 *	the count of the total nand flash chips are currently connecting
 * 	on the CE pin
 * @ConnectMode:
 *	the rb connect mode
 * @BankCntPerChip:
 *	the count of the banks in one nand chip, multiple banks can support
 *	Inter-Leave
 * @BankCntPerChip:
 *	the count of the dies in one nand chip, block management is based
 *	on Die.
 * @PlaneCntPerDie:
 *	the count of planes in one die, multiple planes can support
 *	multi-plane operation
 * @SectorCntPerPage:
 *	the count of sectors in one single physic page, one sector is 0.5k
 * @ChipConnectInfo:
 *	chip connect information, bit == 1 means there is a chip connecting
 *	on the CE pin
 * @PageCntPerPhyBlk:
 *	the count of physic pages in one physic block
 * @BlkCntPerDie:
 *	the count of the physic blocks in one die, include valid block and
 *	invalid block
 * @OperationOpt:
 *	the mask of the operation types which current nand flash can support
 *	support
 * @FrequencePar:
 *	the parameter of the hardware access clock, based on 'MHz'
 * @SpiMode:
 *	spi nand mode, 0:mode 0, 3:mode 3
 * @NandChipId:
 *	the nand chip id of current connecting nand chip
 * @pagewithbadflag:
 *	bad block flag was written at the first byte of spare area of this page
 * @MultiPlaneBlockOffset:
 *	the value of the block number offset between the two plane block
 * @MaxEraseTimes:
 *	the max erase times of a physic block
 * @MaxEccBits:
 *	the max ecc bits that nand support
 * @EccLimitBits:
 *	the ecc limit flag for tne nand
 */
typedef struct {
	__u8 ChipCnt;
	__u8 ConnectMode;
	__u8 BankCntPerChip;
	__u8 DieCntPerChip;
	__u8 PlaneCntPerDie;
	__u8 SectorCntPerPage;
	__u16 ChipConnectInfo;
	__u32 PageCntPerPhyBlk;
	__u32 BlkCntPerDie;
	__u32 OperationOpt;
	__u32 FrequencePar;
	__u32 SpiMode;
	__u8 NandChipId[8];
	__u32 pagewithbadflag;
	__u32 MultiPlaneBlockOffset;
	__u32 MaxEraseTimes;
	__u32 MaxEccBits;
	__u32 EccLimitBits;
	__u32 uboot_start_block;
	__u32 uboot_next_block;
	__u32 logic_start_block;
	__u32 nand_specialinfo_page;
	__u32 nand_specialinfo_offset;
	__u32 physic_block_reserved;
	__u32 sample_mode;
	__u32 sample_delay;
	__u32 Reserved[4];
} boot_spinand_para_t;

extern int spinand_init_mtd_info(struct ubi_mtd_info *mtd_info);
extern void spinand_uboot_blknum(unsigned int *start, unsigned int *end);
extern unsigned int spinand_mtd_read_ubi(unsigned int start,
		unsigned int sectors, void *buffer);
extern unsigned int spinand_mtd_write_ubi(unsigned int start,
		unsigned int sectors, void *buffer);
extern int spinand_ubi_user_volumes_size(void);
extern int spinand_mtd_flush_last_volume(void);
extern int sunxi_mbr_convert_to_gpt(void *sunxi_mbr_buf, char *gpt_buf,
		int storage_type);

#endif
