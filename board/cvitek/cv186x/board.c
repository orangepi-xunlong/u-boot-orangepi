// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2013
 * David Feng <fenghua@phytium.com.cn>
 * Sharma Bhupesh <bhupesh.sharma@freescale.com>
 *
 */
#include <common.h>
#include <dm.h>
#include <malloc.h>
#include <errno.h>
#include <asm/io.h>
#include <console.h>
#include <linux/compiler.h>
#if defined(__aarch64__)
#include <asm/armv8/mmu.h>
#endif
#include <usb/dwc2_udc.h>
#include <usb.h>
#include "cv186x_reg.h"
#include "mmio.h"
#include "pinmux/cv186x_pinmux.h"
#include <linux/delay.h>
#include <bootstage.h>
#include <version.h>
#include <command.h>
#include <mmc.h>
#include <net.h>
#include <serial.h>

#ifdef CONFIG_VIDEO_SOPH
#include <video_soph.h>
#endif

#include <linux/stringify.h>

#if defined(__riscv)
#include <asm/csr.h>
#endif

#define DTSNAME_MAX_LEN 32
void get_dts_type_from_oem(unsigned char *dtsname);
void get_dts_type_from_sram(unsigned char *dtsname);
u64 get_ddr_size_from_sram(void);
DECLARE_GLOBAL_DATA_PTR;
#define SD1_SDIO_PAD

#if defined(__aarch64__)
static struct mm_region cv186x_mem_map[] = {
	{
		.virt = 0x0UL,
		.phys = 0x0UL,
		.size = 0xF0000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	}, {
		.virt = PHYS_SDRAM_1,
		.phys = PHYS_SDRAM_1,
		.size = PHYS_SDRAM_1_SIZE,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	}, {
		/* List terminator */
		0,
	}
};

struct mm_region *mem_map = cv186x_mem_map;
#endif

