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
#include <private_uboot.h>
extern char uboot_hash_value[64];

struct spare_boot_head_t  uboot_spare_head =
{
	{
		/* jump_instruction */
		(0xEA000000 |
		  (((sizeof(struct spare_boot_head_t) +
			sizeof(uboot_hash_value) + sizeof(int) - 1) /
			sizeof(int) - 2) & 0x00FFFFFF)),
		UBOOT_MAGIC,                        /* magic */
		STAMP_VALUE,                        /* check_sum */
		ALIGN_SIZE,                         /* align_size */
		0,                                  /* lenght */
		0,                                  /* uboot_lenght */
		UBOOT_VERSION,                      /* version */
		UBOOT_PLATFORM,                     /* platform */
		{CONFIG_SYS_TEXT_BASE}              /* reerved */
	},
	{
		{ 0 },                              /* dram para */
		408,                                /* run core clock */
		1200,                               /* run core vol */
		0,                                  /* uart port */
		{                                   /* uart gpio */
			{0}, {0}
		},
		0,                                  /* twi port */
		{                                   /* twi gpio */
			{0}, {0}
		},
		0,                                  /* work mode */
		STORAGE_NOR,                        /* storage mode */
		{ {0} },                            /* nand gpio */
		{ 0 },                              /* nand spare data */
		{ {0} },                            /* sdcard gpio */
		{ 0 },                              /* sdcard spare data */
		0,                                  /* secure os */
		0,                                  /* monitor */
		{ 0 },                              /* reserved data */
		UBOOT_START_SECTOR_IN_SDMMC,        /* OTA flag */
		0,                                  /* dtb offset */
		0,                                  /* boot_package_size */
		0,                                  /* dram_scan_size */
		{ 0 }                               /* reserved data */
	}
};


/*******************************************************************************
 *
 *                  About the jump_instruction of Boot_file_head
 *
 *   The instruction in arm :
 *          +--------+---------+------------------------------+
 *          | 31--28 | 27--24  |            23--0             |
 *          +--------+---------+------------------------------+
 *          |  cond  | 1 0 1 0 |        signed_immed_24       |
 *          +--------+---------+------------------------------+
 *  《ARM Architecture Reference Manual》:
 *  Syntax :
 *  B{<cond>}  <target_address>
 *    <cond>    Is the condition under which the instruction is executed. If the
 *              <cond> is ommitted, the AL(always,its code is 0b1110 )is used.
 *    <target_address>
 *              Specified the address to branch to. The branch target address is
 *              calculated by:
 *              1.  Sign-extending the 24-bit signed(wro's complement)immediate
 *                  to 32 bits.
 *              2.  Shifting the result left two bits.
 *              3.  Adding to the contents of the PC, which contains the address
 *                  of the branch instruction plus 8.
 *
 *  The B instruction :
 *          +---------+---------+--------------------------------+
 *          | 31--28  | 27--24  |            23--0               |
 *          +---------+---------+--------------------------------+
 *          | 1 1 1 0 | 1 0 1 0 |        signed_immed_24         |
 *          +---------+---------+--------------------------------+
 *          |        0xEA       |  calc by the size of file head |
 *          +-------------------+--------------------------------+
 *
 *  ( sizeof( boot_file_head_t ) + sizeof( int ) - 1 ) / sizeof( int )    the size of file head
 *  - 2                                                                   instruction prefetch
 *  & 0x00FFFFFF                                                          signed-immed-24
 *  | 0xEA000000                                                          finally we got the B instruction
 *
 *******************************************************************************/
