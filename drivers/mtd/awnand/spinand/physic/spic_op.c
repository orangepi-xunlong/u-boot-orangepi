// SPDX-License-Identifier: GPL-2.0

#define pr_fmt(fmt) "sunxi-spinand-phy: " fmt
/*
 * We should use stardand spi interface actually!
 * However, there are real too much bugs on it and there is no enought time
 * and person to fix it.
 * So, we have to do it as you see.
 */
#include <linux/kernel.h>
#include <linux/compat.h>
#include <linux/printk.h>
#include <asm/io.h>
#include <linux/errno.h>
#include <linux/libfdt.h>
#include <sunxi_board.h>
#include <sys_config.h>
#include <common.h>
#include <linux/mtd/aw-spinand.h>
#include <asm/arch-sunxi/dma.h>
#include <linux/types.h>
#include <asm/arch/clock.h>
#include <fdt_support.h>
#include <private_boot0.h>
#include <private_toc.h>
#include <dm.h>

#include "spic.h"
#include "../sunxi-spinand.h"

#define SPIC_DEBUG 0

static inline __u32 get_reg(__u32 addr)
{
	return readl((volatile unsigned int *)(ulong)addr);
}

static inline void set_reg(__u32 addr, __u32 val)
{
	writel(val, (volatile unsigned int *)(ulong)addr);
}

static struct spic_info {
	int fdt_node;
	unsigned int tx_dma_chan;
	unsigned int rx_dma_chan;
} spic0;

static int spic0_pio_request(void)
{
	int ret;
	struct spic_info *spi = &spic0;

	ret = fdt_path_offset(working_fdt, "spi0");
	if (ret < 0) {
		pr_err("get spi0 from fdt failed¥n");
		return ret;
	}
	spi->fdt_node = ret;

	ret = fdt_set_all_pin("spi0", "pinctrl-0");
	if (ret) {
		pr_err("set pin of spi0 failed¥n");
		return ret;
	}
	return 0;
}

static int spi0_change_clk(unsigned int dclk_src_sel, unsigned int dclk)
{
	u32 reg_val;
	u32 sclk0_src_sel, sclk0, sclk0_src, sclk0_pre_ratio_n, sclk0_src_t, sclk0_ratio_m;
	u32 sclk0_reg_adr;

	/* CCM_SPI0_CLK_REG */
	sclk0_reg_adr = (SUNXI_CCM_BASE + 0x0940);

	/* close dclk */
	if (dclk == 0) {
		reg_val = get_reg(sclk0_reg_adr);
		reg_val &= (~(0x1U<<31));
		set_reg(sclk0_reg_adr, reg_val);

		pr_info("close sclk0\n");
		return 0;
	}

	sclk0_src_sel = dclk_src_sel;
	sclk0 = dclk;
#if defined(CONFIG_MACH_SUN8IW21)
	sclk0_src = 300;
#else
	if (sclk0_src_sel == 0x0)
		sclk0_src = 24;
	else if (sclk0_src_sel < 0x3)
		sclk0_src = clock_get_pll6();
	else
		sclk0_src = 2 * clock_get_pll6();
#endif

	/* sclk0: 2*dclk */
	/* sclk0_pre_ratio_n */
	sclk0_pre_ratio_n = 3;
	if (sclk0_src > 4*16*sclk0)
		sclk0_pre_ratio_n = 3;
	else if (sclk0_src > 2*16*sclk0)
		sclk0_pre_ratio_n = 2;
	else if (sclk0_src > 1*16*sclk0)
		sclk0_pre_ratio_n = 1;
	else
		sclk0_pre_ratio_n = 0;

	sclk0_src_t = sclk0_src>>sclk0_pre_ratio_n;

	/* sclk0_ratio_m */
	sclk0_ratio_m = (sclk0_src_t/(sclk0)) - 1;
	if (sclk0_src_t%(sclk0))
		sclk0_ratio_m += 1;

	/* close clock */
	reg_val = get_reg(sclk0_reg_adr);
	reg_val &= (~(0x1U<<31));
	set_reg(sclk0_reg_adr, reg_val);

	/* configure */
	/* sclk0 <--> 2*dclk */
	reg_val = get_reg(sclk0_reg_adr);
	/* clock source select */
	reg_val &= (~(0x7<<24));
	reg_val |= (sclk0_src_sel&0x7)<<24;
	/* clock pre-divide ratio(N) */
	reg_val &= (~(0x3<<8));
	reg_val |= (sclk0_pre_ratio_n&0x3)<<8;
	/* clock divide ratio(M) */
	reg_val &= ~(0xf<<0);
	reg_val |= (sclk0_ratio_m&0xf)<<0;
	set_reg(sclk0_reg_adr, reg_val);

	/* open clock */
	reg_val = get_reg(sclk0_reg_adr);
	reg_val |= 0x1U << 31;
	set_reg(sclk0_reg_adr, reg_val);

	return 0;
}

