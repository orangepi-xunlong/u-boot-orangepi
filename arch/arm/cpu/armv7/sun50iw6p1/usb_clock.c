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


int usb_open_clock(void)
{
	u32 reg_value = 0;

	//USB0 Clock Reg
	//bit30: USB PHY0 reset
	//Bit29: Gating Special Clk for USB PHY0
	reg_value = readl(SUNXI_CCM_BASE + 0xA70);
	reg_value |= (1 << 29) | (1 << 30);
	writel(reg_value, (SUNXI_CCM_BASE + 0xA70));
	//delay some time
	__msdelay(1);

	//USB BUS Gating Reset Reg
	//bit8:USB_OTG Gating
	reg_value = readl(SUNXI_CCM_BASE + 0xA8C);
	reg_value |= (1 << 8);
	writel(reg_value, (SUNXI_CCM_BASE + 0xA8C));

	//delay to wati SIE stable
	__msdelay(1);

	//USB BUS Gating Reset Reg: USB_OTG reset
	reg_value = readl(SUNXI_CCM_BASE + 0xA8C);
	reg_value |= (1 << 24);
	writel(reg_value, (SUNXI_CCM_BASE + 0xA8C));
	__msdelay(1);

	reg_value = readl(SUNXI_USBOTG_BASE + 0x420);
	reg_value |= (0x01 << 0);
	writel(reg_value, (SUNXI_USBOTG_BASE + 0x420));
	__msdelay(1);

	return 0;
}
int usb_close_clock(void)
{
	u32 reg_value = 0;

	//USB0 Clock Reg
	//bit30: USB PHY0 reset
	//Bit29: Gating Special Clk for USB PHY0
	reg_value = readl(SUNXI_CCM_BASE + 0xA70);
	reg_value &= ~((1 << 29) | (1 << 30));
	writel(reg_value, (SUNXI_CCM_BASE + 0xA70));
	__msdelay(1);

	//USB BUS Gating Reset Reg
	//bit8:USB_OTG Gating
	reg_value = readl(SUNXI_CCM_BASE + 0xA8C);
	reg_value &= ~(1 << 8);
	writel(reg_value, (SUNXI_CCM_BASE + 0xA8C));

	//delay to wati SIE stable
	__msdelay(1);

	//USB BUS Gating Reset Reg: USB_OTG reset
	reg_value = readl(SUNXI_CCM_BASE + 0xA8C);
	reg_value &= ~(1 << 24);
	writel(reg_value, (SUNXI_CCM_BASE + 0xA8C));
	__msdelay(1);
	return 0;
}
