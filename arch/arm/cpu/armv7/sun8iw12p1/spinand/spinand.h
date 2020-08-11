/*
 * (C) Copyright 2017-2020
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef  __spinand_h
#define  __spinand_h

#include <common.h>
#include <asm/arch/gpio.h>
#include <private_boot0.h>
#include <asm/io.h>


#define BOOT0_START_BLK_NUM             0
#define BLKS_FOR_BOOT0                  8
#define BOOT1_START_BLK_NUM             BLKS_FOR_BOOT0


#define BLKS_FOR_BOOT1_IN_128K_BLK   18
#define BLKS_FOR_BOOT1_IN_256K_BLK   18
#define BLKS_FOR_BOOT1_IN_512K_BLK   10
#define BLKS_FOR_BOOT1_IN_1M_BLK     5
#define BLKS_FOR_BOOT1_IN_2M_BLK     5
#define BLKS_FOR_BOOT1_IN_4M_BLK     5
#define BLKS_FOR_BOOT1_IN_8M_BLK     5

extern __u32 SPN_BLOCK_SIZE;
extern __u32 SPN_BLK_SZ_WIDTH;
extern __u32 SPN_PAGE_SIZE;
extern __u32 SPN_PG_SZ_WIDTH;
extern __u32 UBOOT_START_BLK_NUM;
extern __u32 UBOOT_LAST_BLK_NUM;
extern __u32 page_for_bad_block;
extern __u32 OperationOpt;


#define SECTOR_SIZE                  512U
#define SCT_SZ_WIDTH                 9U

#define NAND_OP_TRUE            (0)                     //define the successful return value
#define NAND_OP_FALSE           (-1)                    //define the failed return value
#define ERR_TIMEOUT             14                  //hardware timeout
#define ERR_ECC                 12                  //too much ecc error
#define ERR_NANDFAIL            13                  //nand flash program or erase fail
#define SPINAND_BAD_BLOCK       1
#define SPINAND_GOOD_BLOCK		0


#define NAND_TWO_PLANE_SELECT	 			(1<<7)			//nand flash need plane select for addr
#define NAND_ONEDUMMY_AFTER_RANDOMREAD		(1<<8)			//nand flash need a dummy Byte after random fast read


typedef struct
{
    __u8        ChipCnt;                            //the count of the total nand flash chips are currently connecting on the CE pin
    __u8        ConnectMode;						//the rb connect  mode
	__u8        BankCntPerChip;                     //the count of the banks in one nand chip, multiple banks can support Inter-Leave
    __u8        DieCntPerChip;                      //the count of the dies in one nand chip, block management is based on Die
    __u8        PlaneCntPerDie;                     //the count of planes in one die, multiple planes can support multi-plane operation
    __u8        SectorCntPerPage;                   //the count of sectors in one single physic page, one sector is 0.5k
    __u16       ChipConnectInfo;                    //chip connect information, bit == 1 means there is a chip connecting on the CE pin
    __u32       PageCntPerPhyBlk;                   //the count of physic pages in one physic block
    __u32       BlkCntPerDie;                       //the count of the physic blocks in one die, include valid block and invalid block
    __u32       OperationOpt;                       //the mask of the operation types which current nand flash can support support
    __u32       FrequencePar;                       //the parameter of the hardware access clock, based on 'MHz'
    __u32       SpiMode;                            //spi nand mode, 0:mode 0, 3:mode 3
    __u8        NandChipId[8];                      //the nand chip id of current connecting nand chip
    __u32		pagewithbadflag;					//bad block flag was written at the first byte of spare area of this page
    __u32       MultiPlaneBlockOffset;              //the value of the block number offset between the two plane block
    __u32       MaxEraseTimes;              		//the max erase times of a physic block
    __u32		MaxEccBits;							//the max ecc bits that nand support
    __u32		EccLimitBits;						//the ecc limit flag for tne nand
    __u32		uboot_start_block;
	__u32		uboot_next_block;
	__u32		logic_start_block;
	__u32		nand_specialinfo_page;
	__u32		nand_specialinfo_offset;
	__u32		physic_block_reserved;
	__u32		Reserved[4];
}boot_spinand_para_t;


typedef struct boot_spiflash_info{
	__u32 chip_cnt;
	__u32 blk_cnt_per_chip;
	__u32 blocksize;
	__u32 pagesize;
	__u32 pagewithbadflag; /*bad block flag was written at the first byte of spare area of this page*/
}boot_spiflash_info_t;

struct boot_spinand_physical_param {
	__u32   chip; //chip no
	__u32  block; // block no within chip
	__u32  page; // apge no within block
	__u32  sectorbitmap;
	void   *mainbuf; //data buf
	void   *oobbuf; //oob buf
};

#endif
