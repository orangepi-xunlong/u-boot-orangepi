#include "lh219wq1.h"

#define TCON_DEBUG		(0)
#define CPU_TRI_MODE

void lcd_panel_lh219wq1_init(disp_panel_para *info);
static void lcd_cpu_panel_fr(__u32 sel, __u32 w, __u32 h, __u32 x, __u32 y);
static void LCD_power_on(u32 sel);
static void LCD_power_off(u32 sel);
static void LCD_bl_open(u32 sel);
static void LCD_bl_close(u32 sel);

static void LCD_panel_init(u32 sel);
static void LCD_panel_exit(u32 sel);

#define lh219wq1_reset_1  sunxi_lcd_gpio_set_value(0, 0, 1)
#define lh219wq1_reset_0  sunxi_lcd_gpio_set_value(0, 0, 0)

#define lh219wq1_cs_1  sunxi_lcd_gpio_set_value(0, 1, 1)
#define lh219wq1_cs_0  sunxi_lcd_gpio_set_value(0, 1, 0)

#if TCON_DEBUG
extern s32 tcon0_cpu_rd_24b_data(u32 sel, u32 index, u32 *data, u32 size);

void dump_tcon_register(void)
{
	volatile int *tcon_reg = (int *)(0x06511068);
	int size = 0x180>>2;
	int val, i, sel;
	char output[128];
	char *p;

	while (size > 0) {
		sel = size > 8 ? 8 : size;
		size -= sel;

		p = output;
		p += sprintf(p, "0x%08x:", (int)tcon_reg);
		for (i = 0; i < sel; i++) {
			val = *(tcon_reg);
			p += sprintf(p, " %08x", val);

			tcon_reg++;
		}
		printk("%s\n", output);
	}
}
#endif

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


	items = sizeof(lcd_gamma_tbl)/2;
	for (i = 0; i < items - 1; i++) {
		u32 num = lcd_gamma_tbl[i+1][0] - lcd_gamma_tbl[i][0];

		for (j = 0; j < num; j++) {
			u32 value = 0;

			value = lcd_gamma_tbl[i][1] + ((lcd_gamma_tbl[i+1][1] - lcd_gamma_tbl[i][1]) * j)/num;
			info->lcd_gamma_tbl[lcd_gamma_tbl[i][0] + j] = (value<<16) + (value<<8) + value;
		}
	}
	info->lcd_gamma_tbl[255] = (lcd_gamma_tbl[items-1][1]<<16) + (lcd_gamma_tbl[items-1][1]<<8) + lcd_gamma_tbl[items-1][1];

	memcpy(info->lcd_cmap_tbl, lcd_cmap_tbl, sizeof(lcd_cmap_tbl));
}

static s32 LCD_open_flow(u32 sel)
{
	LCD_OPEN_FUNC(sel, LCD_power_on, 250);
#ifdef CPU_TRI_MODE
	LCD_OPEN_FUNC(sel, LCD_panel_init, 300);
	LCD_OPEN_FUNC(sel, sunxi_lcd_tcon_enable, 150);
#else
	LCD_OPEN_FUNC(sel, sunxi_lcd_tcon_enable, 150);
	LCD_OPEN_FUNC(sel, LCD_panel_init, 300);
#endif
	LCD_OPEN_FUNC(sel, LCD_bl_open, 0);

	return 0;
}

static s32 LCD_close_flow(u32 sel)
{
	LCD_CLOSE_FUNC(sel, LCD_bl_close, 50);
#ifdef CPU_TRI_MODE
	LCD_CLOSE_FUNC(sel, sunxi_lcd_tcon_disable, 10);
	LCD_CLOSE_FUNC(sel, LCD_panel_exit,	10);
#else
	LCD_CLOSE_FUNC(sel, LCD_panel_exit,	10);
	LCD_CLOSE_FUNC(sel, sunxi_lcd_tcon_disable, 10);
#endif
	LCD_CLOSE_FUNC(sel, LCD_power_off, 10);

	return 0;
}

