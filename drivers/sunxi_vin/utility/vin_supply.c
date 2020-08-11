/*
 * vin_supply.c
 *
 * Copyright (c) 2018 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zequn Zheng <zequnzhengi@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "vin_supply.h"
#include "asm/io.h"
#include <power/sunxi/power.h>
#include <power/sunxi/pmu.h>
#include <libfdt.h>
#include <fdt_support.h>
#include <asm/arch/gic.h>
#if 0
static vin_fdt_node_map_t g_vin_fdt_node_map[] = {
	{FDT_VIND0_PATH, -1},
	{"", -1}
};

static vin_fdt_node_map_t g_csi_fdt_node_map[] = {
	{FDT_CSI0_PATH, -1},
	{FDT_CSI1_PATH, -1},
	{FDT_CSI2_PATH, -1},
	{FDT_CSI3_PATH, -1},
	{FDT_MIPI0_PATH, -1},
	{FDT_MIPI1_PATH, -1},
	{FDT_ISP0_PATH, -1},
	{FDT_ISP1_PATH, -1},
	{FDT_VIPP0_PATH, -1},
	{FDT_VIPP1_PATH, -1},
	{FDT_VIPP2_PATH, -1},
	{FDT_VIPP3_PATH, -1},
	{FDT_VIPP4_PATH, -1},
	{FDT_VIPP5_PATH, -1},
	{FDT_VIPP6_PATH, -1},
	{FDT_VIPP7_PATH, -1},
	{FDT_DMA0_PATH, -1},
	{FDT_DMA1_PATH, -1},
	{FDT_DMA2_PATH, -1},
	{FDT_DMA3_PATH, -1},
	{FDT_DMA4_PATH, -1},
	{FDT_DMA5_PATH, -1},
	{FDT_DMA6_PATH, -1},
	{FDT_DMA7_PATH, -1},
	{"", -1}
};
#else
static vin_fdt_node_map_t g_vin_fdt_node_map[] = {
	{FDT_VIND0_PATH, -1},
	{"", -1}
};

static vin_fdt_node_map_t g_csi_fdt_node_map[] = {
	{FDT_CSI1_PATH,  -1},
	{FDT_MIPI1_PATH, -1},
	{FDT_ISP0_PATH,  -1},
	{FDT_VIPP0_PATH, -1},
	{FDT_DMA0_PATH,  -1},
	{"", -1}
};

#endif

struct sensor_list *sensor;
struct vin_core *vinc;
extern struct vin_clk_total *vin_clk;

void vin_fdt_init(void)
{
	int i = 0;
	while (strlen(g_vin_fdt_node_map[i].node_name)) {
		g_vin_fdt_node_map[i].nodeoffset =
			fdt_path_offset(working_fdt, g_vin_fdt_node_map[i].node_name);
		i++;
	}
}

int  vin_fdt_nodeoffset(char *main_name)
{
	int i = 0;
	for (i = 0; ; i++) {
		if (0 == strcmp(g_vin_fdt_node_map[i].node_name, main_name))
			return g_vin_fdt_node_map[i].nodeoffset;
		if (0 == strlen(g_vin_fdt_node_map[i].node_name))
			return -1;
	}
	return -1;
}

unsigned long vin_getprop_regbase(char *main_name, char *sub_name, u32 index)
{
	char compat[32];
	u32 len = 0;
	int node;
	int ret = -1;
	int value[32] = {0};
	unsigned long reg_base = 0;

	len = sprintf(compat, "%s", main_name);
	if (len > 32)
		vin_err("size of mian_name is out of range\n");

	/*node = fdt_path_offset(working_fdt,compat); */
	node = vin_fdt_nodeoffset(compat);
	if (node < 0) {
		vin_err("fdt_path_offset %s fail\n", compat);
		goto exit;
	}

	ret = fdt_getprop_u32(working_fdt, node, sub_name, (uint32_t *)value);
	if (0 > ret)
		vin_err("fdt_getprop_u32 %s.%s fail\n", main_name, sub_name);
	else
		reg_base = value[index * 4] + value[index * 4 + 1];
exit:
	return reg_base;
}

