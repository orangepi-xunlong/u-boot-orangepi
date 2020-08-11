#ifndef _HDMI_H_
#define _HDMI_H_
#include "tcon.h"
typedef enum
{
        DISP_OUTPUT_TYPE_NONE   = 0,
        DISP_OUTPUT_TYPE_LCD    = 1,
        DISP_OUTPUT_TYPE_TV     = 2,
        DISP_OUTPUT_TYPE_HDMI   = 4,
        DISP_OUTPUT_TYPE_VGA    = 8,
}disp_output_type;

enum disp_csc_type
{
        DISP_CSC_TYPE_RGB        = 0,
        DISP_CSC_TYPE_YUV444     = 1,
        DISP_CSC_TYPE_YUV422     = 2,
        DISP_CSC_TYPE_YUV420     = 3,
};

enum disp_data_bits {
	DISP_DATA_8BITS    = 0,
	DISP_DATA_10BITS   = 1,
	DISP_DATA_12BITS   = 2,
	DISP_DATA_16BITS   = 3,
};

enum disp_eotf {
	DISP_EOTF_RESERVED = 0x000,
	DISP_EOTF_BT709 = 0x001,
	DISP_EOTF_UNDEF = 0x002,
	DISP_EOTF_GAMMA22 = 0x004, /* SDR */
	DISP_EOTF_GAMMA28 = 0x005,
	DISP_EOTF_BT601 = 0x006,
	DISP_EOTF_SMPTE240M = 0x007,
	DISP_EOTF_LINEAR = 0x008,
	DISP_EOTF_LOG100 = 0x009,
	DISP_EOTF_LOG100S10 = 0x00a,
	DISP_EOTF_IEC61966_2_4 = 0x00b,
	DISP_EOTF_BT1361 = 0x00c,
	DISP_EOTF_IEC61966_2_1 = 0X00d,
	DISP_EOTF_BT2020_0 = 0x00e,
	DISP_EOTF_BT2020_1 = 0x00f,
	DISP_EOTF_SMPTE2084 = 0x010, /* HDR10 */
	DISP_EOTF_SMPTE428_1 = 0x011,
	DISP_EOTF_ARIB_STD_B67 = 0x012, /* HLG */
};

enum disp_color_space
{
	DISP_UNDEF = 0x00,
	DISP_UNDEF_F = 0x01,
	DISP_GBR = 0x100,
	DISP_BT709 = 0x101,
	DISP_FCC = 0x102,
	DISP_BT470BG = 0x103,
	DISP_BT601 = 0x104,
	DISP_SMPTE240M = 0x105,
	DISP_YCGCO = 0x106,
	DISP_BT2020NC = 0x107,
	DISP_BT2020C = 0x108,
	DISP_GBR_F = 0x200,
	DISP_BT709_F = 0x201,
	DISP_FCC_F = 0x202,
	DISP_BT470BG_F = 0x203,
	DISP_BT601_F = 0x204,
	DISP_SMPTE240M_F = 0x205,
	DISP_YCGCO_F = 0x206,
	DISP_BT2020NC_F = 0x207,
	DISP_BT2020C_F = 0x208,
	DISP_RESERVED = 0x300,
	DISP_RESERVED_F = 0x301,
};

enum disp_tv_mode
{
        DISP_TV_MOD_480I                = 0,
        DISP_TV_MOD_576I                = 1,
        DISP_TV_MOD_480P                = 2,
        DISP_TV_MOD_576P                = 3,
        DISP_TV_MOD_720P_50HZ           = 4,
        DISP_TV_MOD_720P_60HZ           = 5,
        DISP_TV_MOD_1080I_50HZ          = 6,
        DISP_TV_MOD_1080I_60HZ          = 7,
        DISP_TV_MOD_1080P_24HZ          = 8,
        DISP_TV_MOD_1080P_50HZ          = 9,
        DISP_TV_MOD_1080P_60HZ          = 0xa,
        DISP_TV_MOD_1080P_24HZ_3D_FP    = 0x17,
        DISP_TV_MOD_720P_50HZ_3D_FP     = 0x18,
        DISP_TV_MOD_720P_60HZ_3D_FP     = 0x19,
        DISP_TV_MOD_1080P_25HZ          = 0x1a,
        DISP_TV_MOD_1080P_30HZ          = 0x1b,
        DISP_TV_MOD_PAL                 = 0xb,
        DISP_TV_MOD_PAL_SVIDEO          = 0xc,
        DISP_TV_MOD_NTSC                = 0xe,
        DISP_TV_MOD_NTSC_SVIDEO         = 0xf,
        DISP_TV_MOD_PAL_M               = 0x11,
        DISP_TV_MOD_PAL_M_SVIDEO        = 0x12,
        DISP_TV_MOD_PAL_NC              = 0x14,
        DISP_TV_MOD_PAL_NC_SVIDEO       = 0x15,
        DISP_TV_MOD_3840_2160P_30HZ     = 0x1c,
        DISP_TV_MOD_3840_2160P_25HZ     = 0x1d,
        DISP_TV_MOD_3840_2160P_24HZ     = 0x1e,
        DISP_TV_MOD_4096_2160P_24HZ     = 0x1f,
        DISP_TV_MOD_4096_2160P_25HZ     = 0x20,
        DISP_TV_MOD_4096_2160P_30HZ     = 0x21,
        DISP_TV_MOD_3840_2160P_60HZ     = 0x22,
        DISP_TV_MOD_4096_2160P_60HZ     = 0x23,
        DISP_TV_MOD_3840_2160P_50HZ     = 0x24,
        DISP_TV_MOD_4096_2160P_50HZ     = 0x25,
        /*
         * vga
         * NOTE:macro'value of new solution must between
         * DISP_VGA_MOD_640_480P_60 and DISP_VGA_MOD_MAX_NUM
         * or you have to modify is_vag_mode function in drv_tv.h
         */
        DISP_VGA_MOD_640_480P_60         = 0x50,
        DISP_VGA_MOD_800_600P_60         = 0x51,
        DISP_VGA_MOD_1024_768P_60        = 0x52,
        DISP_VGA_MOD_1280_768P_60        = 0x53,
        DISP_VGA_MOD_1280_800P_60        = 0x54,
        DISP_VGA_MOD_1366_768P_60        = 0x55,
        DISP_VGA_MOD_1440_900P_60        = 0x56,
        DISP_VGA_MOD_1920_1080P_60       = 0x57,
        DISP_VGA_MOD_1280_720P_60        = 0x58,
        DISP_VGA_MOD_1920_1200P_60       = 0x5a,
        DISP_VGA_MOD_MAX_NUM             = 0x5b,
        DISP_TV_MODE_NUM                 = 0x5c,
};


struct disp_device_config {
    disp_output_type   type;
	enum disp_tv_mode       mode;
	enum disp_csc_type      format;
	enum disp_data_bits     bits;
	enum disp_eotf          eotf;
	enum disp_color_space   cs;
};

s32 hdmi_get_video_timming_info(struct disp_video_timings *video_info);
s32 hdmi_set_static_config(struct disp_device_config *config);
void hdmi_init (void);
int hdmi_configure(u16 phy_model);
u8 hdmi_hpd_status_get(void);

#endif
