/*
 * (C) Copyright 2016
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *zhouhuacai <zhouhuacai@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/arch/base_pmu.h>

#define  dbg(format,arg...)	printf(format,##arg)
#define  SUNXI_AXP_22X          22
#define  SUNXI_AXP_209          209
#define  AXP22_ADDR              (0x68>>1)
#define  AXP20_ADDR              (0x68>>1)
#define  BOOT_POWER15_DC3OUT_VOL (0x27)
#define  BOOT_POWER22_DC5OUT_VOL (0x25)

static int pmu_type;

static int axp_probe(void)
{
	u8  pmu_type;

	if (axp_i2c_read(AXP22_ADDR, 0x3, &pmu_type)) {
		printf("axp22x or axp209 read error\n");
		return -1;
	}

	pmu_type &= 0x0f;
	printf("PMU: ");

	if (pmu_type & 0x06) {
		/* pmu type AXP221 */
		printf("AXP221\n");
		return SUNXI_AXP_22X;
	} else if (pmu_type & 0x01) {
		/* pmu type AXP209 */
		printf("AXP209\n");
		return SUNXI_AXP_209;
	}

	printf("unknown pmu\n");
	return -1;
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

static int axp209_set_rtc(int set_vol)
{
	/* modify LDO1  vol
	REGF4 write 0x06
	REGF2 write 0x84
	REGF5 write 0x04
	REGFF write 0x01

	REG06 write 0x40
	REG17 write 0x40

	REGFF write 0x00
	REGF4 write 0x00*/

	int reg[8] = {0xf4, 0xF2, 0xf5, 0xff, 0x06, 0x17, 0xff, 0xf4};
	int val[8] = {0x06, 0x84, 0x04, 0x01, 0x40, 0x40, 0x00, 0x00};
	int i;

	for (i = 0; i < 7; i++) {
		printf("set reg%x = %x\n", reg[i], val[i]);

		if (axp_i2c_write(AXP20_ADDR, reg[i], val[i])) {
			printf("set rtc voltage fail\n");
			return -1;
		}

	}

	printf("set rtc voltage ok\n");
	return 0;
}

int set_ddr_voltage(int set_vol)
{
	if (pmu_type == SUNXI_AXP_22X) {
		return axp22X_set_dcdc5(set_vol);
	} else if (pmu_type == SUNXI_AXP_209) {
		return 0;
	}

	return -1;
}

int set_rtc_voltage(int set_vol)
{
	if (pmu_type == SUNXI_AXP_209) {
		return axp209_set_rtc(0);
	}

	return -1;
}

int pmu_init(u8 power_mode)
{
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	pmu_type = axp_probe();

	return pmu_type;
}