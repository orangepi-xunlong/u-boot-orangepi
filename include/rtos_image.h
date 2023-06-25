/*
 * (C) Copyright 2018 allwinnertech  <xulu@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 * xulu@allwinnertech.com
 */

#ifndef _RTOS_IMAGE_H_
#define _RTOS_IMAGE_H_

#define RTOS_BOOT_MAGIC "FREERTOS"

#ifndef AW_CERT_MAGIC
#define AW_CERT_MAGIC "AW_CERT!"
#endif

#pragma pack(4)
struct rtos_img_hdr {
	char rtos_magic[8];
	char cert_magic[8];

	u32 cert_offset;
	u32 cert_size;

	u32 rtos_offset;
	u32 rtos_size;

	char reserved[2016];

	unsigned char cert_data[2048];
};
#pragma pack()

/*
 * add 1 page before freertos-gz
 * +-----------------+
 * | rtos header     | 1 page
 * +-----------------+
 * | freertos-gz     | n pages
 * +-----------------+
 *
 *
 * rtos header format:
 * +-----------------+
 * | rtos magic      | 8 bytes
 * +-----------------+
 * | cert magic      | 8 bytes
 * +-----------------+
 * | cert offset     | 4 bytes (default is 2048)
 * +-----------------+
 * | cert_len        | 4 bytes
 * +-----------------+
 * | rtos offset     | 4 bytes
 * +-----------------+
 * | rtos len        | 4 bytes
 * +-----------------+
 * | reseved         | 2048 - 32 bytes
 * +-----------------+
 * | cert data       | 2048 bytes
 * +-----------------+
 *
 */
#endif
