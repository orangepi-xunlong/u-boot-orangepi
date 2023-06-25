/* SPDX-License-Identifier: GPL-2.0+
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 * Some init for sunxi platform.
 */

#include <common.h>
#include <asm/io.h>
#include <sunxi_board.h>
#include <sunxi_flash.h>
#include <fdt_support.h>
#include <blk.h>
#include <part.h>
#include <asm/arch/rtc.h>
#include <asm/arch/dram.h>
#include <private_uboot.h>
#include <asm/arch/efuse.h>
#include <sunxi_nand.h>
#include <android_misc.h>
#include <android_ab.h>
#include <sys_partition.h>
#include <sys_config.h>
#include <sunxi-ir.h>
#include <smc.h>
#include <mmc.h>
#include <linux/mtd/aw-spinand.h>
#include <boot_gui.h>
#include "../../drivers/spi/spi-sunxi.h"

DECLARE_GLOBAL_DATA_PTR;

/*define ir or key mode value*/
#define EFEX_VALUE                                      (0x81)
#define SPRITE_RECOVERY_VALUE                           (0X82)
#define BOOT_RECOVERY_VALUE                             (0x83)
#define BOOT_FACTORY_VALUE                              (0x84)


#define PARTITION_SETS_MAX_SIZE 1024
#define PARTITION_NAME_MAX_SIZE 16
#define ROOT_PART_NAME_MAX_SIZE (PARTITION_NAME_MAX_SIZE + 5)
#define DEV_PART_NAME_MAX_SIZE (PARTITION_NAME_MAX_SIZE + sizeof("/dev/"))

int disp_update_lcd_param(int lcd_param_index);

int update_fdt_dram_para(void *dtb_base)
{
	/*fix dram para*/
	int i, nodeoffset = 0;
	char dram_str[16] = {0};
	uint32_t *dram_para = NULL;
	dram_para = (uint32_t *)uboot_spare_head.boot_data.dram_para;

	pr_msg("(weak)update dtb dram start\n");
	gd->bd->bi_dram[0].size = (phys_size_t)uboot_spare_head.boot_data.dram_scan_size * 1024 * 1024;
	if (arch_fixup_fdt(dtb_base) < 0) {
		printf("ERROR: arch-specific fdt fixup failed\n");
		return -1;
	}

	nodeoffset = fdt_path_offset(dtb_base, "/dram");
	if (nodeoffset < 0) {
		pr_err("## error: %s : %s\n", __func__, fdt_strerror(nodeoffset));
		return -1;
	}
	for (i = 31; i >= 0; i--) {
		sprintf(dram_str, "dram_para[%02d]", i);
		fdt_setprop_u32(dtb_base, nodeoffset, dram_str, dram_para[i]);
	}
	pr_msg("update dtb dram  end\n");
	return 0;
}

static int fdt_enable_node(char *name, int onoff)
{
	int nodeoffset = 0;
	int ret = 0;

	nodeoffset = fdt_path_offset(working_fdt, name);
	ret = fdt_set_node_status(working_fdt, nodeoffset,
		onoff ? FDT_STATUS_OKAY: FDT_STATUS_DISABLED, 0);

	if (ret < 0) {
		printf("disable nand error: %s\n", fdt_strerror(ret));
	}
	return ret;
}

void sunxi_dump(void *addr, unsigned int size)
{
#if 1
	print_buffer((ulong)addr, addr, 4, ALIGN(size, 16)/4, 4);
#else
	int i, j;
	char *buf = (char *)addr;

	for (j = 0; j < size; j += 16) {
		for (i = 0; (i < 16 && i < size); i++) {
			printf("%02x ", buf[j + i] & 0xff);
		}
		printf("\n");
	}
	printf("\n");
#endif
	return;

}

/*
* name    :  sunxi_str_replace
* fucntion:  replace a word in string  which separated by a space
* note      :  sunxi_str_replace("abc def gh", "def", "replace")    get    "abc replace gh"
*/
static int sunxi_str_replace(char *dest_buf, char *goal, char *replace)
{
	char tmp[128];
	char tmp_str[16];
	int  goal_len, rep_len, dest_len;
	int  i, j, k;

	if( (goal == NULL) || (dest_buf == NULL))
		return -1;

	memset(tmp, 0, 128);
	strcpy(tmp, dest_buf);

	goal_len = strlen(goal);
	dest_len = strlen(dest_buf);

	if(replace != NULL)
		rep_len = strlen(replace);
	else
		rep_len = 0;

	j = 0;
	for(i=0;tmp[i];) {
		k = 0;
		while(((tmp[i] != ' ') && (tmp[i] != 0) )|| (tmp[i+1] == ' '))
		{
			tmp_str[k++] = tmp[i];
			i ++;
			if(i >= dest_len)
				break;
		}
		i ++;
		tmp_str[k] = 0;
		if(!strcmp(tmp_str, goal))
		{
			if(rep_len)
			{
				strcpy(dest_buf + j, replace);
				if(tmp[j + goal_len])
				{
					memcpy(dest_buf + j + rep_len, tmp + j + goal_len, dest_len - j - goal_len);
					dest_buf[dest_len - goal_len + rep_len] = 0;
				}
			}
			else
			{
				if(tmp[j + goal_len])
				{
					memcpy(dest_buf + j, tmp + j + goal_len, dest_len - j - goal_len);
					dest_buf[dest_len - goal_len + rep_len] = 0;
				}
			}

			return 0;
		}
		j = i;
	}

	return 0;
}

