
#ifndef __BOOT_PARAM_H__
#define __BOOT_PARAM_H__

struct user_display_param {
	/*
	 * resolution store like that:
	 *  (type << 8 | mode)
	 */
	char resolution[32];
	char margin[32];

	/*
	 * vender id of the last hdmi output device
	 */
	char vendorid[64];

	/*
	 * Add more fields for HDR support
	 */
	int format;       /* RGB / YUV444 / YUV422 / YUV420     */
	int depth;        /* Color depth: 8 / 10 / 12 / 16      */
	int eotf;         /* Electro-Optical Transfer Functions */
	int color_space;  /* BT.601 / BT.709 / BT.2020          */
};

int bootparam_get_display_region_by_name(const char *name, char *buf, int size);
int get_disp_para_mode(void);

int bootparam_get_hdmi_video_format(void);
int bootparam_get_hdmi_color_depth(void);
int bootparam_get_hdmi_color_space(void);
int bootparam_get_hdmi_eotf(void);
int bootparam_get_disp_device_config(int type, int out[]);

#endif
