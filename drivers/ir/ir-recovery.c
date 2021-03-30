/*
 * (C) Copyright 2007-2016
 * Allwinnertech Technology Co., Ltd <www.allwinnertech.com>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
 
#include <common.h>
#include <asm/arch/platform.h>
#include <asm/arch/intc.h>
#include <sys_config.h>
#include <fdt_support.h>

#include "sunxi-ir.h"

struct sunxi_ir_data ir_data;

static int ir_boot_recovery_mode = 0;
extern struct ir_raw_buffer sunxi_ir_raw;

static int ir_sys_cfg(void)
{
	int nodeoffset, ret, i;
	char *sub_key_value = NULL;
	char addr_name[32];

	if(uboot_spare_head.boot_data.work_mode != WORK_MODE_BOOT)
		return -1;

	nodeoffset = fdt_path_offset(working_fdt, "/soc/ir_boot_recovery");
	if (nodeoffset < 0) {
		return -1;
	}

	ret = fdt_getprop_string(working_fdt, nodeoffset, "status", &sub_key_value);
	if((ret < 0) || (strcmp(sub_key_value, "okay"))) {
		printf("ir boot recovery not used\n");
		return -1;
	}

	for (i = 0; i < MAX_IR_ADDR_NUM; i++) {
		sprintf(addr_name, "ir_recovery_key_code%d", i);
		if (fdt_getprop_u32(working_fdt, nodeoffset, addr_name, (uint32_t*)&(ir_data.ir_recoverykey[i])) <= 0) {
			printf("node %s get failed!\n", addr_name);
			break;
		}
	}
	ir_data.ir_addr_cnt = i;

	for (i = 0; i < ir_data.ir_addr_cnt; i++)
	{
		sprintf(addr_name, "ir_addr_code%d", i);
		if (fdt_getprop_u32(working_fdt, nodeoffset, addr_name, (uint32_t*)&(ir_data.ir_addr[i])) <= 0)
			break;
	}
	return 0;
}

static int ir_code_valid(struct sunxi_ir_data *ir_data, u32 code)
{
	u32 i, tmp;

	tmp = (code >> 8) & 0xffff;
	for (i = 0; i < ir_data->ir_addr_cnt; i++) {
		if (ir_data->ir_addr[i] == tmp) {
			if ((code & 0xff) == ir_data->ir_recoverykey[i]) {
				printf("ir boot recovery detect\n");
				return 0;
			}

			printf("ir boot recovery not detect, code=0x%x, coded=0x%x\n", code, ir_data->ir_recoverykey[i]);
			return -1;
		}
	}

	printf("ir boot recovery not detect\n");
	return -1;
}

int ir_boot_recovery_mode_detect(void)
{
	u32 code;
	if (ir_boot_recovery_mode)
	{
		code = sunxi_ir_raw.scancode;

		if((code != 0) && (!ir_code_valid(&ir_data, code)))
			return 0;
	}

	return -1;
}

int check_ir_boot_recovery(void)
{
	if(ir_sys_cfg())
		return -1;

	if (ir_setup())
		return -1;

	ir_boot_recovery_mode = 1;
	__msdelay(1500);

	ir_disable();
	return 0;
}

