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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __SUNXI_INTC_H__
#define __SUNXI_INTC_H__

#include <linux/types.h>
#include <cpu.h>

#if defined(CONFIG_MACH_SUN50IW3)
#define AW_IRQ_GIC_START    (32)
#define AW_IRQ_NAND                    64
#define AW_IRQ_USB_OTG                 54
#define AW_IRQ_USB_EHCI0               55
#define AW_IRQ_USB_OHCI0               56
#define AW_IRQ_DMA                     70
#define AW_IRQ_TIMER0                  75
#define AW_IRQ_TIMER1                  76
#define AW_IRQ_NMI                     112
#define GIC_IRQ_NUM                    (147)

#elif defined(CONFIG_MACH_SUN50IW5)
#define AW_IRQ_GIC_START    (32)
#define AW_IRQ_NAND                    66
#define AW_IRQ_USB_OTG                 57
#define AW_IRQ_USB_EHCI0               58
#define AW_IRQ_USB_OHCI0               59
#define AW_IRQ_DMA                     74
#define AW_IRQ_TIMER0                  80
#define AW_IRQ_TIMER1                  81
#define AW_IRQ_NMI                     135
#define AW_IRQ_CIR		       138
#define GIC_IRQ_NUM                    (191)

#elif defined(CONFIG_MACH_SUN50IW9)
#define AW_IRQ_GIC_START    (32)
#define AW_IRQ_NAND                    66
#define AW_IRQ_USB_OTG                 57
#define AW_IRQ_USB_EHCI0               58
#define AW_IRQ_USB_OHCI0               59
#define AW_IRQ_DMA                     74
#define AW_IRQ_TIMER0                  80
#define AW_IRQ_TIMER1                  81
#define AW_IRQ_NMI                     135
#define AW_IRQ_CIR		       138
#define GIC_IRQ_NUM                    (191)

#elif defined(CONFIG_MACH_SUN50IW10)
#define AW_IRQ_GIC_START    (32)
#define AW_IRQ_NAND                    70
#define AW_IRQ_USB_OTG                 64
#define AW_IRQ_USB_EHCI0               62
#define AW_IRQ_USB_OHCI0               63
#define AW_IRQ_USB_EHCI1               65
#define AW_IRQ_USB_OHCI1               66
#define AW_IRQ_DMA                     77
#define AW_IRQ_TIMER0                  83
#define AW_IRQ_TIMER1                  84
#define AW_IRQ_NMI                     135
#define GIC_IRQ_NUM                    (191)

#elif defined(CONFIG_MACH_SUN55IW3)
#define AW_IRQ_GIC_START    (32)
#define AW_IRQ_NAND                    70
#define AW_IRQ_USB_OTG                 61
#define AW_IRQ_USB_EHCI0               62
#define AW_IRQ_USB_OHCI0               63
#define AW_IRQ_USB_EHCI1               64
#define AW_IRQ_USB_OHCI1               65
#define AW_IRQ_DMA                     82
#define AW_IRQ_TIMER0                  87
#define AW_IRQ_TIMER1                  88
#define AW_IRQ_NMI                     180
#define GIC_IRQ_NUM                    (287)

#elif defined(CONFIG_MACH_SUN50IW11)
#define AW_IRQ_GIC_START				(32)
#define AW_IRQ_NAND						70
#define AW_IRQ_USB_OTG					61
#define AW_IRQ_USB_EHCI0				62
#define AW_IRQ_USB_OHCI0				63
#define AW_IRQ_DMA						82
#define AW_IRQ_TIMER0					91
#define AW_IRQ_TIMER1					92
#define AW_IRQ_NMI						120
#define GIC_IRQ_NUM                    (191)

#elif defined(CONFIG_MACH_SUN50IW12)
#define AW_IRQ_GIC_START				(32)
#define AW_IRQ_NAND						70
#define AW_IRQ_USB_OTG					64
#define AW_IRQ_USB_EHCI0				62
#define AW_IRQ_USB_OHCI0				63
#define AW_IRQ_USB_EHCI1				65
#define AW_IRQ_USB_OHCI1				66
#define AW_IRQ_DMA						82
#define AW_IRQ_TIMER0					80
#define AW_IRQ_TIMER1					81
#define AW_IRQ_NMI						120
#define GIC_IRQ_NUM                    (191)

