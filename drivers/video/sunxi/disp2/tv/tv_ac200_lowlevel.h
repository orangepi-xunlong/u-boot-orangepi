/*
 * drivers/video/sunxi/disp2/tv/tv_ac200_lowlevel.h
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
#ifndef  _DRV_TV_AC200_LOWLEVEL_H_
#define  _DRV_TV_AC200_LOWLEVEL_H_

#include "tv_ac200.h"

s32 aw1683_tve_init(const u16 *p_dac_cali, const u16 *p_bandgap);
s32 aw1683_tve_plug_status(void);
s32 aw1683_tve_set_mode(u32 mode);
s32 aw1683_tve_open(void);
s32 aw1683_tve_close(void);

#endif
