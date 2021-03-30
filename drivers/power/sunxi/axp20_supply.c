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
#include <power/sunxi/axp20_reg.h>
#include <power/sunxi/axp.h>
#include <power/sunxi/pmu.h>

#if 0
static s32  dcdc2_user_set;
#endif
//static s32  dcdc3_user_set;
//static s32  ldo2_user_set;
//static s32  ldo3_user_set;
//static s32  ldo4_user_set;

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
#if 0
static int axp20_set_dc1sw(int onoff)
{
   	return 0;
}
#endif
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
#if 0
static int axp20_set_dc5ldo(int onoff)
{
    return 0;
}
#endif

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
static int axp20_set_dcdc1(int set_vol, int onoff)
{
	//axp209没有DCDC1
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
static int axp20_set_dcdc2(int set_vol, int onoff)
{
 	u32 vol, tmp;
	volatile u32 i;
    u8  reg_addr, value;
	if(set_vol == -1)
	{
		set_vol = 1400;
	}
    //PMU is AXP209
    reg_addr = BOOT_POWER20_DC2OUT_VOL;
    if(axp_i2c_read(AXP20_ADDR, reg_addr, &value))
    {
        return -1;
    }
    tmp     = value & 0x3f;
    vol     = tmp * 25 + 700;
    //如果电压过高，则调低
    while(vol > set_vol)
    {
        tmp -= 1;
        value &= ~0x3f;
        value |= tmp;
        reg_addr = BOOT_POWER20_DC2OUT_VOL;
        if(axp_i2c_write(AXP20_ADDR, reg_addr, value))
        {
            return -1;
        }
        for(i=0;i<2000;i++);
        reg_addr = BOOT_POWER20_DC2OUT_VOL;
        if(axp_i2c_read(AXP20_ADDR, reg_addr, &value))
        {
            return -1;
        }
        tmp     = value & 0x3f;
        vol     = tmp * 25 + 700;
    }
    //如果电压过低，则调高，根据先调低再调高的过程，保证电压会大于等于用户设定电压
    while(vol < set_vol)
    {
        tmp += 1;
        value &= ~0x3f;
        value |= tmp;
        reg_addr = BOOT_POWER20_DC2OUT_VOL;
        if(axp_i2c_write(AXP20_ADDR, reg_addr, value))
        {
            return -1;
        }
        for(i=0;i<2000;i++);
        reg_addr = BOOT_POWER20_DC2OUT_VOL;
        if(axp_i2c_read(AXP20_ADDR, reg_addr, &value))
        {
            return -1;
        }
        tmp     = value & 0x3f;
        vol     = tmp * 25 + 700;
    }
	//tick_printf("after set, dcdc2 =%dmv\n",vol);

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
static int axp20_set_dcdc3(int set_vol, int onoff)
{
	s32 vol_value;
	//s32 ret;
	u8  reg_addr;
	u8  reg_value;


	if(set_vol == -1)
	{
		#if 0
		if(!dcdc3_user_set)
		{
			ret = script_parser_fetch("target", "dcdc3_vol", &vol_value, 1);
			if(ret)
			{
				printf("boot power:unable to find dcdc3 set\n");
				return -1;
			}
			dcdc3_user_set = vol_value;
		}
		vol_value = dcdc3_user_set;
		#endif
		printf("boot power:invalid dcdc3  vol\n");
		return -1;
	}
	else
	{
		vol_value = set_vol;
	}


	if(!vol_value)
	{
		reg_addr = BOOT_POWER20_OUTPUT_CTL;
		if(!axp_i2c_read(AXP20_ADDR, reg_addr, &reg_value))
		{
			reg_value &= ~(1<<1);
			if(axp_i2c_write(AXP20_ADDR, reg_addr, reg_value))
			{
				printf("boot power:unable to close dcdc3\n");
				return -1;
			}
		}
	}
	else
	{
		reg_addr = BOOT_POWER20_DC3OUT_VOL;
		if(!axp_i2c_read(AXP20_ADDR, reg_addr, &reg_value))
		{
			if((vol_value >= 700) && (vol_value <= 3500))
			{
				reg_value &= 0x80;
				reg_value = ((vol_value - 700)/25);
				if(axp_i2c_write(AXP20_ADDR, reg_addr, reg_value))
				{
					printf("boot power:unable to set dcdc3\n");
					return -1;
				}
			}
		}
	}

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
static int axp20_set_dcdc4(int set_vol, int onoff)
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
static int axp20_set_dcdc5(int set_vol, int onoff)
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
#if 0
static int axp20_set_aldo1(int set_vol, int onoff)
{
	return 0;
}
#endif
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
#if 0
static int axp20_set_aldo2(int set_vol, int onoff)
{
	return 0;
}
#endif
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
#if 0
static int axp20_set_aldo3(int set_vol, int onoff)
{
	return 0;
}
#endif
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
static int axp20_set_ldo1(int set_vol, int onoff)
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
static int axp20_set_ldo2(int set_vol, int onoff)
{
	s32 vol_value;
	//s32 ret;
	u8  reg_addr;
	u8  reg_value;

	if(set_vol == -1)
	{
		#if 0
		if(!ldo2_user_set)
		{
			ret = script_parser_fetch("target", "ldo2_vol", &vol_value, 1);
				if(ret)
				{
					printf("boot power:unable to find ldo2 set\n");

						return -1;
				}
			ldo2_user_set = vol_value;
		}
		vol_value = ldo2_user_set;
		#endif
		printf("boot power:  invalid ldo2 vol\n");
		return -1;
	}
	else
	{
		vol_value = set_vol;
	}
    if(!vol_value)
    {
        reg_addr = BOOT_POWER20_OUTPUT_CTL;
        if(!axp_i2c_read(AXP20_ADDR, reg_addr, &reg_value))
        {
            reg_value &= ~(1<<2);
            if(axp_i2c_write(AXP20_ADDR, reg_addr, reg_value))
            {
                printf("boot power:unable to set ldo2\n");
                    return -1;
            }
        }
    }
    else
    {
        reg_addr  = BOOT_POWER20_LDO24OUT_VOL;
        if(!axp_i2c_read(AXP20_ADDR, reg_addr, &reg_value))
        {
            if((vol_value >= 1800) && (vol_value <= 3300))
            {
                reg_value &= 0x0f;
                reg_value |= (((vol_value - 1800)/100) << 4);
                if(axp_i2c_write(AXP20_ADDR, reg_addr, reg_value))
                {
                    printf("boot power:unable to set ldo2\n");
                        return -1;
                }
            }
        }
    }
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
static int axp20_set_ldo3(int set_vol, int onoff)
{
    s32 vol_value;
	//s32 ret;
	u8  reg_addr;
	u8  reg_value;

	if(set_vol == -1)
	{
		#if 0
		if(!ldo3_user_set)
		{
			ret = script_parser_fetch("target", "ldo3_vol", &vol_value, 1);
				if(ret)
				{
					printf("boot power:unable to find ldo3 set\n");
						return -1;
				}
			ldo3_user_set = vol_value;
		}
		vol_value = ldo3_user_set;
		#endif
		printf("boot power:invalid ldo3 vol\n");
		return -1;
	}
	else
	{
		vol_value = set_vol;
	}
    if(!vol_value)
    {
        reg_addr = BOOT_POWER20_OUTPUT_CTL;
        if(!axp_i2c_read(AXP20_ADDR, reg_addr, &reg_value))
        {
            reg_value &= ~(1<<6);
            if(axp_i2c_write(AXP20_ADDR, reg_addr, reg_value))
            {
                printf("boot power:unable to set ldo2\n");
                    return -1;
            }
        }
    }
    else
    {
        reg_addr  = BOOT_POWER20_LDO3OUT_VOL;
        if(!axp_i2c_read(AXP20_ADDR, reg_addr, &reg_value))
        {
            if((vol_value >= 700) && (vol_value <= 3500))
            {
                reg_value &= 0x80;
                reg_value |= ((vol_value - 700)/25);
                if(axp_i2c_write(AXP20_ADDR, reg_addr, reg_value))
                {
                    printf("boot power:unable to set ldo3\n");
                        return -1;
                }
            }
        }
    }

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
static int axp20_set_ldo4(int set_vol, int onoff)
{
    s32 vol_value;
	//s32 ret;
	u8  reg_addr;
	u8  reg_value;

	if(-1 == set_vol)
	{
		#if 0
		if(!ldo4_user_set)
		{
			ret = script_parser_fetch("target", "ldo4_vol", &vol_value, 1);
				if(ret)
				{
					printf("boot power:unable to find ldo4 set\n");
						return -1;
				}
			ldo4_user_set = vol_value;
		}
		vol_value = ldo4_user_set;
		#endif
		printf("boot power:invalid ldo4 vol\n");
		return -1;
	}
	else
	{
		vol_value = set_vol;
	}
    if(!vol_value)
    {
        reg_addr = BOOT_POWER20_OUTPUT_CTL;
            if(!axp_i2c_read(AXP20_ADDR, reg_addr, &reg_value))
            {
                reg_value &= ~(1<<3);
                    if(axp_i2c_write(AXP20_ADDR, reg_addr, reg_value))
                    {
                        printf("boot power:unable to set ldo2\n");
                            return -1;
                    }
            }
    }
    else
    {
        reg_addr  = BOOT_POWER20_LDO24OUT_VOL;
            if(!axp_i2c_read(AXP20_ADDR, reg_addr, &reg_value))
            {
                reg_value &= 0xf0;
                    /*************************************************************************************
                      0    1300     2000    2500     2700   2800    3000   3300  max


                     *************************************************************************************/
                    if(vol_value < 1300)
                    {
                        reg_value |= 0x00;
                    }
                    else if(vol_value <= 2000)
                    {
                        reg_value |= (vol_value - 1200)/100;
                    }
                    else if(vol_value < 2700)
                    {
                        reg_value |= 0x09;
                    }
                    else if(vol_value <= 2800)
                    {
                        reg_value |= ((vol_value - 2700)/100) + 0x0a;
                    }
                    else
                    {
                        if(vol_value < 3000)
                        {
                            vol_value = 3000;
                        }
                        else if(vol_value > 3300)
                        {
                            vol_value = 3300;
                        }
                        reg_value |= ((vol_value - 3000)/100) + 0x0c;
                    }
                if(axp_i2c_write(AXP20_ADDR, reg_addr, reg_value))
                {
                    printf("boot power:unable to set ldo4\n");
                        return -1;
                }
            }
    }
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
#if 0
static int axp20_set_eldo1(int set_vol, int onoff)
{
	return 0;
}
#endif
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
#if 0
static int axp20_set_eldo2(int set_vol, int onoff)
{
	return 0;
}
#endif
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
#if 0
static int axp20_set_eldo3(int set_vol, int onoff)
{
	return 0;
}
#endif
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
static int axp20_set_gpio0ldo(int set_vol, int onoff)
{
	u8 reg_value;
	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_GPIO0_CTL, &reg_value))
	{
		return -1;
	}
	if(onoff == 0)
	{
		reg_value &= ~(7 << 0);
	}
	else
	{
		reg_value &= ~(7 << 0);
		reg_value |=  (0x01 << 0);
	}
	if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_GPIO0_CTL, reg_value))
	{
		printf("sunxi pmu error : unable to onoff gpio0ldo\n");

		return -1;
	}

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
static int axp20_set_gpio1ldo(int set_vol, int onoff)
{
	u8 reg_value;
	if(axp_i2c_read(AXP20_ADDR, BOOT_POWER20_GPIO1_CTL, &reg_value))
	{
		return -1;
	}
	if(onoff == 0)
	{
		reg_value &= ~(7 << 0);
	}
	else
	{
		reg_value &= ~(7 << 0);
		reg_value |=  (0x01 << 0);
	}
	if(axp_i2c_write(AXP20_ADDR, BOOT_POWER20_GPIO1_CTL, reg_value))
	{
		printf("sunxi pmu error : unable to onoff gpio1ldo\n");

		return -1;
	}

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static int axp20_set_dcdc_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case 1:
			return axp20_set_dcdc1(vol_value, onoff);
		case 2:
			return axp20_set_dcdc2(vol_value, onoff);
		case 3:
			return axp20_set_dcdc3(vol_value, onoff);
		case 4:
			return axp20_set_dcdc4(vol_value, onoff);
		case 5:
			return axp20_set_dcdc5(vol_value, onoff);
	}

	return -1;
}

static int axp20_set_ldo_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case 1:
			return axp20_set_ldo1(vol_value, onoff);
		case 2:
			return axp20_set_ldo2(vol_value, onoff);
		case 3:
			return axp20_set_ldo3(vol_value, onoff);
		case 4:
			return axp20_set_ldo4(vol_value, onoff);
	}

	return -1;
}
static int axp20_set_aldo_output(int sppply_index, int vol_value, int onoff)
{
	return 0;
}

