/*
 * Copyright (c) 2015-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __PLATFORM_COMMON_DEF_H__
#define __PLATFORM_COMMON_DEF_H__

#include <arch.h>
#include <cpu.h>

/*
 * Platform binary types for linking
 */
#if defined AARCH32
#define PLATFORM_LINKER_FORMAT          "elf32-littlearm"
#define PLATFORM_LINKER_ARCH            arm
#elif defined(AARCH64)
#define PLATFORM_LINKER_FORMAT          "elf64-littleaarch64"
#define PLATFORM_LINKER_ARCH            aarch64
#elif defined(__riscv)
#define PLATFORM_LINKER_FORMAT          "elf64-littleriscv"
#define PLATFORM_LINKER_ARCH            riscv
#endif

// Get a bit field from a value
#define GetField(Var, Mask, Shift)	(((Var) >> (Shift)) & (Mask))

#define ROM_LOCATION_HSPERI_ROM 0
#define ROM_LOCATION_SPINOR1 1

#if TEST_FROM_SPINOR1
#define ROM_LOCATION ROM_LOCATION_SPINOR1
#else
#define ROM_LOCATION ROM_LOCATION_HSPERI_ROM
#endif

#ifndef __ASSEMBLY__

/*
 * These definition are used to verify struct size and offset.
 * Hard-coded value only. Do not use sizeof() or offsetof() here.
 */
// struct fip_param1->nand_info must be same as the definition in u-boot
#define NAND_INFO_OFFSET 16

struct spi_nand_info_t {
	uint32_t version;
	uint32_t id;
	uint32_t page_size;
	uint32_t spare_size;
	uint32_t block_size;
	uint32_t pages_per_block;
	uint32_t fip_block_cnt;
	uint8_t pages_per_block_shift;
	uint8_t badblock_pos;
	uint8_t dummy_data1[2];
	uint32_t flags;
	uint8_t ecc_en_feature_offset;
	uint8_t ecc_en_mask;
	uint8_t ecc_status_offset;
	uint8_t ecc_status_mask;
	uint8_t ecc_status_shift;
	uint8_t ecc_status_uncorr_val;
	uint8_t dummy_data2[2];
	uint32_t erase_count; // erase count for sys base block
	uint8_t sck_l;
	uint8_t sck_h;
	uint16_t max_freq;
	uint32_t sample_param;
	uint8_t xtal_switch;
	uint8_t dummy_data3[71];
};

struct spinor_info_t {
	uint32_t ctrl;
	uint32_t dly_ctrl;
	uint32_t tran_csr;
	uint32_t opt;
	uint32_t reserved_1;
	uint32_t reserved_2;
	uint32_t reserved_3;
	uint32_t reserved_4;
	uint32_t reserved_5;
} __packed;

struct chip_conf {
	uint32_t reg;
	uint32_t value;
} __packed;

struct fip_flags {
	struct {
		uint8_t rsa_size : 2;
		uint8_t scs : 2;
		uint8_t encrypted : 2;
		uint8_t reserved1 : 2;
	};
	uint8_t reserved2[7];
} __packed;

struct fip_param1 {
	uint64_t magic1;
	uint32_t magic2;
	uint32_t param_cksum;
	struct spi_nand_info_t nand_info;
	struct spinor_info_t spinor_info;
	struct fip_flags fip_flags;
	uint32_t chip_conf_size;
	uint32_t blcp_img_cksum;
	uint32_t blcp_img_size;
	uint32_t blcp_img_runaddr;
	uint32_t blcp_param_loadaddr;
	uint32_t blcp_param_size;
	uint32_t bl2_img_cksum;
	uint32_t bl2_img_size;
	uint32_t bld_img_size;
	uint32_t param2_loadaddr;
	uint32_t reserved1;
	struct chip_conf chip_conf[95];
	uint8_t bl_ek[32];
	uint8_t root_pk[512];
	uint8_t bl_pk[512];
	uint8_t bl_pk_sig[512];
	uint8_t chip_conf_sig[512];
	uint8_t bl2_img_sig[512];
	uint8_t blcp_img_sig[512];
} __packed __aligned(__alignof__(unsigned int));

struct blcp_param_head {
	uint32_t magic;
	uint32_t cksum;
} __packed;

#define BLCP_PARAM_MAGIC 0x52505043 // "CPPR"
#define BLCP_PARAM_MAX_SIZE 512
#define BLCP_PARAM_RETRY 4

struct bl2_head {
	uint64_t magic1;
	uint64_t magic2;
	uint32_t msid;
	uint32_t version;
	uint64_t reserved1;
} __packed;

