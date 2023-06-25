 //SPDX-License-Identifier: GPL-2.0

#ifndef __NAND_OSAL_UBOOT_H__
#define __NAND_OSAL_UBOOT_H__

#include "aw_nand_type.h"
#include <asm/types.h>

void *NAND_Malloc(unsigned int Size);
void NAND_Free(void *pAddr, unsigned int Size);
int NAND_PhysicLockInit(void);
int NAND_PhysicLock(void);
int NAND_PhysicUnLock(void);
int NAND_PhysicLockExit(void);
__u32 NAND_GetMaxChannelCnt(void);
int NAND_Print(const char *str, ...);
int NAND_Print_DBG(const char *str, ...);
__u32 get_storage_type(void);
int NAND_IS_Burn_Mode(void);
extern int (*NAND_ClkRequest) (__u32 NAND_index);
extern void (*NAND_ClkRelease) (__u32 nand_index);
extern int (*NAND_SetClk) (__u32 nand_index, __u32 nand_clk0,
		__u32 nand_clk1);
extern int (*NAND_GetClk) (__u32 nand_index, __u32 *pnand_clk0, __u32 *pnand_clk1);
void nand_enable_vccq_1p8v(void);
void nand_enable_vccq_3p3v(void);
/*
 *extern int (*NAND_GetClk) (__u32 nand_index, __u32 *pnand_clk0,
 *                __u32 *pnand_clk1);
 */

__s32 NAND_PIORequest(__u32 nand_index);
void NAND_PIORelease(__u32 nand_index);
int NAND_ReleaseDMA(__u32 nand_index);
int NAND_ReleaseVoltage(void);
int NAND_GetVoltage(void);
__s32 NAND_Check_3DNand(void);
__s32 NAND_3DNand_Request(void);
__u32 NAND_GetNdfcVersion(void);
__u32 NAND_GetNdfcDmaMode(void);
__s32 nand_dma_wait_time_out(__u32 no, __u32 *flag);
__s32 nand_rb_wake_up(__u32 no);
__s32 nand_dma_wake_up(__u32 no);
void *NAND_DMASingleMap(__u32 rw, void *buff_addr, __u32 len);
void *NAND_DMASingleUnmap(__u32 rw, void *buff_addr, __u32 len);
__s32 NAND_InvaildDCacheRegion(__u32 rw, __u32 buff_addr, __u32 len);
void *nand_malloc(unsigned int size);
void nand_free(void *pAddr);
__u32 NAND_GetNandIDNumCtrl(void);
__u32 NAND_GetNandExtPara(__u32 para_num);
int nand_request_dma(void);
__s32 NAND_CleanFlushDCacheRegion(void *buff_addr, __u32 len);
int rawnand_dma_config_start(__u32 write, dma_addr_t addr, __u32 length);
int nand_dma_config_start(__u32 rw, __u32 addr, __u32 length);
void *RAWNAND_GetIOBaseAddrCH0(void);
void *RAWNAND_GetIOBaseAddrCH1(void);
void *SPINAND_GetIOBaseAddrCH0(void);
void *SPINAND_GetIOBaseAddrCH1(void);

#endif

