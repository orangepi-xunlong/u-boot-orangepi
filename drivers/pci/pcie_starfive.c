// SPDX-License-Identifier: GPL-2.0+
/*
 * Starfive PLDA PCIe host controller driver
 *
 * Copyright (c) 2023 Starfive, Inc.
 * Author: Mason Huo <mason.huo@starfivetech.com>
 *
 */

#include <common.h>
#include <clk.h>
#include <dm.h>
#include <asm/global_data.h>
#include <dm/device_compat.h>
#include <dm/pinctrl.h>
#include <generic-phy.h>
#include <pci.h>
#include <power-domain.h>
#include <power/regulator.h>
#include <regmap.h>
#include <reset.h>
#include <syscon.h>
#include <asm/io.h>
#include <asm-generic/gpio.h>
#include <linux/delay.h>
#include <linux/iopoll.h>

DECLARE_GLOBAL_DATA_PTR;

#define GEN_SETTINGS			0x80
#define PCIE_PCI_IDS			0x9C
#define PCIE_WINROM			0xFC
#define PMSG_SUPPORT_RX			0x3F0
#define PCI_MISC			0xB4

#define PLDA_EP_ENABLE			0
#define PLDA_RP_ENABLE			1

#define IDS_REVISION_ID			0x02
#define IDS_PCI_TO_PCI_BRIDGE		0x060400
#define IDS_CLASS_CODE_SHIFT		8

#define PREF_MEM_WIN_64_SUPPORT		BIT(3)
#define PMSG_LTR_SUPPORT		BIT(2)
#define PLDA_FUNCTION_DIS		BIT(15)
#define PLDA_FUNC_NUM			4
#define PLDA_PHY_FUNC_SHIFT		9

#define XR3PCI_ATR_AXI4_SLV0		0x800
#define XR3PCI_ATR_SRC_ADDR_LOW		0x0
#define XR3PCI_ATR_SRC_ADDR_HIGH	0x4
#define XR3PCI_ATR_TRSL_ADDR_LOW	0x8
#define XR3PCI_ATR_TRSL_ADDR_HIGH	0xc
#define XR3PCI_ATR_TRSL_PARAM		0x10
#define XR3PCI_ATR_TABLE_OFFSET		0x20
#define XR3PCI_ATR_MAX_TABLE_NUM	8

#define XR3PCI_ATR_SRC_WIN_SIZE_SHIFT	1
#define XR3PCI_ATR_SRC_ADDR_MASK	0xfffff000
#define XR3PCI_ATR_TRSL_ADDR_MASK	0xfffff000
#define XR3_PCI_ECAM_SIZE		28
#define XR3PCI_ATR_TRSL_DIR		BIT(22)
/* IDs used in the XR3PCI_ATR_TRSL_PARAM */
#define XR3PCI_ATR_TRSLID_PCIE_MEMORY	0x0
#define XR3PCI_ATR_TRSLID_PCIE_CONFIG	0x1

#define ECAM_BUS_SHIFT			20
#define ECAM_DEV_SHIFT			15
#define ECAM_FUNC_SHIFT			12
/* Secondary bus number offset in config space */
#define PCI_SECONDARY_BUS		0x19

/* system control */
#define STG_SYSCON_K_RP_NEP_SHIFT		0x8
#define STG_SYSCON_K_RP_NEP_MASK		0x100
#define STG_SYSCON_AXI4_SLVL_ARFUNC_MASK	0x7FFF00
#define STG_SYSCON_AXI4_SLVL_ARFUNC_SHIFT	0x8
#define STG_SYSCON_AXI4_SLVL_AWFUNC_MASK	0x7FFF
#define STG_SYSCON_AXI4_SLVL_AWFUNC_SHIFT	0x0
#define STG_SYSCON_CLKREQ_SHIFT			0x16
#define STG_SYSCON_CLKREQ_MASK			0x400000
#define STG_SYSCON_CKREF_SRC_SHIFT		0x12
#define STG_SYSCON_CKREF_SRC_MASK		0xC0000

struct starfive_pcie {
	struct udevice *dev;

	void __iomem *reg_base;
	void __iomem *cfg_base;

	struct regmap *regmap;
	u32 stg_arfun;
	u32 stg_awfun;
	u32 stg_rp_nep;

	struct clk_bulk	clks;
	struct reset_ctl_bulk	rsts;

	int atr_table_num;
	int first_busno;
};

static int starfive_pcie_addr_valid(pci_dev_t bdf, int first_busno)
{
	if ((PCI_BUS(bdf) == first_busno) && (PCI_DEV(bdf) > 0))
		return 0;
	if ((PCI_BUS(bdf) == first_busno + 1) && (PCI_DEV(bdf) > 0))
		return 0;

	return 1;
}