/* this structure should be modified all of fsbl & MCU & osdrv side */
struct transfer_config_t {
	uint32_t conf_magic;
	uint32_t conf_size;  //conf_size exclude mcu_status & linux_status
	uint32_t isp_buffer_addr;
	uint32_t isp_buffer_size;
	uint32_t encode_img_addr;
	uint32_t encode_img_size;
	uint32_t encode_buf_addr;
	uint32_t encode_buf_size;
	uint8_t  dump_print_enable;
	uint8_t  dump_print_size_idx;
	uint16_t image_type;
	uint16_t checksum; // checksum exclude mcu_status & linux_status
	uint8_t  mcu_status;
	uint8_t  linux_status;
} __packed;

enum _MUC_STATUS_E {
	MCU_STATUS_NONOS_INIT = 1,
	MCU_STATUS_NONOS_RUNNING,
	MCU_STATUS_NONOS_DONE,
	MCU_STATUS_RTOS_T1_INIT,  // before linux running
	MCU_STATUS_RTOS_T1_RUNNING,
	MCU_STATUS_RTOS_T2_INIT,  // after linux running
	MCU_STATUS_RTOS_T2_RUNNING,
	MCU_STATUS_LINUX_INIT,
	MCU_STATUS_LINUX_RUNNING,
};

enum E_IMAGE_TYPE {
	E_FAST_JEPG = 1,
	E_FAST_H264,
	E_FAST_H265,
};

enum DUMP_PRINT_SIZE_E {
	DUMP_PRINT_SZ_IDX_0K = 0,
	DUMP_PRINT_SZ_IDX_4K = 12, // 4096 = 1<<12
	DUMP_PRINT_SZ_IDX_8K,
	DUMP_PRINT_SZ_IDX_16K,
	DUMP_PRINT_SZ_IDX_32K,
	DUMP_PRINT_SZ_IDX_LIMIT,
};

#define BOOT_SRC_TAG 0xCE00

// NO ZERO in boot_src
enum boot_src {
	// Read from flash
	BOOT_SRC_SPI_NAND = 0x0 | BOOT_SRC_TAG,
	BOOT_SRC_SPI_NOR = 0x2 | BOOT_SRC_TAG,
	BOOT_SRC_EMMC = 0x3 | BOOT_SRC_TAG,
	BOOT_SRC_RTC_NOR = 0x5 | BOOT_SRC_TAG,
	BOOT_SRC_PCIE = 0x7 | BOOT_SRC_TAG,

	// Download
	BOOT_SRC_SD = 0xA0 | BOOT_SRC_TAG,
	BOOT_SRC_USB = 0xA3 | BOOT_SRC_TAG,
	BOOT_SRC_UART = 0xA5 | BOOT_SRC_TAG,
};

#define DOWNLOAD_BUTTON 0x1
#define DOWNLOAD_DISABLE 0x2

union sw_info {
	uint32_t value;
	struct {
		uint32_t dis_dbg_inject : 1;
		uint32_t usb_polling_time : 1;
		uint32_t dis_uart_msg: 1;
		uint32_t reserved : 2;
		uint32_t usb_vid : 16;
		uint32_t dis_usb_rxf : 1;
		uint32_t sd_dl : 2;
		uint32_t usd_dl : 2;
		uint32_t uart_dl : 2;
		uint32_t sd_polarity : 2;
		uint32_t reset_type : 1;
		uint32_t sw_info_enable : 1;
	};
} __packed;

struct _time_records {
	uint16_t bl2_start;
	uint16_t ddr_init_start;
	uint16_t ddr_init_end;
	uint16_t blmcu_start;
	uint16_t bl31_start;
	uint16_t bl32_start;
	uint16_t uboot_start;
	uint16_t uboot_cmd_start;
	uint16_t kernel_decompress_start;
	uint16_t kernel_start;
	uint16_t kernel_run_init_start;
} __packed;

extern struct _time_records *time_records;

#endif /* __ASSEMBLY__ */

/*
 * PINMUX
 */
#define PINMUX_SPI0 11
#define PINMUX_SDIO0 22
#define PINMUX_EMMC 25
#define PINMUX_SPI_NOR 26
#define PINMUX_SPI_NAND 27

/*
 * FIP Platform defined flags
 */
#define FIP_REE_SCS_SHIFT 32
#define FIP_REE_SCS_MASK  0x03
#define FIP_REE_ENCRYPT_SHIFT 34
#define FIP_REE_ENCRYPT_MASK  0x03

/*
 * SoC memory map
 */
#define SEC_SUBSYS_BASE         0x33000000
#define SEC_CRYPTODMA_BASE      (SEC_SUBSYS_BASE)
#define SEC_FAB_FIREWALL        (SEC_SUBSYS_BASE + 0x00030000)
#define SEC_DDR_FIREWALL        (SEC_SUBSYS_BASE + 0x00040000)
#define CRYPTO_SIDE_OTP_BASE    (SEC_SUBSYS_BASE + 0x00050000)

