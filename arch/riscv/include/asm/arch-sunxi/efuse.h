/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2018
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 */

#ifndef _SUNXI_EFUSE_H
#define _SUNXI_EFUSE_H

#include <linux/types.h>


#if defined(CONFIG_MACH_SUN20IW1)
#include <asm/arch/sid_sun20iw1.h>
#elif defined(CONFIG_MACH_SUN8IW20)
#include <asm/arch/sid_sun8iw20.h>
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
int sunxi_efuse_verify_rotpk(u8 *hash);
#endif

#endif /* _SUNXI_SID_H */
