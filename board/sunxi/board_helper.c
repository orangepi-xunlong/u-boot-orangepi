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
#include <sys_partition.h>
#include <sys_config.h>
#include <sunxi-ir.h>
#include <smc.h>
#include <mmc.h>

DECLARE_GLOBAL_DATA_PTR;

/*define ir or key mode value*/
#define EFEX_VALUE                                      (0x81)
#define SPRITE_RECOVERY_VALUE                           (0X82)
#define BOOT_RECOVERY_VALUE                             (0x83)
#define BOOT_FACTORY_VALUE                              (0x84)


#define PARTITION_SETS_MAX_SIZE 1024
#define PARTITION_NAME_MAX_SIZE 16
#define ROOT_PART_NAME_MAX_SIZE (PARTITION_NAME_MAX_SIZE + 5)

#ifdef CONFIG_DRAM_VER_1
int update_fdt_dram_para(void *dtb_base)
{
	/*fix dram para*/
	int nodeoffset = 0;
	uint32_t *dram_para = NULL;
	dram_para = (uint32_t *)uboot_spare_head.boot_data.dram_para;

	pr_msg("(weak)update dtb dram start\n");
	nodeoffset = fdt_path_offset(dtb_base, "/dram");
	if (nodeoffset < 0) {
		pr_err("## error: %s : %s\n", __func__, fdt_strerror(nodeoffset));
		return -1;
	}
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_clk", dram_para[0]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_type", dram_para[1]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_zq", dram_para[2]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_odt_en", dram_para[3]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_para1", dram_para[4]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_para2", dram_para[5]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_mr0", dram_para[6]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_mr1", dram_para[7]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_mr2", dram_para[8]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_mr3", dram_para[9]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_tpr0", dram_para[10]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_tpr1", dram_para[11]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_tpr2", dram_para[12]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_tpr3", dram_para[13]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_tpr4", dram_para[14]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_tpr5", dram_para[15]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_tpr6", dram_para[16]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_tpr7", dram_para[17]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_tpr8", dram_para[18]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_tpr9", dram_para[19]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_tpr10", dram_para[20]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_tpr11", dram_para[21]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_tpr12", dram_para[22]);
	fdt_setprop_u32(dtb_base, nodeoffset, "dram_tpr13", dram_para[23]);
	gd->bd->bi_dram[0].size = (phys_size_t)uboot_spare_head.boot_data.dram_scan_size * 1024 * 1024;
	pr_msg("update dtb dram  end\n");
	return 0;
}
#else
#error "platform not support"
#endif

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
	int i, j;
	char *buf = (char *)addr;

	for (j = 0; j < size; j += 16) {
		for (i = 0; (i < 16 && i < size); i++) {
			printf("%02x ", buf[j + i] & 0xff);
		}
		printf("\n");
	}
	printf("\n");

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

static int sunxi_set_bootcmd_from_rtc(int mode, char *bootcmd)
{
	u8 bootmode_flag = 0;

	if (mode == 0)
		return 0;

	bootmode_flag = rtc_get_bootmode_flag();

	/*clear rtc*/
	rtc_set_bootmode_flag(0);
	switch (bootmode_flag) {
	case SUNXI_EFEX_CMD_FLAG:
		debug("find efex cmd\n");
		sunxi_board_run_fel();
		break;
	case SUNXI_BOOT_RECOVERY_FLAG:
		debug("Recovery detected, will boot recovery\n");
		sunxi_str_replace(bootcmd, "boot_normal", "boot_recovery");
		break;
	case SUNXI_FASTBOOT_FLAG:
		debug("Fastboot detected, will boot fastboot\n");
		sunxi_str_replace(bootcmd, "boot_normal", "boot_fastboot");
		break;
	case SUNXI_UBOOT_FLAG:
		debug("uboot shell detected, will uboot shell\n");
		sunxi_set_uboot_shell(1);
		break;
	default:
		return -1;
		break;
	}
	return 0;
}

