/*
 * (C) Copyright 2007-2015
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * SPDX-License-Identifier: GPL-2.0
 */

#ifndef __CCMU_H
#define __CCMU_H


#include "platform.h"

/* pll list */
#define CCMU_PLL_CPUX_CTRL_REG            (SUNXI_CCM_BASE + 0x00)
#define CCMU_PLL_AUDIO_CTRL_REG           (SUNXI_CCM_BASE + 0x08)
#define CCMU_PLL_VIDEO0_CTRL_REG          (SUNXI_CCM_BASE + 0x10)
#define CCMU_PLL_VE_CTRL_REG              (SUNXI_CCM_BASE + 0x18)
#define CCMU_PLL_DDR0_CTRL_REG            (SUNXI_CCM_BASE + 0x20)
#define CCMU_PLL_PERIPH_CTRL_REG          (SUNXI_CCM_BASE + 0x28)
#define CCMU_PLL_VIDEO1_CTRL_REG          (SUNXI_CCM_BASE + 0x30)
#define CCMU_PLL_GPU_CTRL_REG             (SUNXI_CCM_BASE + 0x38)
#define CCMU_PLL_MIPI_CTRL_REG            (SUNXI_CCM_BASE + 0x40)

#define CCMU_PLL_DE_CTRL_REG              (SUNXI_CCM_BASE + 0x48)
#define CCMU_PLL_DDR1_CTRL_REG            (SUNXI_CCM_BASE + 0x4C)

//new mode for pll lock
//#define CCMU_PLL_LOCK_CTRL_REG          (SUNXI_CCM_BASE + 0x320)
#define LOCK_EN_PLL_CPUX                  (1<<0)
#define LOCK_EN_PLL_AUDIO                 (1<<1)
#define LOCK_EN_PLL_VIDEO0                (1<<2)
#define LOCK_EN_PLL_VE                    (1<<3)
#define LOCK_EN_PLL_DDR0                  (1<<4)
#define LOCK_EN_PLL_PERIPH0               (1<<5)
#define LOCK_EN_PLL_VIDEO1                (1<<6)
#define LOCK_EN_PLL_GPU                   (1<<7)
#define LOCK_EN_PLL_MIPI                  (1<<8)
#define LOCK_EN_PLL_HSIC                  (1<<9)
#define LOCK_EN_PLL_DE                    (1<<10)
#define LOCK_EN_PLL_DDR1                  (1<<11)
#define LOCK_EN_PLL_PERIPH1               (1<<12)
#define LOCK_EN_NEW_MODE                  (1<<28)



/* cfg list */
#define CCMU_CPUX_CLK_SRC_REG             (SUNXI_CCM_BASE + 0x50)
#define CCMU_AHB1_APB_HCLKC_CFG_REG       (SUNXI_CCM_BASE + 0x54)

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
#define CCMU_SDMMC3_CLK_REG               (SUNXI_CCM_BASE + 0x94)

#define CCMU_CE_CLK_REG                   (SUNXI_CCM_BASE + 0x9C)

#define CCMU_USBPHY_CLK_REG               (SUNXI_CCM_BASE + 0xCC)
#define CCMU_DRAM_CLK_REG                 (SUNXI_CCM_BASE + 0xF4)
#define CCMU_PLL_DDR_CFG_REG              (SUNXI_CCM_BASE + 0xF8)

#define CCMU_DRAM_CLK_GATING_REG          (SUNXI_CCM_BASE + 0x100)

#define CCMU_AVS_CLK_REG                  (SUNXI_CCM_BASE + 0x144)


/*gate rst list*/
#define CCMU_BUS_SOFT_RST_REG0            (SUNXI_CCM_BASE + 0x2C0)
#define CCMU_BUS_SOFT_RST_REG1            (SUNXI_CCM_BASE + 0x2C4)
#define CCMU_BUS_SOFT_RST_REG2            (SUNXI_CCM_BASE + 0x2D0)
#define CCMU_BUS_SOFT_RST_REG3            (SUNXI_CCM_BASE + 0x2D0)
#define CCMU_BUS_SOFT_RST_REG4            (SUNXI_CCM_BASE + 0x2D8)


