// SPDX-License-Identifier: GPL-2.0+
/*
 * sunxi SPI driver for uboot.
 *
 * Copyright (C) 2018
 * 2013.5.7  Mintow <duanmintao@allwinnertech.com>
 * 2018.11.7 wangwei <wangwei@allwinnertech.com>
 */

#include <common.h>
#include <dm.h>
#include <malloc.h>
#include <memalign.h>
#include <spi.h>
#include <linux/errno.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/sys_proto.h>
#ifdef CONFIG_SUNXI_DMA
#include <asm/arch/dma.h>
#endif
#include <asm/arch/gpio.h>
#include <sunxi_board.h>
#include "spi-sunxi.h"
#include <sys_config.h>
#include <fdt_support.h>
#include <linux/mtd/spi-nor.h>
#include <private_boot0.h>
#include <private_toc.h>
#include "../mtd/spi/sf_internal.h"
#include <linux/sizes.h>
#include <boot_param.h>

#ifdef CONFIG_SPI_USE_DMA
static sunxi_dma_set *spi_tx_dma;
static sunxi_dma_set *spi_rx_dma;
static uint spi_tx_dma_hd;
static uint spi_rx_dma_hd;
#endif

#define	SUNXI_SPI_MAX_TIMEOUT	1000000
#define	SUNXI_SPI_PORT_OFFSET	0x1000
#define SUNXI_SPI_DEFAULT_CLK  (40000000)

/* For debug */
#define SPI_DEBUG 0

#if SPI_DEBUG
#define SPI_EXIT()		printf("%s()%d - %s\n", __func__, __LINE__, "Exit")
#define SPI_ENTER()		printf("%s()%d - %s\n", __func__, __LINE__, "Enter ...")
#define SPI_DBG(fmt, arg...)	printf("%s()%d - "fmt, __func__, __LINE__, ##arg)
#define SPI_INF(fmt, arg...)	printf("%s()%d - "fmt, __func__, __LINE__, ##arg)
#define SPI_ERR(fmt, arg...)	printf("%s()%d - "fmt, __func__, __LINE__, ##arg)

#else
#define SPI_EXIT()		pr_debug("%s()%d - %s\n", __func__, __LINE__, "Exit")
#define SPI_ENTER()		pr_debug("%s()%d - %s\n", __func__, __LINE__, "Enter ...")
#define SPI_DBG(fmt, arg...)	pr_debug("%s()%d - "fmt, __func__, __LINE__, ##arg)
#define SPI_INF(fmt, arg...)	pr_debug("%s()%d - "fmt, __func__, __LINE__, ##arg)
#define SPI_ERR(fmt, arg...)	printf("%s()%d - "fmt, __func__, __LINE__, ##arg)
#endif
#define SUNXI_SPI_OK   0
#define SUNXI_SPI_FAIL -1

static int sunxi_get_spic_clk(int bus);
struct sunxi_spi_slave *g_sspi;

struct sunxi_spi_slave *get_sspi(void)
{
	return g_sspi;
}

#if SPI_DEBUG
static void spi_print_info(struct sunxi_spi_slave *sspi)
{
	void __iomem *base_addr = (void __iomem *)(unsigned long)sspi->base_addr;

	char buf[1024] = {0};
	snprintf(buf, sizeof(buf)-1,
			"sspi->base_addr = 0x%x, the SPI control register:\n"
			"[VER] 0x%02x = 0x%08x, [GCR] 0x%02x = 0x%08x, [TCR] 0x%02x = 0x%08x\n"
			"[ICR] 0x%02x = 0x%08x, [ISR] 0x%02x = 0x%08x, [FCR] 0x%02x = 0x%08x\n"
			"[FSR] 0x%02x = 0x%08x, [WCR] 0x%02x = 0x%08x, [CCR] 0x%02x = 0x%08x\n"
			"[DCR] 0x%02x = 0x%08x, [BCR] 0x%02x = 0x%08x, [TCR] 0x%02x = 0x%08x\n"
			"[BCC] 0x%02x = 0x%08x, [DMA] 0x%02x = 0x%08x",
			sspi->base_addr,
			SPI_VER_REG, readl(base_addr + SPI_VER_REG),
			SPI_GC_REG, readl(base_addr + SPI_GC_REG),
			SPI_TC_REG, readl(base_addr + SPI_TC_REG),
			SPI_INT_CTL_REG, readl(base_addr + SPI_INT_CTL_REG),
			SPI_INT_STA_REG, readl(base_addr + SPI_INT_STA_REG),

			SPI_FIFO_CTL_REG, readl(base_addr + SPI_FIFO_CTL_REG),
			SPI_FIFO_STA_REG, readl(base_addr + SPI_FIFO_STA_REG),
			SPI_WAIT_CNT_REG, readl(base_addr + SPI_WAIT_CNT_REG),
			SPI_CLK_CTL_REG, readl(base_addr + SPI_CLK_CTL_REG),
			SPI_SDC_REG, readl(base_addr + SPI_SDC_REG),

			SPI_BURST_CNT_REG, readl(base_addr + SPI_BURST_CNT_REG),
			SPI_TRANSMIT_CNT_REG, readl(base_addr + SPI_TRANSMIT_CNT_REG),
			SPI_BCC_REG, readl(base_addr + SPI_BCC_REG),
			SPI_DMA_CTL_REG, readl(base_addr + SPI_DMA_CTL_REG));
			printf("%s\n", buf);
}
#endif

static inline struct sunxi_spi_slave *to_sunxi_slave(struct spi_slave *slave)
{
	return container_of(slave, struct sunxi_spi_slave, slave);
}

static s32 sunxi_spi_gpio_request(void)
{
	int ret = 0;

	ret = fdt_set_all_pin("spi0", "pinctrl-0");
	if (ret <= 0) {
		SPI_INF("set pin of spi0 fail %d\n", ret);
		return -1;
	}

	return 0;
}

static s32 sunxi_get_spi_mode(void)
{
	int nodeoffset = 0;
	int ret = 0;
	u32 rval = 0;
	u32 mode = 0;

	nodeoffset =  fdt_path_offset(working_fdt, "spi0/spi_board0");
	if (nodeoffset < 0) {
		SPI_INF("get spi0 para fail\n");
		return -1;
	}

	ret = fdt_getprop_u32(working_fdt, nodeoffset, "spi-rx-bus-width", (uint32_t *)(&rval));
	if (ret < 0) {
		SPI_INF("get spi-rx-bus-width fail %d\n", ret);
		return -2;
	}

	if (rval == 1) {
		mode |= SPI_RX_SLOW;
	} else if (rval == 2) {
		mode |= SPI_RX_DUAL;
	} else if (rval == 4) {
		mode |= SPI_RX_QUAD;
	}

#if 0
	ret = fdt_getprop_u32(working_fdt, nodeoffset, "spi-tx-bus-width", (uint32_t *)(&rval));
	if (ret < 0) {
		SPI_INF("get spi-tx-bus-width fail %d\n", ret);
		return -3;
	}
	if (rval == 1) {
		mode |= SPI_TX_BYTE;
	} else if (rval == 2) {
		mode |= SPI_TX_DUAL;
	} else if (rval == 4) {
		mode |= SPI_TX_QUAD;
	}
#endif
	SPI_INF("get spi-bus-width 0x%x\n", mode);

	return mode;
}

/* config chip select */
static s32 spi_set_cs(u32 chipselect, void __iomem *base_addr)
{
	int ret;
	u32 reg_val = readl(base_addr + SPI_TC_REG);

	if (chipselect < 4) {
		reg_val &= ~SPI_TC_SS_MASK;/* SS-chip select, clear two bits */
		reg_val |= chipselect << SPI_TC_SS_BIT_POS;/* set chip select */
		writel(reg_val, base_addr + SPI_TC_REG);
		ret = SUNXI_SPI_OK;
	} else {
		SPI_ERR("Chip Select set fail! cs = %d\n", chipselect);
		ret = SUNXI_SPI_FAIL;
	}

	return ret;
}

