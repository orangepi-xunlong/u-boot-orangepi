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

#include "spic.h"

static inline __u32 get_reg(__u32 addr)
{
	return readl((volatile unsigned int *)addr);
}

static inline void set_reg(__u32 addr, __u32 val)
{
	writel(val, (volatile unsigned int *)addr);
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

static unsigned int get_pll6_clk(void)
{
	__u32 reg_val;
	__u32 factor_n;
	__u32 factor_m0;
	__u32 factor_m1;

	reg_val  = get_reg(NAND_CLK_BASE_ADDR + 0x20);
	factor_n = ((reg_val >> 8) & 0xFF) + 1;
	factor_m0 = ((reg_val >> 0) & 0x1) + 1;
	factor_m1 = ((reg_val >> 1) & 0x1) + 1;

	return 24000000 * factor_n / factor_m0 / factor_m1 / 2;
}

static int spi0_change_clk(unsigned int dclk_src_sel, unsigned int dclk)
{
	u32 reg_val;
	u32 sclk0_src_sel, sclk0, sclk0_src, sclk0_pre_ratio_n, sclk0_src_t, sclk0_ratio_m;
	u32 sclk0_reg_adr;

	/* CCM_SPI0_CLK_REG */
	sclk0_reg_adr = (NAND_CLK_BASE_ADDR + 0x0940);

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

	if (sclk0_src_sel == 0x0)
		sclk0_src = 24;
	else if (sclk0_src_sel < 0x3)
		sclk0_src = get_pll6_clk() / 1000000;
	else
		sclk0_src = 2 * get_pll6_clk() / 1000000;

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
	reg_val = get_reg(NAND_CLK_BASE_ADDR + 0x096c);
	reg_val &= (~(0x1U << 16));
	reg_val |= (0x1U << 16);
	set_reg(NAND_CLK_BASE_ADDR + 0x096c, reg_val);
	/* ahb clock gate */
	reg_val = get_reg(NAND_CLK_BASE_ADDR + 0x096c);
	reg_val &= (~(0x1U << 0));
	reg_val |= (0x1U << 0);
	set_reg(NAND_CLK_BASE_ADDR + 0x096c, reg_val);

	/* 2. configure spic's sclk0 */
	ret = spi0_change_clk(3, 10);
	if (ret < 0) {
		pr_err("set spi0 dclk failed\n");
		return ret;
	}
	return 0;	
}

static int spic0_set_clk(unsigned int clk)
{
	int ret;

	ret = spi0_change_clk(3, clk);
	if (ret < 0) {
		pr_err("NAND_SetClk, change spic clock failed\n");
		return -1;
	}

	pr_info("set spic0 clk to %u Mhz\n", clk);
	return 0;
}

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
	return 0;    
}

static void spic0_clk_release(void)
{
	u32 reg_val;
	u32 sclk0_reg_adr, sclk1_reg_adr;

	//disable nand sclock gating
	sclk0_reg_adr = (NAND_CLK_BASE_ADDR + 0x0810); //CCM_NAND0_CLK0_REG;
	sclk1_reg_adr = (NAND_CLK_BASE_ADDR + 0x0814); //CCM_NAND0_CLK1_REG;

	reg_val = get_reg(sclk0_reg_adr);
	reg_val &= (~(0x1U<<31));
	set_reg(sclk0_reg_adr, reg_val);

	reg_val = get_reg(sclk1_reg_adr);
	reg_val &= (~(0x1U<<31));
	set_reg(sclk1_reg_adr, reg_val);

	// reset
	reg_val = get_reg(NAND_CLK_BASE_ADDR + 0x082C);
	reg_val &= (~(0x1U<<16));
	set_reg((NAND_CLK_BASE_ADDR + 0x082C), reg_val);
	// ahb clock gate
	reg_val = get_reg(NAND_CLK_BASE_ADDR + 0x082C);
	reg_val &= (~(0x1U<<0));
	set_reg((NAND_CLK_BASE_ADDR + 0x082C), reg_val);

	// disable nand mbus gate
	reg_val = get_reg(NAND_CLK_BASE_ADDR + 0x0804);
	reg_val &= (~(0x1U<<5));
	set_reg((NAND_CLK_BASE_ADDR + 0x0804), reg_val);
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

static void spic0_set_sample_mode(unsigned int smod)
{
	unsigned int rval = get_reg(SPI_TCR) & (~(1 << 13));

	rval |= smod << 13;
	set_reg(SPI_TCR, rval);
}

static void spic0_set_sample(unsigned int sample)
{
	unsigned int rval = get_reg(SPI_TCR) & (~(1 << 11));
	rval |= sample << 11;
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
			writeb(*(tx_buffer + i), SPI_TXD);
			i++;
		}
	}

	/* start transmit */
	if (rcnt) {
		i = 0;
		while (i < rcnt) {
			//receive valid data
			while (((get_reg(SPI_FSR))&0x7f) == 0);
			*(rx_buffer + i) = readb(SPI_RXD);
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
	dma_set.wait_cyc = 32;
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
				writeb(*(txbuf + xcnt), SPI_TXD);
				xcnt++;
			}
		} else {
			tx_dma_flag = 1;
			set_reg(SPI_FCR, get_reg(SPI_FCR) | SPI_TXDMAREQ_EN);
			spic0_dma_start(1, (unsigned int) txbuf, tcnt);
		}
	}

	if(rcnt) {
		if (rcnt <= 64) {
			xcnt = 0;
			timeout = 0xfffff;
			while(xcnt < rcnt) {
				if (((get_reg(SPI_FSR)) & 0x7f) && (--timeout > 0)) {
					*(rxbuf + xcnt) = readb(SPI_RXD);
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
			spic0_dma_start(0, (unsigned int) rxbuf, rcnt);
		}
	}

	if (spic0_wait_dma_finish(tx_dma_flag, rx_dma_flag)) {
		pr_err("DMA wait status timeout!\n");
		return -ETIMEDOUT;
	}

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

int spic0_change_mode(unsigned int clk)
{
	spic0_set_trans_mode(0);

	if (clk >= 60) {
		spic0_set_clk(clk);
		spic0_set_sample_mode(0);
		spic0_set_sample(1);
	} else if (clk <= 24) {
		spic0_set_clk(clk);
		spic0_set_sample_mode(1);
		spic0_set_sample(0);
	} else {
		pr_err("not support mode\n");
		return -EINVAL;
	}
	return 0;
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
