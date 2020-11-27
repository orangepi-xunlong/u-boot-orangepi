/*
 * (C) Copyright 2013-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangflord@allwinnertech.com>
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

#include "common.h"
#include "private_toc.h"
#include <private_uboot.h>


int verify_addsum( void *mem_base, __u32 size )
{
	__u32 *buf;
	__u32 count;
	__u32 src_sum;
	__u32 sum;
	sbrom_toc1_head_info_t *bfh;
	
	bfh = (sbrom_toc1_head_info_t *)mem_base;
	/*generate checksum*/
	src_sum = bfh->add_sum;
	bfh->add_sum = STAMP_VALUE;
	count = size >> 2;
	sum = 0;
	buf = (__u32 *)mem_base;
	do
	{
		sum += *buf++;
		sum += *buf++;
		sum += *buf++;
		sum += *buf++;
	}while( ( count -= 4 ) > (4-1) );

	while( count-- > 0 )
		sum += *buf++;

	bfh->add_sum = src_sum;

//	printf("sum=%x\n", sum);
//	printf("src_sum=%x\n", src_sum);

	//check: 0-success  -1:fail
	if( sum == src_sum )
		return 0;
	else
		return -1;
}
