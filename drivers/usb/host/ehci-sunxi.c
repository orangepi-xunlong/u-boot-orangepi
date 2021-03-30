/*
 * (C) Copyright 20016-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
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

#include <common.h>
#include <pci.h>
#include <usb.h>
#include <asm/io.h>
#include <usb/ehci-fsl.h>
#include <sys_config.h>

#include "ehci.h"
#include "ehci-sunxi.h"

static u32 usb_vbase = SUNXI_EHCI1_BASE;
static u32 usb_vbus_handle = 0;

typedef struct _ehci_config
{
	u32 ehci_base;
	u32 bus_soft_reset_ofs;
	u32 bus_clk_gating_ofs;
	u32 phy_reset_ofs;
	u32 phy_slk_gatimg_ofs;
	u32 usb0_support;
	char name[32];
	char node[32];
}ehci_config_t;

#ifdef CONFIG_ARCH_SUN8IW11P1
static ehci_config_t ehci_cfg[] =
{
	{SUNXI_EHCI0_BASE,26,26,0,8,1,"ehci0","/soc/usbc0"},
	{SUNXI_EHCI1_BASE,27,27,1,9,0,"ehci1","/soc/usbc1"},
};
#else
static ehci_config_t ehci_cfg[] =
{
	{SUNXI_EHCI0_BASE,24,24,0,8,1,"ehci0","/soc/usbc0"},
	{SUNXI_EHCI1_BASE,25,25,1,9,0,"ehci1","/soc/usbc1"},
};
#endif
/*
*******************************************************************************
*                     pin_init
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
ulong config_usb_pin(char *path, char *prop)
{

	user_gpio_set_t     usbc_gpio;
	int status = -1;
	ulong pin_handle = 0;

	do{
		if(fdt_get_one_gpio(path,prop,&usbc_gpio))
		{
			break;
		}

		pin_handle = gpio_request(&usbc_gpio, 1);
		if(!pin_handle)
		{
			break;
		}

		/* set config, ouput */
		if(gpio_set_one_pin_io_status(pin_handle, 1, NULL))
		{
			break;
		}

		/* reserved is pull down */
		if(gpio_set_one_pin_pull(pin_handle, 2, NULL))
		{
			break;
		}
		status = 0;
	}while(0);
	printf("config usb pin %s\n",status?"fail":"success");
	return status ? 0 : pin_handle;
}

int alloc_pin(int index)
{
	usb_vbus_handle = config_usb_pin(ehci_cfg[index].node,
		"usb_drv_vbus_gpio");
        return usb_vbus_handle ? 0:-1;
}

