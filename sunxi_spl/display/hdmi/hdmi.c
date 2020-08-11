#include "common.h"

#include "hdmi.h"
#include "phy.h"
#include "video.h"
#include "access.h"
#include "tcon.h"
#include "packets.h"
#include "irq.h"
#include "scdc.h"
#include "main_controller.h"
#include "fc_video.h"
#include "hdmitx_dev.h"
#include "audio.h"


#define HDMI_CTRL_REG_BASE 0x06000000


enum HDMI_VIC {
	HDMI_VIC_640x480P60 = 1,
	HDMI_VIC_720x480P60_4_3,
	HDMI_VIC_720x480P60_16_9,
	HDMI_VIC_1280x720P60,
	HDMI_VIC_1920x1080I60,
	HDMI_VIC_720x480I_4_3,
	HDMI_VIC_720x480I_16_9,
	HDMI_VIC_720x240P_4_3,
	HDMI_VIC_720x240P_16_9,
	HDMI_VIC_1920x1080P60 = 16,
	HDMI_VIC_720x576P_4_3,
	HDMI_VIC_720x576P_16_9,
	HDMI_VIC_1280x720P50,
	HDMI_VIC_1920x1080I50,
	HDMI_VIC_720x576I_4_3,
	HDMI_VIC_720x576I_16_9,
	HDMI_VIC_1920x1080P50 = 31,
	HDMI_VIC_1920x1080P24,
	HDMI_VIC_1920x1080P25,
	HDMI_VIC_1920x1080P30,
	HDMI_VIC_1280x720P24 = 60,
	HDMI_VIC_1280x720P25,
	HDMI_VIC_1280x720P30,
	HDMI_VIC_3840x2160P24 = 93,
	HDMI_VIC_3840x2160P25,
	HDMI_VIC_3840x2160P30,
	HDMI_VIC_3840x2160P50,
	HDMI_VIC_3840x2160P60,
	HDMI_VIC_4096x2160P24,
	HDMI_VIC_4096x2160P25,
	HDMI_VIC_4096x2160P30,
	HDMI_VIC_4096x2160P50,
	HDMI_VIC_4096x2160P60,
};

typedef enum {
	SDR_LUMINANCE_RANGE = 0,
	HDR_LUMINANCE_RANGE,
	SMPTE_ST_2084,
	HLG
} eotf_t;

struct disp_hdmi_mode {
	enum disp_tv_mode mode;
	int hdmi_mode;/* vic */
};

static struct disp_hdmi_mode hdmi_mode_tbl[] = {
	{DISP_TV_MOD_480I,                HDMI_VIC_720x480I_16_9,     },
	{DISP_TV_MOD_576I,                HDMI_VIC_720x576I_16_9,     },
	{DISP_TV_MOD_480P,                HDMI_VIC_720x480P60_16_9,   },
	{DISP_TV_MOD_576P,                HDMI_VIC_720x576P_16_9,     },
	{DISP_TV_MOD_720P_50HZ,           HDMI_VIC_1280x720P50,       },
	{DISP_TV_MOD_720P_60HZ,           HDMI_VIC_1280x720P60,       },
	{DISP_TV_MOD_1080I_50HZ,          HDMI_VIC_1920x1080I50,      },
	{DISP_TV_MOD_1080I_60HZ,          HDMI_VIC_1920x1080I60,      },
	{DISP_TV_MOD_1080P_24HZ,          HDMI_VIC_1920x1080P24,      },
	{DISP_TV_MOD_1080P_50HZ,          HDMI_VIC_1920x1080P50,      },
	{DISP_TV_MOD_1080P_60HZ,          HDMI_VIC_1920x1080P60,      },
	{DISP_TV_MOD_1080P_25HZ,          HDMI_VIC_1920x1080P25,      },
	{DISP_TV_MOD_1080P_30HZ,          HDMI_VIC_1920x1080P30,      },
	//{DISP_TV_MOD_1080P_24HZ_3D_FP,    HDMI1080P_24_3D_FP,},
	//{DISP_TV_MOD_720P_50HZ_3D_FP,     HDMI720P_50_3D_FP, },
	//{DISP_TV_MOD_720P_60HZ_3D_FP,     HDMI720P_60_3D_FP, },
	{DISP_TV_MOD_3840_2160P_30HZ,     HDMI_VIC_3840x2160P30, },
	{DISP_TV_MOD_3840_2160P_25HZ,     HDMI_VIC_3840x2160P25, },
	{DISP_TV_MOD_3840_2160P_24HZ,     HDMI_VIC_3840x2160P24, },
	{DISP_TV_MOD_4096_2160P_24HZ,     HDMI_VIC_4096x2160P24, },
	{DISP_TV_MOD_4096_2160P_25HZ,	  HDMI_VIC_4096x2160P25, },
	{DISP_TV_MOD_4096_2160P_30HZ,	  HDMI_VIC_4096x2160P30, },

