// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020 BayLibre, SAS
 * Author: Neil Armstrong <narmstrong@baylibre.com>
 */

#include <common.h>
#include <dm.h>
#include <env_internal.h>
#include <init.h>
#include <net.h>
#include <asm/io.h>
#include <asm/arch/boot.h>
#include <asm/arch/eth.h>
#include <asm/arch/sm.h>
#include <asm/global_data.h>
#include <i2c.h>
#include "khadas-mcu.h"

int mmc_get_env_dev(void)
{
	switch (meson_get_boot_device()) {
	case BOOT_DEVICE_EMMC:
		return 2;
	case BOOT_DEVICE_SD:
		return 1;
	default:
		/* boot device is not EMMC|SD */
		return -1;
	}
}

/*
 * The VIM3 on-board  MCU can mux the PCIe/USB3.0 shared differential
 * lines using a FUSB340TMX USB 3.1 SuperSpeed Data Switch between
 * an USB3.0 Type A connector and a M.2 Key M slot.
 * The PHY driving these differential lines is shared between
 * the USB3.0 controller and the PCIe Controller, thus only
 * a single controller can use it.
 */
int meson_ft_board_setup(void *blob, struct bd_info *bd)
{
	return 0;
}

#define EFUSE_MAC_OFFSET	0
#define EFUSE_MAC_SIZE		12
#define MAC_ADDR_LEN		6

//#define P_AO_GPIO_I              (volatile uint32_t *)(0xff800000 + (0x00a << 2))
//#define P_AO_RTI_PULL_UP_REG     (volatile uint32_t *)(0xff800000 + (0x00b << 2))
//#define P_AO_RTI_PULL_UP_EN_REG  (volatile uint32_t *)(0xff800000 + (0x00c << 2))

//#define P_AO_GPIO_I              (0xff800000 + (0x00a << 2))
//#define P_AO_RTI_PULL_UP_REG     (0xff800000 + (0x00b << 2))
//#define P_AO_RTI_PULL_UP_EN_REG  (0xff800000 + (0x00c << 2))

int misc_init_r(void)
{
	//printf("leeboby set AO_GPIO11 PULL\n");
	//setbits_le32(P_AO_GPIO_I, 1 << 11);
	//setbits_le32(P_AO_RTI_PULL_UP_REG, 1 << 11);
	//setbits_le32(P_AO_RTI_PULL_UP_EN_REG, 1 << 11);
	//printf("leeboby set AO_GPIO11 PULL\n");

	return 0;
}
