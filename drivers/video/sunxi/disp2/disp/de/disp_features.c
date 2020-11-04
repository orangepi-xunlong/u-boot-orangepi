/*
 * drivers/video/sunxi/disp2/disp/de/disp_features.c
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include "disp_features.h"

int bsp_disp_feat_get_num_screens(void)
{
	return de_feat_get_num_screens();
}

int bsp_disp_feat_get_num_devices(void)
{
	return de_feat_get_num_devices();
}

int bsp_disp_feat_get_num_channels(unsigned int disp)
{
	return de_feat_get_num_chns(disp);
}

int bsp_disp_feat_get_num_layers(unsigned int disp)
{
	return de_feat_get_num_layers(disp);
}

int bsp_disp_feat_get_num_layers_by_chn(unsigned int disp, unsigned int chn)
{
	return de_feat_get_num_layers_by_chn(disp, chn);
}

/*
 * Query whether specified timing controller support the output_type passed as parameter
 * @disp: the index of timing controller
 * @output_type: the display output type
 * On support, returns 1. Otherwise, returns 0.
 */
int bsp_disp_feat_is_supported_output_types(unsigned int disp, unsigned int output_type)
{
	return de_feat_is_supported_output_types(disp, output_type);
}

int disp_feat_is_support_smbl(unsigned int disp)
{
	return de_feat_is_support_smbl(disp);
}

int bsp_disp_feat_is_support_capture(unsigned int disp)
{
	return de_feat_is_support_wb(disp);
}


int disp_init_feat(struct disp_feat_init *feat_init)
{
#ifdef DE_VERSION_V33X
	struct de_feat_init de_feat;
	de_feat.chn_cfg_mode = feat_init->chn_cfg_mode;
	return de_feat_init_config(&de_feat);
#else
	return de_feat_init();
#endif
}

int disp_feat_is_using_rcq(unsigned int disp)
{
#if defined(DE_VERSION_V33X)
       return de_feat_is_using_rcq(disp);
#endif
       return 0;
}

int disp_feat_is_using_wb_rcq(unsigned int wb)
{
#if defined(DE_VERSION_V33X)
       return de_feat_is_using_wb_rcq(wb);
#endif
       return 0;
}
