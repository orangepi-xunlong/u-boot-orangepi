/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <power/sunxi/axp806_reg.h>
#include <power/sunxi/axp.h>
#include <power/sunxi/pmu.h>

extern int axp806_set_supply_status(int vol_name, int vol_value, int onoff);
extern int axp806_set_supply_status_byname(char *vol_name, int vol_value, int onoff);
extern int axp806_probe_supply_status(int vol_name, int vol_value, int onoff);
extern int axp806_probe_supply_status_byname(char *vol_name);


int axp806_probe(void)
{
	u8	  pmu_type;

	axp_i2c_config(SUNXI_AXP_806, AXP806_ADDR);
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_VERSION, &pmu_type))
	{
		printf("axp read error\n");
		return -1;
	}
	pmu_type &= 0xCF;
	if(pmu_type == 0x40)
	{
		/* pmu type AXP806 */
		tick_printf("PMU: AXP806\n");
		return 0;
	}
	return -1;
}

int axp806_set_coulombmeter_onoff(int onoff)
{
	return 0;
}

int axp806_set_charge_control(void)
{
	return 0;
}

int axp806_probe_battery_ratio(void)
{
	return 0;
}

int axp806_probe_power_status(void)
{
	return 0;
}


int axp806_probe_battery_exist(void)
{
	return 0;
}

int axp806_probe_battery_vol(void)
{
	return 0;
}

int axp806_probe_key(void)
{
	return 0;
}

int axp806_probe_pre_sys_mode(void)
{
	return 0;
}

int axp806_set_next_sys_mode(int data)
{
	return 0;
}

int axp806_probe_this_poweron_cause(void)
{
	return 0;
}

int axp806_set_power_off(void)
{
	return 0;
}

int axp806_set_power_onoff_vol(int set_vol, int stage)
{
	return 0;
}

int axp806_set_charge_current(int current)
{
	return 0;
}

int axp806_probe_charge_current(void)
{
	return 0;
}

int axp806_set_vbus_cur_limit(int current)
{
	return 0;
}

int axp806_probe_vbus_cur_limit(void)
{
	return 0;
}

int axp806_set_vbus_vol_limit(int vol)
{
	return 0;
}

int axp806_probe_int_pending(uchar *addr)
{
	int	  i;

	for(i=0;i<2;i++)
	{
		if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_INTSTS1 + i, addr + i))
		{
			return -1;
		}
	}

	for(i=0;i<2;i++)
	{
		if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_INTSTS1 + i, 0xff))
		{
			return -1;
		}
	}

	return 0;
}

int axp806_probe_int_enable(uchar *addr)
{
	int	  i;
	uchar  int_reg = BOOT_POWER806_INTEN1;

	for(i=0;i<2;i++)
	{
		if(axp_i2c_read(AXP806_ADDR, int_reg, addr + i))
		{
			return -1;
		}
		int_reg ++;
	}

	return 0;
}

int axp806_set_int_enable(uchar *addr)
{
	int	  i;
	uchar  int_reg = BOOT_POWER806_INTEN1;

	for(i=0;i<2;i++)
	{
		if(axp_i2c_write(AXP806_ADDR, int_reg, addr[i]))
		{
			return -1;
		}
		int_reg ++;
	}

	return 0;
}


sunxi_axp_module_init("axp806", SUNXI_AXP_806);


