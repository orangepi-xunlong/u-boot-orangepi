/*
 * Allwinner SoCs bootGUI.
 *
 * Copyright (C) 2017 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <common.h>
#include <malloc.h>
#include <sys_config.h>
#include <fdt_support.h>
#include <sunxi_display2.h>
#include "video_misc_hal.h"
#include <sys_partition.h>
#include <asm/global_data.h>
#include <mmc_cmd.h>

#if defined(CONFIG_BOOT_PARAMETER)
#include <sunxi_bootparam.h>
#endif
DECLARE_GLOBAL_DATA_PTR;

#define DISP_FDT_NODE "disp"

int get_disp_fdt_node(void)
{
	static int fdt_node = -1;

	if (0 <= fdt_node)
		return fdt_node;
	/* notice: make sure we use the only one nodt "disp". */
	fdt_node = fdt_path_offset(working_fdt, DISP_FDT_NODE);
	assert(fdt_node >= 0);
	return fdt_node;
}

void disp_getprop_by_name(int node, const char *name,
	unsigned int *value, unsigned int defval)
{
	if (fdt_getprop_u32(working_fdt, node, name, value) < 0) {
		pr_error("set disp.%s fail. using defval=%u\n", name, defval);
		*value = defval;
	}
}

int hal_save_int_to_kernel(char *name, int value)
{
	int ret = -1;

#ifndef CONFIG_SUNXI_MULITCORE_BOOT
	int node = get_disp_fdt_node();
	ret = fdt_setprop_u32(working_fdt, node, name, (uint32_t)value);
#else
	ret = sunxi_fdt_getprop_store(working_fdt,
		DISP_FDT_NODE, name, (uint32_t)value);
#endif
	/*pr_msg("save_int_to_kernel %s.%s(0x%x) code:%s\n",*/
		/*DISP_FDT_NODE, name, value, fdt_strerror(ret));*/

	env_set_ulong(name, (ulong)value);

	return ret;
}

int hal_save_string_to_kernel(char *name, char *str)
{
	int ret = -1;

#ifndef CONFIG_SUNXI_MULITCORE_BOOT
	int node = get_disp_fdt_node();
	ret = fdt_setprop_string(working_fdt, node, name, str);
#else
	ret = sunxi_fdt_getprop_store_string(working_fdt,
		DISP_FDT_NODE, name, str);
#endif
	/*pr_msg("save_string_to_kernel %s.%s(%s). ret-code:%s\n",*/
		/*DISP_FDT_NODE, name, str, fdt_strerror(ret));*/

	env_set(name, str);

	return ret;
}

int hal_get_disp_device_config(int type, void *config)
{

#ifdef CONFIG_BOOT_PARAMETER
	int tmp[4] = {0}; /* format, depth, cs, eotf */
	struct disp_device_config *out = config;

	if (!out)
		return -1;
	memset(out, 0, sizeof(*out));

	if (bootparam_get_disp_device_config(type, tmp)) {
		pr_error("Can't get display(type:%d) config from boot\n", type);
		return -1;
	}
	if (tmp[2] == 0 || tmp[3] == 0) {
		pr_error("invalid display config\n");
		return -1;
	}
	out->type   = type;
	out->format = tmp[0];
	out->bits   = tmp[1];
	out->cs     = tmp[2];
	out->eotf   = tmp[3];
	return 0;
#else
	return -1;
#endif
}

int hal_save_disp_device_config_to_kernel(int disp, void *from)
{
	const char *tag[] = {
		"disp_config0",
		"disp_config1"
	};
	struct disp_device_config saved;
	struct disp_device_config *config = from;

	if (disp < 0 || disp > 1)
		return -1;

	if (!config && !hal_get_disp_device_config(DISP_OUTPUT_TYPE_HDMI, &saved)) {
		pr_msg("get hdmi config from bootparam success.\n");
		config = &saved;
	}
	if (!config)
		return -1;

	/*pr_msg("hdmi config: format-%d bits-%d cs-%d eotf-%d\n",*/
			/*config->format, config->bits, config->cs, config->eotf);*/

#ifndef CONFIG_SUNXI_MULITCORE_BOOT
	int node = get_disp_fdt_node();
	fdt_appendprop_u32(working_fdt, node, tag[disp], config->type);
	fdt_appendprop_u32(working_fdt, node, tag[disp], config->mode);
	fdt_appendprop_u32(working_fdt, node, tag[disp], config->format);
	fdt_appendprop_u32(working_fdt, node, tag[disp], config->bits);
	fdt_appendprop_u32(working_fdt, node, tag[disp], config->cs);
	fdt_appendprop_u32(working_fdt, node, tag[disp], config->eotf);
#else
/*
	uint32_t array[6];
	array[0] = config->type;
	array[1] = config->mode;
	array[2] = config->format;
	array[3] = config->bits;
	array[4] = config->cs;
	array[5] = config->eotf;

	sunxi_fdt_getprop_store_array(working_fdt,
			DISP_FDT_NODE, tag[disp], array, 6);
*/
	/* We have already save the config on hal_switch_device() */
	pr_msg("bootGUI save config: %s\n", tag[disp]);
#endif
	return 0;
}

/*---------------------------------------------------------*/

int hal_fat_fsload(char *part_name, char *file_name, char *buf, ulong length)
{
	int ret = 0;
	int work_mode = get_boot_work_mode();
	if ((gd->boot_logo_addr) || (work_mode == WORK_MODE_CARD_PRODUCT
		|| work_mode == WORK_MODE_CARD_UPDATE))
		return 0;

	char len[16] = {0};
	char load_addr[16];

	char *const mmc_argv[3] = {"mmc", "part", NULL};
	do_mmcops(NULL, 0, 3, mmc_argv);

	char *const part_argv[6] = {"ext2load", "mmc", "0",
				    load_addr, file_name,     NULL};
	snprintf(len, 16, "%lu", length);
	sprintf(load_addr, "%lx", (ulong)buf);

	if (!do_ext2load(NULL, 0, 5, part_argv))
		ret = length;

	return ret;
}