int sunxi_str_replace_all(char *dest_buf, char *goal, char *replace)
{
	char tmp[128];
	char tmp_str[16];
	int  goal_len, rep_len, dest_len;
	int  i, j = 0, k;
	char first_flag = 0;
	if ((goal == NULL) || (dest_buf == NULL)) {
		return -1;
	}

	memset(tmp, 0, 128);
	strcpy(tmp, dest_buf);

	goal_len = strlen(goal);
	dest_len = strlen(dest_buf);

	if (replace != NULL) {
		rep_len = strlen(replace);
	} else {
		rep_len = 0;
	}
	for (i = 0 ; tmp[i] ;) {
		k = 0;
		while (((tmp[i] != ' ') && (tmp[i] != ';') && (tmp[i] != 0)) || (tmp[i+1] == ' ')) {
		tmp_str[k++] = tmp[i];
		i++;
		if (i >= dest_len)
			break;
	}
	i++;
	tmp_str[k] = 0;
	if (!strcmp(tmp_str, goal)) {
		if (rep_len != 0 && first_flag == 1)
			j += (rep_len - goal_len);
		if (rep_len) {
			strcpy(dest_buf + j, replace);
			if (tmp[j + goal_len]) {
				memcpy(dest_buf + j + rep_len, tmp + j + goal_len, dest_len - j - goal_len);
				dest_buf[dest_len - goal_len + rep_len] = 0;
			}
		} else {
			if (tmp[j + goal_len]) {
				memcpy(dest_buf + j, tmp + j + goal_len, dest_len - j - goal_len);
				dest_buf[dest_len - goal_len + rep_len] = 0;
			}
		}
			first_flag = 1;
		}
		j = i;
	}
	return 0;
}


static int write_recovery_msg_to_misc(char *recovery_msg)
{
	u32 misc_offset = 0;
	char misc_args[2048];
	static struct bootloader_message *misc_message;
	int ret;

	memset(misc_args, 0x0, 2048);
	misc_message = (struct bootloader_message *)misc_args;

	misc_offset = sunxi_partition_get_offset_byname("misc");
	if (!misc_offset) {
		printf("no misc partition\n");
		return 0;
	}
	ret = sunxi_flash_read(misc_offset, 2048 / 512, misc_args);
	if (!ret) {
		printf("error: read misc partition\n");
		return 0;
	}

	if (!strcmp("ir-or-key-recovery", (const char *)recovery_msg)) {
		strcpy(misc_message->command, "boot-recovery");
	} else if (!strcmp("ir-factory", (const char *)recovery_msg)) {
		strcpy(misc_message->command, "boot-recovery");
		strcpy(misc_message->recovery,
		       "recovery\n--wipe_data\n--locale=zh_CN\n");
	} else if (!strcmp("ir-efex", (const char *)recovery_msg)) {
		strcpy(misc_message->command, "efex");
	} else if (!strcmp("sysrecovery", (const char *)recovery_msg)) {
		strcpy(misc_message->command, "sysrecovery");
		/*    strcpy(misc_message->recovery, "sysrecovery"); */
	} else if (!strcmp("erase_misc", (const char *)recovery_msg)) {
		memset(misc_args, 0x0, 2048);
	}
	sunxi_flash_write(misc_offset, 2048 / 512, misc_args);
	return 0;
}

#ifdef CONFIG_RECOVERY_KEY

#define RECOVERY_KEY_EFEX_MODE				(0x0)
#define RECOVERY_KEY_ONEKEY_SPRITE_RECOVERY_MODE	(0x1)
#define RECOVERY_KEY_RECOVERY_MODE			(0x2)
#define RECOVERY_KEY_FACTORY_MODE			(0x3)

#define RECOVERY_KEY_EFEX_VALUE				(0x81)
#define RECOVERY_KEY_SPRITE_RECOVERY_VALUE		(0X82)
#define RECOVERY_KEY_BOOT_RECOVERY_VALUE		(0x83)
#define RECOVERY_KEY_BOOT_FACTORY_VALUE			(0x84)

#define KEY_PRESS_MODE_DISABLE	(0X0)
#define KEY_PRESS_MODE_ENABLE	(0x1)
#define KEY_BOOT_RECOVERY		"/soc/key_boot_recovery"