void pinmux_config(int io_type)
{
		switch (io_type) {
		case PINMUX_UART0:
			PINMUX_CONFIG(UART0_TX, UART0_TX, G11);
			PINMUX_CONFIG(UART0_RX, UART0_RX, G11);
		break;
		case PINMUX_SDIO0:
			/* set pwr switch pin for gpio */
			PINMUX_CONFIG(PAD_VIVO0_D15, GPIO133, G5);
			PINMUX_CONFIG(SD0_CLK, SD0_CLK, G8);
			PINMUX_CONFIG(SD0_CMD, SD0_CMD, G8);
			PINMUX_CONFIG(SD0_D0, SD0_D0, G8);
			PINMUX_CONFIG(SD0_D1, SD0_D1, G8);
			PINMUX_CONFIG(SD0_D2, SD0_D2, G8);
			PINMUX_CONFIG(SD0_D3, SD0_D3, G8);
			//PINMUX_CONFIG(SD0_CD_X, GPIO53, G9);
			PINMUX_CONFIG(SD0_CD_X, SD0_CD_X, G9);
			PINMUX_CONFIG(SD0_PWR_EN, SD0_PWR_EN, G9);
			break;
		case PINMUX_SDIO1:
			/* set pwr switch pin for gpio */
			PINMUX_CONFIG(PAD_VIVO0_D16, GPIO134, G5);
			PINMUX_CONFIG(SD1_CLK, SD1_CLK, G11);
			PINMUX_CONFIG(SD1_CMD, SD1_CMD, G11);
			PINMUX_CONFIG(SD1_D0, SD1_D0, G11);
			PINMUX_CONFIG(SD1_D1, SD1_D1, G11);
			PINMUX_CONFIG(SD1_D2, SD1_D2, G11);
			PINMUX_CONFIG(SD1_D3, SD1_D3, G11);
			//PINMUX_CONFIG(SD1_CD_X, GPIO61, G12);
			PINMUX_CONFIG(SD1_CD_X, SD1_CD_X, G12);
			PINMUX_CONFIG(SD1_PWR_EN, SD1_PWR_EN, G12);
			break;
		case PINMUX_SDIO2:
#ifdef	SD2_SDIO_PWM
			/* 0:sd2 ctrl conncect to PWM and FAN IOs */
			mmio_clrsetbits_32(TOP_BASE + 0x294, 0x1 << 26, 0);
			PINMUX_CONFIG(PWM0, SD2_0_CLK, G9);
			PINMUX_CONFIG(PWM1, SD2_0_CMD, G9);
			PINMUX_CONFIG(PWM2, SD2_0_D0, G9);
			PINMUX_CONFIG(PWM3, SD2_0_D1, G9);
			PINMUX_CONFIG(FAN0, SD2_0_D2, G9);
			PINMUX_CONFIG(FAN1, SD2_0_D3, G9);
#else
			mmio_clrsetbits_32(TOP_BASE + 0x294, 0x1 << 26, 0x1 << 26);
			mmio_clrsetbits_32(0x28104c00, 0xf0, 0x2 << 4);
			mmio_clrsetbits_32(0x28104c04, 0xf0, 0x2 << 4);
			mmio_clrsetbits_32(0x28104c08, 0xf0, 0x2 << 4);
			mmio_clrsetbits_32(0x28104c10, 0xf0, 0x2 << 4);
			mmio_clrsetbits_32(0x28104c14, 0xf0, 0x2 << 4);
			mmio_clrsetbits_32(0x28104c18, 0xf0, 0x2 << 4);
#endif
			break;
		case PINMUX_EMMC:
			PINMUX_CONFIG(EMMC_CLK, EMMC_CLK, G11);
			PINMUX_CONFIG(EMMC_RST_X, EMMC_RST_X, G11);
			PINMUX_CONFIG(EMMC_CMD, EMMC_CMD, G11);
			PINMUX_CONFIG(EMMC_D0, EMMC_D0, G11);
			PINMUX_CONFIG(EMMC_D1, EMMC_D1, G11);
			PINMUX_CONFIG(EMMC_D2, EMMC_D2, G11);
			PINMUX_CONFIG(EMMC_D3, EMMC_D3, G11);
			break;
		case PINMUX_SPI_NAND:
			PINMUX_CONFIG(EMMC_CLK, SPINAND_SCK, G11);
			PINMUX_CONFIG(EMMC_CMD, SPINAND_SDI, G11);
			PINMUX_CONFIG(EMMC_D1, SPINAND_CS_X, G11);
			PINMUX_CONFIG(EMMC_D0, SPINAND_SDO, G11);
			PINMUX_CONFIG(EMMC_D2, SPINAND_HOLD_X, G11);
			PINMUX_CONFIG(EMMC_D3, SPINAND_WP_X, G11);
			// set pin pull, make sure to follow the rom
			PIN_PULL_CONFIG_2(EMMC_CLK, G11, 0);
			PIN_PULL_CONFIG_2(EMMC_CMD, G11, 1);
			PIN_PULL_CONFIG_2(EMMC_D0, G11, 1);
			PIN_PULL_CONFIG_2(EMMC_D1, G11, 1);
			PIN_PULL_CONFIG_2(EMMC_D2, G11, 1);
			PIN_PULL_CONFIG_2(EMMC_D3, G11, 1);
			break;
		case PINMUX_SPI_NOR:
			PINMUX_CONFIG(EMMC_CLK, SPINOR_SCK, G11);
			PINMUX_CONFIG(EMMC_CMD, SPINOR_SDI, G11);
			PINMUX_CONFIG(EMMC_D1, SPINOR_CS_X, G11);
			PINMUX_CONFIG(EMMC_D0, SPINOR_SDO, G11);
			PINMUX_CONFIG(EMMC_D2, SPINOR_HOLD_X, G11);
			PINMUX_CONFIG(EMMC_D3, SPINOR_WP_X, G11);
		break;
		case PINMUX_SPI0:
			/* set for gpio for spix_cs */
			PINMUX_CONFIG(SPI0_CS_X, GPIO107, G9);
			PINMUX_CONFIG(SPI0_SDI, SPI0_SDI, G9);
			PINMUX_CONFIG(SPI0_SDO, SPI0_SDO, G9);
			PINMUX_CONFIG(SPI0_SCK, SPI0_SCK, G9);
		break;
		case PINMUX_DSI:
			// PINMUX_CONFIG(PAD_MIPI_TXM0, XGPIOC_12);
			// PINMUX_CONFIG(PAD_MIPI_TXP0, XGPIOC_13);
			// PINMUX_CONFIG(PAD_MIPI_TXM1, XGPIOC_14);
			// PINMUX_CONFIG(PAD_MIPI_TXP1, XGPIOC_15);
			// PINMUX_CONFIG(PAD_MIPI_TXM2, XGPIOC_16);
			// PINMUX_CONFIG(PAD_MIPI_TXP2, XGPIOC_17);
			// PINMUX_CONFIG(PAD_MIPI_TXM3, XGPIOC_20);
			// PINMUX_CONFIG(PAD_MIPI_TXP3, XGPIOC_21);
			// PINMUX_CONFIG(PAD_MIPI_TXM4, XGPIOC_18);
			// PINMUX_CONFIG(PAD_MIPI_TXP4, XGPIOC_19);
		break;
		case PINMUX_LVDS:
			// PINMUX_CONFIG(PAD_MIPI_TXM0, XGPIOC_12);
			// PINMUX_CONFIG(PAD_MIPI_TXP0, XGPIOC_13);
			// PINMUX_CONFIG(PAD_MIPI_TXM1, XGPIOC_14);
			// PINMUX_CONFIG(PAD_MIPI_TXP1, XGPIOC_15);
			// PINMUX_CONFIG(PAD_MIPI_TXM2, XGPIOC_16);
			// PINMUX_CONFIG(PAD_MIPI_TXP2, XGPIOC_17);
			// PINMUX_CONFIG(PAD_MIPI_TXM3, XGPIOC_20);
			// PINMUX_CONFIG(PAD_MIPI_TXP3, XGPIOC_21);
			// PINMUX_CONFIG(PAD_MIPI_TXM4, XGPIOC_18);
			// PINMUX_CONFIG(PAD_MIPI_TXP4, XGPIOC_19);
		break;
		case PINMUX_I2C0:
			PINMUX_CONFIG(IIC0_SCL, IIC0_SCL, G12);
			PINMUX_CONFIG(IIC0_SDA, IIC0_SDA, G12);
		break;
		case PINMUX_I2S0:
			PINMUX_CONFIG(I2S0_SCLK, I2S0_SCLK, G6);
			PINMUX_CONFIG(I2S0_WSI, I2S0_WSI, G6);
			PINMUX_CONFIG(I2S0_SDI0, I2S0_SDI0, G6);
			PINMUX_CONFIG(I2S0_SDI1, I2S0_SDI1, G6);
			PINMUX_CONFIG(I2S0_SDO, I2S0_SDO, G6);
			PINMUX_CONFIG(I2S0_MCLK, I2S0_MCLK, G6);
		break;
		case PINMUX_I2S1:
			PINMUX_CONFIG(PAD_AOUTL0, I2S1_SCLK, PHY);
			PINMUX_CONFIG(PAD_AOUTR0, I2S1_WSI, PHY);
			PINMUX_CONFIG(PAD_AINL0_MIC, I2S1_SDI, PHY);
			PINMUX_CONFIG(PAD_AINR0_MIC, I2S1_SDO, PHY);
			PINMUX_CONFIG(PWR_GPIO1, I2S1_MCLK, G7);
		break;
		case PINMUX_I2S2:
			PINMUX_CONFIG(PWR_GPIO0, I2S2_MCLK, G7);
			PINMUX_CONFIG(PAD_AOUTL1, I2S2_SCLK, PHY);
			PINMUX_CONFIG(PAD_AOUTR1, I2S2_WSI, PHY);
			PINMUX_CONFIG(PAD_AINL1_MIC, I2S2_SDI, PHY);
			PINMUX_CONFIG(PAD_AINR1_MIC, I2S2_SDO, PHY);
		break;
		case PINMUX_WGN0:
			PINMUX_CONFIG(CAN0_TX, WG0_D0, G11);
			PINMUX_CONFIG(CAN0_RX, WG0_D1, G11);
		break;
		default:
			break;
	}

	// Pins of mipi used for other function
	mmio_write_32(0x6700b0c0, 0x3fffffff);
	mmio_write_32(0x6700b160, 0x0);
}

