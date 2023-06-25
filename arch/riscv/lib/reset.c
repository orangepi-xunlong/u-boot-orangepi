// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018, Bin Meng <bmeng.cn@gmail.com>
 */

#include <common.h>
#include <command.h>
#include <hang.h>

__weak void reset_misc(void)
{
}

int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	printf("resetting ...\n");

	disable_interrupts();

	reset_misc();
	reset_cpu(0);
	hang();

	return 0;
}
