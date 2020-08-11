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
#include <mmc.h>
#include <sunxi_nand.h>
#include <boot_type.h>
#include <asm/arch/spinor.h>
#include <spare_head.h>
#include "sunxi_flash.h"
#include "sys_config.h"
#include "sys_partition.h"
#include "flash_interface.h"
#include <sunxi_board.h>



static int
sunxi_flash_spinor_read(unsigned int start_block, unsigned int nblock, void *buffer)
{
	debug("spinor read: start 0x%x, sector 0x%x\n", start_block, nblock);

    return spinor_read(start_block + CONFIG_SPINOR_LOGICAL_OFFSET, nblock, buffer);
}

static int
sunxi_flash_spinor_write(unsigned int start_block, unsigned int nblock, void *buffer)
{
	debug("spinor write: start 0x%x, sector 0x%x\n", start_block, nblock);

	return spinor_write(start_block + CONFIG_SPINOR_LOGICAL_OFFSET, nblock, buffer);
}



static uint
sunxi_flash_spinor_size(void){

	return spinor_size();
}

static int sunxi_flash_spinor_erase(int erase,void *mbr_buffer)
{
	return spinor_erase(erase,mbr_buffer);
}

static int
sunxi_flash_spinor_init(int stage)
{
	return spinor_init(stage);
}

static int
sunxi_flash_spinor_exit(int force)
{
	return spinor_exit(force);
}

static int
sunxi_flash_spinor_datafinish(void)
{
	return spinor_datafinish();
}

static int
sunxi_flash_spinor_flush(void)
{
	return spinor_flush_cache();
}


static int
sunxi_sprite_spinor_write(unsigned int start_block, unsigned int nblock, void *buffer)
{
	debug("burn spinor write: start 0x%x, sector 0x%x\n", start_block, nblock);

	return spinor_sprite_write(start_block+CONFIG_SPINOR_LOGICAL_OFFSET, nblock, buffer);
}

/*
************************************************************************************************************
*
*											  function
*
*
*
*
*
*
*
*
************************************************************************************************************
*/
int sunxi_sprite_setdata_finish(void)
{
	return sunxi_flash_spinor_datafinish();
}


int spinor_init_for_boot(int workmode, int spino)
{
	int ret = 0;

	tick_printf("spinor:	 %d\n", spino);
	ret = spinor_init(spino);
	//sunxi_flash_init_pt  = sunxi_flash_spinor_init;
	sunxi_flash_exit_pt  = sunxi_flash_spinor_exit;
	sunxi_flash_read_pt  = sunxi_flash_spinor_read;
	sunxi_flash_write_pt = sunxi_flash_spinor_write;
	sunxi_flash_size_pt  = sunxi_flash_spinor_size;
	sunxi_flash_flush_pt = sunxi_flash_spinor_flush;
	tick_printf("sunxi spinor flash init ok\n");
	return ret;

}
int  spinor_init_for_sprite(int workmode)
{
	if(try_spi_nor(0))
	{
		return -1;
	}
	printf("try nor successed \n");
	sunxi_sprite_init_pt  = sunxi_flash_spinor_init;
	sunxi_sprite_exit_pt  = sunxi_flash_spinor_exit;
	sunxi_sprite_read_pt  = sunxi_flash_spinor_read;
	sunxi_sprite_write_pt = sunxi_sprite_spinor_write;
	sunxi_sprite_erase_pt = sunxi_flash_spinor_erase;
	sunxi_sprite_size_pt  = sunxi_flash_spinor_size;
	sunxi_sprite_flush_pt = sunxi_flash_spinor_flush;
	//sunxi_sprite_datafinish_pt = sunxi_flash_spinor_datafinish;
	debug("sunxi sprite has installed spi function\n");

	set_boot_storage_type(STORAGE_NOR);
	return 0;
}

