#include <common.h>
#include <malloc.h>
#include <boot_gui.h>
#include "canvas_utils.h"


#ifdef SURPORT_DRAW_CHARS

#include <video_font.h>

static inline void draw_char_base_checked(char *addr,
	argb_t *color, int bpp, int stride, char ch)
{
	char *line_b = addr;
	char *line_e = addr + stride * VIDEO_FONT_HEIGHT;
	char *cdata = (char *)video_fontdata + ch * VIDEO_FONT_HEIGHT;
	int i = 0;
	unsigned char bits = 0;

	if (32 == bpp) {
		for (; line_b < line_e; line_b += stride) {
			bits = *cdata++;
			addr = line_b;
			for (i = 0; i < VIDEO_FONT_WIDTH; ++i) {
				if ((bits << i) & 0x80)
					*(int *)addr = *(int *)color;
				addr += 4;
			}
		}
	} else if (24 == bpp) {
		for (; line_b < line_e; line_b += stride) {
			bits = *cdata++;
			addr = line_b;
			for (i = 0; i < VIDEO_FONT_WIDTH; ++i) {
				if ((bits << i) & 0x80) {
					*addr++ = color->blue;
					*addr++ = color->green;
					*addr++ = color->red;
				} else {
					addr += 3;
				}
			}
		}
	} else {
		printf("%s no support bpp%d\n", __func__, bpp);
	}
}

/* not auto switch to next line when drawing to the end of line */
static int draw_chars(struct canvas *cv, argb_t *color, point_t *coords,
	char *chs, unsigned int count)
{
	unsigned int font_bytes = 0;
	char *p = NULL;
	char *p_e = NULL;

	if (check_coords(cv, coords)
		|| (coords->x + VIDEO_FONT_WIDTH > cv->width)
		|| (coords->y + VIDEO_FONT_HEIGHT > cv->height)) {
		printf("out of range. x=%d,y=%d, font:w=%d,h=%d, cv:w=%d,h=%d\n",
			coords->x, coords->y, VIDEO_FONT_WIDTH,
			VIDEO_FONT_HEIGHT, cv->width, cv->height);
		return -1;
	}

	if (coords->x + count * VIDEO_FONT_WIDTH > cv->width) {
		count = (cv->width - coords->x) / VIDEO_FONT_WIDTH;
	}

	font_bytes = cv->bpp * VIDEO_FONT_WIDTH >> 3;
	p = (char *)cv->base + cv->stride * coords->y
		+ (cv->bpp * coords->x >> 3);
	p_e = p + font_bytes * count;
	for (; p < p_e; p += font_bytes) {
		draw_char_base_checked(p, color, cv->bpp, cv->stride, *chs);
		++chs;
	}

	return 0;
}

#endif /* #ifdef SURPORT_DRAW_CHARS */

#ifdef SUPORT_DRAW_GEOMETRY

static inline int get_ceil_int(double x)
{
	/* should call ceil(), not this func. */
	double ceil = 1.0 + (int)x;
	return (int)(ceil - (int)(ceil - x));
}

static inline void memset32(int *p, int v, unsigned int count)
{
	while (count--)
		*(p++) = v;
}

static inline char check_coords(struct canvas *cv, point_t *const coords)
{
	return ((coords->x < 0)
		|| (coords->x >= cv->width)
		|| (coords->y < 0)
		|| (coords->y >= cv->height));
}

static inline char check_rect(struct canvas *cv, rect_t *const rect)
{
	return ((rect->left < 0)
		|| (rect->top < 0)
		|| (rect->right > cv->width)
		|| (rect->bottom > cv->height)
		|| (rect->left >= rect->right)
		|| (rect->top >= rect->bottom));
}

/* draw horizen line */
static void draw_line_base_checked(char *addr,
	argb_t *color, int bpp, unsigned int pixel_num)
{
	if (32 == bpp) {
		memset32((int *)addr, *(int *)color, pixel_num);
	} else if (24 == bpp) {
		for (; 0 != pixel_num; --pixel_num) {
			*addr++ = color->blue;
			*addr++ = color->green;
			*addr++ = color->red;
		}
	} else {
		printf("%s: no support the bpp[%d]\n", __func__, bpp);
	}
}

/* draw a line, the points of from and to would be drawn */
static void draw_line_checked(struct canvas *cv, argb_t *color, point_t *from, point_t *to)
{
	double pixel_per_row, pixels_sum_next;
	int pixels_sum;
	int line_stride, row_num;
	char *addr;

	/* draw line from left to right. */
	if (from->x > to->x)
		return draw_line_checked(cv, color, to, from);

	addr = (char *)cv->base + cv->stride * from->y
			+ (cv->bpp * from->x >> 3);

	if (from->y == to->y) {
		return draw_line_base_checked(addr, color, cv->bpp, to->x - from->x + 1);
	}

	if (to->y > from->y) {
		line_stride = cv->stride;
		row_num = 1 + to->y - from->y;
	} else {
		line_stride = -cv->stride;
		row_num = 1 + from->y - to->y;
	}

	if (from->x == to->x) {
		for (; 0 < row_num; --row_num) {
			draw_line_base_checked(addr, color, cv->bpp, 1);
			addr += line_stride;
		}
		return;
	}

	pixel_per_row =  (1.0 + to->x - from->x) / row_num;
	pixels_sum = 0;
	pixels_sum_next = pixel_per_row;
	for (; 1 < row_num; --row_num) {
		draw_line_base_checked(addr, color, cv->bpp,
			get_ceil_int(pixels_sum_next - pixels_sum));
		addr += (line_stride + (cv->bpp
			* ((int)(pixels_sum_next) - pixels_sum) >> 3));
		pixels_sum = (int)pixels_sum_next;
		pixels_sum_next += pixel_per_row;
	}
	draw_line_base_checked(addr, color, cv->bpp,
		1 + to->x - from->x - pixels_sum);
}

