/*
 * Copyright (C) 2019 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SUNXI AXP858  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <sunxi_power/pmu_axp858.h>
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

typedef struct _axp_contrl_info {
	char name[16];

	u32 min_vol;
	u32 max_vol;
	u32 cfg_reg_addr;
	u32 cfg_reg_mask;

	u32 step0_val;
	u32 split1_val;
	u32 step1_val;
	u32 ctrl_reg_addr;

	u32 ctrl_bit_ofs;
} axp_contrl_info;

__attribute__((section(".data"))) axp_contrl_info pmu_axp858_ctrl_tbl[] = {
	/*name,    min,  max, reg,  mask, step0,split1_val, step1,ctrl_reg,ctrl_bit */
	{ "dcdc1", 1500, 3400, 0x13, 0x1f, 100, 0, 0, PMU_ONOFF_CTL1, 0 },
	{ "dcdc2", 500, 1540, 0x14, 0x7f, 10, 1200, 20, PMU_ONOFF_CTL1, 1 },
	{ "dcdc3", 500, 1540, 0x15, 0x7f, 10, 1200, 20, PMU_ONOFF_CTL1, 2 },
	{ "dcdc4", 500, 1540, 0x16, 0x7f, 10, 1200, 20, PMU_ONOFF_CTL1, 3 },
	{ "dcdc5", 800, 1840, 0x17, 0x7f, 10, 1120, 20, PMU_ONOFF_CTL1, 4 },
	{ "dcdc6", 500, 3400, 0x18, 0x1f, 100, 0, 0, PMU_ONOFF_CTL1, 5 },

	{ "aldo1", 700, 3300, 0x19, 0x1f, 100, 0, 0, PMU_ONOFF_CTL2, 0 },
	{ "aldo2", 700, 3300, 0x20, 0x1f, 100, 0, 0, PMU_ONOFF_CTL2, 1 },
	{ "aldo3", 700, 3300, 0x21, 0x1f, 100, 0, 0, PMU_ONOFF_CTL2, 2 },
	{ "aldo4", 700, 3300, 0x22, 0x1f, 100, 0, 0, PMU_ONOFF_CTL2, 3 },
	{ "aldo5", 700, 3300, 0x23, 0x1f, 100, 0, 0, PMU_ONOFF_CTL2, 4 },

	{ "bldo1", 700, 3300, 0x24, 0x1f, 100, 0, 0, PMU_ONOFF_CTL2, 5 },
	{ "bldo2", 700, 3300, 0x25, 0x1f, 100, 0, 0, PMU_ONOFF_CTL2, 6 },
	{ "bldo3", 700, 3300, 0x26, 0x1f, 100, 0, 0, PMU_ONOFF_CTL2, 7 },
	{ "bldo4", 700, 3300, 0x27, 0x1f, 100, 0, 0, PMU_ONOFF_CTL3, 0 },
	{ "bldo5", 700, 3300, 0x28, 0x1f, 100, 0, 0, PMU_ONOFF_CTL3, 1 },

	{ "cldo1", 700, 3300, 0x29, 0x1f, 100, 0, 0, PMU_ONOFF_CTL3, 2 },
	{ "cldo2", 700, 3300, 0x2a, 0x1f, 100, 0, 0, PMU_ONOFF_CTL3, 3 },
	{ "cldo3", 700, 3300, 0x2b, 0x1f, 100, 0, 0, PMU_ONOFF_CTL3, 4 },
	{ "cldo4", 700, 4200, 0x2d, 0x3f, 100, 0, 0, PMU_ONOFF_CTL3, 5 },

	{ "cpusldo", 700, 1400, 0x2e, 0x0f, 50, 0, 0, PMU_ONOFF_CTL3, 6 },
	{ "dc1sw", 1500, 3400, 0x13, 0x1f, 100, 0, 0, PMU_ONOFF_CTL3, 7 },
};

static axp_contrl_info *get_ctrl_info_from_tbl(char *name)
{
	int i    = 0;
	int size = ARRAY_SIZE(pmu_axp858_ctrl_tbl);
	axp_contrl_info *p;

	for (i = 0; i < size; i++) {
		if (!strncmp(name, pmu_axp858_ctrl_tbl[i].name,
			     strlen(pmu_axp858_ctrl_tbl[i].name))) {
			break;
		}
	}
	if (i >= size) {
		axp_err("can't find %s from table\n", name);
		return NULL;
	}
	p = pmu_axp858_ctrl_tbl + i;
	return p;
}

