/*
 * Copyright (C) 2019 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SUNXI AXP152  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <sunxi_power/pmu_axp152.h>
#include <sunxi_power/axp.h>
#include <asm/arch/pmic_bus.h>

/*#include <power/sunxi/pmu.h>*/

#ifdef PMU_DEBUG
#define axp_info(fmt...) tick_printf("[axp][info]: " fmt)
#define axp_err(fmt...) tick_printf("[axp][err]: " fmt)
#else
#define axp_info(fmt...)
#define axp_err(fmt...) tick_printf("[axp][err]: " fmt)
#endif

typedef struct _axp_step_info {
	u32 step_min_vol;
	u32 step_max_vol;
	u32 step_val;
	u32 regation;
} _axp_step_info;

typedef struct _axp_contrl_info {
	char name[16];

	u32 min_vol;
	u32 max_vol;
	u32 cfg_reg_addr;
	u32 cfg_reg_mask;
	u32 ctrl_reg_addr;
	u32 ctrl_bit_ofs;
	u32 reg_addr_offest;
	_axp_step_info axp_step_tbl[4];

} axp_contrl_info;

__attribute__((section(".data"))) axp_contrl_info pmu_axp152_ctrl_tbl[] = {
	{ "dcdc1", 1700, 3500, AXP152_DC1OUT_VOL, 0x0f, AXP152_OUTPUT_CTL, 7, 0,
	{ {1700, 2100, 100}, {2400, 2800, 100}, {3000, 3500, 100} } },

	{ "dcdc2", 700, 2275, AXP152_DC2OUT_VOL, 0x3f, AXP152_OUTPUT_CTL, 6, 0,
	{ {700, 2275, 25}, } },

	{ "dcdc3", 700, 3500, AXP152_DC3OUT_VOL, 0x3f, AXP152_OUTPUT_CTL, 5, 0,
	{ {700, 3500, 50}, } },

	{ "dcdc4", 700, 3500, AXP152_DC4OUT_VOL, 0x7f, AXP152_OUTPUT_CTL, 4, 0,
	{ {700, 3500, 25}, } },

	{ "aldo1", 1200, 3300, AXP152_ALDO12OUT_VOL, 0xf0, AXP152_OUTPUT_CTL, 3, 4,
	{ {1200, 2000, 100}, {2500, 2700, 200}, {2800, 3000, 200}, {3100, 3300, 100} } },

	{ "aldo2", 1200, 3300, AXP152_ALDO12OUT_VOL, 0x0f, AXP152_OUTPUT_CTL, 2, 0,
	{ {1200, 2000, 100}, {2500, 2700, 200}, {2800, 3000, 200}, {3100, 3300, 100} } },

	{ "dldo1", 700, 3500, AXP152_DLDO1OUT_VOL, 0x1f, AXP152_OUTPUT_CTL, 1, 0,
	{ {700, 3500, 100}, } },

	{ "dldo2", 700, 3500, AXP152_DLDO2OUT_VOL, 0x1f, AXP152_OUTPUT_CTL, 0, 0,
	{ {700, 3500, 100}, } },

	{ "ldo0", 2500, 5000, AXP152_LDO0_VOL, 0x30, AXP152_LDO0_VOL, 7, 4,
	{ {2500, 2800, 300, 1}, {3300, 5000, 1700, 1}, }, },

	{ "gpio2ldo", 1800, 3300, AXP152_GPIO2_LDO_MOD, 0x0f, AXP152_GPIO2_CTL, 7, 0,
	{ {1800, 3300, 100}, } },

};


static axp_contrl_info *get_ctrl_info_from_tbl(char *name)
{
	int i    = 0;
	int size = ARRAY_SIZE(pmu_axp152_ctrl_tbl);
	axp_contrl_info *p;

	for (i = 0; i < size; i++) {
		if (!strncmp(name, pmu_axp152_ctrl_tbl[i].name,
			     strlen(pmu_axp152_ctrl_tbl[i].name))) {
			break;
		}
	}
	if (i >= size) {
		axp_err("can't find %s from table\n", name);
		return NULL;
	}
	p = pmu_axp152_ctrl_tbl + i;
	return p;
}



static int pmu_axp152_probe(void)
{
	u8 pmu_chip_id;
	if (pmic_bus_init(AXP152_DEVICE_ADDR, AXP152_RUNTIME_ADDR)) {
		tick_printf("%s pmic_bus_init fail\n", __func__);
		return -1;
	}
	if (pmic_bus_read(AXP152_RUNTIME_ADDR, AXP152_VERSION, &pmu_chip_id)) {
		tick_printf("%s pmic_bus_read fail\n", __func__);
		return -1;
	}
	pmu_chip_id &= 0X0F;
	if (pmu_chip_id == AXP152_CHIP_ID) {
		/*pmu type AXP152*/
		tick_printf("PMU: AXP152\n");
		return 0;
	}
	return -1;
}

