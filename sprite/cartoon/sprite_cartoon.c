/*
 * sprite/cartoon/sprite_cartoon.c
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
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
#include "sprite_cartoon.h"
#include "sprite_cartoon_i.h"
#include "sprite_cartoon_color.h"
#include <sunxi_display2.h>
#include <sunxi_board.h>
#include <boot_gui.h>
#include <sys_partition.h>

DECLARE_GLOBAL_DATA_PTR;

sprite_cartoon_source  sprite_source;
static uint  progressbar_hd;
static int   last_rate;


/*
************************************************************************************************************
*
*                                             function
*
*    name          :	sprite_cartoon_screen_set
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sprite_cartoon_screen_set(void)
{

	struct canvas *cv = NULL;
	cv = fb_lock(FB_ID_0);
	if (NULL == cv) {
		printf("fb lock for sprite cartoon fail\n");
		return -1;
	}
	if (32 == cv->bpp)
		fb_set_alpha_mode(FB_ID_0, FB_PIXEL_ALPHA_MODE, 0x00);
	else if (24 == cv->bpp)
		fb_set_alpha_mode(FB_ID_0, FB_GLOBAL_ALPHA_MODE, 0xFF);
	sprite_source.screen_width = cv->width;
	sprite_source.screen_height = cv->height;

	if ((sprite_source.screen_width < 40) ||
	    (sprite_source.screen_height < 40)) {
		printf("sunxi cartoon error: invalid screen width or height\n");
		return -1;
	}
	sprite_source.screen_size =
	    sprite_source.screen_width * sprite_source.screen_height * 4;
	sprite_source.screen_buf = (char *)cv->base;
	sprite_source.color = SPRITE_CARTOON_GUI_GREEN;
	fb_unlock(FB_ID_0, NULL, 1);

	if (!sprite_source.screen_buf)
		return -1;
	memset(sprite_source.screen_buf, 0, sprite_source.screen_size);

	mdelay(5);
	return 0;
}

/*
************************************************************************************************************
*
*                                             function
*
*    name          :	sprite_cartoon_screen_set
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sprite_cartoon_test(int op)
{
	int i;
	uint progressbar_hd;
	int screen_width, screen_height;
	int x1, x2, y1, y2;

	struct canvas *cv = NULL;

	sprite_cartoon_screen_set();

	cv = fb_lock(FB_ID_0);
	if (NULL == cv) {
		return 0;
	}
	screen_width = cv->width;
	screen_height = cv->height;
	fb_unlock(FB_ID_0, NULL, 0);

	printf("screen_width = %d\n", screen_width);
	printf("screen_height = %d\n", screen_height);

	if (op <= 1) {
		x1 = screen_width / 4;
		x2 = x1 * 3;
		y1 = screen_height / 2 - 40;
		y2 = screen_height / 2 + 40;
	} else {
		x1 = screen_width / 2 - screen_width / 16;
		x2 = screen_width / 2 + screen_width / 16;
		y1 = screen_height * 1 / 8;
		y2 = screen_height * 7 / 8;
	}

	printf("bar x1: %d y1: %d\n", x1, y1);
	printf("bar x2: %d y2: %d\n", x2, y2);

	progressbar_hd = sprite_cartoon_progressbar_create(x1, y1, x2, y2, op);
	sprite_cartoon_progressbar_config(progressbar_hd,
					  SPRITE_CARTOON_GUI_RED,
					  SPRITE_CARTOON_GUI_GREEN, 2);
	sprite_cartoon_progressbar_active(progressbar_hd);

	sprite_uichar_init(24);
	sprite_uichar_printf("this is for test\n");

	sprite_uichar_printf("bar x1: %d y1: %d\n", x1, y1);
	sprite_uichar_printf("bar x2: %d y2: %d\n", x2, y2);

	do {
		for (i = 0; i < 100; i += 50) {
			sprite_cartoon_progressbar_upgrate(progressbar_hd, i);
			mdelay(500);
			sprite_uichar_printf("here %d\n", i);
		}

		sprite_uichar_printf("up %d\n", i);
		for (i = 99; i > 0; i -= 50) {
			sprite_cartoon_progressbar_upgrate(progressbar_hd, i);
			mdelay(500);
		}
		sprite_uichar_printf("down %d\n", i);
	}

	while (0);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :	sprite_cartoon_start
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
uint sprite_cartoon_create(int op)
{

	int screen_width, screen_height;
	int x1, x2, y1, y2;

	if (sprite_cartoon_screen_set()) {
		printf("sprite cartoon create fail\n");

		return -1;
	}

	struct canvas *cv = NULL;
	cv = fb_lock(FB_ID_0);
	if (NULL == cv) {
		return 0;
	}
	if (32 == cv->bpp)
		fb_set_alpha_mode(FB_ID_0, FB_PIXEL_ALPHA_MODE, 0x00);
	else if (24 == cv->bpp)
		fb_set_alpha_mode(FB_ID_0, FB_GLOBAL_ALPHA_MODE, 0xFF);

	screen_width = cv->width;
	screen_height = cv->height;
	fb_unlock(FB_ID_0, NULL, 0);

	printf("screen_width = %d\n", screen_width);
	printf("screen_height = %d\n", screen_height);

	if (op <= 1) {
		x1 = screen_width / 4;
		x2 = x1 * 3;
		y1 = screen_height / 2 - 40;
		y2 = screen_height / 2 + 40;
	} else {
		x1 = screen_width / 2 - screen_width / 16;
		x2 = screen_width / 2 + screen_width / 16;
		y1 = screen_height * 1 / 8;
		y2 = screen_height * 7 / 8;
	}

	printf("bar x1: %d y1: %d\n", x1, y1);
	printf("bar x2: %d y2: %d\n", x2, y2);

	progressbar_hd = sprite_cartoon_progressbar_create(x1, y1, x2, y2, op);
	sprite_cartoon_progressbar_config(progressbar_hd,
					  SPRITE_CARTOON_GUI_RED,
					  SPRITE_CARTOON_GUI_GREEN, 2);
	sprite_cartoon_progressbar_active(progressbar_hd);
	sprite_uichar_init(24);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :	sprite_cartoon_start
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sprite_cartoon_upgrade(int rate)
{

	if (last_rate == rate) {
		return 0;
	}
	last_rate = rate;

	sprite_cartoon_progressbar_upgrate(progressbar_hd, rate);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :	sprite_cartoon_start
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sprite_cartoon_destroy(void)
{

	sprite_cartoon_progressbar_destroy(progressbar_hd);

	return 0;
}

int do_sunxi_screen_char(cmd_tbl_t *cmdtp, int flag, int argc,
			 char *const argv[])
{
	int direction_option = 0;
	if (argc == 2)
		direction_option = simple_strtoul(argv[1], NULL, 10);

	return sprite_cartoon_test(direction_option);
}

U_BOOT_CMD(
	screen_char,	2,	0,	do_sunxi_screen_char,
	"show default screen chars",
	" [direction]\n"
	"direction:\n"
	"0:left to right\n"
	"1:right to left\n"
	"2:up to down\n"
	"3:down to up\n"
);
