// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022 Starfive, Inc.
 * Author:	yanhong <yanhong.wang@starfivetech.com>
 *
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/eeprom.h>
#include <asm/arch/jh7110-regs.h>
#include <cpu_func.h>
#include <dm/uclass.h>
#include <dm/device.h>
#include <env.h>
#include <inttypes.h>
#include <misc.h>
#include <linux/bitops.h>
#include <asm/arch/gpio.h>
#include <bmp_logo.h>
#include <video.h>
#include <splash.h>

#define SYS_CLOCK_ENABLE(clk) \
	setbits_le32(SYS_CRG_BASE + clk, CLK_ENABLE_MASK)

#define PCB_REVISION_MASK	0xF0
#define PCB_REVISION_SHIFT	4
#define PCB_REVISION_A		0x0A
#define PCB_REVISION_B		0x0B
#define CHIP_REVISION_SHIFT	80

#define CPU_VOL_BINNING_OFFSET 0x7fc

enum {
	BOOT_FLASH =	0,
	BOOT_SD,
	BOOT_EMMC,
	BOOT_UART,
};

enum chip_type_t {
	CHIP_A = 0,
	CHIP_B,
	CHIP_MAX,
};

enum board_type_t {
	BOARD_1000M_1000M = 0,
	BOARD_1000M_100M,
	BOARD_TYPE_MAX,
};


enum cpu_voltage_type_t {
	CPU_VOL_1020 = 0xef0,
	CPU_VOL_1040 = 0xfff,
	CPU_VOL_1060 = 0xff0,
	CPU_VOL_1000  = 0x8f0,
};
#define CPU_VOL_MASK	0xfff

static void sys_reset_clear(ulong assert, ulong status, u32 rst)
{
	u32 value;

	clrbits_le32(SYS_CRG_BASE + assert, BIT(rst));
	do {
		value = in_le32(SYS_CRG_BASE + status);
	} while ((value & BIT(rst)) != BIT(rst));
}

static void jh7110_timer_init(void)
{
	SYS_CLOCK_ENABLE(TIMER_CLK_APB_SHIFT);
	SYS_CLOCK_ENABLE(TIMER_CLK_TIMER0_SHIFT);
	SYS_CLOCK_ENABLE(TIMER_CLK_TIMER1_SHIFT);
	SYS_CLOCK_ENABLE(TIMER_CLK_TIMER2_SHIFT);
	SYS_CLOCK_ENABLE(TIMER_CLK_TIMER3_SHIFT);

	sys_reset_clear(SYS_CRG_RESET_ASSERT3_SHIFT,
			SYS_CRG_RESET_STATUS3_SHIFT, TIMER_RSTN_APB_SHIFT);
	sys_reset_clear(SYS_CRG_RESET_ASSERT3_SHIFT,
			SYS_CRG_RESET_STATUS3_SHIFT, TIMER_RSTN_TIMER0_SHIFT);
	sys_reset_clear(SYS_CRG_RESET_ASSERT3_SHIFT,
			SYS_CRG_RESET_STATUS3_SHIFT, TIMER_RSTN_TIMER1_SHIFT);
	sys_reset_clear(SYS_CRG_RESET_ASSERT3_SHIFT,
			SYS_CRG_RESET_STATUS3_SHIFT, TIMER_RSTN_TIMER2_SHIFT);
	sys_reset_clear(SYS_CRG_RESET_ASSERT3_SHIFT,
			SYS_CRG_RESET_STATUS3_SHIFT, TIMER_RSTN_TIMER3_SHIFT);
}

static void jh7110_gmac_init_1000M(int id)
{
	switch (id) {
	case 0:
		clrsetbits_le32(AON_SYSCON_BASE + AON_SYSCFG_12,
			GMAC5_0_SEL_I_MASK,
			BIT(GMAC5_0_SEL_I_SHIFT) & GMAC5_0_SEL_I_MASK);
		break;

	case 1:
		clrsetbits_le32(SYS_SYSCON_BASE + SYS_SYSCON_144,
			GMAC5_1_SEL_I_MASK,
			BIT(GMAC5_1_SEL_I_SHIFT) & GMAC5_1_SEL_I_MASK);
		break;

	default:
		break;
	}
}

static void jh7110_gmac_init_100M(int id)
{
	switch (id) {
	case 0:
		clrsetbits_le32(AON_SYSCON_BASE + AON_SYSCFG_12,
			GMAC5_0_SEL_I_MASK,
			(4 << GMAC5_0_SEL_I_SHIFT) & GMAC5_0_SEL_I_MASK);
		setbits_le32(AON_CRG_BASE + GMAC5_0_CLK_TX_SHIFT, 0x1000000);
		setbits_le32(AON_CRG_BASE + GMAC5_0_CLK_RX_SHIFT, 0x1000000);
		break;

	case 1:
		clrsetbits_le32(SYS_SYSCON_BASE + SYS_SYSCON_144,
			GMAC5_1_SEL_I_MASK,
			(4 << GMAC5_1_SEL_I_SHIFT) & GMAC5_1_SEL_I_MASK);
		setbits_le32(SYS_CRG_BASE + GMAC5_1_CLK_TX_SHIFT, 0x1000000);
		setbits_le32(SYS_CRG_BASE + GMAC5_1_CLK_RX_SHIFT, 0x1000000);
		break;

	default:
		break;
	}
}

