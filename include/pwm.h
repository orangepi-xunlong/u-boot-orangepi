/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * header file for pwm driver.
 *
 * Copyright 2016 Google Inc.
 * Copyright (c) 2011 samsung electronics
 * Donghwa Lee <dh09.lee@samsung.com>
 */

#ifndef _pwm_h_
#define _pwm_h_

#if 0
/* struct pwm_ops: Operations for the PWM uclass */
struct pwm_ops {
	/**
	 * set_config() - Set the PWM configuration
	 *
	 * @dev:	PWM device to update
	 * @channel:	PWM channel to update
	 * @period_ns:	PWM period in nanoseconds
	 * @duty_ns:	PWM duty period in nanoseconds
	 * @return 0 if OK, -ve on error
	 */
	int (*set_config)(struct udevice *dev, uint channel, uint period_ns,
			  uint duty_ns);

	/**
	 * set_enable() - Enable or disable the PWM
	 *
	 * @dev:	PWM device to update
	 * @channel:	PWM channel to update
	 * @enable:	true to enable, false to disable
	 * @return 0 if OK, -ve on error
	 */
	int (*set_enable)(struct udevice *dev, uint channel, bool enable);
	/**
	 * set_invert() - Set the PWM invert
	 *
	 * @dev:        PWM device to update
	 * @channel:    PWM channel to update
	 * @polarity:   true to invert, false to keep normal polarity
	 * @return 0 if OK, -ve on error
	 */
	int (*set_invert)(struct udevice *dev, uint channel, bool polarity);
};

#define pwm_get_ops(dev)	((struct pwm_ops *)(dev)->driver->ops)

/**
 * pwm_set_config() - Set the PWM configuration
 *
 * @dev:	PWM device to update
 * @channel:	PWM channel to update
 * @period_ns:	PWM period in nanoseconds
 * @duty_ns:	PWM duty period in nanoseconds
 * @return 0 if OK, -ve on error
 */
int pwm_set_config(struct udevice *dev, uint channel, uint period_ns,
		   uint duty_ns);

/**
 * pwm_set_enable() - Enable or disable the PWM
 *
 * @dev:	PWM device to update
 * @channel:	PWM channel to update
 * @enable:	true to enable, false to disable
 * @return 0 if OK, -ve on error
 */
int pwm_set_enable(struct udevice *dev, uint channel, bool enable);

/**
 * pwm_set_invert() - Set pwm default polarity
 *
 * @dev:	PWM device to update
 * @channel:	PWM channel to update
 * @polarity:	true to invert, false to keep normal polarity
 * @return 0 if OK, -ve on error
 */
int pwm_set_invert(struct udevice *dev, uint channel, bool polarity);

/* Legacy interface */
#ifndef CONFIG_DM_PWM
int	pwm_init(int pwm_id, int div, int invert);
int	pwm_config(int pwm_id, int duty_ns, int period_ns);
int	pwm_enable(int pwm_id);
void	pwm_disable(int pwm_id);
#endif
#endif

enum pwm_polarity {
	PWM_POLARITY_NORMAL,
	PWM_POLARITY_INVERSED,
};

struct pwm_device {
	const char		*label;
	unsigned long		flags;
	unsigned int		hwpwm;
	unsigned int		pwm;
	struct pwm_chip		*chip;
	void			*chip_data;

	unsigned int		period; /* in nanoseconds */
};

struct pwm_chip {
	struct device		*dev;
	struct list_head	list;
	const struct pwm_ops	*ops;
	int			base;
	unsigned int		npwm;

	struct pwm_device	*pwms;

	//struct pwm_device *	(*of_xlate)(struct pwm_chip *pc,
					   // const struct of_phandle_args *args);
	unsigned int		of_pwm_n_cells;
	bool			can_sleep;
};

/* it mapping uboot2014/uboot2018 version.*/
int pwm_set_polarity(int pwm, enum pwm_polarity polarity);
int pwm_config(int pwm, int duty_ns, int period_ns);
int pwm_enable(int pwm);
int pwm_disable(int pwm);
int pwm_init(void);
int pwm_request(int pwm, const char *label);
int pwm_remove(int pwm);

#endif /* _pwm_h_ */
