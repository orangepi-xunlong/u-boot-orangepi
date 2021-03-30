#include "video_hal.h"
#include <common.h>
#include <malloc.h>
#include <sys_config.h>
#include <fdt_support.h>
#include "hdmi_manage.h"
#include "boot_gui_config.h"
#include <sunxi_board.h>
#include <ext_common.h>

#if defined(CONFIG_VIDEO_SUNXI_V3)
#include "de_drv_v3.h"
#else
#error "please CONFIG_VIDEO_SUNXI_Vx\n"
#endif

#define DISPLAY_RSL_FILENAME "/boot/orangepiEnv.txt"

enum {
	DISP_DEV_OPENED = 0x00000001,
	FB_REQ_LAYER    = 0x00000002,
	FB_SHOW_LAYER   = 0x00000004,
};

typedef struct hal_fb_dev {
	int state;
	void *layer_config;
	int dev_num;
	int screen_id[DISP_DEV_NUM];
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

static int get_disp_fdt_node(void)
{
	static int fdt_node = -1;

	if (0 <= fdt_node)
		return fdt_node;
	/* notice: make sure we use the only one nodt "disp". */
	fdt_node = fdt_path_offset(working_fdt, "disp");
	assert(fdt_node >= 0);
	return fdt_node;
}

int hal_save_int_to_kernel(char *name, int value)
{
	int ret = -1;
	int node = get_disp_fdt_node();

	ret = fdt_setprop_u32(working_fdt, node, name, (uint32_t)value);
	printf("fdt_setprop_u32 %s.%s(0x%x) code:%s\n",
		"disp", name, value, fdt_strerror(ret));

	setenv_ulong(name, (ulong)value);

	return ret;
}

int hal_save_string_to_kernel(char *name, char *str)
{
	int ret = -1;
	int node = get_disp_fdt_node();

	ret = fdt_setprop_string(working_fdt, node, name, str);
	printf("fdt_setprop_string %s.%s(%s). ret-code:%s\n",
		"disp", name, str, fdt_strerror(ret));

	printf("save_string_to_kernel %s : %s\n", name, str);

        setenv(name, str);

	return ret;
}

static void disp_getprop_by_name(int node, const char *name, uint32_t *value, uint32_t defval)
{
	if (fdt_getprop_u32(working_fdt, node, name, value) < 0) {
		printf("fetch script data disp.%s fail. using defval=%d\n", name, defval);
		*value = defval;
	}
}

inline int hal_fat_fsload(char *part_name, char *file_name, char *load_addr, ulong length)
{
#ifdef HAS_FAT_FSLOAD
	return aw_fat_fsload(part_name, file_name, load_addr, length);
#else
	return 0;
#endif
}

int hal_ext4_fsload(char *part_name, char *file_name, char *buf, ulong length)
{
        char len[16] = {0};
        char load_addr[16];
        char * part_argv[5] = {NULL};

        int storage_type = get_boot_storage_type();

        part_argv[0] = "ext4load";
        part_argv[1] = "mmc";
        part_argv[2] = "0";
        if ( storage_type == 2 )
                part_argv[2] = "2";
        part_argv[3] = load_addr;
        part_argv[4] = file_name;

        snprintf(len, 16, "%lu", length);
        sprintf(load_addr, "%lx", (ulong)buf);

        if (!do_ext4_load(NULL, 0, 5, part_argv))
                return length;

        return 0;
}

static disp_device_t *hpd_detect(disp_device_t **devices, int dev_num)
{
	int i;
	unsigned int count = HPD_DETECT_COUNT0;
	disp_device_t *device = NULL;

	device = devices[0];
	while (count--) {
		device->hpd_state = hal_get_hpd_state(
			devices[0]->screen_id, device->type);
		if (device->hpd_state) {
			printf("main-hpd:count=%d,sel=%d,type=%d\n",
				HPD_DETECT_COUNT0 - count,
				device->screen_id, device->type);
			return device;
		}
	}
	count = HPD_DETECT_COUNT1;
	while (count--) {
		for (i = 0; i < dev_num; ++i) {
			device = devices[i];
			device->hpd_state = hal_get_hpd_state(
				device->screen_id, device->type);
			if (device->hpd_state) {
				printf("ext-hpd:count=%d,sel=%d,type=%d\n",
					HPD_DETECT_COUNT1 - count,
					device->screen_id, device->type);
				return device;
			}
		}
	}
	return NULL;
}

static int get_output_mode(disp_device_t *disp_dev, char *buf, int num)
{
        char *p = buf;
        char *needle = NULL;

        while (num > 0) {

                while ((*p != '\n') && (*p != '\0')){
                        p++;
                }

                *p++ = '\0';

		needle = strstr(buf, "disp_mode=");
                if (needle != NULL) {
                        needle = strstr(buf, "=");
                        ++needle;

                        if (needle != NULL && disp_dev->type == DISP_OUTPUT_TYPE_HDMI) {

                                if (strcmp(needle, "480i") == 0)
                                        disp_dev->mode = DISP_TV_MOD_480I;
                                else if (strcmp(needle, "576i") == 0)
                                        disp_dev->mode = DISP_TV_MOD_576I;
                                else if (strcmp(needle, "480p") == 0)
                                        disp_dev->mode = DISP_TV_MOD_480P;
                                else if (strcmp(needle, "576p") == 0)
                                        disp_dev->mode = DISP_TV_MOD_576P;
                                else if (strcmp(needle, "720p50") == 0)
                                        disp_dev->mode = DISP_TV_MOD_720P_50HZ;
                                else if (strcmp(needle, "720p60") == 0)
                                        disp_dev->mode = DISP_TV_MOD_720P_60HZ;
                                else if (strcmp(needle, "1080i50") == 0)
                                        disp_dev->mode = DISP_TV_MOD_1080I_50HZ;
                                else if (strcmp(needle, "1080i60") == 0)
                                        disp_dev->mode = DISP_TV_MOD_1080I_60HZ;
                                else if (strcmp(needle, "1080p24") == 0)
                                        disp_dev->mode = DISP_TV_MOD_1080P_24HZ;
                                else if (strcmp(needle, "1080p50") == 0)
                                        disp_dev->mode = DISP_TV_MOD_1080P_50HZ;
                                else if (strcmp(needle, "1080p60") == 0)
                                        disp_dev->mode = DISP_TV_MOD_1080P_60HZ;
                                else if (strcmp(needle, "1080p25") == 0)
                                        disp_dev->mode = DISP_TV_MOD_1080P_25HZ;
                                else if (strcmp(needle, "1080p30") == 0)
                                        disp_dev->mode = DISP_TV_MOD_1080P_30HZ;
				else if (strcmp(needle, "2160p24") == 0)
					disp_dev->mode = DISP_TV_MOD_3840_2160P_24HZ;
                                else if (strcmp(needle, "2160p25") == 0)
                                        disp_dev->mode = DISP_TV_MOD_3840_2160P_25HZ;
                                else if (strcmp(needle, "2160p30") == 0)
                                        disp_dev->mode = DISP_TV_MOD_3840_2160P_30HZ;
                                else{
                                        printf("disp_mode=%s is error, set HDMI to 1080p60hz by default\n", needle);
                                        disp_dev->mode = DISP_TV_MOD_1080P_60HZ;
                                        return -1;
                                }

                                printf("Set HDMI disp_mode to %s\n", needle);
                                return 0;
                        }
                }

                num -= (p - buf);
                buf = p;
        }

        if (disp_dev->type == DISP_OUTPUT_TYPE_HDMI) {
        	printf("disp_mode setting is error, set HDMI to 1080p60hz by default\n");
        	disp_dev->mode = DISP_TV_MOD_1080P_60HZ;
	}

        return -1;
}

disp_device_t *hal_get_disp_devices(disp_device_t *disp_dev_list, int *dev_num)
{
	int num = 0;
	int node = 0;
	char prop[32] = {'\n'};
	int read_bytes = 0;
	char buf[512] = {0};
	disp_device_t *disp_dev = NULL;
	uint32_t value = 0;
	int mode = -1;

	disp_device_t *hpd_dev[DISP_DEV_NUM];
	int hpd_dev_num = 0;

	node = get_disp_fdt_node();

	//read_bytes = hal_fat_fsload(DISPLAY_PARTITION_NAME,
	//DISPLAY_RSL_FILENAME, buf, sizeof(buf));
        read_bytes = hal_ext4_fsload(NULL,
        DISPLAY_RSL_FILENAME, buf, sizeof(buf));

	disp_dev = disp_dev_list;
	for (num = 0; num < *dev_num; ++num, ++disp_dev) {
		sprintf(prop, "dev%d_output_type", num);
		disp_getprop_by_name(node, prop, &value, 0);
		if (0 == value)
			break;
		memset((void *)disp_dev, 0, sizeof(*disp_dev));
		disp_dev->type = value;
		sprintf(prop, "dev%d_screen_id", num);
		disp_getprop_by_name(node, prop, (uint32_t *)&disp_dev->screen_id, 0);
		sprintf(prop, "dev%d_do_hpd", num);
		disp_getprop_by_name(node, prop, (uint32_t *)&disp_dev->do_hpd, 0);
		if (0 < read_bytes) {
			//mode = get_display_resolution(disp_dev->type, buf, read_bytes);
			//if (-1 != mode)
			//	disp_dev->mode = mode;
			get_output_mode(disp_dev, buf, read_bytes);
		}else if ((0 >= read_bytes) || (-1 == mode)) {
			sprintf(prop, "dev%d_output_mode", num);
			disp_getprop_by_name(node, prop, (uint32_t *)&disp_dev->mode, 0);
		}
		if (disp_dev->do_hpd) {
			hpd_dev[hpd_dev_num] = disp_dev;
			++hpd_dev_num;
		}
	}

	if (0 < num) {
		*dev_num = num;
	} else {
		printf("no found any device of disp. num=%d\n", num);
		memset((void *)disp_dev_list, 0, sizeof(*disp_dev_list));
		disp_dev_list->type = DISP_OUTPUT_TYPE_LCD;
		*dev_num = 1;
	}
	num = 0;

	if (hpd_dev_num) {
		disp_dev = hpd_detect(hpd_dev, hpd_dev_num);
		if (NULL != disp_dev) {
			return disp_dev;
		}
	}
	if (1 < *dev_num) {
		disp_getprop_by_name(node, "def_output_dev", (uint32_t *)&num, 0);
		printf("hpd_dev_num=%d, id of def_output_dev is %d\n", hpd_dev_num, num);
	}

	return &(disp_dev_list[num]);
}

int hal_get_mode_check(int type, int def_check)
{
	if (DISP_OUTPUT_TYPE_HDMI == type) {
		disp_getprop_by_name(get_disp_fdt_node(), "hdmi_mode_check",
			(uint32_t *)&def_check, def_check);
	}
	return def_check;
}

int hal_switch_device(disp_device_t *device, unsigned int fb_id)
{
	int disp_para = 0;
	hal_fb_dev_t *fb_dev = NULL;

	if (DISP_OUTPUT_TYPE_HDMI == device->type) {
		if (0 == device->hpd_state) {
			printf("hdmi hpd out, force open?\n");
			/* Todo: force open hdmi device */
			return 0;
		} else {
			int vendor_id;
			device->mode = hdmi_verify_mode(
				device->screen_id, device->mode, &vendor_id,
				hal_get_mode_check(DISP_OUTPUT_TYPE_HDMI, 1));
		}
	}

	if (0 != _switch_device(device->screen_id, device->type, device->mode)) {
		printf("switch device failed: sel=%d, type=%d, mode=%d\n",
			device->screen_id, device->type, device->mode);
		return -1;
	}
	device->opened = 1;
	disp_para = (((device->type << 8) | device->mode) << (device->screen_id * 16));
	hal_save_int_to_kernel("boot_disp", disp_para);

	fb_dev = get_fb_dev(fb_id);
	if (NULL == fb_dev) {
		printf("this device can not be bounded to fb(%d)", fb_id);
		return -1;
	}

	if (FB_SHOW_LAYER & fb_dev->state) {
		_show_layer_on_dev(fb_dev->layer_config, device->screen_id, 1);
	}
	fb_dev->state |= DISP_DEV_OPENED;

	if (fb_dev->dev_num < sizeof(fb_dev->screen_id) / sizeof(fb_dev->screen_id[0])) {
		fb_dev->screen_id[fb_dev->dev_num] = device->screen_id;
		++(fb_dev->dev_num);
	} else {
		printf("ERR: %s the fb_dev->screen_id[] is overflowed\n", __func__);
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

int get_orangepi_env_value(char * buf, int num, char * name)
{
        char * p = buf;
        char * needle;

        while (num > 0) {

                while ((*p != '\n') && (*p != '\0')){
                        p++;
                }

                *p++ = '\0';

                needle = strstr(buf, name);
                if (needle != NULL) {
                        needle = strstr(buf, "=");
                        ++needle;

                        return (int)simple_strtoul(needle, NULL, 10);
                }

                num -= (p - buf);
                buf = p;
        }

        return -1;
}

int hal_get_fb_configs(fb_config_t *fb_cfgs, int *fb_num)
{
	int num = 0;
	int node = get_disp_fdt_node();
	char prop[32] = {0};

	//int read_bytes = 0;
	//char buf[512] = {0};
	//int value = -1;
	
	//read_bytes = hal_ext4_fsload(NULL, DISPLAY_RSL_FILENAME, buf, sizeof(buf));

	for (num = 0; num < *fb_num; ++num, ++fb_cfgs) {
		memset((void *)fb_cfgs, 0, sizeof(*fb_cfgs));
		sprintf(prop, "fb%d_format", num);
		disp_getprop_by_name(node, prop, (uint32_t *)&fb_cfgs->format_cfg, -1);
		_get_fb_format_config(fb_cfgs->format_cfg, &fb_cfgs->bpp);
		
		/*sprintf(prop, "fb%d_width", num);
		disp_getprop_by_name(node, prop, (uint32_t *)&fb_cfgs->width, 0);*/

		/*sprintf(prop, "fb%d_height", num);
		disp_getprop_by_name(node, prop, (uint32_t *)&fb_cfgs->height, 0);*/
		
		/*
		sprintf(prop, "fb%d_width", num);
		value = get_orangepi_env_value(buf, read_bytes, prop);
		if (value <= -1){
		        disp_getprop_by_name(node, prop, (uint32_t *)&(fb_cfgs->width), 0);
		}else{
		        fb_cfgs->width=value;
		        printf("Set %s to %d\n", prop, fb_cfgs->width);
		}
		
		sprintf(prop, "fb%d_height", num);
		value = get_orangepi_env_value(buf, read_bytes, prop);
		if (value <= -1){
		        disp_getprop_by_name(node, prop, (uint32_t *)&(fb_cfgs->height), 0);
		}else{
		        fb_cfgs->height=value;
		        printf("Set %s to %d\n", prop, fb_cfgs->height);
		}
		*/
	}
	return 0;
}

void *hal_request_layer(unsigned int fb_id)
{
	hal_fb_dev_t *fb_dev = get_fb_dev(fb_id);
	if (NULL == fb_dev) {
		printf("%s: get fb%d dev fail\n", __func__, fb_id);
		return NULL;
	}

	fb_dev->state &= ~(FB_REQ_LAYER | FB_SHOW_LAYER);
	fb_dev->layer_config = (void *)malloc(sizeof(private_data));
	if (NULL == fb_dev->layer_config) {
		printf("%s: malloc for private_data failed.\n", __func__);
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
		printf("%s: fb_id=%d, handle=%p, get_fb_dev=%p\n",
			__func__, fb_id, handle, get_fb_dev(fb_id));
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

int hal_set_layer_geometry(void *handle,
	int width, int height, int bpp, int byte_align)
{
	hal_fb_dev_t *fb_dev = (hal_fb_dev_t *)handle;

	_set_layer_geometry(fb_dev->layer_config, width, height, bpp, byte_align);

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
int hal_set_layer_crop(void *handle,
	int left, int top, int right, int bottom)
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
		_get_screen_size(fb_dev->screen_id[id], &scn_width, &scn_height);
		_get_layer_size(fb_dev->layer_config, &fb_width, &fb_height);
		if ((!scn_width || !scn_height || !fb_width || !fb_height)
			|| (left * scn_width % fb_width)
			|| (right * scn_width % fb_width)
			|| (top * scn_height % fb_height)
			|| (bottom * scn_height % fb_height)) {
			printf("not suport set layer crop[%d,%d,%d,%d], scn[%d,%d], fb[%d,%d]\n",
				left, top, right, bottom, scn_width, scn_height, fb_width, fb_height);
			return -1;
		}
		printf("%s: crop[%d,%d,%d,%d], scn[%d,%d], fb[%d,%d]\n", __func__,
			left, top, right, bottom, scn_width, scn_height, fb_width, fb_height);
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
		_show_layer_on_dev(fb_dev->layer_config, fb_dev->screen_id[i], is_show);

	if (0 != is_show) {
		fb_dev->state |= FB_SHOW_LAYER;
	} else {
		fb_dev->state &= ~FB_SHOW_LAYER;
	}
	return 0;
}

