/*
 * sprite/cartoon/sprite_progressbar/sprite_progressbar.c
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
#include  "../sprite_cartoon_i.h"
#include  "../sprite_cartoon.h"
#include  "../sprite_cartoon_color.h"
#include  "sprite_progressbar_i.h"
#include <sunxi_eink.h>

progressbar_t   progress;
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
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
progressbar_t *sprite_cartoon_progressbar_create(int x1, int y1, int x2, int y2, int op)
{
	progressbar_t *progress = NULL;
	int tmp;

	progress = (progressbar_t *)malloc(sizeof(progressbar_t));
	if (!progress) {
		printf("sprite cartoon ui: create progressbar failed\n");

		return 0;
	}
	progress->direction_option = op;
	if (x1 > x2) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if (y1 > y2) {
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	progress->x1 = x1;
	progress->x2 = x2;
	progress->y1 = y1;
	progress->y2 = y2;
	progress->width = x2 - x1;
	progress->height = y2 - y1;
	progress->st_x = progress->pr_x = x1 + 1;
	progress->st_y = progress->pr_y = y1 + 1;
	progress->frame_color = SPRITE_CARTOON_GUI_WHITE;
	progress->progress_color = SPRITE_CARTOON_GUI_GREEN;
	progress->progress_ratio = 0;
	progress->thick = 1;
	progress->frame_color = SPRITE_CARTOON_GUI_GREEN;
	progress->progress_color = SPRITE_CARTOON_GUI_RED;

	return progress;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
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
int sprite_cartoon_progressbar_config(progressbar_t *p, int frame_color,
				      int progress_color, int thickness)
{
	progressbar_t *progress = p;

	if (!p) {
		return -1;
	}
	progress->frame_color = frame_color;
	progress->progress_color = progress_color;
	progress->progress_ratio = 0;
	progress->thick = thickness;
	switch (progress->direction_option) {
	case 0:
		progress->st_x = progress->pr_x = progress->x1 + thickness;
		progress->st_y = progress->pr_y = progress->y1 + thickness;
		break;
	case 1:
		progress->st_x = progress->pr_x = progress->x2 + thickness;
		progress->st_y = progress->pr_y = progress->y1 + thickness;
		break;
	case 2:
		progress->st_x = progress->pr_x = progress->x1 + thickness;
		progress->st_y = progress->pr_y = progress->y1 + thickness;
		break;
	case 3:
		progress->st_x = progress->pr_x = progress->x1 + thickness;
		progress->st_y = progress->pr_y = progress->y2 + thickness;
		break;
	default:
		break;
	}

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
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
int sprite_cartoon_progressbar_active(progressbar_t *p)
{
	int base_color;
	int i;
	progressbar_t *progress = p;

	if (!p) {
		return -1;
	}
	base_color = sprite_cartoon_ui_get_color();
	sprite_cartoon_ui_set_color(progress->frame_color);
	for (i = 0; i < progress->thick; i++) {
		sprite_cartoon_ui_draw_hollowrectangle(
		    progress->x1 + i, progress->y1 + i, progress->x2 - i,
		    progress->y2 - i);
	}
	sprite_cartoon_ui_set_color(base_color);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
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
int sprite_cartoon_progressbar_destroy(progressbar_t *p)
{
	progressbar_t *progress = p;
	int base_color;

	if (!p) {
		return -1;
	}
	base_color = sprite_cartoon_ui_get_color();
	sprite_cartoon_ui_set_color(SPRITE_CARTOON_GUI_BLACK);
	sprite_cartoon_ui_draw_solidrectangle(progress->x1, progress->y1,
					      progress->x2, progress->y2);

	sprite_cartoon_ui_set_color(base_color);

	free(progress);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
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
int sprite_cartoon_progressbar_upgrate(progressbar_t *p, int rate)
{
	progressbar_t *progress = p;
	int base_color, progresscolor;
	int pixel;
	int x1 = 0, y1 = 0;
	int x2 = 0, y2 = 0;
#if defined(CONFIG_EINK200_SUNXI)
	struct eink_fb_info_t *p_eink_fb = eink_get_fb_inst();
#endif

	if ((rate < 0) || (rate > 100)) {
		printf("sprite_cartoon ui progressbar: invalid progressbar "
		       "rate\n");
		return -1;
	}
	if (!p) {
		printf("sprite_cartoon ui progressbar: invalid progressbar "
		       "pointer\n");
		return -1;
	}
	if (progress->direction_option <= 1)
		pixel = (rate * (progress->width - progress->thick * 2) / 100);
	else
		pixel = (rate * (progress->height - progress->thick * 2) / 100);
	if (rate == progress->progress_ratio) {
		return 0;
	} else {
		if (progress->direction_option <= 1) {
			x1 = progress->pr_x;
			if (progress->direction_option % 2)
				x2 = progress->st_x - pixel;
			else
				x2 = progress->st_x + pixel;
			progresscolor = (rate > progress->progress_ratio)
					    ? (progress->progress_color)
					    : (SPRITE_CARTOON_GUI_BLACK);
			progress->pr_x = x2;
			progress->progress_ratio = rate;
		} else {
			y1 = progress->pr_y;
			if (progress->direction_option % 2)
				y2 = progress->st_y - pixel;
			else
				y2 = progress->st_y + pixel;

			progresscolor = (rate > progress->progress_ratio)
					    ? (progress->progress_color)
					    : (SPRITE_CARTOON_GUI_BLACK);
			progress->pr_y = y2;
			progress->progress_ratio = rate;
		}
	}
	base_color = sprite_cartoon_ui_get_color();
	sprite_cartoon_ui_set_color(progresscolor);

	if (progress->direction_option <= 1) {
		y1 = progress->y1 + progress->thick;
		y2 = progress->y2 - progress->thick;
	} else {
		x1 = progress->x1 + progress->thick;
		x2 = progress->x2 - progress->thick;
	}

	sprite_cartoon_ui_draw_solidrectangle(x1, y1, x2, y2);

	sprite_cartoon_ui_set_color(base_color);
#if defined(CONFIG_EINK200_SUNXI)
	if (p_eink_fb) {
		p_eink_fb->eink_display(p_eink_fb);
	}
#endif

	return 0;
}
