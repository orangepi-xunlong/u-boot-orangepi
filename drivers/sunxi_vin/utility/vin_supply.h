/*
 * vin_supply.h
 *
 * Copyright (c) 2018 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zequn Zheng <zequnzhengi@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __VIN_SUPPLY_H__
#define __VIN_SUPPLY_H__

#include "../vin.h"
#include "../vin_video/vin_core.h"

typedef struct vin_gpio_set_t {
	char gpio_name[32];
	int port;
	int port_num;
	int mul_sel;
	int pull;
	int drv_level;
	int data;
	int gpio;
} vin_gpio_set_t;

typedef struct __vin_node_map {
	char node_name[16];
	int  nodeoffset;
} vin_fdt_node_map_t;

struct vin_gpio_set {
	vin_gpio_set_t  gpio_info;
	int gpio_handle;
};

/* iovdd/avdd/dvdd */
struct pmu_power_set {
	char power_name[16];
	int power_vol;
};

enum gpio_type {
	PWDN = 0,
	RESET,
	MCLK,
	MAX_GPIO_NUM,
};

enum pmic_channel {
	IOVDD = 0,
	AVDD,
	DVDD,
	MAX_POW_NUM,
};

struct sensor_list {
	char sensor_name[16];
	int used;
	int sensor_twi_addr;
	int sensor_twi_id;
	int mclk_id;
	int use_isp;
	struct pmu_power_set vin_power[MAX_POW_NUM];
	struct vin_gpio_set vin_gpio[MAX_GPIO_NUM];
};
/*
struct vin_core {
	int used;
	int sensor_sel;
	int csi_sel;
	int mipi_sel;
	int isp_sel;
	int vipp_sel;
};
*/
int vin_sys_config_parser(int sensor_sel);
int vin_vinc_parser(int vinc_sel);
int vin_set_pmu_channel(int on);
int vin_gpio_set_status(int direction);
int vin_gpio_write(int value_to_gpio);
void vin_fdt_init(void);
int  vin_fdt_nodeoffset(char *main_name);
unsigned long vin_getprop_regbase(char *main_name, char *sub_name, u32 index);
void csi_fdt_init(void);
int  csi_fdt_nodeoffset(char *main_name);
unsigned long csi_getprop_regbase(char *main_name, char *sub_name, u32 index);
unsigned long vin_getprop_irq(char *main_name, char *sub_name, u32 index);
int vin_sys_register_irq(u32 IrqNo, u32 Flags, void *Handler, void *pArg, u32 DataSize, u32 Prio);
void vin_sys_unregister_irq(u32 IrqNo, void *Handler, void *pArg);
void vin_sys_enable_irq(u32 IrqNo);
void vin_sys_disable_irq(u32 IrqNo);
int sunxi_cci_init(void);
int vin_set_mclk(int on);
int vin_set_mclk_freq(unsigned long freq);
void sensor_list_free(void);

#endif

