/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "disp_private.h"

s32 bsp_disp_delay_ms(u32 ms)
{
#if defined(__LINUX_PLAT__)
	u32 timeout = msecs_to_jiffies(ms);

	set_current_state(TASK_UNINTERRUPTIBLE);
	schedule_timeout(timeout);
#endif
#ifdef __BOOT_PLAT__
	/* assume cpu runs at 1000Mhz,10 clock one cycle */
	wBoot_timer_delay(ms);
#endif
#ifdef __UBOOT_PLAT__
	__msdelay(ms);
#endif
	return 0;
}

s32 bsp_disp_delay_us(u32 us)
{
#if defined(__LINUX_PLAT__)
	udelay(us);
#endif
#ifdef __UBOOT_PLAT__
	__usdelay(us);
#endif
	return 0;
}

static struct disp_notifier_block disp_notifier_list;

s32 disp_notifier_init(void)
{
	INIT_LIST_HEAD(&disp_notifier_list.list);
	return 0;
}

s32 disp_notifier_register(struct disp_notifier_block *nb)
{
	if (nb == NULL) {
		DE_WRN("hdl is NULL\n");
		return -1;
	}
	list_add_tail(&(nb->list), &(disp_notifier_list.list));
	return 0;
}

s32 disp_notifier_unregister(struct disp_notifier_block *nb)
{
	struct disp_notifier_block *ptr;

	if (nb == NULL) {
		DE_WRN("hdl is NULL\n");
		return -1;
	}
	list_for_each_entry(ptr, &disp_notifier_list.list, list) {
		if (ptr == nb) {
			list_del(&ptr->list);
			return 0;
		}
	}
	return -1;
}

s32 disp_notifier_call_chain(u32 event, u32 sel, void *v)
{
	struct disp_notifier_block *ptr;

	list_for_each_entry(ptr, &disp_notifier_list.list, list) {
		if (ptr->notifier_call)
			ptr->notifier_call(ptr, event, sel, v);
	}

	return 0;
}
