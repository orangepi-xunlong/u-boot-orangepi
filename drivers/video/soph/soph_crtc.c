#include <clk.h>
#include <config.h>
#include <common.h>
#include <errno.h>
#include <malloc.h>
#include <asm/unaligned.h>
#include <linux/list.h>
#include <dm/device.h>
#include <dm.h>
#include <dm/device-internal.h>
#include <dm/lists.h>
#include <dm/uclass-id.h>
#include <drm_modes.h>
#include "soph_vo_sys_reg.h"
#include "soph_crtc.h"
#include "soph_connector.h"

DECLARE_GLOBAL_DATA_PTR;

#define DEFINE_CSC_COEF0(a, b, c) \
		.coef[0][0] = a, .coef[0][1] = b, .coef[0][2] = c,
#define DEFINE_CSC_COEF1(a, b, c) \
		.coef[1][0] = a, .coef[1][1] = b, .coef[1][2] = c,
#define DEFINE_CSC_COEF2(a, b, c) \
		.coef[2][0] = a, .coef[2][1] = b, .coef[2][2] = c,


static struct disp_csc_matrix csc_mtrx[DISP_CSC_MAX] = {
	// none
	{
		DEFINE_CSC_COEF0(BIT(10),	0,		0)
		DEFINE_CSC_COEF1(0,		BIT(10),	0)
		DEFINE_CSC_COEF2(0,		0,		BIT(10))
		.sub[0] = 0,   .sub[1] = 0,   .sub[2] = 0,
		.add[0] = 0,   .add[1] = 0,   .add[2] = 0
	},
	// yuv2rgb
	// 601 Limited
	//  R = Y + 1.402* Pr                           //
	//  G = Y - 0.344 * Pb  - 0.792* Pr             //
	//  B = Y + 1.772 * Pb                          //
	{
		DEFINE_CSC_COEF0(BIT(10),	0,		1436)
		DEFINE_CSC_COEF1(BIT(10),	BIT(13) | 352,	BIT(13) | 731)
		DEFINE_CSC_COEF2(BIT(10),	1815,		0)
		.sub[0] = 0,   .sub[1] = 128, .sub[2] = 128,
		.add[0] = 0,   .add[1] = 0,   .add[2] = 0
	},
	// 601 Full
	//  R = 1.164 *(Y - 16) + 1.596 *(Cr - 128)                     //
	//  G = 1.164 *(Y - 16) - 0.392 *(Cb - 128) - 0.812 *(Cr - 128) //
	//  B = 1.164 *(Y - 16) + 2.016 *(Cb - 128)                     //
	{
		DEFINE_CSC_COEF0(1192,	0,		1634)
		DEFINE_CSC_COEF1(1192,	BIT(13) | 401,	BIT(13) | 833)
		DEFINE_CSC_COEF2(1192,	2065,		0)
		.sub[0] = 16,  .sub[1] = 128, .sub[2] = 128,
		.add[0] = 0,   .add[1] = 0,   .add[2] = 0
	},
	// 709 Limited
	// R = Y + 1.540(Cr – 128)
	// G = Y - 0.183(Cb – 128) – 0.459(Cr – 128)
	// B = Y + 1.816(Cb – 128)
	{
		DEFINE_CSC_COEF0(BIT(10),	0,		1577)
		DEFINE_CSC_COEF1(BIT(10),	BIT(13) | 187,	BIT(13) | 470)
		DEFINE_CSC_COEF2(BIT(10),	1860,		0)
		.sub[0] = 0,   .sub[1] = 128, .sub[2] = 128,
		.add[0] = 0,   .add[1] = 0,   .add[2] = 0
	},
	// 709 Full
	//  R = 1.164 *(Y - 16) + 1.792 *(Cr - 128)                     //
	//  G = 1.164 *(Y - 16) - 0.213 *(Cb - 128) - 0.534 *(Cr - 128) //
	//  B = 1.164 *(Y - 16) + 2.114 *(Cb - 128)                     //
	{
		DEFINE_CSC_COEF0(1192,	0,		1836)
		DEFINE_CSC_COEF1(1192,	BIT(13) | 218,	BIT(13) | 547)
		DEFINE_CSC_COEF2(1192,	2166,		0)
		.sub[0] = 16,  .sub[1] = 128, .sub[2] = 128,
		.add[0] = 0,   .add[1] = 0,   .add[2] = 0
	},
	// rgb2yuv
	// 601 Limited
	//  Y = 0.299 * R + 0.587 * G + 0.114 * B       //
	// Pb =-0.169 * R - 0.331 * G + 0.500 * B       //
	// Pr = 0.500 * R - 0.419 * G - 0.081 * B       //
	{
		DEFINE_CSC_COEF0(306,		601,		117)
		DEFINE_CSC_COEF1(BIT(13)|173,	BIT(13)|339,	512)
		DEFINE_CSC_COEF2(512,		BIT(13)|429,	BIT(13)|83)
		.sub[0] = 0,   .sub[1] = 0,   .sub[2] = 0,
		.add[0] = 0,   .add[1] = 128, .add[2] = 128
	},
	// 601 Full
	//  Y = 16  + 0.257 * R + 0.504 * g + 0.098 * b //
	// Cb = 128 - 0.148 * R - 0.291 * g + 0.439 * b //
	// Cr = 128 + 0.439 * R - 0.368 * g - 0.071 * b //
	{
		DEFINE_CSC_COEF0(263,		516,		100)
		DEFINE_CSC_COEF1(BIT(13)|152,	BIT(13)|298,	450)
		DEFINE_CSC_COEF2(450,		BIT(13)|377,	BIT(13)|73)
		.sub[0] = 0,   .sub[1] = 0,   .sub[2] = 0,
		.add[0] = 16,  .add[1] = 128, .add[2] = 128
	},
	// 709 Limited
	//   Y =       0.2126   0.7152   0.0722
	//  Cb = 128 - 0.1146  -0.3854   0.5000
	//  Cr = 128 + 0.5000  -0.4542  -0.0468
	{
		DEFINE_CSC_COEF0(218,		732,		74)
		DEFINE_CSC_COEF1(BIT(13)|117,	BIT(13)|395,	512)
		DEFINE_CSC_COEF2(512,		BIT(13)|465,	BIT(13)|48)
		.sub[0] = 0,   .sub[1] = 0,   .sub[2] = 0,
		.add[0] = 0,   .add[1] = 128, .add[2] = 128
	},
	// 709 Full
	//  Y = 16  + 0.183 * R + 0.614 * g + 0.062 * b //
	// Cb = 128 - 0.101 * R - 0.339 * g + 0.439 * b //
	// Cr = 128 + 0.439 * R - 0.399 * g - 0.040 * b //
	{
		DEFINE_CSC_COEF0(187,		629,		63)
		DEFINE_CSC_COEF1(BIT(13)|103,	BIT(13)|347,	450)
		DEFINE_CSC_COEF2(450,		BIT(13)|408,	BIT(13)|41)
		.sub[0] = 0,   .sub[1] = 0,   .sub[2] = 0,
		.add[0] = 16,  .add[1] = 128, .add[2] = 128
	},
};

