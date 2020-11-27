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

#ifndef __SMTA_H__
#define __SMTA_H__


#include "platform.h"

#define SPC_STATUS_REG(n)      (SUNXI_SPC_BASE + (n) * 0x10 + 0x00)
#define SPC_SET_REG(n)         (SUNXI_SPC_BASE + (n) * 0x10 + 0x04)
#define SPC_CLEAR_REG(n)       (SUNXI_SPC_BASE + (n) * 0x10 + 0x08)



void sunxi_spc_set_to_ns(uint type);
int sunxi_deassert_arisc(void);

#endif /* __SMTA_H__ */
