/*
 * drivers/video/sunxi/disp2/disp/lcd/panels.h
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
#ifndef __PANEL_H__
#define __PANEL_H__
#include "../de/bsp_display.h"
#include "lcd_source.h"

extern void LCD_OPEN_FUNC(u32 sel, LCD_FUNC func, u32 delay/*ms*/);
extern void LCD_CLOSE_FUNC(u32 sel, LCD_FUNC func, u32 delay/*ms*/);

typedef struct{
	char name[32];
	disp_lcd_panel_fun func;
}__lcd_panel_t;

extern __lcd_panel_t * panel_array[];

struct sunxi_lcd_drv
{
  struct sunxi_disp_source_ops      src_ops;
};

#if !defined(SUPPORT_DSI)
typedef enum
{
	DSI_DCS_ENTER_IDLE_MODE                         =       0x39 ,                  //01
	DSI_DCS_ENTER_INVERT_MODE                       =       0x21 ,                  //02
	DSI_DCS_ENTER_NORMAL_MODE                       =       0x13 ,                  //03
	DSI_DCS_ENTER_PARTIAL_MODE                      =       0x12 ,                  //04
	DSI_DCS_ENTER_SLEEP_MODE                        =       0x10 ,                  //05
	DSI_DCS_EXIT_IDLE_MODE                          =       0x38 ,                  //06
	DSI_DCS_EXIT_INVERT_MODE                        =       0x20 ,                  //07
	DSI_DCS_EXIT_SLEEP_MODE                         =       0x11 ,                  //08
	DSI_DCS_GET_ADDRESS_MODE                        =       0x0b ,                  //09
	DSI_DCS_GET_BLUE_CHANNEL                        =       0x08 ,                  //10
	DSI_DCS_GET_DIAGNOSTIC_RESULT           =       0x0f ,                  //11
	DSI_DCS_GET_DISPLAY_MODE                        =       0x0d ,                  //12
	DSI_DCS_GET_GREEN_CHANNEL                       =       0x07 ,                  //13
	DSI_DCS_GET_PIXEL_FORMAT                        =       0x0c ,                  //14
	DSI_DCS_GET_POWER_MODE                          =       0x0a ,                  //15
	DSI_DCS_GET_RED_CHANNEL                         =       0x06 ,                  //16
	DSI_DCS_GET_SCANLINE                            =       0x45 ,                  //17
	DSI_DCS_GET_SIGNAL_MODE                         =       0x0e ,                  //18
	DSI_DCS_NOP                                                     =       0x00 ,                  //19
	DSI_DCS_READ_DDB_CONTINUE                       =       0xa8 ,                  //20
	DSI_DCS_READ_DDB_START                          =       0xa1 ,                  //21
	DSI_DCS_READ_MEMORY_CONTINUE            =       0x3e ,                  //22
	DSI_DCS_READ_MEMORY_START                       =       0x2e ,                  //23
	DSI_DCS_SET_ADDRESS_MODE                        =       0x36 ,                  //24
	DSI_DCS_SET_COLUMN_ADDRESS                      =       0x2a ,                  //25
	DSI_DCS_SET_DISPLAY_OFF                         =       0x28 ,                  //26
	DSI_DCS_SET_DISPLAY_ON                          =       0x29 ,                  //27
	DSI_DCS_SET_GAMMA_CURVE                         =       0x26 ,                  //28
	DSI_DCS_SET_PAGE_ADDRESS                        =       0x2b ,                  //29
	DSI_DCS_SET_PARTIAL_AREA                        =       0x30 ,                  //30
	DSI_DCS_SET_PIXEL_FORMAT                        =       0x3a ,                  //31
	DSI_DCS_SET_SCROLL_AREA                         =       0x33 ,                  //32
	DSI_DCS_SET_SCROLL_START                        =       0x37 ,                  //33
	DSI_DCS_SET_TEAR_OFF                            =       0x34 ,                  //34
	DSI_DCS_SET_TEAR_ON                                     =       0x35 ,                  //35
	DSI_DCS_SET_TEAR_SCANLINE                       =       0x44 ,                  //36
	DSI_DCS_SOFT_RESET                                      =       0x01 ,                  //37
	DSI_DCS_WRITE_LUT                                       =       0x2d ,                  //38
	DSI_DCS_WRITE_MEMORY_CONTINUE           =       0x3c ,                  //39
	DSI_DCS_WRITE_MEMORY_START                      =       0x2c ,                  //40
}__dsi_dcs_t;
#endif

