/*
 * Allwinner SoCs bootGUI.
 *
 * Copyright (C) 2017 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <common.h>
#include <command.h>
#include <boot_gui.h>

#define THIS_USAGE "accept 2 args:\n" \
	"boot_gui_test 1 : get fb informations.\n" \
	"boot_gui_test 2 : fb_lock and fb_unlock, draw fb0 directly.\n" \
	"boot_gui_test 3 : test boot gui with double buffers.\n" \
	"boot_gui_test 4 : draw geometry by canvas on fb0.\n" \
	"boot_gui_test 5 : draw chars by canvas on fb0.\n" \
	"boot_gui_test 6 : show fb0 on two different devices.\n" \
	"boot_gui_test 7 : show fb0 on device_0, fb1 on device_1.\n"

static int print_fb_info(int fb_id)
{
	struct canvas *cv = fb_lock(fb_id);
	if (NULL == cv)
		return -1;
	printf("fb%d: width=%d, height=%d, bits_per_pixel=%d, line_stride=%d\n",
		fb_id, cv->width, cv->height, cv->bpp, cv->stride);
	fb_unlock(fb_id, NULL, 0);
	return 0;
}

static int print_all_fb_info(void)
{
	unsigned int fb_id;

	for (fb_id = FB_ID_0; fb_id < FB_ID_NUM; ++fb_id) {
		print_fb_info(fb_id);
	}
	return 0;
}

static int my_fill_rect(char *data, unsigned int bpp, unsigned int stride,
	unsigned int width, unsigned height, unsigned int color)
{
	char *data_end = data + stride * height;

	if (32 == bpp) {
		for (; data < data_end; data += stride) {
			int *p = (int *)data;
			int *p_end = p + width;
			for (; p < p_end;)
				*p++ = (int)color;
		}
	} else if (24 == bpp) {
		for (; data < data_end; data += stride) {
			char *p = data;
			char *p_end = p + width * 3;
			for (; p < p_end;) {
				*p++ = color & 0xFF;
				*p++ = (color >> 8) & 0xFF;
				*p++ = (color >> 16) & 0xFF;
			}
		}
	}
	return 0;
}

static int test_draw_fb_direct(void)
{
	unsigned int loop_cnt = 0;
	unsigned int clist[] = {
		0xFFFF0000,
		0xFF00FF00,
		0xFF0000FF,
		0xFFFFFF00,
		0xFF00FFFF,
		0xFFFF00FF,
	};

	printf("%s begin\n", __func__);
	loop_cnt = 10;
	while (loop_cnt--) {
		char *pdata;
		unsigned int color;
		rect_t dirty_rect;
		struct canvas *cv = fb_lock(FB_ID_0);
		if (NULL == cv) {
			printf("%s: fb lock failed\n", __func__);
			return -1;
		}

		color = clist[loop_cnt % (sizeof(clist) / sizeof(clist[0]))];
		dirty_rect.left = cv->width >> 2;
		dirty_rect.top = cv->height >> 2;
		dirty_rect.right = cv->width - dirty_rect.left;
		dirty_rect.bottom = cv->height - dirty_rect.top;
		pdata = (char *)cv->base + (cv->stride * dirty_rect.top)
			+ (cv->bpp * dirty_rect.left >> 3);
		my_fill_rect(pdata, cv->bpp, cv->stride, dirty_rect.right - dirty_rect.left,
			dirty_rect.bottom - dirty_rect.top, color);
		/* fb_unlock(FB_ID_0, &dirty_rect, 1); */ /* refresh dirty_rect of screen */
		fb_unlock(FB_ID_0, NULL, 1); /* refresh whole screen */
		__msdelay(300);
	}
	return 0;
}

