/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */


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
#include <linux/types.h>
#include "platform.h"

#define CCM_PLL1_CPUX_CTRL  	(CCM_BASE+0x000)
#define CCM_PLL2_AUDIO_CTRL 	(CCM_BASE+0x008)
#define CCM_PLL3_VIDEO_CTRL    	(CCM_BASE+0x010)
#define CCM_PLL4_VE_CTRL     	(CCM_BASE+0x018)
#define CCM_PLL5_DDR_CTRL  		(CCM_BASE+0x020)
#define CCM_PLL6_MOD_CTRL  		(CCM_BASE+0x028)
#define CCM_PLL7_VIDEO1_CTRL	(CCM_BASE+0x030)
#define CCM_PLL8_GPU_CTRL  		(CCM_BASE+0x038)
#define CCM_MIPI_PLL_CTRL		(CCM_BASE+0x040)
#define CCM_HSIC_PLL_CTRL		(CCM_BASE+0x040)
#define CCM_DE_PLL_CTRL		    (CCM_BASE+0x048)
#define CCM_PLL_DDR1_CTRL       (CCM_BASE+0x04C)//--new





#define CCM_CPU_L2_AXI_CTRL		(CCM_BASE+0x050)
#define CCM_AHB1_APB1_CTRL		(CCM_BASE+0x054)
#define CCM_APB2_CLK_CTRL		(CCM_BASE+0x058)
#define CCM_AXI_GATE_CTRL		(CCM_BASE+0x05c)
#define CCM_AHB1_GATE0_CTRL		(CCM_BASE+0x060)
#define CCMU_BUS_CLK_GATING_REG0            (CCM_BASE + 0x60)
#define CCMU_BUS_CLK_GATING_REG1            (CCM_BASE + 0x64)
#define CCMU_BUS_CLK_GATING_REG2            (CCM_BASE + 0x68)
#define CCMU_BUS_CLK_GATING_REG3            (CCM_BASE + 0x6C)
#define CCM_APB1_GATE0_CTRL     CCMU_BUS_CLK_GATING_REG2
#define CCM_APB2_GATE0_CTRL		CCMU_BUS_CLK_GATING_REG3



#define CCM_NAND0_SCLK_CTRL		(CCM_BASE+0x080)
#define CCM_NAND1_SCLK_CTRL		(CCM_BASE+0x084)

//sd/mmc clk reg  
#define CCMU_SDMMC0_CLK_REG                  (CCM_BASE+0x088)
#define CCMU_SDMMC1_CLK_REG                  (CCM_BASE+0x08c)
#define CCMU_SDMMC2_CLK_REG                  (CCM_BASE+0x090)
#define CCMU_SDMMC3_CLK_REG                  (CCM_BASE+0x094)
#define CCM_SDC0_SCLK_CTRL                   CCMU_SDMMC0_CLK_REG
#define CCM_SDC2_SCLK_CTRL                   CCMU_SDMMC2_CLK_REG


#define CCM_TS_SCLK_CTRL		(CCM_BASE+0x098)
#define CCM_SS_SCLK_CTRL		(CCM_BASE+0x09c)
#define CCM_SPI0_SCLK_CTRL		(CCM_BASE+0x0a0)
#define CCM_SPI1_SCLK_CTRL		(CCM_BASE+0x0a4)
#define CCM_SPI2_SCLK_CTRL		(CCM_BASE+0x0a8)
#define CCM_SPI3_SCLK_CTRL		(CCM_BASE+0x0ac)
#define CCM_I2S0_SCLK_CTRL		(CCM_BASE+0x0b0)
#define CCM_I2S1_SCLK_CTRL		(CCM_BASE+0x0b4)

#define CCM_SPDIF_SCLK_CTRL		(CCM_BASE+0x0c0)

#define CCM_USBPHY_SCLK_CTRL	(CCM_BASE+0x0cc)

#define CCM_MDFS_CLK_CTRL		(CCM_BASE+0x0f0)
#define CCM_DRAMCLK_CFG_CTRL	(CCM_BASE+0x0f4)
#define CCM_DDR_CFG_CTRL	    (CCM_BASE+0x0f8) //--new
#define CCM_MBUS_RESET_CTRL     (CCM_BASE+0x0fC ) //--new

#define CCM_AHB1_RESET_CTRL     (CCM_BASE + 0x02c0)
#define CCMU_BUS_SOFT_RST_REG0              (CCM_BASE + 0x2C0)
#define CCMU_BUS_SOFT_RST_REG1              (CCM_BASE + 0x2C4)
#define CCMU_BUS_SOFT_RST_REG2              (CCM_BASE + 0x2C8)
#define CCMU_BUS_SOFT_RST_REG3              (CCM_BASE + 0x2D0)
#define CCMU_BUS_SOFT_RST_REG4              (CCM_BASE + 0x2D8)

#define CCM_DRAMCLK_GATE_CTRL	(CCM_BASE+0x0100)
#define CCM_BE0_SCLK_CTRL		(CCM_BASE+0x0104)
#define CCM_BE1_SCLK_CTRL		(CCM_BASE+0x0108)
#define CCM_FE0_SCLK_CTRL		(CCM_BASE+0x010c)
#define CCM_FE1_SCLK_CTRL		(CCM_BASE+0x0110)
#define CCM_MP_SCLK_CTRL		(CCM_BASE+0x0114)
#define CCM_LCD0C0_SCLK_CTRL	(CCM_BASE+0x0118)
#define CCM_LCD1C0_SCLK_CTRL	(CCM_BASE+0x011c)

