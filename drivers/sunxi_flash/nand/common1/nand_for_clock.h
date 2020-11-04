/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __NAND_FOR_CLOCK_H__
#define __NAND_FOR_CLOCK_H__

#include <common.h>
#include <console.h>
#include <malloc.h>
#include <stdarg.h>
#include <asm/io.h>
#include <asm/arch/dma.h>
#include <asm/arch/clock.h>
#include <fdt_support.h>
#include <spare_head.h>
#include <sunxi_board.h>


typedef int (*NAND_ClkRequest_t)(__u32 nand_index);
typedef void (*NAND_ClkRelease_t)(__u32 nand_index);
typedef int (*NAND_SetClk_t)(__u32 nand_index, __u32 nand_clk0,
			__u32 nand_clk1);
typedef int (*NAND_GetClk_t)(__u32 nand_index, __u32 *pnand_clk0,
			__u32 *pnand_clk1);

__u32 _Getpll6Clk_v0(void);
__s32 _get_ndfc_clk_v0(__u32 nand_index, __u32 *pdclk, __u32 *pcclk);
__s32 change_ndfc_clk_v0(__u32 nand_index, __u32 dclk_src_sel, __u32 dclk,
			__u32 cclk_src_sel, __u32 cclk);
__s32 _close_ndfc_clk_v0(__u32 nand_index);
__s32 _open_ndfc_ahb_gate_and_reset_v0(__u32 nand_index);
__s32 _close_ndfc_ahb_gate_and_reset_v0(__u32 nand_index);
__s32 _cfg_ndfc_gpio_v0(__u32 nand_index);
int NAND_ClkRequest_v0(__u32 nand_index);
void NAND_ClkRelease_v0(__u32 nand_index);
int NAND_SetClk_v0(__u32 nand_index, __u32 nand_clk0, __u32 nand_clk1);
int NAND_GetClk_v0(__u32 nand_index, __u32 *pnand_clk0, __u32 *pnand_clk1);

__u32 _Getpll6Clk_v1(void);
__s32 _get_ndfc_clk_v1(__u32 nand_index, __u32 *pdclk, __u32 *pcclk);
__s32 _change_ndfc_clk_v1(__u32 nand_index, __u32 dclk_src_sel, __u32 dclk,
			__u32 cclk_src_sel, __u32 cclk);
__s32 _close_ndfc_clk_v1(__u32 nand_index);
__s32 _cfg_ndfc_gpio_v1(__u32 nand_index);
__s32 _open_ndfc_ahb_gate_and_reset_v1(__u32 nand_index);
__s32 _close_ndfc_ahb_gate_and_reset_v1(__u32 nand_index);
__s32 _get_spic_clk_v1(__u32 spinand_index, __u32 *pdclk);
__s32 _change_spic_clk_v1(__u32 spinand_index, __u32 dclk_src_sel, __u32 dclk);
__s32 _close_spic_clk_v1(__u32 spinand_index);
__s32 _open_spic_ahb_gate_and_reset_v1(__u32 spinand_index);
__s32 _close_spic_ahb_gate_and_reset_v1(__u32 spinand_index);
int NAND_ClkRequest_v1(__u32 nand_index);
void NAND_ClkRelease_v1(__u32 nand_index);
int NAND_SetClk_v1(__u32 nand_index, __u32 nand_clk0, __u32 nand_clk1);
int NAND_GetClk_v1(__u32 nand_index, __u32 *pnand_clk0, __u32 *pnand_clk1);

extern int NAND_Print(const char *str, ...);
extern __u32 NAND_GetNdfcVersion(void);
extern __u32 get_wvalue(__u32 addr);
extern void put_wvalue(__u32 addr, __u32 v);
extern __u32 get_storage_type(void);

#endif /*NAND_FOR_CLOCK_H*/
