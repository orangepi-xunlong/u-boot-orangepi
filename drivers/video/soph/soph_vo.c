#include <asm/unaligned.h>

#include <config.h>
#include <common.h>
#include <errno.h>
#include <linux/libfdt.h>
#include <fdtdec.h>
#include <fdt_support.h>

#include <linux/list.h>
#include <linux/compat.h>
#include <linux/hdmi.h>
#include <linux/media-bus-format.h>
#include <malloc.h>
#include <video.h>
#include <video_soph.h>
#include <video_bridge.h>
#include <dm/device.h>
#include <dm/of_access.h>
#include <dm/uclass-internal.h>
#include <drm_modes.h>
#include "soph_bmp.h"

#include "soph_vo.h"
#include "soph_crtc.h"
#include "soph_connector.h"
#include "soph_vo_sys_reg.h"
#include "soph_mipipll_cfg.h"
#include "soph_bridge.h"
#include <dm.h>
#include <dm/ofnode.h>
#include <asm/io.h>
#include "soph_logo.h"

#include <fs.h>

#define LOGO_ADDR (0x102E00000)

DECLARE_GLOBAL_DATA_PTR;
static LIST_HEAD(soph_display_list);
static LIST_HEAD(logo_cache_list);

enum {
	PORT_DIR_IN,
	PORT_DIR_OUT,
};

struct soph_vo_priv {
	phys_addr_t vo_sys_f;
	phys_addr_t vo_sys_b ;
	phys_addr_t top_pll;
	struct disp_ctrl_gpios ctrl_gpios;
};

int drm_mode_vrefresh(const struct drm_display_mode *mode)
{
	int refresh = 0;
	unsigned int calc_val;

	if (mode->vrefresh > 0) {
		refresh = mode->vrefresh;
	} else if (mode->htotal > 0 && mode->vtotal > 0) {
		int vtotal;

		vtotal = mode->vtotal;
		/* work out vrefresh the value will be x1000 */
		calc_val = (mode->clock * 1000);
		calc_val /= mode->htotal;
		refresh = (calc_val + vtotal / 2) / vtotal;

		if (mode->flags & DRM_MODE_FLAG_INTERLACE)
			refresh *= 2;
		if (mode->flags & DRM_MODE_FLAG_DBLSCAN)
			refresh /= 2;
		if (mode->vscan > 1)
			refresh /= mode->vscan;
	}
	return refresh;
}

int soph_ofnode_get_display_mode(ofnode node, struct drm_display_mode *mode)
{
	int hactive, vactive, pixelclock;
	int hfront_porch, hback_porch, hsync_len;
	int vfront_porch, vback_porch, vsync_len;
	int val, flags = 0;

#define FDT_GET_INT(val, name) \
	val = ofnode_read_s32_default(node, name, -1); \
	if (val < 0) { \
		printf("Can't get %s\n", name); \
		return -ENXIO; \
	}

#define FDT_GET_INT_DEFAULT(val, name, default) \
	val = ofnode_read_s32_default(node, name, default)

	FDT_GET_INT(hactive, "hactive");
	FDT_GET_INT(vactive, "vactive");
	FDT_GET_INT(pixelclock, "clock-frequency");
	FDT_GET_INT(hsync_len, "hsync-len");
	FDT_GET_INT(hfront_porch, "hfront-porch");
	FDT_GET_INT(hback_porch, "hback-porch");
	FDT_GET_INT(vsync_len, "vsync-len");
	FDT_GET_INT(vfront_porch, "vfront-porch");
	FDT_GET_INT(vback_porch, "vback-porch");
	FDT_GET_INT(val, "hsync-active");
	flags |= val ? DRM_MODE_FLAG_PHSYNC : DRM_MODE_FLAG_NHSYNC;
	FDT_GET_INT(val, "vsync-active");
	flags |= val ? DRM_MODE_FLAG_PVSYNC : DRM_MODE_FLAG_NVSYNC;
	FDT_GET_INT(val, "pixelclk-active");
	flags |= val ? DRM_MODE_FLAG_PPIXDATA : 0;

	FDT_GET_INT_DEFAULT(val, "screen-rotate", 0);
	if (val == DRM_MODE_FLAG_XMIRROR) {
		flags |= DRM_MODE_FLAG_XMIRROR;
	} else if (val == DRM_MODE_FLAG_YMIRROR) {
		flags |= DRM_MODE_FLAG_YMIRROR;
	} else if (val == DRM_MODE_FLAG_XYMIRROR) {
		flags |= DRM_MODE_FLAG_XMIRROR;
		flags |= DRM_MODE_FLAG_YMIRROR;
	}
	mode->hdisplay = hactive;
	mode->hsync_start = mode->hdisplay + hfront_porch;
	mode->hsync_end = mode->hsync_start + hsync_len;
	mode->htotal = mode->hsync_end + hback_porch;

	mode->vdisplay = vactive;
	mode->vsync_start = mode->vdisplay + vfront_porch;
	mode->vsync_end = mode->vsync_start + vsync_len;
	mode->vtotal = mode->vsync_end + vback_porch;

	mode->clock = pixelclock / 1000;
	mode->flags = flags;
	mode->vrefresh = drm_mode_vrefresh(mode);

	return 0;
}

