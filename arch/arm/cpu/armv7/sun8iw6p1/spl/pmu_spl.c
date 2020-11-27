/*
 * (C) Copyright 2016
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *zhouhuacai <zhouhuacai@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
 
#include <common.h>
#include <power/sunxi/axp81X_reg.h>
#include <asm/arch/base_pmu.h>

int pmu_type;

#define AW1673_IC_ID_REG		(0x3)
#define AW1673_DCDC5_VOL_CTRL	(0x24)
#define AXP81X_ADDR             (0x11)


static int axp_probe(void)
{
	u8  pmu_type;

	if (axp_i2c_read(AXP81X_ADDR, 0x3, &pmu_type)) {
		printf("axp read error\n");

		return -1;
	}
	pmu_type &= 0xCF;
	if (pmu_type == 0x41) {
		/* pmu type AXP81x */
		printf("PMU: AXP81X\n");
		return AXP81X_ADDR;
	} else {
		printf("unknow PMU\n");
		return -1;
	}
}


static int axp81X_set_dcdc5(int set_vol)
{
	u8  reg_value = 0;
	u8  pmu_type;
	int i, ddr_vol;

    if (axp_i2c_read(AXP81X_ADDR, 0x3, &pmu_type)) {
	    printf("axp read error\n");

	    return -1;
    }
    pmu_type &= 0xCF;
    if (pmu_type == 0x41) {
	    /* pmu type AXP81x */
	    printf("PMU: AXP81X\n");
    } else {
	    printf("unknow PMU\n");
	    return -1;
    }

    if (set_vol > 0) {
	    if (set_vol < 800) {
		    set_vol = 800;
	    } else if (set_vol > 1840) {
		    set_vol = 1840;
	    }
	    if (axp_i2c_read(AXP81X_ADDR, AW1673_DCDC5_VOL_CTRL, &reg_value)) {
		    printf("sunxi pmu error : unable to read dcdc5\n");
		    return -1;
	    }

	    /* rsb_printk("step1:AW1673_DCDC5_VOL_CTRL = %x\n", reg_value); */
	    reg_value &= (~0x7f);
	    /* dcdc5ï¼?0.8v-1.12v  10mv/step   1.12v-1.84v  20mv/step */
	    if (set_vol > 1120) {
		    reg_value |= (32 + (set_vol - 1120) / 20);
	    } else {
		    reg_value |= (set_vol - 800) / 10;
	    }
	    if (axp_i2c_write(AXP81X_ADDR, AW1673_DCDC5_VOL_CTRL, reg_value)) {
		    printf("sunxi pmu error : unable to set dcdc5\n");
		    return -1;
	    }
	    reg_value = 0;
	    for (i = 0; i < 100; i++)
		    ;
	    if (axp_i2c_read(AXP81X_ADDR, AW1673_DCDC5_VOL_CTRL, &reg_value)) {
		    printf("sunxi pmu error : unable to read dcdc5\n");
		    return -1;
	    }
	    reg_value &= 0x7f;
	    if (reg_value > 32) {
		    ddr_vol = 1120 + 20 * (reg_value - 32);
	    } else {
		    ddr_vol = 800 + 10 * reg_value;
	    }
	    printf("ddr voltage = %d mv\n", ddr_vol);
    }

    return 0;
}

int probe_power_key(void)
{
	u8  reg_value;

	if (axp_i2c_read(AXP81X_ADDR, BOOT_POWER81X_INTSTS5, &reg_value)) {
		return -1;
	}
	reg_value &= (0x03 << 3);
	if (reg_value) {
		if (axp_i2c_write(AXP81X_ADDR, BOOT_POWER81X_INTSTS5,
				  reg_value)) {
			return -1;
		}
	}

	return (reg_value>>3)&3;
}


int set_ddr_voltage(int set_vol)
{
	if (sunxi_rsb_init(0))
		return -1;

	if (sunxi_rsb_config(AXP81X_ADDR, 0x3a3))
		return -1;

	return axp81X_set_dcdc5(set_vol);
}

int pmu_init(u8 power_mode)
{
	if (sunxi_rsb_init(0))
		return -1;

	if (sunxi_rsb_config(AXP81X_ADDR, 0x3a3))
		return -1;

	pmu_type = axp_probe();

	return pmu_type;
}
