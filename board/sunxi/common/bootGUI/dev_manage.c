#include <common.h>
#include <boot_gui.h>
#include "boot_gui_config.h"
#include "dev_manage.h"
#include "hdmi_manage.h"
#include "video_hal.h"
#include "video_misc_hal.h"


#define DISPLAY_PARTITION_NAME "Reserve0"
#define DISPLAY_RSL_FILENAME "disp_rsl.fex"

enum {
	LCD_NOT_SKIP_OPEN = 0,
	LCD_SKIP_OPEN,
};

static int get_display_resolution(const int type, char *buf, int num)
{
	int format = 0;
	char *p = buf;

	while (num > 0) {
		while ((*p != '\n') && (*p != '\0'))
			p++;
		*p++ = '\0';
		format = simple_strtoul(buf, NULL, 16);
		if (type == ((format >> 8) & 0xFF)) {
			printf("get format[%x] for type[%d]\n", format, type);
			return format & 0xff;
		}
		num -= (p - buf);
		buf = p;
	}
	return -1;
}

static int get_device_configs(disp_device_t *disp_dev_list, int *dev_num)
{
	int node = 0;
	char prop[32] = {'\n'};

	int read_bytes = 0;
	char buf[256] = {0};

	disp_device_t *disp_dev = NULL;
	int id = 0;

	node = get_disp_fdt_node();
	read_bytes = hal_fat_fsload(DISPLAY_PARTITION_NAME,
		DISPLAY_RSL_FILENAME, buf, sizeof(buf));

	for (id = 0; id < *dev_num; ++id) {

		uint32_t value = 0;
		sprintf(prop, "dev%d_output_type", id);
		disp_getprop_by_name(node, prop, &value, 0);
		if (0 == value)
			break;

		disp_dev = &(disp_dev_list[id]);
		memset((void *)disp_dev, 0, sizeof(*disp_dev));
		disp_dev->type = value;
		sprintf(prop, "dev%d_screen_id", id);
		disp_getprop_by_name(node, prop, (uint32_t *)&(disp_dev->screen_id), 0);
		sprintf(prop, "dev%d_do_hpd", id);
		disp_getprop_by_name(node, prop, (uint32_t *)&(disp_dev->do_hpd), 0);
		if (0 < read_bytes)
			disp_dev->mode = get_display_resolution(disp_dev->type, buf, read_bytes);
		if ((0 >= read_bytes) || (-1 == disp_dev->mode)) {
			sprintf(prop, "dev%d_output_mode", id);
			disp_getprop_by_name(node, prop, (uint32_t *)&(disp_dev->mode), 0);
		}
	}

	if (0 < id) {
		*dev_num = id;
	} else {
		/* cannot allow that dev_num is no larger than 0 */
		printf("no cfs of display devices.\n");
		memset((void *)disp_dev_list, 0, sizeof(*disp_dev_list));
		disp_dev_list->type = DISP_OUTPUT_TYPE_LCD;
		*dev_num = 1;
	}

	if (1 < *dev_num)
		disp_getprop_by_name(node, "def_output_dev", (uint32_t *)&id, 0);
	else if (1 == *dev_num)
		id = 0;
	return id; /* the def_output_dev for multi devices policy */
}

static disp_device_t *do_hpd_detect(disp_device_t *disp_dev, int dev_num)
{
	disp_device_t *devices[DISP_DEV_NUM];
	int hpd_num, i;
	unsigned int count;

	for (hpd_num = 0, i = 0; i < dev_num; ++i) {
		if (disp_dev[i].do_hpd) {
			devices[hpd_num] = &(disp_dev[i]);
			++hpd_num;
		}
	}

	if (0 < hpd_num) {

		disp_dev = devices[0];
		count = HPD_DETECT_COUNT0;
		while (count--) {
			disp_dev->hpd_state = hal_get_hpd_state(
				devices[0]->screen_id, disp_dev->type);
			if (disp_dev->hpd_state) {
				printf("main-hpd:count=%d,sel=%d,type=%d\n",
					HPD_DETECT_COUNT0 - count,
					disp_dev->screen_id, disp_dev->type);
				return disp_dev;
			}
		}

		count = HPD_DETECT_COUNT1;
		while (count--) {
			for (i = 0; i < hpd_num; ++i) {
				disp_dev = devices[i];
				disp_dev->hpd_state = hal_get_hpd_state(
					disp_dev->screen_id, disp_dev->type);
				if (disp_dev->hpd_state) {
					printf("ext-hpd:count=%d,sel=%d,type=%d\n",
						HPD_DETECT_COUNT1 - count,
						disp_dev->screen_id, disp_dev->type);
					return disp_dev;
				}
			}
		}
	}

	return NULL;
}

