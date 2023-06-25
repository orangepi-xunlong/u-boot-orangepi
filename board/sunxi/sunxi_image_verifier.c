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
#include <private_uboot.h>
#include <sunxi_verify_boot_info.h>
#include <sys_partition.h>
#include <asm/arch/efuse.h>
#ifdef CONFIG_SUNXI_IMAGE_HEADER
#include <sunxi_image_header.h>
#endif
#ifdef CONFIG_CRYPTO
#include <crypto/sha256.h>
#include <crypto/ecc.h>
#include <crypto/ecc_dsa.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

static int sunxi_verify_embed_signature(void *buff, unsigned int len,
					const char *cert_name, void *cert,
					unsigned cert_len);
static int sunxi_verify_signature(void *buff, uint len, const char *cert_name);
static int android_image_get_signature(const struct andr_img_hdr *hdr,
				       ulong *sign_data, ulong *sign_len);

static void *preserved_toc1;
static int preserved_toc1_len;

#ifndef COFNIG_OPTEE25
__attribute__((weak))
int smc_tee_check_hash(const char *name, u8 *hash)
{
	pr_err("call weak func: %s\n", __func__);
	return 0;
}
#endif
int sunxi_verify_os(ulong os_load_addr, const char *cert_name)
{
	struct blk_desc *desc;
	disk_partition_t info = { 0 };
	ulong total_len = 0;
	ulong sign_data, sign_len;
	int ret;
	struct andr_img_hdr *fb_hdr = (struct andr_img_hdr *)os_load_addr;

#ifdef CONFIG_SUNXI_SWITCH_SYSTEM
	char *part_name = env_get("boot_partition");
#else
	const char *part_name = cert_name;
#endif

	desc = blk_get_devnum_by_typename("sunxi_flash", 0);
	if (desc == NULL)
		return -ENODEV;

	ret = sunxi_flash_try_partition(desc, part_name, &info);
	if (ret < 0)
		return -ENODEV;

	total_len = android_image_get_end(fb_hdr) - (ulong)fb_hdr;

	pr_msg("kernel len:%ld, part len:%ld\n", total_len, info.size * 512);
	if (total_len > info.size * 512) {
		pr_err("invalid kernel len\n");
		return -1;
	}
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

	addr = android_image_get_end(hdr);

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
	char *align = (char *)(((ulong)pk + 63) & (~63));
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
#ifdef CONFIG_SUNXI_ANTI_BRUSH
static int check_public_in_rootcert(const char *name,
					sunxi_certif_info_t *sub_certif)
{
	struct sbrom_toc1_item_info  *toc1_item;
	sunxi_certif_info_t  root_certif;
	u8 *buf;
	int ret, i;
	void *toc1_base;

	if (preserved_toc1 == NULL) {
		toc1_base = (void *)(ulong)CONFIG_SUNXI_BOOTPKG_BASE;
	} else {
		toc1_base = preserved_toc1;
	}

	toc1_item =
		(struct sbrom_toc1_item_info
			 *)(toc1_base + sizeof(struct sbrom_toc1_head_info));

	/*Parse root certificate*/
	buf = (u8 *)(toc1_base + toc1_item->data_offset);
	ret =  sunxi_certif_probe_ext(&root_certif, buf, toc1_item->data_len);
	if (ret < 0) {
		pr_err("fail to create root certif\n");
		return -1;
	}

	for (i = 0; i < root_certif.extension.extension_num; i++) {
		if (strcmp((const char *)root_certif.extension.name[i], name)) {
			continue;
		}
		pr_err("find %s key stored in root certif\n", name);

		if (memcmp(root_certif.extension.value[i],
					sub_certif->pubkey.n+1, sub_certif->pubkey.n_len-1)) {
			pr_err("%s key n is incompatible\n", name);
			pr_err(">>>>>>>key in rootcertif<<<<<<<<<<\n");
			sunxi_dump((u8 *)root_certif.extension.value[i], sub_certif->pubkey.n_len-1);
			pr_err(">>>>>>>key in certif<<<<<<<<<<\n");
			sunxi_dump((u8 *)sub_certif->pubkey.n+1, sub_certif->pubkey.n_len-1);
			return -1;
		}
		if (memcmp(root_certif.extension.value[i] + sub_certif->pubkey.n_len-1,
					sub_certif->pubkey.e, sub_certif->pubkey.e_len)) {
			pr_err("%s key e is incompatible\n", name);
			pr_err(">>>>>>>key in rootcertif<<<<<<<<<<\n");
			sunxi_dump((u8 *)root_certif.extension.value[i] +
					sub_certif->pubkey.n_len-1, sub_certif->pubkey.e_len);
			pr_err(">>>>>>>key in certif<<<<<<<<<<\n");
			sunxi_dump((u8 *)sub_certif->pubkey.e, sub_certif->pubkey.e_len);
			return -1;
		}
		break;
	}
	return 0 ;
}
#else
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
#endif

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
#ifdef COFNIG_SUNXI_VERIFY_BOOT_INFO
	sunxi_set_verify_boot_blob(SUNXI_VB_INFO_KEY, hash_of_file, 32);
#endif
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
		return -1;
	}
	pr_msg("show hash of file\n");

	ret = smc_tee_check_hash(cert_name, hash_of_file);
	if (ret == 0xFFFF000F) {
		sunxi_dump(hash_of_file, 32);
		pr_err("optee return hash invalid\n");
		return -1;
	} else if (ret == 0) {
		pr_msg("image %s hash valid\n", cert_name);
#ifdef COFNIG_SUNXI_VERIFY_BOOT_INFO
		sunxi_set_verify_boot_blob(SUNXI_VB_INFO_KEY, hash_of_file, 32);
#endif
		return 0;
	} else {
		sunxi_dump(hash_of_file, 32);
		pr_err("image %s hash not found\n", cert_name);
		return -1;
	}

}

