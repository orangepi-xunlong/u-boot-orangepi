/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * lixiang <lixiang@allwinnertech.com>
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
#include <asm/arch/platform.h>
#include <sunxi_board.h>
#include <smc.h>

static s32 sunxi_i2c_io_null(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	return -1;
}

s32 (* sunxi_i2c_read_pt)(uchar chip, uint addr, int alen, uchar *buffer, int len) = sunxi_i2c_io_null;
s32 (* sunxi_i2c_write_pt)(uchar chip, uint addr, int alen, uchar *buffer, int len) = sunxi_i2c_io_null;

static s32 sunxi_i2c_read_secos(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	int ret = 0;
	ret = (u8)(arm_svc_arisc_read_pmu((ulong)addr));
	if(ret < 0 )
	{
		return -1;
	}
	*buffer = ret&0xff;
	return 0;
}

static s32 sunxi_i2c_write_secos(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	return arm_svc_arisc_write_pmu((ulong)addr,(u32)(*buffer));
}

int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	return sunxi_i2c_read_pt(chip, addr,alen, buffer, len);
}

int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	return sunxi_i2c_write_pt(chip, addr,alen, buffer, len);
}

void i2c_init(int speed, int slaveaddr)
{
	if(sunxi_probe_secure_monitor())
	{
		printf("i2c: secure monitor exist\n");
		sunxi_i2c_read_pt  = sunxi_i2c_read_secos;
		sunxi_i2c_write_pt = sunxi_i2c_write_secos;
	}
	else
	{
		printf("without secure monitor\n");
	}
	return ;
}