static int spic0_clk_request(void)
{
	int reg_val = 0;
	int ret;

	/* 1. release ahb reset and open ahb clock gate */

	/* reset */
	reg_val = get_reg(SUNXI_CCM_BASE + 0x096c);
	reg_val &= (~(0x1U << 16));
	reg_val |= (0x1U << 16);
	set_reg(SUNXI_CCM_BASE + 0x096c, reg_val);
	/* ahb clock gate */
	reg_val = get_reg(SUNXI_CCM_BASE + 0x096c);
	reg_val &= (~(0x1U << 0));
	reg_val |= (0x1U << 0);
	set_reg(SUNXI_CCM_BASE + 0x096c, reg_val);

	/* 2. configure spic's sclk0 */
#if defined(CONFIG_MACH_SUN50IW11) || defined(CONFIG_MACH_SUN8IW20) ||	\
	defined(CONFIG_MACH_SUN8IW21)
	ret = spi0_change_clk(1, 10);
#else
	ret = spi0_change_clk(3, 10);
#endif
	if (ret < 0) {
		pr_err("set spi0 dclk failed\n");
		return ret;
	}
	return 0;
}

static int spic0_set_clk(unsigned int clk)
{
	int ret;

#if defined(CONFIG_MACH_SUN50IW11) || defined(CONFIG_MACH_SUN8IW20) ||	\
	defined(CONFIG_MACH_SUN8IW21)
	ret = spi0_change_clk(1, clk);
#else
	ret = spi0_change_clk(3, clk);
#endif
	if (ret < 0) {
		pr_err("NAND_SetClk, change spic clock failed\n");
		return -1;
	}

	pr_info("set spic0 clk to %u Mhz\n", clk);
	return 0;
}

#if SPIC_DEBUG
static void spi_print_info(void)
{
	char buf[1024] = {0};
	snprintf(buf, sizeof(buf)-1,
			"sspi->base_addr = 0x%x, the SPI control register:\n"
			"[VER] 0x%02x = 0x%08x, [GCR] 0x%02x = 0x%08x, [TCR] 0x%02x = 0x%08x\n"
			"[ICR] 0x%02x = 0x%08x, [ISR] 0x%02x = 0x%08x, [FCR] 0x%02x = 0x%08x\n"
			"[FSR] 0x%02x = 0x%08x, [WCR] 0x%02x = 0x%08x, [CCR] 0x%02x = 0x%08x\n"
			"[SDC] 0x%02x = 0x%08x, [BCR] 0x%02x = 0x%08x, [TCR] 0x%02x = 0x%08x\n"
			"[BCC] 0x%02x = 0x%08x, [DMA] 0x%02x = 0x%08x\n",
			SPI_BASE,
			SPI_VAR, readl((const volatile void *)SPI_VAR),
			SPI_GCR, readl((const volatile void *)SPI_GCR),
			SPI_TCR, readl((const volatile void *)SPI_TCR),
			SPI_IER, readl((const volatile void *)SPI_IER),
			SPI_ISR, readl((const volatile void *)SPI_ISR),

			SPI_FCR, readl((const volatile void *)SPI_FCR),
			SPI_FSR, readl((const volatile void *)SPI_FSR),
			SPI_WCR, readl((const volatile void *)SPI_WCR),
			SPI_CCR, readl((const volatile void *)SPI_CCR),
			SPI_SDC, readl((const volatile void *)SPI_SDC),

			SPI_MBC, readl((const volatile void *)SPI_MBC),
			SPI_MTC, readl((const volatile void *)SPI_MTC),
			SPI_BCC, readl((const volatile void *)SPI_BCC),
			SPI_DMA, readl((const volatile void *)SPI_DMA));
			printf("%s\n", buf);
}
#endif

