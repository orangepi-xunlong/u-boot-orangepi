/*
 * (C) Copyright 2017-2020
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include "spic.h"
#include "spinand.h"
#include "spinand_osal_boot0.h"


extern __u32 SPN_BLOCK_SIZE;
extern __u32 SPN_PAGE_SIZE;


__s32 spi_nand_getsr(__u32 spi_no, __u8* reg)
{
	__s32 ret = NAND_OP_TRUE;
	__u8 sdata[2] ;
	__u32 txnum;
	__u32 rxnum;

	txnum = 2;
	rxnum = 1;

	sdata[0]=SPI_NAND_GETSR;
	sdata[1]=0xc0;       //status adr:0xc0;feature adr:0xb0;protection adr:0xa0

	Spic_config_dual_mode(spi_no, 0, 0, txnum);
	ret = Spic_rw(spi_no, txnum, (void*)sdata, rxnum, (void*)reg, 0);

	return ret;
}

__s32 spi_wait_status(__u32 spi_no,__u8 status)
{
	__s32 timeout = 0xffffff;
	__s32 ret = NAND_OP_TRUE;
//	__u32 i;

	while(1)
	{
		ret = spi_nand_getsr(spi_no, &status);
		if (ret != NAND_OP_TRUE)
		{
			SPINAND_Print("m0_spi_wait_status getsr fail!\n");
			return ret;
		}
		if(!(status & SPI_NAND_READY))
			break;
		if(timeout < 0)
		{
			SPINAND_Print("m0_spi_wait_status timeout!\n");
			return -ERR_TIMEOUT;
		}
		timeout--;
		//for(i=0; i<100; i++);
	}
	return NAND_OP_TRUE;
}


//mode=0:check ecc status  mode=1:check operation status
__s32 spi_nand_read_status(__u32 spi_no, __u32 chip, __u8 status, __u32 mode)
{
	__s32 ret = NAND_OP_TRUE;

	Spic_sel_ss(spi_no, chip);

	if(mode)
	{
		ret = spi_wait_status(spi_no, status);
		if (ret != NAND_OP_TRUE)
			return ret;

		if(status & SPI_NAND_ERASE_FAIL)
		{
			SPINAND_Print("spi_nand_read_status : erase fail, status = %c\n",status);
			ret = NAND_OP_FALSE;
		}
		if(status & SPI_NAND_WRITE_FAIL)
		{
			SPINAND_Print("spi_nand_read_status : write fail, status = %c\n",status);
			ret = NAND_OP_FALSE;
		}
	}
	else
	{
		ret = spi_wait_status(spi_no, status);
		if (ret != NAND_OP_TRUE)
			return ret;

		if(((status >> SPI_NAND_ECC_FIRST_BIT) & SPI_NAND_ECC_BITMAP) == 0x0)
		{
//			PHY_DBG("no error\n",status);
			ret = NAND_OP_TRUE;
		}
		else if(((status >> SPI_NAND_ECC_FIRST_BIT) & SPI_NAND_ECC_BITMAP) == 0x1)
		{
			//PHY_DBG("ecc correct %c\n",status);
			ret = NAND_OP_TRUE;
		}
		else if(((status >> SPI_NAND_ECC_FIRST_BIT) & SPI_NAND_ECC_BITMAP) == 0x2)
		{
			SPINAND_Print("ecc error %c\n",status);
			ret = -ERR_ECC;
		}

	}

	return ret;
}

 __s32 spi_nand_setblocklock(__u32 spi_no, __u32 chip, __u8 reg)
{
	__s32 ret = NAND_OP_TRUE;
	__u8 sdata[3];
	__u32 txnum;
	__u32 rxnum;
	__u8 status = 0;

	txnum = 3;
	rxnum = 0;

	sdata[0]=SPI_NAND_SETSR;
	sdata[1]=0xa0;       //status adr:0xc0;feature adr:0xb0;protection adr:0xa0
	sdata[2]=reg;

	Spic_sel_ss(spi_no, chip);

	Spic_config_dual_mode(spi_no, 0, 0, txnum);
	ret = Spic_rw(spi_no, txnum, (void*)sdata, rxnum, NULL, 0);
	if (ret != NAND_OP_TRUE)
		return ret;

	ret = spi_wait_status(spi_no, status);
	if (ret != NAND_OP_TRUE)
		return ret;

	return ret;
}

 __s32 spi_nand_setotp(__u32 spi_no, __u32 chip, __u8 reg)
{
	__s32 ret = NAND_OP_TRUE;
	__u8 sdata[3];
	__u32 txnum;
	__u32 rxnum;
	__u8 status = 0;

	txnum = 3;
	rxnum = 0;

	sdata[0]=SPI_NAND_SETSR;
	sdata[1]=0xb0;       //status adr:0xc0;feature adr:0xb0;protection adr:0xa0
	sdata[2]=reg;

	Spic_sel_ss(spi_no, chip);

	Spic_config_dual_mode(spi_no, 0, 0, txnum);
	ret = Spic_rw(spi_no, txnum, (void*)sdata, rxnum, NULL, 0);
	if (ret != NAND_OP_TRUE)
		return ret;

	ret = spi_wait_status(spi_no, status);
	if (ret != NAND_OP_TRUE)
		return ret;

	return ret;
}

__s32 spi_nand_reset(__u32 spi_no, __u32 chip)
{
	__u8 sdata = SPI_NAND_RESET;
	__s32 ret = NAND_OP_TRUE;
	__u32 txnum;
	__u32 rxnum;
	__u8  status = 0;

	txnum = 1;
	rxnum = 0;

	Spic_sel_ss(spi_no, chip);

	Spic_config_dual_mode(spi_no, 0, 0, txnum);
	ret = Spic_rw(spi_no, txnum, (void*)&sdata, rxnum, NULL, 0);
	if (ret != NAND_OP_TRUE)
		return ret;

	ret = spi_wait_status(spi_no, status);
	if (ret != NAND_OP_TRUE)
		return ret;

	ret = NAND_OP_TRUE;

	return ret;
}

__s32 spi_nand_read_x1(__u32 spi_no, __u32 page_num, __u32 mbyte_cnt, __u32 sbyte_cnt, void* mbuf, void* sbuf, __u32 column)
{
	__u32 txnum;
	__u32 rxnum;
	__u32 page_addr = page_num ;
	__u8  sdata[8] = {0};
//	__u8 spare_buf[32] = {0};
	__s32 ret = NAND_OP_TRUE;
//	__u32 i = 0,m;
	__u8 status = 0;
	__s32 ecc_status = 0;
	__u8 bad_flag;
	__u8 plane_select;

	plane_select = (page_addr / (SPN_BLOCK_SIZE / SPN_PAGE_SIZE)) & 0x1;

	txnum = 4;
	rxnum = 0;

	sdata[0] = SPI_NAND_PAGE_READ;
	sdata[1] = (page_addr>>16)&0xff; //9dummy+15bit row adr
	sdata[2] = (page_addr>>8)&0xff;
	sdata[3] = page_addr&0xff;

	Spic_config_dual_mode(spi_no, 0, 0, txnum);
	ret = Spic_rw(spi_no, txnum, (void*)sdata, rxnum, NULL, 0);
	if (ret != NAND_OP_TRUE)
		return ret;

	ret = spi_wait_status(spi_no, status);
	if (ret != NAND_OP_TRUE)
		return ret;

	ecc_status = spi_nand_read_status(spi_no, 0, status, 0);
	if(ecc_status == -ERR_ECC)
		printf("ecc err\n");

	if(mbuf)
	{
		if(OperationOpt & NAND_ONEDUMMY_AFTER_RANDOMREAD)
		{
			txnum = 5;
			rxnum = mbyte_cnt;

			sdata[0] = SPI_NAND_FAST_READ_X1;
			sdata[1] = 0x0; //1byte dummy
			sdata[2] = ((column>>8)&0xff);//4bit dummy,12bit column adr
			sdata[3] = column&0xff;
			sdata[4] = 0x0; //1byte dummy
		}
		else
		{
			//read main data
			txnum = 4;
			rxnum = mbyte_cnt;

			sdata[0] = SPI_NAND_FAST_READ_X1;

			if(OperationOpt & NAND_TWO_PLANE_SELECT)
			{
				if(plane_select)
					sdata[1] = ((column>>8)&0x0f) | 0x10;//3bit dummy,1bit plane,12bit column adr
				else
					sdata[1] = ((column>>8)&0x0f);//3bit dummy,1bit plane,12bit column adr
			}
			else
				sdata[1] = ((column>>8)&0xff);//4bit dummy,12bit column adr

				sdata[2] = column&0xff;
				sdata[3] = 0x0; //1byte dummy
			}

			Spic_config_dual_mode(spi_no, 0, 0, txnum); //signal read, dummy:1byte, signal tx:3
			ret = Spic_rw(spi_no, txnum, (void*)sdata, rxnum, mbuf, 0);
			if (ret != NAND_OP_TRUE)
				return ret;
	}

	if(sbuf)
	{
		if(OperationOpt & NAND_ONEDUMMY_AFTER_RANDOMREAD)
		{
			txnum = 5;
			rxnum = 1;

			sdata[0] = SPI_NAND_FAST_READ_X1;
			sdata[1] = 0x0; //1byte dummy
			sdata[2] = (((512 * (SPN_PAGE_SIZE >> SCT_SZ_WIDTH))>>8)&0xff);//4bit dummy,12bit column adr
			sdata[3] = (512 * (SPN_PAGE_SIZE >> SCT_SZ_WIDTH))&0xff;
			sdata[4] = 0x0; //1byte dummy
		}
		else
		{
			//read bad mark area
			txnum = 4;
			rxnum = 1;

			sdata[0] = SPI_NAND_FAST_READ_X1;

			if(OperationOpt & NAND_TWO_PLANE_SELECT)
			{
				if(plane_select)
					sdata[1] = (((512 * (SPN_PAGE_SIZE >> SCT_SZ_WIDTH))>>8)&0x0f) | 0x10;//3bit dummy,1bit plane,12bit column adr
				else
					sdata[1] = (((512 * (SPN_PAGE_SIZE >> SCT_SZ_WIDTH))>>8)&0x0f);//3bit dummy,1bit plane,12bit column adr
			}
			else
				sdata[1] = (((512 * (SPN_PAGE_SIZE >> SCT_SZ_WIDTH))>>8)&0xff);//4bit dummy,12bit column adr

			sdata[2] = (512 * (SPN_PAGE_SIZE >> SCT_SZ_WIDTH))&0xff;
			sdata[3] = 0x0; //1byte dummy
		}

		Spic_config_dual_mode(spi_no, 0, 0, txnum); //signal read, dummy:1byte, signal tx:3
		ret = Spic_rw(spi_no, txnum, (void*)sdata, rxnum, &bad_flag, 0);
		if (ret != NAND_OP_TRUE)
			return ret;

		if(bad_flag != 0xff)
			*(__u8 *)sbuf = 0;
		else
			*(__u8 *)sbuf = 0xff;
//		*(__u8 *)sbuf = spare_buf[0];
//		MEMCPY((__u8 *)sbuf, (__u8 *)spare_buf, sbyte_cnt);

	}

//	ret = m0_spi_nand_read_status(spi_no, 0, status, 0);
//	printf("ecc_status %d\n",ecc_status);
	return 0;
}


__s32 read_single_page(struct boot_spinand_physical_param *readop, __u32 data_size, __u32 spare_only_flag)
{
	__s32 ret = NAND_OP_TRUE;
	__u32 addr;
//	__u32 first_sector;
//	__u32 sector_num;
//	__u8 *data_buf;

//	data_buf = PageCachePool.TmpPageCache;

//	addr = _cal_addr_in_chip(readop->block,readop->page);
	addr = readop->block * SPN_BLOCK_SIZE / SPN_PAGE_SIZE + readop->page;


	Spic_sel_ss(0, readop->chip);

//	first_sector = _cal_first_valid_bit(readop->sectorbitmap);
//	sector_num = _cal_valid_bits(readop->sectorbitmap);

	if(spare_only_flag)
		ret = spi_nand_read_x1(0, addr, 0, 16, NULL, readop->oobbuf, 0);
	else
		ret = spi_nand_read_x1(0, addr, data_size, 16, (__u8 *)readop->mainbuf, readop->oobbuf, 0);

//	if(spare_only_flag == 0)
//		MEMCPY( (__u8 *)readop->mainbuf + 512 * first_sector, data_buf, 512 * sector_num);

	return ret;
}
