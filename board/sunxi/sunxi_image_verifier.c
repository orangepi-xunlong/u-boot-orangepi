/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <openssl_ext.h>
#include <private_toc.h>
#include <asm/arch/ce.h>
#include <sunxi_board.h>
#include <configs/sunxi-common.h>
#include <android_image.h>
#include <sunxi_image_verifier.h>
#include <smc.h>

static int sunxi_verify_embed_signature(void *buff, unsigned int len,
					const char *cert_name, void *cert,
					unsigned cert_len);
static int sunxi_verify_signature(void *buff, uint len, const char *cert_name);
static int android_image_get_signature(const struct andr_img_hdr *hdr,
				       ulong *sign_data, ulong *sign_len);
int sunxi_verify_os(ulong os_load_addr, const char *cert_name)
{
	ulong total_len = 0;
	ulong sign_data, sign_len;
	int ret;
	struct andr_img_hdr *fb_hdr = (struct andr_img_hdr *)os_load_addr;

	total_len += fb_hdr->page_size;
	total_len += ALIGN(fb_hdr->kernel_size, fb_hdr->page_size);
	if (fb_hdr->second_size)
		total_len += ALIGN(fb_hdr->second_size, fb_hdr->page_size);
	if (fb_hdr->ramdisk_size)
		total_len += ALIGN(fb_hdr->ramdisk_size, fb_hdr->page_size);
	if (fb_hdr->recovery_dtbo_size)
		total_len +=
			ALIGN(fb_hdr->recovery_dtbo_size, fb_hdr->page_size);
	if (fb_hdr->dtb_size)
		total_len += ALIGN(fb_hdr->dtb_size, fb_hdr->page_size);

	printf("total_len=%ld\n", total_len);
	if (android_image_get_signature(fb_hdr, &sign_data, &sign_len))
		ret = sunxi_verify_embed_signature((void *)os_load_addr,
						   (unsigned int)total_len,
						   cert_name, (void *)sign_data,
						   sign_len);
	else
		ret = sunxi_verify_signature((void *)os_load_addr,
					     (unsigned int)total_len,
					     cert_name);
	return ret;
}

static int android_image_get_signature(const struct andr_img_hdr *hdr,
				       ulong *sign_data, ulong *sign_len)
{
	struct boot_img_hdr_ex *hdr_ex;
	ulong addr = 0;

	hdr_ex = (struct boot_img_hdr_ex *)hdr;
	if (strncmp((void *)(hdr_ex->cert_magic), AW_CERT_MAGIC,
		    strlen(AW_CERT_MAGIC))) {
		printf("No cert image embeded, image %s\n", hdr_ex->cert_magic);
		return 0;
	}

	addr = (unsigned long)hdr;
	addr += hdr->page_size;
	addr += ALIGN(hdr->kernel_size, hdr->page_size);
	if (hdr->ramdisk_size)
		addr += ALIGN(hdr->ramdisk_size, hdr->page_size);
	if (hdr->second_size)
		addr += ALIGN(hdr->second_size, hdr->page_size);
	if (hdr->recovery_dtbo_size)
		addr += ALIGN(hdr->recovery_dtbo_size, hdr->page_size);
	if (hdr->dtb_size)
		addr += ALIGN(hdr->dtb_size, hdr->page_size);

	*sign_data = (ulong)addr;
	*sign_len  = hdr_ex->cert_size;
	memset(hdr_ex->cert_magic, 0, ANDR_BOOT_MAGIC_SIZE + sizeof(unsigned));
	return 1;
}

#define RSA_BIT_WITDH 2048
static int sunxi_certif_pubkey_check(sunxi_key_t *pubkey, u8 *hash_buf)
{
	ALLOC_CACHE_ALIGN_BUFFER(char, rotpk_hash, 256);
	char all_zero[32];
	char pk[RSA_BIT_WITDH / 8 * 2 + 256]; /*For the stupid sha padding */

	memset(all_zero, 0, 32);
	memset(pk, 0x91, sizeof(pk));
	char *align = (char *)(((u32)pk + 63) & (~63));
	if (*(pubkey->n)) {
		memcpy(align, pubkey->n, pubkey->n_len);
		memcpy(align + pubkey->n_len, pubkey->e, pubkey->e_len);
	} else {
		memcpy(align, pubkey->n + 1, pubkey->n_len - 1);
		memcpy(align + pubkey->n_len - 1, pubkey->e, pubkey->e_len);
	}
	if (sunxi_sha_calc((u8 *)rotpk_hash, 32, (u8 *)align,
			   RSA_BIT_WITDH / 8 * 2)) {
		printf("sunxi_sha_calc: calc  pubkey sha256 with hardware err\n");
		return -1;
	}
	memcpy(hash_buf, rotpk_hash, 32);

	return 0;
}

