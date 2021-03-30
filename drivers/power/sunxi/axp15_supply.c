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

//static int axp15_set_dc1sw(int onoff)
//{
//    return 0;
//}
//
//static int axp15_set_dc5ldo(int onoff)
//{
//    return 0;
//}

static int axp15_set_dcdc1(int set_vol,int onoff)
{
    u8  reg_value;
	u8 	tem_reg_value = 0;
	int i = 0;

	if(axp_i2c_read(AXP15_ADDR,BOOT_POWER15_OUTPUT_CTL,&reg_value))
	{
		return -1;
	}
	printf("the reg_vlaue is %x",reg_value);
    if(set_vol > 0)
    {
        if(set_vol <1700)
        {
            set_vol = 1700;
        }
        else if(set_vol >3500)
        {
            set_vol = 3500;
        }
		else if(set_vol <= 2100)
		{
			tem_reg_value = ((set_vol -1700)/100);
		}
		else if(set_vol < 2400)
		{
			tem_reg_value = 0x05;
		}
		else if(set_vol <= 2800)
		{
			tem_reg_value = ((set_vol-2400)/100)+0x05;
		}
		else if(set_vol < 3000)
		{
			tem_reg_value = 0x0a;
		}
		else
		{
			tem_reg_value = ((set_vol-3000)/100)+0x0a;
		}
        if(axp_i2c_read(AXP15_ADDR,BOOT_POWER15_DC1OUT_VOL,&reg_value))
        {
        	printf("can't read dcdc1_vol!!!\n");
        	return -1;
        }
		reg_value &= ~(0x0f<<0);
		reg_value |= tem_reg_value;
		if(axp_i2c_write(AXP15_ADDR,BOOT_POWER15_DC1OUT_VOL,reg_value))
		{
			printf("can't set dcdc1_vol!!!\n");
			return -1;
		}
		for(i = 0;i<1000;i++);
		reg_value = 0;
        if(axp_i2c_read(AXP15_ADDR,BOOT_POWER15_DC1OUT_VOL,&reg_value))
        {
        	printf("can't read dcdc1_vol!!!\n");
        	return -1;
        }
    }
	if(onoff<0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP15_ADDR,BOOT_POWER15_OUTPUT_CTL,&reg_value))
	{
		printf("can't read output_ctl_reg!!\n");
		return -1;
	}
	if(onoff == 0)
	{
		reg_value &= ~(1<<7);
	}
	else
	{
		reg_value |= (1<<7);
	}
	if(axp_i2c_write(AXP15_ADDR,BOOT_POWER15_OUTPUT_CTL,reg_value))
	{
		printf("can't set dcdc1_onoff status!!\n");
		return -1;
	}
	if(axp_i2c_read(AXP15_ADDR,BOOT_POWER15_DC1OUT_VOL,&reg_value))
	{
		printf("can't read output_ctl_reg!!\n");
		return -1;
	}
	return 0;
}


