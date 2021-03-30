/*
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, d.mueller@elsoft.ch
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* This code should work for both the S3C2400 and the S3C2410
 * as they seem to have the same I2C controller inside.
 * The different address mapping is handled by the s3c24xx.h files below.
 */

#include <common.h>
#include <asm/arch/base_pmu.h>
#include <power/sunxi/axp81X_reg.h>

int pmu_type;

#define dbg(format,arg...)	printf(format,##arg)


static int axp_probe(void)
{
	u8  pmu_type;

	if(axp_i2c_read(AXP81X_ADDR, 0x3, &pmu_type))
	{
		printf("axp read error\n");

		return -1;
	}
	pmu_type &= 0xCF;
	if(pmu_type == 0x41)
	{
		/* pmu type AXP81x */
		printf("PMU: AXP81X\n");
		return AXP81X_ADDR;
	}
	else
	{
		printf("unknow PMU\n");
		return -1;
	}

}

static int axp81X_set_dcdc5(int set_vol)
{
    u8   reg_value;

	if(set_vol < 800)
	{
		set_vol = 800;
	}
	else if(set_vol > 1840)
	{
		set_vol = 1840;
	}
	if(axp_i2c_read(AXP81X_ADDR, BOOT_POWER81X_DC5OUT_VOL, &reg_value))
    {
    	debug("%d\n", __LINE__);

        return -1;
    }
    reg_value &= (~0x7f);
	//dcdc5¡êo 0.8v-1.12v  10mv/step   1.12v-1.84v  20mv/step
    if(set_vol > 1120)
    {
         reg_value |= (32+(set_vol - 1120)/20);
    }
    else
    {
        reg_value |= (set_vol - 800)/10;
    }
	if(axp_i2c_write(AXP81X_ADDR, BOOT_POWER81X_DC5OUT_VOL, reg_value))
	{
		printf("sunxi pmu error : unable to set dcdc5\n");

		return -1;
	}

	if(axp_i2c_read(AXP81X_ADDR, BOOT_POWER81X_OUTPUT_CTL1, &reg_value))
    {
		return -1;
    }
	reg_value |=  (1 << 4);

    if(axp_i2c_write(AXP81X_ADDR, BOOT_POWER81X_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc5\n");

		return -1;
	}

	return 0;
}

static int axp81X_set_dcdc2(int set_vol)
{
    u8   reg_value;

	if(set_vol < 500)
	{
		set_vol = 500;
	}
	else if(set_vol > 1300)
	{
		set_vol = 1300;
	}
	if(axp_i2c_read(AXP81X_ADDR, BOOT_POWER81X_DC2OUT_VOL, &reg_value))
    {
        return -1;
    }
    reg_value &= ~0x7f;
    //dcdc2¡êo 0.5v-1.2v  10mv/step   1.22v-1.3v  20mv/step
    if(set_vol > 1200)
    {
         reg_value |= (70+(set_vol - 1200)/20);
    }
    else
    {
        reg_value |= (set_vol - 500)/10;
    }

    if(axp_i2c_write(AXP81X_ADDR, BOOT_POWER81X_DC2OUT_VOL, reg_value))
    {
    	printf("sunxi pmu error : unable to set dcdc2\n");
        return -1;
    }

	if(axp_i2c_read(AXP81X_ADDR, BOOT_POWER81X_OUTPUT_CTL1, &reg_value))
    {
		return -1;
    }

	reg_value |=  (1 << 1);
	if(axp_i2c_write(AXP81X_ADDR, BOOT_POWER81X_OUTPUT_CTL1, reg_value))
	{
		printf("sunxi pmu error : unable to onoff dcdc2\n");

		return -1;
	}

    return 0;
}

int probe_power_key(void)
{
	u8  reg_value;

	if(axp_i2c_read(AXP81X_ADDR, BOOT_POWER81X_INTSTS5, &reg_value))
    {
        return -1;
    }
    reg_value &= (0x03<<3);
	if(reg_value)
	{
		if(axp_i2c_write(AXP81X_ADDR, BOOT_POWER81X_INTSTS5, reg_value))
	    {
	        return -1;
	    }
	}

	return (reg_value>>3)&3;
}

int set_ddr_voltage(int set_vol)
{
	if(pmu_type == AXP81X_ADDR)
	{
		return axp81X_set_dcdc5(set_vol);
	}

	return -1;
}

int set_pll_voltage(int set_vol)
{
	if(pmu_type == AXP81X_ADDR)
	{
		return axp81X_set_dcdc2(set_vol);
	}

	return -1;
}

int pmu_init(void)
{
	if(sunxi_rsb_init(0))
		return -1;

	if(sunxi_rsb_config(AXP81X_ADDR, 0x3a3))
		return -1;

	pmu_type = axp_probe();

	return pmu_type;
}

