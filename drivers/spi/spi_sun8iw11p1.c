/*
 * (C) Copyright 2007-2018
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
#include <asm/io.h>
#include <asm/arch/spi.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/dma.h>
#include <asm/arch/clock.h>
#include <sys_config_old.h>

//#define SUNXI_NOR_FLASH_DEBUG 1
#ifdef  SUNXI_NOR_FLASH_DEBUG
#define SUNXI_DEBUG(fmt,args...)	printf(fmt ,##args)
#else
#define SUNXI_DEBUG(fmt,args...) do {} while(0)
#endif

static u32 g_cfg_mclk = 0;


void ccm_module_disable_bak(u32 clk_id)
{
	switch(clk_id>>8) {
		case AHB1_BUS0:
			clr_wbit(CCMU_AHB1_RST_REG0, 0x1U<<(clk_id&0xff));
			SUNXI_DEBUG("\nread CCM_AHB1_RST_REG0[0x%x]\n",readl(CCMU_AHB1_RST_REG0));
			break;
	}
}

void ccm_module_enable_bak(u32 clk_id)
{
	switch(clk_id>>8) {
		case AHB1_BUS0:
			set_wbit(CCMU_AHB1_RST_REG0, 0x1U<<(clk_id&0xff));
			SUNXI_DEBUG("\nread enable CCM_AHB1_RST_REG0[0x%x]\n",readl(CCMU_AHB1_RST_REG0));
			break;
	}
}
void ccm_clock_enable_bak(u32 clk_id)
{
	switch(clk_id>>8) {
		case AXI_BUS:
			set_wbit(CCMU_AXI_GATE_CTRL, 0x1U<<(clk_id&0xff));
			break;
		case AHB1_BUS0:
			set_wbit(CCMU_AHB1_GATE0_CTRL, 0x1U<<(clk_id&0xff));
			SUNXI_DEBUG("read s CCM_AHB1_GATE0_CTRL[0x%x]\n",readl(CCMU_AHB1_GATE0_CTRL));
			break;
	}
}

void ccm_clock_disable_bak(u32 clk_id)
{
	switch(clk_id>>8) {
		case AXI_BUS:
			clr_wbit(CCMU_AXI_GATE_CTRL, 0x1U<<(clk_id&0xff));
			break;
		case AHB1_BUS0:
			clr_wbit(CCMU_AHB1_GATE0_CTRL, 0x1U<<(clk_id&0xff));
			SUNXI_DEBUG("read dis CCM_AHB1_GATE0_CTRL[0x%x]\n",readl(CCMU_AHB1_GATE0_CTRL));
			break;
	}
}

#ifdef USE_DMA
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
static int spi_dma_recv_start(uint spi_no, uchar* pbuf, uint byte_cnt)
{
	flush_cache((uint)pbuf, byte_cnt);

	sunxi_dma_start(spi_rx_dma_hd, SPI_RXD, (uint)pbuf, byte_cnt);

	return 0;
}
static int spi_wait_dma_recv_over(uint spi_no)
{
	//int ret = 0;
	//count_recv ++;
	//ret = sunxi_dma_querystatus(spi_rx_dma_hd);
	//if(ret == 0)
	//	printf("dma recv end \n");
	//return ret;
	return  sunxi_dma_querystatus(spi_rx_dma_hd);
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
static int spi_dma_send_start(uint spi_no, uchar* pbuf, uint byte_cnt)
{
	flush_cache((uint)pbuf, byte_cnt);

	sunxi_dma_start(spi_tx_dma_hd, (uint)pbuf, SPI_TXD, byte_cnt);

	return 0;
}
static int spi_wait_dma_send_over(uint spi_no)
{
	//int ret = 0;
	//count_send++;
	//ret = sunxi_dma_querystatus(spi_tx_dma_hd);
	//if(ret == 0)
	//	printf("dma send end \n");
	//return ret ;
	return sunxi_dma_querystatus(spi_tx_dma_hd);
}
#endif

#define set_wbit(addr, v)   (*((volatile unsigned long  *)(addr)) |=  (unsigned long)(v))
#define clr_wbit(addr, v)   (*((volatile unsigned long  *)(addr)) &= ~(unsigned long)(v))

void ccm_module_reset_bak(u32 clk_id)
{
	ccm_module_disable_bak(clk_id);
	ccm_module_enable_bak(clk_id);
}

void pattern_goto(int pos)
{
	//SUNXI_DEBUG("pos =%d\n",pos);
}

u32 spi_cfg_mclk(u32 spi_no, u32 src, u32 mclk)
{
	u32 mclk_base = CCMU_SPI0_SCLK_CTRL;
	u32 source_clk = 0;
	u32 rval;
	u32 m, n, div;


	switch (src) {
	case 0:
		source_clk = 24000000;
		break;
	case 1:
		source_clk = ccm_get_pll_periph_clk()*1000000;
		break;
	default :
		SUNXI_DEBUG("Wrong SPI clock source :%x\n", src);
	}
	SUNXI_DEBUG("SPI clock source :0x%x\n", source_clk);

	
	if(src == 0)
	{
		rval = (1U << 31);
		writel(rval, mclk_base);
		g_cfg_mclk = source_clk;
		return g_cfg_mclk;
	}

	div = (source_clk + mclk - 1) / mclk;
	div = div==0 ? 1 : div;
	if (div > 128) {
		m = 2;
		n = 0;
		SUNXI_DEBUG("Source clock is too high\n");
	} else if (div > 64) {
		n = 3;
		m = div >> 3;
	} else if (div > 32) {
		n = 2;
		m = div >> 2;
	} else if (div > 16) {
		n = 1;
		m = div >> 1;
	} else {
		n = 0;
		m = div;
	}

	rval = (1U << 31) | (src << 24) | (n << 16) | (m - 1);
	writel(rval, mclk_base);
	g_cfg_mclk = source_clk / (1 << n) / (m - 1);
	SUNXI_DEBUG("spi spic->sclk =0x%x\n",g_cfg_mclk );
	return g_cfg_mclk;
}

u32 spi_get_mlk(u32 spi_no)
{
	return g_cfg_mclk ;//spicinfo[spi_no].sclk;
}

void spi_gpio_cfg(int spi_no)
{
	uint reg_val = 0;
	uint reg_addr;

	// PIO SETTING,PortC0:2 SPI0_MOSI SPI0_MISO SPIO_CLK
	reg_addr = SUNXI_PIO_BASE + 0x48;
	reg_val = readl(reg_addr);
	reg_val &= ~(0xfff);
	reg_val |= 0x333;
	writel(reg_val, reg_addr);

	// PIO SETTING,PortC23 SPI0_CS0
	reg_addr = SUNXI_PIO_BASE + 0x50;
	reg_val = readl(reg_addr);
	reg_val &= ~(0xf<<28);
	reg_val |= 0x3<<28;
	writel(reg_val, reg_addr);
}


void spi_onoff(u32 spi_no, u32 onoff)
{
	u32 clkid[] = {SPI0_CKID, SPI1_CKID};
	//u32 reg_val = 0;
	spi_no = 0;
	switch (spi_no) {
	case 0:
            spi_gpio_cfg(0);	
            break;
	}
	ccm_module_reset_bak(clkid[spi_no]);
	if (onoff)
		ccm_clock_enable_bak(clkid[spi_no]);
	else
		ccm_clock_disable_bak(clkid[spi_no]);

}

void spic_set_clk(u32 spi_no, u32 clk)
{
	u32 mclk = spi_get_mlk(spi_no);
	u32 div;
	u32 cdr1 = 0;
	u32 cdr2 = 0;
	u32 cdr_sel = 0;
	
	div = mclk/(clk<<1);
	
	if (div==0) {
		cdr1 = 0;
	
		cdr2 = 0;
		cdr_sel = 0;
	} else if (div<=0x100) {
		cdr1 = 0;
	
		cdr2 = div-1;
		cdr_sel = 1;
	} else {
		div = 0;
		while (mclk > clk) {
		    div++;
		    mclk >>= 1;
		}
		cdr1 = div;
		cdr2 = 0;
		cdr_sel = 0;
	}
	writel((cdr_sel<<12)|(cdr1<<8)|cdr2, SPI_CCR);
	SUNXI_DEBUG("spic_set_clk:mclk=0x%x\n",mclk);
}
