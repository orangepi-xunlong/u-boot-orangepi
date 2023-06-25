/*
 * Copyright (C) 2016 Allwinnertech
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Adjustable factor-based clock implementation
 */
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <fdt_support.h>
#include <linux/compat.h>
#include <clk/clk.h>

static int do_sunxi_clk_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int i;
	int node;
	struct clk *clk;

	node = fdt_path_offset(working_fdt, "/soc/clk_test");
	if (node < 0) {
		printf("not find the node:/soc/clk_test\n");
		return -1;
	}

	of_periph_clk_config_setup(node);
	for (i = 0; i < 4; i++) {
		clk = of_clk_get(node, i);
		if (IS_ERR(clk))
			printf("fail to get clk for eink\n");
	}

	printf("clk test end\n");
	return 0;
}

U_BOOT_CMD(
	sunxi_clk,	3,	1,	do_sunxi_clk_test,
	"do clk test",
	"sunxi_clk test_cmd"
);