#ifdef RECOVERY_KEY_DEBUG
#define key_info(fmt...) tick_printf(fmt)
#else
#define key_info(fmt...)
#endif

int check_recovery_key(void)
{
	user_gpio_set_t	gpio_recovery;
	__u32 gpio_hd;
	int ret;
	int gpio_value = 0;
	int used = 0;
	int mode = 0;
	int press_mode = 0;
	int press_time = 0;

	if (get_boot_work_mode() == WORK_MODE_BOOT) {
		/*check physical gpio pin*/
		ret = script_parser_fetch(KEY_BOOT_RECOVERY, "recovery_key_used",
				(int *)&used, sizeof(int) / 4);
		if (ret || !used) {
			key_info("[key recovery] no use\n");
			return 0;
		}
		ret = fdt_get_one_gpio(KEY_BOOT_RECOVERY, "recovery_key", &gpio_recovery);
		if (ret) {
			key_info("[key recovery] can't find recovery_key config.\n");
			return 0;
		}
		gpio_recovery.mul_sel = 0;		/*forced to input*/
		gpio_hd = sunxi_gpio_request(&gpio_recovery, 1);
		if (!gpio_hd) {
			pr_error("[key recovery] gpio request fail!\n");
			return 0;
			/*gpio request fail,just return.*/
		}

		int time;
		gpio_value = 0;
		for (time = 0; time < 5; time++) {
			gpio_value += gpio_read_one_pin_value(gpio_hd, 0);
			mdelay(1);
		}

		if (gpio_value) {
			key_info("[key recovery] no key press.\n");
			return 0;
			/*no recovery key was pressed, just return.*/
		}

		key_info("[key recovery] find the key\n");
		script_parser_fetch(KEY_BOOT_RECOVERY, "press_mode_enable",
			(int *)&press_mode, sizeof(int) / 4);
		if (press_mode == KEY_PRESS_MODE_DISABLE) {
			script_parser_fetch(KEY_BOOT_RECOVERY, "key_work_mode",
				(int *)&mode, sizeof(int) / 4);
			key_info("[key recovery] do not use prese mode, \
					key_work_mode = %d\n", mode);
		} else if (press_mode == KEY_PRESS_MODE_ENABLE) {
			script_parser_fetch(KEY_BOOT_RECOVERY, "key_press_time",
				(int *)&press_time, sizeof(int) / 4);
			key_info("[key recovery] use prese mode, \
				and key_press_time = %d ms\n", press_time);
			gpio_value = 0;
			for (time = press_time; time > 0; time -= 100) {
				mdelay(100); /*detect key value per 100ms*/
				gpio_value += gpio_read_one_pin_value(gpio_hd, 0);

				/*key was loosed during press time,will not detect key value any more*/
				if (gpio_value)
					break;
			}
			/*key was lossed during press time, so use short_press_mode*/
			if (gpio_value) {
				script_parser_fetch(KEY_BOOT_RECOVERY, "short_press_mode",
					(int *)&mode, sizeof(int) / 4);
				key_info("[key recovery] key short press, short_press_mode = %d\n", mode);
			} else {
			/*key was always pressed in key_press_time, so use long_press_mode*/
				script_parser_fetch(KEY_BOOT_RECOVERY, "long_press_mode",
					(int *)&mode, sizeof(int) / 4);
				key_info("[key recovery] key long press, long_press_mode = %d\n", mode);
			}
		}

		switch (mode) {
		case RECOVERY_KEY_EFEX_MODE:
			gd->key_pressd_value = RECOVERY_KEY_EFEX_VALUE;
			break;
		case RECOVERY_KEY_ONEKEY_SPRITE_RECOVERY_MODE:
			gd->key_pressd_value = RECOVERY_KEY_SPRITE_RECOVERY_VALUE;
			break;
		case RECOVERY_KEY_RECOVERY_MODE:
			gd->key_pressd_value = RECOVERY_KEY_BOOT_RECOVERY_VALUE;
			break;
		case RECOVERY_KEY_FACTORY_MODE:
			gd->key_pressd_value = RECOVERY_KEY_BOOT_FACTORY_VALUE;
			break;
		default:
			gd->key_pressd_value = RECOVERY_KEY_BOOT_RECOVERY_VALUE;
			break;
		}

	}

	return 0;
}
#endif

