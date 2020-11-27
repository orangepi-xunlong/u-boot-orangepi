/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang<wangflord@allwinnertech.com>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __WINE_CONFIG_H
#define __WINE_CONFIG_H

#include "sunxi-base.h"


#undef DEBUG

#ifndef __KERNEL__
#define __KERNEL__
#endif

//#define DEBUG
//#define FPGA_PLATFORM
#define CONFIG_TARGET_NAME      sun50iw1p1
#define CONFIG_ARCH_SUN50IW1P1
#define CONFIG_ARM_A53


/*********************************************************************
 *platform memory map
 **********************************************************************/
#define CONFIG_SYS_SRAM_BASE             (0x10000)
#define CONFIG_SYS_SRAM_SIZE             (0x8000)
#define CONFIG_SYS_SRAMA2_BASE           (0x40000)
#define CONFIG_SYS_SRAMA2_SIZE           (0x14000)
#define CONFIG_SYS_SRAMC_BASE            (0x18000)
#define CONFIG_SYS_SRAMC_SIZE            (160<<10)

#define PLAT_SDRAM_BASE                   0x40000000 
#define CONFIG_SYS_SDRAM_BASE             0x40000000
#define SDRAM_OFFSET(x)                  (0x40000000+(x))


/*********************************************************************
 *boot0 run address config
 **********************************************************************/
#define CONFIG_BOOT0_RUN_ADDR            (0x10000)
#define CONFIG_FES1_RUN_ADDR             (0x18000)
#define CONFIG_TOC0_RUN_ADDR             (0x10480)

#define CONFIG_BOOT0_RET_ADDR            (CONFIG_SYS_SRAM_BASE)
#define CONFIG_TOC0_RET_ADDR             (0)
#define CONFIG_FES1_RET_ADDR             (CONFIG_SYS_SRAMC_BASE + 0x7210)

#define CONFIG_NORMAL_BOOT_STACK         (CONFIG_SYS_SRAM_BASE+CONFIG_SYS_SRAM_SIZE)
#define CONFIG_SECURE_BOOT_STACK         (CONFIG_SYS_SRAMC_BASE+CONFIG_SYS_SRAMC_SIZE)

#define CONFIG_STORAGE_MEDIA_NAND
#define CONFIG_STORAGE_MEDIA_MMC


/*********************************************************************
 *CPUs and ATF run address config
 **********************************************************************/
#define SCP_SRAM_BASE                      (CONFIG_SYS_SRAMA2_BASE)
#define SCP_SRAM_SIZE                      (CONFIG_SYS_SRAMA2_SIZE)
#define BL31_BASE                           PLAT_TRUSTED_DRAM_BASE
#define BL31_SIZE                          (0x100000)
#define SCP_DRAM_BASE                      (PLAT_TRUSTED_DRAM_BASE+BL31_SIZE)
#define SCP_DRAM_SIZE                      (0x4000)
#define SCP_CODE_DRAM_OFFSET		     (0x18000)


/*********************************************************************
 *Uboot run address config
 **********************************************************************/
#define CONFIG_SYS_TEXT_BASE              0x4A000000


/*********************************************************************
 *uboot common config support
 **********************************************************************/
#define CONFIG_SUNXI_SECURE_STORAGE
#define CONFIG_SUNXI_SECURE_SYSTEM
#define PMU_SCRIPT_NAME "charger0"
#define FDT_PATH_REGU "regulator0"
#define CONFIG_SUNXI_CORE_VOL           1200
#define CONFIG_SUNXI_RSB
#define CONFIG_AXP_USE_RSB
#define CONFIG_USB_MAX_CONTROLLER_COUNT 2
#define CONFIG_SUNXI_KEY_SUPPORT
#define CONFIG_SUNXI_KEY_BURN
#define CONFIG_SUNXI_DRAGONBOARD_SUPPORT
#define CONFIG_LZMA

#define FEL_BASE                         0x20
#define SUNXI_FEL_ADDR_IN_SECURE         (0x64)
#define SUNXI_RUN_EFEX_FLAG              (0x5AA5A55A)
#define SUNXI_RUN_EFEX_ADDR              (SUNXI_RTC_BASE + 0x108)

#define CONFIG_SUNXI_I2C
#define CONFIG_CPUX_I2C
#define CONFIG_SYS_I2C_SPEED    400000
#define CONFIG_SYS_I2C_SLAVE    0x10

//#define CONFIG_SUNXI_DMA
#define CONFIG_SUNXI_CHIPID
#define CONFIG_SUNXI_ARISC_EXIST
#define CONFIG_SUNXI_MULITCORE_BOOT
//#define CONFIG_SUNXI_HDCP_IN_SECURESTORAGE
#define CONFIG_GPT_SUPPORT
#ifdef CONFIG_GPT_SUPPORT
#define CONFIG_SUNXI_GPT
#define CONFIG_EFI_PARTITION
#define CONFIG_CONVERT_CARD0_TO_GPT
#endif

