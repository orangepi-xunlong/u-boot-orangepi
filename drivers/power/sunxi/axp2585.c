/*
 * Copyright (C) 2016 Allwinner.
 * wangwei <wangwei@allwinnertech.com>
 *
 * SUNXI AXP1506  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */


#include <common.h>
#include <power/sunxi/axp858_reg.h>
#include <power/sunxi/axp2585_reg.h>
#include <power/sunxi/axp.h>
#include <power/sunxi/pmu.h>

extern int axp2585_set_supply_status(int vol_name, int vol_value, int onoff);
extern int axp2585_set_supply_status_byname(char *vol_name, int vol_value, int onoff);
extern int axp2585_probe_supply_status(int vol_name, int vol_value, int onoff);
extern int axp2585_probe_supply_status_byname(char *vol_name);
extern int script_parser_fetch(char *main_name, char *sub_name, int value[], int count);

int axp2585_boost_set(void)
{
	u8    value;
	int   pmu_boost_en;
	int   pmu_boost_cur_limit;
	int   pmu_boost_vol_limit;
	int   pmu_boost_vol_hold;
	u8    tmp[3];
	script_parser_fetch(PMU_SCRIPT_NAME, "pmu_boost_en", &pmu_boost_en, 1);
	if (axp_i2c_read(AXP2585_ADDR, PMU_BOOST_EN, &value)) {
		return -1;
	}
	if (pmu_boost_en) {
		value |= 0x80;
	} else {
		value &= 0x7f;
	}
	axp_i2c_write(AXP2585_ADDR, PMU_BOOST_EN, value);
	script_parser_fetch(PMU_SCRIPT_NAME, "pmu_boost_cur_limit", &pmu_boost_cur_limit, 1);
	if (pmu_boost_cur_limit <= 500) {
		tmp[0] = 0x00;
	} else if (pmu_boost_cur_limit <= 900) {
		tmp[0] = 0x01;
	} else if (pmu_boost_cur_limit <= 1500) {
		tmp[0] = 0x02;
	} else {
		tmp[0] = 0x03;
	}
	script_parser_fetch(PMU_SCRIPT_NAME, "pmu_boost_vol_hold", &pmu_boost_vol_hold, 1);
	if (pmu_boost_vol_hold <= 2400) {
		tmp[1] = 0x00;
	} else if (pmu_boost_vol_hold <= 2600) {
		tmp[1] = 0x01;
	} else if (pmu_boost_vol_hold <= 2800) {
		tmp[1] = 0x02;
	} else {
		tmp[1] = 0x03;
	}
	script_parser_fetch(PMU_SCRIPT_NAME, "pmu_boost_vol_limit", &pmu_boost_vol_limit, 1);
	if (pmu_boost_vol_limit <= 4500) {
		tmp[2] = 0x0;
	} else if (pmu_boost_vol_limit <= 5510) {
		tmp[2] = (pmu_boost_vol_limit - 4500)/64;
	} else {
		tmp[2] = 0xf;
	}
	value = tmp[0] | (tmp[1]<<2) | (tmp[2]<<4);
	axp_i2c_write(AXP2585_ADDR, PMU_BOOST_CTL, value);
	return 0;

}
int axp2585_probe(void)
{
	u8    pmu_type;
	int    pmu_on_ctl;
	u8    value;
	axp_i2c_config(SUNXI_AXP_2585, AXP2585_ADDR);
	if (axp_i2c_read(AXP2585_ADDR, PMU_IC_TYPE, &pmu_type)) {
		tick_printf("axp2585 read error\n");
		return -1;
	}
	tick_printf("pmu_type = %x\n", pmu_type);
	pmu_type &= 0xCF;
	if (pmu_type == 0x46) {
		/* pmu type AXP2585 */
		tick_printf("BMU: AXP2585\n");
		script_parser_fetch(PMU_SCRIPT_NAME, "pmu_on_ctl", &pmu_on_ctl, 1);
		tick_printf("pmu_on_ctl:%x\n", pmu_on_ctl);
		if (axp_i2c_read(AXP2585_ADDR, PWR_ON_CTL, &value)) {
			pr_msg("axp2585 read error\n");
			return -1;
		}
		value &= 0x0F;
		value |= pmu_on_ctl << 4;
		axp_i2c_write(AXP2585_ADDR, PWR_ON_CTL, value);
		axp2585_boost_set();
		if (axp_i2c_read(AXP2585_ADDR, PMU_BATFET_CTL, &value)) {
			return -1;
		}
		value &= 0x7f;
		if (axp_i2c_write(AXP2585_ADDR, PMU_BATFET_CTL, value)) {
			return -1;
		}
#if 0
		axp_i2c_write(AXP2585_ADDR, PMU_REG_EXTENSION_EN, 0x06);
		axp_i2c_write(AXP2585_ADDR, PMU_REG_LOCK, 0x04);
		axp_i2c_write(AXP2585_ADDR, PMU_ADDR_EXTENSION, 0x01);
		if (axp_i2c_read(AXP2585_ADDR, 0x03, &value)) {
			pr_msg("axp2585 read error\n");
			return -1;
		}
		value &= 0xf7;
		axp_i2c_write(AXP2585_ADDR, 0x03, value);
		axp_i2c_write(AXP2585_ADDR, 0xff, 0x00);
#endif
		return 0;
	}
	return -1;
}

