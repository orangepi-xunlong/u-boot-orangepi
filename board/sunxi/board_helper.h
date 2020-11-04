/* SPDX-License-Identifier: GPL-2.0+
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 * Some init for sunxi platform.
 */

#ifndef _BOARD_HELPER_H_
#define _BOARD_HELPER_H_


void sunxi_dump(void *addr, unsigned int size);
int  sunxi_update_fdt_para_for_kernel(void);
int  sunxi_update_bootcmd(void);
int  sunxi_update_partinfo(void);
int sunxi_update_rotpk_info(void);

#endif