static void jh7110_gmac_sel_tx_to_rgmii(int id)
{
	switch (id) {
	case 0:
		clrsetbits_le32(AON_CRG_BASE + GMAC5_0_CLK_TX_SHIFT,
			GMAC5_0_CLK_TX_MASK,
			BIT(GMAC5_0_CLK_TX_BIT) & GMAC5_0_CLK_TX_MASK);
		break;

	case 1:
		clrsetbits_le32(SYS_CRG_BASE + GMAC5_1_CLK_TX_SHIFT,
			GMAC5_1_CLK_TX_MASK,
			BIT(GMAC5_1_CLK_TX_BIT) & GMAC5_1_CLK_TX_MASK);
		break;

	default:
		break;
	}
}

static void set_uboot_fdt_addr_env(void)
{
	char str[17];
	ulong fdt_addr = (ulong)gd->fdt_blob;

	sprintf(str, "0x%lx", fdt_addr);
	env_set("uboot_fdt_addr", str);
}

static int get_chip_type(void)
{
	int type;
	int len = -1;
	u8 data;

	len = get_data_from_eeprom(CHIP_REVISION_SHIFT, 1, &data);
	if (len <= 0) {
		env_set("chip_vision", "UNKOWN");
		return -EINVAL;
	}

	switch (data) {
	case 'a':
	case 'A':
		type = CHIP_A;
		env_set("chip_vision", "A");
		break;
	case 'b':
	case 'B':
		type = CHIP_B;
		env_set("chip_vision", "B");
		break;
	default:
		type = CHIP_MAX;
		env_set("chip_vision", "UNKOWN");
		break;
	}
	return type;
}
static int get_board_type(void)
{
	u8 pv;
	int type;

	pv = get_pcb_revision_from_eeprom();
	pv = (pv & PCB_REVISION_MASK) >> PCB_REVISION_SHIFT;

	if (pv == PCB_REVISION_A) {
		type = BOARD_1000M_100M;
	} else if (pv == PCB_REVISION_B) {
		type = BOARD_1000M_1000M;
	} else {
		type = BOARD_TYPE_MAX;
	}

	return type;
}

static void jh7110_gmac_init(int chip_type, int pcb_type)
{
	switch (chip_type) {
		case CHIP_A:
			break;
		case CHIP_B:
		default:
			jh7110_gmac_sel_tx_to_rgmii(0);
			jh7110_gmac_sel_tx_to_rgmii(1);
			break;
	}

	switch (pcb_type) {
		case BOARD_1000M_100M:
			jh7110_gmac_init_1000M(0);
			jh7110_gmac_init_100M(1);
			break;

		case BOARD_1000M_1000M:
		default:
			jh7110_gmac_init_1000M(0);
			jh7110_gmac_init_1000M(1);
			break;
	}
}