void sunxi_respond_ir_key_action(void)
{
	int key_value;
#ifdef CONFIG_IR_BOOT_RECOVERY
	int ir_time_out = get_timer_masked();
	while (get_timer_masked() - ir_time_out < 200) {
		if (gd->ir_detect_status != IR_DETECT_NULL)
			break;
	}
	ir_disable();
#endif
	key_value = gd->key_pressd_value;
	/* //ir factory or key factory */
	if (key_value == 0) {
		return;
	} else if (key_value == BOOT_FACTORY_VALUE) {
		printf("[boot factory] set misc : boot factory and wipe data\n");
		write_recovery_msg_to_misc("ir-factory");
	} else if (key_value == EFEX_VALUE) {
		/* //ir efex or key efex */
		printf("[boot efex] set misc : boot efex\n");
		write_recovery_msg_to_misc("ir-efex");
	} else if (key_value == SPRITE_RECOVERY_VALUE) {
		/* //ir sprite recovery */
		printf("[ir sysrecovery] set misc : boot recovery and sysrecovery \n");
		write_recovery_msg_to_misc("sysrecovery");
	} else if (key_value == BOOT_RECOVERY_VALUE) {
		/* //ir recovery or key recovery */
		printf("[boot recovery] set misc : boot recovery\n");
		write_recovery_msg_to_misc("ir-or-key-recovery");
	}
}

#ifndef CONFIG_BOOTCMD_SKIP_RTC
static int sunxi_get_bootcmd_from_rtc(void)
{
	u8 bootmode_flag = 0;

	bootmode_flag = rtc_get_bootmode_flag();
	/*clear rtc*/
	rtc_set_bootmode_flag(0);
	switch (bootmode_flag) {
	case SUNXI_EFEX_CMD_FLAG:
		return SUNXI_EFEX_CMD_FLAG;
	case SUNXI_BOOT_RECOVERY_FLAG:
		return SUNXI_BOOT_RECOVERY_FLAG;
	case SUNXI_FASTBOOT_FLAG:
		return SUNXI_FASTBOOT_FLAG;
	case SUNXI_UBOOT_FLAG:
		return SUNXI_UBOOT_FLAG;
	default:
		return 0;
		break;
	}
}
#endif

#ifndef CONFIG_BOOTCMD_SKIP_ADCKEY
static int sunxi_get_bootcmd_from_adckey(void)
{
	typedef struct adc_key_info {
		char *key_type;
		u32 key_flag;
	} _adc_key_info;
	int key_high, key_low;
	int keyvalue, i;
	char key_mode[16] = {0};
	_adc_key_info key_info_table[] = {
		{"recovery", SUNXI_BOOT_RECOVERY_FLAG},
		{"fastboot", SUNXI_FASTBOOT_FLAG},
		{"fel", SUNXI_EFEX_CMD_FLAG},
	};

	keyvalue = uboot_spare_head.boot_data.key_input;
	pr_msg("key %d\n", keyvalue);
	if (keyvalue) {
		for (i = 0; i < sizeof(key_info_table)/sizeof(key_info_table[0]); i++) {
			sprintf(key_mode, "/soc/%s_key", key_info_table[i].key_type);
			script_parser_fetch(key_mode, "key_max", &key_high, 0);
			script_parser_fetch(key_mode, "key_min", &key_low, 0);
			if (key_high || key_low) {
				pr_notice("%s key high %d, low %d\n", key_info_table[i].key_type, key_high, key_low);
				if ((keyvalue >= key_low) && (keyvalue <= key_high)) {
					pr_notice("key found, android %s\n", key_info_table[i].key_type);
					return key_info_table[i].key_flag;
				}
			}
		}
	}
	return 0;
}
#endif

#ifndef CONFIG_BOOTCMD_SKIP_MISC
static int sunxi_get_bootcmd_from_misc(void)
{
	u32 misc_offset = 0;
	char misc_args[2048];
	char misc_fill[2048];
	struct bootloader_message *misc_message;
	int misc_mode;

	misc_message = (struct bootloader_message *)misc_args;
	memset(misc_args, 0x0, 2048);
	memset(misc_fill, 0xff, 2048);

	misc_offset = sunxi_partition_get_offset_byname("misc");
	if (!misc_offset) {
		pr_msg("no misc partition is found\n");
		return 0;
	} else {
		pr_msg("misc partition found\n");
		sunxi_flash_read(misc_offset, 2048 / 512, misc_args);
	}


	if (strstr((const char *)misc_message->command, "efex")) {
		misc_mode = SUNXI_EFEX_CMD_FLAG;
		/*"sysrecovery" must judge before "recovery" !!!*/
	} else if (strstr((const char *)misc_message->command, "sysrecovery")) {
		misc_mode = SUNXI_SYS_RECOVERY_FLAG;
	} else if (strstr((const char *)misc_message->command, "recovery")) {
		misc_mode = SUNXI_BOOT_RECOVERY_FLAG;
	} else if (strstr((const char *)misc_message->command, "bootloader")) {
		misc_mode = SUNXI_FASTBOOT_FLAG;
	} else if (strstr((const char *)misc_message->command, "uboot")) {
		misc_mode = SUNXI_UBOOT_FLAG;
	} else {
		misc_mode = 0;
	}
	if ((misc_mode) || *(misc_message->recovery))
		print_buffer((ulong)misc_args, misc_args, 4, 0x50, 4);

	if (strstr((const char *)misc_message->recovery, "update_package") ==
	    NULL) {
		memset(misc_message->command, 0x0,
		       sizeof(misc_message->command));
		sunxi_flash_write(misc_offset, 2048 / 512, misc_args);
		sunxi_flash_flush();
	}
	return misc_mode;
}
#endif

