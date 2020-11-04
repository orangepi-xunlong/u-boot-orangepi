
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

void sunxi_fake_poweroff(void)
{
	arm_svc_fake_poweroff();
}

int atf_box_standby(void)
{
	if (get_boot_work_mode() != WORK_MODE_BOOT) {
		return 0;
	}

	sunxi_fake_poweroff();
	return 0;
}