static void spi_set_sdm(void __iomem *base_addr, unsigned int smod)
{
	unsigned int rval = readl(base_addr + SPI_TC_REG) & (~SPI_TC_SDM);

	if (smod)
		rval |= SPI_TC_SDM;
	writel(rval, base_addr + SPI_TC_REG);
}

static void spi_set_sdc(void __iomem *base_addr, unsigned int sample)
{
	unsigned int rval = readl(base_addr + SPI_TC_REG) & (~SPI_TC_SDC);

	if (sample)
		rval |= SPI_TC_SDC;
	writel(rval, base_addr + SPI_TC_REG);
}

static void spi_set_sdc1(void __iomem *base_addr, unsigned int sample)
{
	unsigned int rval = readl(base_addr + SPI_TC_REG) & (~SPI_TC_SDC1);

	if (sample)
		rval |= SPI_TC_SDC1;
	writel(rval, base_addr + SPI_TC_REG);
}

static void spi_set_sample_mode(void __iomem *base_addr, unsigned int mode)
{
	unsigned int sample_mode[7] = {
		DELAY_NORMAL_SAMPLE, DELAY_0_5_CYCLE_SAMPLE,
		DELAY_1_CYCLE_SAMPLE, DELAY_1_5_CYCLE_SAMPLE,
		DELAY_2_CYCLE_SAMPLE, DELAY_2_5_CYCLE_SAMPLE,
		DELAY_3_CYCLE_SAMPLE
	};
	spi_set_sdm(base_addr, (sample_mode[mode] >> 8) & 0xf);
	spi_set_sdc(base_addr, (sample_mode[mode] >> 4) & 0xf);
	spi_set_sdc1(base_addr, sample_mode[mode] & 0xf);
}

static void spi_set_sample_delay(void __iomem  *base_addr,
		unsigned int sample_delay)
{
	unsigned int rval = readl(base_addr + SPI_SDC_REG)&(~(0x3f << 0));

	rval |= sample_delay;
	writel(rval, base_addr + SPI_SDC_REG);
	mdelay(1);
}

/* config spi */
static void spi_config_tc(struct sunxi_spi_slave *sspi, u32 master,
		u32 config, void __iomem *base_addr)
{
	u32 reg_val = readl(base_addr + SPI_TC_REG);
	uint sclk_freq = 0;

	sclk_freq = sunxi_get_spic_clk(0);

	if (sspi->right_sample_delay == SAMP_MODE_DL_DEFAULT) {
		if (sclk_freq >= 60000000)
			reg_val |= SPI_TC_SDC;
		else if (sclk_freq <= 24000000)
			reg_val |= SPI_TC_SDM;
		else
			reg_val &= ~(SPI_TC_SDM | SPI_TC_SDC);
	} else {
		spi_set_sample_mode(base_addr, sspi->right_sample_mode);
		spi_set_sample_delay(base_addr, sspi->right_sample_delay);
		reg_val = readl(base_addr + SPI_TC_REG);
	}

	reg_val |= SPI_TC_DHB | SPI_TC_SS_LEVEL | SPI_TC_SPOL;
	writel(reg_val, base_addr + SPI_TC_REG);

#if 0
	/*1. POL */
	if (config & SPI_POL_ACTIVE_)
		reg_val |= SPI_TC_POL;/*default POL = 1 */
	else
		reg_val &= ~SPI_TC_POL;

	/*2. PHA */
	if (config & SPI_PHA_ACTIVE_)
		reg_val |= SPI_TC_PHA;/*default PHA = 1 */
	else
		reg_val &= ~SPI_TC_PHA;

	/*3. SSPOL,chip select signal polarity */
	if (config & SPI_CS_HIGH_ACTIVE_)
		reg_val &= ~SPI_TC_SPOL;
	else
		reg_val |= SPI_TC_SPOL; /*default SSPOL = 1,Low level effect */

	/*4. LMTF--LSB/MSB transfer first select */
	if (config & SPI_LSB_FIRST_ACTIVE_)
		reg_val |= SPI_TC_FBS;
	else
		reg_val &= ~SPI_TC_FBS;/*default LMTF =0, MSB first */

	/*master mode: set DDB,DHB,SMC,SSCTL*/
	if (master == 1) {
		/*5. dummy burst type */
		if (config & SPI_DUMMY_ONE_ACTIVE_)
			reg_val |= SPI_TC_DDB;
		else
			reg_val &= ~SPI_TC_DDB;/*default DDB =0, ZERO */

		/*6.discard hash burst-DHB */
		if (config & SPI_RECEIVE_ALL_ACTIVE_)
			reg_val &= ~SPI_TC_DHB;
		else
			reg_val |= SPI_TC_DHB;/*default DHB =1, discard unused burst */

		/*7. set SMC = 1 , SSCTL = 0 ,TPE = 1 */
		reg_val &= ~SPI_TC_SSCTL;
	} else {
		/* tips for slave mode config */
		SPI_INF("slave mode configurate control register.\n");
	}
#endif

}

/* set spi clock
 * cdr1: spi_sclk =source_clk/(2^cdr1_M)
 * cdr2: spi_sclk =source_clk/(2*(cdr2_N+1))
 * spi_clk : the spi final clk.
 * ahb_clk: source_clk - the clk from  ccmu config, spic clk src is osc24M or peri0_1X.
*/

static void spi_set_clk(u32 spi_clk, u32 ahb_clk, void __iomem *base_addr, u32 cdr)
{
	u32 reg_val = 0;
	u32 div_clk = 0;

	SPI_DBG("set spi clock %d, mclk %d\n", spi_clk, ahb_clk);
	reg_val = readl(base_addr + SPI_CLK_CTL_REG);

	/* CDR2 */
	if (cdr) {
		div_clk = ahb_clk / (spi_clk * 2) - 1;
		reg_val &= ~SPI_CLK_CTL_CDR2;
		reg_val |= (div_clk | SPI_CLK_CTL_DRS);
		SPI_DBG("CDR2 - n = %d\n", div_clk);
	} else { /* CDR1 */
		while (ahb_clk > spi_clk) {
			div_clk++;
			ahb_clk >>= 1;
		}
		reg_val &= ~(SPI_CLK_CTL_CDR1 | SPI_CLK_CTL_DRS);
		reg_val |= (div_clk << 8);
		SPI_DBG("CDR1 - n = %d\n", div_clk);
	}

	writel(reg_val, base_addr + SPI_CLK_CTL_REG);
}

/* start spi transfer */
static void spi_start_xfer(void __iomem *base_addr)
{
	u32 reg_val = readl(base_addr + SPI_TC_REG);

	reg_val |= SPI_TC_XCH;
	writel(reg_val, base_addr + SPI_TC_REG);
}

/* enable spi bus */
static void spi_enable_bus(void __iomem *base_addr)
{
	u32 reg_val = readl(base_addr + SPI_GC_REG);

	reg_val |= SPI_GC_EN;
	writel(reg_val, base_addr + SPI_GC_REG);
}

/* disbale spi bus */
static void spi_disable_bus(void __iomem *base_addr)
{
	u32 reg_val = readl(base_addr + SPI_GC_REG);

	reg_val &= ~SPI_GC_EN;
	writel(reg_val, base_addr + SPI_GC_REG);
}

/* set master mode */
static void spi_set_master(void __iomem *base_addr)
{
	u32 reg_val = readl(base_addr + SPI_GC_REG);

	reg_val |= SPI_GC_MODE;
	writel(reg_val, base_addr + SPI_GC_REG);
}

/* enable transmit pause */
static void spi_enable_tp(void __iomem *base_addr)
{
	u32 reg_val = readl(base_addr + SPI_GC_REG);

	reg_val |= SPI_GC_TP_EN;
	writel(reg_val, base_addr + SPI_GC_REG);
}