#define CCM_AHB1_RST_REG0                 (SUNXI_CCM_BASE+0x02C0)
#define CCM_AHB1_RST_REG1                 (SUNXI_CCM_BASE+0x02C4)
#define CCM_AHB1_RST_REG2                 (SUNXI_CCM_BASE+0x02C8)
#define CCM_AXI_GATE_CTRL                 (SUNXI_CCM_BASE+0x05c)
#define CCM_AHB1_GATE0_CTRL               (SUNXI_CCM_BASE+0x060)
#define CCM_PLL6_MOD_CTRL                 (SUNXI_CCM_BASE+0x028)
#define CCMU_SPI0_SCLK_CTRL                (SUNXI_CCM_BASE+0x0a0)



#define CCMU_CLK_OUTA_REG                 (SUNXI_CCM_BASE+0x0300)
#define CCMU_CLK_OUTB_REG                 (SUNXI_CCM_BASE+0x0304)
#define CCMU_CLK_OUTC_REG                 (SUNXI_CCM_BASE+0x0308)

/* cmmu pll ctrl bit field */
#define CCM_PLL_STABLE_FLAG               (1U << 28)

/* clock ID */
#define AXI_BUS                           (0)
#define AHB1_BUS0                         (1)
#define AHB1_BUS1                         (2)
#define AHB1_LVDS                         (3)
#define APB1_BUS0                         (4)
#define APB2_BUS0                         (5)

/* ahb1 branc0 */
#define USBOHCI2_CKID                     ((AHB1_BUS0 << 8) | 31)
#define USBOHCI1_CKID                     ((AHB1_BUS0 << 8) | 30)
#define USBOHCI0_CKID                     ((AHB1_BUS0 << 8) | 29)
#define USBEHCI2_CKID                     ((AHB1_BUS0 << 8) | 28)
#define USBEHCI1_CKID                     ((AHB1_BUS0 << 8) | 27)
#define USBEHCI0_CKID                     ((AHB1_BUS0 << 8) | 26)
#define USB_OTG_CKID                      ((AHB1_BUS0 << 8) | 24)
#define SPI3_CKID                         ((AHB1_BUS0 << 8) | 23)
#define SPI2_CKID                         ((AHB1_BUS0 << 8) | 22)
#define SPI1_CKID                         ((AHB1_BUS0 << 8) | 21)
#define SPI0_CKID                         ((AHB1_BUS0 << 8) | 20)
#define HSTMR_CKID                        ((AHB1_BUS0 << 8) | 19)
#define TS_CKID                           ((AHB1_BUS0 << 8) | 18)
#define GMAC_CKID                         ((AHB1_BUS0 << 8) | 17)
#define DRAMREG_CKID                      ((AHB1_BUS0 << 8) | 14)
#define NAND0_CKID                        ((AHB1_BUS0 << 8) | 13)
#define NAND1_CKID                        ((AHB1_BUS0 << 8) | 12)
#define SDC3_CKID                         ((AHB1_BUS0 << 8) | 11)
#define SDC2_CKID                         ((AHB1_BUS0 << 8) | 10)
#define SDC1_CKID                         ((AHB1_BUS0 << 8) | 9 )
#define SDC0_CKID                         ((AHB1_BUS0 << 8) | 8 )
#define DMA_CKID                          ((AHB1_BUS0 << 8) | 6 )
#define SS_CKID                           ((AHB1_BUS0 << 8) | 5 )
#define MIPIDSI_CKID                      ((AHB1_BUS0 << 8) | 1 )
#define MIPICSI_CKID                      ((AHB1_BUS0 << 8) | 0 )
/* ahb1 branc1 */
#define DRC1_CKID                         ((AHB1_BUS1 << 8) | 26)
#define DRC0_CKID                         ((AHB1_BUS1 << 8) | 25)
#define DEU1_CKID                         ((AHB1_BUS1 << 8) | 24)
#define DEU0_CKID                         ((AHB1_BUS1 << 8) | 23)
#define SPINLOCK_CKID                     ((AHB1_BUS1 << 8) | 22)
#define MSGBOX_CKID                       ((AHB1_BUS1 << 8) | 21)
#define GPU_CKID                          ((AHB1_BUS1 << 8) | 20)
#define MP_CKID                           ((AHB1_BUS1 << 8) | 18)
#define FE1_CKID                          ((AHB1_BUS1 << 8) | 15)
#define FE0_CKID                          ((AHB1_BUS1 << 8) | 14)
#define BE1_CKID                          ((AHB1_BUS1 << 8) | 13)
#define BE0_CKID                          ((AHB1_BUS1 << 8) | 12)
#define HDMI_CKID                         ((AHB1_BUS1 << 8) | 11)
#define CSI1_CKID                         ((AHB1_BUS1 << 8) | 9)
#define CSI0_CKID                         ((AHB1_BUS1 << 8) | 8)
#define LCD1_CKID                         ((AHB1_BUS1 << 8) | 5)
#define LCD0_CKID                         ((AHB1_BUS1 << 8) | 4)
#define VE_CKID                           ((AHB1_BUS1 << 8) | 0)
/* ahb1 special for lvds */
#define LVDS_CKID                         ((AHB1_LVDS << 8) | 0)