int sunxi_set_bootcmd(char *bootcmd)
{
	int i, bootmode[4] = {0};
	env_set_hex("force_normal_boot", 1);
#ifndef CONFIG_BOOTCMD_SKIP_ADCKEY
	bootmode[1] = sunxi_get_bootcmd_from_adckey();
#endif
#ifndef CONFIG_BOOTCMD_SKIP_RTC
	bootmode[2] = sunxi_get_bootcmd_from_rtc();
#endif
#ifndef CONFIG_BOOTCMD_SKIP_MISC
	bootmode[3] = sunxi_get_bootcmd_from_misc();
#endif
	for (i = 1; i < sizeof(bootmode)/sizeof(bootmode[0]); i++) {
		if (bootmode[i]) {
			bootmode[0] = bootmode[i];
			tick_printf("bootmode[%d]:0x%x\n", i, bootmode[i]);
			break;
		}
	}
	switch (bootmode[0]) {
	case SUNXI_EFEX_CMD_FLAG:
		sunxi_board_run_fel();
		break;
	case SUNXI_SYS_RECOVERY_FLAG:
		set_boot_work_mode(WORK_MODE_SPRITE_RECOVERY);
		env_set("sysrecovery", "sprite_test");
		strncpy(bootcmd, "run sysrecovery",
			sizeof("run sysrecovery"));
		break;
	case SUNXI_BOOT_RECOVERY_FLAG:
		if (sunxi_partition_get_offset_byname("recovery")) {
			sunxi_str_replace(bootcmd, "boot_normal", "boot_recovery");
		}
		env_set_hex("force_normal_boot", 0);
		break;
	case SUNXI_FASTBOOT_FLAG:
		sunxi_str_replace(bootcmd, "boot_normal", "boot_fastboot");
		break;
	case SUNXI_UBOOT_FLAG:
		sunxi_set_uboot_shell(1);
		break;
	default:
		break;
	}

	return 0;
}

int sunxi_update_bootcmd(void)
{
	char  boot_commond[128];
	int   storage_type = get_boot_storage_type();
	memset(boot_commond, 0x0, 128);
	strncpy(boot_commond, env_get("bootcmd"), sizeof(boot_commond)-1);
	debug("base bootcmd=%s\n", boot_commond);

	if ((storage_type == STORAGE_SD) || (storage_type == STORAGE_EMMC) ||
	    (storage_type == STORAGE_EMMC3) || (storage_type == STORAGE_EMMC0)) {
		sunxi_str_replace(boot_commond, "setargs_nand", "setargs_mmc");
		debug("bootcmd set setargs_mmc\n");
	} else if (storage_type == STORAGE_NOR) {
		sunxi_str_replace(boot_commond, "setargs_nand", "setargs_nor");
	} else if (storage_type == STORAGE_NAND) {
#ifdef CONFIG_SUNXI_UBIFS
#ifndef CONFIG_MACH_SUN8IW18
		if (nand_use_ubi()) {
			sunxi_str_replace(boot_commond, "setargs_nand", "setargs_nand_ubi");
			debug("bootcmd set setargs_nand_ubi\n");
		}
#endif
#else
		debug("bootcmd set setargs_nand\n");
#endif
	}


	sunxi_set_bootcmd(boot_commond);

	env_set("bootcmd", boot_commond);
#ifdef CONFIG_ANDROID_AB
	/*determine whether it is ab-system according to the boot_a partition*/
	if (sunxi_partition_get_offset_byname("boot_a")) {
		if (ab_select_slot_from_partname("misc") == 1) {
			env_set("slot_suffix", "_b");
		} else {
			env_set("slot_suffix", "_a");
		}
	}
#endif
	debug("to be run cmd=%s\n", boot_commond);
	tick_printf("update bootcmd\n");
	return 0;
}

struct os_memory_info_t {
	uint64_t shm_base;
	uint32_t shm_size;
	uint64_t ta_ram_base;
	uint32_t ta_ram_size;
	uint64_t tee_ram_base;
	uint32_t tee_ram_size;
} static os_memory_info;
int secure_os_memory_init(void)
{
	//some platforms uboot and kernel use different fdt
	//so dont add reserve memory until we are updating
	//kernel fdt, save the values for now
#if defined(CONFIG_SUNXI_EXTERN_SECURE_MM_LAYOUT)
	script_parser_fetch("/firmware/optee", "shm_base",
			    (int *)&os_memory_info.shm_base, 0);
	script_parser_fetch("/firmware/optee", "shm_size",
			    (int *)&os_memory_info.shm_size, 0);

	script_parser_fetch("/firmware/optee", "ta_ram_base",
			    (int *)&os_memory_info.ta_ram_base, 0);
	script_parser_fetch("/firmware/optee", "ta_ram_size",
			    (int *)&os_memory_info.ta_ram_size, 0);
#endif
	script_parser_fetch("/firmware/optee", "tee_ram_base",
			    (int *)&os_memory_info.tee_ram_base, 0);
	script_parser_fetch("/firmware/optee", "tee_ram_size",
			    (int *)&os_memory_info.tee_ram_size, 0);
	return 0;
}

