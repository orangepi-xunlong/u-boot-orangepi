/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration settings for the Allwinner A64 (sun50i) CPU
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*#define FPGA_PLATFORM*/
/*
 * A64 specific configuration
 */
#define SUNXI_ARM_A53

#ifdef CONFIG_USB_EHCI_HCD
#define CONFIG_USB_EHCI_SUNXI
#define CONFIG_USB_MAX_CONTROLLER_COUNT 4
#endif

/* sram layout*/
#define SUNXI_SRAM_A1_BASE		(0x00020000L)
#define SUNXI_SRAM_A1_SIZE		(0x8000)

#define SUNXI_SRAM_C_BASE		(0x00028000L)
#define SUNXI_SRAM_C_SIZE		(0x30000)

#define SUNXI_SYS_SRAM_BASE		SUNXI_SRAM_A1_BASE
#define SUNXI_SYS_SRAM_SIZE		(SUNXI_SRAM_A1_SIZE + SUNXI_SRAM_C_SIZE)

#define CONFIG_SYS_BOOTM_LEN 0x2000000

#define CONFIG_SUNXI_USB_PHYS	1

#define GICD_BASE		0x3021000
#define GICC_BASE		0x3022000

#define IR_BASE			SUNXI_R_IR_RX_BASE

/*
 * Include common sunxi configuration where most the settings are
 */
#include <configs/sunxi-common.h>

#endif /* __CONFIG_H */
