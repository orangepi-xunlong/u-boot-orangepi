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

static int axp806_set_dcdc1(int set_vol, int onoff)
{
	u8	 reg_value;
	u8	 tmp_step;

	if(set_vol > 0)
	{
		if(set_vol <= 1110)
		{
			if(set_vol < 600)
			{
				set_vol = 600;
			}
			tmp_step = (set_vol - 600)/10;
		}
		else
		{
			if(set_vol < 1120)
			{
				set_vol = 1120;
			}
			else if(set_vol > 1520)
			{
				set_vol = 1520;
			}

			tmp_step = (set_vol - 1120)/20 + 51;
		}
		if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_DCAOUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0x80;
		reg_value |= tmp_step;
		if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_DCAOUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set dcdc1\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, &reg_value))
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
	if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc1\n");

		return -1;
	}

	return 0;
}

static int axp806_probe_dcdc1(void)
{
	int vol;
	u8	reg_value;

	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, &reg_value))
	{
		return -1;
	}
	if(!(reg_value & (0x01 << 0)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_DCAOUT_VOL, &reg_value))
	{
		return -1;
	}
	reg_value &= 0x7f;
	if(reg_value < 51)
	{
		vol = 600 + 10 * reg_value;
	}
	else
	{
		vol = 1120 + 20 * (reg_value - 51);
	}

	return vol;
}

static int axp806_set_dcdc2(int set_vol, int onoff)
{
	u8	 reg_value;

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
		if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_DCBOUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= ~0x1f;
		reg_value |= (set_vol - 1000)/50;
		if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_DCBOUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set dcdc2\n");
			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, &reg_value))
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
	if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc2\n");

		return -1;
	}

	return 0;
}

static int axp806_probe_dcdc2(void)
{
	u8	reg_value;
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, &reg_value))
	{
		return -1;
	}
	if(!(reg_value & (0x01 << 1)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_DCBOUT_VOL, &reg_value))
	{
		return -1;
	}
	reg_value &= 0x1f;

	return 1000 + 50 * reg_value;
}

static int axp806_set_dcdc3(int set_vol, int onoff)
{
	u8	 reg_value;
	u8	 tmp_step;

	if(set_vol > 0)
	{
		if(set_vol <= 1110)
		{
			if(set_vol < 600)
			{
				set_vol = 600;
			}
			tmp_step = (set_vol - 600)/10;
		}
		else
		{
			if(set_vol < 1120)
			{
				set_vol = 1120;
			}
			else if(set_vol > 1520)
			{
				set_vol = 1520;
			}

			tmp_step = (set_vol - 1120)/20 + 51;
		}
		if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_DCCOUT_VOL, &reg_value))
		{
			printf("sunxi pmu error : unable to set dcdc3\n");
			return -1;
		}
		reg_value &= 0x80;
		reg_value |= tmp_step;
		if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_DCCOUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set dcdc3\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, &reg_value))
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
	if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc3\n");

		return -1;
	}

	return 0;
}

static int axp806_probe_dcdc3(void)
{
	int vol;
	u8	reg_value;
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, &reg_value))
	{
		return -1;
	}
	if(!(reg_value & (0x01 << 2)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_DCCOUT_VOL, &reg_value))
	{
		return -1;
	}
	reg_value &= 0x7f;
	if(reg_value < 51)
	{
		vol = 600 + 10 * reg_value;
	}
	else
	{
		vol = 1120 + 20 * (reg_value - 51);
	}

	return vol;
}

static int axp806_set_dcdc4(int set_vol, int onoff)
{
	u8	 reg_value;
	u8	 tmp_step;

	if(set_vol > 0)
	{
		if(set_vol <= 1500)
		{
			if(set_vol < 600)
			{
				set_vol = 600;
			}
			tmp_step = (set_vol - 600)/20;
		}
		else
		{
			if(set_vol < 1600)
			{
				set_vol = 1600;
			}
			else if(set_vol > 3300)
			{
				set_vol = 3300;
			}

			tmp_step = (set_vol - 1600)/100 + 47;
		}

		if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_DCDOUT_VOL, &reg_value))
		{
			debug("%d\n", __LINE__);

			return -1;
		}
		reg_value &= 0xC0;
		reg_value |= tmp_step;
		if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_DCDOUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set dcdc4\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, &reg_value))
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
	if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc4\n");

		return -1;
	}

	return 0;
}

static int axp806_probe_dcdc4(void)
{
	int vol;
	u8	reg_value;
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, &reg_value))
	{
		return -1;
	}
	if(!(reg_value & (0x01 << 3)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_DCDOUT_VOL, &reg_value))
	{
		return -1;
	}
	reg_value &= 0x3f;
	if(reg_value < 46)
	{
		vol = 600 + 20 * reg_value;
	}
	else
	{
		vol = 1600 + 100 * (reg_value - 46);
	}

	return vol;
}