/*
*******************************************************************************
*                     pin_exit
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
void free_pin(void)
{
	if(usb_vbus_handle)
		gpio_release(usb_vbus_handle, 0);
	usb_vbus_handle = 0;
	return;
}


/*
*******************************************************************************
*                     open_usb_clock
*
* Description:
*
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
u32 open_usb_clock(int index)
{
	u32 reg_value = 0;
	u32 ccmu_base = SUNXI_CCM_BASE;

	//Bus soft reset for USB EHCI
	reg_value = USBC_Readl(ccmu_base + 0x60);
	reg_value |= (1 << ehci_cfg[index].bus_soft_reset_ofs);
	USBC_Writel(reg_value, (ccmu_base + 0x60));

	//BUS clk gating for USB EHCI
	reg_value = USBC_Readl(ccmu_base + 0x2c0);
	reg_value |= (1 << ehci_cfg[index].bus_clk_gating_ofs);
	USBC_Writel(reg_value, (ccmu_base + 0x2c0));

	//open clock for USB PHY
	reg_value = USBC_Readl(ccmu_base + 0xcc);
	reg_value |= (1 << ehci_cfg[index].phy_slk_gatimg_ofs);
	reg_value |= (1 << ehci_cfg[index].phy_reset_ofs);
	USBC_Writel(reg_value, (ccmu_base + 0xcc));

	printf("config usb clk ok\n");
	return 0;
}



/*
*******************************************************************************
*                     close_usb_clock
*
* Description:
*
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
u32 close_usb_clock(int index)
{
	u32 reg_value = 0;
	u32 ccmu_base = SUNXI_CCM_BASE;

	//Bus soft reset for USB EHCI
	reg_value = USBC_Readl(ccmu_base + 0x2c0);
	reg_value &= ~(1 << ehci_cfg[index].bus_soft_reset_ofs);
	USBC_Writel(reg_value, (ccmu_base + 0x2c0));

	//BUS clk gating for USB EHCI
	reg_value = USBC_Readl(ccmu_base + 0x60);
	reg_value &= ~(1 << ehci_cfg[index].bus_clk_gating_ofs);
	USBC_Writel(reg_value, (ccmu_base + 0x60));

	//close clock for USB PHY
	reg_value = USBC_Readl(ccmu_base + 0xcc);
	//PHY0
	reg_value &= ~(1 << ehci_cfg[index].phy_slk_gatimg_ofs);
	reg_value &= ~(1 <<  ehci_cfg[index].phy_reset_ofs);
	USBC_Writel(reg_value, (ccmu_base + 0xcc));

	return 0;
}

/*
*******************************************************************************
*                     enable_usb_passby
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
void usb_passby(int index, u32 enable)
{
	unsigned long reg_value = 0;
	u32 ehci_vbase = ehci_cfg[index].ehci_base;

	if(ehci_cfg[index].usb0_support)
	{
		//the default mode of usb0 is OTG,so change it here.
		reg_value = USBC_Readl(SUNXI_USBOTG_BASE + 0x420);
		reg_value &= ~(0x01);
		USBC_Writel(reg_value, (SUNXI_USBOTG_BASE + 0x420));
	}
	reg_value = USBC_Readl(ehci_vbase + 0x810);
	reg_value &= ~(0x01<<1);
	USBC_Writel(reg_value, (ehci_vbase + 0x810));

	reg_value = USBC_Readl(ehci_vbase + SUNXI_USB_PMU_IRQ_ENABLE);
	if(enable){
		reg_value |= (1 << 10);		/* AHB Master interface INCR8 enable */
		reg_value |= (1 << 9);     	/* AHB Master interface burst type INCR4 enable */
		reg_value |= (1 << 8);     	/* AHB Master interface INCRX align enable */
		reg_value |= (1 << 0);     	/* ULPI bypass enable */
	}else if(!enable){
		reg_value &= ~(1 << 10);	/* AHB Master interface INCR8 disable */
		reg_value &= ~(1 << 9);     /* AHB Master interface burst type INCR4 disable */
		reg_value &= ~(1 << 8);     /* AHB Master interface INCRX align disable */
		reg_value &= ~(1 << 0);     /* ULPI bypass disable */
	}
        USBC_Writel(reg_value, (ehci_vbase + SUNXI_USB_PMU_IRQ_ENABLE));

	return;
}

void sunxi_set_vbus(int on_off)
{
	if(usb_vbus_handle)
	        gpio_write_one_pin_value(usb_vbus_handle, on_off, NULL);
	return;
}

/*
*******************************************************************************
*                     sunxi_start_ehci
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
int sunxi_start_ehci(int index)
{
	if(alloc_pin(index))
		return -1;
	open_usb_clock(index);
	usb_passby(index, 1);
	sunxi_set_vbus(1);
	__msdelay(800);
	return 0;
}

/*
*******************************************************************************
*                     sunxi_stop_ehci
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
void sunxi_stop_ehci(int index)
{
	sunxi_set_vbus(0);
	usb_passby(index, 0);
	close_usb_clock(index);
	free_pin();
	return;
}

/*
 * Create the appropriate control structures to manage
 * a new EHCI host controller.
 */
int ehci_hcd_init(int index, enum usb_init_type init,
		struct ehci_hccr **hccr, struct ehci_hcor **hcor)
{
	printf("start sunxi  %s...\n", ehci_cfg[index].name);
	if(index > ARRAY_SIZE(ehci_cfg))
	{
		printf("the index is too large\n");
		return -1;
	}
	usb_vbase = ehci_cfg[index].ehci_base;
	if(sunxi_start_ehci(index))
	{
		return -1;
	}
	*hccr = (struct ehci_hccr *)usb_vbase;
	*hcor = (struct ehci_hcor *)((uint32_t) (*hccr) +
		HC_LENGTH(ehci_readl(&((*hccr)->cr_capbase))));

	printf("sunxi %s init ok...\n", ehci_cfg[index].name);
	return 0;
}

/*
 * Destroy the appropriate control structures corresponding
 * the the EHCI host controller.
 */
int ehci_hcd_stop(int index)
{
	sunxi_stop_ehci(index);
	printf("stop sunxi %s ok...\n", ehci_cfg[index].name);
	return 0;
}
