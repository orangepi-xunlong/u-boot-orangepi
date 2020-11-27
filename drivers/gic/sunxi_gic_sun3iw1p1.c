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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/gic.h>
#include <sunxi_board.h>

DECLARE_GLOBAL_DATA_PTR;

struct _irq_handler
{
    void                *m_data;
    void (*m_func)( void * data);
};

struct _irq_handler sunxi_int_handlers[GIC_IRQ_NUM];

extern int  interrupts_is_open(void);

static void default_isr(void *data)
{
    printf("default_isr():  called from IRQ %d\n", (uint)data);
    while(1);
}

int irq_enable(int irq_no)
{
    uint reg_val;
    if(irq_no >= GIC_IRQ_NUM)
    {
        return -1;
    }
    if(irq_no < 32)
    {
        writel((readl(INTC_REG_ENABLE0) | (  1<<irq_no)), INTC_REG_ENABLE0);
        reg_val = readl(INTC_REG_MASK0);
        reg_val &= ~(1<<irq_no);
        writel(reg_val, INTC_REG_MASK0);
        reg_val = readl(INTC_REG_IRQ_PENDCLR0);
        if(reg_val & (1<<irq_no)) /* must clear pending bit when enabled */
        {
            reg_val = (1<<irq_no);
            writel(reg_val, INTC_REG_IRQ_PENDCLR0);
        }
    }
    else if(irq_no < 64)
    {
        irq_no -= 32;
        writel((readl(INTC_REG_ENABLE1) | (  1<<irq_no)), INTC_REG_ENABLE1);
        reg_val = readl(INTC_REG_MASK1);
        reg_val &= ~(1<<irq_no);
        writel(reg_val, INTC_REG_MASK1);
    }

    return 0;
}


int irq_disable(int irq_no)
{
    uint reg_val;
    if(irq_no >= GIC_IRQ_NUM)
    {
        return -1;
    }

    if(irq_no < 32)
    {
        reg_val = readl(INTC_REG_ENABLE0);
        reg_val &= ~(1<<irq_no);
        writel(reg_val, INTC_REG_ENABLE0);
        writel((readl(INTC_REG_MASK0)   | (  1<<irq_no)), INTC_REG_MASK0);
    }
    else if(irq_no < 64)
    {
        irq_no -= 32;
        reg_val = readl(INTC_REG_ENABLE1);
        reg_val &= ~(1<<irq_no);
        writel(reg_val, INTC_REG_ENABLE1);
        writel((readl(INTC_REG_MASK1)   | (  1<<irq_no)), INTC_REG_MASK1);
    }

    return 0;
}


void irq_install_handler (int irq, interrupt_handler_t handle_irq, void *data)
{
    int flag = interrupts_is_open();
    //when irq_handler call this function , irq enable bit has already disabled in irq_mode,so don't need to enable I bit
    if(flag)
    {
        disable_interrupts();
    }

    if (irq >= GIC_IRQ_NUM || !handle_irq)
    {
        goto __END;
    }

    sunxi_int_handlers[irq].m_data = data;
    sunxi_int_handlers[irq].m_func = handle_irq;
__END:
    if(flag)
    {
        enable_interrupts();
    }
}

void irq_free_handler(int irq)
{
    disable_interrupts();
    if (irq >= GIC_IRQ_NUM)
    {
        enable_interrupts();
        return;
    }

    sunxi_int_handlers[irq].m_data = NULL;
    sunxi_int_handlers[irq].m_func = default_isr;

    enable_interrupts();
}

#ifdef CONFIG_USE_IRQ

void do_irq (struct pt_regs *pt_regs)
{
    u32 base;

    base = readl(INTC_REG_VCTR)>>2;

    if(sunxi_int_handlers[base].m_func)
    {
        sunxi_int_handlers[base].m_func(sunxi_int_handlers[base].m_data);
    }

    return;

}


int do_irqinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int irq;

    printf("Interrupt-Information:\n");
    printf("Nr(Max)  Routine   Arg\n");

    for (irq = 0; irq < GIC_IRQ_NUM; irq ++)
    {
        if (sunxi_int_handlers[irq].m_func != NULL)
        {
            printf("%d(%d)  0x%08lx  0x%08lx\n",
                    irq, GIC_IRQ_NUM,
                    (ulong)sunxi_int_handlers[irq].m_func,
                    (ulong)sunxi_int_handlers[irq].m_data);
        }
    }

    return 0;
}

#endif


int arch_interrupt_init (void)
{
    int i;
    for (i=0; i<GIC_IRQ_NUM; i++)
    {
        sunxi_int_handlers[i].m_data = default_isr;
        sunxi_int_handlers[i].m_func = 0;
    }
    writel(0, INTC_REG_ENABLE0);
    writel(0, INTC_REG_ENABLE1);

    writel(0, INTC_REG_MASK0);
    writel(0, INTC_REG_MASK1);

    writel(0xffffffff, INTC_REG_IRQ_PENDCLR0);
    writel(0xffffffff, INTC_REG_IRQ_PENDCLR1);

    return 0;
}


int arch_interrupt_exit(void)
{
    *(volatile unsigned int *)(0x01c20c00 + 0x00) = 0;
    *(volatile unsigned int *)(0x01c20c00 + 0x04) |= 0x043f;
    *(volatile unsigned int *)(0x01c20c00 + 0x10) = 0;
    *(volatile unsigned int *)(0x01c20c00 + 0x20) = 0;
    *(volatile unsigned int *)(0x01c02000 + 0x00) = 0;
    *(volatile unsigned int *)(0x01c02000 + 0x04) = 0xffffffff;

    writel(0, INTC_REG_ENABLE0);
    writel(0, INTC_REG_ENABLE1);

    writel(0, INTC_REG_MASK0);
    writel(0, INTC_REG_MASK1);

    writel(0xffffffff, INTC_REG_IRQ_PENDCLR0);
    writel(0xffffffff, INTC_REG_IRQ_PENDCLR1);

    return 0;
}


