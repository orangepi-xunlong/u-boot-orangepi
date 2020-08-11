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
#include <power/sunxi/pmu.h>
#include <asm/arch/timer.h>
#include <asm/arch/key.h>
#include <asm/arch/clock.h>
#include <asm/arch/sys_proto.h>
#include <boot_type.h>
#include <sys_partition.h>
#include <sys_config.h>
#include <asm/arch/dma.h>
#include <fdt_support.h>
#include <sys_config_old.h>

#include <asm/arch/ccmu.h>
#include <asm/arch/clock.h>
#include <asm/arch/platform.h>
#include <power/sunxi/pmu.h>
#include <smc.h>
#include <sunxi_board.h>

/* The sunxi internal brom will try to loader external bootloader
 * from mmc0, nannd flash, mmc2.
 * We check where we boot from by checking the config
 * of the gpio pin.
 */
DECLARE_GLOBAL_DATA_PTR;

extern int sunxi_clock_get_axi(void);
extern int sunxi_clock_get_ahb(void);
extern int sunxi_clock_get_apb(void);
extern int sunxi_clock_get_pll6(void);


int power_source_init(void)
{
	int pll1;
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
		cpu_vol = 1200;
	}
	else
	{
		cpu_vol = dcdc_vol%10000;
	}

	if(!axp_probe())
	{
		axp_probe_factory_mode();
		if(!axp_probe_power_supply_condition())
		{
			if(!axp_set_supply_status(0, PMU_SUPPLY_DCDC2, cpu_vol, -1))
			{
				tick_printf("PMU: dcdc2 %d\n", cpu_vol);
				sunxi_clock_set_corepll(uboot_spare_head.boot_data.run_clock, 0);
			}
			else
			{
				printf("axp_set_dcdc2 fail\n");
			}
		}
		else
		{
			printf("axp_probe_power_supply_condition error\n");
		}
	}
	else
	{
		printf("axp_probe error\n");
	}

	pll1 = sunxi_clock_get_corepll();
	tick_printf("IC Version: %d(0:A 1:B 2:other)\n", readl(0x01C00000+0x24)&0x3);
	tick_printf("PMU: pll1 %d Mhz,PLL6=%d Mhz\n", pll1,sunxi_clock_get_pll6());
    printf("AXI=%d Mhz,AHB=%d Mhz, APB1=%d Mhz \n", sunxi_clock_get_axi(),sunxi_clock_get_ahb(),sunxi_clock_get_apb());


    axp_set_charge_vol_limit();
    axp_set_all_limit();
    axp_set_hardware_poweron_vol();

	axp_set_power_supply_output();

	//power_limit_init();
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
	gd->securemode = SUNXI_NORMAL_MODE;
	gd->bootfile_mode = SUNXI_BOOT_FILE_PKG;
	printf("normal mode\n");

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
	return 0;
}


