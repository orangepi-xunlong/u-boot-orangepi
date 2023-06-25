/*
 * (C) Copyright 2017  <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#ifndef _SUNXI_AVB_H_
#define _SUNXI_AVB_H_

#include <common.h>
#include <avb_verify.h>
#include <openssl_ext.h>

extern int sunxi_avb_check_magic(AvbVBMetaImageHeader *vbmeta_hdr);
extern int sunxi_avb_read_vbmeta_head(char *name, u8 *addr);
extern u64 sunxi_avb_vbmeta_size(AvbVBMetaImageHeader *vbmeta_hdr);
extern void sunxi_avb_dump_vbmeta(AvbVBMetaImageHeader *vbmeta_hdr);
extern void sunxi_avb_setup_env(char *avb_version, char *hash_alg, u64 vbmeta_size, char *sha1_hash, char *lock_state);
extern void sunxi_avb_slot_vbmeta_head(AvbVBMetaImageHeader *vbmeta_hdr);
extern int sunxi_avb_verify_vbmeta(AvbVBMetaImageHeader *vbmeta_hdr, char *sha1);
extern int sunxi_avb_verify_all_vbmeta(AvbVBMetaImageHeader *vbmeta_hdr, AvbVBMetaImageHeader *vbmeta_hdr_system, AvbVBMetaImageHeader *vbmeta_hdr_vendor, char *sha1);
extern int sunxi_avb_get_hash_descriptor_by_name(const char *name,
						 const uint8_t *meta_data,
						 size_t meta_data_size,
						 AvbDescriptor **desc);
extern int sunxi_vbmeta_self_verify(const uint8_t *meta_data, size_t meta_data_size,
					     sunxi_key_t *out_pk);
extern int sunxi_avb_read_vbmeta_data(uint8_t **out_ptr, size_t *out_len);
extern int sunxi_avb_read_vbmeta_in_partition(const char *part_name,
					      uint8_t **out_ptr,
					      size_t *out_len);
extern int sunxi_avb_get_vbmeta_flags(uint32_t *flags);

#endif /*_SUNXI_AVB_H_*/
