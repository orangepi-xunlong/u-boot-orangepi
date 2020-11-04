/*
 * Allwinner Sun50iw3 clock register definitions
 *
 * (C) Copyright 2017  <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _SUNXI_CLOCK_SUN8IW18_H
#define _SUNXI_CLOCK_SUN8IW18_H
#include <asm/arch/cpu.h>
#include <asm/arch/clock_sun50iw3.h>

/*SYS*/
#define CCMU_DMA_BGR_REG                   (SUNXI_CCM_BASE + 0x70C)
#define CCMU_AVS_CLK_REG                   (SUNXI_CCM_BASE + 0x740)

#define USB0_PHY_RESET_BIT			(30)
#define USB0_PHY_CLK_ONOFF_BIT			(29)
#define USB0_PHY_CLK_SEL_BIT			(17)
#define USB0_PHY_CLK_DIV_BIT			(16)

#define USBOTG_RESET_BIT			(24)
#define USBOTG_CLK_ONOFF_BIT			(8)

/*DMA*/
#define DMA_GATING_BASE                   CCMU_DMA_BGR_REG
#define DMA_GATING_PASS                   (1)
#define DMA_GATING_BIT                    (0)

/*CE*/
#define CE_CLK_SRC_MASK                   (0x1)
#define CE_CLK_SRC_SEL_BIT                (24)
#define CE_CLK_SRC                        (0x01)

#define CE_CLK_DIV_RATION_N_BIT           (8)
#define CE_CLK_DIV_RATION_N_MASK          (0x3)
#define CE_CLK_DIV_RATION_N               (0)

#define CE_CLK_DIV_RATION_M_BIT           (0)
#define CE_CLK_DIV_RATION_M_MASK          (0xF)
#define CE_CLK_DIV_RATION_M               (3)

#define CE_SCLK_ONOFF_BIT                 (31)
#define CE_SCLK_ON                        (1)

#define CE_GATING_BASE                    CCMU_CE_BGR_REG
#define CE_GATING_PASS                    (1)
#define CE_GATING_BIT                     (0)

#define CE_RST_REG_BASE                   CCMU_CE_BGR_REG
#define CE_RST_BIT                        (16)
#define CE_DEASSERT                       (1)

#define CE_MBUS_GATING_MASK               (1)
#define CE_MBUS_GATING_BIT                (2)
#define CE_MBUS_GATING                    (1)


/*for other file ,use before define*/
#define CCM_AVS_SCLK_CTRL                   (CCMU_AVS_CLK_REG)
#define CCM_AHB1_GATE0_CTRL			        (CCMU_BUS_CLK_GATING_REG0)
#define CCM_AHB1_RST_REG0                   (CCMU_BUS_SOFT_RST_REG0)

/*for spi*/
#define SPI_RST_OFFSET (16)
#define SPI_GATING_OFFSET (0)

#define SPI0_CKID	0
#define SPI1_CKID	1
#define SPI2_CKID	2
#define SPI3_CKID	3

#endif /* _SUNXI_CLOCK_SUN8IW18_H */
