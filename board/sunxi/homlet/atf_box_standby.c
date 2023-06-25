
/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Liaoyongming <liaoyongming@allwinnertech.com>
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
 */

#include <sunxi_board.h>
#include <smc.h>
#include <spare_head.h>
#include <common.h>
#include <linux/libfdt.h>
#include <securestorage.h>
#include <asm/arch/rtc.h>

void sunxi_fake_poweroff(void)
{
	int ret;

	ret = arm_svc_fake_poweroff((ulong)working_fdt);
	if (!ret) {
#if defined(CONFIG_SUNXI_HOMLET)
		sunxi_boot_init_gpio();
#endif
	}
}

/* parse startup type from factoty menu
 *
 * The result decide homlet/TV startup in fake poweroff
 * or startup direcely.
 *
 * return:
 * 1 if parse result is fake poweroff.
 * 0 if not fake poweroff type or factory menu not support.
 */
uint parse_factory_menu(void)
{
	int ret = 0;
	char buffer[32];
	int data_len;
	char standby_char[10] = "standby";

	if (sunxi_secure_storage_init()) {
		pr_msg("secure storage init fail\n");
		return 0;
	} else {
		/* judge if factory menu has set standby flag */
		ret = sunxi_secure_object_read("BOOTMODE", buffer, 32, &data_len);
		if (ret) {
			pr_msg("sunxi secure storage has no start mode flag\n");
			return 0;
		} else {
			if (!strncmp(buffer, standby_char, strlen(standby_char))) {
				return 1;
			}
		}
	}
	return 0;
}

int atf_box_standby(void)
{

	if (get_boot_work_mode() != WORK_MODE_BOOT) {
		return 0;
	}

	/* only cold start need to judge fake poweroff or not */
	if (parse_factory_menu() > 0) {
		if (rtc_read_data(CONFIG_START_MODE_RTC_REG_INDEX) == 0) {
			rtc_write_data(CONFIG_START_MODE_RTC_REG_INDEX, 0x2);
		}
	}

	sunxi_fake_poweroff();
	return 0;
}

