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


#define SUNXI_CLUSTER_PWROFF_GATING(cluster) \
	(SUNXI_RCPUCFG_BASE + 0x44 + (cluster<<2))
#define SUNXI_CPU_PWR_CLAMP(cluster, cpu)         \
	(SUNXI_RCPUCFG_BASE + 0x50 + (cluster<<4) + (cpu<<2))

#define SUNXI_CLUSTER_CTRL0(cluster)             \
	(SUNXI_CPUX_CFG_BASE + 0x00 + (cluster<<4))
#define SUNXI_CLUSTER_CTRL1(cluster)             \
	(SUNXI_CPUX_CFG_BASE + 0x04 + (cluster<<4))
#define SUNXI_CLUSTER_CPU_STATUS(cluster)        \
	(SUNXI_CPUX_CFG_BASE + 0x30 + (cluster<<2))
#define SUNXI_CPU_RST_CTRL(cluster)              \
	(SUNXI_CPUX_CFG_BASE + 0x80 + (cluster<<2))
#define SUNXI_DBG_REG0                           \
	(SUNXI_CPUX_CFG_BASE + 0x20)
#define SUNXI_CPU_RVBA_L(cpu)                    \
	(SUNXI_CPUX_CFG_BASE + 0xA0 + (cpu)*0x8)
#define SUNXI_CPU_RVBA_H(cpu)                    \
(SUNXI_CPUX_CFG_BASE + 0xA4 + (cpu)*0x8)

#define SUNXI_CPU_ENTRY                          \
	(SUNXI_RTC_BASE + 0x1bc)

#define SUNXI_CLUSTER_PWRON_RESET(cluster)       \
	(SUNXI_RCPUCFG_BASE + 0x30  + (cluster<<2))
#define SUNXI_CPU_SYS_RESET                      \
	(SUNXI_RCPUCFG_BASE + 0x140)


#define C0_RST_CTRL			(0x00)
#define C0_CTRL_REG0			(0x10)
#define C0_CPU_STATUS			(0X80)
#define DBG_REG0			(0xc0)

#define C0_CPUX_RESET_REG		(0x40)
#define C0_CPUX_PWROFF_GATING		(0x44)
#define C0_CPUX_PWR_SW(cpu)		(0x50 + cpu * 4)

#define CORE_RESET_OFFSET		(0x00)
#define C0_CPUX_RESET_OFFSET		(0x00)
#define L1_RST_DISABLE_OFFSET		(0x00)
#define POROFF_GATING_CPUX_OFFSET	(0x00)
#define C_DBGPWRDUP_OFFSET		(0x00)
#define STANDBYWFI_OFFSET		(0x10)


static inline void sunxi_set_wfi_mode(int cpu)
{
	printf("wfi");
	while(1) {
		asm volatile ("wfi");
	}
}

static inline int sunxi_probe_wfi_mode(int cpu)
{
	return readl(SUNXI_CPUX_CFG_BASE + C0_CPU_STATUS) &
		(1 << (STANDBYWFI_OFFSET + cpu));
}


static inline void sunxi_set_secondary_entry(void *entry)
{
	writel((u32)entry, SUNXI_CPU_ENTRY);
	asm volatile("dmb");
	flush_cache((u32)entry, sizeof(entry));
}

static inline int sunxi_probe_cpu_power_status(int cpu)
{
	int val;

	val = readl(SUNXI_RCPUCFG_BASE + C0_CPUX_PWR_SW(cpu));
	if (val == 0xff)
		return 0;

	return 1;
}

