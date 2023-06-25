/*
 * Copyright (C) 2019 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SUNXI AXP  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <sunxi_power/bmu_axp221.h>
#include <sunxi_power/axp.h>
#include <asm/arch/pmic_bus.h>
#include <sys_config.h>

int bmu_axp221_get_poweron_source(void);
int runtime_tick(void);

static int bmu_axp221_probe(void)
{
	u8 pmu_chip_id;
	int axp_bus_num;

	script_parser_fetch(FDT_PATH_POWER_SPLY, "axp_bus_num", &axp_bus_num, AXP221_DEVICE_ADDR);

	if (pmic_bus_init(axp_bus_num, AXP221_RUNTIME_ADDR)) {
		tick_printf("%s pmic_bus_init fail\n", __func__);
		return -1;
	}
	if (pmic_bus_read(AXP221_RUNTIME_ADDR, AXP221_VERSION, &pmu_chip_id)) {
		tick_printf("%s pmic_bus_read fail\n", __func__);
		return -1;
	}
	pmu_chip_id &= 0XCF;
	if (pmu_chip_id == AXP221_CHIP_ID || pmu_chip_id == AXP221_CHIP_ID_EXT) {
		/*pmu type AXP221*/
		bmu_axp221_get_poweron_source();
		tick_printf("PMU: AXP221\n");
		return 0;
	}
	return -1;

}

int bmu_axp221_get_battery_probe(void)
{
	u8 reg_value;
	static int probe_count;

	if (!probe_count) {
		probe_count++;
		int old_time = runtime_tick();
		while ((runtime_tick() - old_time) < 2000) {
			if (pmic_bus_read(AXP221_RUNTIME_ADDR, AXP221_MODE_CHGSTATUS, &reg_value))
				return -1;
			if (reg_value & (1<<5))
				return 1;
		}
	}

	if (pmic_bus_read(AXP221_RUNTIME_ADDR, AXP221_MODE_CHGSTATUS, &reg_value))
		return -1;
	/*bit4 determines whether bit5 is valid*/
	/*bit5 determines whether bat is exist*/
	if (reg_value & (1<<5))
		return 1;

	return -1;
}

int bmu_axp221_reset_capacity(void)
{
	if (pmic_bus_write(AXP221_RUNTIME_ADDR, AXP221_BAT_MAX_CAP1, 0x00))
		return -1;

	if (pmic_bus_write(AXP221_RUNTIME_ADDR, AXP221_BAT_MAX_CAP0, 0x00))
		return -1;

	return 1;
}


int bmu_axp221_get_poweron_source(void)
{
	static uchar reg_value;

	if (pmic_bus_read(AXP221_RUNTIME_ADDR, AXP221_STATUS, &reg_value))
			return -1;

	if (reg_value & (1 << 0)) {
		return AXP_BOOT_SOURCE_CHARGER;
	} else {
		return AXP_BOOT_SOURCE_BUTTON;
	}

	return -1;
}

int bmu_axp221_get_axp_bus_exist(void)
{
	u8 reg_value;
	if (pmic_bus_read(AXP221_RUNTIME_ADDR, AXP221_STATUS, &reg_value)) {
		return -1;
	}
	if (reg_value & 0x10) {		//vbus exist
		return AXP_VBUS_EXIST;
	}
	if (reg_value & 0x40) {		//dc in exist
		return AXP_DCIN_EXIST;
	}
	return 0;

}

int bmu_axp221_get_battery_vol(void)
{
	u8  reg_value_h, reg_value_l;
	int bat_vol, tmp_value;

	if (pmic_bus_read(AXP221_RUNTIME_ADDR, AXP221_BAT_AVERVOL_H8, &reg_value_h)) {
		return -1;
	}
	if (pmic_bus_read(AXP221_RUNTIME_ADDR, AXP221_BAT_AVERVOL_L4, &reg_value_l)) {
	    return -1;
	}
	tmp_value = (reg_value_h << 4) | reg_value_l;
	bat_vol = tmp_value * 1100 / 1000;

	return bat_vol;
}

int bmu_axp221_get_battery_capacity(void)
{
	u8 reg_value;
	int old_time = runtime_tick();
	while ((runtime_tick() - old_time) < 2000) {
		if (pmic_bus_read(AXP221_RUNTIME_ADDR, AXP221_BAT_PERCEN_CAL,
				  &reg_value)) {
			return -1;
		}
		if (reg_value) {
			return reg_value&0x7f;
		}
	}
	return -1;
}


#if 0
int bmu_axp221_set_coulombmeter_onoff(int onoff)
{
	u8 reg_value;

	if (pmic_bus_read(AXP221_RUNTIME_ADDR, AXP221_FUEL_GAUGE_CTL,
			  &reg_value)) {
		return -1;
	}
	if (!onoff)
		reg_value &= ~(0x01 << 3);
	else
		reg_value |= (0x01 << 3);

	if (pmic_bus_write(AXP221_RUNTIME_ADDR, AXP221_FUEL_GAUGE_CTL,
			   reg_value)) {
		return -1;
	}
	return 0;
}

