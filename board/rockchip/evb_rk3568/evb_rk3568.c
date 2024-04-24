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

int rk_board_late_init(void)
{
	int ret, value;
	unsigned int gpio = 148;
	char dtb_name[32];
	const char *model;

	model = fdt_getprop(gd->fdt_blob, 0, "compatible", NULL);
	if(strcmp(model, "rockchip,rk3566-orangepi-3b"))
		return 0;

	ret = gpio_request(gpio, "gpio4_c4");
	if (ret && ret != -EBUSY) {
		printf("gpio: requesting pin %u failed\n", gpio);
		return -1;
	}

	gpio_direction_input(gpio);
	value = gpio_get_value(gpio);
	//printf("gpio value is %d\n", value);

	if(value) {
		char *fdtfile = env_get("fdtfile");
		strcpy(dtb_name, fdtfile);
		sprintf(dtb_name + strlen(dtb_name) - 4, "%s", "-v2.dtb");
		env_set("fdtfile", dtb_name);
	}

	return 0;
}
