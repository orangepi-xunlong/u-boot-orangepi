/* SPDX-License-Identifier: GPL-2.0+
 *
 * Configuration for Versatile Express. Parts were derived from other ARM
 *   configurations.
 *
 */

#ifndef __CV186X_ASIC_H__
#define __CV186X_ASIC_H__

#include <../../../board/cvitek/cv186x/cv186x_reg.h>

/* defined in cvipart.h */
#undef CONFIG_ENV_OFFSET
#undef CONFIG_ENV_OFFSET_REDUND
#undef CONFIG_ENV_SIZE
#undef CONFIG_ENV_IS_IN_SPI_FLASH
#undef CONFIG_ENV_IS_IN_MMC
#undef CONFIG_ENV_IS_IN_NAND
#undef CONFIG_ENV_SECT_SIZE

/* cvi_board_memmap.h is generated from build/boards/{CHIP_ARCH}/{BOARD}/memmap.py */
#include "cvi_board_memmap.h"
/* partition definitions header which is created by mkcvipart.py */
/* please do not modify header manually */
#include "cvipart.h"
#include "cvi_panels/cvi_panel_diffs.h"

// defined in this .h
#undef CONFIG_BOOTCOMMAND

#if defined(__aarch64__)
// #define CONFIG_ARMV8_SWITCH_TO_EL1
#endif

#define CONFIG_REMAKE_ELF

/* Physical Memory Map */
#define CONFIG_SYS_RESVIONSZ		CVIMMAP_ION_SIZE
#define CONFIG_SYS_RESVLOGOSZ		CVIMMAP_BOOTLOGO_SIZE
#define CONFIG_SYS_BOOTMAPSZ		CVIMMAP_KERNEL_MEMORY_SIZE

#define PHYS_SDRAM_1			CVIMMAP_KERNEL_MEMORY_ADDR
#define PHYS_SDRAM_1_SIZE		CVIMMAP_KERNEL_MEMORY_SIZE
#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1

/* Link Definitions */
#define CONFIG_SYS_INIT_SP_ADDR		CVIMMAP_CONFIG_SYS_INIT_SP_ADDR

/* default address for bootm command without arguments */
#define CONFIG_SYS_LOAD_ADDR		0x1000a0000
#define CONFIG_SYS_BOOTM_LEN		(64 << 20)

/* Generic Interrupt Controller Definitions */
#define GICD_BASE			(0x50001000)
#define GICC_BASE			(0x50002000)

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (9 << 20))

#if defined(__aarch64__)
#define CONFIG_SYS_NONCACHED_MEMORY	BIT(20)	/* 1 MiB */
#endif

#if defined(__riscv)
#define CONFIG_SYS_CACHELINE_SIZE	64
#endif

// Frequency of ARM arch timer and RISC-V rdtime
#define SYS_COUNTER_FREQ_IN_SECOND 25000000

/* 16550 Serial Configuration */
#define CONFIG_CONS_INDEX		1
#define CONFIG_SYS_NS16550_COM1		0x29180000
#define CONFIG_SYS_NS16550_COM3		0x291a0000
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_SYS_NS16550_MEM32
#define CONFIG_SYS_NS16550_CLK		200000000

/* include/generated/autoconf.h would define CONFIG_BAUDRATE from drivers/serial/Kconfig (default 115200) */
/*#define CONFIG_MENU_SHOW*/
/* Download related definitions */
#define UPGRADE_SRAM_ADDR    0x0e000030
#define UBOOT_PID_SRAM_ADDR  0x0e000030
#define UPDATE_ADDR	CVIMMAP_ION_ADDR
#define HEADER_ADDR	UPDATE_ADDR
#define USB_UPDATE_MAGIC MAGIC_NUM_USB_DL
/*----------------------------------------------------------------------
 * SPI Flash Configuration
 * ---------------------------------------------------------------------
 */

#ifdef CONFIG_SPI_FLASH
	#define CONFIG_SPI_FLASH_CVSFC

	#define CONFIG_CMD_MTDPARTS
	#define CONFIG_MTD_PARTITIONS
	#define CONFIG_FLASH_CFI_MTD
	#define CONFIG_SYS_MAX_FLASH_BANKS 1
	#define CONFIG_SPI_FLASH_MTD
#endif /* CONFIG_SPI_FLASH */