/* apb1  */
#define IIS1_CKID                         ((APB1_BUS0 << 8) | 13)
#define IIS0_CKID                         ((APB1_BUS0 << 8) | 12)
#define KP_CKID                           ((APB1_BUS0 << 8) | 10)
#define GPADC_CKID                        ((APB1_BUS0 << 8) | 8)
#define PIO_CKID                          ((APB1_BUS0 << 8) | 5)
#define SPDIF_CKID                        ((APB1_BUS0 << 8) | 1)
#define CODEC_CKID                        ((APB1_BUS0 << 8) | 0)
/* apb2  */
#define UART4_CKID                        ((APB2_BUS0 << 8) | 20)
#define UART3_CKID                        ((APB2_BUS0 << 8) | 19)
#define UART2_CKID                        ((APB2_BUS0 << 8) | 18)
#define UART1_CKID                        ((APB2_BUS0 << 8) | 17)
#define UART0_CKID                        ((APB2_BUS0 << 8) | 16)
#define STWI_CKID                         ((APB2_BUS0 << 8) | 4)
#define TWI3_CKID                         ((APB2_BUS0 << 8) | 3)
#define TWI2_CKID                         ((APB2_BUS0 << 8) | 2)
#define TWI1_CKID                         ((APB2_BUS0 << 8) | 1)
#define TWI0_CKID                         ((APB2_BUS0 << 8) | 0)

#define CPUCLK_SRC_32K                    (0)
#define CPUCLK_SRC_24M                    (1)
#define CPUCLK_SRC_PLL1                   (2)

#define AHB1CLK_SRC_LOSC                  (0)
#define AHB1CLK_SRC_24M                   (1)
#define AHB1CLK_SRC_AXI                   (2)
#define AHB1CLK_SRC_PLL6D                 (3)

#define APB2CLK_SRC_LOSC                  (0)
#define APB2CLK_SRC_24M                   (1)
#define APB2CLK_SRC_PLL6                  (2)

#define MBUSCLK_SRC_24M                   (0)
#define MBUSCLK_SRC_PLL6                  (1)
#define MBUSCLK_SRC_PLL5                  (2)


/*DMA*/
#define DMA_GATING_BASE                   CCMU_BUS_CLK_GATING_REG0
#define DMA_GATING_PASS                   (1)
#define DMA_GATING_BIT                    (6)

#endif

