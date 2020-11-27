/*
**********************************************************************************************************************
*
*						           the Embedded Secure Bootloader System
*
*
*						       Copyright(C), 2006-2014, Allwinnertech Co., Ltd.
*                                           All Rights Reserved
*
* File    :
*
* By      :
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#include "common.h"
#include <private_boot0.h>
#include <private_uboot.h>
#include <asm/arch/dram.h>
#include <asm/arch/uart.h>

extern const boot0_file_head_t  BT0_head;
void set_pll( void );


#ifdef  FPGA_PLATFORM
//---------------------------------------------------------------
//  结构体 定义
//---------------------------------------------------------------
typedef struct
{
	__u32		ChannelCnt;
	__u32        ChipCnt;                            //the count of the total nand flash chips are currently connecting on the CE pin
	__u32       ChipConnectInfo;                    //chip connect information, bit == 1 means there is a chip connecting on the CE pin
	__u32		RbCnt;
	__u32		RbConnectInfo;						//the connect  information of the all rb  chips are connected
	__u32        RbConnectMode;						//the rb connect  mode
	__u32        BankCntPerChip;                     //the count of the banks in one nand chip, multiple banks can support Inter-Leave
	__u32        DieCntPerChip;                      //the count of the dies in one nand chip, block management is based on Die
	__u32        PlaneCntPerDie;                     //the count of planes in one die, multiple planes can support multi-plane operation
	__u32        SectorCntPerPage;                   //the count of sectors in one single physic page, one sector is 0.5k
	__u32       PageCntPerPhyBlk;                   //the count of physic pages in one physic block
	__u32       BlkCntPerDie;                       //the count of the physic blocks in one die, include valid block and invalid block
	__u32       OperationOpt;                       //the mask of the operation types which current nand flash can support support
	__u32        FrequencePar;                       //the parameter of the hardware access clock, based on 'MHz'
	__u32        EccMode;                            //the Ecc Mode for the nand flash chip, 0: bch-16, 1:bch-28, 2:bch_32
	__u8        NandChipId[8];                      //the nand chip id of current connecting nand chip
	__u32       ValidBlkRatio;                      //the ratio of the valid physical blocks, based on 1024
	__u32 		good_block_ratio;					//good block ratio get from hwscan
	__u32		ReadRetryType;						//the read retry type
	__u32       DDRType;
	__u32	    uboot_start_block;
	__u32	    uboot_next_block;
	__u32	    logic_start_block;
	__u32	    nand_specialinfo_page;
	__u32	    nand_specialinfo_offset;
	__u32	    physic_block_reserved;
	__u32	     random_cmd2_send_flag;			    //special nand cmd for some nand in batch cmd, only for write
	__u32	     random_addr_num;					    //random col addr num in batch cmd
	__u32	     nand_real_page_size;
	__u32	    Reserved[13];

}boot_nand_para_t;
#endif

/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int BOOT_NandGetPara(void *param, uint size)
{
#if 0 //FPGA_PLATFORM
	boot_nand_para_t *nand_para = (boot_nand_para_t *)param;
	//sand disk flash on fpga
	nand_para->ChannelCnt=1;
	nand_para->ChipCnt = 1;
	nand_para->ChipConnectInfo = 1;
	nand_para->RbCnt = 1;
	nand_para->RbConnectInfo = 1;
	nand_para->RbConnectMode = 1;
	nand_para->BankCntPerChip = 1;
	nand_para->DieCntPerChip = 1;
	nand_para->PlaneCntPerDie = 1;
	nand_para->SectorCntPerPage = 32;
	nand_para->PageCntPerPhyBlk = 512;
	nand_para->BlkCntPerDie = 2048;
	nand_para->OperationOpt = 0x1f041788;
	nand_para->FrequencePar = 40;
	nand_para->EccMode = 4;
	memset(nand_para->NandChipId, 0xff, 8);
	nand_para->NandChipId[0] = 0x2c;
	nand_para->NandChipId[1] = 0x84;
	nand_para->NandChipId[2] = 0x64;
	nand_para->NandChipId[3] = 0x3c;
	nand_para->NandChipId[4] = 0xa5;
	nand_para->ValidBlkRatio = 900;
	nand_para->good_block_ratio = 960;
	nand_para->ReadRetryType = 0x400a01;
	nand_para->DDRType = 0;

	nand_para->uboot_start_block=4;
	nand_para->uboot_next_block=12;

	nand_para->logic_start_block=4;
	nand_para->nand_specialinfo_page=1;
	nand_para->nand_specialinfo_offset=1;
	nand_para->physic_block_reserved=0;
	nand_para->random_cmd2_send_flag=0;
	nand_para->random_addr_num=0;
	nand_para->nand_real_page_size=16384+1216;

#else
	memcpy( (void *)param, BT0_head.prvt_head.storage_data, size);
#endif
	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
__u8  *get_page_buf( void )
{

	return (__u8 *)(CONFIG_SYS_SDRAM_BASE + 1024 * 1024);
}

/*******************************************************************************
*函数名称: g_mod
*函数原型：uint32 g_mod( __u32 dividend, __u32 divisor, __u32 *quot_p )
*函数功能: 从nand flash的某一块中找到一个完好备份将其载入到RAM中。如果成功，返
*          回OK；否则，返回ERROR。
*入口参数: dividend          输入。被除数
*          divisor           输入。除数
*          quot_p            输出。商
*返 回 值: 余数
*******************************************************************************/
__u32 g_mod( __u32 dividend, __u32 divisor, __u32 *quot_p )
{
	if( divisor == 0 )
	{
		*quot_p = 0;
		return 0;
	}
	if( divisor == 1 )
	{
		*quot_p = dividend;
		return 0;
	}

	for( *quot_p = 0; dividend >= divisor; ++(*quot_p) )
		dividend -= divisor;
	return dividend;
}


void set_dram_para(void *dram_addr , __u32 dram_size, __u32 boot_cpu)
{
	struct spare_boot_head_t  *uboot_buf = (struct spare_boot_head_t *)CONFIG_SYS_TEXT_BASE;

	memcpy((void *)uboot_buf->boot_data.dram_para, dram_addr, 32 * sizeof(int));
#ifdef CONFIG_BOOT_A15
	uboot_buf->boot_data.reserved[0] = boot_cpu;
#endif
	return;
}

extern const boot0_file_head_t  BT0_head;

void cpu_init_s(void)
{

}