	{DISP_TV_MOD_3840_2160P_50HZ,     HDMI_VIC_3840x2160P50,},
	{DISP_TV_MOD_4096_2160P_50HZ,     HDMI_VIC_4096x2160P50,},

	{DISP_TV_MOD_3840_2160P_60HZ,     HDMI_VIC_3840x2160P60, },
	{DISP_TV_MOD_4096_2160P_60HZ,     HDMI_VIC_4096x2160P60, },
};





typedef struct hdmi_ctrl {
    videoParams_t        		video_params;
	audioParams_t				audio_params;
	productParams_t				product_params;
	hdmi_tx_dev_t  	   			hdmi_tx_dev;
	struct device_access		dev_access;
	struct system_functions 	sys_functions;
} hdmi_ctrl_t;

static hdmi_ctrl_t sHdmiCtr;


/*
static int videoParams_GetHdmiVicCode(int cea_code)
{
	switch (cea_code) {
	case 95:
		return 1;
		break;
	case 94:
		return 2;
		break;
	case 93:
		return 3;
		break;
	case 98:
		return 4;
		break;
	default:
		return -1;
		break;
	}
	return -1;
}
*/
/*
*@code: svd <1-107, 0x80+4, 0x80+19, 0x80+32>
*@refresh_rate: [optional] Hz*1000,which is 1000 times than 1 Hz
*/
static u32 svd_user_config(u32 code, u32 refresh_rate)
{
	dtd_t *p_dtd;
	//dtd_t dtd;
	int hdmi_vic = 0;

	/*memset(&core->mode.pVideo, 0, sizeof(videoParams_t));*/

	if (code < 1 || code > 256) {
		printf("ERROR:VIC Code is out of range\n");
		return -1;
	}


	p_dtd = get_dtd(code,refresh_rate);
	if (p_dtd == NULL) {
		HDMI_INFO_MSG("Can not find detailed timing\n");
		return -1;

	}

	p_dtd->mLimitedToYcc420 = false;
	p_dtd->mYcc420 = false;

	memcpy(&sHdmiCtr.video_params.mDtd, p_dtd, sizeof(dtd_t));

	hdmi_vic = videoParams_GetHdmiVicCode(sHdmiCtr.video_params.mDtd.mCode);
	if (hdmi_vic > 0) {
		sHdmiCtr.product_params.mOUI = 0x000c03;
		sHdmiCtr.product_params.mVendorPayload[0] = 0x20;
		sHdmiCtr.product_params.mVendorPayload[1] = hdmi_vic;
		sHdmiCtr.product_params.mVendorPayloadLength = 2;

		sHdmiCtr.video_params.mDtd.mCode = 0;
		sHdmiCtr.video_params.mCea_code = 0;
		sHdmiCtr.video_params.mHdmi_code = hdmi_vic;
	} else {
		sHdmiCtr.product_params.mOUI = 0x000c03;
		sHdmiCtr.product_params.mVendorPayload[0] = 0x0;
		sHdmiCtr.product_params.mVendorPayload[1] = 0;
		sHdmiCtr.product_params.mVendorPayloadLength = 2;

		sHdmiCtr.video_params.mCea_code = sHdmiCtr.video_params.mDtd.mCode;
		sHdmiCtr.video_params.mHdmi_code = 0;
	}

    if (sHdmiCtr.video_params.mHdmi_code) {
		sHdmiCtr.video_params.mHdmiVideoFormat = 0x01;
		sHdmiCtr.video_params.m3dStructure = 0;
	} else {
		sHdmiCtr.video_params.mHdmiVideoFormat = 0x0;
		sHdmiCtr.video_params.m3dStructure = 0;
	}

	return 0;
}


static s32 hdmi_set_display_mode(u32 mode)
{
	u32 hdmi_mode;
	u32 i;
	bool find = false;

	for (i = 0; i < sizeof(hdmi_mode_tbl)/sizeof(struct disp_hdmi_mode); i++) {
		if (hdmi_mode_tbl[i].mode == (enum disp_tv_mode)mode) {
			hdmi_mode = hdmi_mode_tbl[i].hdmi_mode;
			find = true;
			break;
		}
	}

	if (find) {
		/*user configure detailed timing according to vic*/
		if (svd_user_config(hdmi_mode, 0) != 0) {
			HDMI_ERROR_MSG("svd user config failed!\n");
			return -1;
		} else {
			HDMI_INFO_MSG("Set hdmi mode: %d\n", hdmi_mode);
			return 0;
		}
	} else {
		HDMI_ERROR_MSG("unsupported video mode %d when set display mode\n", mode);
		return -1;
	}

}

