/*
 * Copyright (C) 2019 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SUNXI AXP  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <sunxi_power/bmu_axp2202.h>
#include <sunxi_power/axp.h>
#include <asm/arch/pmic_bus.h>

static int bmu_axp2202_probe(void)
{
	u8 bmu_chip_id;
	if (pmic_bus_init(AXP2202_DEVICE_ADDR, AXP2202_RUNTIME_ADDR)) {
		tick_printf("%s pmic_bus_init fail\n", __func__);
		return -1;
	}
	if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_CHIP_ID, &bmu_chip_id)) {
		tick_printf("%s pmic_bus_read fail\n", __func__);
		return -1;
	}
	bmu_chip_id &= 0Xff;

	if (bmu_chip_id == 0x01) {
		/*bmu type AXP21*/
		tick_printf("BMU: AXP2202\n");

		if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_VERSION, &bmu_chip_id)) {
			tick_printf("%s pmic_bus_read fail\n", __func__);
			return -1;
		}

		bmu_chip_id &= 0Xff;

		if (bmu_chip_id == 0x00) {
			/*bmu type AXP21*/
			tick_printf("BMU: AXP2202 VER_A\n");
		}
		return 0;
	}

	if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_CHIP_ID_EXT, &bmu_chip_id)) {
		tick_printf("%s pmic_bus_read fail\n", __func__);
		return -1;
	}

	if (bmu_chip_id == 0x02) {
		tick_printf("BMU: AXP2202\n");
		return 0;
	}

	return -1;
}

int bmu_axp2202_set_power_off(void)
{
	u8 reg_value;
	int set_vol = 3200;

	if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_VOFF_THLD, &reg_value)) {
		return -1;
	}
	reg_value &= 0x8f;
	if (set_vol >= 2600 && set_vol <= 3200) {
		reg_value |= (set_vol - 2600) / 100;
	} else if (set_vol <= 2600) {
		reg_value |= 0x00;
	} else {
		reg_value |= 0x70;
	}
	if (pmic_bus_write(AXP2202_RUNTIME_ADDR, AXP2202_VOFF_THLD, reg_value)) {
		return -1;
	}

	return 0;
}

int bmu_axp2202_reset_capacity(void)
{
	if (pmic_bus_write(AXP2202_RUNTIME_ADDR, AXP2202_GAUGE_CONFIG, 0x00))
		return -1;

	return 1;
}


/*
	boot_source	0x20		help			return

	power low	BIT0		boot button		AXP_BOOT_SOURCE_BUTTON
	irq		BIT1		IRQ LOW			AXP_BOOT_SOURCE_IRQ_LOW
	usb		BIT2		VBUS	insert		AXP_BOOT_SOURCE_VBUS_USB
	charge		BIT3		charge to 3.3v		AXP_BOOT_SOURCE_CHARGER
	battery		BIT4		battary in		AXP_BOOT_SOURCE_BATTERY
*/
int bmu_axp2202_get_poweron_source(void)
{
	uchar reg_value;
	if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_PWRON_STATUS, &reg_value)) {
		return -1;
	}
	switch (reg_value) {
	case (1 << AXP_BOOT_SOURCE_BUTTON): return AXP_BOOT_SOURCE_BUTTON;
	case (1 << AXP_BOOT_SOURCE_IRQ_LOW): return AXP_BOOT_SOURCE_IRQ_LOW;
	case (1 << AXP_BOOT_SOURCE_VBUS_USB): {
		if (bmu_get_battery_probe() > -1) {
			return AXP_BOOT_SOURCE_CHARGER;
		} else {
			return AXP_BOOT_SOURCE_VBUS_USB;
		}
	}
	case (1 << AXP_BOOT_SOURCE_CHARGER): return AXP_BOOT_SOURCE_CHARGER;
	case (1 << AXP_BOOT_SOURCE_BATTERY): return AXP_BOOT_SOURCE_BATTERY;
	default: return -1;
	}

}

int bmu_axp2202_set_coulombmeter_onoff(int onoff)
{
	u8 reg_value;

	if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_FUEL_GAUGE_CTL,
			  &reg_value)) {
		return -1;
	}
	if (!onoff)
		reg_value &= ~(0x01 << 2);
	else
		reg_value |= (0x01 << 2);

	if (pmic_bus_write(AXP2202_RUNTIME_ADDR, AXP2202_FUEL_GAUGE_CTL,
			   reg_value)) {
		return -1;
	}
	return 0;
}