#define SEC_SYS_BASE (SEC_SUBSYS_BASE + 0x000B0000)
#define SEC_EFUSE_BASE (SEC_SUBSYS_BASE + 0x000C0000)

#define PERI_SYS_BASE			0x27000000
#define PERI_WDT0_BASE			(PERI_SYS_BASE)
#define PERI_WDT1_BASE			(PERI_SYS_BASE + 0x1000)

#define TOP_BASE 0x028100000
#define PINMUX_BASE (TOP_BASE + 0x00001000)
#define CLKGEN_BASE (TOP_BASE + 0x00002000)
#define RST_BASE (TOP_BASE + 0x00003000)
#define WATCHDOG_BASE (TOP_BASE + 0x00010000)
#define GPIO_BASE (TOP_BASE + 0x00020000)
#define EFUSE_BASE (TOP_BASE + 0x00050000)
#define PLL_G2_BASE (TOP_BASE + 0x00002800)
#define PWM0_BASE (TOP_BASE + 0x60000)

#define G12_PINMUX_BASE	(TOP_BASE + 0x00004c00)

#define CORE_IPMUX_BASE	(TOP_BASE + 0x00005000)

#define PWM2_BASE	0x27052000
#define HLPERIOD0	0x000
#define PERIOD0		0x004
#define POLARITY	0x040
#define PWMSTART	0x044
#define PWM_OE		0x0D0

#define APSYS_BASE		0x36000000
#define GIC_BASE		(APSYS_BASE  + 0x1A000000)

#define HSPERI_BASE 0x29000000
#define SPINAND_BASE (HSPERI_BASE + 0x000D0000)
#define UART0_BASE (HSPERI_BASE + 0x00180000)
#define UART2_BASE (HSPERI_BASE + 0x001A0000)
#define USB_BASE (HSPERI_BASE + 0x00340000)
#define EMMC_BASE (HSPERI_BASE + 0x00300000)
#define SDIO_BASE (HSPERI_BASE + 0x00310000)
#define SYSDMA_BASE (HSPERI_BASE + 0x00340000)
#define SPIF_BASE 0x10000000
#define SPIF1_BASE 0x05400000

#define AXI_SRAM_BASE (TPU_SRAM_BASE + TPU_SRAM_SIZE) // 0x25050000
#define AXI_SRAM_SIZE 0x100
#define AXI_SRAM_RTOS_OFS 0x7C
#define AXI_SRAM_RTOS_BASE (AXI_SRAM_BASE + AXI_SRAM_RTOS_OFS)
#define CVI_RTOS_MAGIC_CODE 0xABC0DEF

#define BL1_VERSION_BASE	(AXI_SRAM_BASE + AXI_SRAM_SIZE)
#define BL1_VERSION_SIZE	0x40
#define BL2_VERSION_BASE	(BL1_VERSION_BASE + BL1_VERSION_SIZE)
#define BL2_VERSION_SIZE	0x40
#define BL31_VERSION_BASE	(BL2_VERSION_BASE + BL2_VERSION_SIZE)

#define MAILBOX_FIELD 0x1900400

#define C906_MAGIC_HEADER 0xA55AC906 // master cpu is c906
#define CA53_MAGIC_HEADER 0xA55ACA53 // master cpu is ca53

#define TPUSYS_REG_CLK_ENABLE	0x26050000
#define TPUSYS_TPU0_REGMEM_REGION2_CLK_ENABLE	0x26000100

#ifdef __riscv
#define RTOS_MAGIC_HEADER C906_MAGIC_HEADER
#else
#define RTOS_MAGIC_HEADER CA53_MAGIC_HEADER
#endif

#define ROM_SIZE 0x20000 // 128KiB
#define TPU_SRAM_ORIGIN_BASE 0x25000000
#ifdef CONFIG_BOARD_fpga
#define TPU_SRAM_SIZE 0x39000 // 192KiB
#else
#define TPU_SRAM_SIZE 0x50000 // 320KiB
#endif

#if ROM_LOCATION == ROM_LOCATION_HSPERI_ROM
#ifdef __riscv
	#define ROM_BASE 0x04418000 // no mirrored address for c906b
	#define TPU_SRAM_BASE TPU_SRAM_ORIGIN_BASE // no mirrored address for c906b
	#define SYSMAP_MIRROR_OFFSET 0x20000000
#else
#define ROM_BASE 0x29400000 // mirrored address
#define TPU_SRAM_BASE 0x25000000 // mirrored address
#endif
#elif ROM_LOCATION == ROM_LOCATION_SPINOR1
	#define ROM_BASE (HSPERI_BASE + 0x00400000)
	#define TPU_SRAM_BASE TPU_SRAM_ORIGIN_BASE
