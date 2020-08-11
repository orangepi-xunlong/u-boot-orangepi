/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __PANEL_H__
#define __PANEL_H__
#include "../de/bsp_display.h"
#include "lcd_source_interface.h"
extern void LCD_OPEN_FUNC(u32 sel, LCD_FUNC func, u32 delay/*ms*/);
extern void LCD_CLOSE_FUNC(u32 sel, LCD_FUNC func, u32 delay/*ms*/);


struct __lcd_panel_t {
	char name[32];
	disp_lcd_panel_fun func;
};

struct sunxi_lcd_drv
{
  struct sunxi_disp_source_ops      src_ops;
};

enum __dsi_dcs_t {
	DSI_DCS_ENTER_IDLE_MODE = 0x39,
	DSI_DCS_ENTER_INVERT_MODE = 0x21,
	DSI_DCS_ENTER_NORMAL_MODE = 0x13,
	DSI_DCS_ENTER_PARTIAL_MODE = 0x12,
	DSI_DCS_ENTER_SLEEP_MODE = 0x10,
	DSI_DCS_EXIT_IDLE_MODE = 0x38,
	DSI_DCS_EXIT_INVERT_MODE = 0x20,
	DSI_DCS_EXIT_SLEEP_MODE = 0x11,
	DSI_DCS_GET_ADDRESS_MODE = 0x0b,
	DSI_DCS_GET_BLUE_CHANNEL = 0x08,
	DSI_DCS_GET_DIAGNOSTIC_RESULT = 0x0f,
	DSI_DCS_GET_DISPLAY_MODE = 0x0d,
	DSI_DCS_GET_GREEN_CHANNEL = 0x07,
	DSI_DCS_GET_PIXEL_FORMAT = 0x0c,
	DSI_DCS_GET_POWER_MODE = 0x0a,
	DSI_DCS_GET_RED_CHANNEL = 0x06,
	DSI_DCS_GET_SCANLINE = 0x45,
	DSI_DCS_GET_SIGNAL_MODE = 0x0e,
	DSI_DCS_NOP = 0x00,
	DSI_DCS_READ_DDB_CONTINUE = 0xa8,
	DSI_DCS_READ_DDB_START = 0xa1,
	DSI_DCS_READ_MEMORY_CONTINUE = 0x3e,
	DSI_DCS_READ_MEMORY_START = 0x2e,
	DSI_DCS_SET_ADDRESS_MODE = 0x36,
	DSI_DCS_SET_COLUMN_ADDRESS = 0x2a,
	DSI_DCS_SET_DISPLAY_OFF = 0x28,
	DSI_DCS_SET_DISPLAY_ON = 0x29,
	DSI_DCS_SET_GAMMA_CURVE = 0x26,
	DSI_DCS_SET_PAGE_ADDRESS = 0x2b,
	DSI_DCS_SET_PARTIAL_AREA = 0x30,
	DSI_DCS_SET_PIXEL_FORMAT = 0x3a,
	DSI_DCS_SET_SCROLL_AREA = 0x33,
	DSI_DCS_SET_SCROLL_START = 0x37,
	DSI_DCS_SET_TEAR_OFF = 0x34,
	DSI_DCS_SET_TEAR_ON = 0x35,
	DSI_DCS_SET_TEAR_SCANLINE = 0x44,
	DSI_DCS_SOFT_RESET = 0x01,
	DSI_DCS_WRITE_LUT = 0x2d,
	DSI_DCS_WRITE_MEMORY_CONTINUE = 0x3c,
	DSI_DCS_WRITE_MEMORY_START = 0x2c,
};

extern struct __lcd_panel_t tft720x1280_panel;
extern struct __lcd_panel_t vvx10f004b00_panel;
extern struct __lcd_panel_t lp907qx_panel;
extern struct __lcd_panel_t starry768x1024_panel;
extern struct __lcd_panel_t sl698ph_720p_panel;
extern struct __lcd_panel_t B116XAN03_panel;
extern struct __lcd_panel_t gm7121_cvbs;