/* init spi0 */
int spic0_init(void)
{
	int ret;
	unsigned int reg;
	struct spic_info *spi = &spic0;

	/* init pin */
	ret = spic0_pio_request();
	if (ret) {
		pr_err("request spi0 gpio fail!\n");
		return ret;
	}
	pr_info("request spi0 gpio ok\n");

	/* request general dma channel */
	ret = sunxi_dma_request(0);
	if (!ret) {
		pr_err("request tx dma fail!\n");
		return ret;
	}
	spi->tx_dma_chan = ret;
	pr_info("request general tx dma channel ok!\n");

	ret = sunxi_dma_request(0);
	if (!ret) {
		pr_err("request rx dma fail!\n");
		return ret;
	}
	spi->rx_dma_chan = ret;
	pr_info("request general rx dma channel ok!\n");

	/* init clk */
	ret = spic0_clk_request();
	if (ret) {
		pr_err("request spic0 clk failed\n");
		return ret;
	}
	ret = spic0_set_clk(20);
	if (ret) {
		pr_err("set spic0 clk failed\n");
		return ret;
	}
	pr_info("init spic0 clk ok\n");

	reg = SPI_SOFT_RST | SPI_TXPAUSE_EN | SPI_MASTER | SPI_ENABLE;
	set_reg(SPI_GCR, reg);

	reg = SPI_SET_SS_1 | SPI_DHB | SPI_SS_ACTIVE0;
	set_reg(SPI_TCR, reg);

	set_reg(SPI_FCR, SPI_TXFIFO_RST | (SPI_TX_WL << 16) | SPI_RX_WL);
	set_reg(SPI_IER, SPI_ERROR_INT);

#if SPIC_DEBUG
	spi_print_info();
#endif
	return 0;
}

static void spic0_clk_release(void)
{
	/* CCM_SPI0_CLK_REG */
	u32 sclk0_reg_adr = (SUNXI_CCM_BASE + 0x0940);
	/* close clock */
	u32 reg_val = get_reg(sclk0_reg_adr);
	reg_val &= (~(0x1U<<31));
	set_reg(sclk0_reg_adr, reg_val);

	/* reset */
	reg_val = get_reg(SUNXI_CCM_BASE + 0x096c);
	reg_val &= (~(0x1U << 16));
	set_reg(SUNXI_CCM_BASE + 0x096c, reg_val);
	/* ahb clock gate */
	reg_val = get_reg(SUNXI_CCM_BASE + 0x096c);
	reg_val &= (~(0x1U << 0));
	set_reg(SUNXI_CCM_BASE + 0x096c, reg_val);
}

int spic0_exit(void)
{
	unsigned int rval;
	struct spic_info *spi = &spic0;

	rval = get_reg(SPI_GCR);
	rval &= (~(SPI_SOFT_RST|SPI_MASTER|SPI_ENABLE));
	set_reg(SPI_GCR, rval);

	sunxi_dma_release(spi->rx_dma_chan);
	spi->rx_dma_chan = 0;

	sunxi_dma_release(spi->tx_dma_chan);
	spi->tx_dma_chan = 0;

	rval = SPI_SET_SS_1 | SPI_DHB | SPI_SS_ACTIVE0;
	set_reg(SPI_TCR, rval);

	spic0_clk_release();

	return 0;
}

static int spic0_wait_tc_end(void)
{
	unsigned int timeout = 0xffffff;

	while(!(get_reg(SPI_ISR) & (0x1 << 12))) {
		timeout--;
		if (!timeout)
			break;
	}

	if(timeout == 0) {
		pr_err("TC Complete wait status timeout!\n");
		return -ETIMEDOUT;
	}

	return 0;
}

static void spic0_sel_ss(unsigned int ssx)
{
	unsigned int rval = get_reg(SPI_TCR) & (~(3 << 4));

	rval |= ssx << 4;
	set_reg(SPI_TCR, rval);
}

static void spic0_set_sdm(unsigned int smod)
{
	unsigned int rval = get_reg(SPI_TCR) & (~(1 << 13));

	rval |= smod << 13;
	set_reg(SPI_TCR, rval);
}

static void spic0_set_sdc(unsigned int sample)
{
	unsigned int rval = get_reg(SPI_TCR) & (~(1 << 11));

	rval |= sample << 11;
	set_reg(SPI_TCR, rval);
}

static void spic0_set_sdc1(unsigned int sample)
{
	unsigned int rval = get_reg(SPI_TCR) & (~(1 << 15));

	rval |= sample << 15;
	set_reg(SPI_TCR, rval);
}

static void spic0_set_trans_mode(unsigned int mode)
{
	unsigned int rval = get_reg(SPI_TCR)&(~(3 << 0));

	rval |= mode << 0;
	set_reg(SPI_TCR, rval);
}