#elif defined(CONFIG_MACH_SUN20IW1) || defined(CONFIG_MACH_SUN8IW20)
#define AW_IRQ_GIC_START				(32)
#define AW_IRQ_NAND						70
#define AW_IRQ_USB_OTG					61
#define AW_IRQ_USB_EHCI0				62
#define AW_IRQ_USB_OHCI0				63
#define AW_IRQ_USB_EHCI1				65
#define AW_IRQ_USB_OHCI1				66
#define AW_IRQ_DMA						82
#define AW_IRQ_TIMER0					91
#define AW_IRQ_TIMER1					92
#define AW_IRQ_NMI						168
#define GIC_IRQ_NUM                    (223)

#elif defined(CONFIG_MACH_SUN8IW15)

#define AW_IRQ_NAND                    (60)
#define AW_IRQ_USB_OTG                 (85)
#define AW_IRQ_USB_EHCI0               (86)
#define AW_IRQ_USB_OHCI0               (87)
#define AW_IRQ_DMA                     (42)
#define AW_IRQ_TIMER0                  (82)
#define AW_IRQ_TIMER1                  (83)
#define AW_IRQ_NMI                     (116)
#define GIC_IRQ_NUM                    (152)



#elif defined(CONFIG_MACH_SUN8IW18)

#define AW_IRQ_NAND                    (95)
#define AW_IRQ_USB_OTG                 (69)
#define AW_IRQ_USB_EHCI0               (70)
#define AW_IRQ_USB_OHCI0               (71)
#define AW_IRQ_DMA                     (66)
#define AW_IRQ_TIMER0                  (117)
#define AW_IRQ_TIMER1                  (118)
#define AW_IRQ_NMI                     (64)
#define GIC_IRQ_NUM                    (168)

#elif defined(CONFIG_MACH_SUN8IW16) || defined(CONFIG_MACH_SUN8IW19)

#define AW_IRQ_NAND                    (61)
#define AW_IRQ_USB_OTG                 (96)
#define AW_IRQ_USB_EHCI0               (97)
#define AW_IRQ_USB_OHCI0               (98)
#define AW_IRQ_DMA                     (42)
#define AW_IRQ_TIMER0                  (92)
#define AW_IRQ_TIMER1                  (93)
#define AW_IRQ_NMI                     (136)
#define GIC_IRQ_NUM                    (179)

#elif defined(CONFIG_MACH_SUN8IW11)

#define AW_IRQ_NAND                    69
#define AW_IRQ_USB_OTG                 70
#define AW_IRQ_USB_EHCI0               71
#define AW_IRQ_USB_OHCI0               72
#define AW_IRQ_DMA                     59
#define AW_IRQ_TIMER0                  54
#define AW_IRQ_TIMER1                  55
#define AW_IRQ_NMI                     32
#define AW_IRQ_CIR                     118
#define GIC_IRQ_NUM                   133

#elif defined(CONFIG_MACH_SUN8IW7)

#define AW_IRQ_NAND                    102
#define AW_IRQ_USB_OTG                 103
#define AW_IRQ_USB_EHCI0               104
#define AW_IRQ_USB_OHCI0               105
#define AW_IRQ_DMA                     82
#define AW_IRQ_TIMER0                  50
#define AW_IRQ_TIMER1                  51
#define AW_IRQ_NMI                     64
#define AW_IRQ_CIR                    69
#define GIC_IRQ_NUM                   157

#elif defined(CONFIG_MACH_SUN8IW21)

#define AW_IRQ_USB_OTG                 61
#define AW_IRQ_USB_EHCI0               62
#define AW_IRQ_USB_OHCI0               63
#define AW_IRQ_DMA                     82
#define AW_IRQ_TIMER0                  91
#define AW_IRQ_TIMER1                  92
#define AW_IRQ_NMI                     168
#define GIC_IRQ_NUM                   	223

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




#if defined(CONFIG_MACH_SUN55IW3)

