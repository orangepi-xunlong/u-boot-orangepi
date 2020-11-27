/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __de_lcd_h__
#define __de_lcd_h__

#include "ebios_lcdc_tve.h"

/*  */
/* detail information of registers */
/*  */

union tcon_gctl_reg_t {
	__u32 dwval;
	struct {
		__u32 io_map_sel:1;	/* default: 0; */
		__u32 res0:29;	/* default:; */
		__u32 tcon_gamma_en:1;	/* default: 0; */
		__u32 tcon_en:1;	/* default: 0; */
	} bits;
};

union tcon_gint0_reg_t {
	__u32 dwval;
	struct {
		__u32 tcon_irq_flag:16;	/* default: 0; */
		__u32 tcon_irq_en:16;	/* default: 0; */
	} bits;
};

union tcon_gint1_reg_t {
	__u32 dwval;
	struct {
		__u32 tcon1_line_int_num:12;	/* default: 0; */
		__u32 res0:4;	/* default:; */
		__u32 tcon0_line_int_num:12;	/* default: 0; */
		__u32 res1:4;	/* default:; */
	} bits;
};

union tcon0_frm_ctl_reg_t {
	__u32 dwval;
	struct {
		__u32 tcon0_frm_test:2;	/* default: 0; */
		__u32 res0:2;	/* default:; */
		__u32 tcon0_frm_mode_b:1;	/* default: 0; */
		__u32 tcon0_frm_mode_g:1;	/* default: 0; */
		__u32 tcon0_frm_mode_r:1;	/* default: 0; */
		__u32 res1:24;	/* default:; */
		__u32 tcon0_frm_en:1;	/* default: 0; */
	} bits;
};

union tcon0_frm_seed_reg_t {
	__u32 dwval;
	struct {
		__u32 seed_value:13;	/* default: 0; */
		__u32 res0:19;	/* default:; */
	} bits;
};

union tcon0_frm_tab_reg_t {
	__u32 dwval;
	struct {
		__u32 frm_table_value;	/* default: 0; */
	} bits;
};

union tcon0_ctl_reg_t {
	__u32 dwval;
	struct {
		__u32 src_sel:3;	/* default: 0; */
		__u32 res0:1;	/* default:; */
		__u32 start_delay:5;	/* default: 0; */
		__u32 res1:11;	/* default:; */
		__u32 interlace_en:1;	/* default: 0; */
		__u32 fifo1_rst:1;	/* default: 0; */
		__u32 test_value:1;	/* default: 0; */
		__u32 rb_swap:1;	/* default: 0; */
		__u32 tcon0_if:2;	/* default: 0; */
		__u32 res2:2;	/* default:; */
		__u32 tcon0_work_mode:1;	/* default: 0; */
		__u32 res3:2;	/* default:; */
		__u32 tcon0_en:1;	/* default: 0; */
	} bits;
};

union tcon0_dclk_reg_t {
	__u32 dwval;
	struct {
		__u32 tcon0_dclk_div:7;	/* default: 0; */
		__u32 res0:21;	/* default:; */
		__u32 tcon0_dclk_en:4;	/* default: 0; */
	} bits;
};

union tcon0_basic0_reg_t {
	__u32 dwval;
	struct {
		__u32 y:12;	/* default: 0; */
		__u32 res0:4;	/* default:; */
		__u32 x:12;	/* default: 0; */
		__u32 res1:4;	/* default:; */
	} bits;
};

union tcon0_basic1_reg_t {
	__u32 dwval;
	struct {
		__u32 hbp:12;	/* default: 0; */
		__u32 res0:4;	/* default:; */
		__u32 ht:13;	/* default: 0; */
		__u32 res1:2;	/* default:; */
		__u32 reservd:1;	/* default: 0; */
	} bits;
};

union tcon0_basic2_reg_t {
	__u32 dwval;
	struct {
		__u32 vbp:12;	/* default: 0; */
		__u32 res0:4;	/* default:; */
		__u32 vt:13;	/* default: 0; */
		__u32 res1:3;	/* default:; */
	} bits;
};

union tcon0_basic3_reg_t {
	__u32 dwval;
	struct {
		__u32 vspw:10;	/* default: 0; */
		__u32 res0:6;	/* default:; */
		__u32 hspw:10;	/* default: 0; */
		__u32 res1:6;	/* default:; */
	} bits;
};