static void LCD_power_on(u32 sel)
{
	sunxi_lcd_power_enable(sel, 0);
	sunxi_lcd_power_enable(sel, 1);
	sunxi_lcd_power_enable(sel, 2);
	lh219wq1_cs_0;
	sunxi_lcd_delay_ms(10);
	sunxi_lcd_pin_cfg(sel, 1);
    LCD_bl_close(sel);
	sunxi_lcd_delay_ms(10);
}

static void LCD_power_off(u32 sel)
{
	sunxi_lcd_pin_cfg(sel, 0);
	sunxi_lcd_delay_ms(10);
	lh219wq1_cs_1;
	sunxi_lcd_power_disable(sel, 0);
	sunxi_lcd_power_disable(sel, 1);
	sunxi_lcd_power_disable(sel, 2);
	sunxi_lcd_delay_ms(10);
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
	disp_panel_para *info = kmalloc(sizeof(disp_panel_para), GFP_KERNEL | __GFP_ZERO);

    bsp_disp_get_panel_info(sel, info);
	lcd_panel_lh219wq1_init(info);

	if (LCD_CPU_AUTO_MODE == info->lcd_cpu_mode) {
		sunxi_lcd_cpu_set_auto_mode(sel);
	}

	kfree(info);
	return;
}

static void LCD_panel_exit(u32 sel)
{
	disp_panel_para *info = kmalloc(sizeof(disp_panel_para), GFP_KERNEL | __GFP_ZERO);

	//pr_info("%s: enter sleep\n", __func__);
	sunxi_lcd_cpu_write_index(0, 0x28);
	sunxi_lcd_cpu_write_index(0, 0x10);
	sunxi_lcd_delay_ms(300);
	lh219wq1_reset_1;
	sunxi_lcd_gpio_set_value(0, 2, 1);
	bsp_disp_get_panel_info(sel, info);
	kfree(info);
	return ;
}

static void lcd_dbi_wr_dcs(__u32 sel, __u8 cmd, __u8 *para, __u32 para_num)
{
	__u8 index		= cmd;
	__u8 *data_p	= para;
	__u16 i;
	sunxi_lcd_cpu_write_index(sel, index);
	for (i = 0; i < para_num; i++) {
		sunxi_lcd_cpu_write_data(sel, *(data_p++));
	}
}

static void lcd_cpu_panel_fr(__u32 sel, __u32 w, __u32 h, __u32 x, __u32 y)
{
	__u8 para[4];
	__u32 para_num;

	para[0] = (x >> 8)		& 0xff;
	para[1] = (x >> 0) 		& 0xff;
	para[2] = ((x + w - 1) >> 8) 	& 0xff;
	para[3] = ((x + w - 1) >> 0) 	& 0xff;
	para_num = 4;
	lcd_dbi_wr_dcs(sel, DSI_DCS_SET_COLUMN_ADDRESS, para, para_num);

	para[0] = (y >> 8)		& 0xff;
	para[1] = (y >> 0) 		& 0xff;
	para[2] = ((y + h - 1) >> 8) 	& 0xff;
	para[3] = ((y + h - 1) >> 0) 	& 0xff;
	para_num = 4;
	lcd_dbi_wr_dcs(sel, DSI_DCS_SET_PAGE_ADDRESS, para, para_num);

	para_num = 0;
	lcd_dbi_wr_dcs(sel, DSI_DCS_WRITE_MEMORY_START, para, para_num);
}

static int lcd_dismode, lcd_x, lcd_y;

void lcd_rotation_lh219wq1(int mode)
{
	lcd_dismode = mode;
}