/* soft reset spi controller */
static void spi_soft_reset(void __iomem *base_addr)
{
	u32 reg_val = readl(base_addr + SPI_GC_REG);

	reg_val |= SPI_GC_SRST;
	writel(reg_val, base_addr + SPI_GC_REG);
}

#if 0
/* enable irq type */
static void spi_enable_irq(u32 bitmap, void __iomem *base_addr)
{
	u32 reg_val = readl(base_addr + SPI_INT_CTL_REG);

	bitmap &= SPI_INTEN_MASK;
	reg_val |= bitmap;
	writel(reg_val, base_addr + SPI_INT_CTL_REG);
}
#endif

/* disable irq type */
static void spi_disable_irq(u32 bitmap, void __iomem *base_addr)
{
	u32 reg_val = readl(base_addr + SPI_INT_CTL_REG);

	bitmap &= SPI_INTEN_MASK;
	reg_val &= ~bitmap;
	writel(reg_val, base_addr + SPI_INT_CTL_REG);
}

/* query irq pending */
static u32 spi_qry_irq_pending(void __iomem *base_addr)
{
	return (SPI_INT_STA_MASK & readl(base_addr + SPI_INT_STA_REG));
}

/* clear irq pending */
static void spi_clr_irq_pending(u32 pending_bit, void __iomem *base_addr)
{
	pending_bit &= SPI_INT_STA_MASK;
	writel(pending_bit, base_addr + SPI_INT_STA_REG);
}

/* query txfifo bytes */
static u32 spi_query_txfifo(void __iomem *base_addr)
{
	u32 reg_val = (SPI_FIFO_STA_TX_CNT & readl(base_addr + SPI_FIFO_STA_REG));

	reg_val >>= SPI_TXCNT_BIT_POS;
	return reg_val;
}

/* query rxfifo bytes */
static u32 spi_query_rxfifo(void __iomem *base_addr)
{
	u32 reg_val = (SPI_FIFO_STA_RX_CNT & readl(base_addr + SPI_FIFO_STA_REG));

	reg_val >>= SPI_RXCNT_BIT_POS;
	return reg_val;
}

/* reset fifo */
static void spi_reset_fifo(void __iomem *base_addr)
{
	u32 reg_val = readl(base_addr + SPI_FIFO_CTL_REG);

	reg_val |= (SPI_FIFO_CTL_RX_RST|SPI_FIFO_CTL_TX_RST);
	/* Set the trigger level of RxFIFO/TxFIFO. */
	reg_val &= ~(SPI_FIFO_CTL_RX_LEVEL|SPI_FIFO_CTL_TX_LEVEL);
	reg_val |= (0x20<<16) | 0x20;
	writel(reg_val, base_addr + SPI_FIFO_CTL_REG);
}

/* set transfer total length BC, transfer length TC and single transmit length STC */
static void spi_set_bc_tc_stc(u32 tx_len, u32 rx_len, u32 stc_len, u32 dummy_cnt, void __iomem *base_addr)
{
	u32 reg_val = readl(base_addr + SPI_BURST_CNT_REG);

	reg_val &= ~SPI_BC_CNT_MASK;
	reg_val |= (SPI_BC_CNT_MASK & (tx_len + rx_len + dummy_cnt));
	writel(reg_val, base_addr + SPI_BURST_CNT_REG);

	reg_val = readl(base_addr + SPI_TRANSMIT_CNT_REG);
	reg_val &= ~SPI_TC_CNT_MASK;
	reg_val |= (SPI_TC_CNT_MASK & tx_len);
	writel(reg_val, base_addr + SPI_TRANSMIT_CNT_REG);

	reg_val = readl(base_addr + SPI_BCC_REG);
	reg_val &= ~SPI_BCC_STC_MASK;
	reg_val |= (SPI_BCC_STC_MASK & stc_len);
	reg_val &= ~(0xf << 24);
	reg_val |= (dummy_cnt << 24);
	writel(reg_val, base_addr + SPI_BCC_REG);
}

/* set ss control */
static void spi_ss_owner(void __iomem *base_addr, u32 on_off)
{
	u32 reg_val = readl(base_addr + SPI_TC_REG);

	on_off &= 0x1;
	if (on_off)
		reg_val |= SPI_TC_SS_OWNER;
	else
		reg_val &= ~SPI_TC_SS_OWNER;
	writel(reg_val, base_addr + SPI_TC_REG);
}

/* set ss level */
static void spi_ss_level(void __iomem *base_addr, u32 hi_lo)
{
	u32 reg_val = readl(base_addr + SPI_TC_REG);

	hi_lo &= 0x1;
	if (hi_lo)
		reg_val |= SPI_TC_SS_LEVEL;
	else
		reg_val &= ~SPI_TC_SS_LEVEL;
	writel(reg_val, base_addr + SPI_TC_REG);
}


#if 1
static void spi_disable_dual(void __iomem  *base_addr)
{
	u32 reg_val = readl(base_addr+SPI_BCC_REG);
	reg_val &= ~SPI_BCC_DUAL_MODE;
	writel(reg_val, base_addr + SPI_BCC_REG);
}

static void spi_enable_dual(void __iomem  *base_addr)
{
	u32 reg_val = readl(base_addr+SPI_BCC_REG);
	reg_val &= ~SPI_BCC_QUAD_MODE;
	reg_val |= SPI_BCC_DUAL_MODE;
	writel(reg_val, base_addr + SPI_BCC_REG);
}

static void spi_disable_quad(void __iomem  *base_addr)
{
	u32 reg_val = readl(base_addr+SPI_BCC_REG);

	reg_val &= ~SPI_BCC_QUAD_MODE;
	writel(reg_val, base_addr + SPI_BCC_REG);
}

static void spi_enable_quad(void __iomem  *base_addr)
{
	u32 reg_val = readl(base_addr+SPI_BCC_REG);

	reg_val |= SPI_BCC_QUAD_MODE;
	writel(reg_val, base_addr + SPI_BCC_REG);
}

void spi_samp_dl_sw_status(void __iomem  *base_addr, unsigned int status)
{
	unsigned int rval = readl(base_addr + SPI_SDC_REG);

	if (status)
		rval |= SPI_SAMP_DL_SW_EN;
	else
		rval &= ~SPI_SAMP_DL_SW_EN;

	writel(rval, base_addr +  SPI_SDC_REG);
}

static void spi_samp_mode(void __iomem  *base_addr, unsigned int status)
{
	unsigned int rval = readl(base_addr + SPI_GC_REG);

	if (status)
		rval |= SPI_SAMP_MODE_EN;
	else
		rval &= ~SPI_SAMP_MODE_EN;

	writel(rval, base_addr + SPI_GC_REG);
}

static int sunxi_spi_mode_check(void __iomem  *base_addr, u32 tcnt, u32 rcnt, u8 cmd)
{
	/* single mode transmit counter*/
	unsigned int stc = 0;

	if (SPINOR_OP_READ_1_1_4 == cmd || SPINOR_OP_READ_1_1_4_4B == cmd ||
		SPINOR_OP_PP_1_1_4 == cmd || SPINOR_OP_PP_1_1_4_4B == cmd) {
		/*tcnt is cmd len, use single mode to transmit*/
		/*rcnt is  the len of recv data, use quad mode to transmit*/
		stc = tcnt;
		spi_enable_quad(base_addr);
		spi_set_bc_tc_stc(tcnt, rcnt, stc, 0, base_addr);
	} else if (SPINOR_OP_READ_1_1_2 == cmd || SPINOR_OP_READ_1_1_2_4B == cmd) {
		/*tcnt is cmd len, use single mode to transmit*/
		/*rcnt is  the len of recv data, use dual mode to transmit*/
		stc = tcnt;
		spi_enable_dual(base_addr);
		spi_set_bc_tc_stc(tcnt, rcnt, stc, 0, base_addr);
	} else {
		/*tcnt is the len of cmd, rcnt is the len of recv data. use single mode to transmit*/
		stc = tcnt+rcnt;
		spi_disable_dual(base_addr);
		spi_disable_quad(base_addr);
		spi_set_bc_tc_stc(tcnt, rcnt, stc, 0, base_addr);
	}

	return 0;
}
#endif


