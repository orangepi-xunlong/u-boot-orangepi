/*
 * arch/arm/mach-sunxi/platsmp.h
 *
 * Copyright(c) 2013-2015 Allwinnertech Co., Ltd.
 *      http://www.allwinnertech.com
 *
 * Author: liugang <liugang@allwinnertech.com>
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

#define SUNXI_MUL_CORE_ENTRY

#define SUNXI_CLUSTER_PWROFF_GATING(cluster)			(SUNXI_RCPUCFG_BASE + 0x44 + (cluster<<6))
#define SUNXI_CPU_PWR_CLAMP(cluster, cpu)			(SUNXI_RCPUCFG_BASE + 0x50 + (cluster<<6) + (cpu<<2))
#define SUNXI_CLUSTER_PWRON_RESET(cluster)			(SUNXI_RCPUCFG_BASE + 0x40  + (cluster<<6))
#define SUNXI_CPU_SUBSYS_RESET					(SUNXI_RCPUCFG_BASE + 0xA0)
/*#define SUNXI_CPU_ENTRY(cpu)					(SUNXI_RCPUCFG_BASE + 0x1C4 + (cpu << 2))*/
#define SUNXI_CPU_ENTRY						(SUNXI_RCPUCFG_BASE + 0x1C4)

#define SUNXI_CLUSTER_RST_CTRL					(SUNXI_CPUXCFG_BASE + 0x00)
#define SUNXI_CLUSTER_CTRL					(SUNXI_CPUXCFG_BASE + 0x10)
#define SUNXI_CLUSTER_CPU_STATUS				(SUNXI_CPUXCFG_BASE + 0x80)
#define SUNXI_CLUSTER_DBG_REG0					(SUNXI_CPUXCFG_BASE + 0xc0)


static inline void sunxi_set_wfi_mode(int cpu)
{
	printf("wfi");
	while(1) {
		asm volatile ("wfi");
	}
}

static inline int sunxi_probe_wfi_mode(int cpu)
{
	return readl(SUNXI_CLUSTER_CPU_STATUS) & (1<<(16 + cpu));
}


static inline void sunxi_set_secondary_entry(void *entry)
{
	writel((u32)entry, SUNXI_CPU_ENTRY);
}

static inline int sunxi_probe_cpu_power_status(int cpu)
{
	int val;

	val = readl(SUNXI_CPU_PWR_CLAMP(0, cpu)) & 0xff;
	if (val == 0xff)
		return 0;

	return 1;
}

static int cpu_power_switch_set(u32 cluster, u32 cpu, bool enable)
{
	if (enable) {
		if (0x00 == (readl(SUNXI_CPU_PWR_CLAMP(cluster, cpu)) & 0xff))
			return 0;

		/* de-active cpu power clamp */
		writel(0xFE, SUNXI_CPU_PWR_CLAMP(cluster, cpu));
		__usdelay(20);

		writel(0xF8, SUNXI_CPU_PWR_CLAMP(cluster, cpu));
		__usdelay(10);

		writel(0xE0, SUNXI_CPU_PWR_CLAMP(cluster, cpu));
		__usdelay(10);

		writel(0xc0, SUNXI_CPU_PWR_CLAMP(cluster, cpu));
		__usdelay(10);

		writel(0x80, SUNXI_CPU_PWR_CLAMP(cluster, cpu));
		__usdelay(10);

		writel(0x00, SUNXI_CPU_PWR_CLAMP(cluster, cpu));
		__usdelay(20);
		while(0x00 != readl(SUNXI_CPU_PWR_CLAMP(cluster, cpu)))
			;
	} else {
		if (0xFF == (readl(SUNXI_CPU_PWR_CLAMP(cluster, cpu)) & 0xff))
			return 0;

		writel(0xFF, SUNXI_CPU_PWR_CLAMP(cluster, cpu));
		__usdelay(30);
		while(0xFF != readl(SUNXI_CPU_PWR_CLAMP(cluster, cpu)));

	}
	return 0;
}

static inline void sunxi_enable_cpu(int cpu)
{

	unsigned int value;

	/*pr_debug("[%s]: start\n", __func__);*/

	/* assert cpu core reset low */
	value = readl(SUNXI_CLUSTER_RST_CTRL);
	value &= (~(0x1 << cpu));
	writel(value, SUNXI_CLUSTER_RST_CTRL);

	/* assert power on reset low */
	value = readl(SUNXI_CLUSTER_PWRON_RESET(0));
	value &= (~(0x1 << cpu));
	writel(value, SUNXI_CLUSTER_PWRON_RESET(0));

	/* Apply power to the PDCPU power domain. */
	cpu_power_switch_set(0, cpu, 1);

	/* Clear power-off gating */
	/* Release the core output clamps */
	value = readl(SUNXI_CLUSTER_PWROFF_GATING(0));
	value &= (~(0x1 << cpu));
	writel(value, SUNXI_CLUSTER_PWROFF_GATING(0));
	__asm volatile ("isb");
	__asm volatile ("isb");
	__usdelay(1);

	/* Deassert power on reset high */
	value = readl(SUNXI_CLUSTER_PWRON_RESET(0));
	value |= (0x1 << cpu);
	writel(value, SUNXI_CLUSTER_PWRON_RESET(0));

	/* Deassert core reset high */
	value = readl(SUNXI_CLUSTER_RST_CTRL);
	value |= (0x1 << cpu);
	writel(value, SUNXI_CLUSTER_RST_CTRL);

	/* Assert DBGPWRDUPx high */
	value = readl(SUNXI_CLUSTER_DBG_REG0);
	value |= (0x1 << cpu);
	writel(value, SUNXI_CLUSTER_DBG_REG0);

	/*pr_debug("[%s]: end\n", __func__);  */
}

static inline void sunxi_disable_cpu(int cpu)
{
	unsigned int value;

	/*pr_debug("[%s]: start\n", __func__);*/

	/* Deassert DBGPWRDUPx low */
	value = readl(SUNXI_CLUSTER_DBG_REG0);
	value &= (~(0x1 << cpu));
	writel(value, SUNXI_CLUSTER_DBG_REG0);
	__usdelay(10);

	/* step8: Activate the core output clamps */
	value = readl(SUNXI_CLUSTER_PWROFF_GATING(0));
	value |= (0x1 << cpu);
	writel(value, SUNXI_CLUSTER_PWROFF_GATING(0));
	udelay(20);

	/* step9: Assert nCPUPORESET LOW */
	value	= readl(SUNXI_CLUSTER_PWRON_RESET(0));
	value &= (~(0x1 << cpu));
	writel(value, SUNXI_CLUSTER_PWRON_RESET(0));
	__usdelay(10);

	/* step10: Remove power from th e PDCPU power domain */
	cpu_power_switch_set(0, cpu, 0);

    /* pr_debug("[%s]: end\n", __func__);*/
}
#endif /* ___PLAT_SMP_H__ */

