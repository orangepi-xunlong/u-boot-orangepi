/*
 * Copyright (C) 2019 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SUNXI AXP  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <sunxi_power/bmu_axp81X.h>
#include <sunxi_power/axp.h>
#include <asm/arch/pmic_bus.h>

int bmu_axp81X_get_poweron_source(void);
int runtime_tick(void);

static int bmu_axp81X_probe(void)
{
	u8 pmu_chip_id;
	if (pmic_bus_init(AXP81X_DEVICE_ADDR, AXP81X_RUNTIME_ADDR)) {
		tick_printf("%s pmic_bus_init fail\n", __func__);
		return -1;
	}
	if (pmic_bus_read(AXP81X_RUNTIME_ADDR, AXP81X_VERSION, &pmu_chip_id)) {
		tick_printf("%s pmic_bus_read fail\n", __func__);
		return -1;
	}
	pmu_chip_id &= 0XCF;
	if (pmu_chip_id == AXP81X_CHIP_ID) {
		/*pmu type AXP803*/
		bmu_axp81X_get_poweron_source();
		tick_printf("PMU: AXP803\n");
		return 0;
	}
	return -1;

}

int bmu_axp81X_get_battery_probe(void)
{
	u8 reg_value;
	static int probe_count;

	if (!probe_count) {
		probe_count++;
		int old_time = runtime_tick();
		while ((runtime_tick() - old_time) < 2000) {
			if (pmic_bus_read(AXP81X_RUNTIME_ADDR, AXP81X_MODE_CHGSTATUS, &reg_value))
				return -1;
			if ((reg_value & (1<<4)) && (reg_value & (1<<5)))
				return 1;
		}
	}

	if (pmic_bus_read(AXP81X_RUNTIME_ADDR, AXP81X_MODE_CHGSTATUS, &reg_value))
		return -1;
	/*bit4 determines whether bit5 is valid*/
	/*bit5 determines whether bat is exist*/
	if ((reg_value & (1<<4)) && (reg_value & (1<<5)))
		return 1;

	return -1;
}

int bmu_axp81X_reset_capacity(void)
{
	if (pmic_bus_write(AXP81X_RUNTIME_ADDR, AXP81X_BAT_MAX_CAP1, 0x00))
		return -1;

	if (pmic_bus_write(AXP81X_RUNTIME_ADDR, AXP81X_BAT_MAX_CAP0, 0x00))
		return -1;

	return 1;
}


int bmu_axp81X_get_poweron_source(void)
{
	static uchar reg_value;
	if (!reg_value) {
		if (pmic_bus_read(AXP81X_RUNTIME_ADDR, AXP81X_OTG_STATUS, &reg_value)) {
			return -1;
		}

		if (pmic_bus_write(AXP81X_RUNTIME_ADDR, AXP81X_OTG_STATUS, 0xff)) {
			return -1;
		}
	}

	if (reg_value & (1 << 0)) {
		return AXP_BOOT_SOURCE_BUTTON;
	} else if (reg_value & (1 << 1)) {
		if (bmu_axp81X_get_battery_probe() > -1)
			return AXP_BOOT_SOURCE_CHARGER;
		else
			return AXP_BOOT_SOURCE_VBUS_USB;
	} else if (reg_value & (1 << 2)) {
		return AXP_BOOT_SOURCE_BATTERY;
	}
	return -1;
}