static void get_buffer_config(DISP_FORMAT_E pixel_format, u64 pAddr, struct disp_cfg *cfg)
{
	u8  bit_width = 8;
	u8  plane_num = 0;
	u32 align_height = 0;
	u32 main_stride = 0;
	u32 c_stride = 0;
	u32 main_size = 0;
	u32 y_size = 0;
	u32 c_size = 0;
	u32 align = DEFAULT_ALIGN;
	u32 height = cfg->mem.height;
	u32 width = cfg->mem.width;

	if ((pixel_format == DISP_FORMAT_YUV_PLANAR_420)
	 || (pixel_format == DISP_FORMAT_NV12)
	 || (pixel_format == DISP_FORMAT_NV21)) {
		align_height = ALIGN(height, 2);
	} else
		align_height = height;

	main_stride = ALIGN((width * bit_width + 7) >> 3, align);
	y_size = main_stride * align_height;

	if (pixel_format == DISP_FORMAT_YUV_PLANAR_420) {
		c_stride = ALIGN(((width >> 1) * bit_width + 7) >> 3, align);
		c_size = (c_stride * align_height) >> 1;

		main_stride = c_stride * 2;
		y_size = main_stride * align_height;
		main_size = y_size + (c_size << 1);
		plane_num = 3;
	} else if (pixel_format == DISP_FORMAT_YUV_PLANAR_422) {
		c_stride = ALIGN(((width >> 1) * bit_width + 7) >> 3, align);
		c_size = c_stride * align_height;

		main_size = y_size + (c_size << 1);
		plane_num = 3;
	} else if (pixel_format == DISP_FORMAT_NV12 || pixel_format == DISP_FORMAT_NV21) {
		c_stride = ALIGN((width * bit_width + 7) >> 3, align);
		c_size = (c_stride * align_height) >> 1;

		main_size = y_size + c_size;
		plane_num = 2;
	} else if (pixel_format == DISP_FORMAT_NV16 || pixel_format == DISP_FORMAT_NV61) {
		c_stride = ALIGN((width * bit_width + 7) >> 3, align);
		c_size = c_stride * align_height;

		main_size = y_size + c_size;
		plane_num = 2;
	} else if (pixel_format == DISP_FORMAT_YUYV || pixel_format == DISP_FORMAT_YVYU ||
			pixel_format == DISP_FORMAT_UYVY || pixel_format == DISP_FORMAT_VYUY) {
		main_stride = ALIGN(((width * bit_width + 7) >> 3) * 2, align);
		y_size = main_stride * align_height;
		main_size = y_size;
		plane_num = 1;
	} else {
		// packed format
		main_stride = ALIGN(((width * bit_width + 7) >> 3) * 3, align);
		y_size = main_stride * align_height;
		main_size = y_size;
		plane_num = 1;
	}

	cfg->mem.pitch_y = main_stride;
	cfg->mem.pitch_c = main_stride;
	debug("pitch_y:%d, pitch_c:%d, ysize:%d, mainsize:%d\n", main_stride, main_stride, y_size, main_size);
	for (int i = 0; i < plane_num; i++) {
		if (i == 0)
			cfg->mem.addr0 = pAddr;
		else if (i == 1)
			cfg->mem.addr1 = (pAddr + y_size);
		else
			cfg->mem.addr2 = (pAddr + main_size);
	}

	debug("cfg->mem.addr0:%llu, cfg->mem.addr1:%llu, cfg->mem.addr2:%llu\n", cfg->mem.addr0, cfg->mem.addr1, cfg->mem.addr2);
}

