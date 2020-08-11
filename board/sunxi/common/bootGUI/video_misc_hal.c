#include <common.h>
#include <malloc.h>
#include <sys_config.h>
#include <fdt_support.h>
#include <sunxi_display2.h>
#include "video_misc_hal.h"

#if defined(CONFIG_BOOT_PARAMETER)
#include <sunxi_bootparam.h>
#endif

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
		printf("set disp.%s fail. using defval=%d\n", name, defval);
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
	printf("save_int_to_kernel %s.%s(0x%x) code:%s\n",
		DISP_FDT_NODE, name, value, fdt_strerror(ret));
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
	printf("save_string_to_kernel %s.%s(%s). ret-code:%s\n",
		DISP_FDT_NODE, name, str, fdt_strerror(ret));
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
		printf("Can't get display(type:%d) config from boot\n", type);
		return -1;
	}
	if (tmp[2] == 0 || tmp[3] == 0) {
		printf("invalid display config\n");
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
		printf("get hdmi config from bootparam success.\n");
		config = &saved;
	}
	if (!config)
		return -1;

	printf("hdmi config: format-%d bits-%d cs-%d eotf-%d\n",
			config->format, config->bits, config->cs, config->eotf);

#ifndef CONFIG_SUNXI_MULITCORE_BOOT
	int node = get_disp_fdt_node();
	fdt_appendprop_u32(working_fdt, node, tag[disp], config->type);
	fdt_appendprop_u32(working_fdt, node, tag[disp], config->mode);
	fdt_appendprop_u32(working_fdt, node, tag[disp], config->format);
	fdt_appendprop_u32(working_fdt, node, tag[disp], config->bits);
	fdt_appendprop_u32(working_fdt, node, tag[disp], config->cs);
	fdt_appendprop_u32(working_fdt, node, tag[disp], config->eotf);
#else
	uint32_t array[6];
	array[0] = config->type;
	array[1] = config->mode;
	array[2] = config->format;
	array[3] = config->bits;
	array[4] = config->cs;
	array[5] = config->eotf;

	sunxi_fdt_getprop_store_array(working_fdt,
			DISP_FDT_NODE, tag[disp], array, 6);
#endif
	return 0;
}

/*---------------------------------------------------------*/

int hal_fat_fsload(char *part_name, char *file_name,
	char *load_addr, ulong length)
{
	int readed_len = -1;

#ifdef CONFIG_BOOT_PARAMETER
	int read_from_fs = 1;
	read_from_fs = get_disp_para_mode();
	if (!read_from_fs) {
		readed_len = bootparam_get_display_region_by_name(
			file_name, load_addr, length);
	}
#ifdef HAS_FAT_FSLOAD
	if (read_from_fs && (0 >= readed_len))
		readed_len = aw_fat_fsload(part_name, file_name, load_addr, length);
#endif /* #ifdef HAS_FAT_FSLOAD */
#endif /* #ifdef CONFIG_BOOT_PARAMETER */

	return readed_len;
}