s32 hdmi_set_static_config(struct disp_device_config *config)
{
	u32 data_bit = 0;

	videoParams_t *pVideo = &sHdmiCtr.video_params;

	//pVideo->update = hdmi_check_updated(core, config);

	memset(pVideo, 0, sizeof(videoParams_t));
	/*set vic mode and dtd*/
	hdmi_set_display_mode(config->mode);

	/*set encoding mode*/
	pVideo->mEncodingIn = config->format;
	pVideo->mEncodingOut = config->format;

	/*set data bits*/
	if ((config->bits >= 0) && (config->bits < 3))
		data_bit = 8 + 2 * config->bits;
	if (config->bits == 3)
		data_bit = 16;
	pVideo->mColorResolution = (u8)data_bit;

	/*set eotf*/
	if (config->eotf) {

		//if (pVideo->pb) {
			pVideo->pb.r_x = 0x33c2;
			pVideo->pb.r_y = 0x86c4;
			pVideo->pb.g_x = 0x1d4c;
			pVideo->pb.g_y = 0x0bb8;
			pVideo->pb.b_x = 0x84d0;
			pVideo->pb.b_y = 0x3e80;
			pVideo->pb.w_x = 0x3d13;
			pVideo->pb.w_y = 0x4042;
			pVideo->pb.luma_max = 0x03e8;
			pVideo->pb.luma_min = 0x1;
			pVideo->pb.mcll = 0x03e8;
			pVideo->pb.mfll = 0x0190;
		//}


		switch (config->eotf) {
		case DISP_EOTF_GAMMA22:
			pVideo->mHdr = 0;
			pVideo->pb.eotf = SDR_LUMINANCE_RANGE;
			break;
		case DISP_EOTF_SMPTE2084:
			pVideo->mHdr = 1;
			pVideo->pb.eotf = SMPTE_ST_2084;
			break;
		case DISP_EOTF_ARIB_STD_B67:
			pVideo->mHdr = 1;
			pVideo->pb.eotf = HLG;
			break;
		default:
			break;
		}

	} else {
		if (config->mode < 4)
			pVideo->mColorimetry = ITU601;
		else
			pVideo->mColorimetry = ITU709;

	}

	/*set color space*/
	switch (config->cs) {
	case DISP_UNDEF:
		pVideo->mColorimetry = 0;
		break;
	case DISP_BT601:
		pVideo->mColorimetry = ITU601;
		break;
	case DISP_BT709:
		pVideo->mColorimetry = ITU709;
		break;
	case DISP_BT2020NC:
		pVideo->mColorimetry = EXTENDED_COLORIMETRY;
		pVideo->mExtColorimetry = BT2020_Y_CB_CR;
		break;
	default:
		pVideo->mColorimetry = 0;
		break;
	}


	pVideo->mHdmi = HDMI;

	//memcpy(&core->config, config, sizeof(struct disp_device_config));
	return 0;
}


