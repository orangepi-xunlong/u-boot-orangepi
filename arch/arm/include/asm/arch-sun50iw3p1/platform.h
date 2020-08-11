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

#define SUNXI_SRAM_A1_BASE               (0x00020000L)
#define SUNXI_SRAM_A2_BASE               (0x00100000L)
#define SUNXI_SRAM_C_BASE                (0x00028000L)

#define SUNXI_CE_BASE                    (0x01904000L)
#define SUNXI_SS_BASE                    SUNXI_CE_BASE

//CPUX
#define SUNXI_CPUXCFG_BASE               (0x09010000L)

//sys ctrl
#define SUNXI_SYSCRL_BASE                (0x03000000L)
#define SUNXI_CCM_BASE                   (0x03001000L)
#define SUNXI_DMA_BASE                   (0x03002000L)
#define SUNXI_MSGBOX_BASE                (0x03003000L)
#define SUNXI_SPINLOCK_BASE              (0x03004000L)
#define SUNXI_HSTMR_BASE                 (0x03005000L)
#define SUNXI_SID_BASE                   (0x03006000L)
#define SUNXI_SMC_BASE                   (0x03007000L)
#define SUNXI_SPC_BASE                   (0x03008000L)

#define SUNXI_TIMER_BASE                 (0x03009000L)
#define SUNXI_CNT64_BASE                 (0x03009C00L)
#define SUNXI_PWM_BASE                   (0x0300A000L)
#define SUNXI_PIO_BASE                   (0x0300B000L)
#define SUNXI_PSI_BASE                   (0x0300C000L)
#define SUNXI_IOMMU_BASE                 (0x0300D000L)
#define SUNXI_DCU_BASE                   (0x03010000L)
#define SUNXI_GIC_BASE                   (0x03020000L)

//storage
#define SUNXI_DRAMCTL0_BASE              (0x04002000L)
#define SUNXI_NFC_BASE                   (0x04011000L)
#define SUNXI_SMHC0_BASE                 (0x04020000L)
#define SUNXI_SMHC1_BASE                 (0x04021000L)
#define SUNXI_SMHC2_BASE                 (0x04022000L)

//noraml
#define SUNXI_UART0_BASE                  (0x05000000L)
#define SUNXI_UART1_BASE                  (0x05000400L)
#define SUNXI_UART2_BASE                  (0x05000800L)
#define SUNXI_UART3_BASE                  (0x05000c00L)
#define SUNXI_UART4_BASE                  (0x05001000L)

#define SUNXI_TWI0_BASE                   (0x05002000L)
#define SUNXI_TWI1_BASE                   (0x05002400L)
#define SUNXI_TWI2_BASE                   (0x05002800L)

#define SUNXI_SCR0_BASE                   (0x05005000L)

#define SUNXI_SPI0_BASE                   (0x05010000L)
#define SUNXI_SPI1_BASE                   (0x05011000L)
#define SUNXI_GMAC_BASE                   (0x05020000L)

#define SUNXI_LRADC_BASE                  (0x05070000L)
#define SUNXI_GPADC_BASE                  (0x05070800L)
#define SUNXI_KEYADC_BASE                 SUNXI_LRADC_BASE

#define SUNXI_USBOTG_BASE                 (0x05100000L)
#define SUNXI_EHCI0_BASE                  (0x05310000L)
#define SUNXI_EHCI1_BASE                  (0x05311000L)

#define ARMV7_GIC_BASE                    (SUNXI_GIC_BASE+0x1000L)
#define ARMV7_CPUIF_BASE                  (SUNXI_GIC_BASE+0x2000L)

//cpus
#define SUNXI_RTC_BASE                      (0x07000000L)
#define SUNXI_CPUS_CFG_BASE                 (0x07000400L)
#define SUNXI_RCPUCFG_BASE                  (SUNXI_CPUS_CFG_BASE)
#define SUNXI_RPRCM_BASE                    (0x07010000L)
#define SUNXI_RPWM_BASE                     (0x07020c00L)
#define SUNXI_RPIO_BASE                     (0x07022000L)
#define SUNXI_RTWI_BASE                     (0x07081400L)
#define SUNXI_RRSB_BASE                     (0x07083000L)

#define RES_CAL_CTRL_REG					(SUNXI_RPRCM_BASE + 0X310)

#define RVBARADDR0_L             (SUNXI_CPUXCFG_BASE+0x40)
#define RVBARADDR0_H             (SUNXI_CPUXCFG_BASE+0x44)

#define SRAM_CONTRL_REG0         (SUNXI_SYSCRL_BASE+0x0)
#define SRAM_CONTRL_REG1         (SUNXI_SYSCRL_BASE+0x4)

#define GPIO_BIAS_MAX_LEN (32)
#define GPIO_BIAS_MAIN_NAME "gpio_bias"
#define GPIO_POW_MODE_REG (0x0340)
#define GPIO_3_3V_MODE 0
#define GPIO_1_8V_MODE 1

#define SCP_DRAM_PARA_OFFSET                 (sizeof(u32) * 2)
#define SCP_DARM_PARA_NUM	             (32)

#endif
