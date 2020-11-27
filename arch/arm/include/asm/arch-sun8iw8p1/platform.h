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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __PLATFORM_H
#define __PLATFORM_H

#define SUNXI_SRAM_A1_BASE          0X00000000
#define SUNXI_SRAM_A1_SIZE          (16 * 1024) /* 16k */

#define SUNXI_SRAM_D_BASE           0X01C00000
#define SUNXI_SRAMC_BASE            0X01C00000
#define SUNXI_DMA_BASE              0X01C02000
#define SUNXI_NFC0_BASE             0X01C03000
#define SUNXI_TS_BASE               0X01C04000
#define SUNXI_NFC1_BASE             0X01C05000

#define SUNXI_LCD0_BASE             0X01C0C000
#define SUNXI_LCD1_BASE             0X01C0D000
#define SUNXI_VE_BASE               0X01C0E000
#define SUNXI_SMHC0_BASE            (0x01c0f000)
#define SUNXI_SMHC1_BASE            (0x01c10000)
#define SUNXI_SMHC2_BASE            (0x01c11000)
#define SUNXI_SMHC3_BASE            (0x01c12000)

#define SUNXI_SS_BASE               0X01C15000
#define SUNXI_HDMI_BASE             0X01C16000
#define SUNXI_MSGBOX_BASE           0X01C17000
#define SUNXI_SPINLOCK_BASE         0X01C18000
#define SUNXI_USBOTG_BASE           0X01C19000
#define SUNXI_USBEHCI0_BASE         0X01C1A000
#define SUNXI_USBEHCI1_BASE         0X01C1B000
#define SUNXI_USBEOCI2_BASE         0X01C1C000

#define SUNXI_CCM_BASE              0X01C20000

#define SUNXI_R_PIO_BASE            0X01F02C00
#define SUNXI_PIO_BASE              0X01C20800
#define SUNXI_TIMER_BASE            0X01C20C00
#define SUNXI_SPDIF_BASE            0X01C21000
#define SUNXI_PWM_BASE              0X01C21400
#define SUNXI_KEYADC_BASE           0x01c22800 

#define SUNXI_DAUDIO0_BASE          0X01C22000
#define SUNXI_DAUDIO1_BASE          0X01C22400

#define SUNXI_LRADC_BASE            0X01C22800
#define SUNXI_CODEC_BASE            0X01C22C00


#define SUNXI_TP_BASE               0X01C25000
#define SUNXI_DMIC_BASE             0X01C25400

#define SUNXI_UART0_BASE            0X01C28000
#define SUNXI_UART1_BASE            0X01C28400
#define SUNXI_UART2_BASE            0X01C28800
#define SUNXI_UART3_BASE            0X01C28C00
#define SUNXI_UART4_BASE            0X01C29000
#define SUNXI_UART5_BASE            0X01C29400
#define ARMA9_SCU_BASE              0x01c80000
#define ARMA9_GIC_BASE              0x01c81000
#define ARMA9_CPUIF_BASE            0x01c82000

#define SUNXI_TWI0_BASE             0X01C2AC00
#define SUNXI_TWI1_BASE             0X01C2B000
#define SUNXI_TWI2_BASE             0X01C2B400
#define SUNXI_TWI3_BASE             0X01C2B800

#define SUNXI_GMAC_BASE             0X01C30000
#define SUNXI_GPU_BASE              0X01C40000

#define SUNXI_DRAMCOM_BASE          0X01C62000
#define SUNXI_DRAMCTL0_BASE         0X01C63000
#define SUNXI_DRAMCTL1_BASE         0X01C64000
#define SUNXI_DRAMPHY0_BASE         0X01C65000
#define SUNXI_DRAMPHY1_BASE         0X01C66000


#define SUNXI_DE_FE0_BASE           0X01E00000
#define SUNXI_DE_FE1_BASE           0X01E20000
#define SUNXI_DE_BE0_BASE           0X01E60000
#define SUNXI_DE_BE1_BASE           0X01E40000
#define SUNXI_DE_DRC0_BASE          0x01e70000
#define SUNXI_DE_DRC1_BASE          0x01e50000
#define SUNXI_DE_DEU0_BASE          0x01eb0000
#define SUNXI_DE_DEU1_BASE          0x01ea0000
#define SUNXI_MIPI_DSI0_BASE        0x01ca0000
#define SUNXI_MIPI_DSI0_DPHY_BASE   0x01ca1000

#define SUNXI_MP_BASE               0X01E80000

#define SUNXI_RTC_BASE              0X01C20400
#define RTC_GENERAL_PURPOSE_REG(n)  (SUNXI_RTC_BASE + 0x100 + (n) * 0x4)


#define SUNXI_RPRCM_BASE            0x01f01400


#define SUNXI_BROM_BASE             0XFFFF0000      /* 32K */

#define SUNXI_CPU_CFG              (SUNXI_TIMER_BASE + 0x13c)

#ifndef __ASSEMBLY__
/* boot type */
typedef enum {
    SUNXI_BOOT_TYPE_NULL = -1,
    SUNXI_BOOT_TYPE_NAND = 0,
    SUNXI_BOOT_TYPE_MMC0 = 1,
    SUNXI_BOOT_TYPE_MMC2 = 2,
    SUNXI_BOOT_TYPE_SPI  = 3
} sunxi_boot_type_t;

