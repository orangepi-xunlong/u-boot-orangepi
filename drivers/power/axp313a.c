// SPDX-License-Identifier: GPL-2.0+
/*
 * AXP313a driver
 *
 * (C) Copyright 2020 Jernej Skrabec <jernej.skrabec@siol.net>
 *
 * Based on axp221.c
 * (C) Copyright 2014 Hans de Goede <hdegoede@redhat.com>
 * (C) Copyright 2013 Oliver Schinagl <oliver@schinagl.nl>
 */

#include <common.h>
#include <command.h>
#include <errno.h>
#include <asm/arch/pmic_bus.h>
#include <axp_pmic.h>

#define AXP313A_DCDC4_1600MV_OFFSET 46

static u8 axp313a_mvolt_to_cfg(int mvolt, int min, int max, int div)
{
	if (mvolt < min)
		mvolt = min;
	else if (mvolt > max)
		mvolt = max;

	return  (mvolt - min) / div;
}

#define AXP313A_DCDC3_1200MV_OFFSET 71
int axp_set_dcdc3(unsigned int mvolt)
{
	int ret;
	u8 cfg;

	if (mvolt >= 1220)
		cfg = AXP313A_DCDC3_1200MV_OFFSET +
			axp313a_mvolt_to_cfg(mvolt, 1220, 1840, 20);
	else
		cfg = axp313a_mvolt_to_cfg(mvolt, 500, 1200, 10);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP313A_OUTPUT_CTRL1,
					AXP313A_OUTPUT_CTRL1_DCDCD_EN);

	ret = pmic_bus_write(AXP313A_DCDCD_VOLTAGE, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP313A_OUTPUT_CTRL1,
				0x1f);
}

int axp_init(void)
{
	u8 axp_chip_id;
	int ret;

	ret = pmic_bus_init();
	if (ret)
		return ret;

	ret = pmic_bus_read(AXP313A_CHIP_VERSION, &axp_chip_id);
	if (ret)
		return ret;

	if ((axp_chip_id & AXP313A_CHIP_VERSION_MASK) != 0x4b)
		return -ENODEV;

        //printf("axp313a pmic id is 0x%x\n",axp_chip_id);

	return ret;
}

#ifndef CONFIG_PSCI_RESET
int do_poweroff(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	pmic_bus_write(AXP313A_SHUTDOWN, AXP313A_POWEROFF);

	/* infinite loop during shutdown */
	while (1) {}

	/* not reached */
	return 0;
}
#endif
