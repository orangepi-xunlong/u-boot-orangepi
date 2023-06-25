/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * drivers/dsp/sun20iw1/dsp_reg.h
 *
 * Copyright (c) 2007-2025 Allwinnertech Co., Ltd.
 * Author: wujiayi <wujiayi@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details
 *
 */

#ifndef __DSP_REG_H
#define __DSP_REG_H

/*
 * DSP CFG BASE
 */
#define DSP0_CFG_BASE		(0x01700000)

/* DSP default reset vector address */
#define DSP_DEFAULT_RST_VEC	(0x100000)

/*
 * Register define
 */
#define DSP_ALT_RESET_VEC_REG	(0x0000) /* DSP Reset Control Register */
#define DSP_CTRL_REG0		(0x0004) /* DSP Control Register0 */
#define DSP_PRID_REG		(0x000c) /* DSP PRID Register */
#define DSP_STAT_REG		(0x0010) /* DSP STAT Register */
#define DSP_BIST_CTRL_REG	(0x0014) /* DSP BIST CTRL Register */
#define DSP_JTRST_REG		(0x001c) /* DSP JTAG CONFIG RESET Register */
#define DSP_VER_REG		(0x0020) /* DSP Version Register */

/*
 * DSP Control Register0
 */
#define BIT_RUN_STALL		(0)
#define BIT_START_VEC_SEL	(1)
#define BIT_DSP_CLKEN		(2)

/*
 * DSP PRID Register
 */
#define PRID_MASK		(0xff << 0)

/*
 * DSP STAT Register
 */
#define BIT_PFAULT_INFO_VALID	(0)
#define BIT_PFAULT_ERROR	(1)
#define BIT_DOUBLE_EXCE_ERROR	(2)
#define BIT_XOCD_MODE		(3)
#define BIT_DEBUG_MODE		(4)
#define BIT_PWAIT_MODE		(5)
#define BIT_IRAM0_LOAD_STORE	(6)

/*
 * BIST Control Register
 */
#define BIT_BIST_EN		(0)
#define BIST_WDATA_PAT_MASK	(0x7 << 1)
#define BIT_BIST_ADDR_MODE_SEL	(4)
#define BIST_REG_SEL_MASK	(0x7 << 5)
#define BIT_BIST_BUSY		(8)
#define BIT_BIST_STOP		(9)
#define BIST_ERR_CYC_MASK	(0x3 << 10)
#define BIST_ERR_PAT_MASK	(0x7 << 12)
#define BIT_BIST_ERR_STA	(15)
#define BIST_SELECT_MASK	(0xf << 16)

/*
 * DSP Version Register
 */
#define SMALL_VER_MASK		(0x1f << 0)
#define LARGE_VER_MASK		(0x1f << 16)

/*
 * CCMU related
 */
#define CCMU_DSP_CLK_REG	(0xc70)
#define BIT_DSP_SCLK_GATING	(31)
#define DSP_CLK_M_MASK		(0x1f << 0)
#define DSP_CLK_SRC_MASK	(0x7 << 24)
#define DSP_CLK_SRC_HOSC	(0)
#define DSP_CLK_SRC_32K		(0x1 << 24)
#define DSP_CLK_SRC_16M		(0x2 << 24)
#define DSP_CLK_SRC_PERI2X	(0x3 << 24)
#define DSP_CLK_SRC_AUDIO1_DIV2	(0x4 << 24)
/* x must be 1 - 32 */
#define DSP_CLK_FACTOR_M(x)      (((x) - 1) << 0)

#define CCMU_DSP_BGR_REG	(0xc7c)
#define BIT_DSP0_CFG_GATING	(1)
#define BIT_DSP0_RST		(16)
#define BIT_DSP0_CFG_RST	(17)
#define BIT_DSP0_DBG_RST	(18)

#define BIT_SRAM_REMAP_ENABLE  (0)
#define SRAMC_SRAM_REMAP_REG   (0x8)

#endif /* __DSP_I_H */
