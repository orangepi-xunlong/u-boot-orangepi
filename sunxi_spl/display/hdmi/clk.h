#ifndef __CLK_H__
#define __CLK_H__

void hdmi_clk_enable (void);
void hdmi_ddc_clk_enable (void);
void tcon_tv_clk_config (u32 rate);
void de_clk_enable (void);
void tcon_top_clk_enable (void);
void hdmi_ddc_gpio_enable (void);

#endif
