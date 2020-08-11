/*
 * (C) Copyright 2017-2020
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include "spinand.h"
#include "spic.h"
#include "spinand_osal_boot0.h"

extern __s32 spi_nand_reset(__u32 spi_no, __u32 chip);
extern __s32 read_single_page(struct boot_spinand_physical_param *readop, __u32 data_size, __u32 spare_only_flag);
extern __s32 spi_nand_setotp(__u32 spi_no, __u32 chip, __u8 reg);
extern __s32 spi_nand_setblocklock(__u32 spi_no, __u32 chip, __u8 reg);
__s32 SpiNand_PhyInit(void);
__s32 SpiNand_PhyExit(void);



//extern  struct __NandStorageInfo_t  NandStorageInfo;
extern __s32 BOOT_NandGetPara(boot_spinand_para_t *param, uint size);
__u32 SPN_BLOCK_SIZE = 0;
__u32 SPN_BLK_SZ_WIDTH = 0;
__u32 SPN_PAGE_SIZE = 0;
__u32 SPN_PG_SZ_WIDTH = 0;
__u32 UBOOT_START_BLK_NUM = 0;
__u32 UBOOT_LAST_BLK_NUM = 0;
__u32 page_for_bad_block = 0;
__u32 OperationOpt = 0;


/*
*******************************************************************************
*****************************************
*                           ANALYZE NAND FLASH STORAGE SYSTEM
*
*Description: Analyze nand flash storage system, generate the nand flash
physical
*             architecture parameter and connect information.
*
*Arguments  : none
*
*Return     : analyze result;
*               = 0     analyze successful;
*               < 0     analyze failed, can't recognize or some other error.
*******************************************************************************
*****************************************
*/
__s32  BOOT_AnalyzeSpiNandSystem(void)
{
	__s32 result;
	boot_spinand_para_t nand_info;

	if( BOOT_NandGetPara( &nand_info,sizeof(boot_spinand_para_t) ) != 0 )
	{
		return -1;
	}

	OperationOpt = nand_info.OperationOpt;
	//   _InitSpiNandPhyInfo(&nand_info);

	UBOOT_START_BLK_NUM = nand_info.uboot_start_block;
	UBOOT_LAST_BLK_NUM = nand_info.uboot_next_block;

	//reset the nand flash chip on boot chip select
	result = spi_nand_reset(0, 0);
	if(result)
	{
		SPINAND_Print("spi nand reset fail!\n");
		return -1;
	}

	spi_nand_setotp(0, 0, 0x18);
	spi_nand_setblocklock(0, 0, 0);

	return 0;
}

__s32 SpiNand_GetFlashInfo(boot_spiflash_info_t *param)
{
	boot_spinand_para_t nand_info;

	BOOT_NandGetPara(&nand_info,sizeof(boot_spinand_para_t));

	param->chip_cnt	 		= nand_info.ChipCnt;
	param->blk_cnt_per_chip = nand_info.BlkCntPerDie * nand_info.DieCntPerChip;
	param->blocksize 		= nand_info.SectorCntPerPage * nand_info.PageCntPerPhyBlk;
	param->pagesize 		= nand_info.SectorCntPerPage;
	param->pagewithbadflag  = 0 ;   // fix page 0 as bad flag page index

	return 0;
}


/*
*******************************************************************************
*****************************************
*                       INIT NAND FLASH
*
*Description: initial nand flash,request hardware resources;
*
*Arguments  : void.
*
*Return     :   = SUCESS  initial ok;
*               = FAIL    initial fail.
*******************************************************************************
*****************************************
*/
__s32 SpiNand_PhyInit(void)
{
	__s32 ret;
	boot_spiflash_info_t param;

	ret = Spic_init(0);
	if (ret)
	{
		SPINAND_Print("Spic_init fail\n");
		goto error;
	}

	ret = BOOT_AnalyzeSpiNandSystem();
	if (ret)
	{
		SPINAND_Print("spi nand scan fail\n");
		goto error;
	}

	SpiNand_GetFlashInfo(&param);

	page_for_bad_block = param.pagewithbadflag;
	SPN_BLOCK_SIZE = param.blocksize * SECTOR_SIZE;
	SPN_PAGE_SIZE = param.pagesize * SECTOR_SIZE;
	SPINAND_Print("spinand UBOOT_START_BLK_NUM %d UBOOT_LAST_BLK_NUM %d\n",UBOOT_START_BLK_NUM,UBOOT_LAST_BLK_NUM);

	return 0;

error:
	SpiNand_PhyExit();
	return -1;

}

/*
*******************************************************************************
*****************************************
*                       RELEASE NAND FLASH
*
*Description: release  nand flash and free hardware resources;
*
*Arguments  : void.
*
*Return     :   = SUCESS  release ok;
*               = FAIL    release fail.
*******************************************************************************
*****************************************
*/
__s32 SpiNand_PhyExit(void)
{
	Spic_exit(0);

	/* close nand flash bus clock gate */
	//NAND_CloseAHBClock();

	return 0;
}

