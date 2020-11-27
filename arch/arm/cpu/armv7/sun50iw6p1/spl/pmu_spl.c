/*
 * (C) Copyright 2016
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *zhouhuacai <zhouhuacai@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */


#include <common.h>
#include <asm/arch/base_pmu.h>
#include <power/sunxi/axp806_reg.h>
#include <asm/arch/platsmp.h>

int pmu_type;

#define dbg(format,arg...)	printf(format,##arg)

#define DUMMY_MODE		(1)
#define VOL_ON			(1)
#define EFUSE_TF_ZONE		(0x1c)
#define BIN_OFFSET		(5)
#define BIN_MASK		(7)
#ifdef CONFIG_SUNXI_MODULE_AXP
static int axp_probe(void)
{
	u8  pmu_type;

	if(axp_i2c_read(CONFIG_SYS_I2C_SLAVE, BOOT_POWER806_VERSION, &pmu_type))
	{
		printf("axp read error\n");

		return -1;
	}
	pmu_type &= 0xCF;
	if(pmu_type == 0x40)
	{
		/* pmu type AXP806 */
		printf("PMU: AXP806\n");
		return AXP806_ADDR;
	}
	else
	{
		printf("unknow PMU\n");
		return -1;
	}

}

static int axp806_set_dcdca(int vol)
{
	int      set_vol = vol;
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
		if(axp_i2c_read(CONFIG_SYS_I2C_SLAVE, BOOT_POWER806_DCAOUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0x80;
		reg_value |= tmp_step;
		if(axp_i2c_write(CONFIG_SYS_I2C_SLAVE, BOOT_POWER806_DCAOUT_VOL, reg_value))
		{
			printf("sunxi pmu error : unable to set dcdc1\n");

			return -1;
		}
	}
	return 0;
}

static int axp806_set_dcdce(int vol)
{
	int ret = -1;
	unsigned char  vol_set;

	if(vol>=1600)
	{
		vol_set = (vol - 1500) / 100;
		vol_set +=45 ;
	}
	else
	{
		vol_set=(vol - 600)/20;
	}

	vol_set=(vol - 1100)/100;
	ret = axp_i2c_write(CONFIG_SYS_I2C_SLAVE, BOOT_POWER806_DCEOUT_VOL,vol_set);
	printf("PMU:Set DDR Vol %dmV %s.\n", vol,ret==0?"OK":"Fail");

	return ret;
}

static int axp806_set_dcdcd(int set_vol, int onoff)
{
	u8	 reg_value;
	u8	 tmp_step;

	if (set_vol > 0) {
		if (set_vol <= 1500) {
			if (set_vol < 600)
				set_vol = 600;
			tmp_step = (set_vol - 600) / 20;
		} else {
			if (set_vol < 1600)
				set_vol = 1600;
			else if (set_vol > 3300)
				set_vol = 3300;

			tmp_step = (set_vol - 1600) / 100 + 47;
		}

		if (axp_i2c_read(AXP806_ADDR,
				BOOT_POWER806_DCDOUT_VOL, &reg_value)) {
			debug("%d\n", __LINE__);

			return -1;
		}
		reg_value &= 0xC0;
		reg_value |= tmp_step;
		if (axp_i2c_write(AXP806_ADDR,
				BOOT_POWER806_DCDOUT_VOL, reg_value)) {
			printf("sunxi pmu error : unable to set dcdcd\n");

			return -1;
		}
	}

	if (onoff < 0)
		return 0;

	if (axp_i2c_read(AXP806_ADDR,
			BOOT_POWER806_OUTPUT_CTL1, &reg_value)) {
		return -1;
	}
	if (onoff == 0)
		reg_value &= ~(1 << 3);
			reg_value |=  (1 << 3);

	if (axp_i2c_write(AXP806_ADDR,
			BOOT_POWER806_OUTPUT_CTL1, reg_value)) {
		printf("sunxi pmu error : unable to onoff dcdcd\n");

		return -1;
	}

	return 0;
}

static void set_vdd_sys_by_bin(void)
{
	u32 bin;
	int set_vol;

	bin = (sid_read_key(EFUSE_TF_ZONE) >> BIN_OFFSET) & BIN_MASK;
	switch (bin)  {
	case 0b001:
		set_vol = 980;
		break;
	case 0b011:
		set_vol = 940;
		break;
	case 0b010:
	default:
		set_vol = 960;
		break;
	}
	printf("vdd-sys vol:%dmv\n", set_vol);
	axp806_set_dcdcd(set_vol, VOL_ON);

}
#endif
u8 vdd_port, vdd_port_num;

void get_cpus_vdd_gpio(u8 reserve0, u8 reserve1)
{
	vdd_port = reserve0;
	vdd_port_num = reserve1;
}

/*
 * PL0 set to 0, when boot setup, and vdd-sys vol will
 * set to 0.98v form 0.8v.
 *
 */
static void set_vdd_sys_by_PL0(void)
{
	volatile unsigned int reg_val;
	int n = 0;
	debug("vdd_port = %d  pin_cfg = %d\n", vdd_port, vdd_port_num);
	if (vdd_port == 12)
		n = 0;
	else if (vdd_port == 13)
		n = 1;

	reg_val = readl(SUNXI_RPIO_BASE + (n * 0x24) + (vdd_port_num >> 3));
	reg_val &= ~(0x7 << ((vdd_port_num % 8)*4));
	reg_val |= (0x1 << ((vdd_port_num % 8)*4));
	writel(reg_val, SUNXI_RPIO_BASE +  (n * 0x24) + (vdd_port_num >> 3));

	__usdelay(10);

	reg_val = readl(SUNXI_RPIO_BASE + (n * 0x24) + 0x10);
	reg_val &= (~0x1);
	writel(reg_val, SUNXI_RPIO_BASE + (n * 0x24) + 0x10);

	__usdelay(10);

	reg_val = readl(SUNXI_RPIO_BASE +  (n * 0x24) + (vdd_port_num >> 3));
	debug("P%c%d: 0x%x\n", n+'L', vdd_port_num, reg_val);
	reg_val = readl(SUNXI_RPIO_BASE + (n * 0x24) + 0x10);
	debug("P%c data: 0x%x\n",  n+'L', reg_val);
	__usdelay(10);
}

int probe_power_key(void)
{
	return 0;
}


int set_ddr_voltage(int set_vol)
{
#ifdef CONFIG_SUNXI_MODULE_AXP
	if(pmu_type == AXP806_ADDR)
	{
		return axp806_set_dcdce(set_vol);
	}
#endif
	return 0;
}

int set_pll_voltage(int set_vol)
{
#ifdef CONFIG_SUNXI_MODULE_AXP
	if(pmu_type == AXP806_ADDR)
	{
		return axp806_set_dcdca(set_vol);
	}
#endif
	return 0;
}
int axp_set_aldo3(int set_vol, int onoff)
{
#ifdef CONFIG_SUNXI_MODULE_AXP
	u8 reg_value;

	if(set_vol > 0) {
		if(set_vol < 700)
			set_vol = 700;
		else if(set_vol > 3300)
			set_vol = 3300;

		if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_ALDO3OUT_VOL, &reg_value))
			return -1;

		reg_value &= 0xE0;
		reg_value |= ((set_vol - 700)/100);
		if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_ALDO3OUT_VOL, reg_value)) {
			printf("sunxi pmu error : unable to set aldo3\n");
			return -1;
		}
	}

	if(axp_i2c_read(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, &reg_value))
		return -1;

	if(onoff == 0)
		reg_value &= ~(1 << 7);
	else
		reg_value |=  (1 << 7);

	if(axp_i2c_write(AXP806_ADDR, BOOT_POWER806_OUTPUT_CTL1, reg_value)) {
		printf("sunxi pmu error : unable to onoff aldo3\n");
		return -1;
	}
