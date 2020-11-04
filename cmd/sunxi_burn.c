/*
 * (C) Copyright 2018 allwinnertech  <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <console.h>
#include <command.h>
#include <securestorage.h>
#include <sunxi_board.h>

extern int sunxi_usb_dev_register(uint dev_name);
extern  void sunxi_usb_main_loop(int delaytime);
extern  int sunxi_usb_extern_loop(void);
extern  int sunxi_usb_init(int delaytime);
extern  int sunxi_usb_exit(void);

DECLARE_GLOBAL_DATA_PTR;

extern volatile int sunxi_usb_burn_from_boot_handshake, sunxi_usb_burn_from_boot_init, sunxi_usb_burn_from_boot_setup;


int do_burn_from_boot(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int    ret;
	ulong begin_time= 0, over_time = 0;

	pr_force("usb burn from boot\n");
	if(sunxi_usb_dev_register(4) < 0)
	{
		pr_err("usb burn fail: not support burn private data\n");
		return -1;
	}

	if(sunxi_usb_init(0))
	{
		pr_err("%s usb init fail\n", __func__);
		sunxi_usb_exit();
		return 0;
	}

	tick_printf("usb prepare ok\n");
	begin_time = get_timer(0);
	over_time = 800; /* 800ms */
	while(1)
	{
		if(sunxi_usb_burn_from_boot_init)
		{
			pr_force("usb sof ok\n");
			break;
		}
		if(get_timer(begin_time) > over_time)
		{
			tick_printf("overtime\n");
			sunxi_usb_exit();
			tick_printf("%s usb : no usb exist\n", __func__);

			return 0;
		}
	}
	tick_printf("usb probe ok\n");
	tick_printf("usb setup ok\n");

	begin_time = get_timer(0);
	over_time = 3000; /* 3000ms */
	while(1)
	{
		ret = sunxi_usb_extern_loop();
		if(ret)
		{
			break;
		}
		if(!sunxi_usb_burn_from_boot_handshake)
		{
			if(get_timer(begin_time) > over_time)
			{
				sunxi_usb_exit();
				tick_printf("%s usb : have no handshake\n", __func__);
				return 0;
			}
		}
		if(ctrlc())
		{
			ret = SUNXI_UPDATE_NEXT_ACTION_NORMAL;
			break;
		}
	}
	tick_printf("exit usb burn from boot\n");
	sunxi_usb_exit();
	sunxi_update_subsequent_processing(ret);

	return 0;
}


U_BOOT_CMD(
	uburn, CONFIG_SYS_MAXARGS, 1, do_burn_from_boot,
	"do a burn from boot",
	"pburn [mode]"
	"NULL"
);


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
int do_read_from_boot(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#ifdef CONFIG_SUNXI_SECURE_STORAGE
	if(argc == 1)
	{
		return sunxi_secure_storage_list();
	}
	if(argc == 2)
	{
		char buffer[4096];
		int ret, data_len;

		memset(buffer, 0, 4096);
		ret = sunxi_secure_storage_init();
		if(ret < 0)
		{
			pr_err("%s secure storage init err\n", __func__);

			return -1;
		}
		ret = sunxi_secure_object_read(argv[1], buffer, 4096, &data_len);
		if(ret < 0)
		{
			pr_err("private data %s is not exist\n", argv[1]);

			return -1;
		}
		pr_force("private data:\n");
		sunxi_dump(buffer, strlen((const char *)buffer));

		return 0;
	}
#endif
	return -1;
}

U_BOOT_CMD(
	pbread, CONFIG_SYS_MAXARGS, 1, do_read_from_boot,
	"read data from private data",
	"pread [name]"
	"NULL"
);