#if defined CONFIG_SUN50IW9_AUTOPRINT
int check_printmode(void)
{
	u32 reg, data;
	reg  = readl(0x0300B0B4);
	data = reg & (0x7 << 16);
	data |= reg & (0x7 << 8);
	/* if PF2 --> TX, PF4 --> RX */
	if (data == 0x00030300) {
		pr_info(" card print\n");
		return 1;
	}
	return 0;
}

int open_autoprint(void)
{
	int nodeoffset, err;
	nodeoffset = fdt_path_offset(working_fdt, "/soc/auto_print");
	if (nodeoffset < 0) {
		pr_err("libfdt fdt_path_offset() returned %s\n",
		       fdt_strerror(nodeoffset));
		return -1;
	}
	err = fdt_setprop_string(working_fdt, nodeoffset, "status", "okay");
	if (err < 0) {
		pr_warn("WARNING: fdt_setprop can't set %s from node %s: %s\n",
			"compatible", "status", fdt_strerror(err));
		return -1;
	}
	return 0;
}
#endif

int sunxi_update_fdt_para_for_kernel(void)
{
	uint storage_type = 0;
	__maybe_unused int ret = 0;
#ifdef CONFIG_SUNXI_SDMMC
	struct mmc *mmc = NULL;
	int dev_num = 0;
#endif

	storage_type = get_boot_storage_type_ext();

#ifdef CONFIG_SUNXI_SDMMC
	/* update sdhc dbt para */
	if (storage_type == STORAGE_EMMC || storage_type == STORAGE_EMMC3
			|| (storage_type == STORAGE_EMMC0)) {
		if (storage_type == STORAGE_EMMC)
			dev_num = 2;
		else if (storage_type == STORAGE_EMMC0)
			dev_num = 0;
		else
			dev_num = 3;
	//	dev_num = (storage_type == STORAGE_EMMC) ? 2 : 3;
		mmc = find_mmc_device(dev_num);
		if (mmc == NULL) {
			printf("can't find valid mmc %d\n", dev_num);
			return -1;
		}
		if (mmc->cfg->sample_mode == AUTO_SAMPLE_MODE) {
			mmc_update_config_for_sdly(mmc);
		}
	}
#endif
	/* creat udc dbt para when in charger_mode */
	if (gd->chargemode == 1) {
		fdt_find_and_setprop(working_fdt, "/soc/udc-controller",
					"charger_mode", NULL, 0, 1);
	}

	/* fix nand&sdmmc */
	switch (storage_type) {
	case STORAGE_NAND:
		fdt_enable_node("nand0", 1);
		fdt_enable_node("spi0", 0);
		break;
	case STORAGE_SPI_NAND:
#ifdef CONFIG_SUNXI_UBIFS
#ifdef CONFIG_MACH_SUN8IW18
		if (nand_use_ubi() == 0) {
			/* sun8iw18 aw-nftl config */
			fdt_enable_node("spinand", 1);
			fdt_enable_node("spi0", 0);
		}
#else
		/* sun50iw11 config */
		fdt_enable_node("spi0", 1);
		fdt_enable_node("/soc/spi/spi-nand", 1);
#endif
#else
		/* legacy config */
		fdt_enable_node("spinand", 1);
		fdt_enable_node("spi0", 0);
#endif
		break;
	case STORAGE_EMMC:
		fdt_enable_node("mmc2", 1);
		fdt_enable_node("sunxi-mmc2", 1);
		break;
	case STORAGE_EMMC0:
		fdt_enable_node("mmc0", 1);
		fdt_enable_node("sunxi-mmc0", 1);
		break;
	case STORAGE_EMMC3:
		fdt_enable_node("mmc3", 1);
		fdt_enable_node("sunxi-mmc3", 1);
		break;
	case STORAGE_SD:
		fdt_enable_node("mmc0", 1);
		fdt_enable_node("sunxi-mmc0", 1);
		{
			uint32_t dragonboard_test = 0;
			script_parser_fetch("/soc/target", "dragonboard_test",
						(int *)&dragonboard_test, 0);
			if (dragonboard_test == 1) {
				fdt_enable_node("mmc2", 1);
				fdt_enable_node("sunxi-mmc2", 1);
#ifdef CONFIG_SUNXI_SDMMC
				mmc_update_config_for_dragonboard(2);
#ifdef CONFIG_MMC3_SUPPORT
				mmc_update_config_for_dragonboard(3);
#endif
#endif
			}
		}
		break;
	case STORAGE_NOR:
		fdt_enable_node("/soc/spi", 1);
		fdt_enable_node("/soc/spi/spi_board0", 1);
		break;
	default:
		break;
	}

#if defined(CONFIG_SUNXI_DRM_SUPPORT)
	ulong drm_base = 0, drm_size = 0;
	if (gd->securemode == SUNXI_SECURE_MODE_WITH_SECUREOS) {
		if (!smc_tee_probe_drm_configure(&drm_base, &drm_size)) {
			pr_msg("drm_base=0x%lx\n", drm_base);
			pr_msg("drm_size=0x%lx\n", drm_size);
			ret = fdt_add_mem_rsv(working_fdt, drm_base, drm_size);
			if (ret)
				pr_err("##add mem rsv error: %s : %s\n",
				       __func__, fdt_strerror(ret));
		}
	}
#endif

#if defined(CONFIG_SUNXI_EXTERN_SECURE_MM_LAYOUT)
	if (os_memory_info.shm_size) {
		pr_msg("shm_base=0x%lx\n", os_memory_info.shm_base);
		pr_msg("shm_size=0x%lx\n", os_memory_info.shm_size);
		ret = fdt_add_mem_rsv(working_fdt, os_memory_info.shm_base,
				      os_memory_info.shm_size);
		if (ret)
			pr_err("##add mem rsv error: %s : %s\n", __func__,
			       fdt_strerror(ret));
	}
	if (os_memory_info.ta_ram_size) {
		pr_msg("ta_ram_base=0x%lx\n", os_memory_info.ta_ram_base);
		pr_msg("ta_ram_size=0x%lx\n", os_memory_info.ta_ram_size);
		ret = fdt_add_mem_rsv(working_fdt, os_memory_info.ta_ram_base,
				      os_memory_info.ta_ram_size);
		if (ret)
			pr_err("##add mem rsv error: %s : %s\n", __func__,
			       fdt_strerror(ret));
	}
#endif
	if (os_memory_info.tee_ram_size) {
		pr_msg("tee_ram_base=0x%lx\n", os_memory_info.tee_ram_base);
		pr_msg("tee_ram_size=0x%lx\n", os_memory_info.tee_ram_size);
		ret = fdt_add_mem_rsv(working_fdt, os_memory_info.tee_ram_base,
				      os_memory_info.tee_ram_size);
		if (ret)
			pr_err("##add mem rsv error: %s : %s\n", __func__,
			       fdt_strerror(ret));
	}
#ifdef CONFIG_SPI_SAMP_DL_EN
	int nodeoffset = 0;
	nodeoffset = fdt_path_offset(working_fdt, "spi0");
	if (nodeoffset < 0) {
		pr_err("## error: %s : %s\n", __func__,
				fdt_strerror(nodeoffset));
		return -1;
	}

	switch (storage_type) {
	case STORAGE_NOR:
#ifdef CONFIG_SUNXI_SPINOR
	{
		struct sunxi_spi_slave *sspi = get_sspi();
		fdt_setprop_u32(working_fdt, nodeoffset,
				"sample_mode", sspi->right_sample_mode);
		fdt_setprop_u32(working_fdt, nodeoffset,
				"sample_delay", sspi->right_sample_delay);
		pr_msg("spinor update sample_mode:%x right_sample_mod:%x\n",
				sspi->right_sample_mode,
				sspi->right_sample_delay);
	}
#endif
		break;
	case STORAGE_SPI_NAND:
#ifdef CONFIG_SUNXI_NAND
	{
		struct aw_spinand *spinand = get_spinand();
		fdt_setprop_u32(working_fdt, nodeoffset,
				"sample_mode", spinand->right_sample_mode);
		fdt_setprop_u32(working_fdt, nodeoffset,
				"sample_delay", spinand->right_sample_delay);
		pr_msg("spinand update sample_mode:%x right_sample_mod:%x\n",
				spinand->right_sample_mode,
				spinand->right_sample_delay);
	}
#endif
		break;
	default:
		pr_err("The storage not support sample function\n");
		break;
	}

#endif

	/* fix dram para */
	update_fdt_dram_para(working_fdt);
#ifdef CONFIG_SUNXI_SPINOR_JPEG
int save_jpg_logo_to_kernel(void);
	save_jpg_logo_to_kernel();
#endif

#ifdef CONFIG_SUNXI_SPINOR_BMP
int save_bmp_logo_to_kernel(void);
	save_bmp_logo_to_kernel();
#endif

#ifdef CONFIG_BOOT_GUI
	save_disp_cmd();
	disp_update_lcd_param(-1);
#endif
#ifdef CONFIG_SUNXI_MAC
	extern int update_sunxi_mac(void);
	update_sunxi_mac();
#endif
#if defined(CONFIG_SUN50IW9_AUTOPRINT)
	if (check_printmode()) {
		open_autoprint();
	}
#endif
	tick_printf("update dts\n");
	return 0;
}

