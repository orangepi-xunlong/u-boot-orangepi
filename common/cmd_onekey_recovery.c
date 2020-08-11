/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Charles <yanjianbo@allwinnertech.com>
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
#include <sprite.h>

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
int do_onekey_sprite_recovery (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;
	if (argc == 1) {
		uboot_spare_head.boot_data.work_mode = WORK_MODE_SPRITE_RECOVERY;
		sprite_led_init();
		ret = sprite_form_sysrecovery();
		sprite_led_exit(ret);
		return ret;
	}
	return ret;
}

U_BOOT_CMD(
	sprite_recovery, 1,	1, do_onekey_sprite_recovery,
	"one key sprite recovery\n",
	"<commond>\n"
);
