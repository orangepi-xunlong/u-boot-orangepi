
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

#include <asm/arch/platform.h>

#undef DEBUG

#ifndef __KERNEL__
#define __KERNEL__
#endif

//#define DEBUG

//#define FPGA_PLATFORM
#define LINUX_MACHINE_ID        4137

#define UBOOT_VERSION			"3.0.0"
#define UBOOT_PLATFORM		    "1.0.0"

#define CONFIG_MACH_TYPE  0xffffffff
#define CONFIG_TARGET_NAME      sun50iw2p1


#define CONFIG_STORAGE_MEDIA_NAND
#define CONFIG_STORAGE_MEDIA_MMC
#define CONFIG_SUNXI_MULITCORE_BOOT
#define CONFIG_LZMA
#define CONFIG_SUNXI_CORE_VOL         1260
#define CONFIG_BOOT0_CPU1_STACK_SIZE  0x1000

#define CONFIG_SYS_GENERIC_BOARD


#define HAVE_VENDOR_COMMON_LIB
/*
 * High Level Configuration Options
 */
//#define CONFIG_SUNXI_WINE_FPGA
#define CONFIG_ALLWINNER			/* It's a Allwinner chip */
#define	CONFIG_SUNXI				/* which is sunxi family */
#define CONFIG_ARCH_SUN50IW2P1
#define CONFIG_ARM_A53

#define CONFIG_SUNXI_USER_KEY
#define CONFIG_SUNXI_PRIVATE_KEY
#define CONFIG_SUNXI_SECURE_STORAGE
#define CONFIG_SUNXI_SECURE_SYSTEM

#define SUNXI_SBOOT_VERSION_NO_UPDATE

#define CONFIG_SUNXI_HDCP_IN_SECURESTORAGE
#define CONFIG_SUNXI_MULTI_POWER_MODE


#define CONFIG_SYS_SRAM_BASE               (0x10000)
#define CONFIG_SYS_SRAMA1_BASE             (0x10000)
#define CONFIG_SYS_SRAMA1_SIZE             (0x8000)
#define CONFIG_SYS_SRAMA2_BASE             (0x40000)
#define CONFIG_SYS_SRAMA2_SIZE             (0x14000)
#define CONFIG_SYS_SRAMC_BASE              (0x18000)
#define CONFIG_SYS_SRAMC_END               (0x34000)

#define PLAT_SDRAM_BASE                      0x40000000
//trusted dram area
#define PLAT_TRUSTED_DRAM_BASE             (PLAT_SDRAM_BASE+(128<<20))
#define PLAT_TRUSTED_DRAM_SIZE             0x01000000
//uboot run addr
#define CONFIG_SYS_TEXT_BASE                 0x4A000000
//book pkg addr
#define CONFIG_BOOTPKG_STORE_IN_DRAM_BASE   (CONFIG_SYS_SDRAM_BASE + 0x2e00000)

//toco mmu
#define TOC0_MMU_BASE_ADDRESS            (CONFIG_SYS_SDRAM_BASE + 0x2800000)
//boot0 stack, in sramc, the high 4K for cpu1 stack.
#define CONFIG_BOOT0_STACK_BOTTOM        (CONFIG_SYS_SRAMC_END-CONFIG_BOOT0_CPU1_STACK_SIZE)

//dram base for uboot
#define CONFIG_SYS_SDRAM_BASE                (PLAT_SDRAM_BASE)
//base addr for boot 0 to load the fw image
#define BL31_BASE                            (PLAT_TRUSTED_DRAM_BASE) //bl31:0x40000000-0x40200000
#define BL31_SIZE                            (0x100000)  //1M
#define SCP_SRAM_BASE                        (CONFIG_SYS_SRAMA2_BASE)
#define SCP_SRAM_SIZE                        (CONFIG_SYS_SRAMA2_SIZE)
#define SCP_DRAM_BASE                        (PLAT_TRUSTED_DRAM_BASE+BL31_SIZE)
#define SCP_DRAM_SIZE                        (0x4000)

//fdt addr for kernel
#define CONFIG_SUNXI_FDT_ADDR                (CONFIG_SYS_SDRAM_BASE+0x04000000)
//parameter for boot
#define CONFIG_SUNXI_PARAMETER_ADDR			 (CONFIG_SYS_SDRAM_BASE+0x03F00000)
//serial number
#define CONFIG_SUNXI_SERIAL
// the sram base address, and the stack address in stage1
#define CONFIG_SYS_INIT_RAM_ADDR	     0x10000
#define CONFIG_SYS_INIT_RAM_SIZE	     0x00007ff0

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

