/*
 * Allwinner SoCs hdmi2.0 driver.
 *
 * Copyright (C) 2016 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */


#include "core_edid.h"
#include "hdmi_core.h"

#if defined(__LINUX_PLAT__)

static void edid_init_sink_cap(sink_edid_t *sink)
{
	memset(sink, 0, sizeof(sink_edid_t));
}

void edid_read_cap(void)
{
	struct hdmi_tx_core *core = get_platform();
	struct edid  *edid = core->mode.edid;
	u8  *edid_ext = core->mode.edid_ext;
	sink_edid_t *sink = core->mode.sink_cap;
	int edid_ok = 0;
	int edid_tries = 3;

	LOG_TRACE();
	if ((!core) || (!edid) || (!edid_ext) || (!sink)) {
		HDMI_INFO_MSG("Could not get core structure\n");
		return;
	}

	memset(edid, 0, sizeof(struct edid));
	memset(edid_ext, 0, 128);
	memset(sink, 0, sizeof(sink_edid_t));
	core->blacklist_sink = -1;

	edid_init_sink_cap(sink);
	core->dev_func.edid_parser_cea_ext_reset(sink);

	do {
		edid_ok = core->dev_func.edid_read(edid);

		if (edid_ok) /* error case */
			continue;

		core->mode.edid = edid;

		if (core->dev_func.edid_parser((u8 *) edid, sink, 128) == false) {
			HDMI_INFO_MSG("Could not parse EDID\n");
			core->mode.edid_done = 0;
			return;
		}

		core->blacklist_sink = hdmi_mode_blacklist_check((u8 *)edid);
		if (core->blacklist_sink >= 0)
			pr_info("Warning: this sink is in blacklist: %d\n", core->blacklist_sink);

		break;
	} while (edid_tries--);

	if (edid_tries <= 0) {
		pr_info("Could not read EDID\n");
		core->mode.edid_done = 0;
		return;
	}

	if (edid->extensions == 0) {
		core->mode.edid_done = 1;
	} else {
		int edid_ext_cnt = 1;

		while (edid_ext_cnt <= edid->extensions) {
			HDMI_INFO_MSG("EDID Extension %d\n", edid_ext_cnt);
			edid_tries = 3;
			do {
				edid_ok = core->dev_func.edid_extension_read(edid_ext_cnt, edid_ext);
				if (edid_ok)
					continue;

				core->mode.edid_ext = edid_ext;
		/* TODO: add support for EDID parsing w/ Ext blocks > 1 */
				if (edid_ext_cnt < 2) {
					if (core->dev_func.edid_parser(edid_ext, sink, 128) == false) {
						pr_info("Could not parse EDID EXTENSIONS\n");
						core->mode.edid_done = 0;
					}
					core->mode.edid_done = 1;
				}
				break;
			} while (edid_tries--);
			edid_ext_cnt++;
		}
	}
}

int edid_sink_supports_vic_code(u32 vic_code)
{
	int i = 0;
	struct hdmi_tx_core *p_core = get_platform();

	if (p_core == NULL)
		return false;


	if (p_core->mode.sink_cap == NULL)
		return false;


	for (i = 0; (i < 128) && (p_core->mode.sink_cap->edid_mSvd[i].mCode != 0); i++) {
		if (p_core->mode.sink_cap->edid_mSvd[i].mCode == vic_code)
			return true;
	}

	for (i = 0; (i < 4) && (p_core->mode.sink_cap->edid_mHdmivsdb.mHdmiVic[i] != 0); i++) {
		if (p_core->mode.sink_cap->edid_mHdmivsdb.mHdmiVic[i] == videoParams_GetHdmiVicCode(vic_code))
				return true;

	}

	if ((vic_code == 0x80 + 19) || (vic_code == 0x80 + 4)
		|| (vic_code == 0x80 + 32)) {
		if (p_core->mode.sink_cap->edid_mHdmivsdb.m3dPresent == 1) {
			HDMI_INFO_MSG("Support basic 3d video format\n");
			return true;
		} else {
		HDMI_INFO_MSG("Do not support any basic 3d video format\n");
		}
	}

	return false;
}



bool get_cea_vic_support(u32 cea_vic)
{
	struct hdmi_tx_core *core = get_platform();
	struct hdmi_mode *mode = &core->mode;
	bool support = false;
	u32 i;

	if (cea_vic == 0) {
		pr_info("error: invalid cea_vic code:%d\n", cea_vic);
		goto exit;
	}

	if ((mode == NULL) || (mode->sink_cap == NULL)) {
		pr_info("error: Has NULL struct\n");
		goto exit;
	}
	for (i = 0; (i < 128) && (mode->sink_cap->edid_mSvd[i].mCode != 0); i++) {
		if (mode->sink_cap->edid_mSvd[i].mCode == cea_vic)
			support = true;
	}

exit:
	return support;
}

