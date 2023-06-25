/*
 * (C) Copyright 2017  <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <sys_partition.h>
#include <asm/arch/ce.h>
#include <sunxi_avb.h>
#include <sunxi_verify_boot_info.h>

int sunxi_avb_check_magic(AvbVBMetaImageHeader *vbmeta_hdr)
{
	return !strncmp((char *)(vbmeta_hdr->magic), "AVB0", 4);
}

int sunxi_avb_read_vbmeta_head(char *name, u8 *addr)
{
	u32 start_block;
	int ret     = 0;
	start_block = sunxi_partition_get_offset_byname(name);
	if (!start_block) {
		pr_msg("cant find part named vbmeta_system\n");
	} else {
		ret = sunxi_flash_read(start_block, 1, addr);
		pr_msg("read data in addr ret = %d \n", ret);
	}
	return ret;
}

void sunxi_avb_dump_vbmeta(AvbVBMetaImageHeader *vbmeta_hdr)
{
	pr_msg("magic = %s \n", vbmeta_hdr->magic);
	pr_msg("major version = %x  minor version = %x\n",
	       vbmeta_hdr->required_libavb_version_major,
	       vbmeta_hdr->required_libavb_version_minor);
	pr_msg("authentication_data_block_size = %x \n",
	       vbmeta_hdr->authentication_data_block_size);
	pr_msg("auxiliary_data_block_size = %x \n",
	       vbmeta_hdr->auxiliary_data_block_size);
	pr_msg("vbmeta_size = %d \n",
	       vbmeta_hdr->authentication_data_block_size +
		       vbmeta_hdr->auxiliary_data_block_size +
		       AVB_VBMETA_IMAGE_HEADER_SIZE);
	pr_msg("algorithm_type = %x \n", vbmeta_hdr->algorithm_type);
	pr_msg("flag = %x \n", vbmeta_hdr->flags);
	pr_msg("release tool = %s \n", vbmeta_hdr->release_string);
}

u64 sunxi_avb_vbmeta_size(AvbVBMetaImageHeader *vbmeta_hdr)
{
	return vbmeta_hdr->authentication_data_block_size +
	       vbmeta_hdr->auxiliary_data_block_size +
	       AVB_VBMETA_IMAGE_HEADER_SIZE;
}

void sunxi_avb_setup_env(char *avb_version, char *hash_alg, u64 vbmeta_size,
			 char *sha1_hash, char *lock_state)
{
	char *str;
	char cmdline[1024] = { 0 };
	char tmpbuf[128]   = { 0 };
	str		   = env_get("bootargs");
	uint32_t vbmeta_flags;
	strcpy(cmdline, str);
	sprintf(tmpbuf, " androidboot.vbmeta.avb_version=%s", "2.0");
	strcat(cmdline, tmpbuf);
	sprintf(tmpbuf, " androidboot.vbmeta.hash_alg=%s", hash_alg);
	strcat(cmdline, tmpbuf);
	sprintf(tmpbuf, " androidboot.vbmeta.size=%llu", vbmeta_size);
	strcat(cmdline, tmpbuf);
	sprintf(tmpbuf, " androidboot.vbmeta.digest=%s", sha1_hash);
	strcat(cmdline, tmpbuf);
	sprintf(tmpbuf, " androidboot.vbmeta.device_state=%s", lock_state);
	strcat(cmdline, tmpbuf);
	sunxi_avb_get_vbmeta_flags(&vbmeta_flags);
	if (vbmeta_flags & AVB_VBMETA_IMAGE_FLAGS_HASHTREE_DISABLED) {
		sprintf(tmpbuf, " androidboot.veritymode=%s", "disabled");
	} else {
		sprintf(tmpbuf, " androidboot.veritymode=%s", "enforcing");
	}
	strcat(cmdline, tmpbuf);
	env_set("bootargs", cmdline);
}

void sunxi_avb_slot_vbmeta_head(AvbVBMetaImageHeader *vbmeta_hdr)
{
	vbmeta_hdr->required_libavb_version_major =
		be32_to_cpu(vbmeta_hdr->required_libavb_version_major);
	vbmeta_hdr->required_libavb_version_minor =
		be32_to_cpu(vbmeta_hdr->required_libavb_version_minor);
	vbmeta_hdr->authentication_data_block_size =
		be64_to_cpu(vbmeta_hdr->authentication_data_block_size);
	vbmeta_hdr->auxiliary_data_block_size =
		be64_to_cpu(vbmeta_hdr->auxiliary_data_block_size);
	vbmeta_hdr->algorithm_type = be64_to_cpu(vbmeta_hdr->algorithm_type);
	vbmeta_hdr->hash_offset    = be64_to_cpu(vbmeta_hdr->hash_offset);
	vbmeta_hdr->hash_size      = be64_to_cpu(vbmeta_hdr->hash_size);
	vbmeta_hdr->signature_offset =
		be64_to_cpu(vbmeta_hdr->signature_offset);
	vbmeta_hdr->signature_size = be64_to_cpu(vbmeta_hdr->signature_size);
	vbmeta_hdr->public_key_offset =
		be64_to_cpu(vbmeta_hdr->public_key_offset);
	vbmeta_hdr->public_key_size = be64_to_cpu(vbmeta_hdr->public_key_size);
	vbmeta_hdr->public_key_metadata_offset =
		be64_to_cpu(vbmeta_hdr->public_key_metadata_offset);
	vbmeta_hdr->public_key_metadata_size =
		be64_to_cpu(vbmeta_hdr->public_key_metadata_size);
	vbmeta_hdr->descriptors_offset =
		be64_to_cpu(vbmeta_hdr->descriptors_offset);
	vbmeta_hdr->descriptors_size =
		be64_to_cpu(vbmeta_hdr->descriptors_size);
	vbmeta_hdr->rollback_index = be64_to_cpu(vbmeta_hdr->descriptors_size);
	vbmeta_hdr->flags	  = be32_to_cpu(vbmeta_hdr->flags);
}

int sunxi_avb_verify_all_vbmeta(AvbVBMetaImageHeader *vbmeta_hdr,
				AvbVBMetaImageHeader *vbmeta_hdr_system,
				AvbVBMetaImageHeader *vbmeta_hdr_vendor,
				char *sha1)
{
	int ret   = 0;
	int rbyte = 0;
	u32 start_block;
	int vbmeta_size	= sunxi_avb_vbmeta_size(vbmeta_hdr);
	int vbmeta_system_size = sunxi_avb_vbmeta_size(vbmeta_hdr_system);
	int vbmeta_vendor_size = sunxi_avb_vbmeta_size(vbmeta_hdr_vendor);
	u8 *addr;
	u8 *addr_vbmeta;
	u8 *addr_vbmeta_system;
	u8 *addr_vbmeta_vendor;
	u8 sha1_hash[64];
	int all_blk =
		(vbmeta_size + vbmeta_system_size + vbmeta_vendor_size) / 512 +
		3;
	pr_msg("alloc block = %d \n", all_blk);
	addr = (u8 *)memalign(CACHE_LINE_SIZE,
			      all_blk * 512 + CONFIG_SUNXI_SHA_CAL_PADDING);
	addr_vbmeta	= (u8 *)malloc(vbmeta_size + 512);
	addr_vbmeta_system = (u8 *)malloc(vbmeta_system_size + 512);
	addr_vbmeta_vendor = (u8 *)malloc(vbmeta_vendor_size + 512);
	start_block	= sunxi_partition_get_offset_byname("vbmeta");
	if (!start_block) {
		pr_msg("cant find part named vbmeta\n");
	} else {
		rbyte = sunxi_flash_read(start_block, vbmeta_size / 512 + 1,
					 addr_vbmeta);
	}
	start_block = sunxi_partition_get_offset_byname("vbmeta_system");
	if (!start_block) {
		pr_msg("cant find part named vbmeta\n");
	} else {
		rbyte = sunxi_flash_read(start_block,
					 vbmeta_system_size / 512 + 1,
					 addr_vbmeta_system);
	}
	start_block = sunxi_partition_get_offset_byname("vbmeta_vendor");
	if (!start_block) {
		pr_msg("cant find part named vbmeta\n");
	} else {
		rbyte = sunxi_flash_read(start_block,
					 vbmeta_vendor_size / 512 + 1,
					 addr_vbmeta_vendor);
	}
	memcpy(addr, addr_vbmeta, vbmeta_size);
	memcpy(addr + vbmeta_size, addr_vbmeta_system, vbmeta_system_size);
	memcpy(addr + vbmeta_size + vbmeta_system_size, addr_vbmeta_vendor,
	       vbmeta_vendor_size);
	sunxi_ss_open();
	rbyte = sunxi_sha_calc(sha1_hash, 32, addr,
			       vbmeta_size + vbmeta_system_size +
				       vbmeta_vendor_size);
	if (rbyte) {
		printf("sunxi_verify_signature err: calc hash failed\n");
		/*sunxi_ss_close();*/
		return ret;
	}
	sunxi_set_verify_boot_blob(SUNXI_VB_INFO_HASH, sha1_hash, 32);
	char *p = sha1;
	int i   = 0;
	for (i = 0; i < 32; i++) {
		sprintf(p, "%02x", sha1_hash[i]);
		p += 2;
	}
	pr_msg("vbmeta hash is %s\n", sha1);
	free(addr);
	free(addr_vbmeta);
	free(addr_vbmeta_system);
	free(addr_vbmeta_vendor);
	ret = 1;
	return ret;
}

