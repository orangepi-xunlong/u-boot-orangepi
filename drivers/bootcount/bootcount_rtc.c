// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2013
 * Heiko Schocher, DENX Software Engineering, hs@denx.de.
 */

#include <common.h>
#include <environment.h>

extern u32 rtc_read_data(int index);
extern void rtc_write_data(int index, u32 val);

#define RTC_BOOTCOUNT_INDEX   7
#define RTC_BOOTCOUNT_OFFSET  0
#define RTC_BOOTCOUNT_BIT     8

#define RTC_BOOTCOUNT_MASK (((1 << RTC_BOOTCOUNT_BIT) -1) << RTC_BOOTCOUNT_OFFSET)

static void set_bootcount_to_rtc(ulong a)
{
	u32 val = ((a << RTC_BOOTCOUNT_OFFSET) & RTC_BOOTCOUNT_MASK);
	rtc_write_data(RTC_BOOTCOUNT_INDEX, val);
}

static ulong get_bootcount_from_rtc(void)
{
	u32 val = 0;
	val = (rtc_read_data(RTC_BOOTCOUNT_INDEX) & RTC_BOOTCOUNT_MASK);
	return (val >> RTC_BOOTCOUNT_OFFSET);
}

void bootcount_store(ulong a)
{
	int upgrade_available = env_get_ulong("upgrade_available", 10, 0);

	if (upgrade_available) {
		set_bootcount_to_rtc(a);
	}
}

ulong bootcount_load(void)
{
	int upgrade_available = env_get_ulong("upgrade_available", 10, 0);
	ulong val = 0;

	if (upgrade_available)
		val = get_bootcount_from_rtc();

	return val;
}
