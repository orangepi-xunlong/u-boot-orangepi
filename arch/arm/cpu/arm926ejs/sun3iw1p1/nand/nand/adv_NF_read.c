/*
************************************************************************************************************************
*                                                         eGON
*                                         the Embedded GO-ON Bootloader System
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : adv_NF_read.c
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2008.09.23
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
* Gary.Wang      2008.09.23       1.1.0        build the file
*
************************************************************************************************************************
*/
#include "common.h"
#include "asm/arch/nand_boot0.h"
#include "../nand_for_boot.h"

__s32 load_and_check_in_one_blk( __u32 blk_num, void *buf, __u32 size, __u32 blk_size)
{
    __u32 copy_base;
    __u32 copy_end;
    __u32 blk_end;
    __u32 blk_base = blk_num * blk_size;
    __s32  status;


    for(copy_base = blk_base, copy_end = copy_base + size, blk_end = blk_base + blk_size;
        copy_end <= blk_end;
        copy_base += size, copy_end = copy_base + size )
    {
        status = NF_read( copy_base >> NF_SCT_SZ_WIDTH, (void *)buf, size >> NF_SCT_SZ_WIDTH );
        if( status == NF_OVERTIME_ERR )
            return ADV_NF_OVERTIME_ERR;
        else if( status == NF_ECC_ERR )
            continue;

        if( verify_addsum( (__u32 *)buf, size ) == 0 )
        {
            return ADV_NF_OK;
        }
    }

    return ADV_NF_ERROR;
}


__s32 load_in_many_blks( __u32 start_blk, __u32 last_blk_num, void *buf,
                         __u32 size, __u32 blk_size, __u32 *blks )
{
    __u32 buf_base;
    __u32 buf_off;
    __u32 size_loaded;
    __u32 cur_blk_base;
    __u32 rest_size;
    __u32 blk_num;
    __u32 blk_size_load;
    __u32 lsb_page_type;

    lsb_page_type = NAND_Getlsbpage_type();
    if(lsb_page_type!=0)
        blk_size_load = NAND_GetLsbblksize();
    else
        blk_size_load = blk_size;

    for( blk_num = start_blk, buf_base = (__u32)buf, buf_off = 0;
         blk_num <= last_blk_num && buf_off < size;
         blk_num++ )
    {
        printf("current block is %d and last block is %d.\n", blk_num, last_blk_num);
        if( NF_read_status( blk_num ) == NF_BAD_BLOCK )
            continue;

        cur_blk_base = blk_num * blk_size;
        rest_size = size - buf_off ;
        size_loaded = ( rest_size < blk_size_load ) ?  rest_size : blk_size_load ;
        if( NF_read( cur_blk_base >> NF_SCT_SZ_WIDTH, (void *)buf_base, size_loaded >> NF_SCT_SZ_WIDTH )
            == NF_OVERTIME_ERR )
            return ADV_NF_OVERTIME_ERR;

        buf_base += size_loaded;
        buf_off  += size_loaded;
    }


    *blks = blk_num - start_blk;
    if( buf_off == size )
        return ADV_NF_OK;
    else
    {
        printf("lack blocks with start block %d and buf size %x.\n", start_blk, size);
        return ADV_NF_LACK_BLKS;
    }
}


