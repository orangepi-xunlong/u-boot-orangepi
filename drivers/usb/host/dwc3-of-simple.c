// SPDX-License-Identifier: GPL-2.0+
/*
 * dwc3-of-simple.c - OF glue layer for simple integrations
 *
 * Copyright (c) 2015 Texas Instruments Incorporated - http://www.ti.com
 *
 * Author: Felipe Balbi <balbi@ti.com>
 *
 * Copyright (C) 2018 BayLibre, SAS
 * Author: Neil Armstrong <narmstron@baylibre.com>
 */

#include <common.h>
#include <dm.h>
#include <reset.h>
#include <clk.h>
#include <asm/io.h>
#include <dm/of_access.h>
#include <linux/delay.h>
#include <asm/gpio.h>

struct dwc3_of_simple {
	struct clk_bulk		clks;
	struct reset_ctl_bulk	resets;
};

static int dwc3_of_simple_reset_init(struct udevice *dev,
				     struct dwc3_of_simple *simple)
{
	int ret;

	ret = reset_get_bulk(dev, &simple->resets);
	if (ret == -ENOTSUPP)
		return 0;
	else if (ret)
		return ret;

	ret = reset_deassert_bulk(&simple->resets);
	if (ret) {
		reset_release_bulk(&simple->resets);
		return ret;
	}

	return 0;
}

static int dwc3_of_simple_clk_init(struct udevice *dev,
				   struct dwc3_of_simple *simple)
{
	int ret;

	ret = clk_get_bulk(dev, &simple->clks);
	if (ret == -ENOSYS)
		return 0;
	if (ret)
		return ret;

#if CONFIG_IS_ENABLED(CLK)
	ret = clk_enable_bulk(&simple->clks);
	if (ret) {
		clk_release_bulk(&simple->clks);
		return ret;
	}
#endif

	return 0;
}

#define OTP_USB_XTAL_PHY_MASK	0x3

#define REG_USB_SYS_REG_00		0x0
#define REG_USB_EN				BIT(0)

#define REG_USB_SYS_REG_0C		0x0c
#define REG_PHY_REF_CLKDIV2                BIT(0)
#define REG_PHY_FSEL_POS                   1
#define REG_PHY_FSEL_MSK                   (0x3fL << REG_PHY_FSEL_POS)
#define REG_PHY_MPLL_MULTIPLIER_POS        7
#define REG_PHY_MPLL_MULTIPLIER_MSK        (0x7fL << REG_PHY_MPLL_MULTIPLIER_POS)
#define REG_PHY_SSC_REF_CLK_SEL_POS        14
#define REG_PHY_SSC_REF_CLK_SEL_MSK        (0x1ffL << REG_PHY_SSC_REF_CLK_SEL_POS)
#define REG_PHY_REF_SSP_EN					BIT(24)

#define REG_USB_SYS_REG_10			0x10
#define REG_PHY_RX0LOSLEPSEM			BIT(23)

#define REG_USB_SYS_REG_14					0x14
#define REG_PHY_PHY_RESET					BIT(5)

#define USB_PHY_TUNE_CTRL_REG0				0x0

#define USB_PHY_TUNE_CTRL_REG1				0x4
#define REG_USB_PHY_PCS_RX_LOS_MASK_VAL_POS 13
#define REG_USB_PHY_PCS_RX_LOS_MASK_VAL_MSK (0x3ff << REG_USB_PHY_PCS_RX_LOS_MASK_VAL_POS)

#define USB_PHY_TUNE_CTRL_REG2				0x8