void csi_fdt_init(void)
{
	int i = 0;
	while (strlen(g_csi_fdt_node_map[i].node_name)) {
		g_csi_fdt_node_map[i].nodeoffset =
			fdt_path_offset(working_fdt, g_csi_fdt_node_map[i].node_name);
		i++;
	}
}

int  csi_fdt_nodeoffset(char *main_name)
{
	int i = 0;
	for (i = 0; ; i++) {
		if (0 == strcmp(g_csi_fdt_node_map[i].node_name, main_name))
			return g_csi_fdt_node_map[i].nodeoffset;
		if (0 == strlen(g_csi_fdt_node_map[i].node_name))
			return -1;
	}
	return -1;
}

unsigned long csi_getprop_regbase(char *main_name, char *sub_name, u32 index)
{
	char compat[32];
	u32 len = 0;
	int node;
	int ret = -1;
	int value[32] = {0};
	unsigned long reg_base = 0;

	len = sprintf(compat, "%s", main_name);
	if (len > 32)
		vin_err("size of mian_name is out of range\n");

	/* node = fdt_path_offset(working_fdt,compat); */
	node = csi_fdt_nodeoffset(compat);
	if (node < 0) {
		vin_err("fdt_path_offset %s fail\n", compat);
		goto exit;
	}

	ret = fdt_getprop_u32(working_fdt, node, sub_name, (uint32_t *)value);
	if (0 > ret)
		vin_err("fdt_getprop_u32 %s.%s fail\n", main_name, sub_name);
	else
		reg_base = value[index * 4] + value[index * 4 + 1];
exit:
	return reg_base;
}


unsigned long vin_getprop_irq(char *main_name, char *sub_name, u32 index)
{
	char compat[32];
	u32 len = 0;
	int node;
	int ret = -1;
	int value[32] = {0};
	u32 irq = 0;

	len = sprintf(compat, "%s", main_name);
	if (len > 32)
		vin_err("size of mian_name is out of range\n");

	/* node = fdt_path_offset(working_fdt,compat); */
	node = csi_fdt_nodeoffset(compat);
	if (node < 0) {
		vin_err("fdt_path_offset %s fail\n", compat);
		goto exit;
	}

	ret = fdt_getprop_u32(working_fdt, node, sub_name, (uint32_t *)value);
	if (0 > ret)
		vin_err("fdt_getprop_u32 %s.%s fail\n", main_name, sub_name);
	else {
		irq = value[index * 3 + 1];
		if (0 == value[index * 3])
			irq += 32;
	}
exit:
	return irq;
}

int vin_sys_register_irq(u32 IrqNo, u32 Flags, void *Handler,
			void *pArg, u32 DataSize, u32 Prio)
{
	vin_log(VIN_LOG_MD, "%s, irqNo=%d, Handler=0x%p, pArg=0x%p\n", __func__, IrqNo, Handler, pArg);
	irq_install_handler(IrqNo, (interrupt_handler_t *)Handler,  pArg);

	return 0;
}

void vin_sys_unregister_irq(u32 IrqNo, void *Handler, void *pArg)
{
	irq_free_handler(IrqNo);
}

void vin_sys_enable_irq(u32 IrqNo)
{
	irq_enable(IrqNo);
}

void vin_sys_disable_irq(u32 IrqNo)
{
	irq_disable(IrqNo);
}

static int vin_pmu_enable(char *name, int vol)
{
	int ret = 0;
	if (0 == strlen(name))
		return 0;

	ret = axp_set_supply_vol_byregulator(name, 1, vol);
	if (!ret)
		vin_log(VIN_LOG_POWER, "enable power %s, ret=%d\n", name, ret);

	return 0;
}

static int vin_pmu_disable(char *name, int vol)
{
	int ret = 0;

	ret = axp_set_supply_vol_byregulator(name, 0, vol);
	if (!ret)
		vin_log(VIN_LOG_POWER, "disable power %s, ret=%d\n", name, ret);
	return 0;
}

/* type: 0:invalid, 1: int; 2:str, 3: gpio */
int vin_sys_script_get_item(char *main_name,
				char *sub_name, int value[], int type)
{
	int ret;
	script_parser_value_type_t ret_type;
	ret = script_parser_fetch_ex(main_name, sub_name, value, &ret_type, (sizeof(script_gpio_set_t)));

	if (ret == SCRIPT_PARSER_OK) {
		if (ret_type == 4)
			return ret_type-1;
		else
			return ret_type;
	}

	return 0;

}

