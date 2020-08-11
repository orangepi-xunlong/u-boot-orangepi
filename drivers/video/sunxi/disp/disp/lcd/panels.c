/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "panels.h"
#include "default_panel.h"
#include "tft720x1280.h"
#include "B116XAN03.h"
#include "gm7121_cvbs.h"
#include "u708_default_panel.h"

struct sunxi_lcd_drv g_lcd_drv;
struct __lcd_panel_t *panel_array[] = {
#if defined(CONFIG_TV_GM7121)
	&gm7121_cvbs,
#endif
	&default_panel,
	&tft720x1280_panel,
	&vvx10f004b00_panel,
	&lp907qx_panel,
	&starry768x1024_panel,
	&sl698ph_720p_panel,
	&B116XAN03_panel,
	&u708_default_panel,
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
