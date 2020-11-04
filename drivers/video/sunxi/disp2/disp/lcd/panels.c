/*
 * drivers/video/sunxi/disp2/disp/lcd/panels.c
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
#include "panels.h"

struct sunxi_lcd_drv g_lcd_drv;

__lcd_panel_t* panel_array[] = {
	&default_panel,
#ifdef CONFIG_LCD_SUPPORT_HE0801A068
	&he0801a068_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_LQ101R1SX03
	&lq101r1sx03_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_LS029B3SX02
	&ls029b3sx02_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_VR_LS055T1SX01
	&vr_ls055t1sx01_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_SL008PN21D
	&sl008pn21d_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_ILI9341
	&ili9341_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_FD055HD003S
	&fd055hd003s_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_TO20T20000
	&to20t20000_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_T30P106
	&t30p106_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_ST7796S
	&st7796s_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_ST7789V
	&st7789v_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_LH219WQ1
	&lh219wq1_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_FRD450H40014
	&frd450h40014_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_H245QBN02
	&h245qbn02_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_S2003T46G
	&s2003t46g_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_LT070ME05000
	&lt070me05000_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_WTQ05027D01
	&wtq05027d01_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_T27P06
	&t27p06_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_DX0960BE40A1
	&dx0960be40a1_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_TFT720X1280
	&tft720x1280_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_S6D7AA0X01
	&S6D7AA0X01_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_INET_DSI_PANEL
	&inet_dsi_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_GG1P4062UTSW
	&gg1p4062utsw_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_VR_SHARP
	&vr_sharp_panel,
#endif
#ifdef CONFIG_LCD_SUPPORT_WILLIAMLCD
	&WilliamLcd_panel,
#endif
#if defined(CONFIG_EINK_PANEL_USED)
	&default_eink,
#endif
#if defined(CONFIG_LCD_SUPPORT_BP101WX1)
	&bp101wx1_panel,
#endif
	/* add new panel below */

	NULL,
};

static void lcd_set_panel_funs(void)
{
	int i;

	for (i=0; panel_array[i] != NULL; i++) {
		sunxi_lcd_set_panel_funs(panel_array[i]->name, &panel_array[i]->func);
	}

	return ;
}

int lcd_init(void)
{
	sunxi_disp_get_source_ops(&g_lcd_drv.src_ops);
	lcd_set_panel_funs();

	return 0;
}
