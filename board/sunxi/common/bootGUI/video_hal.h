#ifndef __VIDEO_HAL_H__
#define __VIDEO_HAL_H__

#define DISPLAY_PARTITION_NAME "Reserve0"

inline int hal_fat_fsload(char *part_name, char *file_name, char *load_addr, unsigned long length);


/*
* ------- video device hal : start --------
*/
typedef struct disp_device {
	int screen_id;
	int type;
	int mode;
	int do_hpd;
	unsigned char hpd_state;
	unsigned char be_force_open;
	unsigned char opened;
	unsigned char reserve;
} disp_device_t;

disp_device_t *hal_get_disp_devices(disp_device_t *disp_dev_list, int *dev_num);
int hal_switch_device(disp_device_t *device, unsigned int fb_id);
int hal_get_hpd_state(int sel, int type);
int hal_is_support_mode(int sel, int type, int mode);
int hal_get_hdmi_edid(int sel, unsigned char *edid_buf, int length);
int hal_get_mode_check(int type, int def_check); /* get the stratagy of check mode , spec for hdmi */
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

int hal_get_fb_configs(fb_config_t *fb_cfgs, int *fb_num);
void *hal_request_layer(unsigned int fb_id);
int hal_release_layer(unsigned int fb_id, void *handle);
int hal_set_layer_addr(void *handle, void *addr);
int hal_set_layer_geometry(void *handle, int width, int height, int bpp, int byte_align);
int hal_set_layer_crop(void *handle, int left, int top, int right, int bottom);
int hal_show_layer(void *handle, char is_show);
int hal_save_int_to_kernel(char *name, int value);
int hal_save_string_to_kernel(char *name, char *str);

/* ------- video fb hal : end -------- */

#endif /* #ifndef __VIDEO_HAL_H__ */
