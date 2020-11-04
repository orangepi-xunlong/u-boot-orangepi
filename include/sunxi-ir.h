/*
 *  * Copyright 2000-2009
 *   * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *    *
 *     * SPDX-License-Identifier:	GPL-2.0+
 *     */

#ifndef __SUNXI_IR_H__
#define __SUNXI_IR_H__

extern void ir_clk_cfg(void);

#define MAX_IR_ADDR_NUM (32)

#define IR_DETECT_NULL (1)
#define IR_DETECT_OK (2)
#define IR_DETECT_END (3)

/* //#define SUNXI_IR_DEBUG */

#ifdef SUNXI_IR_DEBUG
#define print_debug(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define print_debug(fmt, ...)
#endif

#define CONFIG_FPGA_V4_PLATFORM

/* //Registers */
#define IR_REG(x) (x)
#define IR_CTRL_REG IR_REG(0x00) /* //IR Control */
#define IR_RXCFG_REG IR_REG(0x10) /* //Rx Config */
#define IR_RXDAT_REG IR_REG(0x20) /* //Rx Data */
#define IR_RXINTE_REG IR_REG(0x2C) /* //Rx Interrupt Enable */
#define IR_RXINTS_REG IR_REG(0x30) /* //Rx Interrupt Status */
#define IR_SPLCFG_REG IR_REG(0x34) /* //IR Sample Config */

#define IR_FIFO_SIZE (64) /* 64Bytes */
#if (defined CONFIG_FPGA_V4_PLATFORM) || (defined CONFIG_FPGA_V7_PLATFORM)
#define CIR_24M_CLK_USED
#endif

#ifdef CIR_24M_CLK_USED
#define IR_SIMPLE_UNIT (21333) /* simple in ns */
#define IR_CLK (24000000) /* fpga clk output is fixed */
#define IR_CIR_MODE (0x3 << 4) /* CIR mode enable */
#define IR_ENTIRE_ENABLE (0x3 << 0) /* IR entire enable */
#define IR_SAMPLE_DEV (0x3 << 0) /* 24MHz/512 =46875Hz (21333ns) */
#define IR_FIFO_32 (((IR_FIFO_SIZE >> 1) - 1) << 8)
#define IR_IRQ_STATUS ((0x1 << 4) | 0x3)
#else
#define IR_SIMPLE_UNIT (32000) /* simple in ns */
#define IR_CLK (8000000)
#define IR_CIR_MODE (0x3 << 4) /* CIR mode enable */
#define IR_ENTIRE_ENABLE (0x3 << 0) /* IR entire enable */
#define IR_SAMPLE_DEV (0x2 << 0) /* 4MHz/256 =31250Hz (32000ns) */
#define IR_FIFO_32 (((IR_FIFO_SIZE >> 1) - 1) << 8)
#define IR_IRQ_STATUS ((0x1 << 4) | 0x3)
#endif

/* //Bit Definition of IR_RXINTS_REG Register */
#define IR_RXINTS_RXOF (0x1 << 0) /* Rx FIFO Overflow */
#define IR_RXINTS_RXPE (0x1 << 1) /* Rx Packet End */
#define IR_RXINTS_RXDA (0x1 << 4) /* Rx FIFO Data Available */
#ifdef CIR_24M_CLK_USED
#define IR_RXFILT_VAL                                                          \
	(((16) & 0x3f) << 2) /* Filter Threshold = 16*21.3 = ~341us < 500us */
#define IR_RXIDLE_VAL                                                          \
	(((5) & 0xff)                                                          \
	 << 8) /* Idle Threshold = (5+1)*128*21.3 = ~16.4ms > 9ms */
#define IR_ACTIVE_T ((0 & 0xff) << 16) /* Active Threshold */
#define IR_ACTIVE_T_C ((1 & 0xff) << 23) /* Active Threshold */
#else
#define IR_RXFILT_VAL                                                          \
	(((12) & 0x3f) << 2) /* Filter Threshold = 12*32 = ~384us < 500us */
#define IR_RXIDLE_VAL                                                          \
	(((2) & 0xff) << 8) /* Idle Threshold = (2+1)*128*32 = ~23.8ms > 9ms */
#define IR_ACTIVE_T ((99 & 0xff) << 16) /* Active Threshold */
#define IR_ACTIVE_T_C ((0 & 0xff) << 23) /* Active Threshold */
#endif

enum ir_mode {
	CIR_MODE_ENABLE,
	IR_MODULE_ENABLE,
};

enum ir_sample_config {
	IR_SAMPLE_REG_CLEAR,
	IR_CLK_SAMPLE,
	IR_FILTER_TH,
	IR_IDLE_TH,
	IR_ACTIVE_TH,
};

enum ir_irq_config {
	IR_IRQ_STATUS_CLEAR,
	IR_IRQ_ENABLE,
	IR_IRQ_FIFO_SIZE,
};

enum { IR_SUPLY_DISABLE = 0,
       IR_SUPLY_ENABLE,
};

struct ir_raw_event {
	union {
		u32 duration;
		struct {
			u32 carrier;
			u8 duty_cycle;
		};
	};
	unsigned pulse;
	unsigned reset;
	unsigned timeout;
	unsigned carrier_report;
};

#define DEFINE_IR_RAW_EVENT(event)                                             \
	struct ir_raw_event event = { {.duration = 0 },                        \
				      .pulse	  = 0,                     \
				      .reset	  = 0,                     \
				      .timeout	= 0,                     \
				      .carrier_report = 0 }

static inline void init_ir_raw_event(struct ir_raw_event *ev)
{
	memset(ev, 0, sizeof(*ev));
}

struct nec_dec {
	int state;
	unsigned count;
	u32 bits;
	bool is_nec_x;
	bool necx_repeat;
	bool curt_repeat;
};

struct sunxi_ir_data {
	u32 ir_addr_cnt;
	u32 ir_recoverykey[MAX_IR_ADDR_NUM];
	u32 ir_addr[MAX_IR_ADDR_NUM];
};

struct ir_raw_buffer {
	unsigned long dcnt; /*Packet Count*/
	struct nec_dec nec;
	u32 scancode;
#define IR_RAW_BUF_SIZE 128
	struct ir_raw_event raw[IR_RAW_BUF_SIZE];
};

/* macros for IR decoders */
static inline bool geq_margin(unsigned d1, unsigned d2, unsigned margin)
{
	return d1 > (d2 - margin);
}

static inline bool eq_margin(unsigned d1, unsigned d2, unsigned margin)
{
	return (bool)((d1 > (d2 - margin)) && (d1 < (d2 + margin)));
}

static inline bool is_transition(struct ir_raw_event *x, struct ir_raw_event *y)
{
	return x->pulse != y->pulse;
}

static inline void decrease_duration(struct ir_raw_event *ev, unsigned duration)
{
	if (duration > ev->duration)
		ev->duration = 0;
	else
		ev->duration -= duration;
}

/* Returns true if event is normal pulse/space event */
static inline bool is_timing_event(struct ir_raw_event ev)
{
	return !ev.carrier_report && !ev.reset;
}

#define TO_US(duration) DIV_ROUND_CLOSEST((duration), 1000)
#define TO_STR(is_pulse) ((is_pulse) ? "pulse" : "space")

void rc_keydown(struct ir_raw_buffer *ir_raw, u32 scancode, u8 toggle);
int ir_setup(void);
void ir_disable(void);

#endif /* end of __SUNXI_IR_H__ */
