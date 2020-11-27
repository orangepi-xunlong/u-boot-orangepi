#ifndef __NAND_UBOOT_PLATFORM_H__
#define __NAND_UBOOT_PLATFORM_H__

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
#include <malloc.h>
#include <stdarg.h>
#include <asm/arch/dma.h>
#include <sys_config.h>
#include <smc.h>
#include <fdt_support.h>
#include <sys_config_old.h>

//CONFIG_ARCH_SUN8IW1P1
//CONFIG_ARCH_SUN8IW3P1
//CONFIG_ARCH_SUN9IW1P1
//CONFIG_ARCH_SUN50I
//CONFIG_ARCH_SUN8IW7P1

#if defined CONFIG_ARCH_SUN50IW2P1      //h5
#define  PLATFORM_NO            0
#define  PLATFORM_STRINGS    "allwinner,sun50iw2-nand"
#define  PLATFORM_BOOT0_ACESS_PAGE_TABLE_MODE    1
#define  PLATFORM_CLASS         0

#elif defined CONFIG_ARCH_SUN8IW5P1    //A33
#define  PLATFORM_NO            1
#define  PLATFORM_STRINGS    "allwinner,sun8iw5-nand"
#define  PLATFORM_BOOT0_ACESS_PAGE_TABLE_MODE     0
#define  PLATFORM_CLASS         1

#elif defined CONFIG_ARCH_SUN8IW6P1     //H8vr
#define  PLATFORM_NO            2
#define  PLATFORM_STRINGS     "allwinner,sun8iw6-nand"
#define  PLATFORM_BOOT0_ACESS_PAGE_TABLE_MODE     1
#define  PLATFORM_CLASS         0

#elif defined CONFIG_ARCH_SUN8IW10P1     //B100
#define  PLATFORM_NO            3
#define  PLATFORM_STRINGS    "allwinner,sun8iw10-nand"
#define  PLATFORM_BOOT0_ACESS_PAGE_TABLE_MODE     1
#define  PLATFORM_CLASS         1

#elif defined CONFIG_ARCH_SUN8IW11P1     //v40
#define  PLATFORM_NO            4
#define  PLATFORM_STRINGS    "allwinner,sun8iw11-nand"
#define  PLATFORM_BOOT0_ACESS_PAGE_TABLE_MODE     1
#define  PLATFORM_CLASS         1


#else
#error "please select a platform\n"
#endif




extern int sunxi_get_securemode(void);







#endif