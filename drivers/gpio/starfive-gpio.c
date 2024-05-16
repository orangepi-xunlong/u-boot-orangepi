// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022-2023 StarFive Technology Co., Ltd.
 * Author:	yanhong <yanhong.wang@starfivetech.com>
 *
 */

#include <common.h>
#include <dm.h>
#include <asm/arch/gpio.h>
#include <asm/io.h>
#include <errno.h>
#include <asm/gpio.h>
#include <linux/bitops.h>

static int starfive_gpio_probe(struct udevice *dev)
{
	struct starfive_gpio_platdata *plat = dev_get_plat(dev);
	struct gpio_dev_priv *uc_priv = dev_get_uclass_priv(dev);
	char name[18], *str;

	sprintf(name, "gpio@%4lx_", (uintptr_t)plat->base);
	str = strdup(name);
	if (!str)
		return -ENOMEM;
	uc_priv->bank_name = str;

	/*
	 * Use the gpio count mentioned in device tree,
	 * if not specified in dt, set NR_GPIOS as default
	 */
	uc_priv->gpio_count = dev_read_u32_default(dev, "ngpios", NR_GPIOS);

	return 0;
}

static int starfive_gpio_direction_input(struct udevice *dev, u32 offset)
{
	struct starfive_gpio_platdata *plat = dev_get_plat(dev);
	struct gpio_dev_priv *uc_priv = dev_get_uclass_priv(dev);

	if (offset > uc_priv->gpio_count)
		return -EINVAL;

	/* Configure gpio direction as input */
	clrsetbits_le32(plat->base + GPIO_OFFSET(offset),
		GPIO_DOEN_MASK << GPIO_SHIFT(offset),
		HIGH << GPIO_SHIFT(offset));

	return 0;
}

static int starfive_gpio_direction_output(struct udevice *dev, u32 offset,
					int value)
{
	struct starfive_gpio_platdata *plat = dev_get_plat(dev);
	struct gpio_dev_priv *uc_priv = dev_get_uclass_priv(dev);

	if (offset > uc_priv->gpio_count)
		return -EINVAL;

	/* Configure gpio direction as output */
	clrsetbits_le32(plat->base + GPIO_OFFSET(offset),
		GPIO_DOEN_MASK <<  GPIO_SHIFT(offset),
		LOW << GPIO_SHIFT(offset));

	/* Set the output value of the pin */
	clrsetbits_le32(plat->base + GPIO_DOUT + GPIO_OFFSET(offset),
		GPIO_DOUT_MASK << GPIO_SHIFT(offset),
		(value & GPIO_DOUT_MASK) << GPIO_SHIFT(offset));

	return 0;
}

static int starfive_gpio_get_value(struct udevice *dev, u32 offset)
{
	struct starfive_gpio_platdata *plat = dev_get_plat(dev);
	struct gpio_dev_priv *uc_priv = dev_get_uclass_priv(dev);
	int val;

	if (offset > uc_priv->gpio_count)
		return -EINVAL;

	/*Get dout value*/
	val = readl(plat->base + GPIO_DIN + GPIO_OFFSET(offset));
	val &= GPIO_DIN_MASK << GPIO_SHIFT(offset);

	return val ? HIGH : LOW;
}

static int starfive_gpio_set_value(struct udevice *dev, u32 offset, int value)
{
	struct starfive_gpio_platdata *plat = dev_get_plat(dev);
	struct gpio_dev_priv *uc_priv = dev_get_uclass_priv(dev);

	if (offset > uc_priv->gpio_count)
		return -EINVAL;

	clrsetbits_le32(plat->base + GPIO_DOUT + GPIO_OFFSET(offset),
		GPIO_DOUT_MASK << GPIO_SHIFT(offset),
		(value & GPIO_DOUT_MASK) << GPIO_SHIFT(offset));

	return 0;
}

static int starfive_gpio_get_function(struct udevice *dev, unsigned int offset)
{
	struct starfive_gpio_platdata *plat = dev_get_plat(dev);
	struct gpio_dev_priv *uc_priv = dev_get_uclass_priv(dev);
	u32	dir, val;

	if (offset > uc_priv->gpio_count)
		return -1;

	/*Get doen value*/
	val = readl(plat->base + GPIO_OFFSET(offset));
	val &= (GPIO_DOEN_MASK << GPIO_SHIFT(offset));

	dir = (val > 1) ? GPIOF_UNUSED : (val ? GPIOF_INPUT : GPIOF_OUTPUT);

	return dir;
}

static int starfive_gpio_ofdata_to_platdata(struct udevice *dev)
{
	struct starfive_gpio_platdata *plat = dev_get_plat(dev);
	fdt_addr_t addr;

	addr = dev_read_addr(dev);
	if (addr == FDT_ADDR_T_NONE)
		return -EINVAL;

	plat->base = (void *)addr;

	writel(0, plat->base + GPIO_EN);
	writel(0, plat->base + GPIO_LOW_IE);
	writel(0, plat->base + GPIO_HIGH_IE);
	writel(1, plat->base + GPIO_EN);

	return 0;
}

static const struct udevice_id starfive_gpio_match[] = {
	{ .compatible = "starfive,jh7110-gpio" },
	{ }
};

static const struct dm_gpio_ops starfive_gpio_ops = {
	.direction_input        = starfive_gpio_direction_input,
	.direction_output       = starfive_gpio_direction_output,
	.get_value              = starfive_gpio_get_value,
	.set_value              = starfive_gpio_set_value,
	.get_function		= starfive_gpio_get_function,
};

U_BOOT_DRIVER(gpio_starfive) = {
	.name	= "gpio_starfive",
	.id	= UCLASS_GPIO,
	.of_match = starfive_gpio_match,
	.of_to_plat = starfive_gpio_ofdata_to_platdata,
	.plat_auto = sizeof(struct starfive_gpio_platdata),
	.ops	= &starfive_gpio_ops,
	.probe	= starfive_gpio_probe,
};