#define GIC_DIST_BASE        (SUNXI_GIC600_BASE)
#define GIC_IROUTR(_n)       (GIC_DIST_BASE + 0x6000 + 8 * (_n))
#define GICR_LPI_BASE(n)     (GIC_DIST_BASE + 0x60000 + n*0x20000)
#define GICR_WAKER(m)        (GICR_LPI_BASE(m) + 0x0014)
#define GICR_PWRR(m)         (GICR_LPI_BASE(m) + 0x0024)
#define put_wvalue(addr, v)  (*((volatile int *)(addr)) = (unsigned int)(v))
#define get_wvalue(addr)     (*((volatile int *)(addr)))
#define LEVEL_TRIGERRED      (0)
#define EDGE_TRIGERRED       (1)
#define GIC_IRQ_TYPE_CFG(_n)	(GIC_DIST_BASE + 0xc00 + 4 * (_n))
#define GIC_IRQ_MOD_CFG(_n)	(GIC_DIST_BASE + 0xd00 + 4 * (_n))
#else
/* GIC registers */
#define GIC_DIST_BASE       (SUNXI_GIC400_BASE+0x1000)
#define GIC_CPUIF_BASE      (SUNXI_GIC400_BASE+0x2000)

#define GIC_CPU_IF_CTRL                 (GIC_CPUIF_BASE + 0x000) // 0x8000
#define GIC_INT_PRIO_MASK               (GIC_CPUIF_BASE + 0x004) // 0x8004
#define GIC_BINARY_POINT                (GIC_CPUIF_BASE + 0x008) // 0x8008
#define GIC_INT_ACK_REG                 (GIC_CPUIF_BASE + 0x00c) // 0x800c
#define GIC_END_INT_REG                 (GIC_CPUIF_BASE + 0x010) // 0x8010
#define GIC_RUNNING_PRIO                (GIC_CPUIF_BASE + 0x014) // 0x8014
#define GIC_HIGHEST_PENDINT             (GIC_CPUIF_BASE + 0x018) // 0x8018
#define GIC_DEACT_INT_REG               (GIC_CPUIF_BASE + 0x1000)// 0x1000
#define GIC_AIAR_REG                    (GIC_CPUIF_BASE + 0x020) // 0x8020
#define GIC_AEOI_REG                    (GIC_CPUIF_BASE + 0x024) // 0x8024
#define GIC_AHIGHEST_PENDINT    (GIC_CPUIF_BASE + 0x028) // 0x8028
#define GIC_IRQ_MOD_CFG(_n)	(GIC_DIST_BASE + 0xc00 + 4 * (_n))
#endif


#define GIC_DIST_CON		(GIC_DIST_BASE + 0x0000)
#define GIC_CON_TYPE		(GIC_DIST_BASE + 0x0004)
#define GIC_CON_IIDR		(GIC_DIST_BASE + 0x0008)

#define GIC_CON_IGRP(n)		(GIC_DIST_BASE + 0x0080 + (n)*4)
#define GIC_SET_EN(_n)		(GIC_DIST_BASE + 0x100 + 4 * (_n))
#define GIC_CLR_EN(_n)		(GIC_DIST_BASE + 0x180 + 4 * (_n))
#define GIC_PEND_SET(_n)	(GIC_DIST_BASE + 0x200 + 4 * (_n))
#define GIC_PEND_CLR(_n)	(GIC_DIST_BASE + 0x280 + 4 * (_n))
#define GIC_ACT_SET(_n)		(GIC_DIST_BASE + 0x300 + 4 * (_n))
#define GIC_ACT_CLR(_n)		(GIC_DIST_BASE + 0x380 + 4 * (_n))
#define GIC_SGI_PRIO(_n)	(GIC_DIST_BASE + 0x400 + 4 * (_n))
#define GIC_PPI_PRIO(_n)	(GIC_DIST_BASE + 0x410 + 4 * (_n))
#define GIC_SPI_PRIO(_n)	(GIC_DIST_BASE + 0x420 + 4 * (_n))
#define GIC_SPI_PROC_TARG(_n)(GIC_DIST_BASE + 0x820 + 4 * (_n))

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
