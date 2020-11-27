/* 
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/timer.h>


void reboot(void)
{
	struct sunxi_timer_reg *timer_reg;
	struct sunxi_wdog *wdog;

	timer_reg = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;

	 /* enable watchdog */
	wdog = &timer_reg->wdog[0];
	printf("system will reboot\n");
	wdog->cfg = 1;
	wdog->mode = 1;
	while(1);

	return;
}

