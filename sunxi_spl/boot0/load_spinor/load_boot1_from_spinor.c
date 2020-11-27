/*
 * (C) Copyright 2012
 *     wangflord@allwinnertech.com
 *
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
 * along with this program;
 *
 */
#include "common.h"
#include "spare_head.h"
#include "private_boot0.h"
#include "private_uboot.h"
#include <private_toc.h>
#include <asm/arch/spinor.h>



extern const boot0_file_head_t  BT0_head;

void update_flash_para(void)
{
	struct spare_boot_head_t  *bfh = (struct spare_boot_head_t *) CONFIG_SYS_TEXT_BASE;
	bfh->boot_data.storage_type = STORAGE_NOR;
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
int load_boot1_from_spinor(void)
{
	sbrom_toc1_head_info_t	*toc1_head;
	u8  *tmp_buff = (u8 *)CONFIG_BOOTPKG_STORE_IN_DRAM_BASE;
	int start_sector = UBOOT_START_SECTOR_IN_SPINOR;
	uint total_size = 0;

	if(spinor_init(0))
	{
		printf("spinor init fail\n");
		return -1;
	}

	if(spinor_read(start_sector, 1, (void *)tmp_buff ) )
	{
		printf("the first data is error\n");
		goto __load_boot1_from_spinor_fail;
	}
	printf("Succeed in reading toc file head.\n");

	toc1_head = (struct sbrom_toc1_head_info *)tmp_buff;
	if(toc1_head->magic != TOC_MAIN_INFO_MAGIC)
	{
		printf("toc1 magic error\n");
		goto __load_boot1_from_spinor_fail;
	}
	total_size = toc1_head->valid_len;
	printf("The size of toc is %x.\n", total_size );

	if(spinor_read(start_sector, total_size/512, (void *)tmp_buff ))
	{
		printf("spinor read data error\n");
		goto __load_boot1_from_spinor_fail;
	}

	return 0;

__load_boot1_from_spinor_fail:

	return -1;
}

int load_boot1(void)
{
	memcpy((void *)DRAM_PARA_STORE_ADDR, (void *)BT0_head.prvt_head.dram_para,
		SUNXI_DRAM_PARA_MAX * 4);
	return load_boot1_from_spinor();
}


