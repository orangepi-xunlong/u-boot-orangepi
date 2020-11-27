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
#include <power/sunxi/axp858_reg.h>

#define pmu_err(format, arg...)	printf("[pmu]: "format, ##arg)
#define pmu_info(format, arg...)	/*printf("[pmu]: "format,##arg)*/

#define DUMMY_MODE		(1)

typedef struct _axp_contrl_info
{
	char name[8];

	u16 min_vol;
	u16 max_vol;
	u16 cfg_reg_addr;
	u16 cfg_reg_mask;

	u16 step0_val;
	u16 split1_val;
	u16 step1_val;
	u16 ctrl_reg_addr;

	u16 ctrl_bit_ofs;
	u16 res;
} axp_contrl_info;

static int pmu_set_vol(char *name, int set_vol, int onoff);
axp_contrl_info axp_ctrl_tbl[] = { \
/*   name        min,     max,    reg,    mask,  step0,   split1_val,  step1,   ctrl_reg,           ctrl_bit */

	{"dcdc2",	500,  1540, 0x14,  0x7f,   10,	1220,		20,   PMU_DC2OUT_VOL, 1},
	{"dcdc5",	800,  1840, 0x17,  0x7f,   10,	1120,		20,   PMU_DC5OUT_VOL, 4},

};
#define PMU_POWER_KEY_STATUS PMU_IRQ_STATU2
#define PMU_POWER_KEY_OFFSET 0x0

static axp_contrl_info *get_ctrl_info_from_tbl(char *name)
{
	int i = 0;
	int size = ARRAY_SIZE(axp_ctrl_tbl);
	axp_contrl_info *p;

	for (i = 0; i < size; i++) {
		if (!strncmp(name, axp_ctrl_tbl[i].name, strlen(axp_ctrl_tbl[i].name))) {
			break;
		}
	}
	if (i >= size) {
		return NULL;
	}
	p = axp_ctrl_tbl + i;
	return p;
}


#ifdef CONFIG_AXP_USE_RSB
#define RSB_SLAVE_ADDR   (0x745)
#define RSB_RUNTIME_ADDR (0x2d)

#define PMU_IC_TYPY_REG     PMU_IC_TYPE

extern s32 rsb_init(u32 saddr, u32 rtaddr);
extern s32 rsb_exit(void);
extern s32 rsb_write(u32 rtsaddr, u32 daddr, u8 *data, u32 len);
extern s32 rsb_read(u32 rtsaddr, u32 daddr, u8 *data, u32 len);

int pwrok_restart_enable(void)
{
	u8 reg_val = 0;
	if (pmu_bus_read(RSB_RUNTIME_ADDR, PMU_POWER_DISABLE_DOWN, &reg_val)) {
		return -1;
	}
	/* PWROK drive low restart function enable  */
	/* for watchdog reset */
	reg_val |= (0x1U << 6);
	if (pmu_bus_write(RSB_RUNTIME_ADDR, PMU_POWER_DISABLE_DOWN, reg_val)) {
		return -1;
	}
	return 0;
}

static inline void disable_dcdc_pfm_mode(void)
{
	u8 val;

	pmu_bus_read(RSB_RUNTIME_ADDR, 0x80, &val);
	val |= (0x01 << 3); /*dcdc4 for gpu pwm mode*/
	val |= (0x01 << 4); /*dcdc5 for dram pwm mode*/
	pmu_bus_write(RSB_RUNTIME_ADDR, 0x80, val);

	/* disable dcm mode for GPU stability Vdrop issue*/
	pmu_bus_write(RSB_RUNTIME_ADDR, 0xff, 0x0);
	pmu_bus_write(RSB_RUNTIME_ADDR, 0xf4, 0x6);
	pmu_bus_write(RSB_RUNTIME_ADDR, 0xf2, 0x4);
	pmu_bus_write(RSB_RUNTIME_ADDR, 0xf5, 0x4);
	pmu_bus_write(RSB_RUNTIME_ADDR, 0xff, 0x1);
	pmu_bus_write(RSB_RUNTIME_ADDR, 0x12, 0x40);
	pmu_bus_write(RSB_RUNTIME_ADDR, 0xff, 0x0);

}

int pmu_init(u8 power_mode)
{
	u8    pmu_type;

	if (pmu_bus_init()) {
		pmu_err("bus init error\n");
		return -1;
	}

	if (pmu_bus_read(RSB_RUNTIME_ADDR, PMU_IC_TYPY_REG, &pmu_type)) {
		pmu_err("bus read error\n");
		return -1;
	}

	pmu_type &= 0xCF;
	if (pmu_type == 0x44) {
		/* pmu type AXP858 */
		printf("PMU: AXP858\n");
		/*pwrok_restart_enable();*/
		/*disable_dcdc_pfm_mode();*/
		return AXP858_ADDR;
	}

	printf("unknow PMU\n");
	return -1;

}

s32 pmu_bus_init(void)
{
	return rsb_init(RSB_SLAVE_ADDR,  RSB_RUNTIME_ADDR);
}

s32 pmu_bus_read(u32 rtaddr, u32 daddr, u8 *data)
{
	return rsb_read(rtaddr, daddr, data, 1);
}

s32 pmu_bus_write(u32 rtaddr, u32 daddr, u8 data)
{
	return rsb_write(rtaddr, daddr, &data, 1);
}