static int test_draw_fb_with_double_buf(void)
{
#ifdef CONFIG_BOOT_GUI_DOUBLE_BUF
	printf("%s begin\n", __func__);
	unsigned int loop_cnt = 0;
	unsigned int clist[] = {
		0xFFFF0000,
		0xFF00FF00,
		0xFF0000FF,
		0xFFFFFF00,
		0xFF00FFFF,
		0xFFFF00FF,
		0xFFFFFFFF,
	};
	rect_t dirty_rects[4];
	struct canvas *cv = fb_lock(FB_ID_0);
	if (NULL == cv) {
		printf("%s: fb lock failed\n", __func__);
		return -1;
	}
	dirty_rects[0].left = cv->width >> 2;
	dirty_rects[0].right = cv->width >> 1;
	dirty_rects[0].top = cv->height >> 2;
	dirty_rects[0].bottom = cv->height >> 1;
	dirty_rects[1].left = cv->width >> 1;
	dirty_rects[1].right = cv->width - (cv->width >> 2);
	dirty_rects[1].top = cv->height >> 2;
	dirty_rects[1].bottom = cv->height >> 1;
	dirty_rects[2].left = cv->width >> 1;
	dirty_rects[2].right = cv->width - (cv->width >> 2);
	dirty_rects[2].top = cv->height >> 1;
	dirty_rects[2].bottom = cv->height - (cv->height >> 2);
	dirty_rects[3].left = cv->width >> 2;
	dirty_rects[3].right = cv->width >> 1;
	dirty_rects[3].top = cv->height >> 1;
	dirty_rects[3].bottom = cv->height - (cv->height >> 2);
	fb_unlock(FB_ID_0, NULL, 0);

	loop_cnt = 50;
	while (loop_cnt--) {
		char *pdata;
		unsigned int color;
		rect_t *p_rect;
		struct canvas *cv = fb_lock(FB_ID_0);
		if (NULL == cv) {
			printf("%s: fb lock failed\n", __func__);
			return -1;
		}

		color = clist[loop_cnt % (sizeof(clist) / sizeof(clist[0]))];
		p_rect = &dirty_rects[loop_cnt % (sizeof(dirty_rects) / sizeof(dirty_rects[0]))];
		pdata = (char *)cv->base + (cv->stride * p_rect->top)
			+ (cv->bpp * p_rect->left >> 3);
		my_fill_rect(pdata, cv->bpp, cv->stride, p_rect->right - p_rect->left,
			p_rect->bottom - p_rect->top, color);
		fb_unlock(FB_ID_0, p_rect, 1);
		__msdelay(300);
	}
	return 0;
#else
	printf("please to define CONFIG_BOOT_GUI_DOUBLE_BUF\n");
	return -1;
#endif
}

static int test_draw_geometry(void)
{
#ifdef CONFIG_SUPORT_DRAW_GEOMETRY
	printf("%s begin, wait for finish\n", __func__);
	int loop_cnt = 0;
	struct canvas *cv = NULL;
	int width = 0;
	int height = 0;
	unsigned int color = 0xFF000000;
	unsigned int bg_color = 0xFF000000;
	argb_t *p_color = (argb_t *)&color;
	argb_t *p_bg_color = (argb_t *)&bg_color;

	rect_t bound;
	rect_t in_rect;
	point_t from;
	point_t to;

	unsigned int clist[] = {
		0xFFFF0000,
		0xFF00FF00,
		0xFF0000FF,
		0xFFFFFF00,
		0xFF00FFFF,
		0xFFFF00FF,
	};

	cv = fb_lock(FB_ID_0);
	if (NULL == cv)
		return -1;
	width = cv->width;
	height = cv->height;
	fb_unlock(FB_ID_0, NULL, 0); /* not refresh the screen */

	bound.left = 0;
	bound.top = 0;
	bound.right = width;
	bound.bottom = height;

	in_rect.left = width >> 2;
	in_rect.top = height >> 2;
	in_rect.right = width - in_rect.left;
	in_rect.bottom = height - in_rect.top;

	loop_cnt = 100;
	while (0 < loop_cnt) {
		--loop_cnt;
		cv = fb_lock(FB_ID_0);
		if (NULL == cv)
			return -1;

		bg_color += 0x080808;
		if (0xFF888888 < bg_color)
			bg_color = 0xFF000000;
		cv->fill_rect(cv, p_bg_color, &bound); /* fill background color */

		color = clist[loop_cnt % (sizeof(clist) / sizeof(clist[0]))];
		cv->draw_rect(cv, p_color, &bound); /* draw the bound of screen */
		cv->fill_rect(cv, p_color, &in_rect); /* fill the inner rect */

		from.x = 0;
		from.y = 0;
		to.x = width - 1;
		to.y = height - 1;
		cv->draw_line(cv, p_color, &from, &to); /* draw a corner line */
		from.x = 0;
		from.y = height - 1;
		to.x = width - 1;
		to.y = 0;
		cv->draw_line(cv, p_color, &from, &to); /* draw another corner line */

		fb_unlock(FB_ID_0, NULL, 1); /* refresh screen */

		__msdelay(500);
	}
	return 0;
#else
	printf("please to define CONFIG_SUPORT_DRAW_GEOMETRY\n");
	return -1;
#endif
}

