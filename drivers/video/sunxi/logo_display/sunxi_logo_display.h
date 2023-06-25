/*
 * sunxi_logo_display/sunxi_logo_display.h
 *
 * Copyright (c) 2007-2020 Allwinnertech Co., Ltd.
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
#ifndef _SUNXI_LOGO_DISPLAY_H
#define _SUNXI_LOGO_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif


int read_bmp_to_kernel(char *partition_name);

int fat_read_logo_to_kernel(char *name);

int show_bmp_on_fb(char *bmp_head_addr, unsigned int fb_id);

int sunxi_bmp_display(char *name);

int sunxi_bmp_dipslay_screen(sunxi_bmp_store_t bmp_info);

int sunxi_advert_display(char *fatname, char *filename);

#ifdef __cplusplus
}
#endif

#endif /*End of file*/
