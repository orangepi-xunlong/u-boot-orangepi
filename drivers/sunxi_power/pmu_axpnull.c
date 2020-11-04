/*
 * Copyright (C) 2016 Allwinner.
 * wangwei <wangwei@allwinnertech.com>
 *
 * SUNXI AXPnull  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */


#include <common.h>
#include <sunxi_power/axp.h>
#include <asm/arch/pmic_bus.h>
#ifdef PMU_DEBUG
#define axp_info(fmt...)   tick_printf("[axp][info]: "fmt)
#define axp_err(fmt...)    tick_printf("[axp][err]: "fmt)
#else
#define axp_info(fmt...)	tick_printf("[axp][info]: "fmt)
#define axp_err(fmt...)    tick_printf("[axp][err]: "fmt)
#endif

DECLARE_GLOBAL_DATA_PTR;

static int pmu_axpnull_probe(void)
{
	tick_printf("%s %s %d axpdummy probe\n", __FILE__, __func__, __LINE__);
	gd->vbus_status = AXP_VBUS_EXIST;
	return 0;
}

static int pmu_axpnull_set_voltage(char *name, uint set_vol, uint onoff)
{

	return 0;
}

static int pmu_axpnull_get_voltage(char *name)
{
	return 0;

}

static int pmu_axpnull_set_power_off(void)
{
	return 0;
}

static int pmu_axpnull_set_sys_mode(int status)
{
	return 0;
}

static int pmu_axpnull_get_sys_mode(void)
{
	return 0;
}

static int pmu_axpnull_get_key_irq(void)
{
	return 0;
}


U_BOOT_AXP_PMU_INIT(pmu_axpnull) = {
	.pmu_name		= "pmu_axpnull",
	.probe 			= pmu_axpnull_probe,
	.set_voltage		= pmu_axpnull_set_voltage,
	.get_voltage		= pmu_axpnull_get_voltage,
	.set_power_off		= pmu_axpnull_set_power_off,
	.set_sys_mode		= pmu_axpnull_set_sys_mode,
	.get_sys_mode		= pmu_axpnull_get_sys_mode,
	.get_key_irq		= pmu_axpnull_get_key_irq,
	/*.set_bus_vol_limit	= axpnull_set_bus_vol_limit, */
};


