/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "disp_features.h"
#include <common.h>

static const struct disp_features *disp_current_features;

static const unsigned int sun9i_disp_num_layers[] = {
	/* DISP_SCREEN0 */
	4,
	/* DISP_SCREEN1 */
	4,
	/* DISP_SCREEN2 */
	4,
};

static const unsigned int sun9i_disp_supported_output_types[] = {
	/* DISP_SCREEN0 */
	1,
	/* DISP_SCREEN1 */
	4,
	/* DISP_SCREEN2 */
	1,
};

static const enum __disp_layer_feat sun9i_disp_layer_feats[] = {
	/* DISP_SCREEN0 */
	DISP_LAYER_FEAT_GLOBAL_ALPHA | DISP_LAYER_FEAT_POS |
	    DISP_LAYER_FEAT_PRE_MULT_ALPHA | DISP_LAYER_FEAT_PIXEL_ALPHA |
	    DISP_LAYER_FEAT_COLOR_KEY | DISP_LAYER_FEAT_ZORDER |
	    DISP_LAYER_FEAT_GLOBAL_PIXEL_ALPHA,

	/* DISP_SCREEN1 */
	DISP_LAYER_FEAT_GLOBAL_ALPHA | DISP_LAYER_FEAT_POS |
	    DISP_LAYER_FEAT_PRE_MULT_ALPHA | DISP_LAYER_FEAT_PIXEL_ALPHA |
	    DISP_LAYER_FEAT_COLOR_KEY | DISP_LAYER_FEAT_ZORDER |
	    DISP_LAYER_FEAT_GLOBAL_PIXEL_ALPHA,

	/* DISP_SCREEN2 */
	DISP_LAYER_FEAT_GLOBAL_ALPHA | DISP_LAYER_FEAT_POS |
	    DISP_LAYER_FEAT_PRE_MULT_ALPHA | DISP_LAYER_FEAT_PIXEL_ALPHA |
	    DISP_LAYER_FEAT_COLOR_KEY | DISP_LAYER_FEAT_ZORDER |
	    DISP_LAYER_FEAT_GLOBAL_PIXEL_ALPHA,
};

static const enum __disp_layer_feat sun9i_disp_scaler_layer_feats[] = {
	/* DISP_SCREEN0 */
	DISP_LAYER_FEAT_SCALE | DISP_LAYER_FEAT_3D |
	    DISP_LAYER_FEAT_DE_INTERLACE | DISP_LAYER_FEAT_COLOR_ENHANCE |
	    DISP_LAYER_FEAT_DETAIL_ENHANCE,

	/* DISP_SCREEN1 */
	DISP_LAYER_FEAT_SCALE | DISP_LAYER_FEAT_3D |
	    DISP_LAYER_FEAT_DE_INTERLACE | DISP_LAYER_FEAT_COLOR_ENHANCE |
	    DISP_LAYER_FEAT_DETAIL_ENHANCE,

	/* DISP_SCREEN2 */
	DISP_LAYER_FEAT_SCALE | DISP_LAYER_FEAT_3D,
};

static const unsigned int sun9i_disp_smart_backlight_support[] = {
	/* DISP_SCREEN0 */
	1,
	/* DISP_SCREEN1 */
	1,
	/* DISP_SCREEN1 */
	0,
};

static const unsigned int sun9i_disp_image_enhance_support[] = {
	/* DISP_SCREEN0 */
	1,
	/* DISP_SCREEN1 */
	1,
	/* DISP_SCREEN2 */
	0,
};

static const unsigned int sun9i_disp_capture_support[] = {
	/* DISP_SCREEN0 */
	1,
	/* DISP_SCREEN1 */
	1,
	/* DISP_SCREEN2 */
	0,
};

static const struct disp_features sun9i_disp_features = {
	.num_screens = 3,
	.num_layers = sun9i_disp_num_layers,
	.num_scalers = 3,
	.num_captures = 2,
	.num_smart_backlights = 2,
	.supported_output_types = sun9i_disp_supported_output_types,
	.layer_feats = sun9i_disp_layer_feats,
	.scaler_layer_feats = sun9i_disp_scaler_layer_feats,
	.smart_backlight_support = sun9i_disp_smart_backlight_support,
	.image_enhance_support = sun9i_disp_image_enhance_support,
	.capture_support = sun9i_disp_capture_support,
};

