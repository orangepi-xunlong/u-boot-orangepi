// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2017 Álvaro Fernández Rojas <noltari@gmail.com>
 */
#include <asm/io.h>
#include <common.h>
#include <dm.h>
#include <linux/bitops.h>
#include <linux/iopoll.h>
#include <malloc.h>
#include <power-domain-uclass.h>

#define usleep_range(a, b) udelay((b))

#define MAX_DOMAINS	64
#define TIMEOUT_US		100000

/* register define */
#define HW_EVENT_TURN_ON_MASK		0x04
#define HW_EVENT_TURN_OFF_MASK		0x08
#define SW_TURN_ON_POWER_MODE		0x0C
#define SW_TURN_OFF_POWER_MODE		0x10
#define SW_ENCOURAGE			0x44
#define PMU_INT_MASK			0x48
#define PCH_BYPASS			0x4C
#define PCH_PSTATE			0x50
#define PCH_TIMEOUT			0x54
#define LP_TIMEOUT			0x58
#define HW_TURN_ON_MODE			0x5C
#define CURR_POWER_MODE			0x80
#define PMU_EVENT_STATUS		0x88
#define PMU_INT_STATUS			0x8C

/* sw encourage cfg */
#define SW_MODE_ENCOURAGE_EN_LO		0x05
#define SW_MODE_ENCOURAGE_EN_HI		0x50
#define SW_MODE_ENCOURAGE_DIS_LO	0x0A
#define SW_MODE_ENCOURAGE_DIS_HI	0xA0
#define SW_MODE_ENCOURAGE_ON		0xFF

struct sf_power_domain {
	void __iomem *regs;
};

static int sf_power_domain_request(struct power_domain *power_domain)
{
	if (power_domain->id >= MAX_DOMAINS)
		return -EINVAL;

	return 0;
}

static int sf_power_domain_free(struct power_domain *power_domain)
{
	return 0;
}

static int sf_power_domain_on(struct power_domain *power_domain)
{
	struct sf_power_domain *priv = dev_get_priv(power_domain->dev);
	uint32_t mode;
	uint32_t val;
	uint32_t encourage_lo;
	uint32_t encourage_hi;
	int ret;
	mode = SW_TURN_ON_POWER_MODE;
	encourage_lo = SW_MODE_ENCOURAGE_EN_LO;
	encourage_hi = SW_MODE_ENCOURAGE_EN_HI;

	/* write SW_ENCOURAGE to make the configuration take effect */
	__raw_writel(BIT(power_domain->id), priv->regs + mode);
	__raw_writel(SW_MODE_ENCOURAGE_ON, priv->regs + SW_ENCOURAGE);
	__raw_writel(encourage_lo, priv->regs + SW_ENCOURAGE);
	__raw_writel(encourage_hi, priv->regs + SW_ENCOURAGE);

	ret = readl_poll_timeout(priv->regs + CURR_POWER_MODE, val,
					val & BIT(power_domain->id),
					TIMEOUT_US);
	if (ret) {
		pr_err("power_on failed");
		return -ETIMEDOUT;
	}


	return 0;
}

static int sf_power_domain_off(struct power_domain *power_domain)
{
	struct sf_power_domain *priv = dev_get_priv(power_domain->dev);
	uint32_t mode;
	uint32_t val;
	uint32_t encourage_lo;
	uint32_t encourage_hi;
	int ret;
	mode = SW_TURN_OFF_POWER_MODE;
	encourage_lo = SW_MODE_ENCOURAGE_DIS_LO;
	encourage_hi = SW_MODE_ENCOURAGE_DIS_HI;

	__raw_writel(BIT(power_domain->id), priv->regs + mode);
	__raw_writel(SW_MODE_ENCOURAGE_ON, priv->regs + SW_ENCOURAGE);
	__raw_writel(encourage_lo, priv->regs + SW_ENCOURAGE);
	__raw_writel(encourage_hi, priv->regs + SW_ENCOURAGE);

	ret = readl_poll_timeout(priv->regs + CURR_POWER_MODE, val,
					!(val & BIT(power_domain->id)),
					TIMEOUT_US);
	if (ret) {
		pr_err("power_off failed\n");
		return -ETIMEDOUT;
	}


	return 0;
}


static int sf_power_domain_probe(struct udevice *dev)
{
	struct sf_power_domain *priv = dev_get_priv(dev);

	fdt_addr_t addr;
	addr = dev_read_addr_index(dev, 0);
	priv->regs = (void __iomem *)addr;

	if (!priv->regs)
		return -EINVAL;

	return 0;
}

static const struct udevice_id sf_power_domain_ids[] = {
	{ .compatible = "starfive,jh7110-pmu" },
	{ /* sentinel */ }
};

struct power_domain_ops sf_power_domain_ops = {
	.rfree = sf_power_domain_free,
	.off = sf_power_domain_off,
	.on = sf_power_domain_on,
	.request = sf_power_domain_request,
};

U_BOOT_DRIVER(sf_power_domain) = {
	.name = "sf_power_domain",
	.id = UCLASS_POWER_DOMAIN,
	.of_match = sf_power_domain_ids,
	.ops = &sf_power_domain_ops,
	.priv_auto	= sizeof(struct sf_power_domain),
	.probe = sf_power_domain_probe,
};
