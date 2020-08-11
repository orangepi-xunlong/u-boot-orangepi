/*
 * (C) Copyright 2017-2018
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 * Configuration settings for the Allwinner sunxi series of boards.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _SUNXI_BASE_CONFIG_H
#define _SUNXI_BASE_CONFIG_H

#include <asm/arch/platform.h>	/* get chip and board defs */

/*
 * High Level Configuration Options
 */
#define CONFIG_ALLWINNER			/* It's a Allwinner chip */
#define	CONFIG_SUNXI				/* which is sunxi family */


/*u-boot defconfig*/
#define LINUX_MACHINE_ID        4137
#define UBOOT_VERSION			"3.0.0"
#define UBOOT_PLATFORM		    "1.0.0"
#define CONFIG_SYS_GENERIC_BOARD
#define HAVE_VENDOR_COMMON_LIB

#define CONFIG_NR_DRAM_BANKS               1
#define PHYS_SDRAM_1                       SDRAM_OFFSET(0x00000000)	/* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE                  (512 << 20)				/* 512 MB Bank #1 */

#define DRAM_PARA_STORE_ADDR               SDRAM_OFFSET(0x00800000)
/* memtest works on */
#define CONFIG_SYS_MEMTEST_START           SDRAM_OFFSET(0x00000000)
#define CONFIG_SYS_MEMTEST_END             SDRAM_OFFSET(0x02000000)	/* 256M */

#define TOC0_MMU_BASE_ADDRESS              SDRAM_OFFSET(0x02800000)
#define CONFIG_BOOTPKG_STORE_IN_DRAM_BASE  SDRAM_OFFSET(0x02e00000)

#define SUNXI_LOGO_COMPRESSED_LOGO_SIZE_ADDR            SDRAM_OFFSET(0x03000000)
#define SUNXI_LOGO_COMPRESSED_LOGO_BUFF                 SDRAM_OFFSET(0x03000010)
#define SUNXI_SHUTDOWN_CHARGE_COMPRESSED_LOGO_SIZE_ADDR SDRAM_OFFSET(0x03100000)
#define SUNXI_SHUTDOWN_CHARGE_COMPRESSED_LOGO_BUFF  	SDRAM_OFFSET(0x03100010)
#define SUNXI_ANDROID_CHARGE_COMPRESSED_LOGO_SIZE_ADDR  SDRAM_OFFSET(0x03200000)
#define SUNXI_ANDROID_CHARGE_COMPRESSED_LOGO_BUFF   	SDRAM_OFFSET(0x03200010)

/* fdt addr for kernel */
#define CONFIG_SUNXI_FDT_ADDR              SDRAM_OFFSET(0x04000000)

/*#define CONFIG_SUNXI_LOGBUFFER*/
#define SUNXI_DISPLAY_FRAME_BUFFER_ADDR    SDRAM_OFFSET(0x06400000)
#define SUNXI_DISPLAY_FRAME_BUFFER_SIZE    0x01000000

/* trusted dram:0x08000000~0x09000000 */
#define PLAT_TRUSTED_DRAM_BASE             SDRAM_OFFSET(0x08000000)
#define PLAT_TRUSTED_DRAM_SIZE             0x01000000


/* define malloc space,Size of malloc pool */
#define CONFIG_NONCACHE_MEMORY
#define CONFIG_NONCACHE_MEMORY_SIZE        (16 * 1024 * 1024)
#define CONFIG_SYS_MALLOC_LEN              (CONFIG_ENV_SIZE + (64 << 20))


/* for usb efex -- tools private data */
#define SRAM_AREA_A		                    CONFIG_SYS_SRAM_BASE

/* the sram base address, and the stack address in stage1 */
#define CONFIG_SYS_INIT_RAM_ADDR	     CONFIG_SYS_SRAM_BASE
#define CONFIG_SYS_INIT_RAM_SIZE	     CONFIG_SYS_SRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)


/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CONFIG_SYS_NO_FLASH

#define CONFIG_SYS_MONITOR_LEN		(256 << 10)	/* 256 KiB */
#define CONFIG_IDENT_STRING			" Allwinner Technology "

#define CONFIG_ENV_IS_IN_SUNXI_FLASH	/* we store env in one partition of our nand */
#define CONFIG_SUNXI_ENV_PARTITION		"env"	/* the partition name */

/*------------------------------------------------------------------------
 * we save the environment in a nand partition, the partition name is defined
 * in sysconfig.fex, which must be the same as CONFIG_SUNXI_NAND_ENV_PARTITION
 * if not, below CONFIG_ENV_ADDR and CONFIG_ENV_SIZE will be where to store env.
 * */
#define CONFIG_ENV_ADDR				(53 << 20)  /* 16M */
#define CONFIG_ENV_SIZE				(128 << 10)	/* 128KB */
#define CONFIG_CMD_SAVEENV

