// SPDX-License-Identifier: GPL-2.0+
/*
 * sunxi LEDC driver for uboot.
 *
 * Copyright (C) 2018
 * 2019.1.7 lihuaxing <lihuxing@allwinnertech.com>
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <led.h>
#include <asm/io.h>
#include <dm/lists.h>

#include <asm/arch/gpio.h>
#include <sunxi_ledc.h>
#include <fdt_support.h>

static int sunxi_ledc_pin_init(void)
{
	int ledc_nodeoffset;

	ledc_nodeoffset =  fdt_path_offset(working_fdt, "ledc");

	if (ledc_nodeoffset < 0) {
		printf("ledc: get node offset error\n");
		return -1;
	}

	if (fdt_set_all_pin("ledc", "pinctrl-0")) {
		printf("ledc pin init failed!\n");
		return -1;
	}

	return 0;
}

static int ledc_set_reset_ns(void)
{
	struct sunxi_ledc_reg *ledc = (struct sunxi_ledc_reg *)(LEDC_BASE_ADDR);
	uint32_t mask = 0x3FFF;
	int shift = 16;
	int n = 1;          //42*(6+1)

	ledc->rst_t &= ~(mask << shift);
	ledc->rst_t |= n << shift;

	return 0;
}


static int ledc_set_t01_ns(void)
{
	struct sunxi_ledc_reg *ledc = (struct sunxi_ledc_reg *)(LEDC_BASE_ADDR);
	uint32_t mask0 = 0x7FF;
	int n0l = 19;
	int n0h = 8;

	uint32_t mask1 = 0x7FF;
	int shift1 = 16;
	int n1l = 9;
	int n1h = 18;

	ledc->time01 &= ~mask0;
	ledc->time01 |= ((n0h << 6)|(n0l));

	ledc->time01 &= ~(mask1 << shift1);
	ledc->time01 |= ((n1h << 21)|(n1l << 16));

	return 0;
}

static int ledc_set_wait0_ns(void)
{
	struct sunxi_ledc_reg *ledc = (struct sunxi_ledc_reg *)(LEDC_BASE_ADDR);
	uint32_t mask = 0xFF;
	int shift = 0;
	int n = 1;

	ledc->t0_ctrl &= ~(mask << shift);
	ledc->t0_ctrl |= n << shift;
	return 0;
}

static int ledc_set_wait1_ns(void)
{
	struct sunxi_ledc_reg *ledc = (struct sunxi_ledc_reg *)(LEDC_BASE_ADDR);
	uint32_t mask = 0x7FFFFFFF;
	int shift = 0;
	int n = 1;

	ledc->t1_ctrl &= ~(mask << shift);
	ledc->t1_ctrl |= n << shift;
	return 0;
}

static void ledc_irq_enable(uint32_t mask)
{
	struct sunxi_ledc_reg *ledc = (struct sunxi_ledc_reg *)(LEDC_BASE_ADDR);

	ledc->irq_ctrl |= mask;
}

static void ledc_irq_disable(uint32_t mask)
{
	struct sunxi_ledc_reg *ledc = (struct sunxi_ledc_reg *)(LEDC_BASE_ADDR);

	ledc->irq_ctrl &= ~mask;
}

static void sunxi_ledc_enable(void)
{
	struct sunxi_ledc_reg *ledc = (struct sunxi_ledc_reg *)(LEDC_BASE_ADDR);

	ledc->ctrl |= 1;
}

static void sunxi_ledc_reset(void)
{
	struct sunxi_ledc_reg *ledc = (struct sunxi_ledc_reg *)(LEDC_BASE_ADDR);

	ledc_irq_disable(LEDC_TRANS_FINISH_INT_EN | LEDC_FIFO_CPUREQ_INT_EN
		| LEDC_WAITDATA_TIMEOUT_INT_EN | LEDC_FIFO_OVERFLOW_INT_EN
		| LEDC_GLOBAL_INT_EN);
	ledc->ctrl |= 1 << 1;
}

static void sunxi_ledc_set_output_mode(void)
{
	struct sunxi_ledc_reg *ledc = (struct sunxi_ledc_reg *)(LEDC_BASE_ADDR);
	uint32_t mask = 0x7;
	int shift = 6;

	ledc->ctrl &= ~(mask << shift);
	ledc->ctrl |= (0 << shift);
}

static void sunxi_ledc_set_cpu_mode(void)
{
	struct sunxi_ledc_reg *ledc = (struct sunxi_ledc_reg *)(LEDC_BASE_ADDR);
	int shift = 5;

	ledc->DMA_ctrl &= ~(1 << shift);
	ledc_irq_enable(LEDC_FIFO_CPUREQ_INT_EN);
}

static void sunxi_ledc_set_length(unsigned int length)
{
	struct sunxi_ledc_reg *ledc = (struct sunxi_ledc_reg *)(LEDC_BASE_ADDR);
	uint32_t mask = 0x1FFF;
	int shift = 16;

	ledc->ctrl &= ~(mask << shift);
	ledc->ctrl |= length << shift;     //data length

	ledc->rst_t &= ~(0x3FF);
	ledc->rst_t |= length - 1;         //lights num
}

/*
static void sunxi_ledc_set_color(unsigned int g, unsigned int r, unsigned int b)
{
	uint32_t mask = ((g << 16)|(r << 8)|(b));
	data[i] = mask;
}
*/

