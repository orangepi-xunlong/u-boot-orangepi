/*
 * (C) Copyright 2017-2020
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include "spic.h"
#include "spinand.h"
#include "spinand_osal_boot0.h"


__u32 SPIC_IO_BASE = 0;

__s32 Wait_Tc_Complete(void)
{
	__u32 timeout = 0xffffff;

	while(!(readl(SPI_ISR)&(0x1<<12)))//wait transfer complete
	{
		timeout--;
		if (!timeout)
			break;
	}
	if(timeout == 0)
	{
		printf("TC complete wait status timeout!\n");
		return -ERR_TIMEOUT;
	}

	return 0;
}

__s32 Spic_init(__u32 spi_no)
{
	__u32 rval;

	//init pin
    SPINAND_PIORequest(spi_no);
    SPIC_IO_BASE = SPINAND_GetIOBaseAddr();
    //init clk
    SPINAND_ClkRequest(spi_no);
    SPINAND_SetClk(spi_no, 30);

	rval = SPI_SOFT_RST|SPI_TXPAUSE_EN|SPI_MASTER|SPI_ENABLE;
	writel(rval, SPI_GCR);

	//set ss to high,discard unused burst,SPI select signal polarity(low,1=idle)
	rval = SPI_SET_SS_1|SPI_DHB|SPI_SS_ACTIVE0;
	writel(rval, SPI_TCR);

	writel(0x1000, SPI_CCR); //SPI data clk = source clk / 2, Duty Ratio ¡Ö 50%

	writel(SPI_TXFIFO_RST|(SPI_TX_WL<<16)|(SPI_RX_WL), SPI_FCR);
	writel(SPI_ERROR_INT, SPI_IER);

	return 0;
}

__s32 Spic_exit(__u32 spi_no)
{
	__u32 rval;
//	__s32 irq;

	rval = readl(SPI_GCR);
	rval &= (~(SPI_SOFT_RST|SPI_MASTER|SPI_ENABLE));
	writel(rval, SPI_GCR);

	SPINAND_ClkRelease(spi_no);

    //init pin
    SPINAND_PIORelease(spi_no);

//	MEMSET(&spicinfo[spi_no], 0, sizeof(struct spic_info));

	rval = SPI_SET_SS_1|SPI_DHB|SPI_SS_ACTIVE0;   //set ss to high,discard unused burst,SPI select signal polarity(low,1=idle)
	writel(rval, SPI_TCR);

	return 0;
}

void Spic_set_master_slave(__u32 spi_no, __u32 master)
{
	__u32 rval = readl(SPI_GCR)&(~(1 << 1));
	rval |= master << 1;
	writel(rval, SPI_GCR);
//	spicinfo[spi_no].master = master;
}

void Spic_sel_ss(__u32 spi_no, __u32 ssx)
{
	__u32 rval = readl(SPI_TCR)&(~(3 << 4));
	rval |= ssx << 4;
	writel(rval, SPI_TCR);
}
// add for aw1650
void Spic_set_transmit_LSB(__u32 spi_no, __u32 tmod)
{
	__u32 rval = readl(SPI_TCR)&(~(1 << 12));
	rval |= tmod << 12;
	writel(rval, SPI_TCR);
}

void Spic_set_ss_level(__u32 spi_no, __u32 level)
{
	__u32 rval = readl(SPI_TCR)&(~(1 << 7));
	rval |= level << 7;
	writel(rval, SPI_TCR);
}


void Spic_set_sample_mode(__u32 spi_no, __u32 smod)
{
	__u32 rval = readl(SPI_TCR)&(~(1 << 13));
	rval |= smod << 13;
	writel(rval, SPI_TCR);
}

void Spic_set_sample(__u32 spi_no, __u32 sample)
{
	__u32 rval = readl(SPI_TCR)&(~(1 << 11));
	rval |= sample << 11;
	writel(rval, SPI_TCR);
}

void Spic_set_trans_mode(__u32 spi_no, __u32 mode)
{
	__u32 rval = readl(SPI_TCR)&(~(3 << 0));
	rval |= mode << 0;
	writel(rval, SPI_TCR);
}

void Spic_set_wait_clk(__u32 spi_no, __u32 swc, __u32 wcc)
{
	writel((swc << 16) | (wcc), SPI_WCR);
}

void Spic_config_dual_mode(__u32 spi_no, __u32 rxdual, __u32 dbc, __u32 stc)
{
	writel((rxdual<<28)|(dbc<<24)|(stc), SPI_BCC);
}

/*
 * spi txrx
 * _ _______ ______________
 *  |_______|/_/_/_/_/_/_/_|
 */
__s32 Spic_rw(__u32 spi_no, __u32 tcnt, u8* txbuf, __u32 rcnt, u8* rxbuf, __u32 dummy_cnt)
{
	__u32 i = 0,fcr;
//	__u32 tx_dma_flag = 0;
//	__u32 rx_dma_flag = 0;
//	__s32 timeout = 0xffff;

	writel(0, SPI_IER);
	writel(0xffffffff, SPI_ISR);//clear status register

	writel(tcnt, SPI_MTC);
	writel(tcnt+rcnt+dummy_cnt, SPI_MBC);

	//read and write by cpu operation
	if(tcnt)
	{
		i = 0;
		while (i<tcnt)
		{
			//send data
			//while((readw(SPI_FSR)>>16)==SPI_FIFO_SIZE);
			if(((readl(SPI_FSR)>>16) & 0x7f)==SPI_FIFO_SIZE)
				SPINAND_Print("TX FIFO size error!\n");
			writeb(*(txbuf+i),SPI_TXD);
			i++;
		}
	}
	/* start transmit */
	writel(readl(SPI_TCR)|SPI_EXCHANGE, SPI_TCR);
	if(rcnt)
	{
		i = 0;
		#if 0
		timeout = 0xffff;
		while(1)
		{
			if(((readw(SPI_FSR))&0x7f)==rcnt)
				break;
			if(timeout < 0)
			{
				PHY_ERR("RX FIFO size error,timeout!\n");
				break;
			}
			timeout--;
		}
		#endif
		while(i<rcnt)
		{
			//receive valid data
			while(((readl(SPI_FSR))&0x7f)==0);
			//while((((readw(SPI_FSR))&0x7f)!=rcnt)||(timeout < 0))
			//	PHY_ERR("RX FIFO size error!\n");
			*(rxbuf+i)=readb(SPI_RXD);
			i++;
		}
	}

	if(Wait_Tc_Complete())
	{
		SPINAND_Print("wait tc complete timeout!\n");
		return -ERR_TIMEOUT;
	}

    fcr = readl(SPI_FCR);
    fcr &= ~(SPI_TXDMAREQ_EN|SPI_RXDMAREQ_EN);
	writel(fcr, SPI_FCR);
	if (readl(SPI_ISR) & (0xf << 8))	/* (1U << 11) | (1U << 10) | (1U << 9) | (1U << 8)) */
	{
		SPINAND_Print("FIFO status error: 0x%x!\n",readl(SPI_ISR));
		return NAND_OP_FALSE;
	}

	if(readl(SPI_TCR)&SPI_EXCHANGE)
	{
		SPINAND_Print("XCH Control Error!!\n");
	}

	writel(0xffffffff,SPI_ISR);  /* clear  flag */
	return NAND_OP_TRUE;
}