static int axp15_set_dcdc2(int set_vol,int onoff)
{
    u8  reg_value;
	u32 	vol;
	u8  tmp;
	u32 i = 0 ;
    if(set_vol > 0)
    {
        if(set_vol <700)
        {
            set_vol = 700;
        }
        else if(set_vol >2275)
        {
            set_vol = 2275;
        }
        if(axp_i2c_read(AXP15_ADDR,BOOT_POWER15_DC2OUT_VOL,&reg_value))
        {
        	printf("can't read dcdc2_vol!!!\n");
        	return -1;
        }
		tmp = reg_value & 0x3f;
		vol = tmp*25+700;
		printf("the vol is %d\n",vol);
		while(vol>set_vol)
		{
			tmp -= 1;
	        reg_value &= ~0x3f;
	        reg_value |= tmp;
	        if(axp_i2c_write(AXP15_ADDR, BOOT_POWER15_DC2OUT_VOL, reg_value))
	        {
	            return -1;
	        }
	        for(i=0;i<2000;i++);
	        if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_DC2OUT_VOL, &reg_value))
	        {
	            return -1;
	        }
	        tmp = reg_value & 0x3f;
	        vol = tmp * 25 + 700;
			printf("the vol is %d\n",vol);
		}
		while(vol<set_vol)
		{
			tmp += 1;
	        reg_value &= ~0x3f;
	        reg_value |= tmp;
	        if(axp_i2c_write(AXP15_ADDR, BOOT_POWER15_DC2OUT_VOL, reg_value))
	        {
	            return -1;
	        }
	        for(i=0;i<2000;i++);
	        if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_DC2OUT_VOL, &reg_value))
	        {
	            return -1;
	        }
	        tmp = reg_value & 0x3f;
	        vol = tmp * 25 + 700;
			printf("the vol is %d\n",vol);
		}
    }
	if(onoff<0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP15_ADDR,BOOT_POWER15_OUTPUT_CTL,&reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc2\n");
		return -1;
	}
	if(onoff == 0)
	{
		reg_value &= ~(1<<6);
	}
	else
	{
		reg_value |= (1<<6);
	}
	if(axp_i2c_write(AXP15_ADDR,BOOT_POWER15_OUTPUT_CTL,reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc2\n");
		return -1;
	}

	if(axp_i2c_read(AXP15_ADDR,BOOT_POWER15_DC2OUT_VOL,&reg_value))
	{
		printf("can't read output_ctl_reg!!\n");
		return -1;
	}
	return 0;
}

static int axp15_set_dcdc3(int set_vol,int onoff)
{
    u8  reg_value;
    if(set_vol > 0)
    {
        if(set_vol <700)
        {
            set_vol = 700;
        }
        else if(set_vol > 3500)
        {
            set_vol = 3500;
        }
		printf("the set_vol is %d \n",set_vol);
        if(axp_i2c_read(AXP15_ADDR,BOOT_POWER15_DC3OUT_VOL,&reg_value))
        {
        	printf("can't read dcdc3_vol!!!\n");
        	return -1;
        }
		reg_value &= ~0x3f;
		reg_value = ((set_vol -700)/50);

		if(axp_i2c_write(AXP15_ADDR,BOOT_POWER15_DC3OUT_VOL,reg_value))
		{
			printf("sunxi pmu error : unable to set dcdc3\n");
			return -1;
		}
		__msdelay(100);

        if(axp_i2c_read(AXP15_ADDR,BOOT_POWER15_DC3OUT_VOL,&reg_value))
        {
        	printf("can't read dcdc3_vol!!!\n");
        	return -1;
        }
    }

	if(onoff<0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP15_ADDR,BOOT_POWER15_OUTPUT_CTL,&reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc3\n");
		return -1;
	}
	if(onoff == 0)
	{
		reg_value &= ~(1<<5);
	}
	else
	{
		reg_value |= (1<<5);
	}
	if(axp_i2c_write(AXP15_ADDR,BOOT_POWER15_OUTPUT_CTL, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc3\n");
		return -1;
	}

	if(axp_i2c_read(AXP15_ADDR,BOOT_POWER15_DC3OUT_VOL,&reg_value))
	{
		printf("can't read output_ctl_reg!!\n");
		return -1;
	}
	return 0;
}


static int axp15_set_dcdc4(int set_vol,int onoff)
{
    u8  reg_value;
    if(set_vol > 0)
    {
        if(set_vol <700)
        {
            set_vol = 700;
        }
        else if(set_vol >3500)
        {
            set_vol = 3500;
        }
        if(axp_i2c_read(AXP15_ADDR,BOOT_POWER15_DC4OUT_VOL,&reg_value))
        {
        	printf("can't read dcdc4_vol!!!\n");
        	return -1;
        }
		reg_value &= ~0x7f;
		reg_value = ((set_vol -700)/25);
		if(axp_i2c_write(AXP15_ADDR,BOOT_POWER15_DC4OUT_VOL,reg_value))
		{
			printf("sunxi pmu error : unable to set dcdc4\n\n");
			return -1;
		}

    }
	if(onoff<0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP15_ADDR,BOOT_POWER15_OUTPUT_CTL,&reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc4\n");
		return -1;
	}
	if(onoff == 0)
	{
		reg_value &= ~(1<<4);
	}
	else
	{
		reg_value |= (1<<4);
	}
	if(axp_i2c_write(AXP15_ADDR,BOOT_POWER15_OUTPUT_CTL,reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc4\n");
		return -1;
	}

	if(axp_i2c_read(AXP15_ADDR,BOOT_POWER15_DC4OUT_VOL,&reg_value))
	{
		printf("can't read output_ctl_reg!!\n");
		return -1;
	}
	return 0;
}

static int axp15_set_dcdc5(int set_vol, int onoff)
{
	return 0;
}

static int axp15_set_aldo1(int set_vol, int onoff)
{
	u8 reg_value;
	u8 tem_reg_value;

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

		if(set_vol <= 2000)
		{
			tem_reg_value = ((set_vol -1200)/100);
		}
		else if(set_vol <= 2500)
		{
			tem_reg_value = 0x09;
		}
                else if(set_vol < 2700)
                {
                    tem_reg_value = 0x0a;
                }
                else if(set_vol <= 2800)
                {
                    tem_reg_value = ((set_vol - 2700)/100)+0x0a;
                }
                else if(set_vol < 3000)
                {
                    tem_reg_value = 0x0c;
                }
		else if(set_vol <= 3300)
		{
			tem_reg_value = ((set_vol -3000)/100)+0x0c;
		}
		if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_ALDO12OUT_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0x0f;
		reg_value |= (tem_reg_value<<4);
	    if(axp_i2c_write(AXP15_ADDR, BOOT_POWER15_ALDO12OUT_VOL, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set aldo1\n");

	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_OUTPUT_CTL, &reg_value))
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
    if(axp_i2c_write(AXP15_ADDR, BOOT_POWER15_OUTPUT_CTL, reg_value))
	{
		printf("sunxi pmu error : unable to onoff aldo1\n");

		return -1;
	}
	if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_ALDO12OUT_VOL, &reg_value))
	{
		return -1;
	}

	return 0;
}


