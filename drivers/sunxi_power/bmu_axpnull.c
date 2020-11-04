/*
 * Copyright (C) 2019 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SUNXI AXP  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <sunxi_power/axp.h>
#include <asm/arch/pmic_bus.h>

static int bmu_axpnull_probe(void)
{
	tick_printf("BMU: AXPNULL\n");
	return 0;
}

int bmu_axpnull_set_power_off(void)
{
	return 0;
}

int bmu_axpnull_get_poweron_source(void)
{
	return 0;
}

int bmu_axpnull_set_coulombmeter_onoff(int onoff)
{
	return 0;
}

int bmu_axpnull_get_axp_bus_exist(void)
{
	return 0;
}

int bmu_axpnull_get_battery_vol(void)
{
	return 0;
}

int bmu_axpnull_get_battery_capacity(void)
{
	return 0;
}

int bmu_axpnull_get_battery_probe(void)
{
	return 0;
}

int bmu_axpnull_set_vbus_current_limit(int current)
{
	return 0;
}

int bmu_axpnull_get_vbus_current_limit(void)
{
	return 0;
}
int bmu_axpnull_set_charge_current_limit(int current)
{
	return 0;
}

unsigned char bmu_axpnull_get_reg_value(unsigned char reg_addr)
{
	return 0;
}

unsigned char bmu_axpnull_set_reg_value(unsigned char reg_addr, unsigned char reg_value)
{
	return 0;
}


U_BOOT_AXP_BMU_INIT(bmu_axpnull) = {
	.bmu_name		  = "bmu_axpnull",
	.probe			  = bmu_axpnull_probe,
	.set_power_off		  = bmu_axpnull_set_power_off,
	.get_poweron_source       = bmu_axpnull_get_poweron_source,
	.get_axp_bus_exist	= bmu_axpnull_get_axp_bus_exist,
	.set_coulombmeter_onoff   = bmu_axpnull_set_coulombmeter_onoff,
	.get_battery_vol	  = bmu_axpnull_get_battery_vol,
	.get_battery_capacity     = bmu_axpnull_get_battery_capacity,
	.get_battery_probe	= bmu_axpnull_get_battery_probe,
	.set_vbus_current_limit   = bmu_axpnull_set_vbus_current_limit,
	.get_vbus_current_limit   = bmu_axpnull_get_vbus_current_limit,
	.set_charge_current_limit = bmu_axpnull_set_charge_current_limit,
	.get_reg_value	   = bmu_axpnull_get_reg_value,
	.set_reg_value	   = bmu_axpnull_set_reg_value,
};
