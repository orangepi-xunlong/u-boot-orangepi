/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * drivers/dsp/common/dsp_fdt.h
 *
 * Copyright (c) 2007-2025 Allwinnertech Co., Ltd.
 * Author: wujiayi <wujiayi@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details
 *
 */

#ifndef __DSP_FDT_H
#define __DSP_FDT_H

#include <fdt_support.h>
#define DSP_DEBUG  debug
int dts_uart_msg(struct dts_msg_t *pmsg, u32 dsp_id);
int dts_dsp_status(struct dts_msg_t *pmsg, u32 dsp_id);
int dts_gpio_int_msg(struct dts_msg_t *pmsg, u32 dsp_id);
int dts_sharespace_msg(struct dts_msg_t *pmsg, u32 dsp_id);
int dts_get_dsp_memory(ulong *start, u32 *size, u32 dsp_id);
#endif