/* check the valid of cs id */
static int sunxi_spi_check_cs(unsigned int bus, unsigned int cs)
{
	int ret = SUNXI_SPI_FAIL;

	if((bus == 0) && (cs == 0))
		return SUNXI_SPI_OK;

	return ret;
}

static int sunxi_spi_gpio_init(void)
{
#if 0
#if defined(CONFIG_MACH_SUN50IW3)
	sunxi_gpio_set_cfgpin(SUNXI_GPC(0), SUN50I_GPC_SPI0); /*spi0_sclk */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(5), SUN50I_GPC_SPI0); /*spi0_cs0*/
	sunxi_gpio_set_cfgpin(SUNXI_GPC(2), SUN50I_GPC_SPI0); /*spi0_mosi*/
	sunxi_gpio_set_cfgpin(SUNXI_GPC(3), SUN50I_GPC_SPI0); /*spi0_miso*/
	sunxi_gpio_set_pull(SUNXI_GPC(5), 1);
#elif defined(CONFIG_MACH_SUN8IW16)
	sunxi_gpio_set_cfgpin(SUNXI_GPC(0), SUN50I_GPC_SPI0); /*spi0_sclk */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(1), SUN50I_GPC_SPI0); /*spi0_cs0*/

	sunxi_gpio_set_cfgpin(SUNXI_GPC(2), SUN50I_GPC_SPI0); /*spi0_mosi*/
	sunxi_gpio_set_cfgpin(SUNXI_GPC(3), SUN50I_GPC_SPI0); /*spi0_miso*/
	sunxi_gpio_set_cfgpin(SUNXI_GPC(10), SUN50I_GPC_SPI0); /*spi0_cs1*/
	sunxi_gpio_set_cfgpin(SUNXI_GPC(13), SUN50I_GPC_SPI0); /*spi0_wp*/
	sunxi_gpio_set_cfgpin(SUNXI_GPC(14), SUN50I_GPC_SPI0); /*spi0_hold*/

	sunxi_gpio_set_pull(SUNXI_GPC(1), 1);
	sunxi_gpio_set_pull(SUNXI_GPC(10), 1);
#elif defined(CONFIG_MACH_SUN8IW11)
	sunxi_gpio_set_cfgpin(SUNXI_GPC(2), SUNXI_GPC_SPI0); /*spi0_sclk */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(23), SUNXI_GPC_SPI0); /*spi0_cs0*/

	sunxi_gpio_set_cfgpin(SUNXI_GPC(0), SUNXI_GPC_SPI0); /*spi0_mosi*/
	sunxi_gpio_set_cfgpin(SUNXI_GPC(1), SUNXI_GPC_SPI0); /*spi0_miso*/

	sunxi_gpio_set_pull(SUNXI_GPC(23), 1);
#elif  defined(CONFIG_MACH_SUN8IW18)
	sunxi_gpio_set_cfgpin(SUNXI_GPC(0), SUN50I_GPC_SPI0); /*spi0_sclk */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(3), SUN50I_GPC_SPI0); /*spi0_cs0*/

	sunxi_gpio_set_cfgpin(SUNXI_GPC(2), SUN50I_GPC_SPI0); /*spi0_mosi*/
	sunxi_gpio_set_cfgpin(SUNXI_GPC(4), SUN50I_GPC_SPI0); /*spi0_miso*/
	sunxi_gpio_set_cfgpin(SUNXI_GPC(15), SUN50I_GPC_SPI0); /*spi0_wp*/
	sunxi_gpio_set_cfgpin(SUNXI_GPC(16), SUN50I_GPC_SPI0); /*spi0_hold*/

	sunxi_gpio_set_pull(SUNXI_GPC(3), 1);
#elif defined(CONFIG_MACH_SUN8IW19)
	sunxi_gpio_set_cfgpin(SUNXI_GPC(0), SUN50I_GPC_SPI0); /*spi0_sclk */
	sunxi_gpio_set_cfgpin(SUNXI_GPC(1), SUN50I_GPC_SPI0); /*spi0_cs0*/

	sunxi_gpio_set_cfgpin(SUNXI_GPC(2), SUN50I_GPC_SPI0); /*spi0_mosi*/
	sunxi_gpio_set_cfgpin(SUNXI_GPC(3), SUN50I_GPC_SPI0); /*spi0_miso*/
	sunxi_gpio_set_cfgpin(SUNXI_GPC(4), SUN50I_GPC_SPI0); /*spi0_wp*/
	sunxi_gpio_set_cfgpin(SUNXI_GPC(5), SUN50I_GPC_SPI0); /*spi0_hold*/

	sunxi_gpio_set_pull(SUNXI_GPC(1), 1);
#else
	sunxi_spi_gpio_request();
#endif
#else
	sunxi_spi_gpio_request();
#endif
	return 0;
}

static int sunxi_spi_clk_init(u32 bus, u32 mod_clk)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	unsigned long mclk_base = (unsigned long)&ccm->spi0_clk_cfg + bus*0x4;
	u32 source_clk = 0;
	u32 rval;
	u32 m, n, div, factor_m;

	/* SCLK = src/M/N */
	/* N: 00:1 01:2 10:4 11:8 */
    /* M: factor_m + 1 */
#ifdef FPGA_PLATFORM
	n = 0;
	m = 1;
	factor_m = m - 1;
	rval = (1U << 31);
	source_clk = 24000000;
#else
#if defined(CONFIG_MACH_SUN8IW21)
	source_clk = 300000000;
#else
	source_clk = clock_get_pll6() * 1000000;
#endif
	SPI_DBG("source_clk: %d Hz, mod_clk: %d Hz\n", source_clk, mod_clk);

	div = (source_clk + mod_clk - 1) / mod_clk;
	div = div == 0 ? 1 : div;
	if (div > 128) {
		m = 1;
		n = 0;
		return -1;
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

	factor_m = m - 1;
#if defined(CONFIG_MACH_SUN8IW11)
	rval = (1U << 31) | (0x1 << 24) | (n << 16) | factor_m;
#else
	rval = (1U << 31) | (0x1 << 24) | (n << 8) | factor_m;
#endif

#endif
	writel(rval, (volatile void __iomem *)mclk_base);

#if defined(CONFIG_MACH_SUN8IW11)
	/* spi reset */
	writel(readl(&ccm->ahb_reset0_cfg) & ~(1 << 20), &ccm->ahb_reset0_cfg);
	writel(readl(&ccm->ahb_reset0_cfg) | (1 << 20), &ccm->ahb_reset0_cfg);

	/* spi gating */
	writel(readl(&ccm->ahb_gate0) | (1 << 20), &ccm->ahb_gate0);
#else
	/* spi reset */
	setbits_le32(&ccm->spi_gate_reset, (0<<RESET_SHIFT));
	setbits_le32(&ccm->spi_gate_reset, (1<<RESET_SHIFT));

	/* spi gating */
	setbits_le32(&ccm->spi_gate_reset, (1<<GATING_SHIFT));
	/*sclk_freq = source_clk / (1 << n) / m*/
#endif

	SPI_DBG("src: %d Hz, spic:%d Hz,  n=%d, m=%d\n",source_clk, source_clk/ (1 << n) / m, n, m);

	return 0;
}

