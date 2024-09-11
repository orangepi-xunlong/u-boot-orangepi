/*
 * drm/sunxi_drm_drv/sunxi_drm_drv.c
 *
 * Copyright (c) 2007-2024 Allwinnertech Co., Ltd.
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
#include <clk/clk.h>
#include <errno.h>
#include <log.h>
#include <reset.h>
#include <asm/io.h>
#include <dm.h>
#include <dm/ofnode.h>
#include <linux/list.h>
#include <video.h>
#include <command.h>
#include <drm/drm_modes.h>
#include <drm/drm_connector.h>
#include <drm/drm_logo.h>
#include <video_bridge.h>
#include <linux/delay.h>
#include <splash.h>
#include <asm/cache.h>
#include <lcd.h>
#include <rand.h>
#include <mapmem.h>
#include "sunxi_device/sunxi_tcon.h"
#include "sunxi_drm_panel.h"
#include "sunxi_drm_connector.h"
#include "sunxi_drm_helper_funcs.h"
#include "sunxi_drm_bridge.h"
#include "sunxi_drm_drv.h"
#include "sunxi_drm_crtc.h"

DECLARE_GLOBAL_DATA_PTR;

int check_drm_debug_mode(void)
{
	int debug_mode;

	debug_mode = env_get_hex("drm_debug", 0);

	if (debug_mode)
		printf("drm debug mode: %d \n", debug_mode);

	return debug_mode;
}
static int sunxi_de_add_kernel_iova_premap(unsigned long address, unsigned int length)
{
	int ret;
	int node_offset = fdt_path_offset(working_fdt, "/soc/de");

	ret = fdt_appendprop_u64(working_fdt, node_offset, "sunxi-iova-premap", address);
	if (ret) {
		printf("%s fail ret%d\n", __FUNCTION__, ret);
		return ret;
	}
	ret = fdt_appendprop_u64(working_fdt, node_offset, "sunxi-iova-premap", length);
	if (ret) {
		printf("%s fail ret%d\n", __FUNCTION__, ret);
	}

	return ret;
}

int sunxi_drm_kernel_para_flush(void)
{
	struct display_state *state;
	struct sunxi_drm_device *drm = sunxi_drm_device_get();
	struct drm_framebuffer *fb;
	int node = fdt_path_offset(working_fdt, "/soc/sunxi-drm");
	struct drm_display_mode *mode;
	int boot_info_node = 0;
	int boot_mode_node = 0;
	char name[16];
	int i = 0;
	int ret;

	if (!drm || check_drm_debug_mode() == SKIP_DRM_DRIVE)
		return -1;

	sunxi_drm_for_each_display(state, drm) {
		if (state->is_enable) {
			fb = drm_framebuffer_lookup(drm, state->fb_id);
			sunxi_de_add_kernel_iova_premap(fb->dma_addr, fb->buf_size);
			ret = fdt_add_mem_rsv(working_fdt, fb->dma_addr, fb->buf_size);
			printf("ret %d\n", ret);
			snprintf(name, 16, "booting-%d", i);
			boot_info_node = fdt_add_subnode(working_fdt, node, name);
			if (boot_info_node < 0) {
				DRM_ERROR("%s for fdt_add_subnode fail\n", __func__);
				return -1;
			}

			boot_mode_node = fdt_add_subnode(working_fdt, boot_info_node, "mode");
			if (boot_mode_node < 0) {
				DRM_ERROR("%s for fdt_add_subnode fail\n", __func__);
				return -1;
			}
			/*******************************************************************
			 *	sunxi-drm {
			 *		booting-x {
			 *			...
			 *			route = <>;
			 *			logo  = <>;
			 *			mode {
			 *				clock = <>;
			 *				hdisplay = <>;
			 *				vdisplay = <>;
			 *				...
			 *			}
			 *		}
			 *	}
			 ******************************************************************/
#define fdt_mode_append(name, val) \
			fdt_appendprop_u32(working_fdt, boot_mode_node, name, (uint32_t)val)

			mode = &state->conn_state.mode;
			fdt_mode_append("clock", mode->clock);
			fdt_mode_append("hdisplay", mode->hdisplay);
			fdt_mode_append("vdisplay", mode->vdisplay);
			fdt_mode_append("hsync_start", mode->hsync_start);
			fdt_mode_append("vsync_start", mode->vsync_start);
			fdt_mode_append("hsync_end", mode->hsync_end);
			fdt_mode_append("vsync_end", mode->vsync_end);
			fdt_mode_append("htotal", mode->htotal);
			fdt_mode_append("vtotal", mode->vtotal);
			fdt_mode_append("vrefresh", mode->vrefresh);
			fdt_mode_append("vscan", mode->vscan);
			fdt_mode_append("flags", mode->flags);
			fdt_mode_append("hskew", mode->hskew);
			fdt_mode_append("type",  mode->type);
			fdt_mode_append("ratio", mode->picture_aspect_ratio);
			fdt_mode_append("width-mm", mode->width_mm);
			fdt_mode_append("height-mm",  mode->height_mm);
#undef fdt_mode_append

#define fdt_append(name, val) \
			fdt_appendprop_u32(working_fdt, boot_info_node, name, (uint32_t)val)

			fdt_append("logo", fb->dma_addr);
			fdt_append("logo", fb->width);
			fdt_append("logo", fb->height);
			fdt_append("logo", fb->format->depth);

			fdt_append("route", state->crtc_state.crtc_id);
			fdt_append("route", state->crtc_state.tcon_id);
			fdt_append("route", state->crtc_state.tcon_top_id);
			fdt_append("route", state->conn_state.connector->type);
			fdt_append("route", state->conn_state.connector->id);

/* TODO add databits colorspace format...
			fdt_append("color", ); */
#undef fdt_append

			i++;
			sunxi_drm_connector_save_para(state);
		}
	}

	return 0;
}

static int display_init(struct display_state *state);
#define SUNXI_DISPLAY_MAX_WIDTH 4096
#define SUNXI_DISPLAY_MAX_HEIGHT 4096

static const struct udevice_id sunxi_of_match[] = {
	{
		.compatible = "allwinner,sunxi-drm",
	},
	{}
};