extern struct __lcd_panel_t *panel_array[];
extern int sunxi_disp_get_source_ops(struct sunxi_disp_source_ops *src_ops);
int lcd_init(void);

void LCD_set_panel_funs(void);

extern void LCD_OPEN_FUNC(u32 sel, LCD_FUNC func, u32 delay /*ms */);
extern void LCD_CLOSE_FUNC(u32 sel, LCD_FUNC func, u32 delay /*ms */);
extern void LCD_delay_ms(u32 ms);
extern void LCD_delay_us(u32 ns);
extern void TCON_open(u32 sel);
extern void TCON_close(u32 sel);
extern s32 LCD_PWM_EN(u32 sel, bool b_en);
extern s32 LCD_BL_EN(u32 sel, bool b_en);
extern s32 LCD_POWER_EN(u32 sel, bool b_en);
extern void LCD_CPU_register_irq(u32 sel, void (*Lcd_cpuisr_proc) (void));
extern void LCD_CPU_WR(u32 sel, u32 index, u32 data);
extern void LCD_CPU_WR_INDEX(u32 sel, u32 index);
extern void LCD_CPU_WR_DATA(u32 sel, u32 data);
extern void LCD_CPU_AUTO_FLUSH(u32 sel, bool en);
extern void pwm_clock_enable(u32 sel);
extern void pwm_clock_disable(u32 sel);
extern s32 LCD_POWER_ELDO3_EN(u32 sel, bool b_en, u32 voltage);
extern s32 LCD_POWER_DLDO1_EN(u32 sel, bool b_en, u32 voltage);

extern s32 lcd_iic_write(u8 slave_addr, u8 sub_addr, u8 value);
extern s32 lcd_iic_read(u8 slave_addr, u8 sub_addr, u8 *value);

extern s32 lcd_get_panel_para(u32 sel, disp_panel_para *info);

extern s32 dsi_dcs_wr(u32 sel, u8 cmd, u8 *para_p, u32 para_num);
extern s32 dsi_dcs_wr_0para(u32 sel, u8 cmd);
extern s32 dsi_dcs_wr_1para(u32 sel, u8 cmd, u8 para);
extern s32 dsi_dcs_wr_2para(u32 sel, u8 cmd, u8 para1, u8 para2);
extern s32 dsi_dcs_wr_3para(u32 sel, u8 cmd, u8 para1, u8 para2, u8 para3);
extern s32 dsi_dcs_wr_4para(u32 sel, u8 cmd, u8 para1, u8 para2,
			    u8 para3, u8 para4);
extern s32 dsi_dcs_wr_5para(u32 sel, u8 cmd, u8 para1, u8 para2,
			    u8 para3, u8 para4, u8 para5);

extern __s32 dsi_gen_wr_0para(__u32 sel, __u8 cmd);
extern __s32 dsi_gen_wr_1para(__u32 sel, __u8 cmd, __u8 para);
extern __s32 dsi_gen_wr_2para(__u32 sel, __u8 cmd, __u8 para1, __u8 para2);
extern __s32 dsi_gen_wr_3para(__u32 sel, __u8 cmd, __u8 para1,
			      __u8 para2, __u8 para3);
extern __s32 dsi_gen_wr_4para(__u32 sel, __u8 cmd, __u8 para1,
			      __u8 para2, __u8 para3, __u8 para4);
extern __s32 dsi_gen_wr_5para(__u32 sel, __u8 cmd, __u8 para1,
			      __u8 para2, __u8 para3, __u8 para4, __u8 para5);

extern __s32 dsi_dcs_rd(__u32 sel, __u8 cmd, __u8 *para_p, __u32 *num_p);

extern s32 LCD_GPIO_request(u32 sel, u32 io_index);
extern s32 LCD_GPIO_release(u32 sel, u32 io_index);
extern s32 LCD_GPIO_set_attr(u32 sel, u32 io_index, bool b_output);
extern s32 LCD_GPIO_read(u32 sel, u32 io_index);
extern s32 LCD_GPIO_write(u32 sel, u32 io_index, u32 data);

#endif
