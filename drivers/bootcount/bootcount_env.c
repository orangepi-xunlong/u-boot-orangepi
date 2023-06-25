// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2013
 * Heiko Schocher, DENX Software Engineering, hs@denx.de.
 */

#include <common.h>
#include <environment.h>

void bootcount_store(ulong a)
{
	int upgrade_available = env_get_ulong("upgrade_available", 10, 0);
#ifdef CONFIG_ARCH_SUNXI
	char bootcmd_previous[128];
#endif

	if (upgrade_available) {
		env_set_ulong("bootcount", a);
#ifdef CONFIG_ARCH_SUNXI
		/*
		 * In sunxi uboot, this area is sensitive.
		 * Some mode flags areare saved in the bootcmd of env.
		 * like fastboot mode,bootcmd is "run setargs_XXX boot_fastboot"
		 * So we can't simply save env back to flash
		 * ("bootcount_store" will save env back to flash)
		 * Here will save the normal mode flag back to flash
		 */
		memset(bootcmd_previous, 0x0, 128);
		strcpy(bootcmd_previous, env_get("bootcmd"));
		env_set("bootcmd", "run setargs_nand boot_normal");
#endif
		env_save();
#ifdef CONFIG_ARCH_SUNXI
		/* Keep previous commands in memory, whether in normal mode or other special modes */
		env_set("bootcmd", bootcmd_previous);
#endif
	}
}

ulong bootcount_load(void)
{
	int upgrade_available = env_get_ulong("upgrade_available", 10, 0);
	ulong val = 0;

	if (upgrade_available)
		val = env_get_ulong("bootcount", 10, 0);

	return val;
}
