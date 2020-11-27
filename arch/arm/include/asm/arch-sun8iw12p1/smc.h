/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei@allwinnertech.com
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef _TZASC_SMC_H_
#define _TZASC_SMC_H_

#include <asm/arch/platform.h>

#define SMC_CONFIG_REG                (SUNXI_SMC_BASE + 0x0000)
#define SMC_ACTION_REG                (SUNXI_SMC_BASE + 0x0004)
#define SMC_LD_RANGE_REG              (SUNXI_SMC_BASE + 0x0008)
#define SMC_LD_SELECT_REG             (SUNXI_SMC_BASE + 0x000c)
#define SMC_INT_STATUS_REG            (SUNXI_SMC_BASE + 0x0010)
#define SMC_INT_CLEAR_REG             (SUNXI_SMC_BASE + 0x0014)

#define SMC_FAIL_ADDR_REG             (SUNXI_SMC_BASE + 0x0020)
#define SMC_FAIL_CTRL_REG             (SUNXI_SMC_BASE + 0x0028)
#define SMC_FAIL_ID_REG               (SUNXI_SMC_BASE + 0x002c)
#define SMC_SPECU_CTRL_REG            (SUNXI_SMC_BASE + 0x0030)
#define SMC_INVER_EN_REG              (SUNXI_SMC_BASE + 0x0034)

#define SMC_DRM_MATER0_EN_REG         (SUNXI_SMC_BASE + 0x0050)
#define SMC_DRM_MATER1_EN_REG         (SUNXI_SMC_BASE + 0x0054)
#define SMC_DRM_ILLACCE_REG           (SUNXI_SMC_BASE + 0x0058)

#define SMC_MST0_BYP_REG              (SUNXI_SMC_BASE + 0x0070)
#define SMC_MST1_BYP_REG              (SUNXI_SMC_BASE + 0x0074)
#define SMC_MST2_BYP_REG              (SUNXI_SMC_BASE + 0x0078)

#define SMC_MST0_SEC_REG              (SUNXI_SMC_BASE + 0x0080)
#define SMC_MST1_SEC_REG              (SUNXI_SMC_BASE + 0x0084)
#define SMC_MST2_SEC_REG              (SUNXI_SMC_BASE + 0x0088)

#define SMC_MST0_ATTR_REG             (SUNXI_SMC_BASE + 0x0090)
#define SMC_MST1_ATTR_REG             (SUNXI_SMC_BASE + 0x0094)
#define SMC_MST2_ATTR_REG             (SUNXI_SMC_BASE + 0x0098)

#define DRM_BITMAP_CTRL_REG           (SUNXI_SMC_BASE + 0x00A0)
#define DRM_BITMAP_VAL_REG            (SUNXI_SMC_BASE + 0x00A4)
#define DRM_BITMAP_SEL_REG            (SUNXI_SMC_BASE + 0x00A8)

#define DRM_GPU_HW_RST_REG            (SUNXI_SMC_BASE + 0x00B8)
#define DRM_VERSION_REG               (SUNXI_SMC_BASE + 0x00F0)


#define SMC_REGIN_SETUP_LOW_REG(x)    (SUNXI_SMC_BASE + 0x100 + 0x10*(x))
#define SMC_REGIN_SETUP_HIGH_REG(x)   (SUNXI_SMC_BASE + 0x104 + 0x10*(x))
#define SMC_REGIN_ATTRIBUTE_REG(x)    (SUNXI_SMC_BASE + 0x108 + 0x10*(x))


int sunxi_smc_config(uint dram_size, uint secure_region_size);
int sunxi_drm_config(u32 drm_start, u32 dram_size);

#endif    /*  #ifndef _TZASC_SMC_H_  */
