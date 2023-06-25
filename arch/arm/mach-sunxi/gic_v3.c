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
#include <asm/arch/gic.h>
#include <sunxi_board.h>

DECLARE_GLOBAL_DATA_PTR;

struct _irq_handler {
	void *m_data;
	void (*m_func)(void *data);
};

struct _irq_handler sunxi_int_handlers[GIC_IRQ_NUM];

extern int interrupts_is_open(void);

static void gic_spi_set_target(int irq_no, int cpu_id);

static void gic_group_enable(void);
static void gic_sys_ctrl_reg_enable(void);
static void gic_redistributor_init(void);
static void gic_deact_int(u32 irq_num);
static void gic_end_of_int(u32 irq_num);
static u32 get_int_ack_num(void);
static void set_irq_prio_mask(u32 prio_mask);
void gic_type(u32 irq_no, u32 type);
u32 get_cpu_id(void);
u32 get_cluster_id(void);

u32 get_cpu_id(void)
{
	u32 irq_num = 0;
	return irq_num;
}

u32 get_cluster_id(void)
{
	u32 irq_num = 0;
	return irq_num;
}

static void default_isr(void *data)
{
	printf("default_isr():  called from IRQ %d\n", (uint)data);
	while (1)
		;
}

static __inline void sr32(u32 addr, u32 start_bit, u32 num_bits, u32 value)
{
	u32 tmp, msk = (1 << num_bits) - 1;
	tmp = readl(addr) & ~(msk << start_bit);
	tmp |= value << start_bit;
	writel(tmp, addr);
}

int irq_enable(int irq_no)
{
	u32 base;
	u32 base_os;
	u32 bit_os;

	if (irq_no >= GIC_IRQ_NUM) {
		printk("irq NO.(%d) > GIC_IRQ_NUM(%d) !!\n", irq_no, GIC_IRQ_NUM);
		return -1;
	}

	base_os = irq_no >> 5;
	base = GIC_SET_EN(base_os);
	bit_os = irq_no & 0x1f;
	sr32(base, bit_os, 1, 1);

	if((irq_no >= 32) && (irq_no < GIC_IRQ_NUM)){
		//config group
		base_os = irq_no >> 5;
		base = GIC_IRQ_MOD_CFG(base_os);
		bit_os = irq_no & 0x1f;
		sr32(base, bit_os, 1, 1);
	}

	return 0;
}


int irq_disable(int irq_no)
{
	uint reg_val;
	uint offset;

	if (irq_no >= GIC_IRQ_NUM) {
		printf("irq NO.(%d) > GIC_IRQ_NUM(%d) !!\n", irq_no,
		       GIC_IRQ_NUM);
		return -1;
	}
	if (irq_no == AW_IRQ_NMI) {
		*(volatile unsigned int *)(0x01f00c00 + 0x10) |= 1;
		*(volatile unsigned int *)(0x01f00c00 + 0x40) &= ~1;
	}
	gic_spi_set_target(irq_no, 0);

	offset  = irq_no >> 5;
	reg_val = (1 << (irq_no & 0x1f));
	writel(reg_val, GIC_CLR_EN(offset));

	return 0;
}

static void gic_sgi_handler(uint irq_no)
{
	printf("SGI irq %d coming... \n", irq_no);
}

static void gic_ppi_handler(uint irq_no)
{
	printf("PPI irq %d coming... \n", irq_no);
}

static void gic_spi_handler(uint irq_no)
{
	if (sunxi_int_handlers[irq_no].m_func != default_isr) {
		sunxi_int_handlers[irq_no].m_func(
			sunxi_int_handlers[irq_no].m_data);
	}
}

static void gic_clear_pending(uint irq_no)
{
	uint reg_val;
	uint offset;

	offset = irq_no >> 5;
	//reg_val = readl(GIC_PEND_CLR(offset));
	reg_val = (1 << (irq_no & 0x1f));
	writel(reg_val, GIC_PEND_CLR(offset));

	return;
}

void irq_install_handler(int irq, interrupt_handler_t handle_irq, void *data)
{
	int flag = interrupts_is_open();
	//when irq_handler call this function , irq enable bit has already disabled in irq_mode,so don't need to enable I bit
	if (flag) {
		disable_interrupts();
	}

	if (irq >= GIC_IRQ_NUM || !handle_irq) {
		goto __END;
	}

	sunxi_int_handlers[irq].m_data = data;
	sunxi_int_handlers[irq].m_func = handle_irq;
__END:
	if (flag) {
		enable_interrupts();
	}
}