#if 0
static int display_get_force_timing_from_dts(ofnode node, struct drm_display_mode *mode)
{
	int ret = 0;

	ret = soph_ofnode_get_display_mode(node, mode);

	if (ret) {
		mode->clock = 74250;
		mode->flags = 0x5;
		mode->hdisplay = 1280;
		mode->hsync_start = 1390;
		mode->hsync_end = 1430;
		mode->htotal = 1650;
		mode->hskew = 0;
		mode->vdisplay = 720;
		mode->vsync_start = 725;
		mode->vsync_end = 730;
		mode->vtotal = 750;
		mode->vrefresh = 60;
		// mode->picture_aspect_ratio = HDMI_PICTURE_ASPECT_16_9;
		mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	}

	printf("route node %s force_timing, use %dx%dp%d as default mode\n",
	       ret ? "undefine" : "define", mode->hdisplay, mode->vdisplay,
	       mode->vscan);

	return 0;
}
#endif

void drm_mode_max_resolution_filter(struct hdmi_edid_data *edid_data,
				    struct disp_rect *max_output)
{
	int i;

	for (i = 0; i < edid_data->modes; i++) {
		if (edid_data->mode_buf[i].hdisplay > 4096 ||
		    edid_data->mode_buf[i].vdisplay > 2160)
			edid_data->mode_buf[i].invalid = true;
	}
}

/**
 * drm_mode_set_crtcinfo - set CRTC modesetting timing parameters
 * @p: mode
 * @adjust_flags: a combination of adjustment flags
 *
 * Setup the CRTC modesetting timing parameters for @p, adjusting if necessary.
 *
 * - The CRTC_INTERLACE_HALVE_V flag can be used to halve vertical timings of
 *   interlaced modes.
 * - The CRTC_STEREO_DOUBLE flag can be used to compute the timings for
 *   buffers containing two eyes (only adjust the timings when needed, eg. for
 *   "frame packing" or "side by side full").
 * - The CRTC_NO_DBLSCAN and CRTC_NO_VSCAN flags request that adjustment *not*
 *   be performed for doublescan and vscan > 1 modes respectively.
 */
void drm_mode_set_crtcinfo(struct drm_display_mode *p, int adjust_flags)
{
	if ((p == NULL) || ((p->type & DRM_MODE_TYPE_CRTC_C) == DRM_MODE_TYPE_BUILTIN))
		return;

	if (p->flags & DRM_MODE_FLAG_DBLCLK)
		p->crtc_clock = 2 * p->clock;
	else
		p->crtc_clock = p->clock;
	p->crtc_hdisplay = p->hdisplay;
	p->crtc_hsync_start = p->hsync_start;
	p->crtc_hsync_end = p->hsync_end;
	p->crtc_htotal = p->htotal;
	p->crtc_hskew = p->hskew;
	p->crtc_vdisplay = p->vdisplay;
	p->crtc_vsync_start = p->vsync_start;
	p->crtc_vsync_end = p->vsync_end;
	p->crtc_vtotal = p->vtotal;

	if (p->flags & DRM_MODE_FLAG_INTERLACE) {
		if (adjust_flags & CRTC_INTERLACE_HALVE_V) {
			p->crtc_vdisplay /= 2;
			p->crtc_vsync_start /= 2;
			p->crtc_vsync_end /= 2;
			p->crtc_vtotal /= 2;
		}
	}

	if (!(adjust_flags & CRTC_NO_DBLSCAN)) {
		if (p->flags & DRM_MODE_FLAG_DBLSCAN) {
			p->crtc_vdisplay *= 2;
			p->crtc_vsync_start *= 2;
			p->crtc_vsync_end *= 2;
			p->crtc_vtotal *= 2;
		}
	}

	if (!(adjust_flags & CRTC_NO_VSCAN)) {
		if (p->vscan > 1) {
			p->crtc_vdisplay *= p->vscan;
			p->crtc_vsync_start *= p->vscan;
			p->crtc_vsync_end *= p->vscan;
			p->crtc_vtotal *= p->vscan;
		}
	}

	if (adjust_flags & CRTC_STEREO_DOUBLE) {
		unsigned int layout = p->flags & DRM_MODE_FLAG_3D_MASK;

		switch (layout) {
		case DRM_MODE_FLAG_3D_FRAME_PACKING:
			p->crtc_clock *= 2;
			p->crtc_vdisplay += p->crtc_vtotal;
			p->crtc_vsync_start += p->crtc_vtotal;
			p->crtc_vsync_end += p->crtc_vtotal;
			p->crtc_vtotal += p->crtc_vtotal;
			break;
		}
	}

	p->crtc_vblank_start = min(p->crtc_vsync_start, p->crtc_vdisplay);
	p->crtc_vblank_end = max(p->crtc_vsync_end, p->crtc_vtotal);
	p->crtc_hblank_start = min(p->crtc_hsync_start, p->crtc_hdisplay);
	p->crtc_hblank_end = max(p->crtc_hsync_end, p->crtc_htotal);
}

/**
 * drm_mode_is_420_only - if a given videomode can be only supported in YCBCR420
 * output format
 *
 * @connector: drm connector under action.
 * @mode: video mode to be tested.
 *
 * Returns:
 * true if the mode can be supported in YCBCR420 format
 * false if not.
 */