static int test_draw_chars(void)
{
#ifdef CONFIG_SURPORT_DRAW_CHARS
	unsigned int loop_cnt = 0;

	struct canvas *cv = NULL;
	int width = 0;
	int height = 0;

	point_t char_p;
	unsigned int color = 0xFF000000;
	unsigned int clist[] = {
		0xFFFF0000,
		0xFF00FF00,
		0xFF0000FF,
		0xFFFFFF00,
		0xFF00FFFF,
		0xFFFF00FF,
	};
	char *str = "HELLO BOOTGUI !";

	printf("%s begin\n", __func__);
	cv = fb_lock(FB_ID_0);
	if (NULL == cv)
		return -1;
	width = cv->width;
	height = cv->height;
	fb_unlock(FB_ID_0, NULL, 0);

	char_p.x = width >> 2;
	char_p.y = height >> 2;

	loop_cnt = 20;
	while (loop_cnt) {
		loop_cnt--;

		cv = fb_lock(FB_ID_0);
		if (NULL == cv)
			return -1;
		memset((void *)cv->base, 0, cv->stride * cv->height);
		color = clist[loop_cnt % (sizeof(clist) / sizeof(clist[0]))];
		cv->draw_chars(cv, (argb_t *)&color, &char_p, str, strlen(str));

		fb_unlock(FB_ID_0, NULL, 1);
		__msdelay(500);
	}

	do {
		for (char_p.y = 0; char_p.y < height; char_p.y += 4) {
			for (char_p.x = 0; char_p.x < width; char_p.x += 4) {
				cv = fb_lock(FB_ID_0);
				if (NULL == cv)
					return -1;
				memset((void *)cv->base, 0, cv->stride * cv->height);
				cv->draw_chars(cv, (argb_t *)&color, &char_p, str, strlen(str));
				fb_unlock(FB_ID_0, NULL, 1);
				__msdelay(200);
			}
		}
	} while (0);
	return 0;
#else
	printf("please to define CONFIG_SURPORT_DRAW_CHARS\n");
	return -1;
#endif
}

static int test_open_other_device(void)
{
	extern int disp_device_open_ex(int dev_id, int fb_id, int flag);

	if (disp_device_open_ex(1, FB_ID_0, 0)) {
		printf("disp_device_open_ex failed\n");
		return -1;
	}
	return 0;
}

static int test_show_2_fb_on_different_devices(void)
{
	extern int disp_device_open_ex(int dev_id, int fb_id, int flag);

	unsigned int fb_id;
	struct canvas *cv = NULL;

	if (disp_device_open_ex(1, FB_ID_1, 0)) {
		printf("disp_device_open_ex for fb1 failed\n");
		return -1;
	}

	for (fb_id = FB_ID_0; fb_id < FB_ID_2; ++fb_id) {
		int color;
		point_t p;
		char *str;
		cv = fb_lock(fb_id);
		if (NULL == cv) {
			printf("fb%d is invalid\n", fb_id);
			return -1;
		}
		if (FB_ID_0 == fb_id) {
			color = 0xFFFFFF00;
			p.x = cv->width >> 2;
			p.y = cv->height >> 2;
			str = "THIS IS BOOT_FB0";
		} else if (FB_ID_1 == fb_id) {
			color = 0xFF00FFFF;
			p.x = cv->width >> 1;
			p.y = cv->height >> 1;
			str = "THIS IS BOOT_FB1";
		}
		cv->draw_chars(cv, (argb_t *)&color, &p, str, strlen(str));
		fb_unlock(fb_id, NULL, 1);
	}

	return 0;
}

static int boot_gui_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int cmd;
	if (2 > argc) {
		return -1;
	}
	/* printf("%s begin. argc=%d, cmd=%s\n", __func__, argc, argv[1]); */
	cmd = simple_strtoul(argv[1], NULL, 10);
	switch (cmd) {
	case 1:
		print_all_fb_info();
		break;
	case 2:
		test_draw_fb_direct();
		break;
	case 3:
		test_draw_fb_with_double_buf();
		break;
	case 4:
		test_draw_geometry();
		break;
	case 5:
		test_draw_chars();
		break;
	case 6:
		test_open_other_device();
		break;
	case 7:
		test_show_2_fb_on_different_devices();
		break;
	default:
		printf("%s: no support this cmd(%s)\n", __func__, argv[1]);
		return -1;
	}
	return 0;
}

U_BOOT_CMD(
	boot_gui_test, 2, 0, boot_gui_test,
	"boot gui test", THIS_USAGE
);
