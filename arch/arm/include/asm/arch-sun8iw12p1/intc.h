/*
 * (C) Copyright 2016-2020
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *zhouhuacai <zhouhuacai@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */


#ifndef __SUNXI_INTC_H__
#define __SUNXI_INTC_H__

#define AW_IRQ_NAND                    (61)
#define AW_IRQ_USB_OTG                 (96)
#define AW_IRQ_USB_EHCI0               (97)
#define AW_IRQ_USB_OHCI0               (98)
#define AW_IRQ_DMA                     (42)
#define AW_IRQ_TIMER0                  (92)
#define AW_IRQ_TIMER1                  (93)
#define AW_IRQ_NMI                     (136)
#define GIC_IRQ_NUM                    (179)

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
