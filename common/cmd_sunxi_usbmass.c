/*
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: wangwei <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <config.h>
#include <command.h>

DECLARE_GLOBAL_DATA_PTR;

extern int sunxi_usb_dev_register(uint dev_name);
extern void sunxi_usb_main_loop(int mode);

int do_mass_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	printf("run usb mass\n");
	if(sunxi_usb_dev_register(1))
	{
		printf("sunxi usb test: invalid usb device\n");
	}
	sunxi_usb_main_loop(0);

	return 0;
}

U_BOOT_CMD(
	mass_test, 2, 0, do_mass_test,
	"do a usb mass test",
	"NULL"
);