#include "../cvi_board_init.c"

void cpu_pwr_ctrl(void)
{
#if defined(CONFIG_RISCV)
	mmio_write_32(0x01901008, 0x30001);// cortexa53_pwr_iso_en
#elif defined(CONFIG_ARM)
	//mmio_write_32(0x01901004, 0x30001);// c906_top_pwr_iso_en   (pld mark)
#endif
}

void bm_storage_boot_loader_version_uboot(void)
{
	int size = 0;

	size = strlen(version_string);
	for (int i = 0; i < size; i++)
		mmio_write_8(UBOOT_VERSION_BASE + i, version_string[i]);

	mmio_write_8(UBOOT_VERSION_BASE + size, '\0');
}

#ifdef CONFIG_VIDEO_SOPH
void board_show_logo(void)
{
	struct udevice *dev;

	uclass_get_device_by_driver(UCLASS_VIDEO,
			DM_DRIVER_GET(soph_display), &dev);
	soph_show_logo();
}
#endif

void hdcp_load_key(void)
{
	/*Load hdmi hdcp key*/
	run_command("load mmc 0:1 0x102f80000 hdcp_key.bin", 0);
}

/*sm9v1 pinmux init*/
void sm9v1_board_init(void)
{
	PINMUX_CONFIG(CAM_MCLK0, CAM_MCLK0, G9);
	PINMUX_CONFIG(CAM_MCLK1, CAM_MCLK1, G9);
	PINMUX_CONFIG(CAM_MCLK2, CAM_MCLK2, G9);
	PINMUX_CONFIG(CAM_MCLK3, CAM_MCLK3, G9);
	PINMUX_CONFIG(CAM_MCLK4, CAM_MCLK4, G9);
	PINMUX_CONFIG(CAM_MCLK5, CAM_MCLK5, G9);
	PINMUX_CONFIG(IIC0_SCL, IIC0_SCL, G12);
	PINMUX_CONFIG(IIC0_SDA, IIC0_SDA, G12);
	PINMUX_CONFIG(IIC1_SCL, IIC1_SCL, G12);
	PINMUX_CONFIG(IIC1_SDA, IIC1_SDA, G12);
	PINMUX_CONFIG(IIC2_SCL, IIC2_SCL, G12);
	PINMUX_CONFIG(IIC2_SDA, IIC2_SDA, G12);
	//FOR WS BD
	PINMUX_CONFIG(IIC4_SCL, SPI3_SDI, G12);
	PINMUX_CONFIG(IIC4_SDA, SPI3_CS_X, G12);
	PINMUX_CONFIG(IIC5_SCL, SPI3_SCK, G12);
	PINMUX_CONFIG(IIC5_SDA, SPI3_SDO, G12);

	PINMUX_CONFIG(UART2_RX, IIC6_SCL, G12);
	PINMUX_CONFIG(UART2_TX, IIC6_SDA, G12);
	PINMUX_CONFIG(CAM_XLR0, GPIO69, G9);
	PINMUX_CONFIG(CAM_XLR1, GPIO70, G9);
	PINMUX_CONFIG(GPIO4, GPIO115, G12);
	PINMUX_CONFIG(GPIO5, GPIO116, G12);
	PINMUX_CONFIG(UART4_RTS, GPIO93, G12);
	PINMUX_CONFIG(UART4_CTS, GPIO94, G12);
	PINMUX_CONFIG(UART4_RX, UART4_RX, G12);
	PINMUX_CONFIG(UART4_TX, UART4_TX, G12);
	PINMUX_CONFIG(UART1_CTS, GPIO86, G11);
	mmio_write_32(0x27012004, mmio_read_32(0x27012004) | 0x400000);
	mmio_write_32(0x27012000, mmio_read_32(0x27012000) | 0x400000);
	PINMUX_CONFIG(UART1_RTS, GPIO85, G11);
	mmio_write_32(0x27012004, mmio_read_32(0x27012004) | 0x200000);
	mmio_write_32(0x27012000, mmio_read_32(0x27012000) | 0x200000);
	//Power on SM9 VCC_3V3_SYS to make sure phy led active
	mmio_write_32(0x5021004, mmio_read_32(0x5021004) | 0x800);
	mmio_write_32(0x5021000, mmio_read_32(0x5021000) | 0x800);
	//mipi dsi
	//PINMUX_CONFIG(PAD_MIPI0_TX0P, PAD_MIPI0_TX0P, PHY);
	//PINMUX_CONFIG(PAD_MIPI0_TX0N, PAD_MIPI0_TX0N, PHY);
	//PINMUX_CONFIG(PAD_MIPI0_TX1P, PAD_MIPI0_TX1P, PHY);
	//PINMUX_CONFIG(PAD_MIPI0_TX1N, PAD_MIPI0_TX1N, PHY);
	//PINMUX_CONFIG(PAD_MIPI0_TX2P, PAD_MIPI0_TX2P, PHY);
	//PINMUX_CONFIG(PAD_MIPI0_TX2N, PAD_MIPI0_TX2N, PHY);
	//PINMUX_CONFIG(PAD_MIPI0_TX3P, PAD_MIPI0_TX3P, PHY);
	//PINMUX_CONFIG(PAD_MIPI0_TX3N, PAD_MIPI0_TX3N, PHY);
	//PINMUX_CONFIG(PAD_MIPI0_TX4P, PAD_MIPI0_TX4P, PHY);
	//PINMUX_CONFIG(PAD_MIPI0_TX4N, PAD_MIPI0_TX4N, PHY);
	//PINMUX_CONFIG(PAD_MIPI1_TX0P, PAD_MIPI1_TX0P, PHY);
	//PINMUX_CONFIG(PAD_MIPI1_TX0N, PAD_MIPI1_TX0N, PHY);
	//PINMUX_CONFIG(PAD_MIPI1_TX1P, PAD_MIPI1_TX1P, PHY);
	//PINMUX_CONFIG(PAD_MIPI1_TX1N, PAD_MIPI1_TX1N, PHY);
	//PINMUX_CONFIG(PAD_MIPI1_TX2P, PAD_MIPI1_TX2P, PHY);
	//PINMUX_CONFIG(PAD_MIPI1_TX2N, PAD_MIPI1_TX2N, PHY);
	//PINMUX_CONFIG(PAD_MIPI1_TX3P, PAD_MIPI1_TX3P, PHY);
	//PINMUX_CONFIG(PAD_MIPI1_TX3N, PAD_MIPI1_TX3N, PHY);
	//PINMUX_CONFIG(PAD_MIPI1_TX4P, PAD_MIPI1_TX4P, PHY);
	//PINMUX_CONFIG(PAD_MIPI1_TX4N, PAD_MIPI1_TX4N, PHY);
	PINMUX_CONFIG(PWR_UART_TX, PWR_UART_TX, G7);
	PINMUX_CONFIG(PWR_UART_RX, PWR_UART_RX, G7);
	//PINMUX_CONFIG(UART1_TX, UART1_TX, G11);
	//PINMUX_CONFIG(UART1_RX, UART1_RX, G11);
	PINMUX_CONFIG(GPIO1, GPIO112, G12);
	PINMUX_CONFIG(GPIO0, GPIO111, G12);
	PINMUX_CONFIG(GPIO3, GPIO114, G12);
	//LED GPIO
	PINMUX_CONFIG(PWR_GPIO1, PWR_GPIO1, G7);
	//VCC12V-DCIN VCC_SYS_5V VCC_SYS_3V3
	PINMUX_CONFIG(PWR_SEQ1, PWR_GPIO11, G7);//VCC12V-DCIN
	PINMUX_CONFIG(GPIO1, GPIO112, G12);//VCC_SYS_5V
	PINMUX_CONFIG(IIC2_SDA, GPIO99, G12);//VCC_SYS_3V3
	//USB HOST
	PINMUX_CONFIG(CAM_MCLK5, GPIO51, G9);//VCC HUB
	PINMUX_CONFIG(PWR_GPIO5, PWR_GPIO5, G7);//VCC USB CON
	//I2C5
	PINMUX_CONFIG(SD1_D0, IIC5_SDA, G11);
	PINMUX_CONFIG(SD1_D1, IIC5_SCL, G11);
	//I2C6
	PINMUX_CONFIG(SD1_D2, IIC6_SDA, G11);
	PINMUX_CONFIG(SD1_D3, IIC6_SCL, G11);
	//PCEI-NVME POWER EN
	PINMUX_CONFIG(CAM_MCLK4, GPIO50, G9);
	//UART1,UART2,UART4,UART5,UART6
	PINMUX_CONFIG(PAD_MIPI1_TX0P, UART1_TX, PHY);
	PINMUX_CONFIG(PAD_MIPI1_TX0N, UART1_RX, PHY);
	PINMUX_CONFIG(PAD_MIPI1_TX1P, UART1_RTS, PHY);
	PINMUX_CONFIG(PAD_MIPI1_TX1N, UART1_CTS, PHY);
	PINMUX_CONFIG(PAD_MIPI1_TX2P, UART2_TX, PHY);
	PINMUX_CONFIG(PAD_MIPI1_TX2N, UART2_RX, PHY);
	//绿色旧板使用这组UART4
	PINMUX_CONFIG(FAN0, GPIO79, G9);
	PINMUX_CONFIG(FAN1, GPIO80, G9);
	//PINMUX_CONFIG(FAN0, UART4_TX, G9);
	//PINMUX_CONFIG(FAN1, UART4_RX, G9);
	PINMUX_CONFIG(PWM0, UART5_TX, G9);
	PINMUX_CONFIG(PWM1, UART5_RX, G9);
	PINMUX_CONFIG(PWM2, UART6_TX, G9);
	PINMUX_CONFIG(PWM3, UART6_RX, G9);
	PINMUX_CONFIG(SD1_CD_X, GPIO61, G12);
	//PINMUX_CONFIG(SD0_PWR_EN, GPIO60, G9);
	mmio_write_32(0x6700b0c0, 0x3fffffff);
	mmio_write_32(0x6700b160, 0x0);
	//MCU I2C GPIO
	PINMUX_CONFIG(PAD_VIVO0_D13, GPIO131, G5);
	PINMUX_CONFIG(PAD_VIVO0_D14, GPIO132, G5);
	//VCC FOR 4G
	PINMUX_CONFIG(PWR_GPIO4, PWR_GPIO4, G7);
	//For GPIO
	PINMUX_CONFIG(SD1_PWR_EN, GPIO68, G12);
	PINMUX_CONFIG(PAD_MIPI1_TX3P, GPIO124, PHY);
	PINMUX_CONFIG(PAD_MIPI1_TX3N, GPIO125, PHY);
	PINMUX_CONFIG(PAD_MIPI1_TX4P, IIC3_SDA, PHY);
	PINMUX_CONFIG(PAD_MIPI1_TX4N, IIC3_SCL, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX0P, GPIO174, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX0N, GPIO175, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX1P, GPIO176, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX1N, GPIO177, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX2P, GPIO178, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX2N, GPIO179, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX3P, GPIO180, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX3N, GPIO181, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX4P, GPIO182, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX4N, GPIO183, PHY);
	//SHUTDOWN IO
	PINMUX_CONFIG(PWR_GPIO0, PWR_GPIO0, G7);
	//CAM I2C
	PINMUX_CONFIG(PAD_VIVO0_D15, IIC7_SDA, G5);
	PINMUX_CONFIG(PAD_VIVO0_D16, IIC7_SCL, G5);
	//FAN PWM
	PINMUX_CONFIG(GPIO3, PWM11, G12);
	//RECOVERY KEY
	PINMUX_CONFIG(PWR_SEQ2, PWR_GPIO12, G7);
	//GPIO
	PINMUX_CONFIG(I2S0_SCLK, GPIO0, G6);
	PINMUX_CONFIG(I2S0_WSI, GPIO1, G6);
	PINMUX_CONFIG(I2S0_SDI0, GPIO2, G6);
	PINMUX_CONFIG(I2S0_SDI1, GPIO3, G6);
	PINMUX_CONFIG(I2S0_SDO, GPIO4, G6);
	PINMUX_CONFIG(I2S0_MCLK, GPIO5, G6);
	PINMUX_CONFIG(PWR_GPIO2, PWR_GPIO2, G7);
	PINMUX_CONFIG(PWR_GPIO3, PWR_GPIO3, G7);
	//PCIE RST
	PINMUX_CONFIG(PAD_VIVO0_D11, GPIO129, G5);
	PINMUX_CONFIG(PAD_VIVO0_D12, GPIO130, G5);
	//BD UART3
	PINMUX_CONFIG(IIC2_SDA, UART3_TX, G12);
	PINMUX_CONFIG(IIC2_SCL, UART3_RX, G12);
	//GPIO
	PINMUX_CONFIG(UART1_TX, GPIO83, G11);
	PINMUX_CONFIG(UART1_RX, GPIO84, G11);
}

