/*
 * (C) Copyright 2017-2020
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#ifndef __SPINAND_OSAL__
#define __SPINAND_OSAL__

#include <common.h>
#include <asm/arch/gpio.h>
#include <private_boot0.h>
#include <asm/io.h>

extern int SPINAND_Print(const char * str, ...);
extern __u32 SPINAND_GetIOBaseAddr(void);
extern int SPINAND_ClkRequest(__u32 nand_index);
extern void SPINAND_ClkRelease(__u32 nand_index);
extern int SPINAND_SetClk(__u32 nand_index, __u32 nand_clock);
extern void SPINAND_PIORequest(__u32 nand_index);
extern void SPINAND_PIORelease(__u32 nand_index);
extern void* SPINAND_Malloc(unsigned int Size);


#endif