__s32 SpiNand_Check_BadBlock(__u32 block_num)
{
	struct boot_spinand_physical_param  para;
	__u8  oob_buf[16];
//	__u8 *page_buf;
	__s32 ret;

//	page_buf = get_page_buf( );

	para.chip = 0;
	para.block = block_num;
	para.page = 0;
	para.mainbuf = NULL;
	para.oobbuf = oob_buf;
	ret = read_single_page(&para,0, 1);
	if(ret != NAND_OP_TRUE )
	{
		SPINAND_Print("Check_BadBlock: read_single_page FAIL\n");
		return NAND_OP_FALSE;
	}
	if( oob_buf[0] != 0xFF )
	{
		SPINAND_Print("oob_buf[0] = %x\n",oob_buf[0]);
		return SPINAND_BAD_BLOCK;
	}
	else
		return SPINAND_GOOD_BLOCK;
}

__s32 SpiNand_Read( __u32 sector_num, void *buffer, __u32 N )
{
	struct boot_spinand_physical_param  para;
	__u8  oob_buf[16];
	__u32 page_nr;
	__u32 scts_per_page = SPN_PAGE_SIZE >> SCT_SZ_WIDTH;
//	__u32 residue;
	__u32 start_page;
//	__u32 start_sct;
	__u32 i;
	__u32 blk_num;
	__u32 not_full_page_flag = 0;
	__u32 data_size;

	para.chip = 0;
	blk_num = sector_num / (SPN_BLOCK_SIZE >> SCT_SZ_WIDTH);
	para.block = blk_num;
	start_page = (sector_num % (SPN_BLOCK_SIZE >> SCT_SZ_WIDTH)) / scts_per_page;
	para.oobbuf = oob_buf;
	page_nr = N / scts_per_page;
	if(N % scts_per_page)
	{
		page_nr++;
		not_full_page_flag = 1;
	}
	for( i = 0; i < page_nr; i++ )
	{
		para.mainbuf = (__u8 *)buffer + SPN_PAGE_SIZE * i;
		para.page = start_page + i;
		data_size = SPN_PAGE_SIZE;
		if((i == (page_nr - 1)) && not_full_page_flag)
			data_size = (N << SCT_SZ_WIDTH) - (SPN_PAGE_SIZE * i);
		if(read_single_page(&para,data_size, 0) != NAND_OP_TRUE)
			return NAND_OP_FALSE;
	}

	return NAND_OP_TRUE;
}

//#pragma arm section  code="load_in_many_blks"
/*******************************************************************************
*函数名称: load_in_many_blks
*函数原型：int32 load_in_many_blks( __u32 start_blk, __u32 last_blk_num, void *buf,
*						            __u32 size, __u32 blk_size, __u32 *blks )
*函数功能: 从nand flash的某一块start_blk开始，载入file_length长度的内容到内存中。
*入口参数: start_blk         待访问的nand flash起始块号
*          last_blk_num      最后一个块的块号，用来限制访问范围
*          buf               内存缓冲区的起始地址
*          size              文件尺寸
*          blk_size          待访问的nand flash的块大小
*          blks              所占据的块数，包括坏块
*返 回 值: ADV_NF_OK                操作成功
*          ADV_NF_OVERTIME_ERR   操作超时
*          ADV_NF_LACK_BLKS      块数不足
*备    注: 1. 本函数只载入，不校验
*******************************************************************************/
__s32 Spinand_Load_Boot1_Copy( __u32 start_blk, void *buf, __u32 size, __u32 blk_size, __u32* blks )
{
	__u32 buf_base;
	__u32 buf_off;
	__u32 size_loaded;
	__u32 cur_blk_base;
	__u32 rest_size;
	__u32 blk_num;
	//	__u32 blk_size_load;
	//	__u32 lsb_page_type;

	//	blk_size_load = blk_size;

	for( blk_num = start_blk, buf_base = (__u32)buf, buf_off = 0;
	     buf_off < size;blk_num++ )
	{
	//		SPINAND_Print("current block is %d.\n", blk_num);
		if( SpiNand_Check_BadBlock( blk_num ) == SPINAND_BAD_BLOCK )		// 如果当前块是坏块，则进入下一块
			continue;

		cur_blk_base = blk_num * blk_size;
		rest_size = size - buf_off ;                        // 未载入部分的尺寸
		size_loaded = ( rest_size < blk_size ) ?  rest_size : blk_size ;  // 确定此次待载入的尺寸

		if( SpiNand_Read( cur_blk_base >> SCT_SZ_WIDTH, (void *)buf_base, size_loaded >> SCT_SZ_WIDTH )!= NAND_OP_TRUE )
			return NAND_OP_FALSE;

		buf_base += size_loaded;
		buf_off  += size_loaded;
	}


	*blks = blk_num - start_blk;                            // 总共涉及的块数
	if( buf_off == size )
		return NAND_OP_TRUE;                                          // 成功，返回OK
	else
	{
		printf("lack blocks with start block %d and buf size %x.\n", start_blk, size);
		return NAND_OP_FALSE;                                // 失败，块数不足
	}
}