/*sm9v2 pinmux init*/
void sm9v2_board_init(void)
{
	PINMUX_CONFIG(CAM_MCLK0, CAM_MCLK0, G9);
	PINMUX_CONFIG(CAM_MCLK1, CAM_MCLK1, G9);
	PINMUX_CONFIG(CAM_MCLK2, CAM_MCLK2, G9);
	PINMUX_CONFIG(CAM_MCLK3, CAM_MCLK3, G9);
	PINMUX_CONFIG(CAM_MCLK4, CAM_MCLK4, G9);
	PINMUX_CONFIG(CAM_MCLK5, CAM_MCLK5, G9);
	PINMUX_CONFIG(IIC0_SCL, IIC0_SCL, G12);
	PINMUX_CONFIG(IIC0_SDA, IIC0_SDA, G12);
	PINMUX_CONFIG(IIC1_SCL, IIC1_SCL, G12);
	PINMUX_CONFIG(IIC1_SDA, IIC1_SDA, G12);
	PINMUX_CONFIG(IIC2_SCL, IIC2_SCL, G12);
	PINMUX_CONFIG(IIC2_SDA, IIC2_SDA, G12);
	PINMUX_CONFIG(IIC4_SCL, IIC4_SCL, G12);
	PINMUX_CONFIG(IIC4_SDA, IIC4_SDA, G12);
	PINMUX_CONFIG(UART2_RX, UART2_RX, G12);
	PINMUX_CONFIG(UART2_TX, UART2_TX, G12);
	PINMUX_CONFIG(UART2_RTS, UART2_RTS, G12);
	PINMUX_CONFIG(UART2_CTS, UART2_CTS, G12);
	PINMUX_CONFIG(CAM_XLR0, GPIO69, G9);
	PINMUX_CONFIG(CAM_XLR1, GPIO70, G9);
	PINMUX_CONFIG(GPIO4, GPIO115, G12);
	PINMUX_CONFIG(GPIO5, GPIO116, G12);
	PINMUX_CONFIG(UART4_RTS, GPIO93, G12);
	PINMUX_CONFIG(UART4_CTS, GPIO94, G12);
	PINMUX_CONFIG(UART4_RX, UART4_RX, G12);
	PINMUX_CONFIG(UART4_TX, UART4_TX, G12);

	PINMUX_CONFIG(UART1_CTS, UART5_RX, G11);
	mmio_write_32(0x27012004, mmio_read_32(0x27012004) | 0x400000);
	mmio_write_32(0x27012000, mmio_read_32(0x27012000) | 0x400000);
	PINMUX_CONFIG(UART1_RTS, UART5_TX, G11);
	mmio_write_32(0x27012004, mmio_read_32(0x27012004) | 0x200000);
	mmio_write_32(0x27012000, mmio_read_32(0x27012000) | 0x200000);

	//mipi dsi
	PINMUX_CONFIG(PAD_MIPI0_TX0P, PAD_MIPI0_TX0P, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX0N, PAD_MIPI0_TX0N, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX1P, PAD_MIPI0_TX1P, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX1N, PAD_MIPI0_TX1N, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX2P, PAD_MIPI0_TX2P, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX2N, PAD_MIPI0_TX2N, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX3P, PAD_MIPI0_TX3P, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX3N, PAD_MIPI0_TX3N, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX4P, PAD_MIPI0_TX4P, PHY);
	PINMUX_CONFIG(PAD_MIPI0_TX4N, PAD_MIPI0_TX4N, PHY);

	PINMUX_CONFIG(PAD_MIPI1_TX0P, PAD_MIPI1_TX0P, PHY);
	PINMUX_CONFIG(PAD_MIPI1_TX0N, PAD_MIPI1_TX0N, PHY);
	PINMUX_CONFIG(PAD_MIPI1_TX1P, PAD_MIPI1_TX1P, PHY);
	PINMUX_CONFIG(PAD_MIPI1_TX1N, PAD_MIPI1_TX1N, PHY);
	PINMUX_CONFIG(PAD_MIPI1_TX2P, PAD_MIPI1_TX2P, PHY);
	PINMUX_CONFIG(PAD_MIPI1_TX2N, PAD_MIPI1_TX2N, PHY);
	PINMUX_CONFIG(PAD_MIPI1_TX3P, PAD_MIPI1_TX3P, PHY);
	PINMUX_CONFIG(PAD_MIPI1_TX3N, PAD_MIPI1_TX3N, PHY);
	PINMUX_CONFIG(PAD_MIPI1_TX4P, PAD_MIPI1_TX4P, PHY);
	PINMUX_CONFIG(PAD_MIPI1_TX4N, PAD_MIPI1_TX4N, PHY);

	PINMUX_CONFIG(PWR_UART_TX, PWR_GPIO16, G7);
	PINMUX_CONFIG(PWR_UART_RX, PWR_GPIO17, G7);
	PINMUX_CONFIG(UART1_TX, UART1_TX, G11);
	PINMUX_CONFIG(UART1_RX, UART1_RX, G11);
	PINMUX_CONFIG(GPIO1, GPIO112, G12);
	PINMUX_CONFIG(GPIO0, GPIO111, G12);
	PINMUX_CONFIG(GPIO3, GPIO114, G12);
	PINMUX_CONFIG(GPIO2, GPIO113, G12);
	PINMUX_CONFIG(IIC5_SCL, GPIO104, G12);
	PINMUX_CONFIG(IIC5_SDA, GPIO103, G12);

	PINMUX_CONFIG(PAD_VIVO0_D15, UART7_TX, G5);
	PINMUX_CONFIG(PAD_VIVO0_D16, UART7_RX, G5);

	//spi
	PINMUX_CONFIG(PAD_VIVO0_D14, SPI0_SCK, G5);
	PINMUX_CONFIG(PAD_VIVO0_D13, SPI0_SDO, G5);
	PINMUX_CONFIG(PAD_VIVO0_D12, SPI0_SDI, G5);
	PINMUX_CONFIG(PAD_VIVO0_D11, SPI0_CS_X, G5);

	PINMUX_CONFIG(I2S0_SDI1, GPIO3, G6);
	PINMUX_CONFIG(CAM_MCLK2, GPIO48, G9);

	//GPIO
	PINMUX_CONFIG(CAM_MCLK5, GPIO51, G9);
	PINMUX_CONFIG(SD0_PWR_EN, GPIO60, G9);
	PINMUX_CONFIG(SD1_PWR_EN, GPIO68, G12);
	PINMUX_CONFIG(SD1_CD_X, GPIO61, G12);
	PINMUX_CONFIG(CLK_25M_OUT, GPIO119, G7);
	PINMUX_CONFIG(PCIE0_L0_CLKREQ_IN_X, GPIO42, G5);
	PINMUX_CONFIG(PWR_GPIO0, PWR_GPIO0, G7);
}

