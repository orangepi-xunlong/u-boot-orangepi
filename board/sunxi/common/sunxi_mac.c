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
#include <sys_config_old.h>

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

static int get_macaddr_from_file(char *filename, char *mac)
{
	int  ret = 0;
	int  partno = -1;
	char part_info[16] = {0};  /* format: "partno:0" */
	char addr_info[32] = {0};  /* 00000000 */
	char file_info[64] = {0};
	char fetch_name[64] = {0};

	char *bmp_argv[6] = {"fatload", "sunxi_flash",
			part_info, addr_info, file_info, NULL};

	/* check serial_feature config info */
	strcpy(fetch_name, filename);
	strcat(fetch_name, "_filename");
	ret = script_parser_fetch("serial_feature", fetch_name,
		(int *)file_info, sizeof(file_info) / 4);
	if ((ret < 0) || (strlen(file_info) == 0)) {
		strcpy(file_info, "ULI/factory/");
		strcat(file_info, filename);
		strcat(file_info, ".txt");
		pr_msg("%s is not exist, use default: %s\n",
				fetch_name, file_info);
	}

	/* check private partition info */
	partno = sunxi_partition_get_partno_byname("private");
	if (partno < 0) {
		return -1;
	}

	/* get data from file */
	sprintf(part_info, "%x:0", partno);
	sprintf(addr_info, "%lx", (ulong)mac);
	if (do_fat_fsload(0, 0, 6, bmp_argv)) {
		pr_error("load file(%s) error.\n", bmp_argv[4]);
		return -1;
	}
	mac[18] = 0;

	return 0;
}

int update_sunxi_mac(void)
{
	char addr_str[128] = {0};
	char *p = NULL;
	int i = 0;

	char *envtab[] = {
		"mac",
		"wifi_mac",
		"bt_mac"};

	int checktab[] = {1, 1, 0};

	for (i = 0; i < sizeof(envtab) / sizeof(envtab[0]); i++) {
		p = getenv(envtab[i]);
		if ((p != NULL) && (addr_parse(p, checktab[i]) == 0)) {
			pr_msg("Secure key: %s=%s\n", envtab[i], p);
			continue;
		}

		if ((get_macaddr_from_file(envtab[i], addr_str) == 0) &&
			(addr_parse(addr_str, checktab[i]) == 0)) {
			setenv(envtab[i], addr_str);
			pr_msg("Private key: %s=%s\n", envtab[i], addr_str);
			continue;
		}
	}

	return 0;
}