bool drm_mode_is_420_only(const struct drm_display_info *display,
			  struct drm_display_mode *mode)
{
	u8 vic = drm_match_cea_mode(mode);

	return test_bit(vic, display->hdmi.y420_vdb_modes);
}

/**
 * drm_mode_is_420_also - if a given videomode can be supported in YCBCR420
 * output format also (along with RGB/YCBCR444/422)
 *
 * @display: display under action.
 * @mode: video mode to be tested.
 *
 * Returns:
 * true if the mode can be support YCBCR420 format
 * false if not.
 */
bool drm_mode_is_420_also(const struct drm_display_info *display,
			  struct drm_display_mode *mode)
{
	u8 vic = drm_match_cea_mode(mode);

	return test_bit(vic, display->hdmi.y420_cmdb_modes);
}

/**
 * drm_mode_is_420 - if a given videomode can be supported in YCBCR420
 * output format
 *
 * @display: display under action.
 * @mode: video mode to be tested.
 *
 * Returns:
 * true if the mode can be supported in YCBCR420 format
 * false if not.
 */
bool drm_mode_is_420(const struct drm_display_info *display,
		     struct drm_display_mode *mode)
{
	return drm_mode_is_420_only(display, mode) ||
		drm_mode_is_420_also(display, mode);
}
#if 0
static int display_get_timing(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	struct drm_display_mode *mode = &conn_state->mode;
	const struct drm_display_mode *m;
	struct soph_panel *panel = conn_state->connector->panel;

	if (panel->funcs->get_mode)
		return panel->funcs->get_mode(panel, mode);

	if (dev_of_valid(panel->dev) &&
	    !display_get_timing_from_dts(panel, mode)) {
		printf("Using display timing dts\n");
		return 0;
	}

	if (panel->data) {
		m = (const struct drm_display_mode *)panel->data;
		memcpy(mode, m, sizeof(*m));
		printf("Using display timing from compatible panel driver\n");
		return 0;
	}

	return -ENODEV;
}
#endif
static int display_pre_init(void)
{
	struct display_state *state;
	int ret = 0;

	list_for_each_entry(state, &soph_display_list, head) {
		struct connector_state *conn_state = &state->conn_state;
		struct crtc_state *crtc_state = &state->crtc_state;
		struct soph_crtc *crtc = crtc_state->crtc;

		ret = soph_connector_pre_init(state);
		if (ret)
			printf("pre init conn error\n");

		crtc->vps[crtc_state->crtc_id].output_type = conn_state->type;
	}
	return ret;
}

static int display_use_force_mode(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	struct drm_display_mode *mode = &conn_state->mode;

	conn_state->bpc = 8;
	memcpy(mode, &state->force_mode, sizeof(struct drm_display_mode));
	conn_state->bus_format = state->force_bus_format;

	return 0;
}

static int display_get_edid_mode(struct display_state *state)
{
	int ret = 0;
	struct connector_state *conn_state = &state->conn_state;
	struct drm_display_mode *mode = &conn_state->mode;
	int bpc;

	ret = edid_get_drm_mode(conn_state->edid, sizeof(conn_state->edid), mode, &bpc);
	if (!ret) {
		conn_state->bpc = bpc;
		edid_print_info((void *)&conn_state->edid);
	} else {
		conn_state->bpc = 8;
		mode->clock = 74250;
		mode->flags = 0x5;
		mode->hdisplay = 1280;
		mode->hsync_start = 1390;
		mode->hsync_end = 1430;
		mode->htotal = 1650;
		mode->hskew = 0;
		mode->vdisplay = 720;
		mode->vsync_start = 725;
		mode->vsync_end = 730;
		mode->vtotal = 750;
		mode->vrefresh = 60;
		mode->picture_aspect_ratio = HDMI_PICTURE_ASPECT_16_9;
		mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;

		printf("error: %s get mode from edid failed, use 720p60 as default mode\n",
		       state->conn_state.connector->dev->name);
	}

	return ret;
}

