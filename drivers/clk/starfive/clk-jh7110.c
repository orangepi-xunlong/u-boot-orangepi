// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022-2023 StarFive Technology Co., Ltd.
 * Author:	yanhong <yanhong.wang@starfivetech.com>
 *
 */
#include <common.h>
#include <asm/arch/clk.h>
#include <clk-uclass.h>
#include <clk.h>
#include <dt-bindings/clock/starfive-jh7110-clkgen.h>
#include <dm/device.h>
#include <dm/devres.h>
#include <dm.h>
#include <malloc.h>
#include <log.h>
#include <linux/clk-provider.h>
#include <linux/io.h>

#define STARFIVE_CLK_ENABLE_SHIFT	31 /*[31]*/
#define STARFIVE_CLK_INVERT_SHIFT	30 /*[30]*/
#define STARFIVE_CLK_MUX_SHIFT		24 /*[29:24]*/
#define STARFIVE_CLK_DIV_SHIFT		0  /*[23:0]*/

#define SYS_OFFSET(id) ((id) * 4)
#define STG_OFFSET(id) (((id) - JH7110_CLK_SYS_REG_END) * 4)
#define AON_OFFSET(id) (((id) - JH7110_CLK_STG_REG_END) * 4)
#define VOUT_OFFSET(id) (((id) - JH7110_CLK_VOUT_START) * 4)

struct jh7110_clk_priv {
	void __iomem *sys;
	void __iomem *stg;
	void __iomem *aon;
	void __iomem *vout;
};

static const char *cpu_root_sels[2] = {
	[0] = "osc",
	[1] = "pll0_out",
};

static const char *perh_root_sels[2] = {
	[0] = "pll0_out",
	[1] = "pll2_out",
};

static const char *bus_root_sels[2] = {
	[0] = "osc",
	[1] = "pll2_out",
};

static const char *qspi_ref_sels[2] = {
	[0] = "osc",
	[1] = "u0_cdns_qspi_ref_src",
};

static const char *gmac5_tx_sels[2] = {
	[0] = "gmac1_gtxclk",
	[1] = "gmac1_rmii_rtx",
};

static const char *u0_dw_gmac5_axi64_clk_tx_sels[2] = {
	[0] = "gmac0_gtxclk",
	[1] = "gmac0_rmii_rtx",
};

static const char *mclk_sels[2] = {
	[0] = "mclk_inner",
	[1] = "mclk_ext",
};

static const char *u0_i2stx_4ch_bclk_sels[2] = {
	[0] = "i2stx_4ch0_bclk_mst",
	[1] = "i2stx_bclk_ext",
};

static const char *u0_dc8200_clk_pix_sels[2] = {
	[0] = "dc8200_pix0",
	[1] = "hdmitx0_pixelclk",
};

static const char *dom_vout_top_lcd_clk_sels[2] = {
	[0] = "u0_dc8200_clk_pix0_out",
	[1] = "u0_dc8200_clk_pix1_out",
};

static const char *u0_cdns_dsitx_clk_sels[2] = {
	[0] = "dc8200_pix0",
	[1] = "hdmitx0_pixelclk",
};

static ulong starfive_clk_get_rate(struct clk *clk)
{
	struct clk *c;
	int ret;

	debug("%s(#%lu)\n", __func__, clk->id);

	ret = clk_get_by_id(clk->id, &c);
	if (ret)
		return ret;

	return clk_get_rate(c);
}

static ulong starfive_clk_set_rate(struct clk *clk, unsigned long rate)
{
	struct clk *c;
	int ret;

	debug("%s(#%lu), rate: %lu\n", __func__, clk->id, rate);

	ret = clk_get_by_id(clk->id, &c);
	if (ret)
		return ret;

	return clk_set_rate(c, rate);
}

static int __starfive_clk_enable(struct clk *clk, bool enable)
{
	struct clk *c;
	int ret;

	debug("%s(#%lu) en: %d\n", __func__, clk->id, enable);

	ret = clk_get_by_id(clk->id, &c);
	if (ret)
		return ret;

	if (enable)
		ret = clk_enable(c);
	else
		ret = clk_disable(c);

	return ret;
}

static int starfive_clk_disable(struct clk *clk)
{
	return __starfive_clk_enable(clk, 0);
}

static int starfive_clk_enable(struct clk *clk)
{
	return __starfive_clk_enable(clk, 1);
}

static int starfive_clk_set_parent(struct clk *clk, struct clk *parent)
{
	struct clk *c, *cp;
	int ret;

	debug("%s(#%lu), parent: %lu\n", __func__, clk->id, parent->id);

	ret = clk_get_by_id(clk->id, &c);
	if (ret)
		return ret;

	ret = clk_get_by_id(parent->id, &cp);
	if (ret)
		return ret;

	ret = clk_set_parent(c, cp);
	c->dev->parent = cp->dev;

	return ret;
}

static const struct clk_ops starfive_clk_ops = {
	.set_rate = starfive_clk_set_rate,
	.get_rate = starfive_clk_get_rate,
	.enable = starfive_clk_enable,
	.disable = starfive_clk_disable,
	.set_parent = starfive_clk_set_parent,
};

static struct clk *starfive_clk_mux(void __iomem *reg,
				const char *name,
				unsigned int offset,
				u8 width,
				const char * const *parent_names,
				u8 num_parents)
{
	return  clk_register_mux(NULL, name, parent_names, num_parents, 0,
				reg + offset, STARFIVE_CLK_MUX_SHIFT, width, 0);
}