int probe_power_key(void)
{
	u8  reg_value;

	if (pmu_bus_read(RSB_RUNTIME_ADDR, PMU_POWER_KEY_STATUS, &reg_value)) {
		return -1;
	}
	/* POKLIRQ,POKSIRQ */
	reg_value &= (0x03<<PMU_POWER_KEY_OFFSET);
	if (reg_value) {
		if (pmu_bus_write(RSB_RUNTIME_ADDR, PMU_POWER_KEY_STATUS, reg_value)) {
			return -1;
		}
	}

	return  (reg_value>>PMU_POWER_KEY_OFFSET)&3;
}

static int pmu_set_vol(char *name, int set_vol, int onoff)
{
	u8   reg_value;
	axp_contrl_info *p_item = NULL;
	u8 base_step;

	if (set_vol <= 0)
		return 0;

	p_item = get_ctrl_info_from_tbl(name);
	if (!p_item) {
		return -1;
	}

	pmu_info("name %s, min_vol %dmv, max_vol %d, cfg_reg 0x%x, cfg_mask 0x%x \
		step0_val %d, split1_val %d, step1_val %d, ctrl_reg_addr 0x%x, ctrl_bit_ofs %d\n",
	p_item->name,
	p_item->min_vol,
	p_item->max_vol,
	p_item->cfg_reg_addr,
	p_item->cfg_reg_mask,
	p_item->step0_val,
	p_item->split1_val,
	p_item->step1_val,
	p_item->ctrl_reg_addr,
	p_item->ctrl_bit_ofs);

	if (set_vol < p_item->min_vol) {
		set_vol = p_item->min_vol;
	} else if (set_vol > p_item->max_vol) {
		set_vol = p_item->max_vol;
	}
	if (pmu_bus_read(RSB_RUNTIME_ADDR, p_item->cfg_reg_addr, &reg_value)) {
		return -1;
	}

	reg_value &= ~p_item->cfg_reg_mask;
	if (p_item->split1_val && (set_vol > p_item->split1_val)) {
		if (p_item->split1_val < p_item->min_vol) {
			pmu_err("bad split val(%d) for %s\n", p_item->split1_val, name);
			return -1;
		}

		base_step = (p_item->split1_val - p_item->min_vol)/p_item->step0_val;
		reg_value |= (base_step + (set_vol - p_item->split1_val)/p_item->step1_val);
	} else {
		reg_value |= (set_vol - p_item->min_vol)/p_item->step0_val;
	}

	if (pmu_bus_write(RSB_RUNTIME_ADDR, p_item->cfg_reg_addr, reg_value)) {
		pmu_err("unable to set %s\n", name);
		return -1;
	}

	return 0;
}
#endif

#ifdef CONFIG_AXP_USE_I2C

static int axp_probe(void)
{
	u8  pmu_type;

	if (axp_i2c_read(CONFIG_SYS_I2C_SLAVE, PMU_IC_TYPE, &pmu_type)) {
		printf("axp read error\n");
		return -1;
	}
	pmu_type &= 0xCF;
	if (pmu_type == 0x44) {
		printf("PMU: AXP858\n");
		/*pwrok_restart_enable();*/
		/*disable_dcdc_pfm_mode();*/
		return AXP858_ADDR;
	} else {
		printf("unknow PMU\n");
		return -1;
	}

}

int pmu_init(u8 power_mode)
{
	u8 pmu_type;
	if (power_mode == DUMMY_MODE) {
		pmu_type = 0;
	} else {
		i2c_init_cpus(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
		pmu_type = axp_probe();
	}

	return pmu_type;
}

static int pmu_set_vol(char *name, int set_vol, int onoff)
{
	u8   reg_value;
	axp_contrl_info *p_item = NULL;
	u8 base_step;

	if (set_vol <= 0)
		return 0;

	p_item = get_ctrl_info_from_tbl(name);
	if (!p_item) {
		return -1;
	}

	pmu_info("name %s, min_vol %dmv, max_vol %d, cfg_reg 0x%x, cfg_mask 0x%x \
		step0_val %d, split1_val %d, step1_val %d, ctrl_reg_addr 0x%x, ctrl_bit_ofs %d\n",
	p_item->name,
	p_item->min_vol,
	p_item->max_vol,
	p_item->cfg_reg_addr,
	p_item->cfg_reg_mask,
	p_item->step0_val,
	p_item->split1_val,
	p_item->step1_val,
	p_item->ctrl_reg_addr,
	p_item->ctrl_bit_ofs);

	if (set_vol < p_item->min_vol) {
		set_vol = p_item->min_vol;
	} else if (set_vol > p_item->max_vol) {
		set_vol = p_item->max_vol;
	}
	if (axp_i2c_read(CONFIG_SYS_I2C_SLAVE, p_item->cfg_reg_addr, &reg_value)) {
		pmu_err("axp_i2c_read erro\n");
		return -1;
	}

	reg_value &= ~p_item->cfg_reg_mask;
	if (p_item->split1_val && (set_vol > p_item->split1_val)) {
		if (p_item->split1_val < p_item->min_vol) {
			pmu_err("bad split val(%d) for %s\n", p_item->split1_val, name);
			return -1;
		}

		base_step = (p_item->split1_val - p_item->min_vol)/p_item->step0_val;
		reg_value |= (base_step + (set_vol - p_item->split1_val)/p_item->step1_val);
	} else {
		reg_value |= (set_vol - p_item->min_vol)/p_item->step0_val;
	}

	if (axp_i2c_write(CONFIG_SYS_I2C_SLAVE, p_item->cfg_reg_addr, reg_value)) {
		pmu_err("unable to set %s\n", name);
		return -1;
	}

	return 0;
}

#endif

int set_ddr_voltage(int set_vol)
{
	return pmu_set_vol("dcdc5", set_vol, 1);
}

int set_pll_voltage(int set_vol)
{
	return pmu_set_vol("dcdc2", set_vol, 1);
}