sunxi_boot_type_t get_boot_type(void);
#endif /* __ASSEMBLY__ */

#define SUNXI_GET_BITS(value, start_bit, bits_num) ( (value >> start_bit) & \
                                                    ((1 << bits_num) - 1) )


#define SRAM_A1_CPU12_BASE  (0x0)
#define SRAM_A1_CPU0_BASE   (0x8000)
#define SRAM_A1_SIZE        (0x8000)
#define SRAM_A2_CPU12_BASE  (0x40000)
#define SRAM_A2_CPU0_BASE   (0x0)
#define SRAM_A2_SIZE        (0x8000)
#define SRAM_C_BASE         (0x01D00000)

/* base address of modules */
#define SRAMC_BASE          (0x01c00000)

#define DMA_BASE            (0x01c02000)
#define NFC_BASE            (0x01c03000)
#define TSC_BASE            (0x01c04000)

//#define CSI0_BASE         (0x01c09000)
//#define CSI1_BASE         (0x01c0a000)

#define LCD0_BASE           (0x01c0c000)
#define LCD1_BASE           (0x01c0d000)
#define VE_BASE             (0x01c0e000)
#define MMC0_BASE           (0x01c0f000)
#define MMC1_BASE           (0x01c10000)
#define MMC2_BASE           (0x01c11000)
#define MMC3_BASE           (0x01c12000)

#define SS_BASE             (0x01c15000)
//#define HDMI_BASE         (0x01c16000)
#define MSGBOX_BASE         (0x01c17000)
#define SPINLOCK_BASE       (0x01c18000)
#define OTG_BASE            (0x01c19000)
#define EHCI0_BASE          (0x01c1a000)
#define OHCI0_BASE          (0x01c1a000)
#define EHCI1_BASE          (0x01c1b000)
#define OHCI1_BASE          (0x01c1b000)
#define OHCI2_BASE          (0x01c1c000)

#define TZASC_BASE          (0x01c1e000)

#define CCM_BASE            (0x01c20000)

#define PIO_BASE            (0x01c20800)
#define TIMER_BASE          (0x01c20c00)
#define SPDIF_BASE          (0x01c21000)
#define PWM03_BASE          (0x01c21400)

#define I2S0_BASE           (0x01c22000)
#define I2S1_BASE           (0x01c22400)
#define LRADC_BASE          (0x01c22800)
#define ADDA_BASE           (0x01c22c00)
#define KP_BASE             (0x01c23000)
#define TZPC_BASE           (0x01c23400)
#define SID_BASE            (0x01c23800)
#define SJTAG_BASE          (0x01c23c00)

#define GPADC_BASE          (0x01c25000)

#define UART0_BASE          (0x01c28000)
#define UART1_BASE          (0x01c28400)
#define UART2_BASE          (0x01c28800)
#define UART3_BASE          (0x01c28c00)
#define UART4_BASE          (0x01c29000)

//#define STWI_BASE         (0x01c2a800)
#define TWI0_BASE           (0x01c2ac00)
#define TWI1_BASE           (0x01c2b000)
#define TWI2_BASE           (0x01c2b400)
#define TWI3_BASE           (0x01c2b800)

#define GMAC_BASE           (0x01c30000)
#define GPU_BASE            (0x01c40000)

#define HSTMR_BASE          (0x01c60000)

#define DRAMCOM_BASE        (0x01c62000)
#define DRAMCTL0_BASE       (0x01c63000)
#define DRAMCTL1_BASE       (0x01c64000)
#define DRAMPHY0_BASE       (0x01c65000)
#define DRAMPHY1_BASE       (0x01c66000)

#define SPI0_BASE           (0x01c68000)
#define SPI1_BASE           (0x01c69000)
#define SPI2_BASE           (0x01c6a000)
#define SPI3_BASE           (0x01c6b000)

#define MIPI_DSI0_BASE      (0x01ca0000)
#define MIPI_DSI0PHY_BASE   (0x01ca1000)

#define CSI0_BASE           (0x01cb0000)
#define MIPI_CSI0_BASE      (0x01cb1000)
#define MIPI_CSI0PHY_BASE   (0x01cb2000)
#define CSI1_BASE           (0x01cb3000)

#define ISP_BASE            (0x01cb8000)
#define ISPMEM_BASE         (0x01cc0000)

#define DE_BASE             (0x01000000)
#define DE_SIZE             (0x00400000)

#define RTC_BASE            (0x01f00000)
#define R_BREATH_BASE       (0x01f00400)
#define R_TMR01_BASE        (0x01f00800)
#define R_INTC_BASE         (0x01f00C00)
#define R_WDOG_BASE         (0x01f01000)
#define R_PRCM_BASE         (0x01f01400)
#define R_CPUCFG_BASE       (0x01f01C00)
#define R_CIR_BASE          (0x01f02000)
#define R_TWI_BASE          (0x01f02400)
#define R_UART_BASE         (0x01f02800)
#define R_PIO_BASE          (0x01f02c00)
#define R_1WIRE_BASE        (0x01f03000)
#define R_RSB_BASE          (0x01f03400)
#define R_PWM_BASE          (0x01f03800)

#endif
