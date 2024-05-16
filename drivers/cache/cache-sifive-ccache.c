// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 SiFive
 */

#include <common.h>
#include <cache.h>
#include <dm.h>
#include <asm/io.h>
#include <dm/device.h>
#include <linux/bitfield.h>

#define SIFIVE_CCACHE_CONFIG		0x000
#define SIFIVE_CCACHE_CONFIG_WAYS	GENMASK(15, 8)

#define SIFIVE_CCACHE_WAY_ENABLE	0x008


/* Prefetch */
#define SIFIVE_PREFET_HARD_BASE(hart) 		((hart)*0x2000)
/* Prefetch Control Register */
#define SIFIVE_PREFT_EN_MASK			BIT(0)
#define SIFIVE_PREFT_CROSS_PAGE_DIS_MASK	BIT(1)
#define SIFIVE_PREFT_DIST_MASK			GENMASK(7, 2)
#define SIFIVE_PREFT_MAX_ALLOC_DIST_MASK	GENMASK(13, 8)
#define SIFIVE_PREFT_LIN_TO_EXP_THRD_MASK	GENMASK(19, 14)
#define SIFIVE_PREFT_AGE_OUT_EN_MASK		BIT(20)
#define SIFIVE_PREFT_NUM_LDS_AGE_OUT_MASK	GENMASK(27, 21)
#define SIFIVE_PREFT_CROSS_PAGE_EN_MASK		BIT(28)

/* Prefetch Advanced Control Register */
#define SIFIVE_PREFT_ADV_Q_FULL_THRD		GENMASK(3, 0)
#define SIFIVE_PREFT_ADV_HIT_CACHE_THRD		GENMASK(8, 4)
#define SIFIVE_PREFT_ADV_HIT_MSHR_THRD		GENMASK(12, 9)
#define SIFIVE_PREFT_ADV_WINDOW_MASK		GENMASK(18, 13)

#define SIFIVE_PREFET_HARD_MASK		0x1e
#define SIFIVE_MAX_HART_ID		0x20
#define SIFIVE_PREFT_DIST_VAL		0x3
#define SIFIVE_PREFT_DIST_MAX		0x3f
#define SIFIVE_PREFT_EN			0x1

struct sifive_ccache {
	void __iomem *base;
	void __iomem *pre_base;
	u32 pre_hart_mask;
	u32 pre_dist_size;
};

static int sifive_prefetcher_parse(struct udevice *dev)
{
	struct sifive_ccache *priv = dev_get_priv(dev);

	if (!priv->pre_base)
		return -EINVAL;

	if (!dev_read_bool(dev, "prefetch-enable"))
		return -ENOENT;

	priv->pre_hart_mask = dev_read_u32_default(dev, "prefetch-hart-mask",
						   SIFIVE_PREFET_HARD_MASK);
	priv->pre_dist_size = dev_read_u32_default(dev, "prefetch-dist-size",
						   SIFIVE_PREFT_DIST_VAL);
	return  0;
}

static void sifive_prefetcher_cfg_by_id(struct udevice *dev, u32 hart)
{
	struct sifive_ccache *priv = dev_get_priv(dev);
	void __iomem *reg;
	u32 val;

	/* Prefetch Control Register */
	reg = priv->pre_base + SIFIVE_PREFET_HARD_BASE(hart);

	val = readl(reg);
	val &= ~SIFIVE_PREFT_MAX_ALLOC_DIST_MASK;
	val |= SIFIVE_PREFT_DIST_MAX << __ffs(SIFIVE_PREFT_MAX_ALLOC_DIST_MASK);
	writel(val, reg);

	val = readl(reg);
	val &= ~SIFIVE_PREFT_DIST_MASK;
	val |= priv->pre_dist_size << __ffs(SIFIVE_PREFT_DIST_MASK);
	writel(val, reg);

	val |= SIFIVE_PREFT_EN << __ffs(SIFIVE_PREFT_EN_MASK);
	writel(val, reg);
}

static int sifive_prefetcher_enable(struct udevice *dev)
{
	struct sifive_ccache *priv = dev_get_priv(dev);
	u32 hart;
	int ret;

	ret = sifive_prefetcher_parse(dev);
	if (ret)
		return ret;

	for (hart = 0; hart < SIFIVE_MAX_HART_ID; hart++) {
		if (BIT(hart) & priv->pre_hart_mask)
			sifive_prefetcher_cfg_by_id(dev, hart);
	}

	return 0;
}

static int sifive_ccache_enable(struct udevice *dev)
{
	struct sifive_ccache *priv = dev_get_priv(dev);
	u32 config;
	u32 ways;

	/* Enable all ways of composable cache */
	config = readl(priv->base + SIFIVE_CCACHE_CONFIG);
	ways = FIELD_GET(SIFIVE_CCACHE_CONFIG_WAYS, config);

	writel(ways - 1, priv->base + SIFIVE_CCACHE_WAY_ENABLE);

	sifive_prefetcher_enable(dev);

	return 0;
}

static int sifive_ccache_get_info(struct udevice *dev, struct cache_info *info)
{
	struct sifive_ccache *priv = dev_get_priv(dev);

	info->base = (phys_addr_t)priv->base;

	return 0;
}

static const struct cache_ops sifive_ccache_ops = {
	.enable = sifive_ccache_enable,
	.get_info = sifive_ccache_get_info,
};

static int sifive_ccache_probe(struct udevice *dev)
{
	struct sifive_ccache *priv = dev_get_priv(dev);
	fdt_addr_t addr;

	priv->base = dev_read_addr_ptr(dev);
	if (!priv->base)
		return -EINVAL;

	addr =  dev_read_addr_name(dev, "prefetcher");
	if (addr != FDT_ADDR_T_NONE)
		priv->pre_base = (void *)(uintptr_t)addr;

	return 0;
}

static const struct udevice_id sifive_ccache_ids[] = {
	{ .compatible = "sifive,fu540-c000-ccache" },
	{ .compatible = "sifive,fu740-c000-ccache" },
	{}
};

U_BOOT_DRIVER(sifive_ccache) = {
	.name = "sifive_ccache",
	.id = UCLASS_CACHE,
	.of_match = sifive_ccache_ids,
	.probe = sifive_ccache_probe,
	.priv_auto = sizeof(struct sifive_ccache),
	.ops = &sifive_ccache_ops,
};
