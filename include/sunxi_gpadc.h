/*
 * (C) Copyright 2022-2024
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <huangrongcun@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __SUNXI_GPADC_H__
#define __SUNXI_GPADC_H__
#include <common.h>


int sunxi_gpadc_init(void);
int sunxi_gpadc_read(int channel);

#endif /*__SUNXI_GPADC_H__*/