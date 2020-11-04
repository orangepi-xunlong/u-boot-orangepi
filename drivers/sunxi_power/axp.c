/*
 * Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
*/

#include <common.h>
#include <sys_config.h>
#include <sunxi_power/axp.h>

DECLARE_GLOBAL_DATA_PTR;

int do_poweroff(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#ifdef CONFIG_SUNXI_PMU
	pmu_set_power_off();
#else
	#error "NO FOUND PMU"
#endif
#ifdef CONFIG_SUNXI_BMU
	bmu_set_power_off();
#endif
	asm volatile("b .");
	return 0;
}

int axp_probe(void)
{
	int ret = 0;
#ifdef CONFIG_SUNXI_PMU
		ret = pmu_probe();
#else
	#error "NO FOUND PMU"
#endif
#ifdef CONFIG_SUNXI_BMU
		ret = bmu_probe();
#endif
	return ret;
}




