/*
 * drivers/video/sunxi/disp2/disp/de/lowlevel_v2x/de_fcc_type.h
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef __DE_FCC_TYPE__
#define __DE_FCC_TYPE__

#include "de_rtmx.h"

#define FCC_PARA_NUM  6
#define FCC_MODE_NUM  3

union FCC_CTRL_REG {
	unsigned int dwval;
	struct {
		unsigned int en:1;
		unsigned int res0:7;
		unsigned int win_en:1;
		unsigned int res1:23;
	} bits;
};

union FCC_SIZE_REG {
	unsigned int dwval;
	struct {
		unsigned int width:13;
		unsigned int res0:3;
		unsigned int height:13;
		unsigned int res1:3;
	} bits;
};

union FCC_WIN0_REG {
	unsigned int dwval;
	struct {
		unsigned int left:13;
		unsigned int res0:3;
		unsigned int top:13;
		unsigned int res1:3;
	} bits;
};

union FCC_WIN1_REG {
	unsigned int dwval;
	struct {
		unsigned int right:13;
		unsigned int res0:3;
		unsigned int bot:13;
		unsigned int res1:3;
	} bits;
};

union FCC_HUE_RANGE_REG {
	unsigned int dwval;
	struct {
		unsigned int hmin:12;
		unsigned int res0:4;
		unsigned int hmax:12;
		unsigned int res1:4;
	} bits;
};

union FCC_HS_GAIN_REG {
	unsigned int dwval;
	struct {
		unsigned int sgain:9;
		unsigned int res0:7;
		unsigned int hgain:9;
		unsigned int res1:7;
	} bits;
};

union FCC_CSC_CTL_REG {
	unsigned int dwval;
	struct {
		unsigned int bypass:1;
		unsigned int res0:31;
	} bits;
};

union FCC_CSC_COEFF_REG {
	unsigned int dwval;
	struct {
		unsigned int coff:13;
		unsigned int res0:19;
	} bits;
};

union FCC_CSC_CONST_REG {
	unsigned int dwval;
	struct {
		unsigned int cont:20;
		unsigned int res0:12;
	} bits;
};

union FCC_GLB_APH_REG {
	unsigned int dwval;
	struct {
		unsigned int res0:24;
		unsigned int alpha:8;
	} bits;
};

struct __fcc_reg_t {
	union FCC_CTRL_REG fcc_ctl;	                /* 0x00      */
	union FCC_SIZE_REG fcc_size;	              /* 0x04      */
	union FCC_WIN0_REG fcc_win0;	              /* 0x08      */
	union FCC_WIN1_REG fcc_win1;	              /* 0x0c      */
	union FCC_HUE_RANGE_REG fcc_range[6];       /* 0x10-0x24 */
	unsigned int res0[2];	                /* 0x28-0x2c */
	union FCC_HS_GAIN_REG fcc_gain[6];	        /* 0x30-0x44 */
	unsigned int res1[2];	                /* 0x48-0x4c */
	union FCC_CSC_CTL_REG fcc_csc_ctl;	        /* 0x50      */
	unsigned int res2[3];	                /* 0x54-0x5c */
	union FCC_CSC_COEFF_REG fcc_csc_coff0[3];	  /* 0x60-0x68 */
	union FCC_CSC_CONST_REG fcc_csc_const0;	    /* 0x6c      */
	union FCC_CSC_COEFF_REG fcc_csc_coff1[3];	  /* 0x70-0x78 */
	union FCC_CSC_CONST_REG fcc_csc_const1;	    /* 0x7c      */
	union FCC_CSC_COEFF_REG fcc_csc_coff2[3];	  /* 0x80-0x88 */
	union FCC_CSC_CONST_REG fcc_csc_const2;	    /* 0x8c      */
	union FCC_GLB_APH_REG fcc_glb_alpha;	      /* 0x90      */
};

struct __fcc_config_data {
	/* ase */
	unsigned int fcc_en;
	unsigned int sgain[6];

	/* window */
	unsigned int win_en;
	struct de_rect win;
};

#endif
