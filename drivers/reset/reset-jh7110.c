// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022-2023 StarFive Technology Co., Ltd.
 * Author:	samin <samin.guo@starfivetech.com>
 *		yanhong <yanhong.wang@starfivetech.com>
 *
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <log.h>
#include <malloc.h>
#include <reset-uclass.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/iopoll.h>
#include <dt-bindings/reset/starfive-jh7110.h>

#define AONCRG_RESET_ASSERT	0x38
#define AONCRG_RESET_STATUS	0x3C
#define ISPCRG_RESET_ASSERT	0x38
#define ISPCRG_RESET_STATUS	0x3C
#define VOUTCRG_RESET_ASSERT	0x48
#define VOUTCRG_RESET_STATUS	0x4C
#define STGCRG_RESET_ASSERT	0x74
#define STGCRG_RESET_STATUS	0x78
#define SYSCRG_RESET_ASSERT0	0x2F8
#define SYSCRG_RESET_ASSERT1	0x2FC
#define SYSCRG_RESET_ASSERT2	0x300
#define SYSCRG_RESET_ASSERT3	0x304
#define SYSCRG_RESET_STATUS0	0x308
#define SYSCRG_RESET_STATUS1	0x30C
#define SYSCRG_RESET_STATUS2	0x310
#define SYSCRG_RESET_STATUS3	0x314

struct reset_assert_t {
	void __iomem *assert;
	void __iomem *status;
};

enum JH7110_RESET_CRG_GROUP {
	SYSCRG_0 = 0,
	SYSCRG_1,
	SYSCRG_2,
	SYSCRG_3,
	STGCRG,
	AONCRG,
	ISPCRG,
	VOUTCRG,
};

struct jh7110_reset_priv {
	void __iomem *syscrg;
	void __iomem *stgcrg;
	void __iomem *aoncrg;
	void __iomem *ispcrg;
	void __iomem *voutcrg;
};

static int jh7110_get_reset(struct jh7110_reset_priv *priv,
			struct reset_assert_t *reset,
			unsigned long group)
{
	switch (group) {
	case SYSCRG_0:
		reset->assert = priv->syscrg + SYSCRG_RESET_ASSERT0;
		reset->status = priv->syscrg + SYSCRG_RESET_STATUS0;
		break;
	case SYSCRG_1:
		reset->assert = priv->syscrg + SYSCRG_RESET_ASSERT1;
		reset->status = priv->syscrg + SYSCRG_RESET_STATUS1;
		break;
	case SYSCRG_2:
		reset->assert = priv->syscrg + SYSCRG_RESET_ASSERT2;
		reset->status = priv->syscrg + SYSCRG_RESET_STATUS2;
		break;
	case SYSCRG_3:
		reset->assert = priv->syscrg + SYSCRG_RESET_ASSERT3;
		reset->status = priv->syscrg + SYSCRG_RESET_STATUS3;
		break;
	case STGCRG:
		reset->assert = priv->stgcrg + STGCRG_RESET_ASSERT;
		reset->status = priv->stgcrg + STGCRG_RESET_STATUS;
		break;
	case AONCRG:
		reset->assert = priv->aoncrg + AONCRG_RESET_ASSERT;
		reset->status = priv->aoncrg + AONCRG_RESET_STATUS;
		break;

	case ISPCRG:
		reset->assert = priv->ispcrg + ISPCRG_RESET_ASSERT;
		reset->status = priv->ispcrg + ISPCRG_RESET_STATUS;
		break;
	case VOUTCRG:
		reset->assert = priv->voutcrg + VOUTCRG_RESET_ASSERT;
		reset->status = priv->voutcrg + VOUTCRG_RESET_STATUS;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int jh7110_reset_trigger(struct jh7110_reset_priv *priv,
				unsigned long id, bool assert)
{
	struct reset_assert_t reset;
	unsigned long group;
	u32 mask, value, done = 0;
	int ret;
	u32 loop;

	group = id / 32;
	mask = BIT(id % 32);
	ret = jh7110_get_reset(priv, &reset, group);
	if (ret) {
		debug("reset: bad reset id.\n");
		return ret;
	}

	if (!assert)
		done ^= mask;

	value = readl(reset.assert);
	if (assert)
		value |= mask;
	else
		value &= ~mask;
	writel(value, reset.assert);

	loop = 10000; /*Addd loop condition inorder to avoid hang here*/
	do{
		value = in_le32(reset.status);
	}while((value & mask) != done && --loop != 0);

	return ret;
}

static int jh7110_reset_assert(struct reset_ctl *rst)
{
	struct jh7110_reset_priv *priv = dev_get_priv(rst->dev);

	jh7110_reset_trigger(priv, rst->id, true);

	return 0;
}

static int jh7110_reset_deassert(struct reset_ctl *rst)
{
	struct jh7110_reset_priv *priv = dev_get_priv(rst->dev);

	jh7110_reset_trigger(priv, rst->id, false);

	return 0;
}

static int jh7110_reset_free(struct reset_ctl *rst)
{
	return 0;
}

static int jh7110_reset_request(struct reset_ctl *rst)
{
	if (rst->id >= RSTN_JH7110_RESET_END)
		return -EINVAL;

	return 0;
}

struct reset_ops jh7110_reset_reset_ops = {
	.rfree = jh7110_reset_free,
	.request = jh7110_reset_request,
	.rst_assert = jh7110_reset_assert,
	.rst_deassert = jh7110_reset_deassert,
};

static const struct udevice_id jh7110_reset_ids[] = {
	{ .compatible = "starfive,jh7110-reset" },
	{ /* sentinel */ }
};

static int jh7110_reset_probe(struct udevice *dev)
{
	struct jh7110_reset_priv *priv = dev_get_priv(dev);

	priv->syscrg = dev_remap_addr_name(dev, "syscrg");
	if (!priv->syscrg)
		return -EINVAL;

	priv->stgcrg = dev_remap_addr_name(dev, "stgcrg");
	if (!priv->stgcrg)
		return -EINVAL;

	priv->aoncrg = dev_remap_addr_name(dev, "aoncrg");
	if (!priv->aoncrg)
		return -EINVAL;

	priv->ispcrg = dev_remap_addr_name(dev, "ispcrg");
	if (!priv->ispcrg)
		return -EINVAL;

	priv->voutcrg = dev_remap_addr_name(dev, "voutcrg");
	if (!priv->voutcrg)
		return -EINVAL;

	return 0;
}

U_BOOT_DRIVER(jh7110_reset) = {
	.name = "jh7110-reset",
	.id = UCLASS_RESET,
	.of_match = jh7110_reset_ids,
	.ops = &jh7110_reset_reset_ops,
	.probe = jh7110_reset_probe,
	.priv_auto = sizeof(struct jh7110_reset_priv),
};