int axp2585_set_coulombmeter_onoff(int onoff)
{
	return 0;
}

int axp2585_set_charge_control(void)
{
	return 0;
}

int axp2585_probe_battery_exist(void)
{
	u8 reg_value;

	if (axp_i2c_read(AXP2585_ADDR, PMU_BAT_STATUS, &reg_value)) {
		return -1;
	}
	//bit2 -- battery detect result
	if ((reg_value & (1 << 3))) {
		return 1;
	}
	return 0;
}

int axp2585_probe_battery_ratio(void)
{
	u8 reg_value;

	if(axp_i2c_read(AXP2585_ADDR, PMU_BAT_PERCENTAGE, &reg_value))
	{
		return -1;
	}

	//bit7-- 1:valid 0:invalid
	if(reg_value&(1<<7))
	{
		reg_value = reg_value & 0x7f;
	}
	else
	{
		reg_value = 0;
	}

	return reg_value;
}

int axp2585_probe_power_status(void)
{
	u8 reg_value;

	if (axp_i2c_read(AXP2585_ADDR, PMU_CHG_STATUS, &reg_value)) {
		return -1;
	}
	/*bit1: 0: vbus not power,  1: power good*/
	if (reg_value & 0x2) {
		return AXP_VBUS_EXIST;
	}
	return 0;

}

int axp2585_probe_battery_vol(void)
{
	u8 reg_value_h = 0,reg_value_l = 0;
	int tmp_value = 0;
	int bat_vol;

	if(axp_i2c_read(AXP2585_ADDR, PMU_BAT_VOL_H, &reg_value_h))
	{
		return -1;
	}
	if(axp_i2c_read(AXP2585_ADDR, PMU_BAT_VOL_L, &reg_value_l))
	{
		return -1;
	}

	tmp_value = (reg_value_h<<4) | reg_value_l;
	bat_vol = tmp_value * 12/10;

	return bat_vol;
}

int axp2585_probe_key(void)
{
    return 0;
}

int axp2585_probe_pre_sys_mode(void)
{
	return -1;
}

int axp2585_set_next_sys_mode(int data)
{
	return -1;
}

