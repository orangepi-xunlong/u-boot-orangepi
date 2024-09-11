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
#include <sunxi_verify_boot_info.h>
#include <libxbc.h>
#include <fs.h>
#include <fdt_support.h>
DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SUNXI_USER_KEY
char *IGNORE_ENV_VARIABLE[] = {
	"console",    "root",    "init",	   "loglevel",
	"partitions", "vmalloc", "earlyprintk",    "ion_reserve",
	"enforcing",  "cma",     "initcall_debug", "gpt",
};
#define NAME_SIZE 32
int USER_DATA_NUM;
char USER_DATA_NAME[32][NAME_SIZE] = { { '\0' } };

void check_user_data(void)
{
	char *command_p    = NULL;
	char temp_name[32] = { '\0' };
	char temp_value[32] = { '\0' };
	int i, j, k;

	if ((get_boot_storage_type() == STORAGE_SD) ||
	    (get_boot_storage_type() == STORAGE_EMMC) ||
	    (get_boot_storage_type() == STORAGE_EMMC0)) {
		command_p = env_get("setargs_mmc");
#ifdef CONFIG_SUNXI_UFS
	} else if (get_boot_storage_type() == STORAGE_UFS) {
		command_p = env_get("setargs_ufs");
#endif
	} else {
		command_p = env_get("setargs_nand");
	}
	debug("cmd line = %s\n", command_p);
	if (!command_p) {
		printf("cann't get the boot_base from the env\n");
		return;
	}

	while (*command_p != '\0' && *command_p != ' ') { /*Filter "setenv"*/
		command_p++;
	}
	command_p++;
	while (*command_p == ' ') { /* Filter out extra spaces */
		command_p++;
	}
	while (*command_p != '\0' && *command_p != ' ') { /*Filter "bootargs"*/
		command_p++;
	}
	command_p++;
	while (*command_p == ' ') {
		command_p++;
	}

	USER_DATA_NUM = 0;
	while (*command_p != '\0') {
		i = 0;
		while (*command_p != '=' && *command_p != '\0') {
			if (*command_p == ' ') { /* Illegal, environment variables should be in the format of "key = value" */
				while (*command_p == ' ' && *command_p != '\0') {
					command_p++; /* next cmd */
				}
				temp_name[i] = '\0';
				i = 0;
				debug("user data %s a constant\n", temp_name);
				debug("command_p:%s\n", command_p);
				continue;
			}
			temp_name[i++] = *command_p;
			command_p++;
		}
		temp_name[i] = '\0';

		k = 0;
		while (*command_p != ' ' && *command_p != '\0') {
			temp_value[k++] = *command_p;
			command_p++;
		}

		if (i != 0) {
			for (j = 0; j < sizeof(IGNORE_ENV_VARIABLE) /
						sizeof(IGNORE_ENV_VARIABLE[0]);
			     j++) {
				if (!strcmp(IGNORE_ENV_VARIABLE[j],
					    temp_name)) { /* Ignore data in dictionary */
					break;
				}
			}
			if (j >= sizeof(IGNORE_ENV_VARIABLE) /
					 sizeof(IGNORE_ENV_VARIABLE[0])) {
				if (!strcmp(temp_name, "mac_addr")) { /* In special cases mac and mac are not equal */
					strcpy(USER_DATA_NAME[USER_DATA_NUM], "mac");
					USER_DATA_NUM++;
				} else if (!strcmp(temp_name, "snum")) { /* update snum whether it's empty or not in current env */
					strcpy(USER_DATA_NAME[USER_DATA_NUM], "snum");
					USER_DATA_NUM++;
				} else {
					if (temp_value[1] == '$') {
						/* Remove excess characters: =${xxx} -> xxx */
						memmove(temp_value, &temp_value[3], k - 3);
						temp_value[k - 4] = '\0';
						debug("cmd name:%s\n", temp_name);
						debug("env name:%s - value:%s\n", temp_value, env_get(temp_value));
						if (!env_get(temp_value)) {
							strcpy(USER_DATA_NAME[USER_DATA_NUM], temp_name);
							USER_DATA_NUM++;
						}
					}
				}
			}
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

static int get_key_from_private(int partno, char *filename, char *data_buf, int *len)
{
	char part_info[16]  = { 0 }; /* format: "partno:0" */
	char file_info[64]  = { 0 };
	loff_t len_read;
	int storage_type = get_boot_storage_type();

	if (partno < 0)
		return -1;

	strcpy(file_info, filename);

	/* get data from file */
	sprintf(part_info, "0:%x", partno);
	memset(data_buf, 0, *len);

	if (storage_type == STORAGE_UFS) {
		if (fs_set_blk_dev("sunxi_flash_ufs", part_info, FS_TYPE_FAT))
			return 1;
	} else {
		if (fs_set_blk_dev("sunxi_flash", part_info, FS_TYPE_FAT))
			return 1;
	}
	if (fs_read(filename, (ulong)data_buf, 0, 0, &len_read) < 0)
		return -1;
	data_buf[*len] = 0;

	return 0;
}

/*ret:0:not found, 1:found*/
int sunxi_bootargs_load_key(const char *name, int *data_len, char *buffer,
			    int buffer_size)
{
	static int partno     = -1;
	const char *file_path = "ULI/factory";
	int ret;
	int found      = 0;
	int sec_inited = !sunxi_secure_storage_init();

	char fetch_name[64] = { 0 };
	char fetch_data[64] = { 0 };
	char full_name[64]  = { 0 };

	/* check private partition info */
	if (partno == -2) {
		/*already know private non exist*/
	} else if (partno == -1) {
		partno = sunxi_partition_get_partno_byname("private");
		if (partno >= 0) {
			sprintf(buffer, "fatls sunxi_flash 0:%x %s", partno,
				file_path);
			printf("List file under %s\n", file_path);
			if (run_command(buffer, 0)) {
				partno = -2;
			}
		}
	}

	found = 0;
	memset(buffer, 0, 512);
	if (sec_inited) {
		ret = sunxi_secure_object_read(name, buffer, 512, data_len);
		if (!ret && *data_len < 512) {
			found = 1;
		}
	}

	if (!found) {
		*data_len = 512;

		if (partno >= 0) {
			sprintf(fetch_name, "%s_filename", name);
			ret = script_parser_fetch("/soc/serial_feature",
						  fetch_name, (int *)fetch_data,
						  sizeof(fetch_data) / 4);
			if ((ret < 0) || (strlen(fetch_data) == 0))
				sprintf(full_name, "%s/%s.txt", file_path,
					name);
			else
				sprintf(full_name, "%s", fetch_data);

			ret = get_key_from_private(partno, full_name, buffer,
						   data_len);
			if (!ret)
				found = 2;
		}
	}

	if (found) {
		env_set(name, buffer);
		pr_msg("update %s = %s, source:%s\n", name, buffer,
		       found == 2 ? "private" : "secure");
		memset(buffer, 0, 512);
	}
	return found;
}

int update_user_data(void)
{
	if (get_boot_work_mode() != WORK_MODE_BOOT) {
		return 0;
	}
	return 0;

	check_user_data(); //从env中检测用户的环境变量

	int data_len;
	int k;
	char buffer[512];
	int updata_data_num = 0;

	for (k = 0; k < USER_DATA_NUM; k++) {
		if (sunxi_bootargs_load_key(USER_DATA_NAME[k], &data_len,
					    buffer, sizeof(buffer))) {
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
	char tmpbuf[256]	     = { 0 };
	char *verifiedbootstate_info = env_get("verifiedbootstate");
	str			     = env_get("bootargs");
	__attribute__((unused)) char disp_reserve[80];
	strncpy(cmdline, str, sizeof(cmdline) - 1);

	if ((!strcmp(env_get("bootcmd"), "run setargs_mmc boot_normal")) ||
#ifdef CONFIG_SUNXI_UFS
		(!strcmp(env_get("bootcmd"), "run setargs_ufs boot_normal")) ||
#endif
		!strcmp(env_get("bootcmd"), "run setargs_nand boot_normal")) {
		if (gd->chargemode == 0) {
			pr_msg("in boot normal mode,pass normal para to cmdline\n");
			strncat(cmdline, " androidboot.mode=normal",
				sizeof(cmdline) - strlen(cmdline) - 1);
		} else {
			pr_msg("in charger mode, pass charger para to cmdline\n");
			strncat(cmdline, " androidboot.mode=charger",
				sizeof(cmdline) - strlen(cmdline) - 1);
		}
	}
#ifdef CONFIG_SUNXI_SERIAL
	//serial info
	str = env_get("snum");
	sprintf(tmpbuf, " androidboot.serialno=%s", str);
	strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
#endif
#ifdef CONFIG_SUNXI_MAC
	str = env_get("mac");
	if (str && !strstr(cmdline, " mac_addr=")) {
		sprintf(tmpbuf, " mac_addr=%s", str);
		strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	}

	str = env_get("wifi_mac");
	if (str && !strstr(cmdline, " wifi_mac=")) {
		sprintf(tmpbuf, " wifi_mac=%s", str);
		strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	}

	str = env_get("bt_mac");
	if (str && !strstr(cmdline, " bt_mac=")) {
		sprintf(tmpbuf, " bt_mac=%s", str);
		strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	}
#endif
	//harware info
	sprintf(tmpbuf, " androidboot.hardware=%s", CONFIG_SYS_CONFIG_NAME);
	strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	/*boot type*/
	sprintf(tmpbuf, " boot_type=%d", get_boot_storage_type_ext());
	strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	sprintf(tmpbuf, " androidboot.boot_type=%d", get_boot_storage_type_ext());
	strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	/*Only 64bit kernel "secure_os_exist" exists
	 * 64bit normal cpu no need OP-TEE service
	 *secure_os_exist= 0	---> secure os no exist
	 *secure_os_exist= 1	---> secure os exist
	 * */
	if (sunxi_probe_secure_monitor()) {
		sprintf(tmpbuf, " androidboot.secure_os_exist=%d", sunxi_probe_secure_os());
		strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	}

#ifdef CONFIG_SUNXI_ANDROID_BOOT
	str = env_get("android_trust_chain");
	if (str) {
		sprintf(tmpbuf, " androidboot.trustchain=%s", str);
		strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	}else{
		sprintf(tmpbuf, " androidboot.trustchain=false");
		strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	}

	str = env_get("android_drmkey");
	if (str) {
		sprintf(tmpbuf, " androidboot.drmkey=%s", str);
		strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	} else {
		sprintf(tmpbuf, " androidboot.drmkey=false");
		strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	}
#endif

	/*dram_clk*/
	script_parser_fetch("soc/dram_para", "dram_clk", (int *)&dram_clk, 1);
#ifdef CONFIG_ARCH_SUN8IW15P1
	sprintf(tmpbuf, " dram_clk=%d", dram_clk);
	strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
#endif
	/* gpt support */
	sprintf(tmpbuf, " gpt=1");
	strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);

	if (gd->securemode) {
		/* verified boot state info */
		sprintf(tmpbuf, " androidboot.verifiedbootstate=%s",
			verifiedbootstate_info);
		strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	}

	sprintf(tmpbuf, " uboot_message=%s(%s-%s)", PLAIN_VERSION, U_BOOT_DMI_DATE, U_BOOT_TIME);
	strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
#ifdef CONFIG_SPINOR_LOGICAL_OFFSET
#if defined (CONFIG_ENABLE_MTD_CMDLINE_PARTS_BY_ENV)
	char *mtdpart = env_get("sunxi_mtdparts");
	char *tmpaddr;
	char *desbuff =NULL;
	char part_buff[218] = {0};
	char device_name[16] = {0};

	if(mtdpart != NULL) {
		tmpaddr = strchr(mtdpart,':');
		strncpy(device_name, mtdpart, (tmpaddr + 1 - mtdpart));
		strcpy(part_buff, (tmpaddr+1));
		desbuff = tmpbuf;
		sprintf(desbuff, "%s mtdparts=%s%dK(uboot),%s", tmpbuf, device_name,
				(sunxi_flashmap_logical_offset(FLASHMAP_SPI_NOR, LINUX_LOGIC_OFFSET) * 512 / 1024), part_buff);

		strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	}
#else
	/*spi-nor logical offset */
	sprintf(tmpbuf, " mbr_offset=%d", (sunxi_flashmap_logical_offset(FLASHMAP_SPI_NOR, LINUX_LOGIC_OFFSET)* 512));
	strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
#endif
#endif /*CONFIG_SPINOR_LOGICAL_OFFSET*/

#if defined(CONFIG_BOOT_GUI) || defined(CONFIG_SUNXI_SPINOR_BMP) || defined(CONFIG_SUNXI_SPINOR_JPEG)
#ifdef CONFIG_BOOT_GUI
	save_disp_cmdline();
#endif
	if (env_get("disp_reserve")) {
		snprintf(disp_reserve, 80, " disp_reserve=%s",
			env_get("disp_reserve"));
		strncat(cmdline, disp_reserve,
			sizeof(cmdline) - strlen(cmdline) - 1);
	}
#endif

#ifdef CONFIG_SUNXI_POWER
	str = env_get("bootreason");
	if (str) {
		snprintf(tmpbuf, 128, " bootreason=%s", str);
		strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	}
#endif

#ifdef CONFIG_SUNXI_DM_VERITY
	char dm_mod[256] = {0};
	char dm_dev[128] = {0};
	str = env_get("dm_mod");
	if (str) {
		snprintf(dm_mod, 256, " dm-mod.create=%s", str);
		strncat(cmdline, dm_mod, sizeof(cmdline) - strlen(cmdline) - 1);
	}
	str = env_get("dm_dev");
	if (str) {
		snprintf(dm_dev, 128, " dm-mod.waitfor=%s", str);
		strncat(cmdline, dm_dev, sizeof(cmdline) - strlen(cmdline) - 1);
	}

#endif

	str = env_get("aw-ubi-spinand.ubootblks");
	if (str) {
		snprintf(tmpbuf, 128, " aw-ubi-spinand.ubootblks=%s", str);
		strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	}

	str = env_get("trace_enable");
	if (str && strcmp(str, "1") == 0) {
		str = env_get("trace_buf_size");
		if (str) {
			snprintf(tmpbuf, 128, " trace_buf_size=%s", str);
			strncat(cmdline, tmpbuf,
				sizeof(cmdline) - strlen(cmdline) - 1);
		}
		str = env_get("trace_event");
		if (str) {
			snprintf(tmpbuf, 128, " trace_event=%s", str);
			strncat(cmdline, tmpbuf,
				sizeof(cmdline) - strlen(cmdline) - 1);
		}
	}
	uint32_t *dram_para = NULL;
	dram_para	   = (uint32_t *)uboot_spare_head.boot_data.dram_para;

	sprintf(tmpbuf, " androidboot.dramfreq=%d",
		(unsigned int)(dram_para[0]));
	strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	sprintf(tmpbuf, " androidboot.dramsize=%d",
		(unsigned int)(uboot_spare_head.boot_data.dram_scan_size));

	strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
#ifdef CONFIG_SUNXI_UBIFS
	str = env_get("mtdparts");
	if (str) {
		str = strstr(str, "nand");
		snprintf(tmpbuf, 256, " mtdparts=%s", str);
		strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	}
#endif
	str = env_get("init_rc");
	if (str) {
		snprintf(tmpbuf, 128, " androidboot.init_rc=%s", str);
		strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);
	}

	uint8_t uboot_backup;
	uboot_backup = uboot_spare_head.boot_data.uboot_backup;
	if (!uboot_backup) {
		sprintf(tmpbuf, " uboot_backup=ubootA");
	} else {
		sprintf(tmpbuf, " uboot_backup=ubootB");
	}
	strncat(cmdline, tmpbuf, sizeof(cmdline) - strlen(cmdline) - 1);

	env_set("bootargs", cmdline);
	pr_msg("android.hardware = %s\n", CONFIG_SYS_CONFIG_NAME);
#ifdef CONFIG_SUNXI_VERIFY_BOOT_INFO_INSTALL
	sunxi_keymaster_verify_boot_params_install();
#endif

}

void update_vendorbootconfig_and_bootgargs(void)
{

	ulong bootconfig_startaddr = env_get_hex("bootconfig_addr", 0x00);
	ulong bootconfig_size = env_get_hex("bootconfig_size", 0x00);
	ulong ramdisk_addr  = env_get_hex("ramdisk_start", 0x00);
	ulong ramdisk_size = env_get_hex("ramdisk_size", 0x00);
	ulong bootconfig_addr = ramdisk_addr + ramdisk_size - bootconfig_size;
	int ret = 0;

	if (bootconfig_startaddr) {
		if (ramdisk_addr) {
			ret = android_image_cmdline_to_vendorbootconfig(
			    &bootconfig_addr, &bootconfig_size);

			if (ret) {
				ramdisk_size = ramdisk_size + ret;
				env_set_hex("ramdisk_size", ramdisk_size);
				fdt_initrd(
				    working_fdt, (ulong)ramdisk_addr,
				    (ulong)(ramdisk_addr + ramdisk_size));
				env_set_hex("ramdisk_sum",
					    sunxi_generate_checksum(
						(void *)ramdisk_addr,
						ramdisk_size, 8, STAMP_VALUE));
			}
		} else {
			pr_err("get ramdisk addr Failed\n");
		}
	} else {
		pr_msg("no bootconfig addr Failed\n");
	}
}
int android_image_cmdline_to_vendorbootconfig(
	unsigned long *bootconfig_startaddr, unsigned long *bootconfig_size)
{
	char *str = env_get("bootargs");
	struct key_value_set getkey[128];
	ulong addlen	= 0;
	u32 count	   = 0;
	char temp[128]      = { 0 };
	char cmdline[2048]  = { 0 };
	u32 i		    = 0, len;

	if (*bootconfig_size == 0) {
		pr_err("err: bootconfig_size=0\n");
		return -1;
	}
	pr_msg("bootargs=%s\n", str);
	memset(getkey, 0x00, ARRAY_SIZE(getkey) * sizeof(struct key_value_set));
	count = sunxi_cmdline_parse(str, getkey, ARRAY_SIZE(getkey));
	pr_msg("count:%d", count);
	for (i = 0; i < count + 1; i++) {
		memset(temp, 0x00, 128);
		strncat(temp, getkey[i].key, getkey[i].key_len);
		strncat(temp, "=", 1);
		strncat(temp, getkey[i].value, getkey[i].value_len);

		if (strncmp(temp, "androidboot.", strlen("androidboot."))) {
			strcat(temp, " ");
			strncat(cmdline, temp,
				sizeof(cmdline) - strlen(cmdline) - 1);

		} else {
			len       = getkey[i].key_len + getkey[i].value_len + 1;
			temp[len] = 0x0a;
			len       = len + 1;
			int ret   = addBootConfigParameters(temp, len,
							  *bootconfig_startaddr,
							  *bootconfig_size);
			if (ret <= 0) {
				pr_err("Failed to apply boot config params:%s\n",
				       temp);
				ret = 0;
			} else {
				*bootconfig_size += ret;
				addlen += ret;
				pr_msg("line:%d addr:0x%lx size:0x%lx=\n",
				       __LINE__, *bootconfig_startaddr,
				       *bootconfig_size);
			}
		}
	}
	env_set("bootargs", cmdline);
	return addlen;
}