#ifdef CONFIG_SUNXI_ANDROID_BOOT
static int sunxi_get_bootcmd_from_lradc(int mode, char *bootcmd)
{
	int ret1, ret2;
	int key_high, key_low;
	int keyvalue;

	if (mode == 0)
		return 0;

	keyvalue = uboot_spare_head.boot_data.key_input;
	pr_msg("key %d\n", keyvalue);

	ret1 = script_parser_fetch("/soc/recovery_key", "key_max", &key_high,
				   1);
	ret2 = script_parser_fetch("/soc/recovery_key", "key_min", &key_low, 1);
	if ((ret1) || (ret2)) {
		pr_msg("cant find rcvy value\n");
	} else {
		pr_notice("recovery key high %d, low %d\n", key_high, key_low);
		if ((keyvalue >= key_low) && (keyvalue <= key_high)) {
			pr_notice("key found, android recovery\n");
			sunxi_str_replace(bootcmd, "boot_normal",
					  "boot_recovery");
			return 0;
		}
	}
	ret1 = script_parser_fetch("/soc/fastboot_key", "key_max", &key_high,
				   1);
	ret2 = script_parser_fetch("/soc/fastboot_key", "key_min", &key_low, 1);
	if ((ret1) || (ret2)) {
		pr_msg("cant find fstbt value\n");
	} else {
		pr_notice("fastboot key high %d, low %d\n", key_high, key_low);
		if ((keyvalue >= key_low) && (keyvalue <= key_high)) {
			pr_notice("key found, android fastboot\n");
			sunxi_str_replace(bootcmd, "boot_normal",
					  "boot_fastboot");
			return 0;
		}
	}
	return -1;
}

static int sunxi_set_bootcmd_from_misc(int mode, char *bootcmd)
{
	u32 misc_offset = 0;
	char misc_args[2048];
	char misc_fill[2048];
	struct bootloader_message *misc_message;

	if (mode == 0)
		return 0;

	misc_message = (struct bootloader_message *)misc_args;
	memset(misc_args, 0x0, 2048);
	memset(misc_fill, 0xff, 2048);

	misc_offset = sunxi_partition_get_offset_byname("misc");
	if (!misc_offset) {
		pr_msg("no misc partition is found\n");
	} else {
		pr_msg("misc partition found\n");
		sunxi_flash_read(misc_offset, 2048 / 512, misc_args);
	}

	if (strstr((const char *)misc_message->command, "efex") != NULL) {
		pr_notice("find efex cmd\n");
		sunxi_flash_write(misc_offset, 2048 / 512, misc_fill);
		sunxi_board_run_fel();
		return 0;
		/*"sysrecovery" must judge before "recovery" !!!*/
	} else if (strstr((const char *)misc_message->command, "sysrecovery") !=
		   NULL) {
		pr_notice("sprite-test detected, will boot sysrecovery\n");
		uboot_spare_head.boot_data.work_mode =
			WORK_MODE_SPRITE_RECOVERY;
		env_set("sysrecovery", "sprite_test");
		strncpy(bootcmd, "run sysrecovery", sizeof("run sysrecovery"));
	} else if (strstr((const char *)misc_message->command, "recovery") !=
		   NULL) {
		pr_notice("Recovery detected, will boot recovery\n");
		sunxi_str_replace(bootcmd, "boot_normal", "boot_recovery");
	} else if (strstr((const char *)misc_message->command, "bootloader") !=
		   NULL) {
		pr_notice("Fastboot detected, will boot fastboot\n");
		sunxi_str_replace(bootcmd, "boot_normal", "boot_fastboot");
	} else if (strstr((const char *)misc_message->command, "uboot") !=
		   NULL) {
		pr_notice("uboot shell detected, will uboot shell\n");
		sunxi_set_uboot_shell(1);
	} else {
		return -1;
	}
	if (strstr((const char *)misc_message->recovery, "update_package") ==
	    NULL) {
		memset(misc_message->command, 0x0,
		       sizeof(misc_message->command));
		sunxi_flash_write(misc_offset, 2048 / 512, misc_args);
	}

	return 0;
}
#endif