int sunxi_verify_preserve_toc1(void *toc1_head_buf)
{
	struct sbrom_toc1_head_info *toc1_head;
	preserved_toc1_len = 16384;

	toc1_head      = (struct sbrom_toc1_head_info *)(toc1_head_buf);
	if (toc1_head->magic != TOC_MAIN_INFO_MAGIC) {
		pr_err("can not find toc1_head\n");
		return -1;
	}

	preserved_toc1 = malloc(preserved_toc1_len);
	if (preserved_toc1 == NULL) {
		pr_err("fail to malloc root certif\n");
		return -1;
	}
	printf("preserved len:%d\n", preserved_toc1_len);
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

int sunxi_verify_toc1_root_cert(void *buf)
{
	sbrom_toc1_head_info_t *toc1_head;
	struct sbrom_toc1_item_info *toc1_item;
	sunxi_certif_info_t root_certif;
	int ret;
	u8 *cert;
	uint8_t key_hash[32];

	toc1_head = (struct sbrom_toc1_head_info *)(buf);
	if (toc1_head->magic != TOC_MAIN_INFO_MAGIC) {
		pr_err("toc1 magic is error\n");
		return -1;
	}

	toc1_item = (struct sbrom_toc1_item_info *)
				(buf + sizeof(struct sbrom_toc1_head_info));

	/*Parse root certificate*/
	cert = (u8 *)(buf + toc1_item->data_offset);
	ret = sunxi_certif_verify_itself(&root_certif, cert, toc1_item->data_len);
	if (ret < 0) {
		pr_err("verify root cert error\n");
		return -2;
	}

	sunxi_ss_open();
	memset(key_hash, 0x0, sizeof(key_hash));
	ret = sunxi_certif_pubkey_check(&root_certif.pubkey, key_hash);
	if (ret < 0) {
		pr_err("fail to cal pubkey hash\n");
		return -3;
	}

	ret = sunxi_efuse_verify_rotpk(key_hash);
	if (ret < 0) {
		pr_err("verify rotpk error\n");
		return -3;
	}
	return 0;
}

#ifdef CONFIG_SUNXI_ANTI_BRUSH
int sunxi_verify_rotpk_hash(void *input_hash_buf, int len)
{
	u8 hash_of_pubkey[32];

	if (len < 32) {
		pr_err("the input hash is not equal to 32 bytes\n");
		return -1;
	}

	sunxi_ss_open();
	memset(hash_of_pubkey, 0, 32);

	if (sunxi_verify_get_rotpk_hash(hash_of_pubkey)) {
		pr_err("%s: get rotpk_hash failed\n", __func__);
		return -1;
	}

	pr_msg("show hash of publickey in certif\n");
	sunxi_dump(input_hash_buf, 32);
	if (memcmp(input_hash_buf, hash_of_pubkey, 32)) {
		pr_err("hash compare is not correct\n");
		pr_err(">>>>>>>hash of certif<<<<<<<<<<\n");
		sunxi_dump(hash_of_pubkey, 32);
		pr_err(">>>>>>>hash of user input<<<<<<<<<<\n");
		sunxi_dump(input_hash_buf, 32);
	} else {
		pr_err("the hash of input data and toc are equal\n");
		return 0;
	}

	return -1;
}

#else
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
#endif

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
	pr_msg("squashfs len:%d, part len:%ld\n", len, info->size * 512);
	if (len > info->size * 512) {
		pr_err("invalid squashfs len\n");
		free(rootfs_sb);
		return -1;
	}
	free(rootfs_sb);
	return len;
}

