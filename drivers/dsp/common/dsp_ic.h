/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * drivers/dsp/common/dsp_ic.h
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

#ifndef __DSP_IC_H
#define __DSP_IC_H

#if defined(CONFIG_MACH_SUN8IW20)
#include "../sun8iw20/platform.h"
#define ADDR_TPYE	u32
#define DSP_STATUS_STR		"allwinner,sun8iw20p1-dsp"
#define DSP_GPIO_INT_STR	"allwinner,sun8iw20p1-dsp-gpio-int"
#define DSP_UART_STR		"allwinner,sun8iw20p1-dsp-uart"
#define DSP_SHARE_SPACE		"allwinner,sun8iw20p1-dsp-share-space"
#elif defined(CONFIG_MACH_SUN20IW1)
#include "../sun8iw20/platform.h"
#define ADDR_TPYE	u64
#define DSP_STATUS_STR		"allwinner,sun20iw1-dsp"
#define DSP_GPIO_INT_STR	"allwinner,sun20iw1-dsp-gpio-int"
#define DSP_UART_STR		"allwinner,sun20iw1-dsp-uart"
#define DSP_SHARE_SPACE		"allwinner,sun20iw1-dsp-share-space"
#elif defined(CONFIG_MACH_SUN50IW11)
#include "../sun50iw11/platform.h"
#define ADDR_TPYE	u32
#define DSP_UART_STR	"allwinner,sun50iw11-dsp-uart"
#endif
#endif
