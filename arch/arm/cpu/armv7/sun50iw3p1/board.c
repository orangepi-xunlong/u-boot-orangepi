/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/timer.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/clock.h>
#include <efuse_map.h>
#include <asm/arch/platform.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/key.h>
#include <asm/arch/dma.h>

#include <boot_type.h>
#include <sys_partition.h>
#include <sys_config.h>
#include <power/sunxi/pmu.h>
#include <power/sunxi/power.h>
#include <smc.h>
#include <sunxi_board.h>
#include <fdt_support.h>
#include <sys_config_old.h>
#include <arisc.h>

extern int sunxi_arisc_probe(void);
extern int set_sunxi_gpio_power_bias(void);


/* The sunxi internal brom will try to loader external bootloader
 * from mmc0, nannd flash, mmc2.
 * We check where we boot from by checking the config
 * of the gpio pin.
 */
DECLARE_GLOBAL_DATA_PTR;

__weak int axp_probe_id(int pmu_id)
{
	return 0;
}

int power_source_init(void)
{
	int pmu_id;

#ifdef CONFIG_SUNXI_ARISC_EXIST
	sunxi_arisc_wait_ready();
#endif
	pmu_id = uboot_spare_head.boot_ext[0].data[0];
	printf("pmu type = 0x%x\n", pmu_id);

	axp_probe_id(0);
	if (pmu_id > 0) {

		printf("factory\n");
		axp_probe_factory_mode();
		gd->pmu_saved_status = axp_probe_pre_sys_mode();
		printf("axp end\n");

		return 0;
	}
#if 0
	int pll_cpux;
	uint32_t dcdc_vol = 0;
	int cpu_vol = 0;
	int nodeoffset=0;
	//PMU_SUPPLY_DCDC2 is for cpua
	nodeoffset =  fdt_path_offset(working_fdt,FDT_PATH_POWER_SPLY);
	if(nodeoffset >=0)
	{
		fdt_getprop_u32(working_fdt, nodeoffset, "dcdc2_vol", &dcdc_vol);
	}
	if(!dcdc_vol)
	{
		cpu_vol = 900;
	}
	else
	{
		cpu_vol = dcdc_vol%10000;
	}

	if(!axp_probe(0))
	axp_probe_id(0);
	{
		axp_probe_factory_mode();
		axp_probe_power_supply_condition();
		set_sunxi_gpio_power_bias();
	}
	else
	{
		printf("axp_probe error\n");
	}

	pll_cpux = sunxi_clock_get_corepll();
	tick_printf("PMU: cpux %d Mhz,AXI=%d Mhz\n", pll_cpux,sunxi_clock_get_axi());
	printf("PLL6=%d Mhz,AHB1=%d Mhz, APB1=%dMhz AHB2=%dMhz MBus=%dMhz\n", sunxi_clock_get_pll6(),
		sunxi_clock_get_ahb(),
		sunxi_clock_get_apb(),
		sunxi_clock_get_ahb2(),
	if(axp_exist)
	{
		axp_set_charge_vol_limit();
		axp_set_all_limit();
		axp_set_hardware_poweron_vol();
		axp_set_power_supply_output();
		//power_config_gpio_bias();
		power_limit_init();
	}
#endif
	return 0;
}

int sunxi_probe_securemode(void)
{
	int secure_mode = 0;

	secure_mode =  sid_get_security_status();
	printf("secure enable bit: %d\n", secure_mode);

	if(secure_mode)
	{
		//sbrom  set  secureos_exist flag,
		//1: secure os exist 0: secure os not exist
		if(uboot_spare_head.boot_data.secureos_exist==1)
		{
			gd->securemode = SUNXI_SECURE_MODE_WITH_SECUREOS;
			printf("secure mode: with secureos\n");
		}
		else
		{
			gd->securemode = SUNXI_SECURE_MODE_NO_SECUREOS;
			printf("secure mode: no secureos\n");
		}
		gd->bootfile_mode = SUNXI_BOOT_FILE_TOC;
	}
	else
	{
		//boot0  set  secureos_exist flag,
		//1: secure monitor exist 0: secure monitor  not exist
		int burn_secure_mode=0;

		gd->securemode = SUNXI_NORMAL_MODE;
		gd->bootfile_mode = SUNXI_BOOT_FILE_PKG;
		printf("normal mode: with secure monitor\n");

		if (script_parser_fetch("target", "burn_secure_mode", &burn_secure_mode, 1))
			return 0;

		if(burn_secure_mode != 1)
		{
			return 0;
		}
		gd->bootfile_mode = SUNXI_BOOT_FILE_TOC;
	}
	return 0;
}

int sunxi_set_secure_mode(void)
{
	int mode;

	if ((gd->securemode == SUNXI_NORMAL_MODE) &&
		(gd->bootfile_mode == SUNXI_BOOT_FILE_TOC))
	{
		mode = sid_probe_security_mode();
		if(!mode)
		{
			sid_set_security_mode();
			gd->bootfile_mode = SUNXI_BOOT_FILE_TOC;
		}
	}

	return 0;
}

int sunxi_get_securemode(void)
{
	return gd->securemode;
}

int sunxi_probe_secure_monitor(void)
{
	return uboot_spare_head.boot_data.secureos_exist == SUNXI_SECURE_MODE_USE_SEC_MONITOR?1:0;
}


