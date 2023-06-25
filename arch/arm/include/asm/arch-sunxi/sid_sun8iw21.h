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

#define SID_ROTPK_VALUE(n)		(SUNXI_SID_BASE + 0x120 + (n * 4))
#define SID_ROTPK_CTRL			(SUNXI_SID_BASE + 0x140)
#define SID_ROTPK_EFUSED_BIT    (1)
#define SID_ROTPK_CMP_RET_BIT	(0)

#define SID_EFUSE               (SUNXI_SID_BASE + 0x200)
#define SID_SECURE_MODE         (SUNXI_SID_BASE + 0xA0)
#define SID_OP_LOCK  (0xAC)

#define EFUSE_CHIPID            (0x0)
#define EFUSE_ROTPK             (0xA8)
/* write protect */
#define EFUSE_WRITE_PROTECT     (0x38)
/* read  protect */
#define EFUSE_READ_PROTECT      (0x3c)
/* jtag security */
#define EFUSE_LCJS              (0x40)

/*efuse power ctl*/
#define EFUSE_HV_SWITCH			(SUNXI_RTC_BASE + 0x204)
#endif    /*  #ifndef __SID_H__  */