static struct clk *starfive_clk_gate(void __iomem *reg,
				const char *name,
				const char *parent_name,
				unsigned int offset)
{
	return clk_register_gate(NULL, name, parent_name, 0, reg+offset,
				STARFIVE_CLK_ENABLE_SHIFT, 0, NULL);
}

static struct clk *starfive_clk_fix_factor(void __iomem *reg,
					const char *name,
					const char *parent_name,
					unsigned int mult,
					unsigned int div)
{
	return clk_register_fixed_factor(NULL, name, parent_name,
		0, mult, div);

}

static struct clk *starfive_clk_divider(void __iomem *reg,
					const char *name,
					const char *parent_name,
					unsigned int offset,
					u8 width)
{
	return clk_register_divider(NULL, name, parent_name, 0, reg+offset, 0,
				width, CLK_DIVIDER_ONE_BASED);
}


static struct clk *starfive_clk_composite(void __iomem *reg,
				const char *name,
				const char * const *parent_names,
				unsigned int num_parents,
				unsigned int offset,
				unsigned int mux_width,
				unsigned int gate_width,
				unsigned int div_width)
{
	struct clk *clk = ERR_PTR(-ENOMEM);
	struct clk_divider *div = NULL;
	struct clk_gate *gate = NULL;
	struct clk_mux *mux = NULL;
	int mask_arry[4] = {0x1, 0x3, 0x7, 0xF};
	int mask;

	if (mux_width) {
		if (mux_width > 4)
			goto fail;
		else
			mask = mask_arry[mux_width-1];

		mux = kzalloc(sizeof(*mux), GFP_KERNEL);
		if (!mux)
			goto fail;

		mux->reg = reg + offset;
		mux->mask = mask;
		mux->shift = STARFIVE_CLK_MUX_SHIFT;
		mux->num_parents = num_parents;
		mux->flags = 0;
		mux->parent_names = parent_names;
	}

	if (gate_width) {
		gate = kzalloc(sizeof(*gate), GFP_KERNEL);

		if (!gate)
			goto fail;

		gate->reg = reg + offset;
		gate->bit_idx = STARFIVE_CLK_ENABLE_SHIFT;
		gate->flags = 0;
	}

	if (div_width) {
		div = kzalloc(sizeof(*div), GFP_KERNEL);
		if (!div)
			goto fail;

		div->reg = reg + offset;
		if ((offset == SYS_OFFSET(JH7110_UART3_CLK_CORE)) ||
			(offset == SYS_OFFSET(JH7110_UART4_CLK_CORE)) ||
			(offset == SYS_OFFSET(JH7110_UART5_CLK_CORE))) {
			div->shift = 8;
			div->width = 8;
		} else {
			div->shift = STARFIVE_CLK_DIV_SHIFT;
			div->width = div_width;
		}
		div->flags = CLK_DIVIDER_ONE_BASED;
		div->table = NULL;
	}

	clk = clk_register_composite(NULL, name,
				 parent_names, num_parents,
				 &mux->clk, &clk_mux_ops,
				 &div->clk, &clk_divider_ops,
				 &gate->clk, &clk_gate_ops, 0);

	if (IS_ERR(clk))
		goto fail;

	return clk;

fail:
	kfree(gate);
	kfree(div);
	kfree(mux);
	return ERR_CAST(clk);
}

static struct clk *starfive_clk_fix_parent_composite(void __iomem *reg,
						 const char *name,
						 const char *parent_names,
						 unsigned int offset,
						 unsigned int mux_width,
						 unsigned int gate_width,
						 unsigned int div_width)
{
	const char * const *parents;

	parents  = &parent_names;

	return starfive_clk_composite(reg, name, parents, 1, offset,
			mux_width, gate_width, div_width);
}

static struct clk *starfive_clk_gate_divider(void __iomem *reg,
					const char *name,
					const char *parent,
					unsigned int offset,
					unsigned int width)
{
	const char * const *parent_names;

	parent_names  = &parent;

	return starfive_clk_composite(reg, name, parent_names, 1,
				offset, 0, 1, width);
}

