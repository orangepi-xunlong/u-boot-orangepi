#ifndef __PWM_LED_H__
#define __PWM_LED_H__

struct pwm_led {
	int pwm_num;
	int value;
	char label[16];
};

struct pwm_led_rgb {
	struct pwm_led r;
	struct pwm_led g;
	struct pwm_led b;
};


int pwm_led_init(void);
int pwm_led_test(void);
int pwm_led_set(struct pwm_led_rgb led_rgb);

#endif