#else
#errro "ROM_LOCATION"
#endif

/*
 * RTC
 */
#define RTC_AHBSRAM_BASE	0x05200000
#define PM_NON_PRIMARY_CPU_HOLD	(RTC_AHBSRAM_BASE + 0x6700)

#define RTC_SYS_BASE 0x05000000
#define RTC_MACRO_BASE (RTC_SYS_BASE + 0x00026400)
#define RTC_CORE_SRAM_BASE (RTC_SYS_BASE + 0x00026800)
#define RTC_CORE_SRAM_SIZE 0x0800 // 2KB
#define RTC_IO_BASE (RTC_SYS_BASE + 0x00027000)

#define REG_RTC_CTRL_BASE (RTC_SYS_BASE + 0x00025000)
#define RTC_CTRL0_UNLOCKKEY 0x4
#define RTC_CTRL0 0x8
#define RTC_CTRL0_STATUS0 0xC
#define RTCSYS_RST_CTRL 0x18
#define RTC_FC_COARSE_EN 0x40
#define RTC_FC_COARSE_CAL 0x44
#define RTC_FC_FINE_EN 0x48
#define RTC_FC_FINE_CAL 0x50
#define RTC_POR_RST_CTRL 0xAC

#define REG_RTC_BASE (RTC_SYS_BASE + 0x00026000)
#define RTC_ANA_CALIB 0x0
#define RTC_SEC_PULSE_GEN 0x4
#define RTC_EN_PWR_WAKEUP 0xBC
#define RTC_EN_SHDN_REQ 0xC0
#define RTC_EN_PWR_CYC_REQ 0xC8
#define RTC_EN_WARM_RST_REQ 0xCC
#define RTC_EN_PWR_VBAT_DET 0xD0
#define RTC_EN_WDT_RST_REQ 0xE0
#define RTC_EN_SUSPEND_REQ 0xE4
#define RTC_PG_REG 0xF0
#define RTC_ST_ON_REASON 0xF8
#define RTC_ST_OFF_REASON 0xFC

#define RTC_INFO0 (REG_RTC_BASE + 0x1C)
#define RTC_INFO1 (REG_RTC_BASE + 0x20)
#define RTC_INFO2 (REG_RTC_BASE + 0x24)
#define RTC_INFO3 (REG_RTC_BASE + 0x28)

#define REG_RTC_ST_ON_REASON (REG_RTC_BASE + RTC_ST_ON_REASON)

#define RTCSYS_F32KLESS_BASE (RTC_SYS_BASE + 0x0002A000)

#define RTC_INTERNAL_32K 0
#define RTC_EXTERNAL_32K 1

/*
 * AXI SRAM
 */
#define EFUSE_SW_INFO_ADDR (AXI_SRAM_BASE)
#define EFUSE_SW_INFO_SIZE 4

#define BOOT_SOURCE_FLAG_ADDR (EFUSE_SW_INFO_ADDR + EFUSE_SW_INFO_SIZE)
#define BOOT_SOURCE_FLAG_SIZE 4
#define MAGIC_NUM_USB_DL 0x4D474E31 // MGN1
#define MAGIC_NUM_SD_DL 0x4D474E32 // MGN2

#define BOOT_LOG_LEN_ADDR (BOOT_SOURCE_FLAG_ADDR + BOOT_SOURCE_FLAG_SIZE) // 0x25050008
#define BOOT_LOG_LEN_SIZE 4

#define TIME_RECORDS_ADDR (AXI_SRAM_BASE + 0x10) // 0x25050010

// only for debugging
#define ATF_DBG_REG (BOOT_LOG_LEN_ADDR + BOOT_LOG_LEN_SIZE)
#define ATF_ERR_REG (ATF_DBG_REG + 0x04)
#define ATF_ERR_INFO0 (ATF_DBG_REG + 0x08)
#define CP_STATE_REG (ATF_DBG_REG + 0x0C)

#define ATF_ERR (((unsigned int __volatile__ *)ATF_ERR_REG)[0])

#define PLAT_COMMON_INFO		(SHARED_RAM_BASE)
#define PLAT_COMMON_INFO_SIZE		4

#define PLAT_BM_SRAM_FLAG		(PLAT_COMMON_INFO + PLAT_COMMON_INFO_SIZE)
#define PLAT_BM_SRAM_FLAG_SIZE		4
#define MAGIC_NUM_USB_DL		0x4D474E31 // MGN1
#define MAGIC_NUM_SD_DL			0x4D474E32 // MGN2
#define CVI_SAPD_FLAG 0x53415044

#define PLAT_BM_TRUSTED_MAILBOX_BASE	(PLAT_BM_SRAM_FLAG + PLAT_BM_SRAM_FLAG_SIZE)
#define PLAT_BM_TRUSTED_MAILBOX_SIZE	8