static void disp_reg_shadow_sel(u8 disp_id, bool read_shadow)
{
	_reg_write_mask(REG_DISP_CFG(disp_id), BIT(18),
			(read_shadow ? 0x0 : BIT(18)));
}

static void disp_reg_set_shadow_mask(struct soph_crtc *crtc, bool shadow_mask)
{
	if (shadow_mask)
		spin_lock(&crtc->disp_mask_spinlock);

	_reg_write_mask(REG_DISP_CFG(crtc->disp_id), BIT(17),
			(shadow_mask ? BIT(17) : 0x0));

	if (!shadow_mask)
		spin_unlock(&crtc->disp_mask_spinlock);
}

void disp_set_window_bgcolor(u8 disp_id, u16 r, u16 g, u16 b)
{
	_reg_write(REG_DISP_PAT_COLOR3(disp_id), g << 16 | r);
	_reg_write_mask(REG_DISP_PAT_COLOR4(disp_id), 0x0fff, b);
}

void disp_enable_window_bgcolor(u8 disp_id, bool enable)
{
	_reg_write_mask(REG_DISP_PAT_CFG(disp_id), 0x20, enable ? 0x20 : 0);
}

static bool disp_tgen_enable(u8 disp_id, bool enable)
{
	bool is_enable = (_reg_read(REG_DISP_CFG(disp_id)) & 0x80);

	if (is_enable != enable) {
		_reg_write_mask(REG_DISP_CFG(disp_id), 0x0080,
				enable ? 0x80 : 0x00);
	}

	is_enable = (_reg_read(REG_DISP_CFG(disp_id)) & 0x80);

	return is_enable;
}

