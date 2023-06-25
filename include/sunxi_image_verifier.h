/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

struct sunxi_image_verify_pattern_st {
	uint32_t size;
	uint32_t interval;
	int32_t cnt;
};

extern int sunxi_verify_rotpk_hash(void *input_hash_buf, int len);
extern int sunxi_verify_os(ulong os_load_addr, const char *cert_name);
extern int sunxi_verify_partion(struct sunxi_image_verify_pattern_st *pattern, const char *part_name, const char *cert_name, int full);
extern int sunxi_verify_preserve_toc1(void *toc1_head_buf);
extern int sunxi_verify_get_rotpk_hash(void *hash_buf);
extern int sunxi_verify_toc1_root_cert(void *buf);

extern int verify_image_by_vbmeta(const char *image_name,
				  const uint8_t *image_data, size_t image_len,
				  const uint8_t *vb_data, size_t vb_len,
				  const char *pubkey_in_toc1);

#ifdef CONFIG_SUNXI_VERIFY_DSP
extern int sunxi_verify_dsp(ulong img_addr, u32 img_len, u32 dsp_id);
#endif

int sunxi_verify_mips(void *buff, uint len, void *cert, unsigned cert_len);

#ifdef CONFIG_SUNXI_IMAGE_HEADER
int sunxi_image_verify(ulong src, const char *cert_name);
#endif

#ifdef CONFIG_SUNXI_DM_VERITY
extern int sunxi_verity_hash_tree(char *part_name, char *cert_name);
#endif
