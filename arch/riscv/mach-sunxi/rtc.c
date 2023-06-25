
/*
 * SPDX-License-Identifier: GPL-2.0+
 * Sunxi RTC data area ops
 *
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 */

#include <common.h>
#include <errno.h>
#include <asm/io.h>
#include <rtc.h>


#define EFEX_FLAG  (0x5AA5A55A)
#define RTC_FEL_INDEX  2
#define RTC_BOOT_INDEX 6


void rtc_write_data(int index, u32 val)
{
	writel(val, IOMEM_ADDR(SUNXI_RTC_DATA_BASE + index*4));
}

u32 rtc_read_data(int index)
{
	return readl(IOMEM_ADDR(SUNXI_RTC_DATA_BASE + index*4));
}

void rtc_set_fel_flag(void)
{
	do {
		rtc_write_data(RTC_FEL_INDEX, EFEX_FLAG);
		asm volatile("fence.i");
	} while (rtc_read_data(RTC_FEL_INDEX) != EFEX_FLAG);
}

u32 rtc_probe_fel_flag(void)
{
	return rtc_read_data(RTC_FEL_INDEX) == EFEX_FLAG ? 1 : 0;
}

void rtc_clear_fel_flag(void)
{
	do {
		rtc_write_data(RTC_FEL_INDEX, 0);
		asm volatile("fence.i");
	} while (rtc_read_data(RTC_FEL_INDEX) != 0);
}

int rtc_set_bootmode_flag(u8 flag)
{
	do {
		rtc_write_data(RTC_BOOT_INDEX, flag);
		asm volatile("fence.i");
	} while (rtc_read_data(RTC_BOOT_INDEX) != flag);

	return 0;
}

int rtc_get_bootmode_flag(void)
{
	uint boot_flag;

	/* operation should be same with kernel write rtc */
	boot_flag = rtc_read_data(RTC_BOOT_INDEX);

	return boot_flag;
}


