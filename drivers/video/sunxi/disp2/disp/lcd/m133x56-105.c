/*
 * drivers/video/sunxi/disp2/disp/lcd/m133x56-105.c
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <wanpeng@allwinnertech.com>
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
#include <common.h>
#include <sys_config.h>
#include <ncs8801s.h>
#include <linux/libfdt.h>
//#include <i2c.h>
//#include <sunxi_i2c.h>
#include "m133x56-105.h"

static __u32 ncs8801s_hd;
user_gpio_set_t ncs8801s;
//struct i2c_adapter *i2c4_adp;


static void LCD_power_on(u32 sel);
static void LCD_power_off(u32 sel);
static void LCD_bl_open(u32 sel);
static void LCD_bl_close(u32 sel);

static void LCD_panel_init(u32 sel);
static void LCD_panel_exit(u32 sel);

static void LCD_cfg_panel_info(panel_extend_para *info)
{
	u32 i = 0, j = 0;
	u32 items;
	u8 lcd_gamma_tbl[][2] = {
	    //{input value, corrected value}
	    {0, 0},     {15, 15},   {30, 30},   {45, 45},   {60, 60},
	    {75, 75},   {90, 90},   {105, 105}, {120, 120}, {135, 135},
	    {150, 150}, {165, 165}, {180, 180}, {195, 195}, {210, 210},
	    {225, 225}, {240, 240}, {255, 255},
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

			value =
			    lcd_gamma_tbl[i][1] +
			    ((lcd_gamma_tbl[i + 1][1] - lcd_gamma_tbl[i][1]) *
			     j) /
				num;
			info->lcd_gamma_tbl[lcd_gamma_tbl[i][0] + j] =
			    (value << 16) + (value << 8) + value;
		}
	}
	info->lcd_gamma_tbl[255] = (lcd_gamma_tbl[items - 1][1] << 16) +
				   (lcd_gamma_tbl[items - 1][1] << 8) +
				   lcd_gamma_tbl[items - 1][1];

	memcpy(info->lcd_cmap_tbl, lcd_cmap_tbl, sizeof(lcd_cmap_tbl));
}

static s32 LCD_open_flow(u32 sel)
{
	LCD_OPEN_FUNC(sel, LCD_power_on, 10);
	LCD_OPEN_FUNC(sel, LCD_panel_init, 50);   //open lcd power, than delay 200ms
	LCD_OPEN_FUNC(sel, sunxi_lcd_tcon_enable, 220);
	LCD_OPEN_FUNC(sel, LCD_bl_open, 0);

	return 0;
}

static s32 LCD_close_flow(u32 sel)
{
	LCD_CLOSE_FUNC(sel, LCD_bl_close, 220);
	LCD_CLOSE_FUNC(sel, sunxi_lcd_tcon_disable, 10);
	LCD_CLOSE_FUNC(sel, LCD_panel_exit,	200);   //open lcd power, than delay 200ms
	LCD_CLOSE_FUNC(sel, LCD_power_off, 0);

	return 0;
}

static void LCD_power_on(u32 sel)
{
	sunxi_lcd_power_enable(sel, 0);
	sunxi_lcd_pin_cfg(sel, 1);
}

static void LCD_power_off(u32 sel)
{
	sunxi_lcd_pin_cfg(sel, 0);
	sunxi_lcd_power_disable(sel, 0);
}

static void LCD_bl_open(u32 sel)
{
	sunxi_lcd_pwm_enable(sel);
	sunxi_lcd_backlight_enable(sel);
}

static void LCD_bl_close(u32 sel)
{
	sunxi_lcd_backlight_disable(sel);
	sunxi_lcd_pwm_disable(sel);
}

static void LCD_panel_init(u32 sel)
{
	int ret   = 0;
	int busnum = -1;
	char *status = NULL;
	int nodeoffset;
//	printf("ncs8801s>>>enter LCD_panel_init\n");
	nodeoffset = fdt_path_offset(working_fdt, "/soc/ncs8801s");
	if (nodeoffset > 0) {
		fdt_getprop_string(working_fdt, nodeoffset, "status", &status);
	}
	if ((nodeoffset < 0) || (strlen(status) == 0)) {
		printf("ncs8801s status is not set!\n");
		return;
	} else {
		if ((strlen(status) == 4) && (strncmp(status, "okay", 4) == 0)) {
			printf("ncs8801s status is okay !\n");
			ret = fdt_get_one_gpio("/soc/ncs8801s", "ncs8801s_reset", &ncs8801s);
			if (!ret) {
				if (ncs8801s.port) {
					ncs8801s_hd = sunxi_gpio_request(&ncs8801s, 1);
					if (!ncs8801s_hd) {
						printf("reuqest gpio for nsc8801s failed\n");
						return;
					}
					ret = gpio_write_one_pin_value(ncs8801s_hd, 0, "ncs8801s_reset");
					if (ret < 0) {
						printf("gpio_write_one_pin_value write 0 failed\n");
						return;
					}
					mdelay(50);
					ret = gpio_write_one_pin_value(ncs8801s_hd, 1, "ncs8801s_reset");
					if (ret < 0) {
						printf("gpio_write_one_pin_value write 1 failed\n");
						return;
					}
				}
			} else {
				printf("fdt_get_one_gpio /soc/ncs8801s failed\n");
				return;
			}
		} else {
			printf("ncs8801s status is disabled !\n");
			return;
		}
	}

	busnum = i2c_get_bus_num();
	printf("i2c_get_bus_num is busnum=%d\n", busnum);
	if (busnum != SUNXI_VIR_I2C5) {
		i2c_set_bus_num(SUNXI_VIR_I2C5);
	}

	ncs8801s_i2c_writeByte(0xe0>>1, 0x0f, 0x01);
	ncs8801s_i2c_writeByte(0xe0>>1, 0x00, 0x00);
	ncs8801s_i2c_writeByte(0xe0>>1, 0x02, 0x07);
	ncs8801s_i2c_writeByte(0xe0>>1, 0x03, 0x03);
	ncs8801s_i2c_writeByte(0xe0>>1, 0x07, 0xc2);
	ncs8801s_i2c_writeByte(0xe0>>1, 0x09, 0x01);
	ncs8801s_i2c_writeByte(0xe0>>1, 0x0b, 0x00);
	ncs8801s_i2c_writeByte(0xe0>>1, 0x60, 0x00);
	ncs8801s_i2c_writeByte(0xe0>>1, 0x70, 0x00);
	ncs8801s_i2c_writeByte(0xe0>>1, 0x71, 0x01);
	ncs8801s_i2c_writeByte(0xe0>>1, 0x73, 0x80);
	ncs8801s_i2c_writeByte(0xe0>>1, 0x74, 0x20);
	ncs8801s_i2c_writeByte(0xea>>1, 0x00, 0xb0);
	ncs8801s_i2c_writeByte(0xea>>1, 0x84, 0x10);
	ncs8801s_i2c_writeByte(0xea>>1, 0x85, 0x32);
	ncs8801s_i2c_writeByte(0xea>>1, 0x01, 0x00);
	ncs8801s_i2c_writeByte(0xea>>1, 0x02, 0x5c);
	ncs8801s_i2c_writeByte(0xea>>1, 0x0b, 0x47);
	ncs8801s_i2c_writeByte(0xea>>1, 0x0e, 0x06);
	ncs8801s_i2c_writeByte(0xea>>1, 0x0f, 0x06);
	ncs8801s_i2c_writeByte(0xea>>1, 0x11, 0x88);
	ncs8801s_i2c_writeByte(0xea>>1, 0x22, 0x04);
	ncs8801s_i2c_writeByte(0xea>>1, 0x23, 0xf8);
	ncs8801s_i2c_writeByte(0xea>>1, 0x00, 0xb1);
	ncs8801s_i2c_writeByte(0xe0>>1, 0x0f, 0x00);

	i2c_set_bus_num(busnum);

	return;
}

static void LCD_panel_exit(u32 sel)
{
	return ;
}

//sel: 0:lcd0; 1:lcd1
static s32 LCD_user_defined_func(u32 sel, u32 para1, u32 para2, u32 para3)
{
	return 0;
}

__lcd_panel_t m133x56_panel = {
	/* panel driver name, must mach the name of lcd_drv_name in sys_config.fex */
	.name = "m133x56",
	.func = {
		.cfg_panel_info = LCD_cfg_panel_info,
		.cfg_open_flow = LCD_open_flow,
		.cfg_close_flow = LCD_close_flow,
		.lcd_user_defined_func = LCD_user_defined_func,
	},
};
