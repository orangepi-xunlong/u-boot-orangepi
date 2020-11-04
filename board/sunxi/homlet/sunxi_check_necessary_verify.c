/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * weidonghui <weidonghui@allwinnertech.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <linux/ctype.h>
#include <linux/types.h>
#include <asm/global_data.h>
#include <linux/libfdt.h>
#include <fdt_support.h>
#include <sys_config.h>
#include <sunxi_board.h>
#include <asm/arch/efuse.h>

#ifdef CUSTOMER_DEBUG
#define soft_protect_info(fmt...) tick_printf("[soft_protect][info]: " fmt)
#else
#define soft_protect_info(fmt...)
#endif



u32 sunxi_handle_customer_reserved_id(u32 customer_reserved_id)
{
	return arm_svc_customer_encrypt(customer_reserved_id);
}


int sunxi_check_customer_reserved_id(void)
{
	int customer_reserved_id = 0;
	u32 encrypt_customer_reserved_id =
		(readl(SUNXI_SID_BASE + 0x200 +
		EFUSE_OEM_PROGRAM + 0x10) >> 16) & 0xffff;

#ifdef CONFIG_SUNXI_CUSTOMER_RESERVED_ID
	customer_reserved_id = CONFIG_SUNXI_CUSTOMER_RESERVED_ID;
#else
	if (script_parser_fetch(FDT_PATH_PLATFORM,
		"customer_reserved_id", &customer_reserved_id, -1))
		return 0;
#endif
	soft_protect_info("customer_reserved_id:0x%x\n", customer_reserved_id);
	soft_protect_info("encrypt_customer_reserved_id:0x%x\n", encrypt_customer_reserved_id);
	soft_protect_info("decrypt_xor:0x%x\n", sunxi_handle_customer_reserved_id(customer_reserved_id));

	if (sunxi_handle_customer_reserved_id(customer_reserved_id) !=
		encrypt_customer_reserved_id) {
		tick_printf("illegal customer_reserved_id!!!\n");
		sunxi_board_run_fel();
	}
	return 0;
}

