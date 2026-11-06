/*
 * Copyright (c) 2016-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __UART_LOADER_H__
#define __UART_LOADER_H__

typedef unsigned long ulong;

#define xyzModem_xmodem 1
#define xyzModem_ymodem 2
/* Don't define this until the protocol support is in place */
/*#define xyzModem_zmodem 3 */

uint32_t load_serial_ymodem(ulong offset, uint32_t mode);

#endif /* __UART_LOADER_H__ */