union tcon0_hv_if_reg_t {
	__u32 dwval;
	struct {
		__u32 res0:20;	/* default:; */
		__u32 syuv_fdly:2;	/* default: 0; */
		__u32 syuv_seq:2;	/* default: 0; */
		__u32 srgb_seq:4;	/* default: 0; */
		__u32 hv_mode:4;	/* default: 0; */
	} bits;
};

union tcon0_cpu_if_reg_t {
	__u32 dwval;
	struct {
		__u32 trigger_en:1;	/* default: 0; */
		__u32 trigger_start:1;	/* default: 0; */
		__u32 trigger_fifo_en:1;	/* default: 0; */
		__u32 trigger_fifo_bist_en:1;	/* default: 0; */
		__u32 trigger_sync_mode:2;	/* default: 0; */
		__u32 res0:10;	/* default:; */
		__u32 flush:1;	/* default: 0; */
		__u32 auto_:1;	/* default: 0; */
		__u32 res1:4;	/* default:; */
		__u32 rd_flag:1;	/* default: 0; */
		__u32 wr_flag:1;	/* default: 0; */
		__u32 vsync_cs_sel:1;	/* default: 0; */
		__u32 ca:1;	/* default: 0; */
		__u32 da:1;	/* default: 0; */
		__u32 res2:1;	/* default:; */
		__u32 cpu_mode:4;	/* default: 0; */
	} bits;
};

union tcon0_cpu_wr_reg_t {
	__u32 dwval;
	struct {
		__u32 data_wr:24;	/* default: 0; */
		__u32 res0:8;	/* default:; */
	} bits;
};

union tcon0_cpu_rd0_reg_t {
	__u32 dwval;
	struct {
		__u32 data_rd0:24;	/* default:; */
		__u32 res0:8;	/* default:; */
	} bits;
};

union tcon0_cpu_rd1_reg_t {
	__u32 dwval;
	struct {
		__u32 data_rd1:24;	/* default:; */
		__u32 res0:8;	/* default:; */
	} bits;
};

union tcon0_lvds_if_reg_t {
	__u32 dwval;
	struct {
		__u32 tcon0_lvds_data_revert:4;	/* default; 0:revert */
		__u32 tcon0_lvds_clk_revert:1;	/* default; 0: revert */
		__u32 res0:15;	/* default:; */
		__u32 tcon0_lvds_clk_sel:1;	/* default: 0; */
		__u32 res1:2;	/* default:; */
		__u32 tcon0_lvds_correct_mode:1;	/* default: 0; */
		__u32 tcon0_lvds_debug_mode:1;	/* default: 0; */
		__u32 tcon0_lvds_debug_en:1;	/* default: 0; */
		__u32 tcon0_lvds_bitwidth:1;	/* default: 0; */
		__u32 tcon0_lvds_mode:1;	/* default: 0; */
		__u32 tcon0_lvds_dir:1;	/* default: 0; */
		__u32 tcon0_lvds_even_odd_dir:1;	/* default: 0; */
		__u32 tcon0_lvds_link:1;	/* default: 0; */
		__u32 tcon0_lvds_en:1;	/* default: 0; */
	} bits;
};

union tcon0_io_pol_reg_t {
	__u32 dwval;
	struct {
		__u32 data_inv:24;	/* default: 0; */
		__u32 sync_inv:2;	/* default: 0; */
		__u32 clk_inv:1;	/* default: 0; */
		__u32 de_inv:1;	/* default: 0; */
		__u32 dclk_sel:3;	/* default: 0; */
		__u32 io_output_sel:1;	/* default: 0; */
	} bits;
};

union tcon0_io_tri_reg_t {
	__u32 dwval;
	struct {
		__u32 data_output_tri_en:24;	/* default: 0xffffff; */
		__u32 io0_output_tri_en:1;	/* default: 1; */
		__u32 io1_output_tri_en:1;	/* default: 1; */
		__u32 io2_output_tri_en:1;	/* default: 1; */
		__u32 io3_output_tri_en:1;	/* default: 1; */
		__u32 rgb_endian:1;	/* default:; */
		__u32 res0:3;	/* default:; */
	} bits;
};

