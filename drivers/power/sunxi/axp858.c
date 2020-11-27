/*
 * Copyright (C) 2016 Allwinner.
 * wangwei <wangwei@allwinnertech.com>
 *
 * SUNXI AXP858  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */


#include <common.h>
#include <power/sunxi/axp858_reg.h>
#include <power/sunxi/axp.h>
#include <power/sunxi/pmu.h>

extern int axp858_set_supply_status(int vol_name, int vol_value, int onoff);
extern int axp858_set_supply_status_byname(char *vol_name, int vol_value, int onoff);
extern int axp858_probe_supply_status(int vol_name, int vol_value, int onoff);
extern int axp858_probe_supply_status_byname(char *vol_name);
static int axp858_ap_reset_enable(void);

int axp858_probe(void)
{
	u8    pmu_type;

	axp_i2c_config(SUNXI_AXP_858, AXP858_ADDR);
	if (axp_i2c_read(AXP858_ADDR, PMU_IC_TYPE, &pmu_type)) {
		pr_msg("axp read error\n");
		return -1;
	}

	pmu_type &= 0xCF;
	if (pmu_type == 0x44) {
		/* pmu type AXP858 */
		tick_printf("PMU: AXP858\n");
		axp858_ap_reset_enable();
		return 0;
	}

	return -1;
}

int axp858_set_coulombmeter_onoff(int onoff)
{
	return -1;
}

int axp858_set_charge_control(void)
{
	return -1;
}

int axp858_probe_battery_ratio(void)
{
	return -1;
}

int axp858_probe_power_status(void)
{
	return -1;
}

int axp858_probe_battery_exist(void)
{
	return -1;
}

int axp858_probe_battery_vol(void)
{
	return -1;
}

int axp858_probe_key(void)
{
	u8  reg_value;

	if (axp_i2c_read(AXP858_ADDR, PMU_IRQ_STATU2, &reg_value)) {
		return -1;
	}
	/*bit0:POKLIRQ, bit1:POKSIRQ*/
	reg_value &= (0x03);
	if (reg_value) {
		if (axp_i2c_write(AXP858_ADDR, PMU_IRQ_STATU2, reg_value)) {
			return -1;
		}
	}

	return (reg_value)&3;
}

int axp858_probe_pre_sys_mode(void)
{
	u8  reg_value;
	if (axp_i2c_read(AXP858_ADDR, PMU_DATA_BUFFER0, &reg_value)) {
		return -1;
	}
	return reg_value;
}

int axp858_set_next_sys_mode(int data)
{
	if (axp_i2c_write(AXP858_ADDR, PMU_DATA_BUFFER0, (u8)data)) {
		return -1;
	}
	return 0;
}

int axp858_probe_this_poweron_cause(void)
{
	uchar   reg_value;

	if (axp_i2c_read(AXP858_ADDR, PMU_POWER_ON_SOURCE, &reg_value)) {
		return -1;
	}
	if (reg_value & (0x1 << 2)) {
		return AXP_POWER_ON_BY_POWER_KEY;
	} else {
		/* other source: irq pin, etc, need to check charge pmu*/
		return AXP_POWER_ON_BY_POWER_TRIGGER;
	}
}

int axp858_set_power_off(void)
{
	u8 reg_value;

	if (axp_i2c_read(AXP858_ADDR, PMU_POWER_DISABLE_DOWN, &reg_value)) {
		return -1;
	}
	reg_value |= 1 << 7;
	if (axp_i2c_write(AXP858_ADDR, PMU_POWER_DISABLE_DOWN, reg_value)) {
		return -1;
	}

	return 0;
}

int axp858_set_power_onoff_vol(int set_vol, int stage)
{
	return -1;
}

int axp858_set_charge_current(int current)
{
	return -1;
}

int axp858_probe_charge_current(void)
{
	return -1;
}

int axp858_set_vbus_cur_limit(int current)
{
	return -1;
}

int axp858_probe_vbus_cur_limit(void)
{
	return -1;
}

int axp858_set_vbus_vol_limit(int vol)
{
	return -1;
}

int axp858_probe_int_pending(uchar *addr)
{
	return -1;
}

int axp858_probe_int_enable(uchar *addr)
{
	return -1;
}

int axp858_set_int_enable(uchar *addr)
{
	return -1;
}

int axp858_set_power_reset(void)
{
	u8 reg_value;
	if (axp_i2c_read(AXP858_ADDR, PMU_POWER_DISABLE_DOWN, &reg_value)) {
		return -1;
	}
	reg_value |= 1 << 6;
	if (axp_i2c_write(AXP858_ADDR, PMU_POWER_DISABLE_DOWN, reg_value)) {
		return -1;
	}
	return 0;
}

static int axp858_ap_reset_enable(void)
{
	u8	 reg_value;

	if (axp_i2c_read(AXP858_ADDR, PMU_POWER_DISABLE_DOWN, &reg_value))
		return -1;

	reg_value |= 1 << 4;
	if (axp_i2c_write(AXP858_ADDR, PMU_POWER_DISABLE_DOWN, reg_value))
		return -1;

	return 0;
}


void sunxi_pmu_reset(void)
{
	if (uboot_spare_head.boot_data.work_mode == WORK_MODE_BOOT) {
		pr_msg("restart system by pmu\n");
		mdelay(1000);
		axp858_set_power_reset();
		while (1) {
			;
		}
	}
}


sunxi_axp_module_init("axp858", SUNXI_AXP_858);


