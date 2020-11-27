/*
 * (C) Copyright 2017-2020
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *zhouhuacai <zhouhuacai@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/timer.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/clock.h>
#include <asm/arch/efuse.h>
#include <asm/arch/platform.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/key.h>
#include <asm/arch/dma.h>

#include <boot_type.h>
#include <sys_partition.h>
#include <sys_config.h>
#include <power/sunxi/pmu.h>
#include <smc.h>
#include <sunxi_board.h>
#include <fdt_support.h>
#include <sys_config_old.h>
#include <cputask.h>

/* The sunxi internal brom will try to loader external bootloader
 * from mmc0, nannd flash, mmc2.
 * We check where we boot from by checking the config
 * of the gpio pin.
 */
DECLARE_GLOBAL_DATA_PTR;

extern void power_limit_init(void);
extern int sunxi_arisc_probe(void);
extern int set_sunxi_gpio_power_bias(void);

int power_source_init(void)
{
	int pll_cpux;
	int axp_exist = 0;

	printf("u0:%x\n", readl(0x44000));
#ifdef CONFIG_SUNXI_ARISC_EXIST
	sunxi_arisc_probe();
#endif

	axp_exist =  axp_probe();
	if (axp_exist) {
		axp_probe_factory_mode();
		if (axp_probe_power_supply_condition()) {
			printf("axp_probe_power_supply_condition error\n");
		}
		gd->vbus_status = axp_probe_vbus_type();
		/* 		set_sunxi_gpio_power_bias(); */
		axp_set_charge_vol_limit();
		axp_set_all_limit();
		axp_set_hardware_poweron_vol();
		axp_set_power_supply_output();

	} else {
		gd->vbus_status = SUNXI_VBUS_PC;
		printf("axp_probe error\n");
	}

	sunxi_clock_set_corepll(uboot_spare_head.boot_data.run_clock);
	pll_cpux = sunxi_clock_get_corepll();
	tick_printf("PMU: cpux %d Mhz, AXI=%d Mhz\n", pll_cpux,
	sunxi_clock_get_axi());
	printf("PLL6=%d Mhz, AHB1=%d Mhz, APB1=%dMhz\n",
	sunxi_clock_get_pll6(), sunxi_clock_get_ahb(), sunxi_clock_get_apb1());

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_probe_securemode(void)
{
	int secure_mode = 0;

	secure_mode =  uboot_spare_head.boot_data.secureos_exist;
	/*sid_get_security_status();*/
	printf("secure enable bit: %d\n", secure_mode);

	if (secure_mode) {
		/* sbrom  set  secureos_exist flag, */
		/* 1: secure os exist 0: secure os not exist */
		if (uboot_spare_head.boot_data.secureos_exist == 1) {
			gd->securemode = SUNXI_SECURE_MODE_WITH_SECUREOS;
			printf("secure mode: with secureos\n");
		} else {
			gd->securemode = SUNXI_SECURE_MODE_NO_SECUREOS;
			printf("secure mode: no secureos\n");
		}
		gd->bootfile_mode = SUNXI_BOOT_FILE_TOC;
	} else {
		/* boot0  set  secureos_exist flag, */
		/* 1: secure monitor exist 0: secure monitor  not exist */
		int burn_secure_mode = 0;

		gd->securemode = SUNXI_NORMAL_MODE;
		gd->bootfile_mode = SUNXI_BOOT_FILE_PKG;
		printf("normal mode\n");

		if (script_parser_fetch("target", "burn_secure_mode",
					&burn_secure_mode, 1))
			return 0;

		if (burn_secure_mode != 1) {
			return 0;
		}
		gd->bootfile_mode = SUNXI_BOOT_FILE_TOC;
	}
	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_set_secure_mode(void)
{
	int mode;

	if ((gd->securemode == SUNXI_NORMAL_MODE) &&
	    (gd->bootfile_mode == SUNXI_BOOT_FILE_TOC)) {
		mode = sid_probe_security_mode();
		if (!mode) {
			sid_set_security_mode();
			gd->bootfile_mode = SUNXI_BOOT_FILE_TOC;
		}
	}

	return 0;
}

/* int sunxi_get_securemode(void)
{
	return gd->securemode;
}

int sunxi_probe_secure_monitor(void)
{
	return uboot_spare_head.boot_data.secureos_exist == SUNXI_SECURE_MODE_USE_SEC_MONITOR?1:0;
} */