//for usb efex -- tools private data
#define SRAM_AREA_A		             CONFIG_SYS_INIT_RAM_ADDR

#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM_1				CONFIG_SYS_SDRAM_BASE	/* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE			(512 << 20)				/* 0x20000000, 512 MB Bank #1 */

#define CONFIG_NONCACHE_MEMORY
#define CONFIG_NONCACHE_MEMORY_SIZE (16 * 1024 * 1024)
/*
 * define malloc space
 * Size of malloc() pool
 * 1MB = 0x100000, 0x100000 = 1024 * 1024
 */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (128 << 20))



#define FASTBOOT_TRANSFER_BUFFER        (CONFIG_SYS_SDRAM_BASE + 0x01000000)
#define FASTBOOT_TRANSFER_BUFFER_SIZE   (256 << 20)

#define FASTBOOT_ERASE_BUFFER           (CONFIG_SYS_SDRAM_BASE)
#define FASTBOOT_ERASE_BUFFER_SIZE      (16 << 20)

/* SMP Definitions */
#define CPU_RELEASE_ADDR		CONFIG_SYS_INIT_SP_ADDR
/* Generic Timer Definitions */
#define COUNTER_FREQUENCY		0x18000000	/* 24MHz */
/*
* define all parameters
*/
#define FEL_BASE                         (0x20)
#define SUNXI_RUN_EFEX_FLAG              (0x5AA5A55A)
#define SUNXI_RUN_EFEX_ADDR              (0x01f00000 + 0x108)
#define DRAM_PARA_STORE_ADDR             (CONFIG_SYS_SDRAM_BASE + 0x00800000)
#define SYS_CONFIG_MEMBASE               (CONFIG_SYS_SDRAM_BASE + 0x00010000)
#define CONFIG_RELOCATE_SYSCONIFG

#define CONFIG_RELOCATE_PARAMETER

//#define CONFIG_SUNXI_LOGBUFFER
#define SUNXI_DISPLAY_FRAME_BUFFER_ADDR  (CONFIG_SYS_SDRAM_BASE + 0x06400000)
#define SUNXI_DISPLAY_FRAME_BUFFER_SIZE  0x01000000

#define CONFIG_SUNXI_ESM_HDCP
#define SUNXI_ESM_IMG_SIZE_ADDR                                 (0x42d00000)
#define SUNXI_ESM_IMG_BUFF_ADDR                                 (0x42d00000 + 16)

#define SUNXI_LOGO_COMPRESSED_LOGO_SIZE_ADDR        		(0x43000000)
#define SUNXI_LOGO_COMPRESSED_LOGO_BUFF     			(0x43000000 + 16)
#define SUNXI_SHUTDOWN_CHARGE_COMPRESSED_LOGO_SIZE_ADDR  	(0x43100000)
#define SUNXI_SHUTDOWN_CHARGE_COMPRESSED_LOGO_BUFF  		(0x43100000 + 16)
#define SUNXI_ANDROID_CHARGE_COMPRESSED_LOGO_SIZE_ADDR   	(0x43200000)
#define SUNXI_ANDROID_CHARGE_COMPRESSED_LOGO_BUFF   		(0x43200000 + 16)

/*
* define const value
*/
#define BOOT_USB_DETECT_DELAY_TIME       (1000)
#define  FW_BURN_UDISK_MIN_SIZE		     (2 * 1024)

#define BOOT_MOD_ENTER_STANDBY           (0)
#define BOOT_MOD_EXIT_STANDBY            (1)

#define MEMCPY_TEST_DST                  (CONFIG_SYS_SDRAM_BASE)
#define MEMCPY_TEST_SRC                  (CONFIG_SYS_SDRAM_BASE + 0x06000000)


//boot0 fes --start
#define BOOT_PUB_HEAD_VERSION           "1100"
#define EGON_VERSION                    "1100"
#define SUNXI_DRAM_PARA_MAX              32
#define CONFIG_BOOT0_RET_ADDR            (CONFIG_SYS_SRAM_BASE)
#define CONFIG_BOOT0_RUN_ADDR            (0x10000)
#define CONFIG_FES1_RET_ADDR             (0x17210)
#define CONFIG_FES1_RUN_ADDR             (0x18000)
#define CONFIG_SUNXI_CHIPID
//boot0 fes --end

