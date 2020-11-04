/*
 * drivers/video/sunxi/disp2/disp/de/lowlevel_sun50iw1/de_csc.h
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef __DE_CSC_H__
#define __DE_CSC_H__

#include "de_rtmx.h"

int de_dcsc_apply(unsigned int sel, struct disp_csc_config *config);
int de_dcsc_init(disp_bsp_init_para *para);
int de_dcsc_update_regs(unsigned int sel);
int de_dcsc_get_config(unsigned int sel, struct disp_csc_config *config);

int de_ccsc_apply(unsigned int sel, unsigned int ch_id, struct disp_csc_config *config);
int de_ccsc_update_regs(unsigned int sel);
int de_ccsc_init(disp_bsp_init_para *para);
int de_csc_coeff_calc(unsigned int infmt, unsigned int incscmod, unsigned int outfmt, unsigned int outcscmod,
								unsigned int brightness, unsigned int contrast, unsigned int saturation, unsigned int hue,
								unsigned int out_color_range, int *csc_coeff);

#endif

