/*
 * sprite/cartoon/sprite_progressbar/sprite_progressbar_i.h
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
#ifndef __SPRITE_PROGRESSBAR_I_H__
#define __SPRITE_PROGRESSBAR_I_H__

typedef struct _progressbar_t {
	int x1;		    /*左上角x坐标 */
	int y1;		    /*左上角y坐标 */
	int x2;		    /*右下角x坐标 */
	int y2;		    /*右下角y坐标 */
	int st_x;	   /*内部起始点的x坐标 */
	int st_y;	   /*内部起始点的y坐标 */
	int pr_x;	   /*当前进度所在的x坐标 */
	int pr_y;	   /*无效 */
	int thick;	  /*框的厚度，边缘厚度 */
	int width;	  /*整体宽度 */
	int height;	 /*整体高度 */
	int frame_color;    /*边框颜色 */
	int progress_color; /*内部颜色 */
	int progress_ratio; /*当前进度百分比 */
	/*0:left to right*/
	/*1:right to left*/
	/*2:up to down*/
	/*3:down to up*/
	int direction_option;
} progressbar_t;

#endif   //__SPRITE_PROGRESSBAR_I_H__
