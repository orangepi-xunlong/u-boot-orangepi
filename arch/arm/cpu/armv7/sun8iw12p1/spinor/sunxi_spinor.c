/*
 * sunxi_spinor.c
 *
 * Copyright (C) 2017-2020 Allwinnertech Co., Ltd
 *
 * Author        : zhouhuacai
 *
 * Description   : Driver for sunxi spinor
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 *
 * History       :
 *  1.Date        : 2017/4/12
 *    Author      : zhouhuacai
 *    Modification: Created file
 */

#include <common.h>
#include <asm/arch/spinor.h>
#include "spi_hal.h"
#include <sunxi_mbr.h>
#include <linux/crc32.h>




#define  SYSTEM_PAGE_SIZE        (512)
#define  SPINOR_PAGE_SIZE        (256)
#define  NPAGE_IN_1SYSPAGE       (SYSTEM_PAGE_SIZE/SPINOR_PAGE_SIZE)
#define  SPINOR_BLOCK_BYTES      (64 * 1024)
#define  SPINOR_BLOCK_SECTORS    (SPINOR_BLOCK_BYTES/512)

static int quad_flag;
static int quad_state;
static int spinor_4bytes_addr_mode;
static int   spinor_flash_inited;

#define QUAD_READ_ERR	(-1)
#define RET_FAIL	(-1)
#define ENABLE_4BYTES	(1)
#define SPINOR_FREAD_DUAL_OUT 	0x3b
#define SPINOR_FREAD_QUAD_OUT	0x6b

int set_quad_mode(u8 cmd1, u8 cmd2);
int spi_nor_fast_read_quad_output(uint start, uint sector_cnt, void *buf);


struct spinor_info {
	uint		id;
	u8		mode_cmd1;
	u8		mode_cmd2;
	u16		flags;
	#define QUAD_ENABLE 0x01
};

struct spinor_info spi_nor_ids[] = {
/* spinor_id, mode_cmd1, mode_cmd2, flags */

	/* -- Macronix -- */
	{0x1820c2, 0x40, 0x00, QUAD_ENABLE},
	{0x1020c2, 0x40, 0x00},
	{0x1220c2, 0x40, 0x00},
	{0x1320c2, 0x40, 0x00},
	{0x1420c2, 0x40, 0x00},
	{0x1520c2, 0x40, 0x00},
	{0x1620c2, 0x40, 0x00},
	{0x169ec2, 0x40, 0x00},
	{0x1720c2, 0x40, 0x00},
	{0x3725c2, 0x40, 0x00},
	{0x1826c2, 0x40, 0x00},
	{0x1920c2, 0x40, 0x00},
	{0x1926c2, 0x40, 0x00},
	{0x1a20c2, 0x40, 0x00, QUAD_ENABLE},
	{0x1b26c2, 0x40, 0x00, QUAD_ENABLE},

	/* -- Winbond -- */
	{0x1840ef, 0x00, 0x02, QUAD_ENABLE},
	{0x1030ef, 0x00, 0x02},
	{0x1130ef, 0x00, 0x02},
	{0x1230ef, 0x00, 0x02},
	{0x1330ef, 0x00, 0x02},
	{0x1430ef, 0x00, 0x02},
	{0x1530ef, 0x00, 0x02},
	{0x1630ef, 0x00, 0x02},
	{0x1640ef, 0x00, 0x02},
	{0x1660ef, 0x00, 0x02, QUAD_ENABLE},
	{0x1730ef, 0x00, 0x02},
	{0x1740ef, 0x00, 0x02},
	{0x1760ef, 0x00, 0x02, QUAD_ENABLE},
	{0x1860ef, 0x00, 0x02, QUAD_ENABLE},
	{0x1450ef, 0x00, 0x02},
	{0x1440ef, 0x00, 0x02},
	{0x1940ef, 0x00, 0x02},

	/* -- Atmel -- */
	{0x01661f, 0x80, 0x00},
	{0x04661f, 0x80, 0x00},
	{0x01441f, 0x80, 0x00},
	{0x01471f, 0x80, 0x00},
	{0x00481f, 0x80, 0x00},
	{0x00041f, 0x80, 0x00},
	{0x01451f, 0x80, 0x00},
	{0x01461f, 0x80, 0x00},
	{0x00471f, 0x80, 0x00},
	{0x00251f, 0x80, 0x00},

	/* -- ISSI -- */
	{0x209d7f, 0x40, 0x00},
};

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
	return id;
}

static void spinor_enter_4bytes_addr(int enable)
{
	int command = 0;
	if(spinor_4bytes_addr_mode == 1 && enable == 1)
		command = 0xB7;
	else if(spinor_4bytes_addr_mode == 1 && enable == 0)
		command = 0xE9;
	else
		return ;

	spic_config_dual_mode(0, 0, 0, 1);

	spic_rw(1, (void*)&command, 0, 0);
	return;
}


int spinor_init(int stage)
{
	int i, id_size, id = 0;

	if(spinor_flash_inited)
	{
		printf("sunxi spinor is already inited\n");
		return 0;
	}
	else
	{
		if(spic_init(0))
		{
			printf("sunxi spinor is initing...failed\n");

			return -1;
		}
		else
		{
			printf("sunxi spinor is initing...ok\n");
		}

	}

	spinor_flash_inited ++;

	id = spinor_read_id();
	if(!id)
		return -1;

	spinor_4bytes_addr_mode = 1;
	spinor_enter_4bytes_addr(ENABLE_4BYTES);

	if (quad_state != QUAD_READ_ERR) {
		id_size = ARRAY_SIZE(spi_nor_ids);
		for (i = 0; i < id_size; i++) {
			if ((id == spi_nor_ids[i].id) && (spi_nor_ids[i].flags & QUAD_ENABLE)) {
				quad_flag = 1;
				if (set_quad_mode(spi_nor_ids[i].mode_cmd1, spi_nor_ids[i].mode_cmd2))
					quad_flag = 0;
				break;
			}
		}
	}

	return 0;
}

