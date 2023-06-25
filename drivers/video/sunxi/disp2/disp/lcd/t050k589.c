/**
 * drivers/video/sunxi/disp2/disp/lcd/he0801a068.c
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * he0801a-068 panel driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * [lcd0]
 * lcd_used            = 1

 * lcd_driver_name     = "t050k589"
 * lcd_bl_0_percent    = 0
 * lcd_bl_40_percent   = 23
 * lcd_bl_100_percent  = 100
 * lcd_backlight       = 100

 * lcd_if              = 4
 * lcd_x               = 240
 * lcd_y               = 432
 * lcd_width           = 52
 * lcd_height          = 52
 * lcd_dclk_freq       = 18

 * lcd_pwm_used        = 1
 * lcd_pwm_ch          = 0
 * lcd_pwm_freq        = 50000
 * lcd_pwm_pol         = 1
 * lcd_pwm_max_limit   = 255

 * lcd_hbp             = 96
 * lcd_ht              = 480
 * lcd_hspw            = 2
 * lcd_vbp             = 21
 * lcd_vt              = 514
 * lcd_vspw            = 2

 * lcd_dsi_if          = 1
 * lcd_dsi_lane        = 1
 * lcd_dsi_format      = 0
 * lcd_dsi_te          = 1
 * lcd_dsi_eotp        = 0

 * lcd_frm             = 0
 * lcd_io_phase        = 0x0000
 * lcd_hv_clk_phase    = 0
 * lcd_hv_sync_polarity= 0
 * lcd_gamma_en        = 0
 * lcd_bright_curve_en = 0
 * lcd_cmap_en         = 0

 * lcdgamma4iep        = 22

 * lcd_bl_en           = port:PB03<1><0><default><1>
 * lcd_power           = "axp233_dc1sw"
 * lcd_power1           = "axp233_eldo1"

 * lcd_gpio_0          = port:PB02<1><0><default><0>
 * lcd_vsync          = port:PD21<2><0><3><default>
 */
#include "t050k589.h"

static void lcd_power_on(u32 sel);
static void lcd_power_off(u32 sel);
static void lcd_bl_open(u32 sel);
static void lcd_bl_close(u32 sel);

static void lcd_panel_init(u32 sel);
static void lcd_panel_exit(u32 sel);

#define panel_reset(sel, val) sunxi_lcd_gpio_set_value(sel, 0, val)
#define DBG_INFO(format, args...) (printk("[T050K589 LCD INFO] LINE:%04d-->%s:"format, __LINE__, __func__, ##args))
#define DBG_ERR(format, args...) (printk("[T050K589 LCD ERR] LINE:%04d-->%s:"format, __LINE__, __func__, ##args))

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
		u32 num = lcd_gamma_tbl[i+1][0] - lcd_gamma_tbl[i][0];

		for (j = 0; j < num; j++) {
			u32 value = 0;

			value = lcd_gamma_tbl[i][1] +
				((lcd_gamma_tbl[i+1][1] - lcd_gamma_tbl[i][1])
				* j) / num;
			info->lcd_gamma_tbl[lcd_gamma_tbl[i][0] + j] =
							(value<<16)
							+ (value<<8) + value;
		}
	}
	info->lcd_gamma_tbl[255] = (lcd_gamma_tbl[items-1][1]<<16) +
					(lcd_gamma_tbl[items-1][1]<<8)
					+ lcd_gamma_tbl[items-1][1];

	memcpy(info->lcd_cmap_tbl, lcd_cmap_tbl, sizeof(lcd_cmap_tbl));

}

static s32 lcd_open_flow(u32 sel)
{
	DBG_INFO("\n");
	LCD_OPEN_FUNC(sel, lcd_power_on, 15);
	LCD_OPEN_FUNC(sel, lcd_panel_init, 120);
	LCD_OPEN_FUNC(sel, sunxi_lcd_tcon_enable, 5);
	LCD_OPEN_FUNC(sel, lcd_bl_open, 0);
	return 0;
}

static s32 lcd_close_flow(u32 sel)
{
	DBG_INFO("\n");
	LCD_CLOSE_FUNC(sel, lcd_bl_close, 0);
	LCD_CLOSE_FUNC(sel, lcd_panel_exit, 120);
	LCD_CLOSE_FUNC(sel, sunxi_lcd_tcon_disable, 0);
	LCD_CLOSE_FUNC(sel, lcd_power_off, 0);

	return 0;
}

