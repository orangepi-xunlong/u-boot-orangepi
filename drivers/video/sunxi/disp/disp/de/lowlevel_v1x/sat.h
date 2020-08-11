/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

/* -------------------------------------------------------- */
/*  */
/* sat.h */
/*  */
/*  */
/*  */
/* 2013-12-19 15:55:40 */
/*  */
/* -------------------------------------------------------- */
#ifndef __REG67__SAT__H__
#define __REG67__SAT__H__

#include "iep.h"
/* Detail information of registers */
/*  */
union sac_ctrl_reg_t {
	__u32 dwval;
	struct {
		__u32 en:1;	/* bit0 */
		__u32 r0:30;	/* bit1~31 */
	} bits;
};				/* 0x00 */

union sac_size_reg_t {
	__u32 dwval;
	struct {
		__u32 sat_w:12;	/* bit0~11 */
		__u32 r0:4;	/* bit12~15 */
		__u32 sat_h:12;	/* bit16~27 */
		__u32 r1:4;	/* bit28~31 */
	} bits;
};				/* 0x04 */

union sac_win0_reg_t {
	__u32 dwval;
	struct {
		__u32 win_left:12;	/* bit0~11 */
		__u32 r0:4;	/* bit12~15 */
		__u32 win_top:12;	/* bit16~27 */
		__u32 r1:4;	/* bit28~31 */
	} bits;
};				/* 0x08 */

union sac_win1_reg_t {
	__u32 dwval;
	struct {
		__u32 win_right:12;	/* bit0~11 */
		__u32 r0:4;	/* bit12~15 */
		__u32 win_bottom:12;	/* bit16~27 */
		__u32 r1:4;	/* bit28~31 */
	} bits;
};				/* 0x0c */

union sac_low_sat_reg_t {
	__u32 dwval;
	struct {
		__u32 low_sat:6;	/* bit0~5 */
		__u32 r0:26;	/* bit6~31 */
	} bits;
};				/* 0x10 */

union sa_global_reg_t {
	__u32 dwval;
	struct {
		__u32 module_en:1;	/* bit0 */
		__u32 r0:15;	/* bit1~15 */
		__u32 ram_switch:1;	/* bit16 */
		__u32 r1:15;	/* bit17~31 */
	} bits;
};				/* 0x18 */

union sac_num_reg_t {
	__u32 dwval;
	struct {
		__u32 r_num:24;	/* bit0~23 */
		__u32 r0:8;	/* bit24~31 */
	} bits;
};				/* 0x14 */

union sat_ctrl_reg_t {
	__u32 dwval;
	struct {
		__u32 en:1;	/* bit0 */
		__u32 r0:30;	/* bit1~31 */
	} bits;
};				/* 0x20 */

union sat_win0_reg_t {
	__u32 dwval;
	struct {
		__u32 win_left:12;	/* bit0~11 */
		__u32 r0:4;	/* bit12~15 */
		__u32 win_top:12;	/* bit16~27 */
		__u32 r1:4;	/* bit28~31 */
	} bits;
};				/* 0x28 */

union sat_win1_reg_t {
	__u32 dwval;
	struct {
		__u32 win_right:12;	/* bit0~11 */
		__u32 r0:4;	/* bit12~15 */
		__u32 win_bottom:12;	/* bit16~27 */
		__u32 r1:4;	/* bit28~31 */
	} bits;
};				/* 0x2c */

union sat_cscgcoff_reg_t {
	__u32 dwval;
	struct {
		__u32 csc_g_coff:13;	/* bit0~bit12 */
		__u32 r0:19;	/* bit13~bit31 */
	} bits;
};				/* 0x30~0x38 */

union sat_cscgcon_reg_t {
	__u32 dwval;
	struct {
		__u32 csc_g_con:14;	/* bit0~bit13 */
		__u32 r0:18;	/* bit14~bit31 */
	} bits;
};				/* 0x3c */

