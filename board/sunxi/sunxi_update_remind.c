// SPDX-License-Identifier: GPL-2.0+

#include <common.h>
#include <fdt_support.h>
#include <asm/gpio.h>
#include <asm/arch/timer.h>
#include <pwm.h>
#ifdef CONFIG_SUNXI_LEDC
#include <sunxi_ledc.h>
#endif

static void sunxi_update_remind_GPIO_LED(int node)
{
	int i;
	u32 gpio_led_port[8];
	u32 led_port;

	if (fdt_getprop_u32(working_fdt, node, "gpio_led_port", gpio_led_port) < 0) {
		printf("can't get gpio_led_port\n");
	} else {
		led_port = (gpio_led_port[1] << 5) | gpio_led_port[2];
		gpio_direction_output(led_port, 1);
		for (i = 3; i > 0; i--) {
			__msdelay(1000);
			gpio_direction_output(led_port, 0);
			__msdelay(1000);
			gpio_direction_output(led_port, 1);
		}
	}
}

#define PWM_LED_PERIOD (10000)
#define MAX_VALUE (255)

struct pwm_led {
	int pwm_num;
	int value;
	char label[16];
};

static int _set_one_pwm_led(struct pwm_led led)
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

static void sunxi_update_remind_PWM_LED(int node)
{
	int i;
	struct pwm_led remind_pwm_led;
	u32 pwm_channel;
	if (fdt_getprop_u32(working_fdt, node, "pwm_led_channel", &pwm_channel) < 0) {
		printf("can't get pwm_channel\n");
		return;
	} else {
		pwm_init();
		remind_pwm_led.pwm_num = pwm_channel;
		pwm_request(remind_pwm_led.pwm_num, "pwm_led");

		remind_pwm_led.value = 255;
		_set_one_pwm_led(remind_pwm_led);

		for (i = 3; i > 0; i--) {
			__msdelay(1000);
			remind_pwm_led.value = 0;
			_set_one_pwm_led(remind_pwm_led);
			__msdelay(1000);
			remind_pwm_led.value = 255;
			_set_one_pwm_led(remind_pwm_led);
		}
	}
}

#ifdef CONFIG_SUNXI_LEDC

static void sunxi_update_remind_LEDC_LED(int node)
{
	int i;
	int led_count = 1;
	int led_g1 = 255, led_r1 = 0, led_b1 = 0;
	int led_g2 = 0, led_r2 = 0, led_b2 = 0;

	sunxi_ledc_init();

	sunxi_set_led_brightness(led_count, led_g1, led_r1, led_b1);
	for (i = 3; i > 0; i--) {
		__msdelay(1000);
		sunxi_set_led_brightness(led_count, led_g2, led_r2, led_b2);
		__msdelay(1000);
		sunxi_set_led_brightness(led_count, led_g1, led_r1, led_b1);
	}
}

#endif

void sunxi_update_remind(void)
{
	printf("==================================================\n");
	printf("|                                                |\n");
	printf("|                 update  finish                 |\n");
	printf("|                                                |\n");
	printf("==================================================\n");
	int nodeoffset;
	char *remind_type = NULL;
	nodeoffset = fdt_path_offset(working_fdt, "/soc/update_remind");
	if (nodeoffset > 0) {
			fdt_getprop_string(working_fdt, nodeoffset, "remind_type", &remind_type);
			if (strcmp(remind_type, "GPIO_LED") == 0) {
				printf("remind_type : %s\n", remind_type);
				sunxi_update_remind_GPIO_LED(nodeoffset);
			} else if (strcmp(remind_type, "PWM_LED") == 0) {
				printf("remind_type : %s\n", remind_type);
				sunxi_update_remind_PWM_LED(nodeoffset);
#ifdef CONFIG_SUNXI_LEDC
			} else if (strcmp(remind_type, "LEDC_LED") == 0) {
				printf("remind_type : %s\n", remind_type);
				sunxi_update_remind_LEDC_LED(nodeoffset);
#endif
			} else {
				printf("remind_type : other\n");
				//If you use other reminders, you need to implement them yourself.
			}
	}
}