/* draw_rect/fill_rect: not include the line and row of right_bottom */
static void draw_rect_checked(struct canvas *cv, argb_t *color, rect_t *rect)
{
	point_t from = {rect->left, rect->top};
	point_t to = {rect->right - 1, rect->top};
	draw_line_checked(cv, color, &from, &to);

	from.y = rect->bottom - 1;
	to.y = rect->bottom - 1;
	draw_line_checked(cv, color, &from, &to);

	from.y = rect->top + 1;
	to.x = rect->left;
	to.y -= 1;
	draw_line_checked(cv, color, &from, &to);

	from.x = rect->right - 1;
	to.x = rect->right - 1;
	draw_line_checked(cv, color, &from, &to);
}

static void fill_rect_checked(struct canvas *cv, argb_t *color, rect_t *rect)
{
	char *p = (char *)(cv->base + cv->stride * rect->top
		+ (rect->left * cv->bpp >> 3));

	if (32 == cv->bpp) {
		char *p_e = p + cv->stride * (rect->bottom - rect->top);
		unsigned int count = rect->right - rect->left;
		for (; p != p_e; p += cv->stride)
			memset32((int *)p, *(int *)color, count);
	} else if (24 == cv->bpp) {
		char *p_e = p + cv->stride * (rect->bottom - rect->top);
		int fill_bytes = cv->bpp * (rect->right - rect->left) >> 3;
		char *line_b = NULL;
		char *line_e = NULL;
		for (; p != p_e; p += cv->stride) {
			line_b = p;
			line_e = line_b + fill_bytes;
			for (; line_b != line_e;) {
				*line_b++ = color->blue;
				*line_b++ = color->green;
				*line_b++ = color->red;
			}
		}
	} else {
		printf("%s: no support bpp[%d]\n", __func__, cv->bpp);
	}
}

static int draw_point(struct canvas *cv, argb_t *color, point_t *coords)
{
	char *p = NULL;

	if (check_coords(cv, coords)) {
		printf("%s input params out of range\n", __func__);
		return -1;
	}

	p = (char *)cv->base + coords->y * cv->stride
		+ (coords->x * cv->bpp >> 3);
	draw_line_base_checked(p, color, cv->bpp, 1);
	return 0;
}

/* draw a line, the points of from and to would be drawn */
static int draw_line(struct canvas *cv, argb_t *color, point_t *from, point_t *to)
{
	if (check_coords(cv, from) || check_coords(cv, to)) {
		printf("%s input params out of range\n", __func__);
		return -1;
	}
	draw_line_checked(cv, color, from, to);
	return 0;
}

/* draw_rect/fill_rect: not include the line and row of right_bottom */
static int draw_rect(struct canvas *cv, argb_t *color, rect_t *rect)
{
	if (check_rect(cv, rect)) {
		printf("%s input params out of range\n", __func__);
		return -1;
	}
	draw_rect_checked(cv, color, rect);
	return 0;
}

static int fill_rect(struct canvas *cv, argb_t *color, rect_t *rect)
{
	if (check_rect(cv, rect)) {
		printf("%s input params out of range\n", __func__);
		return -1;
	}
	fill_rect_checked(cv, color, rect);
	return 0;
}

static int copy_block(struct canvas *cv, point_t *src, point_t *dst,
	unsigned int width, unsigned int height)
{
	char *src_addr = NULL;
	char *dst_addr = NULL;
	char *dst_addr_e = NULL;
	unsigned int cp_bytes = 0;

	if (check_coords(cv, src) || check_coords(cv, dst)
		|| (0 == width)	|| (0 == height)
		|| (src->x + width  > cv->width)
		|| (src->y + height > cv->height)
		|| (dst->x + width > cv->width)
		|| (dst->y + height > cv->height)) {
		printf("%s input params out of range\n", __func__);
		return -1;
	}

	src_addr = (char *)cv->base + cv->stride * src->y
		+ (cv->bpp * src->x >> 3);
	dst_addr = (char *)cv->base + cv->stride * dst->y
		+ (cv->bpp * dst->x >> 3);
	dst_addr_e = dst_addr + cv->stride * height;
	cp_bytes = cv->bpp * width >> 3;
	for (; dst_addr < dst_addr_e; dst_addr += cv->stride) {
		memcpy((void *)dst_addr, (void *)src_addr, cp_bytes);
		src_addr += cv->stride;
	}

	return 0;
}

#endif /* #ifdef SUPORT_DRAW_GEOMETRY */

static int do_nothing(void)
{
	return 0;
}

int get_canvas_utils(struct canvas *cv)
{
#ifdef SUPORT_DRAW_GEOMETRY
	cv->draw_point = draw_point;
	cv->draw_line = draw_line;
	cv->draw_rect = draw_rect;
	cv->fill_rect = fill_rect;
	cv->copy_block = copy_block;
#else
	cv->draw_point = (void *)do_nothing;
	cv->draw_line = (void *)do_nothing;
	cv->draw_rect = (void *)do_nothing;
	cv->fill_rect = (void *)do_nothing;
	cv->copy_block = (void *)do_nothing;
#endif

#ifdef SURPORT_DRAW_CHARS
	cv->draw_chars = draw_chars;
#else
	cv->draw_chars = (void *)do_nothing;
#endif
	return 0;
}
