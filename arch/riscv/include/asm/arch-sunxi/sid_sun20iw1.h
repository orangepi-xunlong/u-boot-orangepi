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

#define SID_PRCTL               (IOMEM_ADDR(SUNXI_SID_BASE) + 0x40)
#define SID_PRKEY               (IOMEM_ADDR(SUNXI_SID_BASE) + 0x50)
#define SID_RDKEY               (IOMEM_ADDR(SUNXI_SID_BASE) + 0x60)
#define SJTAG_AT0               (IOMEM_ADDR(SUNXI_SID_BASE) + 0x80)
#define SJTAG_AT1               (IOMEM_ADDR(SUNXI_SID_BASE) + 0x84)
#define SJTAG_S                 (IOMEM_ADDR(SUNXI_SID_BASE) + 0x88)
#define SID_EFUSE               (IOMEM_ADDR(SUNXI_SID_BASE) + 0x200)
#define SID_OP_LOCK  (0xAC)

#define EFUSE_CHIPID            (0x0)

#define EFUSE_ANTI_BRUSH		(0x10)

#define ANTI_BRUSH_BIT_OFFSET			(31)
#define ANTI_BRUSH_MODE			(SID_EFUSE + EFUSE_ANTI_BRUSH)

/* write protect */
#define EFUSE_WRITE_PROTECT		(0x40)
/* read  protect */
#define EFUSE_READ_PROTECT		(0x44)
/* jtag security */

#define EFUSE_ROTPK					(0x70)
#define EFUSE_OEM_PROGRAM			(0x38)
#define SID_OEM_PROGRAM_SIZE		(64)

/*write protect*/
#define SCC_ROTPK_BURNED_FLAG					(12)

/*efuse power ctl*/
#define EFUSE_HV_SWITCH			(IOMEM_ADDR(SUNXI_RTC_BASE) + 0x204)
#endif    /*  #ifndef __SID_H__  */
