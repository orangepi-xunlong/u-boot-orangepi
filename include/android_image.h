/*
 * This is from the Android Project,
 * Repository: https://android.googlesource.com/platform/bootable/bootloader/legacy
 * File: include/boot/bootimg.h
 * Commit: 4205b865141ff2e255fe1d3bd16de18e217ef06a
 *
 * Copyright (C) 2008 The Android Open Source Project
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _ANDROID_IMAGE_H_
#define _ANDROID_IMAGE_H_

#define ANDR_BOOT_MAGIC "ANDROID!"
#define ANDR_BOOT_MAGIC_SIZE 8
#define ANDR_BOOT_NAME_SIZE 16
#define ANDR_BOOT_ARGS_SIZE 512
#define BOOT_EXTRA_ARGS_SIZE 1024

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
	u32 unused[2];		/* future expansion: should be 0 */

	char name[ANDR_BOOT_NAME_SIZE]; /* asciiz product name */

	char cmdline[ANDR_BOOT_ARGS_SIZE];

	u32 id[8]; /* timestamp / checksum / sha1 / etc */

    /* Supplemental command line data; kept here to maintain
     * binary compatibility with older versions of mkbootimg */
  unsigned char extra_cmdline[BOOT_EXTRA_ARGS_SIZE];
};


/*
 * Add allwinner certification to image file
 */
#define AW_CERT_MAGIC "AW_CERT!"
#define AW_CERT_DESC_SIZE		64
#define AW_PAGESIZE				2048
#define AW_IMAGE_PADDING_LEN	(AW_PAGESIZE-sizeof(struct andr_img_hdr)-\
									AW_CERT_DESC_SIZE)

/*
 * Pagesize (2048 bytes) offset
 */
struct boot_img_hdr_ex
{
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
 * +-----------------+*
** | image certicate |
** +-----------------+
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
#endif
