/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __UBOOT_TAIL_SPINAND_H__
#define __UBOOT_TAIL_SPINAND_H__

//#include "../nand-partition/phy.h"

typedef struct {
	__u8        ChipCnt;                 /* the count of the total nand flash chips are currently connecting on the CE pin */
	__u8        ConnectMode;             /* the rb connect  mode */
	__u8        BankCntPerChip;          /* the count of the banks in one nand chip, multiple banks can support Inter-Leave */
	__u8        DieCntPerChip;           /* the count of the dies in one nand chip, block management is based on Die */
	__u8        PlaneCntPerDie;          /* the count of planes in one die, multiple planes can support multi-plane operation */
	__u8        SectorCntPerPage;        /* the count of sectors in one single physic page, one sector is 0.5k */
	__u16       ChipConnectInfo;         /* chip connect information, bit == 1 means there is a chip connecting on the CE pin */
	__u32       PageCntPerPhyBlk;        /* the count of physic pages in one physic block */
	__u32       BlkCntPerDie;            /* the count of the physic blocks in one die, include valid block and invalid block */
	__u32       OperationOpt;            /* the mask of the operation types which current nand flash can support support */
	__u32       FrequencePar;            /* the parameter of the hardware access clock, based on 'MHz' */
	__u32       SpiMode;                 /* spi nand mode, 0:mode 0, 3:mode 3 */
	__u8        NandChipId[8];           /* the nand chip id of current connecting nand chip */
	__u32       pagewithbadflag;         /* bad block flag was written at the first byte of spare area of this page */
	__u32       MultiPlaneBlockOffset;   /* the value of the block number offset between the two plane block */
	__u32       MaxEraseTimes;           /* the max erase times of a physic block */
	__u32       MaxEccBits;              /* the max ecc bits that nand support */
	__u32       EccLimitBits;            /* the ecc limit flag for tne nand */
	__u32       uboot_start_block;
	__u32       uboot_next_block;
	__u32       logic_start_block;
	__u32       nand_specialinfo_page;
	__u32       nand_specialinfo_offset;
	__u32       physic_block_reserved;
	__u32       Reserved[4];
} boot_spinand_para_t;

int spinand_write_boot0_one(unsigned char *buf, unsigned int len, unsigned int counter);
int spinand_read_boot0_one(unsigned char *buf, unsigned int len, unsigned int counter);
int spinand_write_uboot_one_in_block(unsigned char *buf, unsigned int len, struct _boot_info *info_buf, unsigned int info_len, unsigned int counter);
int spinand_write_uboot_one_in_many_block(unsigned char *buf, unsigned int len, struct _boot_info *info_buf, unsigned int info_len, unsigned int counter);
int spinand_write_uboot_one(unsigned char *buf, unsigned int len, struct _boot_info *info_buf, unsigned int info_len, unsigned int counter);
int spinand_read_uboot_one_in_block(unsigned char *buf, unsigned int len, unsigned int counter);
int spinand_read_uboot_one_in_many_block(unsigned char *buf, unsigned int len, unsigned int counter);
int spinand_read_uboot_one(unsigned char *buf, unsigned int len, unsigned int counter);
int spinand_get_param(void *nand_param);
int spinand_get_param_for_uboottail(void *nand_param);
__s32 SPINAND_SetPhyArch_V3(struct _boot_info *ram_arch, void *phy_arch);
int SPINAND_UpdatePhyArch(void);
int spinand_erase_chip(unsigned int chip, unsigned int start_block, unsigned int end_block, unsigned int force_flag);
void spinand_erase_special_block(void);
int spinand_uboot_erase_all_chip(UINT32 force_flag);
int spinand_dragonborad_test_one(unsigned char *buf, unsigned char *oob, unsigned int blk_num);
int spinand_physic_info_get_one_copy(unsigned int start_block, unsigned int pages_offset, unsigned int *block_per_copy, unsigned int *buf);
int spinand_add_len_to_uboot_tail(unsigned int uboot_size);
extern __u32 NAND_GetNandIDNumCtrl(void);
extern __u32 NAND_GetNandExtPara(__u32 para_num);

#endif /*UBOOT_TAIL_SPINAND_H*/
