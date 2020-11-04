/*
 *  * Copyright 2000-2009
 *   * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *    *
 *     * SPDX-License-Identifier:	GPL-2.0+
 *     */

#ifndef _SUNXI_KEY_H
#define _SUNXI_KEY_H

#include "asm/arch-sunxi/cpu.h"

struct sunxi_lradc {
	volatile u32 ctrl;         /* lradc control */
	volatile u32 intc;         /* interrupt control */
	volatile u32 ints;         /* interrupt status */
	volatile u32 data0;        /* lradc 0 data */
	volatile u32 data1;        /* lradc 1 data */
};

#define SUNXI_KEY_ADC_CRTL        (SUNXI_KEYADC_BASE + 0x00)
#define SUNXI_KEY_ADC_INTC        (SUNXI_KEYADC_BASE + 0x04)
#define SUNXI_KEY_ADC_INTS        (SUNXI_KEYADC_BASE + 0x08)
#define SUNXI_KEY_ADC_DATA0       (SUNXI_KEYADC_BASE + 0x0C)

#define LRADC_EN                  (0x1)   /* LRADC enable */
#define LRADC_SAMPLE_RATE         0x2    /* 32.25 Hz */
#define LEVELB_VOL                0x2    /* 0x33(~1.6v) */
#define LRADC_HOLD_EN             (0x1 << 6)    /* sample hold enable */
#define KEY_MODE_SELECT           0x0    /* normal mode */

#define ADC0_DATA_PENDING         (1 << 0)    /* adc0 has data */
#define ADC0_KEYDOWN_PENDING      (1 << 1)    /* key down */
#define ADC0_HOLDKEY_PENDING      (1 << 2)    /* key hold */
#define ADC0_ALRDY_HOLD_PENDING   (1 << 3)    /* key already hold */
#define ADC0_KEYUP_PENDING        (1 << 4)    /* key up */


#define GP_CTRL        (SUNXI_GPADC_BASE+0x04)
#define GP_CS_EN       (SUNXI_GPADC_BASE+0x08)
#define GP_DATA_INTC   (SUNXI_GPADC_BASE+0x28)
#define GP_DATA_INTS   (SUNXI_GPADC_BASE+0x38)
#define GP_CH0_DATA    (SUNXI_GPADC_BASE+0x80)

#define GPADC0_DATA_PENDING		(1 << 0)	/* gpadc0 has data */


int sunxi_key_init(void);

int sunxi_key_exit(void);

int sunxi_key_read(void);

int sunxi_key_probe(void);

int sunxi_key_clock_open(void);

int sunxi_key_clock_close(void);

#endif
