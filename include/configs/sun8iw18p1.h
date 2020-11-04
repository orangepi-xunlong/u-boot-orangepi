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
#define CONFIG_AW_MTD_SPINAND 1
#define CONFIG_AW_SPINAND_PHYSICAL_LAYER 1
#define CONFIG_AW_SPINAND_NONSTANDARD_SPI_DRIVER 1
#define CONFIG_AW_SPINAND_PSTORE_MTD_PART 1
#define CONFIG_AW_MTD_SPINAND_UBOOT_BLKS 24
#define CONFIG_AW_SPINAND_ENABLE_PHY_CRC16 0
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_CMD_UBIFS
#define CONFIG_MTD_UBI_WL_THRESHOLD 4096
#define CONFIG_MTD_UBI_BEB_LIMIT 40
#define CONFIG_CMD_UBI
#define CONFIG_RBTREE
#define CONFIG_LZO
/* Nand config */
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE	0x00
/* simulate ubi solution offline burn */
/* #define CONFIG_UBI_OFFLINE_BURN */
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
#define SUNXI_SRAM_A1_BASE		(0x00020000L)
#define SUNXI_SRAM_A1_SIZE		(0x20000)

#define SUNXI_SYS_SRAM_BASE		SUNXI_SRAM_A1_BASE
#define SUNXI_SYS_SRAM_SIZE		SUNXI_SRAM_A1_SIZE


#define CONFIG_SUNXI_USB_PHYS	2

/* twi0 use the same pin with uart, the default use twi1 */
#define CONFIG_SYS_SPD_BUS_NUM	1

/*undefine coms that sun8iw18 doesnt have*/
#undef CONFIG_SYS_NS16550_COM5
#define CONFIG_SYS_BOOTM_LEN 0x2000000

/* watchdog */
#define WATCH_BASE                      0x030090a0
#define WDOG_TIMEOUT                    16

#endif /* __CONFIG_H */
