/*
 * (C) Copyright 2021 allwinnertech  <huangrongcun@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <sprite.h>

static int do_sunxi_sysrecovery_test(cmd_tbl_t *cmdtp, int flag, int argc,
				     char *const argv[])
{
	__maybe_unused int ret;
	printf("run sprite recovery\n");
	sprite_led_init();
	ret = sprite_form_sysrecovery();
	sprite_led_exit(ret);
	return ret;
}

U_BOOT_CMD(sunxi_sysrecovery_test, 1, 1, do_sunxi_sysrecovery_test,
	   "-do a sysrecovery test", "NULL\n");
