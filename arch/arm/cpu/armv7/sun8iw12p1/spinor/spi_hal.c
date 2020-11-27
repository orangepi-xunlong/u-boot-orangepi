/*
 * spi_hal.c
 *
 * Copyright (C) 2017-2020 Allwinnertech Co., Ltd
 *
 * Author        : zhouhuacai

 * Description   : sunxi spi hal layer.
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
#include <asm/arch/gpio.h>
#include <private_boot0.h>
#include <asm/arch/spi.h>
#include <asm/io.h>
#include <asm/arch/ccmu.h>

static u32 g_cfg_mclk = 0;

#if 0
#define SUNXI_DEBUG(fmt,args...)	printf(fmt ,##args)
#else
#define SUNXI_DEBUG(fmt,args...) do {} while(0)
#endif

void spic_config_dual_mode(u32 spi_no, u32 rxdual, u32 dbc, u32 stc)
{
	u32 reg_val;

	reg_val = readl(SPI_BCC);
	reg_val &= ~(0x1U << 29);
	reg_val &= ~(0x1U << 28);
	reg_val &= ~(0xF << 24);
	reg_val &= ~(0xFFFFFF);
	reg_val |=  (rxdual << 28 | dbc << 24 | stc);

	writel(reg_val, SPI_BCC);
}

static inline void ccm_module_disable(u32 clk_id)
{
	clr_wbit(CCMU_SPI_BGR_REG, 0x1U << (SPI_RST_OFFSET + clk_id));
}

static inline void ccm_module_enable(u32 clk_id)
{
	set_wbit(CCMU_SPI_BGR_REG, 0x1U << (SPI_RST_OFFSET + clk_id));
}

static inline void ccm_clock_enable(u32 clk_id)
{
	set_wbit(CCMU_SPI_BGR_REG, 0x1U << (SPI_GATING_OFFSET + clk_id));
}

static inline void ccm_clock_disable(u32 clk_id)
{
	clr_wbit(CCMU_SPI_BGR_REG, 0x1U << (SPI_GATING_OFFSET + clk_id));
}

static inline void ccm_module_reset(u32 clk_id)
{
	ccm_module_disable(clk_id);
	ccm_module_enable(clk_id);
}

u32 ccm_get_pll_periph_clk(void)
{
	u32 rval = readl(CCMU_PLL_PERI0_CTRL_REG);
	u32 n = (0xff & (rval >> 8)) + 1;
	u32 m0 = (0x1 & (rval >> 0)) + 1;
	u32 m1 = (0x1 & (rval >> 1)) + 1;

	return 24000000 * n / (m0*m1);
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
		source_clk = ccm_get_pll_periph_clk() * 1000000;
		break;
	default :
		SUNXI_DEBUG("Wrong SPI clock source :%x\n", src);
	}
	SUNXI_DEBUG("SPI clock source :%d\n", source_clk);

	if (src == 0) {
		rval = (1U << 31);
		writel(rval, mclk_base);
		g_cfg_mclk = source_clk;
		return g_cfg_mclk;
	}

	div = (source_clk + mclk - 1) / mclk;
	div = div == 0 ? 1 : div;
	if (div > 128) {
		m = 1;
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
	SUNXI_DEBUG("spi spic->sclk =0x%x\n", g_cfg_mclk );
	return g_cfg_mclk;
}

u32 spi_get_mlk(u32 spi_no)
{
	return g_cfg_mclk ;
}

void spi_gpio_cfg(int spi_no)
{
	uint reg_val = 0;
	uint reg_addr;

	/* PIO SETTING,SPI0_MOSI(PC2) SPI0_MISO(PC3) SPIO_CLK(PC0)  */
	reg_addr = SUNXI_PIO_BASE + 0x48;
	reg_val = readl(reg_addr);
	reg_val &= ~(0xffff);
	reg_val |= ((4 << 8) | (4 << 12) | (4 << 0));
	writel(reg_val, reg_addr);

	/* PIO SETTING,SPI0_CS0(PC15)  */
	reg_addr = SUNXI_PIO_BASE + 0x4C;
	reg_val = readl(reg_addr);
	reg_val &= ~(0xf << 28);
	reg_val |= 0x4 << 28;
	writel(reg_val, reg_addr);
}