static int sunxi_get_spic_clk(int bus)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	unsigned long mclk_base = (unsigned long)&ccm->spi0_clk_cfg + bus*0x4;
	u32 reg_val = 0;
	u32 src = 0, clk = 0, sclk_freq = 0;
	u32 n, m;

	reg_val = readl((volatile void __iomem *)mclk_base);
	src = (reg_val >> 24)&0x7;
	n = (reg_val >> 8)&0x3;
	m = ((reg_val >> 0)&0xf) + 1;

	switch(src) {
		case 0:
			clk = 24000000;
			break;
		case 1:
#if defined(CONFIG_MACH_SUN8IW21)
			clk = 300000000;
#else
			clk = clock_get_pll6() * 1000000;
#endif
			break;
		default:
			clk = 0;
			break;
	}
	sclk_freq = clk / (1 << n) / m;
	SPI_INF("sclk_freq= %d Hz,reg_val: %x , n=%d, m=%d\n", sclk_freq, reg_val, n, m);
	return sclk_freq;

}


static int sunxi_spi_clk_exit(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

#if defined(CONFIG_MACH_SUN8IW11)
	/* clr spi gating */
	writel(readl(&ccm->ahb_gate0) & ~(1 << 20), &ccm->ahb_gate0);

	/* clr spi reset */
	writel(readl(&ccm->ahb_reset0_cfg) & ~(1 << 20), &ccm->ahb_reset0_cfg);

#else
	/* spi gating */
	clrbits_le32(&ccm->spi_gate_reset, 1<<GATING_SHIFT);

	/* spi reset */
	clrbits_le32(&ccm->spi_gate_reset, 1<<RESET_SHIFT);
#endif
	return 0;
}


static int sunxi_spi_cpu_writel(struct sunxi_spi_slave *sspi, const unsigned char *buf, unsigned int len)
{
	void __iomem *base_addr = (void __iomem *)(unsigned long)sspi->base_addr;
#ifndef CONFIG_DMA_ENGINE
	unsigned char time;
#endif
	unsigned int tx_len = len;	/* number of bytes receieved */
	unsigned char *tx_buf = (unsigned char *)buf;
	unsigned int poll_time = 0x7ffffff;

#if SPI_DEBUG
	printf("spi tx: %d bytes\n", len);
	/*sunxi_dump(tx_buf, len);*/
#endif
	for (; tx_len > 0; --tx_len) {
		writeb(*tx_buf++, base_addr + SPI_TXDATA_REG);
		if (spi_query_txfifo(base_addr) >= MAX_FIFU)
			for (time = 2; 0 < time; --time)
				;
	}

	while (spi_query_txfifo(base_addr) && (--poll_time > 0))
		;
	if (poll_time <= 0) {
		SPI_ERR("cpu transfer data time out!\n");
		return -1;
	}

	return 0;
}

static int sunxi_spi_cpu_readl(struct sunxi_spi_slave *sspi, unsigned char *buf, unsigned int len)
{
	void __iomem *base_addr = (void __iomem *)(unsigned long)sspi->base_addr;
	unsigned int rx_len = len;	/* number of bytes sent */
	unsigned char *rx_buf = buf;
	unsigned int poll_time = 0x7ffffff;

	while (rx_len && poll_time ) {
	/* rxFIFO counter */
		if (spi_query_rxfifo(base_addr) && (--poll_time > 0)) {
			*rx_buf++ =  readb(base_addr + SPI_RXDATA_REG);
			--rx_len;
		}
	}
	if (poll_time <= 0) {
		SPI_ERR("cpu receive data time out!\n");
		return -1;
	}
#if SPI_DEBUG
	printf("spi rx: %d bytes\n" , len);
	/*sunxi_dump(buf, len);*/
#endif
	return 0;
}


#ifdef CONFIG_SPI_USE_DMA
static int spi_dma_recv_start(void *spi_addr, uint spi_no, uchar *pbuf, uint byte_cnt)
{
	flush_cache((ulong)pbuf, byte_cnt);
	sunxi_dma_start(spi_rx_dma_hd, (ulong)spi_addr + SPI_RXDATA_REG, (ulong)pbuf, byte_cnt);

	return 0;
}
static int spi_wait_dma_recv_over(uint spi_no)
{
	return sunxi_dma_querystatus(spi_rx_dma_hd);
}

static int spi_dma_send_start(void *spi_addr, uint spi_no, uchar *pbuf, uint byte_cnt)
{
	flush_cache((ulong)pbuf, byte_cnt);
	sunxi_dma_start(spi_tx_dma_hd, (ulong)pbuf, (ulong)spi_addr + SPI_TXDATA_REG, byte_cnt);

	return 0;
}

static int spi_wait_dma_send_over(uint spi_no)
{
	return sunxi_dma_querystatus(spi_tx_dma_hd);
}

static int spi_dma_cfg(u32 spi_no)
{
	spi_rx_dma = malloc(sizeof(sunxi_dma_set));
	spi_tx_dma = malloc(sizeof(sunxi_dma_set));
	if (!(spi_rx_dma) || !(spi_tx_dma)) {
		printf("no enough memory to malloc \n");
		return -1;
	}
	memset(spi_tx_dma, 0, sizeof(sunxi_dma_set));
	memset(spi_rx_dma, 0, sizeof(sunxi_dma_set));
	spi_rx_dma_hd = sunxi_dma_request(DMAC_DMATYPE_NORMAL);
	spi_tx_dma_hd = sunxi_dma_request(DMAC_DMATYPE_NORMAL);


	if ((spi_tx_dma_hd == 0) || (spi_rx_dma_hd == 0)) {
		printf("spi request dma failed\n");

		return -1;
	}
	/* config spi rx dma */
	spi_rx_dma->loop_mode = 0;
	spi_rx_dma->wait_cyc  = 0x8;
	/* spi_rx_dma->data_block_size = 1 * DMAC_CFG_SRC_DATA_WIDTH_8BIT/8;*/
	spi_rx_dma->data_block_size = 1 * 32 / 8;

	spi_rx_dma->channal_cfg.src_drq_type	 = DMAC_CFG_TYPE_SPI0;	/* SPI0 */
	spi_rx_dma->channal_cfg.src_addr_mode	 = DMAC_CFG_SRC_ADDR_TYPE_IO_MODE;
	spi_rx_dma->channal_cfg.src_burst_length = DMAC_CFG_SRC_1_BURST;
	spi_rx_dma->channal_cfg.src_data_width	 = DMAC_CFG_SRC_DATA_WIDTH_32BIT;

	spi_rx_dma->channal_cfg.dst_drq_type	 = DMAC_CFG_TYPE_DRAM;	/* DRAM */
	spi_rx_dma->channal_cfg.dst_addr_mode	 = DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE;
	spi_rx_dma->channal_cfg.dst_burst_length = DMAC_CFG_DEST_1_BURST;
	spi_rx_dma->channal_cfg.dst_data_width	 = DMAC_CFG_DEST_DATA_WIDTH_32BIT;


	spi_tx_dma->loop_mode = 0;
	spi_tx_dma->wait_cyc  = 0x8;
	/* spi_tx_dma->data_block_size = 1 * DMAC_CFG_SRC_DATA_WIDTH_8BIT/8; */
	spi_tx_dma->data_block_size = 1 * 32 / 8;
	spi_tx_dma->channal_cfg.src_drq_type	 = DMAC_CFG_TYPE_DRAM;
	spi_tx_dma->channal_cfg.src_addr_mode	 = DMAC_CFG_SRC_ADDR_TYPE_LINEAR_MODE;
	spi_tx_dma->channal_cfg.src_burst_length = DMAC_CFG_SRC_1_BURST;
	spi_tx_dma->channal_cfg.src_data_width	 = DMAC_CFG_SRC_DATA_WIDTH_32BIT;

	spi_tx_dma->channal_cfg.dst_drq_type	 = DMAC_CFG_TYPE_SPI0;	/* SPI0 */
	spi_tx_dma->channal_cfg.dst_addr_mode	 = DMAC_CFG_DEST_ADDR_TYPE_IO_MODE;
	spi_tx_dma->channal_cfg.dst_burst_length = DMAC_CFG_DEST_1_BURST;
	spi_tx_dma->channal_cfg.dst_data_width	 = DMAC_CFG_DEST_DATA_WIDTH_32BIT;
	//spi_tx_dma->wait_cyc = 0x10;

	return 0;

}