static int axp806_set_dcdc5(int set_vol, int onoff)
{
	u8	 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 1100)
		{
			set_vol = 1100;
		}
		else if(set_vol > 3400)
		{
			set_vol = 3400;
		}
		if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_DCEOUT_VOL, &reg_value))
		{
			debug("%d\n", __LINE__);

			return -1;
		}
		reg_value &= 0xC0;
		reg_value |= ((set_vol - 1100)/100);
		if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_DCEOUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set dcdc5\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, &reg_value))
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
	if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc5\n");

		return -1;
	}

	return 0;
}

static int axp806_probe_dcdc5(void)
{
	u8	reg_value;
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, &reg_value))
	{
		return -1;
	}
	if(!(reg_value & (0x01 << 4)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_DCEOUT_VOL, &reg_value))
	{
		return -1;
	}
	reg_value &= 0x1f;

	return 1100 + 100 * reg_value;
}

static int axp806_set_aldo1(int set_vol, int onoff)
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
		if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_ALDO1OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
		if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_ALDO1OUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set aldo1\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, &reg_value))
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
	if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to onoff aldo1\n");

		return -1;
	}

	return 0;
}

static int axp806_probe_aldo1(void)
{
	u8	reg_value;
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, &reg_value))
	{
		return -1;
	}
	if(!(reg_value & (0x01 << 5)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_ALDO1OUT_VOL, &reg_value))
	{
		return -1;
	}
	reg_value &= 0x1f;

	return 700 + 100 * reg_value;
}

static int axp806_set_aldo2(int set_vol, int onoff)
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
		if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_ALDO2OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
		if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_ALDO2OUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set aldo2\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, &reg_value))
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
	if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to onoff aldo2\n");

		return -1;
	}

	return 0;
}

static int axp806_probe_aldo2(void)
{
	u8	reg_value;
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, &reg_value))
	{
		return -1;
	}
	if(!(reg_value & (0x01 << 6)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_ALDO2OUT_VOL, &reg_value))
	{
		return -1;
	}
	reg_value &= 0x1f;

	return 700 + 100 * reg_value;
}

static int axp806_set_aldo3(int set_vol, int onoff)
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
		if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_ALDO3OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
		if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_ALDO3OUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set aldo3\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, &reg_value))
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
	if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to onoff aldo3\n");

		return -1;
	}

	return 0;
}

static int axp806_probe_aldo3(void)
{
	u8	reg_value;
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, &reg_value))
	{
		return -1;
	}
	if(!(reg_value & (0x01 << 7)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_ALDO3OUT_VOL, &reg_value))
	{
		return -1;
	}
	reg_value &= 0x1f;

	return 700 + 100 * reg_value;
}

static int axp806_set_bldo1(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 1900)
		{
			set_vol = 1900;
		}
		if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_BLDO1OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0xf0;
		reg_value |= ((set_vol - 700)/100);
		if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_BLDO1OUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set bldo1\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, &reg_value))
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
	if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, reg_value))
	{
		printf("sunxi pmu error : unable to onoff bldo1\n");

		return -1;
	}

	return 0;
}

static int axp806_probe_bldo1(void)
{
	u8	reg_value;
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, &reg_value))
	{
		return -1;
	}
	if(!(reg_value & (0x01 << 0)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_BLDO1OUT_VOL, &reg_value))
	{
		return -1;
	}
	reg_value &= 0xf;

	return 700 + 100 * reg_value;
}

static int axp806_set_bldo2(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 1900)
		{
			set_vol = 1900;
		}
		if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_BLDO2OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0xf0;
		reg_value |= ((set_vol - 700)/100);
		if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_BLDO2OUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set bldo2\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, &reg_value))
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
	if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, reg_value))
	{
		printf("sunxi pmu error : unable to onoff bldo2\n");

		return -1;
	}

	return 0;
}

static int axp806_probe_bldo2(void)
{
	u8	reg_value;
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, &reg_value))
	{
		return -1;
	}
	if(!(reg_value & (0x01 << 1)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_BLDO2OUT_VOL, &reg_value))
	{
		return -1;
	}
	reg_value &= 0xf;

	return 700 + 100 * reg_value;
}

static int axp806_set_bldo3(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 1900)
		{
			set_vol = 1900;
		}
		if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_BLDO3OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0xf0;
		reg_value |= ((set_vol - 700)/100);
		if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_BLDO3OUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set bldo3\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, &reg_value))
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
	if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, reg_value))
	{
		printf("sunxi pmu error : unable to onoff bldo3\n");

		return -1;
	}

	return 0;
}

