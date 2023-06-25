/*
 * drivers/video/sunxi/bootGUI/video_hal.c
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
#include "video_hal.h"
#include "boot_gui_config.h"
#include "video_misc_hal.h"
#include <common.h>
#include <malloc.h>
#include "de_drv_v3.h"

enum { DISP_DEV_OPENED = 0x00000001,
       FB_REQ_LAYER = 0x00000002,
       FB_SHOW_LAYER = 0x00000004,
};

typedef struct hal_fb_dev {
	int state;
	void *layer_config;
	int dev_num;
	int screen_id[DISP_DEV_NUM];
	disp_device_t *disp_dev;
} hal_fb_dev_t;

static hal_fb_dev_t *get_fb_dev(unsigned int fb_id)
{
	static hal_fb_dev_t fb_dev[FRAMEBUFFER_NUM];

	if (FRAMEBUFFER_NUM > fb_id) {
		return &fb_dev[fb_id];
	} else {
		return NULL;
	}
}

int hal_switch_device(disp_device_t *device, unsigned int fb_id)
{
	hal_fb_dev_t *fb_dev;
	struct disp_device_config config;

	config.type = device->type;
	config.mode = device->mode;
	config.bits = device->bits;
	config.format = device->format;
	config.eotf = device->eotf;
	config.cs = device->cs;
	config.dvi_hdmi = device->dvi_hdmi;

	if (0 != _switch_device_config(device->screen_id, &config)) {
		pr_msg("switch device failed: sel=%d, type=%d, mode=%d, "
		       "format=%d, bits=%d, eotf=%d, cs=%d\n",
		       device->screen_id, device->type, device->mode,
		       device->format, device->bits, device->eotf, device->cs);
		return -1;
	}

	device->opened = 1;


	pr_msg("switch device: sel=%d, type=%d, mode=%d, format=%d, bits=%d, "
	       "eotf=%d, cs=%d\n",
	       device->screen_id, device->type, device->mode, device->format,
	       device->bits, device->eotf, device->cs);

	fb_dev = get_fb_dev(fb_id);
	if (NULL == fb_dev) {
		pr_error("this device can not be bounded to fb(%u)", fb_id);
		return -1;
	}
	fb_dev->disp_dev = device;


	if (FB_SHOW_LAYER & fb_dev->state) {
		_show_layer_on_dev(fb_dev->layer_config, device->screen_id, 1);
	}
	fb_dev->state |= DISP_DEV_OPENED;

	if (fb_dev->dev_num <
	    sizeof(fb_dev->screen_id) / sizeof(fb_dev->screen_id[0])) {
		fb_dev->screen_id[fb_dev->dev_num] = device->screen_id;
		++(fb_dev->dev_num);
	} else {
		pr_error("ERR: %s the fb_dev->screen_id[] is overflowed\n",
		       __func__);
	}

	return 0;
}

int hal_get_hpd_state(int sel, int type)
{
	return _get_hpd_state(sel, type);
}

int hal_is_support_mode(int sel, int type, int mode)
{
	return _is_support_mode(sel, type, mode);
}

int hal_get_hdmi_edid(int sel, unsigned char *edid_buf, int length)
{
	return _get_hdmi_edid(sel, edid_buf, length);
}

/* -------------------------------------------------------------------- */

void hal_get_screen_size(int sel, unsigned int *width, unsigned int *height)
{
	_get_screen_size(sel, width, height);
}

void hal_get_fb_format_config(int fmt_cfg, int *bpp)
{
	_get_fb_format_config(fmt_cfg, bpp);
}

void *hal_request_layer(unsigned int fb_id)
{
	hal_fb_dev_t *fb_dev = get_fb_dev(fb_id);
	if (NULL == fb_dev) {
		pr_error("%s: get fb%u dev fail\n", __func__, fb_id);
		return NULL;
	}

	fb_dev->state &= ~(FB_REQ_LAYER | FB_SHOW_LAYER);
	fb_dev->layer_config = (void *)calloc(sizeof(private_data), 1);
	if (NULL == fb_dev->layer_config) {
		pr_error("%s: malloc for private_data failed.\n", __func__);
		return NULL;
	}
	_simple_init_layer(fb_dev->layer_config);
	fb_dev->state |= FB_REQ_LAYER;

	return (void *)fb_dev;
}

int hal_release_layer(unsigned int fb_id, void *handle)
{
	hal_fb_dev_t *fb_dev = (hal_fb_dev_t *)handle;

	if ((NULL == fb_dev) || (fb_dev != get_fb_dev(fb_id))) {
		pr_error("%s: fb_id=%u, handle=%p, get_fb_dev=%p\n", __func__,
		       fb_id, handle, get_fb_dev(fb_id));
		return -1;
	}

	if (fb_dev->state & FB_SHOW_LAYER)
		hal_show_layer((void *)fb_dev, 0);

	if (fb_dev->state & FB_REQ_LAYER) {
		free(fb_dev->layer_config);
		fb_dev->layer_config = NULL;
		fb_dev->state &= ~FB_REQ_LAYER;
	}

	return 0;
}