static int display_init(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	struct soph_connector *conn = conn_state->connector;
	struct crtc_state *crtc_state = &state->crtc_state;
	struct soph_crtc *crtc = crtc_state->crtc;
	const struct soph_crtc_funcs *crtc_funcs = crtc->funcs;
	struct drm_display_mode *mode = &conn_state->mode;

	int ret = 0;

	if (state->is_init)
		return 0;

	if (!crtc_funcs) {
		printf("failed to find crtc functions\n");
		return -ENXIO;
	}

	if (crtc_state->crtc->active && !crtc_state->ports_node &&
	    memcmp(&crtc_state->crtc->active_mode, &conn_state->mode,
		   sizeof(struct drm_display_mode))) {
		printf("%s has been used for output type: %d, mode: %dx%dp%d\n",
			crtc_state->dev->name,
			crtc_state->crtc->active_mode.type,
			crtc_state->crtc->active_mode.hdisplay,
			crtc_state->crtc->active_mode.vdisplay,
			crtc_state->crtc->active_mode.vrefresh);
		return -ENODEV;
	}

	if (crtc_funcs->preinit) {
		ret = crtc_funcs->preinit(state);
		if (ret)
			return ret;
	}

	ret = soph_connector_init(state);
	if (ret)
		goto deinit;

	ret = soph_connector_detect(state);

	if (!ret && !state->force_output)
		goto deinit;
#if 0
	if (conn->panel) {
		/*TBD*/
	} else if (conn->bridge) {
		ret = video_bridge_read_edid(conn->bridge->dev,
					     conn_state->edid, EDID_SIZE);
		if (ret > 0) {
#if defined(CONFIG_I2C_EDID)
			display_get_edid_mode(state);
#endif
		} else {
			// ret = video_bridge_get_timing(conn->bridge->dev);     //tbd?
		}
	}
#endif
	else if (conn->funcs->get_timing) {
		ret = conn->funcs->get_timing(conn, state);
	} else if (conn->funcs->get_edid) {
		ret = conn->funcs->get_edid(conn, state);
#if defined(CONFIG_I2C_EDID)
		if (!ret)
			display_get_edid_mode(state);
#endif
	}

	if (!ret && conn_state->secondary) {
#if 0
		struct soph_connector *connector = conn_state->secondary;

		if (connector->panel) {
			if (connector->panel->funcs->get_mode) {
				struct drm_display_mode *_mode = drm_mode_create();

				ret = connector->panel->funcs->get_mode(connector->panel, _mode);
				if (!ret && !drm_mode_equal(_mode, mode))
					ret = -EINVAL;

				drm_mode_destroy(_mode);
			}
		}
#endif
	}

	if (ret && !state->force_output)
		goto deinit;
	if (state->force_output)
		display_use_force_mode(state);

	debug("%s: %s detailed mode clock %u kHz, flags[%x]\n"
	       "    H: %04d %04d %04d %04d\n"
	       "    V: %04d %04d %04d %04d\n"
	       "bus_format: %x\n",
	       conn->dev->name,
	       state->force_output ? "use force output" : "",
	       mode->clock, mode->flags,
	       mode->hdisplay, mode->hsync_start,
	       mode->hsync_end, mode->htotal,
	       mode->vdisplay, mode->vsync_start,
	       mode->vsync_end, mode->vtotal,
	       conn_state->bus_format);

	drm_mode_set_crtcinfo(mode, CRTC_INTERLACE_HALVE_V);

	if (conn_state->secondary) {
		mode->crtc_clock *= 2;
		mode->crtc_hdisplay *= 2;
		mode->crtc_hsync_start *= 2;
		mode->crtc_hsync_end *= 2;
		mode->crtc_htotal *= 2;
	}

	if (conn->bridge)
		soph_bridge_mode_set(conn->bridge, &conn_state->mode);

	if (crtc_funcs->init) {
		mipipll_clk_set(mode->clock);
		ret = crtc_funcs->init(state);
		if (ret)
			goto deinit;
	}
	state->is_init = 1;

	crtc_state->crtc->active = true;
	memcpy(&crtc_state->crtc->active_mode,
	       &conn_state->mode, sizeof(struct drm_display_mode));

	return 0;

deinit:
	soph_connector_deinit(state);
	return ret;
}

#if 0
static int display_set_plane(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	const struct soph_crtc *crtc = crtc_state->crtc;
	const struct soph_crtc_funcs *crtc_funcs = crtc->funcs;
	int ret;

	if (!state->is_init)
		return -EINVAL;

	if (crtc_funcs->set_plane) {
		ret = crtc_funcs->set_plane(state);
		if (ret)
			return ret;
	}

	return 0;
}
#endif

static int display_enable(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	const struct soph_crtc *crtc = crtc_state->crtc;
	const struct soph_crtc_funcs *crtc_funcs = crtc->funcs;

	if (!state->is_init)
		return -EINVAL;

	if (state->is_enable)
		return 0;

	if (crtc_funcs->prepare)
		crtc_funcs->prepare(state);

	soph_connector_pre_enable(state);

	if (crtc_funcs->enable)
		crtc_funcs->enable(state);

	soph_connector_enable(state);

	state->is_enable = true;

	return 0;
}

#if 0
static int display_disable(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	const struct soph_crtc *crtc = crtc_state->crtc;
	const struct soph_crtc_funcs *crtc_funcs = crtc->funcs;

	if (!state->is_init)
		return 0;

	if (!state->is_enable)
		return 0;

	soph_connector_disable(state);

	if (crtc_funcs->disable)
		crtc_funcs->disable(state);

	soph_connector_post_disable(state);

	state->is_enable = 0;
	state->is_init = 0;

	return 0;
}
#endif

static int display_check(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	struct soph_connector *conn = conn_state->connector;
	const struct soph_connector_funcs *conn_funcs = conn->funcs;
	struct crtc_state *crtc_state = &state->crtc_state;
	const struct soph_crtc *crtc = crtc_state->crtc;
	const struct soph_crtc_funcs *crtc_funcs = crtc->funcs;
	int ret;

	if (!state->is_init)
		return 0;

	if (conn_funcs->check) {
		ret = conn_funcs->check(conn, state);
		if (ret)
			goto check_fail;
	}

	if (crtc_funcs->check) {
		ret = crtc_funcs->check(state);
		if (ret)
			goto check_fail;
	}

	if (crtc_funcs->plane_check) {
		ret = crtc_funcs->plane_check(state);
		if (ret)
			goto check_fail;
	}

	return 0;

check_fail:
	state->is_init = false;
	return ret;
}

