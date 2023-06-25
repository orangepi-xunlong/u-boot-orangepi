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
#include <sunxi_keybox.h>

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

static int search_key_in_linklist(const char *name)
{
	struct sunxi_key_t *start =
		ll_entry_start(struct sunxi_key_t, sunxi_keys);
	const int len = ll_entry_count(struct sunxi_key_t, sunxi_keys);
	int i;
	for (i = 0; i < len; i++) {
		if ((strlen(name) == strlen(start[i].name)) &&
		    (strcmp(name, start[i].name) == 0))
			return i;
	}
	return -1;
}

static int try_reencrypt_and_install(const char *name, int replace)
{
	sunxi_secure_storage_info_t secdata;
	int data_len;
	int ret;
	char old_data[4096];
	memset(old_data, 0, 4096);
	ret = sunxi_secure_object_read(name, old_data, sizeof(old_data),
				       &data_len);
	if (ret)
		return ret;

	memset(&secdata, 0, sizeof(secdata));
	ret = sunxi_secure_object_build(name, old_data, data_len, 0, 0,
					(char *)&secdata);

	ret = smc_tee_keybox_store(name, (void *)&secdata, sizeof(secdata));
	if (ret) {
		return -1;
	} else {
		if (replace) {
			pr_msg("re_encrypt works, replace current data with reencrypted data\n");
			ret = sunxi_secure_object_write(
				secdata.name, (void *)&secdata,
				SUNXI_SECURE_STORTAGE_INFO_HEAD_LEN +
					secdata.len);
			if (ret)
				pr_msg("replace failed\n");
		}
		return 0;
	}
}

#define INSTALL_SUCCESS_AFTER_REENC 1
static int default_keybox_installation(const char *name)
{
	sunxi_secure_storage_info_t secure_object;
	memset(&secure_object, 0, sizeof(secure_object));
	int ret;
	ret = sunxi_secure_object_up(name, (void *)&secure_object,
				     sizeof(secure_object));
	if (ret) {
		pr_err("secure storage read %s fail with:%d\n", name, ret);
		return -1;
	}

	ret = smc_tee_keybox_store(name, (void *)&secure_object,
				   sizeof(secure_object));
	if (ret) {
		pr_err("key install %s fail with:%d\n", name, ret);

		if (strcmp(name, secure_object.name) != 0) {
			pr_msg("try reencrypt and install key %s\n", name);
			ret = try_reencrypt_and_install(name, 1);
			if (!ret) {
				return INSTALL_SUCCESS_AFTER_REENC;
			}
		}
		return -1;
	}
	return 0;
}

