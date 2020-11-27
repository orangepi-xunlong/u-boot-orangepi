/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include  <asm/io.h>
#include  <asm/arch/ccmu.h>
#include  <asm/arch/timer.h>
#include <common.h>

#define HOSC_19M	1
#define HOSC_38M	2
#define HOSC_24M	3

static void disable_otg_clk_reset_gating(void)
{
	u32 reg_temp = 0;

	reg_temp = readl(CCMU_USB_BGR_REG);
	reg_temp &= ~((0x1 << USBOTG_RESET_BIT) | (0x1 << USBOTG_CLK_ONOFF_BIT));
	writel(reg_temp, CCMU_USB_BGR_REG);
}

static void disable_phy_clk_reset_gating(void)
{
	u32 reg_value = 0;

	reg_value = readl(CCMU_USB0_CLK_REG);
	reg_value &= ~((0x1 << USB0_PHY_CLK_ONOFF_BIT) | (0x1 << USB0_PHY_RESET_BIT));
	writel(reg_value, CCMU_USB0_CLK_REG);
}

static void enable_otg_clk_reset_gating(void)
{
	u32 reg_value = 0;

	reg_value = readl(CCMU_USB_BGR_REG);
	reg_value |= (1 << USBOTG_RESET_BIT);
	writel(reg_value, CCMU_USB_BGR_REG);

	__usdelay(500);

	reg_value = readl(CCMU_USB_BGR_REG);
	reg_value |= (1 << USBOTG_CLK_ONOFF_BIT);
	writel(reg_value, CCMU_USB_BGR_REG);

	__usdelay(500);
}

static void enable_phy_clk_reset_gating(void)
{
	u32 reg_value = 0;

	reg_value = readl(CCMU_USB0_CLK_REG);
	reg_value |= (1 << USB0_PHY_CLK_ONOFF_BIT);
	writel(reg_value, CCMU_USB0_CLK_REG);

	__usdelay(500);

	reg_value = readl(CCMU_USB0_CLK_REG);
	reg_value |= (1 << USB0_PHY_RESET_BIT);
	writel(reg_value, CCMU_USB0_CLK_REG);
	__usdelay(500);
}

static u32 prode_which_osc(void)
{
	u32 val = 0;

	val = (readl(XO_CTRL_REG) & (0x3 << 14));
	val = (val >> 14);

	return val;
}


int usb_open_clock(void)
{
	u32 reg_value = 0;
	u32 hosc_type = 0;

	disable_otg_clk_reset_gating();
	disable_phy_clk_reset_gating();
	/*sel HOSC*/
	reg_value = readl(CCMU_USB0_CLK_REG);
	reg_value &= (~(0x1 << USB0_PHY_CLK_SEL_BIT));
	writel(reg_value, CCMU_USB0_CLK_REG);

	hosc_type = prode_which_osc();
	if (hosc_type == HOSC_24M) {
		reg_value = readl(CCMU_USB0_CLK_REG);
		reg_value |= (0x1 << USB0_PHY_CLK_DIV_BIT);
		writel(reg_value, CCMU_USB0_CLK_REG);

		reg_value = readl(SUNXI_USBOTG_BASE + 0x410);
		reg_value &= (~(0x1 << 16));
		writel(reg_value, SUNXI_USBOTG_BASE + 0x410);

	} else if (hosc_type == HOSC_19M) {
		reg_value = readl(CCMU_USB0_CLK_REG);
		reg_value |= (0x1 << USB0_PHY_CLK_DIV_BIT);
		writel(reg_value, CCMU_USB0_CLK_REG);

		reg_value = readl(SUNXI_USBOTG_BASE + 0x410);
		reg_value |= (0x1 << 16);
		writel(reg_value, SUNXI_USBOTG_BASE + 0x410);

	} else if (hosc_type == HOSC_38M) {
		reg_value = readl(CCMU_USB0_CLK_REG);
		reg_value &= (~(0x1 << USB0_PHY_CLK_DIV_BIT));
		writel(reg_value, CCMU_USB0_CLK_REG);

		reg_value = readl(SUNXI_USBOTG_BASE + 0x410);
		reg_value |= (0x1 << 16);
		writel(reg_value, SUNXI_USBOTG_BASE + 0x410);
	}

	reg_value = readl(SUNXI_USBOTG_BASE + 0x420);
	reg_value |= (0x01 << 0);
	writel(reg_value, (SUNXI_USBOTG_BASE + 0x420));
	__msdelay(1);

	enable_otg_clk_reset_gating();
	enable_phy_clk_reset_gating();

	return 0;
}

int usb_close_clock(void)
{
	disable_otg_clk_reset_gating();
	disable_phy_clk_reset_gating();

	return 0;
}