static void jh7110_usb_init(bool usb2_enable)
{
	if (usb2_enable) {
		/*usb 2.0 utmi phy init*/
		clrsetbits_le32(STG_SYSCON_BASE  + STG_SYSCON_4,
			USB_MODE_STRAP_MASK,
			(2<<USB_MODE_STRAP_SHIFT) &
			USB_MODE_STRAP_MASK);/*2:host mode, 4:device mode*/
		clrsetbits_le32(STG_SYSCON_BASE + STG_SYSCON_4,
			USB_OTG_SUSPENDM_BYPS_MASK,
			BIT(USB_OTG_SUSPENDM_BYPS_SHIFT)
			& USB_OTG_SUSPENDM_BYPS_MASK);
		clrsetbits_le32(STG_SYSCON_BASE + STG_SYSCON_4,
			USB_OTG_SUSPENDM_MASK,
			BIT(USB_OTG_SUSPENDM_SHIFT) &
			USB_OTG_SUSPENDM_MASK);/*HOST = 1. DEVICE = 0;*/
		clrsetbits_le32(STG_SYSCON_BASE + STG_SYSCON_4,
			USB_PLL_EN_MASK,
			BIT(USB_PLL_EN_SHIFT) & USB_PLL_EN_MASK);
		clrsetbits_le32(STG_SYSCON_BASE + STG_SYSCON_4,
			USB_REFCLK_MODE_MASK,
			BIT(USB_REFCLK_MODE_SHIFT) & USB_REFCLK_MODE_MASK);
		/* usb 2.0 phy mode,REPLACE USB3.0 PHY module = 1;else = 0*/
		clrsetbits_le32(SYS_SYSCON_BASE + SYS_SYSCON_24,
			PDRSTN_SPLIT_MASK,
			BIT(PDRSTN_SPLIT_SHIFT) &
			PDRSTN_SPLIT_MASK);
	} else {
		/*usb 3.0 pipe phy config*/
		clrsetbits_le32(STG_SYSCON_BASE  + STG_SYSCON_196,
			PCIE_CKREF_SRC_MASK,
			(0<<PCIE_CKREF_SRC_SHIFT) & PCIE_CKREF_SRC_MASK);
		clrsetbits_le32(STG_SYSCON_BASE  + STG_SYSCON_196,
			PCIE_CLK_SEL_MASK,
			(0<<PCIE_CLK_SEL_SHIFT) & PCIE_CLK_SEL_MASK);
		clrsetbits_le32(STG_SYSCON_BASE  + STG_SYSCON_328,
			PCIE_PHY_MODE_MASK,
			BIT(PCIE_PHY_MODE_SHIFT) & PCIE_PHY_MODE_MASK);
		clrsetbits_le32(STG_SYSCON_BASE  + STG_SYSCON_500,
			PCIE_USB3_BUS_WIDTH_MASK,
			(0 << PCIE_USB3_BUS_WIDTH_SHIFT) &
			PCIE_USB3_BUS_WIDTH_MASK);
		clrsetbits_le32(STG_SYSCON_BASE  + STG_SYSCON_500,
			PCIE_USB3_RATE_MASK,
			(0 << PCIE_USB3_RATE_SHIFT) & PCIE_USB3_RATE_MASK);
		clrsetbits_le32(STG_SYSCON_BASE  + STG_SYSCON_500,
			PCIE_USB3_RX_STANDBY_MASK,
			(0 << PCIE_USB3_RX_STANDBY_SHIFT)
			& PCIE_USB3_RX_STANDBY_MASK);
		clrsetbits_le32(STG_SYSCON_BASE  + STG_SYSCON_500,
			PCIE_USB3_PHY_ENABLE_MASK,
			BIT(PCIE_USB3_PHY_ENABLE_SHIFT)
			& PCIE_USB3_PHY_ENABLE_MASK);

		/* usb 3.0 phy mode,REPLACE USB3.0 PHY module = 1;else = 0*/
		clrsetbits_le32(SYS_SYSCON_BASE + SYS_SYSCON_24,
			PDRSTN_SPLIT_MASK,
			(0 << PDRSTN_SPLIT_SHIFT) & PDRSTN_SPLIT_MASK);
	}
	SYS_IOMUX_DOEN(25, LOW);
	SYS_IOMUX_DOUT(25, 7);
}

#if CONFIG_IS_ENABLED(STARFIVE_OTP)
static void get_cpu_voltage_type(struct udevice *dev)
{
	int ret;
	u32 buf = CPU_VOL_1040;

	ret = misc_read(dev, CPU_VOL_BINNING_OFFSET, &buf, sizeof(buf));
	if (ret != sizeof(buf))
		printf("%s: error reading CPU vol from OTP\n", __func__);
	else {
		switch ((buf & CPU_VOL_MASK)) {
		case CPU_VOL_1000:
			env_set("cpu_max_vol", "1000000");
			break;
		case CPU_VOL_1060:
			env_set("cpu_max_vol", "1060000");
			break;
		case CPU_VOL_1020:
			env_set("cpu_max_vol", "1020000");
			break;
		default:
			env_set("cpu_max_vol", "1040000");
			break;
		}
	}
}
#endif

static void jh7110_jtag_init(void)
{
	/*jtag*/
	SYS_IOMUX_DOEN(36, HIGH);
	SYS_IOMUX_DIN(36, 4);
	SYS_IOMUX_DOEN(61, HIGH);
	SYS_IOMUX_DIN(61, 19);
	SYS_IOMUX_DOEN(63, HIGH);
	SYS_IOMUX_DIN(63, 20);
	SYS_IOMUX_DOEN(60, HIGH);
	SYS_IOMUX_DIN(60, 29);
	SYS_IOMUX_DOEN(44, 8);
	SYS_IOMUX_DOUT(44, 22);
}

static void jh7110_i2c_init(int id)
{
	switch (id) {
	case 5:
		//scl
		SYS_IOMUX_COMPLEX(19, 79, 0, 42);
		//sda
		SYS_IOMUX_COMPLEX(20, 80, 0, 43);

		break;

	default:
		break;
	}
}

