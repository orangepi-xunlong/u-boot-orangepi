/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __DISP_LCD_H__
#define __DISP_LCD_H__

#include "disp_private.h"

#define LCD_GPIO_SCL (LCD_GPIO_NUM-2)
#define LCD_GPIO_SDA (LCD_GPIO_NUM-1)
#define LCD_GPIO_NUM 6
#define LCD_POWER_NUM 4
#define LCD_GPIO_REGU_NUM 3
struct __disp_lcd_cfg_t {
	bool lcd_used;

	bool lcd_bl_en_used;
	struct disp_gpio_set_t lcd_bl_en;
	int lcd_bl_gpio_hdl;
	char lcd_bl_regulator[25];
	/* 0: invalid, 1: gpio, 2: regulator */
	u32 lcd_power_type[LCD_POWER_NUM];
	struct disp_gpio_set_t lcd_power[LCD_POWER_NUM];
	char lcd_regu[LCD_POWER_NUM][25];
	/* index4: scl;  index5: sda */
	bool lcd_gpio_used[LCD_GPIO_NUM];
	/* index4: scl; index5: sda */
	struct disp_gpio_set_t lcd_gpio[LCD_GPIO_NUM];
	u32 gpio_hdl[LCD_GPIO_NUM];
	char lcd_gpio_regulator[LCD_GPIO_REGU_NUM][25];

	bool lcd_io_used[28];
	struct disp_gpio_set_t lcd_io[28];
	char lcd_io_regulator[25];

	u32 backlight_bright;
	/* IEP-drc backlight dimming rate: 0 -256 */
	/* (256: no dimming; 0: the most dimming) */
	u32 backlight_dimming;
	u32 backlight_curve_adjust[101];

	u32 lcd_bright;
	u32 lcd_contrast;
	u32 lcd_saturation;
	u32 lcd_hue;
};

s32 disp_init_lcd(struct __disp_bsp_init_para *para);
s32 disp_lcd_gpio_init(struct disp_lcd *lcd);
s32 disp_lcd_gpio_exit(struct disp_lcd *lcd);
s32 disp_lcd_gpio_get_value(struct disp_lcd *lcd, __u32 io_index);

extern void sync_event_proc(u32 screen_id);

#endif
