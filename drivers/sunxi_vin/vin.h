/*
 * vin.h
 *
 * Copyright (c) 2018 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zequn Zheng <zequnzhengi@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __VIN_H__
#define __VIN_H__

#include <common.h>
#include <i2c.h>
#include <sys_config_old.h>
#include <sys_config.h>
#include <clk/clk.h>
#include <malloc.h>
#include <linux/time.h>
#include <linux/list.h>
#include <sunxi_display2.h>

#include "utility/vin_supply.h"
#include "utility/vin_common.h"
#include "utility/media-bus-format.h"
#include "utility/platform_cfg.h"
#include "vin_cci/cci_helper.h"
#include "vin_mipi/sunxi_mipi.h"
#include "vin_csi/sunxi_csi.h"
#include "vin_video/vin_core.h"
#include "vin_video/vin_video.h"
#include "vin_isp/sunxi_isp.h"
#include "vin_vipp/sunxi_scaler.h"
#include "modules/sensor/camera.h"
#include "top_reg.h"

#define EFAULT 1
#define EINVAL 1

#define VIN_CLK_RATE (432*1000*1000)

/* #define ISP_USE_IRQ */
/* #define DMA_USE_IRQ_BUFFER_QUEUE */

struct vin_clk_info {
	struct clk *clock;
	unsigned long frequency;
};

struct vin_mclk_info {
	struct clk *mclk;
	struct clk *clk_24m;
	struct clk *clk_pll;
};

enum {
	VIN_TOP_CLK = 0,
	VIN_TOP_CLK_SRC,
	VIN_MAX_CLK,
};

enum {
	VIN_MIPI_CLK = 0,
	VIN_MIPI_CLK_SRC,
	VIN_MIPI_MAX_CLK,
};

struct vin_clk_total {
	struct vin_clk_info clk[VIN_MAX_CLK];
	struct vin_clk_info mipi_clk[VIN_MIPI_MAX_CLK];
	struct vin_mclk_info mclk[VIN_MAX_CCI];
	struct vin_clk_info isp_clk;
};

typedef enum {
	TVD_PL_YUV420 = 0,
	TVD_MB_YUV420 = 1,
	TVD_PL_YUV422 = 2,
} TVD_FMT_T;

struct disp_screen {
	int x;
	int y;
	int w;
	int h;
};

struct test_layer_info {
	int screen_id;
	int layer_id;
	int mem_id;
	struct disp_layer_config layer_config;
	int addr_map;
	int width, height;/* screen size */
	int fh;/* picture resource file handle */
	int mem;
	int clear;/* is clear layer */
	char filename[32];
	int full_screen;
	unsigned int pixformat;
	enum disp_output_type output_type;
};

/**
 * tvd_dev info
 */
struct tvd_dev {
	unsigned int ch_id;
	unsigned int height;
	unsigned int width;
	unsigned int interface;
	unsigned int system;
	unsigned int row;
	unsigned int column;
	unsigned int ch0_en;
	unsigned int ch1_en;
	unsigned int ch2_en;
	unsigned int ch3_en;
	unsigned int pixformat;
	struct test_layer_info layer_info;
	int frame_no_to_grap;
};

extern unsigned int vin_log_mask;
#define VIN_LOG_MD				(1 << 0) 	/*0x1 */
#define VIN_LOG_FLASH				(1 << 1) 	/*0x2 */
#define VIN_LOG_CCI				(1 << 2) 	/*0x4 */
#define VIN_LOG_CSI				(1 << 3) 	/*0x8 */
#define VIN_LOG_MIPI				(1 << 4) 	/*0x10*/
#define VIN_LOG_ISP				(1 << 5) 	/*0x20*/
#define VIN_LOG_STAT				(1 << 6) 	/*0x40*/
#define VIN_LOG_SCALER				(1 << 7) 	/*0x80*/
#define VIN_LOG_POWER				(1 << 8) 	/*0x100*/
#define VIN_LOG_CONFIG				(1 << 9) 	/*0x200*/
#define VIN_LOG_VIDEO				(1 << 10)	/*0x400*/
#define VIN_LOG_FMT				(1 << 11)	/*0x800*/

#define vin_log(flag, arg...) do { \
	if (flag & vin_log_mask) { \
		switch (flag) { \
		case VIN_LOG_MD: \
			printf("[VIN_LOG_MD]" arg); \
			break; \
		case VIN_LOG_FLASH: \
			printf("[VIN_LOG_FLASH]" arg); \
			break; \
		case VIN_LOG_CCI: \
			printf("[VIN_LOG_CCI]" arg); \
			break; \
		case VIN_LOG_CSI: \
			printf("[VIN_LOG_CSI]" arg); \
			break; \
		case VIN_LOG_MIPI: \
			printf("[VIN_LOG_MIPI]" arg); \
			break; \
		case VIN_LOG_ISP: \
			printf("[VIN_LOG_ISP]" arg); \
			break; \
		case VIN_LOG_STAT: \
			printf("[VIN_LOG_STAT]" arg); \
			break; \
		case VIN_LOG_SCALER: \
			printf("[VIN_LOG_SCALER]" arg); \
			break; \
		case VIN_LOG_POWER: \
			printf("[VIN_LOG_POWER]" arg); \
			break; \
		case VIN_LOG_CONFIG: \
			printf("[VIN_LOG_CONFIG]" arg); \
			break; \
		case VIN_LOG_VIDEO: \
			printf("[VIN_LOG_VIDEO]" arg); \
			break; \
		case VIN_LOG_FMT: \
			printf("[VIN_LOG_FMT]" arg); \
			break; \
		default: \
			printf("[VIN_LOG]" arg); \
			break; \
		} \
	} \
} while (0)

#define vin_err(x, arg...)   printf("[VIN_ERR]"x, ##arg)
#define vin_warn(x, arg...)  printf("[VIN_WARN]"x, ##arg)
#define vin_print(x, arg...) printf("[VIN]"x, ##arg)

int disp_set_addr(int width, int height, unsigned int addr);

#endif
