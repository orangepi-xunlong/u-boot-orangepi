// SPDX-License-Identifier: GPL-2.0+
/*
 * drivers/riscv/common/riscv_fdt.c
 *
 * Copyright (c) 2007-2025 Allwinnertech Co., Ltd.
 * Author: wujiayi <wujiayi@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details
 *
 */
#include <common.h>
#include <fdt_support.h>
#include <common.h>
#include <malloc.h>
#include <asm/io.h>
#include <sys_config.h>
#include <fdt_support.h>

#include "imgdts.h"
#include "riscv_ic.h"
#include "riscv_fdt.h"

#define UART_NUM (6)

int match_compatible(const char *path,
		     const char *compatible_str,
		     int *nodeoffset)
{
	int nodeoff = 0;
	char *pstr = NULL;
	int pstr_num = 0;

	RISCV_DEBUG("riscv:---- name [%s] msg -----\n", path);
	nodeoff = fdt_path_offset(working_fdt, path);
	if (nodeoff < 0) {
		RISCV_DEBUG("riscv:find [%s] node err\n", path);
		return -1;
	}

	pstr = (char *)fdt_getprop(working_fdt, nodeoff, "compatible", &pstr_num);
	if (pstr != NULL) {
		/* match string of compatible  */
		if (strcmp(pstr, compatible_str) == 0) {
			*nodeoffset = nodeoff;
			return 0;

		} else {
			RISCV_DEBUG("riscv:expect compatible:[%s], but use compatible:[%s]\n",\
						compatible_str, pstr);
			return -1;
		}

	} else {
		RISCV_DEBUG("riscv:find compatible err\n");
		return -1;
	}

	return -1;
}

int match_status(int nodeoffset)
{
	char *string_val = NULL;
	int ret = -1;

	fdt_getprop_string(working_fdt,
				nodeoffset,
				"status",
				&string_val);

	if (strcmp("okay", string_val) == 0)
		ret = 0;

	return ret;
}

int match_pinctrl_names(int nodeoffset)
{
	int ret = fdt_stringlist_search(working_fdt,
					 nodeoffset,
					"pinctrl-names",
					"default");
	return ret;
}

int dts_riscv_status(struct dts_msg_t *pmsg, u32 riscv_id)
{
	const char *compatible_str = RISCV_STATUS_STR;
	int nodeoff = 0;
	int ret = 0;
	char str[20];

	memset(str, 0, sizeof(str));
	sprintf(str, "riscv%d", riscv_id);
	ret = match_compatible(str, compatible_str, &nodeoff);
	if (ret < 0) {
		RISCV_DEBUG("riscv%d:dts no config [%s]\n", riscv_id, str);
		return -1;
	}

	RISCV_DEBUG("riscv%d:dts can find config [%s]\n", riscv_id, str);

	ret = match_status(nodeoff);
	if (ret < 0) {
		pmsg->riscv_status = DTS_CLOSE;
		RISCV_DEBUG("%s status is close\n", str);
		return -1;
	}
	pmsg->riscv_status = DTS_OPEN;
	return 0;
}



int riscv_dts_uart_msg(struct dts_msg_t *pmsg, u32 riscv_id)
{
	const char *compatible_str = RISCV_UART_STR;
	int i = 0;
	int nodeoff = 0;
	int ret = 0;
	int pin_count = 0;
	char str[20];
	user_gpio_set_t  pin_set[32];

	for (i = 0; i < UART_NUM; i++) {
		memset(str, 0, sizeof(str));
		sprintf(str, "serial%d", i);
		ret = match_compatible(str, compatible_str, &nodeoff);
		if (ret < 0) {
			continue;
		} else {
			RISCV_DEBUG("riscv%d:dts can find config [%s]\n",\
						riscv_id,\
						str);
			break;
		}
	}

	/*  no uart match ok */
	if (i == UART_NUM) {
		RISCV_DEBUG("riscv%d:dts no config [%s]\n", riscv_id, str);
		return -1;
	}

	pmsg->uart_msg.uart_port = i;
	ret = match_status(nodeoff);
	if (ret < 0) {
		pmsg->uart_msg.status = DTS_CLOSE;
		return -1;
	}
	pmsg->uart_msg.status = DTS_OPEN;

	ret = match_pinctrl_names(nodeoff);
	if (ret < 0)
		return -1;

	memset(str, 0, sizeof(str));
	sprintf(str, "pinctrl-%d", ret);

	/* uart has two pin*/
	pin_count = fdt_get_all_pin(nodeoff, str, pin_set);
	if ((pin_count < 0) || (pin_count != 2))
		return -1;

	for (i = 0; i < pin_count; i++) {
		pmsg->uart_msg.uart_pin_msg[i].port = pin_set[i].port;
		pmsg->uart_msg.uart_pin_msg[i].port_num = pin_set[i].port_num;
		pmsg->uart_msg.uart_pin_msg[i].mul_sel = pin_set[i].mul_sel;
	}

	return 0;
}

