/*
************************************************************************************************************************
*                                                         eGON
*                                         the Embedded GO-ON Bootloader System
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : adv_NF_func.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2007.11.11
*
* Description :
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang       2007.11.11      1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __adv_NF_func_h
#define  __adv_NF_func_h


typedef enum
{
	ADV_NF_OK               =0,
	ADV_NF_FIND_OK          =0,
	ADV_NF_NO_NEW_BAD_BLOCK =0,
	ADV_NF_ERROR              , 
	ADV_NF_NO_FIND_ERR        ,
	ADV_NF_OVERTIME_ERR       ,
	ADV_NF_LACK_BLKS          ,
	ADV_NF_NEW_BAD_BLOCK      ,
}adv_nf_errer_e;
	

extern __s32 load_and_check_in_one_blk( __u32 blk_num, void *buf, __u32 size,
                                        __u32 blk_size, const char *magic );

extern __s32 load_in_many_blks( __u32 start_blk, __u32 last_blk_num, void *buf,
						        __u32 size, __u32 blk_size, __u32 *blks );
						
extern __s32 write_in_one_blk( __u32 blk_num, void *buf, __u32 size, __u32 blk_size );

extern __s32 write_in_many_blks( __u32 start_blk, __u32 last_blk_num, void *buf,
					             __u32 size, __u32 blk_size, __u32 * blks );







#endif     //  ifndef __adv_NF_func_h


/* end of adv_NF_func.h  */
