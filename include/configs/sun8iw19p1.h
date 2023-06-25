/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration settings for the Allwinner sun8iw19 CPU
 */

#ifndef __CONFIG_H
#define __CONFIG_H


#ifdef CONFIG_SUNXI_UBIFS
#define CONFIG_AW_MTD_SPINAND 1
#define CONFIG_AW_SPINAND_PHYSICAL_LAYER 1
#define CONFIG_AW_SPINAND_NONSTANDARD_SPI_DRIVER 1
#define CONFIG_AW_SPINAND_PSTORE_MTD_PART 0
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


/*
 * sun8iw19 specific configuration
 */

#ifdef CONFIG_USB_EHCI_HCD
#define CONFIG_USB_EHCI_SUNXI
#define CONFIG_USB_MAX_CONTROLLER_COUNT 1
#endif

/* sram layout*/
#define SUNXI_SRAM_A1_BASE		(0x00020000L)
#define SUNXI_SRAM_A1_SIZE		(0x18000)

#define SUNXI_SRAM_C_BASE		(0x00038000L)
#define SUNXI_SRAM_C_SIZE		(0x21000)

#define SUNXI_SYS_SRAM_BASE		SUNXI_SRAM_A1_BASE
#define SUNXI_SYS_SRAM_SIZE		(SUNXI_SRAM_A1_SIZE + SUNXI_SRAM_C_SIZE)

#define CONFIG_SUNXI_USB_PHYS	1

#define GICD_BASE		0x3021000
#define GICC_BASE		0x3022000
#define CONFIG_SYS_BOOTM_LEN 0x2000000

/*#define CONFIG_MMC_LOGICAL_OFFSET   (20 * 1024 * 1024/512)*/
/*
 * Include common sunxi configuration where most the settings are
 */
#include <configs/sunxi-common.h>

#define CONFIG_SYS_NONCACHED_MEMORY (1 << 20)

#endif /* __CONFIG_H */
