
#include <common.h>
#include <private_boot0.h>
#include <private_uboot.h>

#include "tcon.h"
#include "clk.h"
#include "hdmi.h"
#include "phy.h"

#define TCON_TV_REG_BASE  0x06515000
#define TCON_TOP_REG_BASE 0x06510000
#define SCREEN_ID 1
#define DE_NUM    0
static enum disp_csc_type sPixelFormat;


int disp_al_hdmi_cfg(u32 screen_id, struct disp_video_timings *video_info)
{

	if (sPixelFormat == DISP_CSC_TYPE_YUV420) {
		video_info->x_res /= 2;
		video_info->hor_total_time /= 2;
		video_info->hor_back_porch /= 2;
		video_info->hor_front_porch /= 2;
		video_info->hor_sync_time /= 2;
	}


	tcon_init(screen_id);
	tcon1_set_timming(screen_id, video_info);

	tcon1_src_select(screen_id, LCD_SRC_DE, DE_NUM);
	tcon1_black_src(screen_id, 1, sPixelFormat);

	return 0;
}

/* hdmi */
int disp_al_hdmi_enable(u32 screen_id)
{
	tcon1_hdmi_clk_enable(screen_id, 1);

	tcon1_open(screen_id);
	return 0;
}

#define TOLOWER(a) ((a<='Z') ? a+('a'-'A') : a )
static bool isxdigit(char c)
{
	if ((c>='0' && c<='9') || (c>='A' && c<='F') || (c>='a' && c<='f'))
		return true;
	return false;
}
static bool isdigit(char c)
{
	if (c>='0' && c<='9')
		return true;
	return false;
}

static unsigned int simple_guess_base(const char *cp)
{
    if (cp[0] == '0') {
        if (TOLOWER(cp[1]) == 'x' && isxdigit(cp[2]))
            return 16;
        else
            return 8;
    } else {
        return 10;
    }
}

unsigned long simple_strtoul(const char *cp, char **endp, unsigned int base)
{
    unsigned long result = 0;

    if (!base)
        base = simple_guess_base(cp);

    if (base == 16 && cp[0] == '0' && TOLOWER(cp[1]) == 'x')
        cp += 2;

    while (isxdigit(*cp)) {
        unsigned int value;

        value = isdigit(*cp) ? *cp - '0' : TOLOWER(*cp) - 'a' + 10;
        if (value >= base)
            break;
        result = result * base + value;
        cp++;
    }

    if (endp)
        *endp = (char *)cp;
    return result;
}

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

void disp_init (void)
{
	char buf[64];
	u32 rate;
	u32 len;
	struct disp_device_config config;
	struct disp_video_timings video_info;
	struct spare_parameter_head_t *param_head = NULL;

	hdmi_clk_enable();
	hdmi_ddc_clk_enable();
	hdmi_ddc_gpio_enable();
	de_clk_enable ();
	tcon_top_clk_enable();
	tcon_set_reg_base(1, TCON_TV_REG_BASE);
	tcon_top_set_reg_base(0, TCON_TOP_REG_BASE);

	hdmi_init();

	param_head = (struct spare_parameter_head_t *)CONFIG_SUNXI_PARAMETER_ADDR;

	len = strlen(param_head->para_data.display_param.resolution);
	strncpy(buf, param_head->para_data.display_param.resolution, 64);
	config.type = DISP_OUTPUT_TYPE_HDMI;
	config.mode = get_display_resolution(DISP_OUTPUT_TYPE_HDMI, buf, len);
	config.format = param_head->para_data.display_param.format;
	config.bits = param_head->para_data.display_param.depth;
	config.eotf = param_head->para_data.display_param.eotf;
	config.cs = param_head->para_data.display_param.color_space;

	if (config.mode == -1)
	{
		config.mode = DISP_TV_MOD_1080P_60HZ;
		config.bits = DISP_DATA_8BITS;
		config.eotf= DISP_EOTF_UNDEF;
		config.cs = DISP_BT709;
		config.format = DISP_CSC_TYPE_YUV444;

	}

	sPixelFormat = config.format;
	/*
	printf("*********** parameter data************\n");
	printf("display resolution : %s\n", param_head->para_data.display_param.resolution);
	printf("display margin     : %s\n", param_head->para_data.display_param.margin);
        printf("display vendorid   : %s\n", param_head->para_data.display_param.vendorid);
	printf("display mode       : %d\n",config.mode);
	printf("display format     : %d\n", param_head->para_data.display_param.format);
	printf("display depth      : %d\n", param_head->para_data.display_param.depth);
	printf("display eotf       : %d\n", param_head->para_data.display_param.eotf);
	printf("display color_space: %d\n", param_head->para_data.display_param.color_space);
      */
	hdmi_set_static_config(&config);
	hdmi_get_video_timming_info(&video_info);

	rate = video_info.pixel_clk * (video_info.pixel_repeat + 1);
	if (sPixelFormat == DISP_CSC_TYPE_YUV420)
		rate /= 2;
	tcon_tv_clk_config(rate);

	disp_al_hdmi_cfg(SCREEN_ID, &video_info);
	disp_al_hdmi_enable(SCREEN_ID);
	hdmi_configure(PHY_MODEL_301);

	printf("disp_init end\n");
}