static void sunxi_ledc_write_data(unsigned int length,
		unsigned int g, unsigned int r, unsigned int b)
{
	struct sunxi_ledc_reg *ledc = (struct sunxi_ledc_reg *)(LEDC_BASE_ADDR);
	int i;

	for (i = 0; i < length; i++)
		ledc->data_reg = ((g << 16)|(r << 8)|(b));
}

static void sunxi_ledc_clk_init(void)
{
	struct sunxi_ccmu_reg *clk = (struct sunxi_ccmu_reg *)(LEDC_CLOCK_ADDR);

	clk->model_clk = (1 << 31)|(0 << 24)|(0 << 8)|(0);

	clk->bus_gate |= (0 << 16);
	clk->bus_gate |= (1 << 16);

	clk->bus_gate |= (1);
}

static void sunxi_ledc_clk_exit(void)
{
	struct sunxi_ccmu_reg *clk = (struct sunxi_ccmu_reg *)(LEDC_CLOCK_ADDR);
	clk->model_clk = 0;

	clk->bus_gate |= (0);
	clk->bus_gate |= (0 << 16);
}

/* this api is use to ctrol the light.
*@length : the num of light.
*@g :      the green color
*@r :      the red   color.
*@b :      the blue  color.
*e.g:  sunxi_set_led_brightness(5, 255, 255, 255);
*/
int sunxi_set_led_brightness(unsigned int length, unsigned int g, unsigned int r, unsigned int b)
{
	sunxi_ledc_set_output_mode();
	sunxi_ledc_set_cpu_mode();
	sunxi_ledc_set_length(length);

	ledc_irq_enable(LEDC_TRANS_FINISH_INT_EN | LEDC_WAITDATA_TIMEOUT_INT_EN
			| LEDC_FIFO_OVERFLOW_INT_EN | LEDC_GLOBAL_INT_EN);
	sunxi_ledc_enable();

	sunxi_ledc_write_data(length, g, r, b);

	return 0;
}

/*init*/
int sunxi_ledc_init(void)
{
	sunxi_ledc_clk_init();
	sunxi_ledc_pin_init();

	ledc_set_reset_ns();
	ledc_set_t01_ns();
	ledc_set_wait0_ns();
	ledc_set_wait1_ns();

	printf("ledc_init ok\n");
	return 0;
}

/*exit*/
int sunxi_ledc_exit(void)
{
	sunxi_ledc_reset();
	sunxi_ledc_clk_exit();
	return 0;
}

/*e.g*/
#if 0
main()
{
	sunxi_ledc_init();
	sunxi_set_led_brightness(3, 100, 255, 0);

	sunxi_ledc_exit();
}
#endif
