/*
 * (C) Copyright 2017  <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <securestorage.h>
#include <sunxi_board.h>
#include <version.h>
#include <sys_config.h>
#include <boot_gui.h>
#include <sys_partition.h>
DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SUNXI_USER_KEY
char *IGNORE_ENV_VARIABLE[] = {
	"console",    "root",    "init",	   "loglevel",
	"partitions", "vmalloc", "earlyprintk",    "ion_reserve",
	"enforcing",  "cma",     "initcall_debug", "gpt",
};
#define NAME_SIZE 32
int USER_DATA_NUM;
char USER_DATA_NAME[10][NAME_SIZE] = { { '\0' } };

void check_user_data(void)
{
	char *command_p    = NULL;
	char temp_name[32] = { '\0' };
	int i, j;

	if ((get_boot_storage_type() == STORAGE_SD) ||
	    (get_boot_storage_type() == STORAGE_EMMC)) {
		command_p = env_get("setargs_mmc");
	} else {
		command_p = env_get("setargs_nand");
	}
	//printf("cmd line = %s\n", command_p);
	if (!command_p) {
		printf("cann't get the boot_base from the env\n");
		return;
	}

	while (*command_p != '\0' && *command_p != ' ') { //过滤第一个环境变酿
		command_p++;
	}
	command_p++;
	while (*command_p == ' ') { //过滤多余的空枿
		command_p++;
	}
	while (*command_p != '\0' && *command_p != ' ') { //过滤第二个环境变酿
		command_p++;
	}
	command_p++;
	while (*command_p == ' ') {
		command_p++;
	}

	USER_DATA_NUM = 0;
	while (*command_p != '\0') {
		i = 0;
		while (*command_p != '=') {
			temp_name[i++] = *command_p;
			command_p++;
		}
		temp_name[i] = '\0';
		if (i != 0) {
			for (j = 0; j < sizeof(IGNORE_ENV_VARIABLE) /
						sizeof(IGNORE_ENV_VARIABLE[0]);
			     j++) {
				if (!strcmp(IGNORE_ENV_VARIABLE[j],
					    temp_name)) { //查词典库，排除系统的环境变量，得到用户的数据
					break;
				}
			}
			if (j >= sizeof(IGNORE_ENV_VARIABLE) /
					 sizeof(IGNORE_ENV_VARIABLE[0])) {
				if (!strcmp(temp_name,
					    "mac_addr")) { //处理mac_addr和mac不相等的情况（特殊情况）
					strcpy(USER_DATA_NAME[USER_DATA_NUM],
					       "mac");
				} else {
					strcpy(USER_DATA_NAME[USER_DATA_NUM],
					       temp_name);
				}
				USER_DATA_NUM++;
			}
		}
		while (*command_p != '\0' && *command_p != ' ') { //下一个变酿
			command_p++;
		}
		while (*command_p == ' ') {
			command_p++;
		}
	}
	/*
	printf("USER_DATA_NUM = %d\n", USER_DATA_NUM);
	for (i = 0; i < USER_DATA_NUM; i++) {
		printf("user data = %s\n", USER_DATA_NAME[i]);
	}
*/
}

static int get_key_from_private(char *filename, char *data_buf, int *len)
{
	int ret		    = 0;
	int partno	  = -1;
	char part_info[16]  = { 0 }; /* format: "partno:0" */
	char addr_info[32]  = { 0 }; /* 00000000 */
	char file_info[64]  = { 0 };
	char fetch_name[64] = { 0 };

	char *bmp_argv[6] = { "fatload", "sunxi_flash", part_info,
			      addr_info, file_info,     NULL };

	/* check serial_feature config info */
	strcpy(fetch_name, filename);
	strcat(fetch_name, "_filename");
	ret = script_parser_fetch("/soc/serial_feature", fetch_name,
				  (int *)file_info, sizeof(file_info) / 4);
	if ((ret < 0) || (strlen(file_info) == 0)) {
		strcpy(file_info, "ULI/factory/");
		strcat(file_info, filename);
		strcat(file_info, ".txt");
	}

	/* check private partition info */
	partno = sunxi_partition_get_partno_byname("private");
	if (partno < 0) {
		return -1;
	}

	/* get data from file */
	sprintf(part_info, "0:%x", partno);
	sprintf(addr_info, "%lx", (ulong)data_buf);
	memset(data_buf, 0, *len);
	if (do_fat_fsload(0, 0, 6, bmp_argv)) {
		pr_error("load file(%s) error.\n", bmp_argv[4]);
		return -1;
	}
	data_buf[*len] = 0;

	return 0;
}

int update_user_data(void)
{
	if (get_boot_work_mode() != WORK_MODE_BOOT) {
		return 0;
	}

	check_user_data(); //从env中检测用户的环境变量

	int data_len;
	int ret, k;
	char buffer[512];
	int updata_data_num = 0;
	int found	   = 0;
	int sec_inited      = !sunxi_secure_storage_init();

	for (k = 0; k < USER_DATA_NUM; k++) {
		found = 0;
		memset(buffer, 0, 512);
		if (sec_inited) {
			ret = sunxi_secure_object_read(USER_DATA_NAME[k],
						       buffer, 512, &data_len);
			if (!ret && data_len < 512) {
				found = 1;
			}
		}

		if (!found) {
			data_len = 512;
			ret = get_key_from_private(USER_DATA_NAME[k], buffer,
						   &data_len);
			if (!ret) {
				found = 2;
			}
		}

		if (found) {
			env_set(USER_DATA_NAME[k], buffer);
			pr_msg("update %s = %s, source:%s\n", USER_DATA_NAME[k],
			       buffer, found == 2 ? "private" : "secure");
			memset(buffer, 0, 512);
			updata_data_num++;
			strcpy(USER_DATA_NAME[k], "\0");
		}
	}
	return 0;
}

