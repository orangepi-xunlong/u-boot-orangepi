#ifndef __SOPH_VO_H__
#define __SOPH_VO_H__

#include <bmp_layout.h>
#include <dm/ofnode.h>
#include <edid.h>
#include <command.h>

enum display_mode {
	SOPH_DISPLAY_FULLSCREEN,
	SOPH_DISPLAY_CENTER,
};

struct disp_rect {
	u16 x;
	u16 y;
	u16 w;
	u16 h;
};

struct panel_state {
	// struct soph_panel *panel;
	ofnode dsp_lut_node;
};

struct overscan {
	int left_margin;
	int right_margin;
	int top_margin;
	int bottom_margin;
};

struct connector_state {
	struct soph_connector *connector;
	struct soph_connector *secondary;

	struct drm_display_mode mode;
	struct overscan overscan;
	u8 edid[EDID_SIZE * 4];
	int bus_format;
	int output_mode;
	int type;
	int output_if;
	int output_flags;
	int color_space;
	unsigned int bpc;

	struct {
		u32 *lut;
		int size;
	} gamma;
};

struct crtc_state {
	struct udevice *dev;
	struct soph_crtc *crtc;
	void *private;
	ofnode node;
	struct device_node *ports_node;
	int crtc_id;

	int format;
	u64 dma_addr;
	int ymirror;
	int rb_swap;
	int xvir;
	int post_csc_mode;
	int dclk_core_div;
	int dclk_out_div;
	struct disp_rect src_rect;
	struct disp_rect crtc_rect;
	struct disp_rect right_src_rect;
	struct disp_rect right_crtc_rect;
	bool yuv_overlay;
	bool post_r2y_en;
	bool post_y2r_en;

	u32 feature;
	struct disp_rect max_output;
};

struct logo_info {
	int mode;
	void *mem;
	bool ymirror;
	u32 offset;
	u32 width;
	int height;
	u32 bpp;
};

struct soph_logo_cache {
	struct list_head head;
	char name[20];
	struct logo_info logo;
};

struct display_state {
	struct list_head head;

	const void *blob;
	ofnode node;

	struct crtc_state crtc_state;
	struct connector_state conn_state;
	struct panel_state panel_state;

	char ulogo_name[30];
	char klogo_name[30];

	struct logo_info logo;
	int logo_mode;
	int charge_logo_mode;
	void *mem_base;
	int mem_size;

	int enable;
	int is_init;
	int is_enable;
	bool force_output;
	struct drm_display_mode force_mode;
	u32 force_bus_format;
};

int drm_mode_vrefresh(const struct drm_display_mode *mode);

bool drm_mode_is_420(const struct drm_display_info *display,
		     struct drm_display_mode *mode);
void drm_mode_max_resolution_filter(struct hdmi_edid_data *edid_data,
				    struct disp_rect *max_output);
int soph_ofnode_get_display_mode(ofnode node, struct drm_display_mode *mode);

int do_soph_show_bmp(struct cmd_tbl *cmdtp, int flag, int argc,
				char *const argv[]);

#endif /* __SOPH_VO_H__ */
