/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Young <guoyingyang@allwinnertech.com>
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

#include "common.h"
#include "asm/armv7.h"
#include <private_toc.h>
#include <asm/arch/uart.h>
#include <boot0_helper.h>

extern sbrom_toc0_config_t *toc0_config ;
extern int debug_enable;
/*
************************************************************************************************************
*
*                                             function
*
*    name          :set_debugmode_flag
*
*    parmeters     :void
*
*    return        :
*
*    note          :if BT0_head.prvt_head.debug_mode_off = 1,do not print any message to uart ;
*                   if you press 'd' button,turn on printf
*
************************************************************************************************************
*/

int set_debugmode_flag(void)
{
	char c = get_uart_input();
	if(c  == 'd')
	{
		debug_enable = LOG_LEVEL_INFO;
		return c;
	}

	if(toc0_config->debug_mode)
		debug_enable = toc0_config->debug_mode;
	else
		debug_enable = 0;

	return c;
}