#define CONFIG_EXTRA_ENV_SETTINGS \
	"bootdelay=3\0" \
	"bootcmd=run setargs_nand boot_normal\0" \
	"console=ttyS0,115200\0" \
	"nand_root=/dev/nandd\0" \
	"mmc_root=/dev/mmcblk0p7\0" \
	"init=/init\0" \
	"loglevel=8\0" \
	"setargs_nand=setenv bootargs console=${console} root=${nand_root}" \
	"init=${init} loglevel=${loglevel} partitions=${partitions}\0" \
	"setargs_mmc=setenv bootargs console=${console} root=${mmc_root}" \
	"init=${init} loglevel=${loglevel} partitions=${partitions}\0" \
	"boot_normal=sunxi_flash read 4007f800 boot;boota 4007f800\0" \
	"boot_recovery=sunxi_flash read 4007f800 recovery;boota 4007f800\0" \
	"boot_fastboot=fastboot\0"

#define CONFIG_SUNXI_SPRITE_ENV_SETTINGS	\
	"bootdelay=0\0" \
	"bootcmd=run sunxi_sprite_test\0" \
	"console=ttyS0,115200\0" \
	"sunxi_sprite_test=sprite_test read\0"

#define CONFIG_BOOTDELAY	1
#define CONFIG_BOOTCOMMAND	"nand read 50000000 boot;boota 50000000"
#define CONFIG_SYS_BOOT_GET_CMDLINE
#define CONFIG_AUTO_COMPLETE

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP				/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER			/* use "hush" command parser	*/
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_PROMPT		"sunxi#"
#define CONFIG_SYS_CBSIZE	256			/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE	384			/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16			/* max number of command args */

/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE			CONFIG_SYS_CBSIZE


/* Serial & console */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)		/* ns16550 reg in the low bits of cpu reg */
#define CONFIG_SYS_NS16550_CLK		(24000000)
#define CONFIG_SYS_NS16550_COM1		SUNXI_UART0_BASE
#define CONFIG_SYS_NS16550_COM2		SUNXI_UART1_BASE
#define CONFIG_SYS_NS16550_COM3		SUNXI_UART2_BASE
#define CONFIG_SYS_NS16550_COM4		SUNXI_UART3_BASE
#define CONFIG_NS16550_FIFO_ENABLE	(1)
#define CONFIG_CONS_INDEX			1			/* which serial channel for console */
/* valid baudrates */
#define CONFIG_BAUDRATE				115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }


/* common config */
#define CONFIG_CMDLINE_EDITING
#define CONFIG_USE_ARCH_MEMCPY
#define CONFIG_USE_ARCH_MEMSET
#define CONFIG_DOS_PARTITION
#define CONFIG_USE_IRQ

/*
 * Stack sizes
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE        (256 << 10)	/* 256 KiB */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ    (4*1024)        /* IRQ stack */
#define CONFIG_STACKSIZE_FIQ    (4*1024)        /* FIQ stack */
#endif


#define BOOT_MOD_ENTER_STANDBY           (0)
#define BOOT_MOD_EXIT_STANDBY            (1)
#define FW_BURN_UDISK_MIN_SIZE           (2 * 1024)

/* fastboot */
#define FASTBOOT_TRANSFER_BUFFER        SDRAM_OFFSET(0x01000000)
#define FASTBOOT_TRANSFER_BUFFER_SIZE   (256 << 20)
#define FASTBOOT_ERASE_BUFFER           SDRAM_OFFSET(0x00000000)
#define FASTBOOT_ERASE_BUFFER_SIZE      (16 << 20)


#define CONFIG_OF_LIBFDT
#define CONFIG_OF_CONTROL
#define CONFIG_RELOCATE_SYSCONIFG      /*sysconfig.fex is relocate*/
#define CONFIG_ANDROID_BOOT_IMAGE      /*image is android boot image*/
#define BOARD_LATE_INIT                /* init the fastboot partitions */
#define CONFIG_SYS_LOAD_ADDR      0x50000000 /* default load address */


/****************************************************************************************
 *
 *      the fowllowing defines are used for boot0/sboot
 *
 ****************************************************************************************/
#define BOOT_PUB_HEAD_VERSION           "1100"
#define EGON_VERSION                    "1100"
#define CONFGI_SECURE_BOOT0_MAIN_VERSION    1
#define CONFGI_SECURE_BOOT0_SUB_VERSION     1

#define SUNXI_DRAM_PARA_MAX              32
#define CONFIG_SBROMSW_BASE              (CONFIG_SYS_SRAM_BASE)
#define CONFIG_HEAP_BASE                 SDRAM_OFFSET(0x00800000)
#define CONFIG_HEAP_SIZE                 (16 * 1024 * 1024)

#define CONFIG_TOC0_CONFIG_ADDR          (CONFIG_SYS_SRAM_BASE + 0x80)
#define CONFIG_TOC1_STORE_IN_DRAM_BASE   SDRAM_OFFSET(0x02e00000)

#define CONFIG_BOOT0_STACK_BOTTOM        CONFIG_NORMAL_BOOT_STACK
#define CONFIG_STACK_BASE                CONFIG_SECURE_BOOT_STACK


#endif /* _SUNXI_COMMON_CONFIG_H */
