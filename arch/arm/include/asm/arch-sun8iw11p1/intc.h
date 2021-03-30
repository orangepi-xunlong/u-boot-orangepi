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

#ifndef FPGA_PLATFORM	//chip irq mapping

#define AW_IRQ_NMI                     32
#define AW_IRQ_TIMER0                  54 
#define AW_IRQ_TIMER1                  55

#define AW_IRQ_DMA                     59

#define AW_IRQ_MMC0                    64
#define AW_IRQ_MMC1                    65
#define AW_IRQ_MMC2                    66
#define AW_IRQ_MMC3                    67


#define AW_IRQ_NAND                    69
#define AW_IRQ_USB_OTG                 70
#define AW_IRQ_USB_EHCI0               71
#define AW_IRQ_USB_OHCI0               72
#define AW_IRQ_SCR                     73
#define AW_IRQ_CSI0                    74
#define AW_IRQ_CSI1                    75

#define AW_IRQ_TCON_LCD0               76
#define AW_IRQ_TCON_LCD1               77

#define AW_IRQ_TCON_TV0                83
#define AW_IRQ_TCON_TV1                84
#define AW_IRQ_MIPI_DSI                89
#define AW_IRQ_HDMI                    90

#define AW_IRQ_DIT                     125
#define AW_IRQ_CENS                    126
#define AW_IRQ_DE                      127
#define AW_IRQ_ROT                     128

#define GIC_IRQ_NUM                    (132+1)


#else
#define GIC_SRC_SPI(_n)                (32 + (_n))

#define AW_IRQ_NMI                     32
#define AW_IRQ_TIMER0                  36 
#define AW_IRQ_TIMER1                  37

#define AW_IRQ_DMA                     39

#define AW_IRQ_MMC0                    41
#define AW_IRQ_MMC1                    0
#define AW_IRQ_MMC2                    42
#define AW_IRQ_MMC3                    0


#define AW_IRQ_NAND                    43
#define AW_IRQ_USB_OTG                 44
#define AW_IRQ_USB_EHCI0               45
#define AW_IRQ_USB_OHCI0               46
#define AW_IRQ_SCR                     47
#define AW_IRQ_CSI0                    48
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

#define GIC_IRQ_NUM                    (63+1)


#endif	//fpga irq mapping

extern int arch_interrupt_init (void);
extern int arch_interrupt_exit (void);
extern int irq_enable(int irq_no);
extern int irq_disable(int irq_no);
extern void irq_install_handler (int irq, interrupt_handler_t handle_irq, void *data);
extern void irq_free_handler(int irq);

#endif
