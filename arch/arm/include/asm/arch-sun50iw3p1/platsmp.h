/*
 * arch/arm/mach-sunxi/platsmp.h
 *
 * Copyright(c) 2017-2018 Allwinnertech Co., Ltd.
 *      http://www.allwinnertech.com
 *
 * Author: wangwei <wangwei@allwinnertech.com>
 *
 * sunxi smp ops header file
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __PLAT_SMP_H
#define __PLAT_SMP_H

#include "platform.h"
#include <asm/arch/timer.h>
#include <asm/io.h>

#define SUNXI_CPU_PWR_SWITCH(cluster, cpu)    (SUNXI_CPUS_CFG_BASE + 0x50 + (cluster<<4) + (cpu<<2))

static inline int sunxi_probe_cpu_power_status(int cpu)
{
	int val;

	val = readl(SUNXI_CPU_PWR_SWITCH(0, cpu)) & 0xff;
	if (val == 0xff)
		return 0;

	return 1;
}


#endif /* __PLAT_SMP_H */
