/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include "common.h"
#include "asm/io.h"
#include "asm/arch/ccmu.h"
#include "asm/arch/ss.h"

static int ss_base_mode = 0;

__weak int ss_get_ver(void)
{
#ifdef SS_VER
	/* CE 2.0 */
	u8 val = (readl(SS_VER)>>8) & 0xf;
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
	if(ss_get_ver() == 2)
	{
		/* CE 2.0 */
		writel(alg_type<<8, SS_TLR);
	}
	writel(readl(SS_TLR)|0x1, SS_TLR);
}

__weak void ss_ctrl_stop(void)
{
	writel(0x0, SS_TLR);
}

__weak void ss_wait_finish(u32 task_id)
{
	uint int_en;
	int_en = readl(SS_ICR) & 0xf;
	int_en = int_en&(0x01<<task_id);
	if(int_en!=0)
	{
	   while((readl(SS_ISR)&(0x01<<task_id))==0) {};
	}
}
__weak void ss_pending_clear(u32 task_id)
{
	u32 reg_val;
	reg_val = readl(SS_ISR);
	if((reg_val&(0x01<<task_id))==(0x01<<task_id))
	{
	   reg_val &= ~(0x0f);
	   reg_val |= (0x01<<task_id);
	}
	writel(reg_val, SS_ISR);
}

__weak void ss_irq_enable(u32 task_id)
{
	int val = readl(SS_ICR);

	val |= (0x1<<task_id);
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
	u32  reg_val;

	reg_val = readl(CCMU_CE_CLK_REG);

	/*set CE src clock*/
	reg_val &= ~(CE_CLK_SRC_MASK<<CE_CLK_SRC_SEL_BIT);
	reg_val |= CE_CLK_SRC<<CE_CLK_SRC_SEL_BIT;
	/*set div n*/
	reg_val &= ~(CE_CLK_DIV_RATION_N_MASK<<CE_CLK_DIV_RATION_N_BIT);
	reg_val |= CE_CLK_DIV_RATION_N<<CE_CLK_DIV_RATION_N_BIT;
	/*set div m*/
	reg_val &= ~(CE_CLK_DIV_RATION_M_MASK<<CE_CLK_DIV_RATION_M_BIT);
	reg_val |= CE_CLK_DIV_RATION_M<<CE_CLK_DIV_RATION_M_BIT;
	/*set src clock on*/
	reg_val |= CE_SCLK_ON<<CE_SCLK_ONOFF_BIT;

	writel(reg_val,CCMU_CE_CLK_REG);

	/*open CE gating*/
	reg_val = readl(CE_GATING_BASE);
	reg_val |= CE_GATING_PASS<<CE_GATING_BIT;
	writel(reg_val,CE_GATING_BASE);

	/*assert*/
	reg_val = readl(CE_RST_REG_BASE);
	reg_val &= ~(CE_DEASSERT<<CE_RST_BIT);
	writel(reg_val,CE_RST_REG_BASE);

	/*de-assert*/
	reg_val = readl(CE_RST_REG_BASE);
	reg_val |= CE_DEASSERT<<CE_RST_BIT;
	writel(reg_val,CE_RST_REG_BASE);
}

__weak void ss_close(void)
{
}