s32 hdmi_get_video_timming_info(struct disp_video_timings *video_info)
{
	dtd_t *dtd = NULL;


	dtd = &sHdmiCtr.video_params.mDtd;
	video_info->vic = dtd->mCode;
	video_info->tv_mode = 0;

	video_info->pixel_clk = (dtd->mPixelClock) * 1000 / (dtd->mPixelRepetitionInput+1);
	if ((video_info->vic == 6) || (video_info->vic == 7) || (video_info->vic == 21) || (video_info->vic == 22))
		video_info->pixel_clk = (dtd->mPixelClock) * 1000 / (dtd->mPixelRepetitionInput + 1) / (dtd->mInterlaced + 1);
	video_info->pixel_repeat = dtd->mPixelRepetitionInput;
	video_info->x_res = (dtd->mHActive) / (dtd->mPixelRepetitionInput+1);
	if (dtd->mInterlaced == 1)
		video_info->y_res = (dtd->mVActive) * 2;
	else if (dtd->mInterlaced == 0)
		video_info->y_res = dtd->mVActive;

	video_info->hor_total_time = (dtd->mHActive + dtd->mHBlanking) / (dtd->mPixelRepetitionInput+1);
	video_info->hor_back_porch = (dtd->mHBlanking - dtd->mHSyncOffset - dtd->mHSyncPulseWidth) / (dtd->mPixelRepetitionInput+1);
	video_info->hor_front_porch = (dtd->mHSyncOffset) / (dtd->mPixelRepetitionInput+1);
	video_info->hor_sync_time = (dtd->mHSyncPulseWidth) / (dtd->mPixelRepetitionInput+1);

	if (dtd->mInterlaced == 1)
		video_info->ver_total_time = (dtd->mVActive + dtd->mVBlanking) * 2 + 1;
	else if (dtd->mInterlaced == 0)
		video_info->ver_total_time = dtd->mVActive + dtd->mVBlanking;
	video_info->ver_back_porch = dtd->mVBlanking - dtd->mVSyncOffset - dtd->mVSyncPulseWidth;
	video_info->ver_front_porch = dtd->mVSyncOffset;
	video_info->ver_sync_time = dtd->mVSyncPulseWidth;

	video_info->hor_sync_polarity = dtd->mHSyncPolarity;
	video_info->ver_sync_polarity = dtd->mVSyncPolarity;
	video_info->b_interlace = dtd->mInterlaced;
	video_info->vactive_space = 0;
	video_info->trd_mode = 0;

	return 0;
}

#define A_HDCPCFG0  0x00014000
#define A_HDCPCFG0_AVMUTE_MASK  0x00000008 /* This register holds the current AVMUTE state of the DWC_hdmi_tx controller, as expected to be perceived by the connected HDMI/HDCP sink device */

static void _EnableAvmute(hdmi_tx_dev_t *dev, u8 bit)
{
	//LOG_TRACE1(bit);
	dev_write_mask(dev, A_HDCPCFG0, A_HDCPCFG0_AVMUTE_MASK, bit);
}


static void hdcp_av_mute(hdmi_tx_dev_t *dev, int enable)
{
	//LOG_TRACE1(enable);
	_EnableAvmute(dev,
			(enable == true) ? 1 : 0);
}

static void api_avmute(hdmi_tx_dev_t *dev, int enable)
{
	packets_AvMute(dev, enable);
	hdcp_av_mute(dev, enable);
}

