/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __DISP_MANAGER_H__
#define __DISP_MANAGER_H__

#include "disp_private.h"

s32 disp_init_mgr(struct __disp_bsp_init_para *para);
s32 disp_mgr_shadow_protect(struct disp_manager *mgr, bool protect);
extern void sync_finish_event_proc(u32 screen_id);

#endif