void spic0_config_io_mode(unsigned int rxmode, unsigned int dbc,
		unsigned int stc)
{
	if (rxmode == 0)
		set_reg(SPI_BCC, (dbc << 24) | (stc));
	else if (rxmode == 1)
		set_reg(SPI_BCC, (1 << 28) | (dbc << 24) | stc);
	else if (rxmode == 2)
		set_reg(SPI_BCC, (1 << 29) | (dbc << 24) | stc);
}

void spic0_samp_dl_sw_status(unsigned int status)
{
	unsigned int rval = get_reg(SPI_SDC);

	if (status)
		rval |= SPI_SAMP_DL_SW_EN;
	else
		rval &= ~SPI_SAMP_DL_SW_EN;

	set_reg(SPI_SDC, rval);
}

void spic0_samp_mode(unsigned int status)
{
	unsigned int rval = get_reg(SPI_GCR);

	if (status)
		rval |= SPI_SAMP_MODE_EN;
	else
		rval &= ~SPI_SAMP_MODE_EN;

	set_reg(SPI_GCR, rval);
}

static int xfer_by_cpu(unsigned int tcnt, char *txbuf,
		unsigned int rcnt, char *rxbuf, unsigned int dummy_cnt)
{
	u32 i = 0;
	int timeout = 0xfffff;
	char *tx_buffer = txbuf ;
	char *rx_buffer = rxbuf;

	set_reg(SPI_IER, 0);
	set_reg(SPI_ISR, 0xffffffff);

	set_reg(SPI_MTC, tcnt);
	set_reg(SPI_MBC, tcnt + rcnt + dummy_cnt);
	set_reg(SPI_TCR, get_reg(SPI_TCR) | SPI_EXCHANGE);
	if (tcnt) {
		i = 0;
		while (i < tcnt) {
			//send data
			while (((get_reg(SPI_FSR) >> 16) == SPI_FIFO_SIZE) );
			writeb(*(tx_buffer + i), (volatile void __iomem *)(ulong)SPI_TXD);
			i++;
		}
	}

	/* start transmit */
	if (rcnt) {
		i = 0;
		while (i < rcnt) {
			//receive valid data
			while (((get_reg(SPI_FSR))&0x7f) == 0);
			*(rx_buffer + i) = readb((volatile void __iomem *)(ulong)SPI_RXD);
			i++;
		}
	}

	//check fifo error
	if ((get_reg(SPI_ISR) & (0xf << 8))) {
		pr_err("check fifo error: 0x%08x\n", get_reg(SPI_ISR));
		return -EINVAL;
	}

	//check tx/rx finish
	timeout = 0xfffff;
	while (!(get_reg(SPI_ISR) & (0x1 << 12))) {
		timeout--;
		if (!timeout) {
			pr_err("SPI_ISR time_out\n");
			return -EINVAL;
		}
	}

	//check SPI_EXCHANGE when SPI_MBC is 0
	if (get_reg(SPI_MBC) == 0) {
		if (get_reg(SPI_TCR) & SPI_EXCHANGE) {
			pr_err("XCH Control Error!!\n");
			return -EINVAL;
		}
	} else {
		pr_err("SPI_MBC Error!\n");
		return -EINVAL;
	}

	set_reg(SPI_ISR, 0xfffff); /* clear  flag */
	return 0;
}