static int sunxi_spi_dma_writel(struct sunxi_spi_slave *sspi, const unsigned char *buf, unsigned int len)
{
	void __iomem *base_addr = (void *)(ulong)sspi->base_addr;
	unsigned int tcnt = len;
	unsigned char *tx_buf = (unsigned char *)buf;
	ulong ctime = 0;
	int ret = 0;

#ifdef SUNXI_DMA_SECURITY
	if((get_boot_work_mode() != WORK_MODE_BOOT)
		&& (sunxi_get_securemode() !=  SUNXI_NORMAL_MODE))
		return sunxi_spi_cpu_writel(sspi, buf, len);
#endif

	writel((readl(base_addr + SPI_FIFO_CTL_REG) | SPI_FIFO_CTL_TX_DRQEN), base_addr + SPI_FIFO_CTL_REG);
	spi_dma_send_start(base_addr, 0, tx_buf, tcnt);

	/* wait DMA finish */
	ctime = get_timer(0);
	while (1) {
		if (spi_wait_dma_send_over(0) == 0) {
			ret = 0;
			break;
		}
		if (get_timer(ctime) > 1000) {
			printf("tx wait_dma_send_over fail\n");
			ret = -1;
			break;
		}
	}

	return ret;

}

static int sunxi_spi_dma_readl(struct sunxi_spi_slave *sspi, unsigned char *buf, unsigned int len)
{
	void __iomem *base_addr = (void *)(ulong)sspi->base_addr;
	unsigned char *rx_buf = buf;
	unsigned int rcnt = len;
	ulong ctime = 0;
	int ret = 0;

#ifdef SUNXI_DMA_SECURITY
	if((get_boot_work_mode() != WORK_MODE_BOOT)
		&& (sunxi_get_securemode() !=  SUNXI_NORMAL_MODE))
		return sunxi_spi_cpu_readl(sspi, buf, len);
#endif
	writel((readl(base_addr + SPI_FIFO_CTL_REG) | SPI_FIFO_CTL_RX_DRQEN), base_addr + SPI_FIFO_CTL_REG);
	spi_dma_recv_start(base_addr, 0, rx_buf, rcnt);

	/* wait DMA finish */
	ctime = get_timer(0);
	while (1) {
		if (spi_wait_dma_recv_over(0) == 0) {
			ret = 0;
			invalidate_dcache_range((ulong)rx_buf, (ulong)rx_buf + rcnt);
			break;
		}
		if (get_timer(ctime) > 5000) {
			printf("rx wait_dma_recv_over fail\n");
			ret = -1;
			break;
		}
	}

	return ret;
}

static int sunxi_spi_dma_disable(struct sunxi_spi_slave *sspi)
{
	void __iomem *base_addr = (void *)(ulong)sspi->base_addr;
	u32 fcr;

	/* disable dma req */
	fcr = readl(base_addr + SPI_FIFO_CTL_REG);
	fcr &= ~(SPI_FIFO_CTL_TX_DRQEN | SPI_FIFO_CTL_RX_DRQEN);
	writel(fcr, base_addr + SPI_FIFO_CTL_REG);

	return 0;
}

static void sunxi_dma_isr(void *p_arg)
{
	/*		printf("dma int occur\n"); */
}

int spi_dma_init(void)
{
	if (spi_dma_cfg(CONFIG_SF_DEFAULT_BUS)) {
		printf("spi dma cfg error!\n");
		return -1;
	}
	sunxi_dma_install_int(spi_rx_dma_hd, sunxi_dma_isr, NULL);
	sunxi_dma_install_int(spi_tx_dma_hd, sunxi_dma_isr, NULL);


	sunxi_dma_enable_int(spi_rx_dma_hd);
	sunxi_dma_enable_int(spi_tx_dma_hd);

	sunxi_dma_setting(spi_rx_dma_hd, (void *)spi_rx_dma);
	sunxi_dma_setting(spi_tx_dma_hd, (void *)spi_tx_dma);

	return 0;
}
#endif

int spi_init(void)
{
	SPI_ENTER();
#ifdef CONFIG_SPI_USE_DMA
	return spi_dma_init();
#endif

	return 0;
}

void spi_init_clk(struct spi_slave *slave)
{
	struct sunxi_spi_slave *sunxi_slave = to_sunxi_slave(slave);
	int ret = 0, nodeoffset = 0;
	u32 rval = 0;

	nodeoffset =  fdt_path_offset(working_fdt, "spi0/spi_board0");
	if (nodeoffset < 0) {
		SPI_INF("get spi0 para fail\n");
	} else {
		ret = fdt_getprop_u32(working_fdt, nodeoffset,
				"spi-max-frequency", (uint32_t *)(&rval));
		if (ret < 0) {
			SPI_INF("get spi-max-frequency fail %d\n", ret);
		} else
			sunxi_slave->max_hz = rval;
	}

	pr_force("spi sunxi_slave->max_hz:%d \n", sunxi_slave->max_hz);
	/* clock */
	if (sunxi_spi_clk_init(0, sunxi_slave->max_hz))
		pr_err("spi clk init error\n");

	spi_config_tc(sunxi_slave, 1, SPI_MODE_3,
			(void __iomem *)(unsigned long)sunxi_slave->base_addr);

	return;
}

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	if (sunxi_spi_check_cs(bus, cs) == SUNXI_SPI_OK)
		return 1;
	else
		return 0;
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
				  unsigned int max_hz, unsigned int mode)
{
	struct sunxi_spi_slave *sunxi_slave;
	int ret = 0;

	SPI_ENTER();
	if (!spi_cs_is_valid(bus, cs)) {
		printf("sunxi_spi: invalid bus %d / chip select %d\n", bus, cs);
		return NULL;
	}

	sunxi_slave = spi_alloc_slave(struct sunxi_spi_slave, bus, cs);
	if (!sunxi_slave)
		return NULL;

	sunxi_slave->max_hz = max_hz;
	sunxi_slave->mode = mode;
	sunxi_slave->cdr = 0;
	sunxi_slave->base_addr = SUNXI_SPI0_BASE + bus*SUNXI_SPI_PORT_OFFSET;
	sunxi_slave->right_sample_delay = SAMP_MODE_DL_DEFAULT;
	sunxi_slave->right_sample_mode = SAMP_MODE_DL_DEFAULT;

	sunxi_slave->slave.mode = mode;
	g_sspi = sunxi_slave;

	/* gpio */
	sunxi_spi_gpio_init();

	/*rx/tx bus width*/
	ret = sunxi_get_spi_mode();
	if (ret > 0) {
		sunxi_slave->mode = ret;
		sunxi_slave->slave.mode = ret;
	}

	/* clock */
	if(sunxi_spi_clk_init(0, max_hz)) {
		free(sunxi_slave);
		return NULL;
	}

	spi_config_tc(sunxi_slave, 1, SPI_MODE_3,
			(void __iomem *)(unsigned long)sunxi_slave->base_addr);

	return &sunxi_slave->slave;

}

void spi_free_slave(struct spi_slave *slave)
{
	struct sunxi_spi_slave *sunxi_slave = to_sunxi_slave(slave);
	SPI_ENTER();
	free(sunxi_slave);

	/* disable module clock */
	sunxi_spi_clk_exit();

	/*release gpio*/
}