int sunxi_avb_verify_vbmeta(AvbVBMetaImageHeader *vbmeta_hdr, char *sha1)
{
	int ret   = 0;
	int rbyte = 0;
	u32 start_block;
	int vbmeta_size = vbmeta_hdr->authentication_data_block_size +
			  vbmeta_hdr->auxiliary_data_block_size +
			  AVB_VBMETA_IMAGE_HEADER_SIZE;
	u8 *addr;
	u8 sha1_hash[64];
	int num_blk = vbmeta_size / 512 + 1;
	addr	= (u8 *)memalign(CACHE_LINE_SIZE,
			      num_blk * 512 + CONFIG_SUNXI_SHA_CAL_PADDING);
	start_block = sunxi_partition_get_offset_byname("vbmeta_system");
	if (!start_block) {
		pr_msg("cant find part named vbmeta_system\n");
	} else {
		rbyte = sunxi_flash_read(start_block, num_blk, addr);
		sunxi_ss_open();
		rbyte = sunxi_sha_calc(sha1_hash, 32, addr, vbmeta_size);
		sunxi_dump(sha1_hash, 32);
		if (rbyte) {
			printf("sunxi_verify_signature err: calc hash failed\n");
			/*sunxi_ss_close();*/
			return ret;
		}
		char *p = sha1;
		int i   = 0;
		for (i = 0; i < 32; i++) {
			sprintf(p, "%02x", sha1_hash[i]);
			p += 2;
		}
		pr_msg("vbmeta hash is %s\n", sha1);
		ret = 1;
	}
	free(addr);
	return ret;
}