#ifdef CONFIG_SUNXI_DM_VERITY
int sunxi_verity_hash_tree(char *part_name, char *cert_name)
{
	struct cert_header {
		char cert_magic[16];
		unsigned int cert_len;
		unsigned int hash_tree_len;
		char salt_str[80];
		char root_hash_str[80];
		int reserved[18];
	};

	int ret = 0;

	disk_partition_t info = {0};

	struct cert_header *ht_cert_header = NULL;
	void *cert_buf = NULL;
	void *hash_tree_buf = NULL;
	uint8_t *p = NULL;
	uint32_t part_len = 0;
	uint32_t cert_len = 0;
	uint32_t hash_tree_len = 0;

	char *verity_dev = NULL;
	char dm_mod[256] = {0};

	sunxi_partition_get_info(part_name, &info);
	part_len = cal_partioin_len(&info);

	// 1. verify signature of hash_tree
#define SUNXI_X509_CERTIFF_MAX_LEN 4096
	cert_len = ALIGN(SUNXI_X509_CERTIFF_MAX_LEN + sizeof(struct cert_header), 4096);
	cert_buf = malloc(cert_len);
	if (!cert_buf) {
		pr_err("malloc cert_buf failed\n");
		ret = -1;
		goto out;
	}

	sunxi_flash_read(info.start + (part_len / SECTOR_SIZE), cert_len / SECTOR_SIZE, cert_buf);

	ht_cert_header = (struct cert_header *)cert_buf;
	if (strncmp(ht_cert_header->cert_magic, "squashfs_verity", 15)) {
		pr_err("ht_cert_header cert_magic error, should be squashfs_verity\n");
		ret = -1;
		goto out;
	}
	if (ht_cert_header->cert_len > SUNXI_X509_CERTIFF_MAX_LEN) {
		pr_err("cert_len %d should not bigger than %d\n", ht_cert_header->cert_len, SUNXI_X509_CERTIFF_MAX_LEN);
		ret = -1;
		goto out;
	}

	hash_tree_len = ALIGN(ht_cert_header->hash_tree_len, SECTOR_SIZE) + 256;
	hash_tree_buf = malloc(hash_tree_len);
	if (!hash_tree_buf) {
		pr_err("malloc hash_tree_buf failed\n");
		ret = -1;
		goto out;
	}

	p = (uint8_t *)((((u32)hash_tree_buf) + (CACHE_LINE_SIZE - 1)) & (~(CACHE_LINE_SIZE - 1)));

	sunxi_flash_read(info.start + (part_len + cert_len) / SECTOR_SIZE, (hash_tree_len - 256) / SECTOR_SIZE, p);

	ret = sunxi_verify_embed_signature(p,
					ht_cert_header->hash_tree_len,
					cert_name,
					cert_buf + sizeof(struct cert_header),
					ht_cert_header->cert_len);

	if (ret) {
		pr_err("verify hash_tree error\n");
		ret = -1;
		goto out;
	}

	// 2. set dm-mod env
	verity_dev = env_get("verity_dev");

	sprintf(dm_mod, "\"rootfs,,,ro,0 %d verity 1 %s %s 4096 4096 %d %d sha256 %s %s\"",
		part_len / SECTOR_SIZE,
		verity_dev,
		verity_dev,
		part_len / 4096,
		(part_len + cert_len) / 4096 + 1,
		ht_cert_header->root_hash_str,
		ht_cert_header->salt_str);

	ret = env_set("dm_mod", dm_mod);
	if (ret) {
		printf("Failed to set dm_mod\n");
		goto out;
	}

	tick_printf("verify squashfs hash_tree success\n");

out:
	if (cert_buf)
		free(cert_buf);

	if (hash_tree_buf)
		free(hash_tree_buf);

	return ret;
}
#endif

