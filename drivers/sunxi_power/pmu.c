/*
 * Copyright (C) 2019 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SUNXI AXP  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <sunxi_power/axp.h>
#include <asm/arch/pmic_bus.h>


#ifdef AXP_DEBUG
#define axp_err(fmt...) tick_printf("[axp][err]: " fmt)
#else
#define axp_err(fmt...)
#endif


__attribute__((section(".data"))) static struct sunxi_pmu_dev_t *sunxi_pmu_dev =
	NULL;

/* traverse the u-boot segment to find the pmu offset*/
static struct sunxi_pmu_dev_t *pmu_get_axp_dev_t(void)
{
	struct sunxi_pmu_dev_t *sunxi_pmu_dev_temp;
	struct sunxi_pmu_dev_t *sunxi_pmu_dev_start =
		ll_entry_start(struct sunxi_pmu_dev_t, pmu);
	int max = ll_entry_count(struct sunxi_pmu_dev_t, pmu);
	for (sunxi_pmu_dev_temp = sunxi_pmu_dev_start;
	     sunxi_pmu_dev_temp != sunxi_pmu_dev_start + max;
	     sunxi_pmu_dev_temp++) {
		if (!strncmp("pmu", sunxi_pmu_dev_temp->pmu_name, 3)) {
			if (!sunxi_pmu_dev_temp->probe()) {
				pr_msg("PMU: %s found\n",
				       sunxi_pmu_dev_temp->pmu_name);
				return sunxi_pmu_dev_temp;
			}
		}
	}
	pr_msg("PMU: no found\n");
	return NULL;
}

/* matches chipid*/
int pmu_probe(void)
{
	sunxi_pmu_dev = pmu_get_axp_dev_t();
	if (sunxi_pmu_dev == NULL)
		return -1;
	return 0;
}

/*get axp info*/
int pmu_get_info(char *name, unsigned char *chipid)
{
	if ((sunxi_pmu_dev) && (sunxi_pmu_dev->get_info))
		return sunxi_pmu_dev->get_info(name, chipid);
	axp_err("not imple:%s\n", __func__);
	*name = 0;
	*chipid = 0;
	return -1;
}


/*Set a certain power, voltage value. */
int pmu_set_voltage(char *name, uint vol_value, uint onoff)
{
	if ((sunxi_pmu_dev) && (sunxi_pmu_dev->set_voltage))
		return sunxi_pmu_dev->set_voltage(name, vol_value, onoff);
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*Read a certain power, voltage value */
int pmu_get_voltage(char *name)
{
	if ((sunxi_pmu_dev) && (sunxi_pmu_dev->get_voltage))
		return sunxi_pmu_dev->get_voltage(name);
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*Set shutdown*/
int pmu_set_power_off(void)
{
	if ((sunxi_pmu_dev) && (sunxi_pmu_dev->set_power_off))
		return sunxi_pmu_dev->set_power_off();
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*Sets the state of the next mode */
int pmu_set_sys_mode(int status)
{
	if ((sunxi_pmu_dev) && (sunxi_pmu_dev->set_sys_mode))
		return sunxi_pmu_dev->set_sys_mode(status);
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*Force DCDC in pwm mode or not */
int pmu_set_dcdc_mode(const char *name, int mode)
{
	if ((sunxi_pmu_dev) && (sunxi_pmu_dev->set_dcdc_mode))
		return sunxi_pmu_dev->set_dcdc_mode(name, mode);
	axp_err("not imple:%s\n", __func__);
	return -1;
}


/*Get the current mode*/
int pmu_get_sys_mode(void)
{
	if ((sunxi_pmu_dev) && (sunxi_pmu_dev->get_sys_mode))
		return sunxi_pmu_dev->get_sys_mode();
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*Get the button length interrupt*/
int pmu_get_key_irq(void)
{
	if ((sunxi_pmu_dev) && (sunxi_pmu_dev->get_key_irq))
		return sunxi_pmu_dev->get_key_irq();
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*Set limit total voltage*/
int pmu_set_bus_vol_limit(int vol_value)
{
	if ((sunxi_pmu_dev) && (sunxi_pmu_dev->set_bus_vol_limit))
		return sunxi_pmu_dev->set_bus_vol_limit(vol_value);
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*get register value*/
unsigned char pmu_get_reg_value(unsigned char reg_addr)
{
	if ((sunxi_pmu_dev) && (sunxi_pmu_dev->get_reg_value))
		return sunxi_pmu_dev->get_reg_value(reg_addr);
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*set register value*/
unsigned char pmu_set_reg_value(unsigned char reg_addr, unsigned char reg_value)
{
	if ((sunxi_pmu_dev) && (sunxi_pmu_dev->set_reg_value))
		return sunxi_pmu_dev->set_reg_value(reg_addr, reg_value);
	axp_err("not imple:%s\n", __func__);
	return -1;
}

