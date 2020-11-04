/*
 * drivers/video/sunxi/disp2/disp/de/lowlevel_v2x/de_wb.h
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
#ifndef __DE_WB_H__
#define __DE_WB_H__

#include "de_rtmx.h"

s32 wb_ebios_set_reg_base(u32 sel, uintptr_t base);
uintptr_t wb_ebios_get_reg_base(u32 sel);
s32 wb_ebios_init(disp_bsp_init_para *para);
s32 wb_ebios_writeback_enable(u32 sel, bool en);
s32 wb_ebios_set_para(u32 sel, struct disp_capture_config *cfg);
s32 wb_ebios_apply(u32 sel, struct disp_capture_config *cfg);
s32 wb_ebios_update_regs(u32 sel);
u32 wb_ebios_get_status(u32 sel);
s32 wb_ebios_enableint(u32 sel);
s32 wb_ebios_disableint(u32 sel);
u32 wb_ebios_queryint(u32 sel);
u32 wb_ebios_clearint(u32 sel);

#endif
