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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
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
#define AW_IRQ_NMI                     64
#define AW_IRQ_TIMER0                  50
#define AW_IRQ_TIMER1                  51

#define AW_IRQ_DMA                     82

#define AW_IRQ_MMC0                    92
#define AW_IRQ_MMC1                    93
#define AW_IRQ_MMC2                    94

#define AW_IRQ_NAND                    102
#define AW_IRQ_USB_OTG                 103
#define AW_IRQ_USB_EHCI0               104
#define AW_IRQ_USB_OHCI0               105
#define AW_IRQ_SCR0                    115
#define AW_IRQ_CSI0                    116
#define AW_IRQ_CSI1                    0

#define AW_IRQ_TCON_LCD0               118
#define AW_IRQ_TCON_LCD1               119

//#define AW_IRQ_TCON_TV0                83
//#define AW_IRQ_TCON_TV1                84
//#define AW_IRQ_MIPI_DSI                89

#define AW_IRQ_HDMI                    120
#define AW_IRQ_SCR1                    121
#define AW_IRQ_DIT                     125
#define AW_IRQ_CENS                    126
#define AW_IRQ_DE                      127
#define AW_IRQ_ROT                     128
#define AW_IRQ_CIR					   69

#define GIC_IRQ_NUM                    (156+1)

#else
#define GIC_SRC_SPI(_n)                (32 + (_n))

#define AW_IRQ_NMI                     0
#define AW_IRQ_TIMER0                  50
#define AW_IRQ_TIMER1                  51

#define AW_IRQ_DMA                     82

#define AW_IRQ_MMC0                    92
#define AW_IRQ_MMC1                    93
#define AW_IRQ_MMC2                    94
#define AW_IRQ_MMC3                    0


#define AW_IRQ_NAND                    102
#define AW_IRQ_USB_OTG                 103
#define AW_IRQ_USB_EHCI0               104
#define AW_IRQ_USB_OHCI0               105
#define AW_IRQ_SCR                     115
#define AW_IRQ_CSI0                    0
#define AW_IRQ_CSI1                    0

#define AW_IRQ_TCON_LCD0               0
#define AW_IRQ_TCON_LCD1               0

#define AW_IRQ_TCON_TV0                0
#define AW_IRQ_TCON_TV1                0
#define AW_IRQ_MIPI_DSI                0
#define AW_IRQ_HDMI                    0

#define AW_IRQ_DIT                     0
#define AW_IRQ_CENS                    0
#define AW_IRQ_DE                      0
#define AW_IRQ_ROT                     0

#define GIC_IRQ_NUM                    (156+1)		*/

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

extern void irq_install_handler (int irq, interrupt_handler_t handle_irq, void *data);
extern void irq_free_handler(int irq);
extern int irq_enable(int irq_no);
extern int irq_disable(int irq_no);

int arch_interrupt_init (void);

int arch_interrupt_exit (void);
extern int sunxi_gic_cpu_interface_init(int cpu);
extern int sunxi_gic_cpu_interface_exit(void);


#endif