static int starfive_pcie_off_conf(pci_dev_t bdf, uint offset, int first_busno)
{
	unsigned int bus = PCI_BUS(bdf) - first_busno;
	unsigned int dev = PCI_DEV(bdf);
	unsigned int func = PCI_FUNC(bdf);

	return (bus << ECAM_BUS_SHIFT) | (dev << ECAM_DEV_SHIFT) |
			(func << ECAM_FUNC_SHIFT) | (offset & ~0x3);
}

static bool plda_pcie_hide_rc_bar(pci_dev_t bdf, int offset, int first_busno)
{
	if ((PCI_BUS(bdf) == first_busno) &&
	    (offset == PCI_BASE_ADDRESS_0 || offset == PCI_BASE_ADDRESS_1))
		return true;

	return false;
}

static int starfive_pcie_config_read(const struct udevice *udev, pci_dev_t bdf,
				uint offset, ulong *valuep,
				enum pci_size_t size)
{
	void __iomem *addr;
	ulong value;
	struct starfive_pcie *priv = dev_get_priv(udev);
	int where = starfive_pcie_off_conf(bdf, offset, priv->first_busno);

	if (!starfive_pcie_addr_valid(bdf, priv->first_busno)) {
		pr_debug("Out of range\n");
		*valuep = pci_get_ff(size);
		return 0;
	}

	addr = priv->cfg_base;
	addr += where;

	if (!addr)
		return -1;

	/* Make sure the LAST TLP is finished, before reading vendor ID. */
	if (offset == PCI_VENDOR_ID)
		mdelay(20);

	value = readl(addr);
	*valuep = pci_conv_32_to_size(value, offset, size);

	return 0;

}

int starfive_pcie_config_write(struct udevice *udev, pci_dev_t bdf,
				 uint offset, ulong value,
				 enum pci_size_t size)
{
	void __iomem *addr;
	ulong old;
	struct starfive_pcie *priv = dev_get_priv(udev);
	int where = starfive_pcie_off_conf(bdf, offset, priv->first_busno);

	if (plda_pcie_hide_rc_bar(bdf, offset, priv->first_busno))
		return -1;

	if (!starfive_pcie_addr_valid(bdf, priv->first_busno)) {
		pr_debug("Out of range\n");
		return 0;
	}

	addr = priv->cfg_base;
	addr += where;

	if (!addr)
		return -1;

	old = readl(addr);
	value = pci_conv_size_to_32(old, value, offset, size);
	writel(value, addr);

	return 0;
}


static void starfive_pcie_set_atr_entry(struct starfive_pcie *priv, phys_addr_t src_addr,
			phys_addr_t trsl_addr, size_t window_size,
			int trsl_param)
{
	void __iomem *base =
		priv->reg_base + XR3PCI_ATR_AXI4_SLV0;

	/* Support AXI4 Slave 0 Address Translation Tables 0-7. */
	if (priv->atr_table_num >= XR3PCI_ATR_MAX_TABLE_NUM)
		priv->atr_table_num = XR3PCI_ATR_MAX_TABLE_NUM - 1;
	base +=  XR3PCI_ATR_TABLE_OFFSET * priv->atr_table_num;
	priv->atr_table_num++;

	/* X3PCI_ATR_SRC_ADDR_LOW:
	 *   - bit 0: enable entry,
	 *   - bits 1-6: ATR window size: total size in bytes: 2^(ATR_WSIZE + 1)
	 *   - bits 7-11: reserved
	 *   - bits 12-31: start of source address
	 */
	writel((lower_32_bits(src_addr) & XR3PCI_ATR_SRC_ADDR_MASK) |
			(fls(window_size) - 1) << XR3PCI_ATR_SRC_WIN_SIZE_SHIFT | 1,
			base + XR3PCI_ATR_SRC_ADDR_LOW);
	writel(upper_32_bits(src_addr), base + XR3PCI_ATR_SRC_ADDR_HIGH);
	writel((lower_32_bits(trsl_addr) & XR3PCI_ATR_TRSL_ADDR_MASK),
			base + XR3PCI_ATR_TRSL_ADDR_LOW);
	writel(upper_32_bits(trsl_addr), base + XR3PCI_ATR_TRSL_ADDR_HIGH);
	writel(trsl_param, base + XR3PCI_ATR_TRSL_PARAM);

	dev_info(priv->dev, "ATR entry: 0x%010llx %s 0x%010llx [0x%010llx] (param: 0x%06x)\n",
	       src_addr, (trsl_param & XR3PCI_ATR_TRSL_DIR) ? "<-" : "->",
	       trsl_addr, (u64)window_size, trsl_param);
}

