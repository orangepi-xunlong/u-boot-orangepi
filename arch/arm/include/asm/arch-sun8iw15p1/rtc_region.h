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

#ifndef __RTC_REGION_H__
#define __RTC_REGION_H__


#define  RTC_DATA_HOLD_REG_BASE        (SUNXI_RTC_BASE + 0x100)
#define  RTC_DATA_HOLD_REG_FEL         (RTC_DATA_HOLD_REG_BASE + 0x8)

#define  SUNXI_RTC_GPREG_NUM 6

uint rtc_region_probe_fel_flag(void);
void rtc_region_clear_fel_flag(void);
void rtc_region_set_fel_flag(int flag);


#endif    /*  #ifndef __RTC_REGION_H__  */