void lcd_panel_lh219wq1_init(disp_panel_para *info)
{
#if TCON_DEBUG
	u32 id[5];
	dump_tcon_register();
#endif

	/* hardware reset */
	lh219wq1_reset_1;
	sunxi_lcd_gpio_set_value(0, 2, 1);
	sunxi_lcd_delay_ms(10);
	lh219wq1_reset_0;
	sunxi_lcd_gpio_set_value(0, 2, 0);
	sunxi_lcd_delay_ms(50);
	lh219wq1_reset_1;
	sunxi_lcd_gpio_set_value(0, 2, 1);
	sunxi_lcd_delay_ms(120);
	sunxi_lcd_cpu_write_index(0, 0x11);
	sunxi_lcd_delay_ms(200);
	sunxi_lcd_cpu_write_index(0, 0x35);
	sunxi_lcd_cpu_write_data(0, 0x00);
	sunxi_lcd_cpu_write_index(0, 0x36);
	sunxi_lcd_cpu_write_data(0, 0x00);
	sunxi_lcd_cpu_write_index(0, 0x3A);
	sunxi_lcd_cpu_write_data(0, 0x66);
    sunxi_lcd_cpu_write_index(0, 0x2A);
    sunxi_lcd_cpu_write_data(0, 0x00);
    sunxi_lcd_cpu_write_data(0, 0x00);
    sunxi_lcd_cpu_write_data(0, 0x00);
    sunxi_lcd_cpu_write_data(0, 0xEF);
    sunxi_lcd_cpu_write_index(0, 0x2B);
    sunxi_lcd_cpu_write_data(0, 0x00);
    sunxi_lcd_cpu_write_data(0, 0x00);
    sunxi_lcd_cpu_write_data(0, 0x01);
    sunxi_lcd_cpu_write_data(0, 0x77);

    sunxi_lcd_cpu_write_index(0, 0xB1);
    sunxi_lcd_cpu_write_data(0, 0x11);
    sunxi_lcd_cpu_write_data(0, 0x20);
    sunxi_lcd_cpu_write_index(0, 0xB2);
    sunxi_lcd_cpu_write_data(0, 0x11);
    sunxi_lcd_cpu_write_data(0, 0x20);
    sunxi_lcd_cpu_write_index(0, 0xB3);
    sunxi_lcd_cpu_write_data(0, 0x11);
    sunxi_lcd_cpu_write_data(0, 0x20);
    sunxi_lcd_cpu_write_index(0, 0xB4);
    sunxi_lcd_cpu_write_data(0, 0x02);
    sunxi_lcd_cpu_write_index(0, 0xB6);
    sunxi_lcd_cpu_write_data(0, 0x01);
    sunxi_lcd_cpu_write_data(0, 0x02);
    sunxi_lcd_cpu_write_index(0, 0xC0);
    sunxi_lcd_cpu_write_data(0, 0x24);
    sunxi_lcd_cpu_write_index(0, 0xC1);
    sunxi_lcd_cpu_write_data(0, 0x02);
	sunxi_lcd_cpu_write_data(0, 0x00);
    sunxi_lcd_cpu_write_index(0, 0xC2);
	sunxi_lcd_cpu_write_data(0, 0x05);
	sunxi_lcd_cpu_write_data(0, 0x01);
    sunxi_lcd_cpu_write_index(0, 0xC3);
    sunxi_lcd_cpu_write_data(0, 0x02);
    sunxi_lcd_cpu_write_data(0, 0x05);
    sunxi_lcd_cpu_write_index(0, 0xC4);
    sunxi_lcd_cpu_write_data(0, 0x02);
    sunxi_lcd_cpu_write_data(0, 0x04);
    sunxi_lcd_cpu_write_index(0, 0xC5);
    sunxi_lcd_cpu_write_data(0, 0x14);
    sunxi_lcd_cpu_write_data(0, 0x2e);

    sunxi_lcd_cpu_write_index(0, 0xEA);
    sunxi_lcd_cpu_write_data(0, 0x01);
	sunxi_lcd_cpu_write_index(0, 0xE0);
	sunxi_lcd_cpu_write_data(0, 0x27);
	sunxi_lcd_cpu_write_data(0, 0x2B);
	sunxi_lcd_cpu_write_data(0, 0x2E);
	sunxi_lcd_cpu_write_data(0, 0x6);
	sunxi_lcd_cpu_write_data(0, 0xD);
	sunxi_lcd_cpu_write_data(0, 0x11);
	sunxi_lcd_cpu_write_data(0, 0x28);
	sunxi_lcd_cpu_write_data(0, 0x7B);
	sunxi_lcd_cpu_write_data(0, 0x35);
	sunxi_lcd_cpu_write_data(0, 0xC);
	sunxi_lcd_cpu_write_data(0, 0x20);
	sunxi_lcd_cpu_write_data(0, 0x26);
	sunxi_lcd_cpu_write_data(0, 0x25);
	sunxi_lcd_cpu_write_data(0, 0x28);
	sunxi_lcd_cpu_write_data(0, 0x3C);
	sunxi_lcd_cpu_write_index(0, 0xE1);
	sunxi_lcd_cpu_write_data(0, 0x8);
	sunxi_lcd_cpu_write_data(0, 0x21);
	sunxi_lcd_cpu_write_data(0, 0x26);
	sunxi_lcd_cpu_write_data(0, 0x9);
	sunxi_lcd_cpu_write_data(0, 0xF);
	sunxi_lcd_cpu_write_data(0, 0x12);
	sunxi_lcd_cpu_write_data(0, 0x1F);
	sunxi_lcd_cpu_write_data(0, 0x38);
	sunxi_lcd_cpu_write_data(0, 0x31);
	sunxi_lcd_cpu_write_data(0, 0xD);
	sunxi_lcd_cpu_write_data(0, 0x23);
	sunxi_lcd_cpu_write_data(0, 0x29);
	sunxi_lcd_cpu_write_data(0, 0x2C);
	sunxi_lcd_cpu_write_data(0, 0x2F);
	sunxi_lcd_cpu_write_data(0, 0x33);
	sunxi_lcd_cpu_write_index(0, 0xE2);
	sunxi_lcd_cpu_write_data(0, 0x27);
	sunxi_lcd_cpu_write_data(0, 0x2C);
	sunxi_lcd_cpu_write_data(0, 0x2F);
	sunxi_lcd_cpu_write_data(0, 0x7);
	sunxi_lcd_cpu_write_data(0, 0xE);
	sunxi_lcd_cpu_write_data(0, 0x11);
	sunxi_lcd_cpu_write_data(0, 0x29);
	sunxi_lcd_cpu_write_data(0, 0x7A);
	sunxi_lcd_cpu_write_data(0, 0x35);
	sunxi_lcd_cpu_write_data(0, 0xC);
	sunxi_lcd_cpu_write_data(0, 0x20);
	sunxi_lcd_cpu_write_data(0, 0x26);
	sunxi_lcd_cpu_write_data(0, 0x24);
	sunxi_lcd_cpu_write_data(0, 0x29);
	sunxi_lcd_cpu_write_data(0, 0x3C);
	sunxi_lcd_cpu_write_index(0, 0xE3);
	sunxi_lcd_cpu_write_data(0, 0x8);
	sunxi_lcd_cpu_write_data(0, 0x20);
	sunxi_lcd_cpu_write_data(0, 0x26);
	sunxi_lcd_cpu_write_data(0, 0x9);
	sunxi_lcd_cpu_write_data(0, 0xF);
	sunxi_lcd_cpu_write_data(0, 0x12);
	sunxi_lcd_cpu_write_data(0, 0x1F);
	sunxi_lcd_cpu_write_data(0, 0x48);
	sunxi_lcd_cpu_write_data(0, 0x30);
	sunxi_lcd_cpu_write_data(0, 0xD);
	sunxi_lcd_cpu_write_data(0, 0x22);
	sunxi_lcd_cpu_write_data(0, 0x28);
	sunxi_lcd_cpu_write_data(0, 0x2B);
	sunxi_lcd_cpu_write_data(0, 0x2E);
	sunxi_lcd_cpu_write_data(0, 0x33);
	sunxi_lcd_cpu_write_index(0, 0xE4);
	sunxi_lcd_cpu_write_data(0, 0x1F);
	sunxi_lcd_cpu_write_data(0, 0x24);
	sunxi_lcd_cpu_write_data(0, 0x27);
	sunxi_lcd_cpu_write_data(0, 0x8);
	sunxi_lcd_cpu_write_data(0, 0xF);
	sunxi_lcd_cpu_write_data(0, 0x12);
	sunxi_lcd_cpu_write_data(0, 0x25);
	sunxi_lcd_cpu_write_data(0, 0x7B);
	sunxi_lcd_cpu_write_data(0, 0x32);
	sunxi_lcd_cpu_write_data(0, 0xC);
	sunxi_lcd_cpu_write_data(0, 0x20);
	sunxi_lcd_cpu_write_data(0, 0x26);
	sunxi_lcd_cpu_write_data(0, 0x20);
	sunxi_lcd_cpu_write_data(0, 0x25);
	sunxi_lcd_cpu_write_data(0, 0x3C);
	sunxi_lcd_cpu_write_index(0, 0xE5);
	sunxi_lcd_cpu_write_data(0, 0x8);
	sunxi_lcd_cpu_write_data(0, 0x24);
	sunxi_lcd_cpu_write_data(0, 0x2B);
	sunxi_lcd_cpu_write_data(0, 0x9);
	sunxi_lcd_cpu_write_data(0, 0xF);
	sunxi_lcd_cpu_write_data(0, 0x12);
	sunxi_lcd_cpu_write_data(0, 0x22);
	sunxi_lcd_cpu_write_data(0, 0x38);
	sunxi_lcd_cpu_write_data(0, 0x35);
	sunxi_lcd_cpu_write_data(0, 0xC);
	sunxi_lcd_cpu_write_data(0, 0x21);
	sunxi_lcd_cpu_write_data(0, 0x27);
	sunxi_lcd_cpu_write_data(0, 0x33);
	sunxi_lcd_cpu_write_data(0, 0x36);
	sunxi_lcd_cpu_write_data(0, 0x3B);
	sunxi_lcd_cpu_write_index(0, 0xF0) ;
	sunxi_lcd_cpu_write_data(0, 0xaa);
	sunxi_lcd_cpu_write_data(0, 0x55);
	sunxi_lcd_cpu_write_data(0, 0x25);
	sunxi_lcd_cpu_write_index(0, 0xF6) ;
	sunxi_lcd_cpu_write_data(0, 0x7f);
	sunxi_lcd_cpu_write_data(0, 0x8a);
	sunxi_lcd_cpu_write_data(0, 0x34);
	sunxi_lcd_cpu_write_data(0, 0x00);
	sunxi_lcd_cpu_write_data(0, 0x04);
	sunxi_lcd_cpu_write_data(0, 0x26);
	sunxi_lcd_cpu_write_data(0, 0x07);
	sunxi_lcd_cpu_write_index(0, 0x11);
    sunxi_lcd_delay_ms(5);
    sunxi_lcd_cpu_write_index(0, 0x29);
    sunxi_lcd_delay_ms(5);

#if TCON_DEBUG
	tcon0_cpu_rd_24b_data(0, 0x0e, id, 2);
	pr_err("lh219wq1 read signal mode:\n");
	pr_err("lh219wq1 0x%02x 0x%02x\n", id[1], id[0]);
#endif

    lcd_cpu_panel_fr(0, info->lcd_x, info->lcd_y, 0, 0);
	lcd_x = info->lcd_x;
	lcd_y = info->lcd_y;
}

void lcd_reflush_lh219wq1(void)
{

}

static s32 LCD_user_defined_func(u32 sel, u32 para1, u32 para2, u32 para3)
{
	return 0;
}

__lcd_panel_t lh219wq1_panel = {
	/* panel driver name, must mach the name of lcd_drv_name in sys_config.fex */
	.name = "lh219wq1",
	.func = {
		.cfg_panel_info = lcd_cfg_panel_info,
		.cfg_open_flow = LCD_open_flow,
		.cfg_close_flow = LCD_close_flow,
		.lcd_user_defined_func = LCD_user_defined_func,
	},
};
