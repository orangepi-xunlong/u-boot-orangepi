/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2020 Aditya Prayoga <aditya@kobol.io>
 */

#ifndef __HELIOS64_H
#define __HELIOS64_H

#include <configs/rk3399_common.h>

#define SDRAM_BANK_SIZE			(2UL << 30)

#if defined(CONFIG_ENV_IS_IN_MMC)
	#define CONFIG_SYS_MMC_ENV_DEV 0
#elif defined(CONFIG_ENV_IS_IN_SPI_FLASH)
	#define CONFIG_ENV_SPI_BUS		CONFIG_SF_DEFAULT_BUS
	#define CONFIG_ENV_SPI_CS		CONFIG_SF_DEFAULT_CS
	#define CONFIG_ENV_SPI_MODE		CONFIG_SF_DEFAULT_MODE
	#define CONFIG_ENV_SPI_MAX_HZ	CONFIG_SF_DEFAULT_SPEED
#endif


#ifndef CONFIG_SPL_BUILD
#if CONFIG_IS_ENABLED(SCSI)

	#define CONFIG_SYS_SCSI_MAX_SCSI_ID     5
	#define CONFIG_SYS_SCSI_MAX_LUN         1
	#define CONFIG_SYS_SCSI_MAX_DEVICE      (CONFIG_SYS_SCSI_MAX_SCSI_ID * \
						CONFIG_SYS_SCSI_MAX_LUN)

	#define BOOT_TARGET_SCSI(func) \
	    func(SCSI, scsi, 0)
#else
	#define BOOT_TARGET_SCSI(func)
#endif

#undef BOOT_TARGET_DEVICES
#define BOOT_TARGET_DEVICES(func) \
	BOOT_TARGET_MMC(func) \
	BOOT_TARGET_USB(func) \
	BOOT_TARGET_SCSI(func) \
	BOOT_TARGET_PXE(func) \
	BOOT_TARGET_DHCP(func)

#endif

#endif