int vin_sys_gpio_request(vin_gpio_set_t *gpio_list, u32 group_count_max)
{
	user_gpio_set_t gpio_info;
	gpio_info.port = gpio_list->port;
	gpio_info.port_num = gpio_list->port_num;
	gpio_info.mul_sel = gpio_list->mul_sel;
	gpio_info.drv_level = gpio_list->drv_level;
	gpio_info.data = gpio_list->data;

	vin_log(VIN_LOG_MD, "disp_sys_gpio_request, port:%d, port_num:%d, mul_sel:%d, pull:%d, drv_level:%d, data:%d\n",
			gpio_list->port, gpio_list->port_num, gpio_list->mul_sel,
			gpio_list->pull, gpio_list->drv_level, gpio_list->data);

	return gpio_request(&gpio_info, group_count_max);
}

int vin_sys_gpio_request_simple(vin_gpio_set_t *gpio_list, u32 group_count_max)
{
	int ret = 0;
	user_gpio_set_t gpio_info;
	gpio_info.port = gpio_list->port;
	gpio_info.port_num = gpio_list->port_num;
	gpio_info.mul_sel = gpio_list->mul_sel;
	gpio_info.drv_level = gpio_list->drv_level;
	gpio_info.data = gpio_list->data;

	vin_print("OSAL_GPIO_Request, port:%d, port_num:%d, mul_sel:%d, "\
		"pull:%d, drv_level:%d, data:%d\n",
		gpio_list->port, gpio_list->port_num, gpio_list->mul_sel,
		gpio_list->pull, gpio_list->drv_level, gpio_list->data);
	ret = gpio_request_early(&gpio_info, group_count_max, 1);
	return ret;
}

int vin_sys_gpio_release(int p_handler, s32 if_release_to_default_status)
{
	if (p_handler != 0xffff)
		gpio_release(p_handler, if_release_to_default_status);
	return 0;
}

/* direction: 0:input, 1:output */
int vin_sys_gpio_set_direction(u32 p_handler, u32 direction, const char *gpio_name)
{
	return gpio_set_one_pin_io_status(p_handler, direction, gpio_name);
}

int vin_sys_gpio_get_value(u32 p_handler, const char *gpio_name)
{
	return gpio_read_one_pin_value(p_handler, gpio_name);
}

int vin_sys_gpio_set_value(u32 p_handler, u32 value_to_gpio, const char *gpio_name)
{
	return gpio_write_one_pin_value(p_handler, value_to_gpio, gpio_name);
}

