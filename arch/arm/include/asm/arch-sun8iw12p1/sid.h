/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
/*
**********************************************************************************************************************
*
*	 the Embedded Secure Bootloader System
*
*
*	 Copyright(C), 2006-2014, Allwinnertech Co., Ltd.
*        All Rights Reserved
*
* File    :
*
* By      :
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/

#ifndef __EFUSE_H__
#define __EFUSE_H__

#include "asm/arch/platform.h"

#define SID_PRCTL				(SUNXI_SID_BASE + 0x40)
#define SID_PRKEY				(SUNXI_SID_BASE + 0x50)
#define SID_RDKEY				(SUNXI_SID_BASE + 0x60)
#define SID_TIMING_REG			(SUNXI_SID_BASE + 0x90)
#define SID_VERSION_REG		(SUNXI_SID_BASE + 0xF0)
#define SID_BDG_ID				(SUNXI_SID_BASE + 0x0100)

#define SID_EFUSE               (SUNXI_SID_BASE + 0x200)
#define SID_OP_LOCK  (0xAC)

#define EFUSE_CHIPD             (0x00)
#define EFUSE_BROM_CONFIG		(0x10)
#define EFUSE_THERMAL_SENSOR    (0x14)
#define EFUSE_TF_ZONE		(0x20)
#define EFUSE_ROTPK             (0x30)
#define EFUSE_NV1               (0x50)
#define EFUSE_TVE               (0x54)
#define EFUSE_LCJS              (0x58)
#define EFUSE_WRITE_PROTECT     (0xBC)
#define EFUSE_READ_PROTECT      (0xBE)

extern void sid_set_security_mode(void);
extern int  sid_probe_security_mode(void);

#endif    /*  #ifndef __EFUSE_H__  */
