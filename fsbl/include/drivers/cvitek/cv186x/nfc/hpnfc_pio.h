 /*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef HPNFC_PIO_H
#define HPNFC_PIO_H


#include "hpnfc_core.h"

#define HPNFC_PIO_CMD_REG0_THREAD_WRITE         HPNFC_CMD_REG0_THREAD_WRITE

#define HPNFC_PIO_CMD_REG0_CC_SHIFT             0
/* command code field mask */
#define HPNFC_PIO_CMD_REG0_CC_MASK              (0xFFFFuL << 0)
/* command code - read page */
#define HPNFC_PIO_CMD_REG0_CC_RD                (0x2200uL)
/* command code - write page */
#define HPNFC_PIO_CMD_REG0_CC_WR                (0x2100uL)
/* command code - copy back */
#define HPNFC_PIO_CMD_REG0_CC_CPB               (0x1200uL)
/* command code - reset */
#define HPNFC_PIO_CMD_REG0_CC_RST               (0x1100uL)
/* command code - set feature */
#define HPNFC_PIO_CMD_REG0_CC_SF                (0x0100uL)
/* command code - erase block */
#define HPNFC_PIO_CMD_REG0_CC_ER                (0x1000uL)
/* command interrupt shift */
#define HPNFC_PIO_CMD_REG0_INT_SHIFT             20
/* command interrupt mask */
#define HPNFC_PIO_CMD_REG0_INT_MASK              HPNFC_CMD_REG0_INT_MASK
/* select DMA master engine for this command */
#define HPNFC_PIO_CMD_REG0_DMA_MASTER_MASK       (1uL << 21)


#define HPNFC_PIO_CMD_REG0_VOL_ID_WRITE(vId)    ((vId & 0xF) << 16)


#define HPNFC_PIO_CMD_REG1_BANK_WRITE(bank)     ((bank & 3) << 24)
#define HPNFC_PIO_CMD_REG1_FADDR_WRITE(fAddr)   (fAddr & 0xFF)
#define HPNFC_PIO_CMD_REG1_ROW_ADRR_WRITE(row)  (row & 0xFF)

#endif
