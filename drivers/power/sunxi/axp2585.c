/*
 * Copyright (C) 2016 Allwinner.
 * wangwei <wangwei@allwinnertech.com>
 *
 * SUNXI AXP1506  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */


#include <common.h>
#include <power/sunxi/axp2585_reg.h>
#include <power/sunxi/axp.h>
#include <power/sunxi/pmu.h>

extern int axp2585_set_supply_status(int vol_name, int vol_value, int onoff);
extern int axp2585_set_supply_status_byname(char *vol_name, int vol_value, int onoff);
extern int axp2585_probe_supply_status(int vol_name, int vol_value, int onoff);
extern int axp2585_probe_supply_status_byname(char *vol_name);


int axp2585_probe(void)
{
	u8    pmu_type;

	if(axp_i2c_write(AXP2585_ADDR,PMU_ADDR_EXTENSION, 0x40))
	{
		printf("axp2585 write 0xff error\n");
		return -1;
	}
	if(axp_i2c_read(AXP2585_ADDR,PMU_IC_TYPE, &pmu_type))
	{
		printf("axp2585 read error\n");
		return -1;
	}
	printf("pmu_type = %x\n",pmu_type);
	pmu_type &= 0xCF;
	if(pmu_type == 0x43)
	{
		/* pmu type AXP2585 */
		tick_printf("PMU: AXP2585\n");
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

	if(axp_i2c_read(AXP2585_ADDR, PMU_POWER_SOURCE_STATUS, &reg_value))
	{
		return -1;
	}
	//bit2 -- battery detect result
	if((reg_value & (1<<2)))
	{
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

	if(axp_i2c_read(AXP2585_ADDR, PMU_VBUS_CHARG_STATUS, &reg_value))
	{
		return -1;
	}
	/*bit7-5: 000: vbus not existvbus exist, other: exist*/
	if(reg_value & 0xE0)
	{
		return AXP_VBUS_EXIST;
	}
	return -1;

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
	u8  reg_value;
	if(axp_i2c_read(AXP2585_ADDR, PMU_DATA_BUFFER0, &reg_value))
	{
		return -1;
	}

	return reg_value;
}

int axp2585_set_next_sys_mode(int data)
{
	if(axp_i2c_write(AXP2585_ADDR, PMU_DATA_BUFFER0, (u8)data))
	{
		return -1;
	}
	return 0;
}

int axp2585_probe_this_poweron_cause(void)
{
	uchar   reg_value;

	if(axp_i2c_read(AXP2585_ADDR, PMU_POWER_ON_STATUS, &reg_value))
	{
		return -1;
	}
	/*bit7 : vbus, bit6: battery inster, bit5: battery charge to normal*/
	if(reg_value&(1<<7))
	{
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
	return 0;
}

int axp2585_set_power_onoff_vol(int set_vol, int stage)
{
	return 0;
}
int axp2585_set_charge_current(int current)
{
	return 0;
}
int axp2585_probe_charge_current(void)
{
	return 0;
}
int axp2585_set_vbus_cur_limit(int current)
{
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