int hdmi_configure(u16 phy_model)
{
	int success = true;
	unsigned int tmds_clk = 0;
	videoParams_t *video = &sHdmiCtr.video_params;
	//audioParams_t *audio = &sHdmiCtr.audio_params;
	productParams_t *product = &sHdmiCtr.product_params;

	sHdmiCtr.hdmi_tx_dev.snps_hdmi_ctrl.hdmi_on = (video->mHdmi == HDMI) ? 1 : 0;
	//hdmiTxDev.audio_on = (video->mHdmi == HDMI) ? 1 : 0;
	sHdmiCtr.hdmi_tx_dev.snps_hdmi_ctrl.pixel_clock = videoParams_GetPixelClock(&sHdmiCtr.hdmi_tx_dev, video);
	sHdmiCtr.hdmi_tx_dev.snps_hdmi_ctrl.color_resolution = video->mColorResolution;
	sHdmiCtr.hdmi_tx_dev.snps_hdmi_ctrl.pixel_repetition = video->mDtd.mPixelRepetitionInput;

	/*be used to calculate tmds clk*/
	HDMI_INFO_MSG("video pixel clock=%d\n", sHdmiCtr.hdmi_tx_dev.snps_hdmi_ctrl.pixel_clock);

	/*for auto scrambling if tmds_clk > 3.4Gbps*/
	switch (video->mColorResolution) {
	case COLOR_DEPTH_8:
		tmds_clk = sHdmiCtr.hdmi_tx_dev.snps_hdmi_ctrl.pixel_clock;
		break;
	case COLOR_DEPTH_10:
		if (video->mEncodingOut != YCC422)
			tmds_clk = sHdmiCtr.hdmi_tx_dev.snps_hdmi_ctrl.pixel_clock * 125 / 100;
		else
			tmds_clk = sHdmiCtr.hdmi_tx_dev.snps_hdmi_ctrl.pixel_clock;

		break;
	default:
		break;
	}
	sHdmiCtr.hdmi_tx_dev.snps_hdmi_ctrl.tmds_clk = tmds_clk;

	if (video->mEncodingIn == YCC420) {
		sHdmiCtr.hdmi_tx_dev.snps_hdmi_ctrl.pixel_clock = sHdmiCtr.hdmi_tx_dev.snps_hdmi_ctrl.pixel_clock / 2;
		sHdmiCtr.hdmi_tx_dev.snps_hdmi_ctrl.tmds_clk /= 2;
	}
	if (video->mEncodingIn == YCC422)
		sHdmiCtr.hdmi_tx_dev.snps_hdmi_ctrl.color_resolution = 8;

	api_avmute(&sHdmiCtr.hdmi_tx_dev, true);

	phy_standby(&sHdmiCtr.hdmi_tx_dev);

	/* Disable interrupts */
	irq_mute(&sHdmiCtr.hdmi_tx_dev);

	success = video_Configure(&sHdmiCtr.hdmi_tx_dev, video);
	if (success == false)
		HDMI_INFO_MSG("Could not configure video\n");

#if 0
	/* Audio - Workaround */
	audio_Initialize(&sHdmiCtr.hdmi_tx_dev);
	success = audio_Configure(&sHdmiCtr.hdmi_tx_dev, audio);
	if (success == false)
		HDMI_INFO_MSG("ERROR:Audio not configured\n");
#endif

	/* Packets */
	success = packets_Configure(&sHdmiCtr.hdmi_tx_dev, video, product);
	if (success == false)
		HDMI_INFO_MSG("ERROR:Could not configure packets\n");

	mc_enable_all_clocks(&sHdmiCtr.hdmi_tx_dev);
	snps_sleep(10000);

	if (sHdmiCtr.hdmi_tx_dev.snps_hdmi_ctrl.tmds_clk  > 340000) {
		scrambling(&sHdmiCtr.hdmi_tx_dev, 1);
		HDMI_INFO_MSG("enable scrambling\n");
	}


	/*add calibrated resistor configuration for all video resolution*/
	dev_write(&sHdmiCtr.hdmi_tx_dev, 0x40018, 0xc0);
	dev_write(&sHdmiCtr.hdmi_tx_dev, 0x4001c, 0x80);

	success = phy_configure(&sHdmiCtr.hdmi_tx_dev, 301);
	if (success == false)
		HDMI_INFO_MSG("ERROR:Could not configure PHY\n");

	/* Disable blue screen transmission
		after turning on all necessary blocks (e.g. HDCP) */
	fc_force_output(&sHdmiCtr.hdmi_tx_dev, false);


	irq_mask_all(&sHdmiCtr.hdmi_tx_dev);
	/* enable interrupts */
	irq_unmute(&sHdmiCtr.hdmi_tx_dev);

	snps_sleep(100000);
	api_avmute(&sHdmiCtr.hdmi_tx_dev, false);

	return success;
}

static void hdmitx_write(u32 addr, u32 data)
{
	//asm volatile("dsb st");
	*((volatile u8 *)(HDMI_CTRL_REG_BASE + (addr >> 2))) = data;
}

static u32 hdmitx_read(u32 addr)
{
	return *((volatile u8 *)(HDMI_CTRL_REG_BASE + (addr >> 2)));
}

static void hdmitx_sleep(int us)
{
	//udelay(us);
}

u8 hdmi_hpd_status_get(void)
{
	return phy_hot_plug_state(&sHdmiCtr.hdmi_tx_dev);

}

void hdmi_init (void)
{
	memset(&sHdmiCtr, 0, sizeof(hdmi_ctrl_t));
	sHdmiCtr.hdmi_tx_dev.snps_hdmi_ctrl.csc_on = 1;
	sHdmiCtr.hdmi_tx_dev.snps_hdmi_ctrl.audio_on = 1;
	sHdmiCtr.hdmi_tx_dev.snps_hdmi_ctrl.data_enable_polarity = 1;

	sHdmiCtr.dev_access.read = hdmitx_read;
	sHdmiCtr.dev_access.write = hdmitx_write;

	sHdmiCtr.sys_functions.sleep = hdmitx_sleep;
/*
	audioParams_t *audio = &sHdmiCtr.audio_params;

	memset(audio, 0, sizeof(audioParams_t));
	audio->mInterfaceType = I2S;
	audio->mCodingType = PCM;
	audio->mSamplingFrequency = 44100;
	audio->mChannelAllocation = 0;
	audio->mChannelNum = 2;
	audio->mSampleSize = 16;
	audio->mClockFsFactor = 64;
	audio->mPacketType = PACKET_NOT_DEFINED;
	audio->mDmaBeatIncrement = DMA_NOT_DEFINED;
*/
	register_system_functions(&sHdmiCtr.sys_functions);
	register_bsp_functions(&sHdmiCtr.dev_access);
	irq_hpd_sense_enable(&sHdmiCtr.hdmi_tx_dev, 1);
}