int vin_sys_config_parser(int sensor_sel)
{
	struct pmu_power_set *vin_power;
	struct vin_gpio_set *vin_gpio;
	char key_name[20];
	char sub_name[20];
	int ret;

	sensor = malloc(sizeof(struct sensor_list));

	vin_power = sensor->vin_power;
	vin_gpio = sensor->vin_gpio;

	sprintf(key_name, "vind0/sensor%d", sensor_sel);

	sprintf(sub_name, "sensor%d_used", sensor_sel);
	ret = vin_sys_script_get_item(key_name, sub_name, &sensor->used, 1);
	if (ret != 1)
		vin_err("Fail to get [sensor%d_used]\n", sensor_sel);
	if (!sensor->used) {
		vin_err("Sensor%d is no open\n", sensor_sel);
		return -1;
	}

	sprintf(sub_name, "sensor%d_mname", sensor_sel);
	ret = vin_sys_script_get_item(key_name, sub_name, (int *)sensor->sensor_name, 2);
	if (ret != 2) {
		vin_err("Fail to get [sensor%d_mname]\n", sensor_sel);
		return -1;
	}

	sprintf(sub_name, "sensor%d_twi_addr", sensor_sel);
	ret = script_parser_fetch(key_name, sub_name, &sensor->sensor_twi_addr, 1);
	if (ret != 0) {
		vin_err("unable to get cci_twi_addr from [vind0/sensor%d]\n", sensor_sel);
		return -1;
	}

	sprintf(sub_name, "sensor%d_twi_cci_id", sensor_sel);
	ret = script_parser_fetch(key_name, sub_name, &sensor->sensor_twi_id, 1);
	if (ret != 0) {
		vin_err("unable to get cci_twi_id from [vind0/sensor%d]\n", sensor_sel);
		return -1;
	}

	sprintf(sub_name, "sensor%d_isp_used", sensor_sel);
	ret = script_parser_fetch(key_name, sub_name, &sensor->use_isp, 1);
	if (ret != 0) {
		vin_err("unable to get isp_used from [vind0/sensor%d]\n", sensor_sel);
		return -1;
	}

	sprintf(sub_name, "sensor%d_mclk_id", sensor_sel);
	ret = script_parser_fetch(key_name, sub_name, &sensor->mclk_id, 1);
	if (ret != 0) {
		vin_err("unable to get cci_twi_id from [vind0/sensor%d]\n", sensor_sel);
		return -1;
	}

	/* iovdd/avdd/dvdd vol */
	sprintf(sub_name, "sensor%d_iovdd_vol", sensor_sel);
	ret = vin_sys_script_get_item(key_name, sub_name, &vin_power[IOVDD].power_vol, 1);
	if (ret != 1) {
		vin_err("Fail to get [sensor%d_iovdd_vol]\n", sensor_sel);
		return -1;
	}

	sprintf(sub_name, "sensor%d_avdd_vol", sensor_sel);
	ret = vin_sys_script_get_item(key_name, sub_name, &vin_power[AVDD].power_vol, 1);
	if (ret != 1) {
		vin_err("Fail to get [sensor%d_avdd_vol]\n", sensor_sel);
		return -1;
	}

	sprintf(sub_name, "sensor%d_dvdd_vol", sensor_sel);
	ret = vin_sys_script_get_item(key_name, sub_name, &vin_power[DVDD].power_vol, 1);
	if (ret != 1) {
		vin_err("Fail to get [sensor%d_dvdd_vol]\n", sensor_sel);
		return -1;
	}

	/* iovdd/advv/dvdd */
	sprintf(sub_name, "sensor%d_iovdd", sensor_sel);
	ret = vin_sys_script_get_item(key_name, sub_name, (int *)vin_power[IOVDD].power_name, 2);
	if (ret != 2) {
		vin_err("Fail to get [sensor%d_iovdd]\n", sensor_sel);
		return -1;
	}

	sprintf(sub_name, "sensor%d_avdd", sensor_sel);
	ret = vin_sys_script_get_item(key_name, sub_name, (int *)vin_power[AVDD].power_name, 2);
	if (ret != 2) {
		vin_err("Fail to get [sensor%d_avdd]\n", sensor_sel);
		return -1;
	}

	sprintf(sub_name, "sensor%d_dvdd", sensor_sel);
	ret = vin_sys_script_get_item(key_name, sub_name, (int *)vin_power[DVDD].power_name, 2);
	if (ret != 2) {
		vin_err("Fail to get [sensor%d_dvdd]\n", sensor_sel);
		return -1;
	}

	/* RESET/PWDN */
	sprintf(sub_name, "sensor%d_reset", sensor_sel);
	ret = vin_sys_script_get_item(key_name, sub_name, (int *)&vin_gpio[RESET].gpio_info, 3);
	if (ret != 3) {
		vin_err("fail to get reset gpio, ret is %d!\n", ret);
		return -1;
	}

	sprintf(sub_name, "sensor%d_pwdn", sensor_sel);
	ret = vin_sys_script_get_item(key_name, sub_name, (int *)&vin_gpio[PWDN].gpio_info, 3);
	if (ret != 3) {
		vin_err("fail to get reset gpio, ret is %d!\n", ret);
		return -1;
	}

	sprintf(sub_name, "sensor%d_mclk", sensor_sel);
	ret = vin_sys_script_get_item(key_name, sub_name, (int *)&vin_gpio[MCLK].gpio_info, 3);
	if (ret != 3) {
		vin_err("fail to get reset gpio, ret is %d!\n", ret);
		return -1;
	}

	return 0;
}