/*set pinmux by prd*/
void set_product_pinmux(void)
{
	char dtstype[DTSNAME_MAX_LEN] = { 0 };
	get_dts_type_from_sram(dtstype);
	if (strstr(dtstype, "sm9v1"))
		sm9v1_board_init();
	else if (strstr(dtstype, "se9b1"))
		sm9v1_board_init();
	else if (strstr(dtstype, "se9b3"))
		sm9v1_board_init();
	else if (strstr(dtstype, "sm9v2"))
		sm9v2_board_init();
}

int board_init(void)
{
#if (!defined CONFIG_TARGET_CVITEK_CV181X_FPGA) && (!defined CONFIG_TARGET_CVITEK_CV186X_FPGA) && \
	(!defined CV186X_FPGA_PALLDIUM_ENV)
	// extern uint32_t BOOT0_START_TIME;
	//uint16_t start_time = DIV_ROUND_UP(BOOT0_START_TIME, SYS_COUNTER_FREQ_IN_SECOND / 1000);
	// Save uboot start time. time is from boot0.h
	//mmio_write_16(TIME_RECORDS_FIELD_UBOOT_START, start_time);
#endif
	cpu_pwr_ctrl();
	pinmux_config(PINMUX_SDIO0);
#if defined(CONFIG_NAND_SUPPORT)
	pinmux_config(PINMUX_SPI_NAND);
#elif defined(CONFIG_SPI_FLASH)
	pinmux_config(PINMUX_SPI_NOR);
#elif defined(CONFIG_EMMC_SUPPORT)
	pinmux_config(PINMUX_EMMC);
#endif
#ifdef CONFIG_DISPLAY_CVITEK_MIPI
	//pinmux_config(PINMUX_DSI);
#elif defined(CONFIG_DISPLAY_CVITEK_LVDS)
	//pinmux_config(PINMUX_LVDS);
#endif

	//pinmux_config(PINMUX_SDIO1);

	pinmux_config(PINMUX_I2C0);
	pinmux_config(PINMUX_SPI0);
	PINMUX_CONFIG(AUX0, GPIO7, G9);//unmute dac1

	PINMUX_CONFIG(UART4_TX, GPIO91, G12);
	PINMUX_CONFIG(UART4_RX, GPIO92, G12);
	//pinmux_config(PINMUX_I2S1);
	//pinmux_config(PINMUX_I2S2);
	// pinmux_config(PINMUX_WGN0);

	cvi_board_init();
	PINMUX_CONFIG(PWR_GPIO3, PWR_GPIO3, G7);
	PINMUX_CONFIG(PWR_GPIO4, PWR_GPIO4, G7);
	PINMUX_CONFIG(PWR_GPIO5, PWR_GPIO5, G7);
	PINMUX_CONFIG(PWR_GPIO6, PWR_GPIO6, G7);

	//for product pinmux
	set_product_pinmux();

	bm_storage_boot_loader_version_uboot();
	return 0;
}