static int starfive_pcie_atr_init(struct starfive_pcie *priv)
{
	struct udevice *ctlr = pci_get_controller(priv->dev);
	struct pci_controller *hose = dev_get_uclass_priv(ctlr);
	int i;

	/* As the two host bridges in JH7110 soc have the same default
	 * address translation table, this cause the second root port can't
	 * access it's host bridge config space correctly.
	 * To workaround, config the ATR of host bridge config space by SW.
	 */
	starfive_pcie_set_atr_entry(priv,
				(phys_addr_t)priv->cfg_base,
				0,
				1 << XR3_PCI_ECAM_SIZE,
				XR3PCI_ATR_TRSLID_PCIE_CONFIG);

	for (i = 0; i < hose->region_count; i++) {
		if (hose->regions[i].flags == PCI_REGION_SYS_MEMORY)
			continue;

		/* Only support identity mappings. */
		if (hose->regions[i].bus_start !=
		    hose->regions[i].phys_start)
			return -EINVAL;

		starfive_pcie_set_atr_entry(priv,
				hose->regions[i].phys_start,
				hose->regions[i].bus_start,
				hose->regions[i].size,
				XR3PCI_ATR_TRSLID_PCIE_MEMORY);

	}

	return 0;
}

static int starfive_pcie_get_syscon(struct udevice *dev)
{
	struct starfive_pcie *priv = dev_get_priv(dev);
	struct udevice *syscon;
	struct ofnode_phandle_args syscfg_phandle;
	u32 cells[4];
	int ret;

	/* get corresponding syscon phandle */
	ret = dev_read_phandle_with_args(dev, "starfive,stg-syscon", NULL, 0, 0,
					&syscfg_phandle);

	if (ret < 0) {
		dev_err(dev, "Can't get syscfg phandle: %d\n", ret);
		return ret;
	}

	ret = uclass_get_device_by_ofnode(UCLASS_SYSCON, syscfg_phandle.node,
									  &syscon);
	if (ret) {
		dev_err(dev, "Unable to find syscon device (%d)\n", ret);
		return ret;
	}

	priv->regmap = syscon_get_regmap(syscon);
	if (!priv->regmap) {
		dev_err(dev, "Unable to find regmap\n");
		return -ENODEV;
	}

	/* get syscon register offset */
	ret = dev_read_u32_array(dev, "starfive,stg-syscon",
				 cells, ARRAY_SIZE(cells));
	if (ret) {
		dev_err(dev, "Get syscon register count err %d\n", ret);
		return -EINVAL;
	}

	dev_dbg(dev, "Get syscon values: %x, %x, %x\n",
			cells[1], cells[2], cells[3]);
	priv->stg_arfun = cells[1];
	priv->stg_awfun = cells[2];
	priv->stg_rp_nep = cells[3];

	return 0;
}

static int starfive_pcie_parse_dt(struct udevice *dev)
{
	struct starfive_pcie *priv = dev_get_priv(dev);
	int ret;

	priv->reg_base = (void *)dev_read_addr_name(dev, "reg");
	if (priv->reg_base == (void __iomem *)FDT_ADDR_T_NONE) {
		dev_err(dev, "Missing required reg address range\n");
		return -EINVAL;
	}

	priv->cfg_base = (void *)dev_read_addr_name(dev, "config");
	if (priv->cfg_base == (void __iomem *)FDT_ADDR_T_NONE) {
		dev_err(dev, "Missing required config address range");
		return -EINVAL;
	}

	ret = starfive_pcie_get_syscon(dev);
	if (ret) {
		dev_err(dev, "Can't get syscon: %d\n", ret);
		return ret;
	}

	ret = reset_get_bulk(dev, &priv->rsts);
	if (ret) {
		dev_err(dev, "Can't get reset: %d\n", ret);
		return ret;
	}

	ret = clk_get_bulk(dev, &priv->clks);
	if (ret) {
		dev_err(dev, "Can't get clock: %d\n", ret);
		return ret;
	}

	return 0;
}

