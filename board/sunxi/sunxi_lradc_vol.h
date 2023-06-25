/* SPDX-License-Identifier: GPL-2.0+
 * (C) Copyright 2019-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * oujiayu <oujiayu@allwinnertech.com>
 *
 * Configuration settings for the Allwinner sunxi lradc of boards.
 */

#ifndef SUNXI_LRADC_VOL_H_
#define SUNXI_LRADC_VOL_H_
#include "sunxi_lradc.h"

#define LRADC_DATA0		(0x0c)
#define LRADC_DATA1		(0x10)

int sunxi_read_lradc_vol(void);

#endif