int spi_claim_bus(struct spi_slave *slave)
{
	struct sunxi_spi_slave *sspi = to_sunxi_slave(slave);
	void __iomem *base_addr = (void __iomem *)(unsigned long)sspi->base_addr;
	uint sclk_freq = 0;

	SPI_ENTER();

	sclk_freq = sunxi_get_spic_clk(0);
	if(!sclk_freq)
		return -1;

	spi_soft_reset(base_addr);

	/* 1. enable the spi module */
	spi_enable_bus(base_addr);
	/* 2. set the default chip select */
	spi_set_cs(0, base_addr);

	/* 3. master: set spi module clock;
	 * 4. set the default frequency	10MHz
	 */
	spi_set_master(base_addr);
	spi_set_clk(sspi->max_hz, sclk_freq, base_addr, sspi->cdr);
	/* 5. master : set POL,PHA,SSOPL,LMTF,DDB,DHB; default: SSCTL=0,SMC=1,TBW=0.
	 * 6. set bit width-default: 8 bits
	 */
	spi_ss_level(base_addr, 1);
	spi_enable_tp(base_addr);
	/* 7. spi controller sends ss signal automatically*/
	spi_ss_owner(base_addr, 0);
	/* 8. reset fifo */
	spi_reset_fifo(base_addr);

	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	struct sunxi_spi_slave *sspi = to_sunxi_slave(slave);

	SPI_ENTER();
	/* disable the spi controller */
	spi_disable_bus((void __iomem *)(unsigned long)sspi->base_addr);

}

void sunxi_update_right_delay_para(struct mtd_info *mtd)
{
	struct spi_nor *nor = mtd->priv;
	struct spi_slave *slave = nor->spi;
	struct sunxi_spi_slave *sspi = to_sunxi_slave(slave);
	void __iomem *base_addr = (void *)(ulong)sspi->base_addr;
	unsigned int sample_delay;
	unsigned int start_ok = 0, end_ok = 0, len_ok = 0, mode_ok = 0;
	unsigned int start_backup = 0, end_backup = 0, len_backup = 0;
	unsigned int mode = 0, startry_mode = 0, endtry_mode = 6, block = 0;
	u8 erase_opcode = nor->erase_opcode;
	uint32_t erasesize = mtd->erasesize;
	if (mtd->size > SZ_16M)
		nor->erase_opcode = SPINOR_OP_BE_32K_4B;
	else
		nor->erase_opcode = SPINOR_OP_BE_32K;
	mtd->erasesize = 32 * 1024;

	size_t retlen;
	u_char *cache_source;
	u_char *cache_target;
	u_char *cache_boot0;
	int len = nor->page_size;

	struct erase_info instr;
	instr.addr = block * mtd->erasesize;
	instr.len = (endtry_mode - startry_mode + 1) * mtd->erasesize;

	//sspi->msglevel &= ~sspi_MSG_EN;
	cache_boot0 = malloc_align(instr.len, 64);
	mtd->_read(mtd, instr.addr, instr.len, &retlen, cache_boot0);
	mtd->_erase(mtd, &instr);

	/* re-initialize from device tree */
	spi_init_clk(slave);

	cache_source = malloc_align(len, 64);
	cache_target = malloc_align(len, 64);
	memset(cache_source, 0xa5, len);

	spi_samp_mode(base_addr, 1);
	spi_samp_dl_sw_status(base_addr, 1);
	for (mode = startry_mode; mode <= endtry_mode; mode++) {
		spi_set_sample_mode(base_addr, mode);
		for (sample_delay = 0; sample_delay < 64; sample_delay++) {
			spi_set_sample_delay(base_addr, sample_delay);
			mtd->_write(mtd, block * mtd->erasesize +
					sample_delay * nor->page_size,
					len, &retlen, cache_source);
		}

		for (sample_delay = 0; sample_delay < 64; sample_delay++) {
			spi_set_sample_delay(base_addr, sample_delay);
			memset(cache_target, 0, len);
			mtd->_read(mtd, block * mtd->erasesize +
					sample_delay * nor->page_size,
					len, &retlen, cache_target);

			if (strncmp((char *)cache_source, (char *)cache_target,
						len) == 0) {
				pr_debug("mode:%d delat:%d [OK]\n",
						mode, sample_delay);
				if (!len_backup) {
					start_backup = sample_delay;
					end_backup = sample_delay;
				} else
					end_backup = sample_delay;
				len_backup++;
			} else {
				pr_debug("mode:%d delay:%d [ERROR]\n",
						mode, sample_delay);
				if (!start_backup)
					continue;
				else {
					if (len_backup > len_ok) {
						len_ok = len_backup;
						start_ok = start_backup;
						end_ok = end_backup;
						mode_ok = mode;
					}

					len_backup = 0;
					start_backup = 0;
					end_backup = 0;
				}
			}
		}

		if (len_backup > len_ok) {
			len_ok = len_backup;
			start_ok = start_backup;
			end_ok = end_backup;
			mode_ok = mode;
		}
		len_backup = 0;
		start_backup = 0;
		end_backup = 0;

		block++;
	}

	if (!len_ok) {
		spi_samp_mode(base_addr, 0);
		spi_samp_dl_sw_status(base_addr, 0);
		if ((sspi->max_hz / 1000 / 1000) >= 60)
			sspi->right_sample_mode = 2;
		else if ((sspi->max_hz / 1000 / 1000) <= 24)
			sspi->right_sample_mode = 0;
		else
			sspi->right_sample_mode = 1;
	} else {
		sspi->right_sample_delay = (start_ok + end_ok) / 2;
		sspi->right_sample_mode = mode_ok;
		spi_set_sample_delay(base_addr, sspi->right_sample_delay);
	}
	pr_info("Sample mode:%d start:%d end:%d right_sample_delay:0x%x\n",
			mode_ok, start_ok, end_ok,
			sspi->right_sample_delay);
	spi_set_sample_mode(base_addr, sspi->right_sample_mode);

	mtd->_write(mtd, instr.addr, instr.len, &retlen, cache_boot0);

	//sspi->msglevel |= sspi_MSG_EN;
	nor->erase_opcode = erase_opcode;
	mtd->erasesize = erasesize;
	free_align(cache_source);
	free_align(cache_target);
	free_align(cache_boot0);
	return ;
}

#if defined(CONFIG_SPI_SAMP_DL_EN) && defined(CONFIG_SUNXI_SPINOR)
void boot_try_delay_param(struct mtd_info *mtd, boot_spinor_info_t *boot_info)
{
	struct spi_nor *nor = mtd->priv;
	struct spi_slave *slave = nor->spi;
	struct sunxi_spi_slave *sspi = to_sunxi_slave(slave);
	void __iomem *base_addr = (void *)(ulong)sspi->base_addr;
	unsigned int sample_delay;
	unsigned int start_ok = 0, end_ok = 0, len_ok = 0, mode_ok = 0;
	unsigned int start_backup = 0, end_backup = 0, len_backup = 0;
	unsigned int mode = 0, startry_mode = 0, endtry_mode = 6;
	size_t retlen, len = 512;
	boot0_file_head_t *boot0_head;
	boot0_head = malloc_align(len, 64);

	/* re-initialize from device tree */
	spi_init_clk(slave);

	spi_samp_mode(base_addr, 1);
	spi_samp_dl_sw_status(base_addr, 1);
	for (mode = startry_mode; mode <= endtry_mode; mode++) {
		spi_set_sample_mode(base_addr, mode);
		for (sample_delay = 0; sample_delay < 64; sample_delay++) {
			spi_set_sample_delay(base_addr, sample_delay);
			memset(boot0_head, 0, len);
			mtd->_read(mtd, 0, len, &retlen, (u_char *)boot0_head);

			if (strncmp((char *)boot0_head->boot_head.magic,
				(char *)BOOT0_MAGIC,
				sizeof(boot0_head->boot_head.magic)) == 0) {
				pr_debug("mode:%d delat:%d [OK]\n",
						mode, sample_delay);
				if (!len_backup) {
					start_backup = sample_delay;
					end_backup = sample_delay;
				} else
					end_backup = sample_delay;
				len_backup++;
			} else {
				pr_debug("mode:%d delay:%d [ERROR]\n",
						mode, sample_delay);
				if (!start_backup)
					continue;
				else {
					if (len_backup > len_ok) {
						len_ok = len_backup;
						start_ok = start_backup;
						end_ok = end_backup;
						mode_ok = mode;
					}

					len_backup = 0;
					start_backup = 0;
					end_backup = 0;
				}
			}
		}
		if (len_backup > len_ok) {
			len_ok = len_backup;
			start_ok = start_backup;
			end_ok = end_backup;
			mode_ok = mode;
		}
		len_backup = 0;
		start_backup = 0;
		end_backup = 0;
	}

	if (!len_ok) {
		spi_samp_mode(base_addr, 0);
		spi_samp_dl_sw_status(base_addr, 0);
		if ((sspi->max_hz / 1000 / 1000) >= 60)
			sspi->right_sample_mode = 2;
		else if ((sspi->max_hz / 1000 / 1000) <= 24)
			sspi->right_sample_mode = 0;
		else
			sspi->right_sample_mode = 1;
	} else {
		sspi->right_sample_delay = (start_ok + end_ok) / 2;
		sspi->right_sample_mode = mode_ok;
		spi_set_sample_delay(base_addr, sspi->right_sample_delay);
	}
	pr_info("Sample mode:%d start:%d end:%d right_sample_delay:0x%x\n",
			mode_ok, start_ok, end_ok,
			sspi->right_sample_delay);
	spi_set_sample_mode(base_addr, sspi->right_sample_mode);

	boot_info->sample_delay = sspi->right_sample_delay;
	boot_info->sample_mode = sspi->right_sample_mode;
	free_align(boot0_head);
	return;
}