static int starfive_pcie_init_port(struct udevice *dev)
{
	int ret, i;
	unsigned int value;
	struct starfive_pcie *priv = dev_get_priv(dev);

	ret = clk_enable_bulk(&priv->clks);
	if (ret) {
		dev_err(dev, "Failed to enable clks (ret=%d)\n", ret);
		return ret;
	}

	ret = reset_deassert_bulk(&priv->rsts);
	if (ret) {
		dev_err(dev, "Failed to deassert resets (ret=%d)\n", ret);
		goto err_deassert_clk;
	}

	ret = pinctrl_select_state(dev, "perst-active");
	if (ret) {
		dev_err(dev, "Set perst-active pinctrl failed: %d\n", ret);
		goto err_deassert_reset;
	}

	/* Disable physical functions except #0 */
	for (i = 1; i < PLDA_FUNC_NUM; i++) {
		regmap_update_bits(priv->regmap,
				priv->stg_arfun,
				STG_SYSCON_AXI4_SLVL_ARFUNC_MASK,
				(i << PLDA_PHY_FUNC_SHIFT) <<
				STG_SYSCON_AXI4_SLVL_ARFUNC_SHIFT);
		regmap_update_bits(priv->regmap,
				priv->stg_awfun,
				STG_SYSCON_AXI4_SLVL_AWFUNC_MASK,
				(i << PLDA_PHY_FUNC_SHIFT) <<
				STG_SYSCON_AXI4_SLVL_AWFUNC_SHIFT);

		value = readl(priv->reg_base + PCI_MISC);
		value |= PLDA_FUNCTION_DIS;
		writel(value, priv->reg_base + PCI_MISC);
	}

	regmap_update_bits(priv->regmap,
				priv->stg_arfun,
				STG_SYSCON_AXI4_SLVL_ARFUNC_MASK,
				0 << STG_SYSCON_AXI4_SLVL_ARFUNC_SHIFT);
	regmap_update_bits(priv->regmap,
				priv->stg_awfun,
				STG_SYSCON_AXI4_SLVL_AWFUNC_MASK,
				0 << STG_SYSCON_AXI4_SLVL_AWFUNC_SHIFT);

	/* Enable root port*/
	value = readl(priv->reg_base + GEN_SETTINGS);
	value |= PLDA_RP_ENABLE;
	writel(value, priv->reg_base + GEN_SETTINGS);

	/* PCIe PCI Standard Configuration Identification Settings. */
	value = (IDS_PCI_TO_PCI_BRIDGE << IDS_CLASS_CODE_SHIFT) | IDS_REVISION_ID;
	writel(value, priv->reg_base + PCIE_PCI_IDS);

	/* The LTR message forwarding of PCIe Message Reception was set by core
	 * as default, but the forward id & addr are also need to be reset.
	 * If we do not disable LTR message forwarding here, or set a legal
	 * forwarding address, the kernel will get stuck after this driver probe.
	 * To workaround, disable the LTR message forwarding support on
	 * PCIe Message Reception.
	 */
	value = readl(priv->reg_base + PMSG_SUPPORT_RX);
	value &= ~PMSG_LTR_SUPPORT;
	writel(value, priv->reg_base + PMSG_SUPPORT_RX);

	/* Prefetchable memory window 64-bit addressing support */
	value = readl(priv->reg_base + PCIE_WINROM);
	value |= PREF_MEM_WIN_64_SUPPORT;
	writel(value, priv->reg_base + PCIE_WINROM);

	starfive_pcie_atr_init(priv);

	/* Ensure that PERST has been asserted for at least 300 ms */
	mdelay(300);
	ret = pinctrl_select_state(dev, "perst-default");
	if (ret) {
		dev_err(dev, "Set perst-default pinctrl failed: %d\n", ret);
		return ret;
	}

	return 0;

err_deassert_reset:
	reset_assert_bulk(&priv->rsts);
err_deassert_clk:
	clk_disable_bulk(&priv->clks);

	return ret;
}

static int starfive_pcie_probe(struct udevice *dev)
{
	struct starfive_pcie *priv = dev_get_priv(dev);
	int ret;

	priv->atr_table_num = 0;
	priv->dev = dev;
	priv->first_busno = dev_seq(dev);

	ret = starfive_pcie_parse_dt(dev);
	if (ret)
		return ret;

	regmap_update_bits(priv->regmap,
				priv->stg_rp_nep,
				STG_SYSCON_K_RP_NEP_MASK,
				1 << STG_SYSCON_K_RP_NEP_SHIFT);

	regmap_update_bits(priv->regmap,
				priv->stg_awfun,
				STG_SYSCON_CKREF_SRC_MASK,
				2 << STG_SYSCON_CKREF_SRC_SHIFT);

	regmap_update_bits(priv->regmap,
				priv->stg_awfun,
				STG_SYSCON_CLKREQ_MASK,
				1 << STG_SYSCON_CLKREQ_SHIFT);

	ret = starfive_pcie_init_port(dev);
	if (ret)
		return ret;

	dev_err(dev, "Starfive PCIe bus probed.\n");

	return 0;
}

static const struct dm_pci_ops starfive_pcie_ops = {
	.read_config	= starfive_pcie_config_read,
	.write_config	= starfive_pcie_config_write,
};

static const struct udevice_id starfive_pcie_ids[] = {
	{ .compatible = "starfive,jh7110-pcie" },
	{ }
};

U_BOOT_DRIVER(starfive_pcie_drv) = {
	.name			= "starfive_pcie",
	.id			= UCLASS_PCI,
	.of_match		= starfive_pcie_ids,
	.ops			= &starfive_pcie_ops,
	.probe			= starfive_pcie_probe,
	.priv_auto	= sizeof(struct starfive_pcie),
};