#define CONFIG_OPTEE25
#define CONFIG_WIDEVINE_KEY_INSTALL
#define CONFIG_KEYMASTER_KEY_INSTALL
#define CONFIG_DETECT_RTC_BOOT_MODE

/*********************************************************************
 *uboot cmd support
 **********************************************************************/
#define CONFIG_CMD_IRQ
#define CONFIG_CMD_ELF
#define CONFIG_CMD_BOOTA
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_FAT			/* with this we can access bootfs in nand */
#define CONFIG_CMD_BOOTA		/* boot android image */
#define CONFIG_CMD_RUN			/* run a command */
#define CONFIG_CMD_BOOTD		/* boot the default command */
#define CONFIG_CMD_FASTBOOT
#define CONFIG_CMD_SUNXI_SPRITE
#define CONFIG_CMD_SUNXI_TIMER
#define CONFIG_CMD_SUNXI_EFEX
#define CONFIG_CMD_SUNXI_SHUTDOWN
#define CONFIG_CMD_SUNXI_BMP
#define CONFIG_CMD_SUNXI_BURN
#define CONFIG_CMD_SUNXI_MEMTEST
#define CONFIG_CMD_FDT
#define CONFIG_CMD_LZMADEC
#ifdef CONFIG_SUNXI_DMA
#define CONFIG_SUNXI_CMD_DMA
#endif
#define CONFIG_SUNXI_CMD_SMC



/*********************************************************************
 *module support
 **********************************************************************/
#define CONFIG_SUNXI_MODULE_SPRITE
#define CONFIG_SUNXI_MODULE_NAND
#define CONFIG_SUNXI_MODULE_SDMMC
#define CONFIG_SUNXI_MODULE_USB
#define CONFIG_SUNXI_MODULE_AXP
#define CONFIG_SUNXI_MODULE_DISPLAY

#ifdef CONFIG_SUNXI_MODULE_SDMMC
/* mmc config */
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_CMD_MMC
#define CONFIG_MMC_SUNXI
#define CONFIG_MMC_SUNXI_USE_DMA
#define CONFIG_STORAGE_EMMC
#define CONFIG_MMC_LOGICAL_OFFSET   (20 * 1024 * 1024/512)
#endif

#ifdef CONFIG_SUNXI_MODULE_NAND
/* Nand config */
#define CONFIG_NAND
#define CONFIG_STORAGE_NAND
#define CONFIG_NAND_SUNXI
#define CONFIG_SYS_MAX_NAND_DEVICE      1
#define CONFIG_SYS_NAND_BASE            0x00
#endif


#ifdef CONFIG_SUNXI_MODULE_USB
#define CONFIG_USBD_HS
#define CONFIG_USB_EHCI_SUNXI
//for usb host
#ifdef CONFIG_USB_EHCI_SUNXI
	#define CONFIG_EHCI_DCACHE
	#define CONFIG_CMD_USB
	#define CONFIG_USB_STORAGE
	#define CONFIG_USB_EHCI
#endif

//#define CONFIG_USB_ETHER
#ifdef CONFIG_USB_ETHER
/* net support */
#define CONFIG_CMD_NET
#define CONFIG_NET_MULTI
#define CONFIG_CMD_PING
#define CONFIG_CMD_NFS
/*
 * Reducing the ARP timeout from default 5000UL to 1000UL we speed up the
 * initial TFTP transfer or PING, etc, should the user wish one, significantly.
 */
#define CONFIG_ARP_TIMEOUT	1000UL

/* USB SUSPORT */
#define CONFIG_USB_ETHER
#define CONFIG_USB_ETH_RNDIS
#define CONFIG_USB_GADGET_DUALSPEED
#define CONFIG_USB_SUNXI_UDC0

#endif

#endif

#ifdef CONFIG_SUNXI_MODULE_DISPLAY
#define CONFIG_SUNXI_DISPLAY
#define CONFIG_VIDEO_SUNXI_V3
#define CONFIG_SUNXI_MODULE_HDMI
#define CONFIG_SUNXI_MODULE_TV
#define CONFIG_SUNXI_LOGBUFFER
#endif

#ifdef CONFIG_SUNXI_MODULE_AXP
#define CONFIG_SUNXI_AXP
#define CONFIG_SUNXI_AXP81X
#define CONFIG_SUNXI_AXP_CONFIG_ONOFF
#endif


//#define CONFIG_SYS_DCACHE_OFF

/* Open sunxi UBIFS funciton */
/* #define CONFIG_SUNXI_UBIFS */

#ifdef CONFIG_SUNXI_UBIFS
/* NAND DEBUG SUPPORT */
#define CONFIG_CMD_NAND

/* UBIFS SUPPORT */
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_RBTREE
#define CONFIG_LZO

/* MTD SUPPORT */
#define MTDIDS_DEFAULT "nand0=nand"
#define MTDPARTS_DEFAULT "mtdparts=nand:-(sunxi_nand_mtd)"
#endif

/* Generate Serial Number */
#define CONFIG_SUNXI_SERIAL
/* get param from secure storage to cmdline */
#define CONFIG_SUNXI_USER_KEY

#endif /* __CONFIG_H */