static int axp20_set_dldo_output(int sppply_index, int vol_value, int onoff)
{
	return 0;
}

static int axp20_set_eldo_output(int sppply_index, int vol_value, int onoff)
{
	return 0;
}

static int axp20_set_gpioldo_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case 0:
			return axp20_set_gpio0ldo(vol_value, onoff);
		case 1:
			return axp20_set_gpio1ldo(vol_value, onoff);
	}
	return -1;
}

static int axp20_set_misc_output(int sppply_index, int vol_value, int onoff)
{
	return 0;
}


int axp20_set_supply_status(int vol_name, int vol_value, int onoff)
{
	int supply_type;
	int sppply_index;
	supply_type  = vol_name & 0xffff0000;
	sppply_index = vol_name & 0x0000ffff;
	switch(supply_type)
	{
		case PMU_SUPPLY_DCDC_TYPE:
			return axp20_set_dcdc_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_ALDO_TYPE:
			return axp20_set_aldo_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_ELDO_TYPE:
			return axp20_set_eldo_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_DLDO_TYPE:
			return axp20_set_dldo_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_GPIOLDO_TYPE:
			return axp20_set_gpioldo_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_MISC_TYPE:
			return axp20_set_misc_output(vol_name, vol_value, onoff);

			break;
		default:
			return -1;
	}
	return 0;
}

int axp20_set_supply_status_byname(char *vol_name, int vol_value, int onoff)
{
	int sppply_index;

	if(!strncmp(vol_name, "dcdc", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp20_set_dcdc_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "ldo", 3))
	{
		sppply_index = simple_strtoul(vol_name + 3, NULL, 10);

		return axp20_set_ldo_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "aldo", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp20_set_aldo_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "eldo", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp20_set_eldo_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "dldo", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp20_set_dldo_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "gpio", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp20_set_gpioldo_output(sppply_index, vol_value, onoff);
	}

	return 0;
}

int axp20_probe_supply_status(int vol_name, int vol_value, int onoff)
{
	return 0;
}

int axp20_probe_supply_status_byname(char *vol_name)
{
	return 0;
}


