/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <sys_config.h>
#include <securestorage.h>
#include <private_uboot.h>

DECLARE_GLOBAL_DATA_PTR;

extern int do_burn_from_boot(cmd_tbl_t *cmdtp, int flag, int argc,
			     char *const argv[]);

//#define  SUNXI_SECURESTORAGE_TEST_ERASE

int sunxi_keydata_burn_by_usb(void)
{
	char buffer[512];
	__maybe_unused int data_len;
	__maybe_unused int ret = 0;
	__maybe_unused uint burn_private_start;
	__maybe_unused uint burn_private_len;

	int workmode	 = uboot_spare_head.boot_data.work_mode;
	int if_need_burn_key = 0;
	char *env_burn_key;
	env_burn_key = env_get("burn_key");
	if (env_burn_key) {
		if_need_burn_key = simple_strtoul(env_burn_key, NULL, 16);
	} else {
		ret = script_parser_fetch("/soc/target", "burn_key", &if_need_burn_key, 1);
	}

	if (if_need_burn_key != 1) {
		pr_err("out of usb burn from boot: not need burn key\n");
		return 0;
	}

	if (workmode != WORK_MODE_BOOT) {
		pr_err("out of usb burn from boot: not boot mode\n");
		return 0;
	}
	memset(buffer, 0, 512);
#ifdef CONFIG_SUNXI_SECURE_STORAGE
	if (sunxi_secure_storage_init()) {
		pr_err("sunxi secure storage is not supported\n");
	} else {
#ifndef SUNXI_SECURESTORAGE_TEST_ERASE
		ret = sunxi_secure_object_read("key_burned_flag", buffer, 512,
					       &data_len);
		if (ret) {
			pr_msg("sunxi secure storage has no flag\n");
		} else {
			if (!strcmp(buffer, "key_burned")) {
				pr_msg("find key burned flag\n");
				return 0;
			}
			pr_msg("do not find key burned flag\n");
		}
#else
		if (!sunxi_secure_storage_erase("key_burned_flag"))
			sunxi_secure_storage_exit();

		return 0;
#endif
	}
#endif
	return do_burn_from_boot(NULL, 0, 0, NULL);
}
