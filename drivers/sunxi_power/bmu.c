/*
 * Copyright (C) 2019 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
*/
#include <common.h>
#include <sunxi_power/axp.h>

#ifdef PMU_DEBUG
#define axp_err(fmt...) tick_printf("[axp][err]: " fmt)
#else
#define axp_err(fmt...)
#endif

__attribute__((section(".data"))) static struct sunxi_bmu_dev_t *sunxi_bmu_dev =
	NULL;

/* traverse the u-boot segment to find the bmu offset*/
static struct sunxi_bmu_dev_t *bmu_get_axp_dev_t(void)
{
	struct sunxi_bmu_dev_t *sunxi_bmu_dev_temp;
	struct sunxi_bmu_dev_t *sunxi_bmu_dev_start =
		ll_entry_start(struct sunxi_bmu_dev_t, bmu);
	int max = ll_entry_count(struct sunxi_bmu_dev_t, bmu);
	for (sunxi_bmu_dev_temp = sunxi_bmu_dev_start;
	     sunxi_bmu_dev_temp != sunxi_bmu_dev_start + max;
	     sunxi_bmu_dev_temp++) {
		if (!strncmp("bmu", sunxi_bmu_dev_temp->bmu_name, 3)) {
			if (!sunxi_bmu_dev_temp->probe()) {
				pr_msg("BMU: %s found\n",
				       sunxi_bmu_dev_temp->bmu_name);
				return sunxi_bmu_dev_temp;
			}
		}
	}
	pr_msg("BMU: no found\n");
	return NULL;
}

/* matches chipid*/
int bmu_probe(void)
{
	sunxi_bmu_dev = bmu_get_axp_dev_t();
	if (sunxi_bmu_dev == NULL)
		return -1;
	return 0;
}

/*Set shutdown*/
int bmu_set_power_off(void)
{
	if ((sunxi_bmu_dev) && (sunxi_bmu_dev->set_power_off))
		return sunxi_bmu_dev->set_power_off();
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*Set ntc onoff*/
int bmu_set_ntc_onoff(int onoff)
{
	if ((sunxi_bmu_dev) && (sunxi_bmu_dev->set_ntc_onoff))
		return sunxi_bmu_dev->set_ntc_onoff(onoff);
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/* Get the reason for triggering the boot, (button to power on, power on)*/
int bmu_get_poweron_source(void)
{
	if ((sunxi_bmu_dev) && (sunxi_bmu_dev->get_poweron_source))
		return sunxi_bmu_dev->get_poweron_source();
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*Get the current axp bus: DCIN&VBUS&BATTERY&NO exist */
int bmu_get_axp_bus_exist(void)
{
	if  ((sunxi_bmu_dev) && (sunxi_bmu_dev->get_axp_bus_exist))
		return sunxi_bmu_dev->get_axp_bus_exist();
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*Set coulomb counter switch*/
int bmu_set_coulombmeter_onoff(int onoff)
{
	if ((sunxi_bmu_dev) && (sunxi_bmu_dev->set_coulombmeter_onoff))
		return sunxi_bmu_dev->set_coulombmeter_onoff(onoff);
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*Get the average battery voltage*/
int bmu_get_battery_vol(void)
{
	if ((sunxi_bmu_dev) && (sunxi_bmu_dev->get_battery_vol))
		return sunxi_bmu_dev->get_battery_vol();
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*Get battery capacity*/
int bmu_get_battery_capacity(void)
{
	if ((sunxi_bmu_dev) && (sunxi_bmu_dev->get_battery_capacity))
		return sunxi_bmu_dev->get_battery_capacity();
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*Get the battery presence flag*/
int bmu_get_battery_probe(void)
{
	if ((sunxi_bmu_dev) && (sunxi_bmu_dev->get_battery_probe))
		return sunxi_bmu_dev->get_battery_probe();
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*limit total current*/
int bmu_set_vbus_current_limit(int current)
{
	if ((sunxi_bmu_dev) && (sunxi_bmu_dev->set_vbus_current_limit))
		return sunxi_bmu_dev->set_vbus_current_limit(current);
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*Get current limit current*/
int bmu_get_vbus_current_limit(void)
{
	if ((sunxi_bmu_dev) && (sunxi_bmu_dev->get_vbus_current_limit))
		return sunxi_bmu_dev->get_vbus_current_limit();
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*Set the current charge size*/
int bmu_set_charge_current_limit(int current)
{
	if ((sunxi_bmu_dev) && (sunxi_bmu_dev->set_charge_current_limit))
		return sunxi_bmu_dev->set_charge_current_limit(current);
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*get register value*/
unsigned char bmu_get_reg_value(unsigned char reg_addr)
{
	if ((sunxi_bmu_dev) && (sunxi_bmu_dev->get_reg_value))
		return sunxi_bmu_dev->get_reg_value(reg_addr);
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*set register value*/
unsigned char bmu_set_reg_value(unsigned char reg_addr, unsigned char reg_value)
{
	if ((sunxi_bmu_dev) && (sunxi_bmu_dev->set_reg_value))
		return sunxi_bmu_dev->set_reg_value(reg_addr, reg_value);
	axp_err("not imple:%s\n", __func__);
	return -1;
}

/*reset battery capacity to zero */
int bmu_reset_capacity(void)
{
	if  ((sunxi_bmu_dev) && (sunxi_bmu_dev->reset_capacity))
		return sunxi_bmu_dev->reset_capacity();
	axp_err("not imple:%s\n", __func__);
	return -1;
}
