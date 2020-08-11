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

static void gic_spi_set_target(int irq_no, int cpu_id);
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int irq_enable(int irq_no)
{
	uint reg_val;
	uint offset;

	if (irq_no >= GIC_IRQ_NUM)
	{
		printf("irq NO.(%d) > GIC_IRQ_NUM(%d) !!\n", irq_no, GIC_IRQ_NUM);
		return -1;
	}
	if(irq_no == AW_IRQ_NMI)
	{
		*(volatile unsigned int *)(0x01f00c00 + 0x10) |= 1;
		*(volatile unsigned int *)(0x01f00c00 + 0x40) |= 1;
	}
	gic_spi_set_target(irq_no, 0);

	offset   = irq_no >> 5; // é™?2
	reg_val  = readl(GIC_SET_EN(offset));
	reg_val |= 1 << (irq_no & 0x1f);
	writel(reg_val, GIC_SET_EN(offset));

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int irq_disable(int irq_no)
{
	uint reg_val;
	uint offset;

	if (irq_no >= GIC_IRQ_NUM)
	{
		printf("irq NO.(%d) > GIC_IRQ_NUM(%d) !!\n", irq_no, GIC_IRQ_NUM);
		return -1;
	}
	if(irq_no == AW_IRQ_NMI)
	{
		*(volatile unsigned int *)(0x01f00c00 + 0x10) |= 1;
		*(volatile unsigned int *)(0x01f00c00 + 0x40) &= ~1;
	}
	gic_spi_set_target(irq_no, 0);

	offset   = irq_no >> 5; // é™¤32
	reg_val  = (1 << (irq_no & 0x1f));
	writel(reg_val, GIC_CLR_EN(offset));

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
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

/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static void gic_spi_set_target(int irq_no, int cpu_id)
{
	uint reg_val, addr, offset;

	irq_no -= 32;
	/* dispatch the usb interrupt to CPU1 */
	addr = GIC_SPI_PROC_TARG(irq_no>>2);
	reg_val = readl(addr);
	offset  = 8 * (irq_no & 3);
	reg_val &= ~(0xff<<offset);
	reg_val |=  (((1<<cpu_id) & 0xf) <<offset);
	writel(reg_val, addr);

	return ;
}

