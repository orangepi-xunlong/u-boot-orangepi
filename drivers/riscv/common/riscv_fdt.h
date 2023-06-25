/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * drivers/riscv/common/riscv_fdt.h
 *
 * Copyright (c) 2007-2025 Allwinnertech Co., Ltd.
 * Author: majiangjiang <majiangjiang@allwinnertech.com>
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

#ifndef __RISCV_FDT_H
#define __RISCV_FDT_H

#include <fdt_support.h>
#define RISCV_DEBUG  debug
int riscv_dts_uart_msg(struct dts_msg_t *pmsg, u32 riscv_id);
int dts_riscv_status(struct dts_msg_t *pmsg, u32 riscv_id);
int riscv_dts_gpio_int_msg(struct dts_msg_t *pmsg, u32 riscv_id);
int riscv_dts_sharespace_msg(struct dts_msg_t *pmsg, u32 riscv_id);
#endif