static int axp15_set_aldo2(int set_vol, int onoff)
{
	u8 reg_value;
	u8 tem_reg_value;
	if(set_vol > 0)
	{
		if(set_vol < 1200)
		{
			set_vol = 1200;
		}
		else if(set_vol > 3300)
		{
			set_vol = 3300;
		}
		if(set_vol <= 2000)
		{
			tem_reg_value = ((set_vol -1200)/100);
		}
		else if(set_vol <= 2500)
		{
			tem_reg_value = 0x09;
		}
                else if(set_vol < 2700)
                {
                    tem_reg_value = 0x0a;
                }
                else if(set_vol <= 2800)
                {
                    tem_reg_value = ((set_vol - 2700)/100)+0x0a;
                }
                else if(set_vol < 3000)
                {
                    tem_reg_value = 0x0c;
                }
		else if(set_vol <= 3300)
		{
			tem_reg_value = ((set_vol -3000)/100)+0x0c;
		}
		if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_ALDO12OUT_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xf0;
		reg_value |= (tem_reg_value);
	    if(axp_i2c_write(AXP15_ADDR, BOOT_POWER15_ALDO12OUT_VOL, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set aldo2\n");

	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_OUTPUT_CTL, &reg_value))
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
    if(axp_i2c_write(AXP15_ADDR, BOOT_POWER15_OUTPUT_CTL, reg_value))
	{
		printf("sunxi pmu error : unable to onoff aldo2\n");

		return -1;
	}

	if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_ALDO12OUT_VOL, &reg_value))
	{
		return -1;
	}

	return 0;
}