static void disp_set_addr(struct soph_crtc *crtc, u64 addr0, u64 addr1, u64 addr2)
{
	disp_reg_set_shadow_mask(crtc, true);

	_reg_write(REG_DISP_ADDR0_L(crtc->disp_id), addr0);
	_reg_write(REG_DISP_ADDR0_H(crtc->disp_id), addr0 >> 32);
	_reg_write(REG_DISP_ADDR1_L(crtc->disp_id), addr1);
	_reg_write(REG_DISP_ADDR1_H(crtc->disp_id), addr1 >> 32);
	_reg_write(REG_DISP_ADDR2_L(crtc->disp_id), addr2);
	_reg_write(REG_DISP_ADDR2_H(crtc->disp_id), addr2 >> 32);

	disp_reg_set_shadow_mask(crtc, false);
}

static void disp_set_csc(u8 disp_id, struct disp_csc_matrix *cfg)
{
	_reg_write(REG_DISP_IN_CSC0(disp_id), BIT(31) |
		   (cfg->coef[0][1] << 16) | (cfg->coef[0][0]));
	_reg_write(REG_DISP_IN_CSC1(disp_id),
		   (cfg->coef[1][0] << 16) | (cfg->coef[0][2]));
	_reg_write(REG_DISP_IN_CSC2(disp_id),
		   (cfg->coef[1][2] << 16) | (cfg->coef[1][1]));
	_reg_write(REG_DISP_IN_CSC3(disp_id),
		   (cfg->coef[2][1] << 16) | (cfg->coef[2][0]));
	_reg_write(REG_DISP_IN_CSC4(disp_id), (cfg->coef[2][2]));
	_reg_write(REG_DISP_IN_CSC_SUB(disp_id),
		   (cfg->sub[2] << 16) | (cfg->sub[1] << 8) |
		   cfg->sub[0]);
	_reg_write(REG_DISP_IN_CSC_ADD(disp_id),
		   (cfg->add[2] << 16) | (cfg->add[1] << 8) |
		   cfg->add[0]);
}

static void disp_set_in_csc(struct soph_crtc *crtc, enum disp_csc csc)
{
	if (csc == DISP_CSC_NONE)
		_reg_write(REG_DISP_IN_CSC0(crtc->disp_id), 0);
	else if (csc < DISP_CSC_MAX)
		disp_set_csc(crtc->disp_id, &csc_mtrx[csc]);

	crtc->disp_cfg.in_csc = csc;
}

static void drm_timing_to_cvi_timing(u8 disp_id, struct disp_timing *timing, struct drm_display_mode *mode)
{
	u32 hfp, hbp, hsw, vfp, vbp, vsw;

	timing->width = mode->hdisplay;
	timing->height = mode->vdisplay;

	hfp = mode->hsync_start - mode->hdisplay;
	hbp = mode->htotal - mode->hsync_end;
	hsw = mode->hsync_end - mode->hsync_start;
	vfp = mode->vsync_start - mode->vdisplay;
	vbp = mode->vtotal - mode->vsync_end;
	vsw = mode->vsync_end - mode->vsync_start;

	//cvitek regs
	timing->vtotal = mode->vtotal - 1;
	timing->htotal = mode->htotal - 1;
	timing->vsync_start = 0;
	timing->vsync_end = timing->vsync_start + vsw - 1;
	timing->vfde_start = timing->vmde_start = timing->vsync_start + vsw + vbp;
	timing->vfde_end = timing->vmde_end = timing->vfde_start + timing->height - 1;
	timing->hsync_start = 0;
	timing->hsync_end = timing->hsync_start + hsw - 1;
	timing->hfde_start = timing->hmde_start = timing->hsync_start + hsw + hbp;
	timing->hfde_end = timing->hmde_end = timing->hfde_start + timing->width - 1;
}

static void disp_set_mode_timing(struct crtc_state *crtc_state,
			     struct drm_display_mode *adj_mode)
{
	struct soph_crtc *crtc = crtc_state->crtc;
	u8 disp_id = crtc->disp_id;
	u32 tmp = 0;

