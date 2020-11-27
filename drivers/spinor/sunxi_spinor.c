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
#include <malloc.h>
#include <spi.h>
#include <asm/arch/spi.h>
#include <sunxi_mbr.h>
#include <private_boot0.h>
#include <asm/arch/spinor.h>
#include <sunxi_board.h>
#include <fdt_support.h>
#include <sys_config_old.h>
#include <private_toc.h>


static int   spinor_flash_inited = 0;
uint  total_write_bytes;
static char *spinor_store_buffer;
//static char  spinor_mbr[SUNXI_MBR_SIZE];
static char *spinor_write_cache;
static int   spinor_cache_block = -1;
static int   spinor_4bytes_addr_mode = 0;
static int spi_freq = 0;
static int quad_flag;
static int quad_state;
static int check_mbr_flag;

#define QUAD_READ_ERR	(-1)

#define ENABLE_4BYTES	1
#define DISABLE_4BYTES  0
#define  SYSTEM_PAGE_SIZE        (512)
#define  SPINOR_PAGE_SIZE        (256)
#define  NPAGE_IN_1SYSPAGE       (SYSTEM_PAGE_SIZE/SPINOR_PAGE_SIZE)
#define  SPINOR_BLOCK_BYTES      (64 * 1024)
#define  SPINOR_BLOCK_SECTORS    (SPINOR_BLOCK_BYTES/512)
#define SPINOR_FREAD_DUAL_OUT    0x3b // sclk <= 75MHz
#define SPI_NORMAL_FRQ	(40000000)
#define SPINOR_FREAD_QUAD_OUT	0x6b

static void spinor_enter_4bytes_addr(int);
static void spinor_config_addr_mode(u8 *sdata, uint page_addr, uint *num, u8 cmd);
static int set_quad_mode(u8 cmd1, u8 cmd2);
static int spi_nor_fast_read_quad_output(uint start, uint sector_cnt, void *buf);
extern int spinor_get_boot0_size(uint *length, void *addr);

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

