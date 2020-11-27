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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __SUNXI_INTC_H__
#define __SUNXI_INTC_H__
#include "platform.h"

#ifndef FPGA_PLATFORM    //chip irq mapping

#define  AW_IRQ_NMI         (0)

#define  AW_IRQ_UART0       (1)
#define  AW_IRQ_UART1       (2)
#define  AW_IRQ_UART2       (3)
//#define  AW_IRQ_UART3     (4)
#define  AW_IRQ_SPDIF       (5)
#define  AW_IRQ_IR0         (6)

#define  AW_IRQ_TWI0        (7)
#define  AW_IRQ_TWI1        (8)
#define  AW_IRQ_TWI2        (9)

#define  AW_IRQ_SPI0        (10)
#define  AW_IRQ_SPI1        (11)

#define  AW_IRQ_TIMER0      (13)
#define  AW_IRQ_TIMER1      (14)
#define  AW_IRQ_TIMER2      (15)
#define  AW_IRQ_TIMER3      (16) //watchdog
#define  AW_IRQ_RSB         (17)

#define  AW_IRQ_DMA         (18)
#define  AW_IRQ_TP          (20)

#define  AW_IRQ_AC97        (21)

#define  AW_IRQ_LRADC       (22)
#define  AW_IRQ_MMC0        (23)
#define  AW_IRQ_MMC1        (24)


#define  AW_IRQ_USB_OTG     (26)

#define  AW_IRQ_TVD         (27)
#define  AW_IRQ_TVE01       (28)

#define  AW_IRQ_LCD0        (29)
//#define  AW_IRQ_LCD1      (45)

#define  AW_IRQ_DE_FE0      (30)
#define  AW_IRQ_DE_FE1      (31)

#define  AW_IRQ_CSI         (32)
#define  AW_IRQ_DEINTERLACE (33)
#define  AW_IRQ_VE          (34)

#define  AW_IRQ_DAUDIO      (35)
#define  AW_IRQ_PIOD        (38)
#define  AW_IRQ_PIOE        (39)
#define  AW_IRQ_PIOF        (40)

#define  GIC_IRQ_NUM        (AW_IRQ_PIOF+1)

#else
#define  AW_IRQ_NMI         (0)

#define  AW_IRQ_UART0       (1)
#define  AW_IRQ_UART1       (1)
#define  AW_IRQ_UART2       (3)
//#define  AW_IRQ_UART3     (4)
#define  AW_IRQ_SPDIF       (5)
#define  AW_IRQ_IR0         (6)

#define  AW_IRQ_TWI0        (7)
#define  AW_IRQ_TWI1        (8)
#define  AW_IRQ_TWI2        (9)

#define  AW_IRQ_SPI0        (10)
#define  AW_IRQ_SPI1        (11)

#define  AW_IRQ_TIMER0      (13)
#define  AW_IRQ_TIMER1      (14)
#define  AW_IRQ_TIMER2      (15)
#define  AW_IRQ_TIMER3      (16) //watchdog
#define  AW_IRQ_RSB         (17)

#define  AW_IRQ_DMA         (18)
#define  AW_IRQ_TP          (20)

#define  AW_IRQ_AC97        (21)

#define  AW_IRQ_LRADC       (22)
#define  AW_IRQ_MMC0        (23)
#define  AW_IRQ_MMC1        (24)


#define  AW_IRQ_USB_OTG     (26)

#define  AW_IRQ_TVD         (27)
#define  AW_IRQ_TVE01       (28)

#define  AW_IRQ_LCD0        (29)
//#define  AW_IRQ_LCD1      (45)

#define  AW_IRQ_DE_FE0      (30)
#define  AW_IRQ_DE_FE1      (31)

#define  AW_IRQ_CSI         (32)
#define  AW_IRQ_DEINTERLACE (33)
#define  AW_IRQ_VE          (34)

#define  AW_IRQ_DAUDIO      (35)
#define  AW_IRQ_PIOD        (38)
#define  AW_IRQ_PIOE        (39)
#define  AW_IRQ_PIOF        (40)

#define  GIC_IRQ_NUM        (AW_IRQ_PIOF+1)


#endif    //fpga irq mapping


#define  INTC_REG_VCTR             (SUNXI_INTC_BASE + 0x00)
#define  INTC_REG_VTBLBADDR        (SUNXI_INTC_BASE + 0x04)

#define  INTC_REG_NMI_CTRL         (SUNXI_INTC_BASE + 0x0C)

#define  INTC_REG_IRQ_PENDCLR0     (SUNXI_INTC_BASE + 0x10)
#define  INTC_REG_IRQ_PENDCLR1     (SUNXI_INTC_BASE + 0x14)
//#define  INTC_REG_IRQ_PENDCLR2   (SUNXI_INTC_BASE + 0x18)

#define  INTC_REG_ENABLE0          (SUNXI_INTC_BASE + 0x20)
#define  INTC_REG_ENABLE1          (SUNXI_INTC_BASE + 0x24)
//#define  INTC_REG_ENABLE2        (SUNXI_INTC_BASE + 0x48)

#define  INTC_REG_MASK0            (SUNXI_INTC_BASE + 0x30)
#define  INTC_REG_MASK1            (SUNXI_INTC_BASE + 0x34)
//#define  INTC_REG_MASK2          (SUNXI_INTC_BASE + 0x58)


#define  INTC_REG_RESP0            (SUNXI_INTC_BASE + 0x40)
#define  INTC_REG_RSEP1            (SUNXI_INTC_BASE + 0x44)
//#define  INTC_REG_RESP2          (SUNXI_INTC_BASE + 0x68)

#define  INTC_REG_FF0              (SUNXI_INTC_BASE + 0x50)
#define  INTC_REG_FF1              (SUNXI_INTC_BASE + 0x54)
//#define  INTC_REG_FF2            (SUNXI_INTC_BASE + 0x78)


#define  INTC_REG_PRIO0            (SUNXI_INTC_BASE + 0x60)
#define  INTC_REG_PRIO1            (SUNXI_INTC_BASE + 0x64)
#define  INTC_REG_PRIO2            (SUNXI_INTC_BASE + 0x68)
#define  INTC_REG_PRIO3            (SUNXI_INTC_BASE + 0x6C)

extern int arch_interrupt_init (void);
extern int arch_interrupt_exit (void);
extern int irq_enable(int irq_no);
extern int irq_disable(int irq_no);
extern void irq_install_handler (int irq, interrupt_handler_t handle_irq, void *data);
extern void irq_free_handler(int irq);

#endif