/*-----------------------------------------------------------------------
 * SPI NAND Flash Configuration
 *----------------------------------------------------------------------
 */

#ifdef CONFIG_NAND_SUPPORT
	/*#define CONFIG_ENV_IS_IN_NAND*/ /* env in nand flash */
	#define CONFIG_CMD_NAND
	#define CONFIG_SYS_MAX_NAND_DEVICE	 1

	#define CONFIG_NAND_FLASH_CVSNFC
	#define CONFIG_SYS_MAX_NAND_CHIPS 1
	/*#define CONFIG_SYS_NAND_SELF_INIT*/

	#define CONFIG_CMD_MTDPARTS
	#define CONFIG_MTD_PARTITIONS
	/* For CMD_UBI && CMD_UBIFS */
	#define CONFIG_RBTREE
	#define CONFIG_LZO
	//#define CONFIG_CMD_UBI
	//#define CONFIG_CMD_UBIFS
	#define CONFIG_MTD_UBI_WL_THRESHOLD 4096
	#define CONFIG_MTD_UBI_BEB_LIMIT 20
	#define NANDBOOT_V2

	#define CONFIG_SYS_NAND_MAX_CHIPS		1
	#define CONFIG_SYS_NAND_BASE			SPI_NAND_REG_BASE
	#define CONFIG_CVSNFC_MAX_CHIP			CONFIG_SYS_MAX_NAND_DEVICE
	#define CONFIG_CVSNFC_REG_BASE_ADDRESS		SPI_NAND_REG_BASE
	#define CONFIG_CVSNFC_BUFFER_BASE_ADDRESS	SPI_NAND_MEM_BASE
	#define CONFIG_CVSNFC_HARDWARE_PAGESIZE_ECC
	#define CONFIG_SYS_NAND_BASE_LIST		{CONFIG_SYS_NAND_BASE}

#endif /* CONFIG_NAND_SUPPORT */

/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE		512	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_SYS_MAXARGS		64	/* max command args */

#ifndef CONFIG_ENV_SECT_SIZE
#define CONFIG_ENV_SECT_SIZE		0x00040000
#endif

#define CONFIG_ENV_OVERWRITE

#define CONFIG_MMC_UHS_SUPPORT
#define CONFIG_MMC_HS200_SUPPORT
#define CONFIG_MMC_SUPPORTS_TUNING

/* #define CONFIG_USB_DWC2 */
/* #define CONFIG_USB_DWC2_REG_ADDR	0x04340000 */
/* Enable below CONFIG for fastboot */

/* To use usb fastboot, you need to enable below Kconfig
 * CONFIG_USB, CONFIG_USB_GADGET and CONFIG_USB_GADGET_DWC2_OTG
 */

#ifdef CONFIG_USB_GADGET
#define CONFIG_USB_FUNCTION_FASTBOOT
#define CONFIG_USB_GADGET_DOWNLOAD
#define CONFIG_G_DNL_MANUFACTURER "Cvitek"
#define CONFIG_G_DNL_VENDOR_NUM   0x18d1
#define CONFIG_G_DNL_PRODUCT_NUM 0x4ee0
#endif

#define CONFIG_IPADDR			192.168.0.3
#define CONFIG_NETMASK			255.255.255.0
#define CONFIG_GATEWAYIP		192.168.0.11
#define CONFIG_SERVERIP			192.168.56.101

#ifdef CONFIG_USE_DEFAULT_ENV
/* The following Settings are chip dependent */
/******************************************************************************/
	#define UIMAG_ADDR	CVIMMAP_UIMAG_ADDR

	#ifdef CONFIG_BOOTLOGO
		#define LOGO_RESERVED_ADDR __stringify(CVIMMAP_BOOTLOGO_ADDR)//yuv load addr
		#define LOGO_READ_ADDR "0x84080000" //jpeg load addr
		#define VO_ALIGNMENT "16"
		#define LOGOSIZE "0x80000" //jpeg max size
	#endif
