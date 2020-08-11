
/*
 * header for cameras.
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *	Yang Feng <yangfeng@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __CAMERA__H__
#define __CAMERA__H__

#include "../../vin.h"
#include "../../utility/platform_cfg.h"

/*
 * Basic window sizes.  These probably belong somewhere more globally
 * useful.
 */
#define ABS_SENSOR(x)                 ((x) > 0 ? (x) : -(x))

#define HXGA_WIDTH    4000
#define HXGA_HEIGHT   3000
#define QUXGA_WIDTH   3264
#define QUXGA_HEIGHT  2448
#define QSXGA_WIDTH   2592
#define QSXGA_HEIGHT  1936
#define QXGA_WIDTH    2048
#define QXGA_HEIGHT   1536
#define HD1080_WIDTH  1920
#define HD1080_HEIGHT 1080
#define UXGA_WIDTH    1600
#define UXGA_HEIGHT   1200
#define SXGA_WIDTH    1280
#define SXGA_HEIGHT   960
#define HD720_WIDTH   1280
#define HD720_HEIGHT  720
#define XGA_WIDTH     1024
#define XGA_HEIGHT    768
#define SVGA_WIDTH    800
#define SVGA_HEIGHT   600
#define VGA_WIDTH     640
#define VGA_HEIGHT    480
#define QVGA_WIDTH    320
#define QVGA_HEIGHT   240
#define CIF_WIDTH     352
#define CIF_HEIGHT    288
#define QCIF_WIDTH    176
#define QCIF_HEIGHT   144

#define CSI_GPIO_HIGH     1
#define CSI_GPIO_LOW     0
#define CCI_BITS_8           8
#define CCI_BITS_16         16
#define SENSOR_MAGIC_NUMBER 0x156977

#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))


enum power_seq_cmd {
	PWR_OFF = 0,
	PWR_ON = 1,
};

struct sensor_format_struct {
	char desc[20];
	u32 mbus_code;
	unsigned int width;
	unsigned int height;
	unsigned int o_width;
	unsigned int o_height;
	unsigned int offs_h;
	unsigned int offs_v;
	u32 fourcc;
	enum v4l2_field field;
};


extern struct sensor_list *sensor;

#define DEV_DBG_EN   0
#if (DEV_DBG_EN == 1)
#define sensor_dbg(x, arg...) printf("[%s_debug]"x, sensor->sensor_name, ##arg)
#else
#define sensor_dbg(x, arg...)
#endif

#define sensor_err(x, arg...)  printf("[%s_err]"x, sensor->sensor_name, ##arg)
#define sensor_print(x, arg...) printf("[%s]"x, sensor->sensor_name, ##arg)

int sensor_g_mbus_config(struct v4l2_mbus_config *cfg, struct mbus_framefmt_res *res);
int sensor_power(int on);
int sensor_init(void);
int sensor_s_stream(int enable);
int sensor_test_i2c(void);


#endif /*__CAMERA__H__*/
