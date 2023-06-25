/*
 * Copyright (C) 2019 Allwinner.
 * weidonghui <weidonghui@allwinnertech.com>
 *
 * SUNXI AXP  Driver
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifndef __AXP_H__
#define __AXP_H__

#include <common.h>
#include <linker_lists.h>

#define AXP_POWER_ON_BY_POWER_KEY 0
#define AXP_POWER_ON_BY_POWER_TRIGGER 1

#define SUNXI_AXP_DEV_MAX (2)

#define AXP_VBUS_EXIST (2)
#define AXP_DCIN_EXIST (3)
#define AXP_VBUS_DCIN_NOT_EXIST (4)
#define BATTERY_EXIST (0X20)

#define POWER_KEY_EXIST 1
#define POWER_KEY_NOTEXIST 0

#define BATTERY_RATIO_DEFAULT 0
#define BATTERY_RATIO_TOO_LOW_WITHOUT_DCIN 1
#define BATTERY_RATIO_TOO_LOW_WITH_DCIN 2
#define BATTERY_RATIO_ENOUGH 3
#define BATTERY_VOL_TOO_LOW 4

#define AXP_POWER_ON_BY_POWER_KEY 0
#define AXP_POWER_ON_BY_POWER_TRIGGER 1

enum sunxi_axp_type {
	AXP_TYPE_MAIN = 0,
	AXP_TYPE_SLAVE,
};

struct sunxi_pmu_dev_t {
const char *pmu_name;
int (*probe)(void); /* matches chipid*/
int (*get_info)(char *name, unsigned char *chipid); /*get axp info*/
int (*set_voltage)(char *name, uint vol_value, uint onoff); /*Set a certain power, voltage value. */
int (*get_voltage)(char *name); /*Read a certain power, voltage value */
int (*set_power_off)(void); /*Set shutdown*/
int (*set_sys_mode)(int status); /*Sets the state of the next mode */
int (*set_dcdc_mode)(const char *name, int mode); /*force dcdc mode in pwm or not */
int (*get_sys_mode)(void); /*Get the current state*/
int (*get_key_irq)(void); /*Get the button length interrupt*/
int (*set_bus_vol_limit)(int vol_value); /*Set limit total voltage*/
unsigned char (*get_reg_value)(unsigned char reg_addr);/*get register value*/
unsigned char (*set_reg_value)(unsigned char reg_addr, unsigned char reg_value);/*set register value*/
};



#define U_BOOT_AXP_PMU_INIT(_name)                                             \
	ll_entry_declare(struct sunxi_pmu_dev_t, _name, pmu)

struct sunxi_bmu_dev_t {
const char *bmu_name;
int (*probe)(void); /* matches chipid*/
int (*set_power_off)(void); /*Set shutdown*/
int (*get_poweron_source)(void); /* Get the reason for triggering the boot, (button to power on, power on)*/
int (*get_axp_bus_exist)(void); /*Get the current axp bus: DCIN&VBUS&BATTERY&NO exist */
int (*set_coulombmeter_onoff)(int onoff); /*Set coulomb counter switch*/
int (*get_battery_vol)(void); /*Get the average battery voltage*/
int (*get_battery_capacity)(void); /*Get battery capacity*/
int (*get_battery_probe)(void); /*Get the battery presence flag*/
int (*set_vbus_current_limit)(int current); /*limit total current*/
int (*get_vbus_current_limit)(void); /*Get current limit current*/
int (*set_charge_current_limit)(int current); /*Set the current charge size*/
int (*reset_capacity)(void);
unsigned char (*get_reg_value)(unsigned char reg_addr);/*get register value*/
unsigned char (*set_reg_value)(unsigned char reg_addr, unsigned char reg_value);/*set register value*/
int (*set_ntc_onoff)(int onoff); /* set ntc onoff, default on */
};

#define U_BOOT_AXP_BMU_INIT(_name)                                             \
	ll_entry_declare(struct sunxi_bmu_dev_t, _name, bmu)

#define AXP_BOOT_SOURCE_BUTTON         0
#define AXP_BOOT_SOURCE_IRQ_LOW                1
#define AXP_BOOT_SOURCE_VBUS_USB       2
#define AXP_BOOT_SOURCE_CHARGER                3
#define AXP_BOOT_SOURCE_BATTERY                4


int pmu_probe(void);
int pmu_get_info(char *name, unsigned char *chipid);
int pmu_set_voltage(char *name, uint vol_value, uint onoff);
int pmu_get_voltage(char *name);
int pmu_set_power_off(void);
int pmu_set_sys_mode(int status);
int pmu_set_dcdc_mode(const char *name, int mode);
int pmu_get_sys_mode(void);
int pmu_get_key_irq(void);
int pmu_set_bus_vol_limit(int vol_value);
unsigned char pmu_get_reg_value(unsigned char reg_addr);
unsigned char pmu_set_reg_value(unsigned char reg_addr, unsigned char reg_value);

int bmu_probe(void);
int bmu_set_power_off(void);
int bmu_get_poweron_source(void);
int bmu_get_axp_bus_exist(void);
int bmu_set_coulombmeter_onoff(int onoff);
int bmu_get_battery_vol(void);
int bmu_get_battery_capacity(void);
int bmu_get_battery_probe(void);
int bmu_set_vbus_current_limit(int current);
int bmu_get_vbus_current_limit(void);
int bmu_set_charge_current_limit(int current);
int bmu_reset_capacity(void);
unsigned char bmu_get_reg_value(unsigned char reg_addr);
unsigned char bmu_set_reg_value(unsigned char reg_addr, unsigned char reg_value);
int bmu_set_ntc_onoff(int onoff);
int axp_probe(void);

#endif /* __AXP_H__ */