#define CCM_LCD0C1_SCLK_CTRL	(CCM_BASE+0x012c)
#define CCM_LCD1C1_SCLK_CTRL	(CCM_BASE+0x0130)
#define CCM_CSI0_SCLK_CTRL		(CCM_BASE+0x0134)
#define CCM_CSI1_SCLK_CTRL		(CCM_BASE+0x0138)
#define CCM_VE_SCLK_CTRL		(CCM_BASE+0x013c)
#define CCM_CODEC_SCLK_CTRL		(CCM_BASE+0x0140)
#define CCM_AVS_SCLK_CTRL		(CCM_BASE+0x0144)

#define CCM_HDMI_SCLK_CTRL		(CCM_BASE+0x0150)

#define CCM_MBUS_SCLK_CTRL0		(CCM_BASE+0x015c)
#define CCM_MBUS_SCLK_CTRL1		(CCM_BASE+0x0160)

#define CCM_MIPIDSI_SCLK_CTRL	(CCM_BASE+0x0168)
#define CCM_MIPICSI0_SCLK_CTRL	(CCM_BASE+0x016c)

#define CCM_DRC0_SCLK_CTRL		(CCM_BASE+0x0180)
#define CCM_DRC1_SCLK_CTRL		(CCM_BASE+0x0184)
#define CCM_DEU0_SCLK_CTRL		(CCM_BASE+0x0188)
#define CCM_DEU1_SCLK_CTRL		(CCM_BASE+0x018c)

#define CCM_GPU_CORECLK_CTRL	(CCM_BASE+0x01A0)
#define CCM_GPU_MEMCLK_CTRL		(CCM_BASE+0x01A4)
#define CCM_GPU_HYDCLK_CTRL		(CCM_BASE+0x01A8)

#define CCM_PLL_STABLE_REG		(CCM_BASE+0x0200)
#define CCM_MCLK_STABLE_REG		(CCM_BASE+0x0204)

#define CCM_PPL1_BIAS_REG		(CCM_BASE+0x0220)
#define CCM_PPL2_BIAS_REG		(CCM_BASE+0x0224)
#define CCM_PPL3_BIAS_REG		(CCM_BASE+0x0228)
#define CCM_PPL4_BIAS_REG		(CCM_BASE+0x022C)
#define CCM_PPL5_BIAS_REG		(CCM_BASE+0x0230)
#define CCM_PPL6_BIAS_REG		(CCM_BASE+0x0234)
#define CCM_PPL7_BIAS_REG		(CCM_BASE+0x0238)
#define CCM_PPL8_BIAS_REG		(CCM_BASE+0x023C)
#define CCM_MIPIPLL_BIAS_REG	(CCM_BASE+0x0240)
#define CCM_DDR1_BIAS_REG		(CCM_BASE+0x024C)


#define CCM_PPL1_TUNE_REG		(CCM_BASE+0x0250)

#define CCM_PPL5_TUNE_REG		(CCM_BASE+0x0260)

#define CCM_MIPIPLL_TUNE_REG	(CCM_BASE+0x0270)

#define CCM_PPL1_PAT_REG		(CCM_BASE+0x0280)
#define CCM_PPL2_PAT_REG		(CCM_BASE+0x0284)
#define CCM_PPL3_PAT_REG		(CCM_BASE+0x0288)
#define CCM_PPL4_PAT_REG		(CCM_BASE+0x028C)
#define CCM_PPL5_PAT_REG		(CCM_BASE+0x0290)

#define CCM_PPL7_PAT_REG		(CCM_BASE+0x0298)
#define CCM_PPL8_PAT_REG		(CCM_BASE+0x029C)

#define CCM_MIPIPLL_PAT_REG		(CCM_BASE+0x02A0)
#define CCM_DDR1_PAT_REG0		(CCM_BASE+0x02AC)
#define CCM_DDR1_PAT_REG1		(CCM_BASE+0x02B0)



#define CCM_AHB1_RST_REG0		(CCM_BASE+0x02C0)
#define CCM_AHB1_RST_REG1		(CCM_BASE+0x02C4)
#define CCM_AHB1_RST_REG2		(CCM_BASE+0x02C8)

#define CCM_APB1_RST_REG		(CCM_BASE+0x02D0)
#define CCM_APB2_RST_REG		(CCM_BASE+0x02D8)

#define CCM_SECURITY_REG        (CCM_BASE+0x02F0)

#define CCM_CLK_OUTA_REG		(CCM_BASE+0x0300)
#define CCM_CLK_OUTB_REG		(CCM_BASE+0x0304)
#define CCM_CLK_OUTC_REG		(CCM_BASE+0x0308)

/* cmmu pll ctrl bit field */
#define CCM_PLL_STABLE_FLAG		(1U << 28)



/*DMA*/
#define DMA_GATING_BASE                     CCMU_BUS_CLK_GATING_REG0
#define DMA_GATING_PASS                     (1)
#define DMA_GATING_BIT                      (6)


#endif
