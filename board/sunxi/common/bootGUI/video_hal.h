#ifndef __VIDEO_HAL_H__
#define __VIDEO_HAL_H__
#include <sunxi_display2.h>

/*
* ------- video device hal : start --------
*/
typedef struct disp_device {
	int screen_id;
	int type;
	int mode;
	int format;
	int bits;
	int eotf;
	int cs;
	int do_hpd;
	unsigned char hpd_state;
	unsigned char be_force_open;
	unsigned char opened;
	unsigned char reserve;
} disp_device_t;

int hal_switch_device(disp_device_t *device, unsigned int fb_id);
int hal_get_hpd_state(int sel, int type);
int hal_is_support_mode(int sel, int type, int mode);
int hal_get_hdmi_edid(int sel, unsigned char *edid_buf, int length);
void hal_get_screen_size(int sel, unsigned int *width, unsigned int *height);

/* ------- video device hal : end -------- */

/*
* ------- video fb hal : start --------
*/
typedef struct rect_size {
	unsigned int width;
	unsigned int height;
} rect_sz_t;

typedef struct fb_config {
	int format_cfg;
	int bpp;
	int width;
	int height;
} fb_config_t;

typedef struct fb_layer {
	void *addr;
	int format;
	rect_sz_t size;
	int crop_left;
	int crop_top;
	rect_sz_t crop_sz;
	int bpp;
	int align;
} fb_layer_t;

void hal_get_fb_format_config(int fmt_cfg, int *bpp);
void *hal_request_layer(unsigned int fb_id);
int hal_release_layer(unsigned int fb_id, void *handle);
int hal_set_layer_addr(void *handle, void *addr);
int hal_set_layer_alpha_mode(void *handle,
	unsigned char alpha_mode, unsigned char alpha_value);
int hal_set_layer_geometry(void *handle,
	int width, int height, int bpp, int stride);
int hal_set_layer_crop(void *handle, int left, int top, int right, int bottom);
int hal_show_layer(void *handle, char is_show);

/* ------- video fb hal : end -------- */

#endif /* #ifndef __VIDEO_HAL_H__ */
