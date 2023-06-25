/* SPDX-License-Identifier: GPL-2.0+ */
/* Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "M101B31.h"

static void lcd_power_on(u32 sel);
static void lcd_power_off(u32 sel);
static void lcd_bl_open(u32 sel);
static void lcd_bl_close(u32 sel);

static void lcd_panel_init(u32 sel);
static void lcd_panel_exit(u32 sel);

#define panel_reset(sel, val) sunxi_lcd_gpio_set_value(sel, 0, val)

static void lcd_cfg_panel_info(panel_extend_para *info)
{
	u32 i = 0, j = 0;
	u32 items;
	u8 lcd_gamma_tbl[][2] = {
		{0, 0},
		{15, 15},
		{30, 30},
		{45, 45},
		{60, 60},
		{75, 75},
		{90, 90},
		{105, 105},
		{120, 120},
		{135, 135},
		{150, 150},
		{165, 165},
		{180, 180},
		{195, 195},
		{210, 210},
		{225, 225},
		{240, 240},
		{255, 255},
	};

	u32 lcd_cmap_tbl[2][3][4] = {
	{
		{LCD_CMAP_G0, LCD_CMAP_B1, LCD_CMAP_G2, LCD_CMAP_B3},
		{LCD_CMAP_B0, LCD_CMAP_R1, LCD_CMAP_B2, LCD_CMAP_R3},
		{LCD_CMAP_R0, LCD_CMAP_G1, LCD_CMAP_R2, LCD_CMAP_G3},
		},
		{
		{LCD_CMAP_B3, LCD_CMAP_G2, LCD_CMAP_B1, LCD_CMAP_G0},
		{LCD_CMAP_R3, LCD_CMAP_B2, LCD_CMAP_R1, LCD_CMAP_B0},
		{LCD_CMAP_G3, LCD_CMAP_R2, LCD_CMAP_G1, LCD_CMAP_R0},
		},
	};

	items = sizeof(lcd_gamma_tbl) / 2;
	for (i = 0; i < items - 1; i++) {
		u32 num = lcd_gamma_tbl[i + 1][0] - lcd_gamma_tbl[i][0];

		for (j = 0; j < num; j++) {
			u32 value = 0;

			value = lcd_gamma_tbl[i][1] +
				((lcd_gamma_tbl[i + 1][1] - lcd_gamma_tbl[i][1])
				* j) / num;
			info->lcd_gamma_tbl[lcd_gamma_tbl[i][0] + j] =
							(value << 16)
							+ (value << 8) + value;
		}
	}
	info->lcd_gamma_tbl[255] = (lcd_gamma_tbl[items - 1][1] << 16) +
					(lcd_gamma_tbl[items - 1][1] << 8)
					+ lcd_gamma_tbl[items - 1][1];

	memcpy(info->lcd_cmap_tbl, lcd_cmap_tbl, sizeof(lcd_cmap_tbl));
}

static s32 lcd_open_flow(u32 sel)
{
	LCD_OPEN_FUNC(sel, lcd_power_on, 120);
	LCD_OPEN_FUNC(sel, lcd_panel_init, 1);
	LCD_OPEN_FUNC(sel, sunxi_lcd_tcon_enable, 5);
	LCD_OPEN_FUNC(sel, lcd_bl_open, 0);

	return 0;
}

static s32 lcd_close_flow(u32 sel)
{
	LCD_CLOSE_FUNC(sel, lcd_bl_close, 0);
	LCD_CLOSE_FUNC(sel, lcd_panel_exit, 1);
	LCD_CLOSE_FUNC(sel, sunxi_lcd_tcon_disable, 10);
	LCD_CLOSE_FUNC(sel, lcd_power_off, 0);

	return 0;
}

static void lcd_power_on(u32 sel)
{
	sunxi_lcd_pin_cfg(sel, 1);
	panel_reset(sel, 0);
	sunxi_lcd_power_enable(sel, 0);
	sunxi_lcd_power_enable(sel, 1);
	sunxi_lcd_delay_ms(50);
	panel_reset(sel, 1);
	sunxi_lcd_delay_ms(1);
	panel_reset(sel, 0);
	sunxi_lcd_delay_ms(1);
	panel_reset(sel, 1);
}

static void lcd_power_off(u32 sel)
{
	panel_reset(sel, 0);
	sunxi_lcd_delay_ms(1);
	sunxi_lcd_power_disable(sel, 1);
	sunxi_lcd_delay_ms(1);
	sunxi_lcd_power_disable(sel, 0);
	sunxi_lcd_pin_cfg(sel, 0);
}

static void lcd_bl_open(u32 sel)
{
	sunxi_lcd_pwm_enable(sel);
	sunxi_lcd_backlight_enable(sel);
}

static void lcd_bl_close(u32 sel)
{
	sunxi_lcd_backlight_disable(sel);
	sunxi_lcd_pwm_disable(sel);
}

static void lcd_panel_init(u32 sel)
{
	sunxi_lcd_dsi_clk_enable(sel);
	/*M101B31 initial code */
	sunxi_lcd_dsi_gen_write_3para(sel, 0xFF, 0x98, 0x81, 0x03);

	sunxi_lcd_dsi_gen_write_1para(sel, 0x01, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x02, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x03, 0x53);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x04, 0x53);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x05, 0x13);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x06, 0x04);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x07, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x08, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x09, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x0a, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x0b, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x0c, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x0d, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x0e, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x0f, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x10, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x11, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x12, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x13, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x14, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x15, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x16, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x17, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x18, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x19, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x1a, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x1b, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x1c, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x1d, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x1e, 0xc0);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x1f, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x20, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x21, 0x09);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x22, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x23, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x24, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x25, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x26, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x27, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x28, 0x55);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x29, 0x03);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x2a, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x2b, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x2c, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x2d, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x2e, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x2f, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x30, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x31, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x32, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x33, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x34, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x35, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x36, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x37, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x38, 0x3C);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x39, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x3a, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x3b, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x3c, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x3d, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x3e, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x3f, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x40, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x41, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x42, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x43, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x44, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x45, 0x00);

	sunxi_lcd_dsi_gen_write_1para(sel, 0x50, 0x01);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x51, 0x23);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x52, 0x45);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x53, 0x67);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x54, 0x89);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x55, 0xab);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x56, 0x01);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x57, 0x23);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x58, 0x45);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x59, 0x67);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x5a, 0x89);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x5b, 0xab);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x5c, 0xcd);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x5d, 0xef);

	sunxi_lcd_dsi_gen_write_1para(sel, 0x5e, 0x01);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x5f, 0x0A);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x60, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x61, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x62, 0x08);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x63, 0x15);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x64, 0x14);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x65, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x66, 0x11);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x67, 0x10);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x68, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x69, 0x0F);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x6a, 0x0E);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x6b, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x6c, 0x0D);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x6d, 0x0C);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x6e, 0x06);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x6f, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x70, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x71, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x72, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x73, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x74, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x75, 0x0A);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x76, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x77, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x78, 0x06);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x79, 0x15);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x7a, 0x14);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x7b, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x7c, 0x10);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x7d, 0x11);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x7e, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x7f, 0x0C);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x80, 0x0D);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x81, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x82, 0x0E);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x83, 0x0F);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x84, 0x08);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x85, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x86, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x87, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x88, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x89, 0x02);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x8A, 0x02);
	sunxi_lcd_dsi_gen_write_3para(sel, 0xFF, 0x98, 0x81, 0x04);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x3B, 0xC0);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x6C, 0x15);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x6E, 0x30);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x6F, 0x55);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x3A, 0x24);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x8D, 0x1F);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x87, 0xBA);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x26, 0x76);
	sunxi_lcd_dsi_gen_write_1para(sel, 0xB2, 0xD1);
	sunxi_lcd_dsi_gen_write_1para(sel, 0xB5, 0x07);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x35, 0x1F);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x88, 0x0B);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x21, 0x30);
	sunxi_lcd_dsi_gen_write_3para(sel, 0xFF, 0x98, 0x81, 0x01);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x22, 0x0A);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x31, 0x09);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x40, 0x53);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x53, 0x37);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x55, 0x88);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x50, 0x95);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x51, 0x95);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x60, 0x30);
	sunxi_lcd_dsi_gen_write_3para(sel, 0xFF, 0x98, 0x81, 0x00);
	sunxi_lcd_dsi_gen_write_1para(sel, 0x35, 0x00);
	sunxi_lcd_dsi_gen_write_0para(sel, 0x11);
	sunxi_lcd_delay_ms(120);
	sunxi_lcd_dsi_gen_write_0para(sel, 0x29);
	sunxi_lcd_delay_ms(5);
}

static void lcd_panel_exit(u32 sel)
{
	sunxi_lcd_dsi_gen_write_0para(sel, 0x10);
	sunxi_lcd_delay_ms(1);
	sunxi_lcd_dsi_gen_write_0para(sel, 0x28);
	sunxi_lcd_delay_ms(1);
}

/*sel: 0:lcd0; 1:lcd1*/
static s32 lcd_user_defined_func(u32 sel, u32 para1, u32 para2, u32 para3)
{
	return 0;
}

__lcd_panel_t M101B31_panel = {
	/* panel driver name, must mach the name of
	 * lcd_drv_name in sys_config.fex
	 */
	.name = "M101B31",
	.func = {
		.cfg_panel_info = lcd_cfg_panel_info,
			.cfg_open_flow = lcd_open_flow,
			.cfg_close_flow = lcd_close_flow,
			.lcd_user_defined_func = lcd_user_defined_func,
	},
};