int vin_vinc_parser(int vinc_sel)
{
	char key_name[20];
	char sub_name[25];
	int ret;

	vinc = malloc(sizeof(struct vin_core));

	sprintf(key_name, "vind0/vinc%d", vinc_sel);

	sprintf(sub_name, "vinc%d_used", vinc_sel);
	ret = vin_sys_script_get_item(key_name, sub_name, &vinc->used, 1);
	if (ret != 1) {
		vin_err("Fail to get [vinc%d_used]\n", vinc_sel);
		return -1;
	}
	if (!vinc->used) {
		vin_err("sensor%d is no open\n", vinc_sel);
		return -1;
	}

	vinc->id = vinc_sel;
	vinc->vipp_sel = vinc_sel;

	sprintf(sub_name, "vinc%d_csi_sel", vinc_sel);
	ret = vin_sys_script_get_item(key_name, sub_name, &vinc->csi_sel, 1);
	if (ret != 1) {
		vin_err("Fail to get [vinc%d_csi_sel]\n", vinc_sel);
		return -1;
	}

	sprintf(sub_name, "vinc%d_mipi_sel", vinc_sel);
	ret = vin_sys_script_get_item(key_name, sub_name, &vinc->mipi_sel, 1);
	if (ret != 1) {
		vin_err("Fail to get [vinc%d_mipi_sel]\n", vinc_sel);
		return -1;
	}

	sprintf(sub_name, "vinc%d_isp_sel", vinc_sel);
	ret = vin_sys_script_get_item(key_name, sub_name, &vinc->isp_sel, 1);
	if (ret != 1) {
		vin_err("Fail to get [vinc%d_isp_sel]\n", vinc_sel);
		return -1;
	}

	sprintf(sub_name, "vinc%d_isp_tx_ch", vinc_sel);
	ret = vin_sys_script_get_item(key_name, sub_name, &vinc->isp_tx_ch, 1);
	if (ret != 1) {
		vin_err("Fail to get [vinc%d_isp_tx_ch]\n", vinc_sel);
		return -1;
	}

	sprintf(sub_name, "vinc%d_rear_sensor_sel", vinc_sel);
	ret = vin_sys_script_get_item(key_name, sub_name, &vinc->rear_sensor, 1);
	if (ret != 1) {
		vin_err("Fail to get [vinc%d_rear_sensor_sel]\n", vinc_sel);
		return -1;
	}

	return 0;

}

int sunxi_cci_init(void)
{
	int sensor_twi_addr, sensor_twi_id;

	sensor_twi_addr = sensor->sensor_twi_addr;
	sensor_twi_id = sensor->sensor_twi_id;

#if defined(CONFIG_SYS_I2C)
	i2c_set_bus_num(sensor_twi_id);
	i2c_init(CONFIG_SYS_I2C_SPEED, sensor_twi_addr>>1);
	vin_log(VIN_LOG_CCI, "speed=%d, id=%d, slave=0x%x\n", (u32)CONFIG_SYS_I2C_SPEED, sensor_twi_id, sensor_twi_addr);
#endif
	return 0;
}

int vin_set_pmu_channel(int on)
{
	struct pmu_power_set *vin_power;

	vin_power = sensor->vin_power;

	if (on) {
		if (strcmp(vin_power[IOVDD].power_name, "") && vin_power[IOVDD].power_vol)
			vin_pmu_enable(vin_power[IOVDD].power_name, vin_power[IOVDD].power_vol/1000);
		if (strcmp(vin_power[AVDD].power_name, "") && vin_power[AVDD].power_vol)
			vin_pmu_enable(vin_power[AVDD].power_name, vin_power[AVDD].power_vol/1000);
		if (strcmp(vin_power[DVDD].power_name, "") && vin_power[DVDD].power_vol)
			vin_pmu_enable(vin_power[DVDD].power_name, vin_power[DVDD].power_vol/1000);
	} else {
		if (strcmp(vin_power[AVDD].power_name, "") && vin_power[AVDD].power_vol)
			vin_pmu_disable(vin_power[AVDD].power_name, vin_power[AVDD].power_vol/1000);
		if (strcmp(vin_power[DVDD].power_name, "") && vin_power[DVDD].power_vol)
			vin_pmu_disable(vin_power[DVDD].power_name, vin_power[DVDD].power_vol/1000);
		if (strcmp(vin_power[IOVDD].power_name, "") && vin_power[IOVDD].power_vol)
			vin_pmu_disable(vin_power[IOVDD].power_name, vin_power[IOVDD].power_vol/1000);
	}

	return 0;
}

