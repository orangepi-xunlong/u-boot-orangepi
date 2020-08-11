/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
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


int spinor_read_id(void )
{
	uint id = 0;
	uint sdata = SPINOR_RDID;
	int  txnum ;
	int rxnum;

	txnum = 1;
	rxnum = 3;
	spic_config_dual_mode(0, 0, 0, txnum);
	spic_rw(txnum, (void *)&sdata, rxnum, (void *)(&id));
	printf("spinor id is %x\n", id);
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
int spinor_init(int stage)
{
	if(spic_init(0))
	{
		return -1;
	}
	spinor_read_id();

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
int spinor_exit(int stage)
{
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

		spic_config_dual_mode(0, 0, 0, txnum);

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