static void get_boot_mode(void)
{
	u32 value;

	value = in_le32(AON_IOMUX_BASE + AON_GPIO_DIN_REG);
	switch (value & 0x03) {
		case BOOT_FLASH:
			env_set("bootmode", "flash");
			env_set("devnum", "1");
			break;

		case BOOT_SD:
			env_set("bootmode", "sd");
			env_set("devnum", "1");
			break;

		case BOOT_EMMC:
			env_set("bootmode", "emmc");
			env_set("devnum", "0");
			break;

		default:
			env_set("bootmode", "uart");
			env_set("devnum", "1");
			break;
	}
}

static void jh7110_gpio_init(void)
{
	/* This is for fixing don't detect wm8960 occasionally.
	 * Set scl/sda gpio output enable
	 * Set drive strength to 12mA
	 * Set gpio pull up
	 */
	SYS_IOMUX_COMPLEX(57, 9, 0, 1);
	SYS_IOMUX_SET_DS(57, 3);
	SYS_IOMUX_SET_PULL(57, GPIO_PULL_UP);

	SYS_IOMUX_COMPLEX(58, 10, 0, 1);
	SYS_IOMUX_SET_DS(58, 3);
	SYS_IOMUX_SET_PULL(58, GPIO_PULL_UP);
}

int board_init(void)
{
	enable_caches();

	jh7110_jtag_init();
	jh7110_timer_init();

	jh7110_usb_init(true);

	jh7110_i2c_init(5);
	jh7110_gpio_init();

	return 0;
}

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
	struct udevice *dev;
	int ret;

	get_boot_mode();

	jh7110_gmac_init(get_chip_type(), get_board_type());
	/*
	 * save the memory info by environment variable in u-boot,
	 * It will used to update the memory configuration in dts,
	 * which passed to kernel lately.
	 */
	env_set_hex("memory_addr", gd->ram_base);
	env_set_hex("memory_size", gd->ram_size);

	ret = uclass_get_device(UCLASS_VIDEO, 0, &dev);
	if (ret)
		return ret;

	ret = video_bmp_display(dev, (ulong)&bmp_logo_bitmap[0], BMP_ALIGN_CENTER, BMP_ALIGN_CENTER, true);
	if (ret)
		goto err;

err:
	return 0;
}
#endif

#ifdef CONFIG_MISC_INIT_R

int misc_init_r(void)
{
	char mac0[6] = {0x6c, 0xcf, 0x39, 0x6c, 0xde, 0xad};
	char mac1[6] = {0x6c, 0xcf, 0x39, 0x7c, 0xae, 0x5d};

#if CONFIG_IS_ENABLED(STARFIVE_OTP)
	struct udevice *dev;
	char buf[16];
	int ret;
#define MACADDR_OFFSET 0x8

	ret = uclass_get_device_by_driver(UCLASS_MISC,
				DM_DRIVER_GET(starfive_otp), &dev);
	if (ret) {
		debug("%s: could not find otp device\n", __func__);
		goto err;
	}

	ret = misc_read(dev, MACADDR_OFFSET, buf, sizeof(buf));
	if (ret != sizeof(buf))
		printf("%s: error reading mac from OTP\n", __func__);
	else
		if (buf[0] != 0xff) {
			memcpy(mac0, buf, 6);
			memcpy(mac1, &buf[8], 6);
		}
err:
#endif
	eth_env_set_enetaddr("eth0addr", mac0);
	eth_env_set_enetaddr("eth1addr", mac1);

	get_chip_type();
	set_uboot_fdt_addr_env();
#if CONFIG_IS_ENABLED(STARFIVE_OTP)
	get_cpu_voltage_type(dev);
#endif
	return 0;
}
#endif

#ifdef CONFIG_ID_EEPROM

#include <asm/arch/eeprom.h>
#define STARFIVE_JH7110_EEPROM_DDRINFO_OFFSET	91

static bool check_eeprom_dram_info(ulong size)
{
	switch (size) {
	case 1:
	case 2:
	case 4:
	case 8:
	case 16:
		return true;
	default:
		return false;
	}
}

static int resize_ddr_from_eeprom(void)
{
	struct udevice *dev;
	ulong size;
	u32 len = 1;
	u8 data = 0;
	int ret;

	/* I2C init */
	ret = uclass_get_device(UCLASS_I2C, 0, &dev);
	if (ret) {
		debug("I2C init failed: %d\n", ret);
		return 0;
	}

	/* read memory size info */
	ret = get_data_from_eeprom(STARFIVE_JH7110_EEPROM_DDRINFO_OFFSET, len, &data);
	if (ret == len) {
		size = hextoul(&data, NULL);
		if (check_eeprom_dram_info(size))
			return size;
	}
	return 0;
}
#else
static int resize_ddr_from_eeprom(void)
{
	return 0;
}
#endif /* CONFIG_ID_EEPROM */

int board_ddr_size(void)
{
	return resize_ddr_from_eeprom();
}