int bmu_axp2202_get_axp_bus_exist(void)
{
	u8 reg_value;
	if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_COMM_STATUS0, &reg_value)) {
		return -1;
	}
	/*bit1: 0: vbus not power,  1: power good*/
	if (reg_value & 0x20) {
		return AXP_VBUS_EXIST;
	}
	return 0;
}

int bmu_axp2202_get_battery_vol(void)
{
	u8 reg_value_h = 0, reg_value_l = 0;
	int i, vtemp[3];

	for (i = 0; i < 3; i++) {
		if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_BAT_AVERVOL_H6,
				  &reg_value_h)) {
			return -1;
		}
		if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_BAT_AVERVOL_L8,
				  &reg_value_l)) {
			return -1;
		}
		/*step 1mv*/
		vtemp[i] = ((reg_value_h & 0x3F) << 8) | reg_value_l;
	}
	if (vtemp[0] > vtemp[1]) {
		vtemp[0] = vtemp[0] ^ vtemp[1];
		vtemp[1] = vtemp[0] ^ vtemp[1];
		vtemp[0] = vtemp[0] ^ vtemp[1];
	}
	if (vtemp[1] > vtemp[2]) {
		vtemp[1] = vtemp[2] ^ vtemp[1];
		vtemp[2] = vtemp[2] ^ vtemp[1];
		vtemp[1] = vtemp[2] ^ vtemp[1];
	}
	if (vtemp[0] > vtemp[1]) {
		vtemp[0] = vtemp[0] ^ vtemp[1];
		vtemp[1] = vtemp[0] ^ vtemp[1];
		vtemp[0] = vtemp[0] ^ vtemp[1];
	}
	return vtemp[1];
}

int bmu_axp2202_battery_check(int ratio)
{
	int bat_vol, dcin_exist;
	u8 reg_value;

	bat_vol = bmu_axp2202_get_battery_vol();
	dcin_exist = bmu_axp2202_get_axp_bus_exist();

	if (!ratio) {
		if ((dcin_exist != AXP_VBUS_EXIST) && (bat_vol > 3700)) {
			pmic_bus_setbits(AXP2202_RUNTIME_ADDR, AXP2202_RESET_CFG, BIT(2));
			pmic_bus_clrbits(AXP2202_RUNTIME_ADDR, AXP2202_RESET_CFG, BIT(2));
			tick_printf("%s only battery reset gauge: soc = 0\n", __func__);
			return -1;
		}
		if (dcin_exist == AXP_VBUS_EXIST) {
			pmic_bus_clrbits(AXP2202_RUNTIME_ADDR, AXP2202_MODULE_EN, BIT(1));
			mdelay(500);
			pmic_bus_setbits(AXP2202_RUNTIME_ADDR, AXP2202_RESET_CFG, BIT(2));
			pmic_bus_clrbits(AXP2202_RUNTIME_ADDR, AXP2202_RESET_CFG, BIT(2));
			mdelay(500);
			pmic_bus_setbits(AXP2202_RUNTIME_ADDR, AXP2202_MODULE_EN, BIT(1));
			tick_printf("%s adapt reset gauge: soc = 0\n", __func__);
			pmic_bus_write(AXP2202_RUNTIME_ADDR, AXP2202_CURVE_CHECK, 0x81);
			pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_CURVE_CHECK, &reg_value);
			tick_printf("AXP2202_CURVE_CHECK:%x\n\n", reg_value);
			return -1;
		}
	}

	tick_printf("battery_check pass\n");
	return 0;
}


int bmu_axp2202_get_battery_capacity(void)
{
	u8 reg_value;
	int check;
	static int check_count = 1;

	if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_BAT_PERCEN_CAL,
			  &reg_value)) {
		return -1;
	}

	if (check_count == 1) {
		check = bmu_axp2202_battery_check(reg_value);
		if (check != 0) {
			if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_BAT_PERCEN_CAL,
					  &reg_value)) {
				return -1;
			}
		}
		check_count = 0;
	}

	return reg_value;
}

int bmu_axp2202_get_battery_probe(void)
{
	u8 reg_value;

	if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_COMM_STATUS0, &reg_value)) {
		return -1;
	}

	if (reg_value & 0x08)
		return 1;

	return -1;
}