int bmu_axp221_set_vbus_current_limit(int current)
{
	u8 reg_value;
	if (pmic_bus_read(AXP221_RUNTIME_ADDR, AXP221_VBUS_CUR_SET, &reg_value)) {
		return -1;
	}
	reg_value &= 0xf8;

	if (current >= 2000) {
		/*limit to 2000mA */
		reg_value |= 0x05;
	} else if (current >= 1500) {
		/* limit to 1500mA */
		reg_value |= 0x04;
	} else if (current >= 1000) {
		/* limit to 1000mA */
		reg_value |= 0x03;
	} else if (current >= 900) {
		/*limit to 900mA */
		reg_value |= 0x02;
	} else if (current >= 500) {
		/*limit to 500mA */
		reg_value |= 0x01;
	} else if (current >= 100) {
		/*limit to 100mA */
		reg_value |= 0x0;
	} else
		reg_value |= 0x01;

	tick_printf("Input current:%d mA\n", current);
	if (pmic_bus_write(AXP221_RUNTIME_ADDR, AXP221_VBUS_CUR_SET, reg_value)) {
		return -1;
	}
	return 0;
}

int bmu_axp221_get_vbus_current_limit(void)
{
	uchar reg_value;
	if (pmic_bus_read(AXP221_RUNTIME_ADDR, AXP221_VBUS_CUR_SET, &reg_value)) {
		return -1;
	}
	reg_value &= 0x07;
	if (reg_value == 0x05) {
		printf("limit to 2000mA \n");
		return 2000;
	} else if (reg_value == 0x04) {
		printf("limit to 1500mA \n");
		return 1500;
	} else if (reg_value == 0x03) {
		printf("limit to 1000mA \n");
		return 1000;
	} else if (reg_value == 0x02) {
		printf("limit to 900mA \n");
		return 900;
	} else if (reg_value == 0x01) {
		printf("limit to 500mA \n");
		return 500;
	} else if (reg_value == 0x00) {
		printf("limit to 100mA \n");
		return 100;
	} else {
		printf("do not limit current \n");
		return 0;
	}
}
int bmu_axp221_set_charge_current_limit(int current)
{
	u8 reg_value;
	int step;

	if (pmic_bus_read(AXP221_RUNTIME_ADDR, AXP221_CHARGE1, &reg_value)) {
		return -1;
	}
	reg_value &= ~0x1f;
	if (current > 2000) {
		current = 2000;
	}
	if (current <= 200)
		step = current / 25;
	else
		step = (current / 100) + 6;

	reg_value |= (step & 0x1f);
	if (pmic_bus_write(AXP221_RUNTIME_ADDR, AXP221_CHARGE1, reg_value)) {
		return -1;
	}

	return 0;
}
#endif

unsigned char bmu_axp221_get_reg_value(unsigned char reg_addr)
{
	u8 reg_value;
	if (pmic_bus_read(AXP221_RUNTIME_ADDR, reg_addr, &reg_value)) {
		return -1;
	}
	return reg_value;
}

unsigned char bmu_axp221_set_reg_value(unsigned char reg_addr, unsigned char reg_value)
{
	unsigned char reg;
	if (pmic_bus_write(AXP221_RUNTIME_ADDR, reg_addr, reg_value)) {
		return -1;
	}
	if (pmic_bus_read(AXP221_RUNTIME_ADDR, reg_addr, &reg)) {
		return -1;
	}
	return reg;
}


U_BOOT_AXP_BMU_INIT(bmu_axp221) = {
	.bmu_name		  = "bmu_axp221",
	.probe			  = bmu_axp221_probe,
	/*set_power_off		  = bmu_axp221_set_power_off,*/
	.get_poweron_source       = bmu_axp221_get_poweron_source,
	.get_axp_bus_exist	= bmu_axp221_get_axp_bus_exist,
	/*.set_coulombmeter_onoff   = bmu_axp221_set_coulombmeter_onoff,*/
	.get_battery_vol	  = bmu_axp221_get_battery_vol,
	.get_battery_probe	= bmu_axp221_get_battery_probe,
	.get_battery_capacity     = bmu_axp221_get_battery_capacity,
	/*.set_vbus_current_limit   = bmu_axp221_set_vbus_current_limit,
	.get_vbus_current_limit   = bmu_axp221_get_vbus_current_limit,
	.set_charge_current_limit = bmu_axp221_set_charge_current_limit,*/
	.get_reg_value	   = bmu_axp221_get_reg_value,
	.set_reg_value	   = bmu_axp221_set_reg_value,
	.reset_capacity	   = bmu_axp221_reset_capacity,
};

