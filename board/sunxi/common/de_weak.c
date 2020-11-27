/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef CONFIG_SUNXI_DISPLAY

void  __attribute__((weak)) display_update_dtb(void)
{

}
int  __attribute__((weak)) board_display_layer_request(void)
{
	return 0;
}

int  __attribute__((weak)) board_display_layer_release(void)
{
	return 0;
}
int  __attribute__((weak)) board_display_wait_lcd_open(void)
{
	return 0;
}
int __attribute__((weak)) board_display_wait_lcd_close(void)
{
	return 0;
}
int __attribute__((weak)) board_display_set_exit_mode(int lcd_off_only)
{
	return 0;
}
int __attribute__((weak)) board_display_layer_open(void)
{
	return 0;
}

int __attribute__((weak)) board_display_layer_close(void)
{
	return 0;
}

int __attribute__((weak)) board_display_layer_para_set(void)
{
	return 0;
}

int __attribute__((weak)) board_display_show_until_lcd_open(int display_source)
{
	return 0;
}

int __attribute__((weak)) board_display_show(int display_source)
{
	return 0;
}

int __attribute__((weak)) board_display_framebuffer_set(int width, int height, int bitcount, void *buffer)
{
	return 0;
}

void __attribute__((weak)) board_display_set_alpha_mode(int mode)
{
	return ;
}

int __attribute__((weak)) board_display_framebuffer_change(void *buffer)
{
	return 0;
}
int __attribute__((weak)) board_display_device_open(void)
{
	return 0;
}

int __attribute__((weak)) borad_display_get_screen_width(void)
{
	return 0;
}

int __attribute__((weak)) borad_display_get_screen_height(void)
{
	return 0;
}

void __attribute__((weak)) board_display_setenv(char *data)
{
	return;
}

#endif