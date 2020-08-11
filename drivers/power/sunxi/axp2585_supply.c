/*
 * Copyright (C) 2016 Allwinner.
 * wangwei <wangwei@allwinnertech.com>
 *
 * SUNXI AXP1506  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <power/sunxi/axp2585_reg.h>
#include <power/sunxi/axp.h>
#include <power/sunxi/pmu.h>

int axp2585_set_supply_status(int vol_name, int vol_value, int onoff)
{
	return 0;
}

int axp2585_set_supply_status_byname(char *vol_name, int vol_value, int onoff)
{
	return 0;
}

int axp2585_probe_supply_status(int vol_name, int vol_value, int onoff)
{
	return 0;
}

int axp2585_probe_supply_status_byname(char *vol_name)
{
	return 0;
}