static uint __spinor_wrsr(u8 reg)
{
	u8 sdata[2] = {0};
	s32 ret = -1;
	u8  status = 0;
	u32 i = 0;
	uint txnum = 0;
	uint rxnum = 0 ;
	ret = __spinor_wren();
	if (ret==-1)
		goto __err_out_;
	txnum = 2;
	rxnum = 0;

	sdata[0] = SPINOR_WRSR;
	sdata[1] = reg;

	spic_config_dual_mode(0, 0, 0, txnum);
	ret = spic_rw(txnum, (void*)sdata, rxnum, (void*)0);
	if (ret==-1)
		goto __err_out_;

	do {
		ret = __spinor_rdsr( &status);
		if (ret==-1)
			goto __err_out_;
		for(i=0; i<100; i++);
	} while(status&0x01);
	printf("_spinor_wrsr status is %d \n",status);
	ret = 0;

__err_out_:

	return ret;
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
static int __spinor_erase_block(uint block_index)
{
    uint  blk_addr = block_index * SPINOR_BLOCK_BYTES;
    u8    sdata[4] = {0};
    int   ret = -1;
    u8    status = 0;
    uint  i = 0;
    uint  txnum, rxnum;

	ret = __spinor_wren();
	if (-1 == ret)
	{
	    return -1;
	}

	txnum = 4;
	rxnum = 0;

	sdata[0] = SPINOR_SE;
	sdata[1] = (blk_addr >> 16) & 0xff;
	sdata[2] = (blk_addr >> 8 ) & 0xff;
	sdata[3] =  blk_addr        & 0xff;

	spic_config_dual_mode(0, 0, 0, txnum);
	ret = spic_rw(txnum, (void *)sdata, rxnum, 0);
	if (-1 == ret)
	{
	 	return -1;
	}

	do
	{
		ret = __spinor_rdsr(&status);
		if (-1 == ret)
		{
			return -1;
		}
		for (i = 0; i < 100; i++);
	} while (status & 0x01);

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
static int __spinor_erase_all(void)
{
	uint  sdata = 0;
	int   ret = -1;
	u8    status = 0;
	uint  txnum, rxnum;
	printf("begin to erase all .");
	int count = 0;
	ret = __spinor_wren();
	if (-1 == ret)
	{
	 	return -1;
	}

	txnum = 1;
	rxnum = 0;

	sdata = SPINOR_BE;

	spic_config_dual_mode(0, 0, 0, txnum);
	ret = spic_rw(txnum, (void*)&sdata, rxnum, 0);
	if (ret==-1)
	{
		return -1;
	}

	do
	{
		ret = __spinor_rdsr(&status);
		if (-1 == ret)
		{
		 	return -1;
		}
		count ++;
		__msdelay(1000);
		printf(".");
	} while (status & 0x01);
	printf("\nstatus = %d\n",status);
	printf("count = %d \n",count);
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
static int __spinor_pp(uint page_addr, void *buf, uint len)
{
	u8   sdata[300];
	u8   status = 0;
	uint i = 0;
	int  ret = -1;
	uint txnum = 0;
	uint rxnum = 0;

	if (len > 256)
	{
		return -1;
	}

	ret = __spinor_wren();
	if (ret < 0)
	{
		return -1;
	}

	memset((void *)sdata, 0xff, sizeof(sdata)/sizeof(sdata[0]));
	spinor_config_addr_mode(sdata,page_addr,&txnum,SPINOR_PP);
	memcpy((void *)(sdata+txnum), buf, len);
	txnum += len;

	spic_config_dual_mode(0, 0, 0, txnum);
	ret = spic_rw(txnum, (void *)sdata, rxnum, 0);
	if (-1 == ret)
	{
		return -1;
	}

	do
	{
		ret = __spinor_rdsr(&status);
		if (ret < 0)
		{
			return -1;
		}

		for (i=0; i < 100; i++);
	} while (status & 0x01);

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
static int __spinor_read_id(uint *id)
{
	uint sdata = SPINOR_RDID;
	int  txnum ;
	txnum = 1;
	spic_config_dual_mode(0, 0, 0, txnum);
	return spic_rw(1, (void *)&sdata, 3, (void *)id);
}

static int spi_nor_fast_read_dual_output(uint start, uint sector_cnt, void *buf)
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

static int __spinor_sector_normal_read(uint start, uint sector_cnt, void *buf)
{
	uint page_addr;
	uint rbyte_cnt;
	u8   sdata[4] = {0};
	int ret = 0;
	uint tmp_cnt, tmp_offset = 0;
	void  *tmp_buf;
	uint txnum, rxnum;
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
	    tmp_buf = (u8 *)buf + (tmp_offset << 9);

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

static int __spinor_sector_read(uint start, uint sector_cnt, void *buf)
{
	int ret = 0;

	if (quad_flag == 1) {
		ret = spi_nor_fast_read_quad_output(start, sector_cnt, buf);
		if (ret == -1)
			ret = spi_nor_fast_read_dual_output(start, sector_cnt, buf);
		if (ret == -1)
			ret = __spinor_sector_normal_read(start, sector_cnt, buf);
	} else if (spi_freq >= SPI_NORMAL_FRQ)
		ret = spi_nor_fast_read_dual_output(start, sector_cnt, buf);
	else
		ret = __spinor_sector_normal_read(start, sector_cnt, buf);

	return ret;
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
static int __spinor_sector_write(uint sector_start, uint sector_cnt, void *buf)
{
	uint page_addr = sector_start *  SYSTEM_PAGE_SIZE;
	uint nor_page_cnt = sector_cnt * (NPAGE_IN_1SYSPAGE);
	uint i = 0;
	int ret = -1;

	printf("start = 0x%x, cnt=0x%x\n", sector_start, sector_cnt);
//	printf("nor_page_cnt=%d\n", nor_page_cnt);
	for (i = 0; i < nor_page_cnt; i++)
	{
//		//printf("spinor program page : 0x%x\n", page_addr + SPINOR_PAGE_SIZE * i);
//		if((i & 0x1ff) == 0x1ff)
//		{
//			printf("current cnt=%d\n", i);
//		}
		ret = __spinor_pp(page_addr + SPINOR_PAGE_SIZE * i, (void *)((uint)buf + SPINOR_PAGE_SIZE * i), SPINOR_PAGE_SIZE);
		if (-1 == ret)
		{
		    return -1;
		}
	}

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
	int i, id_size, spi_size = 0;
	u32 id = 0;

	spi_size = spinor_size();
	if(spinor_flash_inited)
	{
		puts("sunxi spinor is already inited\n");
                return 0;
	}
	else
	{
		puts("sunxi spinor is initing...");
		if(spic_init(0))
		{
			puts("Fail\n");

			return -1;
		}
		else
		{
			puts("OK\n");
		}

	}
	__spinor_read_id(&id);
	if(!id)
		return -1;

	printf("spinor id:0x%x\n",id);

	spinor_flash_inited ++;

	if(spi_size > 16*1024*1024/512)
	{
		spinor_4bytes_addr_mode = 1;
		spinor_enter_4bytes_addr(ENABLE_4BYTES);
	}

	if(uboot_spare_head.boot_data.work_mode != WORK_MODE_BOOT)
	{
		spinor_store_buffer  = (char *)malloc(SPINOR_STORE_BUFFER_SIZE);
		if(!spinor_store_buffer)
		{
			puts("memory malloced fail for store buffer\n");

			return -1;
		}
	}
	spinor_write_cache  = (char *)malloc_noncache(64 * 1024);
	if(!spinor_write_cache)
	{
		puts("memory malloced fail for spinor data buffer\n");

		if(spinor_store_buffer)
		{
			free(spinor_store_buffer);

			return -1;
		}
	}

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
	if (script_parser_fetch("boot_spi_board0", "boot_spi_speed_hz", &spi_freq, 1) < 0)
		spi_freq = SPI_DEFAULT_CLK;
	printf("spi_freq = %d\n", spi_freq);

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
int spinor_exit(int force)
{
	int need_exit = 0;
	int ret = 0;

	if(!spinor_flash_inited)
	{
		printf("sunxi spinor has not inited\n");

		return -1;
	}
	if(force == 1)
	{
		if(spinor_flash_inited)
		{
			printf("force sunxi spinor exit\n");

			spinor_flash_inited = 0;
			need_exit = 1;
		}
	}
	else
	{
		if(spinor_flash_inited)
		{
			spinor_flash_inited --;
		}
		if(!spinor_flash_inited)
		{
			printf("sunxi spinor is exiting\n");
			need_exit = 1;
		}
	}
	if(need_exit)
	{
		//printf("spi cache block == %d \n",spinor_cache_block);
		//printf("spi cache addr == %x \n",(uint)spinor_write_cache);
		//if((spinor_cache_block >= 0) && (spinor_write_cache))
		//{
		//	if(__spinor_sector_write(spinor_cache_block * SPINOR_BLOCK_SECTORS, SPINOR_BLOCK_SECTORS, spinor_write_cache))
		//	{
		//		printf("spinor write cache block fail\n");

		//		ret = -1;
		//	}
		//}
		//spic_exit(0);
		free(spinor_store_buffer);
		free_noncache(spinor_write_cache);
		spic_exit(0);
	}

	return ret;
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
int spinor_read(uint start, uint nblock, void *buffer)
{
	int tmp_block_index;

	printf("spinor read: start 0x%x, sector 0x%x\n", start, nblock);

	if(spinor_cache_block < 0)
	{
		debug("%s %d\n", __FILE__, __LINE__);
		if(__spinor_sector_read(start, nblock, buffer))
		{
			printf("spinor read fail no buffer\n");

			return 0;
		}
	}
	else
	{
		tmp_block_index = start/SPINOR_BLOCK_SECTORS;
		if(spinor_cache_block == tmp_block_index)
		{
			memcpy(buffer, spinor_write_cache + (start % SPINOR_BLOCK_SECTORS) * 512, nblock * 512);
		}
		else
		{
			if(__spinor_sector_read(start, nblock, buffer))
			{
				printf("spinor read fail with buffer\n");

				return 0;
			}
		}
	}

	return nblock;
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
int spinor_write(uint start, uint nblock, void *buffer)
{
	int tmp_block_index;

	printf("spinor write: start 0x%x, sector 0x%x\n", start, nblock);

	if(spinor_cache_block < 0)
	{
		spinor_cache_block = start/SPINOR_BLOCK_SECTORS;
		if(__spinor_sector_read(spinor_cache_block * SPINOR_BLOCK_SECTORS, SPINOR_BLOCK_SECTORS, spinor_write_cache))
		{
			printf("spinor read cache block fail\n");

			return 0;
		}
		__spinor_erase_block(spinor_cache_block);
		memcpy(spinor_write_cache + (start % SPINOR_BLOCK_SECTORS) * 512, buffer, nblock * 512);
	}
	else
	{
		tmp_block_index = start/SPINOR_BLOCK_SECTORS;
		if(spinor_cache_block == tmp_block_index)
		{
			memcpy(spinor_write_cache + (start % SPINOR_BLOCK_SECTORS) * 512, buffer, nblock * 512);
		}
		else
		{
			if(__spinor_sector_write(spinor_cache_block * SPINOR_BLOCK_SECTORS, SPINOR_BLOCK_SECTORS, spinor_write_cache))
			{
				printf("spinor write cache block fail\n");

				return 0;
			}
			spinor_cache_block = tmp_block_index;
			if(__spinor_sector_read(spinor_cache_block * SPINOR_BLOCK_SECTORS, SPINOR_BLOCK_SECTORS, spinor_write_cache))
			{
				printf("spinor read cache block fail\n");

				return 0;
			}
			__spinor_erase_block(spinor_cache_block);
			memcpy(spinor_write_cache + (start % SPINOR_BLOCK_SECTORS) * 512, buffer, nblock * 512);
		}
	}

	return nblock;
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
int spinor_flush_cache(void)
{
	if((spinor_cache_block >= 0) && (spinor_write_cache))
	{
		if(__spinor_sector_write(spinor_cache_block * SPINOR_BLOCK_SECTORS, SPINOR_BLOCK_SECTORS, spinor_write_cache))
		{
			printf("spinor write cache block fail\n");

			return -1;
		}
	}

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :  erase:  0:
*
*							  1: erase the all flash
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int spinor_erase_all_blocks(int erase)
{
	if(erase)		//当参数为0，表示根据情况，自动判断是否需要进入擦除
	{
		__spinor_erase_all();
	}

	return 0;
}

int spinor_erase(int erase, void *mbr_buffer)
{
	int i = 0,start = 0;
	int ret =-1;
	unsigned int from, nr,erase_block;
	sunxi_mbr_t *mbr = (sunxi_mbr_t *)mbr_buffer;

	if (!erase)
		return 0;

	for (i=0;i<mbr->PartCount;i++)
	{
		printf("erase %s part\n", mbr->array[i].name);

		from = mbr->array[i].addrlo + CONFIG_SPINOR_LOGICAL_OFFSET;
		nr = mbr->array[i].lenlo;
		printf("from:0x%x,nblock:0x%x \n",from,nr);

		for (start=from;start<(from+nr);start+=SPINOR_BLOCK_SECTORS)
		{
			erase_block = start/SPINOR_BLOCK_SECTORS;
			 printf("erasing block index:%x (0x%x)\n",erase_block,(from+nr)/SPINOR_BLOCK_SECTORS);
			ret = __spinor_erase_block(erase_block);
			 if (ret <0)
			{
			    printf("erase %s part fail\n", mbr->array[i].name);
			    return -1;
			}
		}
	}
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
int spinor_size(void)
{
	int size = 0;
	int ret = -1;

	ret = script_parser_fetch("norflash", "size", &size, 1);
	if (ret < 0 )
	{
		size = 8*1024*1024/512;
		printf("get flash_size warning\n");
	}
	else
	{
		size =  size*1024*1024/512;
	}
	printf("flash size =0x%x sectors\n",size);

	return size;
}

static int __spinor_sprite_sector_write(uint start,uint nsector,void* buffer)
{
	int sector_index;

	debug("start: 0x%x, nsector: 0x%x \n",start,nsector);

	if (nsector > SPINOR_BLOCK_SECTORS-start % SPINOR_BLOCK_SECTORS)
	{
	    printf("sector cnt error\n");
	    return -1;
	}

	sector_index = start/SPINOR_BLOCK_SECTORS;
	if ((start % SPINOR_BLOCK_SECTORS)||(nsector < SPINOR_BLOCK_SECTORS))
	{
	    if (__spinor_sector_read(sector_index * SPINOR_BLOCK_SECTORS, SPINOR_BLOCK_SECTORS, spinor_store_buffer))
	    {
	        printf("spinor read  sector fail\n");
	       return -1;
	    }
	}

	memcpy((spinor_store_buffer + ((start % SPINOR_BLOCK_SECTORS) * 512)), buffer, nsector*512);

	if (__spinor_erase_block(sector_index))
	{
	    printf("erase 0x%x sector fail\n",sector_index);
	    return -1;
	}

	if (__spinor_sector_write(sector_index*SPINOR_BLOCK_SECTORS,SPINOR_BLOCK_SECTORS,spinor_store_buffer))
	{
	    printf("spinor write  0x%x sector fail\n",sector_index);
	   return -1;
	}

#ifdef SPINOR_DEBUG
   /* verify */
   if (__spinor_sector_read(start, nsector, spinor_store_buffer))
   {
       printf("spinor read  sector fail\n");
      return -1;
   }
   if (memcmp(spinor_store_buffer,buffer,nsector*512))
   {
       printf("***write 0x%x sector fail***\n",sector_index*SPINOR_BLOCK_SECTORS);
       return -1;
   }
#endif
	return SPINOR_BLOCK_SECTORS;
}

int spinor_sprite_write(uint start, uint nblock, void *buffer)
{
	uint start_sector;
	int ret;
	int offset;
	int nsector = 0;
	int sector_once_write = SPINOR_BLOCK_SECTORS;

	if (nblock < SPINOR_BLOCK_SECTORS)
	{
		sector_once_write = nblock;
	}

	offset = start%SPINOR_BLOCK_SECTORS;
	/*  deal with first sector,make it align with 64k */
	if (offset)
	{
		nsector = SPINOR_BLOCK_SECTORS - offset;
		ret =  __spinor_sprite_sector_write(start, nsector, buffer);
		if (ret < 0)
		{
			printf("spinor sprite write fail\n");
			return 0;
		}

	}

	for (start_sector =start+nsector;start_sector<start+nblock;start_sector+= SPINOR_BLOCK_SECTORS)
	{
		if (start+nblock - start_sector < SPINOR_BLOCK_SECTORS)
			sector_once_write = start+nblock - start_sector;

		ret = __spinor_sprite_sector_write(start_sector,sector_once_write, (void *)((uint)buffer + (start_sector -start) * 512));
		if (ret < 0)
		{
			printf("spinor sprite write fail\n");
			return 0;
		}
	}
	return nblock;
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
//static void spinor_dump(void *buffer, int len)
//{
//	char *addr;
//	int  i;
//
//	addr = (char *)buffer;
//	for(i=0;i<len;i++)
//	{
//		printf("%02x  ", addr[i]);
//		if((i & 0x07) == 0x07)
//		{
//			puts("\n");
//		}
//	}
//}

int update_boot0_dram_para(char *buffer)
{
	boot0_file_head_t    *boot0  = (boot0_file_head_t *)buffer;
	int i;
	uint *addr = (uint *)DRAM_PARA_STORE_ADDR;

	//校验特征字符是否正确
	printf("%s\n", boot0->boot_head.magic);
	if(strncmp((const char *)boot0->boot_head.magic, BOOT0_MAGIC, MAGIC_SIZE))
	{
		printf("sunxi sprite: boot0 magic is error\n");
		return -1;
	}
	if(sunxi_verify_checksum((void *)buffer, boot0->boot_head.length, boot0->boot_head.check_sum))
	{
		printf("sunxi sprite: boot0 checksum is error\n");

		return -1;
	}

	for(i=0;i<32;i++)
	{
		printf("dram para[%d] = %x\n", i, addr[i]);
	}
	memcpy((void *)&boot0->prvt_head.dram_para, (void *)DRAM_PARA_STORE_ADDR, 32 * 4);
	set_boot_dram_update_flag(boot0->prvt_head.dram_para);
	/* regenerate check sum */
	boot0->boot_head.check_sum = sunxi_generate_checksum(buffer, boot0->boot_head.length, boot0->boot_head.check_sum);
	//校验数据是否正确
	if(sunxi_verify_checksum((void *)buffer, boot0->boot_head.length, boot0->boot_head.check_sum))
	{
		printf("sunxi sprite: boot0 checksum is error\n");
		return -1;
	}
	printf("update dram para success \n");
	return 0;

}

static int __spinor_boot_sector_write(uint start, uint nsector, void *buffer)
{
	int sector_index;

	debug("start: 0x%x, nsector: 0x%x \n", start, nsector);

	if (nsector > SPINOR_BLOCK_SECTORS-start % SPINOR_BLOCK_SECTORS) {
		printf("sector cnt error\n");
		return -1;
	}

	sector_index = start / SPINOR_BLOCK_SECTORS;
	if ((start % SPINOR_BLOCK_SECTORS) || (nsector < SPINOR_BLOCK_SECTORS)) {
		if (__spinor_sector_read(sector_index * SPINOR_BLOCK_SECTORS, SPINOR_BLOCK_SECTORS, spinor_write_cache)) {
			printf("spinor read  sector fail\n");
			return -1;
		}
	}

	memcpy((spinor_write_cache + ((start % SPINOR_BLOCK_SECTORS) * 512)), buffer, nsector*512);

	if (__spinor_erase_block(sector_index)) {
		printf("erase 0x%x sector fail\n", sector_index);
		return -1;
	}

	if (__spinor_sector_write(sector_index * SPINOR_BLOCK_SECTORS, SPINOR_BLOCK_SECTORS, spinor_write_cache)) {
		printf("spinor write  0x%x sector fail\n", sector_index);
		return -1;
	}

#ifdef SPINOR_DEBUG
	/* verify */
	if (__spinor_sector_read(start, nsector, spinor_write_cache)) {
		printf("spinor read  sector fail\n");
		return -1;
	}

	if (memcmp(spinor_write_cache, buffer, nsector*512)) {
		printf("***write 0x%x sector fail***\n", sector_index*SPINOR_BLOCK_SECTORS);
		return -1;
	}
#endif
	return SPINOR_BLOCK_SECTORS;
}

int spinor_boot_write(uint start, uint nblock, void *buffer)
{
	uint start_sector;
	int ret;
	int offset;
	int nsector = 0;
	int sector_once_write = SPINOR_BLOCK_SECTORS;

	if (nblock < SPINOR_BLOCK_SECTORS) {
		sector_once_write = nblock;
	}

	offset = start%SPINOR_BLOCK_SECTORS;
	/*  deal with first sector,make it align with 64k */
	if (offset) {
		nsector = SPINOR_BLOCK_SECTORS - offset;
		ret =  __spinor_boot_sector_write(start, nsector, buffer);
		if (ret < 0) {
			printf("spinor sprite write fail\n");
			return 0;
		}
	}

	for (start_sector = start + nsector; start_sector < start + nblock; start_sector += SPINOR_BLOCK_SECTORS) {
		if (start+nblock - start_sector < SPINOR_BLOCK_SECTORS)
			sector_once_write = start+nblock - start_sector;
		ret = __spinor_boot_sector_write(start_sector, sector_once_write,
						(void *)((uint)buffer + (start_sector - start) * 512));
		if (ret < 0) {
			printf("spinor sprite write fail\n");
			return 0;
		}
	}
	return nblock;
}

int spinor_download_uboot(uint length, void *buffer)
{
	int ret = -1;
	if(spinor_flash_inited==0)
	{
		printf("warning:spinor need init\n");
		sunxi_sprite_init(0);
	}

	if (uboot_spare_head.boot_data.work_mode == WORK_MODE_BOOT) {
		ret = spinor_boot_write(UBOOT_START_SECTOR_IN_SPINOR, length/512, buffer);
	} else {
		ret = spinor_sprite_write(UBOOT_START_SECTOR_IN_SPINOR, length/512, buffer);
	}

	if (ret < 0) {
		printf("spinor download uboot fail\n");
		return -1;
	}

	return 0;
}

int spinor_download_boot0(uint length, void *buffer)
{
	int ret = -1;

    ret = spinor_get_boot0_size(&length, buffer);
	if (ret < 0) {
		printf("spinor boot0_size is fail\n");
		return -1;
	}
	if (ret > 0) {
		if (update_boot0_dram_para(buffer)) {
			return -1;
		}
	}

	if (uboot_spare_head.boot_data.work_mode == WORK_MODE_BOOT) {
		ret = spinor_boot_write(0, length/512, buffer);
	} else {
		ret = spinor_sprite_write(0, length/512, buffer);
	}

	if (ret < 0)
	{
		printf("spinor download boot0 fail\n");
		return -1;
	}

	return 0;
}

int spinor_datafinish(void)
{
	uint  id = 0xff;
	int   ret;

	printf("spinor_datafinish\n");
	__spinor_read_id(&id);

	printf("spinor id = 0x%x\n", id);
	__spinor_wrsr(0);
	__spinor_erase_all();

	printf("spinor has erasered\n");

	if(update_boot0_dram_para(spinor_store_buffer))
	{
		return -1;
	}

	flush_cache((unsigned long)spinor_store_buffer,total_write_bytes);
	printf("total write bytes = %d\n", total_write_bytes);
	ret = __spinor_sector_write(0, total_write_bytes/512, spinor_store_buffer);
	if(ret)
	{
		printf("spinor write img fail\n");

		return -1;
	}
//	buffer = (char *)malloc(8 * 1024 * 1024);
//	memset(buffer, 0xff, 8 * 1024 * 1024);
//	ret = __spinor_sector_read(0, total_write_bytes/512, buffer);
//	if(ret)
//	{
//		printf("spinor read img fail\n");
//
//		return -1;
//	}
//	printf("spinor read data ok\n");
//	for(i=0;i<total_write_bytes;i++)
//	{
//		if(buffer[i] != spinor_store_buffer[i])
//		{
//			printf("compare spinor read and write error\n");
//			printf("offset %d\n", i);
//
//			return -1;
//		}
//	}
	spinor_enter_4bytes_addr(DISABLE_4BYTES);
	printf("spinor download data ok\n");

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
//int nand_get_mbr(char *buffer, int mbr_size)
//{
//	memcpy(spinor_mbr, buffer, SUNXI_MBR_SIZE);

//	return 0;
//}

#ifdef SPI_NOR_TEST
s32 spi_nor_rw_test(u32 spi_no)
{

	const u32 test_len = 256*64;
	char* wbuf = (char*)malloc(test_len);
	char* rbuf = (char*)malloc(test_len);
	u32 i;
	s32 ret;
	uint id =0xffff;

	if(spinor_init(0))
	{
		printf("spinor init failed \n");
		goto _free_mem;
		return -1;
	}
	if(__spinor_read_id(&id))
	{
		printf("spinor read id failed \n");
		goto _free_mem;
		return -1;
	}
	__spinor_wrsr(0);
	printf("the id is %x\n",id);
	printf("bfeore spi_nor_rw_test\n");

	__spinor_sector_read(0, 1, rbuf);

	for(i = 0; i < 256; i+=4)
	{
		printf("verify rbuf=0x%x\n",*(u32 *)(rbuf+i));
	}

	printf("before erase all \n");
	ret = __spinor_erase_all();
	printf("after erase all \n");
	printf("bfeore spi_nor_rw_test,ret=%d\n",ret);

	if (!wbuf || !rbuf) {
		printf("spi %d malloc buffer failed\n", spi_no);
		goto _free_mem;
		return -1;
	}

	for (i=0; i<test_len; i++)
		wbuf[i] = i;


	i = 0;
	ret = __spinor_sector_read( i, 1, rbuf);
 	if (ret) {
 		printf("spi %d read page %d failed\n", spi_no,i);
		goto _free_mem;
		return -1;
 	}

	for (i = 0; i < (test_len >> 8)/4; i++)
	printf("verify rbuf=[0x%x]\n",*(u32 *)(rbuf+i*4));

//	ret = __spinor_erase_block(0);	//erase 64K
//	if (ret) {
//		printf("spi erase ss1 nor failed\n");
//		return -1;
//	}

    for (i=0; i<10; i++)
	printf("b rbuf=[0x%x]\n",*(u32 *)(rbuf));
    for (i = 0; i < (test_len >> 8); i++) {
		printf("begin to write \n");
		ret = __spinor_sector_write(i, 1, wbuf);
		if (ret) {
			printf("spi %d write page %d failed\n", spi_no,i);
			goto _free_mem;
			return -1;
		}
		printf("end to write \n");
		memset(rbuf, 0, 256);
		printf("begin to read\n");
		ret = __spinor_sector_read(i, 1, rbuf);
		if (ret) {
			printf("spi %d read page %d failed\n", spi_no,i);
			goto _free_mem;
			return -1;
		}
		printf("end to read \n");
		//compare data
		if (memcmp(wbuf, rbuf, 256)) {
			printf("spi %d page %d read/write failed\n", spi_no, i);
			while(1);
			goto _free_mem;
			return -1;
		} else
			printf("spi %d page %d read/write ok\n", spi_no, i);

	}

	_free_mem:
		free(wbuf);
		free(rbuf);

	return 0;
}
#endif


u32 try_spi_nor(u32 spino)
{
	uint id =0;
	if(spinor_init(spino))
	{
		printf("spinor init failed \n");
		return -1;
	}
	if(__spinor_read_id(&id))
	{
		printf("spinor read id failed \n");
		return -1;
	}
	if(id == 0)
	{
		return -1;
	}
	printf("spinor id is %x \n",id);
	return 0;
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

static void spinor_config_addr_mode(u8 *sdata, uint page_addr, uint *num, u8 cmd)
{
	if ((sdata == NULL)||(num == NULL))
	{
		return;
	}

	if(spinor_4bytes_addr_mode == 0)
	{
		*num = 4;
		sdata[0]  =  cmd;
		sdata[1]  = (page_addr >> 16) & 0xff;
		sdata[2]  = (page_addr >> 8 ) & 0xff;
		sdata[3]  =  page_addr        & 0xff;
	}
	else if(spinor_4bytes_addr_mode == 1)
	{
		*num = 5;
		sdata[0]  =  cmd;
		sdata[1]  = (page_addr >> 24) & 0xff;
		sdata[2]  = (page_addr >> 16) & 0xff;
		sdata[3]  = (page_addr >> 8 ) & 0xff;
		sdata[4]  =  page_addr        & 0xff;
	}

}

static int set_quad_mode(u8 cmd1, u8 cmd2)
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

		mdelay(5);
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

static int spi_nor_fast_read_quad_output(uint start, uint sector_cnt, void *buf)
{
	uint page_addr;
	uint rbyte_cnt;
	int ret = 0;
	u8 sdata[5] = {0};
	uint tmp_cnt, tmp_offset = 0;
	void  *tmp_buf;
	uint txnum, rxnum;
	sunxi_mbr_t	*mbr;
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
	mbr = (sunxi_mbr_t *)buf;
	if (check_mbr_flag == 0) {
		if (!strncmp((const char *)mbr->magic, SUNXI_MBR_MAGIC, 8)) {
				int crc = 0;
				crc = crc32(0, (const unsigned char *)&mbr->version, SUNXI_MBR_SIZE-4);
				if (crc != mbr->crc32) {
					quad_flag = 0;
					ret = -1;
				}
		} else {
			quad_flag = 0;
			ret = -1;
		}
		check_mbr_flag = 1;
	}
	spic_config_dual_mode(0, 0, 0, 0);

	if (ret == -1)
		quad_state = QUAD_READ_ERR;

	return ret;
}

