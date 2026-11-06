// SPDX-License-Identifier: GPL-2.0+
/*
 * SiFive FU740 DesignWare PCIe Controller
 *
 * Copyright (C) 2020-2021 SiFive, Inc.
 *
 * Based in early part on the i.MX6 PCIe host controller shim which is:
 *
 * Copyright (C) 2013 Kosagi
 *		http://www.kosagi.com
 *
 * Based on driver from author: Alan Mikhak <amikhak@wirelessfabric.com>
 */
#include <asm/io.h>
#include <asm-generic/gpio.h>
#include <clk.h>
#include <common.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <generic-phy.h>
#include <linux/bitops.h>
#include <linux/log2.h>
#include <pci.h>
#include <pci_ep.h>
#include <pci_ids.h>
#include <regmap.h>
#include <reset.h>
#include <syscon.h>

#include "pcie_dw_common.h"

struct pcie_cv186x {
	/* Must be first member of the struct */
	struct pcie_dw dw;
	u32 index;

	/* private control regs */
	void __iomem *apb_base;
	void __iomem *sii_base;
	void __iomem *top_apb_base;

	struct gpio_desc reset_gpio;
	struct clk aux_ck;
};

enum pcie_cv186x_devtype {
	CV186X_PCIE_UNKNOWN_TYPE = 0,
	CV186X_PCIE_ENDPOINT_TYPE = 1,
	CV186X_PCIE_HOST_TYPE = 3
};

#define cv_pcie_info(pcie, fmt, arg...)	printf(fmt, ## arg)
#define cv_pcie_err(pcie, fmt, arg...)	printf(fmt, ## arg)

#define PCIE_MISC_CONTROL_1		0x8bc
#define DBI_RO_WR_EN			BIT(0)

#define PCIE_CV186X_CTRL0_DBI (0x20800000)

#define DBI_OFFSET			0x0
#define PCIE_CAP_BASE			0x70
#define PCI_CONFIG(r)			(DBI_OFFSET + (r))
#define PCIE_CAPABILITIES(r)		PCI_CONFIG(PCIE_CAP_BASE + (r))

/* Link capability */
#define PF0_PCIE_CAP_LINK_CAP		PCIE_CAPABILITIES(0xc)
#define PCIE_LINK_CAP_MAX_SPEED_MASK	0xf
#define PCIE_LINK_CAP_MAX_SPEED_GEN1	BIT(0)
#define PCIE_LINK_CAP_MAX_SPEED_GEN2	BIT(1)
#define PCIE_LINK_CAP_MAX_SPEED_GEN3	BIT(2)
#define PCIE_LINK_CAP_MAX_SPEED_GEN4	BIT(3)

static void pcie_enable_ltssm(struct pcie_cv186x *pcie, int state)
{
	u32 val;

	val = readl(pcie->sii_base + 0x58);
	writel(val | 0x1, pcie->sii_base + 0x58);
}

#define PCIE_SMLH_LINKUP_BIT BIT(6)
#define PCIE_RDLH_LINKUP_BIT BIT(7)

static void pcie_cv186x_clrbits(void __iomem *addr, u32 mask)
{
	u32 val;

	val = readl(addr) & (~mask);
	writel(val, addr);
}

static void pcie_cv186x_setbits(void __iomem *addr, u32 mask)
{
	u32 val;

	val = readl(addr);
	writel(val | mask, addr);
}

static int pcie_cv186x_check_link(struct pcie_cv186x *pcie)
{
	u32 val;

	val = readl(pcie->sii_base + 0xb4);
	return (val & PCIE_SMLH_LINKUP_BIT) &&
		(val & PCIE_RDLH_LINKUP_BIT);
}

static void pcie_cv186x_force_gen1(struct pcie_cv186x *pcie)
{
	u32 val, linkcap;

	/*
	 * Force Gen1 operation when starting the link. In case the link is
	 * started in Gen2 mode, there is a possibility the devices on the
	 * bus will not be detected at all. This happens with PCIe switches.
	 */

	/* ctrl_ro_wr_enable */
	val = readl(pcie->dw.dbi_base + PCIE_MISC_CONTROL_1);
	val |= DBI_RO_WR_EN;
	writel(val, pcie->dw.dbi_base + PCIE_MISC_CONTROL_1);

	/* configure link cap */
	linkcap = readl(pcie->dw.dbi_base + PF0_PCIE_CAP_LINK_CAP);
	linkcap |= PCIE_LINK_CAP_MAX_SPEED_MASK;
	writel(linkcap, pcie->dw.dbi_base + PF0_PCIE_CAP_LINK_CAP);

	/* ctrl_ro_wr_disable */
	val &= ~DBI_RO_WR_EN;
	writel(val, pcie->dw.dbi_base + PCIE_MISC_CONTROL_1);
}

