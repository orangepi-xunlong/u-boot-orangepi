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
#include <asm/arch/efuse.h>

static int ss_base_mode;

__weak void ss_get_soc_status(void)
{
#ifdef CONFIG_SUNXI_SECURE_CRYPTO
	if (sid_get_security_status()) {
		ss_base_mode = 0;
	} else {
		ss_base_mode = 1;
	}
#else
	ss_base_mode = 0;
#endif
}


__weak int ss_get_ver(void)
{
	u8 val = (readl(SS_VER) >> 8) & 0xf;
	return val;
}

__weak void ss_set_drq(u32 addr)
{
	writel(addr, SS_TDQ);
}

__weak void ss_ctrl_start(u8 alg_type)
{
	u32 val = 0;
	while ((readl(SS_TLR) & (0x1 << alg_type)) == 1) {
	};
	val = readl(SS_TLR);
	val |= (0x1 << alg_type);
	writel(val, SS_TLR);
}

__weak void ss_ctrl_stop(void)
{
	writel(0x0, SS_TLR);
}

__weak u32 ss_check_err(u32 channel_id)
{
	return (readl(SS_ERR) & (0xff << channel_id));
}

__weak void ss_wait_finish(u32 task_id)
{
	uint int_en;
	int_en = readl(SS_ICR) & 0xf;
	int_en = int_en & (0x01 << task_id * 2);
	if (int_en != 0) {
		while ((readl(SS_ISR) & (0x01 << task_id * 2)) == 0) {
			;
		}
	}
}

__weak u32 ss_pending_clear(u32 task_id)
{
	u32 reg_val;
	u32 res, ret = 0;
	reg_val = readl(SS_ISR);
	res	= reg_val & (0x3 << task_id * 2);

	if (res == 0x1) {
		ret = 0;
	} else if (res == 0x2) {
		ret = ss_check_err(task_id);
	}
	reg_val = (reg_val & 0xff) | (0x3 << task_id * 2);
	writel(reg_val, SS_ISR);
	writel(0x0, SS_ICR);
	return ret;
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

__weak void ss_open(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	u32 reg_val;
	static int initd;

	ss_get_soc_status();

	if (initd)
		return;
	initd = 1;

	reg_val = readl(&ccm->ce_clk_cfg); /*ce CLOCK*/
	reg_val &= (~((0x3 << 8) | (0xf << 0)));
	reg_val |= (0x0 << 0);
	writel(reg_val, &ccm->ce_clk_cfg);

	reg_val = readl(&ccm->ce_clk_cfg);

	reg_val = readl(&ccm->ce_clk_cfg);

	/*set CE src clock*/
	reg_val &= ~(CE_CLK_SRC_MASK << CE_CLK_SRC_SEL_BIT);
	udelay(10);
#ifdef FPGA_PLATFORM
	/* OSC24M */
	reg_val |= 0 << CE_CLK_SRC_SEL_BIT;
#else
	reg_val |= CE_CLK_SRC << CE_CLK_SRC_SEL_BIT;
	/*set div n*/
	reg_val &= ~(CE_CLK_DIV_RATION_N_MASK << CE_CLK_DIV_RATION_N_BIT);
	reg_val |= CE_CLK_DIV_RATION_N << CE_CLK_DIV_RATION_N_BIT;
	/*set div m*/
	reg_val &= ~(CE_CLK_DIV_RATION_M_MASK << CE_CLK_DIV_RATION_M_BIT);
	reg_val |= CE_CLK_DIV_RATION_M << CE_CLK_DIV_RATION_M_BIT;
#endif
	/*set src clock on*/
	reg_val |= CE_SCLK_ON << CE_SCLK_ONOFF_BIT;

	writel(reg_val, &ccm->ce_clk_cfg);

	/*open CE gating*/
	reg_val = readl(&ccm->ce_gate_reset);
	reg_val |= CE_GATING_PASS << CE_GATING_BIT;
	reg_val |= CE_GATING_PASS << CE_SYS_GATING_BIT;
	writel(reg_val, &ccm->ce_gate_reset);

	reg_val = readl(&ccm->mbus_gate);
	reg_val &= ~(CE_MBUS_GATING_MASK << CE_MBUS_GATING_BIT);
	reg_val |= CE_MBUS_GATING << CE_MBUS_GATING_BIT;
	writel(reg_val, &ccm->mbus_gate);

	/*de-assert*/
	reg_val = readl(&ccm->ce_gate_reset);
	reg_val |= CE_DEASSERT << CE_RST_BIT;
	reg_val |= CE_DEASSERT << CE_SYS_RST_BIT;
	writel(reg_val, &ccm->ce_gate_reset);
}

__weak void ss_close(void)
{
}
