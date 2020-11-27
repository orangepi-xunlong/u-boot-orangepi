/*
 * (C) Copyright 2016
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *luoweijian <luoweijian@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <power/sunxi/axp22_reg.h>
#include <asm/arch/base_pmu.h>

#define SUNXI_AXP_22X            22
#define AXP22_ADDR              (0x68>>1)
#define BOOT_POWER22_DC5OUT_VOL (0x25)
#define RSB_SADDR_AXP22X	    (0x3A3)


#define dbg(format,arg...)	printf(format,##arg)

static int pmu_type;

static int axp_probe(void)
{
	u8  pmu_type;

	printf("boot0 axp probe\n");
	if (axp_i2c_read(AXP22_ADDR, 0x3, &pmu_type)) {
		printf("axp22x read error\n");
		return -1;
	}

	pmu_type &= 0x0f;

	if (pmu_type & 0x06) {
		/* pmu type AXP221 */
		printf("PMU: AXP221\n");

		return SUNXI_AXP_22X;
	}

	printf("unknown pmu type\n");

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

int probe_power_key(void)
{
	u8  reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_INTSTS3, &reg_value))
    {
        return -1;
    }

    reg_value &= 0x03;

	if(reg_value)
	{
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_INTSTS3, reg_value))
	    {
	        return -1;
	    }
	}

	return reg_value;
}


/*int set_ddr_voltage(int set_vol)
{
	pmu_type = axp_probe();

    if (pmu_type == SUNXI_AXP_22X) {
		return axp22X_set_dcdc5(set_vol);
	}

	return -1;
}*/

int set_ddr_voltage(int set_vol)
{
	if(sunxi_rsb_init(0))
		return -1;

	if(sunxi_rsb_config(AXP22_ADDR, RSB_SADDR_AXP22X))
		return -1;

	return axp22X_set_dcdc5(set_vol);
}

int pmu_init(u8 power_mode)
{
	if(sunxi_rsb_init(0))
		return -1;

	if(sunxi_rsb_config(AXP22_ADDR, RSB_SADDR_AXP22X))
		return -1;
	pmu_type = axp_probe();

	return pmu_type;
}