#define PLAT_BM_HOLD_BASE		(PLAT_BM_TRUSTED_MAILBOX_BASE + PLAT_BM_TRUSTED_MAILBOX_SIZE)
#define PLAT_BM_HOLD_SIZE		(PLATFORM_CORE_COUNT * PLAT_BM_HOLD_ENTRY_SIZE)
#define PLAT_BM_HOLD_ENTRY_SIZE		8
#define PLAT_BM_HOLD_STATE_WAIT		0
#define PLAT_BM_HOLD_STATE_GO		1

/* SHARED RAM */
#define SHARED_RAM_BASE			(AXI_SRAM_BASE + 0x40)
#define SHARED_RAM_SIZE			0x50
#define SHARED_RAM_LIMIT		(PLAT_BM_HOLD_BASE + PLAT_BM_HOLD_SIZE)

/* End of AXI SRAM */

#include <mmap.h>

/*
 * Arch timer definitions
 */
#ifdef CONFIG_BOARD_fpga
#define SYS_COUNTER_FREQ_IN_SECOND 5000000
#else
#define SYS_COUNTER_FREQ_IN_SECOND 25000000
#endif

/*
 * If enable, the global variable of emmc/sd clock could be changed by blp
 */
#define SUPPORT_SD_EMMC_CLOCK_ADJUSTMENT

/*
 * UART download
 */
#define UART_DL_MAGIC 0x5552444c // "URDL"
#define UART_DL_KERMIT_TIMEROUT 10000 // ms

/*
 * SD/EMMC definitions
 */
#define PLAT_SD_CLK 25000000
#define PLAT_EMMC_CLK 25000000

#define ENABLE_SDIO_IO_CELL_POWER
#define ENABLE_SDIO_SOURCE_SELECT_SETTING

#define EMMC_BUS_WIDTH EMMC_BUS_WIDTH_1
#define DEFAULT_DIV_EMMC_INIT_CLOCK 0x2

/*
 * USB definitions
 */
#define USB_PHY_DETECTION

/*
 * ARM-TF lives in SRAM, partition it here
 */
#if defined(NO_TPU_SRAM)
#define BL_RAM_BASE			(DRAM_BASE + 0x20000)
#else
#define BL_RAM_BASE			TPU_SRAM_BASE
#endif
#define BL_RAM_SIZE			TPU_SRAM_SIZE
#define BL_RAM_LIMIT			(BL_RAM_BASE + BL_RAM_SIZE)

/*
 * IO buffer specific defines.
 * block IO buffer's start address and size must be block size aligned
 * From 0 to 8 KiB
 */
#define BM_IO_BUF_BASE		BL_RAM_BASE
#define BM_IO_BUF_SIZE		0x2000

#define BM_EMMC_BUF_BASE	BM_IO_BUF_BASE
#define BM_EMMC_BUF_SIZE	BM_IO_BUF_SIZE
#define BM_USB_BUF_BASE		BM_IO_BUF_BASE
#define BM_USB_BUF_SIZE		BM_IO_BUF_SIZE

/*
 * BLD + DDRC specific defines.
 * From 8KiB to 72KiB
 */
#define BLD_BASE	(BM_IO_BUF_BASE + BM_IO_BUF_SIZE)
#define BLD_SIZE	0x10000

/*
 * BLD specific defines.
 * From 72KiB to 136KiB
 */
#define PLAT_FIP_1ST_BASE	(BLD_BASE + BLD_SIZE)
#define PLAT_FIP_1ST_SIZE	0x10000

/*
 * BL2 specific defines.
 * Put BL2 as the same location of (BLD + FIP_1ST)
 * From 8KiB to 188KiB
 */
#define BL2_LIMIT	(BL2_BASE + BL2_SIZE)

/*
 * Free BL_RAM
 */
#define BL_RAM_FREE_START (BL2_LIMIT)
#define BL_RAM_FREE_END (RTL_SIM_LOG_BUF)

/*
 * RTL simulation
 */
#define RTL_SIM_LOG_BUF		(BL1_RW_BASE - RTL_SIM_LOG_BUF_SIZE)
#define RTL_SIM_LOG_BUF_SIZE	0x400

/*
 * BL1 specific defines.
 * From 192KiB to 256KiB
 */
#define BL1_RW_LIMIT	(BL1_RW_BASE + BL1_RW_SIZE)

/*
 * Boot Record
 * At the end of BL_RAM
 */
