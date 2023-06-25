/*
 * Copyright (C) 2019 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SUNXI AXP21  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <sunxi_power/pmu_axp806.h>
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
	u32 step2_val;
	u32 split2_val;
} axp_contrl_info;

__attribute__((section(".data"))) axp_contrl_info pmu_axp806_ctrl_tbl[] = {
	/*name,    min,  max, reg,  mask, step0,split1_val, step1,ctrl_reg,ctrl_bit */
	{ "dcdca", 600,  1520, AXP806_DCAOUT_VOL, 0x7f, 10, 1120, 20, AXP806_OUTPUT_CTL1, 0 },
	{ "dcdcb", 1000, 2550, AXP806_DCBOUT_VOL, 0x1f, 50, 0, 0, AXP806_OUTPUT_CTL1, 1 },
	{ "dcdcc", 600,  1520, AXP806_DCCOUT_VOL, 0x7f, 10, 1120, 20, AXP806_OUTPUT_CTL1, 2 },
	{ "dcdcd", 600,  3300, AXP806_DCDOUT_VOL, 0x3f, 20, 1600, 100, AXP806_OUTPUT_CTL1, 3 },
	{ "dcdce", 1100, 3400, AXP806_DCEOUT_VOL, 0x1f, 100, 0, 0, AXP806_OUTPUT_CTL1, 4 },
	{ "aldo1", 700, 3300, AXP806_ALDO1OUT_VOL, 0x1f, 100, 0, 0, AXP806_OUTPUT_CTL1, 5 },
	{ "aldo2", 700, 3400, AXP806_ALDO2OUT_VOL, 0x1f, 100, 0, 0, AXP806_OUTPUT_CTL1, 6 },
	{ "aldo3", 700, 3300, AXP806_ALDO3OUT_VOL, 0x1f, 100, 0, 0, AXP806_OUTPUT_CTL1, 7 },
	{ "bldo1", 700, 1900, AXP806_BLDO1OUT_VOL, 0x0f, 100, 0, 0, AXP806_OUTPUT_CTL2, 0 },
	{ "bldo2", 700, 1900, AXP806_BLDO2OUT_VOL, 0x0f, 100, 0, 0, AXP806_OUTPUT_CTL2, 1 },
	{ "bldo3", 700, 1900, AXP806_BLDO3OUT_VOL, 0x0f, 100, 0, 0, AXP806_OUTPUT_CTL2, 2 },
	{ "bldo4", 700, 1900, AXP806_BLDO4OUT_VOL, 0x0f, 100, 0, 0, AXP806_OUTPUT_CTL2, 3 },
	{ "cldo1", 700, 3300, AXP806_CLDO1OUT_VOL, 0x1f, 100, 0, 0, AXP806_OUTPUT_CTL2, 4 },
	{ "cldo2", 700, 4200, AXP806_CLDO2OUT_VOL, 0x1f, 100, 3600, 200, AXP806_OUTPUT_CTL2, 5 },
	{ "cldo3", 700, 3300, AXP806_CLDO3OUT_VOL, 0x1f, 100, 0, 0, AXP806_OUTPUT_CTL2, 6 },
	{ "dc1sw", 1100, 3400, AXP806_DCEOUT_VOL, 0x1f, 100, 0, 0, AXP806_OUTPUT_CTL2, 7 },
};

static axp_contrl_info *get_ctrl_info_from_tbl(char *name)
{
	int i    = 0;
	int size = ARRAY_SIZE(pmu_axp806_ctrl_tbl);
	axp_contrl_info *p;

	for (i = 0; i < size; i++) {
		if (!strncmp(name, pmu_axp806_ctrl_tbl[i].name,
			     strlen(pmu_axp806_ctrl_tbl[i].name))) {
			break;
		}
	}
	if (i >= size) {
		axp_err("can't find %s from table\n", name);
		return NULL;
	}
	p = pmu_axp806_ctrl_tbl + i;
	return p;
}


static int pmu_axp806_probe(void)
{
	u8 pmu_chip_id;
	if (pmic_bus_init(AXP806_DEVICE_ADDR, AXP806_RUNTIME_ADDR)) {
		tick_printf("%s pmic_bus_init fail\n", __func__);
		return -1;
	}
	if (pmic_bus_read(AXP806_RUNTIME_ADDR, AXP806_VERSION, &pmu_chip_id)) {
		tick_printf("%s pmic_bus_read fail\n", __func__);
		return -1;
	}
	pmu_chip_id &= 0XCF;
	if (pmu_chip_id == AXP806_CHIP_ID) {
		/*pmu type AXP806*/
		tick_printf("PMU: AXP806\n");
		return 0;
	}
	return -1;
}

