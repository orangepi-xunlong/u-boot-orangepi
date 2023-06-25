/*
 * Copyright (C) 2019 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SUNXI AXP  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <sunxi_power/bmu_axp2585.h>
#include <sunxi_power/axp.h>
#include <asm/arch/pmic_bus.h>

static int bmu_axp2585_probe(void)
{
	u8 bmu_chip_id;
	if (pmic_bus_init(AXP2585_DEVICE_ADDR, AXP2585_RUNTIME_ADDR)) {
		tick_printf("%s pmic_bus_init fail\n", __func__);
		return -1;
	}
	if (pmic_bus_read(AXP2585_RUNTIME_ADDR, AXP2585_CHIP_ID,
			  &bmu_chip_id)) {
		tick_printf("%s pmic_bus_read fail\n", __func__);
		return -1;
	}
	bmu_chip_id &= 0XCF;
	if (bmu_chip_id == 0x46 || bmu_chip_id == 0x49) {
		/*bmu type AXP2585*/
		tick_printf("BMU: AXP2585\n");
		return 0;
	}
	return -1;
}

int bmu_axp2585_set_power_off(void)
{
	u8 reg_value;
	if (pmic_bus_read(AXP2585_RUNTIME_ADDR, BMU_POWER_ON_STATUS,
			  &reg_value)) {
		return -1;
	}
	pmic_bus_write(AXP2585_RUNTIME_ADDR, BMU_POWER_ON_STATUS, reg_value);
	if (pmic_bus_read(AXP2585_RUNTIME_ADDR, BMU_CHG_STATUS, &reg_value)) {
		return -1;
	}
	if (reg_value & 0x02) {
		return 0;
	}
	if (pmic_bus_read(AXP2585_RUNTIME_ADDR, BMU_BATFET_CTL, &reg_value)) {
		return -1;
	}
	reg_value &= 0x7f;
	if (pmic_bus_write(AXP2585_RUNTIME_ADDR, BMU_BATFET_CTL, reg_value)) {
		return -1;
	}
	tick_printf("before reg[10]:%x\n", reg_value);
	mdelay(50);
	reg_value |= 1 << 7;
	if (pmic_bus_write(AXP2585_RUNTIME_ADDR, BMU_BATFET_CTL, reg_value)) {
		return -1;
	}
	return 0;
}

int bmu_axp2585_get_poweron_source(void)
{
	uchar reg_value;
	if (pmic_bus_read(AXP2585_RUNTIME_ADDR, BMU_POWER_ON_STATUS,
			  &reg_value)) {
		return -1;
	}
	tick_printf("poweron cause %x\n", reg_value);
	/*bit7 : vbus, bit6: battery inster, bit5: battery charge to normal*/
	if (reg_value & (1 << 7)) {
		reg_value &= 0x80;
		pmic_bus_write(AXP2585_RUNTIME_ADDR, BMU_POWER_ON_STATUS,
			       reg_value);
		return AXP_POWER_ON_BY_POWER_TRIGGER;
	} else {
		/*need to check supply pmu*/
		return AXP_POWER_ON_BY_POWER_KEY;
	}

	return reg_value & 0x01;
}

int bmu_axp2585_get_axp_bus_exist(void)
{
	u8 reg_value;
	if (pmic_bus_read(AXP2585_RUNTIME_ADDR, BMU_CHG_STATUS, &reg_value)) {
		return -1;
	}
	/*bit1: 0: vbus not power,  1: power good*/
	if (reg_value & 0x2) {
		return AXP_VBUS_EXIST;
	}
	return 0;
}

int bmu_axp2585_get_battery_vol(void)
{
	u8 reg_value_h = 0, reg_value_l = 0;
	int tmp_value = 0;
	int bat_vol;

	if (pmic_bus_read(AXP2585_RUNTIME_ADDR, BMU_BAT_VOL_H, &reg_value_h)) {
		return -1;
	}
	if (pmic_bus_read(AXP2585_RUNTIME_ADDR, BMU_BAT_VOL_L, &reg_value_l)) {
		return -1;
	}

	tmp_value = (reg_value_h << 4) | reg_value_l;
	bat_vol   = tmp_value * 12 / 10;

	return bat_vol;
}

int bmu_axp2585_get_battery_capacity(void)
{
	u8 reg_value;

	if (pmic_bus_read(AXP2585_RUNTIME_ADDR, BMU_BAT_PERCENTAGE,
			  &reg_value)) {
		return -1;
	}

	//bit7-- 1:valid 0:invalid
	if (reg_value & (1 << 7)) {
		reg_value = reg_value & 0x7f;
	} else {
		reg_value = 0;
	}

	return reg_value;
}

int bmu_axp2585_get_battery_probe(void)
{
	u8 reg_value;

	if (pmic_bus_read(AXP2585_RUNTIME_ADDR, BMU_BAT_STATUS, &reg_value)) {
		return -1;
	}
	//bit2 -- battery detect result
	if ((reg_value & (1 << 3))) {
		return 1;
	}
	return 0;
}

int bmu_axp2585_set_vbus_current_limit(int current)
{
	u8 reg_value;
	u8 temp;
	if (pmic_bus_read(AXP2585_RUNTIME_ADDR, BMU_BATFET_CTL, &reg_value)) {
		return -1;
	}
	if (current) {
		if (current > 3250) {
			temp = 0x3F;
		} else if (current >= 100) {
			temp = (current - 100) / 50;
		} else {
			temp = 0x00;
		}
	} else {
		/*default was 2500ma*/
		temp = 0x30;
	}
	reg_value &= 0xB0;
	reg_value |= temp;
	tick_printf("Input current:%d mA\n", current);
	if (pmic_bus_write(AXP2585_RUNTIME_ADDR, BMU_BATFET_CTL, reg_value)) {
		return -1;
	}
	return 0;
}

unsigned char bmu_axp2585_get_reg_value(unsigned char reg_addr)
{
	u8 reg_value;
	if (pmic_bus_read(AXP2585_RUNTIME_ADDR, reg_addr, &reg_value)) {
		return -1;
	}
	return reg_value;
}

unsigned char bmu_axp2585_set_reg_value(unsigned char reg_addr, unsigned char reg_value)
{
	unsigned char reg;
	if (pmic_bus_write(AXP2585_RUNTIME_ADDR, reg_addr, reg_value)) {
		return -1;
	}
	if (pmic_bus_read(AXP2585_RUNTIME_ADDR, reg_addr, &reg)) {
		return -1;
	}
	return reg;
}

U_BOOT_AXP_BMU_INIT(bmu_axp2585) = {
	.bmu_name	   = "bmu_axp2585",
	.probe		    = bmu_axp2585_probe,
	.set_power_off      = bmu_axp2585_set_power_off,
	.get_poweron_source = bmu_axp2585_get_poweron_source,
	.get_axp_bus_exist  = bmu_axp2585_get_axp_bus_exist,
	/*.set_coulombmeter_onoff	= bmu_axp2585_set_coulombmeter_onoff,*/
	.get_battery_vol	= bmu_axp2585_get_battery_vol,
	.get_battery_capacity   = bmu_axp2585_get_battery_capacity,
	.get_battery_probe      = bmu_axp2585_get_battery_probe,
	.set_vbus_current_limit = bmu_axp2585_set_vbus_current_limit,
	/*.get_vbus_current_limit		= bmu_axp2585_get_vbus_current_limit,*/
	/*.set_charge_current_limit	= bmu_axp2585_set_charge_current_limit,*/
	.get_reg_value = bmu_axp2585_get_reg_value,
	.set_reg_value = bmu_axp2585_set_reg_value,
};
