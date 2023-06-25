/*
 * Allwinner SoCs eink driver.
 *
 * Copyright (C) 2019 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 *
 *	File name   :       sunxi_eink.h
 *
 *	Description :       eink engine 2.0  struct declaration
 *
 *	History     :       2019/03/20 liuli   initial version
 *
 */
#ifndef _SUNXI_EINK_H_
#define _SUNXI_EINK_H_
#include <sunxi_display2.h>
#include <asm/arch/timer.h>

enum EINK_CMD {
	EINK_UPDATE_IMG,
	EINK_WRITE_BACK_IMG,
	EINK_SET_TEMP,
	EINK_GET_TEMP,
	EINK_SET_GC_CNT,
	EINK_WAIT_PIPE_FINISH,
};

enum upd_mode {
	EINK_INIT_MODE = 0x01,
	EINK_DU_MODE = 0x02,
	EINK_GC16_MODE = 0x04,
	EINK_GC4_MODE = 0x08,
	EINK_A2_MODE = 0x10,
	EINK_GL16_MODE = 0x20,
	EINK_GLR16_MODE = 0x40,
	EINK_GLD16_MODE = 0x80,
	EINK_GU16_MODE	= 0x84,

	/* use self upd win not de*/
	EINK_RECT_MODE  = 0x400,
	/* AUTO MODE: auto select update mode by E-ink driver */
	EINK_AUTO_MODE = 0x8000,

/*	EINK_NO_MERGE = 0x80000000,*/
};

enum dither_mode {
	QUANTIZATION,
	FLOYD_STEINBERG,
	ATKINSON,
	ORDERED,
	SIERRA_LITE,
	BURKES,
};

struct upd_win {
	u32 left;
	u32 top;
	u32 right;
	u32 bottom;
};

enum upd_pixel_fmt {
	EINK_RGB888 = 0x0,
	EINK_Y8 = 0x09,
	EINK_Y5 = 0x0a,
	EINK_Y4 = 0x0b,
	EINK_Y3 = 0x0e,
};

enum buf_state {
	FREE = 0x0,
	CAN_USED = 0x1,
	USED = 0x2,
};

struct upd_pic_size {
	u32 width;
	u32 height;
	u32 align;
};

struct eink_img {
	void			*vaddr;
	void			*paddr;
	u32			pitch;
	bool                    force_fresh;
	bool			win_calc_en;
	bool			de_bypass_flag;
	bool			win_calc_fin;
	bool			mode_select_fin;
	bool			upd_all_en;
	enum upd_mode           upd_mode;
	struct upd_win		upd_win;
	struct upd_pic_size	size;
	enum upd_pixel_fmt      out_fmt;
	enum dither_mode        dither_mode;
	enum buf_state		state;
	unsigned int		*eink_hist;
};

struct eink_upd_cfg {
	struct upd_win		upd_win;
	enum upd_mode		upd_mode;
	enum dither_mode	dither_mode;
	enum upd_pixel_fmt	out_fmt;
	bool			force_fresh;
	bool			upd_all_en;
	bool			de_bypass;
};

struct eink_fb_info_t;
struct disp_layer_config;

struct raw_rgb_t {
	unsigned int width;
	unsigned int height;
	unsigned int bpp;//bit per pixel
	unsigned int stride;
	unsigned int file_size;
	void *addr;
};
/**
 * eink fb info
 */
struct eink_fb_info_t {
	struct raw_rgb_t *p_rgb;
	char *last_buf_addr;
	char *gray_buffer_addr;
	struct eink_img cur_img;
	struct eink_img last_img;
	struct disp_layer_config configs;
	int (*de_format_convert)(struct eink_fb_info_t *p_info);
	int (*eink_update)(struct eink_fb_info_t *p_info);
	int (*eink_display)(struct eink_fb_info_t *p_info);
	int (*set_update_mode)(struct eink_fb_info_t *p_info, enum upd_mode update_mode);
	int (*update_all_en)(struct eink_fb_info_t *p_info, int en);
	void (*wait_pipe_finish)(struct eink_fb_info_t *p_info);
	struct raw_rgb_t *(*get_rgb_info)(struct eink_fb_info_t *p_info);
};

int eink_framebuffer_init(void);

int eink_framebuffer_exit(void);

struct eink_fb_info_t *eink_get_fb_inst(void);
#endif
