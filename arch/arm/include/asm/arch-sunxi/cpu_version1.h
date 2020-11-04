/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 */

#ifndef _SUNXI_CPU_VERSION1_H
#define _SUNXI_CPU_VERSION1_H

#define SUNXI_DE2_BASE			0x01000000

#define SUNXI_SYSCRL_BASE		0x01c00000
#define SUNXI_DMA_BASE			0x01c02000
#define SUNXI_NFC_BASE			0x01c03000
#define SUNXI_TS_BASE			0x01c06000
#define SUNXI_SPI0_BASE			0x01c68000
#define SUNXI_SPI1_BASE			0x01c69000
#define SUNXI_MSGBOX_BASE		0x01c17000
#define SUNXI_CSI_BASE			0x01cb0000
#define SUNXI_TVE0_BASE			0x01e00000
#define SUNXI_EMAC_BASE			0x01c30000
#define SUNXI_LCD0_BASE			0x01c0C000
#define SUNXI_LCD1_BASE			0x01c0d000
#define SUNXI_VE_BASE			0x01c0e000
#define SUNXI_MMC0_BASE			0x01c0f000
#define SUNXI_MMC1_BASE			0x01c10000
#define SUNXI_MMC2_BASE			0x01c11000
#define SUNXI_SS_BASE			0x01c15000
#define SUNXI_SPINLOCK_BASE		0x01c18000

#define SUNXI_USBOTG_BASE		(0x01c19000L)
#define SUNXI_EHCI0_BASE		(0x01c1a000L)
#define SUNXI_EHCI1_BASE		(0x01c1b000L)
#define SUNXI_EHCI2_BASE		(0x01c1c000L)
#define SUNXI_EHCI3_BASE		(0x01c1d000L)

#define SUNXI_CCM_BASE			0x01c20000
#define SUNXI_PIO_BASE			0x01c20800
#define SUNXI_TIMER_BASE		0x01c20c00
#define SUNXI_PWM_BASE			0x01c21400

#define SUNXI_AD_DA_BASE		0x01c22c00

/* SID address space starts at 0x01c1400, but e-fuse is at offset 0x200 */
#define SUNXI_SID_BASE			0x01c14000
#define SUNXI_SID_SRAM_BASE		0x01c14200

#define SUNXI_UART0_BASE		0x01c28000
#define SUNXI_UART1_BASE		0x01c28400
#define SUNXI_UART2_BASE		0x01c28800
#define SUNXI_UART3_BASE		0x01c28c00


#define SUNXI_TWI0_BASE			0x01c2ac00
#define SUNXI_TWI1_BASE			0x01c2b000
#define SUNXI_TWI2_BASE			0x01c2b400

#define SUNXI_SCR_BASE			0x01c2c400

#define SUNXI_DRAM_COM_BASE		0x01c62000
#define SUNXI_DRAM_CTL0_BASE	0x01c63000
#define SUNXI_DRAM_PHY0_BASE	0x01c65000
#define SUNXI_GIC400_BASE		0x01c80000

#define SUNXI_TVE0_BASE			0x01e00000

#define SUNXI_HDMI_BASE			0x01ee0000

#define SUNXI_RTC_BASE			0x01f00000
#define SUNXI_PRCM_BASE			0x01f01400
#define SUNXI_CPUCFG_BASE		0x01f01c00
#define SUNXI_R_CIR_RX_BASE		0x01f02000
#define SUNXI_R_TWI_BASE		0x01f02400
#define SUNXI_R_UART_BASE		0x01f02800
#define SUNXI_R_PIO_BASE		0x01f02c00
#define SUNXI_RPIO_BASE			0x01f02c00

/* CoreSight Debug Module */
#define SUNXI_CSDM_BASE			0x3f500000

#define SUNXI_DDRII_DDRIII_BASE		0x40000000	/* 2 GiB */

#define SUNXI_BROM_BASE			0xffff0000	/* 32 kiB */

#define SUNXI_CPU_CFG			(SUNXI_TIMER_BASE + 0x13c)

/* SS bonding ids used for cpu identification */
#define SUNXI_SS_BOND_ID_A31		4
#define SUNXI_SS_BOND_ID_A31S		5

#define SUNXI_RTC_DATA_BASE     (SUNXI_RTC_BASE+0x100)

#ifndef __ASSEMBLY__
void sunxi_board_init(void);
void sunxi_reset(void);
int sunxi_get_ss_bonding_id(void);
int sunxi_get_sid(unsigned int *sid);
#endif /* __ASSEMBLY__ */

#endif /* _SUNXI_CPU_VERSION1_H */