int sunxi_avb_get_descriptor_by_name(const uint8_t *image_data,
				     size_t image_size, const char *name,
				     AvbDescriptor **out_descriptor)
{
	const AvbVBMetaImageHeader *header = NULL;
	const uint8_t *image_end;
	const uint8_t *desc_start;
	const uint8_t *desc_end;
	const uint8_t *p;
	AvbHashDescriptor swappedDesc;

	header    = (const AvbVBMetaImageHeader *)image_data;
	image_end = image_data + image_size;

	desc_start = image_data + sizeof(AvbVBMetaImageHeader) +
		     be64_to_cpu(header->authentication_data_block_size) +
		     be64_to_cpu(header->descriptors_offset);

	desc_end = desc_start + be64_to_cpu(header->descriptors_size);

	if (desc_start < image_data || desc_start > image_end ||
	    desc_end < image_data || desc_end > image_end ||
	    desc_end < desc_start) {
		pr_error("Descriptors not inside passed-in data.\n");
		return -3;
	}

	for (p = desc_start; p < desc_end;) {
		const AvbDescriptor *dh = (const AvbDescriptor *)p;
		avb_assert_aligned(dh);
		uint64_t nb_following = be64_to_cpu(dh->num_bytes_following);
		uint64_t nb_total     = sizeof(AvbDescriptor) + nb_following;
		switch ((be64_to_cpu(dh->tag))) {
		case AVB_DESCRIPTOR_TAG_HASH:
			if (avb_hash_descriptor_validate_and_byteswap(
				    (AvbHashDescriptor *)dh, &swappedDesc)) {
				const char *part_name =
					(const char *)p +
					sizeof(AvbHashDescriptor);

				if (memcmp(part_name, name,
					   swappedDesc.partition_name_len) ==
				    0) {
					*out_descriptor = malloc(nb_total);
					if (*out_descriptor == 0) {
						pr_error(
							"malloc for descriptor failed\n");
						return -2;
					}
					memcpy(*out_descriptor, p, nb_total);
					memcpy(*out_descriptor, &swappedDesc,
					       sizeof(AvbHashDescriptor));
					return 0;
				}
			}
			break;
		}
		p += nb_total;
	}

	return -1;
}