static int display_mode_valid(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	struct soph_connector *conn = conn_state->connector;
	const struct soph_connector_funcs *conn_funcs = conn->funcs;
	struct crtc_state *crtc_state = &state->crtc_state;
	const struct soph_crtc *crtc = crtc_state->crtc;
	const struct soph_crtc_funcs *crtc_funcs = crtc->funcs;
	int ret;

	if (!state->is_init)
		return 0;

	if (conn_funcs->mode_valid) {
		ret = conn_funcs->mode_valid(conn, state);
		if (ret)
			goto invalid_mode;
	}

	if (crtc_funcs->mode_valid) {
		ret = crtc_funcs->mode_valid(state);
		if (ret)
			goto invalid_mode;
	}

	return 0;

invalid_mode:
	state->is_init = false;
	return ret;
}

static int display_logo(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	struct connector_state *conn_state = &state->conn_state;
	struct logo_info *logo = &state->logo;
	int hdisplay, vdisplay, ret;

	hdisplay = conn_state->mode.crtc_hdisplay;
	vdisplay = conn_state->mode.vdisplay;
	crtc_state->src_rect.w = logo->width;
	crtc_state->src_rect.h = logo->height;
	crtc_state->src_rect.x = 0;
	crtc_state->src_rect.y = 0;
	crtc_state->ymirror = logo->ymirror;

	crtc_state->dma_addr = (u64)(unsigned long long)(logo->mem + logo->offset);

	crtc_state->xvir = ALIGN(crtc_state->src_rect.w * logo->bpp, 64) >> 5;

	if (state->logo_mode == SOPH_DISPLAY_FULLSCREEN) {
		crtc_state->crtc_rect.x = 0;
		crtc_state->crtc_rect.y = 0;
		crtc_state->crtc_rect.w = hdisplay;
		crtc_state->crtc_rect.h = vdisplay;
	} else {
		if (crtc_state->src_rect.w >= hdisplay) {
			crtc_state->crtc_rect.x = 0;
			crtc_state->crtc_rect.w = hdisplay;
		} else {
			crtc_state->crtc_rect.x = (hdisplay - crtc_state->src_rect.w) / 2;
			crtc_state->crtc_rect.w = crtc_state->src_rect.w;
		}

		if (crtc_state->src_rect.h >= vdisplay) {
			crtc_state->crtc_rect.y = 0;
			crtc_state->crtc_rect.h = vdisplay;
		} else {
			crtc_state->crtc_rect.y = (vdisplay - crtc_state->src_rect.h) / 2;
			crtc_state->crtc_rect.h = crtc_state->src_rect.h;
		}
	}

	ret = display_init(state);
	if (!state->is_init || ret)
		return -ENODEV;

	display_mode_valid(state);
	display_check(state);
#if 0
	display_set_plane(state);
#endif
	display_enable(state);

	return 0;
}

static int get_crtc_id(ofnode connect, bool is_ports_node)
{
	ofnode port_node;
	int val;

	if (is_ports_node) {
		port_node.of_offset = fdt_parent_offset(gd->fdt_blob,
						     ofnode_to_offset(connect));

		if (!port_node.of_offset)
			goto err;

		val = ofnode_read_u32_default(np_to_ofnode(port_node.np), "reg", -1);
		if (val < 0)
			goto err;
	} else {
		val = ofnode_read_u32_default(connect, "reg", -1);
		if (val < 0)
			goto err;
	}

	return val;
err:
	printf("Can't get crtc id, default set to id = 0\n");
	return 0;
}

struct soph_logo_cache *find_or_alloc_logo_cache(const char *bmp)
{
	struct soph_logo_cache *tmp, *logo_cache = NULL;

	list_for_each_entry(tmp, &logo_cache_list, head) {
		if (!strcmp(tmp->name, bmp)) {
			logo_cache = tmp;
			break;
		}
	}

	if (!logo_cache) {
		logo_cache = malloc(sizeof(*logo_cache));
		if (!logo_cache) {
			printf("failed to alloc memory for logo cache\n");
			return NULL;
		}
		memset(logo_cache, 0, sizeof(*logo_cache));
		strcpy(logo_cache->name, bmp);
		INIT_LIST_HEAD(&logo_cache->head);
		list_add_tail(&logo_cache->head, &logo_cache_list);
	}

	return logo_cache;
}

void soph_load_logo(void *addr)
{
	char cmd_all[30];
	int ret;

#if defined(CONFIG_ROOTFS_UBUNTU) || defined(CONFIG_ROOTFS_DEBIAN)
	sprintf(cmd_all, "%s %s %s 0x%llx %s", "load", "mmc",
			"0:1", (u64)addr, "soph_logo.bmp");
	ret = run_command(cmd_all, 0);
	if (ret)
		printf("run soph load command error!\n");
#else
	char *misc_part_offset, *misc_part_size, *cmd;

	misc_part_offset = env_get("MISC_PART_OFFSET");
	misc_part_size = env_get("MISC_PART_SIZE");

	#ifdef CONFIG_NAND_SUPPORT
		sprintf(cmd_all, "%s %s 0x%llx %s", "nand", "read",
			(u64)addr, "MISC");
		run_command(cmd_all, 0);
		return;
	#elif defined(CONFIG_SPI_FLASH)
		cmd = "sf";
		run_command("sf probe", 0);
	#else
		cmd = "mmc";
		run_command("mmc dev 0", 0);
	#endif
		sprintf(cmd_all, "%s %s 0x%llx %s %s", cmd, "read",
			(u64)addr, misc_part_offset, misc_part_size);

	ret = run_command(cmd_all, 0);
	if (ret)
		printf("run soph load command error!\n");
#endif
}

