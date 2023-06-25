/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * This is from the Android Project,
 * Repository: https://android.googlesource.com/platform/system/core/
 * File: mkbootimg/bootimg.h
 * Commit: d162828814b08ada310846a33205befb69ef5799
 *
 * Copyright (C) 2008 The Android Open Source Project
 */

#ifndef _ANDROID_IMAGE_H_
#define _ANDROID_IMAGE_H_

typedef struct andr_img_hdr andr_img_hdr;
typedef struct boot_img_hdr boot_img_hdr;
typedef struct vendor_boot_img_hdr vendor_boot_img_hdr;

#define ANDR_BOOT_DTBO_MAIGC "androidboot.dtbo_idx"
#define ANDR_BOOT_MAGIC "ANDROID!"
#define ANDR_BOOT_MAGIC_SIZE 8
#define ANDR_BOOT_NAME_SIZE 16
#define ANDR_BOOT_ARGS_SIZE 512
#define ANDR_BOOT_EXTRA_ARGS_SIZE 1024

#define VENDOR_BOOT_MAGIC_SIZE 8
#define VENDOR_BOOT_MAGIC "VNDRBOOT"
#define VENDOR_BOOT_NAME_SIZE 16
#define VENDOR_BOOT_ARGS_SIZE 2048
#define VENDOR_BOOT_HEAD_SIZE 2108

struct andr_img_hdr {
	char magic[ANDR_BOOT_MAGIC_SIZE];

	u32 kernel_size;	/* size in bytes */
	u32 kernel_addr;	/* physical load addr */

	u32 ramdisk_size;	/* size in bytes */
	u32 ramdisk_addr;	/* physical load addr */

	u32 second_size;	/* size in bytes */
	u32 second_addr;	/* physical load addr */

	u32 tags_addr;		/* physical addr for kernel tags */
	u32 page_size;		/* flash page size we assume */
	u32 unused;		/* reserved for future expansion: MUST be 0 */

	/* operating system version and security patch level; for
	 * version "A.B.C" and patch level "Y-M-D":
	 * ver = A << 14 | B << 7 | C         (7 bits for each of A, B, C)
	 * lvl = ((Y - 2000) & 127) << 4 | M  (7 bits for Y, 4 bits for M)
	 * os_version = ver << 11 | lvl */
	u32 os_version;

	char name[ANDR_BOOT_NAME_SIZE]; /* asciiz product name */

	char cmdline[ANDR_BOOT_ARGS_SIZE];

	u32 id[8]; /* timestamp / checksum / sha1 / etc */

	/* Supplemental command line data; kept here to maintain
	 * binary compatibility with older versions of mkbootimg */
	char extra_cmdline[ANDR_BOOT_EXTRA_ARGS_SIZE];
	u32 recovery_dtbo_size;	/* size of recovery dtbo image */
	u64 recovery_dtbo_offset;	/*physical load addr */
	u32 header_size;	/*size of boot image header in bytes */
	u32 dtb_size;
	u64 dtb_addr;
} __attribute__((packed));

/*
 * Add allwinner certification to image file
 */
#define AW_CERT_MAGIC "AW_CERT!"
#define AW_CERT_DESC_SIZE		64
#define AW_PAGESIZE			2048
#define AW_IMAGE_PADDING_LEN	(AW_PAGESIZE-sizeof(struct andr_img_hdr)-\
									AW_CERT_DESC_SIZE)

/*
 * Pagesize (2048 bytes) offset
 */
struct boot_img_hdr_ex {
	struct andr_img_hdr std_hdr;
	unsigned char padding[AW_IMAGE_PADDING_LEN];

	/*Reserved AW_CERT_DESC_SIZE for allwinner specific data*/
	unsigned char cert_magic[ANDR_BOOT_MAGIC_SIZE];
	unsigned cert_size;
};

/*
 * +-----------------+
 * | boot header     | 1 page
 * +-----------------+
 * | kernel          | n pages
 * +-----------------+
 * | ramdisk         | m pages
 * +-----------------+
 * | second stage    | o pages
 * +-----------------+
 *
 * n = (kernel_size + page_size - 1) / page_size
 * m = (ramdisk_size + page_size - 1) / page_size
 * o = (second_size + page_size - 1) / page_size
 *
 * 0. all entities are page_size aligned in flash
 * 1. kernel and ramdisk are required (size != 0)
 * 2. second is optional (second_size == 0 -> no second)
 * 3. load each element (kernel, ramdisk, second) at
 *    the specified physical address (kernel_addr, etc)
 * 4. prepare tags at tag_addr.  kernel_args[] is
 *    appended to the kernel commandline in the tags.
 * 5. r0 = 0, r1 = MACHINE_TYPE, r2 = tags_addr
 * 6. if second_size != 0: jump to second_addr
 *    else: jump to kernel_addr
 */

struct boot_img_hdr {
	u8 magic[ANDR_BOOT_MAGIC_SIZE];
	u32 kernel_size; /* size in bytes */
	u32 ramdisk_size; /* size in bytes */
	u32 os_version;
	u32 header_size; /* size of boot image header in bytes */
	u32 reserved[4];
	u32 header_version; /* offset remains constant for version check */
	char cmdline[ANDR_BOOT_ARGS_SIZE + ANDR_BOOT_EXTRA_ARGS_SIZE];
} __attribute__((packed));

struct vendor_boot_img_hdr {
	u8 magic[VENDOR_BOOT_MAGIC_SIZE];
	u32 header_version;
	u32 page_size; /* flash page size we assume */
	u32 kernel_addr; /* physical load addr */
	u32 ramdisk_addr; /* physical load addr */
	u32 vendor_ramdisk_size; /* size in bytes */
	u8 cmdline[VENDOR_BOOT_ARGS_SIZE];
	u32 tags_addr; /* physical addr for kernel tags */
	u8 name[VENDOR_BOOT_NAME_SIZE]; /* asciiz product name */
	u32 header_size; /* size of vendor boot image header in
	* bytes */
	u32 dtb_size; /* size of dtb image */
	u64 dtb_addr; /* physical load address */
} __attribute__((packed));


#endif
