/*
 * Copyright (c) 2013-2014, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <console.h>
#include <assert.h>
#include <debug.h>

// #include "cv_private.h"

/* Putchar() should either return the character printed or EOF in case of error.
 * Our current console_putc() function assumes success and returns the
 * character. Write all other printing functions in terms of putchar(), if
 * possible, so they all benefit when this is improved.
 */
#if defined(BL1_RTL_SIM)
int putchar(int c)
{
	static unsigned char *log_ptr = (unsigned char *)RTL_SIM_LOG_BUF;
	static size_t cnt = 0;

	*log_ptr++ = c;
	cnt++;
	if(cnt >= RTL_SIM_LOG_BUF_SIZE) {
		log_ptr = (unsigned char *)RTL_SIM_LOG_BUF;
		cnt = 0;
	}

	return c;
}
#else
int putchar(int c)
{
	int res;
/*
#if defined(IMAGE_BL1)
	// Skip UART message
	if (plat_bm_gpio_read(BIT_MASK_GPIO_DIS_UART_LOG))
		return c;
#endif
*/
	if (console_putc((unsigned char)c) >= 0)
		res = c;
	else
		res = EOF;

	return res;
}
#endif