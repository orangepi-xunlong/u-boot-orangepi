/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#include <common.h>
#include <spare_head.h>
#include <private_uboot.h>
#include <boot_type.h>
#include <sys_partition.h>
#include <sys_config.h>
#include <asm/arch/key.h>
#include <power/sunxi/pmu.h>
#include "debug_mode.h"
#include <fdt_support.h>
#include <sys_config_old.h>

DECLARE_GLOBAL_DATA_PTR;

enum _USER_MODE
{
	NORMAL_MODE = 0,
	FEL_MODE ,
	DEBUG_MODE ,
};


#ifdef CONFIG_SUNXI_KEY_SUPPORT
#define   KEY_DELAY_MAX          (8)
#define   KEY_DELAY_EACH_TIME    (40)
#define   KEY_MAX_COUNT_GO_ON    ((KEY_DELAY_MAX * 1000)/(KEY_DELAY_EACH_TIME))


/*
* fun      :       check_fel_key
* return  :       0: normal  1:enter fel
*/
static int check_config_fel_key(uint32_t key_value)
{
	uint32_t fel_key_max=0, fel_key_min=0;
	int ret = 0;

	if(!key_value)
	{
		return NORMAL_MODE;
	}

	ret = script_parser_fetch("fel_key", "fel_key_max", (int *)&fel_key_max, 1);
	if(ret < 0)
	{
		debug("%s:get fel_key_max error\n",__func__);
		return NORMAL_MODE;
	}

	ret = script_parser_fetch("fel_key", "fel_key_min", (int *)&fel_key_min, 1);
	if(ret < 0)
	{
		debug("%s:get fel_key_min error\n",__func__);
		return NORMAL_MODE;
	}

	if((key_value <= fel_key_max) && (key_value >= fel_key_min))
	{
		printf("fel key detected\n");
		//jum to fel
		return FEL_MODE;
	}
	printf("key value = %d, fel_key = [%d,%d]\n", key_value, fel_key_min, fel_key_max);
	return NORMAL_MODE;

}


///*
//* fun      :       check_user_mode
//* return  :       0: normal  1:enter fel
//*note     :      press an  key and not loosen:
//                     1) enter debug when  usb plug in  up to 3 times
//*                    2) enter fel,if press power key  up to 3 times
//*/
//static int  check_user_mode(void)
//{
//	int time_tick = 0;
//	int power_plug_count = 0;
//	int new_power_status = 0;
//	int old_power_status = 0;
//
//	int count = 0;
//	int power_key = 0;
//
//	old_power_status = axp_probe_power_source();
//	//add by guoyingyang
//	while(sunxi_key_read() > 0) //press key and not loosen
//	{
//		time_tick++;
//
//		//detect usb plug in&out  for debug mode
//		new_power_status = axp_probe_power_source();
//		if(new_power_status != old_power_status)
//		{
//			power_plug_count++;
//			old_power_status = new_power_status;
//		}
//		if(power_plug_count == 3)
//		{
//			debug_mode_set();
//			return DEBUG_MODE;
//		}
//
//		//detect power key status for fel mode
//		power_key = axp_probe_key();
//		if(power_key > 0)
//		{
//			count ++;
//		}
//		if(count == 3)
//		{
//			printf("you can loosen the key to update now\n");
//			//jump to fel
//			return FEL_MODE;
//		}
//
//
//		__msdelay(KEY_DELAY_EACH_TIME);
//		if(time_tick > KEY_MAX_COUNT_GO_ON)
//		{
//			printf("time out\n");
//			break;
//		}
//	}
//	printf("key not pressed anymore\n");
//	return NORMAL_MODE;
//}


/*
 * Read a key  when user pressed a  key
 * return :     0:normal  -1: jump to fel
 */
//we have pass the uart input from boot0, then we do not need to detect it
int check_update_key(void)
{
	int key_value = 0;

	if(uboot_spare_head.boot_data.work_mode != WORK_MODE_BOOT)
	{
	    return 0;
	}

	key_value = uboot_spare_head.boot_ext[0].data[2];
	if(check_config_fel_key(key_value) == FEL_MODE)
	{
		return -1;
	}

	return 0;
}
#else
int check_update_key(void)
{
	return 0;
}

#endif

/*
 * Read a char from serial when call board_init_f
 *
 */

//do not need it anylonger, we pass the uart input from boot0
#if 0
int check_uart_input(void)
{
	extern int do_key_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
	int c = 0;
	if(uboot_spare_head.boot_data.work_mode != WORK_MODE_BOOT)
	{
	    return 0;
	}
	if(tstc())
	{
		c = getc();
		printf("0x%x\n", c);
	}
	else
	{
		puts("no uart input\n");
	}

	if(c == '3')
	{
#ifdef SUNXI_KEY_SUPPORT
		sunxi_key_init();
		do_key_test(NULL, 0, 1, NULL);
#endif
	}
	else if(c == 's')		//shell mode
	{
		gd->force_shell = 1;
        gd->debug_mode = 1;
	}
	return 0;
}
#endif