#define ALIGN_6BITS (6)
#define ALIGN_6BITS_SIZE (1<<ALIGN_6BITS)
#define ALIGN_6BITS_MASK (~(ALIGN_6BITS_SIZE-1))
#define ALIGN_TO_64BYTES(len) (((len) +63)&ALIGN_6BITS_MASK)
static int spic0_dma_start(unsigned int tx_mode, unsigned int addr,
		unsigned length)
{
	int ret = 0;
	struct spic_info *spi = &spic0;
	sunxi_dma_set dma_set;

	dma_set.loop_mode = 0;
	dma_set.wait_cyc = 8;
	dma_set.data_block_size = 0;

	if (tx_mode) {
		dma_set.channal_cfg.src_drq_type = DMAC_CFG_TYPE_DRAM;
		dma_set.channal_cfg.src_addr_mode = DMAC_CFG_SRC_ADDR_TYPE_LINEAR_MODE;
		dma_set.channal_cfg.src_burst_length = DMAC_CFG_SRC_8_BURST;
		dma_set.channal_cfg.src_data_width = DMAC_CFG_SRC_DATA_WIDTH_32BIT;
		dma_set.channal_cfg.reserved0 = 0;

		dma_set.channal_cfg.dst_drq_type = DMAC_CFG_TYPE_SPI0;
		dma_set.channal_cfg.dst_addr_mode = DMAC_CFG_DEST_ADDR_TYPE_IO_MODE;
		dma_set.channal_cfg.dst_burst_length = DMAC_CFG_DEST_8_BURST;
		dma_set.channal_cfg.dst_data_width = DMAC_CFG_DEST_DATA_WIDTH_32BIT;
		dma_set.channal_cfg.reserved1 = 0;
	} else {
		dma_set.channal_cfg.src_drq_type = DMAC_CFG_TYPE_SPI0;
		dma_set.channal_cfg.src_addr_mode = DMAC_CFG_SRC_ADDR_TYPE_IO_MODE;
		dma_set.channal_cfg.src_burst_length = DMAC_CFG_SRC_8_BURST;
		dma_set.channal_cfg.src_data_width = DMAC_CFG_SRC_DATA_WIDTH_32BIT;
		dma_set.channal_cfg.reserved0 = 0;

		dma_set.channal_cfg.dst_drq_type = DMAC_CFG_TYPE_DRAM;
		dma_set.channal_cfg.dst_addr_mode = DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE;
		dma_set.channal_cfg.dst_burst_length = DMAC_CFG_DEST_8_BURST;
		dma_set.channal_cfg.dst_data_width = DMAC_CFG_DEST_DATA_WIDTH_32BIT;
		dma_set.channal_cfg.reserved1 = 0;
	}

	if (tx_mode) {
		ret = sunxi_dma_setting(spi->tx_dma_chan, &dma_set);
		if (ret < 0) {
			pr_err("uboot: sunxi_dma_setting for tx faild!\n");
			return -1;
		}
	} else {
		ret = sunxi_dma_setting(spi->rx_dma_chan, &dma_set);
		if (ret < 0) {
			pr_err("uboot: sunxi_dma_setting for rx faild!\n");
			return -1;
		}
	}

	if (tx_mode) {
		flush_cache((uint)addr, ALIGN_TO_64BYTES(length));
		ret = sunxi_dma_start(spi->tx_dma_chan, addr,
				(__u32)SPI_TX_IO_DATA, length);
	} else {
		flush_cache((uint)addr, ALIGN_TO_64BYTES(length));
		ret = sunxi_dma_start(spi->rx_dma_chan,
				(__u32)SPI_RX_IO_DATA, addr, length);
	}
	if (ret < 0) {
		pr_err("uboot: sunxi_dma_start for spi nand faild!\n");
		return -1;
	}

	return 0;
}

static int spic0_wait_dma_finish(unsigned int tx_flag, unsigned int rx_flag)
{
	struct spic_info *spi = &spic0;
	__u32 timeout = 0xffffff;

	if (tx_flag) {
		timeout = 0xffffff;
		while (sunxi_dma_querystatus(spi->tx_dma_chan)) {
			timeout--;
			if (!timeout)
				break;
		}

		if (timeout <= 0) {
			pr_err("TX DMA wait status timeout!\n");
			return -ETIMEDOUT;
		}
	}

	if (rx_flag) {
		timeout = 0xffffff;
		while (sunxi_dma_querystatus(spi->rx_dma_chan)) {
			timeout--;
			if (!timeout)
				break;
		}

		if (timeout <= 0) {
			pr_err("RX DMA wait status timeout!\n");
			return -ETIMEDOUT;
		}
	}

	return 0;
}