int bmu_axp2202_set_vbus_current_limit(int current)
{
	u8 reg_value;
	if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_VBUS_CUR_SET, &reg_value)) {
		return -1;
	}
	reg_value &= 0xc0;
	if ((current >= 100) && (current <= 3250))
		reg_value |= (current - 100) / 50;
	else if (current < 100)
		reg_value |= 0x00;
	else if (current > 3250)
		reg_value |= 0x3f;

	tick_printf("Input current:%d mA\n", current);
	if (pmic_bus_write(AXP2202_RUNTIME_ADDR, AXP2202_VBUS_CUR_SET, reg_value)) {
		return -1;
	}
	return 0;
}

int bmu_axp2202_get_vbus_current_limit(void)
{
	uchar reg_value;
	int current;
	if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_VBUS_CUR_SET, &reg_value)) {
		return -1;
	}
	reg_value &= 0x3f;
	current =  (reg_value * 50) + 100;
	printf("limit to %d mA \n", current);
	return current;
}
int bmu_axp2202_set_charge_current_limit(int current)
{
	u8 reg_value;
	int step;

	if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_CHARGE1, &reg_value)) {
		return -1;
	}
	reg_value &= ~0x3f;
	if (current > 3072) {
		current = 3072;
	}
	step = (current - 64) / 64;

	reg_value |= (step & 0x3f);
	if (pmic_bus_write(AXP2202_RUNTIME_ADDR, AXP2202_CHARGE1, reg_value)) {
		return -1;
	}

	return 0;
}

unsigned char bmu_axp2202_get_reg_value(unsigned char reg_addr)
{
	u8 reg_value;
	if (pmic_bus_read(AXP2202_RUNTIME_ADDR, reg_addr, &reg_value)) {
		return -1;
	}
	return reg_value;
}

unsigned char bmu_axp2202_set_reg_value(unsigned char reg_addr, unsigned char reg_value)
{
	unsigned char reg;
	if (pmic_bus_write(AXP2202_RUNTIME_ADDR, reg_addr, reg_value)) {
		return -1;
	}
	if (pmic_bus_read(AXP2202_RUNTIME_ADDR, reg_addr, &reg)) {
		return -1;
	}
	return reg;
}

int bmu_axp2202_set_ntc_onff(int onoff)
{
	unsigned char reg_value;
	if (!onoff) {
		if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_ADC_CH_EN0, &reg_value))
			return -1;

		reg_value &= ~(1 << 1);
		if (pmic_bus_write(AXP2202_RUNTIME_ADDR, AXP2202_ADC_CH_EN0, reg_value))
			return -1;

		if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_TS_CFG, &reg_value))
			return -1;

		reg_value |= (1 << 4);
		reg_value &= ~(3 << 2);
		if (pmic_bus_write(AXP2202_RUNTIME_ADDR, AXP2202_TS_CFG, reg_value))
			return -1;
	} else {
		if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_ADC_CH_EN0, &reg_value))
			return -1;

		reg_value |= (1 << 1);
		if (pmic_bus_write(AXP2202_RUNTIME_ADDR, AXP2202_ADC_CH_EN0, reg_value))
			return -1;

		if (pmic_bus_read(AXP2202_RUNTIME_ADDR, AXP2202_TS_CFG, &reg_value))
			return -1;

		reg_value &= ~(1 << 4);
		reg_value |= (3 << 2);
		if (pmic_bus_write(AXP2202_RUNTIME_ADDR, AXP2202_TS_CFG, reg_value))
			return -1;

	}

	return 0;
}

U_BOOT_AXP_BMU_INIT(bmu_axp2202) = {
	.bmu_name		  = "bmu_axp2202",
	.probe			  = bmu_axp2202_probe,
	.set_power_off		  = bmu_axp2202_set_power_off,
	.get_poweron_source       = bmu_axp2202_get_poweron_source,
	.get_axp_bus_exist	= bmu_axp2202_get_axp_bus_exist,
	.set_coulombmeter_onoff   = bmu_axp2202_set_coulombmeter_onoff,
	.get_battery_vol	  = bmu_axp2202_get_battery_vol,
	.get_battery_capacity     = bmu_axp2202_get_battery_capacity,
	.get_battery_probe	= bmu_axp2202_get_battery_probe,
	.set_vbus_current_limit   = bmu_axp2202_set_vbus_current_limit,
	.get_vbus_current_limit   = bmu_axp2202_get_vbus_current_limit,
	.set_charge_current_limit = bmu_axp2202_set_charge_current_limit,
	.get_reg_value	   = bmu_axp2202_get_reg_value,
	.set_reg_value	   = bmu_axp2202_set_reg_value,
	.reset_capacity    = bmu_axp2202_reset_capacity,
	.set_ntc_onoff     = bmu_axp2202_set_ntc_onff,
};