static int sunxi_keybox_install_keys(void)
{
	int i;
	int ret;
	struct sunxi_key_t *start =
		ll_entry_start(struct sunxi_key_t, sunxi_keys);
	int flush_required = 0;

	if (key_list_inited == 0)
		return -1;

	for (i = 0; i < key_list_cnt; i++) {
		ret = search_key_in_linklist(key_list[i]);

		if ((ret >= 0) && (start[ret].key_load_cb != NULL)) {
			pr_msg("load key %s with regesited cb\n", key_list[i]);
			start[ret].key_load_cb(key_list[i]);
			continue;
		}

		pr_msg("load key %s with default cb\n", key_list[i]);
		/* deafult behavior */
		ret = default_keybox_installation(key_list[i]);
		if (ret == INSTALL_SUCCESS_AFTER_REENC) {
			/* reencrypt include a writing, need flush afterward */
			flush_required = 1;
		} else if (ret) {
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
	if (flush_required) {
		sunxi_secstorage_flush();
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
	if (!ret)
		ret = (search_key_in_linklist(key_name) >= 0);
	return ret;
}

int sunxi_keybox_init(void)
{
	int ret;
	int workmode;
	workmode = get_boot_work_mode();
	if (workmode != WORK_MODE_BOOT)
		return 0;

	if (sunxi_probe_secure_os() == 0) {
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

int sunxi_keybox_burn_key(const char *name, char *buf, int key_len, int encrypt,
			  int write_protect)
{
	struct sunxi_key_t *start =
		ll_entry_start(struct sunxi_key_t, sunxi_keys);
	int i;
	i = search_key_in_linklist(name);
	if ((i >= 0) && (start[i].key_burn_cb != NULL)) {
		pr_msg("burning key %s with regesited cb\n", name);
		return start[i].key_burn_cb(name, buf, key_len, encrypt,
					    write_protect);
	}
	/* default behavior */
	pr_msg("burning key %s with default cb\n", name);
	return sunxi_secure_object_down(name, buf, key_len, encrypt,
					write_protect);
}

#ifdef CONFIG_SUNXI_ANDROID_BOOT
/* android keybox keys */
SUNXI_KEYBOX_KEY(widevine, NULL, NULL);

const static char *android_trust_chain_map[8] = { "ec_key",    "ec_cert1",
						  "ec_cert2",  "ec_cert3",
						  "rsa_key",   "rsa_cert1",
						  "rsa_cert2", "rsa_cert3" };

enum ANDROID_TRUST_CHAIN_INSTALL_STATUS_EN{
	INSTALLING,
	INSTALL_FAILED
};

int android_trust_chain_load_cb(const char *name)
{
	/*
	 * ignore input name, use fix order to load key
	 * so do not need walk through trust chain key
	 * name every single install
	 */
	static int idx			      = 0;
	static int trust_chain_install_status = INSTALLING;
	pr_msg("trust chain key, load in fix order,"
			"input key name ignored\n");
	switch (trust_chain_install_status) {
	case INSTALL_FAILED:
		/*already installed, meaning less to install the rest*/
		return -2;
		break;
	case INSTALLING:
	default:
		break;
	}
	if (default_keybox_installation(android_trust_chain_map[idx]) != 0) {
		trust_chain_install_status = INSTALL_FAILED;
		return -2;
	} else {
		if ((idx == ARRAY_SIZE(android_trust_chain_map) - 1) &&
		    (trust_chain_install_status != INSTALL_FAILED)) {
			/* all success installed*/
			env_set("android_trust_chain", "true");
		}
	}
	idx++;
	return 0;
}
SUNXI_KEYBOX_KEY(ec_key, NULL, android_trust_chain_load_cb);
SUNXI_KEYBOX_KEY(ec_cert1, NULL, android_trust_chain_load_cb);
SUNXI_KEYBOX_KEY(ec_cert2, NULL, android_trust_chain_load_cb);
SUNXI_KEYBOX_KEY(ec_cert3, NULL, android_trust_chain_load_cb);
SUNXI_KEYBOX_KEY(rsa_key, NULL, android_trust_chain_load_cb);
SUNXI_KEYBOX_KEY(rsa_cert1, NULL, android_trust_chain_load_cb);
SUNXI_KEYBOX_KEY(rsa_cert2, NULL, android_trust_chain_load_cb);
SUNXI_KEYBOX_KEY(rsa_cert3, NULL, android_trust_chain_load_cb);
#endif

#if defined (CONFIG_SUNXI_SDMMC) && defined (CONFIG_SUPPORT_EMMC_RPMB)
__weak int sunxi_mmc_rpmb_burn_key(char *buf)
{
	return -5;
}
#define RPMB_SZ_MAC 32
#define RPMB_SZ_DATA 256
int rpmb_key_burn(__maybe_unused const char *name, char *buf, int len,
		  __maybe_unused int encrypt, __maybe_unused int write_protect)
{
	int ret;
	int storage_type = get_boot_storage_type();
	switch (storage_type) {
	case STORAGE_EMMC:
	case STORAGE_EMMC0:
	case STORAGE_EMMC3:
		break;
	default:
		pr_err("not supported storage_type:%d\n", storage_type);
		return -1;
	}
	if (len != RPMB_SZ_MAC) {
		pr_err("invalid lengh %d, expected %d\n", len, RPMB_SZ_MAC);
		return -2;
	}
	ret = sunxi_mmc_rpmb_burn_key(buf);
	if (ret == -3) {
		pr_msg("rpmb key burned, key valid, only store key\n");
	} else if (ret == -2) {
		pr_err("rpmb key burn failed, key not valid, skipped\n");
		return ret;
	} else if (!ret) {
		pr_err("rpmb burn key failed\n");
		return ret;
	}
	return sunxi_secure_object_down("rpmb_key", buf, len, 0, 1);
};
SUNXI_KEYBOX_KEY(rpmb_key, rpmb_key_burn, NULL);
#endif
