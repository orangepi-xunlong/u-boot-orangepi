/*
 * cci_helper.c
 *
 * Copyright (c) 2018 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zequn Zheng <zequnzhengi@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "cci_helper.h"

extern struct sensor_list *sensor;

static int cci_write(addr_type addr, data_type value)
{
	/* int addr_width = cci_drv->addr_width; */
	/* int data_width = cci_drv->data_width; */
	int ret;

	ret = i2c_write(sensor->sensor_twi_addr>>1, addr, 2, (__u8 *)&value, 1);

	if (ret) {
		vin_log(VIN_LOG_CCI, "at %s error, addr_width = %d , data_width = %d!\n",
				__func__, 16, 8);
		return -1;
	}

	return ret;
}

static int cci_read(addr_type addr, data_type *value)
{
	/* int addr_width = cci_drv->addr_width; */
	/* int data_width = cci_drv->data_width; */
	int ret;
	*value = 0;

	ret = i2c_read(sensor->sensor_twi_addr>>1, addr , 2, (__u8 *)value, 1);

	if (ret) {
		vin_log(VIN_LOG_CCI, "%s error! addr_width = %d , data_width = %d!\n ",\
			__func__, 16, 8);
		return -1;
	}

	return ret;
}


int sensor_read(addr_type reg, data_type *value)
{
	int ret = 0, cnt = 0;

	ret = cci_read(reg, value);
	while ((ret != 0) && (cnt < 2)) {
		ret = cci_read(reg, value);
		cnt++;
	}
	if (cnt > 0)
		vin_warn("[%s]sensor read retry = %d\n", sensor->sensor_name, cnt);

	return ret;
}

int sensor_write(addr_type reg, data_type value)
{
	int ret = 0, cnt = 0;

	ret = cci_write(reg, value);
	while ((ret != 0) && (cnt < 2)) {
		ret = cci_write(reg, value);
		cnt++;
	}
	if (cnt > 0)
		vin_warn("[%s]sensor write retry = %d\n", sensor->sensor_name, cnt);

	return ret;
}

int sensor_write_array(struct regval_list *regs, int array_size)
{
	int ret = 0, i = 0;

	if (!regs)
		return -1;

	while (i < array_size) {
		if (regs->addr == REG_DLY) {
			mdelay(regs->data);
		} else {
			ret = sensor_write(regs->addr, regs->data);
			if (ret < 0)
				vin_err("%s sensor write array error!\n",
					sensor->sensor_name);
		}
		i++;
		regs++;
	}
	return 0;
}