/******************************************************************************/
/* define common env */
/*******************************************************************************/
	/* Config FDT_NO */
	#ifndef USE_HOSTCC
		#define FDT_NO __stringify(CVICHIP) "_" __stringify(CVIBOARD)
	#else
		#define FDT_NO ""
	#endif

	/* config root */
	#ifdef CONFIG_NAND_SUPPORT
		#ifdef CONFIG_SKIP_RAMDISK
			#define ROOTARGS "ubi.mtd=ROOTFS ubi.block=0,0 root=/dev/ubiblock0_0 rootfstype=squashfs"

		#else
			#define ROOTARGS "ubi.mtd=ROOTFS ubi.block=0,0"
		#endif /* CONFIG_SKIP_RAMDISK */
	#elif CONFIG_SD_BOOT
		#define ROOTARGS "root=" ROOTFS_DEV " rootwait rw"
	#else
		#define ROOTARGS "rootfstype=squashfs rootwait ro root=" ROOTFS_DEV
	#endif

	/* BOOTARGS */
	#define PARTS  PART_LAYOUT

	/* config uart */
	#define CONSOLEDEV "ttyS0\0"

	/* config loglevel */
	#ifdef RELEASE
		#define OTHERBOOTARGS   "othbootargs=earlycon release loglevel=0 \0"
	#else
		#define OTHERBOOTARGS   "othbootargs=earlycon loglevel=9 \0"
	#endif

	/* config mtdids */
	#ifdef CONFIG_NAND_SUPPORT
		#define MTDIDS_DEFAULT "nand0=cvsnfc"
	#elif CONFIG_SPI_FLASH
		#define MTDIDS_DEFAULT "nor1=flash-0"
	#else
		#define MTDIDS_DEFAULT ""
	#endif

	#if defined(CONFIG_ROOTFS_UBUNTU) || defined(CONFIG_ROOTFS_DEBIAN)
		#define CONFIG_EXTRA_ENV_SETTINGS	\
		"devtype=mmc\0" \
		"devnum=1\0" \
		"distro_bootpart=1\0" \
		"scriptaddr=0x120000000\0" \
		"ramdisk_addr_r=0x140000000\0" \
		"ramdisk_addr_b=0x1c0000000\0" \
		"unzip_addr=0x160000000\0" \
		"pxefile_addr_r=0x180000000\0" \
		"fdt_addr_r=0x1b0000000\0" \
		"kernel_addr_r=0x1f0000000\0" \
		"nfsargs=setenv bootargs root=/dev/nfs init=/init rw " \
			"nfsroot=${serverip}:${rootpath},v3,tcp " \
			"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}:${netdev}:off " \
			"console=${consoledev},${baudrate} ${othbootargs};\0"       \
		"netdev=eth0\0"		\
		"consoledev=" CONSOLEDEV  \
		"baudrate=115200\0" \
		"uImage_addr=" __stringify(UIMAG_ADDR) "\0" \
		"update_addr=" __stringify(UPDATE_ADDR) "\0" \
		"fdt_high=" __stringify(0x100200000) "\0" \
		"initrd_high=" __stringify(0x104000000) "\0" \
		"mtdparts=" PARTS "\0" \
		"mtdids=" MTDIDS_DEFAULT "\0" \
		"root=" ROOTARGS "\0" \
		"sddev=1\0" \
		"sdboot=" SD_BOOTM_COMMAND "\0" \
		"recboot=" RECBOOTCOMMAND "\0" \
		"UBOOT_ENV_SUPPORT_OTA_ENABLE=1\0" \
		OTHERBOOTARGS \
		PARTS_OFFSET
	#else
		#define CONFIG_EXTRA_ENV_SETTINGS	\
		"netdev=eth0\0"		\
		"consoledev=" CONSOLEDEV  \
		"baudrate=115200\0" \
		"uImage_addr=" __stringify(UIMAG_ADDR) "\0" \
		"update_addr=" __stringify(UPDATE_ADDR) "\0" \
		"fdt_high=" __stringify(0x100200000) "\0" \
		"initrd_high=" __stringify(0x104000000) "\0" \
		"mtdparts=" PARTS "\0" \
		"mtdids=" MTDIDS_DEFAULT "\0" \
		"sddev=1\0" \
		"ota_part=7\0" \
		"ota_enable=0\0" \
		"root=" ROOTARGS "\0" \
		"sdboot=" SD_BOOTM_COMMAND "\0" \
		OTHERBOOTARGS \
		PARTS_OFFSET
	#endif