static int axp806_probe_bldo3(void)
{
	u8	reg_value;
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, &reg_value))
	{
		return -1;
	}
	if(!(reg_value & (0x01 << 2)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_BLDO3OUT_VOL, &reg_value))
	{
		return -1;
	}
	reg_value &= 0xf;

	return 700 + 100 * reg_value;
}

static int axp806_set_bldo4(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 1900)
		{
			set_vol = 1900;
		}
		if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_BLDO4OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0xf0;
		reg_value |= ((set_vol - 700)/100);
		if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_BLDO4OUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set bldo4\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, &reg_value))
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
	if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, reg_value))
	{
		printf("sunxi pmu error : unable to onoff bldo4\n");

		return -1;
	}

	return 0;
}

static int axp806_probe_bldo4(void)
{
	u8	reg_value;
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, &reg_value))
	{
		return -1;
	}
	if(!(reg_value & (0x01 << 3)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_BLDO4OUT_VOL, &reg_value))
	{
		return -1;
	}
	reg_value &= 0xf;

	return 700 + 100 * reg_value;
}

static int axp806_set_cldo1(int set_vol, int onoff)
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
		if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_CLDO1OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0xe0;
		reg_value |= ((set_vol - 700)/100);
		if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_CLDO1OUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set cldo1\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, &reg_value))
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
	if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, reg_value))
	{
		printf("sunxi pmu error : unable to onoff cldo1\n");

		return -1;
	}

	return 0;
}

static int axp806_probe_cldo1(void)
{
	u8	reg_value;
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, &reg_value))
	{
		return -1;
	}
	if(!(reg_value & (0x01 << 4)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_CLDO1OUT_VOL, &reg_value))
	{
		return -1;
	}
	reg_value &= 0x1f;

	return 700 + 100 * reg_value;
}

static int axp806_set_cldo2(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 1900)
		{
			set_vol = 1900;
		}
		if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_CLDO2OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0xf0;
		reg_value |= ((set_vol - 700)/100);
		if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_CLDO2OUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set cldo2\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, &reg_value))
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
	if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, reg_value))
	{
		printf("sunxi pmu error : unable to onoff cldo2\n");

		return -1;
	}

	return 0;
}

static int axp806_probe_cldo2(void)
{
	int vol;
	u8	reg_value;
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, &reg_value))
	{
		return -1;
	}
	if(!(reg_value & (0x01 << 5)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_CLDO2OUT_VOL, &reg_value))
	{
		return -1;
	}
	reg_value &= 0x1f;
	if(reg_value < 28)
	{
		vol = 700 + 100 * reg_value;
	}
	else
	{
		vol = 3600 + 200 * (reg_value - 28);
	}

	return vol;
}

static int axp806_set_cldo3(int set_vol, int onoff)
{
	u8 reg_value;

	if(set_vol > 0)
	{
		if(set_vol < 700)
		{
			set_vol = 700;
		}
		else if(set_vol > 1900)
		{
			set_vol = 1900;
		}
		if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_CLDO3OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0xf0;
		reg_value |= ((set_vol - 700)/100);
		if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_CLDO3OUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set cldo3\n");

			return -1;
		}
	}

	if(onoff < 0)
	{
		return 0;
	}
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, &reg_value))
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
	if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, reg_value))
	{
		printf("sunxi pmu error : unable to onoff cldo3\n");

		return -1;
	}

	return 0;
}

static int axp806_probe_cldo3(void)
{
	u8	reg_value;
	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL2, &reg_value))
	{
		return -1;
	}
	if(!(reg_value & (0x01 << 6)))
	{
		return 0;
	}

	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_CLDO3OUT_VOL, &reg_value))
	{
		return -1;
	}
	reg_value &= 0x1f;

	return 700 + 100 * reg_value;
}
/*
************************************************************************************************************
*
*											  function
*
*	 name		   :
*
*	 parmeters	   :
*
*	 return		   :
*
*	 note		   :
*
*
************************************************************************************************************
*/
static int axp806_set_dcdc_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case 1:
			return axp806_set_dcdc1(vol_value, onoff);
		case 2:
			return axp806_set_dcdc2(vol_value, onoff);
		case 3:
			return axp806_set_dcdc3(vol_value, onoff);
		case 4:
			return axp806_set_dcdc4(vol_value, onoff);
		case 5:
			return axp806_set_dcdc5(vol_value, onoff);
	}

	return -1;
}

static int axp806_set_aldo_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case 1:
			return axp806_set_aldo1(vol_value, onoff);
		case 2:
			return axp806_set_aldo2(vol_value, onoff);
		case 3:
			return axp806_set_aldo3(vol_value, onoff);
	}

	return -1;
}

