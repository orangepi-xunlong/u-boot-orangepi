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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * SPDX-License-Identifier: GPL-2.0
 */

#ifndef __PLATFORM_H
#define __PLATFORM_H

/*SRAM*/
#define SUNXI_SRAM_A1_BASE                  (0x00020000L)
#define SUNXI_SRAM_A2_BASE                  (0x00100000L)
#define SUNXI_SRAM_C_BASE                   (0x00028000L)

/*CE*/
#define SUNXI_CE_BASE                       (0x01904000L)
#define SUNXI_SS_BASE                       SUNXI_CE_BASE

//CPUX
#define SUNXI_C0_CPUX_CFG_BASE              (0x09010000L)
#define SUNXI_C1_CPUX_CFG_BASE              (0x09810000L)
#define SUNXI_DE_BASE                       (0x01000000L)

/*sys ctrl*/
#define SUNXI_SYSCRL_BASE                   (0x03000000L)
#define SUNXI_CCM_BASE                      (0x03001000L)
#define SUNXI_DMA_BASE                      (0x03002000L)
#define SUNXI_MSGBOX_BASE                   (0x03003000L)
#define SUNXI_SPINLOCK_BASE                 (0x03004000L)
#define SUNXI_HSTMR_BASE                    (0x03005000L)
#define SUNXI_SID_BASE                      (0x03006000L)
#define SUNXI_SMC_BASE                      (0x03007000L)
#define SUNXI_SPC_BASE                      (0x03008000L)
#define SUNXI_TIMER_BASE                    (0x03009000L)
#define SUNXI_PWM_BASE                      (0x0300A000L)
#define SUNXI_PIO_BASE                      (0x0300B000L)
#define SUNXI_PSI_BASE                      (0x0300C000L)
#define SUNXI_DCU_BASE                      (0x03010000L)
#define SUNXI_GIC_BASE                      (0x03020000L)
#define SUNXI_IOMMU_BASE                    (0x0303D000L)
#define SUNXI_CCI400_BASE                   (0x030D0000L)

/*storage*/
#define SUNXI_DRAMCTL0_BASE                 (0x04002000L)
#define SUNXI_NFC_BASE                      (0x04011000L)
#define SUNXI_SMHC0_BASE                    (0x04020000L)
#define SUNXI_SMHC1_BASE                    (0x04021000L)
#define SUNXI_SMHC2_BASE                    (0x04022000L)


/*normal*/
#define SUNXI_UART0_BASE                    (0x05000000L)
#define SUNXI_UART1_BASE                    (0x05000400L)
#define SUNXI_UART2_BASE                    (0x05000800L)
#define SUNXI_UART3_BASE                    (0x05000c00L)

#define SUNXI_TWI0_BASE                     (0x05002000L)
#define SUNXI_TWI1_BASE                     (0x05002400L)
#define SUNXI_TWI2_BASE                     (0x05002800L)

#define SUNXI_SCR_BASE                      (0x05005000L)

#define SUNXI_SPI0_BASE                     (0x05010000L)
#define SUNXI_SPI1_BASE                     (0x05011000L)
#define SUNXI_GMAC_BASE                     (0x05020000L)

#define SUNXI_LRADC_BASE                    (0x07030800L)
#define SUNXI_GPADC_BASE                    (0x05070000L)
#define SUNXI_KEYADC_BASE                   SUNXI_LRADC_BASE

#define SUNXI_USBOTG_BASE                   (0x05100000L)
#define SUNXI_EHCI0_BASE                    (0x05200000L)
#define SUNXI_EHCI1_BASE                    (0x05311000L)

#define SUNXI_LCD0_BASE                     (0x06511000L)
#define SUNXI_VE_BASE                       (0x01A00000L)

/*for usb SUNXI_SYSCRL_BASE*/
#define SUNXI_SRAM_D_BASE                   (SUNXI_SYSCRL_BASE)
#define ARMV7_GIC_BASE                      (SUNXI_GIC_BASE+0x1000L)
#define ARMV7_CPUIF_BASE                    (SUNXI_GIC_BASE+0x2000L)

//cpus
#define SUNXI_RTC_BASE                      (0x07000000L)
#define SUNXI_RCPUCFG_BASE                  (0x07000400L)
#define SUNXI_RPRCM_BASE                    (0x07010000L)
#define SUNXI_RPWM_BASE                     (0x07020c00L)
#define SUNXI_RPIO_BASE                     (0x07022000L)
#define SUNXI_RTWI_BASE                     (0x07081400L)
#define SUNXI_RRSB_BASE                     (0x07083000L)
#define SUNXI_SUART0_BASE                   (0x07080000L)
#define SUNXI_SUART1_BASE                   (0x07080400L)
#define SUNXI_SUART2_BASE                   (0x07080800L)
#define SUNXI_SUART3_BASE                   (0x07080C00L)
#define SUNXI_SUART4_BASE                   (0x07081000L)

#define SUNXI_RLRADC_BRG_REG				(SUNXI_RPRCM_BASE+0x016c)
#define SUNXI_RLRADC_RST_BIT				(16)
#define SUNXI_RLRADC_GATING_BIT				(0)

#define SUNXI_RTWI_BRG_REG					(SUNXI_RPRCM_BASE+0x019c)
#define SUNXI_RTWI0_RST_BIT					(16)
#define SUNXI_RTWI0_GATING_BIT				(0)

#define SUNXI_RRSB_BRG_REG					(SUNXI_RPRCM_BASE+0x01BC)
#define SUNXI_RRSB_RST_BIT					(16)
#define SUNXI_RRSB_GATING_BIT				(0)

#define RVBARADDR0_L                        (SUNXI_C0_CPUX_CFG_BASE+0x40)
#define RVBARADDR0_H                        (SUNXI_C0_CPUX_CFG_BASE+0x44)

#define TIMESTAMP
#define TIMESTAMP_CTRL_BASE                 (0x08120000)
/*dram_para_offset is the numbers of u32 before dram data sturcture(dram_para) in struct arisc_para*/
#define SCP_DRAM_PARA_OFFSET                 (sizeof(u32) * 2)
#define SCP_DARM_PARA_NUM	             (32)

#endif