int axp2585_probe_this_poweron_cause(void)
{
	uchar   reg_value;
	if (axp_i2c_read(AXP858_ADDR, PMU_DATA_BUFFER0, &reg_value)) {
		return -1;
	}
	if (reg_value == PMU_PRE_BOOT_MODE) {
		axp_i2c_write(AXP858_ADDR, PMU_DATA_BUFFER0, PMU_PRE_SYS_MODE);
		return AXP_POWER_ON_BY_POWER_KEY;
	}
	if (axp_i2c_read(AXP2585_ADDR, PMU_POWER_ON_STATUS, &reg_value))
	{
		return -1;
	}
	tick_printf("poweron cause %x\n", reg_value);
	/*bit7 : vbus, bit6: battery inster, bit5: battery charge to normal*/
	if(reg_value&(1<<7))
	{
		reg_value &= 0x80;
		axp_i2c_write(AXP2585_ADDR, PMU_POWER_ON_STATUS, reg_value);
		return AXP_POWER_ON_BY_POWER_TRIGGER;
	}
	else
	{
		/*need to check supply pmu*/
		return AXP_POWER_ON_BY_POWER_KEY;
	}

	return reg_value & 0x01;
}

int axp2585_set_power_off(void)
{
	u8 reg_value;
	if (axp_i2c_read(AXP2585_ADDR, PMU_POWER_ON_STATUS, &reg_value)) {
		return -1;
	}
	axp_i2c_write(AXP2585_ADDR, PMU_POWER_ON_STATUS, reg_value);
	if (axp_i2c_read(AXP2585_ADDR, PMU_CHG_STATUS, &reg_value)) {
		return -1;
	}
	if (reg_value & 0x02) {
		return 0;
	}
	if (axp_i2c_read(AXP2585_ADDR, PMU_BATFET_CTL, &reg_value)) {
		return -1;
	}
	reg_value &= 0x7f;
	if (axp_i2c_write(AXP2585_ADDR, PMU_BATFET_CTL, reg_value)) {
		return -1;
	}
	tick_printf("before reg[10]:%x\n", reg_value);
	mdelay(50);
	reg_value |= 1 << 7;
	if (axp_i2c_write(AXP2585_ADDR, PMU_BATFET_CTL, reg_value)) {
		return -1;
	}
	return 0;
}

int axp2585_set_power_onoff_vol(int set_vol, int stage)
{
	return 0;
}
int axp2585_set_charge_current(int current)
{
	u8 reg_value;
	u8 temp;
	if (axp_i2c_read(AXP2585_ADDR, PMU_CHG_CUR_LIMIT, &reg_value)) {
		return -1;
	}
	if (current > 3072) {
		temp = 0x3F;
	} else {
		temp = current / 64;
	}
	reg_value &= 0xB0;
	reg_value |= temp;
	tick_printf("Charge current:%d ma\n", current);
	if (axp_i2c_write(AXP2585_ADDR, PMU_CHG_CUR_LIMIT, reg_value)) {
		return -1;
	}
	return 0;
}
int axp2585_probe_charge_current(void)
{
	return 0;
}
int axp2585_set_vbus_cur_limit(int current)
{
	u8 reg_value;
	u8 temp;
	if (axp_i2c_read(AXP2585_ADDR, PMU_BATFET_CTL, &reg_value)) {
		return -1;
	}
	if (current) {
		if (current > 3250) {
			temp = 0x3F;
		} else if (current >= 100) {
			temp = (current - 100)/50;
		} else  {
			temp = 0x00;
		}
	} else {
		/*default was 2500ma*/
		temp = 0x30;
	}
	reg_value &= 0xB0;
	reg_value |= temp;
	tick_printf("Input current:%d ma\n", current);
	if (axp_i2c_write(AXP2585_ADDR, PMU_BATFET_CTL, reg_value)) {
		return -1;
	}
	return 0;
}

int axp2585_probe_vbus_cur_limit(void)
{
	return  0;
}
int axp2585_set_vbus_vol_limit(int vol)
{
	return 0;
}
int axp2585_probe_int_pending(uchar *addr)
{

	return 0;
}

int axp2585_probe_int_enable(uchar *addr)
{

	return 0;

}

int axp2585_set_int_enable(uchar *addr)
{

	return 0;
}

sunxi_axp_module_init("axp2585", SUNXI_AXP_2585);