/********************************************************************************/
	/* UBOOT_VBOOT commands */
	#ifdef UBOOT_VBOOT
		#define UBOOT_VBOOT_BOOTM_COMMAND \
					"aes_itb dec_fdt_key 0 ${uImage_addr} ${uImage_addr}; " \
					"if test $? -ne 0; then " \
					"  echo ITB decryption failed; " \
					"else; " \
					"  bootm ${uImage_addr}#config-" FDT_NO ";" \
					"fi;"
	#else
		#define UBOOT_VBOOT_BOOTM_COMMAND "bootm ${uImage_addr}#config-" FDT_NO ";"
		#define UBOOT_DTS_TYPE_BOOTM_COMMAND "bootm ${uImage_addr}#${DTS_TYPE}"
	#endif

	/* BOOTLOGO */
	#ifdef CONFIG_BOOTLOGO
		#define SHOWLOGOCMD "run showlogo;"

		#ifdef CONFIG_NAND_SUPPORT
			#define LOAD_LOGO "nand read " LOGO_READ_ADDR " MISC;"
		#elif defined(CONFIG_SPI_FLASH)
			#define LOAD_LOGO "sf probe;sf read " LOGO_READ_ADDR " ${MISC_PART_OFFSET} ${MISC_PART_SIZE};"
		#else
			#define LOAD_LOGO "mmc dev 0;mmc read " LOGO_READ_ADDR " ${MISC_PART_OFFSET} ${MISC_PART_SIZE};"
		#endif
		#define SHOWLOGOCOMMAND LOAD_LOGO CVI_JPEG START_VO START_VL SET_VO_BG
	#else
		#define SHOWLOGOCMD
	#endif

	#define SET_BOOTARGS "setenv bootargs ${root} ${mtdparts} " \
					"console=$consoledev,$baudrate $othbootargs;"

	#define SD_BOOTM_COMMAND                                                       \
		SET_BOOTARGS                                                           \
		"echo Boot from SD dev ${sddev} ...;"                                  \
		"mmc dev ${sddev} && fatload mmc ${sddev} ${uImage_addr} boot.itb;"    \
		"if test $? -eq 0; then " UBOOT_DTS_TYPE_BOOTM_COMMAND "; fi;"

	/*#define SD_BOOTM_COMMAND \ */
	/*				SET_BOOTARGS \ */
	/*				"echo Boot from SD with ramboot.itb;" \ */
	/*				"mmc dev 1 && fatload mmc 1 ${uImage_addr} ramboot.itb; " \ */
	/*				"if test $? -eq 0; then " \ */
	/*				UBOOT_VBOOT_BOOTM_COMMAND \ */
	/*				"fi;" */

	#define CONFIG_RAMBOOTCOMMAND                                                  \
		SET_BOOTARGS                                                           \
		"echo Boot from ramboot.itb;" UBOOT_VBOOT_BOOTM_COMMAND

	#if defined(CONFIG_ROOTFS_UBUNTU) || defined(CONFIG_ROOTFS_DEBIAN)
		#define RECBOOTCOMMAND                                                         \
			"setenv bootargs console=${consoledev},${baudrate} ${othbootargs}; "   \
			"load mmc 0:2 ${uImage_addr} recovery.itb;"                            \
			"bootm ${uImage_addr}#config-" FDT_NO ";"

		#if defined(CONFIG_SD_BOOT)
			#define CONFIG_BOOTCOMMAND                                                     \
				"run sdboot"
		#else
			#if defined(CONFIG_NVME_BOOT)
				#define CONFIG_BOOTCOMMAND                                                     \
					"cvi_update || pci e; nvme scan; load nvme 0:1 ${scriptaddr} boot.scr.nvme; source ${scriptaddr}"
			#elif defined(CONFIG_SATA_BOOT)
				#define CONFIG_BOOTCOMMAND                                                     \
					"cvi_update || scsi scan; load scsi 0:1 ${scriptaddr} boot.scr.sata; source ${scriptaddr}"
			#else	//default eMMC
				#define CONFIG_BOOTCOMMAND                                                     \
					"cvi_update || load mmc 0:1 ${scriptaddr} boot.scr.emmc; source ${scriptaddr} || run ramboot"
			#endif
		#endif
	#else
		#define CONFIG_BOOTCOMMAND                                                     \
			SHOWLOGOCMD                                                            \
				"cvi_update || run ramboot || run emmcboot || run norboot || run nandboot"
	#endif

	#if defined(CONFIG_NAND_SUPPORT)
		/* For spi nand boot, need to reset DMA and its setting before exiting uboot */
		/* 0x4330058 : DMA reset */
		/* 0x3000154 : restore DMA remap to 0 */
		#define CONFIG_NANDBOOTCOMMAND                                                 \
			SET_BOOTARGS                                                           \
			"nand read ${uImage_addr} BOOT;" UBOOT_DTS_TYPE_BOOTM_COMMAND
	#elif defined(CONFIG_SPI_FLASH)
		#define CONFIG_NORBOOTCOMMAND                                                  \
			SET_BOOTARGS                                                           \
			"sf probe;sf read ${uImage_addr} ${BOOT_PART_OFFSET} ${BOOT_PART_SIZE};" \
			UBOOT_DTS_TYPE_BOOTM_COMMAND
	#elif defined(CONFIG_EMMC_SUPPORT)
		#define CONFIG_EMMCBOOTCOMMAND                                                 \
			SET_BOOTARGS                                                           \
			"mmc dev 0 ;"                                                          \
			"mmc read ${uImage_addr} ${BOOT_PART_OFFSET} ${BOOT_PART_SIZE} ;" UBOOT_DTS_TYPE_BOOTM_COMMAND

		#define CONFIG_RAMBOOTICOMMAND                                                 \
			"setenv bootargs console=$consoledev,$baudrate $othbootargs; "         \
			"mmc dev 1;fatload mmc 1 0x104000000 Image;"                           \
			"fatload mmc 1 0x105000000 boot.cpio.gz.img;"                          \
			"fatload mmc 1 0x106000000 " FDT_NO ".dtb;"                            \
			"echo run Rambooti...;"                                                \
			"if test $? -eq 0; then "                                              \
			"booti 0x104000000 0x105000000 0x106000000;"                           \
			"fi;"
	#endif

