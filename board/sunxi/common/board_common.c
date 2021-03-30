#include <common.h>
#include <spare_head.h>
#include <sunxi_mbr.h>
#include <boot_type.h>
#include <sys_partition.h>
#include <sys_config.h>
#include <sunxi_board.h>
#include <fastboot.h>
#include <android_misc.h>
#include <power/sunxi/pmu.h>
#include "debug_mode.h"
#include "sunxi_string.h"
#include "sunxi_serial.h"
#include <fdt_support.h>
#include <sys_config_old.h>
#include <arisc.h>
#include <sunxi_nand.h>
#include <mmc.h>
#include <asm/arch/dram.h>


DECLARE_GLOBAL_DATA_PTR;

#define PARTITION_SETS_MAX_SIZE	 1024

extern void sprite_led_init(void);
extern int sprite_led_exit(int status);


void sunxi_update_subsequent_processing(int next_work)
{
	printf("next work %d\n", next_work);
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

void sunxi_fastboot_init(void)
{
	struct fastboot_ptentry fb_part;
	int index, part_total;
	char partition_sets[PARTITION_SETS_MAX_SIZE];
	char part_name[16];
	char *partition_index = partition_sets;
	int offset = 0;
	int temp_offset = 0;
	int storage_type = uboot_spare_head.boot_data.storage_type;

	printf("--------fastboot partitions--------\n");
	part_total = sunxi_partition_get_total_num();
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
		if(!storage_type)
		{
			sprintf(part_name, "nand%c", 'a' + index);
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

#define ONEKEY_USB_RECOVERY_MODE			(0x01)
#define ONEKEY_SPRITE_RECOVERY_MODE			(0x02)
#define USB_RECOVERY_KEY_VALUE				(0x81)
#define SPRITE_RECOVERY_KEY_VALUE			(0X82)
#define IR_BOOT_RECOVERY_VALUE				(0x88)

#ifdef CONFIG_IR_BOOT_RECOVERY
extern int check_ir_boot_recovery(void);
extern int ir_boot_recovery_mode_detect(void);
int respond_ir_boot_recovery_action(void)
{
	int ret;

	check_ir_boot_recovery();

	ret = ir_boot_recovery_mode_detect();
	if (!ret) {
		gd->key_pressd_value = IR_BOOT_RECOVERY_VALUE;
		return 0;
	}

	return -1;
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
	int nodeoffset, ret;
	__u32 gpio_hd;
	 char *sub_key_value = NULL;
	int sprite_mode = 0;
	user_gpio_set_t  gpio_info;

	if(uboot_spare_head.boot_data.work_mode != WORK_MODE_BOOT)
	{
		return 0;
	}
#ifdef CONFIG_IR_BOOT_RECOVERY
	/*check the ir*/
	ret = respond_ir_boot_recovery_action();
	if (!ret)
		return 0;
#endif
	/*check physical gpio pin*/
	nodeoffset = fdt_path_offset(working_fdt, FDT_PATH_SPRITE_RECOVERY_KEY);
	if(nodeoffset >= 0)
	{
		ret = fdt_getprop_string(working_fdt, nodeoffset, "status", &sub_key_value);
		if((ret < 0) || (strcmp(sub_key_value, "okay")))
		{
			printf("sprite recovery not used\n");
			return 0;
		}
		ret = fdt_get_one_gpio(FDT_PATH_SPRITE_RECOVERY_KEY, "recovery_key", &gpio_info);
		if(ret >= 0)
		{
			gpio_info.mul_sel = 0;
			gpio_hd = gpio_request(&gpio_info, 1);
			if(ret >= 0)
			{
				int times;
				int gpio_value = 0;
				for(times = 0; times < 4; times++)
				{
					gpio_value += gpio_read_one_pin_value(gpio_hd, NULL);
					__msdelay(5);
				}
				if(!gpio_value)
				{
					printf("[sprite recovery] find the key\n");
					fdt_getprop_u32(working_fdt, nodeoffset, "mode", (uint32_t*)&sprite_mode);
					if(sprite_mode == ONEKEY_USB_RECOVERY_MODE)
					{
						gd->key_pressd_value = USB_RECOVERY_KEY_VALUE;
					}
					else if(sprite_mode == ONEKEY_SPRITE_RECOVERY_MODE)
					{
						gd->key_pressd_value = SPRITE_RECOVERY_KEY_VALUE;
						uboot_spare_head.boot_data.work_mode = WORK_MODE_SPRITE_RECOVERY;
					}
					else
					{
						printf("no option for one key recovery's mode (%d)\n", sprite_mode);
					}
				}
			}
		}
	}
	return 0;
}



#define    ANDROID_NULL_MODE            0
#define    ANDROID_FASTBOOT_MODE		1
#define    ANDROID_RECOVERY_MODE		2
#define    USER_SELECT_MODE 			3
#define	   ANDROID_SPRITE_MODE			4
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
#ifdef CONFIG_ARCH_HOMELET
static int detect_sprite_boot_mode(void)
{
	int key_value;
	key_value = gd->key_pressd_value;

	if (key_value == USB_RECOVERY_KEY_VALUE)
	{
		printf("[box recovery] set to one key usb recovery\n");
		return ANDROID_RECOVERY_MODE;
	}
	else if (key_value == SPRITE_RECOVERY_KEY_VALUE)
	{
		printf("[box recovery] set to one key sprite recovery\n");
		return ANDROID_SPRITE_MODE;
	}
	else if (key_value == IR_BOOT_RECOVERY_VALUE)
	{
		printf("[ir recovery] set to ir boot recovery\n");
		return ANDROID_RECOVERY_MODE;
	}

	return ANDROID_NULL_MODE;
}

#else
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
static int detect_other_boot_mode(void)
{
	int ret1, ret2;
	int key_high, key_low;
	int keyvalue;

	keyvalue = gd->key_pressd_value;
	printf("key %d\n", keyvalue);

	ret1 = script_parser_fetch("recovery_key", "key_max", &key_high, 1);
	ret2 = script_parser_fetch("recovery_key", "key_min", &key_low,  1);
	if((ret1) || (ret2))
	{
		printf("cant find rcvy value\n");
	}
	else
	{
		printf("recovery key high %d, low %d\n", key_high, key_low);
		if((keyvalue >= key_low) && (keyvalue <= key_high))
		{
			printf("key found, android recovery\n");

			return ANDROID_RECOVERY_MODE;
		}
	}
	ret1 = script_parser_fetch("fastboot_key", "key_max", &key_high, 1);
	ret2 = script_parser_fetch("fastboot_key", "key_min", &key_low, 1);
	if((ret1) || (ret2))
	{
		printf("cant find fstbt value\n");
	}
	else
	{
		printf("fastboot key high %d, low %d\n", key_high, key_low);
		if((keyvalue >= key_low) && (keyvalue <= key_high))
		{
			printf("key found, android fastboot\n");
			return ANDROID_FASTBOOT_MODE;
		}
	}

	return ANDROID_NULL_MODE;
}
#endif

int update_bootcmd(void)
{
	int   mode;
	int   pmu_value;
    int   need_flush =0;
	u32   misc_offset = 0;
	char  misc_args[2048];
	char  misc_fill[2048];
	char  boot_commond[256];
	static struct bootloader_message *misc_message;

	if(gd->force_shell)
	{
		char delaytime[8];
		sprintf(delaytime, "%d", 3);
		setenv("bootdelay", delaytime);
	}
	
	memset(boot_commond, 0x0, 128);
	strcpy(boot_commond, getenv("bootcmd"));
	printf("base bootcmd=%s\n", boot_commond);
	
	if((uboot_spare_head.boot_data.storage_type == STORAGE_SD) ||
		(uboot_spare_head.boot_data.storage_type == STORAGE_EMMC)||
		uboot_spare_head.boot_data.storage_type == STORAGE_EMMC3)
	{
		sunxi_str_replace(boot_commond, "setargs_nand", "setargs_mmc");
		printf("bootcmd set setargs_mmc\n");
	}
	else if(uboot_spare_head.boot_data.storage_type == STORAGE_NOR)
	{
		sunxi_str_replace(boot_commond, "setargs_nand", "setargs_nor");
	}
	else if(uboot_spare_head.boot_data.storage_type == STORAGE_NAND)
	{
		printf("bootcmd set setargs_nand\n");
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


#if defined(CONFIG_ARCH_HOMELET)
	mode = detect_sprite_boot_mode();
#else
	mode = detect_other_boot_mode();
#endif

	switch(mode)
	{
		case ANDROID_RECOVERY_MODE:
			strcpy(misc_message->command, "boot-recovery");
			break;
		case ANDROID_FASTBOOT_MODE:
			strcpy(misc_message->command, "bootloader");
			break;
		case ANDROID_SPRITE_MODE:
			strcpy(misc_message->command, "usb-recovery");
			break;
		case ANDROID_NULL_MODE:
			{
				pmu_value = axp_probe_pre_sys_mode();
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
			}
			break;
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
	else if(!strcmp(misc_message->command, "usb-recovery"))
	{
		puts("Recovery detected, will usb recovery\n");
		sunxi_str_replace(boot_commond, "boot_normal", "boot_recovery");
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
    if(need_flush == 1)
        sunxi_flash_flush();

	
	setenv("bootcmd", boot_commond);
	printf("to be run cmd=%s\n", boot_commond);

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

	printf("%s call\n",__func__);
	if(!nand_uboot_probe())  //nand_uboot_init(1)
	{
		printf("try nand successed \n");
		//disable_node("mmc2");
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
			//else if(card_num == 3)
			//	disable_node("mmc2");
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
	int nodeoffset = 0;
	uint storage_type = 0;


	storage_type = uboot_spare_head.boot_data.storage_type;

	//fix nand&sdmmc
	switch(storage_type)
	{
		case STORAGE_NAND:
		//	disable_node("mmc2");
			disable_node("mmc3");
			break;
		case STORAGE_EMMC:
			disable_node("nand0");
			disable_node("mmc3");
			break;
		case STORAGE_EMMC3:
			disable_node("nand0");
		//	disable_node("mmc2");
			break;
		case STORAGE_SD:
		{
			uint32_t dragonboard_test = 0;
			ret = script_parser_fetch("target", "dragonboard_test", (int *)&dragonboard_test, 1);
			if(dragonboard_test == 1)
			{
				dragonboard_handler(dtb_base);
			}
			else
			{
				disable_node("nand0");
				//disable_node("mmc2");
				disable_node("mmc3");
			}
		}
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

	//fix dram para
	uint32_t *dram_para = NULL;
	dram_para = (uint32_t *)uboot_spare_head.boot_data.dram_para;
	puts("update dtb dram start\n");
	nodeoffset = fdt_path_offset(dtb_base, "/dram");
	if(nodeoffset<0)
	{
		printf("## error: %s : %s\n", __func__,fdt_strerror(ret));
		return -1;
	}
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_clk", dram_para[0]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_type", dram_para[1]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_zq", dram_para[2]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_odt_en", dram_para[3]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_para1", dram_para[4]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_para2", dram_para[5]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_mr0", dram_para[6]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_mr1", dram_para[7]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_mr2", dram_para[8]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_mr3", dram_para[9]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_tpr0", dram_para[10]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_tpr1", dram_para[11]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_tpr2", dram_para[12]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_tpr3", dram_para[13]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_tpr4", dram_para[14]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_tpr5", dram_para[15]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_tpr6", dram_para[16]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_tpr7", dram_para[17]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_tpr8", dram_para[18]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_tpr9", dram_para[19]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_tpr10", dram_para[20]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_tpr11", dram_para[21]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_tpr12", dram_para[22]);
	fdt_setprop_u32(dtb_base,nodeoffset, "dram_tpr13", dram_para[23]);
	puts("update dtb dram  end\n");

	return 0;
}

int get_boot_work_mode(void)
{
	return uboot_spare_head.boot_data.work_mode;
}

int get_boot_storage_type(void)
{
	return uboot_spare_head.boot_data.storage_type;
}

u32 get_boot_dram_para_addr(void)
{
	return (u32)uboot_spare_head.boot_data.dram_para;
}

u32 get_boot_dram_para_size(void)
{
	return 32*4;
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
int write_usb_recovery_to_misc(void)
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
	strcpy(misc_message->command, "usb-recovery");
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
	key_value = gd->key_pressd_value;

	if (key_value == USB_RECOVERY_KEY_VALUE)
	{
		printf("[box recovery] set to one key usb recovery\n");
		write_usb_recovery_to_misc();
	}
	else if (key_value == IR_BOOT_RECOVERY_VALUE)
	{
		printf("[ir recovery] set to ir boot recovery\n");
		write_usb_recovery_to_misc();
	}
	else if (key_value == SPRITE_RECOVERY_KEY_VALUE)
	{
		printf("[box recovery] set to one key sprite recovery\n");
		//setenv("bootcmd", "sprite_recovery");
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
??? 从这里到 ???END 的内容可能被删除/插入过
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
	if( (get_boot_work_mode() == WORK_MODE_BOOT) &&
		(STORAGE_SD != get_boot_storage_type()))
	{
		if(get_boot_dram_update_flag())
		{
			printf("begin to update boot0 atfer ota\n");
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

static void sunxi_set_boot_disk(void)
{
    switch(get_boot_storage_type())
    {
      case STORAGE_SD:
        setenv("boot_disk", "0");
        setenv("boot_part", "0:1");
        break;

      case STORAGE_EMMC:
        setenv("boot_disk", "2");
        setenv("boot_part", "2:1");
        break;

      default:
        printf("storage not supported\n");
        break;
    }
}

int board_late_init(void)
{
	int ret  = 0;
	if(get_boot_work_mode() == WORK_MODE_BOOT)
	{
		sunxi_fastboot_init();
		sunxi_set_boot_disk();
		#ifdef  CONFIG_ARCH_HOMELET
		respond_physical_key_action();
		#endif
		#ifndef CONFIG_SUNXI_SPINOR
		update_dram_para_for_ota();
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

static int gpio_control(void)
{
	int ret;
	char *sub_key_value = NULL;
	char  tmp_gpio_name[16]={0};
	user_gpio_set_t	tmp_gpio_config;
	int count = 0;
	int nodeoffset = -1;

	nodeoffset = fdt_path_offset(working_fdt, "/soc/boot_init_gpio");
	if(nodeoffset < 0 || get_boot_work_mode() != WORK_MODE_BOOT)
	{
	    return 0;
	}

	ret = fdt_getprop_string(working_fdt, nodeoffset, "status", &sub_key_value);
	if ((ret < 0) || strncmp("okay", sub_key_value, 4))
	{
		return 0;
	}

	printf("boot_init_gpio used\n");
    while(1)
    {
        sprintf(tmp_gpio_name, "gpio%d", count);
        ret = fdt_get_one_gpio("/soc/boot_init_gpio", tmp_gpio_name, &tmp_gpio_config);
	    if(ret < 0)
	    {
		    break;
	    }

	    ret = gpio_request_early(&tmp_gpio_config, 1, 1);
	    if(ret < 0)
	    {
		    printf("[boot_init_gpio] %s gpio_request_early failed\n", tmp_gpio_name);
		    return -1;
	    }
	    count++;
	}
	return  0;
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
