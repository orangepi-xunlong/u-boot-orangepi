// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022-2023 StarFive Technology Co., Ltd.
 * Author:	yanhong <yanhong.wang@starfivetech.com>
 *
 */

#include <common.h>
#include <asm/arch/clk.h>
#include <dm.h>
#include <fdtdec.h>
#include <init.h>
#include <ram.h>
#include <reset.h>
#include <syscon.h>
#include <asm/io.h>
#include <clk.h>
#include <wait_bit.h>
#include <linux/bitops.h>

#include "starfive_ddr.h"

DECLARE_GLOBAL_DATA_PTR;

struct starfive_ddr_priv {
	struct udevice	*dev;
	struct ram_info info;
	void __iomem	*ctrlreg;
	void __iomem	*phyreg;
	struct reset_ctl rst_axi;
	struct reset_ctl rst_osc;
	struct reset_ctl rst_apb;
	u32	fre;
};

enum ddr_type_t starfive_ddr_type;

__weak int starfive_get_ddr_type(void)
{
	return -EINVAL;
}

static int starfive_ddr_setup(struct udevice *dev, struct starfive_ddr_priv *priv)
{
	enum ddr_size_t size;
	int ret;

	switch (priv->info.size) {
	case 0x40000000:
		size = DDR_SIZE_1G;
		break;
	case 0x80000000:
		size = DDR_SIZE_2G;
		break;
	case 0x100000000:
		size = DDR_SIZE_4G;
		break;
	case 0x200000000:
		size = DDR_SIZE_8G;
		break;
	case 0x400000000:
	default:
		pr_err("unsupport size %lx\n", priv->info.size);
		return -1;
	}

	starfive_ddr_type = DDR_TYPE_LPDDR4;	/*set default ddr type */
	ret = starfive_get_ddr_type();
	if (ret >= 0) {
		switch (ret) {
		case 0x0:
			starfive_ddr_type = DDR_TYPE_LPDDR4;
			break;
		case 0x1:
			starfive_ddr_type = DDR_TYPE_DDR4;
			break;
		case 0x2:
			starfive_ddr_type = DDR_TYPE_LPDDR3;
			break;
		case 0x3:
			starfive_ddr_type = DDR_TYPE_DDR3;
			break;
		default:
			pr_err("unsupport ddr type %d\n", ret);
			return -EINVAL;
		}
	}

	ddr_phy_train(priv->phyreg + (PHY_BASE_ADDR << 2));
	ddr_phy_util(priv->phyreg + (PHY_AC_BASE_ADDR << 2));
	ddr_phy_start(priv->phyreg, size);

	if (starfive_ddr_type == DDR_TYPE_LPDDR4)
		DDR_REG_SET(BUS, DDR_BUS_OSC_DIV2);

	ddrcsr_boot(priv->ctrlreg, priv->ctrlreg + SEC_CTRL_ADDR,
		   priv->phyreg, size);

	return 0;
}

static int starfive_ddr_probe(struct udevice *dev)
{
	struct starfive_ddr_priv *priv = dev_get_priv(dev);
	fdt_addr_t addr;
	u64 rate;
	int ret;

	priv->dev = dev;
	addr = dev_read_addr_index(dev, 0);
	priv->ctrlreg = (void __iomem *)addr;
	addr = dev_read_addr_index(dev, 1);
	priv->phyreg = (void __iomem *)addr;
	ret = dev_read_u32(dev, "clock-frequency", &priv->fre);
	if (ret)
		goto init_end;

	ret = reset_get_by_name(dev, "axi", &priv->rst_axi);
	if (ret)
		goto init_end;

	ret = reset_get_by_name(dev, "osc", &priv->rst_osc);
	if (ret)
		goto err_axi;

	ret = reset_get_by_name(dev, "apb", &priv->rst_apb);
	if (ret)
		goto err_osc;

	priv->info.base = gd->ram_base;
	priv->info.size = gd->ram_size;

	switch (priv->fre) {
	case 2133:
		rate = 1066000000;
		break;

	case 2800:
		rate = 1400000000;
		break;
	default:
		printk("Unknown DDR frequency %d\n", priv->fre);
		ret = -1;
		goto init_end;
	};

	DDR_REG_SET(BUS, DDR_BUS_OSC_DIV2);
	starfive_jh7110_pll_set_rate(PLL1, rate);
	udelay(100);
	DDR_REG_SET(BUS, DDR_BUS_PLL1_DIV2);
	reset_assert(&priv->rst_osc);
	reset_deassert(&priv->rst_osc);
	reset_assert(&priv->rst_apb);
	reset_deassert(&priv->rst_apb);
	reset_assert(&priv->rst_axi);
	reset_deassert(&priv->rst_axi);

	ret = starfive_ddr_setup(dev, priv);
	printf("%sDDR4: %ldG version: g8ad50857.\n",
		starfive_ddr_type == DDR_TYPE_LPDDR4 ? "LP":"",
		priv->info.size/1024/1024/1024);
	goto init_end;

err_osc:
	reset_free(&priv->rst_osc);
err_axi:
	reset_free(&priv->rst_axi);
	pr_err("reset_get_by_name(axi) failed: %d", ret);
init_end:

	return ret;
}

static int starfive_ddr_get_info(struct udevice *dev, struct ram_info *info)
{
	struct starfive_ddr_priv *priv = dev_get_priv(dev);

	*info = priv->info;

	return 0;
}

static struct ram_ops starfive_ddr_ops = {
	.get_info = starfive_ddr_get_info,
};

static const struct udevice_id starfive_ddr_ids[] = {
	{ .compatible = "starfive,jh7110-dmc" },
	{ }
};

U_BOOT_DRIVER(starfive_ddr) = {
	.name = "starfive_ddr",
	.id = UCLASS_RAM,
	.of_match = starfive_ddr_ids,
	.ops = &starfive_ddr_ops,
	.probe = starfive_ddr_probe,
	.priv_auto = sizeof(struct starfive_ddr_priv),
};