static void lcd_power_on(u32 sel)
{
	DBG_INFO("\n");
	sunxi_lcd_pin_cfg(sel, 1);

//	panel_reset(sel, 0);
//	sunxi_lcd_delay_ms(10);
	/*3.3v*/
	sunxi_lcd_power_enable(sel, 0);
	sunxi_lcd_delay_ms(10);
	sunxi_lcd_gpio_set_value(sel, 1, 1);
#if 0
	panel_reset(sel, 1);
	sunxi_lcd_delay_ms(300);
	panel_reset(sel, 0);
	sunxi_lcd_delay_ms(20);
	panel_reset(sel, 1);
	sunxi_lcd_delay_ms(10);
#endif
	sunxi_lcd_power_enable(sel, 1);
}

static void lcd_power_off(u32 sel)
{
	DBG_INFO("\n");
	sunxi_lcd_pin_cfg(sel, 0);
	sunxi_lcd_delay_ms(20);
	panel_reset(sel, 0);
	sunxi_lcd_delay_ms(5);
	sunxi_lcd_power_disable(sel, 1);
	sunxi_lcd_delay_ms(5);
	sunxi_lcd_power_disable(sel, 0);
	sunxi_lcd_delay_ms(20);
	sunxi_lcd_gpio_set_value(sel, 1, 0);
}

static void lcd_bl_open(u32 sel)
{
	DBG_INFO("\n");
	sunxi_lcd_pwm_enable(sel);
	sunxi_lcd_backlight_enable(sel);
}

static void lcd_bl_close(u32 sel)
{
	DBG_INFO("\n");
	sunxi_lcd_backlight_disable(sel);
	sunxi_lcd_pwm_disable(sel);
}
#define REGFLAG_DELAY               0xFE
#define REGFLAG_END_OF_TABLE        0xFF   // END OF REGISTERS MARKER

struct LCM_setting_table {
	__u8 cmd;
	__u32 count;
	__u8 para_list[64];
};


