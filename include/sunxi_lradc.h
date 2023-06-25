/* SPDX-License-Identifier: GPL-2.0+
 * (C) Copyright 2019-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * oujiayu <oujiayu@allwinnertech.com>
 *
 * Configuration settings for the Allwinner sunxi lradc of boards.
 */

#ifndef SUNXI_LRADC_H_
#define SUNXI_LRADC_H_

#define LRADC_CTRL              (0x00)

#define FIRST_CONCERT_DLY	(0 << 24)
#define CHAN			(0x0)
#define ADC_CHAN_SELECT		(CHAN << 22)
#define LRADC_KEY_MODE		(0)
#define KEY_MODE_SELECT		(LRADC_KEY_MODE << 12)
#define LRADC_HOLD_EN		(1 << 6)
#define LEVELB_VOL		(0 << 4)
#define LRADC_SAMPLE_250HZ	(0 << 2)
#define LRADC_EN		(1 << 0)

enum key_mode {
	CONCERT_DLY_SET = (1 << 0),
	ADC_CHAN_SET = (1 << 1),
	KEY_MODE_SET = (1 << 2),
	LRADC_HOLD_SET = (1 << 3),
	LEVELB_VOL_SET = (1 << 4),
	LRADC_SAMPLE_SET = (1 << 5),
	LRADC_EN_SET = (1 << 6),
};

int lradc_reg_init(void);

#endif
