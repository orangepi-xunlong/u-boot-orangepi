// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2023 Starfive, Inc.
 * Author:	keith.zhao<keith.zhao@statfivetech.com>
 */

#include <common.h>
#include <fdtdec.h>
#include <errno.h>
#include <dm.h>
#include <i2c.h>
#include <log.h>
#include <power/pmic.h>
#include <power/regulator.h>
#include <dm/device.h>

#define	LP8732		0x0
#define LP8733		0x1

#define LP873X_LDO_NUM		8

/* Drivers name */
#define LP873X_LDO_DRIVER	"lp873x_ldo"
#define LP873X_BUCK_DRIVER	"lp873x_buck"

#define LP873X_BUCK_VOLT_MASK		0xFF
#define LP873X_BUCK_VOLT_MAX_HEX	0xFF
#define LP873X_BUCK_VOLT_MAX		3360000
#define LP873X_BUCK_MODE_MASK		0x1

#define LP873X_LDO_VOLT_MASK    0x1F
#define LP873X_LDO_VOLT_MAX_HEX 0x19
#define LP873X_LDO_VOLT_MAX     3300000
#define LP873X_LDO_MODE_MASK	0x1

static const struct pmic_child_info pmic_children_info[] = {
	{ .prefix = "ldo", .driver = LP873X_LDO_DRIVER },
	{ },
};

static int lp873x_write(struct udevice *dev, uint reg, const uint8_t *buff,
			  int len)
{
	if (dm_i2c_write(dev, reg, buff, len)) {
		pr_err("write error to device: %p register: %#x!\n", dev, reg);
		return -EIO;
	}

	return 0;
}

static int lp873x_read(struct udevice *dev, uint reg, uint8_t *buff, int len)
{
	if (dm_i2c_read(dev, reg, buff, len)) {
		pr_err("read error from device: %p register: %#x!\n", dev, reg);
		return -EIO;
	}

	return 0;
}

static int starfive_bind(struct udevice *dev)
{
	ofnode regulators_node;
	int children;

	regulators_node = dev_read_subnode(dev, "regulators");
	if (!ofnode_valid(regulators_node)) {
		printf("%s: %s regulators subnode not found!\n", __func__,
		      dev->name);
		return -ENXIO;
	}

	children = pmic_bind_children(dev, regulators_node, pmic_children_info);
	if (!children)
		printf("%s: %s - no child found\n", __func__, dev->name);

	/* Always return success for this device */
	return 0;
}

static struct dm_pmic_ops lp873x_ops = {
	.read = lp873x_read,
	.write = lp873x_write,
};

static const struct udevice_id starfive_ids[] = {
	{ .compatible = "starfive,jh7110-evb-regulator", .data = LP8732 },
	{ }
};

U_BOOT_DRIVER(pmic_starfive) = {
	.name = "pmic_starfive",
	.id = UCLASS_PMIC,
	.of_match = starfive_ids,
	.bind = starfive_bind,
	.ops = &lp873x_ops,
};
