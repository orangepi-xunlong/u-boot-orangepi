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

#define SID_PRCTL				(SUNXI_SID_BASE + 0x40)
#define SID_PRKEY				(SUNXI_SID_BASE + 0x50)
#define SID_RDKEY				(SUNXI_SID_BASE + 0x60)
#define SJTAG_AT0				(SUNXI_SID_BASE + 0x80)
#define SJTAG_AT1				(SUNXI_SID_BASE + 0x84)
#define SJTAG_S					(SUNXI_SID_BASE + 0x88)
#define SID_RF(n)				(SUNXI_SID_BASE + (n) * 4 + 0x80)

#define SID_EFUSE               (SUNXI_SID_BASE + 0x200)
#define SID_OP_LOCK				(0xAC)
#define EFUSE_LCJS				(0xF4)
#define EFUSE_CHIP_CONFIG       (0xFC)

/* write protect */
#define EFUSE_WRITE_PROTECT	EFUSE_CHIP_CONFIG
/* read  protect */
#define EFUSE_READ_PROTECT	EFUSE_CHIP_CONFIG
#endif    /*  #ifndef __SID_H__  */
