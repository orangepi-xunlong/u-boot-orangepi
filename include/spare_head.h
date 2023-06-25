
/*
 * (C) Copyright 2018
 * wangwei <wangwei@allwinnertech.com>
 * SPDX-License-Identifier: GPL-2.0+
 */


#ifndef  __spare_head_h__
#define  __spare_head_h__

/* work mode */
#define WORK_MODE_PRODUCT      (1<<4)
#define WORK_MODE_UPDATE       (1<<5)

#define WORK_MODE_BOOT          0x00	/*normal boot mode */
#define WORK_MODE_USB_PRODUCT   0x10	/*usb product mode */
#define WORK_MODE_CARD_PRODUCT  0x11	/*card burn mode */
#define WORK_MODE_USB_DEBUG     0x12    /*some test mode by usb efex protocol */
#define WORK_MODE_SPRITE_RECOVERY 0x13	/*update firmware from internal backup part */
#define WORK_MODE_CARD_UPDATE   0x14	/*update firmware from sdcard */
#define WORK_MODE_USB_UPDATE    0x20    /*usb update mode */
#define WORK_MODE_UDISK_UPDATE    0x15
#define WORK_MODE_OUTER_UPDATE  0x21

#define WORK_MODE_USB_TOOL_PRODUCT  0x04
#define WORK_MODE_USB_TOOL_UPDATE   0x08
#define WORK_MODE_ERASE_KEY         0x20

#define UBOOT_MAGIC             "uboot"
#define STAMP_VALUE             0x5F0A6C39
#define ALIGN_SIZE              16 * 1024
#define MAGIC_SIZE              8
#define STORAGE_BUFFER_SIZE    (256)

#define PARAMETER_MAGIC			"para"
#define BOOT_PARAMETER_SIZE		(4094*2)

#define SUNXI_UPDATE_NEXT_ACTION_NORMAL			(1)
#define SUNXI_UPDATE_NEXT_ACTION_REBOOT			(2)
#define SUNXI_UPDATE_NEXT_ACTION_SHUTDOWN		(3)
#define SUNXI_UPDATE_NEXT_ACTION_REUPDATE		(4)
#define SUNXI_UPDATE_NEXT_ACTION_BOOT			(5)
#define SUNXI_UPDATA_NEXT_ACTION_SPRITE_TEST    (6)

#define SUNXI_DEBUG_MODE_FLAG           (0x59)
#define SUNXI_EFEX_CMD_FLAG             (0x5A)
#define SUNXI_BOOT_RESIGNATURE_FLAG     (0x5B)
#define SUNXI_BOOT_RECOVERY_FLAG        (0x5C)
#define SUNXI_SYS_RECOVERY_FLAG         (0x5D)
#define SUNXI_USB_RECOVERY_FLAG         (0x5E)
#define SUNXI_FASTBOOT_FLAG             (0x5F)
#define SUNXI_UBOOT_FLAG                (0x60)
#define SUNXI_CHARGING_FLAG		(0x61)

#define SUNXI_VBUS_UNKNOWN                      (0)
#define SUNXI_VBUS_EXIST                        (1)
#define SUNXI_VBUS_NOT_EXIST                    (2)

#define BOOT0_SDMMC_START_ADDR                  (16)

#define BOOT0_EMMC3_START_ADDR                  (384)
#define BOOT0_EMMC3_BACKUP_START_ADDR           (512)


#define UBOOT_START_SECTOR_IN_SDMMC             (32800)
#define UBOOT_BACKUP_START_SECTOR_IN_SDMMC      (24576)


#define SUNXI_NORMAL_MODE                            0
#define SUNXI_SECURE_MODE_WITH_SECUREOS              1
#define SUNXI_SECURE_MODE_NO_SECUREOS                2

#define SUNXI_RUN_CRASHDUMP_RESET_FLAG                (0x5AA55AA5)
#define SUNXI_RUN_CRASHDUMP_RESET_READY               (0x5AA55AA6)
#define SUNXI_RUN_CRASHDUMP_REFRESH_READY             (0x5AA55AA7)

#define SUNXI_DTB_OFFSET (2<<20)
#define SUNXI_DTBO_OFFSET (1<<20)

typedef enum _SUNXI_BOOT_FILE_MODE
{
	SUNXI_BOOT_FILE_NORMAL =0,
	SUNXI_BOOT_FILE_TOC = 1,
	SUNXI_BOOT_FILE_RES0 = 2,
	SUNXI_BOOT_FILE_RES1 = 3,
	SUNXI_BOOT_FILE_PKG = 4
}SUNXI_BOOT_FILE_MODE;


#define   BOOT_FROM_SD0      0
#define   BOOT_FROM_NFC      1
#define   BOOT_FROM_SD2      2
#define   BOOT_FROM_SPI_NOR  3
#define   BOOT_FROM_SPI_NAND 4


#define SUNXI_SECURE_MODE_USE_SEC_MONITOR        1
#define	TOC_ITEM_ENTRY_STATUS_ENCRYP_NOT_USED   0x00
#define	TOC_ITEM_ENTRY_STATUS_ENCRYP_USED       0x01

#define	TOC_ITEM_ENTRY_TYPE_NULL                0x00
#define	TOC_ITEM_ENTRY_TYPE_KEY_CERTIF          0x01
#define	TOC_ITEM_ENTRY_TYPE_BIN_CERTIF          0x02
#define	TOC_ITEM_ENTRY_TYPE_BIN                 0x03
#define TOC_ITEM_ENTRY_TYPE_LOGO                0x04

typedef struct _normal_gpio_cfg
{
    char      port;            /* port : PA/PB/PC ... */
    char      port_num;        /* internal port num: PA00/PA01 ... */
    char      mul_sel;         /* function num: input/output/io-disalbe ...*/
    char      pull;            /* pull-up/pull-down/no-pull */
    char      drv_level;       /* driver level: level0-3*/
    char      data;            /* pin state when the port is configured as input or output*/
    char      reserved[2];
}
normal_gpio_cfg;

typedef struct _special_gpio_cfg
{
	unsigned char port;      /* port : PA/PB/PC ... */
	unsigned char port_num;  /* intern port num: PA00/PA01 .. */
	char          mul_sel;   /* function num: input/output/io-disalbe ...*/
	char          data;      /* pin state when the port is configured as input or output*/
}special_gpio_cfg;

typedef struct sdcard_spare_info_t
{
	int card_no[4];         /*controller num*/
	int speed_mode[4];      /*speed mode: 0--low speed, other--high speed */
	int line_sel[4];        /*line mode: 1/4/8*/
	int line_count[4];      /*line count*/
}
sdcard_spare_info;

typedef enum
{
	STORAGE_NAND =0,
	STORAGE_SD,
	STORAGE_EMMC,
	STORAGE_NOR,
	STORAGE_EMMC3,
	STORAGE_SPI_NAND,
	STORAGE_SD1,
	STORAGE_EMMC0,
}SUNXI_BOOT_STORAGE;

#endif

