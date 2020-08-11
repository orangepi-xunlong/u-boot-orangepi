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
#include <power/sunxi/axp15_reg.h>
#include <power/sunxi/axp.h>
#include <power/sunxi/pmu.h>

extern int axp15_set_supply_status(int vol_name, int vol_value, int onoff);
extern int axp15_set_supply_status_byname(char *vol_name, int vol_value, int onoff);
extern int axp15_probe_supply_status(int vol_name, int vol_value, int onoff);
extern int axp15_probe_supply_status_byname(char *vol_name);

/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int axp15_probe(void)
{
	u8    pmu_type;
	__msdelay(100);//确保硬件ADC准备好。
	if(axp_i2c_read(AXP15_ADDR,BOOT_POWER15_VERSION, &pmu_type))
	{
		printf("axp152 read error\n");
		return -1;
	}
	printf("pmu_type = %x\n",pmu_type);
	pmu_type &= 0x0f;
	if(pmu_type == 0x05)
	{
		/* pmu type AXP152 */
		tick_printf("PMU: AXP152\n");
		return 0;
	}
	return -1;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int axp15_set_coulombmeter_onoff(int onoff)
{
    return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/

int axp15_set_charge_control(void)
{
	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/

int axp15_probe_battery_exist(void)
{
	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/

int axp15_probe_battery_ratio(void)
{
	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/

int axp15_probe_power_status(void)
{
	return 0;
}
/*
************************************************************************************************************
*
*											  function
*
*	 函数名称：
*
*	 参数列表：
*
*	 返回值  ：
*
*	 说明	 ：
*
*
************************************************************************************************************
*/

int axp15_probe_battery_vol(void)
{
	return 0;
}
/*
************************************************************************************************************
*
*											  function
*
*	 函数名称：
*
*	 参数列表：
*
*	 返回值  ：
*
*	 说明	 ：
*
*
************************************************************************************************************
*/
int axp15_probe_key(void)
{
 	u8  reg_addr, value;
    s32 key_exist;
    key_exist = 0;
    reg_addr = BOOT_POWER15_INTSTS2;
    if(axp_i2c_read(AXP15_ADDR, reg_addr, &value))
    {
        return -1;
    }
    key_exist = (value >> 0) & 0x03;
    if(key_exist)
    {
    	value |= 0x01 << 1;
    	axp_i2c_write(AXP15_ADDR, reg_addr, value);
	}
    return key_exist;
}
/*
************************************************************************************************************
*
*											  function
*
*	 函数名称：
*
*	 参数列表：
*
*	 返回值  ：
*
*	 说明	 ：
*
*
************************************************************************************************************
*/
int axp15_probe_pre_sys_mode(void)
{
	return 0;
}


/*
************************************************************************************************************
*
*											  function
*
*	 函数名称：
*
*	 参数列表：
*
*	 返回值  ：
*
*	 说明	 ：
*
*
************************************************************************************************************
*/

int axp15_set_next_sys_mode(int data)
{
	return 0;
}

int axp15_probe_this_poweron_cause(void)
{
	return 0;
}

int axp15_set_power_off(void)
{
	u8 reg_addr;
    u8  reg_value;
	printf("axp15_set_power_off\n");
	reg_addr = BOOT_POWER15_OFF_CTL;
	if(axp_i2c_read(AXP15_ADDR, reg_addr, &reg_value))
	{
	    printf("axp15_set_power_off read error\n");
    	return -1;
	}
	reg_value |= 1 << 7;
	if(axp_i2c_write(AXP15_ADDR, reg_addr, reg_value))
    {
        return -1;
    }
    printf("axp15_set_power_off failed\n");
	return 0;
}

int axp15_set_power_onoff_vol(int set_vol, int stage)
{
	return 0;
}
int axp15_set_charge_current(int current)
{
	return 0;
}
int axp15_probe_charge_current(void)
{
	return 0;
}
int axp15_set_vbus_cur_limit(int current)
{
	return 0;
}

int axp15_probe_vbus_cur_limit(void)
{
 
	return  0;
}
int axp15_set_vbus_vol_limit(int vol)
{
	return 0;
}
int axp15_probe_int_pending(uchar *addr)
{
	int i;
	for(i = 0;i<3;i++)
	{
		if(axp_i2c_read(AXP15_ADDR,BOOT_POWER15_INTSTS1+i,addr+i))
		{
			printf("can't read the axp15's int_pending!\n");
			return -1;
		}
	}
	for(i=0;i<3;i++)
	{
		if(axp_i2c_write(AXP15_ADDR,BOOT_POWER15_INTSTS1+i,0xff))
		{
			printf("can't write the axp15's int_pending!\n");
			return -1;
		}
	}
	return 0;
}

int axp15_probe_int_enable(uchar *addr)
{
	int i = 0;
	uchar int_reg = BOOT_POWER15_INTEN1;
	for(i=0;i<3;i++)
	{
		if(axp_i2c_read(AXP15_ADDR,int_reg,addr+i))
		{
			printf("can't read the axp15's int_en!\n");
			return -1;
		}
		int_reg++;
	}
	return 0;

}

int axp15_set_int_enable(uchar *addr)
{
	int i = 0;
	uchar int_reg = BOOT_POWER15_INTEN1;

	for(i=0;i<3;i++)
	{
		if(axp_i2c_write(AXP15_ADDR, int_reg, addr[i]))
		{
			printf("can't write int_enable reg into BOOT_POWER15_INTEN1!\n");
			return -1;
		}
		int_reg++;
	}
	return 0;
}

sunxi_axp_module_init("axp15", SUNXI_AXP_15X);