static int pcie_cv186x_wait_for_link(struct pcie_cv186x *pcie)
{
	u32 val;
	int timeout;

	/* Wait for the link to train */
	mdelay(20);
	timeout = 20;

	do {
		mdelay(1);
	} while (--timeout && !pcie_cv186x_check_link(pcie));

	val = readl(pcie->sii_base + 0xb4);
	if (!(val & PCIE_SMLH_LINKUP_BIT) ||
	    !(val & PCIE_RDLH_LINKUP_BIT)) {
		cv_pcie_info(pcie, "Failed to negotiate PCIe link!\n");
		return -ETIMEDOUT;
	}

	return 0;
}

static int pcie_cv186x_start_link(struct pcie_cv186x *pcie)
{
	if (pcie_cv186x_check_link(pcie))
		return -EALREADY;

	pcie_cv186x_force_gen1(pcie);

	/* set ltssm */
	pcie_enable_ltssm(pcie, 1);
	return 0;
}

static void pcie_cv186x_phy_pwr_stable(struct pcie_cv186x *pcie)
{
	if (pcie->index == 1) {
		pcie_cv186x_setbits(pcie->top_apb_base + 0x4a4, 0x1 << 11);
		pcie_cv186x_setbits(pcie->top_apb_base + 0x4b8, 0x3 << 22);
		pcie_cv186x_setbits(pcie->top_apb_base + 0x4a4, 0x1 << 1);
	} else {
		pcie_cv186x_setbits(pcie->top_apb_base + 0x4a4, 0x1 << 10);
		pcie_cv186x_setbits(pcie->top_apb_base + 0x4b8, 0x3 << 15);
		pcie_cv186x_setbits(pcie->top_apb_base + 0x4a4, 0x1);
	}
}

static void pcie_cv186x_remote_config(struct pcie_cv186x *pcie)
{
	if (pcie->index == 1) {
		writel(0x80000000, pcie->top_apb_base + 0x0c);
		writel(0xBFFFFFFF, pcie->top_apb_base + 0x10);
	} else {
		writel(0xC0000000, pcie->top_apb_base + 0x18);
		writel(0xFFFFCFFF, pcie->top_apb_base + 0x1c);
	}
}

static void pcie_cv186x_sram_init(struct pcie_cv186x *pcie)
{
	if (pcie->index == 1) {
		while (!(readl(pcie->top_apb_base + 0x4b8) & 0x800)) {
		//TODO: timeout break
		}
		pcie_cv186x_setbits(pcie->top_apb_base + 0x4b4, 0x1 << 5);
	} else {
		while (!(readl(pcie->top_apb_base + 0x4b8) & 0x20)) {
		//TODO: timeout break
		}
		pcie_cv186x_setbits(pcie->top_apb_base + 0x4b0, 0x1 << 27);
	}
}

static void pcie_cv186x_mode_select(struct pcie_cv186x *pcie)
{
	if (pcie->index == 0) {
		pcie_cv186x_clrbits(pcie->top_apb_base + 0x44, 0x3);
	}
}

static int pcie_cv186x_init_port(struct udevice *dev,
				 enum pcie_cv186x_devtype mode)
{
	struct pcie_cv186x *pcie = dev_get_priv(dev);
	//int ret;
	u32 val;

	/* enable pcieauxclk */
	//clk_enable(&pcie->aux_ck);

	if (dm_gpio_is_valid(&pcie->reset_gpio)) {
		dm_gpio_set_value(&pcie->reset_gpio, 0);
		/* 300ms */
		udelay(300 * 1000);
		dm_gpio_set_value(&pcie->reset_gpio, 1);
	}

	pcie_cv186x_mode_select(pcie);

	val = readl(pcie->top_apb_base + 0x34);

	//val &= ~(0xF << 22);
	//writel(val, pcie->top_apb_base + 0x34);
	//mdelay(100);
	//val |= 0xA << 22;
	//writel(val, pcie->top_apb_base + 0x34);

	writel(val | (0x1 << 2), pcie->top_apb_base + 0x34);

	pcie_cv186x_phy_pwr_stable(pcie);

	// assert phy reset
	writel(readl(pcie->apb_base) & (~0x4), pcie->apb_base);

	// ctrl cold reset
	writel(readl(pcie->apb_base) & (~0x1), pcie->apb_base);
	writel(readl(pcie->apb_base) | 0x1, pcie->apb_base);

	// config device type
#define CV186X_PCIE_DM_MASK (0xf << 9)
#define CV186X_PCIE_DM_RC   (0x4 << 9)
	val = readl(pcie->sii_base + 0x50) & (~CV186X_PCIE_DM_MASK);
	writel(val | CV186X_PCIE_DM_RC, pcie->sii_base + 0x50);

	pcie_cv186x_remote_config(pcie);

	writel(readl(pcie->sii_base + 0x60) & (0x1 << 29), pcie->sii_base + 0x60);

	// deassert phy reset
	writel(readl(pcie->apb_base) | 0x4, pcie->apb_base);

