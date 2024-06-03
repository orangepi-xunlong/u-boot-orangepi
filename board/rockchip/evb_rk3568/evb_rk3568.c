/*
 * SPDX-License-Identifier:     GPL-2.0+
 *
 * (C) Copyright 2020 Rockchip Electronics Co., Ltd
 */

#include <common.h>
#include <dwc3-uboot.h>
#include <usb.h>
#include <dm.h>
#include <asm/gpio.h>
#include <asm/arch/resource_img.h>
#include <sysmem.h>
#include <fdt_support.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_USB_DWC3
static struct dwc3_device dwc3_device_data = {
	.maximum_speed = USB_SPEED_HIGH,
	.base = 0xfcc00000,
	.dr_mode = USB_DR_MODE_PERIPHERAL,
	.index = 0,
	.dis_u2_susphy_quirk = 1,
	.usb2_phyif_utmi_width = 16,
};

int usb_gadget_handle_interrupts(void)
{
	dwc3_uboot_handle_interrupt(0);
	return 0;
}

int board_usb_init(int index, enum usb_init_type init)
{
	return dwc3_uboot_init(&dwc3_device_data);
}
#endif

#define OFFSET_TO_BIT(bit)     (1UL << (bit))

#define REG_L(R)       (R##_l)
#define REG_H(R)       (R##_h)
#define READ_REG(REG)  ((readl(REG_L(REG)) & 0xFFFF) | \
                       ((readl(REG_H(REG)) & 0xFFFF) << 16))
#define WRITE_REG(REG, VAL)    \
{\
       writel(((VAL) & 0xFFFF) | 0xFFFF0000, REG_L(REG)); \
       writel((((VAL) & 0xFFFF0000) >> 16) | 0xFFFF0000, REG_H(REG));\
}
#define CLRBITS_LE32(REG, MASK)        WRITE_REG(REG, READ_REG(REG) & ~(MASK))
#define SETBITS_LE32(REG, MASK)        WRITE_REG(REG, READ_REG(REG) | (MASK))

int rk_board_late_init(void)
{
	int value;
	char dtb_name[32];
	char dtb_ver[10];
	const char *model;

	model = fdt_getprop(gd->fdt_blob, 0, "compatible", NULL);
	if(!strcmp(model, "rockchip,rk3566-orangepi-3b")) {
		strcpy(dtb_ver, "-v2.dtb");
	} else if(!strcmp(model, "rockchip,rk3566-orangepi-cm4")) {
		strcpy(dtb_ver, "-v1.4.dtb");
	} else
		return 0;

#define GPIO4_BASE      0xfe770000
	struct rockchip_gpio_regs *regs = (void *)GPIO4_BASE;

	CLRBITS_LE32(&regs->swport_ddr, OFFSET_TO_BIT(20));
	value = readl(&regs->ext_port) & OFFSET_TO_BIT(20) ? 1 : 0;

	if(value) {
		char *fdtfile = env_get("fdtfile");
		strcpy(dtb_name, fdtfile);
		sprintf(dtb_name + strlen(dtb_name) - 4, "%s", dtb_ver);
		env_set("fdtfile", dtb_name);
	}

	return 0;
}
