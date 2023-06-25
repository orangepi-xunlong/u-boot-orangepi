/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 */

#ifndef _SUNXI_RTC_H
#define _SUNXI_RTC_H

void rtc_write_data(int index, u32 val);
u32  rtc_read_data(int index);
void rtc_set_fel_flag(void);
u32  rtc_probe_fel_flag(void);
void rtc_clear_fel_flag(void);
int rtc_get_bootmode_flag(void);
int rtc_set_bootmode_flag(u8 flag);


#endif /* _SUNXI_RTC_H */
