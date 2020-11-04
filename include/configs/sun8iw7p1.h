/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2014 Chen-Yu Tsai <wens@csie.org>
 *
 * Configuration settings for the Allwinner A23 (sun8i) CPU
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * Include common sunxi configuration where most the settings are
 */
#include <configs/sunxi-common.h>

#ifdef CONFIG_SUNXI_UBIFS
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_CMD_UBIFS
#define CONFIG_MTD_UBI_WL_THRESHOLD 4096
#define CONFIG_MTD_UBI_BEB_LIMIT 20
#define CONFIG_CMD_UBI
#define CONFIG_RBTREE
#define CONFIG_LZO
/* Nand config */
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE	0x00

/* MTD SUPPORT */
#define MTDIDS_DEFAULT "nand0=nand"
#define MTDPARTS_DEFAULT "mtdparts=nand:-(sunxi_nand_mtd)"
#endif

#define SUNXI_DMA_SECURITY
/*
 * sun8iw18 specific configuration
 */
/*#define CONFIG_SF_DEFAULT_BUS   0
#define CONFIG_SF_DEFAULT_CS    0
#define CONFIG_SF_DEFAULT_SPEED 50000000
#define CONFIG_SF_DEFAULT_MODE  (SPI_RX_DUAL|SPI_RX_QUAD)
*/
#ifdef CONFIG_USB_EHCI_HCD
#define CONFIG_USB_EHCI_SUNXI
#endif

/* sram layout*/
#define SUNXI_SRAM_A1_BASE		(0x0)
#define SUNXI_SRAM_A1_SIZE		(0x10000)

#define SUNXI_SYS_SRAM_BASE		SUNXI_SRAM_A1_BASE
#define SUNXI_SYS_SRAM_SIZE		SUNXI_SRAM_A1_SIZE

/*read non zero when cpu is secure*/
#define SECURE_READ_TEST_REG	0x01C1E000

#define CONFIG_SUNXI_USB_PHYS	2

/* twi0 use the same pin with uart, the default use twi1 */
#define CONFIG_SYS_SPD_BUS_NUM	1

#define IR_BASE SUNXI_R_CIR_RX_BASE

/*undefine coms that sun8iw18 doesnt have*/
#undef CONFIG_SYS_NS16550_COM5
#define CONFIG_SYS_BOOTM_LEN 0x2000000
#endif /* __CONFIG_H */
