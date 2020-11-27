#include "panels.h"

struct sunxi_lcd_drv g_lcd_drv;

extern __lcd_panel_t default_panel;
extern __lcd_panel_t lt070me05000_panel;
extern __lcd_panel_t wtq05027d01_panel;
extern __lcd_panel_t t27p06_panel;
extern __lcd_panel_t dx0960be40a1_panel;
extern __lcd_panel_t tft720x1280_panel;
extern __lcd_panel_t S6D7AA0X01_panel;
extern __lcd_panel_t inet_dsi_panel;
extern __lcd_panel_t default_eink;
extern __lcd_panel_t fx070_panel;

__lcd_panel_t* panel_array[] = {
#if defined(CONFIG_ARCH_SUN50IW3P1)
	&lq101r1sx03_panel,
	&ls029b3sx02_panel,
	&vr_ls055t1sx01_panel,
	&sl008pn21d_panel,
	&he0801a068_panel,
#else
#if defined(CONFIG_ARCH_SUN8IW12P1)
	&ili9341_panel,
	&fd055hd003s_panel,
	&default_panel,
#else
	&default_panel,
	&lt070me05000_panel,
	&wtq05027d01_panel,
	&t27p06_panel,
	&dx0960be40a1_panel,
	&tft720x1280_panel,
	&S6D7AA0X01_panel,
	&inet_dsi_panel,
	&gg1p4062utsw_panel,
	&vr_sharp_panel,
	&he0801a068_panel,
	&WilliamLcd_panel,
	&default_eink,
	&fx070_panel,

#endif
#endif /*endif CONFIG_ARCH_SUN50IW3P1 */
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
