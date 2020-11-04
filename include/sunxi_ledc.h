/*
 * Copyright (C) 2016 Allwinner.
 *
 *
 * SUNXI LEDC Controller Definition
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifndef _SUNXI_LEDC_H_
#define _SUNXI_LEDC_H_


/*interrupt ctrol*/
enum sunxi_ledc_irq_ctrl{
	LEDC_TRANS_FINISH_INT_EN     = (1 << 0),
	LEDC_FIFO_CPUREQ_INT_EN      = (1 << 1),
	LEDC_WAITDATA_TIMEOUT_INT_EN = (1 << 3),
	LEDC_FIFO_OVERFLOW_INT_EN    = (1 << 4),
	LEDC_GLOBAL_INT_EN           = (1 << 5),
};

/*interrupt status*/
enum sunxi_ledc_irq_status{
	LEDC_TRANS_FINISH_INT     = (1 << 0),
	LEDC_FIFO_CPUREQ_INT      = (1 << 1),
	LEDC_WAITDATA_TIMEOUT_INT = (1 << 3),
	LEDC_FIFO_OVERFLOW_INT    = (1 << 4),
	LEDC_FIFO_FULL            = (1 << 16),
	LEDC_FIFO_EMPTY           = (1 << 17),
};


#define LEDC_BASE_ADDR                  0x6700000
#define LEDC_CONTROL_OFFSET             0x50
#define FIFO_SIZE                       128

#define LEDC_CLOCK_ADDR                 0x03001BF0

struct sunxi_ledc_reg{
    volatile unsigned int ctrl;        /* control */
    volatile unsigned int time01;      /* t01 time */
    volatile unsigned int fnsh_cnt;    /* data finish count */
    volatile unsigned int rst_t;       /* reset time */
    volatile unsigned int t0_ctrl;     /* wait time0 control */
    volatile unsigned int data_reg;    /* LEDC data */
    volatile unsigned int DMA_ctrl;    /* DMA control */
    volatile unsigned int irq_ctrl;    /* interrupt control */
    volatile unsigned int irq_sta;     /* interrupt status */
    volatile unsigned int reserved;    /* reserved */
    volatile unsigned int t1_ctrl;     /* wait time1 control */
    volatile unsigned int version;     /* LEDC version */
    volatile unsigned int fifo;        /* LEDC fifo data */
};

struct sunxi_ccmu_reg{
    volatile unsigned int model_clk;     /* 0xBF0 LEDC clock */
    volatile unsigned int reserved1[2];	 /* 0xBF4 0xBF8 */
    volatile unsigned int bus_gate;  	 /* 0xBFC BUS gating */
};

int sunxi_ledc_init(void);

int sunxi_set_led_brightness(unsigned int length, unsigned int g, unsigned int r, unsigned int b);

int sunxi_ledc_exit(void);
#endif