int sunxi_avb_get_hash_descriptor_by_name(const char *name,
					  const uint8_t *meta_data,
					  size_t meta_data_size,
					  AvbDescriptor **desc)
{
	if (sunxi_avb_get_descriptor_by_name(meta_data, meta_data_size, name,
					     desc)) {
		pr_error("no descriptor for %s\n", name);
		return -1;
	}

	switch ((*desc)->tag) {
	case AVB_DESCRIPTOR_TAG_HASH:
		/*do nothing*/
		break;
	default:
		pr_error("partition has a non hash descriptor\n");
		return -1;
		break;
	}
	return 0;
}

int sunxi_vbmeta_self_verify(const uint8_t *meta_data, size_t meta_data_size,
			     sunxi_key_t *out_pk)
{
	AvbVBMetaImageHeader h;
	const uint8_t *header_block;
	const uint8_t *authentication_block;
	const uint8_t *auxiliary_block;
	const uint8_t *self_sign_key;
	uint8_t hash_result[32];
	uint32_t total_len;
	AvbRSAPublicKeyHeader key_h;

	memcpy(&h, meta_data, sizeof(AvbVBMetaImageHeader));
	if (!sunxi_avb_check_magic((AvbVBMetaImageHeader *)&h)) {
		pr_error("invalid vbmeta head\n");
		return -1;
	}
	sunxi_avb_slot_vbmeta_head((AvbVBMetaImageHeader *)&h);
	/* Ensure inner block sizes are multiple of 64. */
	if ((h.authentication_data_block_size & 0x3f) != 0 ||
	    (h.auxiliary_data_block_size & 0x3f) != 0) {
		pr_error("Block size is not a multiple of 64.\n");
		return -1;
	}
	if (h.hash_size != 32) {
		pr_error("expected 32byte hash\n");
		return -1;
	}

	header_block	     = (uint8_t *)meta_data;
	authentication_block = header_block + sizeof(AvbVBMetaImageHeader);
	auxiliary_block =
		authentication_block + h.authentication_data_block_size;

	sunxi_ss_open();

	total_len = sizeof(AvbVBMetaImageHeader) + h.auxiliary_data_block_size;
	uint8_t *hash_cal_buf = memalign(
		CACHE_LINE_SIZE, total_len + CONFIG_SUNXI_SHA_CAL_PADDING);
	if (hash_cal_buf == NULL) {
		pr_error("not enough memory\n");
	}
	memcpy(hash_cal_buf, header_block, sizeof(AvbVBMetaImageHeader));
	memcpy(hash_cal_buf + sizeof(AvbVBMetaImageHeader), auxiliary_block,
	       h.auxiliary_data_block_size);
	sunxi_sha_calc(hash_result, 32, hash_cal_buf, total_len);
	free(hash_cal_buf);

	if (memcmp(authentication_block + h.hash_offset, hash_result, 32) !=
	    0) {
		pr_error("hash not match, hash of file:\n");
		sunxi_dump(hash_result, 32);
		pr_error("hash in authentication_block:\n");
		sunxi_dump((void *)authentication_block + h.hash_offset, 32);
		return -1;
	}
	sunxi_set_verify_boot_blob(SUNXI_VB_INFO_KEY, hash_result, 32);

	memcpy(&key_h, auxiliary_block + h.public_key_offset,
	       sizeof(AvbRSAPublicKeyHeader));
	key_h.key_num_bits = be32_to_cpu(key_h.key_num_bits);
	key_h.n0inv	   = 0x10001;
	if (key_h.key_num_bits != 2048 || key_h.n0inv != 0x10001) {
		pr_error("not supported key\n");
		pr_error("actual n size:%x, e:%x\n", key_h.key_num_bits,
			 key_h.n0inv);
		pr_error("exptect n size:%x, e:%x\n", 2048, 0x10001);
		return -2;
	}

	out_pk->n_len = 2048 / 8;
	out_pk->n     = malloc(2048 / 8);
	self_sign_key = auxiliary_block + h.public_key_offset +
			sizeof(AvbRSAPublicKeyHeader);
	for (total_len = 0; total_len < out_pk->n_len; total_len++) {
		out_pk->n[total_len] = self_sign_key[total_len];
	}

	/*
	 * key e always 0x10001 in avb, use actual len 3
	 * instead of preserved len 4.
	 */
	out_pk->e_len = 3;
	out_pk->e     = malloc(3);
	memcpy(out_pk->e, &key_h.n0inv, 3);

	memset(hash_result, 0, 32);
	sunxi_rsa_calc(out_pk->n, out_pk->n_len, out_pk->e, out_pk->e_len,
		       hash_result, 32,
		       (uint8_t *)authentication_block + h.signature_offset,
		       h.signature_size);
	if (memcmp(authentication_block + h.hash_offset, hash_result, 32) !=
	    0) {
		pr_error("hash not match, hash from signature:\n");
		sunxi_dump(hash_result, 32);
		pr_error("hash in authentication_block:\n");
		sunxi_dump((void *)authentication_block + h.hash_offset, 32);

		free(out_pk->e);
		free(out_pk->n);
		return -2;
	}

	return 0;
}