union sat_cscrcoff_reg_t {
	__u32 dwval;
	struct {
		__u32 csc_r_coff:13;	/* bit0~bit12 */
		__u32 r0:19;	/* bit13~bit31 */
	} bits;
};				/* 0x40~0x48 */

union sat_cscrcon_reg_t {
	__u32 dwval;
	struct {
		__u32 csc_r_con:14;	/* bit0~bit13 */
		__u32 r0:18;	/* bit14~bit31 */
	} bits;
};				/* 0x4c */

union sat_cscbcoff_reg_t {
	__u32 dwval;
	struct {
		__u32 csc_b_coff:13;	/* bit0~bit12 */
		__u32 r0:19;	/* bit13~bit31 */
	} bits;
};				/* 0x50~0x58 */

union sat_cscbcon_reg_t {
	__u32 dwval;
	struct {
		__u32 csc_b_con:14;	/* bit0~bit13 */
		__u32 r0:18;	/* bit14~bit31 */
	} bits;
};				/* 0x5c */

union sat_cbcoef_reg_t {
	__u32 dwval;
	struct {
		__u32 coeff0:8;	/* bit0~bit7 */
		__u32 coeff1:8;	/* bit8~bit15 */
		__u32 coeff2:8;	/* bit16~bit23 */
		__u32 coeff3:8;	/* bit24~bit31 */
	} bits;
};				/* 0x100~0x1fc */

union sat_crcoef_reg_t {
	__u32 dwval;
	struct {
		__u32 coeff0:8;	/* bit0~bit7 */
		__u32 coeff1:8;	/* bit8~bit15 */
		__u32 coeff2:8;	/* bit16~bit23 */
		__u32 coeff3:8;	/* bit24~bit31 */
	} bits;
};				/* 0x200~0x2fc */

struct __sa_reg {
	union sac_ctrl_reg_t sacctrl;	/* 0x00 */
	union sac_size_reg_t sacsize;	/* 0x04 */
	union sac_win0_reg_t sacwin0;	/* 0x08 */
	union sac_win1_reg_t sacwin1;	/* 0x0c */
	union sac_low_sat_reg_t lowsat;	/* 0x10 */
	union sac_num_reg_t R_num;	/* 0x14 */
	union sa_global_reg_t saglobal;	/* 0x18 */
	__u32 r0;		/* 0x1c */
	union sat_ctrl_reg_t satctrl;	/* 0x20 */
	__u32 r1;		/* 0x24 */
	union sat_win0_reg_t satwin0;	/* 0x28 */
	union sat_win1_reg_t satwin1;	/* 0x2c */
	union sat_cscgcoff_reg_t cscgcoff[3];	/* 0x30~0x38 */
	union sat_cscgcon_reg_t cscgcon;	/* 0x3c */
	union sat_cscrcoff_reg_t cscrcoff[3];	/* 0x40~0x48 */
	union sat_cscrcon_reg_t cscrcon;	/* 0x4c */
	union sat_cscbcoff_reg_t cscbcoff[3];	/* 0x50~0x58 */
	union sat_cscbcon_reg_t cscbcon;	/* 0x5c */
	__u32 r2[40];		/* 0x60~0xfc */
	union sat_cbcoef_reg_t satcbtab[64];	/* 0x100~0x1fc */
	union sat_crcoef_reg_t satcrtab[64];	/* 0x200~0x2fc */
};

__s32 SAT_Set_Reg_Base(__u32 sel, uintptr_t base);
__u32 SAT_Get_Reg_Base(__u32 sel);
__s32 SAT_Enable_Disable(__u32 sel, __u32 en);
__s32 SAT_Set_Display_Size(__u32 sel, __u32 width, __u32 height);
__s32 SAT_Set_Window(__u32 sel, disp_window *window);
__s32 SAT_Set_Sync_Para(__u32 sel);

#endif				/* __REG67__SAT__H__ */