static int check_public_in_rootcert(const char *name,
				    sunxi_certif_info_t *sub_certif)
{
	int ret;
	uint8_t key_hash[32];
	char request_key_name[16];

	sunxi_certif_pubkey_check(&sub_certif->pubkey, key_hash);

	strcpy(request_key_name, name);
	strcat(request_key_name, "-key");

	ret = smc_tee_check_hash(request_key_name, key_hash);
	if (ret == 0xFFFF000F) {
		printf("optee return pubkey hash invalid\n");
		return -1;
	} else if (ret == 0) {
		printf("pubkey %s valid\n", name);
		return 0;
	} else {
		printf("pubkey %s not found\n", name);
		return -1;
	}
}

static int sunxi_verify_embed_signature(void *buff, uint len,
					const char *cert_name, void *cert,
					unsigned cert_len)
{
	u8 hash_of_file[32];
	int ret;
	sunxi_certif_info_t sub_certif;
	void *cert_buf;

	cert_buf = malloc(cert_len);
	if (!cert_buf) {
		printf("out of memory\n");
		return -1;
	}
	memcpy(cert_buf, cert, cert_len);

	memset(hash_of_file, 0, 32);
	sunxi_ss_open();
	ret = sunxi_sha_calc(hash_of_file, 32, buff, len);
	if (ret) {
		printf("sunxi_verify_signature err: calc hash failed\n");
		goto __ERROR_END;
	}
	if (sunxi_certif_verify_itself(&sub_certif, cert_buf, cert_len)) {
		printf("%s error: cant verify the content certif\n", __func__);
		printf("cert dump\n");
		sunxi_dump(cert_buf, cert_len);
		goto __ERROR_END;
	}

	if (memcmp(hash_of_file, sub_certif.extension.value[0], 32)) {
		printf("hash compare is not correct\n");
		printf(">>>>>>>hash of file<<<<<<<<<<\n");
		sunxi_dump(hash_of_file, 32);
		printf(">>>>>>>hash in certif<<<<<<<<<<\n");
		sunxi_dump(sub_certif.extension.value[0], 32);
		goto __ERROR_END;
	}

	/*Approvel certificate by trust-chain*/
	if (check_public_in_rootcert(cert_name, &sub_certif)) {
		printf("check rootpk[%s] in rootcert fail\n", cert_name);
		goto __ERROR_END;
	}
	free(cert_buf);
	return 0;
__ERROR_END:
	if (cert_buf)
		free(cert_buf);
	return -1;
}

static int sunxi_verify_signature(void *buff, uint len, const char *cert_name)
{
	u8 hash_of_file[32];
	int ret;

	memset(hash_of_file, 0, 32);
	sunxi_ss_open();
	ret = sunxi_sha_calc(hash_of_file, 32, buff, len);
	if (ret) {
		printf("sunxi_verify_signature err: calc hash failed\n");
		//sunxi_ss_close();

		return -1;
	}
	//sunxi_ss_close();
	printf("show hash of file\n");
	sunxi_dump(hash_of_file, 32);

	ret = smc_tee_check_hash(cert_name, hash_of_file);
	if (ret == 0xFFFF000F) {
		printf("optee return hash invalid\n");
		return -1;
	} else if (ret == 0) {
		printf("image %s hash valid\n", cert_name);
		return 0;
	} else {
		printf("image %s hash not found\n", cert_name);
		return -1;
	}
}

static void *preserved_toc1;
static int preserved_toc1_len;
int sunxi_verify_preserve_toc1(void *toc1_head_buf)
{
	struct sbrom_toc1_head_info *toc1_head;

	toc1_head      = (struct sbrom_toc1_head_info *)(toc1_head_buf);
	preserved_toc1 = malloc(toc1_head->valid_len + 4096);
	if (preserved_toc1 == NULL) {
		printf("fail to malloc root certif\n");
		return -1;
	}
	preserved_toc1_len = toc1_head->valid_len;
	printf("preserved len:%d\n", toc1_head->valid_len);
	memcpy(preserved_toc1, toc1_head, preserved_toc1_len);
	return 0;
}

int sunxi_verify_get_rotpk_hash(void *hash_buf)
{
	struct sbrom_toc1_item_info *toc1_item;
	sunxi_certif_info_t root_certif;
	u8 *buf;
	int ret;
	void *toc1_base;

	if (preserved_toc1 == NULL) {
		toc1_base = (void *)SUNXI_CFG_TOC1_STORE_IN_DRAM_BASE;
	} else {
		toc1_base = preserved_toc1;
	}
	toc1_item =
		(struct sbrom_toc1_item_info
			 *)(toc1_base + sizeof(struct sbrom_toc1_head_info));

	/*Parse root certificate*/
	buf = (u8 *)(toc1_base + toc1_item->data_offset);
	ret = sunxi_certif_verify_itself(&root_certif, buf,
					 toc1_item->data_len);

	ret = sunxi_certif_pubkey_check(&root_certif.pubkey, hash_buf);
	if (ret < 0) {
		printf("fail to cal pubkey hash\n");
		return -1;
	}

	return 0;
}