static struct udevice *sunxi_drm_of_find_device(ofnode endpoint, enum uclass_id id)
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

	ret = uclass_get_device_by_ofnode(id, conn, &dev);
	if (ret) {
		DRM_ERROR("uclass_get_device_by_ofnode fail:%s\n", ofnode_get_name(conn));
		return NULL;
	}

	return dev;
}



static int sunxi_of_find_panel(struct udevice *dev, struct sunxi_drm_panel **panel)
{
	struct device_node *ep_node, *panel_node;
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

	ep_node = sunxi_of_graph_get_remote_node(dev_ofnode(dev), PORT_DIR_OUT, 0);
	if (!ep_node)
		return -ENODEV;

	port = ofnode_get_parent(np_to_ofnode(ep_node));
	if (!ofnode_valid(port))
		return -ENODEV;

	panel_node = sunxi_of_graph_get_port_parent(port);
	if (!panel_node)
		return -ENODEV;

	ret = uclass_get_device_by_ofnode(UCLASS_PANEL, np_to_ofnode(panel_node), &panel_dev);
	if (!ret)
		goto found;

	return -ENODEV;

found:
	*panel = (struct sunxi_drm_panel *)dev_get_driver_data(panel_dev);
	return 0;
}


static int sunxi_of_find_bridge(struct udevice *dev, struct sunxi_drm_bridge **bridge)
{
/*	struct device_node *ep_node, *bridge_node;
	ofnode port;
	struct udevice *bridge_dev;
	int ret = 0;

	ep_node = sunxi_of_graph_get_remote_node(dev_ofnode(dev), PORT_DIR_OUT, 0);
	if (!ep_node)
		return -ENODEV;

	port = ofnode_get_parent(np_to_ofnode(ep_node));
	if (!ofnode_valid(port))
		return -ENODEV;

	bridge_node = sunxi_of_graph_get_port_parent(port);
	if (!bridge_node)
		return -ENODEV;

	ret = uclass_get_device_by_ofnode(UCLASS_VIDEO_BRIDGE, np_to_ofnode(bridge_node),
					  &bridge_dev);
	if (!ret)
		goto found;

	return -ENODEV;

found:
	*bridge = (struct sunxi_drm_bridge *)dev_get_driver_data(bridge_dev);
	return 0;*/
//TODO
	DRM_ERROR("NOT support bridge yet\n");
	return -ENODEV;
}

static int sunxi_of_find_panel_or_bridge(struct udevice *dev, struct sunxi_drm_panel **panel,
					    struct sunxi_drm_bridge **bridge)
{
	int ret = 0;

	if (*panel)
		return 0;

	*panel = NULL;
	*bridge = NULL;

	if (panel) {
		ret  = sunxi_of_find_panel(dev, panel);
		if (!ret)
			return 0;
	}

	if (ret) {
		ret = sunxi_of_find_bridge(dev, bridge);
		if (!ret)
			ret = sunxi_of_find_panel_or_bridge((*bridge)->dev, panel,
							       &(*bridge)->next_bridge);
	}

	return ret;
}


static struct sunxi_drm_connector *sunxi_drm_of_get_connector(ofnode endpoint)
{
	struct sunxi_drm_connector *conn;
	struct udevice *dev;
	int ret;
	struct sunxi_drm_device *drm = sunxi_drm_device_get();

	dev = sunxi_drm_of_find_device(endpoint, UCLASS_DISPLAY);
	if (!dev) {
		DRM_ERROR("Warn: can't find connect driver\n");
		return NULL;
	}

	conn = get_sunxi_drm_connector_by_device(drm, dev);
	if (!conn) {
		DRM_ERROR("get_sunxi_drm_connector_by_device fail!\n");
		return NULL;
	}

	ret = sunxi_of_find_panel_or_bridge(dev, &conn->panel, &conn->bridge);
	if (ret)
		DRM_ERROR("Warn: no find panel or bridge\n");


	return conn;
}