/****************************************************************************************/
/*																						*/
/*      the fowllowing defines are used in sbrom                                        */
/*																						*/
/****************************************************************************************/
#define CONFIG_SBROMSW_BASE              (CONFIG_SYS_SRAM_BASE)
#define CONFIG_PRINT_SIZE                (8*1024)
#define CONFIG_STACK_BASE                (CONFIG_SYS_SRAMC_END-CONFIG_BOOT0_CPU1_STACK_SIZE -CONFIG_PRINT_SIZE)
#define CONFIG_BOOT0_CPU1_STACK_BOTTOM   (CONFIG_SYS_SRAMC_END-CONFIG_PRINT_SIZE)
#define CONFIG_DEBUG_BASE                (CONFIG_BOOT0_CPU1_STACK_BOTTOM)
#define CONFIG_NORMAL_DEBUG_BASE         (CONFIG_SYS_SRAMC_BASE)
#define CONFIG_HEAP_BASE                 (CONFIG_SYS_SDRAM_BASE + 0x800000)
#define CONFIG_HEAP_SIZE                 (16 * 1024 * 1024)
#define CONFIG_TOC0_RET_ADDR             (0)
#define CONFIG_TOC0_RUN_ADDR             (0x10480)
#define CONFIG_TOC0_CONFIG_ADDR          (CONFIG_SBROMSW_BASE + 0x80)
#define CONFIG_TOC1_STORE_IN_DRAM_BASE   (CONFIG_SYS_SDRAM_BASE + 0x2e00000)
#define SUNXI_FEL_ADDR_IN_SECURE         (0x00000064)
/****************************************************************************************/
/*																						*/
/*      all the defines are finished                                                    */
/*																						*/
/****************************************************************************************/


//#define SUNXI_DMA_LINK_NULL       (0x1ffff800)

#define CONFIG_USE_ARCH_MEMCPY
#define CONFIG_USE_ARCH_MEMSET
/*
 * Display CPU and Board information
 */
//#define CONFIG_DISPLAY_CPUINFO
//#define CONFIG_DISPLAY_BOARDINFO
#undef CONFIG_DISPLAY_CPUINFO
#undef CONFIG_DISPLAY_BOARDINFO

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


#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG
#define CONFIG_CMDLINE_EDITING


/*
 * Reducing the ARP timeout from default 5000UL to 1000UL we speed up the
 * initial TFTP transfer or PING, etc, should the user wish one, significantly.
 */
#define CONFIG_ARP_TIMEOUT	1000UL


/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP				/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER			/* use "hush" command parser	*/
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_PROMPT		"sunxi#"
#define CONFIG_SYS_CBSIZE	256			/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE	384			/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	25			/* max number of command args */

/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE			CONFIG_SYS_CBSIZE

/* memtest works on */
#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		((CONFIG_SYS_SDRAM_BASE + 256)<<20)	/* 256M */
#define CONFIG_SYS_LOAD_ADDR		0x50000000					/* default load address */

#define CONFIG_SYS_HZ				1000

/* valid baudrates */
#define CONFIG_BAUDRATE				115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

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

#define CONFIG_DOS_PARTITION
#define CONFIG_USE_IRQ
#define CONFIG_SUNXI_DMA
#define CONFIG_OF_LIBFDT
#define CONFIG_OF_CONTROL
#define CONFIG_ANDROID_BOOT_IMAGE      /*image is android boot image*/
#define CONFIG_USBD_HS
#define BOARD_LATE_INIT		      /* init the fastboot partitions */
#define CONFIG_SUNXI_KEY_BURN

#define CONFIG_SUNXI_I2C_NULL
//#define CONFIG_SUNXI_I2C
//#define CONFIG_CPUS_I2C
#define CONFIG_AXP_USE_I2C
#define CONFIG_SYS_I2C_SPEED 400000
#define CONFIG_SYS_I2C_SLAVE 0x36

#define CONFIG_SUNXI_KEY_SUPPORT
#define CONFIG_SUNXI_ARISC_EXIST
#define CONFIG_SUNXI_SID_SECURITY_STATUS
/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE			(256 << 10)				/* 256 KiB */
#define LOW_LEVEL_SRAM_STACK		0x00013FFC
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ    (4*1024)        /* IRQ stack */
#define CONFIG_STACKSIZE_FIQ    (4*1024)        /* FIQ stack */
#endif


/***************************************
 *module support: sdmmc&&nand platform | spinor platform
 ****************************************/