static struct LCM_setting_table jd9365_initialization_setting[] = {
{0xE0, 1, {0x00}},
{0xE1, 1, {0x93}},
{0xE2, 1, {0x65}},
{0xE3, 1, {0xF8}},
{0xE0, 1, {0x01}},
{0x00, 1, {0x00}},
{0x01, 1, {0x22}},
{0x03, 1, {0x00}},
{0x04, 1, {0x24}},
{0x17, 1, {0x00}},
{0x18, 1, {0xBF}},  //VGMP=4.7V
{0x19, 1, {0x03}},
{0x1A, 1, {0x00}},
{0x1B, 1, {0xBF}},  //VGMN=-4.7V
{0x1C, 1, {0x03}},
{0x1F, 1, {0x79}},
{0x20, 1, {0x2D}},
{0x21, 1, {0x2D}},
{0x22, 1, {0x4F}},
{0x26, 1, {0xF1}},
{0x37, 1, {0x05}}, //0x09
{0x38, 1, {0x04}},	//JDT=100 column inversion
{0x39, 1, {0x0C}},
{0x3A, 1, {0x18}},
{0x3C, 1, {0x78}},
{0x40, 1, {0x04}},	//RSO=720RGB
{0x41, 1, {0xA0}},	//LN=640->1280 line
{0x55, 1, {0x01}},	//DCDCM=0010,
{0x56, 1, {0x01}},
{0x57, 1, {0x6D}},
{0x58, 1, {0x0A}},
{0x59, 1, {0x1A}},
{0x5A, 1, {0x65}},
{0x5B, 1, {0x14}},
{0x5C, 1, {0x15}},
{0x5D, 1, {0x70}},
{0x5E, 1, {0x5F}},
{0x5F, 1, {0x50}},
{0x60, 1, {0x45}},
{0x61, 1, {0x40}},
{0x62, 1, {0x30}},
{0x63, 1, {0x33}},
{0x64, 1, {0x1B}},
{0x65, 1, {0x32}},
{0x66, 1, {0x31}},
{0x67, 1, {0x30}},
{0x68, 1, {0x4E}},
{0x69, 1, {0x3C}},
{0x6A, 1, {0x45}},
{0x6B, 1, {0x39}},
{0x6C, 1, {0x39}},
{0x6D, 1, {0x2F}},
{0x6E, 1, {0x26}},
{0x6F, 1, {0x02}},
{0x70, 1, {0x70}},
{0x71, 1, {0x5F}},
{0x72, 1, {0x50}},
{0x73, 1, {0x45}},
{0x74, 1, {0x40}},
{0x75, 1, {0x30}},
{0x76, 1, {0x33}},
{0x77, 1, {0x1B}},
{0x78, 1, {0x32}},
{0x79, 1, {0x31}},
{0x7A, 1, {0x30}},
{0x7B, 1, {0x4E}},
{0x7C, 1, {0x3C}},
{0x7D, 1, {0x45}},
{0x7E, 1, {0x39}},
{0x7F, 1, {0x39}},
{0x80, 1, {0x2F}},
{0x81, 1, {0x26}},
{0x82, 1, {0x02}},
{0xE0, 1, {0x02}},
{0x00, 1, {0x13}},
{0x01, 1, {0x11}},
{0x02, 1, {0x0B}},
{0x03, 1, {0x09}},
{0x04, 1, {0x07}},
{0x05, 1, {0x05}},
{0x06, 1, {0x1F}},
{0x07, 1, {0x1F}},
{0x08, 1, {0x1F}},
{0x09, 1, {0x1F}},
{0x0A, 1, {0x1F}},
{0x0B, 1, {0x1F}},
{0x0C, 1, {0x1F}},
{0x0D, 1, {0x1F}},
{0x0E, 1, {0x1F}},
{0x0F, 1, {0x1F}},
{0x10, 1, {0x1F}},
{0x11, 1, {0x1F}},
{0x12, 1, {0x01}},
{0x13, 1, {0x03}},
{0x14, 1, {0x1F}},
{0x15, 1, {0x1F}},
{0x16, 1, {0x12}},
{0x17, 1, {0x10}},
{0x18, 1, {0x0A}},
{0x19, 1, {0x08}},
{0x1A, 1, {0x06}},
{0x1B, 1, {0x04}},
{0x1C, 1, {0x1F}},
{0x1D, 1, {0x1F}},
{0x1E, 1, {0x1F}},
{0x1F, 1, {0x1F}},
{0x20, 1, {0x1F}},
{0x21, 1, {0x1F}},
{0x22, 1, {0x1F}},
{0x23, 1, {0x1F}},
{0x24, 1, {0x1F}},
{0x25, 1, {0x1F}},
{0x26, 1, {0x1F}},
{0x27, 1, {0x1F}},
{0x28, 1, {0x00}},
{0x29, 1, {0x02}},
{0x2A, 1, {0x1F}},
{0x2B, 1, {0x1F}},
{0x2C, 1, {0x00}},
{0x2D, 1, {0x02}},
{0x2E, 1, {0x08}},
{0x2F, 1, {0x0A}},
{0x30, 1, {0x04}},
{0x31, 1, {0x06}},
{0x32, 1, {0x1F}},
{0x33, 1, {0x1F}},
{0x34, 1, {0x1F}},
{0x35, 1, {0x1F}},
{0x36, 1, {0x1F}},
{0x37, 1, {0x1F}},
{0x38, 1, {0x1F}},
{0x39, 1, {0x1F}},
{0x3A, 1, {0x1F}},
{0x3B, 1, {0x1F}},
{0x3C, 1, {0x1F}},
{0x3D, 1, {0x1F}},
{0x3E, 1, {0x12}},
{0x3F, 1, {0x10}},
{0x40, 1, {0x1F}},
{0x41, 1, {0x1F}},
{0x42, 1, {0x01}},
{0x43, 1, {0x03}},
{0x44, 1, {0x09}},
{0x45, 1, {0x0B}},
{0x46, 1, {0x05}},
{0x47, 1, {0x07}},
{0x48, 1, {0x1F}},
{0x49, 1, {0x1F}},
{0x4A, 1, {0x1F}},
{0x4B, 1, {0x1F}},
{0x4C, 1, {0x1F}},
{0x4D, 1, {0x1F}},
{0x4E, 1, {0x1F}},
{0x4F, 1, {0x1F}},
{0x50, 1, {0x1F}},
{0x51, 1, {0x1F}},
{0x52, 1, {0x1F}},
{0x53, 1, {0x1F}},
{0x54, 1, {0x13}},
{0x55, 1, {0x11}},
{0x56, 1, {0x1F}},
{0x57, 1, {0x1F}},
{0x58, 1, {0x40}},
{0x59, 1, {0x00}},
{0x5A, 1, {0x00}},
{0x5B, 1, {0x30}},
{0x5C, 1, {0x04}},
{0x5D, 1, {0x30}},
{0x5E, 1, {0x01}},
{0x5F, 1, {0x02}},
{0x60, 1, {0x30}},
{0x61, 1, {0x01}},
{0x62, 1, {0x02}},
{0x63, 1, {0x03}},
{0x64, 1, {0x64}},
{0x65, 1, {0x75}},
{0x66, 1, {0x08}},
{0x67, 1, {0x72}},
{0x68, 1, {0x07}},
{0x69, 1, {0x10}},
{0x6A, 1, {0x64}},
{0x6B, 1, {0x08}},
{0x6C, 1, {0x00}},
{0x6D, 1, {0x00}},
{0x6E, 1, {0x00}},
{0x6F, 1, {0x00}},
{0x70, 1, {0x00}},
{0x71, 1, {0x00}},
{0x72, 1, {0x06}},
{0x73, 1, {0x86}},
{0x74, 1, {0x00}},
{0x75, 1, {0x07}},
{0x76, 1, {0x00}},
{0x77, 1, {0x5D}},
{0x78, 1, {0x19}},
{0x79, 1, {0x00}},
{0x7A, 1, {0x05}},
{0x7B, 1, {0x05}},
{0x7C, 1, {0x00}},
{0x7D, 1, {0x03}},
{0x7E, 1, {0x86}},
{0xE0, 1, {0x04}},
{0x09, 1, {0x10}},
{0xE0, 1, {0x00}},
{0xE6, 1, {0x02}},
{0xE7, 1, {0x02}},

{0x11, 0, {0x00}},
{REGFLAG_DELAY, 120, {}},
{0x29, 0, {0x00}},
{REGFLAG_DELAY, 5, {}},
{0x35, 0, {0x00}},
// Setting ending by predefined flag
{REGFLAG_END_OF_TABLE, REGFLAG_END_OF_TABLE, {}},
};