static int xfer_by_dma(unsigned int tcnt, char *txbuf,
		unsigned int rcnt, char *rxbuf, unsigned int dummy_cnt)
{
	unsigned int xcnt = 0, fcr;
	unsigned int tx_dma_flag = 0;
	unsigned int rx_dma_flag = 0;
	int timeout = 0xffff;

	set_reg(SPI_IER, 0);
	set_reg(SPI_ISR, 0xffffffff);//clear status register

	set_reg(SPI_MTC, tcnt);
	set_reg(SPI_MBC, tcnt + rcnt + dummy_cnt);

	/* start transmit */

	set_reg(SPI_TCR, get_reg(SPI_TCR) | SPI_EXCHANGE);
	if(tcnt) {
		if(tcnt <= 64) {
			xcnt = 0;
			timeout = 0xfffff;
			while (xcnt < tcnt) {
				while (((get_reg(SPI_FSR) >> 16) & 0x7f) >= SPI_FIFO_SIZE) {
					if (--timeout < 0)
						return -ETIMEDOUT;
				}
				writeb(*(txbuf + xcnt), (volatile void __iomem *)(ulong)SPI_TXD);
				xcnt++;
			}
		} else {
			tx_dma_flag = 1;
			set_reg(SPI_FCR, get_reg(SPI_FCR) | SPI_TXDMAREQ_EN);
			spic0_dma_start(1, (unsigned long) txbuf, tcnt);
		}
	}

	if(rcnt) {
		if (rcnt <= 64) {
			xcnt = 0;
			timeout = 0xfffff;
			while(xcnt < rcnt) {
				if (((get_reg(SPI_FSR)) & 0x7f) && (--timeout > 0)) {
					*(rxbuf + xcnt) = readb((volatile void __iomem *)(ulong)SPI_RXD);
					xcnt++;
				}
			}
			if (timeout <= 0) {
				pr_err("cpu receive data timeout!\n");
				return -ETIMEDOUT;
			}
		} else {
			rx_dma_flag = 1;
			set_reg(SPI_FCR, (get_reg(SPI_FCR) | SPI_RXDMAREQ_EN));
			spic0_dma_start(0, (unsigned long) rxbuf, rcnt);
		}
	}

	if (spic0_wait_dma_finish(tx_dma_flag, rx_dma_flag)) {
		pr_err("DMA wait status timeout!\n");
		return -ETIMEDOUT;
	}

	if (rx_dma_flag)
		invalidate_dcache_range((unsigned long)rxbuf,
				ALIGN_TO_64BYTES((unsigned long)rxbuf + rcnt));

	if (spic0_wait_tc_end()) {
		pr_err("wait tc complete timeout!\n");
		return -ETIMEDOUT;
	}

	fcr = get_reg(SPI_FCR);
	fcr &= ~(SPI_TXDMAREQ_EN | SPI_RXDMAREQ_EN);
	set_reg(SPI_FCR, fcr);
	if (get_reg(SPI_ISR) & (0xf << 8)) {
		pr_err("FIFO status error: 0x%x!¥n",get_reg(SPI_ISR));
		return -EINVAL;
	}

	if (get_reg(SPI_TCR) & SPI_EXCHANGE)
		pr_err("XCH Control Error!!¥n");

	set_reg(SPI_ISR, 0xffffffff);  /* clear  flag */
	return 0;
}

static int spic0_is_secure_chip(void)
{
	return sunxi_get_secureboard();
}

/* int spic0_is_burn_mode(void) */
/* return 0:uboot is in boot mode */
/* rerurn 1:uboot is in [usb,card...]_product mode */
static int spic0_is_burn_mode(void)
{
	int mode = get_boot_work_mode();

	if (WORK_MODE_BOOT == mode)
		return 0;
	else
		return 1;
}

static void spic0_set_sample_mode(unsigned int mode)
{
	unsigned int sample_mode[7] = {
		DELAY_NORMAL_SAMPLE, DELAY_0_5_CYCLE_SAMPLE,
		DELAY_1_CYCLE_SAMPLE, DELAY_1_5_CYCLE_SAMPLE,
		DELAY_2_CYCLE_SAMPLE, DELAY_2_5_CYCLE_SAMPLE,
		DELAY_3_CYCLE_SAMPLE
	};
	spic0_set_sdm((sample_mode[mode] >> 8) & 0xf);
	spic0_set_sdc((sample_mode[mode] >> 4) & 0xf);
	spic0_set_sdc1(sample_mode[mode] & 0xf);
}

static void spic0_set_sample_delay(unsigned int sample_delay)
{
	unsigned int rval = get_reg(SPI_SDC)&(~(0x3f << 0));

	rval |= sample_delay;
	set_reg(SPI_SDC, rval);
	mdelay(1);
}

