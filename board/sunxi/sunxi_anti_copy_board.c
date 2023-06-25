/*
 * (C) Copyright 2017-2018
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 * Configuration settings for the Allwinner sunxi series of boards.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <malloc.h>
#include <fdt_support.h>
#include <sunxi_board.h>
#include <asm/io.h>
#include <securestorage.h>
#include <asm/arch/ce.h>
#include <memalign.h>

static int sunxi_get_copy_board_data(char *name, u8 *buf, int *len)
{
	int ret;
	int retLen;

	if (sunxi_secure_storage_init()) {
		pr_err("%s secure storage init failed\n", __func__);
		return -1;
	}

	ret = sunxi_secure_object_read(name, (void *)buf, 4096,
					(int *)&retLen);
	if (ret < 0) {
		pr_err("Error: read cipher object fail\n");
		return -1;
	}
	pr_err("retLen = %d\n", retLen);
	*len = retLen;
	sunxi_dump(buf, retLen);

	if (sunxi_secure_storage_exit()) {
		pr_err("%s secure storage exit failed\n", __func__);
		return -1;
	}

	return 0;
}

int sunxi_anti_copy_board(void)
{
	int ret = -1;
	int cipher_len;
	int plaintext_len;
	struct aes_crypt_info_t aes_info;
	void *cipher_buf = NULL;
	void *plaintext_buf = NULL;
	void *dst_buf = NULL;

	cipher_buf = (char *)memalign(CONFIG_SYS_CACHELINE_SIZE, 4096);
	if (!cipher_buf) {
		pr_err("%s: malloc cipher_buf failed\n", __func__);
		return -1;
	}

	plaintext_buf = (char *)memalign(CONFIG_SYS_CACHELINE_SIZE, 4096);
	if (!plaintext_buf) {
		pr_err("%s: malloc plaintext_buf failed\n", __func__);
		return -1;
	}

	dst_buf = (char *)memalign(CONFIG_SYS_CACHELINE_SIZE, 4096);
	if (!dst_buf) {
		pr_err("%s: malloc dst_buf failed\n", __func__);
		goto fail;
	}

	ret = sunxi_get_copy_board_data("cipher", cipher_buf, &cipher_len);
	if (ret) {
		pr_err("get cipher data failed\n");
		goto fail;
	}

	ret = sunxi_get_copy_board_data("plaintext", plaintext_buf, &plaintext_len);
	if (ret) {
		pr_err("get plaintext data failed\n");
		goto fail;
	}

	memset(&aes_info, 0x0, sizeof(struct aes_crypt_info_t));
	aes_info.src_buf = cipher_buf;
	aes_info.src_len = cipher_len;
	aes_info.dst_buf = dst_buf;
	aes_info.enc_flag = AES_DECRYPT;
	aes_info.aes_mode = AES_MODE_ECB;
	aes_info.key_type = KEY_TYPE_SSK;
	ret = sunxi_do_aes_crypt(&aes_info);
	if (ret) {
		pr_err("sunxi_crypt_data failed\n");
		goto fail;
	}

	ret = memcmp(dst_buf, plaintext_buf, plaintext_len);
	if (ret) {
		pr_err("board data compare fail\n");
		sunxi_dump(dst_buf, plaintext_len);
		goto fail;
	}

	ret = 0;
fail:
	if (cipher_buf)
		free(cipher_buf);
	if (plaintext_buf)
		free(plaintext_buf);
	if (dst_buf)
		free(dst_buf);
	return ret;
}
