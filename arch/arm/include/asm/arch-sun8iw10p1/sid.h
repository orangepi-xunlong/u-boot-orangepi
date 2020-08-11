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
#include <efuse_map.h>

#define SID_OP_LOCK  (0xAC)

#define SID_PRCTL			(SUNXI_SID_BASE + 0x40)
#define SID_PRKEY			(SUNXI_SID_BASE + 0x50)
#define SID_RDKEY			(SUNXI_SID_BASE + 0x60)

#define EFUSE_10 			(0x10)
#define EFUSE_14 			(0x14)
#define EFUSE_18 			(0x18)
#define EFUSE_1C 			(0x1C)

#define EFUSE_THERMAL_SENSOR    	(EFUSE_10)
#define EFUSE_RESERVED			(EFUSE_14)

// size (bit)
#define	SID_THERMAL_SIZE		(32)
#define	EFUSE_RESERVED_SIZE		(96)
#define EFUSE_CHIPCONFIG 0xFFFF/*it is a fake value*/

extern void sid_set_security_mode(void);
extern int  sid_probe_security_mode(void);
extern int sid_get_security_status(void);
extern uint sid_read_key(uint key_index);
extern void sid_program_key(uint key_index, uint key_value);


#endif    /*  #ifndef __EFUSE_H__  */
