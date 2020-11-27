/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CCMU_H
#define __CCMU_H


#include "platform.h"
#define CCMU_PLL_CPUX_CTRL_REG            (SUNXI_CCM_BASE + 0x00)
#define CCMU_PLL_AUDIO_CTRL_REG           (SUNXI_CCM_BASE + 0x08)
#define CCMU_PLL_VIDEO0_CTRL_REG          (SUNXI_CCM_BASE + 0x10)
#define CCMU_PLL_VE_CTRL_REG              (SUNXI_CCM_BASE + 0x18)
#define CCMU_PLL_DDR0_CTRL_REG            (SUNXI_CCM_BASE + 0x20)
#define CCMU_PLL_PERIPH0_CTRL_REG         (SUNXI_CCM_BASE + 0x28)

#define CCMU_PLL_VIDEO1_CTRL_REG          (SUNXI_CCM_BASE + 0x30)
#define CCMU_PLL_GPU_CTRL_REG             (SUNXI_CCM_BASE + 0x38)
#define CCMU_PLL_MIPI_CTRL_REG            (SUNXI_CCM_BASE + 0x40)
#define CCMU_PLL_PERIPH1_CTRL_REG         (SUNXI_CCM_BASE + 0x44)
#define CCMU_PLL_DE_CTRL_REG              (SUNXI_CCM_BASE + 0x48)


/* cfg list */
#define CCMU_CPUX_AXI_CFG_REG             (SUNXI_CCM_BASE + 0x50)
#define CCMU_AHB1_APB1_CFG_REG            (SUNXI_CCM_BASE + 0x54)
#define CCMU_APB2_CFG_GREG                (SUNXI_CCM_BASE + 0x58)
#define CCMU_AHB2_CFG_GREG                (SUNXI_CCM_BASE + 0x5C)

/* gate list */
#define CCMU_BUS_CLK_GATING_REG0          (SUNXI_CCM_BASE + 0x60)
#define CCMU_BUS_CLK_GATING_REG1          (SUNXI_CCM_BASE + 0x64)
#define CCMU_BUS_CLK_GATING_REG2          (SUNXI_CCM_BASE + 0x68)
#define CCMU_BUS_CLK_GATING_REG3          (SUNXI_CCM_BASE + 0x6C)
#define CCMU_BUS_CLK_GATING_REG4          (SUNXI_CCM_BASE + 0x70)

/* module list */
#define CCMU_NAND0_CLK_REG                (SUNXI_CCM_BASE + 0x80)
#define CCMU_SDMMC0_CLK_REG               (SUNXI_CCM_BASE + 0x88)
#define CCMU_SDMMC1_CLK_REG               (SUNXI_CCM_BASE + 0x8C)
#define CCMU_SDMMC2_CLK_REG               (SUNXI_CCM_BASE + 0x90)

#define CCMU_CE_CLK_REG                   (SUNXI_CCM_BASE + 0x9C)

#define CCMU_USBPHY_CLK_REG               (SUNXI_CCM_BASE + 0xCC)
#define CCMU_DRAM_CLK_REG                 (SUNXI_CCM_BASE + 0xF4)
#define CCMU_PLL_DDR_CFG_REG              (SUNXI_CCM_BASE + 0xF8)
#define CCMU_MBUS_RST_REG                 (SUNXI_CCM_BASE + 0xFC)
#define CCMU_DRAM_CLK_GATING_REG          (SUNXI_CCM_BASE + 0x100)

#define CCMU_AVS_CLK_REG                  (SUNXI_CCM_BASE + 0x144)
#define CCMU_MBUS_CLK_REG                 (SUNXI_CCM_BASE + 0x15C)

/*gate rst list*/
#define CCMU_BUS_SOFT_RST_REG0            (SUNXI_CCM_BASE + 0x2C0)
#define CCMU_BUS_SOFT_RST_REG1            (SUNXI_CCM_BASE + 0x2C4)
#define CCMU_BUS_SOFT_RST_REG2            (SUNXI_CCM_BASE + 0x2C8)
#define CCMU_BUS_SOFT_RST_REG3            (SUNXI_CCM_BASE + 0x2D0)
#define CCMU_BUS_SOFT_RST_REG4            (SUNXI_CCM_BASE + 0x2D8)

/*CE*/
#define CE_CLK_SRC_MASK                     (0x3)
#define CE_CLK_SRC_SEL_BIT                  (24)
#define CE_CLK_SRC                          (0x01)

#define CE_CLK_DIV_RATION_N_BIT             (16)
#define CE_CLK_DIV_RATION_N_MASK            (0x3)
#define CE_CLK_DIV_RATION_N                 (0)

#define CE_CLK_DIV_RATION_M_BIT             (0)
#define CE_CLK_DIV_RATION_M_MASK            (0xF)
#define CE_CLK_DIV_RATION_M                 (3)

#define CE_SCLK_ONOFF_BIT                   (31)
#define CE_SCLK_ON                          (1)

#define CE_GATING_BASE                      CCMU_BUS_CLK_GATING_REG0
#define CE_GATING_PASS                      (1)
#define CE_GATING_BIT                       (5)

#define CE_RST_REG_BASE                     CCMU_BUS_SOFT_RST_REG0
#define CE_RST_BIT                          (5)
#define CE_DEASSERT                         (1)

/*DMA*/
#define DMA_GATING_BASE                     CCMU_BUS_CLK_GATING_REG0
#define DMA_GATING_PASS                     (1)
#define DMA_GATING_BIT                      (6)

#endif
