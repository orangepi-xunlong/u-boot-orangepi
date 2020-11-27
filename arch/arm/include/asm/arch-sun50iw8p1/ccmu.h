/*
 * (C) Copyright 2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * zhouhuacai <zhouhuacai@allwinnertech.com>
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

/*PLL list*/
#define CCMU_PLL_CPUX_CTRL_REG             (SUNXI_CCM_BASE + 0x00)
#define CCMU_PLL_DDR0_CTRL_REG             (SUNXI_CCM_BASE + 0x10)
#define CCMU_PLL_DDR1_CTRL_REG             (SUNXI_CCM_BASE + 0x18)
#define CCMU_PLL_PERI0_CTRL_REG            (SUNXI_CCM_BASE + 0x20)
#define CCMU_PLL_PERI1_CTRL_REG            (SUNXI_CCM_BASE + 0x28)
#define CCMU_PLL_GPU_CTRL_REG              (SUNXI_CCM_BASE + 0x30)
#define CCMU_PLL_VIDEO0_CTRL_REG           (SUNXI_CCM_BASE + 0x40)
#define CCMU_PLL_VIDEO1_CTRL_REG           (SUNXI_CCM_BASE + 0x48)
#define CCMU_PLL_VIDEO2_CTRL_REG           (SUNXI_CCM_BASE + 0x50)
#define CCMU_PLL_VE_CTRL_REG               (SUNXI_CCM_BASE + 0x58)
#define CCMU_PLL_DE_CTRL_REG               (SUNXI_CCM_BASE + 0x60)
#define CCMU_PLL_ISP_CTRL_REG              (SUNXI_CCM_BASE + 0x68)
#define CCMU_PLL_HSIC_CTRL_REG             (SUNXI_CCM_BASE + 0x70)
#define CCMU_PLL_AUDIO_CTRL_REG            (SUNXI_CCM_BASE + 0x78)
#define CCMU_PLL_24M_CTRL_REG              (SUNXI_CCM_BASE + 0xB8)

/* cfg list */
#define CCMU_CPUX_AXI_CFG_REG              (SUNXI_CCM_BASE + 0x500)
#define CCMU_PSI_AHB1_AHB2_CFG_REG         (SUNXI_CCM_BASE + 0x510)
#define CCMU_AHB3_CFG_GREG                 (SUNXI_CCM_BASE + 0x51C)
#define CCMU_APB1_CFG_GREG                 (SUNXI_CCM_BASE + 0x520)
#define CCMU_APB2_CFG_GREG                 (SUNXI_CCM_BASE + 0x524)
#define CCMU_MBUS_CFG_REG                  (SUNXI_CCM_BASE + 0x540)

#define CCMU_CE_CLK_REG                    (SUNXI_CCM_BASE + 0x680)
#define CCMU_CE_BGR_REG                    (SUNXI_CCM_BASE + 0x68C)

#define CCMU_VE_CLK_REG                    (SUNXI_CCM_BASE + 0x690)
#define CCMU_VE_BGR_REG                    (SUNXI_CCM_BASE + 0x69C)

/*SYS*/
#define CCMU_DMA_BGR_REG                   (SUNXI_CCM_BASE + 0x70C)
#define CCMU_AVS_CLK_REG                   (SUNXI_CCM_BASE + 0x740)

/* storage */
#define CCMU_DRAM_CLK_REG                  (SUNXI_CCM_BASE + 0x800)
#define CCMU_MBUS_MST_CLK_GATING_REG       (SUNXI_CCM_BASE + 0x804)
#define CCMU_DRAM_BGR_REG                  (SUNXI_CCM_BASE + 0x80C)

#define CCMU_NAND_CLK_REG                  (SUNXI_CCM_BASE + 0x810)
#define CCMU_NAND_BGR_REG                  (SUNXI_CCM_BASE + 0x82C)

#define CCMU_SDMMC0_CLK_REG                (SUNXI_CCM_BASE + 0x830)
#define CCMU_SDMMC1_CLK_REG                (SUNXI_CCM_BASE + 0x834)
#define CCMU_SDMMC2_CLK_REG                (SUNXI_CCM_BASE + 0x838)
#define CCMU_SMHC_BGR_REG                  (SUNXI_CCM_BASE + 0x84c)

/*normal interface*/
#define CCMU_UART_BGR_REG                  (SUNXI_CCM_BASE + 0x90C)
#define CCMU_TWI_BGR_REG                   (SUNXI_CCM_BASE + 0x91C)

#define CCMU_SCR_BGR_REG                   (SUNXI_CCM_BASE + 0x93C)

#define CCMU_SPI0_CLK_REG                  (SUNXI_CCM_BASE + 0x940)
#define CCMU_SPI1_CLK_REG                  (SUNXI_CCM_BASE + 0x944)
#define CCMU_SPI_BGR_CLK_REG               (SUNXI_CCM_BASE + 0x96C)
#define CCMU_GPADC_BGR_REG                 (SUNXI_CCM_BASE + 0x9EC)
/*USB0*/
#define CCMU_USB0_CLK_REG                  (SUNXI_CCM_BASE + 0xA70)
#define CCMU_USB_BGR_REG                   (SUNXI_CCM_BASE + 0xA8C)

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


/*for other file ,use before define*/
#define CCM_AVS_SCLK_CTRL                   (CCMU_AVS_CLK_REG)
#define CCM_AHB1_GATE0_CTRL			        (CCMU_BUS_CLK_GATING_REG0)
#define CCM_AHB1_RST_REG0                   (CCMU_BUS_SOFT_RST_REG0)

#endif

