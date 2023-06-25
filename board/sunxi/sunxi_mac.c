/*
 * (C) Copyright 2016
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *zhouhuacai <zhouhuacai@allwinnertech.com>
 *
 * SPDX-License-Identifier:›GPL-2.0+
 */
#include <common.h>
#include <fdt_support.h>

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

struct addr_info_t {
	char *envname;
	char *dtsname;
	int   flag;
};

static struct addr_info_t addr[] = {
	{"mac",      "addr_eth",  1},
	{"wifi_mac", "addr_wifi", 1},
	{"bt_mac",   "addr_bt",   0},
};

int update_sunxi_mac(void)
{
	char *p = NULL;
	int   i = 0;
	int   nodeoffset = 0;
	struct fdt_header *dtb_base = working_fdt;

	nodeoffset = fdt_path_offset(dtb_base, "/soc/addr_mgt");

	for (i = 0; i < ARRAY_SIZE(addr); i++) {
		p = env_get(addr[i].envname);
		if (p != NULL) {
			if (addr_parse(p, addr[i].flag)) {
				/*if not pass, clean it, do not pass through cmdline*/
				pr_err("%s format illegal\n", addr[i].envname);
				env_set(addr[i].envname, "");
				continue;
			}

			if (nodeoffset >= 0)
				fdt_setprop_string(dtb_base, nodeoffset, addr[i].dtsname, p);
		}
	}

	return 0;
}