static int pmu_axp858_ap_reset_enable(void)
{
	u8 reg_value;

	if (pmic_bus_read(AXP858_RUNTIME_ADDR, PMU_POWER_DISABLE_DOWN,
			  &reg_value))
		return -1;

	reg_value |= 1 << 4;
	if (pmic_bus_write(AXP858_RUNTIME_ADDR, PMU_POWER_DISABLE_DOWN,
			   reg_value))
		return -1;

	return 0;
}

static int pmu_axp858_probe(void)
{
	u8 pmu_chip_id;
	if (pmic_bus_init(AXP858_DEVICE_ADDR, AXP858_RUNTIME_ADDR)) {
		tick_printf("%s pmic_bus_init fail\n", __func__);
		return -1;
	}
	if (pmic_bus_read(AXP858_RUNTIME_ADDR, AXP858_CHIP_ID, &pmu_chip_id)) {
		tick_printf("%s pmic_bus_read fail\n", __func__);
		return -1;
	}
	pmu_chip_id &= 0XCF;
	if (pmu_chip_id == 0x44) {
		/*pmu type AXP858*/
		pmu_axp858_ap_reset_enable();
		tick_printf("PMU: AXP858\n");
		return 0;
	}
	return -1;
}

static int pmu_axp858_set_voltage(char *name, uint set_vol, uint onoff)
{
	u8 reg_value;
	axp_contrl_info *p_item = NULL;
	u8 base_step;

	p_item = get_ctrl_info_from_tbl(name);
	if (!p_item) {
		return -1;
	}

	axp_info(
		"name %s, min_vol %dmv, max_vol %d, cfg_reg 0x%x, cfg_mask 0x%x \
		step0_val %d, split1_val %d, step1_val %d, ctrl_reg_addr 0x%x, ctrl_bit_ofs %d\n",
		p_item->name, p_item->min_vol, p_item->max_vol,
		p_item->cfg_reg_addr, p_item->cfg_reg_mask, p_item->step0_val,
		p_item->split1_val, p_item->step1_val, p_item->ctrl_reg_addr,
		p_item->ctrl_bit_ofs);

	if (set_vol > 0) {
		if (set_vol < p_item->min_vol) {
			set_vol = p_item->min_vol;
		} else if (set_vol > p_item->max_vol) {
			set_vol = p_item->max_vol;
		}
		if (pmic_bus_read(AXP858_RUNTIME_ADDR, p_item->cfg_reg_addr,
				  &reg_value)) {
			return -1;
		}

		reg_value &= ~p_item->cfg_reg_mask;
		if (p_item->split1_val && (set_vol > p_item->split1_val)) {
			if (p_item->split1_val < p_item->min_vol) {
				axp_err("bad split val(%d) for %s\n",
					p_item->split1_val, name);
			}

			base_step = (p_item->split1_val - p_item->min_vol) /
				    p_item->step0_val;
			reg_value |= (base_step +
				      (set_vol - p_item->split1_val) /
					      p_item->step1_val);
		} else {
			reg_value |=
				(set_vol - p_item->min_vol) / p_item->step0_val;
		}

		if (pmic_bus_write(AXP858_RUNTIME_ADDR, p_item->cfg_reg_addr,
				   reg_value)) {
			axp_err("unable to set %s\n", name);
			return -1;
		}
	}

	if (onoff < 0) {
		return 0;
	}
	if (pmic_bus_read(AXP858_RUNTIME_ADDR, p_item->ctrl_reg_addr,
			  &reg_value)) {
		return -1;
	}
	if (onoff == 0) {
		reg_value &= ~(1 << p_item->ctrl_bit_ofs);
	} else {
		reg_value |= (1 << p_item->ctrl_bit_ofs);
	}
	if (pmic_bus_write(AXP858_RUNTIME_ADDR, p_item->ctrl_reg_addr,
			   reg_value)) {
		axp_err("unable to onoff %s\n", name);
		return -1;
	}
	return 0;
}