int sunxi_avb_get_vbmeta_flags(uint32_t *flags)
{
	u32 start_block;
	uint8_t vb_data[512];
	AvbVBMetaImageHeader *h;

	start_block = sunxi_partition_get_offset_byname("vbmeta");
	if (!start_block) {
		pr_error("cant find part named vbmeta\n");
		return -1;
	}

	sunxi_flash_read(start_block, 1, (void *)vb_data);
	if (!sunxi_avb_check_magic((AvbVBMetaImageHeader *)vb_data)) {
		pr_error("invalid vbmeta head\n");
		return -1;
	}
	sunxi_avb_slot_vbmeta_head((AvbVBMetaImageHeader *)&vb_data);
	h      = (AvbVBMetaImageHeader *)vb_data;
	*flags = h->flags;
	return 0;
}

int sunxi_avb_read_vbmeta_data(uint8_t **out_ptr, size_t *out_len)
{
	u32 start_block;
	uint8_t *vb_data;
	size_t vb_len;

	start_block = sunxi_partition_get_offset_byname("vbmeta");
	if (!start_block) {
		pr_error("cant find part named vbmeta\n");
		return -1;
	}

	vb_data = malloc(512);
	if (vb_data == NULL) {
		pr_error("not enough memory\n");
		return -1;
	}
	sunxi_flash_read(start_block, 1, (void *)vb_data);
	if (!sunxi_avb_check_magic((AvbVBMetaImageHeader *)vb_data)) {
		pr_error("invalid vbmeta head\n");
		free(vb_data);
		return -1;
	}
	sunxi_avb_slot_vbmeta_head((AvbVBMetaImageHeader *)vb_data);
	vb_len = sunxi_avb_vbmeta_size((AvbVBMetaImageHeader *)vb_data);
	vb_len = ALIGN(vb_len, 4096);
	free((void *)vb_data);
	vb_data =
		memalign(CACHE_LINE_SIZE, (vb_len + 511) / 512 * 512 +
						  CONFIG_SUNXI_SHA_CAL_PADDING);
	if (vb_data == NULL) {
		pr_error("not enough memory\n");
		return -1;
	}
	sunxi_flash_read(start_block, (vb_len + 511) / 512, (void *)vb_data);

	*out_ptr = vb_data;
	*out_len = vb_len;
	return 0;
}

int sunxi_avb_read_vbmeta_in_partition(const char *part_name, uint8_t **out_ptr,
				       size_t *out_len)
{
	u32 start_block;
	u32 part_sectors;
	uint8_t *avb_data;
	uint8_t avb_footer_sector[512];
	uint8_t *avb_footer_buf;
	uint32_t start_addr;
	uint32_t read_len;
	AvbFooter footer;

	if (sunxi_partition_get_info_byname(part_name, &start_block,
					    &part_sectors)) {
		pr_error("cant find part named %s\n", part_name);
		return -1;
	}
	sunxi_flash_read(start_block + part_sectors - 1, 1, avb_footer_sector);
	avb_footer_buf = &avb_footer_sector[512 - AVB_FOOTER_SIZE];
	if (!avb_footer_validate_and_byteswap((const AvbFooter *)avb_footer_buf,
					      &footer)) {
		pr_err("%s: No footer detected.\n", part_name);
		return -1;
	}
	start_addr = round_down(footer.vbmeta_offset, 512);
	read_len   = round_up(footer.vbmeta_size + footer.vbmeta_offset, 512) -
		   start_addr;

	avb_data =
		memalign(CACHE_LINE_SIZE, (read_len + 511) / 512 * 512 +
						  CONFIG_SUNXI_SHA_CAL_PADDING);
	if (avb_data == NULL) {
		pr_error("not enough memory\n");
		return -1;
	}
	sunxi_flash_read(start_block + start_addr / 512, read_len / 512,
			 (void *)avb_data);

	if ((footer.vbmeta_offset - start_addr) != 0) {
		memcpy(avb_data, avb_data + (footer.vbmeta_offset - start_addr),
		       footer.vbmeta_size);
	}
	*out_ptr = avb_data;
	*out_len = footer.vbmeta_size;
	return 0;
}
