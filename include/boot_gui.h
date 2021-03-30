#ifndef __BOOT_GUI_H__
#define __BOOT_GUI_H__


enum {
	FB_ID_0 = 0,
	FB_ID_1,
	FB_ID_2,
	FB_ID_3,
	FB_ID_NUM, /* excuse me: it is not the number of framebuffer */
	FB_ID_INVALID,
};

typedef struct color {
	unsigned char blue;
	unsigned char green;
	unsigned char red;
	unsigned char alpha;
} argb_t;

typedef struct point {
	int x;
	int y;
} point_t;

typedef struct rect {
	int left;
	int top;
	int right;
	int bottom;
} rect_t;

struct canvas {
	unsigned char *base;    /* read only */

	char *pixel_format_name; /* read only */
	argb_t color;           /* default color */

	int width;              /* read only */
	int height;             /* read only */
	int bpp;                /* read only */
	int stride;             /* read only */

	int (*draw_point)(struct canvas *cv, argb_t *color, point_t *coords);

	/* draw a line, the points of from and to would be drawn */
	int (*draw_line)(struct canvas *cv, argb_t *color, point_t *from, point_t *to);

	/* draw_rect/fill_rect: not include the line and row of right_bottom */
	int (*draw_rect)(struct canvas *cv, argb_t *color, rect_t *rect);
	int (*fill_rect)(struct canvas *cv, argb_t *color, rect_t *rect);

	int (*copy_block)(struct canvas *cv, point_t *src, point_t *dst,
		unsigned int width, unsigned int height);

	/* not auto switch to next line when drawing to the end of line */
	int (*draw_chars)(struct canvas *cv, argb_t *color, point_t *coords,
		char *chs, unsigned int count);

	/* only deal the interest region in spec case:
	 * such as scn_win of interset region is interge, smooth boot
	*/
	int (*set_interest_region)(struct canvas *cv, rect_t *rects, unsigned int count,
		argb_t *uninterest_region_color);

	void *this_fb; /* prohibit read&write */
};

#ifdef CONFIG_BOOT_GUI

extern int disp_devices_open(void);

extern int fb_init(void);
extern int fb_quit(void);
extern struct canvas *fb_lock(const unsigned int fb_id);
extern int fb_unlock(unsigned int fb_id, rect_t *dirty_rects, int count);

extern int save_disp_cmd(void);

#endif /* #ifdef CONFIG_BOOT_GUI */

#endif /* __BOOT_GUI_H__ */
