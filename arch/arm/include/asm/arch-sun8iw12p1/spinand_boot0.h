/*
 * (C) Copyright 2017-2020
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef  __basic_spinand_func_h
#define  __basic_spinand_func_h

#define BOOT0_START_BLK_NUM          0
#define SPINAND_BOOT1_START_BLK_NUM  8


#define BLKS_FOR_BOOT1_IN_128K_BLK   18
#define BLKS_FOR_BOOT1_IN_256K_BLK   18
#define BLKS_FOR_BOOT1_IN_512K_BLK   10
#define BLKS_FOR_BOOT1_IN_1M_BLK     5
#define BLKS_FOR_BOOT1_IN_2M_BLK     5
#define BLKS_FOR_BOOT1_IN_4M_BLK     5
#define BLKS_FOR_BOOT1_IN_8M_BLK     5


#define SECTOR_SIZE              512U
#define SCT_SZ_WIDTH             9U

#define NAND_OP_TRUE            (0)      //define the successful return value
#define NAND_OP_FALSE           (-1)     //define the failed return value
#define ERR_TIMEOUT             14       //hardware timeout
#define ERR_ECC                 12       //too much ecc error
#define ERR_NANDFAIL            13       //nand flash program or erase fail
#define SPINAND_BAD_BLOCK       1
#define SPINAND_GOOD_BLOCK      0


#define NAND_TWO_PLANE_SELECT          (1<<7) //nand flash need plane select for addr
#define NAND_ONEDUMMY_AFTER_RANDOMREAD (1<<8) //nand flash need a dummy Byte after random fast read

extern __u32 SPN_BLOCK_SIZE;
extern __u32 SPN_BLK_SZ_WIDTH;
extern __u32 SPN_PAGE_SIZE;
extern __u32 SPN_PG_SZ_WIDTH;
extern __u32 UBOOT_START_BLK_NUM;
extern __u32 UBOOT_LAST_BLK_NUM;
extern __u32 page_for_bad_block;
extern __u32 OperationOpt;

extern __s32 load_Boot1_from_spinand( void );
extern __s32 Spinand_Load_Boot1_Copy( __u32 start_blk, void *buf,__u32 size, __u32 blk_size, __u32* blks );
extern __s32 SpiNand_PhyInit(void);
extern __s32 SpiNand_PhyExit(void);
extern __s32 SpiNand_Read( __u32 sector_num, void *buffer, __u32 N );
extern __s32 SpiNand_Check_BadBlock(__u32 block_num);

#endif     //  ifndef __basic_nf_func_h