#define DTSNAME_MAX_LEN 32
#if defined(CONFIG_ROOTFS_UBUNTU) || defined(CONFIG_ROOTFS_DEBIAN)
#define DEFAULT_DTSNAME "config-bm1688_wevb_8G"
#else
#define DEFAULT_DTSNAME "config-" __stringify(CVICHIP) "_" __stringify(CVIBOARD)
#endif

void get_dts_type_from_oem(unsigned char *dtsname)
{
#ifdef CONFIG_EMMC_SUPPORT
	if (!dtsname) {
		printf("get dts type from oem failed!\n");
		return;
	}
	run_command("mmc dev 0 2\0", 0);
	run_command("mmc read 0x120000000 0 0xc0", 0);
	memcpy(dtsname, (void *)(0x120000000 + 0xa0), DTSNAME_MAX_LEN);
	dtsname[DTSNAME_MAX_LEN - 1] = '\0';
#endif
	// use default dts
	if (strlen(dtsname) == 0)
		memcpy(dtsname, DEFAULT_DTSNAME, sizeof(DEFAULT_DTSNAME));
	printf("OEM INFO: DTS_TYPE[%s]\n", dtsname);
}

/*get dts from sram*/
void get_dts_type_from_sram(unsigned char *dtsname)
{
#ifdef CONFIG_EMMC_SUPPORT
	if (!dtsname) {
		return;
	}
	memcpy(dtsname, (void *)DTSTYPE_OEM_INFO, DTSNAME_MAX_LEN);
	dtsname[DTSNAME_MAX_LEN - 1] = '\0';
#endif
	/*use default dts*/
	if (strlen(dtsname) == 0)
		memcpy(dtsname, DEFAULT_DTSNAME, sizeof(DEFAULT_DTSNAME));
}

