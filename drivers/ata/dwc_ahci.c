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
#include <power/regulator.h>

struct dwc_ahci_priv {
	void *base;
	void *wrapper_base;
	struct udevice *vpcie3v3;
};

static int dwc_ahci_bind(struct udevice *dev)
{
	struct udevice *scsi_dev;

	return ahci_bind_scsi(dev, &scsi_dev);
}

static int dwc_ahci_ofdata_to_platdata(struct udevice *dev)
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

static int dwc_ahci_probe(struct udevice *dev)
{
	struct dwc_ahci_priv *priv = dev_get_priv(dev);
	int ret;
	struct phy phy;

	ret = generic_phy_get_by_name(dev, "sata-phy", &phy);
	if (ret) {
		pr_err("can't get the phy from DT\n");
		return ret;
	}

	ret = generic_phy_init(&phy);
	if (ret) {
		pr_err("unable to initialize the sata phy\n");
		return ret;
	}

	ret = generic_phy_power_on(&phy);
	if (ret) {
		pr_err("unable to power on the sata phy\n");
		return ret;
	}

	ret = device_get_supply_regulator(dev, "vpcie3v3-supply",
	                                   &priv->vpcie3v3);
	if (ret) {
		pr_err("failed to get vpcie3v3 supply (ret=%d)\n", ret);
		//return ret;
	}

	if (priv->vpcie3v3) {
		ret = regulator_set_enable(priv->vpcie3v3, true);
		if (ret) {
			dev_err(priv->dev, "failed to enable vpcie3v3 (ret=%d)\n", ret);
			//return ret;
		}
	}

	return ahci_probe_scsi(dev, (ulong)priv->base);
}

static const struct udevice_id dwc_ahci_ids[] = {
	{ .compatible = "snps,dwc-ahci" },
	{ }
};

U_BOOT_DRIVER(dwc_ahci) = {
	.name	= "dwc_ahci",
	.id	= UCLASS_AHCI,
	.of_match = dwc_ahci_ids,
	.bind	= dwc_ahci_bind,
	.ofdata_to_platdata = dwc_ahci_ofdata_to_platdata,
	.ops	= &scsi_ops,
	.probe	= dwc_ahci_probe,
	.priv_auto_alloc_size	= sizeof(struct dwc_ahci_priv),
};