bool get_hdmi_vic_support(u32 cea_vic)
{
	struct hdmi_tx_core *core = get_platform();
	struct hdmi_mode *mode = &core->mode;
	bool support = false;
	u32 i;

	if (cea_vic == 0) {
		pr_info("error: invalid cea_vic code:%d\n", cea_vic);
		goto exit;
	}

	if ((mode == NULL) || (mode->sink_cap == NULL)) {
		pr_info("error: Has NULL struct\n");
		goto exit;
	}
	for (i = 0; (i < 4) && (mode->sink_cap->edid_mHdmivsdb.mHdmiVic[i] != 0); i++) {
		if (mode->sink_cap->edid_mHdmivsdb.mHdmiVic[i] == videoParams_GetHdmiVicCode(cea_vic))
			support = true;
	}

exit:
	return support;
}


/*
* mainly for correcting hardware config after booting
* reason: as we do not read edid during booting,
* some configs that depended on edid
* can not be sure, so we have to correct them after
* booting and reading edid
*/
void edid_correct_hardware_config(void)
{
	struct hdmi_tx_core *core = get_platform();
	u8 vsif_data[2];
	int cea_code = 0;
	int hdmi_code = 0;

	if (!core->mode.edid_done)
		return;

	/*correct vic setting*/
	core->dev_func.get_vsif(vsif_data);
	if ((vsif_data[0]) >> 5 == 0x1) {
		/*To check if there is sending hdmi_vic*/
		cea_code = videoParams_GetCeaVicCode(vsif_data[1]);
		if (cea_code > 0) {
			if (get_cea_vic_support(cea_code)) {
				vsif_data[0] = 0x0;
				vsif_data[1] = 0x0;
				core->dev_func.set_vsif(vsif_data);
				core->dev_func.set_video_code(cea_code);
			}
		}
	} else if ((vsif_data[0] >> 5) == 0x0) {
		/*To check if there is sending cea_vic*/
		cea_code = core->dev_func.get_video_code();
		if (!get_cea_vic_support(cea_code)) {
			hdmi_code = videoParams_GetHdmiVicCode(cea_code);
			if (hdmi_code > 0) {
				vsif_data[0] = 0x1 << 5;
				vsif_data[1] = hdmi_code;
				if (get_hdmi_vic_support(cea_code)) {
					core->dev_func.set_video_code(0);
					core->dev_func.set_vsif(vsif_data);
				}
			}
		}
	}
}

#endif

void videoParams_SetYcc420Support(dtd_t *paramsDtd, shortVideoDesc_t *paramsSvd)
{
	paramsDtd->mLimitedToYcc420 = paramsSvd->mLimitedToYcc420;
	paramsDtd->mYcc420 = paramsSvd->mYcc420;
	HDMI_INFO_MSG("set ParamsDtd->limite %d\n",
				paramsDtd->mLimitedToYcc420);
	HDMI_INFO_MSG("set ParamsDtd->supports %d\n", paramsDtd->mYcc420);
}

