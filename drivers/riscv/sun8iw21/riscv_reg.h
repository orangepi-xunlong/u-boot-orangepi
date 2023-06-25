/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * drivers/riscv/sun21iw1/riscv_reg.h
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

#ifndef __RISCV_REG_H
#define __RISCV_REG_H

/*
 * RISCV CFG BASE
 */
#define RISCV_CFG_BASE		(0x06010000)

/* RISCV default reset vector address */
#define RISCV_DEFAULT_RST_VEC	(0x100000)

/*
 * Register define
 */
#define RISCV_VER_REG		(0x0000) /* RISCV Reset Control Register */
#define RF1P_CFG_REG		(0x0010) /* RISCV Control Register0 */
#define TS_TMODE_SEL_REG	(0x0040) /* RISCV PRID Register */
#define E907_STA_ADD_REG	(0x0204) /* RISCV STAT Register */
#define E907_WAKEUP_EN_REG	(0x0220) /* RISCV BIST CTRL Register */
#define E907_WAKEUP_MASK0_REG	(0x0224) /* RISCV JTAG CONFIG RESET Register */
#define E907_WAKEUP_MASK1_REG	(0x0228) /* RISCV JTAG CONFIG RESET Register */
#define E907_WAKEUP_MASK2_REG	(0x022C) /* RISCV JTAG CONFIG RESET Register */
#define E907_WAKEUP_MASK3_REG	(0x0230) /* RISCV JTAG CONFIG RESET Register */
#define E907_WAKEUP_MASK4_REG	(0x0234) /* RISCV JTAG CONFIG RESET Register */
#define E907_WORK_MODE_REG	(0x0248) /* RISCV Version Register */

/*
 * RISCV Control Register0
 */
#define BIT_RUN_STALL		(0)
#define BIT_START_VEC_SEL	(1)
#define BIT_RISCV_CLKEN		(2)

/*
 * RISCV PRID Register
 */
#define PRID_MASK		(0xff << 0)

/*
 * RISCV STAT Register
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
 * RISCV Version Register
 */
#define SMALL_VER_MASK		(0x1f << 0)
#define LARGE_VER_MASK		(0x1f << 16)

/*
 * CCMU related
 */
#define CCMU_RISCV_CLK_REG	(0x0d00)
#define RISCV_CLK_MASK		(0x7 << 24)
#define RISCV_CLK_HOSC		(0)
#define RISCV_CLK_32K		(0x1 << 24)
#define RISCV_CLK_16M		(0x2 << 24)
#define RISCV_CLK_PERI_600M	(0x3 << 24)
#define RISCV_CLK_PERI_480M	(0x4 << 24)
#define RISCV_CLK_CPUPLL	(0x5 << 24)
/* x must be 1 - 4 */
#define RISCV_AXI_FACTOR_N(x)	(((x) - 1) << 0)
/* x must be 1 - 32 */
#define RISCV_CLK_FACTOR_M(x)      (((x) - 1) << 0)
#define RISCV_CLK_M_MASK		(0x1f << 0)

#define RISCV_GATING_RST_REG	(0x0d04)
#define RISCV_GATING_RST_FIELD  (0x16aa << 16)
#define RISCV_SYS_APB_SOFT_RSTN (0x0 << 2)
#define RISCV_SOFT_RSTN		(0x0 << 1)
#define RISCV_CLK_GATING	(0x1 << 0)

#define RISCV_CFG_BGR_REG	(0x0d0c)
#define RISCV_CFG_RST		(0x1 << 16)
#define RISCV_CFG_GATING	(0x1 << 0)

#define RISCV_STA_ADD_REG	(0x0204)

#endif /* __RISCV_I_H */
