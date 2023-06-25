/*
 * Allwinner Sun50iw10 do poweroff in uboot with arisc
 *
 * (C) Copyright 2021  <xinouyang@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <sunxi_board.h>
#include <smc.h>

int sunxi_platform_power_off(void)
{
	/* reset arisc */
	/* as arisc in sun50iw10 is assert in boot0 and probe in uboot,
	 * and arisc probe is in board_late_init,
	 * and when using dragon sn , it won't run board_late_init,
	 * so have to reset arisc before poweroff*/
	int reg_val;
	reg_val = readl(SUNXI_R_CPUCFG_BASE);
	reg_val &= ~1;
	writel(reg_val, SUNXI_R_CPUCFG_BASE);
	reg_val |= 1;
	writel(reg_val, SUNXI_R_CPUCFG_BASE);
	/* call uboot poweroff */
	sunxi_arisc_probe();
	arm_svc_poweroff();
	return 0;
}