static int axp15_set_aldo3(int set_vol, int onoff)
{
	return 0;
}

static int axp15_set_dldo1(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 3500)
		{
			set_vol = 3500;
		}
		if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_DLDO1OUT_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
	    if(axp_i2c_write(AXP15_ADDR, BOOT_POWER15_DLDO1OUT_VOL, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set dldo1\n");

	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_OUTPUT_CTL, &reg_value))
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
    if(axp_i2c_write(AXP15_ADDR, BOOT_POWER15_OUTPUT_CTL, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dldo1\n");

		return -1;
	}
	if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_DLDO1OUT_VOL, &reg_value))
	{
		return -1;
	}

	return 0;
}

static int axp15_set_dldo2(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 3500)
		{
			set_vol = 3500;
		}
		if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_DLDO2OUT_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
	    if(axp_i2c_write(AXP15_ADDR, BOOT_POWER15_DLDO2OUT_VOL, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set dldo2\n");

	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_OUTPUT_CTL, &reg_value))
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
    if(axp_i2c_write(AXP15_ADDR, BOOT_POWER15_OUTPUT_CTL, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dldo2\n");

		return -1;
	}
	if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_DLDO2OUT_VOL, &reg_value))
	{
		return -1;
	}

	return 0;
}


static int axp15_set_dldo3(int set_vol, int onoff)
{
	return 0;
}

static int axp15_set_dldo4(int set_vol, int onoff)
{
	return 0;
}

static int axp15_set_eldo1(int set_vol, int onoff)
{
	return 0;
}

static int axp15_set_eldo2(int set_vol, int onoff)
{
	return 0;
}

static int axp15_set_eldo3(int set_vol, int onoff)
{
	return 0;
}

static int axp15_set_gpio0ldo(int set_vol, int onoff)
{
	return 0;
}

static int axp15_set_gpio1ldo(int set_vol, int onoff)
{
	return 0;
}

static int axp15_set_gpio2ldo(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 1800)
		{
			set_vol = 1800;
		}
		else if(set_vol > 3300)
		{
			set_vol = 3300;
		}
		if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_GPIO2_LDO_MOD, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xf0;
		reg_value |= ((set_vol - 1800)/100);
	    if(axp_i2c_write(AXP15_ADDR, BOOT_POWER15_GPIO2_LDO_MOD, reg_value))
	    {
	    	printf("sunxi pmu error : unable to set gpio2ldo\n");

	        return -1;
	    }
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_GPIO2_CTL, &reg_value))
    {
		return -1;
    }
	if(onoff == 0)
	{
		reg_value &= ~(7 << 0);
		reg_value |=  (3 << 0);
	}
	else
	{
		reg_value &= ~(7 << 0);
		reg_value |=  (2 << 0);
	}
    if(axp_i2c_write(AXP15_ADDR, BOOT_POWER15_GPIO2_CTL, reg_value))
	{
		printf("sunxi pmu error : unable to onoff gpio1ldo\n");

		return -1;
	}

	return 0;
}

static int axp15_set_ldo0(int set_vol, int onoff)
{
	u8 reg_value = 0;

	if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_LDO0_VOL, &reg_value))
	{
		return -1;
	}
	reg_value &= ~(3<<4);
	if(set_vol < 2800)
	{
		set_vol = 2500;
		reg_value |= (3<<4);
	}
	else if(set_vol < 3300)
	{
		set_vol = 2800;
		reg_value |= (2<<4);
	}
	else if(set_vol < 5000)
	{
		set_vol = 3300;
		reg_value |= (1<<4);
	}
	else
	{
		set_vol = 5000;
		reg_value |= (0<<4);
	}

	if(axp_i2c_write(AXP15_ADDR, BOOT_POWER15_LDO0_VOL, reg_value))
	{
		return -1;
	}
	if(onoff < 0)
	{
		return 0;
	}

	reg_value = 0;
	if(axp_i2c_read(AXP15_ADDR, BOOT_POWER15_LDO0_VOL, &reg_value))
	{
		return -1;
	}
	if(onoff == 1)
	{
		reg_value |= (1<<7);
	}
	else
	{
		reg_value &= ~(1<<7);
	}
	if(axp_i2c_write(AXP15_ADDR, BOOT_POWER15_LDO0_VOL, reg_value))
	{
		return -1;
	}
	return 0;
}

