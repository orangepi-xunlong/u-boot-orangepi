/*
 * Copyright (c) 2015-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __PLATFORM_DEF_H__
#define __PLATFORM_DEF_H__

#include "platform_common_def.h"

#define CV1822_FPGA 1
/*******************************************************************************
 * HW/SW feature definitions
 ******************************************************************************/
//#define CLI_NO_WAIT // if defined, will not give user several seconds to hit any key for entering CLI

//#define BL1_IN_SPI_FLASH // Boot from SPI NOR1 instead of onboard ROM

#define BL1_USE_CLI
#define BL1_SEC_BOOT
#define BL1_DISABLE_YMODEM
#define BL1_INIT_EFUSE
#define BL1_INIT_GPIO
#define BL1_INIT_EMMC
#define BL1_INIT_SPI_NAND
#define BL1_INIT_SPI_NOR
#define BL1_INIT_SDFAT
#define BL1_INIT_USB
// #define BL1_RTL_SIM

// #define BL2_USE_CLI
#define BL2_INIT_GPIO
// #define BL2_INIT_DDR
#define BL2_INIT_EMMC
#define BL2_INIT_SPI_NAND
#define BL2_INIT_SPI_NOR
#define BL2_INIT_SDFAT

#define BL31_IN_DDR

#define BL_USE_DW_TIMER

#define CLI_HOT_KEY (' ')
#define CLI_HOT_KEY_STR "SPACE"

/* Instead of byte memcpy , use dword memcpy when src, dst and len is dword aligned */
#define FAST_MEMCOPY

#define ENABLE_SDIO_IO_CELL_POWER

#define ENABLE_SDIO_SOURCE_SELECT_SETTING

//#if (defined(PLAT_BM_FIP_BASE))
//#undef PLAT_BM_FIP_BASE
//#define PLAT_BM_FIP_BASE		(DRAM_BASE + 0x10000000)
//#endif
/*
 * BL1 specific defines.
 */
#ifdef BL1_IN_SPI_FLASH
#undef BL1_ROM_BASE
#undef BL1_RO_BASE
#undef BL1_RO_LIMIT
#undef SPIF_FIP_OFFSET
#define BL1_ROM_BASE			SPIF1_BASE
#define BL1_RO_BASE			BL1_ROM_BASE
#define BL1_RO_LIMIT			(BL1_ROM_BASE + BL1_ROM_SIZE)
#define SPIF_FIP_OFFSET			(BL1_ROM_SIZE)
#endif

/*
 * clock definitions.
 * to get more close to wall time, on FPGA we need 10MHz, and on Palladium
 * we need 50KHz. but this will make too many timer interrupts in kenrel,
 * which makes kernel barely unable to boot. 10MHz on Palladium and 500MHz
 * on FPGA seem to be still a slow but acceptable choice. we cat achieve
 * different timer setting for kernel by adding clock-frequency node at timer
 * in device tree.
 */
#define SYS_COUNTER_FREQ_IN_TICKS		25000000

/*
 * If enable, the global variable of emmc/sd clock could be changed by blp
 */
#define SUPPORT_SD_EMMC_CLOCK_ADJUSTMENT

/*
 * UART buadrate and clock
 */
#define PLAT_BM_CONSOLE_BAUDRATE		115200
#define PLAT_BM_BOOT_UART_CLK_IN_HZ		25000000

/*
 * dw timer clock
 */
#define PLAT_DW_TIMER_CLK_HZ		25000000

/*
 * SD/EMMC clock
 */
#define PLAT_BM_SD_CLK				25000000
#define PLAT_BM_EMMC_CLK			25000000

/*
 * eMMC definitions
 */
#define EMMC_BUS_WIDTH				EMMC_BUS_WIDTH_1

/*
 * USB definitions
 */
#define USB_PHY_DETECTION

#endif /* __PLATFORM_DEF_H__ */
