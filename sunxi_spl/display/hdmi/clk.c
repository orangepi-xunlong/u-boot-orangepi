#include "common.h"

#define PLL_DE_CLK_REG_BASE 	          0x03001060
#define PLL_VIDEO1_CLK_REG_BASE   	  	  0x03001048
#define DE_CLK_GATE_BUS_REG_BASE	   	  0x0300160c
#define DE_CLK_GATE_ENABLE_REG_BASE   	  0x03001600
#define	TCON_TV_CLK_REG_BASE              0x03001b80
#define	TCON_TV_CLK_GATE_BUS_REG_BASE     0x03001b9c
#define HDMI_DDC_CLK_REG_BASE          	  0x03001b04
#define HDMI_CLK_GATE_BUS_REG_BASE        0x03001b1c
#define HDMI_CLK_GATE_ENABLE_REG_BASE     0x03001b00
#define TCON_TOP_CLK_GATE_BUS_REG_BASE    0x03001b5c

#define GPIO_DDC_GROUP_FUNC_ADDR 	0x0300b100
#define GPIO_DDC_GROUP_PULL_ADDR 	0x0300b118
#define GPIO_DDC_GROUP_DLEVEL_ADDR  0x0300b110

void hdmi_clk_enable (void)
{
	volatile unsigned int *pll_video1_reg = (unsigned int *)PLL_VIDEO1_CLK_REG_BASE;
	volatile unsigned int *hdmi_clk_gate_bus_reg = (unsigned int *)HDMI_CLK_GATE_BUS_REG_BASE;
	volatile unsigned int *hdmi_clk_gate_enable_reg = (unsigned int *)HDMI_CLK_GATE_ENABLE_REG_BASE;

	*pll_video1_reg = 0x80006201;
	*hdmi_clk_gate_bus_reg = 0x00030001;
	*hdmi_clk_gate_enable_reg = 0x81000000;
}

void de_clk_enable (void)
{
	volatile unsigned int *pll_de_reg = (unsigned int *)PLL_DE_CLK_REG_BASE;
	volatile unsigned int *de_clk_gate_bus_reg = (unsigned int *)DE_CLK_GATE_BUS_REG_BASE;
	volatile unsigned int *de_clk_gate_enable_reg = (unsigned int *)DE_CLK_GATE_ENABLE_REG_BASE;

	*pll_de_reg = 0x80001c00;
	*de_clk_gate_bus_reg = 0x00010001;
	*de_clk_gate_enable_reg = 0x80000000;
}

void hdmi_ddc_clk_enable (void)
{
	volatile unsigned int *hdmi_ddc_clk_reg = (unsigned int *)HDMI_DDC_CLK_REG_BASE;

	*hdmi_ddc_clk_reg = 0x80000000;
}

void hdmi_ddc_gpio_enable (void)
{
	volatile unsigned int *gpio_ddc_group_func_reg = (unsigned int *)GPIO_DDC_GROUP_FUNC_ADDR;
	volatile unsigned int *gpio_ddc_group_pull_reg = (unsigned int *)GPIO_DDC_GROUP_PULL_ADDR;
	volatile unsigned int *gpio_ddc_group_dlevel_reg = (unsigned int *)GPIO_DDC_GROUP_DLEVEL_ADDR;

	*gpio_ddc_group_func_reg   = 0x722;
	*gpio_ddc_group_pull_reg   = 0x5;
	*gpio_ddc_group_dlevel_reg = 0x15555f;

}

void tcon_tv_clk_config (u32 rate)
{
	volatile unsigned int *tcon_tv_reg = (unsigned int *)TCON_TV_CLK_REG_BASE;
	volatile unsigned int *tcon_tv_clk_gate_bus_reg = (unsigned int *)TCON_TV_CLK_GATE_BUS_REG_BASE;

	switch (rate) {
		case 148500000:
			*tcon_tv_reg = 0x82000003;
			break;
		case 297000000:
			*tcon_tv_reg = 0x82000001;
			break;
		case 74250000:
			*tcon_tv_reg = 0x82000007;
			break;
		case 594000000:
			*tcon_tv_reg = 0x82000000;
			break;
		case 27027000:
		case 27000000:
			*tcon_tv_reg = 0x8200010a;
			break;
		default:
			*tcon_tv_reg = 0x82000003;
			break;
	}
	*tcon_tv_clk_gate_bus_reg = 0x00010001;
}

void tcon_top_clk_enable (void)
{
	volatile unsigned int *tcon_top_clk_gate_bus_reg = (unsigned int *)TCON_TOP_CLK_GATE_BUS_REG_BASE;

	*tcon_top_clk_gate_bus_reg = 0x00010001;
}
