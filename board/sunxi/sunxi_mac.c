/*
 * (C) Copyright 2016
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *zhouhuacai <zhouhuacai@allwinnertech.com>
 *
 * SPDX-License-Identifier:›GPL-2.0+
 */
#include <common.h>
#include <smc.h>
#include <u-boot/md5.h>
#include <sunxi_board.h>
#include <sys_partition.h>
#include <sys_config.h>

static int str2num(char *str, char *num)
{
	int val = 0, i;
	char *p = str;
	for (i = 0; i < 2; i++) {
		val *= 16;
		if (*p >= '0' && *p <= '9')
			val += *p - '0';
		else if (*p >= 'A' && *p <= 'F')
			val += *p - 'A' + 10;
		else if (*p >= 'a' && *p <= 'f')
			val += *p - 'a' + 10;
		else
			return -1;
		p++;
	}
	*num = val;
	return 0;
}


static int addr_parse(const char *addr_str, int check)
{
	char addr[6];
	char cmp_buf[6];
	char *p = (char *)addr_str;
	int i;
	if (!p || strlen(p) < 17)
		return -1;

	for (i = 0; i < 6; i++) {
		if (str2num(p, &addr[i]))
			return -1;

		p += 2;
		if ((i < 5) && (*p != ':'))
			return -1;

		p++;
	}

	if (check && (addr[0] & 0x3))
		return -1;

	memset(cmp_buf, 0x00, 6);
	if (memcmp(addr, cmp_buf, 6) == 0)
		return -1;

	memset(cmp_buf, 0xFF, 6);
	if (memcmp(addr, cmp_buf, 6) == 0)
		return -1;

	return 0;
}

int update_sunxi_mac(void)
{
	char *p		   = NULL;
	int i		   = 0;

	char *envtab[] = { "mac", "wifi_mac", "bt_mac" };

	int checktab[] = { 1, 1, 0 };

	for (i = 0; i < sizeof(envtab) / sizeof(envtab[0]); i++) {
		p = env_get(envtab[i]);
		if (p != NULL) {
			if (addr_parse(p, checktab[i]) == 0) {
				continue;
			} else {
				/*if not pass, clean it, do not pass through cmdline*/
				pr_err("%s format illegal\n", envtab[i]);
				env_set(envtab[i], "");
			}
		}
	}

	return 0;
}
