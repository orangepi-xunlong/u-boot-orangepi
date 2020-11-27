/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <command.h>
#include <power/sunxi/pmu.h>
#include <power/sunxi/power.h>
#include <sys_partition.h>
#include <sys_config.h>
#include <sys_config_old.h>
#include <securestorage.h>
#include <fdt_support.h>
#include <smc.h>
#include <sunxi_board.h>

DECLARE_GLOBAL_DATA_PTR;

extern int do_burn_from_boot(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

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
//#define  SUNXI_SECURESTORAGE_TEST_ERASE

int sunxi_keydata_burn_by_usb(void)
{
	char buffer[512];
	__maybe_unused int  data_len;
	__maybe_unused int  ret = 0;
	__maybe_unused uint burn_private_start;
	__maybe_unused uint burn_private_len;

	int workmode = uboot_spare_head.boot_data.work_mode;
	int if_need_burn_key=0;
//	int nodeoffset;

	//PMU_SUPPLY_DCDC2 is for cpua
//	nodeoffset =  fdt_path_offset(working_fdt,FDT_PATH_TARGET);
//	if(nodeoffset >=0)
//	{
//		//ret = script_parser_fetch("target", "burn_key", &if_need_burn_key, 1);
//		fdt_getprop_u32(working_fdt, nodeoffset, "burn_key", &if_need_burn_key);
//	}
	ret = script_parser_fetch("target", "burn_key", &if_need_burn_key, 1);

	if(if_need_burn_key != 1)
	{
		return 0;
	}

	if(workmode != WORK_MODE_BOOT)
	{
		pr_error("out of usb burn from boot: not boot mode\n");
		return 0;
	}
	if (gd->vbus_status != SUNXI_VBUS_EXIST)
	{
		pr_error("vbus not exist,without usb\n");
		return 0;
	}

	if(gd->power_step_level == BATTERY_RATIO_TOO_LOW_WITH_DCIN)
	{
		pr_error("out of usb burn from boot: not enough energy\n");
		return 0;
	}
	memset(buffer, 0, 512);
#ifdef CONFIG_SUNXI_SECURE_STORAGE
	if(sunxi_secure_storage_init())
#endif
	{
		pr_error("sunxi secure storage is not supported\n");
#ifdef CONFIG_SUNXI_PRIVATE_KEY
		burn_private_start = sunxi_partition_get_offset_byname("private");
		burn_private_len   = sunxi_partition_get_size_byname("private");

		if(!burn_private_start)
		{
			pr_msg("private partition is not exist\n");
			return -1;
		}
		else
		{
			ret = sunxi_flash_read(burn_private_start + burn_private_len - (8192+512)/512, 1, buffer);
			if(ret != 1)
			{
				pr_error("cant read private part\n");
				return -1;
			}
			if(!strcmp(buffer, "key_burned"))
			{
				pr_msg("find key burned flag\n");
				return 0;
			}
		}
#endif
	}
#ifdef CONFIG_SUNXI_SECURE_STORAGE
	else
	{
#ifndef SUNXI_SECURESTORAGE_TEST_ERASE
		ret = sunxi_secure_object_read("key_burned_flag", buffer, 512, &data_len);
		if(ret)
		{
			pr_error("sunxi secure storage has no flag\n");
		}
		else
		{
			if(!strcmp(buffer, "key_burned"))
			{
				pr_msg("find key burned flag\n");
				return 0;
			}
			pr_msg("do not find key burned flag\n");
		}
#else
		if(!sunxi_secure_storage_erase("key_burned_flag"))
			sunxi_secure_storage_exit();

		return 0;
#endif
	}
#endif
	return do_burn_from_boot(NULL, 0, 0, NULL);
}


int do_efuse_read(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *efuse_name;
	char buffer[32];
	int  ret;

	if(argc != 2)
	{
		pr_error("the efuse item name is empty\n");

		return -1;
	}
	efuse_name = argv[1];
	pr_msg("try to read %s\n", efuse_name);
	memset(buffer, 0, 32);
	pr_msg("buffer addr=0x%x\n", (u32)buffer);
	ret = arm_svc_efuse_read(efuse_name, buffer);
	if(ret)
	{
		pr_error("read efuse key [%s] failed\n", efuse_name);
	}
	else
	{
		pr_msg("read efuse key [%s] successed\n", efuse_name);
		sunxi_dump(buffer, 32);
	}

	return 0;
}

U_BOOT_CMD(
	efuse_read, 3, 0, do_efuse_read,
	"read efuse key",
	"usage: efuse_read efusename"
);