#define BOOT_RECORD_BASE	(BL_RAM_LIMIT - BOOT_RECORD_SIZE)
#define BOOT_RECORD_SIZE	(4 * 8)
#define BLD_TIME_ADDR		(BOOT_RECORD_BASE + 0x00)
#define BL2_TIME_ADDR		(BOOT_RECORD_BASE + 0x04)
#define BL31_TIME_ADDR		(BOOT_RECORD_BASE + 0x08)
#define BL32_TIME_ADDR		(BOOT_RECORD_BASE + 0x0C)
#define BL33_TIME_ADDR		(BOOT_RECORD_BASE + 0x10)
#define BL1_TIME_ADDR		(BOOT_RECORD_BASE + 0x14)
#define BOOT_RECORD_R7_ADDR	(BOOT_RECORD_BASE + 0x18)
#define BOOT_RECORD_R8_ADDR	(BOOT_RECORD_BASE + 0x1C)

/*
 * BL3-1 specific defines.
 *
 * Put BL3-1 at the base of DRAM. BL31_BASE is calculated using the
 * current BL3-1 debug size plus a little space for growth.
 *
 * Linux kernel reserves first 512KiB of DRAM for ATF.
 */
#define DRAM_BL_SIZE			(128*1024)
#define BL31_BASE			DRAM_BASE
#define BL31_LIMIT			(DRAM_BASE + 0x40000)
#define BL31_PROGBITS_LIMIT		(DRAM_BASE + 0x42000)

/*
 * all sizes need to be page aligned for page table requirement.
 */
#define NS_DRAM0_BASE			(BL31_LIMIT)
#define NS_DRAM0_SIZE			(0x8000000 - 0x40000) // 256MB

#define NS_IMAGE_OFFSET			(DRAM_BASE + 0x03080000) // 48MB + 512KB

/*
 * License File specific defines.
 */
#define LICENSE_FILE_BUF_BASE		BL31_LIMIT
#define LICENSE_FILE_MAX_SIZE           0x10000

/*
 * BL3-2 specific defines.
 *
 * Put BL32 after BL3-1.
 */
#define BL32_BASE			(LICENSE_FILE_BUF_BASE + LICENSE_FILE_MAX_SIZE)
#define BL32_LIMIT			(BL32_BASE + 0x50000)

/*
 * BMSP specific defines.
 */
#define BMSP_SEC_MEM_BASE		(BL32_BASE)
#define BMSP_SEC_MEM_SIZE		(BL32_LIMIT - BL32_BASE)
#define BMSP_IRQ_SEC_PHY_TIMER		29

/*
 * BL33 specific defines.
 */
#define BL33_DECOMP_BUFFER_BASE		(DRAM_BASE + 0xa0000) // 512KB
#define BL33_LZMA_SIZE			(128 * 1024)
#define BL33_DECOMP_BUFFER_SIZE		(BL33_LZMA_SIZE + 0x01000000) // 16MB

/*
 * FIP binary defines.
 */
#define PLAT_BM_FIP_BASE		(DRAM_BASE + (8 * 0x100000)) // 8MiB at 0x80800000
#define PLAT_BM_FIP_MAX_SIZE		0x00200000 // 2 MiB

/* Special value used to verify platform parameters from BL2 to BL3-1 */
#define BM_BL31_PLAT_PARAM_VAL		0x0f1e2d3c4b5a6978ULL

#define PLATFORM_STACK_SIZE 0x2000

#ifdef CONFIG_BOARD_fpga
#define PLATFORM_MAX_CPUS_PER_CLUSTER	1
#define PLATFORM_CLUSTER_COUNT		1
#define PLATFORM_CLUSTER0_CORE_COUNT	PLATFORM_MAX_CPUS_PER_CLUSTER
#define PLATFORM_CLUSTER1_CORE_COUNT	0
#define PLATFORM_CORE_COUNT		(PLATFORM_CLUSTER0_CORE_COUNT + \
					 PLATFORM_CLUSTER1_CORE_COUNT)
#else
#define PLATFORM_MAX_CPUS_PER_CLUSTER	4
#define PLATFORM_CLUSTER_COUNT		2
#define PLATFORM_CLUSTER0_CORE_COUNT	PLATFORM_MAX_CPUS_PER_CLUSTER
#define PLATFORM_CLUSTER1_CORE_COUNT	PLATFORM_MAX_CPUS_PER_CLUSTER
#define PLATFORM_CORE_COUNT		(PLATFORM_CLUSTER0_CORE_COUNT + \
					 PLATFORM_CLUSTER1_CORE_COUNT)
#endif

#define BM_PRIMARY_CPU			0

#define PLAT_NUM_PWR_DOMAINS		(PLATFORM_CLUSTER_COUNT + \
					PLATFORM_CORE_COUNT + 1)
#define PLAT_MAX_PWR_LVL		MPIDR_AFFLVL1

#define PLAT_MAX_RET_STATE		1
#define PLAT_MAX_OFF_STATE		2

