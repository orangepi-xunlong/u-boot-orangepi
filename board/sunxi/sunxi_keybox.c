/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
/*
 * manage key involved with secure os, usually
 * encrypt/decrypt by secure os
 */

#include <common.h>
#include <malloc.h>
#include <securestorage.h>
#include <sunxi_board.h>
#include <smc.h>
#include <sunxi_hdcp_key.h>

DECLARE_GLOBAL_DATA_PTR;

#define KEY_NAME_MAX_SIZE 64
static uint8_t key_list_inited;
static char (*key_list)[KEY_NAME_MAX_SIZE];
static char key_list_cnt;

static int sunxi_keybox_init_key_list_from_env(void)
{
	char *command_p = NULL;
	int key_count   = 0;
	int key_idx = 0, key_name_char_idx = 0;

	if (key_list_inited)
		return -1;
	command_p = env_get("keybox_list");
	if (!command_p) {
		pr_msg("keybox_list not set in env, leave empty\n");
		key_list_inited = 1;
		return 0;
	}
	/*key count equals ',' count plue one */
	key_count = 1;
	while (*command_p != '\0') {
		if (*command_p == ',') {
			key_count++;
		}
		command_p++;
	}

	key_list = (char(*)[KEY_NAME_MAX_SIZE])malloc(KEY_NAME_MAX_SIZE *
						      key_count);
	if (key_list == NULL)
		return -2;
	key_idx		  = 0;
	key_name_char_idx = 0;
	command_p	 = env_get("keybox_list");
	while (*command_p != '\0') {
		if (*command_p == ',') {
			key_list[key_idx][key_name_char_idx] = '\0';
			key_idx++;
			key_name_char_idx = 0;
			if (key_idx >= key_count) {
				break;
			}
		} else if (*command_p == ' ') {
			/*skip space*/
		} else if (key_name_char_idx < KEY_NAME_MAX_SIZE) {
			key_list[key_idx][key_name_char_idx] = *command_p;
			key_name_char_idx++;
		}
		command_p++;
	}
	key_list[key_idx][key_name_char_idx] = '\0';

	key_list_cnt    = key_count;
	key_list_inited = 1;

	return 0;
}

static __maybe_unused int sunxi_keybox_init_key_list(char *key_name[],
						     int keyCount)
{
	int i;
	if (key_list_inited)
		return -1;
	key_list = (char(*)[KEY_NAME_MAX_SIZE])malloc(KEY_NAME_MAX_SIZE *
						      keyCount);
	if (key_list == NULL)
		return -2;
	for (i = 0; i < keyCount; i++) {
		strncpy(key_list[i], key_name[i], KEY_NAME_MAX_SIZE);
		key_list[i][KEY_NAME_MAX_SIZE - 1] = '\0';
	}
	key_list_cnt    = keyCount;
	key_list_inited = 1;
	return 0;
}

static int sunxi_keybox_install_keys(void)
{
	int i;
	int ret;
	sunxi_secure_storage_info_t secure_object;

	if (key_list_inited == 0)
		return -1;

	memset(&secure_object, 0, sizeof(secure_object));
	for (i = 0; i < key_list_cnt; i++) {
		ret = sunxi_secure_object_up(key_list[i],
					     (void *)&secure_object,
					     sizeof(secure_object));
		if (ret) {
			pr_err("secure storage read %s fail with:%d\n",
			       key_list[i], ret);
			continue;
		}

		ret = smc_tee_keybox_store(key_list[i], (void *)&secure_object,
					   sizeof(secure_object));
		if (ret) {
			pr_err("key install %s fail with:%d\n", key_list[i],
			       ret);
			continue;
		}

#if defined(CONFIG_SUNXI_HDCP_IN_SECURESTORAGE)
		if ((strlen("hdcpkey") == strlen(key_list[i])) &&
		    (strcmp("hdcpkey", key_list[i]) == 0)) {
			ret = sunxi_hdcp_key_post_install();
			if (ret) {
				pr_err("key %s post install process failed\n",
				       key_list[i]);
			}
		}
#endif
	}
	return 0;
}

int sunxi_keybox_has_key(char *key_name)
{
	int i;
	int ret = 0;
	for (i = 0; i < key_list_cnt; i++) {
		if (strcmp(key_name, key_list[i]) == 0) {
			ret = 1;
		}
	}
	return ret;
}

int sunxi_keybox_init(void)
{
	int ret;
	int workmode;
	workmode = get_boot_work_mode();
	if (workmode != WORK_MODE_BOOT)
		return 0;

	if (gd->securemode != SUNXI_SECURE_MODE_WITH_SECUREOS) {
		pr_msg("no secure os for keybox operation\n");
		return 0;
	}
	ret = sunxi_keybox_init_key_list_from_env();
	if (ret != 0)
		pr_err("sunxi keybox read env failed with:%d", ret);
	sunxi_secure_storage_init();
	ret = sunxi_keybox_install_keys();
	if (ret != 0)
		pr_err("sunxi keybox install failed with:%d", ret);
	return 0;
}