union tcon1_ctl_reg_t {
	__u32 dwval;
	struct {
		__u32 src_sel:2;	/* default: 0; */
		__u32 res0:2;	/* default:; */
		__u32 start_delay:5;	/* default: 0; */
		__u32 res1:11;	/* default:; */
		__u32 interlace_en:1;	/* default: 0; */
		__u32 res2:10;	/* default:; */
		__u32 tcon1_en:1;	/* default: 0; */
	} bits;
};

union tcon1_basic0_reg_t {
	__u32 dwval;
	struct {
		__u32 y:12;	/* default: 0; */
		__u32 res0:4;	/* default:; */
		__u32 x:12;	/* default: 0; */
		__u32 res1:4;	/* default:; */
	} bits;
};

union tcon1_basic1_reg_t {
	__u32 dwval;
	struct {
		__u32 ls_yo:12;	/* default: 0; */
		__u32 res0:4;	/* default:; */
		__u32 ls_xo:12;	/* default: 0; */
		__u32 res1:4;	/* default:; */
	} bits;
};

union tcon1_basic2_reg_t {
	__u32 dwval;
	struct {
		__u32 yo:12;	/* default: 0; */
		__u32 res0:4;	/* default:; */
		__u32 xo:12;	/* default: 0; */
		__u32 res1:4;	/* default:; */
	} bits;
};

union tcon1_basic3_reg_t {
	__u32 dwval;
	struct {
		__u32 hbp:12;	/* default: 0; */
		__u32 res0:4;	/* default:; */
		__u32 ht:13;	/* default: 0; */
		__u32 res1:3;	/* default:; */
	} bits;
};

union tcon1_basic4_reg_t {
	__u32 dwval;
	struct {
		__u32 vbp:12;	/* default: 0; */
		__u32 res0:4;	/* default:; */
		__u32 vt:13;	/* default: 0; */
		__u32 res1:3;	/* default:; */
	} bits;
};

union tcon1_basic5_reg_t {
	__u32 dwval;
	struct {
		__u32 vspw:10;	/* default: 0; */
		__u32 res0:6;	/* default:; */
		__u32 hspw:10;	/* default: 0; */
		__u32 res1:6;	/* default:; */
	} bits;
};

union tcon1_ps_sync_reg_t {
	__u32 dwval;
	struct {
		__u32 sync_y:16;	/* default: 0; */
		__u32 sync_x:16;	/* default: 0; */
	} bits;
};

union tcon1_io_pol_reg_t {
	__u32 dwval;
	struct {
		__u32 data_inv:24;	/* default: 0; */
		__u32 io0_inv:1;	/* default: 0; */
		__u32 io1_inv:1;	/* default: 0; */
		__u32 io2_inv:1;	/* default: 0; */
		__u32 io3_inv:1;	/* default: 0; */
		__u32 res0:4;	/* default:; */
	} bits;
};

union tcon1_io_tri_reg_t {
	__u32 dwval;
	struct {
		__u32 data_output_tri_en:24;	/* default: 0xffffff; */
		__u32 io0_output_tri_en:1;	/* default: 1; */
		__u32 io1_output_tri_en:1;	/* default: 1; */
		__u32 io2_output_tri_en:1;	/* default: 1; */
		__u32 io3_output_tri_en:1;	/* default: 1; */
		__u32 res0:4;	/* default:; */
	} bits;
};

union tcon_ecc_fifo_reg_t {
	__u32 dwval;
	struct {
		__u32 ecc_fifo_setting:8;	/* default:; */
		__u32 ecc_fifo_blank_en:1;	/* default:; */
		__u32 res0:7;	/* default:; */
		__u32 ecc_fifo_err_bits:8;	/* default:; */
		__u32 res1:6;	/* default:; */
		__u32 ecc_fifo_err_flag:1;	/* default:; */
		__u32 ecc_fifo_bist_en:1;	/* default:; */
	} bits;
};