int hal_set_layer_addr(void *handle, void *addr)
{
	hal_fb_dev_t *fb_dev = (hal_fb_dev_t *)handle;

	if (addr != _get_layer_addr(fb_dev->layer_config)) {
		_set_layer_addr(fb_dev->layer_config, addr);
		if (fb_dev->state & FB_SHOW_LAYER) {
			hal_show_layer((void *)fb_dev, 1);
		}
	}

	return 0;
}

int hal_set_layer_alpha_mode(void *handle, unsigned char alpha_mode,
			     unsigned char alpha_value)
{
	hal_fb_dev_t *fb_dev = (hal_fb_dev_t *)handle;

	_set_layer_alpha_mode(fb_dev->layer_config, alpha_mode, alpha_value);

	if (fb_dev->state & FB_SHOW_LAYER)
		hal_show_layer((void *)fb_dev, 1);

	return 0;
}

int hal_set_layer_geometry(void *handle, int width, int height, int bpp,
			   int stride)
{
	hal_fb_dev_t *fb_dev = (hal_fb_dev_t *)handle;

	_set_layer_geometry(fb_dev->layer_config, width, height, bpp, stride);

	if (fb_dev->state & FB_SHOW_LAYER)
		hal_show_layer((void *)fb_dev, 1);

	return 0;
}

/*
* screen window must be reset when set layer crop,
* but values of screen window maybe are float.
* hal_set_layer_crop can be call if one of these condition is true:
* a) the values of screen widow are not float;
* b) DE suport float srceen window;
* c) the applicator of BootGUI accept the result of the tiny offset
*   showing of bootlogo on fb of between boot and linux at smooth boot.
*/
int hal_set_layer_crop(void *handle, int left, int top, int right, int bottom)
{
#ifdef SUPORT_SET_FB_CROP
#error "FIXME: not verify yet"
	hal_fb_dev_t *fb_dev = (hal_fb_dev_t *)handle;

	int id = 0;
	for (id = 0; id < fb_dev->dev_num; ++id) {
		int scn_width = 0;
		int scn_height = 0;
		int fb_width = 0;
		int fb_height = 0;
		_get_screen_size(fb_dev->screen_id[id], &scn_width,
				 &scn_height);
		_get_layer_size(fb_dev->layer_config, &fb_width, &fb_height);
		if ((!scn_width || !scn_height || !fb_width || !fb_height) ||
		    (left * scn_width % fb_width) ||
		    (right * scn_width % fb_width) ||
		    (top * scn_height % fb_height) ||
		    (bottom * scn_height % fb_height)) {
			pr_error("not suport set layer crop[%d,%d,%d,%d], "
			       "scn[%d,%d], fb[%d,%d]\n",
			       left, top, right, bottom, scn_width, scn_height,
			       fb_width, fb_height);
			return -1;
		}
		pr_msg("%s: crop[%d,%d,%d,%d], scn[%d,%d], fb[%d,%d]\n",
		       __func__, left, top, right, bottom, scn_width,
		       scn_height, fb_width, fb_height);
	}

	_set_layer_crop(fb_dev->layer_config, left, top, right, bottom);

	if (fb_dev->state & FB_SHOW_LAYER)
		hal_show_layer((void *)fb_dev, 1);

	return 0;
#else
	return -1;
#endif
}

int hal_show_layer(void *handle, char is_show)
{
	int i = 0;
	hal_fb_dev_t *fb_dev = (hal_fb_dev_t *)handle;

	for (i = 0; i < fb_dev->dev_num; ++i)
		_show_layer_on_dev(fb_dev->layer_config, fb_dev->screen_id[i],
				   is_show);

	if (0 != is_show) {
		fb_dev->state |= FB_SHOW_LAYER;
	} else {
		fb_dev->state &= ~FB_SHOW_LAYER;
	}
	return 0;
}

int hal_save_boot_disp(void *handle)
{
	int disp_para0, disp_para1 = 0, disp_para2 = 0;
	hal_fb_dev_t *fb_dev = (hal_fb_dev_t *)handle;

	disp_para0 = (((fb_dev->disp_dev->type << 8) | fb_dev->disp_dev->mode)
		      << (fb_dev->disp_dev->screen_id * 16));

	disp_para1 = ((fb_dev->disp_dev->cs << 16) |
		      (fb_dev->disp_dev->bits << 8) | fb_dev->disp_dev->format);
	disp_para2 = (fb_dev->disp_dev->eotf);

	hal_save_int_to_kernel("boot_disp", disp_para0);
	hal_save_int_to_kernel("boot_disp1", disp_para1);
	hal_save_int_to_kernel("boot_disp2", disp_para2);
	return  0;
}
