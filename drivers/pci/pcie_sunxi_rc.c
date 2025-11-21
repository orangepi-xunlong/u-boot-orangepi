// SPDX-License-Identifier: GPL-2.0+
/*
 * sunxi DesignWare based PCIe host controller driver
 *
 * Copyright (c) 2021 sunxi, Inc.
 */

#include <common.h>
#include <clk.h>
#include <dm.h>
#include <generic-phy.h>
#include <pci.h>
#include <power-domain.h>
#include <power/regulator.h>
#include <reset.h>
#include <syscon.h>
#include <asm/io.h>
#include <asm-generic/gpio.h>
#include <asm/arch-sunxi/clock.h>
#include <linux/iopoll.h>
#include <linux/ioport.h>
#include "pcie-sunxi.h"

DECLARE_GLOBAL_DATA_PTR;

#define sunxi_pcie_DBG			0

#define __pcie_dev_print_emit(fmt, ...) \
({ \
	printf(fmt, ##__VA_ARGS__); \
})

#ifdef dev_err
#undef dev_err
#define dev_err(dev, fmt, ...) \
({ \
	if (dev) \
		__pcie_dev_print_emit("%s: " fmt, dev->name, \
				##__VA_ARGS__); \
})
#endif

#ifdef dev_info
#undef dev_info
#define dev_info dev_err
#endif

#ifdef DEBUG
#define dev_dbg dev_err
#else
#define dev_dbg(dev, fmt, ...)					\
({								\
	if (0)							\
		__dev_printk(7, dev, fmt, ##__VA_ARGS__);	\
})
#endif

#define msleep(a)		udelay((a) * 1000)

static int sunxi_pcie_addr_valid(pci_dev_t d, int first_busno)
{
	if ((PCI_BUS(d) == first_busno) && (PCI_DEV(d) > 0))
		return 0;
	if ((PCI_BUS(d) == first_busno + 1) && (PCI_DEV(d) > 0))
		return 0;

	return 1;
}

static void sunxi_pcie_prog_outbound_atu(struct sunxi_pcie_port *pp, int index, int type,
					u64 cpu_addr, u64 pci_addr, u32 size)
{
	struct sunxi_pcie *pci = to_sunxi_pcie_from_pp(pp);

	sunxi_pcie_writel_dbi(pci, PCIE_ATU_LOWER_BASE_OUTBOUND(index), lower_32_bits(cpu_addr));
	sunxi_pcie_writel_dbi(pci, PCIE_ATU_UPPER_BASE_OUTBOUND(index), upper_32_bits(cpu_addr));
	sunxi_pcie_writel_dbi(pci, PCIE_ATU_LIMIT_OUTBOUND(index), lower_32_bits(cpu_addr + size - 1));
	sunxi_pcie_writel_dbi(pci, PCIE_ATU_LOWER_TARGET_OUTBOUND(index), lower_32_bits(pci_addr));
	sunxi_pcie_writel_dbi(pci, PCIE_ATU_UPPER_TARGET_OUTBOUND(index), upper_32_bits(pci_addr));
	sunxi_pcie_writel_dbi(pci, PCIE_ATU_CR1_OUTBOUND(index), type);
	sunxi_pcie_writel_dbi(pci, PCIE_ATU_CR2_OUTBOUND(index), PCIE_ATU_ENABLE);
}

static int sunxi_pcie_rd_other_conf(struct sunxi_pcie_port *pp, pci_dev_t d, int where, int size, ulong *val)
{
	int ret = PCIBIOS_SUCCESSFUL, type;
	u64 busdev;

	busdev = PCIE_ATU_BUS(PCI_BUS(d)) | PCIE_ATU_DEV(PCI_DEV(d)) | PCIE_ATU_FUNC(PCI_FUNC(d));

	if (PCI_BUS(d) != 0)
		type = PCIE_ATU_TYPE_CFG0;
	else
		type = PCIE_ATU_TYPE_CFG1;

	sunxi_pcie_prog_outbound_atu(pp, PCIE_ATU_INDEX0, type, pp->cfg0_base, busdev, pp->cfg0_size);

	ret = sunxi_pcie_cfg_read(pp->va_cfg0_base + where, size, val);

	return ret;
}

static int sunxi_pcie_wr_other_conf(struct sunxi_pcie_port *pp, pci_dev_t d, int where, int size, ulong val)
{
	int ret = PCIBIOS_SUCCESSFUL, type;
	u64 busdev;

	busdev = PCIE_ATU_BUS(PCI_BUS(d)) | PCIE_ATU_DEV(PCI_DEV(d)) | PCIE_ATU_FUNC(PCI_FUNC(d));

	if (PCI_BUS(d) != 0)
		type = PCIE_ATU_TYPE_CFG0;
	else
		type = PCIE_ATU_TYPE_CFG1;

	sunxi_pcie_prog_outbound_atu(pp, PCIE_ATU_INDEX0, type, pp->cfg0_base, busdev, pp->cfg0_size);

	ret = sunxi_pcie_cfg_write(pp->va_cfg0_base + where, size, val);

	return ret;
}

static int sunxi_pcie_host_rd_own_conf(struct sunxi_pcie_port *pp, int where, int size, ulong *val)
{
	int ret;

	ret = sunxi_pcie_cfg_read(pp->dbi_base + where, size, val);

	return ret;
}

static int sunxi_pcie_host_wr_own_conf(struct sunxi_pcie_port *pp, int where, int size, ulong val)
{
	int ret;

	ret = sunxi_pcie_cfg_write(pp->dbi_base + where, size, val);

	return ret;
}

static int sunxi_pcie_read_config(struct udevice *bus, pci_dev_t bdf,
				 uint offset, ulong *value,
				 enum pci_size_t size)
{
	struct sunxi_pcie *pcie = dev_get_priv(bus);
	int ret, size_len = 4;

	if (!sunxi_pcie_addr_valid(bdf, pcie->first_busno)) {
		debug("- out of range\n");
		*value = pci_get_ff(size);
		return 0;
	}

	if (size == PCI_SIZE_8)
		size_len = 1;
	else if (size == PCI_SIZE_16)
		size_len = 2;
	else if (size == PCI_SIZE_32)
		size_len = 4;

	if (PCI_BUS(bdf) != 0)
		ret = sunxi_pcie_rd_other_conf(&pcie->pcie_port, bdf, offset, size_len, value);
	else
		ret = sunxi_pcie_host_rd_own_conf(&pcie->pcie_port, offset, size_len, value);

	return ret;
}

static int sunxi_pcie_write_config(struct udevice *bus, pci_dev_t bdf,
				 uint offset, ulong value,
				 enum pci_size_t size)
{
	struct sunxi_pcie *pcie = dev_get_priv(bus);
	int ret, size_len = 4;

	if (!sunxi_pcie_addr_valid(bdf, pcie->first_busno)) {
		debug("- out of range\n");
		return 0;
	}

	if (size == PCI_SIZE_8)
		size_len = 1;
	else if (size == PCI_SIZE_16)
	size_len = 2;
	else if (size == PCI_SIZE_32)
		size_len = 4;

	if (PCI_BUS(bdf) != 0)
		ret = sunxi_pcie_wr_other_conf(&pcie->pcie_port, bdf, offset, size_len, value);
	else
		ret = sunxi_pcie_host_wr_own_conf(&pcie->pcie_port, offset, size_len, value);

	return ret;
}

static void sunxi_pcie_host_setup_rc(struct sunxi_pcie_port *pp)
{
	ulong val, i;
	phys_addr_t mem_base;
	struct sunxi_pcie *pci = to_sunxi_pcie_from_pp(pp);

	sunxi_pcie_plat_set_rate(pci);

	/* setup RC BARs */
	sunxi_pcie_writel_dbi(pci, PCI_BASE_ADDRESS_0, 0x4);
	sunxi_pcie_writel_dbi(pci, PCI_BASE_ADDRESS_1, 0x0);

	/* setup interrupt pins */
	val = sunxi_pcie_readl_dbi(pci, PCI_INTERRUPT_LINE);
	val &= PCIE_INTERRUPT_LINE_MASK;
	val |= PCIE_INTERRUPT_LINE_ENABLE;
	sunxi_pcie_writel_dbi(pci, PCI_INTERRUPT_LINE, val);

	/* setup bus numbers */
	val = sunxi_pcie_readl_dbi(pci, PCI_PRIMARY_BUS);
	val &= 0xff000000;
	val |= 0x00ff0100;
	sunxi_pcie_writel_dbi(pci, PCI_PRIMARY_BUS, val);

	/* setup command register */
	val = sunxi_pcie_readl_dbi(pci, PCI_COMMAND);

	val &= PCIE_HIGH16_MASK;
	val |= PCI_COMMAND_IO | PCI_COMMAND_MEMORY |
		PCI_COMMAND_MASTER | PCI_COMMAND_SERR;

	sunxi_pcie_writel_dbi(pci, PCI_COMMAND, val);

	//whether need to fixed me
	if (IS_ENABLED(CONFIG_PCI_MSI) && !pp->has_its) {
		for (i = 0; i < 8; i++) {
			sunxi_pcie_host_wr_own_conf(pp, PCIE_MSI_INTR_ENABLE(i), 4, ~0);
		}
	}

	//mem defualt to atu1 & io defualt to atu2
	if (pp->cpu_pcie_addr_quirk)
		mem_base = pp->mem_base - PCIE_CPU_BASE;
	else
		mem_base = pp->mem_base;

	sunxi_pcie_prog_outbound_atu(pp, PCIE_ATU_INDEX1, PCIE_ATU_TYPE_MEM, mem_base,
						SUNXI_PCIE_MEM_ADDR,
						pp->mem_size);

	sunxi_pcie_prog_outbound_atu(pp, PCIE_ATU_INDEX2, PCIE_ATU_TYPE_IO, pp->io_base,
						SUNXI_PCIE_IO_ADDR,
						pp->io_size);

	sunxi_pcie_host_wr_own_conf(pp, PCI_BASE_ADDRESS_0, 4, 0);

	sunxi_pcie_dbi_ro_wr_en(pci);

	sunxi_pcie_host_wr_own_conf(pp, PCI_CLASS_DEVICE, 2, PCI_CLASS_BRIDGE_PCI);

	sunxi_pcie_dbi_ro_wr_dis(pci);

	sunxi_pcie_host_rd_own_conf(pp, PCIE_LINK_WIDTH_SPEED_CONTROL, 4, &val);
	val |= PORT_LOGIC_SPEED_CHANGE;
	sunxi_pcie_host_wr_own_conf(pp, PCIE_LINK_WIDTH_SPEED_CONTROL, 4, val);
}

static int sunxi_pcie_host_link_up_status(struct sunxi_pcie_port *pp)
{
	u32 val;
	int ret;
	struct sunxi_pcie *pcie = to_sunxi_pcie_from_pp(pp);

	val = sunxi_pcie_readl(pcie, PCIE_LINK_STAT);

	if ((val & RDLH_LINK_UP) && (val & SMLH_LINK_UP))
		ret = 1;
	else
		ret = 0;

	return ret;
}

static int sunxi_pcie_host_link_up(struct sunxi_pcie_port *pp)
{
	 return sunxi_pcie_host_link_up_status(pp);
}

static int sunxi_pcie_host_wait_for_link(struct sunxi_pcie_port *pp)
{
	int retries;

	for (retries = 0; retries < LINK_WAIT_MAX_RETRIE; retries++) {
		if (sunxi_pcie_host_link_up(pp)) {
			printf("pcie link up success\n");
			return 0;
		}
		udelay(LINK_WAIT_USLEEP_MAX);
	}

	printf("Link up timeout\n");
	return -ETIMEDOUT;
}

static int sunxi_pcie_host_establish_link(struct sunxi_pcie *pci)
{
	struct sunxi_pcie_port *pp = &pci->pcie_port;

	if (sunxi_pcie_host_link_up(pp)) {
		printf("pcie is already link up\n");
		return 0;
	}

	sunxi_pcie_plat_ltssm_enable(pci);

	return sunxi_pcie_host_wait_for_link(pp);
}

static int sunxi_pcie_host_wait_for_speed_change(struct sunxi_pcie *pci)
{
	u32 tmp;
	unsigned int retries;

	for (retries = 0; retries < LINK_WAIT_MAX_RETRIE; retries++) {
		tmp = sunxi_pcie_readl_dbi(pci, PCIE_LINK_WIDTH_SPEED_CONTROL);
		if (!(tmp & PORT_LOGIC_SPEED_CHANGE))
			return 0;
		udelay(SPEED_CHANGE_USLEEP_MAX);
	}

	printf("Speed change timeout\n");
	return -ETIMEDOUT;
}

static int sunxi_pcie_host_speed_change(struct sunxi_pcie *pci, int gen)
{
	int val;
	int ret;

	sunxi_pcie_dbi_ro_wr_en(pci);
	val = sunxi_pcie_readl_dbi(pci, LINK_CONTROL2_LINK_STATUS2);
	val &= ~0xf;
	val |= gen;
	sunxi_pcie_writel_dbi(pci, LINK_CONTROL2_LINK_STATUS2, val);

	val = sunxi_pcie_readl_dbi(pci, PCIE_LINK_WIDTH_SPEED_CONTROL);
	val &= ~PORT_LOGIC_SPEED_CHANGE;
	sunxi_pcie_writel_dbi(pci, PCIE_LINK_WIDTH_SPEED_CONTROL, val);

	val = sunxi_pcie_readl_dbi(pci, PCIE_LINK_WIDTH_SPEED_CONTROL);
	val |= PORT_LOGIC_SPEED_CHANGE;
	sunxi_pcie_writel_dbi(pci, PCIE_LINK_WIDTH_SPEED_CONTROL, val);

	ret = sunxi_pcie_host_wait_for_speed_change(pci);
	if (!ret)
		printf("PCIe speed of Gen%d\n", gen);
	else
		printf("PCIe speed of Gen1\n");

	sunxi_pcie_dbi_ro_wr_dis(pci);
	return 0;
}

static void sunxi_pcie_host_init(struct udevice *dev)
{
	struct sunxi_pcie *pci = dev_get_priv(dev);
	unsigned int pcie_reset_gpio = sunxi_name_to_gpio(CONFIG_PCIE_PERST_GPIO);
	unsigned int pcie_wake_gpio = sunxi_name_to_gpio(CONFIG_PCIE_WAKE_GPIO);
	unsigned int pcie_power_gpio = sunxi_name_to_gpio(CONFIG_PCIE_POWER_GPIO);

	sunxi_pcie_plat_ltssm_disable(pci);

	if (pcie_reset_gpio >= 0 && pcie_wake_gpio >= 0 && pcie_power_gpio >=0) {
		/* set cfg, ouput */
		sunxi_gpio_set_cfgpin(pcie_reset_gpio, 1);
		sunxi_gpio_set_cfgpin(pcie_wake_gpio, 1);
		sunxi_gpio_set_cfgpin(pcie_power_gpio, 1);

		gpio_set_value(pcie_wake_gpio, 1);
		gpio_set_value(pcie_power_gpio, 1);
		gpio_set_value(pcie_reset_gpio, 0);
		mdelay(100);
		gpio_set_value(pcie_reset_gpio, 1);
	}
	mdelay(100);

	sunxi_pcie_host_setup_rc(&pci->pcie_port);

	sunxi_pcie_host_establish_link(pci);

	sunxi_pcie_host_speed_change(pci, pci->link_gen);
}

static int sunxi_pcie_probe(struct udevice *dev)
{
	struct sunxi_pcie *pcie = dev_get_priv(dev);
	struct udevice *ctlr = pci_get_controller(dev);
	struct pci_controller *hose = dev_get_uclass_priv(ctlr);
	int ret;

	pcie->first_busno = dev->seq;
	pcie->dev = dev;
	pcie->drvdata = (struct sunxi_pcie_of_data *)dev_get_driver_data(dev);
	pcie->link_gen = dev_read_u32_default(dev, "max-link-speed", 1);
	pcie->pcie1v8_supply = dev_read_string(dev, "pcie1v8_supply");
	pcie->pcie3v3_supply = dev_read_string(dev, "pcie3v3_supply");
	pcie->phy = kmalloc(sizeof(struct phy), __GFP_ZERO);
	ret = generic_phy_get_by_name(dev, "pcie-phy", pcie->phy);
	if (ret < 0) {
		pr_err("failed get pcie-phy %d\n", ret);
		return -EINVAL;
	}

	/* PCI I/O space */
	pci_set_region(&hose->regions[0],
			SUNXI_PCIE_IO_ADDR,
			SUNXI_PCIE_IO_ADDR,
			SUNXI_PCIE_IO_SIZE, PCI_REGION_IO);


	/* System memory space */
	pci_set_region(&hose->regions[1],
			SUNXI_PCIE_MEM_ADDR,
			SUNXI_PCIE_MEM_ADDR,
			SUNXI_PCIE_MEM_SIZE, PCI_REGION_MEM);
	hose->region_count = 2;

	sunxi_pcie_plat_init(dev);

	sunxi_pcie_plat_hw_init(dev);

	/* Start the controller. */
	sunxi_pcie_host_init(dev);

	return 0;
}

static const struct dm_pci_ops sunxi_pcie_ops = {
	.read_config	= sunxi_pcie_read_config,
	.write_config	= sunxi_pcie_write_config,
};

static const struct sunxi_pcie_of_data	sunxi_pcie_rc_v210_of_data = {
	.mode = SUNXI_PCIE_RC_TYPE,
	.cpu_pcie_addr_quirk = true,
};

static const struct sunxi_pcie_of_data	sunxi_pcie_rc_v300_of_data = {
	.mode = SUNXI_PCIE_RC_TYPE,
	.has_pcie_slv_clk = true,
	.need_pcie_rst = true,
	.pcie_slv_clk_400m = true,
	.has_pcie_its_clk = true,
};

static const struct udevice_id sunxi_pcie_ids[] = {
	{
		.compatible = "allwinner,sunxi-pcie-v210-rc",
		.data = (ulong)&sunxi_pcie_rc_v210_of_data,
	},
	{
		.compatible = "allwinner,sunxi-pcie-v300-rc",
		.data = (ulong)&sunxi_pcie_rc_v300_of_data,
	},
	{ }
};

U_BOOT_DRIVER(sunxi_pcie) = {
	.name			= "pcie_dw_sunxi",
	.id			= UCLASS_PCI,
	.of_match		= sunxi_pcie_ids,
	.ops			= &sunxi_pcie_ops,
	.probe			= sunxi_pcie_probe,
	.priv_auto_alloc_size	= sizeof(struct sunxi_pcie),
};
