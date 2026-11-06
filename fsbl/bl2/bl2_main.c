// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
 *
 */
#include <debug.h>
#include <console.h>
#include <platform.h>
#include <bl2.h>
#include <string.h>
#include <delay_timer.h>

void bl2_main(void)
{
	ATF_STATE = ATF_STATE_BL2_MAIN;

	platform_setup();

	load_ddr();

	load_rest();

	NOTICE("BL2 end.\n");

	while (1)
		;
}