static void edid_set_video_prefered(sink_edid_t *sink_cap,
						videoParams_t *pVideo)
{
	/* Configure using Native SVD or HDMI_VIC */
	int i = 0;
	int hdmi_vic = 0;
#if defined(__LINUX_PLAT__)
	bool cea_vic_support = false;
	bool hdmi_vic_support = false;
#endif
	unsigned int vic_index = 0;
	struct hdmi_tx_core *core = get_platform();

	LOG_TRACE();
	if ((sink_cap == NULL) || (pVideo == NULL)) {
		HDMI_ERROR_MSG("Have NULL params\n");
		return;
	}

	if ((pVideo->mDtd.mCode == 0x201)
		|| (pVideo->mDtd.mCode == 0x202)
		|| (pVideo->mDtd.mCode == 0x203)) {
		pVideo->mCea_code = 0;
		pVideo->mHdmi_code = 0;
		return;
	}
#if defined(__LINUX_PLAT__)
	if (!core->mode.edid_done) {
		pVideo->mCea_code = pVideo->mDtd.mCode;
		return;
	}

	if ((pVideo->mDtd.mCode == 0x80 + 19) ||
		(pVideo->mDtd.mCode == 0x80 + 4) ||
		(pVideo->mDtd.mCode == 0x80 + 32)) {
		pVideo->mCea_code = 0;
		pVideo->mHdmi_code = 0;
		return;
	}


#endif
	pVideo->mHdmi20 = sink_cap->edid_m20Sink;

	/*parse if this vic in sink support yuv420*/
	for (i = 0; (i < 128) && (sink_cap->edid_mSvd[i].mCode != 0); i++) {
		if (sink_cap->edid_mSvd[i].mCode == pVideo->mDtd.mCode) {
			vic_index = i;
			break;
		}
	}

	videoParams_SetYcc420Support(&pVideo->mDtd,
					&sink_cap->edid_mSvd[vic_index]);
#if defined(__LINUX_PLAT__)
	cea_vic_support = get_cea_vic_support(pVideo->mDtd.mCode);
	if (cea_vic_support)
		pr_info("Support cea vic mode:%d\n", pVideo->mDtd.mCode);
	hdmi_vic_support = get_hdmi_vic_support(pVideo->mDtd.mCode);
	if (hdmi_vic_support)
		pr_info("Support hdmi vic mode:%d\n", videoParams_GetHdmiVicCode(pVideo->mDtd.mCode));

	if (cea_vic_support) {
		pVideo->mCea_code = pVideo->mDtd.mCode;
		pVideo->mHdmi_code = 0;
		core->mode.pProduct.mOUI = 0x000c03;
		core->mode.pProduct.mVendorPayload[0] = 0x0;
		core->mode.pProduct.mVendorPayload[1] = 0;
		core->mode.pProduct.mVendorPayload[2] = 0;
		core->mode.pProduct.mVendorPayloadLength = 3;
	} else if (hdmi_vic_support) {
		pVideo->mCea_code = 0;
		hdmi_vic = videoParams_GetHdmiVicCode(pVideo->mDtd.mCode);
		pVideo->mHdmi_code = hdmi_vic;
		core->mode.pProduct.mOUI = 0x000c03;
		core->mode.pProduct.mVendorPayload[0] = 0x20;
		core->mode.pProduct.mVendorPayload[1] = hdmi_vic;
		core->mode.pProduct.mVendorPayload[2] = 0;
		core->mode.pProduct.mVendorPayloadLength = 3;
	} else {
		pVideo->mCea_code = pVideo->mDtd.mCode;
		pr_info("[HDMI2 error]: sink do not support this mode:%d\n",
						pVideo->mDtd.mCode);
	}

	pVideo->scdc_ability = sink_cap->edid_mHdmiForumvsdb.mSCDC_Present;
#else

	hdmi_vic = videoParams_GetHdmiVicCode(pVideo->mDtd.mCode);
	if (hdmi_vic > 0) {
		core->mode.pProduct.mOUI = 0x000c03;
		core->mode.pProduct.mVendorPayload[0] = 0x20;
		core->mode.pProduct.mVendorPayload[1] = hdmi_vic;
		core->mode.pProduct.mVendorPayloadLength = 2;

		pVideo->mDtd.mCode = 0;
		pVideo->mCea_code = 0;
		pVideo->mHdmi_code = hdmi_vic;
	} else {
		if ((pVideo->mDtd.mCode == 96)
			|| (pVideo->mDtd.mCode == 97)
			|| (pVideo->mDtd.mCode == 101)
			|| (pVideo->mDtd.mCode == 102)) {
			core->mode.pProduct.mOUI = 0xc45dd8;
			core->mode.pProduct.mVendorPayload[0] = 1;
			core->mode.pProduct.mVendorPayload[1] = 0;
			core->mode.pProduct.mVendorPayloadLength = 2;

			pVideo->mCea_code = pVideo->mDtd.mCode;
			pVideo->mHdmi_code = 0;
		} else {
			core->mode.pProduct.mOUI = 0x000c03;
			core->mode.pProduct.mVendorPayload[0] = 0;
			core->mode.pProduct.mVendorPayload[1] = 0;
			core->mode.pProduct.mVendorPayloadLength = 2;

			pVideo->mCea_code = pVideo->mDtd.mCode;
			pVideo->mHdmi_code = 0;
		}
	}

#endif
}



void edid_set_video_prefered_core(void)
{
	struct hdmi_tx_core *core = get_platform();
#if defined(__LINUX_PLAT__)
	if (core->mode.sink_cap == NULL)
		pr_info("%s: sink cap is NULL!\n", __func__);
	else
#endif
	edid_set_video_prefered(core->mode.sink_cap,
				&core->mode.pVideo);
}