	drm_timing_to_cvi_timing(crtc->disp_id, &crtc->disp_timing, adj_mode);

	disp_tgen_enable(disp_id, false);

	if (adj_mode->flags & DRM_MODE_FLAG_NVSYNC)
		tmp |= 0x20;
	if (adj_mode->flags & DRM_MODE_FLAG_NHSYNC)
		tmp |= 0x40;

	tmp |= (crtc_state->format << 12);

	disp_reg_shadow_sel(crtc->disp_id, false);
	disp_reg_set_shadow_mask(crtc, true);

	_reg_write_mask(REG_DISP_CFG(disp_id), 0xf060, tmp);
	_reg_write(REG_DISP_TOTAL(disp_id),
		   (crtc->disp_timing.htotal << 16) | crtc->disp_timing.vtotal);
	_reg_write(REG_DISP_VSYNC(disp_id),
		   (crtc->disp_timing.vsync_end << 16) | crtc->disp_timing.vsync_start);
	_reg_write(REG_DISP_VFDE(disp_id),
		   (crtc->disp_timing.vfde_end << 16) | crtc->disp_timing.vfde_start);
	_reg_write(REG_DISP_VMDE(disp_id),
		   (crtc->disp_timing.vmde_end << 16) | crtc->disp_timing.vmde_start);
	_reg_write(REG_DISP_HSYNC(disp_id),
		   (crtc->disp_timing.hsync_end << 16) | crtc->disp_timing.hsync_start);
	_reg_write(REG_DISP_HFDE(disp_id),
		   (crtc->disp_timing.hfde_end << 16) | crtc->disp_timing.hfde_start);
	_reg_write(REG_DISP_HMDE(disp_id),
		   (crtc->disp_timing.hmde_end << 16) | crtc->disp_timing.hmde_start);

	disp_reg_set_shadow_mask(crtc, false);

	disp_tgen_enable(disp_id, true);

}

static int disp_set_rect(struct soph_crtc *crtc, struct disp_rect rect)
{
	if ((rect.y > crtc->disp_timing.vfde_end) ||
	    (rect.x > crtc->disp_timing.hfde_end) ||
	    ((crtc->disp_timing.vfde_start + rect.y + rect.h - 1) >
	      crtc->disp_timing.vfde_end) ||
	    ((crtc->disp_timing.hfde_start + rect.x + rect.w - 1) >
	      crtc->disp_timing.hfde_end)) {
		printf("[drm][disp] %s: me's pos(%d, %d) size(%d, %d) ",
		       __func__, rect.x, rect.y, rect.w, rect.h);
		printf(" out of range(%d, %d).\n",
			crtc->disp_timing.hfde_end, crtc->disp_timing.vfde_end);
		return -EINVAL;
	}

	crtc->disp_timing.vmde_start = rect.y + crtc->disp_timing.vfde_start;
	crtc->disp_timing.hmde_start = rect.x + crtc->disp_timing.hfde_start;
	crtc->disp_timing.vmde_end = crtc->disp_timing.vmde_start + rect.h - 1;
	crtc->disp_timing.hmde_end = crtc->disp_timing.hmde_start + rect.w - 1;

	disp_reg_set_shadow_mask(crtc, true);

	_reg_write(REG_DISP_HMDE(crtc->disp_id),
		   (crtc->disp_timing.hmde_end << 16) | crtc->disp_timing.hmde_start);
	_reg_write(REG_DISP_VMDE(crtc->disp_id),
		   (crtc->disp_timing.vmde_end << 16) | crtc->disp_timing.vmde_start);

	disp_reg_set_shadow_mask(crtc, false);
	return 0;
}

