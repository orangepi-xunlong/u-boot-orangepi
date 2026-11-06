/*
 * Copyright (c) 2019, Cvitek. All rights reserved.
 *
 */
#ifndef __CVI_OTP_H__
#define __CVI_OTP_H__

#include <platform.h>

#define SEC_EFUSE_SHADOW_REG (CRYPTO_SIDE_OTP_BASE + 0x100)
#define EFUSE_SIZE 0x100

#define EFUSE_LDR_DES_KEY_REG (SEC_EFUSE_SHADOW_REG + 0xD8)

#define EFUSE_KPUB_HASH_REG (SEC_EFUSE_SHADOW_REG + 0xA8)

#define EFUSE_SCS_CONFIG_REG (SEC_EFUSE_SHADOW_REG + 0xA0)
#define BIT_SCS_ENABLE 0
#define BIT_TEE_SCS_ENABLE 2
#define BIT_BOOT_LOADER_ENCRYPTION 6

#define OTP_CFG_BASE			(CRYPTO_SIDE_OTP_BASE + 0x1280)
#define OTP_CFG_BUSY			(OTP_CFG_BASE + 0x20)
#define OTP_CFG_PDSTB			(OTP_CFG_BASE + 0x24)

#endif /*__CVI_OTP_H__*/
