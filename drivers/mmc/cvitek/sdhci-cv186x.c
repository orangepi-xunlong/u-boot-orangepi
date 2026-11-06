// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2016 Socionext Inc.
 *   Author: Masahiro Yamada <yamada.masahiro@socionext.com>
 */

#include <common.h>
#include <dm.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/sizes.h>
#include <linux/libfdt.h>
#include <reset.h>
#include <mmc.h>
#include <sdhci.h>

#define MMC_TYPE_MMC  0       /* MMC card */
#define MMC_TYPE_SD   1       /* SD card */
#define MMC_TYPE_SD1  2       /* SD1 card */
#define MMC_TYPE_SDIO 3       /* SDIO card */

#ifdef DEBUG
#define pr_debug(fmt, ...) \
	printf(fmt, ##__VA_ARGS__)
#endif

struct cvi_sdhci_plat {
	struct mmc_config cfg;
	struct mmc mmc;
};

struct cvi_sdhci_host {
	struct sdhci_host host;
	//int pll_index;
	//int pll_reg;
	int no_1_8_v;
	int is_64_addressing;
	int reset_tx_rx_phy;
	u32 mmc_fmax_freq;
	u32 mmc_fmin_freq;
	u8 final_tap;
	struct reset_ctl reset_ctl;
};

struct cvi_sdhci_driver_data {
	const struct sdhci_ops *ops;
	int index;
};

#define G11_IO_BASE     0x28104b00
#define G8_IO_BASE		0x28104800
#define DRIVE_MASK		(0xf << 8)
#define PULL_MASK       (0x3 << 2)
#define PAD_MASK        (0x1 << 13)
#define PULL_UP         (0x1 << 2)
#define PULL_DOWN       (0x2 << 2)
#define DRIVE_VALUE(_n)  ((_n) << 8)
#define IO_MASK         (DRIVE_MASK | PULL_MASK | PAD_MASK)

static void cvi_emmc_pad_setting(void)
{
	/* clk */
	mmio_clrsetbits_32(G11_IO_BASE + 0x1c, IO_MASK, (PAD_MASK | PULL_DOWN | DRIVE_VALUE(4)));
	mmio_clrsetbits_32(G11_IO_BASE + 0x20, IO_MASK, (PAD_MASK | PULL_UP | DRIVE_VALUE(3)));
	mmio_clrsetbits_32(G11_IO_BASE + 0x24, IO_MASK, (PAD_MASK | PULL_UP | DRIVE_VALUE(3)));
	mmio_clrsetbits_32(G11_IO_BASE + 0x28, IO_MASK, (PAD_MASK | PULL_UP | DRIVE_VALUE(3)));
	mmio_clrsetbits_32(G11_IO_BASE + 0x2c, IO_MASK, (PAD_MASK | PULL_UP | DRIVE_VALUE(3)));
	mmio_clrsetbits_32(G11_IO_BASE + 0x30, IO_MASK, (PAD_MASK | PULL_UP | DRIVE_VALUE(3)));
}

static void cvi_sdio2_pad_setting(void)
{
}

static int cvi_ofdata_to_platdata(struct udevice *dev)
{
	struct cvi_sdhci_host *cvi_host = dev_get_priv(dev);
	struct sdhci_host *host = &cvi_host->host;
	int node = dev_of_offset(dev);

	host->name = strdup(dev->name);
	host->ioaddr = (void *)devfdt_get_addr(dev);
	host->bus_width = fdtdec_get_int(gd->fdt_blob, node, "bus-width", 4);
	host->max_clk = fdtdec_get_uint(gd->fdt_blob, node, "src-frequency", 0);

	cvi_host->mmc_fmin_freq = fdtdec_get_uint(gd->fdt_blob, node, "min-frequency", 200000);
	cvi_host->mmc_fmax_freq = fdtdec_get_uint(gd->fdt_blob, node, "max-frequency", 0);
	cvi_host->is_64_addressing = fdtdec_get_bool(gd->fdt_blob, node, "64_addressing");
	cvi_host->reset_tx_rx_phy = fdtdec_get_bool(gd->fdt_blob, node, "reset_tx_rx_phy");
	cvi_host->no_1_8_v = fdtdec_get_bool(gd->fdt_blob, node, "no-1-8-v");

	if (cvi_host->no_1_8_v)
		host->quirks |= SDHCI_QUIRK_NO_1_8_V;

	if (host->ioaddr == (void *)FDT_ADDR_T_NONE)
		return -EINVAL;

	return 0;
}

static int cvi_sdhci_bind(struct udevice *dev)
{
	struct cvi_sdhci_plat *plat = dev_get_plat(dev);

	pr_debug("[hq] %s\n", __func__);
	return sdhci_bind(dev, &plat->mmc, &plat->cfg);
}

#ifdef CONFIG_MMC_SUPPORTS_TUNING
static void cvi_mmc_set_tap(struct sdhci_host *host, u16 tap)
{
	pr_debug("%s %d\n", __func__, tap);
	// Set sd_clk_en(0x2c[2]) to 0
	sdhci_writew(host,
		     sdhci_readw(host, SDHCI_CLOCK_CONTROL) & (~(BIT(2))),
		     SDHCI_CLOCK_CONTROL);
	sdhci_writel(host,
		     sdhci_readl(host, CVI_SDHCI_VENDOR_MSHC_CTRL_R) & (~(BIT(1))),
		     CVI_SDHCI_VENDOR_MSHC_CTRL_R);
	sdhci_writel(host, BIT(8) | tap << 16, CVI_SDHCI_PHY_TX_RX_DLY);
	sdhci_writel(host, 0, CVI_SDHCI_PHY_CONFIG);
	// Set sd_clk_en(0x2c[2]) to 1
	sdhci_writew(host, sdhci_readw(host, SDHCI_CLOCK_CONTROL) | BIT(2), SDHCI_CLOCK_CONTROL);
	mdelay(1);
}

static inline uint32_t CHECK_MASK_BIT(void *_mask, uint32_t bit)
{
	u32 w = bit / 8;
	u32 off = bit % 8;

	return ((uint8_t *)_mask)[w] & (1 << off);
}

static inline void SET_MASK_BIT(void *_mask, uint32_t bit)
{
	u32 byte = bit / 8;
	u32 offset = bit % 8;
	((uint8_t *)_mask)[byte] |= (1 << offset);
}

static void reset_after_tuning_pass(struct sdhci_host *host)
{
	pr_debug("tuning pass\n");

	/* Clear BUF_RD_READY intr */
	sdhci_writew(host, sdhci_readw(host, SDHCI_INT_STATUS) & (~(0x1 << 5)),
		     SDHCI_INT_STATUS);

	/* Set SDHCI_SOFTWARE_RESET.SW_RST_DAT = 1 to clear buffered tuning block */
	sdhci_writeb(host,
		     sdhci_readb(host, SDHCI_SOFTWARE_RESET) | (0x1 << 2), SDHCI_SOFTWARE_RESET);

	/* Set SDHCI_SOFTWARE_RESET.SW_RST_CMD = 1	*/
	sdhci_writeb(host,
		     sdhci_readb(host, SDHCI_SOFTWARE_RESET) | (0x1 << 1),
		     SDHCI_SOFTWARE_RESET);

	while (sdhci_readb(host, SDHCI_SOFTWARE_RESET) & 0x3)
		;
}

static int sdhci_cvi_set_ios_post(struct sdhci_host *host)
{
	struct mmc *mmc = (struct mmc *)host->mmc;
	u32 ctrl;

	if (mmc->signal_voltage == MMC_SIGNAL_VOLTAGE_180) {
		ctrl = sdhci_readw(host, SDHCI_HOST_CONTROL2);
		ctrl |= SDHCI_CTRL_VDD_180;
		sdhci_writew(host, ctrl, SDHCI_HOST_CONTROL2);
	}

	sdhci_set_uhs_timing(host);
	return 0;
}

int cvi_general_execute_tuning(struct mmc *mmc, u8 opcode)
{
	u16 min = 0;
	u32 k = 0;
	s32 ret;
	u32 retry_cnt = 0;

	u32 tuning_result[4] = {0, 0, 0, 0};
	u32 rx_lead_lag_result[4] = {0, 0, 0, 0};
	char tuning_graph[TUNE_MAX_PHCODE + 1];
	char rx_lead_lag_graph[TUNE_MAX_PHCODE + 1];

	u32 reg = 0;
	u32 reg_rx_lead_lag = 0;
	s32 max_lead_lag_idx = -1;
	s32 max_window_idx = -1;
	s32 cur_window_idx = -1;
	u16 max_lead_lag_size = 0;
	u16 max_window_size = 0;
	u16 cur_window_size = 0;
	s32 rx_lead_lag_phase = -1;
	s32 final_tap = -1;
	u32 rate = 0;

	struct cvi_sdhci_host *cvi_host = dev_get_priv(mmc->dev);
	struct sdhci_host *host = &cvi_host->host;

	u32 norm_stat_en_b, err_stat_en_b;
	u32 norm_signal_en_b, ctl2;

	norm_stat_en_b = sdhci_readw(host, SDHCI_INT_ENABLE);
	err_stat_en_b = sdhci_readw(host, SDHCI_ERR_INT_STATUS_EN);
	norm_signal_en_b = sdhci_readl(host, SDHCI_SIGNAL_ENABLE);

	reg = sdhci_readw(host, SDHCI_ERR_INT_STATUS);
	pr_debug("mmc%d : SDHCI_ERR_INT_STATUS 0x%x\n", host->index, reg);

	reg = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	pr_debug("mmc%d : host ctrl2 0x%x\n", host->index, reg);
	/* Set Host_CTRL2_R.SAMPLE_CLK_SEL=0 */
	sdhci_writew(host,
		     sdhci_readw(host, SDHCI_HOST_CONTROL2) & (~(0x1 << 7)),
		     SDHCI_HOST_CONTROL2);
	sdhci_writew(host,
		     sdhci_readw(host, SDHCI_HOST_CONTROL2) & (~(0x3 << 4)),
		     SDHCI_HOST_CONTROL2);

	reg = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	pr_debug("mmc%d : host ctrl2 0x%x\n", host->index, reg);

	while (min < TUNE_MAX_PHCODE) {
		retry_cnt = 0;
		cvi_mmc_set_tap(host, min);
		reg_rx_lead_lag = sdhci_readw(host, CVI_SDHCI_PHY_DLY_STS) & BIT(1);

retry_tuning:
		ret = mmc_send_tuning(host->mmc, opcode, NULL);

		if (!ret && retry_cnt < MAX_TUNING_CMD_RETRY_COUNT) {
			retry_cnt++;
			goto retry_tuning;
		}

		if (ret)
			SET_MASK_BIT(tuning_result, min);

		if (reg_rx_lead_lag)
			SET_MASK_BIT(rx_lead_lag_result, min);

		min++;
	}

	reset_after_tuning_pass(host);

	pr_debug("mmc%d : tuning result:	  0x%08x 0x%08x 0x%08x 0x%08x\n", host->index,
		 tuning_result[0], tuning_result[1],
		 tuning_result[2], tuning_result[3]);
	pr_debug("mmc%d : rx_lead_lag result: 0x%08x 0x%08x 0x%08x 0x%08x\n", host->index,
		 rx_lead_lag_result[0], rx_lead_lag_result[1],
		 rx_lead_lag_result[2], rx_lead_lag_result[3]);
	for (k = 0; k < TUNE_MAX_PHCODE; k++) {
		if (CHECK_MASK_BIT(tuning_result, k) == 0)
			tuning_graph[k] = '-';
		else
			tuning_graph[k] = 'x';
		if (CHECK_MASK_BIT(rx_lead_lag_result, k) == 0)
			rx_lead_lag_graph[k] = '0';
		else
			rx_lead_lag_graph[k] = '1';
	}
	tuning_graph[TUNE_MAX_PHCODE] = '\0';
	rx_lead_lag_graph[TUNE_MAX_PHCODE] = '\0';

	pr_debug("mmc%d : tuning graph:      %s\n", host->index, tuning_graph);
	pr_debug("mmc%d : rx_lead_lag graph: %s\n", host->index, rx_lead_lag_graph);

	// Find a final tap as median of maximum window.
	for (k = 0; k < TUNE_MAX_PHCODE; k++) {
		if (CHECK_MASK_BIT(tuning_result, k) == 0) {
			if (-1 == cur_window_idx)
				cur_window_idx = k;

			cur_window_size++;

			if (cur_window_size > max_window_size) {
				max_window_size = cur_window_size;
				max_window_idx = cur_window_idx;
				if (max_window_size >= TAP_WINDOW_THLD)
					final_tap = cur_window_idx + (max_window_size / 2);
			}
		} else {
			cur_window_idx = -1;
			cur_window_size = 0;
		}
	}

	cur_window_idx = -1;
	cur_window_size = 0;
	for (k = 0; k < TUNE_MAX_PHCODE; k++) {
		if (CHECK_MASK_BIT(rx_lead_lag_result, k) == 0) {
			//from 1 to 0 and window_size already computed.
			if (rx_lead_lag_phase == 1 && cur_window_size > 0) {
				max_lead_lag_idx = cur_window_idx;
				max_lead_lag_size = cur_window_size;
				break;
			}
			if (cur_window_idx == -1)
				cur_window_idx = k;

			cur_window_size++;
			rx_lead_lag_phase = 0;
		} else {
			rx_lead_lag_phase = 1;
			if (cur_window_idx != -1 && cur_window_size > 0) {
				cur_window_size++;
				max_lead_lag_idx = cur_window_idx;
				max_lead_lag_size = cur_window_size;
			} else {
				cur_window_size = 0;
			}
		}
	}
	rate = max_window_size * 100 / max_lead_lag_size;
	pr_debug("mmc%d : MaxWindow[Idx, Width]:[%d,%u]\n",
		 host->index, max_window_idx, max_window_size);
	pr_debug("mmc%d : Tuning Tap: %d\n", host->index, final_tap);
	pr_debug("mmc%d : RX_LeadLag[Idx, Width]:[%d,%u]\n",
		 host->index, max_lead_lag_idx, max_lead_lag_size);
	pr_debug("mmc%d : rate = %d\n", host->index, rate);

	cvi_mmc_set_tap(host, final_tap);
	cvi_host->final_tap = final_tap;
	ret = mmc_send_tuning(host->mmc, opcode, NULL);
	printf("mmc%d : finished tuning, code:%d\n", host->index, final_tap);

	ctl2 = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	ctl2 &= ~SDHCI_CTRL_EXEC_TUNING;
	sdhci_writew(host, ctl2, SDHCI_HOST_CONTROL2);

	sdhci_writew(host, norm_stat_en_b, SDHCI_INT_ENABLE);
	sdhci_writel(host, norm_signal_en_b, SDHCI_SIGNAL_ENABLE);
	sdhci_writew(host, err_stat_en_b, SDHCI_ERR_INT_STATUS_EN);

	return ret;
}
#endif

static void cvi_general_reset(struct sdhci_host *host, u8 mask)
{
	u16 ctrl_2;
	struct cvi_sdhci_host *cvi_host = container_of(host, struct cvi_sdhci_host, host);

	if (host->index == MMC_TYPE_MMC) {
		//reg_0x200[0] = 1 for mmc
		sdhci_writel(host,
			     sdhci_readl(host, CVI_SDHCI_VENDOR_MSHC_CTRL_R) | BIT(0),
			     CVI_SDHCI_VENDOR_MSHC_CTRL_R);
	}

	ctrl_2 = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	pr_debug("%s-%d MMC%d : ctrl_2 = 0x%04x\n", __func__, __LINE__, host->index, ctrl_2);

	ctrl_2 &= SDHCI_CTRL_UHS_MASK;
	if (ctrl_2 == SDHCI_CTRL_UHS_SDR104) {
		//reg_0x200[1] = 0
		sdhci_writel(host,
			     sdhci_readl(host, CVI_SDHCI_VENDOR_MSHC_CTRL_R) & ~(BIT(1)),
			     CVI_SDHCI_VENDOR_MSHC_CTRL_R);
		if (host->index == MMC_TYPE_SDIO) {
			//reg_0x200[16] = 1 for sd1
			sdhci_writel(host,
				     sdhci_readl(host, CVI_SDHCI_VENDOR_MSHC_CTRL_R) | BIT(16),
				     CVI_SDHCI_VENDOR_MSHC_CTRL_R);
		}
		//reg_0x24c[0] = 0
		sdhci_writel(host,
			     sdhci_readl(host, CVI_SDHCI_PHY_CONFIG) & ~(BIT(0)),
			     CVI_SDHCI_PHY_CONFIG);
		//reg_0x240[22:16] = tap reg_0x240[9:8] = 1 reg_0x240[6:0] = 0
		sdhci_writel(host, (BIT(8) | ((cvi_host->final_tap & 0x7F) << 16)), CVI_SDHCI_PHY_TX_RX_DLY);
	} else {
		//Reset as DS/HS setting.
		//reg_0x200[1] = 1
		sdhci_writel(host,
			     sdhci_readl(host, CVI_SDHCI_VENDOR_MSHC_CTRL_R) | BIT(1),
			     CVI_SDHCI_VENDOR_MSHC_CTRL_R);
		if (host->index == MMC_TYPE_SDIO) {
			//reg_0x200[16] = 1 for sd1
			sdhci_writel(host,
				     sdhci_readl(host, CVI_SDHCI_VENDOR_MSHC_CTRL_R) | BIT(16),
				     CVI_SDHCI_VENDOR_MSHC_CTRL_R);
		}
		//reg_0x24c[0] = 1
		sdhci_writel(host,
			     sdhci_readl(host, CVI_SDHCI_PHY_CONFIG) | BIT(0),
			     CVI_SDHCI_PHY_CONFIG);
		//reg_0x240[25:24] = 1 reg_0x240[22:16] = 0 reg_0x240[9:8] = 1 reg_0x240[6:0] = 0
		sdhci_writel(host, 0x1000100, CVI_SDHCI_PHY_TX_RX_DLY);
	}
}

#ifdef CONFIG_MMC_UHS_SUPPORT
static void cvi_sd_voltage_switch(struct mmc *mmc)
{
	struct sdhci_host *host = mmc->priv;

	if (host->index == MMC_TYPE_SD) {
#if CONFIG_IS_ENABLED(DM_MMC) && CONFIG_IS_ENABLED(DM_GPIO)
		if (dm_gpio_is_valid(&host->pwr_gpio))
			dm_gpio_set_value(&host->pwr_gpio, 1);

#endif
		udelay(1000);
		/* set ms to 1 */
		mmio_clrsetbits_32(0x281051e0, 1 << 6, 1 << 6);
	} else if (host->index == MMC_TYPE_SD1) {
#if CONFIG_IS_ENABLED(DM_MMC) && CONFIG_IS_ENABLED(DM_GPIO)
		if (dm_gpio_is_valid(&host->pwr_gpio))
			dm_gpio_set_value(&host->pwr_gpio, 1);

#endif
		udelay(1000);
		/* set ms to 1 */
		mmio_clrsetbits_32(0x281051e0, 1 << 4, 1 << 4);
	}
}
#endif

int cvi_get_cd(struct sdhci_host *host)
{
	u32 reg;

	reg = sdhci_readl(host, SDHCI_PRESENT_STATE);
	pr_debug("%s reg = 0x08%x\n", __func__, reg);
	if (reg & SDHCI_CARD_PRESENT)
		return 1;
	else
		return 0;
}

void sd0_pad_setup(void)
{
	/* SD0_CMD pull down */
	mmio_clrsetbits_32(G8_IO_BASE + 0x4, PULL_MASK, PULL_DOWN);
	/* SD0_SD0 pull down */
	mmio_clrsetbits_32(G8_IO_BASE + 0x8, PULL_MASK, PULL_DOWN);
	mmio_clrsetbits_32(G8_IO_BASE + 0xc, PULL_MASK, PULL_DOWN);
	mmio_clrsetbits_32(G8_IO_BASE + 0x10, PULL_MASK, PULL_DOWN);
	mmio_clrsetbits_32(G8_IO_BASE + 0x14, PULL_MASK, PULL_DOWN);
	mdelay(2);
	/* clk always pull down, set driving */
	mmio_clrsetbits_32(G8_IO_BASE + 0x0, IO_MASK, (PULL_DOWN | PAD_MASK | DRIVE_VALUE(4)));
	/* set driving */
	mmio_clrsetbits_32(G8_IO_BASE + 0x4, IO_MASK, (PULL_UP | PAD_MASK | DRIVE_VALUE(4)));
	mmio_clrsetbits_32(G8_IO_BASE + 0x8, IO_MASK, (PULL_UP | PAD_MASK | DRIVE_VALUE(4)));
	mmio_clrsetbits_32(G8_IO_BASE + 0xc, IO_MASK, (PULL_UP | PAD_MASK | DRIVE_VALUE(4)));
	mmio_clrsetbits_32(G8_IO_BASE + 0x10, IO_MASK, (PULL_UP | PAD_MASK | DRIVE_VALUE(4)));
	mmio_clrsetbits_32(G8_IO_BASE + 0x14, IO_MASK, (PULL_UP | PAD_MASK | DRIVE_VALUE(4)));
}

void sd1_pad_setup(void)
{
	/* SD1_CMD pull down */
	mmio_clrsetbits_32(G11_IO_BASE + 0x38, PULL_MASK, PULL_DOWN);
	/* SD1_SD0 pull down */
	mmio_clrsetbits_32(G11_IO_BASE + 0x3c, PULL_MASK, PULL_DOWN);
	mmio_clrsetbits_32(G11_IO_BASE + 0x40, PULL_MASK, PULL_DOWN);
	mmio_clrsetbits_32(G11_IO_BASE + 0x44, PULL_MASK, PULL_DOWN);
	mmio_clrsetbits_32(G11_IO_BASE + 0x48, PULL_MASK, PULL_DOWN);
	mdelay(2);
	/* clk always pull down, set driving */
	mmio_clrsetbits_32(G11_IO_BASE + 0x34, IO_MASK, (PULL_DOWN | PAD_MASK | DRIVE_VALUE(4)));
	/* set driving */
	mmio_clrsetbits_32(G11_IO_BASE + 0x38, IO_MASK, (PULL_UP | PAD_MASK | DRIVE_VALUE(4)));
	mmio_clrsetbits_32(G11_IO_BASE + 0x3c, IO_MASK, (PULL_UP | PAD_MASK | DRIVE_VALUE(4)));
	mmio_clrsetbits_32(G11_IO_BASE + 0x40, IO_MASK, (PULL_UP | PAD_MASK | DRIVE_VALUE(4)));
	mmio_clrsetbits_32(G11_IO_BASE + 0x44, IO_MASK, (PULL_UP | PAD_MASK | DRIVE_VALUE(4)));
	mmio_clrsetbits_32(G11_IO_BASE + 0x48, IO_MASK, (PULL_UP | PAD_MASK | DRIVE_VALUE(4)));
}

static int cvi_sdhci_probe(struct udevice *dev)
{
	struct cvi_sdhci_driver_data *drv_data =
				(struct cvi_sdhci_driver_data *)dev_get_driver_data(dev);
	struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
	struct cvi_sdhci_plat *plat = dev_get_plat(dev);
	struct cvi_sdhci_host *cvi_host = dev_get_priv(dev);
	struct sdhci_host *host = &cvi_host->host;
	int ret;

	printf("%s-%d: mmc%d probe\n", __func__, __LINE__, host->index);
	ret = reset_get_by_name(dev, "sdhci", &cvi_host->reset_ctl);
	if (ret) {
		pr_debug("warning: reset_get_by_name failed\n");
	} else {
		// Try to solve 1.8 to 3.3v converter HW issue
		ret = reset_assert(&cvi_host->reset_ctl);
		if (ret) {
			printf("%s failed assert reset\n", __func__);
			return ret;
		}

		ret = reset_deassert(&cvi_host->reset_ctl);
		if (ret) {
			printf("%s failed deassert reset\n", __func__);
			return ret;
		}
	}

	upriv->mmc = &plat->mmc;
	host->mmc = &plat->mmc;
	host->mmc->priv = host;
	host->mmc->dev = dev;
	host->ops = drv_data->ops;
	host->index = drv_data->index;
	pr_debug("host %p, mmc %p, priv %p\n", host, host->mmc, host->mmc->priv);

	ret = sdhci_setup_cfg(&plat->cfg, host, cvi_host->mmc_fmax_freq, cvi_host->mmc_fmin_freq);

	if (ret)
		return ret;

	if (host->index == MMC_TYPE_MMC) {
		cvi_emmc_pad_setting();
		sdhci_writel(host, sdhci_readl(host, CVI_SDHCI_VENDOR_MSHC_CTRL_R) | BIT(0),
			     CVI_SDHCI_VENDOR_MSHC_CTRL_R);
	} else if (host->index == MMC_TYPE_SD) {
		sd0_pad_setup();

	} else if (host->index == MMC_TYPE_SD1) {
		sd1_pad_setup();

	} else if (host->index == MMC_TYPE_SDIO) {
		cvi_sdio2_pad_setting();
	} else {
		printf("wrong host index : %d !!\n", host->index);
		return -ENXIO;
	}

	ret = sdhci_probe(dev);
	if (cvi_host->is_64_addressing) {
		sdhci_writew(host, sdhci_readw(host, SDHCI_HOST_CONTROL2)
				| SDHCI_HOST_VER4_ENABLE | SDHCI_HOST_ADDRESSING,
				SDHCI_HOST_CONTROL2);
	}

	if (cvi_host->reset_tx_rx_phy) {
		/* Default value */
		sdhci_writel(host,
			     sdhci_readl(host, CVI_SDHCI_VENDOR_MSHC_CTRL_R)
			     | BIT(1) | BIT(8) | BIT(9),
			     CVI_SDHCI_VENDOR_MSHC_CTRL_R);
		sdhci_writel(host, 0x01000100, CVI_SDHCI_PHY_TX_RX_DLY);
		sdhci_writel(host, 00000001, SDHCI_PHY_CONFIG);
	}

	return ret;
}

const struct sdhci_ops cvi_sdhci_emmc_ops = {
#ifdef CONFIG_MMC_SUPPORTS_TUNING
	.platform_execute_tuning = cvi_general_execute_tuning,
#endif
	.reset = cvi_general_reset,
	.set_ios_post = sdhci_cvi_set_ios_post,
};

const struct sdhci_ops cvi_sdhci_sd_ops = {
	.get_cd = cvi_get_cd,
#ifdef CONFIG_MMC_SUPPORTS_TUNING
	.platform_execute_tuning = cvi_general_execute_tuning,
#endif
#ifdef CONFIG_MMC_UHS_SUPPORT
	.voltage_switch = cvi_sd_voltage_switch,
#endif
	.reset = cvi_general_reset,
	.set_ios_post = sdhci_cvi_set_ios_post,
};

static const struct cvi_sdhci_driver_data sdhci_cvi_emmc_drvdata = {
	.ops = &cvi_sdhci_emmc_ops,
	.index = MMC_TYPE_MMC,
};

static const struct cvi_sdhci_driver_data sdhci_cvi_sd_drvdata = {
	.ops = &cvi_sdhci_sd_ops,
	.index = MMC_TYPE_SD,
};

static const struct cvi_sdhci_driver_data sdhci_cvi_sd1_drvdata = {
	.ops = &cvi_sdhci_sd_ops,
	.index = MMC_TYPE_SD1,
};

static const struct cvi_sdhci_driver_data sdhci_cvi_sdio_drvdata = {
	.ops = &cvi_sdhci_sd_ops,
	.index = MMC_TYPE_SDIO,
};

static const struct udevice_id cvi_sdhci_match[] = {
	{
		.compatible = "cvitek,cv186x-emmc",
		.data = (ulong)&sdhci_cvi_emmc_drvdata,
	},
	{
		.compatible = "cvitek,cv186x-sd",
		.data = (ulong)&sdhci_cvi_sd_drvdata,
	},
	{
		.compatible = "cvitek,cv186x-sd1",
		.data = (ulong)&sdhci_cvi_sd1_drvdata,
	},
	{
		.compatible = "cvitek,cv186x-sdio",
		.data = (ulong)&sdhci_cvi_sdio_drvdata,
	},
	{ }
};

U_BOOT_DRIVER(cvi_sdhci) = {
	.name = "cvi_sdhci",
	.id = UCLASS_MMC,
	.of_match = cvi_sdhci_match,
	.of_to_plat = cvi_ofdata_to_platdata,
	.bind = cvi_sdhci_bind,
	.probe = cvi_sdhci_probe,
	.priv_auto = sizeof(struct cvi_sdhci_host),
	.plat_auto = sizeof(struct cvi_sdhci_plat),
	.ops = &sdhci_ops,
};