static int pmu_axp858_get_voltage(char *name)
{
	u8 reg_value;
	axp_contrl_info *p_item = NULL;
	u8 base_step;
	int vol;

	p_item = get_ctrl_info_from_tbl(name);
	if (!p_item) {
		return -1;
	}

	if (pmic_bus_read(AXP858_RUNTIME_ADDR, p_item->ctrl_reg_addr,
			  &reg_value)) {
		return -1;
	}
	if (!(reg_value & (0x01 << p_item->ctrl_bit_ofs))) {
		return 0;
	}

	if (pmic_bus_read(AXP858_RUNTIME_ADDR, p_item->cfg_reg_addr,
			  &reg_value)) {
		return -1;
	}
	reg_value &= p_item->cfg_reg_mask;
	if (p_item->split1_val) {
		base_step = (p_item->split1_val - p_item->min_vol) /
			    p_item->step0_val;
		if (reg_value > base_step) {
			vol = p_item->split1_val +
			      p_item->step1_val * (reg_value - base_step);
		} else {
			vol = p_item->min_vol + p_item->step0_val * reg_value;
		}
	} else {
		vol = p_item->min_vol + p_item->step0_val * reg_value;
	}
	return vol;
}

static int pmu_axp858_set_power_off(void)
{
	u8 reg_value;
	if (pmic_bus_read(AXP858_RUNTIME_ADDR, PMU_POWER_DISABLE_DOWN,
			  &reg_value)) {
		return -1;
	}
	reg_value |= 1 << 7;
	if (pmic_bus_write(AXP858_RUNTIME_ADDR, PMU_POWER_DISABLE_DOWN,
			   reg_value)) {
		return -1;
	}
	return 0;
}

static int pmu_axp858_set_sys_mode(int status)
{
	if (pmic_bus_write(AXP858_RUNTIME_ADDR, PMU_DATA_BUFFER0, (u8)status)) {
		return -1;
	}
	return 0;
}

static int pmu_axp858_get_sys_mode(void)
{
	u8 reg_value;
	if (pmic_bus_read(AXP858_RUNTIME_ADDR, PMU_DATA_BUFFER0, &reg_value)) {
		return -1;
	}
	return reg_value;
}

static int pmu_axp858_get_key_irq(void)
{
	u8 reg_value;
	if (pmic_bus_read(AXP858_RUNTIME_ADDR, PMU_IRQ_STATU2, &reg_value)) {
		return -1;
	}
	reg_value &= (0x03);
	if (reg_value) {
		if (pmic_bus_write(AXP858_RUNTIME_ADDR, PMU_IRQ_STATU2,
				   reg_value)) {
			return -1;
		}
	}
	return (reg_value)&3;
}

unsigned char pmu_axp858_get_reg_value(unsigned char reg_addr)
{
	u8 reg_value;
	if (pmic_bus_read(AXP858_RUNTIME_ADDR, reg_addr, &reg_value)) {
		return -1;
	}
	return reg_value;
}

unsigned char pmu_axp858_set_reg_value(unsigned char reg_addr, unsigned char reg_value)
{
	unsigned char reg;
	if (pmic_bus_write(AXP858_RUNTIME_ADDR, reg_addr, reg_value)) {
		return -1;
	}
	if (pmic_bus_read(AXP858_RUNTIME_ADDR, reg_addr, &reg)) {
		return -1;
	}
	return reg;
}


U_BOOT_AXP_PMU_INIT(pmu_axp858) = {
	.pmu_name      = "pmu_axp858",
	.probe	 = pmu_axp858_probe,
	.set_voltage   = pmu_axp858_set_voltage,
	.get_voltage   = pmu_axp858_get_voltage,
	.set_power_off = pmu_axp858_set_power_off,
	.set_sys_mode  = pmu_axp858_set_sys_mode,
	.get_sys_mode  = pmu_axp858_get_sys_mode,
	.get_key_irq   = pmu_axp858_get_key_irq,
	/*.set_bus_vol_limit	= pmu_axp858_set_bus_vol_limit, */
	.get_reg_value = pmu_axp858_get_reg_value,
	.set_reg_value = pmu_axp858_set_reg_value,

};