static void lcd_panel_init(u32 sel)
{
	int index;

	DBG_INFO("\n");
	sunxi_lcd_pin_cfg(sel, 1);
	// sunxi_lcd_delay_ms(120);
	panel_reset(sel, 1);
	sunxi_lcd_delay_ms(1);
	panel_reset(sel, 0);
	sunxi_lcd_delay_ms(1);
	panel_reset(sel, 1);
	sunxi_lcd_delay_ms(7);

	sunxi_lcd_dsi_clk_enable(sel);
	sunxi_lcd_delay_ms(1);
	//sunxi_lcd_dsi_dcs_write_0para(sel, DSI_DCS_SOFT_RESET);
	//sunxi_lcd_delay_ms(120);
#if 1
	DBG_INFO("initialization:jd9365\n");
	for (index = 0;; ++index) {
		//DBG_INFO("index:%d, count:%d, cmd:%d, para_lsit[0]:%d.\n",
		//		  index,
		//		  jd9365_initialization_setting[index].count,
		//		  jd9365_initialization_setting[index].cmd,
		//		  jd9365_initialization_setting[index].para_list[0]);

		if (jd9365_initialization_setting[index].count == REGFLAG_END_OF_TABLE) {
			break;
		} else if (jd9365_initialization_setting[index].count == REGFLAG_DELAY) {
			sunxi_lcd_delay_ms(jd9365_initialization_setting[index].para_list[0]);
		} else {
			sunxi_lcd_dsi_dcs_write(sel, jd9365_initialization_setting[index].cmd,
						    jd9365_initialization_setting[index].para_list,
							jd9365_initialization_setting[index].count);
		}
	}

#endif

}

static void lcd_panel_exit(u32 sel)
{
	sunxi_lcd_dsi_clk_disable(sel);
}

/*sel: 0:lcd0; 1:lcd1*/
static s32 lcd_user_defined_func(u32 sel, u32 para1, u32 para2, u32 para3)
{
	return 0;
}

__lcd_panel_t t050k589_panel = {
	/* panel driver name, must mach the name of
	 * lcd_drv_name in sys_config.fex
	 */
	.name = "t050k589",
	.func = {
		.cfg_panel_info = lcd_cfg_panel_info,
			.cfg_open_flow = lcd_open_flow,
			.cfg_close_flow = lcd_close_flow,
			.lcd_user_defined_func = lcd_user_defined_func,
	},
};