static int load_bmp_logo(struct display_state *s)
{
	struct soph_logo_cache *logo_cache;
	struct bmp_header *header = NULL;
	void *dst, *pdst;
	int ret = 0;
	int dst_size;

	struct logo_info *logo = &s->logo;
	char *bmp_name = s->ulogo_name;

	if (!logo || !bmp_name)
		return -EINVAL;
	logo_cache = find_or_alloc_logo_cache(bmp_name);
	if (!logo_cache)
		return -ENOMEM;

	if (logo_cache->logo.mem) {
		memcpy(logo, &logo_cache->logo, sizeof(*logo));
		return 0;
	}

#ifdef CONFIG_LOGO_FROM_INTERNEL
	header = (struct bmp_header *)logo_bmp;
#else
	soph_load_logo(s->mem_base);
	header = (struct bmp_header *)s->mem_base;
#endif

	logo->bpp = get_unaligned_le16(&header->bit_count);
	logo->width = get_unaligned_le32(&header->width);
	logo->height = get_unaligned_le32(&header->height);

	dst_size = logo->width * logo->height * logo->bpp >> 3;

	if (logo->height < 0)
		logo->height = -logo->height;

	/*
	 * only support 24bpp;
	 */
	if (logo->bpp == 24) {
		dst = (void *)LOGO_ADDR;
	} else {
		printf("failed to display logo with bpp:%d\n", logo->bpp);
		ret = -EINVAL;
	}

#ifdef CONFIG_LOGO_FROM_INTERNEL
	pdst = (void *)logo_bmp;
#else
	pdst = (void *)header;
#endif

	if (bmpdecoder(pdst, dst, logo->bpp)) {
		printf("failed to decode bmp %s\n", bmp_name);
		ret = -EINVAL;
	}

	logo->mem = dst;
	memcpy(&logo_cache->logo, logo, sizeof(*logo));

	flush_dcache_range((ulong)dst, ALIGN((ulong)dst + dst_size, 32));

	return ret;
}

int soph_show_logo(void)
{
	int ret = 0;
	struct display_state *s;

	list_for_each_entry(s, &soph_display_list, head) {
		s->logo.mode = s->logo_mode;
		if (load_bmp_logo(s))
			printf("failed to display uboot logo\n");
		else
			ret = display_logo(s);
	}

	return ret;
}

static const struct device_node *soph_of_graph_get_port_by_id(ofnode node, int id)
{
	ofnode ports, port;
	u32 reg;

	ports = ofnode_find_subnode(node, "ports");
	if (!ofnode_valid(ports))
		return NULL;

	ofnode_for_each_subnode(port, ports) {
		if (ofnode_read_u32(port, "reg", &reg))
			continue;

		if (reg == id)
			break;
	}

	if (reg == id)
		return ofnode_to_np(port);

	return NULL;
}

static const struct device_node *soph_of_graph_get_port_parent(ofnode port)
{
	ofnode parent;
	int is_ports_node;

	parent = ofnode_get_parent(port);
	is_ports_node = strstr(ofnode_to_np(parent)->full_name, "ports") ? 1 : 0;
	if (is_ports_node)
		parent = ofnode_get_parent(parent);

	return ofnode_to_np(parent);
}

static const struct device_node *soph_of_graph_get_remote_node(ofnode node, int port,
								   int endpoint)
{
	const struct device_node *port_node;
	ofnode ep;
	u32 reg;
	uint phandle;

	port_node = soph_of_graph_get_port_by_id(node, port);
	if (!port_node)
		return NULL;

	ofnode_for_each_subnode(ep, np_to_ofnode(port_node)) {
		if (ofnode_read_u32(ep, "reg", &reg))
			break;
		if (reg == endpoint)
			break;
	}

	if (!ofnode_valid(ep))
		return NULL;

	if (ofnode_read_u32(ep, "remote-endpoint", &phandle))
		return NULL;

	ep = ofnode_get_by_phandle(phandle);
	if (!ofnode_valid(ep))
		return NULL;

	return ofnode_to_np(ep);
}

static int soph_of_find_panel(struct udevice *dev, struct soph_panel **panel)
{
	const struct device_node *ep_node, *panel_node;
	ofnode panel_ofnode, port;
	struct udevice *panel_dev;
	int ret = 0;

	*panel = NULL;
	panel_ofnode = dev_read_subnode(dev, "panel");
	if (ofnode_valid(panel_ofnode) && ofnode_is_available(panel_ofnode)) {
		ret = uclass_get_device_by_ofnode(UCLASS_PANEL, panel_ofnode,
						  &panel_dev);
		if (!ret)
			goto found;
	}

	ep_node = soph_of_graph_get_remote_node(dev->node_, PORT_DIR_OUT, 0);
	if (!ep_node)
		return -ENODEV;

	port = ofnode_get_parent(np_to_ofnode(ep_node));
	if (!ofnode_valid(port))
		return -ENODEV;

	panel_node = soph_of_graph_get_port_parent(port);
	if (!panel_node)
		return -ENODEV;

	ret = uclass_get_device_by_ofnode(UCLASS_PANEL, np_to_ofnode(panel_node), &panel_dev);
	if (!ret)
		goto found;

	return -ENODEV;

found:
	*panel = (struct soph_panel *)dev_get_driver_data(panel_dev);
	return 0;
}