static int pmu_axp152_get_info(char *name, unsigned char *chipid)
{
	strncpy(name, "axp152", sizeof("axp152"));
	*chipid = AXP152_CHIP_ID;
	return 0;
}

static int pmu_set_vol(char *name, uint set_vol, uint onoff)
{
	u8 reg_value, i;
	axp_contrl_info *p_item = NULL;
	u8 base_step		= 0;

	p_item = get_ctrl_info_from_tbl(name);
	if (!p_item) {
		return -1;
	}

	if ((set_vol > 0) && (p_item->min_vol)) {
		if (set_vol < p_item->min_vol) {
			set_vol = p_item->min_vol;
		} else if (set_vol > p_item->max_vol) {
			set_vol = p_item->max_vol;
		}
		if (pmic_bus_read(AXP152_RUNTIME_ADDR, p_item->cfg_reg_addr,
				  &reg_value)) {
			return -1;
		}

		reg_value &= ~p_item->cfg_reg_mask;

		for (i = 0; p_item->axp_step_tbl[i].step_max_vol != 0; i++) {
			if ((set_vol > p_item->axp_step_tbl[i].step_max_vol) &&
				(set_vol < p_item->axp_step_tbl[i+1].step_min_vol)) {
				set_vol = p_item->axp_step_tbl[i].step_max_vol;
			}
			if (p_item->axp_step_tbl[i].step_max_vol >= set_vol) {
				reg_value |= ((base_step + ((set_vol - p_item->axp_step_tbl[i].step_min_vol)/
					p_item->axp_step_tbl[i].step_val)) << p_item->reg_addr_offest);
			if (p_item->axp_step_tbl[i].regation) {
				u8 reg_value_temp = (~reg_value & p_item->cfg_reg_mask);
				reg_value &= ~p_item->cfg_reg_mask;
				reg_value |= reg_value_temp;
			}
				axp_info("sum:0x%x\n", (base_step + ((set_vol - p_item->axp_step_tbl[i].step_min_vol)/p_item->axp_step_tbl[i].step_val)));
				break;
			} else {
				base_step += ((p_item->axp_step_tbl[i].step_max_vol -
					p_item->axp_step_tbl[i].step_min_vol + p_item->axp_step_tbl[i].step_val) /
					p_item->axp_step_tbl[i].step_val);
				axp_info("base_step:0x%x\n", base_step);
			}
		}

		if (pmic_bus_write(AXP152_RUNTIME_ADDR, p_item->cfg_reg_addr,
				   reg_value)) {
			axp_err("unable to set %s\n", name);
			return -1;
		}
	}

	if (onoff < 0) {
		return 0;
	}
	if (pmic_bus_read(AXP152_RUNTIME_ADDR, p_item->ctrl_reg_addr,
			  &reg_value)) {
		return -1;
	}
	if (onoff == 0) {
		reg_value &= ~(1 << p_item->ctrl_bit_ofs);
	} else {
		reg_value |= (1 << p_item->ctrl_bit_ofs);
	}
	if (pmic_bus_write(AXP152_RUNTIME_ADDR, p_item->ctrl_reg_addr,
			   reg_value)) {
		axp_err("unable to onoff %s\n", name);
		return -1;
	}
	return 0;
}

static int pmu_axp152_get_voltage(char *name)
{

	u8 reg_value, i;
	axp_contrl_info *p_item = NULL;
	u8 base_step1 = 0;
	u8 base_step2 = 0;
	int vol;

	p_item = get_ctrl_info_from_tbl(name);
	if (!p_item) {
		return -1;
	}

	if (pmic_bus_read(AXP152_RUNTIME_ADDR, p_item->ctrl_reg_addr,
		&reg_value)) {
		return -1;
	}
	if (!(reg_value & (0x01 << p_item->ctrl_bit_ofs))) {
		return 0;
	}

	if (pmic_bus_read(AXP152_RUNTIME_ADDR, p_item->cfg_reg_addr,
		&reg_value)) {
		return -1;
	}
	reg_value &= p_item->cfg_reg_mask;
	reg_value >>= p_item->reg_addr_offest;
	for (i = 0; p_item->axp_step_tbl[i].step_max_vol != 0; i++) {
		base_step1 += ((p_item->axp_step_tbl[i].step_max_vol -
				p_item->axp_step_tbl[i].step_min_vol + p_item->axp_step_tbl[i].step_val) /
				p_item->axp_step_tbl[i].step_val);
		if (p_item->axp_step_tbl[i].regation)
				reg_value = (~reg_value & (p_item->cfg_reg_mask >> p_item->reg_addr_offest));
		if (reg_value < base_step1) {
			vol =  (reg_value - base_step2) * p_item->axp_step_tbl[i].step_val +
					p_item->axp_step_tbl[i].step_min_vol;
			return vol;
		}
		if (p_item->axp_step_tbl[i].regation)
			reg_value = (~reg_value & (p_item->cfg_reg_mask >> p_item->reg_addr_offest));
		base_step2 += ((p_item->axp_step_tbl[i].step_max_vol -
				p_item->axp_step_tbl[i].step_min_vol + p_item->axp_step_tbl[i].step_val) /
				p_item->axp_step_tbl[i].step_val);
	}

	return -1;
}

