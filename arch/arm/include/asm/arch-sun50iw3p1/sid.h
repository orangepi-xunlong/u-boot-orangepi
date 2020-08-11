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

#define SID_PRCTL               (SUNXI_SID_BASE + 0x40)
#define SID_PRKEY               (SUNXI_SID_BASE + 0x50)
#define SID_RDKEY               (SUNXI_SID_BASE + 0x60)
#define SJTAG_AT0               (SUNXI_SID_BASE + 0x80)
#define SJTAG_AT1               (SUNXI_SID_BASE + 0x84)
#define SJTAG_S                 (SUNXI_SID_BASE + 0x88)

#define SID_EFUSE               (SUNXI_SID_BASE + 0x200)
#define SID_SECURE_MODE         (SUNXI_SID_BASE + 0xA0)
#define SID_OP_LOCK  (0xAC)

#define EFUSE_CHIPD             (0x00) /* 0x0-0xf, 128bits */
#define EFUSE_BROM_CONFIG       (0x10) /* 16 bit config, 16 bits try */
#define EFUSE_THERMAL_SENSOR    (0x14) /* 0x14-0x1b, 64bits */
#define EFUSE_TF_ZONE           (0x1C) /* 0x1c-0x2b, 128bits */
#define EFUSE_OEM_PROGRAM       (0x2C) /* 0x2c-0x3f, 160bits */

/* write protect */
#define EFUSE_WRITE_PROTECT     (0x40) /* 0x40-0x43, 32bits */
/* read  protect */
#define EFUSE_READ_PROTECT      (0x44) /* 0x44-0x47, 32bits */
/* jtag security */
#define EFUSE_LCJS              (0x48)
/* jtag attribute */
#define EFUSE_ATTR              (0x4C)

/*0x50-0x6F
0x50-0x67: 192bits HUK hardware Unique key
0x68-0x6b: 32bits ID of operator
0x6C-0x6F: 32bits ID
*/
#define EFUSE_HUK               (0x50)
#define EFUSE_INDENTIFICATION   (0x68)
#define EFUSE_ID                (0x6C)

#define EFUSE_ROTPK             (0x70) /* 0x70-0x8f, 256bits */
#define EFUSE_SSK               (0x90) /* 0x90-0x9f, 128bits */
#define EFUSE_RSSK              (0xA0) /* 0xA0-0xBf, 256bits */
#define EFUSE_HDCP_HASH         (0xC0) /* 0xC0-0xCf, 128bits */
#define EFUSE_EK_HASH           (0xD0) /* 0xD0-0xDf, 128bits */
#define EFUSE_SN                (0xE0) /* 0xE0-0xF7, 192bits */
#define EFUSE_NV1               (0xF8) /* 0xF8-0xFB, 32 bits */
#define EFUSE_NV2               (0xFC) /* 0xFC-0x117,224bits */
#define EFUSE_BACKUP_KEY        (0x118)/* 0x118-0x13F,320bits */


extern void sid_set_security_mode(void);
extern int  sid_probe_security_mode(void);
extern int sid_get_security_status(void);
extern uint sid_read_key(uint key_index);
extern void sid_program_key(uint key_index, uint key_value);

#endif    /*  #ifndef __EFUSE_H__  */
