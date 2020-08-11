/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef _DISP_FEATURES_H_
#define _DISP_FEATURES_H_

#define SUPPORT_DSI
#define SUPPORT_LVDS
#define DISP_SCREEN_NUM 1
#define DISP_DEVICE_NUM 1

#define SUNXI_DE_BE0_OFFSET               0x60000
#define SUNXI_DE_BE1_OFFSET               0
#define SUNXI_DE_BE2_OFFSET               0

/*basic data information definition*/
enum __disp_layer_feat {
	DISP_LAYER_FEAT_GLOBAL_ALPHA = 1 << 0,
	DISP_LAYER_FEAT_PIXEL_ALPHA = 1 << 1,
	DISP_LAYER_FEAT_GLOBAL_PIXEL_ALPHA = 1 << 2,
	DISP_LAYER_FEAT_PRE_MULT_ALPHA = 1 << 3,
	DISP_LAYER_FEAT_COLOR_KEY = 1 << 4,
	DISP_LAYER_FEAT_ZORDER = 1 << 5,
	DISP_LAYER_FEAT_POS = 1 << 6,
	DISP_LAYER_FEAT_3D = 1 << 7,
	DISP_LAYER_FEAT_SCALE = 1 << 8,
	DISP_LAYER_FEAT_DE_INTERLACE = 1 << 9,
	DISP_LAYER_FEAT_COLOR_ENHANCE = 1 << 10,
	DISP_LAYER_FEAT_DETAIL_ENHANCE = 1 << 11,
};

struct disp_features {
	const unsigned int num_screens;
	const unsigned int *num_layers;
	const unsigned int num_scalers;
	const unsigned int num_captures;
	const unsigned int num_smart_backlights;
	const unsigned int num_color_managers;

	const unsigned int *supported_output_types;
	const enum __disp_layer_feat *layer_feats;
	const enum __disp_layer_feat *scaler_layer_feats;
	const unsigned int *smart_backlight_support;
	const unsigned int *image_enhance_support;
	const unsigned int *smart_color_support;
	const unsigned int *capture_support;
};

int bsp_disp_feat_get_num_screens(void);
int bsp_disp_feat_get_num_layers(unsigned int screen_id);
int bsp_disp_feat_get_num_scalers(void);
int bsp_disp_feat_get_num_color_managers(void);
int bsp_disp_feat_get_num_smart_backlights(void);
unsigned int bsp_disp_feat_get_supported_output_types(unsigned int screen_id);
/* enum sunxi_disp_output_id  */
/* sunxi_disp_feat_get_supported_outputs(unsigned int screen_id); */
enum __disp_layer_feat bsp_disp_feat_get_layer_feats(unsigned int screen_id,
						     unsigned int mode,
						     unsigned int scaler_index);
int bsp_disp_feat_get_smart_backlight_support(unsigned int screen_id);
int bsp_disp_feat_get_image_enhance_support(unsigned int screen_id);
int bsp_disp_feat_get_capture_support(unsigned int screen_id);
int bsp_disp_feat_get_num_captures(void);
extern int bsp_disp_get_screen_width(unsigned int screen_id);
extern int bsp_disp_get_screen_height(unsigned int screen_id);

int disp_init_feat(void);
#endif
