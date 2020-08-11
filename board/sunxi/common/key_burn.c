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
#include <efuse_map.h>

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

	//PMU_SUPPLY_DCDC2 is for cpua

	ret = script_parser_fetch("target", "burn_key", &if_need_burn_key, 1);

	if(if_need_burn_key != 1)
	{
		return 0;
	}

	if(workmode != WORK_MODE_BOOT)
	{
		printf("out of usb burn from boot: not boot mode\n");
		return 0;
	}
	if(gd->vbus_status != SUNXI_VBUS_PC)
	{
		printf("out of usb burn from boot: without usb\n");
		return 0;
	}

	if(gd->power_step_level == BATTERY_RATIO_TOO_LOW_WITH_DCIN)
	{
		printf("out of usb burn from boot: not enough energy\n");
		return 0;
	}
	memset(buffer, 0, 512);
#ifdef CONFIG_SUNXI_SECURE_STORAGE
	if(sunxi_secure_storage_init())
#endif
	{
		printf("sunxi secure storage is not supported\n");
#ifdef CONFIG_SUNXI_PRIVATE_KEY
		burn_private_start = sunxi_partition_get_offset_byname("private");
		burn_private_len   = sunxi_partition_get_size_byname("private");

		if(!burn_private_start)
		{
			printf("private partition is not exist\n");
			return -1;
		}
		else
		{
			ret = sunxi_flash_read(burn_private_start + burn_private_len - (8192+512)/512, 1, buffer);
			if(ret != 1)
			{
				printf("cant read private part\n");
				return -1;
			}
			if(!strcmp(buffer, "key_burned"))
			{
				printf("find key burned flag\n");
				return 0;
			}
		}
#else
	     return -1;
#endif
	}
#ifdef CONFIG_SUNXI_SECURE_STORAGE
	else
	{
#ifndef SUNXI_SECURESTORAGE_TEST_ERASE
		ret = sunxi_secure_object_read("key_burned_flag", buffer, 512, &data_len);
		if(ret)
		{
			printf("sunxi secure storage has no flag\n");
		}
		else
		{
			if(!strcmp(buffer, "key_burned"))
			{
				printf("find key burned flag\n");
				return 0;
			}
			printf("do not find key burned flag\n");
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

#ifdef CONFIG_SUNXI_EFUSE_TEST
int do_efuse_read(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *efuse_name;
	char buffer[32];
	int  ret;

	if(argc != 2)
	{
		printf("the efuse item name is empty\n");

		return -1;
	}
	efuse_name = argv[1];
	printf("try to read %s\n", efuse_name);
	memset(buffer, 0, 32);
	printf("buffer addr=0x%x\n", (u32)buffer);
	ret = arm_svc_efuse_read(efuse_name, buffer);
	if(ret)
	{
		printf("read efuse key [%s] failed\n", efuse_name);
	}
	else
	{
		printf("read efuse key [%s] successed\n", efuse_name);
		sunxi_dump(buffer, 32);
	}

	return 0;
}

U_BOOT_CMD(
	efuse_read, 3, 0, do_efuse_read,
	"read efuse key",
	"usage: efuse_read efusename"
);


#define EFUSE_TEST_BUF_LEN 128
u8 g_tempbuffer[EFUSE_TEST_BUF_LEN];
extern void sunxi_efuse_dump(void);
static void usage(void)
{
	printf("do_efuse_entry name 1/2/3 [0x00345678_abcdefdd_...] \n");
	printf("read ==1  write==2  3==dump info necessary\n");
}

static char ___map(char c)
{
	printf("c is  %c\n",c);
	if((c >='0') && (c <='9'))
		return (c - '0');
	if((c >='a') && (c <='z'))
		return (c-'a' +10);
	if((c >='A') && (c <='Z'))
		return (c-'A' +10);

	return 0;
}
static  unsigned char convert(char* src)
{
	unsigned char  temp ;
	temp = (___map(*src))*16 + ___map(*(src+1));
	printf("temp 0x%x",temp);
	return temp;
}
static int parse_wt_data(u8* dst, u32* dst_len,char* src,int len)
{
	*dst_len = 0;
	int i = 0 ,j;
	if((len+1) %9)
	{
		printf("len 0x%x\n",len);
		return -1;
	}
	while((*src)&&(len>0))
	{
		dst[0] = convert(src);
		dst[1] = convert(src+2);
		dst[2] = convert(src+4);
		dst[3] = convert(src+6);
		for(j = 0;j<4;j++)
		printf("dst[%d] 0x%x\n",i++,dst[j]);
		src += 9;
		len -= 9;
		*dst_len += 4;
		dst += 4;
	}
	return 	*dst_len;
}
int do_efuse_entry(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *efuse_name;
	char* wt_data = NULL;

	int  ret = 0;
	char* cmd = NULL;
	if(argc < 3)
	{
		usage();
		return -1;
	}
	efuse_name = argv[1];
	cmd = argv[2];
	wt_data = argv[3] + 2;

	printf("efuse_name is  %s cmd is %s wt_data is %s\n",
	efuse_name,cmd,wt_data?wt_data:"NULL");
	memset(g_tempbuffer, 0, EFUSE_TEST_BUF_LEN);

	switch(*cmd)
	{
		/*read RO + RW + PRIVATE + NACCESS*/
		case '1':
			sunxi_efuse_read(efuse_name,g_tempbuffer,&ret);
			if((ret > 0) && (ret <= EFUSE_TEST_BUF_LEN))
			{
				printf("read efuse key [%s][len:%dByte] successed\n", efuse_name,ret);
				sunxi_dump(g_tempbuffer, EFUSE_TEST_BUF_LEN);
			}
			else
			{
				printf("read efuse key [%s] failed err is %d\n",efuse_name,ret);
			}
			break;
		case '2':
		/*write RO + RW + PRIVATE + NACCESS*/
			if(strlen(argv[3]) <= 2)
			{
				printf("%s %d input write data wrong\n",
				__FUNCTION__,__LINE__);
				return -1;
			}
			efuse_key_info_t key_buf;
			memset(&key_buf, 0,sizeof(key_buf));
			memcpy(key_buf.name,efuse_name,
			strlen(efuse_name) < SUNXI_KEY_NAME_LEN ?strlen(efuse_name):SUNXI_KEY_NAME_LEN);
			key_buf.key_data = g_tempbuffer;
			ret = parse_wt_data(key_buf.key_data,&key_buf.len,wt_data,(strlen(argv[3]) - 2));
			if(ret <= 0)
			{
				printf("%s %d wt data err\n",__FUNCTION__,__LINE__);
				return -1;
			}
			printf("%s %d len 0x%x\nkey_buf data is:\n",__FUNCTION__,__LINE__,key_buf.len);
			int i = 0;
			for(i = 0;i<key_buf.len;i++)
			printf("0x%x ",key_buf.key_data[i]);
			printf("\n");
			ret = sunxi_efuse_write(&key_buf);
			if(ret == 0)
			{
				printf("wt efuse key [%s] successed\n", efuse_name);
			}
			else
			{
				printf("wt efuse key [%s] failed err is %d\n",efuse_name,ret);
			}
			break;
		/*dump all info of keymap*/
		case '3':
			sunxi_efuse_dump();
			break;
		default:
			usage();
			return -1;
			break;
	}
	return 0;
}

U_BOOT_CMD(
	efuse_entry, 4, 0, do_efuse_entry,
	"efuse test entry",
	"usage: efuse_entry name 1/2/3 [0x00345678_abcdefdd_...] "
);
#endif

