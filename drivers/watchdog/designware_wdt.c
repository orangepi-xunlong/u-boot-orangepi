// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2013 Altera Corporation <www.altera.com>
 */

#include <clk.h>
#include <common.h>
#include <dm.h>
#include <reset.h>
#include <wdt.h>
#include <asm/io.h>
#include <linux/io.h>
#include <linux/bitops.h>
#include "../../board/cvitek/cv186x/cv186x_reg.h"

#define DW_WDT_CR	0x00
#define DW_WDT_TORR	0x04
#define DW_WDT_CCVR	0x08
#define DW_WDT_CRR	0x0C
#define DW_WDT_TOC	0x1C

#define DW_WDT_CR_EN_OFFSET	0x00
#define DW_WDT_CR_RMOD_OFFSET	0x01
#define DW_WDT_CRR_RESTART_VAL	0x76

#define DW_WDT_MAX_TOP		15

struct designware_wdt_priv {
	void __iomem	*base;
	unsigned int	clk_khz;
	struct reset_ctl_bulk resets;
};

static inline int dw_wdt_top_in_ms(unsigned int clk_khz, unsigned int top)
{
	/*
	* There are 16 possible timeout values in 0..15 where the number of
	* cycles is 2 ^ (16 + i) and the watchdog counts down.
	*/
	return (1U << (16 + top)) / clk_khz;
}

static inline int dw_wdt_top_xlate_toc(unsigned int clk_khz, unsigned int top, unsigned int top_val)
{
	// T = WDT_TOC <<( WDT_TORR +1) when TOR_MODE = 1
	return ((top * clk_khz) >> (top_val + 1)) + 1;// approximate value
}

/*
 * Set the watchdog time interval.
 * Counter is 16 bit.
 */
static int designware_wdt_settimeout(void __iomem *base, unsigned int clk_khz,
				     unsigned int timeout)
{
	int i, top_val = DW_WDT_MAX_TOP;
	unsigned int toc;

	/* calculate the timeout range value */
	for (i = 0; i <= DW_WDT_MAX_TOP; ++i)
		if (dw_wdt_top_in_ms(clk_khz, i) >= timeout) {
			top_val = i - 1;
			break;
		}

	toc = dw_wdt_top_xlate_toc(clk_khz, timeout, top_val);

	writel(top_val | (top_val << 4), base + DW_WDT_TORR);
	writel(toc, base + DW_WDT_TOC);

	return 0;
}

static int designware_wdt_gettimeout(void __iomem *base, unsigned int clk_khz)
{
	unsigned int toc, torr;

	torr = readl(base + DW_WDT_TORR) & 0xF;
	toc = readl(base + DW_WDT_TOC) & 0xFFFF;

	return (toc << (torr + 1)) / clk_khz;
}

static void designware_wdt_enable(void __iomem *base)
{
	writel(BIT(DW_WDT_CR_EN_OFFSET) | BIT(6) | BIT(7), base + DW_WDT_CR);
}

static inline unsigned int designware_wdt_is_enabled(void __iomem *base)
{
	return readl(base + DW_WDT_CR) & BIT(0);
}

static void designware_wdt_reset_common(void __iomem *base)
{
	if (designware_wdt_is_enabled(base))
		/* restart the watchdog counter */
		writel(DW_WDT_CRR_RESTART_VAL, base + DW_WDT_CRR);
}

#if !CONFIG_IS_ENABLED(WDT)
void hw_watchdog_reset(void)
{
	designware_wdt_reset_common((void __iomem *)CONFIG_DW_WDT_BASE);
}

void hw_watchdog_init(void)
{
	/* reset to disable the watchdog */
	hw_watchdog_reset();
	/* set timer in miliseconds */
	designware_wdt_settimeout((void __iomem *)CONFIG_DW_WDT_BASE,
				  CONFIG_DW_WDT_CLOCK_KHZ,
				  CONFIG_WATCHDOG_TIMEOUT_MSECS);
	/* enable the watchdog */
	designware_wdt_enable((void __iomem *)CONFIG_DW_WDT_BASE);
	/* reset the watchdog */
	hw_watchdog_reset();
}
#else
static int designware_wdt_reset(struct udevice *dev)
{
	struct designware_wdt_priv *priv = dev_get_priv(dev);

	designware_wdt_reset_common(priv->base);

	return 0;
}