void irq_free_handler(int irq)
{
	disable_interrupts();
	if (irq >= GIC_IRQ_NUM) {
		enable_interrupts();
		return;
	}

	sunxi_int_handlers[irq].m_data = NULL;
	sunxi_int_handlers[irq].m_func = default_isr;

	enable_interrupts();
}

void do_irq(struct pt_regs *pt_regs)
{
	u32 idnum;

#ifdef CONFIG_ARCH_SUN8IW6P1
	/* fix gic bug when enable secure*/
	if (sunxi_probe_secure_os())
		idnum = get_int_ack_num();
	else
		idnum = get_int_ack_num();
#else
	idnum = get_int_ack_num();
#endif

	if ((idnum == 1022) || (idnum == 1023)) {
		printf("spurious irq !!\n");
		return;
	}
	if (idnum >= GIC_IRQ_NUM) {
		debug("irq NO.(%d) > GIC_IRQ_NUM(%d) !!\n", idnum,
		      GIC_IRQ_NUM - 32);
		return;
	}
	if (idnum < 16)
		gic_sgi_handler(idnum);
	else if (idnum < 32)
		gic_ppi_handler(idnum);
	else
		gic_spi_handler(idnum);
#ifdef CONFIG_ARCH_SUN8IW6P1
	/* fix gic bug when enable secure*/
	if (sunxi_probe_secure_os())
		writel(idnum, GIC_AEOI_REG);
	else
		writel(idnum, GIC_END_INT_REG);
#else
	gic_end_of_int(idnum);
#endif

	gic_deact_int(idnum);
	gic_clear_pending(idnum);

	return;
}

int do_irqinfo(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int irq;

	printf("Interrupt-Information:\n");
	printf("Nr(Max)  Routine   Arg\n");

	for (irq = 0; irq < GIC_IRQ_NUM; irq++) {
		if (sunxi_int_handlers[irq].m_func != NULL) {
			printf("%d(%d)  0x%08lx  0x%08lx\n", irq, GIC_IRQ_NUM,
			       (ulong)sunxi_int_handlers[irq].m_func,
			       (ulong)sunxi_int_handlers[irq].m_data);
		}
	}

	return 0;
}

static void gic_distributor_init(void)
{
	__u32 gic_irqs;
	__u32 i, rdata;

	writel(0, GIC_DIST_CON);

	/* check GIC hardware configutation */
	gic_irqs = ((readl(GIC_CON_TYPE) & 0x1f) + 1) * 32;
	if (gic_irqs > 1020) {
		gic_irqs = 1020;
	}
	if (gic_irqs < GIC_IRQ_NUM) {
		debug("GIC parameter config error, only support %d"
		      " irqs < %d(spec define)!!\n",
		      gic_irqs, GIC_IRQ_NUM);
		return;
	}

	/* set trigger type to be level-triggered, active low */
	for (i = 0; i < GIC_IRQ_NUM; i += 16) {
		writel(0, GIC_IRQ_TYPE_CFG(i>>4));
	}
	/* set priority */
	for (i = GIC_SRC_SPI(0); i < GIC_IRQ_NUM; i += 4) {
		writel(0xa0a0a0a0, GIC_SPI_PRIO((i - 32) >> 2));
	}
	/* disable all interrupts */
	for (i = 32; i < GIC_IRQ_NUM; i += 32) {
		writel(0xffffffff, GIC_CLR_EN(i >> 5));
	}
	/* clear all interrupt active state */
	for (i = 32; i < GIC_IRQ_NUM; i += 32) {
		writel(0xffffffff, GIC_ACT_CLR(i >> 5));
	}
	//Global setting
	rdata = readl(GIC_DIST_CON);
	rdata |= (0x3 << 4) | (0x7 << 0);
	writel(rdata, GIC_DIST_CON);

	return;
}