static int soph_of_find_bridge(struct udevice *dev, struct soph_bridge **bridge)
{
	const struct device_node *ep_node, *bridge_node;
	ofnode port;
	struct udevice *bridge_dev;
	int ret = 0;

	ep_node = soph_of_graph_get_remote_node(dev->node_, PORT_DIR_OUT, 0);
	if (!ep_node)
		return -ENODEV;

	port = ofnode_get_parent(np_to_ofnode(ep_node));
	if (!ofnode_valid(port))
		return -ENODEV;

	bridge_node = soph_of_graph_get_port_parent(port);
	if (!bridge_node)
		return -ENODEV;

	ret = uclass_get_device_by_ofnode(UCLASS_VIDEO_BRIDGE, np_to_ofnode(bridge_node),
					  &bridge_dev);
	if (!ret)
		goto found;

	return -ENODEV;

found:
	*bridge = (struct soph_bridge *)dev_get_driver_data(bridge_dev);
	return 0;
}

static int soph_of_find_panel_or_bridge(struct udevice *dev, struct soph_panel **panel,
					    struct soph_bridge **bridge)
{
	int ret = 0;
	*panel = NULL;
	*bridge = NULL;

	if (panel) {
		ret  = soph_of_find_panel(dev, panel);
		if (!ret)
			return 0;
	}

	if (ret) {
		ret = soph_of_find_bridge(dev, bridge);
		if (!ret)
			ret = soph_of_find_panel_or_bridge((*bridge)->dev, panel,
							       &(*bridge)->next_bridge);
	}

	return ret;
}

static struct udevice *soph_of_find_connector_device(ofnode endpoint)
{
	ofnode ep, port, ports, conn;
	uint phandle;
	struct udevice *dev;
	int ret;

	if (ofnode_read_u32(endpoint, "remote-endpoint", &phandle))
		return NULL;

	ep = ofnode_get_by_phandle(phandle);
	if (!ofnode_valid(ep) || !ofnode_is_available(ep))
		return NULL;

	port = ofnode_get_parent(ep);
	if (!ofnode_valid(port))
		return NULL;

	ports = ofnode_get_parent(port);
	if (!ofnode_valid(ports))
		return NULL;

	conn = ofnode_get_parent(ports);
	if (!ofnode_valid(conn) || !ofnode_is_available(conn))
		return NULL;

	ret = uclass_get_device_by_ofnode(UCLASS_DISPLAY, conn, &dev);
	if (ret)
		return NULL;

	return dev;
}

static struct soph_connector *soph_of_get_connector(ofnode endpoint)
{
	struct soph_connector *conn;
	struct udevice *dev;
	int ret;

	dev = soph_of_find_connector_device(endpoint);
	if (!dev) {
		debug("Warn: can't find connect driver\n");
		return NULL;
	}

	conn = get_soph_connector_by_device(dev);
	if (!conn)
		return NULL;
	ret = soph_of_find_panel_or_bridge(dev, &conn->panel, &conn->bridge);
	if (ret)
		debug("Warn: no find panel or bridge\n");

	return conn;
}

static int soph_vo_ofdata_to_platdata(struct udevice *dev)
{
	struct soph_vo_priv *priv = dev_get_priv(dev);

	priv->vo_sys_f = devfdt_get_addr_name(dev, "vo_sys_f");
	if (priv->vo_sys_f == FDT_ADDR_T_NONE) {
		printf("%s: Get VO vo_sys_f address failed (ret=%llu)\n", __func__, (u64)priv->vo_sys_f);
		return -ENXIO;
	}
	priv->vo_sys_b = devfdt_get_addr_name(dev, "vo_sys_b");
	if (priv->vo_sys_b == FDT_ADDR_T_NONE) {
		printf("%s: Get vo_sys_b address failed (ret=%llu)\n", __func__, (u64)priv->vo_sys_b);
		return -ENXIO;
	}
	priv->top_pll = devfdt_get_addr_name(dev, "top_pll");
	if (priv->top_pll == FDT_ADDR_T_NONE) {
		printf("%s: Get top_pll address failed (ret=%llu)\n", __func__, (u64)priv->top_pll);
		return -ENXIO;
	}
	debug("%s: base(vo_sys_f)=%#llx base(vo_sys_b)=%#llx base(top_pll)=%#llx\n", __func__
	     , priv->vo_sys_f, priv->vo_sys_b, priv->top_pll);
	return 0;
}

