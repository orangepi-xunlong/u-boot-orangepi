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
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/plic.h>
#include <sunxi_board.h>

DECLARE_GLOBAL_DATA_PTR;


struct _irq_handler {
	void                *m_data;
	void (*m_func)(void *data);
};

struct _irq_handler sunxi_int_handlers[PLIC_IRQ_NUM];

extern int  interrupts_is_open(void);

static void default_isr(void *data)
{
	printf("default_isr():  called from IRQ 0x%lx\n", (phys_addr_t)data);
	while
		(1);
}

int irq_enable(int irq_no)
{
	uint reg_val;
	uint offset;
	/* static int old_irq_no; */
	if (irq_no >= PLIC_IRQ_NUM) {
		printf("irq NO.(%d) > PLIC_IRQ_NUM(%d) !!\n", irq_no, PLIC_IRQ_NUM);
		return -1;
	}
	offset   = irq_no >> 5;
	reg_val = readl(PLIC_M_S_IE_REG(offset));
	reg_val |= (1 << (irq_no & 0x1f));
	writel(reg_val, PLIC_M_S_IE_REG(offset));
	writel(1, PLIC_PRIO_REG(irq_no));
	/* if (old_irq_no != 0) {
	 *         writel(old_irq_no, PLIC_SCLAIM_REG);
	 * }
	 * old_irq_no = irq_no; */
	return 0;
}

int irq_disable(int irq_no)
{
	uint reg_val;
	uint offset;

	if (irq_no >= PLIC_IRQ_NUM) {
		printf("irq NO.(%d) > PLIC_IRQ_NUM(%d) !!\n", irq_no, PLIC_IRQ_NUM);
		return -1;
	}

	offset   = irq_no >> 5;
	reg_val = readl(PLIC_M_S_IE_REG(offset));
	reg_val &= ~(1 << (irq_no & 0x1f));
	writel(reg_val, PLIC_M_S_IE_REG(offset));

	return 0;
}

static void plic_spi_handler(uint irq_no)
{
	if (sunxi_int_handlers[irq_no].m_func != default_isr) {
		sunxi_int_handlers[irq_no].m_func(sunxi_int_handlers[irq_no].m_data);
	}
}


void irq_install_handler (int irq, interrupt_handler_t handle_irq, void *data)
{
	/* int flag = interrupts_is_open();
	 * //when irq_handler call this function , irq enable bit has already disabled in irq_mode,so don't need to enable I bit
	 * if (flag) {
	 *         disable_interrupts();
	 * } */

	if (irq >= PLIC_IRQ_NUM || !handle_irq) {
		return;
	}

	sunxi_int_handlers[irq].m_data = data;
	sunxi_int_handlers[irq].m_func = handle_irq;
	/* if (flag) {
	 *         enable_interrupts();
	 * } */
}

void irq_free_handler(int irq)
{
	disable_interrupts();
	if (irq >= PLIC_IRQ_NUM) {
		enable_interrupts();
		return;
	}

	sunxi_int_handlers[irq].m_data = NULL;
	sunxi_int_handlers[irq].m_func = default_isr;

	enable_interrupts();
}


void external_interrupt(struct pt_regs *regs)
{
	u32 idnum = 0;
	unsigned long riscv_mode = get_cur_riscv_mode();

	riscv_mode == PRV_M ? csr_clear(mie, SR_MIE) : csr_clear(sie, SR_SIE);
	do {
		idnum = readl(PLIC_M_S_CLAIM_REG);
		if (idnum != 0) {
			plic_spi_handler(idnum);
			irq_enable(idnum);
			writel(idnum, PLIC_M_S_CLAIM_REG);
		}
	} while (idnum != 0);
	riscv_mode == PRV_M ? csr_set(mie, SR_MIE) : csr_set(sie, SR_SIE);
	return;
}