static void gic_cpuif_init(void)
{
	__u32 rdata;
	rdata = readl(GIC_DIST_CON);
	rdata |= (0x1 << 4) | (0x3 << 0);
	writel(rdata, GIC_DIST_CON);

	gic_sys_ctrl_reg_enable();

	set_irq_prio_mask(0xF0);

	gic_group_enable();
}

static void gic_spi_set_target(int irq_no, int cpu_id)
{
	put_wvalue(GIC_IROUTR(irq_no), (cpu_id << 8));
	return;
}

int arch_interrupt_init(void)
{
	int i;
	if ((get_cpu_id() == 0) && (get_cluster_id() == 0)) {
		for (i				     = 0; i < GIC_IRQ_NUM; i++)
			sunxi_int_handlers[i].m_data = default_isr;
	}
	if (sunxi_probe_secure_monitor() || sunxi_probe_secure_os())
		tick_printf("gic: sec monitor mode\n");
	else {
		tick_printf("gic: normal mode\n");
		gic_distributor_init();
		gic_redistributor_init();
	}
		gic_cpuif_init();
	return 0;
}

int arch_interrupt_exit(void)
{
	if (!(sunxi_probe_secure_monitor() || sunxi_probe_secure_os())) {
		gic_distributor_init();
		gic_cpuif_init();
	}

	return 0;
}

int sunxi_gic_cpu_interface_init(int cpu)
{
	gic_cpuif_init();

	return 0;
}

/*
 * set_irq_prio_mask
 * */
static void set_irq_prio_mask(u32 prio_mask)
{
	u32 temp = 0;
        asm volatile("MRC p15, 0, %0, c4, c6, 0" : "=r"(temp));
	temp &= ~(0xFF << 0);
	temp |= prio_mask;
	asm volatile("MCR p15, 0, %0, c4, c6, 0" :: "r"(temp));
}

static u32 get_int_ack_num(void)
{
	u32 irq_num = 0;

	asm volatile("MRC p15, 0, %0, c12, c12, 0" : "=r"(irq_num));
	return irq_num;
}

static void gic_end_of_int(u32 irq_num)
{
	asm volatile("MCR p15, 0, %0, c12, c12, 1" :: "r"(irq_num));
}

static void gic_deact_int(u32 irq_num)
{
	asm volatile("MCR     p15, 0, %0, c12, c11, 1" :: "r"(irq_num));
}

static void gic_redistributor_init(void)
{
	u32 rdata;
	u32 cpuid = get_cpu_id();

	put_wvalue(GICR_PWRR(cpuid), 0x2);
	put_wvalue(GICR_WAKER(cpuid), 0x0);
	do {
		rdata = get_wvalue(GICR_WAKER(cpuid)) & (0x1 << 2);
	} while (rdata);
}

static void gic_sys_ctrl_reg_enable(void)
{
	u32 value = 1;
	asm volatile ("MCR     p15, 0, %0, c12, c12, 5" :: "r"(value));
}

static void gic_group_enable(void)
{
	u32 value = 1;
	asm volatile("MCR     p15, 0, %0, c12, c12, 7" :: "r"(value));
}

void gic_type(u32 irq_no, u32 type)
{
	u32 rdata;
	u32 n, mod;
	n   = irq_no / 16;
	mod = irq_no % 16;

	if (type == EDGE_TRIGERRED) {
		rdata = get_wvalue(GIC_IRQ_MOD_CFG(n)) | (1 << (mod + 1));
		put_wvalue(GIC_IRQ_MOD_CFG(n), rdata);
	} else if (type == LEVEL_TRIGERRED) {
		rdata = get_wvalue(GIC_IRQ_MOD_CFG(n)) & (~(1 << (mod + 1)));
		put_wvalue(GIC_IRQ_MOD_CFG(n), rdata);
	}
}

s32 set_irq_prio(u32 irq_no, u32 param)
{
	s32 ret = 0;
	u32 div, mod;
	u32 rdata;

	irq_no -= 32;
	/* set priority */

	if (irq_no > GIC_IRQ_NUM) {
		printf("ERROR: irq_no too");
		return -1;
	} else {
		div = irq_no / 4;
		mod = irq_no % 4;

		rdata = get_wvalue(GIC_SPI_PRIO(div)) & (~(0xFF << mod));
		rdata |= (param << mod);
		put_wvalue(GIC_SPI_PRIO(div), rdata);
	}
	return ret;
}
