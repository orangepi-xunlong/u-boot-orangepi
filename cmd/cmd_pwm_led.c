/*
 * (C) Copyright 2019
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#include <common.h>
#include <command.h>
#include <sunxi_board.h>
#include <pwm.h>
#include <pwm_led.h>
#include <fdt_support.h>
#include <asm/arch/timer.h>

#define PWM_LED_PERIOD (10000)
#define MAX_VALUE (255)

static struct pwm_led_rgb led_rgb_global;

int load_pwm_led_rgb_config_from_dtb(struct pwm_led_rgb *led_rgb)
{
	int node;
	u32 led_r, led_g, led_b;
	u32 led_r_pwm, led_g_pwm, led_b_pwm;
	int workmode = get_boot_work_mode();
	char *dev_name = "pwm_led";

	if (workmode != WORK_MODE_BOOT)
		return 0;

	node = fdt_node_offset_by_prop_value(working_fdt, -1, "device_type", dev_name, strlen(dev_name) + 1);
	if (node < 0) {
		printf("unable to find pwm led node in device tree.\n");
		return -1;
	}
	if (fdt_getprop_u32(working_fdt, node, "led_r_pwm", &led_r_pwm) < 0)
		led_r_pwm = -1;

	if (fdt_getprop_u32(working_fdt, node, "led_g_pwm", &led_g_pwm) < 0)
		led_g_pwm = -1;

	if (fdt_getprop_u32(working_fdt, node, "led_b_pwm", &led_b_pwm) < 0)
		led_b_pwm = -1;

	if (fdt_getprop_u32(working_fdt, node, "led_r", &led_r) < 0)
		led_r = 0;

	if (fdt_getprop_u32(working_fdt, node, "led_g", &led_g) < 0)
		led_g = 0;

	if (fdt_getprop_u32(working_fdt, node, "led_b", &led_b) < 0)
		led_b = 0;

	led_rgb->r.pwm_num = led_r_pwm;
	led_rgb->r.value   = led_r;
	strcpy(led_rgb->r.label, "led_red");

	led_rgb->g.pwm_num = led_g_pwm;
	led_rgb->g.value   = led_g;
	strcpy(led_rgb->r.label, "led_green");

	led_rgb->b.pwm_num = led_b_pwm;
	led_rgb->b.value   = led_b;
	strcpy(led_rgb->r.label, "led_blue");

	return 0;
}

int pwm_led_load(struct pwm_led_rgb *led_rgb)
{
	int ret;
	ret = load_pwm_led_rgb_config_from_dtb(led_rgb);
	return ret;
}

int store_led_values_to_dtb(struct pwm_led led_rgb)
{
	/* TODO */
	return 0;
}

int pwm_led_store(struct pwm_led led_rgb)
{
	int ret;
	ret = store_led_values_to_dtb(led_rgb);
	return ret;
}

int pwm_led_init(void)
{
	int ret, workmode;

	workmode = get_boot_work_mode();
	if (workmode != WORK_MODE_BOOT)
		return 0;

	ret = pwm_led_load(&led_rgb_global);
	if (ret)
		return 0;

	pwm_init();
	pwm_request(led_rgb_global.r.pwm_num, led_rgb_global.r.label);
	pwm_request(led_rgb_global.g.pwm_num, led_rgb_global.g.label);
	pwm_request(led_rgb_global.b.pwm_num, led_rgb_global.b.label);

	printf("init led: r:%d, g:%d b:%d\n", led_rgb_global.r.value, led_rgb_global.g.value, led_rgb_global.b.value);
	pwm_led_set(led_rgb_global);

	printf("pwm_led_init end\n");
	return 0;
}

static int set_one_pwm_led(struct pwm_led led)
{
	unsigned int duty_ns, period_ns, level;
	if (led.value != 0) {
		period_ns = PWM_LED_PERIOD;
		level = MAX_VALUE - led.value;
		if (level == 0) /* for pwm, duty_ns shoule not be zero */
			level = 1;
		duty_ns = (period_ns * level) / MAX_VALUE;
		pwm_config(led.pwm_num, duty_ns, period_ns);
		pwm_enable(led.pwm_num);
	} else {
		pwm_disable(led.pwm_num);
	}
	return 0;
}

int pwm_led_set(struct pwm_led_rgb led_rgb)
{
	set_one_pwm_led(led_rgb.r);
	set_one_pwm_led(led_rgb.g);
	set_one_pwm_led(led_rgb.b);

	/* pwm_led_store(led_rgb); */

	return 0;
}
int pwm_led_test(void)
{
	int i, workmode;

	workmode = get_boot_work_mode();
	if (workmode != WORK_MODE_BOOT)
		return 0;

	led_rgb_global.r.value = 0;
	led_rgb_global.g.value = 0;
	led_rgb_global.b.value = 0;

	printf("test red\n");
	for (i = 0; i <= MAX_VALUE; i++) {
		led_rgb_global.r.value = i;
		pwm_led_set(led_rgb_global);
		__msdelay(10);
	}
	for (i = 0; i <= MAX_VALUE; i++) {
		led_rgb_global.r.value = MAX_VALUE - i;
		pwm_led_set(led_rgb_global);
		__msdelay(10);
	}

	led_rgb_global.r.value = 0;
	led_rgb_global.g.value = 0;
	led_rgb_global.b.value = 0;

	printf("test green\n");
	for (i = 0; i <= MAX_VALUE; i++) {
		led_rgb_global.g.value = i;
		pwm_led_set(led_rgb_global);
		__msdelay(10);
	}
	for (i = 0; i <= MAX_VALUE; i++) {
		led_rgb_global.g.value = MAX_VALUE - i;
		pwm_led_set(led_rgb_global);
		__msdelay(10);
	}

	printf("test blue\n");
	for (i = 0; i <= MAX_VALUE; i++) {
		led_rgb_global.b.value = i;
		pwm_led_set(led_rgb_global);
		__msdelay(10);
	}
	for (i = 0; i <= MAX_VALUE; i++) {
		led_rgb_global.b.value = MAX_VALUE - i;
		pwm_led_set(led_rgb_global);
		__msdelay(10);
	}
	return 0;
}

int do_pwm_led(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{

	ulong red_value, blue_value, green_value;

	if (argc < 3) {
		pwm_led_test();
		return cmd_usage(cmdtp);
	}

	red_value = simple_strtoul(argv[1], NULL, 10);
	green_value = simple_strtoul(argv[2], NULL, 10);
	blue_value = simple_strtoul(argv[3], NULL, 10);

	if (red_value > MAX_VALUE)
		red_value = MAX_VALUE;
	else if (red_value < 0)
		red_value = 0;

	if (blue_value > MAX_VALUE)
		blue_value = MAX_VALUE;
	else if (blue_value < 0)
		blue_value = 0;

	if (green_value > MAX_VALUE)
		green_value = MAX_VALUE;
	else if (green_value < 0)
		green_value = 0;

	printf("init led: r:%ld, g:%ld b:%ld\n", red_value, green_value, blue_value);

	led_rgb_global.r.value = red_value;
	led_rgb_global.g.value = green_value;
	led_rgb_global.b.value = blue_value;
	pwm_led_set(led_rgb_global);
	return 0;
}

U_BOOT_CMD(
	pwm_led,	5,	1,	do_pwm_led,
	"pwm_led  - set pwm led\n",
	"pwm_led red<0-255> green<0-255> blue<0-255>\n"
);