int bmu_axp81X_get_axp_bus_exist(void)
{
	u8 reg_value;
	if (pmic_bus_read(AXP81X_RUNTIME_ADDR, AXP81X_STATUS, &reg_value)) {
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

int bmu_axp81X_get_battery_vol(void)
{
	u8  reg_value_h, reg_value_l;
	int bat_vol, tmp_value;

	if (pmic_bus_read(AXP81X_RUNTIME_ADDR, AXP81X_BAT_AVERVOL_H8, &reg_value_h)) {
		return -1;
	}
	if (pmic_bus_read(AXP81X_RUNTIME_ADDR, AXP81X_BAT_AVERVOL_L4, &reg_value_l)) {
	    return -1;
	}
	tmp_value = (reg_value_h << 4) | reg_value_l;
	bat_vol = tmp_value * 1100 / 1000;

	return bat_vol;
}

int bmu_axp81X_get_battery_capacity(void)
{
	u8 reg_value;
	int old_time = runtime_tick();
	while ((runtime_tick() - old_time) < 2000) {
		if (pmic_bus_read(AXP81X_RUNTIME_ADDR, AXP81X_BAT_PERCEN_CAL,
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
int bmu_axp81X_set_coulombmeter_onoff(int onoff)
{
	u8 reg_value;

	if (pmic_bus_read(AXP81X_RUNTIME_ADDR, AXP81X_FUEL_GAUGE_CTL,
			  &reg_value)) {
		return -1;
	}
	if (!onoff)
		reg_value &= ~(0x01 << 3);
	else
		reg_value |= (0x01 << 3);

	if (pmic_bus_write(AXP81X_RUNTIME_ADDR, AXP81X_FUEL_GAUGE_CTL,
			   reg_value)) {
		return -1;
	}
	return 0;
}

int bmu_axp81X_set_vbus_current_limit(int current)
{
	u8 reg_value;
	if (pmic_bus_read(AXP81X_RUNTIME_ADDR, AXP81X_VBUS_CUR_SET, &reg_value)) {
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
	if (pmic_bus_write(AXP81X_RUNTIME_ADDR, AXP81X_VBUS_CUR_SET, reg_value)) {
		return -1;
	}
	return 0;
}

int bmu_axp81X_get_vbus_current_limit(void)
{
	uchar reg_value;
	if (pmic_bus_read(AXP81X_RUNTIME_ADDR, AXP81X_VBUS_CUR_SET, &reg_value)) {
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
int bmu_axp81X_set_charge_current_limit(int current)
{
	u8 reg_value;
	int step;

	if (pmic_bus_read(AXP81X_RUNTIME_ADDR, AXP81X_CHARGE1, &reg_value)) {
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
	if (pmic_bus_write(AXP81X_RUNTIME_ADDR, AXP81X_CHARGE1, reg_value)) {
		return -1;
	}

	return 0;
}
#endif

unsigned char bmu_axp81X_get_reg_value(unsigned char reg_addr)
{
	u8 reg_value;
	if (pmic_bus_read(AXP81X_RUNTIME_ADDR, reg_addr, &reg_value)) {
		return -1;
	}
	return reg_value;
}

unsigned char bmu_axp81X_set_reg_value(unsigned char reg_addr, unsigned char reg_value)
{
	unsigned char reg;
	if (pmic_bus_write(AXP81X_RUNTIME_ADDR, reg_addr, reg_value)) {
		return -1;
	}
	if (pmic_bus_read(AXP81X_RUNTIME_ADDR, reg_addr, &reg)) {
		return -1;
	}
	return reg;
}

int bmu_axp81X_set_ntc_onff(int onoff)
{
	unsigned char reg_value;
	if (!onoff) {
		if (pmic_bus_read(AXP81X_RUNTIME_ADDR, AXP81X_ADC_EN, &reg_value))
			return -1;

		reg_value &= ~(1 << 0);
		if (pmic_bus_write(AXP81X_RUNTIME_ADDR, AXP81X_ADC_EN, reg_value))
			return -1;

		if (pmic_bus_read(AXP81X_RUNTIME_ADDR, AXP81X_ADC_SPEED_TS, &reg_value))
			return -1;

		reg_value |= (1 << 2);
		reg_value &= ~(3 << 0);
		if (pmic_bus_write(AXP81X_RUNTIME_ADDR, AXP81X_ADC_SPEED_TS, reg_value))
			return -1;
	} else {
		if (pmic_bus_read(AXP81X_RUNTIME_ADDR, AXP81X_ADC_EN, &reg_value))
			return -1;

		reg_value |= (1 << 0);
		if (pmic_bus_write(AXP81X_RUNTIME_ADDR, AXP81X_ADC_EN, reg_value))
			return -1;

		if (pmic_bus_read(AXP81X_RUNTIME_ADDR, AXP81X_ADC_SPEED_TS, &reg_value))
			return -1;

		reg_value &= ~(1 << 2);
		reg_value |= (3 << 0);
		if (pmic_bus_write(AXP81X_RUNTIME_ADDR, AXP81X_ADC_SPEED_TS, reg_value))
			return -1;

	}
	return 0;
}

U_BOOT_AXP_BMU_INIT(bmu_axp81X) = {
	.bmu_name		  = "bmu_axp81X",
	.probe			  = bmu_axp81X_probe,
	/*set_power_off		  = bmu_axp81X_set_power_off,*/
	.get_poweron_source       = bmu_axp81X_get_poweron_source,
	.get_axp_bus_exist	= bmu_axp81X_get_axp_bus_exist,
	/*.set_coulombmeter_onoff   = bmu_axp81X_set_coulombmeter_onoff,*/
	.get_battery_vol	  = bmu_axp81X_get_battery_vol,
	.get_battery_probe	= bmu_axp81X_get_battery_probe,
	.get_battery_capacity     = bmu_axp81X_get_battery_capacity,
	/*.set_vbus_current_limit   = bmu_axp81X_set_vbus_current_limit,
	.get_vbus_current_limit   = bmu_axp81X_get_vbus_current_limit,
	.set_charge_current_limit = bmu_axp81X_set_charge_current_limit,*/
	.get_reg_value	   = bmu_axp81X_get_reg_value,
	.set_reg_value	   = bmu_axp81X_set_reg_value,
	.reset_capacity	   = bmu_axp81X_reset_capacity,
	.set_ntc_onoff     = bmu_axp81X_set_ntc_onff,
};