void spi_onoff(u32 spi_no, u32 onoff)
{
	u32 clkid[] = {SPI0_CKID, SPI1_CKID, SPI2_CKID, SPI3_CKID};

	spi_no = 0;
	switch (spi_no) {
	case 0:
		spi_gpio_cfg(0);
		break;
	}
	ccm_module_reset(clkid[spi_no]);
	if (onoff)
		ccm_clock_enable(clkid[spi_no]);
	else
		ccm_clock_disable(clkid[spi_no]);
}

void spic_set_clk(u32 spi_no, u32 clk)
{
	u32 mclk = spi_get_mlk(spi_no);
	u32 div;
	u32 cdr1 = 0;
	u32 cdr2 = 0;
	u32 cdr_sel = 0;

	div = mclk / (clk << 1);

	if (div == 0) {
		cdr1 = 0;

		cdr2 = 0;
		cdr_sel = 0;
	} else if (div <= 0x100) {
		cdr1 = 0;

		cdr2 = div - 1;
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

	writel((cdr_sel << 12) | (cdr1 << 8) | cdr2, SPI_CCR);
	SUNXI_DEBUG("spic_set_clk:mclk=%d\n", mclk);
}

int spic_init(u32 spi_no)
{
	u32 rval;

	spi_no = 0;
	spi_onoff(spi_no, 1);
#ifdef FPGA_PLATFORM
	/*24M  */
	spi_cfg_mclk(spi_no, 0, SPI_MCLK);
#else
	/*pll6  */
	spi_cfg_mclk(spi_no, 1, SPI_MCLK);
#endif
	spic_set_clk(spi_no, SPI_DEFAULT_CLK);

	rval = SPI_SOFT_RST | SPI_TXPAUSE_EN | SPI_MASTER | SPI_ENABLE;
	writel(rval, SPI_GCR);
	rval = SPI_SET_SS_1 | SPI_DHB | SPI_SS_ACTIVE0;
	writel(rval, SPI_TCR);
	writel(SPI_TXFIFO_RST | (SPI_TX_WL << 16) | (SPI_RX_WL), SPI_FCR);

	return 0;
}

int spic_rw( u32 tcnt, void *txbuf, u32 rcnt, void *rxbuf)
{
	u32 i = 0, fcr;
	int timeout = 0xfffff;

	u8 *tx_buffer = txbuf ;
	u8 *rx_buffer = rxbuf;
	writel(0, SPI_IER);
	writel(0xffffffff, SPI_ISR);

	writel(tcnt, SPI_MTC);
	writel(tcnt + rcnt, SPI_MBC);
	writel(readl(SPI_TCR) | SPI_EXCHANGE, SPI_TCR);
	if (tcnt) {
		i = 0;
		while (i < tcnt) {
			/*send data  */
			while (((readl(SPI_FSR) >> 16) == SPI_FIFO_SIZE) );
			writeb(*(tx_buffer + i), SPI_TXD);
			i++;
		}
	}

	timeout = 0xfffff;
	/* start transmit */
	if (rcnt) {
		i = 0;
		while (i < rcnt) {
			/*receive valid data  */
			while (((readl(SPI_FSR)) & 0x7f) == 0);
			*(rx_buffer + i) = readb(SPI_RXD);
			i++;
		}
	}

	fcr = readl(SPI_FCR);
	fcr &= ~(SPI_TXDMAREQ_EN | SPI_RXDMAREQ_EN);
	writel(fcr, SPI_FCR);
	if ((readl(SPI_ISR) & (0xf << 8)) ||
	    (timeout == 0))	/* (1U << 11) | (1U << 10) | (1U << 9) | (1U << 8)) */
		return RET_FAIL;
	if (!readl(SPI_MBC)) {
		if (readl(SPI_TCR)&SPI_EXCHANGE)
			printf("XCH Control Error!!\n");
	}

	writel(0xfffff, SPI_ISR); /* clear  flag */
	return RET_OK;
}

int spic_exit(u32 spi_no)
{
	return 0;
}