int update_right_delay_para(struct mtd_info *mtd)
{
	struct aw_spinand *spinand = mtd_to_spinand(mtd);
	struct aw_spinand_chip *chip = spinand_to_chip(spinand);
	struct aw_spinand_info *info = chip->info;
	unsigned int erasesize = info->phy_block_size(chip);
	unsigned int len = info->phy_page_size(chip);
	unsigned int sample_delay;
	unsigned int start_backup = 0, end_backup = 0;
	unsigned int mode = 0, startry_mode = 0, endtry_mode = 6, block = 0;
	unsigned int ret, val;
	int fdt_off;
	unsigned int half_cycle_ps, sample_delay_ps = 160;
	unsigned int min_delay = 0, max_delay = 0, right_sample_delay = 0;

	size_t retlen;
	u_char *cache_source;
	u_char *cache_target;
	u_char *cache_boot0;

	struct erase_info instr;
	instr.addr = block * erasesize;
	instr.len = (endtry_mode - startry_mode + 1) * erasesize;

	spinand->msglevel &= ~SPINAND_MSG_EN;
	cache_boot0 = malloc_align(instr.len, 64);
	mtd->_read(mtd, instr.addr, instr.len, &retlen, cache_boot0);
	mtd->_erase(mtd, &instr);

	/* re-initialize from device tree */
	fdt_off = fdt_path_offset(working_fdt, "spi0/spi-nand");
	if (fdt_off < 0) {
		pr_info("get spi-nand node from fdt failed\n");
		return -EINVAL;
	}
	ret = fdt_getprop_u32(working_fdt, fdt_off, "spi-max-frequency", &val);
	if (ret < 0) {
		pr_err("can't get spi-max-frequency\n");
		return -EINVAL;
	}
	spic0_set_clk(val / 1000 / 1000);
	/* How much (ps) is required for half a cycle */
	half_cycle_ps = 1 * 1000 * 1000  / (val / 1000 / 1000) / 2;

	cache_source = malloc_align(len, 64);
	cache_target = malloc_align(len, 64);
	memset(cache_source, 0xa5, len);

	spic0_samp_mode(1);
	spic0_samp_dl_sw_status(1);
	for (mode = startry_mode; mode <= endtry_mode; mode++) {
		spic0_set_sample_mode(mode);
		for (sample_delay = 0; sample_delay < 64; sample_delay++) {
			spic0_set_sample_delay(sample_delay);
			mtd->_write(mtd, block * erasesize +
					sample_delay * len,
					len, &retlen, cache_source);
		}

		for (sample_delay = 0; sample_delay < 64; sample_delay++) {
			spic0_set_sample_delay(sample_delay);
			memset(cache_target, 0, len);
			mtd->_read(mtd, block * erasesize +
					sample_delay * len,
					len, &retlen, cache_target);

			if (strncmp((char *)cache_source, (char *)cache_target,
						len) == 0) {
				pr_debug("mode:%d delat:%d time:%dps[OK]\n",
						mode, sample_delay,
						mode * half_cycle_ps +
						sample_delay * sample_delay_ps);
				if (!start_backup) {
					start_backup = mode * half_cycle_ps +
						sample_delay * sample_delay_ps;
					end_backup = mode * half_cycle_ps +
						sample_delay * sample_delay_ps;
				} else {
					end_backup = mode * half_cycle_ps +
						sample_delay * sample_delay_ps;
				}
			} else {
				pr_debug("mode:%d delay:%d time:%dps [ERROR]\n",
						mode, sample_delay,
						mode * half_cycle_ps +
						sample_delay * sample_delay_ps);
				if (!start_backup)
					continue;
				else {
					if (!min_delay)
						min_delay = start_backup;
					else if (start_backup < min_delay)
						min_delay = start_backup;
					if (end_backup > max_delay)
						max_delay = end_backup;

					start_backup = 0;
					end_backup = 0;
				}
			}
		}

		if ((start_backup < min_delay || !min_delay) && start_backup)
			min_delay = start_backup;
		if (end_backup > max_delay)
			max_delay = end_backup;

		start_backup = 0;
		end_backup = 0;

		block++;
	}

	right_sample_delay = (min_delay + max_delay) / 2;
	if (!right_sample_delay) {
		spic0_samp_mode(0);
		spic0_samp_dl_sw_status(0);
		if ((val / 1000 / 1000) >= 60)
			spinand->right_sample_mode = 2;
		else if ((val / 1000 / 1000) <= 24)
			spinand->right_sample_mode = 0;
		else
			spinand->right_sample_mode = 1;
	} else {

		spinand->right_sample_delay =
			(right_sample_delay % half_cycle_ps) / sample_delay_ps;
		spinand->right_sample_mode =
			right_sample_delay / half_cycle_ps;
		spic0_set_sample_delay(spinand->right_sample_delay);
	}
	pr_info("Sample mode:%d  min_delay:%d max_delay:%d right_delay:%x)\n",
			spinand->right_sample_mode,
			min_delay, max_delay,
			spinand->right_sample_delay);
	spic0_set_sample_mode(spinand->right_sample_mode);

	mtd->_write(mtd, instr.addr, instr.len, &retlen, cache_boot0);

	spinand->msglevel |= SPINAND_MSG_EN;
	free_align(cache_source);
	free_align(cache_target);
	free_align(cache_boot0);
	return 0;
}

