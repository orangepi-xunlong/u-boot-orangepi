

#ifndef __TCON_H__
#define __TCON_H__

enum __lcd_src_t {
	LCD_SRC_DE = 0,
	LCD_SRC_COLOR_BAR = 1,
	LCD_SRC_GRAYSCALE = 2,
	LCD_SRC_BLACK_BY_WHITE = 3,
	LCD_SRC_BLACK = 4,
	LCD_SRC_WHITE = 5,
	LCD_SRC_GRID = 7,
	LCD_SRC_BLUE = 8
};

enum __de_perh_t {
	LCD0 = 0,
	LCD1 = 1,
	TV0 = 2,
	TV1 = 3
};

enum __lcd_irq_id_t {
	LCD_IRQ_TCON0_VBLK = 15,
	LCD_IRQ_TCON1_VBLK = 14,
	LCD_IRQ_TCON0_LINE = 13,
	LCD_IRQ_TCON1_LINE = 12,
	LCD_IRQ_TCON0_TRIF = 11,
	LCD_IRQ_TCON0_CNTR = 10,
};

struct disp_video_timings
{
        unsigned int    vic;  //video infomation code
        unsigned int    tv_mode;
        unsigned int    pixel_clk;//khz
        unsigned int    pixel_repeat;//pixel repeat (pixel_repeat+1) times
        unsigned int    x_res;
        unsigned int    y_res;
        unsigned int    hor_total_time;
        unsigned int    hor_back_porch;
        unsigned int    hor_front_porch;
        unsigned int    hor_sync_time;
        unsigned int    ver_total_time;
        unsigned int    ver_back_porch;
        unsigned int    ver_front_porch;
        unsigned int    ver_sync_time;
        unsigned int    hor_sync_polarity;//0: negative, 1: positive
        unsigned int    ver_sync_polarity;//0: negative, 1: positive
        bool            b_interlace;
        unsigned int    vactive_space;
        unsigned int    trd_mode;
};

s32 tcon_init(u32 sel);
s32 tcon1_open(u32 sel);
s32 tcon1_cfg(u32 sel, struct disp_video_timings *timing);
s32 tcon1_set_timming(u32 sel, struct disp_video_timings *timming);
s32 tcon1_src_select(u32 sel, enum __lcd_src_t src, enum __de_perh_t de_no);
s32 tcon_irq_enable(u32 sel, enum __lcd_irq_id_t id);
s32 tcon_top_set_reg_base(u32 sel, u32 base);
s32 tcon_set_reg_base(u32 sel, u32 base);
s32 tcon_de_attach(u32 tcon_index, u32 de_index);
s32 tcon1_hdmi_clk_enable(u32 sel, u32 en);
s32 tcon1_black_src(u32 sel, u32 on_off, u32 color);


#endif
