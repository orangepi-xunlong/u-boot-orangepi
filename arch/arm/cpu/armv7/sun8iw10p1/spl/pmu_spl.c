/*
 * (C) Copyright 2016
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *zhouhuacai <zhouhuacai@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/arch/base_pmu.h>

#define SUNXI_AXP_22X                   22
#define SUNXI_AXP_15X                   15

#define AXP22_ADDR              (0x68>>1)
#define AXP15_ADDR              (0x60>>1)

#define BOOT_POWER15_DC3OUT_VOL (0x27)
#define BOOT_POWER22_DC5OUT_VOL (0x25)
#define AXP15X_IC_ID_REG        (0x05)
#define AXP15X_DDR_VOL_CTRL     (0x0)
#define AXP15X_ADDR             (0x60>>1)

#define dbg(format,arg...)	printf(format,##arg)

static int pmu_type;

static int axp_probe(void)
{
	u8  pmu_type;

	if (axp_i2c_read(AXP15X_ADDR, 0x3, &pmu_type)) {
		printf("axp15x read error\n");

		if (axp_i2c_read(AXP22_ADDR, 0x3, &pmu_type)) {
			printf("axp22x read error\n");

			return -1;
		}
	}

	pmu_type &= 0x0f;

	if (pmu_type == 0x05) {
		/* pmu type AXP152 */
		printf("PMU: AXP15X\n");

		return SUNXI_AXP_15X;
	}

	if (pmu_type & 0x06) {
		/* pmu type AXP221 */
		printf("PMU: AXP221\n");

		return SUNXI_AXP_22X;
	}

	printf("unknown pmu type\n");

	return -1;
}

static int axp15X_set_dcdc3(int set_vol)
{
	u8  reg_value = 0;

	if (set_vol > 0) {
		if (set_vol < 700) {
			set_vol = 700;
		} else if (set_vol > 3500) {
			set_vol = 3500;
		}

		if (axp_i2c_read(AXP15_ADDR, BOOT_POWER15_DC3OUT_VOL, &reg_value)) {
			printf("can't read dcdc3_vol!!!\n");
			return -1;
		}

		reg_value &= ~0x3f;
		reg_value = ((set_vol - 700) / 50);

		if (axp_i2c_write(AXP15_ADDR, BOOT_POWER15_DC3OUT_VOL, reg_value)) {
			printf("sunxi pmu error : unable to set dcdc3\n");
			return -1;
		}

		printf("ddr voltage = %d mv\n", set_vol);
	}

	return 0;
}

static int axp22X_set_dcdc5(int set_vol)
{
	u8  reg_value = 0;

	if (set_vol > 0) {
		if (set_vol < 1000) {
			set_vol = 1000;
		} else if (set_vol > 2550) {
			set_vol = 2550;
		}

		if (axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC5OUT_VOL, &reg_value)) {
			dbg("sunxi pmu error : unable to read dcdc5\n");
			return -1;
		}

		reg_value = ((set_vol - 1000) / 50);

		if (axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DC5OUT_VOL, reg_value)) {
			dbg("sunxi pmu error : unable to set dcdc5\n");
			return -1;
		}

		printf("ddr voltage = %d mv\n", set_vol);
	}

	return 0;
}

int set_ddr_voltage(int set_vol)
{
	pmu_type = axp_probe();

	if (pmu_type == SUNXI_AXP_15X) {
		return axp15X_set_dcdc3(set_vol);
	} else if (pmu_type == SUNXI_AXP_22X) {
		return axp22X_set_dcdc5(set_vol);
	}

	return -1;
}

int pmu_init(u8 power_mode)
{
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	pmu_type = axp_probe();

	return pmu_type;
}
