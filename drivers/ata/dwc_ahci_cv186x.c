// SPDX-License-Identifier: GPL-2.0+
/*
 * DWC SATA platform driver
 *
 * (C) Copyright 2016
 *     Texas Instruments Incorporated, <www.ti.com>
 *
 * Author: Mugunthan V N <mugunthanvnm@ti.com>
 */

#include <common.h>
#include <dm.h>
#include <ahci.h>
#include <scsi.h>
#include <sata.h>
#include <asm/io.h>
#include <generic-phy.h>
#include <linux/io.h>
#include <linux/delay.h>

#define writel_with_flush(a, b)	do { writel(a, b); readl(b); } while (0)

struct dwc_ahci_priv {
	void *base;
	void *wrapper_base;
};

#define SSPERI_TOP_REG_BASE     0x20BE0000
#define SSPERI_AXI_BASE         0x20000000

static int sophgo_phy_init(void)
{
	uint32_t reg;
	int timeout = 100;

	reg = readl(SSPERI_AXI_BASE + 0x40);
	reg &= ~(0x1 << 2);
	writel_with_flush(reg, SSPERI_AXI_BASE + 0x40);

	/* set pipe for sata and pcie both enable */
	reg = readl(SSPERI_TOP_REG_BASE + 0x44);
	/* pipe mode set for sata */
	reg &= ~(0x3);
	reg |= (0x2);
	writel_with_flush(reg, SSPERI_TOP_REG_BASE + 0x44);

	/* set phy protocol for sata operation */
	reg = readl(SSPERI_TOP_REG_BASE + 0x278);
	reg &= ~(0x3);
	reg |= 0x2;
	writel_with_flush(reg, SSPERI_TOP_REG_BASE + 0x278);

	/* phy lane0 rx_term_acdc control bit */
	reg = readl(SSPERI_TOP_REG_BASE + 0x48);
	reg &= ~(0x3);
	reg |= 0x2;
	writel_with_flush(reg, SSPERI_TOP_REG_BASE + 0x48);

	/* phy vph for 1.8v */
	reg = readl(SSPERI_TOP_REG_BASE + 0x34);
	reg &= ~(0x7);
	reg |= 0x7;
	writel_with_flush(reg, SSPERI_TOP_REG_BASE + 0x34);

	/* phy0 sram_ld_done 0
	 * phy0 sram_bypass 0
	 * phy0 refclk for use_pad
	 * phy0 ref_repeat_clk
	 */
	reg = readl(SSPERI_TOP_REG_BASE + 0x4b0);
	reg &= ~(0xf << 24);
	reg &= ~(0x1 << 23);
	reg |= (0x2 << 24);
	reg |= (0x1 << 23);
	writel_with_flush(reg, SSPERI_TOP_REG_BASE + 0x4b0);

	/* set phy0 pwr_en/stable */
	reg = readl(SSPERI_TOP_REG_BASE + 0x4b8);
	reg |= (0xf << 15);
	writel_with_flush(reg, SSPERI_TOP_REG_BASE + 0x4b8);

	reg = readl(SSPERI_TOP_REG_BASE + 0x4a4);
	reg |= (0x1 << 10);
	writel_with_flush(reg, SSPERI_TOP_REG_BASE + 0x4a4);

	udelay(10);
	/* release phy_reset */
	reg = readl(SSPERI_TOP_REG_BASE + 0x34);
	reg |= 0x1 << 28;
	writel_with_flush(reg, SSPERI_TOP_REG_BASE + 0x34);

	while (!(readl(SSPERI_TOP_REG_BASE + 0x4b8) & 0x20)) {
		if (timeout-- > 0) {
			mdelay(1);
		} else {
			printf("phy sram init timeout");
			return -1;
		}
	}
	reg = readl(SSPERI_TOP_REG_BASE + 0x4b0);
	reg |= 0x1 << 27;
	writel_with_flush(reg, SSPERI_TOP_REG_BASE + 0x4b0);

	return 0;
}

static int dwc_ahci_bind(struct udevice *dev)
{
	struct udevice *scsi_dev;

	return ahci_bind_scsi(dev, &scsi_dev);
}

static int dwc_ahci_of_to_plat(struct udevice *dev)
{
	struct dwc_ahci_priv *priv = dev_get_priv(dev);
	fdt_addr_t addr;

	priv->base = map_physmem(dev_read_addr(dev), sizeof(void *),
				 MAP_NOCACHE);

	addr = devfdt_get_addr_index(dev, 1);
	if (addr != FDT_ADDR_T_NONE) {
		priv->wrapper_base = map_physmem(addr, sizeof(void *),
						 MAP_NOCACHE);
	} else {
		priv->wrapper_base = NULL;
	}

	return 0;
}

static int dwc_ahci_cv186x_probe(struct udevice *dev)
{
	struct dwc_ahci_priv *priv = dev_get_priv(dev);
	int ret;

	printf("%s:\n", __func__);
	ret = sophgo_phy_init();
	if (ret) {
		printf("unable to initialize the sata phy\n");
		return ret;
	}

	return ahci_probe_scsi(dev, (ulong)priv->base);
}

static const struct udevice_id dwc_ahci_ids[] = {
	{ .compatible = "snps,dwc-ahci" },
	{ }
};

U_BOOT_DRIVER(dwc_ahci) = {
	.name	= "dwc_ahci_cv186x",
	.id	= UCLASS_AHCI,
	.of_match = dwc_ahci_ids,
	.bind	= dwc_ahci_bind,
	.of_to_plat = dwc_ahci_of_to_plat,
	.ops	= &scsi_ops,
	.probe	= dwc_ahci_cv186x_probe,
	.priv_auto	= sizeof(struct dwc_ahci_priv),
};