static int jh7110_clk_init(struct udevice *dev)
{
	struct jh7110_clk_priv *priv = dev_get_priv(dev);
	struct starfive_pll_freq conf;
	u64 pll;

	pll = starfive_jh7110_pll_get_rate(PLL0, &conf);
	clk_dm(JH7110_PLL0_OUT,
	       starfive_clk_fix_factor(priv->sys,
				       "pll0_out", "osc", conf.fbdiv,
				       conf.prediv * conf.postdiv1));

	pll = starfive_jh7110_pll_get_rate(PLL1, &conf);
	clk_dm(JH7110_PLL1_OUT,
	       starfive_clk_fix_factor(priv->sys,
				       "pll1_out", "osc", conf.fbdiv,
				       conf.prediv * conf.postdiv1));

	pll = starfive_jh7110_pll_get_rate(PLL2, &conf);
	clk_dm(JH7110_PLL2_OUT,
	       starfive_clk_fix_factor(priv->sys,
				       "pll2_out", "osc", conf.fbdiv,
				       conf.prediv * conf.postdiv1));
	/*root*/
	clk_dm(JH7110_CPU_ROOT,
	       starfive_clk_mux(priv->sys, "cpu_root",
				SYS_OFFSET(JH7110_CPU_ROOT), 1,
				cpu_root_sels, ARRAY_SIZE(cpu_root_sels)));
	clk_dm(JH7110_CPU_CORE,
	       starfive_clk_divider(priv->sys,
				    "cpu_core", "cpu_root",
				    SYS_OFFSET(JH7110_CPU_CORE), 3));
	clk_dm(JH7110_CPU_BUS,
	       starfive_clk_divider(priv->sys,
				    "cpu_bus", "cpu_core",
				    SYS_OFFSET(JH7110_CPU_BUS), 2));
	clk_dm(JH7110_DDR_ROOT,
	       starfive_clk_fix_factor(priv->sys,
				       "ddr_root", "pll1_out", 1, 1));
	clk_dm(JH7110_GMACUSB_ROOT,
	       starfive_clk_fix_factor(priv->sys,
				       "gmacusb_root", "pll0_out", 1, 1));
	clk_dm(JH7110_PERH_ROOT,
	       starfive_clk_composite(priv->sys,
				      "perh_root",
				      perh_root_sels, ARRAY_SIZE(perh_root_sels),
				      SYS_OFFSET(JH7110_PERH_ROOT), 1, 0, 2));

	clk_dm(JH7110_BUS_ROOT,
	       starfive_clk_mux(priv->sys, "bus_root",
				SYS_OFFSET(JH7110_BUS_ROOT), 1,
				bus_root_sels,	ARRAY_SIZE(bus_root_sels)));
	clk_dm(JH7110_AXI_CFG0,
	       starfive_clk_divider(priv->sys,
				    "axi_cfg0", "bus_root",
				    SYS_OFFSET(JH7110_AXI_CFG0), 2));
	clk_dm(JH7110_STG_AXIAHB,
	       starfive_clk_divider(priv->sys,
				    "stg_axiahb", "axi_cfg0",
				    SYS_OFFSET(JH7110_STG_AXIAHB), 2));
	clk_dm(JH7110_AHB0,
	       starfive_clk_gate(priv->sys,
				 "ahb0", "stg_axiahb",
				 SYS_OFFSET(JH7110_AHB0)));
	clk_dm(JH7110_AHB1,
	       starfive_clk_gate(priv->sys,
				 "ahb1", "stg_axiahb",
				 SYS_OFFSET(JH7110_AHB1)));
	clk_dm(JH7110_APB_BUS_FUNC,
	       starfive_clk_divider(priv->sys,
				    "apb_bus_func", "stg_axiahb",
				    SYS_OFFSET(JH7110_APB_BUS_FUNC), 4));
	clk_dm(JH7110_PCLK2_MUX_FUNC_PCLK,
	       starfive_clk_fix_factor(priv->sys,
				       "u2_pclk_mux_func_pclk", "apb_bus_func", 1, 1));
	clk_dm(JH7110_U2_PCLK_MUX_PCLK,
	       starfive_clk_fix_factor(priv->sys,
				       "u2_pclk_mux_pclk", "u2_pclk_mux_func_pclk", 1, 1));

	clk_dm(JH7110_APB_BUS,
	       starfive_clk_fix_factor(priv->sys,
				       "apb_bus", "u2_pclk_mux_pclk", 1, 1));
	clk_dm(JH7110_APB0,
	       starfive_clk_gate(priv->sys,
				 "apb0", "apb_bus",
				 SYS_OFFSET(JH7110_APB0)));
	clk_dm(JH7110_APB12,
	       starfive_clk_fix_factor(priv->sys,
				       "apb12", "apb_bus", 1, 1));
	clk_dm(JH7110_AON_APB,
	       starfive_clk_fix_factor(priv->sys,
				       "aon_apb", "apb_bus_func", 1, 1));
	clk_dm(JH7110_NOCSTG_BUS,
	       starfive_clk_divider(priv->sys,
				    "nocstg_bus", "bus_root",
				    SYS_OFFSET(JH7110_NOCSTG_BUS), 3));
	/*QSPI*/
	clk_dm(JH7110_QSPI_CLK_AHB,
	       starfive_clk_gate(priv->sys,
				 "u0_cdns_qspi_clk_ahb", "ahb1",
				 SYS_OFFSET(JH7110_QSPI_CLK_AHB)));
	clk_dm(JH7110_QSPI_CLK_APB,
	       starfive_clk_gate(priv->sys,
				 "u0_cdns_qspi_clk_apb", "apb12",
				 SYS_OFFSET(JH7110_QSPI_CLK_APB)));
	clk_dm(JH7110_QSPI_REF_SRC,
	       starfive_clk_divider(priv->sys,
				    "u0_cdns_qspi_ref_src", "gmacusb_root",
				    SYS_OFFSET(JH7110_QSPI_REF_SRC), 5));
	clk_dm(JH7110_QSPI_CLK_REF,
	       starfive_clk_composite(priv->sys,
				      "u0_cdns_qspi_clk_ref",
				      qspi_ref_sels, ARRAY_SIZE(qspi_ref_sels),
				      SYS_OFFSET(JH7110_QSPI_CLK_REF), 1, 1, 0));

	/*SDIO*/
	clk_dm(JH7110_SDIO0_CLK_AHB,
	       starfive_clk_gate(priv->sys,
				 "u0_dw_sdio_clk_ahb", "ahb0",
				 SYS_OFFSET(JH7110_SDIO0_CLK_AHB)));
	clk_dm(JH7110_SDIO1_CLK_AHB,
	       starfive_clk_gate(priv->sys,
				 "u1_dw_sdio_clk_ahb", "ahb0",
				 SYS_OFFSET(JH7110_SDIO1_CLK_AHB)));
	clk_dm(JH7110_SDIO0_CLK_SDCARD,
	       starfive_clk_fix_parent_composite(priv->sys,
						 "u0_dw_sdio_clk_sdcard", "axi_cfg0",
						 SYS_OFFSET(JH7110_SDIO0_CLK_SDCARD), 0, 1, 4));
	clk_dm(JH7110_SDIO1_CLK_SDCARD,
	       starfive_clk_fix_parent_composite(priv->sys,
						 "u1_dw_sdio_clk_sdcard", "axi_cfg0",
						 SYS_OFFSET(JH7110_SDIO1_CLK_SDCARD), 0, 1, 4));

	/*STG*/
	clk_dm(JH7110_USB_125M,
	       starfive_clk_divider(priv->sys,
				    "usb_125m", "gmacusb_root",
				    SYS_OFFSET(JH7110_USB_125M), 4));
	clk_dm(JH7110_NOC_BUS_CLK_STG_AXI,
	       starfive_clk_gate(priv->sys,
				    "u0_sft7110_noc_bus_clk_stg_axi",
				    "nocstg_bus",
				    SYS_OFFSET(JH7110_NOC_BUS_CLK_STG_AXI)));

	/*GMAC1*/
	clk_dm(JH7110_GMAC5_CLK_AHB,
	       starfive_clk_gate(priv->sys,
				 "u1_dw_gmac5_axi64_clk_ahb", "ahb0",
				 SYS_OFFSET(JH7110_GMAC5_CLK_AHB)));
	clk_dm(JH7110_GMAC5_CLK_AXI,
	       starfive_clk_gate(priv->sys,
				 "u1_dw_gmac5_axi64_clk_axi", "stg_axiahb",
				 SYS_OFFSET(JH7110_GMAC5_CLK_AXI)));
	clk_dm(JH7110_GMAC_SRC,
	       starfive_clk_divider(priv->sys,
				    "gmac_src", "gmacusb_root",
				    SYS_OFFSET(JH7110_GMAC_SRC), 3));
	clk_dm(JH7110_GMAC1_GTXCLK,
	       starfive_clk_divider(priv->sys,
				    "gmac1_gtxclk", "gmacusb_root",
				    SYS_OFFSET(JH7110_GMAC1_GTXCLK), 4));
	clk_dm(JH7110_GMAC1_GTXC,
	       starfive_clk_gate(priv->sys,
				 "gmac1_gtxc", "gmac1_gtxclk",
				 SYS_OFFSET(JH7110_GMAC1_GTXC)));
	clk_dm(JH7110_GMAC1_RMII_RTX,
	       starfive_clk_divider(priv->sys,
				    "gmac1_rmii_rtx", "gmac1_rmii_refin",
				    SYS_OFFSET(JH7110_GMAC1_RMII_RTX), 5));
	clk_dm(JH7110_GMAC5_CLK_PTP,
	       starfive_clk_gate_divider(priv->sys,
					 "u1_dw_gmac5_axi64_clk_ptp", "gmac_src",
					 SYS_OFFSET(JH7110_GMAC5_CLK_PTP), 5));
	clk_dm(JH7110_GMAC5_CLK_TX,
	       starfive_clk_composite(priv->sys,
				      "u1_dw_gmac5_axi64_clk_tx",
				      gmac5_tx_sels, ARRAY_SIZE(gmac5_tx_sels),
				      SYS_OFFSET(JH7110_GMAC5_CLK_TX), 1, 1, 0));
	/*GMAC0*/
	clk_dm(JH7110_AON_AHB,
	       starfive_clk_fix_factor(priv->sys, "aon_ahb",
				       "stg_axiahb", 1, 1));
	clk_dm(JH7110_GMAC0_GTXCLK,
	       starfive_clk_gate_divider(priv->sys, "gmac0_gtxclk",
					 "gmacusb_root", SYS_OFFSET(JH7110_GMAC0_GTXCLK), 4));
	clk_dm(JH7110_GMAC0_PTP,
	       starfive_clk_gate_divider(priv->sys, "gmac0_ptp",
					 "gmac_src", SYS_OFFSET(JH7110_GMAC0_PTP), 5));
	clk_dm(JH7110_GMAC0_GTXC,
	       starfive_clk_gate(priv->sys,
				 "gmac0_gtxc", "gmac0_gtxclk",
				 SYS_OFFSET(JH7110_GMAC0_GTXC)));
	/*UART0*/
	clk_dm(JH7110_UART0_CLK_APB,
	       starfive_clk_gate(priv->sys,
				 "u0_dw_uart_clk_apb", "apb0",
				 SYS_OFFSET(JH7110_UART0_CLK_APB)));
	clk_dm(JH7110_UART0_CLK_CORE,
	       starfive_clk_gate(priv->sys,
				 "u0_dw_uart_clk_core", "osc",
				 SYS_OFFSET(JH7110_UART0_CLK_CORE)));

	/*UART1*/
	clk_dm(JH7110_UART1_CLK_APB,
	       starfive_clk_gate(priv->sys,
				 "u1_dw_uart_clk_apb", "apb0",
				 SYS_OFFSET(JH7110_UART1_CLK_APB)));
	clk_dm(JH7110_UART1_CLK_CORE,
	       starfive_clk_gate(priv->sys,
				 "u1_dw_uart_clk_core", "osc",
				 SYS_OFFSET(JH7110_UART1_CLK_CORE)));

	/*UART2*/
	clk_dm(JH7110_UART2_CLK_APB,
	       starfive_clk_gate(priv->sys,
				 "u2_dw_uart_clk_apb", "apb0",
				 SYS_OFFSET(JH7110_UART2_CLK_APB)));
	clk_dm(JH7110_UART2_CLK_CORE,
	       starfive_clk_gate(priv->sys,
				 "u2_dw_uart_clk_core", "osc",
				 SYS_OFFSET(JH7110_UART2_CLK_CORE)));

	/*UART3*/
	clk_dm(JH7110_UART3_CLK_APB,
	       starfive_clk_gate(priv->sys,
				 "u3_dw_uart_clk_apb", "apb0",
				 SYS_OFFSET(JH7110_UART3_CLK_APB)));
	clk_dm(JH7110_UART3_CLK_CORE,
	       starfive_clk_gate_divider(priv->sys,
					 "u3_dw_uart_clk_core", "perh_root",
					 SYS_OFFSET(JH7110_UART3_CLK_CORE), 8));

	/*UART4*/
	clk_dm(JH7110_UART4_CLK_APB,
	       starfive_clk_gate(priv->sys,
				 "u4_dw_uart_clk_apb", "apb0",
				 SYS_OFFSET(JH7110_UART4_CLK_APB)));
	clk_dm(JH7110_UART4_CLK_CORE,
	       starfive_clk_gate_divider(priv->sys,
					 "u4_dw_uart_clk_core", "perh_root",
					 SYS_OFFSET(JH7110_UART4_CLK_CORE), 8));

	/*UART5*/
	clk_dm(JH7110_UART5_CLK_APB,
	       starfive_clk_gate(priv->sys,
				 "u5_dw_uart_clk_apb", "apb0",
				 SYS_OFFSET(JH7110_UART5_CLK_APB)));
	clk_dm(JH7110_UART5_CLK_CORE,
	       starfive_clk_gate_divider(priv->sys,
					 "u5_dw_uart_clk_core", "perh_root",
					 SYS_OFFSET(JH7110_UART5_CLK_CORE), 8));

	clk_dm(JH7110_STG_APB,
	       starfive_clk_fix_factor(priv->stg,
				       "stg_apb", "apb_bus", 1, 1));
	/*USB*/
	clk_dm(JH7110_USB0_CLK_USB_APB,
	       starfive_clk_gate(priv->stg,
				 "u0_cdn_usb_clk_usb_apb", "stg_apb",
				 STG_OFFSET(JH7110_USB0_CLK_USB_APB)));
	clk_dm(JH7110_USB0_CLK_UTMI_APB,
	       starfive_clk_gate(priv->stg,
				 "u0_cdn_usb_clk_utmi_apb", "stg_apb",
				 STG_OFFSET(JH7110_USB0_CLK_UTMI_APB)));
	clk_dm(JH7110_USB0_CLK_AXI,
	       starfive_clk_gate(priv->stg,
				 "u0_cdn_usb_clk_axi", "stg_axiahb",
				 STG_OFFSET(JH7110_USB0_CLK_AXI)));
	clk_dm(JH7110_USB0_CLK_LPM,
	       starfive_clk_gate_divider(priv->stg,
					 "u0_cdn_usb_clk_lpm", "osc",
					 STG_OFFSET(JH7110_USB0_CLK_LPM), 2));
	clk_dm(JH7110_USB0_CLK_STB,
	       starfive_clk_gate_divider(priv->stg,
					 "u0_cdn_usb_clk_stb", "osc",
					 STG_OFFSET(JH7110_USB0_CLK_STB), 3));
	clk_dm(JH7110_USB0_CLK_APP_125,
	       starfive_clk_gate(priv->stg,
				 "u0_cdn_usb_clk_app_125", "usb_125m",
				 STG_OFFSET(JH7110_USB0_CLK_APP_125)));
	clk_dm(JH7110_USB0_REFCLK,
	       starfive_clk_divider(priv->stg,
				    "u0_cdn_usb_refclk", "osc",
				    STG_OFFSET(JH7110_USB0_REFCLK), 2));

	/*GMAC1*/
	clk_dm(JH7110_U0_GMAC5_CLK_AHB,
	       starfive_clk_gate(priv->aon,
				 "u0_dw_gmac5_axi64_clk_ahb", "aon_ahb",
				 AON_OFFSET(JH7110_U0_GMAC5_CLK_AHB)));
	clk_dm(JH7110_U0_GMAC5_CLK_AXI,
	       starfive_clk_gate(priv->aon,
				 "u0_dw_gmac5_axi64_clk_axi", "aon_ahb",
				 AON_OFFSET(JH7110_U0_GMAC5_CLK_AXI)));
	clk_dm(JH7110_GMAC0_RMII_RTX,
	       starfive_clk_divider(priv->aon,
				    "gmac0_rmii_rtx", "gmac0_rmii_refin",
				    AON_OFFSET(JH7110_GMAC0_RMII_RTX), 5));
	clk_dm(JH7110_U0_GMAC5_CLK_PTP,
	       starfive_clk_fix_factor(priv->aon,
				       "u0_dw_gmac5_axi64_clk_ptp",
				       "gmac0_ptp", 1, 1));
	clk_dm(JH7110_U0_GMAC5_CLK_TX,
	       starfive_clk_composite(priv->aon,
				      "u0_dw_gmac5_axi64_clk_tx",
				      u0_dw_gmac5_axi64_clk_tx_sels,
				      ARRAY_SIZE(u0_dw_gmac5_axi64_clk_tx_sels),
				      AON_OFFSET(JH7110_U0_GMAC5_CLK_TX), 1, 1, 0));
	/*otp*/
	clk_dm(JH7110_OTPC_CLK_APB,
	       starfive_clk_gate(priv->aon,
				 "u0_otpc_clk_apb",	"aon_apb",
				 AON_OFFSET(JH7110_OTPC_CLK_APB)));
	/*vout*/
	clk_dm(JH7110_VOUT_ROOT,
	       starfive_clk_fix_factor(priv->sys,
				       "vout_root", "pll2_out", 1, 1));
	clk_dm(JH7110_AUDIO_ROOT,
	       starfive_clk_divider(priv->sys,
				    "audio_root", "pll2_out",
				    SYS_OFFSET(JH7110_AUDIO_ROOT), 5));
	clk_dm(JH7110_VOUT_SRC,
	       starfive_clk_gate(priv->sys,
				 "u0_dom_vout_top_clk_vout_src", "vout_root",
				 SYS_OFFSET(JH7110_VOUT_SRC)));
	clk_dm(JH7110_VOUT_AXI,
	       starfive_clk_divider(priv->sys,
				    "vout_axi", "vout_root",
				    SYS_OFFSET(JH7110_VOUT_AXI), 3));
	clk_dm(JH7110_VOUT_TOP_CLK_VOUT_AXI,
	       starfive_clk_gate(priv->sys,
				 "u0_dom_vout_top_vout_axi", "vout_axi",
				 SYS_OFFSET(JH7110_VOUT_TOP_CLK_VOUT_AXI)));
	clk_dm(JH7110_NOC_BUS_CLK_DISP_AXI,
	       starfive_clk_gate(priv->sys,
				 "u0_sft7110_noc_bus_clk_disp_axi", "vout_axi",
				 SYS_OFFSET(JH7110_NOC_BUS_CLK_DISP_AXI)));
	clk_dm(JH7110_VOUT_TOP_CLK_VOUT_AHB,
	       starfive_clk_gate(priv->sys,
				 "u0_dom_vout_top_clk_vout_ahb", "ahb1",
				 SYS_OFFSET(JH7110_VOUT_TOP_CLK_VOUT_AHB)));
	clk_dm(JH7110_MCLK_INNER,
	       starfive_clk_divider(priv->sys,
				    "mclk_inner", "audio_root",
				    SYS_OFFSET(JH7110_MCLK_INNER), 5));
	clk_dm(JH7110_MCLK,
	       starfive_clk_mux(priv->sys, "mclk",
				SYS_OFFSET(JH7110_MCLK), 1,
				mclk_sels,	ARRAY_SIZE(mclk_sels)));
	clk_dm(JH7110_I2STX_4CH0_BCLK_MST,
	       starfive_clk_gate_divider(priv->sys,
					 "i2stx_4ch0_bclk_mst", "mclk",
					 SYS_OFFSET(JH7110_I2STX_4CH0_BCLK_MST), 5));
	clk_dm(JH7110_I2STX0_4CHBCLK,
	       starfive_clk_mux(priv->sys, "u0_i2stx_4ch_bclk",
				SYS_OFFSET(JH7110_I2STX0_4CHBCLK), 1,
				u0_i2stx_4ch_bclk_sels,	ARRAY_SIZE(u0_i2stx_4ch_bclk_sels)));
	clk_dm(JH7110_VOUT_TOP_CLK_HDMITX0_BCLK,
	       starfive_clk_fix_factor(priv->sys,
				       "u0_dom_vout_top_clk_hdmitx0_bclk",
				       "u0_i2stx_4ch_bclk", 1, 1));
	clk_dm(JH7110_VOUT_TOP_CLK_HDMITX0_MCLK,
	       starfive_clk_gate(priv->sys,
				 "u0_dom_vout_top_clk_hdmitx0_mclk", "mclk",
				 SYS_OFFSET(JH7110_VOUT_TOP_CLK_HDMITX0_MCLK)));
	clk_dm(JH7110_VOUT_TOP_CLK_MIPIPHY_REF,
	       starfive_clk_divider(priv->sys,
				    "u0_dom_vout_top_clk_mipiphy_ref", "osc",
				    SYS_OFFSET(JH7110_MCLK_INNER), 2));

	/*i2c5*/
	clk_dm(JH7110_I2C5_CLK_APB,
		starfive_clk_gate(priv->sys,
			"u5_dw_i2c_clk_apb", "apb0",
			SYS_OFFSET(JH7110_I2C5_CLK_APB)));
	clk_dm(JH7110_I2C5_CLK_CORE,
		starfive_clk_gate(priv->sys,
			"u5_dw_i2c_clk_core", "osc",
			SYS_OFFSET(JH7110_I2C5_CLK_CORE)));
	/*i2c2*/
	clk_dm(JH7110_I2C2_CLK_APB,
		starfive_clk_gate(priv->sys,
			"u2_dw_i2c_clk_apb", "apb0",
			SYS_OFFSET(JH7110_I2C2_CLK_APB)));
	clk_dm(JH7110_I2C2_CLK_CORE,
		starfive_clk_fix_factor(priv->sys,
			"u2_dw_i2c_clk_core", "u2_dw_i2c_clk_apb", 1, 1));

	/*pcie0*/
	clk_dm(JH7110_PCIE0_CLK_TL,
		starfive_clk_gate(priv->stg,
			"u0_plda_pcie_clk_tl",
			"stg_axiahb",
			STG_OFFSET(JH7110_PCIE0_CLK_TL)));
	clk_dm(JH7110_PCIE0_CLK_AXI_MST0,
		starfive_clk_gate(priv->stg,
			"u0_plda_pcie_clk_axi_mst0",
			"stg_axiahb",
			STG_OFFSET(JH7110_PCIE0_CLK_AXI_MST0)));
	clk_dm(JH7110_PCIE0_CLK_APB,
		starfive_clk_gate(priv->stg,
			"u0_plda_pcie_clk_apb",
			"stg_apb",
			STG_OFFSET(JH7110_PCIE0_CLK_APB)));

	/*pcie1*/
	clk_dm(JH7110_PCIE1_CLK_TL,
		starfive_clk_gate(priv->stg,
			"u1_plda_pcie_clk_tl",
			"stg_axiahb",
			STG_OFFSET(JH7110_PCIE1_CLK_TL)));
	clk_dm(JH7110_PCIE1_CLK_AXI_MST0,
		starfive_clk_gate(priv->stg,
			"u1_plda_pcie_clk_axi_mst0",
			"stg_axiahb",
			STG_OFFSET(JH7110_PCIE1_CLK_AXI_MST0)));
	clk_dm(JH7110_PCIE1_CLK_APB,
		starfive_clk_gate(priv->stg,
			"u1_plda_pcie_clk_apb",
			"stg_apb",
			STG_OFFSET(JH7110_PCIE1_CLK_APB)));

	return 0;
}

