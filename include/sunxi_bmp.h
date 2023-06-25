/*
 * include/sunxi_bmp.h
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

#ifndef __SUNXI_BAT_H__
#define __SUNXI_BAT_H__

typedef struct {
	int x;
	int y;
	int bit;
	void *buffer;
} sunxi_bmp_store_t;


extern int sunxi_bmp_decode(unsigned long addr, sunxi_bmp_store_t *bmp_info);
extern int sunxi_bmp_dipslay_screen(sunxi_bmp_store_t bmp_info);
extern int sunxi_bmp_display_mem(unsigned char *source, sunxi_bmp_store_t *bmp_info);
extern int show_bmp_on_fb(char *bmp_head_addr, unsigned int fb_id);
extern int sunxi_partition_get_partno_byname(const char *part_name);
extern int sunxi_advert_display(char *fatname, char *filename);

#define IDLE_STATUS 0
#define DISPLAY_DRIVER_INIT_OK  1
#define DISPLAY_LOGO_LOAD_OK 2
#define DISPLAY_LOGO_LOAD_FAIL 3

#endif  /* __SUNXI_BAT_H__ */
