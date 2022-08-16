// SPDX-License-Identifier: GPL-2.0+
/*
 * AXP806 driver
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

#define AXP806_DCDC4_1600MV_OFFSET 46

int axp_set_dcdc4(unsigned int mvolt)
{
	u8 reg_value;
	u8 tmp_step;

	if (mvolt > 0) {
		if (mvolt <= 1500) {
			if (mvolt < 600)
				mvolt = 600;
			tmp_step = (mvolt - 600) / 20;
		} else {
			if (mvolt < 1600)
				mvolt = 1600;
			else if (mvolt > 3300)
				mvolt = 3300;

			tmp_step = (mvolt - 1600) / 100 + 47;
		}

		if (pmic_bus_read(AXP806_DCDCD_VOLTAGE, &reg_value)) {
			printf("sunxi pmu error : unable to read dcdcd reg\n");
			return -1;
		}

		reg_value &= 0xC0;
		reg_value |= tmp_step;
		if (pmic_bus_write(AXP806_DCDCD_VOLTAGE, reg_value)) {
			printf("sunxi pmu error : unable to set dcdcd\n");
			return -1;
		}
	}

	if (pmic_bus_read(AXP806_OUTPUT_CTRL1, &reg_value)) {
		return -1;
	}

	reg_value |=  (1 << 3);

	if (pmic_bus_write(AXP806_OUTPUT_CTRL1, reg_value)) {
		printf("sunxi pmu error : unable to on dcdcd\n");
		return -1;
	}

	return 0;
}

static inline void disable_pmu_pfm_mode(void)
{
	u8 val;

	pmic_bus_read(AXP806_DCMOD_CTL2, &val);
	val |= 0x1f;
	pmic_bus_write(AXP806_DCMOD_CTL2, val);
}

int axp_init(void)
{
	u8 axp_chip_id;
        u8 val;
	int ret;

	ret = pmic_bus_init();
	if (ret)
		return ret;

	ret = pmic_bus_read(AXP806_CHIP_VERSION, &axp_chip_id);
	if (ret)
		return ret;

	if ((axp_chip_id & AXP806_CHIP_VERSION_MASK) != 0x40)
		return -ENODEV;

	printf("PMU: AXP806\n");

	disable_pmu_pfm_mode();

	return ret;
}

#ifndef CONFIG_PSCI_RESET
int do_poweroff(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	pmic_bus_write(AXP806_SHUTDOWN, AXP806_POWEROFF);

	/* infinite loop during shutdown */
	while (1) {}

	/* not reached */
	return 0;
}
#endif