static int jh7110_clk_vout_init(struct udevice *dev)
{
	struct jh7110_clk_priv *priv = dev_get_priv(dev);

	clk_dm(JH7110_DISP_ROOT,
	       starfive_clk_fix_factor(priv->vout,
				       "disp_root", "u0_dom_vout_top_clk_vout_src",
				       1, 1));
	clk_dm(JH7110_DISP_AXI,
	       starfive_clk_fix_factor(priv->vout,
				       "disp_axi", "u0_dom_vout_top_vout_axi",
				       1, 1));
	clk_dm(JH7110_DISP_AHB,
	       starfive_clk_fix_factor(priv->vout,
				       "disp_ahb", "u0_dom_vout_top_clk_vout_ahb",
				       1, 1));
	clk_dm(JH7110_HDMITX0_MCLK,
	       starfive_clk_fix_factor(priv->vout,
				       "hdmitx0_mclk", "u0_dom_vout_top_clk_hdmitx0_mclk",
				       1, 1));
	clk_dm(JH7110_HDMITX0_SCK,
	       starfive_clk_fix_factor(priv->vout,
				       "hdmitx0_sck", "u0_dom_vout_top_clk_hdmitx0_bclk",
				       1, 1));
	clk_dm(JH7110_APB,
	       starfive_clk_divider(priv->vout,
				    "apb", "disp_ahb",
				    VOUT_OFFSET(JH7110_APB), 5));
	clk_dm(JH7110_U0_PCLK_MUX_FUNC_PCLK,
	       starfive_clk_fix_factor(priv->vout,
				       "u0_pclk_mux_func_pclk", "apb",
				       1, 1));
	clk_dm(JH7110_DISP_APB,
	       starfive_clk_fix_factor(priv->vout,
				       "disp_apb", "u0_pclk_mux_func_pclk",
				       1, 1));
	clk_dm(JH7110_TX_ESC,
	       starfive_clk_divider(priv->vout,
				    "tx_esc", "disp_ahb",
				    VOUT_OFFSET(JH7110_TX_ESC), 5));
	clk_dm(JH7110_DC8200_PIX0,
	       starfive_clk_divider(priv->vout,
				    "dc8200_pix0", "disp_root",
				    VOUT_OFFSET(JH7110_DC8200_PIX0), 6));
	clk_dm(JH7110_DSI_SYS,
	       starfive_clk_divider(priv->vout,
				    "dsi_sys", "disp_root",
				    VOUT_OFFSET(JH7110_DSI_SYS), 5));
	clk_dm(JH7110_U0_DC8200_CLK_PIX0,
	       starfive_clk_composite(priv->vout,
				      "u0_dc8200_clk_pix0",
				      u0_dc8200_clk_pix_sels,
				      ARRAY_SIZE(u0_dc8200_clk_pix_sels),
				      VOUT_OFFSET(JH7110_U0_DC8200_CLK_PIX0), 1, 1, 0));
	clk_dm(JH7110_U0_DC8200_CLK_PIX1,
	       starfive_clk_composite(priv->vout,
				      "u0_dc8200_clk_pix1",
				      u0_dc8200_clk_pix_sels,
				      ARRAY_SIZE(u0_dc8200_clk_pix_sels),
				      VOUT_OFFSET(JH7110_U0_DC8200_CLK_PIX1), 1, 1, 0));
	clk_dm(JH7110_U0_DC8200_CLK_AXI,
	       starfive_clk_gate(priv->vout,
				 "u0_dc8200_clk_axi", "disp_axi",
				 VOUT_OFFSET(JH7110_U0_DC8200_CLK_AXI)));
	clk_dm(JH7110_U0_DC8200_CLK_CORE,
	       starfive_clk_gate(priv->vout,
				 "u0_dc8200_clk_core", "disp_axi",
				 VOUT_OFFSET(JH7110_U0_DC8200_CLK_CORE)));
	clk_dm(JH7110_U0_DC8200_CLK_AHB,
	       starfive_clk_gate(priv->vout,
				 "u0_dc8200_clk_ahb", "disp_ahb",
				 VOUT_OFFSET(JH7110_U0_DC8200_CLK_AHB)));
	clk_dm(JH7110_U0_DC8200_CLK_PIX0_OUT,
	       starfive_clk_fix_factor(priv->vout,
				       "u0_dc8200_clk_pix0_out", "u0_dc8200_clk_pix0",
				       1, 1));
	clk_dm(JH7110_U0_DC8200_CLK_PIX1_OUT,
	       starfive_clk_fix_factor(priv->sys,
				       "u0_dc8200_clk_pix1_out", "u0_dc8200_clk_pix1",
				       1, 1));
	clk_dm(JH7110_DOM_VOUT_TOP_LCD_CLK,
	       starfive_clk_composite(priv->vout,
				      "dom_vout_top_lcd_clk",
				      dom_vout_top_lcd_clk_sels,
				      ARRAY_SIZE(dom_vout_top_lcd_clk_sels),
				      VOUT_OFFSET(JH7110_DOM_VOUT_TOP_LCD_CLK), 1, 1, 0));
	clk_dm(JH7110_U0_MIPITX_DPHY_CLK_TXESC,
	       starfive_clk_gate(priv->vout,
				 "u0_mipitx_dphy_clk_txesc", "tx_esc",
				 VOUT_OFFSET(JH7110_U0_MIPITX_DPHY_CLK_TXESC)));
	clk_dm(JH7110_U0_CDNS_DSITX_CLK_SYS,
	       starfive_clk_gate(priv->vout,
				 "u0_cdns_dsiTx_clk_sys", "dsi_sys",
				 VOUT_OFFSET(JH7110_U0_CDNS_DSITX_CLK_SYS)));
	clk_dm(JH7110_U0_CDNS_DSITX_CLK_APB,
	       starfive_clk_gate(priv->vout,
				 "u0_cdns_dsiTx_clk_apb", "dsi_sys",
				 VOUT_OFFSET(JH7110_U0_CDNS_DSITX_CLK_APB)));
	clk_dm(JH7110_U0_CDNS_DSITX_CLK_TXESC,
	       starfive_clk_gate(priv->vout,
				 "u0_cdns_dsiTx_clk_txesc", "tx_esc",
				 VOUT_OFFSET(JH7110_U0_CDNS_DSITX_CLK_TXESC)));
	clk_dm(JH7110_U0_CDNS_DSITX_CLK_DPI,
	       starfive_clk_composite(priv->vout,
				      "u0_cdns_dsitx_clk_api",
				      u0_cdns_dsitx_clk_sels,
				      ARRAY_SIZE(u0_cdns_dsitx_clk_sels),
				      VOUT_OFFSET(JH7110_U0_CDNS_DSITX_CLK_DPI), 1, 1, 0));
	clk_dm(JH7110_U0_HDMI_TX_CLK_SYS,
	       starfive_clk_gate(priv->vout,
				 "u0_hdmi_tx_clk_sys", "disp_apb",
				 VOUT_OFFSET(JH7110_U0_HDMI_TX_CLK_SYS)));
	clk_dm(JH7110_U0_HDMI_TX_CLK_MCLK,
	       starfive_clk_gate(priv->vout,
				 "u0_hdmi_tx_clk_mclk", "hdmitx0_mclk",
				 VOUT_OFFSET(JH7110_U0_HDMI_TX_CLK_MCLK)));
	clk_dm(JH7110_U0_HDMI_TX_CLK_BCLK,
	       starfive_clk_gate(priv->vout,
				 "u0_hdmi_tx_clk_bclk", "hdmitx0_sck",
				 VOUT_OFFSET(JH7110_U0_HDMI_TX_CLK_BCLK)));

	return 0;
}

