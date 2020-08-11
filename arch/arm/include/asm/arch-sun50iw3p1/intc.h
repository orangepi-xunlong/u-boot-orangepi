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

#define AW_IRQ_GIC_START    (32)

#ifndef CONFIG_FPGA	//chip irq mapping

#define AW_IRQ_NAND                    64
#define AW_IRQ_USB_OTG                 54
#define AW_IRQ_USB_EHCI0               55
#define AW_IRQ_USB_OHCI0               56
#define AW_IRQ_DMA                     70
#define AW_IRQ_TIMER0                  75
#define AW_IRQ_TIMER1                  76
#define AW_IRQ_NMI                     112
#define GIC_IRQ_NUM                    (147)

#else

#define AW_IRQ_NAND                    64
#define AW_IRQ_USB_OTG                 54
#define AW_IRQ_USB_EHCI0               55
#define AW_IRQ_USB_OHCI0               56
#define AW_IRQ_DMA                     70
#define AW_IRQ_TIMER0                  75
#define AW_IRQ_TIMER1                  76
#define AW_IRQ_NMI                     112
#define GIC_IRQ_NUM                    (147)

#endif	//fpga irq mapping

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

extern int arch_interrupt_init (void);
extern int arch_interrupt_exit (void);
extern int irq_enable(int irq_no);
extern int irq_disable(int irq_no);
extern void irq_install_handler (int irq, interrupt_handler_t handle_irq, void *data);
extern void irq_free_handler(int irq);

extern int sunxi_gic_cpu_interface_init(int cpu);
extern int sunxi_gic_cpu_interface_exit(void);

#endif
