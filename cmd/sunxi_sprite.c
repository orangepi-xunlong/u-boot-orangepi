/*
 *  * Copyright 2000-2009
 *   * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *    *
 *     * SPDX-License-Identifier:	GPL-2.0+
 *     */
#include <common.h>
#include <config.h>
#include <command.h>
#include <sunxi_board.h>
#include <sprite.h>
DECLARE_GLOBAL_DATA_PTR;


#ifdef CONFIG_SUNXI_AUTO_UPDATE
extern int sunxi_auto_update_main(void);
extern int sunxi_board_shutdown(void);
#endif


int do_sprite_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	__maybe_unused int ret;
	printf("sunxi work mode=0x%x\n", get_boot_work_mode());
	if(get_boot_work_mode() == WORK_MODE_USB_PRODUCT) {
		printf("run usb efex\n");
		if(sunxi_usb_dev_register(2))
		{
			printf("invalid usb device\n");
		}
		sunxi_usb_main_loop(2500);
	}
#ifdef CONFIG_SUNXI_SDMMC
	else if (get_boot_work_mode() == WORK_MODE_CARD_PRODUCT) {
		printf("run card sprite\n");
		sprite_led_init();
		ret = sunxi_card_sprite_main(0, NULL);
		sprite_led_exit(ret);
		return ret;
	}
#endif
#ifdef CONFIG_SUNXI_SPRITE_RECOVERY
	else if (get_boot_work_mode() == WORK_MODE_SPRITE_RECOVERY) {
		printf("run sprite recovery\n");
		sprite_led_init();
		ret = sprite_form_sysrecovery();
		sprite_led_exit(ret);
		return ret;
	}
#endif
#ifdef CONFIG_SUNXI_AUTO_UPDATE
	else if (get_boot_work_mode() == WORK_MODE_CARD_UPDATE ||
		get_boot_work_mode() == WORK_MODE_UDISK_UPDATE) {
		printf("run auto update\n");
		sprite_led_init();
		ret = run_command("auto_update_check 1", 0);
		sprite_led_exit(ret);
		if (!ret) {
			printf("update finish,going to poweroff the system...\n");
			sunxi_update_subsequent_processing(SUNXI_UPDATE_NEXT_ACTION_SHUTDOWN);
		}
		return ret;
	}
#endif
	else if (get_boot_work_mode() == WORK_MODE_USB_DEBUG) {

		printf("run usb debug\n");
		if (sunxi_usb_dev_register(2)) {
			printf("sunxi usb test: invalid usb device\n");
		}

		/*disable dcache for modify dram through usb without flush*/
		printf("disable D cache\n");
		dcache_disable();
		sunxi_usb_main_loop(0);
	} else {
		printf("others\n");
	}

	return 0;
}

U_BOOT_CMD(
	sprite_test, 2, 0, do_sprite_test,
	"do a sprite test",
	"NULL"
);