#endif

void update_bootargs(void)
{
	int dram_clk = 0;
	char *str;
	char cmdline[2048]	   = { 0 };
	char tmpbuf[128]	     = { 0 };
	char *verifiedbootstate_info = env_get("verifiedbootstate");
	str			     = env_get("bootargs");
	__attribute__((unused)) char disp_reserve[80];
	strcpy(cmdline, str);

	if ((!strcmp(env_get("bootcmd"), "run setargs_mmc boot_normal")) ||
	    !strcmp(env_get("bootcmd"), "run setargs_nand boot_normal")) {
		if (gd->chargemode == 0) {
			pr_msg("in boot normal mode,pass normal para to cmdline\n");
			strcat(cmdline, " androidboot.mode=normal");
		} else {
			pr_msg("in charger mode, pass charger para to cmdline\n");
			strcat(cmdline, " androidboot.mode=charger");
		}
	}
#ifdef CONFIG_SUNXI_SERIAL
	//serial info
	str = env_get("snum");
	sprintf(tmpbuf, " androidboot.serialno=%s", str);
	strcat(cmdline, tmpbuf);
#endif
#ifdef CONFIG_SUNXI_MAC
	str = env_get("mac");
	if (str && !strstr(cmdline, " mac_addr=")) {
		sprintf(tmpbuf, " mac_addr=%s", str);
		strcat(cmdline, tmpbuf);
	}

	str = env_get("wifi_mac");
	if (str && !strstr(cmdline, " wifi_mac=")) {
		sprintf(tmpbuf, " wifi_mac=%s", str);
		strcat(cmdline, tmpbuf);
	}

	str = env_get("bt_mac");
	if (str && !strstr(cmdline, " bt_mac=")) {
		sprintf(tmpbuf, " bt_mac=%s", str);
		strcat(cmdline, tmpbuf);
	}
#endif
	//harware info
	sprintf(tmpbuf, " androidboot.hardware=%s", CONFIG_SYS_CONFIG_NAME);
	strcat(cmdline, tmpbuf);
	/*boot type*/
	sprintf(tmpbuf, " boot_type=%d", get_boot_storage_type_ext());
	strcat(cmdline, tmpbuf);
	sprintf(tmpbuf, " androidboot.boot_type=%d", get_boot_storage_type_ext());
	strcat(cmdline, tmpbuf);
	/*Only 64bit kernel "secure_os_exist" exists
	 * 64bit normal cpu no need OP-TEE service
	 *secure_os_exist= 0	---> secure os no exist
	 *secure_os_exist= 1	---> secure os exist
	 * */
	if (sunxi_probe_secure_monitor()) {
		sprintf(tmpbuf, " androidboot.secure_os_exist=%d", sunxi_probe_secure_os());
		strcat(cmdline, tmpbuf);
	}
	/*dram_clk*/
	script_parser_fetch("soc/dram_para", "dram_clk", (int *)&dram_clk, 1);
#ifdef CONFIG_ARCH_SUN8IW15P1
	sprintf(tmpbuf, " dram_clk=%d", dram_clk);
	strcat(cmdline, tmpbuf);
#endif
	/* gpt support */
	sprintf(tmpbuf, " gpt=1");
	strcat(cmdline, tmpbuf);

	if (gd->securemode) {
		/* verified boot state info */
		sprintf(tmpbuf, " androidboot.verifiedbootstate=%s",
			verifiedbootstate_info);
		strcat(cmdline, tmpbuf);
	}

	sprintf(tmpbuf, " uboot_message=%s(%s-%s)", PLAIN_VERSION, U_BOOT_DMI_DATE, U_BOOT_TIME);
	strcat(cmdline, tmpbuf);

#if defined(CONFIG_BOOT_GUI) || defined(CONFIG_SUNXI_SPINOR_BMP) || defined(CONFIG_SUNXI_SPINOR_JPEG)
#ifdef CONFIG_BOOT_GUI
	save_disp_cmdline();
#endif
	if (env_get("disp_reserve")) {
		snprintf(disp_reserve, 80, " disp_reserve=%s",
			env_get("disp_reserve"));
		strcat(cmdline, disp_reserve);
	}
#endif

#ifdef CONFIG_SUNXI_POWER
	str = env_get("bootreason");
	if (str) {
		snprintf(tmpbuf, 128, " bootreason=%s", str);
		strcat(cmdline, tmpbuf);
	}
#endif

	str = env_get("aw-ubi-spinand.ubootblks");
	if (str) {
		snprintf(tmpbuf, 128, " aw-ubi-spinand.ubootblks=%s", str);
		strcat(cmdline, tmpbuf);
	}

	env_set("bootargs", cmdline);
	pr_msg("android.hardware = %s\n", CONFIG_SYS_CONFIG_NAME);

}