static int pmu_axp152_set_voltage(char *name, uint set_vol, uint onoff)
{
	int i, temp_vol, src_vol = pmu_axp152_get_voltage(name);
	u32 step_voltage = 0xffff;
	axp_contrl_info *p_item = NULL;
	p_item = get_ctrl_info_from_tbl(name);
	if (!p_item) {
		return -1;
	}
	for (i = 0; p_item->axp_step_tbl[i].step_val != 0; i++) {
		step_voltage = min(step_voltage, p_item->axp_step_tbl[i].step_val);
	}
	if (step_voltage == 0xffff)
		return -1;
	if ((set_vol == 0) || (!strncmp(name, "ldo0", strlen("ldo0")))) {
		pmu_set_vol(name, set_vol, onoff);
	} else if (src_vol > set_vol) {
		for (temp_vol = src_vol; temp_vol >= set_vol; temp_vol -= step_voltage) {
			pmu_set_vol(name, temp_vol, onoff);
		}
		udelay(step_voltage*10/25);
	} else if (src_vol < set_vol) {
		for (temp_vol = src_vol; temp_vol <= set_vol; temp_vol += step_voltage) {
			pmu_set_vol(name, temp_vol, onoff);
		}
		udelay(step_voltage*10/25);
	}
	return 0;

}

static int pmu_axp152_set_power_off(void)
{
	u8 reg_value;
	if (pmic_bus_read(AXP152_RUNTIME_ADDR, AXP152_OFF_CTL, &reg_value)) {
		return -1;
	}
	reg_value |= 1 << 7;
	if (pmic_bus_write(AXP152_RUNTIME_ADDR, AXP152_OFF_CTL, reg_value)) {
		return -1;
	}
	return 0;
}


static int pmu_axp152_get_key_irq(void)
{
	u8 reg_value;
	if (pmic_bus_read(AXP152_RUNTIME_ADDR, AXP152_INTSTS2, &reg_value)) {
		return -1;
	}
	reg_value &= (0x03);
	if (reg_value) {
		if (pmic_bus_write(AXP152_RUNTIME_ADDR, AXP152_INTSTS2,
				   reg_value | (0x01 << 1))) {
			return -1;
		}
	}
	return reg_value & 3;
}

unsigned char pmu_axp152_get_reg_value(unsigned char reg_addr)
{
	u8 reg_value;
	if (pmic_bus_read(AXP152_RUNTIME_ADDR, reg_addr, &reg_value)) {
		return -1;
	}
	return reg_value;
}

unsigned char pmu_axp152_set_reg_value(unsigned char reg_addr, unsigned char reg_value)
{
	unsigned char reg;
	if (pmic_bus_write(AXP152_RUNTIME_ADDR, reg_addr, reg_value)) {
		return -1;
	}
	if (pmic_bus_read(AXP152_RUNTIME_ADDR, reg_addr, &reg)) {
		return -1;
	}
	return reg;
}


U_BOOT_AXP_PMU_INIT(pmu_axp152) = {
	.pmu_name	  = "pmu_axp152",
	.probe		   = pmu_axp152_probe,
	.get_info 		= pmu_axp152_get_info,
	.set_voltage       = pmu_axp152_set_voltage,
	.get_voltage       = pmu_axp152_get_voltage,
	.set_power_off     = pmu_axp152_set_power_off,
	/*.set_sys_mode      = pmu_axp152_set_sys_mode,*/
	/*.get_sys_mode      = pmu_axp152_get_sys_mode,*/
	.get_key_irq       = pmu_axp152_get_key_irq,
	/*.set_bus_vol_limit = pmu_axp152_set_bus_vol_limit,*/
	.get_reg_value	   = pmu_axp152_get_reg_value,
	.set_reg_value	   = pmu_axp152_set_reg_value,
};