int sunxi_ofnode_get_display_mode(ofnode node, struct drm_display_mode *mode, u32 *bus_flags)
{
	u32 hactive, vactive, pixelclock, val;
	u32 hfront_porch, hback_porch, hsync_len;
	u32 vfront_porch, vback_porch, vsync_len;
	int flags = 0;


	ofnode_read_u32(node, "hactive", &hactive);
	ofnode_read_u32(node, "vactive", &vactive);
	ofnode_read_u32(node, "clock-frequency", &pixelclock);
	ofnode_read_u32(node, "hsync-len", &hsync_len);
	ofnode_read_u32(node, "hfront-porch", &hfront_porch);
	ofnode_read_u32(node, "hback-porch", &hback_porch);
	ofnode_read_u32(node, "vsync-len", &vsync_len);
	ofnode_read_u32(node, "vfront-porch", &vfront_porch);
	ofnode_read_u32(node, "vback-porch", &vback_porch);

	if (!ofnode_read_u32(node, "hsync-active", &val))
		flags |= val ? DISPLAY_FLAGS_HSYNC_HIGH :
				DISPLAY_FLAGS_HSYNC_LOW;
	if (!ofnode_read_u32(node, "vsync-active", &val))
		flags |= val ? DISPLAY_FLAGS_VSYNC_HIGH :
				DISPLAY_FLAGS_VSYNC_LOW;

	val = ofnode_read_bool(node, "interlaced");
	flags |= val ? DISPLAY_FLAGS_INTERLACED : 0;
	val = ofnode_read_bool(node, "doublescan");
	flags |= val ? DISPLAY_FLAGS_DOUBLESCAN : 0;
	val = ofnode_read_bool(node, "doubleclk");
	flags |= val ? DISPLAY_FLAGS_DOUBLECLK : 0;

	if (!ofnode_read_u32(node, "de-active", &val)) {
		*bus_flags |= val ? DRM_BUS_FLAG_DE_HIGH : DRM_BUS_FLAG_DE_LOW;
		flags |= val ? DISPLAY_FLAGS_DE_HIGH : DISPLAY_FLAGS_DE_LOW;
	}
	if (!ofnode_read_u32(node, "pixelclk-active", &val)) {
		*bus_flags |= val ? DRM_BUS_FLAG_PIXDATA_DRIVE_POSEDGE :
					DRM_BUS_FLAG_PIXDATA_DRIVE_NEGEDGE;
		flags |= val ? DISPLAY_FLAGS_PIXDATA_POSEDGE :
					DISPLAY_FLAGS_PIXDATA_NEGEDGE;
	}

	if (!ofnode_read_u32(node, "syncclk-active", &val))
		flags |= val ? DISPLAY_FLAGS_SYNC_POSEDGE :
				DISPLAY_FLAGS_SYNC_NEGEDGE;
	else if (flags & (DISPLAY_FLAGS_PIXDATA_POSEDGE |
			DISPLAY_FLAGS_PIXDATA_NEGEDGE))
		flags |= flags & DISPLAY_FLAGS_PIXDATA_POSEDGE ?
				DISPLAY_FLAGS_SYNC_POSEDGE :
				DISPLAY_FLAGS_SYNC_NEGEDGE;

	val = ofnode_read_bool(node, "interlaced");
	flags |= val ? DISPLAY_FLAGS_INTERLACED : 0;
	val = ofnode_read_bool(node, "doublescan");
	flags |= val ? DISPLAY_FLAGS_DOUBLESCAN : 0;
	val = ofnode_read_bool(node, "doubleclk");
	flags |= val ? DISPLAY_FLAGS_DOUBLECLK : 0;

	mode->hdisplay = hactive;
	mode->hsync_start = mode->hdisplay + hfront_porch;
	mode->hsync_end = mode->hsync_start + hsync_len;
	mode->htotal = mode->hsync_end + hback_porch;

	mode->vdisplay = vactive;
	mode->vsync_start = mode->vdisplay + vfront_porch;
	mode->vsync_end = mode->vsync_start + vsync_len;
	mode->vtotal = mode->vsync_end + vback_porch;

	mode->clock = pixelclock / 1000;

	mode->flags = 0;
	if (flags & DISPLAY_FLAGS_HSYNC_HIGH)
		mode->flags |= DRM_MODE_FLAG_PHSYNC;
	else if (flags & DISPLAY_FLAGS_HSYNC_LOW)
		mode->flags |= DRM_MODE_FLAG_NHSYNC;
	if (flags & DISPLAY_FLAGS_VSYNC_HIGH)
		mode->flags |= DRM_MODE_FLAG_PVSYNC;
	else if (flags & DISPLAY_FLAGS_VSYNC_LOW)
		mode->flags |= DRM_MODE_FLAG_NVSYNC;
	if (flags & DISPLAY_FLAGS_INTERLACED)
		mode->flags |= DRM_MODE_FLAG_INTERLACE;
	if (flags & DISPLAY_FLAGS_DOUBLESCAN)
		mode->flags |= DRM_MODE_FLAG_DBLSCAN;
	if (flags & DISPLAY_FLAGS_DOUBLECLK)
		mode->flags |= DRM_MODE_FLAG_DBLCLK;

	mode->vrefresh = drm_mode_vrefresh(mode);

	return 0;
}


int sunxi_get_baseparameter(void)
{
	return 0;
}


static int display_pre_init(void)
{
	struct display_state *state;
	int ret = 0;
	struct sunxi_drm_device *drm = sunxi_drm_device_get();

	sunxi_drm_for_each_display(state, drm) {
		ret = sunxi_drm_connector_pre_init(state);
		if (ret)
			DRM_ERROR("pre init conn error\n");
		ret = display_init(state);
		if (ret)
			DRM_ERROR("display_init fail\n");
	}


	return ret;
}

static struct sunxi_drm_device *sunxi_drm_device_init(struct udevice *dev)
{
	struct sunxi_drm_device *drm = dev_get_priv(dev);

	drm->dev = dev;

	INIT_LIST_HEAD(&drm->display_list);
	INIT_LIST_HEAD(&drm->connector_list);
	INIT_LIST_HEAD(&drm->plane_list);
	INIT_LIST_HEAD(&drm->fb_list);

	return drm;
}

struct sunxi_drm_device *sunxi_drm_device_get(void)
{
	struct udevice *dev;

	if (check_drm_debug_mode() == SKIP_DRM_DRIVE)
		return NULL;
	if (uclass_get_device_by_driver(UCLASS_VIDEO,
					DM_GET_DRIVER(sunxi_display), &dev)) {
		return NULL;
	}

	return (struct sunxi_drm_device *)dev_get_priv(dev);
}


static int get_crtc_id(ofnode connect, bool is_ports_node)
{
	ofnode port_node, remote;
	int phandle;
	int val;

	if (is_ports_node) {
		port_node = ofnode_get_parent(connect);
		if (!ofnode_valid(port_node))
			goto err;

		val = ofnode_read_u32_default(port_node, "reg", -1);
		if (val < 0)
			goto err;
	} else {
		phandle = ofnode_read_u32_default(connect, "remote-endpoint", -1);
		if (phandle < 0)
			goto err;

		remote = ofnode_get_by_phandle(phandle);
		if (!ofnode_valid(remote))
			goto err;

		val = ofnode_read_u32_default(remote, "reg", -1);
		if (val < 0)
			goto err;
	}

	return val;
err:
	printf("Can't get crtc id, default set to id = 0\n");
	return 0;
}

