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
#include <power/sunxi/axp22_reg.h>
#include <power/sunxi/axp.h>
#include <power/sunxi/pmu.h>
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
static int axp22_set_dc1sw(int onoff)
{
    u8   reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
    {
		return -1;
    }
    if(onoff)
    {
		reg_value |= (1 << 7);
	}
	else
	{
		reg_value &= ~(1 << 7);
	}
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
	{
		printf("sunxi pmu error : unable to set dc1sw\n");

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
static int axp22_set_dc5ldo(int onoff)
{
    u8   reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
    {
		return -1;
    }
    if(onoff)
    {
		reg_value |= (1 << 0);
	}
	else
	{
		reg_value &= ~(1 << 0);
	}
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to set dc5ldo\n");

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
static int axp22_set_dcdc1(int set_vol, int onoff)
{
    u8   reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 1600)
		{
			set_vol = 1600;
		}
		else if(set_vol > 3400)
		{
			set_vol = 3400;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC1OUT_VOL, &reg_value))
	    {
			return -1;
	    }
		reg_value = ((set_vol - 1600)/100);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DC1OUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set dcdc1\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(1 << 1);
	}
	else
	{
		reg_value |=  (1 << 1);
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc1\n");

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
static int axp22_set_dcdc2(int set_vol, int onoff)
{
    u8   reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 600)
		{
			set_vol = 600;
		}
		else if(set_vol > 1540)
		{
			set_vol = 1540;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC2OUT_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= ~0x3f;
	    reg_value |= (set_vol - 600)/20;
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DC2OUT_VOL, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set dcdc2\n");
	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(1 << 2);
	}
	else
	{
		reg_value |=  (1 << 2);
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc2\n");

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
static int axp22_set_dcdc3(int set_vol, int onoff)
{
    u8   reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 600)
		{
			set_vol = 600;
		}
		else if(set_vol > 1860)
		{
			set_vol = 1860;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC3OUT_VOL, &reg_value))
	    {
	    	debug("%d\n", __LINE__);

	        return -1;
	    }
		reg_value = ((set_vol - 600)/20);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DC3OUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set dcdc3\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(1 << 3);
	}
	else
	{
		reg_value |=  (1 << 3);
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc3\n");

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
static int axp22_set_dcdc4(int set_vol, int onoff)
{
    u8   reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 600)
		{
			set_vol = 600;
		}
		else if(set_vol > 1540)
		{
			set_vol = 1540;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC4OUT_VOL, &reg_value))
	    {
	    	debug("%d\n", __LINE__);

	        return -1;
	    }
		reg_value = ((set_vol - 600)/20);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DC4OUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set dcdc4\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(1 << 4);
	}
	else
	{
		reg_value |=  (1 << 4);
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc4\n");

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
static int axp22_set_dcdc5(int set_vol, int onoff)
{
    u8   reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 1000)
		{
			set_vol = 1000;
		}
		else if(set_vol > 2550)
		{
			set_vol = 2550;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC5OUT_VOL, &reg_value))
	    {
	    	debug("%d\n", __LINE__);

	        return -1;
	    }
		reg_value = ((set_vol - 1000)/50);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DC5OUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set dcdc5\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(1 << 5);
	}
	else
	{
		reg_value |=  (1 << 5);
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc5\n");

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
static int axp22_set_aldo1(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 3300)
		{
			set_vol = 3300;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ALDO1OUT_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_ALDO1OUT_VOL, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set aldo1\n");

	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(1 << 6);
	}
	else
	{
		reg_value |=  (1 << 6);
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to onoff aldo1\n");

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
static int axp22_set_aldo2(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 3300)
		{
			set_vol = 3300;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ALDO2OUT_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_ALDO2OUT_VOL, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set aldo2\n");

	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(1 << 7);
	}
	else
	{
		reg_value |=  (1 << 7);
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to onoff aldo2\n");

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
static int axp22_set_aldo3(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 3300)
		{
			set_vol = 3300;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ALDO3OUT_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_ALDO3OUT_VOL, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set aldo3\n");

	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ALDO_CTL, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(1 << 7);
	}
	else
	{
		reg_value |=  (1 << 7);
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_ALDO_CTL, reg_value))
	{
		printf("sunxi pmu error : unable to onoff aldo3\n");

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
static int axp22_set_dldo1(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 3300)
		{
			set_vol = 3300;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DLDO1_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DLDO1_VOL, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set dldo1\n");

	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(1 << 3);
	}
	else
	{
		reg_value |=  (1 << 3);
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dldo1\n");

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
static int axp22_set_dldo2(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 3300)
		{
			set_vol = 3300;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DLDO2_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DLDO2_VOL, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set dldo2\n");

	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(1 << 4);
	}
	else
	{
		reg_value |=  (1 << 4);
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dldo2\n");

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
static int axp22_set_dldo3(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 3300)
		{
			set_vol = 3300;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DLDO3_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DLDO3_VOL, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set dldo3\n");

	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(1 << 5);
	}
	else
	{
		reg_value |=  (1 << 5);
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dldo3\n");

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
static int axp22_set_dldo4(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 3300)
		{
			set_vol = 3300;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DLDO4_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DLDO4_VOL, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set dldo4\n");

	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(1 << 6);
	}
	else
	{
		reg_value |=  (1 << 6);
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dldo4\n");

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
static int axp22_set_eldo1(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 3300)
		{
			set_vol = 3300;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ELDO1_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_ELDO1_VOL, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set eldo1\n");

	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(1 << 0);
	}
	else
	{
		reg_value |=  (1 << 0);
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
	{
		printf("sunxi pmu error : unable to onoff eldo1\n");

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
static int axp22_set_eldo2(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 3300)
		{
			set_vol = 3300;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ELDO2_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_ELDO2_VOL, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set eldo2\n");

	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(1 << 1);
	}
	else
	{
		reg_value |=  (1 << 1);
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
	{
		printf("sunxi pmu error : unable to onoff eldo2\n");

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
static int axp22_set_eldo3(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 3300)
		{
			set_vol = 3300;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ELDO3_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_ELDO3_VOL, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set eldo3\n");

	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(1 << 2);
	}
	else
	{
		reg_value |=  (1 << 2);
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
	{
		printf("sunxi pmu error : unable to onoff eldo3\n");

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
static int axp22_set_gpio0ldo(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 3300)
		{
			set_vol = 3300;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_GPIO0_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_GPIO0_VOL, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set gpio0ldo\n");

	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_GPIO0_CTL, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(7 << 0);
		reg_value |=  (4 << 0);
	}
	else
	{
		reg_value &= ~(7 << 0);
		reg_value |=  (3 << 0);
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_GPIO0_CTL, reg_value))
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
static int axp22_set_gpio1ldo(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 3300)
		{
			set_vol = 3300;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_GPIO1_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_GPIO1_VOL, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set gpio1ldo\n");

	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_GPIO1_CTL, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(7 << 0);
		reg_value |=  (4 << 0);
	}
	else
	{
		reg_value &= ~(7 << 0);
		reg_value |=  (3 << 0);
	}
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_GPIO1_CTL, reg_value))
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
static int axp22_set_dcdc_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case 1:
			return axp22_set_dcdc1(vol_value, onoff);
		case 2:
			return axp22_set_dcdc2(vol_value, onoff);
		case 3:
			return axp22_set_dcdc3(vol_value, onoff);
		case 4:
			return axp22_set_dcdc4(vol_value, onoff);
		case 5:
			return axp22_set_dcdc5(vol_value, onoff);
	}

	return -1;
}

static int axp22_set_aldo_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case 1:
			return axp22_set_aldo1(vol_value, onoff);
		case 2:
			return axp22_set_aldo2(vol_value, onoff);
		case 3:
			return axp22_set_aldo3(vol_value, onoff);
	}

	return -1;
}

static int axp22_set_dldo_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case 1:
			return axp22_set_dldo1(vol_value, onoff);
		case 2:
			return axp22_set_dldo2(vol_value, onoff);
		case 3:
			return axp22_set_dldo3(vol_value, onoff);
		case 4:
			return axp22_set_dldo4(vol_value, onoff);
	}

	return -1;
}

static int axp22_set_eldo_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case 1:
			return axp22_set_eldo1(vol_value, onoff);
		case 2:
			return axp22_set_eldo2(vol_value, onoff);
		case 3:
			return axp22_set_eldo3(vol_value, onoff);
	}

	return -1;
}

static int axp22_set_gpioldo_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case 0:
			return axp22_set_gpio0ldo(vol_value, onoff);
		case 1:
			return axp22_set_gpio1ldo(vol_value, onoff);
	}

	return -1;
}