#define CONFIG_SUNXI_MODULE_SPRITE
#define CONFIG_SUNXI_MODULE_NAND
#define CONFIG_SUNXI_MODULE_SDMMC
#define CONFIG_SUNXI_MODULE_AXP
#define CONFIG_SUNXI_MODULE_USB
#define CONFIG_SUNXI_MODULE_DISPLAY
#define CONFIG_SUNXI_MODULE_HDMI
#define CONFIG_SUNXI_MODULE_TV
#define CONFIG_SUNXI_MODULE_CLK
#define CONFIG_SUNXI_MODULE_PWM



/***************************************************************
*
* all the config command
*
***************************************************************/


#define CONFIG_CMD_BOOTA		/* boot android image */
#define CONFIG_CMD_RUN			/* run a command */
#define CONFIG_CMD_BOOTD		/* boot the default command */
#define CONFIG_CMD_FDT

#define CONFIG_CMD_FAT			/* with this we can access bootfs in nand */
#define CONFIG_CMD_IRQ
#define CONFIG_CMD_ELF
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_FASTBOOT
#define CONFIG_CMD_SUNXI_SPRITE
#define CONFIG_CMD_SUNXI_TIMER
#define CONFIG_CMD_SUNXI_EFEX
#define CONFIG_CMD_SUNXI_SHUTDOWN
#define CONFIG_CMD_SUNXI_BMP
#ifdef CONFIG_SUNXI_KEY_BURN
#define CONFIG_CMD_SUNXI_BURN
#endif
#define CONFIG_CMD_SUNXI_MEMTEST
#define CONFIG_SUNXI_CMD_SMC


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
//#define CONFIG_CMD_NAND                         /* NAND support */
#define CONFIG_SYS_MAX_NAND_DEVICE      1
#define CONFIG_SYS_NAND_BASE            0x00
#endif



#ifdef CONFIG_SUNXI_MODULE_DISPLAY
#define CONFIG_SUNXI_DISPLAY
#define CONFIG_VIDEO_SUNXI_V3
#define ENABLE_ADVERT_PICTURE
#define CONFIG_BOOT_GUI
#ifdef CONFIG_BOOT_GUI
#define UPDATE_DISPLAY_MODE
#endif
#endif

#define PMU_SCRIPT_NAME                 "charger0"
#define FDT_PATH_REGU                   "regulator0"
#ifdef CONFIG_SUNXI_MODULE_AXP
#define CONFIG_SUNXI_AXP
#define CONFIG_SUNXI_AXP806
#define CONFIG_SUNXI_AXP_CONFIG_ONOFF
#endif

#ifdef CONFIG_SUNXI_MODULE_SPINOR
#define CONFIG_SUNXI_SPI
#define CONFIG_SUNXI_SPINOR
#define CONFIG_SPINOR_LOGICAL_OFFSET        ((512 - 16) * 1024/512)
#define UBOOT_START_SECTOR_IN_SPINOR        (24*1024/512)
#define SPINOR_STORE_BUFFER_SIZE            (2<<20)
#endif

#ifdef CONFIG_SUNXI_MODULE_USB

//#define CONFIG_USB_ETHER
#ifdef CONFIG_USB_ETHER
/* USB SUSPORT */

#define CONFIG_USB_ETH_RNDIS
#define CONFIG_USB_SUNXI_UDC0
#define CONFIG_USB_GADGET_DUALSPEED

/* net support */
#define CONFIG_CMD_NET
#define CONFIG_NET_MULTI
#define CONFIG_CMD_PING
#define CONFIG_CMD_NFS
#endif

#endif

//#define CONFIG_SYS_DCACHE_OFF

/* config for homlet platform */
#define CONFIG_ARCH_HOMELET
#define CONFIG_BOX_STANDBY
#define CONFIG_BOOT_PARAMETER

#define CONFIG_SUNXI_IR
#define CONFIG_SUNXI_IR_NEC_DECODE
#define CONFIG_IR_BOOT_RECOVERY
#define IR_BASE			(0x01f02000)


#define USE_BOARD_CONFIG

/* net support */
//#define CONFIG_SUNXI_GETH
#ifdef CONFIG_SUNXI_GETH
//#define CONFIG_SUNXI_EXT_PHY
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_MII
//#define CONFIG_ETHADDR 	72:D6:05:4F:B9:3B
//#define CONFIG_IPADDR   	192.168.200.254
//#define CONFIG_SERVERIP      	192.168.200.20
//#define CONFIG_NETMASK       	255.255.255.0
//#define CONFIG_GATEWAYIP     	192.168.200.1
#endif

#endif /* __CONFIG_H */
