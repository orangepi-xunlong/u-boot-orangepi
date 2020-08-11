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
#include <common.h>
#include <asm/io.h>
#include <asm/arch/timer.h>
#include "rtc_region.h"

#define  RTC_DATA_HOLD_REG_BASE        (SUNXI_RTC_BASE + 0x100)
#define  RTC_DATA_HOLD_REG_FEL         (RTC_DATA_HOLD_REG_BASE + 0x8)

extern void boot0_jmp_other(unsigned int addr);

static u32 before_crc;
static u32 after_crc;

extended_standby_t *pextended_standby = (extended_standby_t *)(STANDBY_SPACE_BASE_ADDR + EXTENDED_STANDBY_BASE_OFFSET);

pm_dram_para_t *ppm_dram_para;

/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
uint rtc_region_probe_fel_flag(void)
{
	uint fel_flag, reg_value;
	int  i;

	fel_flag = readl(RTC_DATA_HOLD_REG_FEL);

	for(i=0;i<=5;i++)
	{
		reg_value = readl(RTC_DATA_HOLD_REG_BASE + i*4);
		printf("rtc[%d] value = 0x%x\n", i, reg_value);
	}

	return fel_flag;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void rtc_region_clear_fel_flag(void)
{
	volatile uint flag = 0;
	do
	{
		writel(0, RTC_DATA_HOLD_REG_FEL);
		asm volatile("DSB");
		asm volatile("ISB");
		flag  = readl(RTC_DATA_HOLD_REG_FEL);
	}
	while(flag != 0);
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
s32 standby_dram_crc_enable(pm_dram_para_t *pdram_state)
{
	return pdram_state->crc_en;
}

/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
u32 standby_dram_crc(pm_dram_para_t *pdram_state)
{
	u32 *pdata;
	u32 crc = 0;

	pdata = (u32 *)(pdram_state->crc_start);
	printf ("src:0x%x \n", (unsigned int)pdata);
	printf ("len addr = 0x%x, len:0x%x \n", (unsigned int) (&(pdram_state->crc_len)), pdram_state->crc_len);
	while ( pdata < (u32 *) (pdram_state->crc_start + pdram_state->crc_len)) {
		crc += *pdata;
		pdata++;
	}
	printf ("crc finish...\n");
	return crc;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static int probe_super_standby_flag(void)
{
	uint reg_value = 0;
	int standby_flag = 0;

	reg_value = readl (RTC_STANDBY_FLAG_REG);
	standby_flag = (reg_value & ~(0xfffe0000)) >> 16;
	printf("rtc standby flag is 0x%x, super standby flag is 0x%x\n", reg_value, standby_flag);
	writel (0, RTC_STANDBY_FLAG_REG);

	return standby_flag;
}

/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void handler_super_standby(void)
{
	if (probe_super_standby_flag()) {
		ppm_dram_para = (pm_dram_para_t *)(&(pextended_standby->soc_dram_state));
		if (standby_dram_crc_enable (ppm_dram_para)) {
			before_crc = readl (DRAM_CRC_REG_ADDR);
			after_crc = standby_dram_crc (ppm_dram_para);
			printf ("before_crc = 0x%x, after_crc = 0x%x\n", before_crc, after_crc);
			if (before_crc != after_crc) {
				printf ("dram crc error ...\n");
				asm ("b .");
			}
		}
		dram_enable_all_master();
		//((volitale unsigned int *)0xf1c62000 + 0x94)
		printf ("find standby flag, jump to addr 0x%x \n", readl (RTC_STANDBY_SOFT_ENTRY_REG));
		__msdelay(500);
		boot0_jmp_other (readl (RTC_STANDBY_SOFT_ENTRY_REG));
	}
}