/* direction: 0:input, 1:output */
int vin_gpio_set_status(int direction)
{
	struct vin_gpio_set *vin_gpio;

	vin_gpio = sensor->vin_gpio;

	if (direction)
		vin_gpio[RESET].gpio_handle = vin_sys_gpio_request(&vin_gpio[RESET].gpio_info, 1);
	/* output mode */
	vin_sys_gpio_set_direction(vin_gpio[RESET].gpio_handle, direction, vin_gpio[RESET].gpio_info.gpio_name);

	if (direction)
		vin_gpio[PWDN].gpio_handle = vin_sys_gpio_request(&vin_gpio[PWDN].gpio_info, 1);
	/* output mode */
	vin_sys_gpio_set_direction(vin_gpio[PWDN].gpio_handle, direction, vin_gpio[PWDN].gpio_info.gpio_name);

	return 0;
}

int vin_gpio_write(int value_to_gpio)
{
	struct vin_gpio_set *vin_gpio;
	int ret;

	vin_gpio = sensor->vin_gpio;

	ret = vin_sys_gpio_set_value(vin_gpio[RESET].gpio_handle, value_to_gpio, vin_gpio[RESET].gpio_info.gpio_name);
	if (ret) {
		vin_err("Fail to set reset %d\n", value_to_gpio);
		return -1;
	}

	ret = vin_sys_gpio_set_value(vin_gpio[PWDN].gpio_handle, value_to_gpio, vin_gpio[PWDN].gpio_info.gpio_name);
	if (ret) {
		vin_err("Fail to set pdwn %d\n", value_to_gpio);
		return -1;
	}

	return 0;
}

/*
 *set frequency of master clock
 */
int vin_set_mclk_freq(unsigned long freq)
{
	struct clk *mclk_src = NULL;
	int mclk_id = 0;

	mclk_id = sensor->mclk_id;

	if (mclk_id < 0) {
		vin_err("get mclk id failed\n");
		return -1;
	}

	if (freq == 24000000 || freq == 12000000 || freq == 6000000) {
		if (vin_clk->mclk[mclk_id].clk_24m) {
			mclk_src = vin_clk->mclk[mclk_id].clk_24m;
		} else {
			vin_err("csi master clock 24M source is null\n");
			return -1;
		}
	} else {
		if (vin_clk->mclk[mclk_id].clk_pll) {
			mclk_src = vin_clk->mclk[mclk_id].clk_pll;
		} else {
			vin_err("csi master clock pll source is null\n");
			return -1;
		}
	}

	if (vin_clk->mclk[mclk_id].mclk) {
		if (clk_set_parent(vin_clk->mclk[mclk_id].mclk, mclk_src)) {
			vin_err("set mclk%d source failed!\n", mclk_id);
			return -1;
		}
		if (clk_set_rate(vin_clk->mclk[mclk_id].mclk, freq)) {
			vin_err("set csi master%d clock error\n", mclk_id);
			return -1;
		}
		vin_log(VIN_LOG_POWER, "mclk%d set rate %ld, get rate %ld\n", mclk_id,
			freq, clk_get_rate(vin_clk->mclk[mclk_id].mclk));
	} else {
		vin_err("csi master clock is null\n");
		return -1;
	}
	return 0;
}


int vin_set_mclk(int on)
{
	struct vin_gpio_set *vin_gpio;

	vin_gpio = sensor->vin_gpio;

	if (on) {
		clk_prepare_enable(vin_clk->mclk[sensor->mclk_id].mclk);
		vin_sys_gpio_request(&vin_gpio[MCLK].gpio_info, 1);
	} else {
		clk_disable(vin_clk->mclk[sensor->mclk_id].mclk);
		vin_sys_gpio_request(&vin_gpio[MCLK].gpio_info, 0);
	}
	return 0;
}

void sensor_list_free(void)
{
	free(sensor);
}

