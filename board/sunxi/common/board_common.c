/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#include <common.h>
#include <spare_head.h>
#include <sunxi_mbr.h>
#include <boot_type.h>
#include <sys_partition.h>
#include <sys_config.h>
#include <sys_config_old.h>
#include <sunxi_board.h>
#include <fastboot.h>
#include <android_misc.h>
#include <power/sunxi/pmu.h>
#include "debug_mode.h"
#include "sunxi_string.h"
#include "sunxi_serial.h"
#include <fdt_support.h>
#include <arisc.h>
#include <sunxi_nand.h>
#include <mmc.h>
#include <asm/arch/dram.h>
#include <malloc.h>
#include <efuse_map.h>
#include <smc.h>
#ifdef CONFIG_SUNXI_MULITCORE_BOOT
#include <cputask.h>
#endif
#include "../../../drivers/ir/sunxi-ir.h"
#include <asm/arch/platform.h>

DECLARE_GLOBAL_DATA_PTR;

#define PARTITION_SETS_MAX_SIZE	 1024


extern void sprite_led_init(void);
extern int sprite_led_exit(int status);
int __attribute__((weak)) mmc_request_update_boot0(int dev_num)
{
	return 0;
}
void  __attribute__((weak)) mmc_update_config_for_sdly(struct mmc *mmc)
{
	return;
}
void  __attribute__((weak)) mmc_update_config_for_dragonboard(int card_no)
{
	return;
}

