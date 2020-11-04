/*
 * (C) Copyright 2017-2018
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 * Configuration settings for the Allwinner sunxi series of boards.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <sys_partition.h>
#include <malloc.h>
#include <fdt_support.h>
#include <sunxi_board.h>
#include <asm/io.h>
#include <smc.h>

int get_serial_num_from_file(char *serial)
{
	int partno = -1;
	char *sn_filename = NULL;
	char part_info[16] = {0};  /* format is "partno:0" */
	char addr_info[32] = {0};  /* "00000000" */
	char file_info[64] = {0};
	char *bmp_argv[6] = { "fatload", "sunxi_flash", part_info,
		addr_info, file_info, NULL };
	int nodeoffset;

	nodeoffset = fdt_path_offset(working_fdt, "/soc/serial_feature");
	if (nodeoffset > 0) {
		fdt_getprop_string(working_fdt, nodeoffset, "sn_filename", &sn_filename);
	}
	if ((nodeoffset < 0) || (strlen(sn_filename) == 0)) {
		pr_err("sunxi_serial: sn_filename is not set\n");
		return -1;
	}
	/* check private partition info */
	partno = sunxi_partition_get_partno_byname("private");
	if (partno < 0)
		return -1;

	/* get data from file */
	sprintf(part_info, "0:%x", partno);
	sprintf(addr_info, "%lx", (ulong)serial);
	sprintf(file_info, "%s", sn_filename);
	if (do_fat_fsload(0, 0, 5, bmp_argv)) {
		pr_err("load file(%s) error\n", bmp_argv[4]);
		return -1;
	}
	return 0;
}

int get_serial_num_from_chipid(char *serial)
{
	u32 sunxi_soc_chipid[4];
	u32 sunxi_serial[3];

#ifdef CONFIG_ARCH_SUN8IW8P1
	memset((void *)sunxi_soc_chipid, 0, sizeof(sunxi_soc_chipid));
#else
	sunxi_soc_chipid[0] = smc_readl(SUNXI_SID_BASE + 0x200);
	sunxi_soc_chipid[1] = smc_readl(SUNXI_SID_BASE + 0x200 + 0x4);
	sunxi_soc_chipid[2] = smc_readl(SUNXI_SID_BASE + 0x200 + 0x8);
	sunxi_soc_chipid[3] = smc_readl(SUNXI_SID_BASE + 0x200 + 0xc);
#endif

	/* high 76bits  for serialno */
	sunxi_serial[0] = (sunxi_soc_chipid[1] >> 20) & 0xFFF;
	sunxi_serial[1] = sunxi_soc_chipid[2];
	sunxi_serial[2] = sunxi_soc_chipid[3];

	sprintf(serial, "%03x%08x%08x", sunxi_serial[0], sunxi_serial[1], sunxi_serial[2]);
	return 0;
}

int sunxi_set_serial_num(void)
{
	char *serial = NULL;
	char *p = NULL;

	p = env_get("snum");
	if (p != NULL)
		return 0;

	serial = (char *)malloc_align(128, 64);
	if (!serial) {
		pr_err("error : malloc serial fail\n");
		return -1;
	}
	memset(serial, '\0', 128);

	get_serial_num_from_chipid(serial);

	pr_msg("serial num is: %s\n", serial);
	if (env_set("snum", serial))
		pr_err("error:set env snum fail\n");

	free_align(serial);

	return 0;
}
