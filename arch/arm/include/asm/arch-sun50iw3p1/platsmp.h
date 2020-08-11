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

#define SUNXI_CLUSTER_PWRON_RESET(cluster)        (SUNXI_CPUS_CFG_BASE + 0x40  + (cluster<<2))
#define SUNXI_CLUSTER_PWROFF_GATING(cluster)      (SUNXI_CPUS_CFG_BASE + 0x44 + (cluster<<2))
#define SUNXI_CPU_PWR_CLAMP(cluster, cpu)         (SUNXI_CPUS_CFG_BASE + 0x50 + (cluster<<4) + (cpu<<2))

#define SUNXI_C0_RST_CTRL(cluster)                (SUNXI_CPUX_CFG_BASE + 0x00 + (cluster<<2))
#define SUNXI_CLUSTER_CTRL0(cluster)              (SUNXI_CPUX_CFG_BASE + 0x10 + (cluster<<4))
#define SUNXI_CLUSTER_CTRL1(cluster)              (SUNXI_CPUX_CFG_BASE + 0x14 + (cluster<<4))
#define SUNXI_CPU_RVBA_L(cpu)                     (SUNXI_CPUX_CFG_BASE + 0x40 + (cpu)*0x8)
#define SUNXI_CPU_RVBA_H(cpu)                     (SUNXI_CPUX_CFG_BASE + 0x44 + (cpu)*0x8)
#define SUNXI_CLUSTER_CPU_STATUS(cluster)         (SUNXI_CPUX_CFG_BASE + 0x80 + (cluster<<2))
#define SUNXI_DBG_REG0                            (SUNXI_CPUX_CFG_BASE + 0xc0)


static inline void sunxi_set_wfi_mode(int cpu)
{
	while(1) {
		asm volatile ("wfi");
	}
}

static inline int sunxi_probe_wfi_mode(int cpu)
{
	return readl(SUNXI_CLUSTER_CPU_STATUS(0)) & (1<<(16 + cpu));
}

static inline int sunxi_probe_cpu_power_status(int cpu)
{
	int val;

	val = readl(SUNXI_CPU_PWR_CLAMP(0, cpu)) & 0xff;
	if (val == 0xff)
		return 0;

	return 1;
}

static inline void sunxi_set_secondary_entry(void *entry)
{
	SUNXI_CPU_RVBA_L(1,entry);
	SUNXI_CPU_RVBA_H(1,0);
}

static int cpu_power_switch_set(u32 cluster, u32 cpu, bool enable)
{
	if (enable) {
		if (0x00 == readl(SUNXI_CPU_PWR_CLAMP(cluster, cpu)))
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
		if (0xFF == readl(SUNXI_CPU_PWR_CLAMP(cluster, cpu)))
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

	/* Assert nCPUPORESET LOW */
	value	= readl(SUNXI_C0_RST_CTRL(0));
	value &= (~(1<<cpu));
	writel(value, SUNXI_C0_RST_CTRL(0));

	/* Assert cpu power-on reset */
	value = readl(SUNXI_CLUSTER_PWRON_RESET(0));
	value &= (~(1<<cpu));
	writel(value, SUNXI_CLUSTER_PWRON_RESET(0));

	/* Apply power to the PDCPU power domain. */
	cpu_power_switch_set(0, cpu, 1);

	/* Release the core output clamps */
	value = readl(SUNXI_CLUSTER_PWROFF_GATING(0));
	value &= (~(0x1<<cpu));
	writel(value, SUNXI_CLUSTER_PWROFF_GATING(0));
	__asm volatile ("isb");
	__asm volatile ("isb");
	__usdelay(1);

	/* Deassert cpu power-on reset */
	value	= readl(SUNXI_CLUSTER_PWRON_RESET(0));
	value |= ((1<<cpu));
	writel(value, SUNXI_CLUSTER_PWRON_RESET(0));

	/* Deassert core reset */
	value	= readl(SUNXI_CPU_RST_CTRL(0));
	value |= (1<<cpu);
	writel(value, SUNXI_CPU_RST_CTRL(0));

	/* Assert DBGPWRDUP HIGH */
	value = readl(SUNXI_DBG_REG0);
	value |= (1<<cpu);
	writel(value, SUNXI_DBG_REG0);
}

static inline void sunxi_disable_cpu(int cpu)
{
	unsigned int value;

	/* Deassert DBGPWRDUP HIGH */
	value = readl(SUNXI_DBG_REG0);
	value &= (~(1<<cpu));
	writel(value, SUNXI_DBG_REG0);
	__usdelay(10);

	/* step8: Activate the core output clamps */
	value = readl(SUNXI_CLUSTER_PWROFF_GATING(0));
	value |= (1 << cpu);
	writel(value, SUNXI_CLUSTER_PWROFF_GATING(0));
	__usdelay(20);

	/* step9: Assert nCPUPORESET LOW */
	value	= readl(SUNXI_CLUSTER_PWRON_RESET(0));
	value &= (~(1<<cpu));
	writel(value, SUNXI_CLUSTER_PWRON_RESET(0));
	__usdelay(10);

	/* step10: Remove power from th e PDCPU power domain */
	cpu_power_switch_set(0, cpu, 0);
}

#endif /* __PLAT_SMP_H */
