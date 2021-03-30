#ifndef __DE_DRV_V3_H__
#define __DE_DRV_V3_H__

#include <sunxi_display2.h>

extern long disp_ioctl(void *hd, unsigned int cmd, void *arg);
extern void enable_hdmi_clock_prepare(void);

typedef struct disp_layer_config private_data;

static inline int _switch_device(int sel, int type, int mode)
{
	unsigned long arg[4] = {0};
	int ret = 0;

	switch (type) {
	case DISP_OUTPUT_TYPE_HDMI:
	case DISP_OUTPUT_TYPE_TV:
	case DISP_OUTPUT_TYPE_LCD:
	case DISP_OUTPUT_TYPE_VGA:
#ifdef ENABLE_HDMI_CLK_PREPARE
		if (type == DISP_OUTPUT_TYPE_HDMI)
			enable_hdmi_clock_prepare();
#endif
		arg[0] = sel;
		arg[1] = type;
		arg[2] = mode;
		ret = disp_ioctl(NULL, DISP_DEVICE_SWITCH, (void *)arg);
		break;
	default:
		return -1;
	}
	return ret;
}

static inline int _get_hpd_state(int sel, int type)
{
	unsigned long arg[4] = {0};
	int hpd_state = 0;

	arg[0] = sel;
	switch (type) {
	case DISP_OUTPUT_TYPE_HDMI:
		hpd_state = disp_ioctl(NULL, DISP_HDMI_GET_HPD_STATUS, (void *)arg);
		break;
	case DISP_OUTPUT_TYPE_TV:
		hpd_state = disp_ioctl(NULL, DISP_TV_GET_HPD_STATUS, (void *)arg);
		break;
	default:
		return 0;
	}
	return hpd_state ? 1 : 0;
}

static inline int _is_support_mode(int sel, int type, int mode)
{
	unsigned long arg[4] = {0};

	if (DISP_OUTPUT_TYPE_HDMI == type) {
		arg[0] = sel;
		arg[1] = mode;
		return disp_ioctl(NULL, DISP_HDMI_SUPPORT_MODE, (void *)arg);
	}
	return 0;
}

static inline int _get_hdmi_edid(int sel, unsigned char *edid_buf, int length)
{
	unsigned long arg[4] = {0};
	unsigned char *buf;
	const int edid_length = 0x100;

	arg[0] = sel;
	arg[1] = (unsigned long)&buf;
	disp_ioctl(NULL, DISP_HDMI_GET_EDID, (void *)arg);
	length = (length <= edid_length) ? length : edid_length;
	memcpy((void *)edid_buf, (void *)buf, length);
	return length;
}

static inline void _get_screen_size(int sel, unsigned int *width, unsigned int *height)
{
	unsigned long arg[4] = {0};
	arg[0] = sel;

	*width = disp_ioctl(NULL, DISP_GET_SCN_WIDTH, (void *)arg);
	*height = disp_ioctl(NULL, DISP_GET_SCN_HEIGHT, (void *)arg);
}

static inline void _get_fb_format_config(int fmt_cfg, int *bpp)
{
	if (8 == fmt_cfg) {
		/* DISP_FORMAT_RGB_888; */
		*bpp = 24;
	} else {
		/* DISP_FORMAT_ARGB_8888; */
		*bpp = 32;
	}
}

static inline void _simple_init_layer(void *layer_config)
{
	struct disp_layer_config *layer = (struct disp_layer_config *)layer_config;

	memset((void *)layer, 0, sizeof(*layer));
	layer->channel = 1;
	layer->layer_id = 0;

	layer->info.fb.flags = DISP_BF_NORMAL;
	layer->info.fb.scan = DISP_SCAN_PROGRESSIVE;
	layer->info.mode = LAYER_MODE_BUFFER;
	layer->info.b_trd_out = 0;
	layer->info.out_trd_mode = 0;
}

static inline void *_get_layer_addr(void *layer_config)
{
	struct disp_layer_config *layer = (struct disp_layer_config *)layer_config;
	return (void *)((uint)(layer->info.fb.addr[0]));
}

static inline void _set_layer_addr(void *layer_config, void *addr)
{
	struct disp_layer_config *layer = (struct disp_layer_config *)layer_config;
	layer->info.fb.addr[0] = (uint)addr; /* argb only has one planrer ? */
}

/* NOTICE: _set_layer_crop must be call after _set_layer_geometry calling */
static inline void _set_layer_crop(void *layer_config,
	int left, int top, int right, int bottom)
{
	struct disp_layer_config *layer = (struct disp_layer_config *)layer_config;
	layer->info.fb.crop.x = ((unsigned long long)left) << 32;
	layer->info.fb.crop.y = ((unsigned long long)top) << 32;
	layer->info.fb.crop.width = ((unsigned long long)(right - left)) << 32;
	layer->info.fb.crop.height = ((unsigned long long)(bottom - top)) << 32;
}

static inline void _get_layer_size(void *layer_config,
	int *width, int *height)
{
	struct disp_layer_config *layer = (struct disp_layer_config *)layer_config;
	*width = layer->info.fb.size[0].width;
	*height = layer->info.fb.size[0].height;
}

static inline void _set_layer_geometry(void *layer_config,
	int width, int height, int bpp, int byte_align)
{
	struct disp_layer_config *layer = (struct disp_layer_config *)layer_config;

	if (32 == bpp) {
		layer->info.alpha_mode = 0x0;
		layer->info.alpha_value = 0x0;
		layer->info.fb.format = DISP_FORMAT_ARGB_8888;
	} else if (24 == bpp) {
		layer->info.alpha_mode = 0x1;
		layer->info.alpha_value = 0xFF;
		layer->info.fb.format = DISP_FORMAT_RGB_888;
	} else {
		printf("%s: no support the bpp[%d]\n", __func__, bpp);
		layer->info.alpha_mode = 0x0;
		layer->info.alpha_value = 0x0;
		layer->info.fb.format = DISP_FORMAT_ARGB_8888;
	}
	layer->info.fb.size[0].width = width;
	layer->info.fb.size[0].height = height;
	/* notice: reset crop as same as fb size */
	_set_layer_crop((void *)layer, 0, 0, width, height);
}

static void _show_layer_on_dev(void *layer_config, int sel, char is_show)
{
	uint arg[4] = {0, 0, 0, 0};
	struct disp_layer_config *layer = (struct disp_layer_config *)layer_config;

	if (is_show) {
		if (0 == layer->info.fb.addr[0])
			return;
		_get_screen_size(sel, &layer->info.screen_win.width,
			&layer->info.screen_win.height);
		if ((0 == layer->info.screen_win.height)
			|| (0 == layer->info.screen_win.width))
			return; /* do not show the layer if device is not opened */

		layer->info.screen_win.x = 0;
		layer->info.screen_win.y = 0;

		layer->enable = 1;
	} else {
		layer->enable = 0;
	}

	arg[0] = sel;
	arg[1] = (uint)layer;
	arg[2] = 1;
	disp_ioctl(NULL, DISP_LAYER_SET_CONFIG, (void *)arg);
}


#endif /* #ifndef __DE_DRV_V3_H__ */