int sunxi_verify_partion(struct sunxi_image_verify_pattern_st *pattern,
			const char *part_name, const char *cert_name, int full)
{
	int ret = 0;
	disk_partition_t info = { 0 };
	int i;
	uint8_t *p		      = 0;
	uint8_t *unaligned_sample_buf = 0;
	void *cert_buf;
	uint32_t cert_len;
	uint64_t part_len;
	uint32_t whole_sample_len;

	if (sunxi_partition_get_info(part_name, &info)) {
		printf("get part: %s info failed\n", part_name);
		return -ENODEV;
	}

	part_len = cal_partioin_len(&info);

	if (full == 1) {
		whole_sample_len = part_len;
	} else {
		if (pattern->cnt == -1) {
			if (part_len == -1) {
				return -1;
			}
			pattern->cnt = part_len / pattern->interval;
		}
		whole_sample_len = pattern->cnt * pattern->size;
	}

#if 0
	pr_msg("pattern size:%d, interval:%d,cnt:%d, whole_sample_len:%d, "
		"cert_name : %s, full: %d\n", pattern->size, pattern->interval,
		pattern->cnt, whole_sample_len, cert_name, full);
#endif

	unaligned_sample_buf = (uint8_t *)malloc(whole_sample_len + 256);
	if (!unaligned_sample_buf) {
		printf("no memory for verify\n");
		return -1;
	}
	p = (uint8_t *)((((ulong)unaligned_sample_buf) + (CACHE_LINE_SIZE - 1)) &
			(~(CACHE_LINE_SIZE - 1)));

	if (full == 1) {
		sunxi_flash_read(info.start, whole_sample_len / SECTOR_SIZE, p);
	} else {
		for (i = 0; i < pattern->cnt; i++) {
	#if 0
			pr_msg("from %lx read %d block:to %p\n",
			    info.start + i * pattern->interval / SECTOR_SIZE,
			    pattern->size / SECTOR_SIZE, p + i * pattern->size);
	#endif
			sunxi_flash_read(
			    info.start + i * pattern->interval / SECTOR_SIZE,
			    pattern->size / SECTOR_SIZE, p + i * pattern->size);
		}
	}

#define SUNXI_X509_CERTIFF_MAX_LEN 4096
	cert_buf = malloc(ALIGN(SUNXI_X509_CERTIFF_MAX_LEN + 4, SECTOR_SIZE));
	if (!cert_buf) {
		printf("not enough meory\n");
	} else {
		memset(cert_buf, 0, SUNXI_X509_CERTIFF_MAX_LEN + 4);
		sunxi_flash_read(info.start + (part_len / SECTOR_SIZE),
			(ALIGN(SUNXI_X509_CERTIFF_MAX_LEN + 4, SECTOR_SIZE)) /
				SECTOR_SIZE, cert_buf);
		memcpy(&cert_len, cert_buf, sizeof(cert_len));
		memcpy(cert_buf, cert_buf + 4, cert_len);
		ret = sunxi_verify_embed_signature(p,
						   whole_sample_len,
						   cert_name,
						   cert_buf,
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

int sunxi_verify_mips(void *buff, uint len, void *cert, unsigned cert_len)
{
	return sunxi_verify_embed_signature(buff, len, "mips", cert, cert_len);
}

#ifdef CONFIG_SUNXI_AVB
#include <sunxi_avb.h>
int verify_image_by_vbmeta(const char *image_name, const uint8_t *image_data,
			   size_t image_len, const uint8_t *vb_data,
			   size_t vb_len, const char *pubkey_in_toc1)
{
	AvbDescriptor *desc = NULL;
	AvbHashDescriptor *hdh;
	const uint8_t *salt;
	const uint8_t *expected_hash;
	uint8_t *salt_buf;
	size_t salt_buf_len;
	ALLOC_CACHE_ALIGN_BUFFER(u8, hash_result, 32);
	char slot_vbmeta[20] = "vbmeta";
	char *slot_suffix    = env_get("slot_suffix");

	memcpy(slot_vbmeta, pubkey_in_toc1, strlen(pubkey_in_toc1));
	slot_vbmeta[strlen(pubkey_in_toc1)] = 0;
	if (slot_suffix != NULL) {
		strcat(slot_vbmeta, slot_suffix);
	}

	if (sunxi_avb_get_hash_descriptor_by_name(image_name, vb_data, vb_len,
						  &desc)) {
		pr_error("get descriptor for %s failed\n", image_name);
		if (strcmp(image_name, pubkey_in_toc1) != 0) {
			/*maybe signature is in the very partition, not vbmeta, try that*/
			uint8_t *vb_meta_data = 0;
			size_t vb_len;
			int ret;
			ret = sunxi_avb_read_vbmeta_in_partition(
				image_name, &vb_meta_data, &vb_len);
			if (ret == 0) {
				ret = verify_image_by_vbmeta(
					image_name, image_data, image_len,
					vb_meta_data, vb_len, image_name);
				if (ret == 0) {
					pr_msg("verify passed with non vbmeta partition signature\n");
				}
			} else {
				pr_error("read vbmeta in %s partition failed\n",
					 image_name);
			}
			if (vb_meta_data)
				free(vb_meta_data);
			return ret;
		}
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
					   slot_vbmeta)) {
			pr_error("hash compare is not correct\n");
			goto descriptot_need_free;
		}
	} else {
		if (check_public_in_rootcert(slot_vbmeta, &sub_certif)) {
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

#ifdef CONFIG_SUNXI_VERIFY_DSP
int sunxi_verify_dsp(ulong img_addr, u32 image_len, u32 dsp_id)
{
	int ret = 0;
	char *cert_name = NULL;

	if (gd->securemode) {
		if (dsp_id == 0) {
			cert_name = env_get("dsp0_partition");
			pr_msg("cert_name = %s\n", cert_name);
		} else if (dsp_id == 1) {
			cert_name = env_get("dsp1_partition");
			pr_msg("cert_name = %s\n", cert_name);
		}
#ifdef CONFIG_SUNXI_IMAGE_HEADER
		ret = sunxi_image_header_check((sunxi_image_header_t *)img_addr);
		if (ret == 0) {
			pr_error("find sunxi_image_header\n");
			if (sunxi_image_verify(img_addr, cert_name) != 0) {
				pr_error("dsp %s verify failed\n", cert_name);
				return -1;
			} else {
				printf("dsp %d verify success!\n", dsp_id);
			}
		} else {
			pr_error("not find sunxi_image_header\n");
			return -2;
		}
#else
		if (!image_len) {
			pr_error("dsp len is zero\n");
			return -1;
		}
		ret = sunxi_verify_signature((void *)img_addr, image_len, cert_name);
		if (ret < 0) {
			pr_error("dsp: sunxi_verify_signature fail: %d\n", ret);
			return -2;
		}
#endif
	}

	return 0;
}
#endif

#ifdef CONFIG_SUNXI_VERIFY_RISCV
int sunxi_verify_riscv(ulong img_addr, u32 image_len, u32 riscv_id)
{
	int ret = 0;
	char *cert_name = NULL;

	if (gd->securemode) {
		if (riscv_id == 0) {
			cert_name = env_get("riscv0_partition");
			pr_msg("cert_name = %s\n", cert_name);
		}
#ifdef CONFIG_SUNXI_IMAGE_HEADER
		ret = sunxi_image_header_check((sunxi_image_header_t *)img_addr);
		if (ret == 0) {
			pr_error("find sunxi_image_header\n");
			if (sunxi_image_verify(img_addr, cert_name) != 0) {
				pr_error("riscv %s verify failed\n", cert_name);
				return -1;
			} else {
				printf("riscv %d verify success!\n", riscv_id);
			}
		} else {
			pr_error("not find sunxi_image_header\n");
			return -2;
		}
#else
		if (!image_len) {
			pr_error("riscv len is zero\n");
			return -1;
		}
		ret = sunxi_verify_signature((void *)img_addr, image_len, cert_name);
		if (ret < 0) {
			pr_error("riscv: sunxi_verify_signature fail: %d\n", ret);
			return -2;
		}
#endif
	}

	return 0;
}
#endif

#ifdef CONFIG_SUNXI_IMAGE_HEADER
static int sunxi_rsa_pk_check(uint8_t *key_n, uint32_t n_len,
			uint8_t *key_e, uint32_t e_len, const char *name)
{
	int ret = 0;
	u8 pk[2048 / 8 * 2 + 256 + 32] = {0}; // for sha padding
	u8 key_hash[256 / 8] = {0};
	u8 *align = (u8 *)(((ulong)pk + 31) & (~31));
	char request_key_name[16] = {0};

	memset(pk, 0x91, sizeof(pk));
	memset(key_hash, 0x0, sizeof(key_hash));
	memcpy(align, key_n, n_len);
	memcpy(align + n_len, key_e, e_len);

	ret = sunxi_sha_calc(key_hash, 32, align, 2048 / 8 * 2);
	if (ret) {
		pr_error("sunxi_sha_calc: calc rsa pubkey sha256 with hardware err\n");
		return ret;
	}

	memset(request_key_name, 0x0, sizeof(request_key_name));
	strcpy(request_key_name, name);
	strcat(request_key_name, "-key");

	ret = smc_tee_check_hash(request_key_name, key_hash);
	if (ret == 0xFFFF000F) {
		pr_error("optee return pubkey hash invalid\n");
		return -1;
	} else if (ret == 0) {
		pr_error("pubkey %s valid\n", name);
	} else {
		pr_error("pubkey %s not found\n", name);
		return -1;
	}

	return 0;
}

static int sunxi_ecc_pk_check(uint8_t *ecc_pkey, uint32_t ecc_pkey_len, const char *name)
{
	int ret = 0;
	u8 pk[256 / 8 * 2 + 256 + 32] = {0}; // for sha padding
	u8 key_hash[256 / 8] = {0};
	u8 *align = (u8 *)(((ulong)pk + 31) & (~31));
	char request_key_name[16] = {0};

	memset(key_hash, 0x0, sizeof(key_hash));
	memcpy(align, ecc_pkey, ecc_pkey_len);
	ret = sunxi_sha_calc(key_hash, 32, align, ecc_pkey_len);
	if (ret) {
		pr_error("sunxi_sha_calc: calc ecc pubkey sha256 with hardware err\n");
		return ret;
	}

	memset(request_key_name, 0x0, sizeof(request_key_name));
	strcpy(request_key_name, name);
	strcat(request_key_name, "-key");

	ret = smc_tee_check_hash(request_key_name, key_hash);
	if (ret == 0xFFFF000F) {
		pr_error("optee return pubkey hash invalid\n");
		return -1;
	} else if (ret == 0) {
		pr_error("pubkey %s valid\n", name);
	} else {
		pr_error("pubkey %s not found\n", name);
		return -1;
	}

	return 0;
}

int sunxi_image_verify(ulong os_load_addr, const char *cert_name)
{
	int ret = 0;
	uint8_t *src = (uint8_t *)(os_load_addr);
	uint8_t key_n[256] = {0};
	uint8_t key_e[256] = {0};
	uint8_t ecc_pkey[64] = {0};
	uint8_t sign[256] = {0};
	uint8_t hash_of_file[32] = {0};
	uint8_t sign_of_hash[32] = {0};

	sunxi_image_header_t *ih = (sunxi_image_header_t *)src;
	sunxi_tlv_header_t tlv_tmp = {0};
	sunxi_tlv_header_t *tlv = &tlv_tmp; //(sunxi_tlv_header_t *)(src + ih->ih_hsize + ih->ih_psize);


	memcpy(tlv, src + ih->ih_hsize + ih->ih_psize, sizeof(sunxi_tlv_header_t));

	ret = sunxi_tlv_header_check(tlv);
	if (ret) {
		return ret;
	}

	sunxi_ss_open();
	/*Approvel certificate by trust-chain*/
	if (tlv->th_pkey_type == PKEY_TYPE_RSA) {
		memcpy(key_n, src + ih->ih_hsize + ih->ih_psize + tlv->th_size, 256);
		memcpy(key_e, src + ih->ih_hsize + ih->ih_psize + tlv->th_size + 256, 256);
		memcpy(sign, src + ih->ih_hsize + ih->ih_psize + tlv->th_size + tlv->th_pkey_size, 256);
		ret = sunxi_rsa_pk_check(key_n, 256, key_e, 3, cert_name);
		if (ret) {
			return ret;
		}
	} else if (tlv->th_pkey_type == PKEY_TYPE_ECC) {
		memcpy(ecc_pkey, src + ih->ih_hsize + ih->ih_psize + tlv->th_size, 64);
		memcpy(sign, src + ih->ih_hsize + ih->ih_psize + tlv->th_size + tlv->th_pkey_size, 64);
		ret = sunxi_ecc_pk_check(ecc_pkey, 64, cert_name);
		if (ret) {
			return ret;
		}
	}  else {
		pr_error("not support the pkey type %d\n", tlv->th_pkey_type);
		return -1;
	}

	/*calc payload hash*/
	memset(hash_of_file, 0, sizeof(hash_of_file));
	ret = sunxi_sha_calc(hash_of_file, sizeof(hash_of_file), src,
			ih->ih_hsize + ih->ih_psize + tlv->th_size + tlv->th_pkey_size);
	if (ret) {
		pr_error("calc file sha256 with hardware err\n");
		return -1;
	}

	/*verify*/
	if (tlv->th_pkey_type == PKEY_TYPE_RSA) {
		ret = sunxi_rsa_calc(key_n, 256,
				key_e, 3,
				sign_of_hash,  32,
				sign, 256);
		if (ret) {
			pr_error("sunxi_rsa_calc: calc rsa2048 with hardware err\n");
			return -2;
		}
		if (memcmp(hash_of_file, sign_of_hash, 32)) {
			pr_error("hash compare is not correct\n");
			pr_error(">>>>>>>hash of file<<<<<<<<<<\n");
			sunxi_dump((u8 *)hash_of_file, 32);
			pr_error(">>>>>>>sign of hash<<<<<<<<<<\n");
			sunxi_dump((u8 *)sign_of_hash, 32);
			return -3;
		}
	} else if (tlv->th_pkey_type == PKEY_TYPE_ECC) {
#ifdef CONFIG_CRYPTO
		if (!uECC_verify(ecc_pkey, hash_of_file,
				TC_SHA256_DIGEST_SIZE, sign, &curve_secp256r1)) {
			pr_error("uECC_vierfy error\n");
			return -3;
		}
#else
		pr_error("can not find ecc api\n");
#endif
	}

	memcpy(src, src + ih->ih_hsize, ih->ih_psize);
	return 0;
}

#endif