static int sunxi_drm_drv_probe(struct udevice *dev)
{
	struct video_priv *uc_priv = dev_get_uclass_priv(dev);
	struct video_uc_platdata *plat = dev_get_uclass_platdata(dev);
	struct udevice *crtc_dev, *tcon_dev;
	struct sunxi_drm_crtc *crtc;
	struct sunxi_drm_connector *conn;
	struct display_state *s, *tmp_s;
	struct sunxi_drm_device *drm;
	const char *name;
	size_t phandle_count = 0;
	int ret = -1, phandle[16] = {-1}, i = 0;
	ofnode node, route_node, timing_node;
	ofnode port_node[2], sunxi_node[2], ep_node[2], port_parent_node[2];
	bool is_ports_node[2] = {false, false};
	struct drm_mode_fb_cmd2 cmd2;
	struct drm_framebuffer *fb;

	memset(&cmd2, 0, sizeof(struct drm_mode_fb_cmd2));

	route_node = dev_read_subnode(dev, "route");
	if (!ofnode_valid(route_node))
		return -ENODEV;

	drm = sunxi_drm_device_init(dev);
	if (!drm) {
		return -ENODEV;
	}

	ofnode_for_each_subnode(node, route_node) {
		if (!ofnode_valid(node) || !ofnode_is_available(node))
			continue;
		//fill "connect" with de's port(out)
		phandle_count = ofnode_read_size(node, "endpoints");
		if (phandle_count <= 0) {
			return -ENODEV;
		}
		ret = ofnode_read_u32_array(node, "endpoints", (void *)phandle, phandle_count / sizeof(u32));
		phandle_count /= sizeof(u32);
		for (i = 0; i < phandle_count; ++i) {
			if (phandle[i] < 0) {
				DRM_ERROR("Invalid phandle:%d\n", i);
				break;
			}
			ep_node[i] = ofnode_get_by_phandle(phandle[i]);

			if (!ofnode_valid(ep_node[i])) {
				DRM_ERROR("Warn: can't find endpoint node from phandle:%d\n", i);
				break;
			}
			port_node[i] = ofnode_get_parent(ep_node[i]);
			if (!ofnode_valid(port_node[i])) {
				DRM_ERROR("Warn: can't find port node from phandle:%d\n", i);
				break;
			}

			port_parent_node[i] = ofnode_get_parent(port_node[i]);
			if (!ofnode_valid(port_parent_node[i])) {
				DRM_ERROR("Warn: can't find port parent node from phandle:%d\n", i);
				break;
			}

			is_ports_node[i] = strstr(ofnode_get_name(port_parent_node[i]), "ports") ? 1 : 0;
			if (is_ports_node[i]) {
				sunxi_node[i] = ofnode_get_parent(port_parent_node[i]);
				if (!ofnode_valid(sunxi_node[i])) {
					DRM_ERROR("Warn: can't find crtc node from phandle:%d\n", i);
					break;
				}
			} else {
				sunxi_node[i] = port_parent_node[i];
			}
		}

		if (i != phandle_count || phandle_count == 0) {
			pr_err("only find %d phandle, format:endpoints=<&disp0_out_tcon0 &tcon0_out_rgb0>;\n", phandle_count);
			continue;
		}

		//probe de driver
		ret = uclass_get_device_by_ofnode(UCLASS_MISC,
						  sunxi_node[0],
						  &crtc_dev);
		if (ret) {
			printf("Warn: can't find crtc driver %d\n", ret);
			continue;
		}

		if (ofnode_valid(sunxi_node[1])) {
			//probe tcon driver
			ret = uclass_get_device_by_ofnode(UCLASS_MISC,
							  sunxi_node[1],
							  &tcon_dev);
			if (ret) {
				printf("Warn: can't find tcon driver %d\n", ret);
				continue;
			}
		}


		crtc = sunxi_drm_find_crtc_by_port(crtc_dev, port_node[0]);

		conn = sunxi_drm_of_get_connector(ep_node[phandle_count - 1]);
		if (!conn) {
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

		s->force_output = ofnode_read_bool(node, "force-output");

		if (s->force_output) {
			timing_node = ofnode_find_subnode(node, "force_timing");
			ret = sunxi_ofnode_get_display_mode(timing_node,
								&s->force_mode,
								&s->conn_state.info.bus_flags);
		}

		s->conn_state.connector = conn;
		//TODO: get secondary connecotr if user enable
		s->conn_state.secondary = NULL;
		s->conn_state.online_wb_conn = get_sunxi_drm_connector_by_type(drm, DRM_MODE_CONNECTOR_VIRTUAL);
		s->conn_state.type = conn->type;
		s->conn_state.tcon_dev = tcon_dev;
		s->conn_state.overscan.left_margin = 100;
		s->conn_state.overscan.right_margin = 100;
		s->conn_state.overscan.top_margin = 100;
		s->conn_state.overscan.bottom_margin = 100;
		s->crtc_state.node = sunxi_node[0];
		s->crtc_state.dev = crtc_dev;
		s->crtc_state.crtc = crtc;
		s->crtc_state.crtc_id = get_crtc_id(ep_node[0], is_ports_node[0]);
		s->node = node;
		s->drm = drm;

		list_add_tail(&s->head, &drm->display_list);
		++drm->num_display;
	}

	if (list_empty(&drm->display_list)) {
		pr_err("Failed to found available display route\n");
		return -ENODEV;
	}
	sunxi_get_baseparameter();
	display_pre_init();

	sunxi_drm_for_each_display(tmp_s, drm) {
		cmd2.width = tmp_s->conn_state.mode.hdisplay;
		cmd2.height = tmp_s->conn_state.mode.vdisplay;
		cmd2.pixel_format = DRM_FORMAT_ARGB8888;
		cmd2.pitches[0] = cmd2.width * (ALIGN(32, 8) >> 3);
		cmd2.offsets[0] = 0;

		tmp_s->fb_id = drm_framebuffer_alloc(drm, &cmd2);

		if (tmp_s->fb_id  < 0) {
			DRM_ERROR("drm_framebuffer_alloc fail!\n");
			return -ENODEV;
		}

		fb = drm_framebuffer_lookup(drm, tmp_s->fb_id);

		if (!uc_priv->xsize || !uc_priv->ysize) {
			uc_priv->xsize = tmp_s->conn_state.mode.hdisplay;
			uc_priv->ysize = tmp_s->conn_state.mode.vdisplay;
			plat->base = fb->dma_addr;
			plat->size = fb->buf_size;
		}
	}

	if (!uc_priv->xsize || !uc_priv->ysize) {
		uc_priv->xsize = SUNXI_DISPLAY_MAX_WIDTH;
		uc_priv->ysize = SUNXI_DISPLAY_MAX_HEIGHT;
	}

	uc_priv->bpix = VIDEO_BPP32;

	/* Enable flushing if we enabled dcache */
	video_set_flush_dcache(dev, true);

	return 0;
}


int sunxi_drm_drv_bind(struct udevice *dev)
{
/*	struct video_uc_platdata *plat = dev_get_uclass_platdata(dev);*/
	/*framebuffer size*/
/*	plat->size = SUNXI_DISPLAY_MAX_WIDTH * SUNXI_DISPLAY_MAX_HEIGHT * 4;*/

	return 0;
}


U_BOOT_DRIVER(sunxi_display) = {
	.name	= "sunxi_display",
	.id	= UCLASS_VIDEO,
	.of_match = sunxi_of_match,
	.bind	= sunxi_drm_drv_bind,
	.probe	= sunxi_drm_drv_probe,
	.priv_auto_alloc_size = sizeof(struct sunxi_drm_device),
//	.flags = DM_FLAG_PRE_RELOC,
};


struct sunxi_drm_crtc *sunxi_drm_crtc_find(struct sunxi_drm_device *drm, int crtc_id)
{
	struct display_state *state;

	sunxi_drm_for_each_display(state, drm) {
		if (state->crtc_state.crtc_id == crtc_id) {
			return state->crtc_state.crtc;
		}
	}

	return NULL;
}

struct display_state *display_state_get_by_crtc(struct sunxi_drm_crtc *crtc)
{
	struct display_state *state;

	if (!crtc) {
		return NULL;
	}

	sunxi_drm_for_each_display(state, crtc->drm) {
		if (state->crtc_state.crtc == crtc) {
			return state;
		}
	}
	return NULL;
}

int sunxi_de_print_state(const char *cmd)
{
	struct display_state *state;
	struct crtc_state *crtc_state;
	struct sunxi_drm_crtc *crtc;
	const struct sunxi_drm_crtc_funcs *crtc_funcs;
	int ret = -EINVAL;
	struct sunxi_drm_device *drm = sunxi_drm_device_get();
	struct drm_plane *plane;
	const struct drm_plane_funcs *plane_func;


	if (!drm || list_empty(&drm->display_list)) {
		DRM_ERROR("Get sunxi drm device fail!\n");
		return -1;
	}

	drm_for_each_plane(plane, drm) {
		plane_func = plane->funcs;
		if (plane_func->print_state) {
			plane_func->print_state(plane, NULL);
		}

	}

	sunxi_drm_for_each_display(state, drm) {
		if (!state->is_init)
			continue;
		crtc_state = &state->crtc_state;
		crtc = crtc_state->crtc;
		crtc_funcs = crtc->funcs;
		if (crtc_funcs->regs_dump) {
			ret = crtc_funcs->regs_dump(state);
		}
		if (crtc_funcs->print_state) {
			ret = crtc_funcs->print_state(state, NULL);
		}
		sunxi_connector_print_state(state);
	}

	if (ret)
		ret = CMD_RET_USAGE;

	return ret;
}



static int load_bmp_logo(struct display_state *state, char *bmp_name)
{
	if (state->logo) {
		state->logo->unload_file(state->logo);
		state->logo = NULL;
	}

	state->logo = load_file(bmp_name, "bootloader");
	if (!state->logo) {
		state->logo = load_file(bmp_name, "boot-resource");
	}

	if (!state->logo) {
		DRM_ERROR("Can not load %s file!\n", bmp_name);
		return -1;
	}

	/*DRM_INFO("Load file :%s size:%u addr:0x%lx\n", bmp_name, state->logo->file_size, (ulong)state->logo->file_addr);*/

	return 0;
}

static int display_set_plane(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	struct connector_state *conn_state = &state->conn_state;
	struct drm_mode_set_plane plane_req;
	struct drm_display_mode *mode = &conn_state->mode;

	if (!state->is_init)
		return -EINVAL;

	memset(&plane_req, 0, sizeof(struct drm_mode_set_plane));

	plane_req.crtc_id = crtc_state->crtc_id;

	plane_req.fb_id = state->fb_id;
	if (plane_req.fb_id < 0) {
		pr_err("Fail to drm_framebuffer_alloc:%d\n", plane_req.fb_id);
		return -ENOMEM;
	}

	plane_req.plane_id = drm_get_primary_plane_id(state->drm, plane_req.crtc_id);
	plane_req.crtc_x = 0;
	plane_req.crtc_y = 0;
	plane_req.crtc_w = mode->hdisplay;
	plane_req.crtc_h = mode->vdisplay;
	plane_req.src_x = 0;
	plane_req.src_y = 0;
	plane_req.src_w = mode->hdisplay << 16;
	plane_req.src_h = mode->vdisplay << 16;

	return drm_mode_setplane(state->drm, &plane_req);

}

static int display_enable(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	const struct sunxi_drm_crtc *crtc = crtc_state->crtc;
	const struct sunxi_drm_crtc_funcs *crtc_funcs = crtc->funcs;
	/*struct drm_framebuffer *fb = NULL;*/
	/*struct video_priv *uc_priv = dev_get_uclass_priv(state->drm->dev);*/

	if (!state->is_init)
		return -EINVAL;

	if (state->is_enable)
		return 0;

	if (crtc_funcs->prepare)
		crtc_funcs->prepare(state);

	sunxi_drm_connector_pre_enable(state);

	if (crtc_funcs->enable)
		crtc_funcs->enable(state);

	display_set_plane(state);

	sunxi_drm_connector_enable(state);

	if (crtc_funcs->enable_vblank) {
		crtc_funcs->enable_vblank(state);
	}

	/*fb = drm_framebuffer_lookup(state->drm, state->fb_id);*/

	/*if (fb) {*/
		/*memcpy((void *)fb->dma_addr, state->logo->file_addr, fb->buf_size);*/
		/*flush_dcache_range((ulong)fb->dma_addr,*/
				   /*ALIGN((ulong)(fb->dma_addr + fb->buf_size),*/
					 /*CONFIG_SYS_CACHELINE_SIZE));*/
	/*}*/


	state->is_enable = true;


	return 0;
}

static int display_check(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	struct sunxi_drm_connector *conn = conn_state->connector;
	const struct sunxi_drm_connector_funcs *conn_funcs = conn->funcs;
	struct crtc_state *crtc_state = &state->crtc_state;
	const struct sunxi_drm_crtc *crtc = crtc_state->crtc;
	const struct sunxi_drm_crtc_funcs *crtc_funcs = crtc->funcs;
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



static int display_disable(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	const struct sunxi_drm_crtc *crtc = crtc_state->crtc;
	const struct sunxi_drm_crtc_funcs *crtc_funcs = crtc->funcs;

	if (!state->is_init)
		return 0;

	if (!state->is_enable)
		return 0;

	if (crtc_funcs->disable_vblank) {
		crtc_funcs->disable_vblank(state);
	}

	sunxi_drm_connector_disable(state);

	if (crtc_funcs->disable)
		crtc_funcs->disable(state);

	sunxi_drm_connector_post_disable(state);

	state->is_enable = false;
	state->is_init = false;

	return 0;
}

static int display_get_timing_from_dts(struct sunxi_drm_panel *panel,
				       struct drm_display_mode *mode,
				       u32 *bus_flags)
{
	struct ofnode_phandle_args args;
	ofnode dt, timing;
	int ret;

	dt = dev_read_subnode(panel->dev, "display-timings");
	if (ofnode_valid(dt)) {
		ret = ofnode_parse_phandle_with_args(dt, "native-mode", NULL,
						     0, 0, &args);
		if (ret)
			return ret;

		timing = args.node;
	} else {
		timing = dev_read_subnode(panel->dev, "panel-timing");
	}

	if (!ofnode_valid(timing)) {
		DRM_ERROR("failed to get display timings from DT\n");
		return -ENXIO;
	}
	sunxi_ofnode_get_display_mode(timing, mode, bus_flags);

	return 0;
}

static int display_get_timing(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	struct drm_display_mode *mode = &conn_state->mode;
	const struct drm_display_mode *m;
	struct sunxi_drm_panel *panel = conn_state->connector->panel;

	if (panel->funcs->get_mode)
		return panel->funcs->get_mode(panel, mode);


	if ((ofnode_valid(dev_ofnode(panel->dev))) &&
	    !display_get_timing_from_dts(panel, mode, &conn_state->info.bus_flags)) {
		DRM_ERROR("Using display timing dts\n");
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

static int display_mode_fixup(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	const struct sunxi_drm_crtc *crtc = crtc_state->crtc;
	const struct sunxi_drm_crtc_funcs *crtc_funcs = crtc->funcs;
	int ret;

	if (crtc_funcs->mode_fixup) {
		ret = crtc_funcs->mode_fixup(state);
		if (ret)
			return ret;
	}

	return 0;
}


static int display_mode_valid(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	struct sunxi_drm_connector *conn = conn_state->connector;
	const struct sunxi_drm_connector_funcs *conn_funcs = conn->funcs;
	struct crtc_state *crtc_state = &state->crtc_state;
	const struct sunxi_drm_crtc *crtc = crtc_state->crtc;
	const struct sunxi_drm_crtc_funcs *crtc_funcs = crtc->funcs;
	int ret;

	if (conn_funcs->mode_valid) {
		ret = conn_funcs->mode_valid(conn, state);
		if (ret)
			return ret;
	}

	if (crtc_funcs->mode_valid) {
		ret = crtc_funcs->mode_valid(state);
		if (ret)
			return ret;
	}

	return 0;
}

static int display_init(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	struct sunxi_drm_connector *conn = conn_state->connector;
	struct crtc_state *crtc_state = &state->crtc_state;
	struct sunxi_drm_crtc *crtc = crtc_state->crtc;
	const struct sunxi_drm_crtc_funcs *crtc_funcs = crtc->funcs;
	struct drm_display_mode *mode = &conn_state->mode;
	int ret = 0;
	struct drm_plane *plane;

	if (state->is_init)
		return 0;

	if (!crtc_funcs) {
		printf("failed to find crtc functions\n");
		return -ENXIO;
	}

	if (crtc_state->crtc->active && !crtc_state->ports_node &&
	    memcmp(&crtc_state->crtc->active_mode, &conn_state->mode,
		   sizeof(struct drm_display_mode))) {
		DRM_ERROR("%s has been used for output type: %d, mode: %dx%dp%d\n",
			crtc_state->dev->name,
			crtc_state->crtc->active_mode.type,
			crtc_state->crtc->active_mode.hdisplay,
			crtc_state->crtc->active_mode.vdisplay,
			crtc_state->crtc->active_mode.vrefresh);
		return -ENODEV;
	}


	ret = sunxi_drm_connector_init(state);
	if (ret)
		goto deinit;

	ret = sunxi_drm_connector_detect(state);

	if (!ret && !state->force_output)
		goto deinit;

	ret = 0;
	if (conn->panel) {
		sunxi_drm_panel_prepare(conn->panel);
		ret = display_get_timing(state);
		if (!ret)
			conn_state->info.bpc = conn->panel->bpc;
		if (ret < 0 && conn->funcs->get_edid_timing) {
			ret = conn->funcs->get_edid_timing(conn, state);
		}
	} else if (conn->bridge) {
/*		ret = video_bridge_read_edid(conn->bridge->dev,
					     conn_state->edid, EDID_SIZE);
		if (ret > 0) {
			display_get_edid_mode(state);
		} else if (conn->bridge->funcs->get_mode) {
			conn->bridge->funcs->get_mode(conn->bridge, &conn_state->mode);
		}*/
		DRM_ERROR("NOT support bridge yet\n");
	} else if (conn->funcs->get_timing) {
		ret = conn->funcs->get_timing(conn, state);
	} else if (conn->funcs->get_edid_timing) {
		ret = conn->funcs->get_edid_timing(conn, state);
	}

	if (!ret && conn_state->secondary) {
		struct sunxi_drm_connector *connector = conn_state->secondary;

		if (connector->panel) {
			if (connector->panel->funcs->get_mode) {
				struct drm_display_mode *_mode = drm_mode_create();

				ret = connector->panel->funcs->get_mode(connector->panel, _mode);
				if (!ret && !drm_mode_equal(_mode, mode))
					ret = -EINVAL;

				drm_mode_destroy(_mode);
			}
		}
	}

	if (ret && !state->force_output)
		goto deinit;
	//TODO:
	/*if (state->force_output)*/
		/*display_use_force_mode(state);*/

	if (display_mode_valid(state))
		goto deinit;

	printf("%s: %s detailed mode clock %u kHz, flags[%x]\n"
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
	       conn_state->info.bus_formats[0]);

	if (display_mode_fixup(state))
		goto deinit;

	if (conn->bridge)
		sunxi_drm_bridge_mode_set(conn->bridge, &conn_state->mode);

	/*if (crtc_funcs->init) {*/
		/*ret = crtc_funcs->init(state);*/
		/*if (ret)*/
			/*goto deinit;*/
	/*}*/
	state->is_init = true;

	drm_for_each_plane(plane, state->drm)
		if (plane->funcs->reset)
			plane->funcs->reset(plane);

	crtc_state->crtc->active = true;
	memcpy(&crtc_state->crtc->active_mode,
	       &conn_state->mode, sizeof(struct drm_display_mode));

	return 0;

deinit:
	sunxi_drm_connector_deinit(state);
	return ret;
}

static int display_logo(struct display_state *state)
{
	struct sunxi_drm_device *drm = state->drm;
	struct video_priv *priv = dev_get_uclass_priv(drm->dev);
	struct video_uc_platdata *plat = dev_get_uclass_platdata(drm->dev);
	struct bmp_image *bmp = NULL;
	struct drm_framebuffer *fb = NULL;
	int ret = 0, left_offset = 0, upper_offset = 0;

	if (!state->is_init)
		return -ENODEV;

	ret = display_check(state);
	if (ret) {
		DRM_ERROR("dislay check fail!\n");
		return ret;
	}

	if (!state->logo) {
		DRM_ERROR("Logo not found!\n");
		return -EINVAL;
	}

	fb = drm_framebuffer_lookup(state->drm, state->fb_id);

	if (fb) {
		memset((void *)fb->dma_addr, 0, fb->buf_size);
		flush_dcache_range((ulong)fb->dma_addr,
				   ALIGN((ulong)(fb->dma_addr + fb->buf_size),
					 CONFIG_SYS_CACHELINE_SIZE));
	}

	// FIXME: dual display, modify it if some new demands need later
	if (plat->base != fb->dma_addr) {
		plat->base = fb->dma_addr;
		plat->size = fb->buf_size;
		priv->xsize = state->conn_state.mode.hdisplay;
		priv->ysize = state->conn_state.mode.vdisplay;
		priv->fb = map_sysmem(plat->base, plat->size);
		priv->line_length = priv->xsize * VNBYTES(priv->bpix);
		priv->fb_size = priv->line_length * priv->ysize;
	}

	bmp = map_sysmem((ulong)state->logo->file_addr, 0);
	if (fb->width > bmp->header.width)
		left_offset = ((fb->width - bmp->header.width) >> 1);
	if (fb->height > bmp->header.height)
		upper_offset = ((fb->height - bmp->header.height) >> 1);

	bmp_display((ulong)state->logo->file_addr, left_offset, upper_offset);

	ret = display_enable(state);
	if (ret) {
		DRM_ERROR("dislay enable fail!\n");
		return ret;
	}

	return ret;
}

int sunxi_show_bmp_and_backlight(char *bmp, char *reg)
{
	struct display_state *s;
	int ret = 0;
	struct sunxi_drm_device *drm = sunxi_drm_device_get();

	if (!drm || list_empty(&drm->display_list)) {
		DRM_ERROR("Get sunxi drm device fail!\n");
		return -1;
	}

	if (!bmp) {
		sunxi_drm_for_each_display(s, drm) {
			display_disable(s);
		}
		return -ENOENT;
	}

	sunxi_drm_for_each_display(s, drm) {
		/*s->logo.mode = s->charge_logo_mode;*/
		if (load_bmp_logo(s, bmp))
			continue;
		if (!strcmp(reg, "on"))
			s->backlight = true;
		else if (!strcmp(reg, "off"))
			s->backlight = false;
		else
			return -1;
		ret = display_logo(s);
	}
	return ret;
}

int sunxi_drm_disable(void)
{
	struct display_state *state;
	struct sunxi_drm_device *drm = sunxi_drm_device_get();

	DRM_INFO("%s:%d\n", __func__, __LINE__);
	if (!drm || list_empty(&drm->display_list)) {
		DRM_ERROR("%s:Get sunxi drm device fail!\n", __func__);
		return -1;
	}

	sunxi_drm_for_each_display(state, drm) {
		if (state->is_enable)
			sunxi_drm_panel_post_disable(state);
	}

	return 0;
}

int sunxi_show_bmp(char *bmp)
{
	struct display_state *s;
	int ret = 0;
	struct sunxi_drm_device *drm = sunxi_drm_device_get();

	if (!drm || list_empty(&drm->display_list)) {
		DRM_ERROR("Get sunxi drm device fail!\n");
		return -1;
	}

	if (!bmp) {
		sunxi_drm_for_each_display(s, drm) {
			display_disable(s);
		}
		return -ENOENT;
	}

	sunxi_drm_for_each_display(s, drm) {
		/*s->logo.mode = s->charge_logo_mode;*/
		if (load_bmp_logo(s, bmp))
			continue;
		s->backlight = true;
		ret = display_logo(s);
	}
	return ret;
}

int sunxi_bmp_display(char *name)
{
	return sunxi_show_bmp(name);
}
int sunxi_backlight_ctrl(char *reg)
{
	struct display_state *state = NULL;
	int i = 0;
	struct sunxi_drm_device *drm = sunxi_drm_device_get();
	bool flag;

	if (!drm || list_empty(&drm->display_list)) {
		DRM_ERROR("Get sunxi drm device fail!\n");
		return -1;
	}

	if (!strcmp(reg, "on"))
		flag = true;
	else if (!strcmp(reg, "off"))
		flag = false;
	else
		return -1;

	sunxi_drm_for_each_display(state, drm) {
		if (state->is_enable)
			sunxi_drm_connector_backlight(state, flag);
		i++;
	}

	return 0;
}

int sunxi_show_logo(void)
{
	struct display_state *s = NULL;
	int ret = 0, i = 0;
	/*int count = 0;*/
	struct sunxi_drm_device *drm = sunxi_drm_device_get();

	if (!drm || list_empty(&drm->display_list)) {
		DRM_ERROR("Get sunxi drm device fail!\n");
		return -1;
	}

	sunxi_drm_for_each_display(s, drm) {
		/*s->logo.mode = s->logo_mode;*/
		if (load_bmp_logo(s, s->ulogo_name)) {
			DRM_ERROR("load logo fail\n");
		} else {
			s->backlight = true;
			ret = display_logo(s);
			if (ret)
				DRM_ERROR("failed to display uboot logo for disp %d\n");
		}
		i++;
	}

	return ret;
}

struct drm_framebuffer *drm_fb_lock(void)
{
	struct sunxi_drm_device *drm = sunxi_drm_device_get();
	struct drm_framebuffer *fb = NULL;
	struct display_state *state = NULL;
	int ret;

	if (!drm || list_empty(&drm->display_list)) {
		DRM_ERROR("Get sunxi drm device fail!\n");
		return NULL;
	}

	sunxi_drm_for_each_display(state, drm) {
		ret = display_check(state);
		if (ret) {
			DRM_ERROR("display check fail!\n");
			continue;
		}
		state->backlight = true;
		ret = display_enable(state);
		if (ret) {
			DRM_ERROR("display enable fail!\n");
			return NULL;
		}
		if (state->is_enable) {
			fb = drm_framebuffer_lookup(drm, state->fb_id);
			if (fb)
				return fb;
		}
	}
	return NULL;
}

static int do_sunxi_logo_show(struct cmd_tbl_s *cmdtp, int flag, int argc,
			char *const argv[])
{
	if (argc != 1)
		return CMD_RET_USAGE;

	sunxi_show_logo();

	return 0;
}

static int do_sunxi_backlight_ctrl(struct cmd_tbl_s *cmdtp, int flag, int argc,
				char *const argv[])
{
	if (argc != 2)
		return CMD_RET_USAGE;

	sunxi_backlight_ctrl(argv[1]);

	return 0;
}
static int do_sunxi_show_bmp(struct cmd_tbl_s *cmdtp, int flag, int argc,
				char *const argv[])
{
	if (argc != 2)
		return CMD_RET_USAGE;

	sunxi_show_bmp(argv[1]);

	return 0;
}

static int sunxi_de_online_wb(void)
{
	struct display_state *state;
	struct sunxi_drm_device *drm = sunxi_drm_device_get();
	int wb_fb_id = 0, ret = -1;
	const struct sunxi_drm_connector_funcs *conn_funcs;
	struct drm_framebuffer *fb;
	struct drm_mode_fb_cmd2 cmd2;
	char name[80] = {0}, part_name[20] = {0};

	if (!drm || list_empty(&drm->display_list)) {
		DRM_ERROR("Get sunxi drm device fail!\n");
		return -1;
	}
	memset(&cmd2, 0, sizeof(struct drm_mode_fb_cmd2));

	sunxi_drm_for_each_display(state, drm) {
		if (!state->conn_state.online_wb_conn || !state->is_enable) {
			continue;
		}
		conn_funcs = state->conn_state.online_wb_conn->funcs;
		if (conn_funcs->check) {
			ret = conn_funcs->check(state->conn_state.online_wb_conn, state);
			if (ret) {
				pr_err("wb check fail!:%d\n", ret);
				continue;
			}
		}
		cmd2.width = state->conn_state.mode.hdisplay;
		cmd2.height = state->conn_state.mode.vdisplay;
		cmd2.pixel_format = DRM_FORMAT_ARGB8888;
		cmd2.pitches[0] = cmd2.width * (ALIGN(32, 8) >> 3);
		cmd2.offsets[0] = 0;
		wb_fb_id = drm_framebuffer_alloc(drm, &cmd2);
		if (wb_fb_id < 0) {
			DRM_ERROR("drm_framebuffer_alloc fail!\n");
			continue;
		}
		sunxi_drm_set_wb_fb_id(state, wb_fb_id);
		fb = drm_framebuffer_lookup(drm, wb_fb_id);
		if (fb) {
			snprintf(name, 80, "conn%d_%ux%u_%d.rgb", state->conn_state.type, cmd2.width, cmd2.height, rand());
			snprintf(part_name, 20, "boot-resource");
			ret = write_file(name, part_name, (void *)fb->dma_addr, fb->buf_size);
			if (ret) {
				snprintf(part_name, 20, "bootloader");
				ret = write_file(name, part_name, (void *)fb->dma_addr, fb->buf_size);
			}
			DRM_WARN("Write %s to %s %s!\n", name, part_name,
				 (ret) ? "fail" : "successfully");
		}
		sunxi_drm_set_wb_fb_id(state, -1);
	}

	return 0;
}

static int do_sunxi_sunxi_drm(struct cmd_tbl_s *cmdtp, int flag, int argc,
				char *const argv[])
{
	int ret = CMD_RET_USAGE;
	struct display_state *state;
	struct sunxi_drm_device *drm = sunxi_drm_device_get();
	int pattern = 1;


	if (!drm || list_empty(&drm->display_list)) {
		DRM_ERROR("Get sunxi drm device fail!\n");
		return -1;
	}

	if (!strcmp(argv[1], "dump")) {
		ret = sunxi_de_print_state(argv[1]);
	} else if (!strcmp(argv[1], "wb")) {
		ret = sunxi_de_online_wb();
	} else if (!strcmp(argv[1], "colorbar")) {
		if (argc == 3) {
			pattern = simple_strtol(argv[2], NULL, 10);
		}
		sunxi_drm_for_each_display(state, drm) {
			if (state->conn_state.tcon_dev) {
				ret = sunxi_tcon_show_pattern(state->conn_state.tcon_dev, pattern);
			}
		}

	}


	return ret;
}

U_BOOT_CMD(
	sunxi_show_logo, 1, 1, do_sunxi_logo_show,
	"load and display logo from resource partition",
	NULL
);

U_BOOT_CMD(
	sunxi_show_bmp, 2, 1, do_sunxi_show_bmp,
	"load and display bmp from resource partition",
	"    <bmp_name>"
);

U_BOOT_CMD(
	sunxi_backlihgt, 2, 1, do_sunxi_backlight_ctrl,
	"backlight ctrl",
	"    off or on"
);

static char sunxi_drm_help_text[] =
	"sunxi_drm dump - Print state and reg value\n"
	"sunxi_drm colorbar [type] - Set clock frequency\n"
	"sunxi_drm wb - Online Writeback image to boot-resource partition\n";

U_BOOT_CMD(
	sunxi_drm, 3, 1, do_sunxi_sunxi_drm,
	"sunxi drm debug cmd",
	sunxi_drm_help_text
);


//End of File
