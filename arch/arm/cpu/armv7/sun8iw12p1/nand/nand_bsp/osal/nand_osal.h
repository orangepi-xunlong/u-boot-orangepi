/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#ifndef __NAND_OSAL_H__
#define __NAND_OSAL_H__

#include "../nand_common.h"

#define __OS_EBASE_SYSTEM__
#define __OS_NAND_DBG__

#define NAND_IO_BASE_ADDR  0x04011000
#define NAND_CLK_BASE_ADDR 0x03001000

#define   NAND_IO_BASE_ADDR0   0x04011000
#define   NAND_IO_BASE_ADDR1   0x04012000

extern void *NAND_IORemap(unsigned int base_addr, unsigned int size);

//USE_SYS_CLK
extern int NAND_ClkRequest(unsigned int nand_index);
extern void NAND_ClkRelease(unsigned int nand_index);
extern int NAND_SetClk(unsigned int nand_index, unsigned int nand_clk);
extern int NAND_GetClk(unsigned int nand_index);

extern void NAND_PIORequest(unsigned int nand_index);
extern void NAND_PIORelease(unsigned int nand_index);

extern void NAND_EnRbInt(void);
extern void NAND_ClearRbInt(void);
extern int NAND_WaitRbReady(void);
extern int NAND_WaitDmaFinish(void);
extern void NAND_RbInterrupt(void);

extern void* NAND_Malloc(unsigned int Size);
extern void NAND_Free(void *pAddr, unsigned int Size);
extern int  NAND_Print(const char * str, ...);

//define the memory set interface
#define MEMSET(x,y,z)            			memset((x),(y),(z))

//define the memory copy interface
#define MEMCPY(x,y,z)                   	memcpy((x),(y),(z))

//define the memory alocate interface
#define MALLOC(x)                       	NAND_Malloc((x))

//define the memory release interface
#define FREE(x,size)                    	NAND_Free((x),(size))

//define the message print interface
extern int printf(const char *fmt, ...);
#define NAND_Print(fmt, args...) printf(fmt, ##args)
#define PRINT(fmt, args...)	 printf(fmt, ##args)

#endif //__NAND_OSAL_H__
