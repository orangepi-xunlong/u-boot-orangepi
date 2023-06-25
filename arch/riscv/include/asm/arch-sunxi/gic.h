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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.         See the
 * GNU General Public License for more details.
 *
 */

#ifndef __SUNXI_INTC_H__
#define __SUNXI_INTC_H__

#include <linux/types.h>
#include <cpu.h>

#if defined(CONFIG_MACH_SUN20IW1) || defined(CONFIG_MACH_SUN8IW20)
#define AW_IRQ_GIC_START				(32)
#define AW_IRQ_NAND						70
#define AW_IRQ_USB_OTG					61
#define AW_IRQ_USB_EHCI0				62
#define AW_IRQ_USB_OHCI0				63
#define AW_IRQ_DMA						82
#define AW_IRQ_TIMER0					91
#define AW_IRQ_TIMER1					92
#define AW_IRQ_NMI						168
#define GIC_IRQ_NUM                    (223)
#else
#error "Interrupt definition not available for this architecture"
#endif




/* processer target */
#define GIC_CPU_TARGET(_n)	(1 << (_n))
#define GIC_CPU_TARGET0		GIC_CPU_TARGET(0)
#define GIC_CPU_TARGET1		GIC_CPU_TARGET(1)
#define GIC_CPU_TARGET2		GIC_CPU_TARGET(2)
#define GIC_CPU_TARGET3		GIC_CPU_TARGET(3)
#define GIC_CPU_TARGET4		GIC_CPU_TARGET(4)
#define GIC_CPU_TARGET5		GIC_CPU_TARGET(5)
#define GIC_CPU_TARGET6		GIC_CPU_TARGET(6)
#define GIC_CPU_TARGET7		GIC_CPU_TARGET(7)
/* trigger mode */
#define GIC_SPI_LEVEL_TRIGGER	(0)	//2b'00
#define GIC_SPI_EDGE_TRIGGER	(2)	//2b'10



/* GIC registers */
#define GIC_DIST_BASE       (SUNXI_GIC400_BASE + 0x1000)
#define GIC_CPUIF_BASE      (SUNXI_GIC400_BASE + 0x2000)

#define GIC_DIST_CON			(IOMEM_ADDR(GIC_DIST_BASE) + 0x0000)
#define GIC_CON_TYPE			(IOMEM_ADDR(GIC_DIST_BASE) + 0x0004)
#define GIC_CON_IIDR			(IOMEM_ADDR(GIC_DIST_BASE) + 0x0008)

#define GIC_CON_IGRP(n)			(IOMEM_ADDR(GIC_DIST_BASE) + 0x0080 + (n)*4)
#define GIC_SET_EN(_n)			(IOMEM_ADDR(GIC_DIST_BASE) + 0x100 + 4 * (_n))
#define GIC_CLR_EN(_n)			(IOMEM_ADDR(GIC_DIST_BASE) + 0x180 + 4 * (_n))
#define GIC_PEND_SET(_n)		(IOMEM_ADDR(GIC_DIST_BASE) + 0x200 + 4 * (_n))
#define GIC_PEND_CLR(_n)		(IOMEM_ADDR(GIC_DIST_BASE) + 0x280 + 4 * (_n))
#define GIC_ACT_SET(_n)			(IOMEM_ADDR(GIC_DIST_BASE) + 0x300 + 4 * (_n))
#define GIC_ACT_CLR(_n)			(IOMEM_ADDR(GIC_DIST_BASE) + 0x380 + 4 * (_n))
#define GIC_SGI_PRIO(_n)		(IOMEM_ADDR(GIC_DIST_BASE) + 0x400 + 4 * (_n))
#define GIC_PPI_PRIO(_n)		(IOMEM_ADDR(GIC_DIST_BASE) + 0x410 + 4 * (_n))
#define GIC_SPI_PRIO(_n)		(IOMEM_ADDR(GIC_DIST_BASE) + 0x420 + 4 * (_n))
#define GIC_IRQ_MOD_CFG(_n)		(IOMEM_ADDR(GIC_DIST_BASE) + 0xc00 + 4 * (_n))
#define GIC_SPI_PROC_TARG(_n)	(IOMEM_ADDR(GIC_DIST_BASE) + 0x820 + 4 * (_n))

#define GIC_CPU_IF_CTRL			(IOMEM_ADDR(GIC_CPUIF_BASE) + 0x000) // 0x8000
#define GIC_INT_PRIO_MASK		(IOMEM_ADDR(GIC_CPUIF_BASE) + 0x004) // 0x8004
#define GIC_BINARY_POINT		(IOMEM_ADDR(GIC_CPUIF_BASE) + 0x008) // 0x8008
#define GIC_INT_ACK_REG			(IOMEM_ADDR(GIC_CPUIF_BASE) + 0x00c) // 0x800c
#define GIC_END_INT_REG			(IOMEM_ADDR(GIC_CPUIF_BASE) + 0x010) // 0x8010
#define GIC_RUNNING_PRIO		(IOMEM_ADDR(GIC_CPUIF_BASE) + 0x014) // 0x8014
#define GIC_HIGHEST_PENDINT		(IOMEM_ADDR(GIC_CPUIF_BASE) + 0x018) // 0x8018
#define GIC_DEACT_INT_REG		(IOMEM_ADDR(GIC_CPUIF_BASE) + 0x1000)// 0x1000
#define GIC_AIAR_REG			(IOMEM_ADDR(GIC_CPUIF_BASE) + 0x020) // 0x8020
#define GIC_AEOI_REG			(IOMEM_ADDR(GIC_CPUIF_BASE) + 0x024) // 0x8024
#define GIC_AHIGHEST_PENDINT	(IOMEM_ADDR(GIC_CPUIF_BASE) + 0x028) // 0x8028

/* software generated interrupt */
#define GIC_SRC_SGI(_n)		(_n)
/* private peripheral interrupt */
#define GIC_SRC_PPI(_n)		(16 + (_n))
/* external peripheral interrupt */
#define GIC_SRC_SPI(_n)		(32 + (_n))

int  arch_interrupt_init (void);
int  arch_interrupt_exit (void);
int  irq_enable(int irq_no);
int  irq_disable(int irq_no);
void irq_install_handler (int irq, interrupt_handler_t handle_irq, void *data);
void irq_free_handler(int irq);
int  sunxi_gic_cpu_interface_init(int cpu);
int  sunxi_gic_cpu_interface_exit(void);

#endif
