/*
 * Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
*/

#include <common.h>
#include <private_uboot.h>

#define BROM_FILE_HEAD_SIZE		(sizeof(struct spare_boot_head_t) & 0x00FFFFF)
#define BROM_FILE_HEAD_BIT_10_1		((BROM_FILE_HEAD_SIZE & 0x7FE) >> 1)
#define BROM_FILE_HEAD_BIT_11		((BROM_FILE_HEAD_SIZE & 0x800) >> 11)
#define BROM_FILE_HEAD_BIT_19_12	((BROM_FILE_HEAD_SIZE & 0xFF000) >> 12)
#define BROM_FILE_HEAD_BIT_20		((BROM_FILE_HEAD_SIZE & 0x100000) >> 20)

#define BROM_FILE_HEAD_SIZE_OFFSET	((BROM_FILE_HEAD_BIT_20 << 31) | \
									(BROM_FILE_HEAD_BIT_10_1 << 21) | \
									(BROM_FILE_HEAD_BIT_11 << 20) | \
									(BROM_FILE_HEAD_BIT_19_12 << 12))
#define JUMP_INSTRUCTION		(BROM_FILE_HEAD_SIZE_OFFSET | 0x6f)

#pragma pack(1)
struct spare_boot_head_t  uboot_spare_head = {
	{
		/* jump_instruction */
		JUMP_INSTRUCTION,
		UBOOT_MAGIC,
		STAMP_VALUE,
		ALIGN_SIZE,
		0,
		0,
		UBOOT_VERSION,
		UBOOT_PLATFORM,
		{CONFIG_SYS_TEXT_BASE}
	},
	{
		{0},		//dram para
		1008,			//run core clock
		1200,			//run core vol
		0,			//uart port
		{             //uart gpio
			{0}, {0}
		},
		0,			//twi port
		{             //twi gpio
			{0}, {0}
		},
		0,			//work mode
		0,			//storage mode
		{
			{0}
		},		//nand gpio
		{0},		//nand info
		{
			{0}
		},		//sdcard gpio
		{0}, 		//sdcard info
		0,                          //secure os
		0,                          //monitor
		0,                        /* see enum UBOOT_FUNC_MASK_EN */
		{0},						//reserved data
		UBOOT_START_SECTOR_IN_SDMMC, //OTA flag
		0,                           //dtb offset
		0,                           //boot_package_size
		0,							//dram_scan_size
		{0}			//reserved data
	},

};

#pragma pack()
/*******************************************************************************
*
*                  关于Boot_file_head中的jump_instruction字段
*
*  jump_instruction字段存放的是一条跳转指令：( B  BACK_OF_Boot_file_head )，此跳
*转指令被执行后，程序将跳转到Boot_file_head后面第一条指令。
*
*  ARM指令中的B指令编码如下：
*          +--------+---------+------------------------------+
*          | 31--28 | 27--24  |            23--0             |
*          +--------+---------+------------------------------+
*          |  cond  | 1 0 1 0 |        signed_immed_24       |
*          +--------+---------+------------------------------+
*  《ARM Architecture Reference Manual》对于此指令有如下解释：
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
*  由此可知，此指令编码的最高8位为：0b11101010，低24位根据Boot_file_head的大小动
*态生成，所以指令的组装过程如下：
*  ( sizeof( boot_file_head_t ) + sizeof( int ) - 1 ) / sizeof( int )    求出文件头
*                                              占用的“字”的个数
*  - 2                                         减去PC预取的指令条数
*  & 0x00FFFFFF                                求出signed-immed-24
*  | 0xEA000000                                组装成B指令
*
*******************************************************************************/
