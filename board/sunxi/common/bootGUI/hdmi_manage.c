#include "dev_manage.h"
#include "hdmi_manage.h"

#define EDID_LENGTH 0x80
#define ID_VENDOR 0x08
#define VENDOR_INFO_SIZE 10

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

int hdmi_verify_mode(int channel, int mode, int *vid, int check)
{
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
	int i = 0;
	unsigned char edid_buf[256] = {0};
	char saved_vendor_id[VENDOR_INFO_SIZE] = {0};
	int vendor_size = 0;

	hal_get_hdmi_edid(channel, edid_buf, 128); /* here we only get edid block0 */

	if (-2 == edid_checksum((char const *)edid_buf))
		check = 0;

	vendor_size = get_saved_vendor_id(saved_vendor_id, VENDOR_INFO_SIZE);
	if (2 == check) {
		/* if vendor id change , check mode: check = 1 */
		if ((0 == vendor_size)
			|| !is_same_vendor(edid_buf, saved_vendor_id, vendor_size)) {
			check = 1;
		}
	}

	/* check if support the output_mode by television,
	 * return 0 is not support */
	if ((1 == check)
		&& (1 != hal_is_support_mode(channel,
			DISP_OUTPUT_TYPE_HDMI, mode))) {
		for (i = 0; i < sizeof(HDMI_MODES) / sizeof(HDMI_MODES[0]); i++) {
			if (1 == hal_is_support_mode(channel,
				DISP_OUTPUT_TYPE_HDMI, HDMI_MODES[i])) {
				printf("find mode[%d] in HDMI_MODES\n", HDMI_MODES[i]);
				mode = HDMI_MODES[i];
				break;
			}
		}
	}

	hal_save_int_to_kernel("tv_vdid", *vid);

	return mode;
}


