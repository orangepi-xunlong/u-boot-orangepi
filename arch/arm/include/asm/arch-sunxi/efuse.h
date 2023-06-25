/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2018
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 */

#ifndef _SUNXI_EFUSE_H
#define _SUNXI_EFUSE_H

#include <linux/types.h>


#if defined(CONFIG_MACH_SUN50IW3)
#include <asm/arch/sid_sun50iw3.h>
#elif defined(CONFIG_MACH_SUN8IW16)
#include <asm/arch/sid_sun8iw16.h>
#elif defined(CONFIG_MACH_SUN8IW19)
#include <asm/arch/sid_sun8iw19.h>
#elif defined(CONFIG_MACH_SUN9I)
#include <asm/arch/sid_sun9i.h>
#elif defined(CONFIG_MACH_SUN8IW18)
#include <asm/arch/sid_sun8iw18.h>
#elif defined(CONFIG_MACH_SUN50IW9)
#include <asm/arch/sid_sun50iw9.h>
#elif defined(CONFIG_MACH_SUN50IW10)
#include <asm/arch/sid_sun50iw10.h>
#elif defined(CONFIG_MACH_SUN50IW11)
#include <asm/arch/sid_sun50iw11.h>
#elif defined(CONFIG_MACH_SUN50IW12)
#include <asm/arch/sid_sun50iw12.h>
#elif defined(CONFIG_MACH_SUN8IW15)
#include <asm/arch/sid_sun8iw15.h>
#elif defined(CONFIG_MACH_SUN8IW7)
#include <asm/arch/sid_sun8iw7.h>
#elif defined(CONFIG_MACH_SUN20IW1)
#include <asm/arch/sid_sun20iw1.h>
#elif defined(CONFIG_MACH_SUN8IW20)
#include <asm/arch/sid_sun8iw20.h>
#elif defined(CONFIG_MACH_SUN8IW21)
#include <asm/arch/sid_sun8iw21.h>
#elif defined(CONFIG_MACH_SUN50IW5)
#include <asm/arch/sid_sun50iw5.h>
#elif defined(CONFIG_MACH_SUN8IW11)
#include <asm/arch/sid_sun8iw11.h>
#elif defined(CONFIG_MACH_SUN55IW3)
#include <asm/arch/sid_sun55iw3.h>
#else
#error "platform not support"
#endif

#ifndef __ASSEMBLY__
void sid_program_key(uint key_index, uint key_value);
uint sid_read_key(uint key_index);
int sunxi_efuse_get_security_status(void);
int sunxi_efuse_get_rotpk_status(void);
int sid_probe_security_mode(void);
int sid_set_security_mode(void);
int  sid_get_security_status(void);
int sunxi_efuse_get_soc_ver(void);
int sunxi_efuse_verify_rotpk(u8 *hash);
#endif

#endif /* _SUNXI_SID_H */