/* Local power state for power domains in Run state. */
#define PLAT_LOCAL_STATE_RUN		0
/* Local power state for retention. Valid only for CPU power domains */
#define PLAT_LOCAL_STATE_RET		1
/*
 * Local power state for OFF/power-down. Valid for CPU and cluster power
 * domains.
 */
#define PLAT_LOCAL_STATE_OFF		2

/*
 * Macros used to parse state information from State-ID if it is using the
 * recommended encoding for State-ID.
 */
#define PLAT_LOCAL_PSTATE_WIDTH		4
#define PLAT_LOCAL_PSTATE_MASK		((1 << PLAT_LOCAL_PSTATE_WIDTH) - 1)

/*
 * Some data must be aligned on the biggest cache line size in the platform.
 * This is known only to the platform as it might have a combination of
 * integrated and external caches.
 */
#define CACHE_WRITEBACK_SHIFT		6
#define CACHE_WRITEBACK_GRANULE		(1 << CACHE_WRITEBACK_SHIFT)

#define PLAT_PHY_ADDR_SPACE_SIZE	(1ull << 36)
#define PLAT_VIRT_ADDR_SPACE_SIZE	(1ull << 36)
#define MAX_MMAP_REGIONS		8
#define MAX_XLAT_TABLES			6 // varies when memory layout changes
#define MAX_IO_DEVICES			8 // FIP, MEMMAP, eMMC, SD card, SPI flash, NAND and I2C/USB download
#define MAX_IO_HANDLES			2 // FIP and one of [MEMMAP, eMMC, SPI flash, I2C/USB download]
#define MAX_IO_BLOCK_DEVICES		2 // eMMC or SPI flash

/*
 * SPI flash offset defines.
 */
#define SPIF_FIP_OFFSET			(0)

/*
 * device register defines.
 */
#define DEVICE0_BASE			0x00000000 // ITCM + DTCM
#define DEVICE0_SIZE			0x03000000 // 48MB
#define DEVICE1_BASE			0x50000000 // peripheral registers
#define DEVICE1_SIZE			0x30000000
#define DEVICE2_BASE			0x06000000 // SPI flash
#define DEVICE2_SIZE			0x00100000 // 1MB

/*
 * GIC definitions.
 */
#define PLAT_ARM_GICD_BASE		(GIC_BASE + 0x1000)
#define PLAT_ARM_GICC_BASE		(GIC_BASE + 0x2000)

#define BM_IRQ_SEC_SGI_0		8
#define BM_IRQ_SEC_SGI_1		9
#define BM_IRQ_SEC_SGI_2		10
#define BM_IRQ_SEC_SGI_3		11
#define BM_IRQ_SEC_SGI_4		12
#define BM_IRQ_SEC_SGI_5		13
#define BM_IRQ_SEC_SGI_6		14
#define BM_IRQ_SEC_SGI_7		15
#define BM_IRQ_SEC_PHY_TIMER		29

#define PLAT_ARM_G1S_IRQS		BM_IRQ_SEC_PHY_TIMER, \
					BM_IRQ_SEC_SGI_1, \
					BM_IRQ_SEC_SGI_2, \
					BM_IRQ_SEC_SGI_3, \
					BM_IRQ_SEC_SGI_4, \
					BM_IRQ_SEC_SGI_5, \
					BM_IRQ_SEC_SGI_7
#define PLAT_ARM_G0_IRQS		BM_IRQ_SEC_SGI_0, \
					BM_IRQ_SEC_SGI_6

/*
 * UART definitions
 */
#define PLAT_BOOT_UART_BASE		UART0_BASE
#define PLAT_CRASH_UART_BASE		UART0_BASE

/*
 * TOP registers.
 */
/*
 * TOP registers.
 */
#define REG_TOP_CHIPID (TOP_BASE + 0x0)
#define REG_TOP_CONF_INFO (TOP_BASE + 0x4)
#define REG_TOP_USB_PHY_CTRL (TOP_BASE + 0x48)

#define BIT_C906L_BOOT_FROM_RTCSYS_EN BIT(6)

#define REG_GP_REG0 (TOP_BASE + 0x80)
#define REG_GP_REG1 (TOP_BASE + 0x84)
#define REG_GP_REG2 (TOP_BASE + 0x88) // Trig simulation bench to increse cntpct_el0
#define REG_GP_REG3 (TOP_BASE + 0x8C)

#define REG_USB_ECO_REG (TOP_BASE + 0xB4)
#define REG_USB_ECO_RXF 0x80

#define REG_CLK_BYPASS_SEL_REG (CLKGEN_BASE + 0x30)
#define REG_CLK_DIV0_CTL_CA53_REG (CLKGEN_BASE + 0x40)
#define REG_CLK_DIV0_CTL_CPU_AXI0_REG (CLKGEN_BASE + 0x48)
#define REG_CLK_DIV0_CTL_TPU_AXI_REG (CLKGEN_BASE + 0x54)