static int pmu_axp806_get_info(char *name, unsigned char *chipid)
{
	strncpy(name, "axp806", sizeof("axp806"));
	*chipid = AXP806_CHIP_ID;
	return 0;
}


static int pmu_axp806_set_voltage(char *name, uint set_vol, uint onoff)
{
	u8 reg_value;
	axp_contrl_info *p_item = NULL;
	u8 base_step		= 0;

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

	if ((set_vol > 0) && (p_item->min_vol)) {
		if (set_vol < p_item->min_vol) {
			set_vol = p_item->min_vol;
		} else if (set_vol > p_item->max_vol) {
			set_vol = p_item->max_vol;
		}
		if (pmic_bus_read(AXP806_RUNTIME_ADDR, p_item->cfg_reg_addr,
				  &reg_value)) {
			return -1;
		}

		reg_value &= ~p_item->cfg_reg_mask;
		if (p_item->split2_val && (set_vol > p_item->split2_val)) {
			base_step = (p_item->split2_val - p_item->split1_val) /
				    p_item->step1_val;

			base_step += (p_item->split1_val - p_item->min_vol) /
				     p_item->step0_val;
			reg_value |= (base_step +
				      (set_vol - p_item->split2_val/p_item->step2_val*p_item->step2_val) /
					      p_item->step2_val);
		} else if (p_item->split1_val &&
			   (set_vol > p_item->split1_val)) {
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
		if (pmic_bus_write(AXP806_RUNTIME_ADDR, p_item->cfg_reg_addr,
				   reg_value)) {
			axp_err("unable to set %s\n", name);
			return -1;
		}
	}

	if (onoff < 0) {
		return 0;
	}
	if (pmic_bus_read(AXP806_RUNTIME_ADDR, p_item->ctrl_reg_addr,
			  &reg_value)) {
		return -1;
	}
	if (onoff == 0) {
		reg_value &= ~(1 << p_item->ctrl_bit_ofs);
	} else {
		reg_value |= (1 << p_item->ctrl_bit_ofs);
	}
	if (pmic_bus_write(AXP806_RUNTIME_ADDR, p_item->ctrl_reg_addr,
			   reg_value)) {
		axp_err("unable to onoff %s\n", name);
		return -1;
	}
	return 0;
}

static int pmu_axp806_get_voltage(char *name)
{
	u8 reg_value;
	axp_contrl_info *p_item = NULL;
	u8 base_step;
	int vol;

	p_item = get_ctrl_info_from_tbl(name);
	if (!p_item) {
		return -1;
	}

	if (pmic_bus_read(AXP806_RUNTIME_ADDR, p_item->ctrl_reg_addr,
			  &reg_value)) {
		return -1;
	}
	if (!(reg_value & (0x01 << p_item->ctrl_bit_ofs))) {
		return 0;
	}

	if (pmic_bus_read(AXP806_RUNTIME_ADDR, p_item->cfg_reg_addr,
			  &reg_value)) {
		return -1;
	}
	reg_value &= p_item->cfg_reg_mask;
	if (p_item->split2_val) {
		u32 base_step2;
		base_step = (p_item->split1_val - p_item->min_vol) /
				     p_item->step0_val;

		base_step2 = base_step + (p_item->split2_val - p_item->split1_val) /
			    p_item->step1_val;

		if (reg_value >= base_step2) {
			vol = ALIGN(p_item->split2_val, p_item->step2_val) +
			      p_item->step2_val * (reg_value - base_step2);
		} else if (reg_value >= base_step) {
			vol = p_item->split1_val +
			      p_item->step1_val * (reg_value - base_step);
		} else {
			vol = p_item->min_vol + p_item->step0_val * reg_value;
		}
	} else if (p_item->split1_val) {
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
#if 0
static int pmu_axp806_set_power_off(void)
{
	u8 reg_value;
	if (pmic_bus_read(AXP806_RUNTIME_ADDR, AXP806_OFF_CTL, &reg_value)) {
		return -1;
	}
	reg_value |= 1;
	if (pmic_bus_write(AXP806_RUNTIME_ADDR, AXP806_OFF_CTL, reg_value)) {
		return -1;
	}
	return 0;
}

static int pmu_axp806_set_sys_mode(int status)
{
	if (pmic_bus_write(AXP806_RUNTIME_ADDR, AXP806_DATA_BUFFER3,
			   (u8)status)) {
		return -1;
	}
	return 0;
}

static int pmu_axp806_get_sys_mode(void)
{
	u8 reg_value;
	if (pmic_bus_read(AXP806_RUNTIME_ADDR, AXP806_DATA_BUFFER3, &reg_value)) {
		return -1;
	}
	return reg_value;
}

static int pmu_axp806_get_key_irq(void)
{
	u8 reg_value;
	if (pmic_bus_read(AXP806_RUNTIME_ADDR, AXP806_INTSTS1, &reg_value)) {
		return -1;
	}
	reg_value &= (0x03 << 2);
	if (reg_value) {
		if (pmic_bus_write(AXP806_RUNTIME_ADDR, AXP806_INTSTS1,
				   reg_value)) {
			return -1;
		}
	}
	return (reg_value >> 2) & 3;
}

int pmu_axp806_set_bus_vol_limit(int vol)
{
	uchar reg_value;

	if (!vol)
		return 0;

	/* set bus vol limit off */
	if (pmic_bus_read(AXP806_RUNTIME_ADDR, AXP806_VBUS_VOL_SET, &reg_value)) {
		return -1;
	}
	reg_value &= 0xf0;

	if (vol < 3880) {
		vol = 3880;
	} else if (vol > 5080) {
		vol = 5080;
	}
	reg_value |= (vol - 3880) / 80;
	if (pmic_bus_write(AXP806_RUNTIME_ADDR, AXP806_VBUS_VOL_SET, reg_value)) {
		return -1;
	}

	return 0;
}
#endif

static int pmu_axp806_set_dcdc_mode(const char *name, int mode)
{
	u8 reg_value = 0, mask = 0;

	if (!strncmp(name, "dcdca_mode", sizeof("dcdca_mode")))
		mask = AXP806_DCDCA_PWM_BIT;

	if (!strncmp(name, "dcdcb_mode", sizeof("dcdcb_mode")))
		mask = AXP806_DCDCB_PWM_BIT;

	if (!strncmp(name, "dcdcc_mode", sizeof("dcdcc_mode")))
		mask = AXP806_DCDCC_PWM_BIT;

	if (!strncmp(name, "dcdcd_mode", sizeof("dcdcd_mode")))
		mask = AXP806_DCDCD_PWM_BIT;

	if (!strncmp(name, "dcdce_mode", sizeof("dcdce_mode")))
		mask = AXP806_DCDCE_PWM_BIT;

	if (pmic_bus_read(AXP806_RUNTIME_ADDR, AXP806_DCDC_MODESET, &reg_value))
		return -1;

	reg_value &= ~(1 << mask);
	reg_value |= (mode << mask);

	if (pmic_bus_write(AXP806_RUNTIME_ADDR, AXP806_DCDC_MODESET, reg_value))
		return -1;

	return 0;
}

unsigned char pmu_axp806_get_reg_value(unsigned char reg_addr)
{
	u8 reg_value;
	if (pmic_bus_read(AXP806_RUNTIME_ADDR, reg_addr, &reg_value)) {
		return -1;
	}
	return reg_value;
}

unsigned char pmu_axp806_set_reg_value(unsigned char reg_addr, unsigned char reg_value)
{
	unsigned char reg;
	if (pmic_bus_write(AXP806_RUNTIME_ADDR, reg_addr, reg_value)) {
		return -1;
	}
	if (pmic_bus_read(AXP806_RUNTIME_ADDR, reg_addr, &reg)) {
		return -1;
	}
	return reg;
}

U_BOOT_AXP_PMU_INIT(pmu_axp806) = {
	.pmu_name	  = "pmu_axp806",
	.get_info		= pmu_axp806_get_info,
	.probe		   = pmu_axp806_probe,
	.set_voltage       = pmu_axp806_set_voltage,
	.get_voltage       = pmu_axp806_get_voltage,
	/*.set_power_off     = pmu_axp806_set_power_off,
	.set_sys_mode      = pmu_axp806_set_sys_mode,
	.get_sys_mode      = pmu_axp806_get_sys_mode,
	.get_key_irq       = pmu_axp806_get_key_irq,
	.set_bus_vol_limit = pmu_axp806_set_bus_vol_limit,*/
	.set_dcdc_mode     = pmu_axp806_set_dcdc_mode,
	.get_reg_value	   = pmu_axp806_get_reg_value,
	.set_reg_value	   = pmu_axp806_set_reg_value,
};