u64 get_ddr_size_from_sram(void)
{
	u64 ddr_size;

	ddr_size = mmio_read_32(DDR_SIZE_OEM_INFO);

	//printf("ddr_size = %lld\n", ddr_size);
	ddr_size &= 0xFF;
	//printf("ddr_size = %lld\n", ddr_size);
	switch (ddr_size) {
	case 2:
	case 4:
	case 8:
	case 12:
	case 16:
		break;

	default:
		return PHYS_SDRAM_1_SIZE;
	}
	return ddr_size * 1024 * 1024 * 1024;
}

/*set default console by oem*/
struct serial_device *default_serial_console(void)
{
	/*select console uart*/
	if (mmio_read_8(CONSOLE_OEM_INFO) == CONSOLE_USE_UART2){
		return &eserial3_device;//uart2
	}
	return &eserial1_device;//uart0
}

/*set consoledev by oem*/
void setup_sophgo_console(void)
{
	if (mmio_read_8(CONSOLE_OEM_INFO) == CONSOLE_USE_UART2)//uart2
		env_set("consoledev", "ttyS2");
	printf("set console 0x%x\n",mmio_read_8(CONSOLE_OEM_INFO));
}

int setup_sophgo_dts(void)
{
	//char configName[DTSNAME_MAX_LEN * 2] = {0};
	//char dtsType[DTSNAME_MAX_LEN] = {0};
	//char *ptr = NULL;
	u64 ddr_size;

	ddr_size = get_ddr_size_from_sram();
	ddr_size /= 0x40000000;
	if(ddr_size == 16) {
		env_set("ddrsize", "16");
	}
	else if(ddr_size == 8) {
		env_set("ddrsize", "8");
	}
/*
	get_dts_type_from_oem(dtsType);
	ptr = strstr(dtsType, "config-");
	//If the string does not start with "config-"
	if (!ptr && ptr != dtsType) {
		sprintf(configName, "config-%s", dtsType);
		env_set("DTS_TYPE", configName);
		return 0;
	}

	env_set("DTS_TYPE", dtsType);
	*/
	return 0;
}

