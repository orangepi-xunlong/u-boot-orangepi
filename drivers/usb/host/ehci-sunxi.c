/*
 * SPDX-License-Identifier: GPL-2.0+
 * (C) Copyright 20016-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 */

#include <common.h>
#include <asm/arch/clock.h>
#include <asm/arch/usb_phy.h>
#include <asm/io.h>
#include <dm.h>
#include <asm/arch/gpio.h>
#include <asm/gpio.h>
#include <asm/arch/clock.h>
#include "ehci.h"


#define SUNXI_USB_CTRL		0x800
#define SUNXI_USB_PHY_CTRL	0x810
#define HOST_MAXNUM		5

static uintptr_t usb_vbase = SUNXI_EHCI0_BASE;
unsigned int usb_drv_vbus_gpio = -1;

typedef struct _ehci_config
{
	uintptr_t ehci_base;
	u32 bus_soft_reset_ofs;
	u32 bus_clk_gating_ofs;
	u32 phy_reset_ofs;
	u32 phy_slk_gatimg_ofs;
	u32 usb0_support;
	char name[32];
}ehci_config_t;


static ehci_config_t ehci_cfg[HOST_MAXNUM] = {
	{SUNXI_EHCI0_BASE, USBEHCI0_RST_BIT, USBEHCI0_GATIING_BIT, USBPHY0_RST_BIT, USBPHY0_SCLK_GATING_BIT, 1, "USB-DRD"},
#ifdef SUNXI_EHCI1_BASE
	{SUNXI_EHCI1_BASE, USBEHCI1_RST_BIT, USBEHCI1_GATIING_BIT, USBPHY1_RST_BIT, USBPHY1_SCLK_GATING_BIT, 0, "USB1-Host"},
#endif
#ifdef SUNXI_EHCI2_BASE
	{SUNXI_EHCI2_BASE, USBEHCI2_RST_BIT, USBEHCI2_GATIING_BIT, USBPHY2_RST_BIT, USBPHY2_SCLK_GATING_BIT, 0, "USB2-Host"},
#endif
#ifdef SUNXI_EHCI3_BASE
	{SUNXI_EHCI3_BASE, USBEHCI3_RST_BIT, USBEHCI3_GATIING_BIT, USBPHY3_RST_BIT, USBPHY3_SCLK_GATING_BIT, 0, "USB3-Host"},
#endif
};


/*Port:port+num<fun><pull><drv><data>*/
ulong config_usb_pin(int index)
{

	if (index == 0)
		usb_drv_vbus_gpio = sunxi_name_to_gpio(CONFIG_USB0_VBUS_PIN);
	else if (index == 1)
		usb_drv_vbus_gpio = sunxi_name_to_gpio(CONFIG_USB1_VBUS_PIN);
	else if (index == 2)
		usb_drv_vbus_gpio = sunxi_name_to_gpio(CONFIG_USB2_VBUS_PIN);
	else
		usb_drv_vbus_gpio = sunxi_name_to_gpio(CONFIG_USB3_VBUS_PIN);

	if (usb_drv_vbus_gpio == -1)
		return 0;
	/*usbc0 port:PB0<0><1><default><default>*/
	/*usbc1 port:PB1<0><1><default><default>*/
	sunxi_gpio_set_cfgpin(usb_drv_vbus_gpio, 0);
	sunxi_gpio_set_pull(usb_drv_vbus_gpio, SUNXI_GPIO_PULL_UP);

	/* set cfg, ouput */
	sunxi_gpio_set_cfgpin(usb_drv_vbus_gpio, 1);

	/* reserved is pull down */
	sunxi_gpio_set_pull(usb_drv_vbus_gpio, 2);

	return 0;
}

void sunxi_set_vbus(int onoff)
{
	if (usb_drv_vbus_gpio == -1)
		return;

	gpio_set_value(usb_drv_vbus_gpio, onoff);
	return;
}

s32 __attribute__((weak)) axp_usb_vbus_output(void){ return 0;}

int alloc_pin(int index)
{
	if (axp_usb_vbus_output())
		return 0;
	config_usb_pin(index);
	return 0;
}

void free_pin(void)
{
	return;
}