static const unsigned int sun8iw3_disp_num_layers[] = {
	/* DISP_SCREEN0 */
	4,
};

static const unsigned int sun8iw3_disp_supported_output_types[] = {
	/* DISP_SCREEN0 */
	1,
};

static const enum __disp_layer_feat sun8iw3_disp_layer_feats[] = {
	/* DISP_SCREEN0 */
	DISP_LAYER_FEAT_GLOBAL_ALPHA | DISP_LAYER_FEAT_POS |
	    DISP_LAYER_FEAT_PRE_MULT_ALPHA | DISP_LAYER_FEAT_PIXEL_ALPHA |
	    DISP_LAYER_FEAT_COLOR_KEY | DISP_LAYER_FEAT_ZORDER,
};

static const enum __disp_layer_feat sun8iw3_disp_scaler_layer_feats[] = {
	/* DISP_SCREEN0 */
	DISP_LAYER_FEAT_SCALE | DISP_LAYER_FEAT_COLOR_ENHANCE |
	    DISP_LAYER_FEAT_DETAIL_ENHANCE,
};

static const unsigned int sun8iw3_disp_smart_backlight_support[] = {
	/* DISP_SCREEN0 */
	1,
};

static const unsigned int sun8iw3_disp_image_enhance_support[] = {
	/* DISP_SCREEN0 */
	1,
};

static const unsigned int sun8iw3_disp_capture_support[] = {
	/* DISP_SCREEN0 */
	1,
};

static const struct disp_features sun8iw3_disp_features = {
	.num_screens = 1,
	.num_layers = sun8iw3_disp_num_layers,
	.num_scalers = 1,
	.num_captures = 1,
	.num_smart_backlights = 1,
	.supported_output_types = sun8iw3_disp_supported_output_types,
	.layer_feats = sun8iw3_disp_layer_feats,
	.scaler_layer_feats = sun8iw3_disp_scaler_layer_feats,
	.smart_backlight_support = sun8iw3_disp_smart_backlight_support,
	.image_enhance_support = sun8iw3_disp_image_enhance_support,
	.capture_support = sun8iw3_disp_capture_support,
};

int bsp_disp_feat_get_num_screens(void)
{
	return disp_current_features->num_screens;
}

int bsp_disp_feat_get_num_layers(unsigned int screen_id)
{
	return disp_current_features->num_layers[screen_id];
}

int bsp_disp_feat_get_num_scalers(void)
{
	return disp_current_features->num_scalers;
}

int bsp_disp_feat_get_num_captures(void)
{
	return disp_current_features->num_captures;
}

int bsp_disp_feat_get_num_smart_backlights(void)
{
	return disp_current_features->num_smart_backlights;
}

int bsp_disp_feat_get_num_color_managers(void)
{
	return disp_current_features->num_color_managers;
}

unsigned int bsp_disp_feat_get_supported_output_types(unsigned int screen_id)
{
	return disp_current_features->supported_output_types[screen_id];
}

enum __disp_layer_feat bsp_disp_feat_get_layer_feats(unsigned int screen_id,
						     unsigned int mode,
						     unsigned int scaler_index)
{
	enum __disp_layer_feat layer_feats;

	layer_feats = disp_current_features->layer_feats[screen_id];
	if (mode == 4) {
		layer_feats |=
		    disp_current_features->scaler_layer_feats[scaler_index];
	}

	return layer_feats;
}

int bsp_disp_feat_get_smart_backlight_support(unsigned int screen_id)
{
	return disp_current_features->smart_backlight_support[screen_id];
}

int bsp_disp_feat_get_image_enhance_support(unsigned int screen_id)
{
	return disp_current_features->image_enhance_support[screen_id];
}

int bsp_disp_feat_get_capture_support(unsigned int screen_id)
{
	return disp_current_features->capture_support[screen_id];
}

int disp_init_feat(void)
{
#if defined(CONFIG_ARCH_SUN9IW1P1) || defined(CONFIG_ARCH_SUN6I)
	disp_current_features = &sun9i_disp_features;
#elif defined(CONFIG_ARCH_SUN8IW3P1) || defined(CONFIG_ARCH_SUN8IW5P1)
	disp_current_features = &sun8iw3_disp_features;
#else
#error "undefined platform!!!"
#endif
	return 0;
}
