// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <axp_pmic.h>
#include <errno.h>



#ifdef CONFIG_DISPLAY_CPUINFO
int print_cpuinfo(void)
{
	tick_printf("CPU:   Allwinner Family\n");
	return 0;
}
#endif



int sunxi_get_sid(unsigned int *sid)
{
#ifdef SUNXI_SID_SRAM_BASE
	int i;
	for (i = 0; i< 4; i++)
		sid[i] = readl((ulong)SUNXI_SID_SRAM_BASE + 4 * i);
	return 0;
#else
	return -ENODEV;
#endif
}
