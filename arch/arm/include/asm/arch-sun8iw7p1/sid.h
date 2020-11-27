/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
/*
**********************************************************************************************************************
*
*						           the Embedded Secure Bootloader System
*
*
*						       Copyright(C), 2006-2014, Allwinnertech Co., Ltd.
*                                           All Rights Reserved
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
#define SJTAG_AT0				(SUNXI_SID_BASE + 0x80)
#define SJTAG_AT1				(SUNXI_SID_BASE + 0x84)
#define SJTAG_S					(SUNXI_SID_BASE + 0x88)
#define SID_RF(n)				(SUNXI_SID_BASE + (n) * 4 + 0x80)

#define SID_EFUSE               (SUNXI_SID_BASE + 0x200)
#define SID_OP_LOCK				(0xAC)


#define EFUSE_CHIPID            (0x00)
#define EFUSE_OEM_PROGRAM       (0x10)
#define EFUSE_NV1               (0x14)
#define EFUSE_NV2               (0x18)
#define EFUSE_RSAKEY_HASH       (0x20)
#define EFUSE_THERMAL_SENSOR    (0x34)
#define EFUSE_RENEWABILITY      (0x3C)
#define EFUSE_HUK               (0x44)
#define EFUSE_INDENTIFICATION   (0x5C)
#define EFUSE_ID                (0x60)
#define EFUSE_ROTPK             (0x64)
#define EFUSE_SSK               (0x84)
#define EFUSE_RSSK              (0x94)
#define EFUSE_HDCP_HASH         (0xB4)
#define EFUSE_EK_HUSH           (0xC4)
#define EFUSE_SN                (0xD4)
#define EFUSE_BACKUP_KEY        (0xEC)
#define EFUSE_LCJS              (0xF4)
#define EFUSE_ATTR              (0xF8)
#define EFUSE_CHIP_CONFIG       (0xFC)

/* write protect */
#define EFUSE_WRITE_PROTECT	EFUSE_CHIP_CONFIG
/* read  protect */
#define EFUSE_READ_PROTECT	EFUSE_CHIP_CONFIG

/* size (bit)*/
#define SID_CHIPID_SIZE			(128)
#define SID_OEM_PROGRAM_SIZE	(32)
#define SID_NV1_SIZE			(32)
#define SID_NV2_SIZE			(96)
#define SID_RSAKEY_HASH_SIZE	(160)
#define SID_THERMAL_SIZE		(64)
#define SID_RENEWABILITY_SIZE	(64)
#define SID_IN_SIZE			    (192)
#define SID_IDENTIFY_SIZE		(32)
#define SID_ID_SIZE			    (32)
#define SID_ROTPK_SIZE			(256)
#define SID_SSK_SIZE			(128)
#define SID_RSSK_SIZE			(256)
#define SID_HDCP_HASH_SIZE		(128)
#define SID_EK_HASH_SIZE		(128)
#define SID_SN_SIZE			    (192)
#define SID_BACKUPKEY_SIZE      (64)

/*chip config show flag*/
#define	SCC_SN_DONTSHOW_FLAG						(20)
#define	SCC_ID_DONTSHOW_FLAG						(19)
#define	SCC_IN_DONTSHOW_FLAG						(17)
#define	SCC_RSSK_DONTSHOW_FLAG						(16)
#define	SCC_SSK_DONTSHOW_FLAG						(15)
#define	SCC_ROTPK_DONTSHOW_FLAG						(14)

/*chip config burned flag*/
#define	SCC_SN_BURNED_FLAG						    (9)
#define	SCC_RSAKEYHASH_BURNED_FLAG					(8)
#define	SCC_EK_HASH_BURNED_FLAG						(7)
#define	SCC_HDCP_HASH_BURNED_FLAG					(6)
#define	SCC_RSSK_BURNED_FLAG					    (5)
#define	SCC_SSK_BURNED_FLAG							(4)
#define	SCC_ROTPK_BURNED_FLAG						(3)
#define	SCC_SECURE_ENABLE_BURNED_FLAG				(1)
#define	SCC_TEST_DISABLE							(0)

uint sid_read_key(uint key_index);
void sid_program_key(uint key_index, uint key_value);
void sid_set_security_mode(void);
int  sid_probe_security_mode(void);
int  sid_get_security_status(void);

#endif    /*  #ifndef __EFUSE_H__  */
