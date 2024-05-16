// SPDX-License-Identifier: GPL-2.0
/**
 * cdns-starfive.c - Cadence USB Controller
 *
 * Copyright (C) 2022-2023 StarFive Technology Co., Ltd.
 * Author:	yanhong <yanhong.wang@starfivetech.com>
 */

#include <common.h>
#include <clk.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <linux/usb/otg.h>
#include <reset.h>

#include "core.h"

struct cdns_starfive {
	struct udevice *dev;
	struct clk_bulk clks;
	struct reset_ctl_bulk resets;
};

static int cdns_starfive_probe(struct udevice *dev)
{
	struct cdns_starfive *data = dev_get_plat(dev);
	int ret;

	data->dev = dev;

	ret = clk_get_bulk(dev, &data->clks);
	if (ret)
		goto err;

	ret = reset_get_bulk(dev, &data->resets);
	if (ret)
		goto err_clk;

	ret = clk_enable_bulk(&data->clks);
	if (ret)
		goto err_reset;

	ret = reset_deassert_bulk(&data->resets);
	if (ret)
		goto err_reset;

	return 0;
err_reset:
	reset_release_bulk(&data->resets);
err_clk:
	clk_release_bulk(&data->clks);
err:
	return ret;
}

static int cdns_starfive_remove(struct udevice *dev)
{
	struct cdns_starfive *data = dev_get_plat(dev);

	clk_release_bulk(&data->clks);
//	reset_assert_bulk(&data->resets);

	return 0;
}

static const struct udevice_id cdns_starfive_of_match[] = {
	{ .compatible = "starfive,jh7110-cdns3", },
	{},
};

U_BOOT_DRIVER(cdns_starfive) = {
	.name = "cdns-starfive",
	.id = UCLASS_NOP,
	.of_match = cdns_starfive_of_match,
	.bind = cdns3_bind,
	.probe = cdns_starfive_probe,
	.remove = cdns_starfive_remove,
	.plat_auto	= sizeof(struct cdns_starfive),
};
