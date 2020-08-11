/*
 * vin_core.h for video manage
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

#ifndef _VIN_CORE_H_
#define _VIN_CORE_H_

#include "../vin.h"
#include "../utility/platform_cfg.h"
#include "../vin_mipi/bsp_mipi_csi.h"
#include "../utility/vin_supply.h"
#include "vin_video.h"
#include "dma_reg.h"

#define MAX_FRAME_MEM	(500*1024*1024)
#define MIN_WIDTH	(32)
#define MIN_HEIGHT	(32)
#define MAX_WIDTH	(4800)
#define MAX_HEIGHT	(4800)
#define DUMP_CSI	(1 << 0)
#define DUMP_ISP	(1 << 1)

struct vin_core {
	unsigned int is_empty;
	int id;
	int used;
	unsigned long base;
	/* about some global info */
	int rear_sensor;
	int front_sensor;
	int sensor_sel;
	int csi_sel;
	int mipi_sel;
	int isp_sel;
	int vipp_sel;
	int isp_tx_ch;
	int hflip;
	int vflip;
	int fps_ds;
	int irq;
	int first_frame;
	struct isp_debug_mode isp_dbg;
	struct list_head vidq_active;
};

int vin_core_probe(void);
int vin_core_remove(void);

#endif /*_VIN_CORE_H_*/