static int axp22_set_misc_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case PMU_SUPPLY_DC5LDO:
			return axp22_set_dc5ldo(onoff);
		case PMU_SUPPLY_DC1SW:
			return axp22_set_dc1sw(onoff);
	}

	return -1;
}


int axp22_set_supply_status(int vol_name, int vol_value, int onoff)
{
	int supply_type;
	int sppply_index;

	supply_type  = vol_name & 0xffff0000;
	sppply_index = vol_name & 0x0000ffff;

	switch(supply_type)
	{
		case PMU_SUPPLY_DCDC_TYPE:
			return axp22_set_dcdc_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_ALDO_TYPE:
			return axp22_set_aldo_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_ELDO_TYPE:
			return axp22_set_eldo_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_DLDO_TYPE:
			return axp22_set_dldo_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_GPIOLDO_TYPE:
			return axp22_set_gpioldo_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_MISC_TYPE:
			return axp22_set_misc_output(vol_name, vol_value, onoff);

			break;

		default:
			return -1;
	}
}

int axp22_set_supply_status_byname(char *vol_name, int vol_value, int onoff)
{
	int sppply_index;

	if(!strncmp(vol_name, "dcdc", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp22_set_dcdc_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "aldo", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp22_set_aldo_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "eldo", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp22_set_eldo_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "dldo", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp22_set_dldo_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "gpio", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp22_set_gpioldo_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "dc1sw", 5))
	{
		return axp22_set_dc1sw(onoff);
	}
	else if(!strncmp(vol_name, "dc5ldo", 5))
	{
		return axp22_set_dc5ldo(onoff);
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
static int axp22_probe_dcdc1(void)
{
    u8  reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
    {
		return -1;
    }
	if(!(reg_value & (1 << 1)))
	{
		return 0;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC1OUT_VOL, &reg_value))
    {
		return -1;
    }
    reg_value &= 0x1f;

    return 1600 + 100 * reg_value;
}
static int axp22_probe_dcdc2(void)
{
    u8  reg_value;
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
    {
		return -1;
    }
	if(!(reg_value & (1 << 2)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC2OUT_VOL, &reg_value))
    {
		return -1;
    }
    reg_value &= 0x3f;

    return 600 + 20 * reg_value;
}
static int axp22_probe_dcdc3(void)
{
    u8  reg_value;
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
    {
		return -1;
    }
	if(!(reg_value & (1 << 3)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC3OUT_VOL, &reg_value))
    {
		return -1;
    }
    reg_value &= 0x3f;

    return 600 + 20 * reg_value;
}
static int axp22_probe_dcdc4(void)
{
    int vol;
    u8  reg_value;
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
    {
		return -1;
    }
	if(!(reg_value & (1 << 4)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC4OUT_VOL, &reg_value))
    {
		return -1;
    }
    reg_value &= 0x3f;
    if(reg_value < 48)
    {
    	vol = 600 + 20 * reg_value;
    }
    else
    {
    	vol = 1800 + 100 * (reg_value - 48);
    }

    return vol;
}
static int axp22_probe_dcdc5(void)
{
    u8  reg_value;
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
    {
		return -1;
    }
	if(!(reg_value & (1 << 5)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC5OUT_VOL, &reg_value))
    {
		return -1;
    }
    reg_value &= 0x1f;

    return 1000 + 50 * reg_value;
}
static int axp22_probe_aldo1(void)
{
    u8  reg_value;
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
    {
		return -1;
    }
	if(!(reg_value & (1 << 6)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ALDO1OUT_VOL, &reg_value))
    {
		return -1;
    }
    reg_value &= 0x1f;

    return 700 + 100 * reg_value;
}
static int axp22_probe_aldo2(void)
{
    u8  reg_value;
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
    {
		return -1;
    }
	if(!(reg_value & (1 << 7)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ALDO2OUT_VOL, &reg_value))
    {
		return -1;
    }
    reg_value &= 0x1f;

    return 700 + 100 * reg_value;
}
static int axp22_probe_aldo3(void)
{
    u8  reg_value;
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ALDO_CTL, &reg_value))
    {
		return -1;
    }
	if(!(reg_value & (1<<7)))
		return 0;
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ALDO3OUT_VOL, &reg_value))
    {
		return -1;
    }
    reg_value &= 0x1f;

    return 700 + 100 * reg_value;
}
static int axp22_probe_dldo1(void)
{
    int vol;
    u8  reg_value;
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
    {
		return -1;
    }
	if(!(reg_value & (1<<3)))
		return 0;
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DLDO1_VOL, &reg_value))
    {
		return -1;
    }
    reg_value &= 0x1f;
    if(reg_value < 27)
    {
    	vol = 700 + 100 * reg_value;
    }
    else
    {
    	vol = 3400 + 200 * (reg_value - 27);
    }

    return vol;
}
static int axp22_probe_dldo2(void)
{
    u8  reg_value;
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
    {
		return -1;
    }
	if(!(reg_value & (1<<4)))
		return 0;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DLDO2_VOL, &reg_value))
    {
		return -1;
    }
    reg_value &= 0x1f;

    return 700 + 100 * reg_value;
}
static int axp22_probe_eldo1(void)
{
    u8  reg_value;
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
    {
		return -1;
    }
	if(!(reg_value & (1<<0)))
		return 0;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ELDO1_VOL, &reg_value))
    {
		return -1;
    }
    reg_value &= 0x1f;

    return 700 + 100 * reg_value;
}
static int axp22_probe_eldo2(void)
{
    u8  reg_value;
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
    {
		return -1;
    }
	if(!(reg_value & (1<<1)))
		return 0;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ELDO2_VOL, &reg_value))
    {
		return -1;
    }
    reg_value &= 0x1f;

    return 700 + 100 * reg_value;
}
static int axp22_probe_eldo3(void)
{
    u8  reg_value;
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
    {
		return -1;
    }
	if(!(reg_value & (1<<2)))
		return 0;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ELDO3_VOL, &reg_value))
    {
		return -1;
    }
    reg_value &= 0x1f;

    return 700 + 100 * reg_value;
}