static void disp_set_mem(struct soph_crtc *crtc, struct disp_mem *mem)
{
	enum drm_intf intf;

	disp_reg_set_shadow_mask(crtc, true);

	_reg_write(REG_DISP_OFFSET(crtc->disp_id),
		   (mem->start_y << 16) | mem->start_x);
	_reg_write(REG_DISP_SIZE(crtc->disp_id),
		   ((mem->height - 1) << 16) | (mem->width - 1));
	_reg_write_mask(REG_DISP_PITCH_Y(crtc->disp_id), 0x00ffffff,
			mem->pitch_y);
	_reg_write(REG_DISP_PITCH_C(crtc->disp_id), mem->pitch_c);

	disp_reg_set_shadow_mask(crtc, false);

	intf = crtc->disp_id ? DRM_INTF_DISP1 : DRM_INTF_DISP0;
#if 0
	extend_axi_to_36bit(mem->addr0 >> 32, intf);
#endif
	disp_set_addr(crtc, mem->addr0, mem->addr1, mem->addr2);
}

// To avoid bw fail
static void disp_set_bw_cfg(u32 fmt, u32 disp_id)
{
	if (fmt == DISP_FORMAT_RGB_888 || fmt == DISP_FORMAT_BGR_888) {
		_reg_write(REG_DISP_LINE_BUFFER(disp_id), 0x1);
		_reg_write(REG_DISP_RD_TH(disp_id), 0x1);
		_reg_write(REG_DISP_FIFO(disp_id), 0x78);
	} else {
		_reg_write(REG_DISP_LINE_BUFFER(disp_id), 0x0);
		_reg_write(REG_DISP_RD_TH(disp_id), 0x0);
		if (fmt == DISP_FORMAT_YUV_PLANAR_420 || fmt == DISP_FORMAT_YUV_PLANAR_422)
			_reg_write(REG_DISP_FIFO(disp_id), 0x4400480);
		else
			_reg_write(REG_DISP_FIFO(disp_id), 0x4800480);
	}

	_reg_write_mask(REG_DISP_PITCH_Y(disp_id), 0xff000000, 0xff << 24);
}

static int soph_disp_preinit(struct display_state *state)
{
	// const struct disp_data *disp_data = state->crtc_state.crtc->data;
	// state->crtc_state.max_output = disp_data->max_output;

	return 0;
}

static int soph_disp_init(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	struct connector_state *conn_state = &state->conn_state;
	struct drm_display_mode *mode = &conn_state->mode;
	struct soph_crtc *crtc = crtc_state->crtc;
	struct disp_rect rect;

	u32 fmt = crtc_state->format = DISP_FORMAT_RGB_888;
	u8 disp_id = crtc->disp_id = crtc_state->crtc_id;

	disp_set_mode_timing(crtc_state, mode);

	/*rgb->bgr*/
	_reg_write(REG_DISP_SWAP_RGB(disp_id), 0x5);
	/*set fde window color to white*/
	_reg_write_mask(REG_DISP_PAT_COLOR1(disp_id), 0x3ff0000, 0x3ff0000);
	_reg_write_mask(REG_DISP_PAT_COLOR2(disp_id), 0x3ff03ff, 0x3ff03ff);

	//csc
	switch (fmt) {
	case DISP_FORMAT_YUV_PLANAR_420:
	case DISP_FORMAT_YUV_PLANAR_422:
	case DISP_FORMAT_NV21:
	case DISP_FORMAT_NV12:
	case DISP_FORMAT_NV61:
	case DISP_FORMAT_NV16:
	case DISP_FORMAT_UYVY:
	case DISP_FORMAT_VYUY:
	case DISP_FORMAT_YUYV:
	case DISP_FORMAT_YVYU:
		disp_set_in_csc(crtc, DISP_CSC_601_LIMIT_YUV2RGB);
		break;
	case DISP_FORMAT_RGB_888:
	case DISP_FORMAT_BGR_888:
		disp_set_in_csc(crtc, DISP_CSC_NONE);
		break;
	default:
		break;
	}

	disp_set_bw_cfg(DISP_FORMAT_RGB_888, disp_id);

	rect.x = (mode->hdisplay / 2) - (crtc_state->src_rect.w / 2);
	rect.y = (mode->vdisplay / 2) - (crtc_state->src_rect.h / 2);
	rect.w = crtc_state->src_rect.w;
	rect.h = crtc_state->src_rect.h;
	disp_set_rect(crtc, rect);

	crtc->disp_cfg.mem.start_x = crtc_state->crtc_rect.x;
	crtc->disp_cfg.mem.start_y = crtc_state->crtc_rect.y;
	crtc->disp_cfg.mem.width = crtc_state->src_rect.w;
	crtc->disp_cfg.mem.height = crtc_state->src_rect.h;
	get_buffer_config(fmt, crtc_state->dma_addr, &crtc->disp_cfg);

	disp_set_mem(crtc, &crtc->disp_cfg.mem);

	return 0;
}

