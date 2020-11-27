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


/*
*******************************************************************************
*                     usb_open_clock
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
int usb_open_clock(void)
{
	u32 reg_value = 0;

#ifdef FPGA_PLATFORM
	//change interfor on  fpga
	reg_value = USBC_Readl(SUNXI_SYSCRL_BASE+0x04);
	reg_value |= 0x01;
	USBC_Writel(reg_value,SUNXI_SYSCRL_BASE+0x04);
#endif

	//Enable module clock for USB phy0
	reg_value = readl(SUNXI_CCM_BASE + 0xcc);
	reg_value |= (1 << 0) | (1 << 8);
	writel(reg_value, (SUNXI_CCM_BASE + 0xcc));
	//delay some time
	__msdelay(10);

	//Gating AHB clock for USB_phy0
	reg_value = readl(SUNXI_CCM_BASE + 0x60);
	reg_value |= (1 << 23);
	writel(reg_value, (SUNXI_CCM_BASE + 0x60));

	//delay to wati SIE stable
	__msdelay(10);

	reg_value = readl(SUNXI_CCM_BASE + 0x2C0);
	reg_value |= (1 << 23);
	writel(reg_value, (SUNXI_CCM_BASE + 0x2C0));
	__msdelay(10);

	reg_value = readl(SUNXI_USBOTG_BASE + 0x420);
	reg_value |= (0x01 << 0);
	writel(reg_value, (SUNXI_USBOTG_BASE + 0x420));
	__msdelay(10);

	reg_value = readl(SUNXI_USBOTG_BASE + 0x410);
	reg_value &= ~(0x01 << 1);
	writel(reg_value, (SUNXI_USBOTG_BASE + 0x410));
	__msdelay(10);
	return 0;
}
/*
*******************************************************************************
*                     usb_op_clock
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
int usb_close_clock(void)
{
    u32 reg_value = 0;

    /* AHB reset */
    reg_value = readl(SUNXI_CCM_BASE + 0x2C0);
    reg_value &= ~(1 << 23);
    writel(reg_value, (SUNXI_CCM_BASE + 0x2C0));
    __msdelay(10);

    //关usb ahb时钟
	reg_value = readl(SUNXI_CCM_BASE + 0x60);
	reg_value &= ~(1 << 23);
	writel(reg_value, (SUNXI_CCM_BASE + 0x60));
    //等sie的时钟变稳
	__msdelay(10);

	//关USB phy时钟
	reg_value = readl(SUNXI_CCM_BASE + 0xcc);
	reg_value &= ~((1 << 0) | (1 << 8));
	writel(reg_value, (SUNXI_CCM_BASE + 0xcc));
	__msdelay(10);

	return 0;
}