union tcon_debug_reg_t {
	__u32 dwval;
	struct {
		__u32 tcon1_current_line:12;	/* default:; */
		__u32 res0:1;	/* default:; */
		__u32 ecc_fifo_bypass:1;	/* default: 0; */
		__u32 res1:2;	/* default:; */
		__u32 tcon0_current_line:12;	/* default:; */
		__u32 tcon1_field_polarity:1;	/* default:; */
		__u32 tcon0_field_polarity:1;	/* default:; */
		__u32 tcon1_fifo_under_flow:1;	/* default:; */
		__u32 tcon0_fifo_under_flow:1;	/* default:; */
	} bits;
};

union tcon_ceu_ctl_reg_t {
	__u32 dwval;
	struct {
		__u32 res0:31;	/* default:; */
		__u32 ceu_en:1;	/* default: 0; */
	} bits;
};

union tcon_ceu_coef_mul_reg_t {
	__u32 dwval;
	struct {
		__u32 value:13;	/* default: 0; */
		__u32 res0:19;	/* default:; */
	} bits;
};

union tcon_ceu_coef_add_reg_t {
	__u32 dwval;
	struct {
		__u32 value:19;	/* default: 0; */
		__u32 res0:13;	/* default:; */
	} bits;
};

union tcon_ceu_coef_rang_reg_t {
	__u32 dwval;
	struct {
		__u32 max:8;	/* default: 0; */
		__u32 res0:8;	/* default:; */
		__u32 min:8;	/* default: 0; */
		__u32 res1:8;	/* default:; */
	} bits;
};

union tcon0_cpu_tri0_reg_t {
	__u32 dwval;
	struct {
		__u32 block_size:12;	/* default: 0; */
		__u32 res0:4;	/* default:; */
		__u32 block_space:12;	/* default: 0; */
		__u32 res1:4;	/* default:; */
	} bits;
};

union tcon0_cpu_tri1_reg_t {
	__u32 dwval;
	struct {
		__u32 block_num:16;	/* default: 0; */
		__u32 block_current_num:16;	/* default: 0; */
	} bits;
};

union tcon0_cpu_tri2_reg_t {
	__u32 dwval;
	struct {
		__u32 trans_start_set:13;	/* default: 0; */
		__u32 sync_mode:2;	/* default: 0; */
		__u32 trans_start_mode:1;	/* default: 0; */
		__u32 start_delay:16;	/* default: 0x20; */
	} bits;
};

union tcon0_cpu_tri3_reg_t {
	__u32 dwval;
	struct {
		__u32 counter_m:8;	/* default: 0; */
		__u32 counter_n:16;	/* default: 0; */
		__u32 res0:4;	/* default:; */
		__u32 tri_int_mode:2;	/* default: 0; */
		__u32 res1:2;	/* default:; */
	} bits;
};

union tcon_cmap_ctl_reg_t {
	__u32 dwval;
	struct {
		__u32 out_format:1;	/* default: 0; */
		__u32 res0:30;	/* default:; */
		__u32 cmap_en:1;	/* default: 0; */
	} bits;
};

union tcon_cmap_odd0_reg_t {
	__u32 dwval;
	struct {
		__u32 out0:16;	/* default: 0; */
		__u32 out1:16;	/* default: 0; */
	} bits;
};

union tcon_cmap_odd1_reg_t {
	__u32 dwval;
	struct {
		__u32 out2:16;	/* default: 0; */
		__u32 out3:16;	/* default: 0; */
	} bits;
};

union tcon_cmap_even0_reg_t {
	__u32 dwval;
	struct {
		__u32 out0:16;	/* default: 0; */
		__u32 out1:16;	/* default: 0; */
	} bits;
};

union tcon_cmap_even1_reg_t {
	__u32 dwval;
	struct {
		__u32 out2:16;	/* default: 0; */
		__u32 out3:16;	/* default: 0; */
	} bits;
};

union tcon_safe_period_reg_t {
	__u32 dwval;
	struct {
		__u32 safe_period_mode:2;	/* default: 0; */
		__u32 res0:14;	/* default:; */
		__u32 safe_period_fifo_num:13;	/* default: 0; */
		__u32 res1:3;	/* default:; */
	} bits;
};

union tcon_mux_ctl_reg_t {
	__u32 dwval;
	struct {
		__u32 dsi_src:2;	/* default: 0; */
		__u32 res0:6;	/* default:; */
		__u32 hdmi_src:2;	/* default: 0; */
		__u32 res1:22;	/* default:; */
	} bits;
};