static int axp806_set_bldo_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case 1:
			return axp806_set_bldo1(vol_value, onoff);
		case 2:
			return axp806_set_bldo2(vol_value, onoff);
		case 3:
			return axp806_set_bldo3(vol_value, onoff);
		case 4:
			return axp806_set_bldo4(vol_value, onoff);
	}

	return -1;
}

static int axp806_set_cldo_output(int sppply_index, int vol_value, int onoff)
{
	switch(sppply_index)
	{
		case 1:
			return axp806_set_cldo1(vol_value, onoff);
		case 2:
			return axp806_set_cldo2(vol_value, onoff);
		case 3:
			return axp806_set_cldo3(vol_value, onoff);
	}

	return -1;
}

int axp806_set_supply_status(int vol_name, int vol_value, int onoff)
{
	int supply_type;
	int sppply_index;

	supply_type	 = vol_name & 0xffff0000;
	sppply_index = vol_name & 0x0000ffff;

	switch(supply_type)
	{
		case PMU_SUPPLY_DCDC_TYPE:
			return axp806_set_dcdc_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_ALDO_TYPE:
			return axp806_set_aldo_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_BLDO_TYPE:
			return axp806_set_bldo_output(sppply_index, vol_value, onoff);

		case PMU_SUPPLY_CLDO_TYPE:
			return axp806_set_cldo_output(sppply_index, vol_value, onoff);

		default:
			return -1;
	}
}

int axp806_set_supply_status_byname(char *vol_name, int vol_value, int onoff)
{
	int sppply_index;

	if(!strncmp(vol_name, "dcdc", 4))
	{
		sppply_index = 1 + vol_name[4] - 'a';

		return axp806_set_dcdc_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "aldo", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp806_set_aldo_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "bldo", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp806_set_bldo_output(sppply_index, vol_value, onoff);
	}
	else if(!strncmp(vol_name, "cldo", 4))
	{
		sppply_index = simple_strtoul(vol_name + 4, NULL, 10);

		return axp806_set_cldo_output(sppply_index, vol_value, onoff);
	}

	return 0;
}

/*
************************************************************************************************************
*
*											  function
*
*	 name		   :
*
*	 parmeters	   :
*
*	 return		   :
*
*	 note		   :
*
*
************************************************************************************************************
*/
static int axp806_probe_dcdc_output(int sppply_index)
{
	switch(sppply_index)
	{
		case 1:
			return axp806_probe_dcdc1();
		case 2:
			return axp806_probe_dcdc2();
		case 3:
			return axp806_probe_dcdc3();
		case 4:
			return axp806_probe_dcdc4();
		case 5:
			return axp806_probe_dcdc5();
	}

	return -1;
}

static int axp806_probe_aldo_output(int sppply_index)
{
	switch(sppply_index)
	{
		case 1:
			return axp806_probe_aldo1();
		case 2:
			return axp806_probe_aldo2();
		case 3:
			return axp806_probe_aldo3();
	}

	return -1;
}

static int axp806_probe_bldo_output(int sppply_index)
{
	switch(sppply_index)
	{
		case 1:
			return axp806_probe_bldo1();
		case 2:
			return axp806_probe_bldo2();
		case 3:
			return axp806_probe_bldo3();
		case 4:
			return axp806_probe_bldo4();
	}

	return -1;
}

static int axp806_probe_cldo_output(int sppply_index)
{
	switch(sppply_index)
	{
		case 1:
			return axp806_probe_cldo1();
		case 2:
			return axp806_probe_cldo2();
		case 3:
			return axp806_probe_cldo3();
	}

	return -1;
}

int axp806_probe_supply_status(int vol_name, int vol_value, int onoff)
{

	return 0;
}

int axp806_probe_supply_status_byname(char *vol_name)
{
	int sppply_index;

	if(!strncmp(vol_name, "dcdc", 4))
	{
		sppply_index = 1 + vol_name[4] - 'a';

		return axp806_probe_dcdc_output(sppply_index);
	}
	else if(!strncmp(vol_name, "aldo", 4))
	{
		sppply_index = 1 + vol_name[4] - '1';

		return axp806_probe_aldo_output(sppply_index);
	}
	else if(!strncmp(vol_name, "bldo", 4))
	{
		sppply_index = 1 + vol_name[4] - '1';

		return axp806_probe_bldo_output(sppply_index);
	}
	else if(!strncmp(vol_name, "cldo", 4))
	{
		sppply_index = 1 + vol_name[4] - '1';

		return axp806_probe_cldo_output(sppply_index);
	}

	return -1;
}

