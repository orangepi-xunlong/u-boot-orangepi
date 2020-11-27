/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>

/*
 * 64bit arch timer.CNTPCT
 * Freq = 24000000Hz
 */
static inline u64 get_arch_counter(void)
{
	u32 low=0, high = 0;
	asm volatile("mrrc p15, 0, %0, %1, c14"
		: "=r" (low), "=r" (high)
		:
		: "memory");
	return (((u64)high)<<32 | low);
}

/*
 * get current time.(millisecond)
 */
u32 get_sys_ticks(void)
{
	return (u32)get_arch_counter()/24000;
}


__weak void __usdelay(unsigned long us)
{
	u64 t1, t2;

	t1 = get_arch_counter();
	t2 = t1 + us*24;
	do
	{
		t1 = get_arch_counter();
	}
	while(t2 >= t1);
}

__weak void __msdelay(unsigned long ms)
{
	__usdelay(ms*1000);
}

__weak int timer_init(void)
{
	return 0;
}

__weak void timer_exit(void)
{

}