extern int sunxi_disp_get_source_ops(struct sunxi_disp_source_ops *src_ops);
int lcd_init(void);

extern __lcd_panel_t default_panel;

#ifdef CONFIG_LCD_SUPPORT_HE0801A068
extern __lcd_panel_t he0801a068_panel;
#endif

#ifdef CONFIG_EINK_PANEL_USED
extern __lcd_panel_t default_eink;
#endif

#ifdef CONFIG_LCD_SUPPORT_LT070ME05000
extern __lcd_panel_t lt070me05000_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_WTQ05027D01
extern __lcd_panel_t wtq05027d01_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_T27P06
extern __lcd_panel_t t27p06_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_DX0960BE40A1
extern __lcd_panel_t dx0960be40a1_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_TFT720X1280
extern __lcd_panel_t tft720x1280_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_S6D7AA0X01
extern __lcd_panel_t S6D7AA0X01_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_GG1P4062UTSW
extern __lcd_panel_t gg1p4062utsw_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_LS029B3SX02
extern __lcd_panel_t ls029b3sx02_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_FD055HD003S
extern __lcd_panel_t fd055hd003s_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_FRD450H40014
extern __lcd_panel_t frd450h40014_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_H245QBN02
extern __lcd_panel_t h245qbn02_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_ILI9341
extern __lcd_panel_t ili9341_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_LH219WQ1
extern __lcd_panel_t lh219wq1_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_ST7789V
extern __lcd_panel_t st7789v_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_ST7796S
extern __lcd_panel_t st7796s_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_ST7701S
extern __lcd_panel_t st7701s_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_WTL096601G03
extern __lcd_panel_t wtl096601g03_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_T30P106
extern __lcd_panel_t t30p106_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_TO20T20000
extern __lcd_panel_t to20t20000_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_S2003T46G
extern __lcd_panel_t s2003t46g_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_WILLIAMLCD
extern __lcd_panel_t WilliamLcd_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_LQ101R1SX03
extern __lcd_panel_t lq101r1sx03_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_INET_DSI_PANEL
extern __lcd_panel_t inet_dsi_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_VR_LS055T1SX01
extern __lcd_panel_t vr_ls055t1sx01_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_SL008PN21D
extern __lcd_panel_t sl008pn21d_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_VR_SHARP
extern __lcd_panel_t vr_sharp_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_BP101WX1
extern __lcd_panel_t bp101wx1_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_M133X56
extern __lcd_panel_t m133x56_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_K101IM2QA04
extern __lcd_panel_t k101im2qa04_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_K101IM2BYL02L
extern __lcd_panel_t k101im2byl02l_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_FX070
extern __lcd_panel_t fx070_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_ST7789V_CPU
extern __lcd_panel_t st7789v_cpu_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_ST7701S
extern __lcd_panel_t st7701s_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_WTL096601G03
extern __lcd_panel_t wtl096601g03_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_M101B31
extern __lcd_panel_t M101B31_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_CC08021801_310_800X1280
extern __lcd_panel_t CC08021801_310_800X1280_mipi_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_KD101NA5
extern __lcd_panel_t kd101na5_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_KD070D57
extern __lcd_panel_t kd070d57_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_ZS080NI4003E7H3H_A
extern __lcd_panel_t zs080ni4003e7h3h_a_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_K080_IM2HYL802R_800X1280
extern __lcd_panel_t K080_IM2HYL802R_800X1280_mipi_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_K101_IM2BYL02_L_800X1280
extern __lcd_panel_t K101_IM2BYL02_L_800X1280_mipi_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_JD9366AB_3
extern __lcd_panel_t jd9366ab_3_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_TFT08006
extern __lcd_panel_t tft08006_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_T050K589
extern __lcd_panel_t t050k589_panel;
#endif

#ifdef CONFIG_LCD_SUPPORT_K101_MM2QA01_A
extern __lcd_panel_t K101_MM2QA01_A_mipi_panel;
#endif
#endif