	pcie_cv186x_sram_init(pcie);

	while (!(readl(pcie->sii_base + 0x5c) & 0x100)) {
		//TODO: timeout break
	}
	while (readl(pcie->dw.dbi_base + 0x258) & 0xffff0000) {
		//TODO: timeout break
	}

	pcie_dw_setup_host(&pcie->dw);

	if (pcie_cv186x_start_link(pcie) == -EALREADY)
		cv_pcie_info(pcie, "PCIe link is already up\n");

	if (pcie_cv186x_wait_for_link(pcie) == -ETIMEDOUT)
		return -ETIMEDOUT;

	return 0;
}

static int pcie_cv186x_probe(struct udevice *dev)
{
	struct pcie_cv186x *pcie = dev_get_priv(dev);
	struct udevice *parent = pci_get_controller(dev);
	struct pci_controller *hose = dev_get_uclass_priv(parent);
	int err;

	pcie->dw.first_busno = dev_seq(dev);
	pcie->dw.dev = dev;

	err = pcie_cv186x_init_port(dev, CV186X_PCIE_HOST_TYPE);
	if (err) {
		cv_pcie_info(pcie, "Failed to init port.\n");
		return err;
	}

	printf("PCIE-%d: Link up (Gen%d-x%d, Bus%d)\n",
	       dev_seq(dev), pcie_dw_get_link_speed(&pcie->dw),
	       pcie_dw_get_link_width(&pcie->dw),
	       hose->first_busno);

	if (0)
	return pcie_dw_prog_outbound_atu_unroll(&pcie->dw,
						PCIE_ATU_REGION_INDEX0,
						PCIE_ATU_TYPE_MEM,
						pcie->dw.mem.phys_start,
						pcie->dw.mem.bus_start,
						pcie->dw.mem.size);

	return 0;
}

static void __iomem *get_fdt_addr(struct udevice *dev, const char *name)
{
	fdt_addr_t addr;

	addr = dev_read_addr_name(dev, name);

	return (addr == FDT_ADDR_T_NONE) ? NULL : (void __iomem *)addr;
}

static void __iomem *get_fdt_addr_size(struct udevice *dev, const char *name, fdt_size_t *size)
{
	fdt_addr_t addr;

	addr = dev_read_addr_size_name(dev, name, size);

	return (addr == FDT_ADDR_T_NONE) ? NULL : (void __iomem *)addr;
}

static int pcie_cv186x_of_to_plat(struct udevice *dev)
{
	struct pcie_cv186x *pcie = dev_get_priv(dev);
	int ret;

	/* get designware DBI base addr */
	pcie->dw.dbi_base = get_fdt_addr(dev, "dbi");
	if (!pcie->dw.dbi_base)
		return -EINVAL;

	if ((uintptr_t)pcie->dw.dbi_base == PCIE_CV186X_CTRL0_DBI)
		pcie->index = 0;
	else
		pcie->index = 1;

	pcie->dw.atu_base = get_fdt_addr(dev, "atu");
	if (!pcie->dw.atu_base)
		return -EINVAL;

	pcie->dw.cfg_base = get_fdt_addr_size(dev, "config",
			&pcie->dw.cfg_size);
	if (!pcie->dw.cfg_base)
		return -EINVAL;

	pcie->apb_base = get_fdt_addr(dev, "apb");
	if (!pcie->apb_base)
		return -EINVAL;

	pcie->sii_base = get_fdt_addr(dev, "sii");
	if (!pcie->sii_base)
		return -EINVAL;

	pcie->top_apb_base = (void *)0x20be0000;//get_fdt_addr(dev, "top_apb");
	if (!pcie->top_apb_base)
		return -EINVAL;

	ret = gpio_request_by_name(dev, "reset-gpio", 0, &pcie->reset_gpio,
			     GPIOD_IS_OUT);
	if (ret) {
		dev_err(dev, "failed to find reset-gpio property\n");
		return ret;
	}

	//err = clk_get_by_index(dev, 0, &pcie->aux_ck);
	//if (err) {
		//cv_pcie_info(pcie, "clk_get_by_index(aux_ck) failed: %d\n", err);
		//return err;
	//}

	return 0;
}

static const struct dm_pci_ops pcie_cv186x_ops = {
	.read_config	= pcie_dw_read_config,
	.write_config	= pcie_dw_write_config,
};

static const struct udevice_id pcie_cv186x_ids[] = {
	{ .compatible = "cvitek,cv186x-pcie" },
	{}
};

U_BOOT_DRIVER(pcie_cv186x) = {
	.name		= "pcie_cv186x",
	.id		= UCLASS_PCI,
	.of_match	= pcie_cv186x_ids,
	.ops		= &pcie_cv186x_ops,
	.of_to_plat	= pcie_cv186x_of_to_plat,
	.probe		= pcie_cv186x_probe,
	.priv_auto	= sizeof(struct pcie_cv186x),
};