static int jh7110_clk_vout_probe(struct udevice *dev)
{
	struct jh7110_clk_priv *priv = dev_get_priv(dev);

	priv->vout = (void __iomem *)dev_read_addr(dev);
	if (!priv->vout)
		return -EINVAL;

	return jh7110_clk_vout_init(dev);
}

static int jh7110_clk_probe(struct udevice *dev)
{
	struct jh7110_clk_priv *priv = dev_get_priv(dev);

	priv->aon = dev_remap_addr_name(dev, "aon");
	if (!priv->aon)
		return -EINVAL;

	priv->sys = dev_remap_addr_name(dev, "sys");
	if (!priv->sys)
		return -EINVAL;

	priv->stg = dev_remap_addr_name(dev, "stg");
	if (!priv->stg)
		return -EINVAL;

	return jh7110_clk_init(dev);
}

static const struct udevice_id jh7110_clk_of_match[] = {
	{ .compatible = "starfive,jh7110-clkgen" },
	{ }
};

U_BOOT_DRIVER(jh7110_clk) = {
	.name = "jh7110_clk",
	.id = UCLASS_CLK,
	.of_match = jh7110_clk_of_match,
	.probe = jh7110_clk_probe,
	.ops = &starfive_clk_ops,
	.priv_auto = sizeof(struct jh7110_clk_priv),
};

static const struct udevice_id jh7110_clk_vout_of_match[] = {
	{ .compatible = "starfive,jh7110-clk-vout" },
	{ }
};

U_BOOT_DRIVER(jh7110_clk_vout) = {
	.name = "jh7110_clk_vout",
	.id = UCLASS_CLK,
	.of_match = jh7110_clk_vout_of_match,
	.probe = jh7110_clk_vout_probe,
	.ops = &starfive_clk_ops,
	.priv_auto = sizeof(struct jh7110_clk_priv),
};

