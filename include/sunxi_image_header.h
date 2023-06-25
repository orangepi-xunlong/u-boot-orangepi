/*
 * (C) Copyright 2007-2021
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef	__SUNXI_IMAGE_HEADER__H__
#define	__SUNXI_IMAGE_HEADER__H__

#define SUNXI_IH_MAGIC	(0x48495741)
#define SUNXI_TH_MAGIC	(0x4854)

#define PKEY_TYPE_RSA	(0x0)
#define PKEY_TYPE_ECC	(0x1)

/* Image Header(128B) */
typedef struct sunxi_image_header {
	uint32_t ih_magic;             /* Image Header Magic Number */
	uint16_t ih_hversion;          /* Image Header Version:     */
	uint16_t ih_pversion;          /* Image Payload Version:    */
	uint16_t ih_hchksum;           /* Image Header Checksum     */
	uint16_t ih_dchksum;           /* Image Data Checksum       */
	uint32_t ih_hsize;             /* Image Header Size         */
	uint32_t ih_psize;             /* Image Payload Size        */
	uint32_t ih_tsize;             /* Image TLV Size            */
	uint32_t ih_load;              /* Image Load Address        */
	uint32_t ih_ep;                /* Image Entry Point         */
	uint32_t ih_imgattr;           /* Image Attribute           */
	uint32_t ih_nxtsecaddr;        /* Next Section Address      */
	uint8_t  ih_name[16];          /* Image Name                */
	uint32_t ih_priv[18];          /* Image Private Data        */
} sunxi_image_header_t;

/* TLV Header(32B) */
typedef struct sunxi_tlv_header {
	uint16_t th_magic;
	uint16_t th_size;
	uint16_t th_pkey_type;
	uint16_t th_pkey_size;
	uint16_t th_sign_type;
	uint16_t th_sign_size;
	uint32_t th_priv[5];
} sunxi_tlv_header_t;

int sunxi_image_header_check(sunxi_image_header_t *ih);
int sunxi_tlv_header_check(sunxi_tlv_header_t *th);
#endif	//__SUNXI_IMAGE_HEADER__H__