#define GROUP_NUM  (7)
int riscv_dts_gpio_int_msg(struct dts_msg_t *pmsg, u32 riscv_id)
{
	const char *compatible_str = RISCV_GPIO_INT_STR;
	const char *group_str[GROUP_NUM] = {"PA", "PB",
					"PC", "PD",
					"PE", "PF",
					"PG"};
	int nodeoff = 0;
	int ret = 0;
	unsigned int *pdata = NULL;
	char str[20];
	char *pins = NULL;

	memset(str, 0, sizeof(str));
	sprintf(str, "riscv%d_gpio_int", riscv_id);
	ret = match_compatible(str, compatible_str, &nodeoff);
	if (ret < 0) {
		RISCV_DEBUG("riscv%d:dts no config [%s]\n", riscv_id, str);
		return -1;
	}

	RISCV_DEBUG("riscv%d:dts can find config [%s]\n", riscv_id, str);

	ret = match_status(nodeoff);
	if (ret < 0) {
		memset((void *)&pmsg->gpio_int, 0,
				sizeof(struct dts_gpio_int_t));
		return -1;
	}

	ret = fdt_getprop_string(working_fdt, nodeoff, "pin-group", &pins);
	if (ret < 0) {
		memset((void *)&pmsg->gpio_int, 0,
				sizeof(struct dts_gpio_int_t));
		return -1;
	}

	int i = 0, j = 0;

	if (ret%3 != 0) {
		RISCV_DEBUG("riscv%d:[%s] format err\n", riscv_id, str);
		return -1;
	}
	ret /= 3;
	pdata = &pmsg->gpio_int.gpio_a;
	for (i = 0; i < ret; i++) {
		memset(str, 0, sizeof(str));
		memcpy(str, pins, 2);
		RISCV_DEBUG("[%d]%s\n", i, str);
		for (j = 0; j < GROUP_NUM; j++) {
			if (strcmp(str, group_str[j]) == 0) {
				RISCV_DEBUG("---%s ok\n", str);
				pdata[j] = DTS_OPEN;
			}
		}
		pins += 3;
	}

	RISCV_DEBUG("%d\n", pmsg->gpio_int.gpio_a);
	RISCV_DEBUG("%d\n", pmsg->gpio_int.gpio_b);
	RISCV_DEBUG("%d\n", pmsg->gpio_int.gpio_c);
	RISCV_DEBUG("%d\n", pmsg->gpio_int.gpio_d);
	RISCV_DEBUG("%d\n", pmsg->gpio_int.gpio_e);
	RISCV_DEBUG("%d\n", pmsg->gpio_int.gpio_f);
	RISCV_DEBUG("%d\n", pmsg->gpio_int.gpio_g);
	return 0;
}


int riscv_dts_sharespace_msg(struct dts_msg_t *pmsg, u32 riscv_id)
{
	const char *compatible_str = RISCV_SHARE_SPACE;
	int i = 0;
	int nodeoff = 0;
	int ret = 0;
	char str[30];
	u32 reg_data[8];

	memset(reg_data, 0, sizeof(reg_data));
	memset(str, 0, sizeof(str));
	sprintf(str, "share_space%d", riscv_id);
	ret = match_compatible(str, compatible_str, &nodeoff);
	if (ret < 0) {
		RISCV_DEBUG("riscv%d:dts no config [%s]\n", riscv_id, str);
		return -1;
	}
	RISCV_DEBUG("riscv%d:dts can find config [%s]\n", riscv_id, str);

	ret = match_status(nodeoff);
	if (ret < 0) {
		pmsg->dts_sharespace.status = DTS_CLOSE;
		return -1;
	}

	pmsg->dts_sharespace.status = DTS_OPEN;

	ret = fdt_getprop_u32(working_fdt, nodeoff, "reg", reg_data);
	if (ret < 0) {
		memset((void *)&pmsg->dts_sharespace, 0,
				sizeof(struct dts_sharespace_t));
		return -1;
	}

	for (i = 0; i < 8; i++) {
		RISCV_DEBUG("riscv%d:dts reg[%d]=0x%x\n", riscv_id, i, reg_data[i]);
	}
	pmsg->dts_sharespace.riscv_write_addr = reg_data[0];
	pmsg->dts_sharespace.riscv_write_size = reg_data[1];
	pmsg->dts_sharespace.arm_write_addr = reg_data[2];
	pmsg->dts_sharespace.arm_write_size = reg_data[3];
	pmsg->dts_sharespace.riscv_log_addr = reg_data[4];
	pmsg->dts_sharespace.riscv_log_size = reg_data[5];

	return 0;
}