u32 open_usb_clock(int index)
{
	u32 reg_value = 0;

	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	/* Bus reset and gating for ehci */
	reg_value = readl(&ccm->usb_gate_reset);
	reg_value |= (1 << ehci_cfg[index].bus_soft_reset_ofs);
	reg_value |= (1 << ehci_cfg[index].bus_clk_gating_ofs);
	writel(reg_value, &ccm->usb_gate_reset);

#if defined(CONFIG_MACH_SUN50IW9)
	reg_value = readl(&ccm->usb2_clk_cfg);
	reg_value |= (1 << ehci_cfg[2].phy_slk_gatimg_ofs);
	reg_value |= (1 << ehci_cfg[2].phy_reset_ofs);
	writel(reg_value, &ccm->usb2_clk_cfg);
#endif
	/* open clk for usb phy */
	if (index == 0) {
		reg_value = readl(&ccm->usb0_clk_cfg);
		reg_value |= (1 << ehci_cfg[index].phy_slk_gatimg_ofs);
		reg_value |= (1 << ehci_cfg[index].phy_reset_ofs);
		writel(reg_value, &ccm->usb0_clk_cfg);
	} else if (index == 1) {
		reg_value = readl(&ccm->usb1_clk_cfg);
		reg_value |= (1 << ehci_cfg[index].phy_slk_gatimg_ofs);
		reg_value |= (1 << ehci_cfg[index].phy_reset_ofs);
		writel(reg_value, &ccm->usb1_clk_cfg);
	} else if (index == 2) {
#if defined(CONFIG_MACH_SUN50IW9)
		/*noting to do*/
#else
#ifdef SUNXI_EHCI2_BASE
		reg_value = readl(&ccm->usb2_clk_cfg);
		reg_value |= (1 << ehci_cfg[index].phy_slk_gatimg_ofs);
		reg_value |= (1 << ehci_cfg[index].phy_reset_ofs);
		writel(reg_value, &ccm->usb2_clk_cfg);
#endif
#endif
	} else {
#ifdef SUNXI_EHCI3_BASE
		reg_value = readl(&ccm->usb3_clk_cfg);
		reg_value |= (1 << ehci_cfg[index].phy_slk_gatimg_ofs);
		reg_value |= (1 << ehci_cfg[index].phy_reset_ofs);
		writel(reg_value, &ccm->usb3_clk_cfg);
#endif
	}
	printf("config usb clk ok\n");
	return 0;
}

u32 close_usb_clock(int index)
{
	u32 reg_value = 0;
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	/* Bus reset and gating for ehci */
	reg_value = readl(&ccm->usb_gate_reset);
	reg_value &= ~(1 << ehci_cfg[index].bus_soft_reset_ofs);
	reg_value &= ~(1 << ehci_cfg[index].bus_clk_gating_ofs);
	writel(reg_value, &ccm->usb_gate_reset);

	/* close clk for usb phy */
	if (index == 0) {
		reg_value = readl(&ccm->usb0_clk_cfg);
		reg_value &= ~(1 << ehci_cfg[index].phy_slk_gatimg_ofs);
		reg_value &= ~(1 << ehci_cfg[index].phy_reset_ofs);
		writel(reg_value, &ccm->usb0_clk_cfg);
	} else if (index == 1) {
		reg_value = readl(&ccm->usb1_clk_cfg);
		reg_value &= ~(1 << ehci_cfg[index].phy_slk_gatimg_ofs);
		reg_value &= ~(1 << ehci_cfg[index].phy_reset_ofs);
		writel(reg_value, &ccm->usb1_clk_cfg);
	} else if (index == 2) {
#if defined(CONFIG_MACH_SUN50IW9)
		/*noting to do*/
#else
#ifdef SUNXI_EHCI2_BASE
		reg_value = readl(&ccm->usb2_clk_cfg);
		reg_value &= ~(1 << ehci_cfg[index].phy_slk_gatimg_ofs);
		reg_value &= ~(1 << ehci_cfg[index].phy_reset_ofs);
		writel(reg_value, &ccm->usb2_clk_cfg);
#endif
#endif
	} else {
#ifdef SUNXI_EHCI3_BASE
		reg_value = readl(&ccm->usb3_clk_cfg);
		reg_value &= ~(1 << ehci_cfg[index].phy_slk_gatimg_ofs);
		reg_value &= ~(1 << ehci_cfg[index].phy_reset_ofs);
		writel(reg_value, &ccm->usb3_clk_cfg);
#endif
	}

	return 0;
}


void usb_passby(int index, u32 enable)
{
	unsigned long reg_value = 0;
	uintptr_t ehci_vbase = ehci_cfg[index].ehci_base;

	if(ehci_cfg[index].usb0_support)
	{
		/* the default mode of usb0 is OTG,so change it here. */
		reg_value = readl((void *)(SUNXI_USBOTG_BASE + 0x420));
		reg_value &= ~(0x01);
		writel(reg_value, (void *)(SUNXI_USBOTG_BASE + 0x420));
	}

	reg_value = readl((void *)(ehci_vbase + SUNXI_USB_PHY_CTRL));
	reg_value &= ~(0x01 << 3);
	writel(reg_value, (void *)(ehci_vbase + SUNXI_USB_PHY_CTRL));

	reg_value = readl((void *)(ehci_vbase + SUNXI_USB_CTRL));
	if (enable) {
		reg_value |= (1 << 10);		/* AHB Master interface INCR8 enable */
		reg_value |= (1 << 9);     	/* AHB Master interface burst type INCR4 enable */
		reg_value |= (1 << 8);     	/* AHB Master interface INCRX align enable */
		reg_value |= (1 << 0);     	/* ULPI bypass enable */
	} else if(!enable) {
		reg_value &= ~(1 << 10);	/* AHB Master interface INCR8 disable */
		reg_value &= ~(1 << 9);     /* AHB Master interface burst type INCR4 disable */
		reg_value &= ~(1 << 8);     /* AHB Master interface INCRX align disable */
		reg_value &= ~(1 << 0);     /* ULPI bypass disable */
	}
	writel(reg_value, (void *)(ehci_vbase + SUNXI_USB_CTRL));

	return;
}


int sunxi_start_ehci(int index)
{
	if(alloc_pin(index))
		return -1;
	open_usb_clock(index);
	usb_passby(index, 1);
	sunxi_set_vbus(1);
	mdelay(800);
	return 0;
}

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
	*hcor = (struct ehci_hcor *)((uintptr_t)(*hccr) +
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
