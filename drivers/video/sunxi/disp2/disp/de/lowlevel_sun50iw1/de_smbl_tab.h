/*
 * drivers/video/sunxi/disp2/disp/de/lowlevel_sun50iw1/de_smbl_tab.h
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
#include "de_smbl.h"

#if defined(CONFIG_DISP2_SUNXI_SUPPORT_SMBL)
u16 pwrsv_lgc_tab[1408][256];




//u8 spatial_coeff[9]={228,241,228,241,255,241,228,241,228};

u8 smbl_filter_coeff[272];

u8 hist_thres_drc[8];
u8 hist_thres_pwrsv[8];
u8 drc_filter[IEP_LH_PWRSV_NUM];
u32 csc_bypass_coeff[12];
#endif