static unsigned char lcd_is_skip_open(int channel)
{
#ifdef LCD_CHECK_SKIP_OPEN
	/* this is for skiping lcd opening for carlet produce */
	char data_buf[8] = {0};
	int read_bytes = hal_fat_fsload("Reserve0", "ban_bl.fex",
		data_buf, sizeof(data_buf));
	return (read_bytes >= 0) ? LCD_SKIP_OPEN : LCD_NOT_SKIP_OPEN;
#else
	return LCD_NOT_SKIP_OPEN;
#endif
}

int disp_devices_open(void)
{
	disp_device_t devices[DISP_DEV_NUM];
	int actual_dev_num = DISP_DEV_NUM;
	int def_output_dev;
	int verify_mode;
	disp_device_t *output_dev = NULL;

	/* 1.get display devices list */
	memset((void *)devices, 0, sizeof(devices) / sizeof(devices[0]));
	def_output_dev = get_device_configs(devices, &actual_dev_num);

	/* 2.chose one as output by doing hpd */
	output_dev = do_hpd_detect(devices, actual_dev_num);
	if (NULL == output_dev)
		output_dev = &(devices[def_output_dev]);

	/* 3.do open one device */
	switch (output_dev->type) {
	case DISP_OUTPUT_TYPE_HDMI:
		if (0 == output_dev->hpd_state) {
			printf("hdmi hpd out, force open?\n");
			/* Todo: force open hdmi device */
		} else {
			int vendor_id;
			struct disp_device_config saved;
			if (!hal_get_disp_device_config(DISP_OUTPUT_TYPE_HDMI, &saved)) {
				output_dev->bits = saved.bits;
				output_dev->format = saved.format;
				output_dev->cs = saved.cs;
				output_dev->eotf = saved.eotf;
			} else {
				output_dev->format = (output_dev->type == DISP_OUTPUT_TYPE_LCD) ?
						DISP_CSC_TYPE_RGB : DISP_CSC_TYPE_YUV444;
				output_dev->bits = DISP_DATA_8BITS;
				output_dev->eotf = DISP_EOTF_GAMMA22;
				output_dev->cs = DISP_BT709;
			}

			verify_mode = hdmi_verify_mode(
				output_dev->screen_id, output_dev->mode, &vendor_id);
			if (verify_mode != output_dev->mode) {
				/* If the mode is change, need to reset the
				 * other configs (format/bits/cs).
				 * TODO: select format and bits according to edid*/
				output_dev->mode = verify_mode;
				if (verify_mode == DISP_TV_MOD_3840_2160P_50HZ
						|| verify_mode == DISP_TV_MOD_3840_2160P_60HZ) {
					output_dev->bits = DISP_DATA_8BITS;
					output_dev->format = DISP_CSC_TYPE_YUV420;
					output_dev->cs = DISP_BT709;
				} else {
					output_dev->bits = DISP_DATA_8BITS;
					output_dev->format = DISP_CSC_TYPE_YUV444;
					output_dev->cs = verify_mode > DISP_TV_MOD_720P_50HZ ?
						DISP_BT709 : DISP_BT601;
				}
			}
			hal_switch_device(output_dev, FB_ID_0); /* fixme */
		}
		break;
	case DISP_OUTPUT_TYPE_LCD:
		if (LCD_SKIP_OPEN == lcd_is_skip_open(output_dev->screen_id)) {
			printf("lcd skip open\n");
			hal_save_int_to_kernel("ban_bl", 1);
			return 0;
		}
	case DISP_OUTPUT_TYPE_TV:
	case DISP_OUTPUT_TYPE_VGA:
		hal_switch_device(output_dev, FB_ID_0); /* fixme */
		break;
	default:
		printf("open device(type=%d) fail!\n", output_dev->type);
		return -1;
	}

#ifdef UPDATE_DISPLAY_MODE
	int init_disp = 0;
	int i = 0;
	for (i = 0; i < actual_dev_num; ++i) {
		if (0 == devices[i].opened) {
			init_disp |= (((devices[i].type << 8) | devices[i].mode)
				<< (devices[i].screen_id * 16));
		}
	}
	if (0 != init_disp)
		hal_save_int_to_kernel("init_disp", init_disp);

	/* update display device config to kerner */
	if (hal_save_disp_device_config_to_kernel(0, 0))
		printf("save disp device failed\n");
#endif

	return 0;
}

#if defined(CONFIG_BOOT_GUI_TEST)
/* for test */
int disp_device_open_ex(int dev_id, int fb_id, int flag)
{
	disp_device_t devices[DISP_DEV_NUM];
	int actual_dev_num = DISP_DEV_NUM;
	int def_output_dev;
	disp_device_t *output_dev = NULL;

	/* 1.get display devices list */
	memset((void *)devices, 0, sizeof(devices) / sizeof(devices[0]));
	def_output_dev = get_device_configs(devices, &actual_dev_num);
	def_output_dev = def_output_dev;
	if (dev_id >= actual_dev_num) {
		printf("invalid para: dev_id=%d, actual_dev_num=%d\n",
			dev_id, actual_dev_num);
		return -1;
	}
	output_dev = &(devices[dev_id]);
	return hal_switch_device(output_dev, fb_id);
}
#endif
