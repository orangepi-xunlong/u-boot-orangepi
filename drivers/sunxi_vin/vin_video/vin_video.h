
/*
 * vin_video.h for video api
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

#ifndef _VIN_VIDEO_H_
#define _VIN_VIDEO_H_

#include "../vin.h"
#include "../utility/vin_common.h"
#include "../utility/sunxi_camera_v2.h"
#include "dma_reg.h"

/* buffer for one video frame */
struct vin_buffer {
	unsigned int *phy_addr;
	struct list_head list;
};
#define BUF_NUM 3

struct vin_addr {
	u32	y;
	u32	cb;
	u32	cr;
};

int vin_subdev_s_stream(int enable);
int vin_set_addr(unsigned int phy_addr);
int buffer_queue(void);
int buffer_free(void);
#endif /*_VIN_VIDEO_H_*/