struct mmc *  __attribute__((weak)) find_mmc_device(int dev_num)
{
	return NULL;
}
int __attribute__((weak)) update_fdt_dram_para(void *dtb_base)
{
	/*fix dram para*/
	int nodeoffset = 0;
	uint32_t *dram_para = NULL;
	dram_para = (uint32_t *)uboot_spare_head.boot_data.dram_para;

	pr_msg("(weak)update dtb dram start\n");
	nodeoffset = fdt_path_offset(dtb_base, "/dram");
	if (nodeoffset < 0) {
		printf("## error: %s : %s\n", __func__, fdt_strerror(nodeoffset));
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
	pr_msg("update dtb dram  end\n");
	return 0;
}

void sunxi_update_subsequent_processing(int next_work)
{
	int mode = 0;

	printf("next work %d\n", next_work);

	mode = uboot_spare_head.boot_data.work_mode;

	if ((mode & WORK_MODE_PRODUCT) || (mode & WORK_MODE_UPDATE))
	{
#ifdef CONFIG_MMC_BOOT_START
		sunxi_mmc_enable_bootop(1, 1);
#endif
	}

	switch(next_work)
	{
		case SUNXI_UPDATE_NEXT_ACTION_REBOOT:
		case SUNXI_UPDATA_NEXT_ACTION_SPRITE_TEST:
			printf("SUNXI_UPDATE_NEXT_ACTION_REBOOT\n");
			sunxi_board_restart(0);
			break;

		case SUNXI_UPDATE_NEXT_ACTION_SHUTDOWN:
			printf("SUNXI_UPDATE_NEXT_ACTION_SHUTDOWN\n");
			sunxi_board_shutdown();
			break;

		case SUNXI_UPDATE_NEXT_ACTION_REUPDATE:
			printf("SUNXI_UPDATE_NEXT_ACTION_REUPDATE\n");
			sunxi_board_run_fel();
			break;

		case SUNXI_UPDATE_NEXT_ACTION_BOOT:
		case SUNXI_UPDATE_NEXT_ACTION_NORMAL:
		default:
			printf("SUNXI_UPDATE_NEXT_ACTION_NULL\n");
			break;
	}

	return ;
}

__maybe_unused static void  __set_part_name_for_nand(int index, char* part_name,int part_type)
{
	if(PART_TYPE_GPT == part_type)
	{
		sprintf(part_name, "nand0p%d", index+1);
	}
	else
	{
		sprintf(part_name, "nand%c", 'a' + index);
	}
}
__maybe_unused static void __set_part_name_for_sdmmc(int index, char* part_name,int part_type, int part_total)
{
	if(PART_TYPE_GPT == part_type)
	{
		sprintf(part_name, "mmcblk0p%d", index+1);
	}
	else
	{
		if(index == 0)
		{
			strcpy(part_name, "mmcblk0p2");
		}
		else if( (index+1)==part_total)
		{
			strcpy(part_name, "mmcblk0p1");
		}
		else
		{
			sprintf(part_name, "mmcblk0p%d", index + 4);
		}
	}
}


void sunxi_fastboot_init(void)
{
	struct fastboot_ptentry fb_part;
	int index, part_total;
	char partition_sets[PARTITION_SETS_MAX_SIZE];
	char part_name[16];
	char *partition_index = partition_sets;
	int offset = 0;
	int temp_offset = 0;
	int storage_type = get_boot_storage_type();
	__maybe_unused int part_type = 0;

	printf("--------fastboot partitions--------\n");
	part_total = sunxi_partition_get_total_num();
	part_type  = sunxi_partition_get_type();
	if((part_total <= 0) || (part_total > SUNXI_MBR_MAX_PART_COUNT))
	{
		printf("mbr not exist\n");

		return ;
	}
	printf("-total partitions:%d-\n", part_total);
	printf("%-12s  %-12s  %-12s\n", "-name-", "-start-", "-size-");

	memset(partition_sets, 0, PARTITION_SETS_MAX_SIZE);

	for(index = 0; index < part_total && index < SUNXI_MBR_MAX_PART_COUNT; index++)
	{
		sunxi_partition_get_name(index, &fb_part.name[0]);
		fb_part.start = sunxi_partition_get_offset(index) * 512;
		fb_part.length = sunxi_partition_get_size(index) * 512;
		fb_part.flags = 0;
		printf("%-12s: %-12x  %-12x\n", fb_part.name, fb_part.start, fb_part.length);

		memset(part_name, 0, 16);
#ifdef CONFIG_SUNXI_GPT
		if(!storage_type)
		{
			sprintf(part_name, "nand0p%d", index+1);
		}
		else
		{
			sprintf(part_name, "mmcblk0p%d", index+1);
		}

#else
		if(!storage_type)
		{
			__set_part_name_for_nand(index, part_name, part_type);
		}
                else if(storage_type == STORAGE_NOR)
                {
                        sprintf(part_name, "mtdblock%d", index + 1);
                }
		else
		{
			__set_part_name_for_sdmmc(index, part_name, part_type,part_total);
		}
#endif

		temp_offset = strlen(fb_part.name) + strlen(part_name) + 2;
		if(temp_offset >= PARTITION_SETS_MAX_SIZE)
		{
			printf("partition_sets is too long, please reduces partition name\n");
			break;
		}
		//fastboot_flash_add_ptn(&fb_part);
		sprintf(partition_index, "%s@%s:", fb_part.name, part_name);
		offset += temp_offset;
		partition_index = partition_sets + offset;
	}

	partition_sets[offset-1] = '\0';
	partition_sets[PARTITION_SETS_MAX_SIZE - 1] = '\0';
	printf("-----------------------------------\n");

	setenv("partitions", partition_sets);
}


/*ir or key mode in sys_config*/
#define EFEX_MODE_CONFIG                                (0x0)
#define ONEKEY_SPRITE_RECOVERY_MODE_CONFIG              (0x1)
#define RECOVERY_MODE_CONFIG                            (0x2)
#define FACTORY_MODE_CONFIG                             (0x3)

/*define ir or key mode value*/
#define EFEX_VALUE                                      (0x81)
#define SPRITE_RECOVERY_VALUE                           (0X82)
#define BOOT_RECOVERY_VALUE                             (0x83)
#define BOOT_FACTORY_VALUE                              (0x84)

/*key press mode in sys_config*/
#define KEY_PRESS_MODE_DISABLE                          (0X0)
#define KEY_PRESS_MODE_ENABLE                           (0x1)


#define USB_RECOVERY_KEY_VALUE				(0x81)
#define SPRITE_RECOVERY_KEY_VALUE			(0X82)
#define IR_BOOT_RECOVERY_VALUE				(0x88)

#ifdef CONFIG_IR_BOOT_RECOVERY
extern int check_ir_boot_recovery(void);
int respond_ir_boot_recovery_action(void)
{
    return check_ir_boot_recovery();
}
#endif
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int check_physical_key_early(void)
{
	user_gpio_set_t	gpio_recovery;
	__u32 gpio_hd;
	int ret;
	int gpio_value = 0;
	int used = 0;
	int mode = 0;
        int press_mode = 0;
        int press_time = 0;

	if(uboot_spare_head.boot_data.work_mode != WORK_MODE_BOOT)
	{
		return 0;
	}
#ifdef CONFIG_IR_BOOT_RECOVERY
	/*check the ir*/
	respond_ir_boot_recovery_action();
#endif
	/*check physical gpio pin*/
	ret = script_parser_fetch("key_boot_recovery", "used", (int *)&used, sizeof(int) / 4);
	if (ret || !used)
	{
		pr_msg("[key recovery] no use\n");
		return 0;
	}

	ret = script_parser_fetch("key_boot_recovery", "recovery_key", (int *)&gpio_recovery, sizeof(user_gpio_set_t) / 4);
        if(ret)
        {
                pr_error("[key recovery] can't find recovery_key config.\n");
                return 0;
        }
        gpio_recovery.mul_sel = 0;		//强制设置成输入
        gpio_hd = gpio_request(&gpio_recovery, 1);
        if (!gpio_hd)
        {
            pr_error("[key recovery] gpio request fail!\n");
            return 0;                           //gpio request fail,just return.
        }

        int time;
        gpio_value = 0;
        for(time = 0; time < 4; time++)
        {
            gpio_value += gpio_read_one_pin_value(gpio_hd, 0);
            __msdelay(5);
        }

        if (gpio_value)
        {
                pr_msg("[key recovery] no key press.\n");
                return 0;                       //no recovery key was pressed, just return.
        }

        pr_msg("[box recovery] find the key\n");
        script_parser_fetch("key_boot_recovery", "press_mode_enable", (int *)&press_mode, sizeof(int) / 4);
        if (press_mode == KEY_PRESS_MODE_DISABLE)
        {
            script_parser_fetch("key_boot_recovery", "key_work_mode", (int *)&mode, sizeof(int) / 4);
            pr_msg("[key recovery] do not use prese mode, key_work_mode = %d\n", mode);
        }
        else if (press_mode == KEY_PRESS_MODE_ENABLE)
        {
            script_parser_fetch("key_boot_recovery", "key_press_time", (int *)&press_time, sizeof(int) / 4);
            pr_msg("[key recovery] use prese mode, and key_press_time = %d ms\n", press_time);
            gpio_value = 0;
            for (time = press_time; time > 0; time-=100)
            {
                __msdelay(100);          //detect key value per 100ms
                gpio_value += gpio_read_one_pin_value(gpio_hd, 0);

                if (gpio_value)          //key was loosed during press time, will not detect key value any more
                    break;
            }

            if (gpio_value)                  //key was lossed during press time, so use short_press_mode
            {
                script_parser_fetch("key_boot_recovery", "short_press_mode", (int *)&mode, sizeof(int) / 4);
                pr_msg("[key recovery] key short press, short_press_mode = %d\n", mode);
            }
            else                             //key was always pressed in key_press_time, so use long_press_mode
            {
                script_parser_fetch("key_boot_recovery", "long_press_mode", (int *)&mode, sizeof(int) / 4);
                pr_msg("[key recovery] key long press, long_press_mode = %d\n", mode);
            }
        }

        switch (mode)
        {
            case EFEX_MODE_CONFIG:
                gd->key_pressd_value = EFEX_VALUE;
                break;
            case ONEKEY_SPRITE_RECOVERY_MODE_CONFIG:
                uboot_spare_head.boot_data.work_mode = WORK_MODE_SPRITE_RECOVERY;
                break;
            case RECOVERY_MODE_CONFIG:
                gd->key_pressd_value = BOOT_RECOVERY_VALUE;
                break;
            case FACTORY_MODE_CONFIG:
                gd->key_pressd_value = BOOT_FACTORY_VALUE;
                break;
            default:
                gd->key_pressd_value = BOOT_RECOVERY_VALUE;
                break;
        }

	return 0;
}


int update_bootcmd(void)
{
	int   pmu_value;
        int   need_flush =0;
	u32   misc_offset = 0;
	char  misc_args[2048];
	char  misc_fill[2048];
	char  boot_commond[256];
	static struct bootloader_message *misc_message;
	int   storage_type = get_boot_storage_type();

	if(gd->force_shell)
	{
		char delaytime[8];
		sprintf(delaytime, "%d", 3);
		setenv("bootdelay", delaytime);
	}

	memset(boot_commond, 0x0, 128);
	strcpy(boot_commond, getenv("bootcmd"));
	pr_msg("base bootcmd=%s\n", boot_commond);

	if ((storage_type == STORAGE_SD) ||
		(storage_type == STORAGE_EMMC)||
		storage_type == STORAGE_EMMC3) {
		sunxi_str_replace(boot_commond, "setargs_nand", "setargs_mmc");
		pr_msg("bootcmd set setargs_mmc\n");
	} else if (storage_type == STORAGE_NOR) {
		sunxi_str_replace(boot_commond, "setargs_nand", "setargs_nor");
	} else if (storage_type == STORAGE_NAND) {
		pr_msg("bootcmd set setargs_nand\n");
	}

	//user enter debug mode by plug usb up to 3 times
	if(debug_mode_get())
	{
		//if enter debug mode,set loglevel = 8
		debug_mode_update_info();
		return 0;
	}

	misc_message = (struct bootloader_message *)misc_args;
	memset(misc_args, 0x0, 2048);
	memset(misc_fill, 0xff, 2048);

#ifdef CONFIG_SUNXI_MULITCORE_BOOT
        pmu_value = gd->pmu_saved_status;
#else
        pmu_value = axp_probe_pre_sys_mode();
#endif
        if(pmu_value == PMU_PRE_FASTBOOT_MODE)
        {
            puts("PMU : ready to enter fastboot mode\n");
            strcpy(misc_message->command, "bootloader");
        }
        else if(pmu_value == PMU_PRE_RECOVERY_MODE)
        {
            puts("PMU : ready to enter recovery mode\n");
            strcpy(misc_message->command, "boot-recovery");
        }
        else
        {
            misc_offset = sunxi_partition_get_offset_byname("misc");
            debug("misc_offset = %x\n",misc_offset);
            if(!misc_offset)
            {
                printf("no misc partition is found\n");
            }
            else
            {
                printf("misc partition found\n");
                //read misc partition data
                sunxi_flash_read(misc_offset, 2048/512, misc_args);
            }
        }

	if(!strcmp(misc_message->command, "efex"))
	{
		/* there is a recovery command */
		puts("find efex cmd\n");
		sunxi_flash_write(misc_offset, 2048/512, misc_fill);
		sunxi_board_run_fel();
		return 0;
	}
	else if(!strcmp(misc_message->command, "boot-resignature"))
	{
		puts("error: find boot-resignature cmd,but this cmd not implement\n");
		//sunxi_flash_write(misc_offset, 2048/512, misc_fill);
		//sunxi_oem_op_lock(SUNXI_LOCKING, NULL, 1);
	}
	else if(!strcmp(misc_message->command, "boot-recovery"))
	{
		if(!strcmp(misc_message->recovery, "sysrecovery"))
		{
			puts("recovery detected, will sprite recovery\n");
			strncpy(boot_commond, "sprite_recovery", sizeof("sprite_recovery"));
			uboot_spare_head.boot_data.work_mode = WORK_MODE_SPRITE_RECOVERY;//misc cmd must update work_mode
			sunxi_flash_write(misc_offset, 2048/512, misc_fill);
                        need_flush = 1;
		}
		else
		{
			puts("Recovery detected, will boot recovery\n");
			sunxi_str_replace(boot_commond, "boot_normal", "boot_recovery");
		}
		/* android recovery will clean the misc */
	}
	else if(!strcmp(misc_message->command, "bootloader"))
	{
		puts("Fastboot detected, will boot fastboot\n");
                sunxi_str_replace(boot_commond, "boot_normal", "boot_fastboot");
                if(misc_offset)
                {
                    sunxi_flash_write(misc_offset, 2048/512, misc_fill);
                    need_flush = 1;
                }
	}
	else if(!strcmp(misc_message->command ,"debug_mode"))
	{
		puts("debug_mode detected ,will enter debug_mode");
		if(0 == debug_mode_set())
		{
			debug_mode_update_info();
		}
		sunxi_flash_write(misc_offset,2048/512,misc_fill);
		need_flush = 1;
        }
        else if(!strcmp(misc_message->command, "sysrecovery"))
        {
            puts("sysrecovery detected, will sprite recovery\n");
            strncpy(boot_commond, "sprite_recovery", sizeof("sprite_recovery"));
            uboot_spare_head.boot_data.work_mode = WORK_MODE_SPRITE_RECOVERY;//misc cmd must update work_mode
            sunxi_flash_write(misc_offset, 2048/512, misc_fill);
            need_flush = 1;
        }

        if( uboot_spare_head.boot_data.work_mode == WORK_MODE_SPRITE_RECOVERY)
        {
#ifdef CONFIG_SUNXI_MULITCORE_BOOT
            extern int sunxi_secondary_cpu_poweroff(void);
            /*power off multi core to shutdown logo display in order to show sprite progressbar*/
            sunxi_secondary_cpu_poweroff();
#endif
        }

        if(need_flush == 1)
            sunxi_flash_flush();


#ifdef CONFIG_SUNXI_MULITCORE_BOOT
	if (gd->need_shutdown) {
		sunxi_secondary_cpu_poweroff();
		sunxi_board_shutdown();
		for(;;);
	}
#endif

	setenv("bootcmd", boot_commond);
	pr_msg("to be run cmd=%s\n", boot_commond);

	return 0;
}


int disable_node(char* name)
{

	int nodeoffset = 0;
	int ret = 0;

	if(strcmp(name,"mmc3") == 0)
	{
#ifndef CONFIG_MMC3_SUPPORT
		return 0;
#endif
	}
	nodeoffset = fdt_path_offset(working_fdt, name);
	ret = fdt_set_node_status(working_fdt,nodeoffset,FDT_STATUS_DISABLED,0);
	if(ret < 0)
	{
		printf("disable nand error: %s\n", fdt_strerror(ret));
	}
	return ret;
}

int  dragonboard_handler(void* dtb_base)
{
#ifdef CONFIG_SUNXI_DRAGONBOARD_SUPPORT
	int card_num = 0;

	int ret;
	printf("%s call\n",__func__);

#ifdef CONFIG_SUNXI_UBIFS
	if (sunxi_get_mtd_ubi_mode_status())
		ret = sunxi_nand_uboot_probe();
	else
#endif
		ret = nand_uboot_probe();

	/* nand_uboot_init(1) */
	if (!ret)
	{
		printf("try nand successed\n");
		disable_node("mmc2");
		disable_node("mmc3");
	}
	else
	{
		struct mmc *mmc_tmp = NULL;
#ifdef CONFIG_MMC3_SUPPORT
		card_num = 3;
#else
		card_num = 2;
#endif
		printf("try to find card%d \n", card_num);
		board_mmc_set_num(card_num);
		board_mmc_pre_init(card_num);
		mmc_tmp = find_mmc_device(card_num);
		if(!mmc_tmp)
		{
			return -1;
		}
		if (0 == mmc_init(mmc_tmp))
		{
			printf("try mmc successed \n");
			disable_node("nand0");
			if(card_num == 2)
				disable_node("mmc3");
			else if(card_num == 3)
				disable_node("mmc2");
		}
		else
		{
			printf("try card%d successed \n", card_num);
		}
		mmc_exit();
	}
#endif
	return 0;

}


int update_fdt_para_for_kernel(void* dtb_base)
{
	int ret;
	__maybe_unused int nodeoffset = 0;
	uint storage_type = 0;
	struct mmc *mmc =NULL;
	int dev_num = 0;
	extern void display_update_dtb(void);

	storage_type = get_boot_storage_type_ext();

	//update sdhc dbt para
	if(storage_type == STORAGE_EMMC || storage_type == STORAGE_EMMC3)
	{
		dev_num = (storage_type == STORAGE_EMMC)? 2:3;
		mmc = find_mmc_device(dev_num);
		if(mmc == NULL)
		{
			printf("can't find valid mmc %d\n", dev_num);
			return -1;
		}
		if(mmc->cfg->platform_caps.sample_mode == AUTO_SAMPLE_MODE)
		{
			mmc_update_config_for_sdly(mmc);
		}
	}

	//fix nand&sdmmc
	switch(storage_type)
	{
		case STORAGE_NAND:
			disable_node("mmc2");
			disable_node("mmc3");
			#ifdef CONFIG_STORAGE_MEDIA_SPINAND
				disable_node("spinand");
				disable_node("spi0");
			#endif
			break;
		case STORAGE_SPI_NAND:
			disable_node("nand0");
			disable_node("mmc2");
			disable_node("spi0");
			break;
		case STORAGE_EMMC:
			disable_node("nand0");
			disable_node("mmc3");
			disable_node("spi0");
			break;
		case STORAGE_EMMC3:
			disable_node("nand0");
			disable_node("mmc2");
			break;
		case STORAGE_SD:
		{
			uint32_t dragonboard_test = 0;
			ret = script_parser_fetch("target", "dragonboard_test", (int *)&dragonboard_test, 1);
			if(dragonboard_test == 1)
			{
				dragonboard_handler(dtb_base);
				mmc_update_config_for_dragonboard(2);
				#ifdef CONFIG_MMC3_SUPPORT
				mmc_update_config_for_dragonboard(3);
				#endif
			}
			else
			{
				disable_node("nand0");
				disable_node("mmc2");
				disable_node("mmc3");
			}
		}
			break;
		case STORAGE_NOR:
			disable_node("nand0");
			disable_node("mmc2");
			break;
		default:
			break;

	}
	//fix memory
	ret = fdt_fixup_memory(dtb_base, gd->bd->bi_dram[0].start ,gd->bd->bi_dram[0].size);
	if(ret < 0)
	{
		return -1;
	}
#if defined(CONFIG_ARCH_SUN50IW6P1) ||  defined(CONFIG_ARCH_SUN50IW3P1)
	ulong drm_base = 0, drm_size = 0;
	if ((gd->securemode == SUNXI_SECURE_MODE_WITH_SECUREOS) &&
		(!smc_tee_probe_drm_configure(&drm_base, &drm_size))) {
		pr_msg("drm_base=0x%x\n", drm_base);
		pr_msg("drm_size=0x%x\n", drm_size);
		ret = fdt_add_mem_rsv(dtb_base, drm_base, drm_size);
		if (ret)
			printf("##add mem rsv error: %s : %s\n",
				__func__, fdt_strerror(ret));
	}
#endif

	/* fix dram para */
	ret = update_fdt_dram_para(dtb_base);
	if (ret < 0) {
		return -1;
	}

#ifdef CONFIG_SUNXI_ESM_HDCP
        //update esm.img info
        nodeoffset = fdt_path_offset(dtb_base, "/soc/esm");
        if(nodeoffset < 0)
        {
            printf("## error: %s : %s\n", __func__,fdt_strerror(ret));
            return -1;
        }
        u32 esm_img_size_addr = (u32)SUNXI_ESM_IMG_SIZE_ADDR;
        u32 esm_img_buff_addr = (u32)SUNXI_ESM_IMG_BUFF_ADDR;
        fdt_setprop_u32(dtb_base, nodeoffset, "esm_img_size_addr", esm_img_size_addr);
        fdt_setprop_u32(dtb_base, nodeoffset, "esm_img_buff_addr", esm_img_buff_addr);
#endif

	return 0;
}

int get_boot_work_mode(void)
{
	return uboot_spare_head.boot_data.work_mode;
}

int get_boot_storage_type_ext(void)
{
	/* get real storage type that from BROM at boot mode*/
	return uboot_spare_head.boot_data.storage_type;
}

int get_boot_storage_type(void)
{
	/* we think that nand and spi-nand are the same storage medium */
	/* so we can use the same process to deal with them */
	if(uboot_spare_head.boot_data.storage_type == STORAGE_NAND
		|| uboot_spare_head.boot_data.storage_type == STORAGE_SPI_NAND)
	{
		return STORAGE_NAND;
	}
	return uboot_spare_head.boot_data.storage_type;
}

void set_boot_storage_type(int storage_type)
{
	uboot_spare_head.boot_data.storage_type = storage_type;
}


u32 get_boot_dram_para_addr(void)
{
	return (u32)uboot_spare_head.boot_data.dram_para;
}

u32 get_boot_dram_para_size(void)
{
	return 32*4;
}

u32 get_parameter_addr(void)
{
	return (u32)gd->parameter_mod_buf;
}

u32 get_boot_dram_update_flag(void)
{
	u32 flag = 0;
	//boot pass dram para to uboot
	//dram_tpr13:bit31
	//0:uboot update boot0  1: not
	__dram_para_t *pdram = (__dram_para_t *)uboot_spare_head.boot_data.dram_para;
	flag = (pdram->dram_tpr13>>31)&0x1;
	return flag == 0 ? 1:0;
}

void set_boot_dram_update_flag(u32 *dram_para)
{
	//dram_tpr13:bit31
	//0:uboot update boot0  1: not
	u32 flag = 0;
	__dram_para_t *pdram = (__dram_para_t *)dram_para;
	flag = pdram->dram_tpr13;
	flag |= (1<<31);
	pdram->dram_tpr13 = flag;
}

u32 get_pmu_byte_from_boot0(void)
{
	return uboot_spare_head.boot_ext[0].data[0];
}

/*
************************************************************************************************************
*
*                                             function
*
*    name          :	get_debugmode_flag
*
*    parmeters     :
*
*    return        :
*
*    note          :	guoyingyang@allwinnertech.com
*
*
************************************************************************************************************
*/
int get_debugmode_flag(void)
{
	int debug_mode = 0;

	if(get_boot_work_mode() != WORK_MODE_BOOT)
	{
		gd->debug_mode = 1;
		return 0;
	}

	 if (!script_parser_fetch("platform", "debug_mode", &debug_mode, 1))
	{
		gd->debug_mode = debug_mode;
	}
	else
	{
		gd->debug_mode = 1;
	}
	return 0;
}

/*
************************************************************************************************************
*
*                                             function
*
*    functiton     :  write "usb-recovery" to misc partiton,when enter kernel,execute this cmd to start OTA
*
*    parmeters     :
*
*    return        :
*
*    note          :	yanjianbo@allwinnertech.com
*
*
************************************************************************************************************
*/
int write_recovery_msg_to_misc(char * recovery_msg)
{
    u32   misc_offset = 0;
    char  misc_args[2048];
    static struct bootloader_message *misc_message;
    int ret;

    memset(misc_args, 0x0, 2048);
    misc_message = (struct bootloader_message *)misc_args;

    misc_offset = sunxi_partition_get_offset_byname("misc");
    if(!misc_offset)
    {
        printf("no misc partition\n");
        return 0;
    }
    ret = sunxi_flash_read(misc_offset, 2048/512, misc_args);
    if (!ret)
    {
        printf("error: read misc partition\n");
        return 0;
    }

    if(!strcmp("ir-or-key-recovery", (const char *)recovery_msg))
    {
        strcpy(misc_message->command, "boot-recovery");
    }
    else if(!strcmp("ir-factory", (const char *)recovery_msg))
    {
        strcpy(misc_message->command, "boot-recovery");
        strcpy(misc_message->recovery, "recovery\n--wipe_data\n--locale=zh_CN\n");
    }
    else if(!strcmp("ir-efex", (const char *)recovery_msg))
    {
        strcpy(misc_message->command, "efex");
    }
    else if(!strcmp("ir-sysrecovery", (const char *)recovery_msg))
    {
        strcpy(misc_message->command, "boot-recovery");
        strcpy(misc_message->recovery, "sysrecovery");
    }

    sunxi_flash_write(misc_offset, 2048/512, misc_args);
    return 0;

}

/*
************************************************************************************************************
*
*                                             function
*
*    name          :	the action after user pressing recovery key when boot
*
*    parmeters     :
*
*    return        :
*
*    note          :	yanjianbo@allwinnertech.com
*
*
************************************************************************************************************
*/
void respond_physical_key_action(void)
{
        int key_value;
#ifdef CONFIG_IR_BOOT_RECOVERY
        while(1)
        {
            if (gd->ir_detect_status != IR_DETECT_NULL)
                break;
        }
#endif
        key_value = gd->key_pressd_value;

        if (key_value == BOOT_FACTORY_VALUE)             //ir factory or key factory
        {
            printf("[boot factory] set misc : boot factory and wipe data\n");
            write_recovery_msg_to_misc("ir-factory");
        }
        else if (key_value == EFEX_VALUE)                //ir efex or key efex
        {
            printf("[boot efex] set misc : boot efex\n");
            write_recovery_msg_to_misc("ir-efex");
        }
        else if (key_value == SPRITE_RECOVERY_VALUE)     //ir sprite recovery
        {
            printf("[ir sysrecovery] set misc : boot recovery and sysrecovery \n");
            write_recovery_msg_to_misc("ir-sysrecovery");
        }
        else if (key_value == BOOT_RECOVERY_VALUE)       //ir recovery or key recovery
        {
            printf("[boot recovery] set misc : boot recovery\n");
            write_recovery_msg_to_misc("ir-or-key-recovery");
        }
}


/*
************************************************************************************************************
*
*                                             function
*
*    name          :	update_dram_para_for_ota
*
*    parmeters     :
*
*    return        :
*
*    note          :	after ota, should restore dram para to flash.
*
*
************************************************************************************************************
*/

int update_dram_para_for_ota(void)
{
	extern int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

#ifdef CONFIG_ARCH_SUN50IW1P1
	return 0;
#endif

#ifdef FPGA_PLATFORM
	return 0;
#else
	int ret = 0;
	int card_num = 2;

	if (get_boot_storage_type() == STORAGE_EMMC3)
	{
		card_num = 3;
	}

	if( (get_boot_work_mode() == WORK_MODE_BOOT) &&
		(STORAGE_SD != get_boot_storage_type()))
	{
		if(get_boot_dram_update_flag()|| mmc_request_update_boot0(card_num))
		{
			printf("begin to update boot0 atfer ota or programmer\n");
			ret = sunxi_flash_update_boot0();
			printf("update boot0 %s\n", ret==0?"success":"fail");
			//if update fail, should reset the system
			if(ret)
			{
				do_reset(NULL, 0, 0,NULL);
			}
		}
	}
	return ret;
#endif
}

int update_gpt_for_programmer(void)
{
	extern int card_download_standard_mbr(void *buffer);
	extern int card_fetch_standard_mbr(void);

#ifdef FPGA_PLATFORM
	return 0;
#else
	int ret = 0;
	int programmer = 0;
	int storage_type = get_boot_storage_type();
	sunxi_mbr_t *mbr;

	if( (get_boot_work_mode() == WORK_MODE_BOOT) &&
	((STORAGE_EMMC == storage_type || STORAGE_EMMC3 == storage_type)))
	{
		ret = script_parser_fetch("target", "programmer", &programmer, 1);
		if(ret || (programmer == 0))
		{
			printf("not suppert programmer\n");
			return -1;
		}

		ret = card_fetch_standard_mbr();
		if(ret)
		{
			mbr = (sunxi_mbr_t *)sunxi_partition_fetch_mbr();
			printf("begin to update gpt\n");
			ret = card_download_standard_mbr(mbr);
			printf("update gpt %s\n", ret==0?"success":"fail");
			if(ret)
			{
				do_reset(NULL, 0, 0,NULL);
			}
		}
	}
	return ret;
#endif
}
int board_late_init(void)
{
	int ret  = 0;
	if(get_boot_work_mode() == WORK_MODE_BOOT)
	{
		sunxi_fastboot_init();
#ifdef  CONFIG_ARCH_HOMELET
		respond_physical_key_action();
#endif
#ifndef CONFIG_SUNXI_SPINOR
		update_dram_para_for_ota();
		update_gpt_for_programmer();
#endif
		update_bootcmd();
#if defined(CONFIG_SUNXI_USER_KEY)
		update_user_data();
#endif
		ret = update_fdt_para_for_kernel(working_fdt);
#ifdef CONFIG_SUNXI_SERIAL
		sunxi_set_serial_num();
#endif
		return 0;
	}

	return ret;
}

#ifdef CONFIG_BACKUP_PARTITION
int detect_part_back_work(void)
{
	sunxi_partition     *part_info;
	u32                 i;
	char                work_buffer[512] = "not_set";
	unsigned int        work_addr = 0;
	int                 workmode  = uboot_spare_head.boot_data.work_mode;
	sunxi_mbr_t         *mbr_info = NULL;
	char                boot_commond[256];
	int                 ret  = -1;
	int                 used = 0;
	ret = script_parser_fetch("backup_part", "used", (int *)&used, sizeof(int) / 4);
	if (ret || !used)
	{
		printf("%s:[backup_part] no use\n",__func__);
		return 0;
	}

	if (workmode == WORK_MODE_BOOT || workmode == WORK_MODE_SPRITE_RECOVERY)
	{
		 mbr_info = sunxi_partition_fetch_mbr();
	}
	else if((workmode & WORK_MODE_PRODUCT) || (workmode == 0x30))
	{
		return 0;// sprite not need to detect_part_back_work
	}

	for(part_info = mbr_info->array, i=0;i<mbr_info->PartCount;i++, part_info++)
	{
	    debug("%s: for name = %s, backup_flag = 0x%x\n",__func__,part_info->name,part_info->backup_flag);
	    if(0x8000 == part_info->backup_flag)
		{
			work_addr = part_info->addrlo + part_info->lenlo - 1;
			debug("%s: work_addr = 0x%x\n",__func__,work_addr);
			break;
		}
	}

	if(0 == work_addr)
	{
	    printf("not found backup_flag @ sys_part\n");
	    return -1;
	}

	if(!sunxi_flash_read(work_addr, 1, work_buffer))//read  the  main partttion's last-block to check
	{
		printf("sunxi sprite error: download backup_flag error : start 0x%x, sectors 0x%x\n",work_addr, 1);
		return -1;
	}

	if(!strncmp("work",work_buffer,4))//detect main partttion 'work' flag, use main partition
	{
		printf("%s: detect 'work'!\n",__func__);
		return 0;
	}
	else                             //NOT detect main partttion 'work' flag, use BACKUP partition
	{
		if(!strncmp("no_work",work_buffer,7))
		{
			printf("%s: detect 'no_work'!\n",__func__);
		}
		else
		{
			printf("%s: detect nothing, but set no_work, please check!\n",__func__);
		}
		memset(boot_commond, 0x0, 128);
		strcpy(boot_commond, getenv("bootcmd"));

		sunxi_str_replace(boot_commond, "boot_normal", "boot_normal_bak");
		setenv("bootcmd", boot_commond);
	}
	return 0;
}
#endif

uint sunxi_generate_checksum(void *buffer, uint length, uint src_sum)
{
#ifndef CONFIG_USE_NEON_SIMD
	uint *buf;
	uint count;
	uint sum;

	count = length >> 2;
	sum = 0;
	buf = (__u32 *)buffer;
	do
	{
		sum += *buf++;
		sum += *buf++;
		sum += *buf++;
		sum += *buf++;
	}while( ( count -= 4 ) > (4-1) );

	while( count-- > 0 )
		sum += *buf++;
#else
	uint sum;

	sum = add_sum_neon(buffer, length);
#endif
	sum = sum - src_sum + STAMP_VALUE;

    return sum;
}

int sunxi_verify_checksum(void *buffer, uint length, uint src_sum)
{
	uint sum;
	sum = sunxi_generate_checksum(buffer, length, src_sum);

	debug("src sum=%x, check sum=%x\n", src_sum, sum);
	if( sum == src_sum )
		return 0;
	else
		return -1;
}


static int gpio_control(void)
{
	int ret;
	int used;

	ret = script_parser_fetch("boot_init_gpio", "boot_init_gpio_used", &used, sizeof(int) / 4);
	if (!ret && used) {
			gpio_request_ex("boot_init_gpio", NULL);
	}
	return 0;
}

int sunxi_led_init(void)
{
	//init some gpios for led when boot
	gpio_control();
	//init led when card product
	#ifdef CONFIG_CMD_SUNXI_SPRITE
	sprite_led_init();
	#endif
	return 0;
}

void sunxi_led_exit(int status)
{
	#ifdef CONFIG_CMD_SUNXI_SPRITE
	sprite_led_exit(status);
	#endif
}

#ifdef CONFIG_SUNXI_PIO_POWER_MODE
#define GPIO_POW_MODE_SEL (0x0340)
#define GPIO_POW_MODE_VAL (0x0348)

void sunxi_auto_sel_pio_mode(void)
{
	u32 val,tmp;

	val = readl(SUNXI_PIO_BASE + GPIO_POW_MODE_VAL)& 0xFFF;
	tmp = readl(SUNXI_PIO_BASE + GPIO_POW_MODE_SEL);
	tmp |= val;
	writel(tmp, (SUNXI_PIO_BASE + GPIO_POW_MODE_SEL));
}
#endif /* CONFIG_SUNXI_PIO_POWER_MODE */

/*--------------------------------------------------------------------*/
static char int2hex(int i)
{
	if(i>=0 && i <=9)
		return i+'0';
	else if(i>=10 && i<=15)
		return i-10 + 'A';
	else
		return -1;
}

static int hex2int(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	else if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	else if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	else
		return -1;
}
/* from 11:22:33:44:55:66(17bytes) to 112233445566(6bytes) */
int sunxi_mac_char_to_int(char *mac_char, char *result)
{
	int i = 0, tmp0 = 0, tmp1 = 0;
	if (mac_char == NULL || result == NULL) {
		printf("%s,%d,input null\n", __func__, __LINE__);
		return -1;
	}

	for (i = 0; i < 6; i++) {
		tmp0 = hex2int(mac_char[3*i]);
		tmp1 = hex2int(mac_char[3*i+1]);
		if (tmp0 == -1 || tmp1 == -1) {
			printf("wifi mac format is wrong\n");
			return -1;
		}
		result[i] |= ((tmp0 & 0x0f) << 4);
		result[i] |= (tmp1 & 0x0f);

		if ((mac_char[3*i+2] != ':' && mac_char[3*i+2] != '-') && i <= 4) {
			printf("wifi mac char format wrong\n");
			return -1;
		}
		printf("result = %02x\n", result[i]);
	}
	result[6] = 0xAA; /* flag to indicate had burned mac */
	return 0;
}

//transfrom 0x11,0x22,0x33,0x44,0x55,0x66(6bytes) to 11:22:33:44:55:66(18bytes including'\0')
int sunxi_mac_int_to_char(char *mac_int, char* result)
{
	int i;
	for(i=0;i < 6;i++)
	{
		result[i*3] = int2hex(mac_int[i] >> 4 & 0x0f);
		result[i*3+1] = int2hex(mac_int[i] & 0x0f);

		if( result[i*3] == -1 || result[i*3+1] == -1)
		{
			printf("wifi mac hex format is wrong\n");
			return -1;
		}
		if(i <= 4)
			result[i*3+2] = ':';
	}
	result[17] = '\0';
	return 0;
}

int check_mac_from_efuse(char *mac_name)
{
	int len = 0;
	int i = 0;
	char mac_int[32] = {0};

	sunxi_efuse_read(mac_name, mac_int, &len);
	if (len <= 0) {
		printf("read %s from efuse failed\n", mac_name);
		return -1;
	}

	if (mac_int[6] == 0xAA) {
		printf("find %s in efuse\n", mac_name);
		return 1;
	}

	for (i = 0; i > 7; i++) {
		if (mac_int[i] != 0x00) {
			printf("efuse %s has dirty\n", mac_name);
			return 2;
		}
	}
	return 0;
}
/* the mac maybe stored in efuse of the ic.*/
int update_mac_from_efuse(void)
{
	int need_update = 0;
	int len = 0;
	char mac_int[32] = {0};
	char mac_char[32] = {0};

	sunxi_efuse_read(EFUSE_WIFIMAC_NAME, mac_int, &len);
	if (len > 0) {
		if (mac_int[6] == 0xAA) {
			if (sunxi_mac_int_to_char(mac_int, mac_char) >= 0) {
				printf("wifi mac string is %s\n", mac_char);
				setenv(EFUSE_WIFIMAC_NAME, mac_char);
				need_update++;
			}
		} else {
			printf("have not burned wifi_mac in efuse\n");
		}
	} else {
		printf("read wifi_mac from efuse failed\n");
	}

	len = 0;
	memset(mac_int, 0, 32);
	memset(mac_char, 0, 32);

	sunxi_efuse_read(EFUSE_ETHMAC_NAME, mac_int, &len);
	if (len > 0) {
		if (mac_int[6] == 0xAA) {
			if (sunxi_mac_int_to_char(mac_int, mac_char) >= 0) {
				printf("eth mac string is %s\n", mac_char);
				setenv(EFUSE_ETHMAC_NAME, mac_char);
				need_update++;
			}
		} else {
			printf("have not burned mac in efuse\n");
		}
	} else {
		printf("read eth mac from efuse failed\n");
	}

	return need_update;
}