static int designware_wdt_stop(struct udevice *dev)
{
	void __iomem    *reg;

	designware_wdt_reset(dev);

	reg = ioremap(REG_SOFT_RST, PAGE_SIZE);
	if (!reg)
		return -EINVAL;

	writel(0xFEFFFFFF, reg + 0x8);
	writel(0xFFFFFFFF, reg + 0x8);
	iounmap(reg);

	return 0;
}

static int designware_wdt_start(struct udevice *dev, u64 timeout, ulong flags)
{
	struct designware_wdt_priv *priv = dev_get_priv(dev);

	if (designware_wdt_is_enabled(priv->base))
		return 0;

	/* set timer in miliseconds */
	designware_wdt_settimeout(priv->base, priv->clk_khz, timeout);

	designware_wdt_enable(priv->base);

	/* reset the watchdog */
	return designware_wdt_reset(dev);
}

static int designware_wdt_is_started(struct udevice *dev)
{
	struct designware_wdt_priv *priv = dev_get_priv(dev);

	return designware_wdt_is_enabled(priv->base);
}

static int designware_wdt_get_timeout(struct udevice *dev)
{
	struct designware_wdt_priv *priv = dev_get_priv(dev);

	return designware_wdt_gettimeout(priv->base, priv->clk_khz);
}

static int designware_wdt_expire_now(struct udevice *dev, ulong flags)
{
	struct designware_wdt_priv *priv = dev_get_priv(dev);

	if (designware_wdt_is_enabled(priv->base)) {
		printf("expire: %d (ms)\n", readl(priv->base + DW_WDT_CCVR)/priv->clk_khz);
	} else {
		printf("expire : -1\n");
	}

	return 0;
}

#if 0 // UNNEED
static inline int enable_wdt_reset_system(void)
{
	void __iomem    *reg;
	unsigned int val;

	/* set ((TOP_BASE + 8), 0x4); */
	reg = ioremap(TOP_BASE, PAGE_SIZE);
	if (!reg)
		return -EINVAL;

	val = readl((reg + CV_TOP_WDT_OFFSET));
	val |= CV_TOP_WDT_VAL;
	writel(val, (reg + CV_TOP_WDT_OFFSET));

	val = readl((reg + 0x1a8));
	val |= 0x7;
	writel(val, (reg + 0x1a8));
	iounmap(reg);

	return 0;
}
#endif

static int designware_wdt_probe(struct udevice *dev)
{
	struct designware_wdt_priv *priv = dev_get_priv(dev);
	__maybe_unused int ret;

	priv->base = dev_remap_addr(dev);
	if (!priv->base)
		return -EINVAL;

#if CONFIG_IS_ENABLED(CLK)
	struct clk clk;

	ret = clk_get_by_index(dev, 0, &clk);
	if (ret)
		return ret;

	ret = clk_enable(&clk);
	if (ret)
		goto err;

	priv->clk_khz = clk_get_rate(&clk) / 1000;
	if (!priv->clk_khz) {
		ret = -EINVAL;
		goto err;
	}
#else
	priv->clk_khz = CONFIG_DW_WDT_CLOCK_KHZ;
#endif

	return 0;

#if CONFIG_IS_ENABLED(CLK)
err:
	clk_free(&clk);
#endif
	return ret;
}

static const struct wdt_ops designware_wdt_ops = {
	.start = designware_wdt_start,
	.reset = designware_wdt_reset,
	.stop = designware_wdt_stop,
	.expire_now = designware_wdt_expire_now,
	.is_started = designware_wdt_is_started,
	.get_timeout = designware_wdt_get_timeout,
};

static const struct udevice_id designware_wdt_ids[] = {
	{ .compatible = "snps,dw-wdt"},
	{}
};

U_BOOT_DRIVER(designware_wdt) = {
	.name = "designware_wdt",
	.id = UCLASS_WDT,
	.of_match = designware_wdt_ids,
	.priv_auto	= sizeof(struct designware_wdt_priv),
	.probe = designware_wdt_probe,
	.ops = &designware_wdt_ops,
	.flags = DM_FLAG_PRE_RELOC,
};
#endif