static int soph_disp_set_plane(struct display_state *state)
{
	return 0;
}

static int soph_disp_prepare(struct display_state *state)
{
	return 0;
}

static int soph_disp_enable(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	bool is_enable;

	disp_enable_window_bgcolor(crtc_state->crtc_id, false);

	is_enable = disp_tgen_enable(crtc_state->crtc_id, true);
	while (is_enable != true)
		is_enable = disp_tgen_enable(crtc_state->crtc_id, true);

	return 0;
}

static int soph_disp_disable(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	bool is_enable;

	is_enable = disp_tgen_enable(crtc_state->crtc_id, false);
	while (is_enable != false)
		is_enable = disp_tgen_enable(crtc_state->crtc_id, false);

	return 0;
}

static int soph_disp_mode_valid(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	struct drm_display_mode *mode = &conn_state->mode;
	struct videomode vm;

	drm_display_mode_to_videomode(mode, &vm);

	if (vm.hactive < 32 || vm.vactive < 32 ||
	    (vm.hfront_porch * vm.hsync_len * vm.hback_porch *
	     vm.vfront_porch * vm.vsync_len * vm.vback_porch == 0)) {
		printf("ERROR: unsupported display timing\n");
		return -EINVAL;
	}

	return 0;
}

static int soph_disp_plane_check(struct display_state *state)
{
	return 0;
}

const struct soph_crtc_funcs soph_disp_funcs = {
	.preinit = soph_disp_preinit,
	.init = soph_disp_init,
	.set_plane = soph_disp_set_plane,
	.prepare = soph_disp_prepare,
	.enable = soph_disp_enable,
	.disable = soph_disp_disable,
	.mode_valid = soph_disp_mode_valid,
	.plane_check = soph_disp_plane_check,
};

static int soph_disp_probe(struct udevice *dev)
{
	struct udevice *child;
	int ret;

	for (device_find_first_child(dev, &child);
	     child;
	     device_find_next_child(&child)) {
		ret = device_probe(child);
		if (ret)
			return ret;
	}

	return 0;
}

static int soph_disp_bind(struct udevice *dev)
{
	ofnode ports, node;
	int ret;

	ports = dev_read_subnode(dev, "port");
	if (!ofnode_valid(ports))
		return 0;

	ofnode_for_each_subnode(node, ports) {
		const char *name = ofnode_get_name(node);

		ret = device_bind_driver_to_node(dev, "soph_disp", name,
						 node, NULL);
		if (ret) {
			printf("unable to bind disp device node: %d\n", ret);
			return ret;
		}
	}

	return 0;
}

static const struct soph_crtc soph_disp0_data = {
	.funcs = &soph_disp_funcs,
};

static const struct soph_crtc soph_disp1_data = {
	.funcs = &soph_disp_funcs,
};

static const struct udevice_id soph_disp_ids[] = {
	{  .compatible = "cvitek,cv186x_disp0",
	   .data = (ulong)&soph_disp0_data,
	},
	{  .compatible = "cvitek,cv186x_disp1",
	   .data = (ulong)&soph_disp1_data,
	}, { }
};

U_BOOT_DRIVER(soph_disp) = {
	.name	= "soph_disp",
	.id	= UCLASS_VIDEO_CRTC,
	.of_match = soph_disp_ids,
	.bind	= soph_disp_bind,
	.probe	= soph_disp_probe,
};

UCLASS_DRIVER(soph_crtc) = {
	.id		= UCLASS_VIDEO_CRTC,
	.name		= "CRTC",
};
