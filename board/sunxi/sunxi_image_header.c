/*
 * (C) Copyright 2018-2021
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <sunxi_image_header.h>

/**
 * @brief Get 16-bit checksum of the data buffer
 * @param[in] buf Pointer to the data buffer
 * @param[in] len length of the data buffer
 * @return 16-bit checksum
 */
static uint16_t image_get_checksum16(void *buf, uint32_t len)
{
	uint16_t chksum16 = 0;
	uint32_t chksum32;
	uint32_t *p32;
	uint16_t *p16;

	if (buf == NULL) {
		printf("error: buf %p\n", buf);
		return 0;
	}

	p32 = (uint32_t *)buf;
	while (len >= 4) {
		chksum32 = *p32++;
		len -= 4;
		chksum16 += (uint16_t)(chksum32);
		chksum16 += (uint16_t)(chksum32 >> 16);
	}

	p16 = (uint16_t *)p32;
	while (len >= 2) {
		chksum16 += *p16++;
		len -= 2;
	}

	if (len > 0) {
		chksum16 += *(uint8_t *)p16;
	}

	return chksum16;
}

int sunxi_tlv_header_check(sunxi_tlv_header_t *th)
{
	// check tlv magic
	if (th->th_magic != SUNXI_TH_MAGIC) {
		printf("tlv header magic 0x%x error\n", th->th_magic);
		return -1;
	}

	// check pkey size and sign size
	if (th->th_pkey_type == PKEY_TYPE_RSA) {
		if (th->th_pkey_size != 2048 / 8 * 2) {
			printf("tlv rsa pkey size %d error\n", th->th_pkey_size);
			return -2;
		}
		if (th->th_sign_size != 2048 / 8) {
			printf("tlv rsa sign size %d error\n", th->th_sign_size);
			return -3;
		}
	} else if (th->th_pkey_type == PKEY_TYPE_ECC) {
		if (th->th_pkey_size != 256 / 8 * 2) {
			printf("tlv ecc pkey size %d error\n", th->th_pkey_size);
			return -2;
		}
		if (th->th_sign_size != 256 / 8 * 2) {
			printf("tlv ecc sign size %d error\n", th->th_sign_size);
			return -3;
		}
	} else {
		printf("error: unknow sign type: %d\n", th->th_pkey_type);
		return -4;
	}

	return 0;
}

int sunxi_image_header_check(sunxi_image_header_t *ih)
{
	uint16_t chksum = 0;

	// check image header magic
	if (ih->ih_magic != SUNXI_IH_MAGIC) {
		printf("image header magic 0x%x error\n", ih->ih_magic);
		return -1;
	}

	// image header checksum verify
	chksum = image_get_checksum16((void *)ih, ih->ih_hsize);
	if (chksum != 0xFFFF) {
		printf("header chksum error: %d!\n", chksum);
		return -2;
	}

	// payload checksum verify
	chksum = ih->ih_dchksum;
	chksum += image_get_checksum16((void *)((uint8_t *)ih + ih->ih_hsize), ih->ih_psize);
	if (chksum != 0xFFFF) {
		printf("data chksum error: %d!\n", chksum);
		return -3;
	}

	return 0;
}