static inline void sunxi_enable_cpu(int cpu)
{
	unsigned int value;

	/*pr_debug("[%s]: start\n", __func__);*/

	/* assert cpu core reset low */
	value = readl(SUNXI_CPUX_CFG_BASE + C0_RST_CTRL);
	value &= (~(0x1 << (CORE_RESET_OFFSET + cpu)));
	writel(value, SUNXI_CPUX_CFG_BASE + C0_RST_CTRL);

	/* assert power on reset low */
	value = readl(SUNXI_RCPUCFG_BASE + C0_CPUX_RESET_REG);
	value &= (~(0x1 << (C0_CPUX_RESET_OFFSET + cpu)));
	writel(value, SUNXI_RCPUCFG_BASE + C0_CPUX_RESET_REG);

	/* L1RSTDISABLE hold low */
	value = readl(SUNXI_CPUX_CFG_BASE + C0_CTRL_REG0);
	value &= (~(0x1 << (L1_RST_DISABLE_OFFSET + cpu)));
	writel(value, SUNXI_CPUX_CFG_BASE + C0_CTRL_REG0);

	/* DBGPWRDUPx hold low */
	value = readl(SUNXI_CPUX_CFG_BASE + DBG_REG0);
	value &= (~(0x1 << (C_DBGPWRDUP_OFFSET + cpu)));
	writel(value, SUNXI_CPUX_CFG_BASE + DBG_REG0);

	/* hold power-off gating */
	value = readl(SUNXI_RCPUCFG_BASE + C0_CPUX_PWROFF_GATING);
	value |= ((0x1 << (POROFF_GATING_CPUX_OFFSET + cpu)));
	writel(value, (SUNXI_RCPUCFG_BASE + C0_CPUX_PWROFF_GATING));

	/* Release power switch */
	writel(0xFE, SUNXI_RCPUCFG_BASE + C0_CPUX_PWR_SW(cpu));
	udelay(20);
	writel(0xFC, SUNXI_RCPUCFG_BASE + C0_CPUX_PWR_SW(cpu));
	udelay(10);
	writel(0xF8, SUNXI_RCPUCFG_BASE + C0_CPUX_PWR_SW(cpu));
	udelay(10);
	writel(0xF0, SUNXI_RCPUCFG_BASE + C0_CPUX_PWR_SW(cpu));
	udelay(10);
	writel(0xC0, SUNXI_RCPUCFG_BASE + C0_CPUX_PWR_SW(cpu));
	udelay(10);
	writel(0x00, SUNXI_RCPUCFG_BASE + C0_CPUX_PWR_SW(cpu));
	udelay(20);
	while (readl(SUNXI_RCPUCFG_BASE + C0_CPUX_PWR_SW(cpu)) != 0x00)
		;

	/* Clear power-off gating */
	value = readl(SUNXI_RCPUCFG_BASE + C0_CPUX_PWROFF_GATING);
	value &= (~(0x1 << (POROFF_GATING_CPUX_OFFSET + cpu)));
	writel(value, (SUNXI_RCPUCFG_BASE + C0_CPUX_PWROFF_GATING));

	udelay(20);

	/* Deassert power on reset high */
	value = readl(SUNXI_RCPUCFG_BASE + C0_CPUX_RESET_REG);
	value |= (0x1 << (C0_CPUX_RESET_OFFSET + cpu));
	writel(value, SUNXI_RCPUCFG_BASE + C0_CPUX_RESET_REG);

	/* Deassert core reset high */
	value = readl(SUNXI_CPUX_CFG_BASE + C0_RST_CTRL);
	value |= (0x1 << (CORE_RESET_OFFSET + cpu));
	writel(value, SUNXI_CPUX_CFG_BASE + C0_RST_CTRL);

	/* Assert DBGPWRDUPx high */
	value = readl(SUNXI_CPUX_CFG_BASE + DBG_REG0);
	value |= (0x1 << (C_DBGPWRDUP_OFFSET + cpu));
	writel(value, SUNXI_CPUX_CFG_BASE + DBG_REG0);

	/*pr_debug("[%s]: end\n", __func__);  */
}

static inline void sunxi_disable_cpu(int cpu)
{
	unsigned int value;

	/*pr_debug("[%s]: start\n", __func__);*/

	/* assert cpu core reset */
	value = readl(SUNXI_CPUX_CFG_BASE + C0_RST_CTRL);
	value &= (~(0x1 << (CORE_RESET_OFFSET + cpu)));
	writel(value, SUNXI_CPUX_CFG_BASE + C0_RST_CTRL);

	/* Deassert DBGPWRDUPx low */
	value = readl(SUNXI_CPUX_CFG_BASE + DBG_REG0);
	value &= (~(0x1 << (C_DBGPWRDUP_OFFSET + cpu)));
	writel(value, SUNXI_CPUX_CFG_BASE + DBG_REG0);

	/* power gating off */
	value = readl(SUNXI_RCPUCFG_BASE + C0_CPUX_PWROFF_GATING);
	value |= (0x1 << (POROFF_GATING_CPUX_OFFSET + cpu));
	writel(value, (SUNXI_RCPUCFG_BASE + C0_CPUX_PWROFF_GATING));

	udelay(20);

	/* power switch off */
	writel(0xff, SUNXI_RCPUCFG_BASE + C0_CPUX_PWR_SW(cpu));

	udelay(30);

	while (readl(SUNXI_RCPUCFG_BASE + C0_CPUX_PWR_SW(cpu)) != 0xff)
		;

	/* pr_debug("[%s]: end\n", __func__);*/
}
#endif /* ___PLAT_SMP_H__ */