#endif
	return 0;
}

static void sunxi_set_all_cpu_off(void)
{
	int off = 0, cluster = 0;
	u32 value;
	value = readl(SUNXI_CLUSTER_PWROFF_GATING(0));
	value |= ((0x1<<1)|(0x1<<2)|(0x1<<3));
	writel(value, SUNXI_CLUSTER_PWROFF_GATING(0));
	cpu_power_switch_set(cluster, 1, off);
	cpu_power_switch_set(cluster, 2, off);
	cpu_power_switch_set(cluster, 3, off);
}

static inline void disable_pmu_pfm_mode(void)
{
	u8 val;
	axp_i2c_read(CONFIG_SYS_I2C_SLAVE, BOOT_POWER806_DCMOD_CTL2, &val);
	val |= 0x1f;
	axp_i2c_write(CONFIG_SYS_I2C_SLAVE, BOOT_POWER806_DCMOD_CTL2, val);
}

int pmu_init(u8 power_mode)
{
	sunxi_set_all_cpu_off();
	if (power_mode == DUMMY_MODE)
	{
		pmu_type = 0;
		set_vdd_sys_by_PL0();
	}
#ifdef CONFIG_SUNXI_MODULE_AXP
	else
	{
		i2c_init_cpus(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);

		pmu_type = axp_probe();
		if (AXP806_ADDR == pmu_type) {
			disable_pmu_pfm_mode();
			/*set vdd-sys before dram init*/
			set_vdd_sys_by_bin();
		}
	}
#endif
	return pmu_type;
}