int sunxi_update_bootcmd(void)
{
	int   boot_mode;
	char  boot_commond[128];
	int   storage_type = get_boot_storage_type();

	memset(boot_commond, 0x0, 128);
	strncpy(boot_commond, env_get("bootcmd"), sizeof(boot_commond)-1);
	debug("base bootcmd=%s\n", boot_commond);

	if ((storage_type == STORAGE_SD) || (storage_type == STORAGE_EMMC) ||
	    storage_type == STORAGE_EMMC3) {
		sunxi_str_replace(boot_commond, "setargs_nand", "setargs_mmc");
		debug("bootcmd set setargs_mmc\n");
	} else if (storage_type == STORAGE_NOR) {
		sunxi_str_replace(boot_commond, "setargs_nand", "setargs_nor");
	} else if (storage_type == STORAGE_NAND) {
		debug("bootcmd set setargs_nand\n");
	}

	boot_mode = -1;

#ifdef CONFIG_SUNXI_ANDROID_BOOT
	boot_mode = sunxi_get_bootcmd_from_lradc(boot_mode, boot_commond);
#endif

	boot_mode = sunxi_set_bootcmd_from_rtc(boot_mode, boot_commond);

#ifdef CONFIG_SUNXI_ANDROID_BOOT
	boot_mode = sunxi_set_bootcmd_from_misc(boot_mode, boot_commond);
#endif

	env_set("bootcmd", boot_commond);
	debug("to be run cmd=%s\n", boot_commond);
	tick_printf("update bootcmd\n");
	return 0;
}

int sunxi_update_fdt_para_for_kernel(void)
{
	uint storage_type = 0;
	int ret = 0;
#ifdef CONFIG_SUNXI_SDMMC
	struct mmc *mmc = NULL;
	int dev_num = 0;
#endif

	storage_type = get_boot_storage_type_ext();

#ifdef CONFIG_SUNXI_SDMMC
	/* update sdhc dbt para */
	if (storage_type == STORAGE_EMMC || storage_type == STORAGE_EMMC3) {
		dev_num = (storage_type == STORAGE_EMMC) ? 2 : 3;
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
	/* fix nand&sdmmc */
	switch (storage_type) {
	case STORAGE_NAND:
#ifdef CONFIG_SUNXI_UBIFS
		if (nand_use_ubi() == 0)
#endif
		{
			fdt_enable_node("nand0", 1);
			fdt_enable_node("spi0", 0);
		}
		break;
	case STORAGE_SPI_NAND:
#ifdef CONFIG_SUNXI_UBIFS
		if (nand_use_ubi() == 0)
#endif
		{
			fdt_enable_node("spinand", 1);
			fdt_enable_node("spi0", 0);
		}
		break;
	case STORAGE_EMMC:
		fdt_enable_node("mmc2", 1);
		break;
	case STORAGE_EMMC3:
		fdt_enable_node("mmc3", 1);
		break;
	case STORAGE_SD:
		fdt_enable_node("mmc0", 1);
#if 0
	{
		uint32_t dragonboard_test = 0;
		script_parser_fetch("target", "dragonboard_test",
					  (int *)&dragonboard_test, 1);
		if (dragonboard_test == 1) {

		} else {

		}
	}
#endif
	break;
	case STORAGE_NOR:
		fdt_enable_node("/soc/spi", 1);
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

	/* fix dram para */
	ret = update_fdt_dram_para(working_fdt);
	if (ret < 0) {
		return -1;
	}
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

	memset(partition_sets, 0, PARTITION_SETS_MAX_SIZE);
	desc = blk_get_devnum_by_typename("sunxi_flash", 0);
	if (desc == NULL)
		return -ENODEV;

	for (index = 1;; index++) {
		ret = part_get_info(desc, index, &info);
		debug("%s: try part %d, ret = %d\n", __func__, index, ret);
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
		offset += temp_offset;
		partition_index = partition_sets + offset;

	}
	partition_sets[offset - 1] = '\0';
	partition_sets[PARTITION_SETS_MAX_SIZE - 1] = '\0';
	env_set("partitions", partition_sets);

	if (*root_part_name != 0) {
		printf("set root to %s\n", root_part_name);
		if (storage_type == STORAGE_NAND)
			env_set("nand_root", root_part_name);
		else if (storage_type == STORAGE_NOR)
			env_set("nor_root", root_part_name);
		else
			env_set("mmc_root", root_part_name);
	}
	tick_printf("update part info\n");
	return 0;
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
	return 0;
}