static int dwc3_of_simple_probe(struct udevice *dev)
{
	struct dwc3_of_simple *simple = dev_get_plat(dev);
	int ret;
	fdt_addr_t reg_usbsys, reg_phy_tune_ctrl_reg;
	u32 value;
	u32 los_mask;
	u32 otp_usb_xtal_phy;
	struct gpio_desc vbus;

	ret = dwc3_of_simple_clk_init(dev, simple);
	if (ret)
		return ret;

	ret = dwc3_of_simple_reset_init(dev, simple);
	if (ret)
		return ret;

	if (device_is_compatible(dev, "sophgo,cv186x-dwc3")) {
		reg_usbsys = dev_read_addr_index(dev, 0);
		reg_phy_tune_ctrl_reg = dev_read_addr_index(dev, 1);

		value = readl(reg_usbsys + REG_USB_SYS_REG_14) | (REG_PHY_PHY_RESET);
		writel(value, reg_usbsys + REG_USB_SYS_REG_14);

		mdelay(20);

		value = readl(reg_usbsys + REG_USB_SYS_REG_14) & (~REG_PHY_PHY_RESET);
		writel(value, reg_usbsys + REG_USB_SYS_REG_14);

		value = readl(reg_usbsys + REG_USB_SYS_REG_0C) | (REG_PHY_REF_SSP_EN);
		writel(value, reg_usbsys + REG_USB_SYS_REG_0C);

		value = readl(reg_usbsys + REG_USB_SYS_REG_00) | (REG_USB_EN);
		writel(value, reg_usbsys + REG_USB_SYS_REG_00);

		value = readl(reg_usbsys + REG_USB_SYS_REG_10) | REG_PHY_RX0LOSLEPSEM;
		writel(value, reg_usbsys + REG_USB_SYS_REG_10);

		otp_usb_xtal_phy = readl(reg_usbsys + REG_USB_SYS_REG_0C) & OTP_USB_XTAL_PHY_MASK;

		value &= readl(reg_usbsys + REG_USB_SYS_REG_0C)
				& ~(REG_PHY_REF_CLKDIV2) & ~(REG_PHY_FSEL_MSK)
				& ~(REG_PHY_MPLL_MULTIPLIER_MSK) & ~(REG_PHY_SSC_REF_CLK_SEL_MSK);
		switch (otp_usb_xtal_phy) {
		case 0x0:    // xtal = 24 MHz
			value |= (0x2A << REG_PHY_FSEL_POS);
			los_mask = 240;
			break;
		case 0x1:    // xtal = 19.2 MHz
			value |= (0x38 << REG_PHY_FSEL_POS);
			los_mask = 192;
			break;
		case 0x2:    // xtal = 20 MHz
			value |= (0x31 << REG_PHY_FSEL_POS);
			los_mask = 200;
			break;
		case 0x3:    // xtal = 40 MHz
			value |= REG_PHY_REF_CLKDIV2 | (0x31 << REG_PHY_FSEL_POS);
			los_mask = 200;
			break;
		}
		writel(value, reg_usbsys + REG_USB_SYS_REG_0C);

		value = readl(reg_phy_tune_ctrl_reg + USB_PHY_TUNE_CTRL_REG1) & ~(REG_USB_PHY_PCS_RX_LOS_MASK_VAL_MSK);
		value |= (los_mask << REG_USB_PHY_PCS_RX_LOS_MASK_VAL_POS);
		writel(value, reg_phy_tune_ctrl_reg + USB_PHY_TUNE_CTRL_REG1);

		if (gpio_request_by_name(dev, "vbus-gpio", 0, &vbus, GPIOD_IS_OUT))
			return -EINVAL;

		dm_gpio_set_value(&vbus, 1);
	}

	return 0;
}

static int dwc3_of_simple_remove(struct udevice *dev)
{
	struct dwc3_of_simple *simple = dev_get_plat(dev);

	reset_release_bulk(&simple->resets);

	clk_release_bulk(&simple->clks);

	return dm_scan_fdt_dev(dev);
}

static const struct udevice_id dwc3_of_simple_ids[] = {
	{ .compatible = "amlogic,meson-gxl-dwc3" },
	{ .compatible = "rockchip,rk3399-dwc3" },
	{ .compatible = "ti,dwc3" },
	{ .compatible = "sophgo,cv186x-dwc3" },
	{ }
};

U_BOOT_DRIVER(dwc3_of_simple) = {
	.name = "dwc3-of-simple",
	.id = UCLASS_SIMPLE_BUS,
	.of_match = dwc3_of_simple_ids,
	.probe = dwc3_of_simple_probe,
	.remove = dwc3_of_simple_remove,
	.plat_auto	= sizeof(struct dwc3_of_simple),
	.flags = DM_FLAG_ALLOC_PRIV_DMA,
};