int spinor_exit(int stage)
{
	return spic_exit(0);;
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


static int __spinor_wren(void)
{
	uint sdata = SPINOR_WREN;
	int ret = -1;
	int  txnum ;
	txnum = 1;

	spic_config_dual_mode(0, 0, 0, txnum);
	ret = spic_rw(1, (void *)&sdata, 0, NULL);
	return ret;
}

static int __spinor_rdsr(u8 *reg)
{
	u8 sdata = SPINOR_RDSR;
	int ret = -1;
	int  txnum ;

	txnum = 1;
	spic_config_dual_mode(0, 0, 0, txnum);
	ret = spic_rw(1, (void *)&sdata, 1, (void *)reg);

	return ret;
}


int set_quad_mode(u8 cmd1, u8 cmd2)
{
	u8 i = 0;
	int ret = 0;
	u8 reg[2] = {0};
	u8 sdata[3] = {0};
	uint txnum, rxnum;

	txnum = 3;
	rxnum = 0;

	ret = __spinor_wren();
	if (ret == -1)
		goto __err_out_;

	sdata[0] = SPINOR_WRSR;
	sdata[1] = cmd1;
	sdata[2] = cmd2;

	spic_config_dual_mode(0, 0, 0, txnum);
	ret = spic_rw(txnum, (void *)sdata, rxnum, (void *)0);
	if (ret == -1)
		goto __err_out_;

	do {
		ret = __spinor_rdsr(&reg[0]);
		if (ret == RET_FAIL)
			goto __err_out_;

		__msdelay(5);
		i++;
		if (i > 4)
			goto __err_out_;
	} while (reg[0] & SPINOR_WRSR);

	ret = __spinor_rdsr(&reg[1]);

	if ((!ret) & (reg[1] & (cmd2 | cmd1)))
		printf("Quad Mode Enable OK...\n");
	else
		goto __err_out_;

	return 0;

__err_out_:

	return ret;
}

int spi_nor_fast_read_dual_output(uint start, uint sector_cnt, void *buf)
{
	uint page_addr;
	uint rbyte_cnt;
	u8   sdata[5] = {0};
	int ret = 0;
	uint tmp_cnt, tmp_offset = 0;
	void  *tmp_buf;
	uint txnum, rxnum;
	txnum = 5;
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

	    sdata[0]  =  SPINOR_FREAD_DUAL_OUT;
	    sdata[1]  = (page_addr >> 16) & 0xff;
	    sdata[2]  = (page_addr >> 8 ) & 0xff;
	    sdata[3]  =  page_addr        & 0xff;
	    sdata[4]  = 0 ;

	    rxnum   = rbyte_cnt;
	    tmp_buf = (u8 *)buf + (tmp_offset << 9);

		spic_config_dual_mode(0, 1, 0, txnum);

	//		flush_cache(tmp_buf,rxnum);  // guoyingyang debug
	    if (spic_rw(txnum, (void *)sdata, rxnum, tmp_buf))
	    {
	        ret = -1;
	        break;
	    }

	    sector_cnt -= tmp_cnt;
	    tmp_offset += tmp_cnt;
	}

	spic_config_dual_mode(0, 0, 0, 0);

	return ret;
}


int spi_nor_fast_read_quad_output(uint start, uint sector_cnt, void *buf)
{
	uint page_addr;
	uint rbyte_cnt;
	int ret = 0;
	u8 sdata[5] = {0};
	uint tmp_cnt, tmp_offset = 0;
	void  *tmp_buf;
	uint txnum, rxnum;
	txnum = 5;

	while (sector_cnt) {
		if (sector_cnt > 127)
			tmp_cnt = 127;
		else
			tmp_cnt = sector_cnt;

		page_addr = (start + tmp_offset) * SYSTEM_PAGE_SIZE;
		rbyte_cnt = tmp_cnt * SYSTEM_PAGE_SIZE;

		sdata[0]  =  SPINOR_FREAD_QUAD_OUT;
		sdata[1]  = (page_addr >> 16) & 0xff;
		sdata[2]  = (page_addr >> 8) & 0xff;
		sdata[3]  =  page_addr        & 0xff;
		sdata[4]  = 0;

		rxnum   = rbyte_cnt;
		tmp_buf = (u8 *)buf + (tmp_offset << 9);

		spic_config_dual_mode(0, 2, 0, txnum);

		if (spic_rw(txnum, (void *)sdata, rxnum, (void *)tmp_buf)) {
			ret = -1;
			break;
		}

		sector_cnt -= tmp_cnt;
		tmp_offset += tmp_cnt;
	}

	spic_config_dual_mode(0, 0, 0, 0);

	if (ret == -1)
		quad_state = QUAD_READ_ERR;

	return ret;
}

int boot0_spinor_sector_read(uint start, uint sector_cnt, void *buf)
{
	int ret = 0;

	if (quad_flag == 1) {
		ret = spi_nor_fast_read_quad_output(start, sector_cnt, buf);
		if (ret == -1)
			ret = spi_nor_fast_read_dual_output(start, sector_cnt, buf);
		if (ret == -1)
			ret = spinor_read(start, sector_cnt, buf);
	} else
		ret = spinor_read(start, sector_cnt, buf);

	return ret;
}