int set_right_delay_para(struct mtd_info *mtd)
{
	struct aw_spinand *spinand = mtd_to_spinand(mtd);
	struct aw_spinand_chip *chip = spinand_to_chip(spinand);
	struct aw_spinand_info *info = chip->info;
	unsigned int phy_block_size = info->phy_block_size(chip);
	u_char *buffer;
	boot0_file_head_t *boot0;
	boot_spinand_para_t *boot_info;
	size_t retlen;
	unsigned start, end;
	int ret = 0;
	buffer = calloc(1, 2048);

	start = 0;
	/* start addr of uboot is the end addr of boot0 */
	spinand_uboot_blknum(&end, NULL);

	/* In general, size of boot0 is less than a block */
	while (start < end) {
		ret = mtd->_read(mtd, start * phy_block_size, 2048,
				&retlen, buffer);
		boot0 = (boot0_file_head_t *)buffer;
		if (ret >= 0 && !strncmp((const char *)boot0->boot_head.magic,
				BOOT0_MAGIC, strlen(BOOT0_MAGIC)))
			break;

		pr_err("read boot0 to blk %d failed\n", start);
		start++;
		if (start >= end) {
			spic0_change_mode(80);
			return -1;
		}
	}

	if (gd->bootfile_mode  == SUNXI_BOOT_FILE_NORMAL
		 || gd->bootfile_mode  == SUNXI_BOOT_FILE_PKG) {
		boot0 = (boot0_file_head_t *)buffer;
		boot_info = (boot_spinand_para_t *)boot0->prvt_head.storage_data;
	} else {
		sbrom_toc0_config_t *toc0_config = NULL;
		toc0_config = (sbrom_toc0_config_t *)(buffer + 0x80);
		boot_info = (boot_spinand_para_t *)toc0_config->storage_data;
	}

	pr_alert("spinand sample_mode:%x sample_delay:%x\n",
			boot_info->sample_mode, boot_info->sample_delay);

	if ((boot_info->sample_delay || boot_info->sample_mode) &&
			boot_info->sample_delay != 0xaaaaffff &&
			boot_info->sample_delay < 64) {
		spic0_samp_mode(1);
		spic0_samp_dl_sw_status(1);
		spic0_set_sample_mode(boot_info->sample_mode);
		spic0_set_sample_delay(boot_info->sample_delay);
		spinand->right_sample_delay = boot_info->sample_delay;
		spinand->right_sample_mode = boot_info->sample_mode;

		spic0_set_clk(boot_info->FrequencePar);
	} else
		spic0_change_mode(boot_info->FrequencePar);

	free(buffer);
	return 0;
}

int spic0_change_mode(unsigned int clk)
{
	spic0_set_trans_mode(0);
	if (clk >= 60) {
		spic0_set_clk(clk);
		spic0_set_sdm(0);
		spic0_set_sdc(1);
	} else if (clk <= 24) {
		spic0_set_clk(clk);
		spic0_set_sdm(1);
		spic0_set_sdc(0);
	} else {
		spic0_set_clk(clk);
		spic0_set_sdm(0);
		spic0_set_sdc(0);
	}
	return 0;
}

static int spic0_rw(unsigned int tcnt, char *txbuf,
		unsigned int rcnt, char *rxbuf, unsigned int dummy_cnt)
{
	int secure_burn_flag = (spic0_is_secure_chip() && spic0_is_burn_mode());
	if (!secure_burn_flag)
		return xfer_by_dma(tcnt, txbuf, rcnt, rxbuf, dummy_cnt);
	else
		return xfer_by_cpu(tcnt, txbuf, rcnt, rxbuf, dummy_cnt);
}

int spi0_write(void *txbuf, unsigned int txnum, int mode)
{
	spic0_sel_ss(0);
	if (mode == SPI0_MODE_AUTOSET)
		spic0_config_io_mode(0, 0, txnum);
	return spic0_rw(txnum, txbuf, 0, NULL, 0);
}

int spi0_write_then_read(void *txbuf, unsigned int txnum,
		void *rxbuf, unsigned int rxnum, int mode)
{
	spic0_sel_ss(0);
	if (mode == SPI0_MODE_AUTOSET)
		spic0_config_io_mode(0, 0, txnum);
	return spic0_rw(txnum, txbuf, rxnum, rxbuf, 0);
}