int sunxi_update_partinfo(void)
{
	int index, ret;
	char partition_sets[PARTITION_SETS_MAX_SIZE];
	char part_name[PARTITION_NAME_MAX_SIZE];
	char root_part_name[ROOT_PART_NAME_MAX_SIZE];
	char *root_partition;
	char blkoops_part_name[DEV_PART_NAME_MAX_SIZE];
	char *blkoops_partition;
	char *partition_index = partition_sets;
	int offset = 0;
	int temp_offset = 0;
	int storage_type = get_boot_storage_type();
	struct blk_desc *desc;
	disk_partition_t info = { 0 };

	memset(root_part_name, 0, ROOT_PART_NAME_MAX_SIZE);
	root_partition = env_get("root_partition");
	if (root_partition)
		printf("root_partition is %s\n", root_partition);

	memset(blkoops_part_name, 0, DEV_PART_NAME_MAX_SIZE);
	blkoops_partition = env_get("blkoops_partition");
	if (blkoops_partition)
		printf("blkoops_partition is %s\n", blkoops_partition);

	memset(partition_sets, 0, PARTITION_SETS_MAX_SIZE);
	desc = blk_get_devnum_by_typename("sunxi_flash", 0);
	if (desc == NULL)
		return -ENODEV;

	for (index = 1;; index++) {
#if defined (CONFIG_ENABLE_MTD_CMDLINE_PARTS_BY_ENV) /*Get partitiones by env*/
		extern int sunxi_partition_parse_get_info(int index, disk_partition_t *info);
		ret = sunxi_partition_parse_get_info(index, &info);
#else /* Get partitiones by GPT */
		ret = part_get_info(desc, index, &info);
		debug("%s: try part %d, ret = %d\n", __func__, index, ret);
#endif
		if (ret < 0)
			break;

		memset(part_name, 0, PARTITION_NAME_MAX_SIZE);
		if (storage_type == STORAGE_NAND) {
			sprintf(part_name, "nand0p%d", index);
		} else if (storage_type == STORAGE_NOR) {
			sprintf(part_name, "mtdblock%d", index);
		} else {
			sprintf(part_name, "mmcblk0p%d", index);
		}

		temp_offset = strlen((char*)info.name) + strlen(part_name) + 2;
		if (temp_offset >= PARTITION_SETS_MAX_SIZE) {
			printf("partition_sets is too long, please reduces "
			       "partition name\n");
			break;
		}
		sprintf(partition_index, "%s@%s:", info.name, part_name);
		if (root_partition && strncmp(root_partition, (char *)info.name, sizeof(info.name)) == 0)
			sprintf(root_part_name, "/dev/%s", part_name);
		if (blkoops_partition && strncmp(blkoops_partition, (char *)info.name, sizeof(info.name)) == 0)
			sprintf(blkoops_part_name, "/dev/%s", part_name);
		offset += temp_offset;
		partition_index = partition_sets + offset;

	}
	partition_sets[offset - 1] = '\0';
	partition_sets[PARTITION_SETS_MAX_SIZE - 1] = '\0';
	env_set("partitions", partition_sets);

#ifdef CONFIG_SUNXI_DM_VERITY
	if (*root_part_name != 0) {
		printf("set root to dm-0\n");
		env_set("verity_dev", root_part_name);
		if (storage_type == STORAGE_NAND)
			env_set("nand_root", "/dev/dm-0");
		else if (storage_type == STORAGE_NOR)
			env_set("nor_root", "/dev/dm-0");
		else
			env_set("mmc_root", "/dev/dm-0");
	}
#else
	if (*root_part_name != 0) {
		printf("set root to %s\n", root_part_name);
		if (storage_type == STORAGE_NAND)
			env_set("nand_root", root_part_name);
		else if (storage_type == STORAGE_NOR)
			env_set("nor_root", root_part_name);
		else
			env_set("mmc_root", root_part_name);
	}
#endif
	if (*blkoops_part_name != 0) {
		printf("set blkoops_blkdev to %s\n", blkoops_part_name);
		env_set("blkoops_blkdev", blkoops_part_name);
	}
	tick_printf("update part info\n");
	return 0;
}

int sunxi_force_rotpk(void)
{
	return ((uboot_spare_head.boot_data.func_mask &
		 UBOOT_FUNC_MASK_BIT_FORCE_ROTPK) ==
		UBOOT_FUNC_MASK_BIT_FORCE_ROTPK);
}

int sunxi_update_rotpk_info(void)
{
	char rotpk_status[16] = "";
	int ret;
	ret = sunxi_efuse_get_rotpk_status();
	if (ret >= 0) {
		sprintf(rotpk_status, "%d", ret);
		env_set("rotpk_status", rotpk_status);
	}

	if (sunxi_force_rotpk()) {
		if (ret < 1) {
			pr_err("rotpk required but not burned\n");
			return -1;
		}
	}
	return 0;
}
