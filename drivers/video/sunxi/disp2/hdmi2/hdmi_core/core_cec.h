/*
 * drivers/video/sunxi/disp2/hdmi2/hdmi_core/core_cec.h
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
#ifndef _CORE_CEC_H_
#define _CORE_CEC_H_

int cec_thread_init(void *param);
void cec_thread_exit(void);
s32 hdmi_cec_enable(int enable);
void hdmi_cec_wakup_request(void);

#endif
