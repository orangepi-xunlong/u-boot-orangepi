/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#include <common.h>
#include <sys_config.h>
#include <sys_config_old.h>
#include <sunxi_bootparam.h>

extern u32 get_parameter_addr(void);
static int get_boot_display_params(struct user_display_param *params)
{
	struct display_params *save;
	struct spare_parameter_head_t *spare =
		(struct spare_parameter_head_t *)get_parameter_addr();

	if (!spare) {
		printf("warning: parameter address is null, can't read\n");
		memset(params, 0, sizeof(struct user_display_param));
		return -1;
	}
	save = &spare->para_data.display_param;
	strncpy(params->resolution, save->resolution, sizeof(params->resolution));
	strncpy(params->margin, save->margin, sizeof(params->margin));
	strncpy(params->vendorid, save->vendorid, sizeof(params->vendorid));

	params->format = save->format;
	params->depth  = save->depth;
	params->eotf   = save->eotf;
	params->color_space = save->color_space;
	return 0;
}

int bootparam_get_display_region_by_name(const char *name, char *buf, int size)
{
	int rlen = -1;
	struct user_display_param params;
	memset(&params, 0, sizeof(params));
	if (get_boot_display_params(&params) != 0)
		return -1;

	if (strcmp(name, "disp_rsl.fex") == 0) {
		strncpy(buf, params.resolution, size);
		rlen = strlen(buf);
	} else if (strcmp(name, "tv_vdid.fex") == 0) {
		strncpy(buf, params.vendorid, size);
		rlen = strlen(buf);
	} else {
		rlen = -1;
	}

	return rlen;
}

int get_disp_para_mode(void)
{
	int ret;
	int disp_para_zone = 0;

	ret = script_parser_fetch("disp", "disp_para_zone", &disp_para_zone, 1);
	if((disp_para_zone == 0) || ret) {
		return -1;
	}

	return 0;
}

int bootparam_get_hdmi_video_format(void)
{
	struct user_display_param params;
	memset(&params, 0, sizeof(params));
	if (get_boot_display_params(&params) != 0)
		return -1;
	return params.format;
}

int bootparam_get_hdmi_color_depth(void)
{
	struct user_display_param params;
	memset(&params, 0, sizeof(params));
	if (get_boot_display_params(&params) != 0)
		return -1;
	return params.depth;
}

int bootparam_get_hdmi_color_space(void)
{
	struct user_display_param params;
	memset(&params, 0, sizeof(params));
	if (get_boot_display_params(&params) != 0)
		return -1;
	return params.color_space;
}

int bootparam_get_hdmi_eotf(void)
{
	struct user_display_param params;
	memset(&params, 0, sizeof(params));
	if (get_boot_display_params(&params) != 0)
		return -1;
	return params.eotf;
}

int bootparam_get_disp_device_config(int type, int out[])
{
	int offset;
	struct user_display_param params;

	memset(&params, 0, sizeof(params));
	if (get_boot_display_params(&params) != 0)
		return -1;

	switch (type) {
	case 2:
		offset = 16;
		break;
	case 4:
	default:
		offset = 0;
		break;
	}

	out[0] = (params.format >> offset) & 0xffff;
	out[1] = (params.depth >> offset) & 0xffff;
	out[2] = (params.color_space >> offset) & 0xffff;
	out[3] = (params.eotf >> offset) & 0xffff;
	return 0;
}