int sunxi_verify_rotpk_hash(void *input_hash_buf, int len)
{
	int ret;
	if (len != 32) {
		return -1;
	}
	ret = smc_tee_check_hash("rotpk", input_hash_buf);
	if (ret == 0xFFFF000F) {
		printf("rotpk invalid\n");
		return -1;
	} else if (ret == 0) {
		return 0;
	} else {
		printf("rotpk not found\n");
		return -1;
	}
	return ret;
}

#define SECTOR_SIZE 512
static int cal_partioin_len(disk_partition_t *info)
{
	typedef long long squashfs_inode;
	struct squashfs_super_block {
		unsigned int s_magic;
		unsigned int inodes;
		int mkfs_time /* time of filesystem creation */;
		unsigned int block_size;
		unsigned int fragments;
		unsigned short compression;
		unsigned short block_log;
		unsigned short flags;
		unsigned short no_ids;
		unsigned short s_major;
		unsigned short s_minor;
		squashfs_inode root_inode;
		long long bytes_used;
		long long id_table_start;
		long long xattr_id_table_start;
		long long inode_table_start;
		long long directory_table_start;
		long long fragment_table_start;
		long long lookup_table_start;
	};
#define SQUASHFS_MAGIC 0x73717368
	struct squashfs_super_block *rootfs_sb;
	int len;

	rootfs_sb =
		malloc(ALIGN(sizeof(struct squashfs_super_block), SECTOR_SIZE));
	if (!rootfs_sb)
		return -1;

	sunxi_flash_read(
		info->start,
		(ALIGN(sizeof(struct squashfs_super_block), SECTOR_SIZE) /
		 SECTOR_SIZE),
		rootfs_sb);

	if (rootfs_sb->s_magic != SQUASHFS_MAGIC) {
		printf("unsupport rootfs, magic: %d\n", rootfs_sb->s_magic);
		free(rootfs_sb);
		return -1;
	}

	len = (rootfs_sb->bytes_used + 4096 - 1) / 4096 * 4096;
	free(rootfs_sb);
	return len;
}

int sunxi_verify_partion(struct sunxi_image_verify_pattern_st *pattern,
			 const char *part_name)
{
	struct blk_desc *desc;
	int ret;
	disk_partition_t info = { 0 };
	int i;
	uint8_t *p		      = 0;
	uint8_t *unaligned_sample_buf = 0;
	void *cert_buf;
	uint32_t cert_len;
	uint64_t part_len;
	uint32_t whole_sample_len;

	desc = blk_get_devnum_by_typename("sunxi_flash", 0);
	if (desc == NULL)
		return -ENODEV;

	ret = sunxi_flash_try_partition(desc, part_name, &info);
	if (ret < 0)
		return -ENODEV;
	part_len = cal_partioin_len(&info);

	if (pattern->cnt == -1) {
		if (part_len == -1) {
			return -1;
		}
		pattern->cnt = part_len / pattern->interval;
	}
	whole_sample_len = pattern->cnt * pattern->size;

#if 0
	printf("pattern size:%d,interval:%d,cnt:%d,ttl_smp_size:%d\n", pattern->size,
	       pattern->interval, pattern->cnt, whole_sample_len);
#endif

	unaligned_sample_buf = (uint8_t *)malloc(whole_sample_len + 256);
	if (!unaligned_sample_buf) {
		printf("no memory for verify\n");
		return -1;
	}
	p = (uint8_t *)((((u32)unaligned_sample_buf) + (CACHE_LINE_SIZE - 1)) &
			(~(CACHE_LINE_SIZE - 1)));

	for (i = 0; i < pattern->cnt; i++) {
#if 0
		printf("from %lx read %d block:to %p\n",
		       info.start + i * pattern->interval / SECTOR_SIZE,
		       pattern->size / SECTOR_SIZE, p + i * pattern->size);
#endif
		sunxi_flash_read(
			info.start + i * pattern->interval / SECTOR_SIZE,
			pattern->size / SECTOR_SIZE, p + i * pattern->size);
	}

	ret = 0;

#define SUNXI_X509_CERTIFF_MAX_LEN 4096
	cert_buf = malloc(ALIGN(SUNXI_X509_CERTIFF_MAX_LEN + 4, SECTOR_SIZE));
	if (!cert_buf) {
		printf("not enough meory\n");
	} else {
		memset(cert_buf, 0, SUNXI_X509_CERTIFF_MAX_LEN + 4);
		sunxi_flash_read(
			info.start + (part_len / SECTOR_SIZE),
			(ALIGN(SUNXI_X509_CERTIFF_MAX_LEN + 4, SECTOR_SIZE)) /
				SECTOR_SIZE,
			cert_buf);
		memcpy(&cert_len, cert_buf, sizeof(cert_len));
		memcpy(cert_buf, cert_buf + 4, cert_len);
		ret = sunxi_verify_embed_signature(p,
						   pattern->cnt * pattern->size,
						   "rootfs", cert_buf,
						   cert_len);
		free(cert_buf);
	}

	free(unaligned_sample_buf);
	if (ret == 0) {
		printf("partition %s verify pass\n", part_name);
	} else {
		printf("partition %s verify failed\n", part_name);
	}
	return ret;
}