union tcon0_lvds_ana_reg_t {
	__u32 dwval;
	struct {
		__u32 pwsmb:1;	/* default: 0; */
		__u32 pwslv:1;	/* default: 0; */
		__u32 res0:2;	/* default:; */
		__u32 pd:2;	/* default: 0; */
		__u32 res1:2;	/* default:; */
		__u32 v:2;	/* default: 0; */
		__u32 res2:2;	/* default:; */
		__u32 den:4;	/* default: 0; */
		__u32 denc:1;	/* default: 0; */
		__u32 c:2;	/* default: 0; */
		__u32 res3:1;	/* default:; */
		__u32 en_drvd:4;	/* default: 0; */
		__u32 en_drvc:1;	/* default: 0; */
		__u32 res4:5;	/* default:; */
		__u32 en_ldo:1;	/* default: 0; */
		__u32 en_mb:1;	/* default: 0; */
	} bits;
};

union tcon1_fill_ctl_reg_t {
	__u32 dwval;
	struct {
		__u32 res0:31;	/* default:; */
		__u32 tcon1_fill_en:1;	/* default: 0; */
	} bits;
};

union tcon1_fill_begin_reg_t {
	__u32 dwval;
	struct {
		__u32 fill_begin:24;	/* default: 0; */
		__u32 res0:8;	/* default:; */
	} bits;
};

union tcon1_fill_end_reg_t {
	__u32 dwval;
	struct {
		__u32 fill_end:24;	/* default: 0; */
		__u32 res0:8;	/* default:; */
	} bits;
};

union tcon1_fill_data_reg_t {
	__u32 dwval;
	struct {
		__u32 fill_value:24;	/* default: 0; */
		__u32 res0:8;	/* default:; */
	} bits;
};

union tcon_gamma_tlb_reg_t {
	__u32 dwval;
	struct {
		__u32 pixel:24;	/* default: 0; */
		__u32 res0:8;	/* default:; */
	} bits;
	struct {
		__u32 blue:8;	/* default: 0; */
		__u32 green:8;	/* default: 0; */
		__u32 red:8;	/* default: 0; */
		__u32 res0:8;	/* default:; */
	} bytes;
};

union tcon_reservd_reg_t {
	__u32 dwval;
	struct {
		__u32 res0;	/* default:; */
	} bits;
};

