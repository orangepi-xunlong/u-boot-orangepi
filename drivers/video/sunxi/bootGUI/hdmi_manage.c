/*
 * drivers/video/sunxi/bootGUI/hdmi_manage.c
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
#include <common.h>
#include "dev_manage.h"
#include "hdmi_manage.h"
#include "video_hal.h"
#include "video_misc_hal.h"

#define EDID_LENGTH 0x80
#define ID_VENDOR 0x08
#define VENDOR_INFO_SIZE 10

#define DISPLAY_PARTITION_NAME "Reserve0"

enum {
	ALWAYS_NOT_CHECK_MODE = 0,
	ALWAYS_CHECK_MODE,
	CHECK_MODE_ONLY_IF_SAME_TV,
};

#if 0
static int get_mode_check_policy(int def_check)
{
	disp_getprop_by_name(get_disp_fdt_node(), "hdmi_mode_check",
		(uint32_t *)&def_check, def_check);
	return def_check;
}

/*
* the format of saved vendor id is string of hex:
* "data[0],data[1],...,data[9],".
*/
static int get_saved_vendor_id(char *vendor_id, int num)
{
	char data_buf[64] = {0};
	char *p = data_buf;
	char *pdata = data_buf;
	char *p_end;
	int len = 0;
	int i;

	len = hal_fat_fsload(DISPLAY_PARTITION_NAME,
		"tv_vdid.fex", data_buf, sizeof(data_buf));
	if (0 >= len)
		return 0;

	i = 0;
	p_end = p + len;
	for (; p < p_end; ++p) {
		if (',' == *p) {
			*p = '\0';
			vendor_id[i] = (char)simple_strtoul(pdata, NULL, 16);
			++i;
			if (i >= num)
				break;
			pdata = p + 1;
		}
	}
	return i;
}

static int is_same_vendor(unsigned char *edid_buf,
	char *vendor, int num)
{
	int i;
	char *pdata = (char *)edid_buf + ID_VENDOR;
	for (i = 0; i < num; ++i) {
		if (pdata[i] != vendor[i]) {
			printf("different vendor[current <-> saved]\n");
			for (i = 0; i < num; ++i) {
				printf("[%x <-> %x]\n", pdata[i], vendor[i]);
			}
			return 0;
		}
	}
	printf("same vendor\n");
	return !0;
}

static int edid_checksum(char const *edid_buf)
{
	char csum = 0;
	char all_null = 0;
	int i = 0;

	for (i = 0; i < EDID_LENGTH; i++) {
		csum += edid_buf[i];
		all_null |= edid_buf[i];
	}

	if (csum == 0x00 && all_null) {
	/* checksum passed, everything's good */
		return 0;
	} else if (all_null) {
		printf("edid all null\n");
		return -2;
	} else {
		printf("edid checksum err\n");
		return -1;
	}
}
#endif

int hdmi_verify_mode(int channel, int mode, int *vid)
{
	/* check if support the output_mode by television,
	 * return 0 is not support */
	if (1 != hal_is_support_mode(channel, DISP_OUTPUT_TYPE_HDMI, mode)) {
		int i = 0;
		/* self-define hdmi mode list */
		const int HDMI_MODES[] = {
		    DISP_TV_MOD_3840_2160P_30HZ,
		    DISP_TV_MOD_1080P_60HZ,
		    DISP_TV_MOD_1080I_60HZ,
		    DISP_TV_MOD_1080P_50HZ,
		    DISP_TV_MOD_1080I_50HZ,
		    DISP_TV_MOD_720P_60HZ,
		    DISP_TV_MOD_720P_50HZ,
		    DISP_TV_MOD_576P,
		    DISP_TV_MOD_480P,
		};
		for (i = 0; i < sizeof(HDMI_MODES) / sizeof(HDMI_MODES[0]);
		     i++) {
			if (1 == hal_is_support_mode(channel,
						     DISP_OUTPUT_TYPE_HDMI,
						     HDMI_MODES[i])) {
				printf("find mode[%d] in HDMI_MODES\n",
				       HDMI_MODES[i]);
				mode = HDMI_MODES[i];
				break;
			}
		}
	}

	hal_save_int_to_kernel("tv_vdid", *vid);

	return mode;
}