static int soph_display_probe(struct udevice *dev)
{
	struct video_uc_plat *plat = dev_get_uclass_plat(dev);
	struct soph_vo_priv *priv = dev_get_priv(dev);
	const void *blob = gd->fdt_blob;
	int phandle;
	struct udevice *crtc_dev;
	struct soph_crtc *crtc;
	struct soph_connector *conn;
	struct display_state *s;
	const char *name;
	int ret;
	ofnode node, route_node, ep_node, port_node, disp_node, port_parent_node;

	bool is_ports_node = false;

	/* Before relocation we don't need to do anything */
	if (!(gd->flags & GD_FLG_RELOC))
		return 0;

	vo_sys_f_base = priv->vo_sys_f;
	vo_sys_b_base = priv->vo_sys_b;
	top_pll_base = priv->top_pll;

	route_node = dev_read_subnode(dev, "route");
	if (!ofnode_valid(route_node))
		return -ENODEV;

	ofnode_for_each_subnode(node, route_node) {
		if (!ofnode_is_available(node))
			continue;
		phandle = ofnode_read_u32_default(node, "connector", -1);
		if (phandle < 0) {
			printf("Warn: can't find connect node's handle\n");
			continue;
		}
		ep_node.of_offset = fdt_node_offset_by_phandle(gd->fdt_blob, phandle);
		if (!ofnode_valid(np_to_ofnode(ep_node.np))) {
			printf("Warn: can't find endpoint node from phandle\n");
			continue;
		}
		port_node.of_offset = fdt_parent_offset(gd->fdt_blob,
						     ofnode_to_offset(ep_node));
		if (!ofnode_valid(np_to_ofnode(port_node.np))) {
			printf("Warn: can't find port node from phandle\n");
			continue;
		}

		port_parent_node.of_offset = fdt_parent_offset(gd->fdt_blob,
						     ofnode_to_offset(port_node));
		if (!ofnode_valid(np_to_ofnode(port_parent_node.np))) {
			printf("Warn: can't find port parent node from phandle\n");
			continue;
		}

		disp_node = port_parent_node;

		ret = uclass_get_device_by_ofnode(UCLASS_VIDEO_CRTC,
						  np_to_ofnode(disp_node.np),
						  &crtc_dev);
		if (ret) {
			printf("Warn: can't find crtc driver %d\n", ret);
			continue;
		}

		crtc = (struct soph_crtc *)dev_get_driver_data(crtc_dev);

		conn = soph_of_get_connector(np_to_ofnode(ep_node.np));
		if (!conn) {
			debug("Warn: can't get connect driver\n");
			continue;
		}

		s = malloc(sizeof(*s));
		if (!s)
			continue;

		memset(s, 0, sizeof(*s));

		INIT_LIST_HEAD(&s->head);

		ret = ofnode_read_string_index(node, "logo,uboot", 0, &name);
		if (!ret)
			memcpy(s->ulogo_name, name, strlen(name));

		ret = ofnode_read_string_index(node, "logo,mode", 0, &name);
		if (!strcmp(name, "fullscreen"))
			s->logo_mode = SOPH_DISPLAY_FULLSCREEN;
		else
			s->logo_mode = SOPH_DISPLAY_CENTER;
	#if 0
		s->force_output = ofnode_read_bool(node, "force-output");
		if (s->force_output) {
			timing_node = ofnode_find_subnode(node, "force_timing");
			ret = display_get_force_timing_from_dts(timing_node, &s->force_mode);

			if (ofnode_read_u32(node, "force-bus-format", &s->force_bus_format))
				s->force_bus_format = MEDIA_BUS_FMT_RGB888_1X24;
		}
	#endif

		s->blob = blob;
		s->conn_state.connector = conn;
		s->conn_state.secondary = NULL;
		s->conn_state.type = conn->type;

		s->crtc_state.node = np_to_ofnode(disp_node.np);
		s->crtc_state.dev = crtc_dev;
		s->crtc_state.crtc = crtc;
		s->crtc_state.crtc_id = get_crtc_id(np_to_ofnode(ep_node.np), is_ports_node);
		s->node = node;
		s->mem_base = (void *)plat->base;
		s->mem_size = plat->size;
	#if 0
		s->force_output = ofnode_read_bool(node, "force-output");
	#endif

		list_add_tail(&s->head, &soph_display_list);
	}

	if (list_empty(&soph_display_list)) {
		debug("Failed to found available display route\n");
		return -ENODEV;
	}

	display_pre_init();
	video_set_flush_dcache(dev, true);

	return 0;
}

int soph_display_bind(struct udevice *dev)
{
	struct video_uc_plat *plat = dev_get_uclass_plat(dev);

	plat->size = DRM_SOPH_FB_WIDTH * DRM_SOPH_FB_HEIGHT * 4;

	return 0;
}

static const struct udevice_id soph_display_ids[] = {
	{ .compatible = "cvitek,drm-subsystem" },
	{ }
};

U_BOOT_DRIVER(soph_display) = {
	.name	= "soph_display",
	.id	= UCLASS_VIDEO,
	.of_match = soph_display_ids,
	.bind = soph_display_bind,
	.probe = soph_display_probe,
	.of_to_plat = soph_vo_ofdata_to_platdata,
	.priv_auto	= sizeof(struct soph_vo_priv),
};

int do_soph_logo_show(struct cmd_tbl *cmdtp, int flag, int argc,
			char *const argv[])
{
	if (argc != 1)
		return CMD_RET_USAGE;

	soph_show_logo();

	return 0;
}

U_BOOT_CMD(
	soph_show_logo, 1, 1, do_soph_logo_show,
	"Load and display logo",
	NULL
);