static int axp22_probe_dcdc_output(int sppply_index)
{
	switch(sppply_index)
	{
		case 1:
			return axp22_probe_dcdc1();
		case 2:
			return axp22_probe_dcdc2();
		case 3:
			return axp22_probe_dcdc3();
		case 4:
			return axp22_probe_dcdc4();
		case 5:
			return axp22_probe_dcdc5();
	}

	return -1;
}

static int axp22_probe_aldo_output(int sppply_index)
{
	switch(sppply_index)
	{
		case 1:
			return axp22_probe_aldo1();
		case 2:
			return axp22_probe_aldo2();
		case 3:
			return axp22_probe_aldo3();
	}

	return -1;
}

static int axp22_probe_dldo_output(int sppply_index)
{
	switch(sppply_index)
	{
		case 1:
			return axp22_probe_dldo1();
		case 2:
			return axp22_probe_dldo2();
	}

	return -1;
}

static int axp22_probe_eldo_output(int sppply_index)
{
	switch(sppply_index)
	{
		case 1:
			return axp22_probe_eldo1();
		case 2:
			return axp22_probe_eldo2();
		case 3:
			return axp22_probe_eldo3();
	}

	return -1;
}



int axp22_probe_supply_status(int vol_name, int vol_value, int onoff)
{
	return 0;
}

int axp22_probe_supply_status_byname(char *vol_name)
{
	int sppply_index;

	sppply_index = 1 + vol_name[4] - '1';

	if(!strncmp(vol_name, "dcdc", 4))
	{
		return axp22_probe_dcdc_output(sppply_index);
	}
	else if(!strncmp(vol_name, "aldo", 4))
	{
		return axp22_probe_aldo_output(sppply_index);
	}
	else if(!strncmp(vol_name, "dldo", 4))
	{
		return axp22_probe_dldo_output(sppply_index);
	}
	else if(!strncmp(vol_name, "eldo", 4))
	{
		return axp22_probe_eldo_output(sppply_index);
	}
	return -1;
}