#else
	/* define your environment */
	//#define CONFIG_BOOTCOMMAND ""

#endif /* CONFIG_USE_DEFAULT_ENV */

#if (CONFIG_IS_ENABLED(CMD_PCI) && CONFIG_IS_ENABLED(CMD_NVME))
	#define BOOT_TARGET_NVME(func) \
		func(NVME, nvme, 0) \
		func(NVME, nvme, na)
#else
        #define BOOT_TARGET_NVME(func)
#endif

#if CONFIG_IS_ENABLED(CMD_MMC)
	#define BOOT_TARGET_MMC(func) \
		func(MMC, mmc, 1) \
		func(MMC, mmc, 0)
#else
	#define BOOT_TARGET_MMC(func)
#endif


#if CONFIG_IS_ENABLED(CMD_USB)
	#define BOOT_TARGET_USB(func) func(USB, usb, 0)
#else
	#define BOOT_TARGET_USB(func)
#endif

#if CONFIG_IS_ENABLED(CMD_PXE)
	#define BOOT_TARGET_PXE(func) func(PXE, pxe, na)
#else
	#define BOOT_TARGET_PXE(func)
#endif

#if CONFIG_IS_ENABLED(CMD_DHCP)
	#define BOOT_TARGET_DHCP(func) func(DHCP, dhcp, na)
#else
	#define BOOT_TARGET_DHCP(func)
#endif

#define BOOT_TARGET_DEVICES(func) \
	BOOT_TARGET_MMC(func) \
	BOOT_TARGET_NVME(func) \
	BOOT_TARGET_USB(func) \
	BOOT_TARGET_PXE(func) \
	BOOT_TARGET_DHCP(func)

#include <config_distro_bootcmd.h>

#define UIMAG_ADDR      CVIMMAP_UIMAG_ADDR
#define CONFIG_EXTRA_ENV_SETTINGS \
	"scriptaddr=0x120000000\0" \
	"ramdisk_addr_r=0x140000000\0" \
	"kernel_addr_r=0x1f0000000\0" \
	"uImage_addr=" __stringify(UIMAG_ADDR) "\0" \
	"fdt_addr_r=0x1b0000000\0" \
	"pxefile_addr_r=0x180000000\0" \
        BOOTENV

#endif /* __CV186X_ASIC_H__ */