#if defined(CONFIG_ROOTFS_UBUNTU) || defined(CONFIG_ROOTFS_DEBIAN)
#ifdef CONFIG_BOARD_LATE_INIT
static void get_ether_addr_from_emmc(unsigned char *mac, int i)
{
	#if defined(CONFIG_SPI_FLASH)
	//todo: read from spinor flash

	#else	//default eMMC
	run_command("mmc dev 0 2\0", 0);
	run_command("mmc read 0x120000000 0 0x60", 0);
	memcpy(mac, (void *)(0x120000000 + 0x40 + i*0x10), 6);
	#endif
}
#define ETHER_NUM 2
#define MAC_SIZE 6
static int setup_mac(void)
{
	int i;
	uint8_t mac[ETHER_NUM][MAC_SIZE];
	char eth[16];

	// get ethaddr from emmc boot1 part
	for (i = 0; i < ETHER_NUM; i++) {
		get_ether_addr_from_emmc(mac[i], i);
		if (i == 0)
			snprintf(eth, sizeof(eth), "ethaddr");
		else
			snprintf(eth, sizeof(eth), "eth%uaddr", i);

		if (is_zero_ethaddr(mac[i])) {
			debug("%s mac address not specified, use default\n",
						eth);
			continue;
		}

		if (!is_valid_ethaddr(mac[i])) {
			printf("Invalid mac address %pM\n", mac[i]);
			continue;
		}

		if (eth_env_set_enetaddr(eth, mac[i]) == -EEXIST)
			printf("mac%d address has benn set before\n", i);
	}

	return 0;
}

int board_late_init(void)
{
	console_record_reset_enable();
	setup_mac();
	setup_sophgo_dts();
	setup_sophgo_console();
	hdcp_load_key();
#ifdef CONFIG_VIDEO_SOPH
	board_show_logo();
#endif
	return 0;
}
#endif
#else
int board_late_init(void)
{
	setup_sophgo_dts();
	hdcp_load_key();
#ifdef CONFIG_VIDEO_SOPH
	board_show_logo();
#endif
	return 0;
}
#endif

#if defined(__aarch64__)
int dram_init(void)
{
	//printf("=====================>dram_init \n");
	gd->ram_size = get_ddr_size_from_sram();
	mem_map[1].size = gd->ram_size;
	return 0;
}

int dram_init_banksize(void)
{
	//printf("=====================>dram_init_banksize \n");
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = get_ddr_size_from_sram();

	return 0;
}
#endif

#ifdef CV_SYS_OFF
static void cv_system_off(void)
{
	mmio_write_32(REG_RTC_BASE + RTC_EN_SHDN_REQ, 0x01);
	while (mmio_read_32(REG_RTC_BASE + RTC_EN_SHDN_REQ) != 0x01)
		;
	mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0_UNLOCKKEY, 0xAB18);
	mmio_setbits_32(REG_RTC_CTRL_BASE + RTC_CTRL0, 0xFFFF0800 | (0x1 << 0));

	while (1)
		;
}
#endif

void cv_system_reset(void)
{
	mmio_write_32(REG_RTC_BASE + RTC_EN_PWR_CYC_REQ, 0x01);
	while (mmio_read_32(REG_RTC_BASE + RTC_EN_PWR_CYC_REQ) != 0x01)
		;
	mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0_UNLOCKKEY, 0xAB18);
	mmio_setbits_32(REG_RTC_CTRL_BASE + RTC_CTRL0, 0xFFFF0800 | (0x1 << 3));

	while (1)
		;
}

/*
 * Board specific reset that is system reset.
 */
void reset_cpu(void)
{
	cv_system_reset();
}

#ifdef CONFIG_USB_GADGET_DWC2_OTG
struct dwc2_plat_otg_data cv182x_otg_data = {
	.regs_otg = USB_BASE,
	.usb_gusbcfg	= 0x40081400,
	.rx_fifo_sz	 = 512,
	.np_tx_fifo_sz  = 512,
	.tx_fifo_sz	 = 512,
};

int board_usb_init(int index, enum usb_init_type init)
{
	uint32_t value;

	value = mmio_read_32(TOP_BASE + REG_TOP_SOFT_RST) & (~BIT_TOP_SOFT_RST_USB);
	mmio_write_32(TOP_BASE + REG_TOP_SOFT_RST, value);
	udelay(50);
	value = mmio_read_32(TOP_BASE + REG_TOP_SOFT_RST) | BIT_TOP_SOFT_RST_USB;
	mmio_write_32(TOP_BASE + REG_TOP_SOFT_RST, value);

	/* Set USB phy configuration */
	value = mmio_read_32(REG_TOP_USB_PHY_CTRL);
	mmio_write_32(REG_TOP_USB_PHY_CTRL, value | BIT_TOP_USB_PHY_CTRL_EXTVBUS
					| USB_PHY_ID_OVERRIDE_ENABLE
					| USB_PHY_ID_VALUE);

	/* Enable ECO RXF */
	mmio_write_32(REG_TOP_USB_ECO, mmio_read_32(REG_TOP_USB_ECO) | BIT_TOP_USB_ECO_RX_FLUSH);

	printf("cvi_usb_hw_init done\n");

	return dwc2_udc_probe(&cv182x_otg_data);
}
#endif

void board_save_time_record(uintptr_t saveaddr)
{
	uint64_t boot_us = 0;
#if defined(__aarch64__)
	boot_us = timer_get_boot_us();
#elif defined(__riscv)
	// Read from CSR_TIME directly. RISC-V timers is initialized later.
	boot_us = csr_read(CSR_TIME) / (SYS_COUNTER_FREQ_IN_SECOND / 1000000);
#else
#error "Unknown ARCH"
#endif

	mmio_write_16(saveaddr, DIV_ROUND_UP(boot_us, 1000));
}

#if defined(CONFIG_MULTI_DTB_FIT)
int board_fit_config_name_match(const char *name)
{
	char fit_name[DTSNAME_MAX_LEN] = {0};
	char dtstype[DTSNAME_MAX_LEN] = {0};

	get_dts_type_from_sram(dtstype);
	memcpy(fit_name, name + 7, strlen(name) - 7 -2);//only match product name eg:_sm9v1_
	if (strstr((char *)dtstype, fit_name)) {
		return 0;
	}
	return -1;
}
#endif