static int axp15_set_dcdc_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case 1:
			return axp15_set_dcdc1(vol_value, onoff);
		case 2:
			return axp15_set_dcdc2(vol_value, onoff);
		case 3:
			return axp15_set_dcdc3(vol_value, onoff);
		case 4:
			return axp15_set_dcdc4(vol_value, onoff);
		case 5:
			return axp15_set_dcdc5(vol_value, onoff);
	}

	return -1;
}

static int axp15_set_aldo_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case 1:
			return axp15_set_aldo1(vol_value, onoff);
		case 2:
			return axp15_set_aldo2(vol_value, onoff);
		case 3:
			return axp15_set_aldo3(vol_value, onoff);
	}

	return -1;
}

static int axp15_set_dldo_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case 1:
			return axp15_set_dldo1(vol_value, onoff);
		case 2:
			return axp15_set_dldo2(vol_value, onoff);
		case 3:
			return axp15_set_dldo3(vol_value, onoff);
		case 4:
			return axp15_set_dldo4(vol_value, onoff);
	}

	return -1;
}

static int axp15_set_eldo_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case 1:
			return axp15_set_eldo1(vol_value, onoff);
		case 2:
			return axp15_set_eldo2(vol_value, onoff);
		case 3:
			return axp15_set_eldo3(vol_value, onoff);
	}

	return -1;
}

static int axp15_set_gpioldo_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case 0:
			return axp15_set_gpio0ldo(vol_value, onoff);
		case 1:
			return axp15_set_gpio1ldo(vol_value, onoff);
		case 2:
			return axp15_set_gpio2ldo(vol_value,onoff);
	}

	return -1;
}

static int axp15_set_ldo0_output(int sppply_index, int vol_value, int onoff)
{
	return axp15_set_ldo0(vol_value, onoff);
}




static int axp15_set_misc_output(int sppply_index, int vol_value, int onoff)
{
	return 0;
}


int axp15_set_supply_status(int vol_name, int vol_value, int onoff)
{
	int supply_type;
	int sppply_index;

	supply_type  = vol_name & 0xffff0000;
	sppply_index = vol_name & 0x0000ffff;

	switch(supply_type)
	{
		case PMU_SUPPLY_DCDC_TYPE:
			return axp15_set_dcdc_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_ALDO_TYPE:
			return axp15_set_aldo_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_ELDO_TYPE:
			return axp15_set_eldo_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_DLDO_TYPE:
			return axp15_set_dldo_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_GPIOLDO_TYPE:
			return axp15_set_gpioldo_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_MISC_TYPE:
			return axp15_set_misc_output(vol_name, vol_value, onoff);

			break;

		default:
			return -1;
	}
}


int axp15_set_supply_status_byname(char *vol_name, int vol_value, int onoff)
{
	int sppply_index;

	if(!strncmp(vol_name, "dcdc", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp15_set_dcdc_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "aldo", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp15_set_aldo_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "eldo", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp15_set_eldo_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "dldo", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp15_set_dldo_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "gpio", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp15_set_gpioldo_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "ldo", 3))
	{
		sppply_index = 0;//simple_strtoul(vol_name + 3, NULL, 10);

		return axp15_set_ldo0_output(sppply_index, vol_value, onoff);
	}

	return 0;
}
int axp15_probe_supply_status(int vol_name, int vol_value, int onoff)
{
	return 0;
}

int axp15_probe_supply_status_byname(char *vol_name)
{
	return 0;
}
