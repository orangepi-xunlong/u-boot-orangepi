/*
 * Copyright (c) 2019, Cvitek. All rights reserved.
 *
 */
#ifndef __CV_EFUSE_H
#define __CV_EFUSE_H

#include "platform_common_def.h"

// Non-secure part
#define EFUSE_SHADOW_REG    (EFUSE_BASE + 0x100)
#define EFUSE_SIZE 0x100
#define EFUSE_CUSTOMER		(EFUSE_SHADOW_REG + 0x04)
#define EFUSE_FTSN1		(EFUSE_SHADOW_REG + 0x04)
#define EFUSE_FTSN2		(EFUSE_SHADOW_REG + 0x08)
#define EFUSE_FTSN3		(EFUSE_SHADOW_REG + 0x0C)
#define EFUSE_FTSN4		(EFUSE_SHADOW_REG + 0x10)
#define EFUSE_PATCH_START   (EFUSE_SHADOW_REG + 0x2C)
#define EFUSE_PATCH_SIZE    96	/* 96 bytes = 24 words = 12 pair of entities */
#define EFUSE_MARKET_SEGMENT			(EFUSE_SHADOW_REG + 0x14)
#define EFUSE_BONDING0					(EFUSE_SHADOW_REG + 0x28)
#define EFUSE_NV_COUNTER				(EFUSE_SHADOW_REG + 0x94)
#define EFUSE_SCS_CONFIG_REG			(EFUSE_SHADOW_REG + 0xA0)
#define EFUSE_KPUB_HASH_REG				(EFUSE_SHADOW_REG + 0xA8)
#define EFUSE_MFR_SECRET_ID_REG			(EFUSE_SHADOW_REG + 0xFC)

#define EFUSE_SECRET_START_OFFSET 0xc8
#define EFUSE_SECRET_END_OFFSET 0x100

#define EFUSE_LOCK_ADDR 0xF8

enum {
	EFUSE_LOCK_KPUB0,
	EFUSE_LOCK_DEBUG_PW,
	EFUSE_LOCK_LOADER_EK,
	EFUSE_LOCK_DEVICE_EK,
};

// Secure part
#define SEC_EFUSE_SHADOW_REG			(SEC_EFUSE_BASE + 0x100)
#define EFUSE_LDR_DES_KEY_REG			(SEC_EFUSE_SHADOW_REG + 0xD8)

#define EFUSE_MODE      (EFUSE_BASE + 0x0)
#define EFUSE_ADR       (EFUSE_BASE + 0x4)
#define EFUSE_DIR_CMD   (EFUSE_BASE + 0x8)
#define EFUSE_RD_DATA   (EFUSE_BASE + 0xC)
#define EFUSE_STATUS    (EFUSE_BASE + 0x10)
#define EFUSE_ONE_WAY   (EFUSE_BASE + 0x14)

#define BIT_AREAD       (1<<0)
#define BIT_MREAD       (1<<1)
#define BIT_PRG         (1<<2)
#define BIT_PWR_DN      (1<<3)
#define BIT_CMD         (1<<4)
#define BIT_BUSY        (1<<0)
#define CMD_REFRESH     (0x30)

#define BIT_SCS_ENABLE			0
#define BIT_TEE_SCS_ENABLE		2
#define BIT_BOOT_LOADER_ENCRYPTION	6
#define BIT_ROOT_PUBLIC_KEY_SELECTION	20
#define BIT_LDR_KEY_SELECTION		23
#define BIT_ROM_PATCH_DISABLE		26

// Field of EFUSE_CUSTOMER
#define EC_FASTBOOT_SHIFT 0x0
#define EC_FASTBOOT_MASK 0x7

#define EC_FMUX_VALUE_SHIFT 0x5
#define EC_FMUX_VALUE_MASK 0x7

#define EC_FMUX_OFFSET_SHIFT 0x8
#define EC_FMUX_OFFSET_MASK 0xFF

#define EC_GPIO_OFFSET_SHIFT 0x10
#define EC_GPIO_OFFSET_MASK 0x1F

#define EC_GPIO_PORT_SHIFT 0x15
#define EC_GPIO_PORT_MASK 0x7

enum EFUSE_READ_TYPE {
	AREAD,
	MREAD
};

enum EFUSE_PATTERN {
	USERCONF,
	PATCH
};

struct efuse_area {
	uint32_t start;
	uint32_t size;
};

#endif /*__CV_EFUSE_H*/