/* device define */
struct __de_lcd_dev_t {
	union tcon_gctl_reg_t tcon_gctl;	/* 0x000 */
	union tcon_gint0_reg_t tcon_gint0;	/* 0x004 */
	union tcon_gint1_reg_t tcon_gint1;	/* 0x008 */
	union tcon_reservd_reg_t tcon_reg00c;	/* 0x00c */
	union tcon0_frm_ctl_reg_t tcon0_frm_ctl;	/* 0x010 */
	union tcon0_frm_seed_reg_t tcon0_frm_seed_pr;	/* 0x014 */
	union tcon0_frm_seed_reg_t tcon0_frm_seed_pg;	/* 0x018 */
	union tcon0_frm_seed_reg_t tcon0_frm_seed_pb;	/* 0x01c */
	union tcon0_frm_seed_reg_t tcon0_frm_seed_lr;	/* 0x020 */
	union tcon0_frm_seed_reg_t tcon0_frm_seed_lg;	/* 0x024 */
	union tcon0_frm_seed_reg_t tcon0_frm_seed_lb;	/* 0x028 */
	union tcon0_frm_tab_reg_t tcon0_frm_tbl_0;	/* 0x02c */
	union tcon0_frm_tab_reg_t tcon0_frm_tbl_1;	/* 0x030 */
	union tcon0_frm_tab_reg_t tcon0_frm_tbl_2;	/* 0x034 */
	union tcon0_frm_tab_reg_t tcon0_frm_tbl_3;	/* 0x038 */
	union tcon_reservd_reg_t tcon_reg03c;	/* 0x03c */
	union tcon0_ctl_reg_t tcon0_ctl;	/* 0x040 */
	union tcon0_dclk_reg_t tcon0_dclk;	/* 0x044 */
	union tcon0_basic0_reg_t tcon0_basic0;	/* 0x048 */
	union tcon0_basic1_reg_t tcon0_basic1;	/* 0x04c */
	union tcon0_basic2_reg_t tcon0_basic2;	/* 0x050 */
	union tcon0_basic3_reg_t tcon0_basic3;	/* 0x054 */
	union tcon0_hv_if_reg_t tcon0_hv_ctl;	/* 0x058 */
	union tcon_reservd_reg_t tcon_reg05c;	/* 0x05c */
	union tcon0_cpu_if_reg_t tcon0_cpu_ctl;	/* 0x060 */
	union tcon0_cpu_wr_reg_t tcon0_cpu_wr;	/* 0x064 */
	union tcon0_cpu_rd0_reg_t tcon0_cpu_rd;	/* 0x068 */
	union tcon0_cpu_rd1_reg_t tcon0_cpu_fet;	/* 0x06c */
	/* 0x070~0x80 */
	union tcon_reservd_reg_t tcon_reg070[5];
	union tcon0_lvds_if_reg_t tcon0_lvds_ctl;	/* 0x084 */
	union tcon0_io_pol_reg_t tcon0_io_pol;	/* 0x088 */
	union tcon0_io_tri_reg_t tcon0_io_tri;	/* 0x08c */
	union tcon1_ctl_reg_t tcon1_ctl;	/* 0x090 */
	union tcon1_basic0_reg_t tcon1_basic0;	/* 0x094 */
	union tcon1_basic1_reg_t tcon1_basic1;	/* 0x098 */
	union tcon1_basic2_reg_t tcon1_basic2;	/* 0x09c */
	union tcon1_basic3_reg_t tcon1_basic3;	/* 0x0a0 */
	union tcon1_basic4_reg_t tcon1_basic4;	/* 0x0a4 */
	union tcon1_basic5_reg_t tcon1_basic5;	/* 0x0a8 */
	union tcon_reservd_reg_t tcon_reg0ac;	/* 0x0ac */
	union tcon1_ps_sync_reg_t tcon1_ps_ctl;	/* 0x0b0 */
	/* 0x0b4~0x0ec */
	union tcon_reservd_reg_t tcon_reg0b4[15];
	union tcon1_io_pol_reg_t tcon1_io_pol;	/* 0x0f0 */
	union tcon1_io_tri_reg_t tcon1_io_tri;	/* 0x0f4 */
	union tcon_ecc_fifo_reg_t tcon_ecfifo_ctl;	/* 0x0f8 */
	union tcon_debug_reg_t tcon_debug;	/* 0x0fc */
	union tcon_ceu_ctl_reg_t tcon_ceu_ctl;	/* 0x110 */
	/* 0x104~0x10c */
	union tcon_reservd_reg_t tcon_reg104[3];
	union tcon_ceu_coef_mul_reg_t tcon_ceu_coef_rr;	/* 0x110 */
	union tcon_ceu_coef_mul_reg_t tcon_ceu_coef_rg;	/* 0x114 */
	union tcon_ceu_coef_mul_reg_t tcon_ceu_coef_rb;	/* 0x118 */
	union tcon_ceu_coef_add_reg_t tcon_ceu_coef_rc;	/* 0x11c */
	union tcon_ceu_coef_mul_reg_t tcon_ceu_coef_gr;	/* 0x120 */
	union tcon_ceu_coef_mul_reg_t tcon_ceu_coef_gg;	/* 0x124 */
	union tcon_ceu_coef_mul_reg_t tcon_ceu_coef_gb;	/* 0x128 */
	union tcon_ceu_coef_add_reg_t tcon_ceu_coef_gc;	/* 0x12c */
	union tcon_ceu_coef_mul_reg_t tcon_ceu_coef_br;	/* 0x130 */
	union tcon_ceu_coef_mul_reg_t tcon_ceu_coef_bg;	/* 0x134 */
	union tcon_ceu_coef_mul_reg_t tcon_ceu_coef_bb;	/* 0x138 */
	union tcon_ceu_coef_add_reg_t tcon_ceu_coef_bc;	/* 0x13c */
	union tcon_ceu_coef_rang_reg_t tcon_ceu_coef_rv;	/* 0x140 */
	union tcon_ceu_coef_rang_reg_t tcon_ceu_coef_gv;	/* 0x144 */
	union tcon_ceu_coef_rang_reg_t tcon_ceu_coef_bv;	/* 0x148 */
	/* 0x14c~0x15c */
	union tcon_reservd_reg_t tcon_reg14c[5];
	union tcon0_cpu_tri0_reg_t tcon0_cpu_tri0;	/* 0x160 */
	union tcon0_cpu_tri1_reg_t tcon0_cpu_tri1;	/* 0x164 */
	union tcon0_cpu_tri2_reg_t tcon0_cpu_tri2;	/* 0x168 */
	union tcon0_cpu_tri3_reg_t tcon0_cpu_tri3;	/* 0x16c */
	/* 0x170~0x17c */
	union tcon_reservd_reg_t tcon_reg170[4];
	/* 0x180 */
	union tcon_cmap_ctl_reg_t tcon_cmap_ctl;
	/* 0x184~0x18c */
	union tcon_reservd_reg_t tcon_reg184[3];
	union tcon_cmap_odd0_reg_t tcon_cmap_odd0;	/* 0x190 */
	union tcon_cmap_odd1_reg_t tcon_cmap_odd1;	/* 0x194 */
	union tcon_cmap_even0_reg_t tcon_cmap_even0;	/* 0x198 */
	union tcon_cmap_even1_reg_t tcon_cmap_even1;	/* 0x19c */
	/* 0x1a0~0x1ec */
	union tcon_reservd_reg_t tcon_reg1a0[20];
	union tcon_safe_period_reg_t tcon_volume_ctl;	/* 0x1f0 */
	/* 0x1f4~0x1fc */
	union tcon_reservd_reg_t tcon_reg1f4[3];
	/* 0x200 */
	union tcon_mux_ctl_reg_t tcon_mul_ctl;
	/* 0x204~0x21c */
	union tcon_reservd_reg_t tcon_reg204[7];
	/* 0x220~0x224 */
	union tcon0_lvds_ana_reg_t tcon0_lvds_ana[2];
	/* 0x228~0x2fc */
	union tcon_reservd_reg_t tcon_reg228[54];
	union tcon1_fill_ctl_reg_t tcon_fill_ctl;	/* 0x300 */
	union tcon1_fill_begin_reg_t tcon_fill_start0;	/* 0x304 */
	union tcon1_fill_end_reg_t tcon_fill_end0;	/* 0x308 */
	union tcon1_fill_data_reg_t tcon_fill_data0;	/* 0x30c */
	union tcon1_fill_begin_reg_t tcon_fill_start1;	/* 0x310 */
	union tcon1_fill_end_reg_t tcon_fill_end1;	/* 0x314 */
	union tcon1_fill_data_reg_t tcon_fill_data1;	/* 0x318 */
	union tcon1_fill_begin_reg_t tcon_fill_start2;	/* 0x31c */
	union tcon1_fill_end_reg_t tcon_fill_end2;	/* 0x320 */
	union tcon1_fill_data_reg_t tcon_fill_data2;	/* 0x324 */
	/* 0x328~0x3fc */
	union tcon_reservd_reg_t tcon_reg328[54];
	union tcon_gamma_tlb_reg_t tcon_gamma_tlb[256];	/* 0x400 */
};

/* __s32 tcon0_cfg_mode_auto(__u32 sel, disp_panel_para * panel); */
/* __s32 tcon0_cfg_mode_tri(__u32 sel, disp_panel_para * panel); */
__s32 tcon_cmap(__u32 sel, __u32 mode, unsigned int lcd_cmap_tbl[2][3][4]);
__s32 tcon_gamma(__u32 sel, __u32 mode, __u32 gamma_tbl[256]);
__s32 tcon_ceu(__u32 sel, __u32 mode, __s32 b, __s32 c, __s32 s, __s32 h);
__s32 tcon0_frm(__u32 sel, __u32 mode);
__u32 tcon0_cpu_16b_to_24b(__u32 value);
__u32 tcon0_cpu_24b_to_16b(__u32 value);

extern __u32 dsi_pixel_bits[4];
extern __u32 tcon_div;
extern __s32 bsp_disp_lcd_delay_us(__u32 ns);

#endif