extern int update_boot_param(struct spi_nor *flash);
int sunxi_set_right_delay_para(struct mtd_info *mtd)
{
	struct spi_nor *nor = mtd->priv;
	struct spi_slave *slave = nor->spi;
	struct sunxi_spi_slave *sspi = to_sunxi_slave(slave);
	void __iomem *base_addr = (void *)(ulong)sspi->base_addr;
	boot_spinor_info_t *boot_info = NULL;
	size_t retlen;

	struct sunxi_boot_param_region *boot_param = NULL;
	boot_param = malloc_align(BOOT_PARAM_SIZE, 64);

	mtd->_read(mtd, (CONFIG_SPINOR_UBOOT_OFFSET << 9) - BOOT_PARAM_SIZE,
			BOOT_PARAM_SIZE, &retlen, (u_char *)boot_param);
	boot_info = (boot_spinor_info_t *)boot_param->spiflash_info;

	if (strncmp((const char *)boot_param->header.magic,
				(const char *)BOOT_PARAM_MAGIC,
				sizeof(boot_param->header.magic)) ||
		strncmp((const char *)boot_info->magic,
				(const char *)SPINOR_BOOT_PARAM_MAGIC,
				sizeof(boot_info->magic))) {
		boot_try_delay_param(mtd, boot_info);
		if (update_boot_param(nor))
			printf("update boot param error\n");
	}

	if (boot_info->sample_delay == SAMP_MODE_DL_DEFAULT) {
		boot_try_delay_param(mtd, boot_info);
		if (update_boot_param(nor))
			printf("update boot param error\n");
	}

	pr_force("spi sample_mode:%x sample_delay:%x\n",
			boot_info->sample_mode, boot_info->sample_delay);

	spi_samp_mode(base_addr, 1);
	spi_samp_dl_sw_status(base_addr, 1);
	spi_set_sample_mode(base_addr, boot_info->sample_mode);
	spi_set_sample_delay(base_addr, boot_info->sample_delay);
	sspi->right_sample_delay = boot_info->sample_delay;
	sspi->right_sample_mode = boot_info->sample_mode;

	spi_init_clk(slave);
	free_align(boot_param);
	return 0;
}
#endif

int spi_xfer(struct spi_slave *slave, unsigned int bitlen,
		const void *dout, void *din, unsigned long flags)
{
	struct sunxi_spi_slave *sspi = to_sunxi_slave(slave);
	unsigned int len = bitlen / 8;
	int timeout = 0xfffff;
	void __iomem *base_addr = (void __iomem *)(unsigned long)sspi->base_addr;

	unsigned int tcnt = 0, rcnt = 0;
	static char cmd[16] = {0};
	static int cmd_len = 0;

	SPI_ENTER();

	/* Half-duplex only */
	if (din && dout)
		return -EINVAL;
	/* No data */
	if (!din && !dout)
		return 0;

	if( (flags&SPI_XFER_BEGIN) && (flags&SPI_XFER_END) ) {
		SPI_INF("xfer begin&end flag\n");
		memcpy(cmd, dout, len);
		cmd_len = len;
		rcnt = 0;
		tcnt = 0;
		/*stc = cmd_len; only cmd, no data*/
	}
	else if(flags & SPI_XFER_BEGIN) {
		SPI_INF("xfer only begin flag\n");
		memcpy(cmd, dout, len);
		cmd_len = len;
		return 0;
	} else if(flags & SPI_XFER_END){
		SPI_INF("xfer only end flag\n");
		tcnt = (dout ? len : 0); /*write cmd*/
		rcnt = (din ? len : 0);   /*read cmd*/
		/* stc = cmd_len + len; */
	}

	spi_disable_irq(0xffffffff, base_addr);
	spi_clr_irq_pending(0xffffffff, base_addr);

	//spi_set_bc_tc_stc(tcnt+cmd_len, rcnt, stc, 0, base_addr);
	sunxi_spi_mode_check(base_addr, tcnt+cmd_len, rcnt, cmd[0]);
	//spi_config_tc(sspi, 1, SPI_MODE_3, base_addr);
	spi_ss_level(base_addr, 1);
	spi_start_xfer(base_addr);

	/*send cmd*/
	if(cmd_len) {
		if(sunxi_spi_cpu_writel(sspi, (const void *)cmd, cmd_len))
			return -1;
		cmd_len = 0;
	}

	/* send data */
	if (tcnt) {
#ifdef CONFIG_SPI_USE_DMA
		if (tcnt <= 64) {
			if (sunxi_spi_cpu_writel(sspi, dout, tcnt))
				return -1;
		} else {
			if (sunxi_spi_dma_writel(sspi, dout, tcnt))
				return -1;
		}
#else
		if (sunxi_spi_cpu_writel(sspi, dout, tcnt))
			return -1;
#endif
	}

	/* recv data */
	if (rcnt) {
#ifdef CONFIG_SPI_USE_DMA
		if (rcnt <= 64) {
			if (sunxi_spi_cpu_readl(sspi, din, rcnt))
				return -1;
		} else {
			if (sunxi_spi_dma_readl(sspi, din, rcnt))
				return -1;
		}
#else
		if (sunxi_spi_cpu_readl(sspi, din, rcnt))
			return -1;
#endif
	}

	/* check int status error */
	if (spi_qry_irq_pending(base_addr) & SPI_INT_STA_ERR) {
		printf("int stauts error");
		return -1;
	}

	/* check tx/rx finish */
	timeout = 0xfffff;
	/* wait transfer complete */
	while (!(spi_qry_irq_pending(base_addr)&SPI_INT_STA_TC)) {
		timeout--;
		if (!timeout) {
			printf("SPI_ISR time_out \n");
			return -1;
		}
	}

#ifdef CONFIG_SPI_USE_DMA
	sunxi_spi_dma_disable(sspi);
#endif

	/* check SPI_EXCHANGE when SPI_MBC is 0 */
	if (readl(base_addr + SPI_BURST_CNT_REG) == 0) {
		if (readl(base_addr + SPI_TC_REG) & SPI_TC_XCH) {
			printf("XCH Control Error!!\n");
			return -1;
		}
	} else {
		printf("SPI_MBC Error!\n");
		return -1;
	}
	spi_clr_irq_pending(0xffffffff, base_addr);

	return 0;
}
