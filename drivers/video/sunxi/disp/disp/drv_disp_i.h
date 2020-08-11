/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __DRV_DISP_I_H__
#define __DRV_DISP_I_H__

#include "de/bsp_display.h"

enum disp_return_value {
	DIS_SUCCESS = 0,
	DIS_FAIL = -1,
	DIS_PARA_FAILED = -2,
	DIS_PRIO_ERROR = -3,
	DIS_OBJ_NOT_INITED = -4,
	DIS_NOT_SUPPORT = -5,
	DIS_NO_RES = -6,
	DIS_OBJ_COLLISION = -7,
	DIS_DEV_NOT_INITED = -8,
	DIS_DEV_SRAM_COLLISION = -9,
	DIS_TASK_ERROR = -10,
	DIS_PRIO_COLLSION = -11
};

#define HANDTOID(handle)  ((handle) - 100)
#define IDTOHAND(ID)  ((ID) + 100)

#define DISP_IO_NUM     8
#define DISP_IO_SCALER0 0
#define DISP_IO_SCALER1 1
#define DISP_IO_IMAGE0  2
#define DISP_IO_IMAGE1  3
#define DISP_IO_LCDC0   4
#define DISP_IO_LCDC1   5
#define DISP_IO_TVEC0    6
#define DISP_IO_TVEC1    7

/* half word input */
#define sys_get_hvalue(n)  readw((void *)(n))

/* half word output */
#define sys_put_hvalue(n, c) writew(c, (void *)(n))

/* word input */
#define sys_get_wvalue(n)   readl((void *)(n))

/* word output */
#define sys_put_wvalue(n, c) writel(c, (void *)(n))

#endif
