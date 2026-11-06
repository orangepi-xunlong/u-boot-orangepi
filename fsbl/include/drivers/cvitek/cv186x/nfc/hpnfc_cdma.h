 /*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef HPNFC_CDMA_H
#define HPNFC_CDMA_H

#include <hpnfc_if.h>
#include "hpnfc_core.h"

/***************************************************/
/*  CDMA descriptor fields */
/***************************************************/

/** command DMA descriptor type - erase command */
#define HPNFC_CDMA_CT_ERASE         0x1000
/** command DMA descriptor type - reset command */
#define HPNFC_CDMA_CT_RST           0x1100
/** command DMA descriptor type - copy back command */
#define HPNFC_CDMA_CT_CPYB          0x1200
/** command DMA descriptor type - write page command */
#define HPNFC_CDMA_CT_WR            0x2100
/** command DMA descriptor type - read page command */
#define HPNFC_CDMA_CT_RD            0x2200
/** command DMA descriptor type - nop command */
#define HPNFC_CDMA_CT_NOP           0xFFFF

/** flash pointer memory - shift */
#define HPNFC_CDMA_CFPTR_MEM_SHIFT  24
/** flash pointer memory - mask */
#define HPNFC_CDMA_CFPTR_MEM_MASK   (7uL << 24)
/** flash pointer row addr - mask */
#define HPNFC_CDMA_CFPTR_ROW_MASK   (0xFFFFFF)


/* command DMA descriptor flags - selects DMA master */
#define HPNFC_CDMA_CF_DMA_MASTER    (1uL << 10)
/** command DMA descriptor flags - the next descriptor
 * address field is valid and descriptor processing should continue */
#define HPNFC_CDMA_CF_CONT          (1uL << 9)
/** command DMA descriptor flags - issue interrupt after
 * the completion of descriptor processing */
#define HPNFC_CDMA_CF_INT           (1uL << 8)
/*This field informs the controller the volume this current command
 * is targeting to.*/
#define HPNFC_CDMA_CF_VOL_SHIFT     4
#define HPNFC_CDMA_CF_VOL_MASK      (0xFuL << 4)
#define HPNFC_CDMA_CF_FL_PTR_CON    (0x1uL << 1)
#define HPNFC_CDMA_CF_MEM_PTR_CON   (0x1uL << 0)


#define HPNFC_CDMA_SYNC_SV_WRITE(startValue)    ((uint32_t)startValue << 24)
#define HPNFC_CDMA_SYNC_SV_READ(syncArg)        ((syncArg >> 24) & 0xFF)
#define HPNFC_CDMA_SYNC_EV_WRITE(endValue)      ((uint32_t)endValue << 16)
#define HPNFC_CDMA_SYNC_EV_READ(syncArg)        ((syncArg >> 16) & 0xFF)
#define HPNFC_CDMA_SYNC_VALID_MASK              (1uL << 2)
#define HPNFC_CDMA_SYNC_GREAT_MASK              (1uL << 1)
#define HPNFC_CDMA_SYNC_INC_MASK                (1uL << 0)


/***************************************************/

/* Command DMA descriptor */
typedef struct HPNFC_CdmaDesc {
	/* next descriptor address */
	uint64_t nextPointer;
	/* glash address is a 32-bit address comprising of BANK and ROW ADDR. */
	uint32_t flashPointer;
	uint32_t rsvd0;
	/* operation the controller needs to perform */
	uint16_t commandType;
	/* number of commands executed with single descriptor */
	uint16_t commandCounter;
	/* flags for operation of this command */
	uint16_t commandFlags;
	uint16_t rsvd1;
	/* system/host memory address required for data DMA commands. */
	uint64_t memoryPointer;
	/* status of operation */
	uint32_t status[2];
	/* address pointer to sync buffer location */
	uint64_t syncFlagPointer;
	/* Controls the buffer sync mechanism. */
	uint32_t syncArguments;
	uint32_t rsvd3;
	/* dummy bytes */
	uint64_t rsvd4;
} HPNFC_CdmaDesc;


void DumpDescriptor(HPNFC_CdmaDesc* cdmaDesc);
int32_t HPNFC_Cdma_DescPrepare(HPNFC_State *hpnfc, HPNFC_CdmaDesc* CdmaDesc, HPNFC_CdmaCmd* cdmaCmd);
int32_t HPNFC_Cdma_CheckChainStatus(HPNFC_CdmaDesc* cdmaDesc);
void HPNFC_Cdma_GetFailed(HPNFC_CdmaDesc* cdmaDesc, HPNFC_CdmaDesc** failed);
void HPNFC_Cdma_GetNextFailed(HPNFC_CdmaDesc* cdmaDesc, HPNFC_CdmaDesc** failed);
void HPNFC_Cdma_DescParse(HPNFC_State *hpnfc, HPNFC_CdmaDesc* cdmaDesc, HPNFC_CdmaCmd* cdmaCmd);
HPNFC_CdmaDesc* HPNFC_Cdma_GetUnfinished(HPNFC_CdmaDesc* cdmaDesc);

#endif