#if 0
static int do_part_verify_test(cmd_tbl_t *cmdtp, int flag, int argc,
			       char *const argv[])
{
	struct sunxi_image_verify_pattern_st verify_pattern = { 0x1000,
								0x100000, -1 };
	if (sunxi_verify_partion(&verify_pattern, "rootfs") != 0) {
		return -1;
	}

	return 0;
}

U_BOOT_CMD(part_verify_test, 3, 0, do_part_verify_test,
	   "do a partition verify test", "NULL");
#endif

#ifdef CONFIG_SUNXI_AVB
#include <sunxi_avb.h>
int verify_image_by_vbmeta(const char *image_name, const uint8_t *image_data,
			   size_t image_len, const uint8_t *vb_data,
			   size_t vb_len)
{
	AvbDescriptor *desc = NULL;
	AvbHashDescriptor *hdh;
	const uint8_t *salt;
	const uint8_t *expected_hash;
	uint8_t *salt_buf;
	size_t salt_buf_len;
	ALLOC_CACHE_ALIGN_BUFFER(u8, hash_result, 32);

	if (sunxi_avb_get_hash_descriptor_by_name(image_name, vb_data, vb_len,
						  &desc)) {
		pr_error("get descriptor for %s failed\n", image_name);
		return -1;
	}

	sunxi_certif_info_t sub_certif;
	int ret = sunxi_vbmeta_self_verify(vb_data, vb_len, &sub_certif.pubkey);
	if (ret) {
		if (ret == -2) {
			/*
			 * rsa pub key check failed, still possible
			 * to use cert in toc1 to check, go on
			 */
		} else {
			pr_error("vbmeta self verify failed\n");
			goto descriptot_need_free;
		}
	}

	if (ret == -2) {
		if (sunxi_verify_signature((uint8_t *)vb_data, vb_len,
					   "vbmeta")) {
			pr_error("hash compare is not correct\n");
			goto descriptot_need_free;
		}
	} else {
		if (check_public_in_rootcert("vbmeta", &sub_certif)) {
			pr_error("self sign key verify failed\n");
			goto descriptot_need_free;
		}
	}

	hdh  = (AvbHashDescriptor *)desc;
	salt = (uint8_t *)hdh + sizeof(AvbHashDescriptor) +
	       hdh->partition_name_len;
	expected_hash = salt + hdh->salt_len;
	if (image_len != hdh->image_size) {
		pr_error("image_len not match, actual:%d, expected:%lld\n",
			 image_len, hdh->image_size);
		goto descriptot_need_free;
	}

	/*
	 * hardware require 64Byte align when doing multi step calculation,
	 * since salt is usually 32Byte, the only way to calc hash of salt +
	 * image is put salt in front of image_data and calc their hash at once
	 * memory right before image_data might already be used, recover them
	 * after hash calculation
	 */
	salt_buf_len = ALIGN(hdh->salt_len, CACHE_LINE_SIZE);
	salt_buf     = (uint8_t *)malloc(salt_buf_len);
	if (salt_buf == NULL) {
		pr_error("not enough memory\n");
		goto descriptot_need_free;
	}
	memcpy(salt_buf, image_data - salt_buf_len, salt_buf_len);
	memcpy((uint8_t *)image_data - hdh->salt_len, salt, hdh->salt_len);
	flush_cache((u32)image_data - salt_buf_len, salt_buf_len);

	sunxi_ss_open();
	sunxi_sha_calc(hash_result, 32, (uint8_t *)image_data - hdh->salt_len,
		       hdh->salt_len + image_len);

	memcpy((uint8_t *)image_data - salt_buf_len, salt_buf, salt_buf_len);

	free(salt_buf);
	free(desc);

	if (memcmp(expected_hash, hash_result, 32) != 0) {
		pr_error("hash not match, hash of file:\n");
		sunxi_dump(hash_result, 32);
		pr_error("hash in descriptor:\n");
		sunxi_dump((void *)expected_hash, 32);
		return -1;
	}

	return 0;

descriptot_need_free:
	free(desc);
	return -1;
}
#endif

