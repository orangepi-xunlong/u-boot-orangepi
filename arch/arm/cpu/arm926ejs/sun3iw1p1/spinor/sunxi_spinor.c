/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <asm/arch/spinor.h>
#include "spi_hal.h"

#define  SYSTEM_PAGE_SIZE        (512)
#define  SPINOR_PAGE_SIZE        (256)
#define  NPAGE_IN_1SYSPAGE       (SYSTEM_PAGE_SIZE/SPINOR_PAGE_SIZE)
#define  SPINOR_BLOCK_BYTES      (64 * 1024)
#define  SPINOR_BLOCK_SECTORS    (SPINOR_BLOCK_BYTES/512)

//#define SPINOR_TEST

extern int   spic_init(unsigned int spi_no);
extern int   spic_exit(unsigned int spi_no);
extern int   spic_rw  (unsigned int tcnt, void* txbuf, unsigned int rcnt, void* rxbuf);


/********************************************************************************************************
* Function   : spinor_read_rdid
* Description: read spinor rdid(Read Identification (RDID))
* Arguments  :
* Return     :
*********************************************************************************************************/
__s32 spinor_test_read_rdid(__u32 *id)
{
    __u8 sdata[4] = {0};
    *id = 0;
    sdata[0] = SPINOR_RDID;
    return spic_rw(1, (void *)sdata, 3, id);
}



int spinor_exit(int stage)
{
    return 0;
}


int spinor_read(uint start, uint sector_cnt, void *buffer)
{
    __u32 page_addr;
    __u32 rbyte_cnt;
    __u8  sdata[4] = {0};
    int   ret = 0;
    __u32 tmp_cnt, tmp_offset = 0;
    void  *tmp_buf;
    __u32 txnum, rxnum;
    __u8  *buf = (__u8 *)buffer;

    txnum = 4;

    while (sector_cnt)
    {
        if (sector_cnt > 127)
        {
            tmp_cnt = 127;
        }
        else
        {
            tmp_cnt = sector_cnt;
        }

        page_addr = (start + tmp_offset) * SYSTEM_PAGE_SIZE;
        rbyte_cnt = tmp_cnt * SYSTEM_PAGE_SIZE;
        sdata[0]  =  SPINOR_READ;
        sdata[1]  = (page_addr >> 16) & 0xff;
        sdata[2]  = (page_addr >> 8 ) & 0xff;
        sdata[3]  =  page_addr        & 0xff;

        rxnum   = rbyte_cnt;
        tmp_buf = (__u8 *)buf + (tmp_offset << 9);

        if (spic_rw(txnum, (void *)sdata, rxnum, tmp_buf))
        {
            ret = -1;
            break;
        }

        sector_cnt -= tmp_cnt;
        tmp_offset += tmp_cnt;
    }

    return ret;
}


int spinor_write(uint start, uint sector_cnt, void *buffer)
{
    uint tmpbuf;
    uint tmppage;

    uint page_addr;
    uint nor_page_cnt;

    char status = 0;
    uint i = 0;
    int  ret = -1;
    uint txnum, rxnum;
    char data[SPINOR_PAGE_SIZE + 4];

    page_addr = start * SYSTEM_PAGE_SIZE;
    nor_page_cnt = sector_cnt * (NPAGE_IN_1SYSPAGE);


    for (i = 0; i < nor_page_cnt; i++)
    {

        data[0] = SPINOR_WREN;
        ret = spic_rw(1, data, 0, 0);
        if (ret < 0)
        {
            return -1;
        }

        txnum = SPINOR_PAGE_SIZE+4;
        rxnum = 0;

        memset((void *)data, 0xff, SPINOR_PAGE_SIZE+4);

        tmppage = page_addr     + SPINOR_PAGE_SIZE * i;
        tmpbuf  = ((uint)buffer + SPINOR_PAGE_SIZE * i);

        data[0] = SPINOR_PP;
        data[1] = (tmppage >> 16) & 0xff;
        data[2] = (tmppage >> 8 ) & 0xff;
        data[3] =  tmppage        & 0xff;
        memcpy((void *)(data+4),(void *)tmpbuf, SPINOR_PAGE_SIZE);

        ret = spic_rw(txnum, (void *)data, rxnum, 0);
        if (ret < 0)
        {
            return -1;
        }

        do{
            data[0] = SPINOR_RDSR;
            ret   = spic_rw(1, data, 1, &status);
            if (ret < 0)
            {
                return -1;
            }

        } while (status & 0x01);

    }

    return 0;
}


int spinor_erase_block(uint block_index)
{
    uint  blk_addr = block_index * SPINOR_BLOCK_BYTES;
    char  sdata[4] = {0};
    int   ret = -1;
    char  status = 0;
    uint  txnum, rxnum;

    sdata[0] = SPINOR_WREN;
    ret = spic_rw(1, sdata, 0, 0);
    if (ret < 0)
    {
        return -1;
    }

    txnum = 4;
    rxnum = 0;

    sdata[0] = SPINOR_BE;
    sdata[1] = (blk_addr >> 16) & 0xff;
    sdata[2] = (blk_addr >> 8 ) & 0xff;
    sdata[3] =  blk_addr        & 0xff;

    ret = spic_rw(txnum, (void *)sdata, rxnum, 0);
    if (ret < 0)
    {
        return -1;
    }

    do{
        sdata[0] = SPINOR_RDSR;
        ret   = spic_rw(1, sdata, 1, &status);
        if (ret < 0)
        {
            return -1;
        }

    } while(status & 0x01);

    return 0;
}



int spinor_erase_all_blocks(int erase)
{
    if(erase)
    {
        char  sdata[4];
        char  status = 0;
        int   ret = -1;

        sdata[0] = SPINOR_WREN;
        ret = spic_rw(1, sdata, 0, 0);
        if (ret < 0)
        {
            return -1;
        }

        sdata[0] = SPINOR_CE;
        ret = spic_rw(1, sdata, 0, 0);
        if (ret < 0)
        {
            return -1;
        }

        do{

            sdata[0] = SPINOR_RDSR;
            ret   = spic_rw(1, sdata, 1, &status);
            if (ret < 0)
            {
                return -1;
            }

        } while (status & 0x01);
    }

    return 0;

}


int spinor_init(int stage)
{

    if(spic_init(0))
    {
        return -1;
    }
#ifdef   SPINOR_TEST
    {
        __u32  id =0;
        spinor_test_read_rdid(&id);
        printf("---spinor id = %x\n",id );
    }
#endif
    return 0;
}




