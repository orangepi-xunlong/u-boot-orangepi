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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * SPDX-License-Identifier:	GPL-2.0
 */

#ifndef __PLATFORM_H
#define __PLATFORM_H

#define SUNXI_SRAM_D_BASE                         (0x00010000L)
/* base address of modules */
#define SUNXI_DE_BASE                             (0x01000000L)
#define SUNXI_DE_INTERLACED_BASE                  (0x01400000L)
#define SUNXI_CPU_MBIST_BASE                      (0x01502000L)
#define SUNXI_CPUX_CFG_BASE                       (0x01700000L)


#define SUNXI_SYSCRL_BASE                       (0x01c00000L)

#define SUNXI_DMA_BASE                           (0x01c02000L)
#define SUNXI_NFC_BASE                           (0x01c03000L)
#define SUNXI_TSC_BASE                           (0x01c04000L)
#define SUNXI_SPI0_BASE                          (0x01c05000L)
#define SUNXI_SPI1_BASE                          (0x01c06000L)
#define SUNXI_CSI0_BASE                          (0x01c09000L)
#define SUNXI_EMAC_BASE                          (0x01c0B000L)

#define SUNXI_VE_BASE                                (0x01c0e000L)

#define SUNXI_SMHC0_BASE                             (0x01c0f000L)
#define SUNXI_SMHC1_BASE                             (0x01c10000L)
#define SUNXI_SMHC2_BASE                             (0x01c11000L)
#define SUNXI_SMHC3_BASE                             (0x01c12000L)

#define SUNXI_USBOTG_BASE                            (0x01c13000L)
#define SUNXI_USB0HOST_BASE                          (0x01c14000L)
#define SUNXI_CE_BASE                                (0x01c15000L)
#define SUNXI_SPI2_BASE                              (0x01c17000L)
#define SUNXI_SATA_BASE                              (0x01c18000L)

#define SUNXI_USB1HOST_BASE                          (0x01c19000L)
#define SUNXI_SID_BASE                               (0x01c1b000L)
#define SUNXI_USB2HOST_BASE                          (0x01c1c000L)
#define SUNXI_CSI1_BASE                              (0x01c1d000L)
#define SUNXI_SPI3_BASE                              (0x01c1f000L)

#define SUNXI_CCM_BASE                               (0x01c20000L)
#define SUNXI_RTC_BASE                               (0x01c20400L)
#define SUNXI_PIO_BASE                               (0x01c20800L)
#define SUNXI_TIMER_BASE			     (0x01c20c00L)
#define SUNXI_SPDIF_BASE			     (0x01c21000L)
#define SUNXI_AC97_BASE                              (0x01c21400L)
#define SUNXI_IR0_BASE                               (0x01c21800L)
#define SUNXI_IR1_BASE                               (0x01c21c00L)
#define SUNXI_DAI0_BASE                              (0x01c22000L)
#define SUNXI_DAI1_BASE                              (0x01c22400L)
#define SUNXI_DAI2_BASE                              (0x01c22800L)

#define SUNXI_AC_BASE                                (0x01c22c00L)
#define SUNXI_KEYPAD_BASE                            (0x01c23000L)
#define SUNXI_KEYADC_BASE                            (0x01c24400L)
#define SUNXI_THC_BASE                               (0x01c24c00L)
#define SUNXI_RTP_BASE                               (0x01c25000L)
#define SUNXI_PMU_BASE                               (0x01c25400L)
#define SUNXI_CPUCFG_BASE                            (0x01c25c00L)

#define SUNXI_UART0_BASE                             (0x01c28000L)
#define SUNXI_UART1_BASE                             (0x01c28400L)
#define SUNXI_UART2_BASE                             (0x01c28800L)
#define SUNXI_UART3_BASE                             (0x01c28c00L)
#define SUNXI_UART4_BASE                             (0x01c29000L)

#define SUNXI_TWI0_BASE                          (0x01c2ac00L)
#define SUNXI_TWI1_BASE                          (0x01c2b000L)
#define SUNXI_TWI2_BASE                          (0x01c2b400L)
#define SUNXI_TWI3_BASE                          (0x01c2b800L)
#define SUNXI_SCR_BASE			         (0x01c2c400L)

#define SUNXI_GPU_BASE                           (0x01c40000L)
#define SUNXI_GMAC_BASE                          (0x01c50000L)
#define SUNXI_HSTMR_BASE			 (0x01c60000L)

#define SUNXI_DRAMCOM_BASE                       (0x01c62000L)
#define SUNXI_DRAMCTL0_BASE                      (0x01c63000L)


#define ARMA9_SCU_BASE                           (0x01c80000L)
#define ARMA9_GIC_BASE                           (0x01c81000L)
#define ARMA9_CPUIF_BASE                         (0x01c82000L)

#define SUNXI_MIPI_DSI0_BASE                     (0x01ca0000L)
#define SUNXI_MIPI_DSIPHY_BASE                   (0x01ca1000L)

#define SUNXI_HDMI_BASE                         (0x01ee0000L)
#define HDMI_BASE                                SUNXI_HDMI_BASE
#define RTC_STANDBY_FLAG_REG                     (SUNXI_RTC_BASE + 0x1f8)
#define RTC_STANDBY_SOFT_ENTRY_REG               (SUNXI_RTC_BASE + 0x1fc)
#define DRAM_CRC_REG_ADDR                        (SUNXI_RTC_BASE + 0x10c) /* 0x01c2050C */

#define SUNXI_EHCI0_BASE                         (SUNXI_USB0HOST_BASE)
#define SUNXI_EHCI1_BASE                         (SUNXI_USB1HOST_BASE)
#endif
