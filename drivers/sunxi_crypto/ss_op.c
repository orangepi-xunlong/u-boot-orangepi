/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/ce.h>

static int ss_base_mode;

__weak u32 ss_get_addr_align(void)
{
#if defined(CONFIG_MACH_SUN50IW9)
	return 2; /*addr must be word align, for aceess 4G space of ddr*/
#else
	return 0;
#endif
}

__weak int ss_get_ver(void)
{
#ifdef SS_VER
	/* CE 2.0 */
	u8 val = (readl(SS_VER) >> 8) & 0xf;
	return val;
#else
	return 0;
#endif
}

__weak void ss_set_drq(u32 addr)
{
	writel(addr, SS_TDQ);
}

__weak void ss_ctrl_start(u8 alg_type)
{
	if (ss_get_ver() == 2) {
		/* CE 2.0 */
		writel(alg_type << 8, SS_TLR);
	}
	writel(readl(SS_TLR) | 0x1, SS_TLR);
}

__weak void ss_ctrl_stop(void)
{
	writel(0x0, SS_TLR);
}

__weak void ss_wait_finish(u32 task_id)
{
	uint int_en;
	int_en = readl(SS_ICR) & 0xf;
	int_en = int_en & (0x01 << task_id);
	if (int_en != 0) {
		while ((readl(SS_ISR) & (0x01 << task_id)) == 0) {
		};
	}
}
__weak void ss_pending_clear(u32 task_id)
{
	u32 reg_val;
	reg_val = readl(SS_ISR);
	if ((reg_val & (0x01 << task_id)) == (0x01 << task_id)) {
		reg_val &= ~(0x0f);
		reg_val |= (0x01 << task_id);
	}
	writel(reg_val, SS_ISR);
}

__weak void ss_irq_enable(u32 task_id)
{
	int val = readl(SS_ICR);

	val |= (0x1 << task_id);
	writel(val, SS_ICR);
}

__weak void ss_irq_disable(u32 task_id)
{
	int val = readl(SS_ICR);

	val &= ~(1 << task_id);
	writel(val, SS_ICR);
}

__weak u32 ss_check_err(void)
{
	return (readl(SS_ERR) & 0xffff);
}

__weak void ss_open(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	u32 reg_val;
	static int initd;

	if (initd)
		return;
	initd = 1;

#if defined(CONFIG_SUNXI_VERSION1)
	/* enable SS working clock */
	reg_val = readl(&ccm->ss_clk_cfg); /* SS CLOCK */
	reg_val &= ~(0xf << 24);
	reg_val |= 0x1 << 24;
	reg_val &= ~(0x3 << 16);
	reg_val |= 0x0 << 16;
	reg_val &= ~(0xf);
	reg_val |= (4 - 1);
	reg_val |= 0x1U << 31;
	writel(reg_val, &ccm->ss_clk_cfg);
	/* enable SS AHB clock */
	reg_val = readl(&ccm->ahb_gate0);
	reg_val |= 0x1 << 5; /* SS AHB clock on */
	writel(reg_val, &ccm->ahb_gate0);
	/* del-assert SS reset */
	reg_val = readl(&ccm->ahb_reset0_cfg);
	reg_val |= 0x1 << 5; /* SS AHB clock reset */
	writel(reg_val, &ccm->ahb_reset0_cfg);
#else
	reg_val = readl(&ccm->ce_clk_cfg);

	/*set CE src clock*/
	reg_val &= ~(CE_CLK_SRC_MASK << CE_CLK_SRC_SEL_BIT);
	reg_val |= CE_CLK_SRC << CE_CLK_SRC_SEL_BIT;
	/*set div n*/
	reg_val &= ~(CE_CLK_DIV_RATION_N_MASK << CE_CLK_DIV_RATION_N_BIT);
	reg_val |= CE_CLK_DIV_RATION_N << CE_CLK_DIV_RATION_N_BIT;
	/*set div m*/
	reg_val &= ~(CE_CLK_DIV_RATION_M_MASK << CE_CLK_DIV_RATION_M_BIT);
	reg_val |= CE_CLK_DIV_RATION_M << CE_CLK_DIV_RATION_M_BIT;
	/*set src clock on*/
	reg_val |= CE_SCLK_ON << CE_SCLK_ONOFF_BIT;

	writel(reg_val, &ccm->ce_clk_cfg);

	/*open CE gating*/
	reg_val = readl(&ccm->ce_gate_reset);
	reg_val |= CE_GATING_PASS << CE_GATING_BIT;
	writel(reg_val, &ccm->ce_gate_reset);

	/*de-assert*/
	reg_val = readl(&ccm->ce_gate_reset);
	reg_val |= CE_DEASSERT << CE_RST_BIT;
	writel(reg_val, &ccm->ce_gate_reset);

	reg_val = readl(&ccm->mbus_gate);
	reg_val &= ~(CE_MBUS_GATING_MASK << CE_MBUS_GATING_BIT);
	reg_val |= CE_MBUS_GATING << CE_MBUS_GATING_BIT;
	writel(reg_val, &ccm->mbus_gate);
#endif
}

__weak void ss_close(void)
{
}
