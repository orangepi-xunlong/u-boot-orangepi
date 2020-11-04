/*
 * drivers/video/sunxi/disp2/disp/de/lowlevel_v2x/de_vep_table.h
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
#ifndef __DE_VEP_TAB__
#define __DE_VEP_TAB__

extern int y2r[192];
extern int r2y[128];
extern int y2y[64];
extern int r2r[32];
extern int bypass_csc[12];
extern unsigned int sin_cos[128];
extern int fcc_range_gain[6];
extern unsigned char ce_bypass_lut[256];
extern unsigned char ce_constant_lut[256];

#endif
