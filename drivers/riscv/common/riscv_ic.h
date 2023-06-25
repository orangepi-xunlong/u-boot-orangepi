/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * drivers/riscv/common/riscv_ic.h
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

#ifndef __RISCV_IC_H
#define __RISCV_IC_H

#if defined(CONFIG_MACH_SUN8IW21)
#include "../sun8iw21/platform.h"
#define ADDR_TPYE	u32
#define RISCV_STATUS_STR	"allwinner,sun8iw21p1-riscv"
#define RISCV_GPIO_INT_STR	"allwinner,sun8iw21p1-riscv-gpio-int"
#define RISCV_UART_STR		"allwinner,sun8iw21p1-riscv-uart"
#define RISCV_SHARE_SPACE	"allwinner,sun8iw21p1-riscv-share-space"
#elif defined(CONFIG_MACH_SUN50IW11)
#include "../sun50iw11/platform.h"
#define ADDR_TPYE	u32
#define RISCV_UART_STR	"allwinner,sun50iw11-riscv-uart"
#endif
#endif
