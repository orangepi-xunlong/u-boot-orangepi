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
 */
#include <common.h>
#include <sunxi_board.h>
#include <sunxi_verify_boot_info.h>
#include <securestorage.h>
#include <smc.h>

#pragma pack(1)
struct sunxi_verify_boot_params {
	uint16_t version;
	uint8_t verify_boot_state;
	uint8_t lock_state;
	uint8_t verify_boot_hash[32];
	uint8_t verify_boot_key[32]; /*rotpk hash*/
};
#pragma pack()
struct sunxi_verify_boot_params verify_boot_params = { 0 };

__maybe_unused void
sunxi_set_verify_boot_info(uint32_t mask, const uint8_t *data, size_t data_len)
{
	switch (mask) {
	case SUNXI_VB_INFO_HASH:
		memcpy(verify_boot_params.verify_boot_hash, data,
		       data_len > 32 ? 32 : data_len);
		break;
	case SUNXI_VB_INFO_KEY:
		memcpy(verify_boot_params.verify_boot_key, data,
		       data_len > 32 ? 32 : data_len);
		break;
	case SUNXI_VB_INFO_LOCK:
		verify_boot_params.lock_state = *data;
		break;
	case SUNXI_VB_INFO_BOOTSTATE:
		verify_boot_params.verify_boot_state = *data;
		break;
	}
}

#if !defined(CONFIG_SUNXI_VERIFY_BOOT_INFO_INSTALL)
void sunxi_set_verify_boot_blob(uint32_t mask, const uint8_t *data,
				size_t data_len)
{
}

void sunxi_set_verify_boot_number(uint32_t mask, uint32_t data)
{
}

int sunxi_keymaster_verify_boot_params_install(void)
{
	return 0;
}
#else
void sunxi_set_verify_boot_blob(uint32_t mask, const uint8_t *data,
				size_t data_len)
{
	sunxi_set_verify_boot_info(mask, data, data_len);
}

void sunxi_set_verify_boot_number(uint32_t mask, uint32_t data)
{
	uint8_t actual = data;
	sunxi_set_verify_boot_info(mask, &actual, 1 /*dummy*/);
}

DECLARE_GLOBAL_DATA_PTR;
int sunxi_keymaster_verify_boot_params_install(void)
{
	sunxi_secure_storage_info_t param_object;
	int workmode;
	int ret;
	workmode = get_boot_work_mode();
	if (workmode != WORK_MODE_BOOT)
		return 0;

	if (gd->securemode != SUNXI_SECURE_MODE_WITH_SECUREOS)
		return 0;

	verify_boot_params.version = 1;

	strcpy(param_object.name, "verify-boot");
	param_object.encrypted     = 0;
	param_object.write_protect = 0;
	param_object.len	   = sizeof(struct sunxi_verify_boot_params);
	memcpy(param_object.key_data, &verify_boot_params, param_object.len);
	ret = smc_tee_keybox_store(param_object.name, (void *)&param_object,
				   sizeof(param_object));
	if (ret) {
		pr_error("install verify-boot info failed\n");
	}
	return 0;
}
#endif
