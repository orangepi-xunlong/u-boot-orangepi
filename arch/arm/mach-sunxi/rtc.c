
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
	writel(val, SUNXI_RTC_DATA_BASE + index*4);
}

u32 rtc_read_data(int index)
{
	return readl(SUNXI_RTC_DATA_BASE + index*4);
}

void rtc_set_fel_flag(void)
{
	do {
		rtc_write_data(RTC_FEL_INDEX, EFEX_FLAG);
		asm volatile("DSB");
		asm volatile("ISB");
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
		asm volatile("DSB");
		asm volatile("ISB");
	} while (rtc_read_data(RTC_FEL_INDEX) != 0);
}

int rtc_set_bootmode_flag(u8 flag)
{
	do {
		rtc_write_data(RTC_BOOT_INDEX, flag);
		asm volatile("DSB");
		asm volatile("ISB");
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

#ifdef CONFIG_SUNXI_REBOOT_SCRIPT
int do_reboot_script(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	u32 cnt;
	cnt = rtc_read_data(0);
	cnt++;
	if (argc < 2) {
		printf("input err\n");
		return -1;
	}
	if (argv[2]) {
		cnt = simple_strtoul(argv[2], NULL, 16);
		printf("set cnt to %d\n", cnt);
	}

	if (!strncmp(argv[1], "reset", sizeof("reset"))) {
		printf("reboot cnt:%d\n", cnt);
	} else if (!strncmp(argv[1], "efex", sizeof("efex"))) {
		printf("reboot efex cnt:%d\n", cnt);
	} else {
		printf("input err\n");
		return -1;
	}
	rtc_write_data(0, cnt);

	run_command(argv[1], 0);

	return 0;
}

U_BOOT_CMD(reboot_script, 6, 1, do_reboot_script, "reboot_script sub-system",
		"reboot_script efex [cnt]\n"
		"reboot_script reset [cnt]\n");
#endif

int rtc_set_dcxo_off(void)
{
	__attribute__((unused)) u32 reg_val;
#ifdef CONFIG_MACH_SUN50IW10
	/* set wifi off */
	reg_val = readl(RTC_XO_CTRL_REG);
	reg_val |= (1 << 31);
	writel(reg_val, RTC_XO_CTRL_REG);
#endif
	return 0;
}
