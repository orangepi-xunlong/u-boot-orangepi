/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * wangwei <wangwei@allwinnertech.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __SID_H__
#define __SID_H__

#include <linux/types.h>
#include <asm/arch/cpu.h>

#define SID_PRCTL               (SUNXI_SID_BASE + 0x40)
#define SID_PRKEY               (SUNXI_SID_BASE + 0x50)
#define SID_RDKEY               (SUNXI_SID_BASE + 0x60)
#define SJTAG_AT0               (SUNXI_SID_BASE + 0x80)
#define SJTAG_AT1               (SUNXI_SID_BASE + 0x84)
#define SJTAG_S                 (SUNXI_SID_BASE + 0x88)

#define SID_EFUSE               (SUNXI_SID_BASE + 0x200)
#define SID_SECURE_MODE         (SUNXI_SID_BASE + 0xA0)
#define SID_OP_LOCK  (0xAC)

#define EFUSE_CHIPD             (0x00) /* 0x0-0xf, 128bits */
#define EFUSE_BROM_CONFIG       (0x10) /* 16 bit config, 16 bits try */
#define EFUSE_THERMAL_SENSOR    (0x14) /* 0x14-0x1b, 64bits */
#define EFUSE_TF_ZONE           (0x20)
#define EFUSE_OEM_PROGRAM       (0x64)

/* write protect */
#define EFUSE_WRITE_PROTECT     (0xBC)
/* read  protect */
#define EFUSE_READ_PROTECT      (0xBC)

#define EFUSE_ROTPK             (0x30)
#define EFUSE_NV1               (0x50)

#define SID_OEM_PROGRAM_SIZE	(92)
#endif    /*  #ifndef __SID_H__  */