#define REG_CLK_DIV_AXI4 (CLKGEN_BASE + 0xB8)

#define REG_PLL_G2_CTRL (PLL_G2_BASE + 0x0)
#define REG_APLL0_CSR (PLL_G2_BASE + 0x0C)
#define REG_DISPPLL_CSR (PLL_G2_BASE + 0x10)
#define REG_CAM0PLL_CSR (PLL_G2_BASE + 0x14)
#define REG_CAM1PLL_CSR (PLL_G2_BASE + 0x18)
#define REG_PLL_G2_SSC_SYN_CTRL (PLL_G2_BASE + 0x40)
#define REG_APLL_SSC_SYN_CTRL (PLL_G2_BASE + 0x50)
#define REG_APLL_SSC_SYN_SET (PLL_G2_BASE + 0x54)
#define REG_DISPPLL_SSC_SYN_CTRL (PLL_G2_BASE + 0x60)
#define REG_DISPPLL_SSC_SYN_SET (PLL_G2_BASE + 0x64)
#define REG_CAM0PLL_SSC_SYN_CTRL (PLL_G2_BASE + 0x70)
#define REG_CAM0PLL_SSC_SYN_SET (PLL_G2_BASE + 0x74)
#define REG_CAM1PLL_SSC_SYN_CTRL (PLL_G2_BASE + 0x80)
#define REG_CAM1PLL_SSC_SYN_SET (PLL_G2_BASE + 0x84)

#define SHIFT_TOP_USB_ID 8
#define SHIFT_TOP_USB_VBUS 9
#define BIT_TOP_USB_ID BIT(SHIFT_TOP_USB_ID)
#define BIT_TOP_USB_VBUS BIT(SHIFT_TOP_USB_VBUS)

#define REG_TOP_SD_PWRSW_CTRL (TOP_BASE + 0x1F4)
#define REG_TOP_SD_CTRL_OPT (TOP_BASE + 0x294)
#define BIT_IO_TRAP_SD0_PWR_DIN BIT(27)
#define BIT_SD0_PWR_EN_POLARITY BIT(16)
#define BIT_SD1_PWR_EN_POLARITY BIT(17)

#define PCIE_BOOT_REG 0x281000b0

/*
 * DEBUG register
 */
#define ATF_STATE_REG REG_GP_REG1
#define ATF_STATE (((unsigned int *)ATF_STATE_REG)[0])

#define ATF_WAIT_DEBUG_REG REG_GP_REG0
#define ATF_WAIT_DEBUG_MAGIC 0x6526228C
#define ATF_WAIT_DEBUG_TIMEOUT 1000

/*
 * NCORE register
 */
#define NCORE_BASE			0x50200000
#define NCORE_DIRUCASER0	0x80040
#define NCORE_DIRUMRHER		0x80070
#define NCORE_DIRUSFER		0x80010
#define NCORE_CSADSER		0xff040

/*
 * Check RAM limitation
 */
#if SHARED_RAM_LIMIT > (SHARED_RAM_BASE + SHARED_RAM_SIZE)
#error "No enough SHARED_RAM"
#endif

#if BL_RAM_FREE_END < BL_RAM_FREE_START
#error "No enough BL_RAM"
#endif

/*
 * Platform specific image desc.
 */

#define DDRC_IMAGE_DESC {				\
	.image_id = CV_DDRC_IMAGE_ID,			\
	SET_STATIC_PARAM_HEAD(image_info, PARAM_EP,	\
		VERSION_1, image_info_t, 0),		\
	.image_info.image_base = BLD_BASE,		\
	SET_STATIC_PARAM_HEAD(ep_info, PARAM_EP,	\
		VERSION_1, entry_point_info_t, SECURE),	\
	.ep_info.pc = BLD_BASE,				\
}

#define BLPARAM_IMAGE_DESC {				\
	.image_id = CV_BLPARAM_IMAGE_ID,		\
	SET_STATIC_PARAM_HEAD(image_info, PARAM_EP,	\
		VERSION_1, image_info_t, 0),		\
	.image_info.image_base = BLD_BASE,		\
	SET_STATIC_PARAM_HEAD(ep_info, PARAM_EP,	\
		VERSION_1, entry_point_info_t, SECURE),	\
	.ep_info.pc = BLD_BASE,				\
}

#ifndef __ASSEMBLY__
int plat_is_forcing_device_memory(void);
int plat_is_download_enabled(void);
void bm_ncore_init(uintptr_t ncore_base);
void bm_ncore_enable_cluster(int en);
#endif

#endif /* __PLATFORM_COMMON_DEF_H__ */
