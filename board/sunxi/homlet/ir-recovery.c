/*
 *  * Copyright 2000-2009
 *   * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *    *
 *     * SPDX-License-Identifier:	GPL-2.0+
 *     */

#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gic.h>
#include <sys_config.h>
#include <fdt_support.h>
#include <malloc.h>
#include <private_uboot.h>

#include <sunxi-ir.h>
#include "asm/arch/timer.h"

DECLARE_GLOBAL_DATA_PTR;

/*define ir or key mode value*/
#define EFEX_VALUE (0x81)
#define SPRITE_RECOVERY_VALUE (0X82)
#define BOOT_RECOVERY_VALUE (0x83)
#define BOOT_FACTORY_VALUE (0x84)

/*ir or key mode in sys_config*/
#define EFEX_MODE_CONFIG (0x0)
#define ONEKEY_SPRITE_RECOVERY_MODE_CONFIG (0x1)
#define RECOVERY_MODE_CONFIG (0x2)
#define FACTORY_MODE_CONFIG (0x3)

struct sunxi_ir_data ir_data;
struct timer_list ir_timer_t;

static int ir_boot_recovery_mode;
extern struct ir_raw_buffer sunxi_ir_raw;
static int ir_detect_time = 1500;
static u32 code_store[32];
static int code_num;

#define FDT_IR_BOOT_SETTINGS_PATH "/soc/ir_boot_recovery"

static int ir_sys_cfg(void)
{
	int i, ret;
	u32 value = 0;
	char addr_name[32];
	int detect_time = 0;
	int node_offset = 0;

	if (uboot_spare_head.boot_data.work_mode != WORK_MODE_BOOT)
		return -1;

	node_offset = fdt_path_offset(working_fdt, FDT_IR_BOOT_SETTINGS_PATH);
	if (node_offset < 0) {
		pr_err("%s: error:no node for ir boot recovery settings\n",
		       __func__);
		return -1;
	}

	char *node_status;
	ret = fdt_getprop_string(working_fdt, node_offset, "status",
				 &node_status);
	if ((ret < 0) || (strcmp(node_status, "okay") != 0)) {
		pr_notice("ir boot recovery not used\n");
		return -1;
	}

	ret = script_parser_fetch(FDT_IR_BOOT_SETTINGS_PATH, "ir_detect_time",
				  (int *)&detect_time, sizeof(int) / 4);
	if (ret) {
		pr_notice("use default detect time %d\n", ir_detect_time);
		detect_time = ir_detect_time;
	}

	ir_detect_time = detect_time;

	for (i = 0; i < MAX_IR_ADDR_NUM; i++) {
		sprintf(addr_name, "ir_recovery_key_code%d", i);
		if (script_parser_fetch(FDT_IR_BOOT_SETTINGS_PATH, addr_name,
					(int *)&value, 1)) {
			pr_notice("node %s get failed!\n", addr_name);
			break;
		}
		code_store[i] = ir_data.ir_recoverykey[i] = value;
		sprintf(addr_name, "ir_addr_code%d", i);
		if (script_parser_fetch(FDT_IR_BOOT_SETTINGS_PATH, addr_name,
					(int *)&value, 1)) {
			break;
		}
		ir_data.ir_addr[i] = value;
		code_num++;
		debug("key_code = %s, value = %d\n", addr_name, value);
		debug("code num = %d\n", code_num);
	}
	ir_data.ir_addr_cnt = i;
/*
	for (i = 0; i < ir_data.ir_addr_cnt; i++) {
		sprintf(addr_name, "ir_addr_code%d", i);
		if (script_parser_fetch(FDT_IR_BOOT_SETTINGS_PATH, addr_name,
					(int *)&value, 1)) {
			break;
		}
		ir_data.ir_addr[i] = value;
		pr_notice("addr_code = %s, value = %d\n", addr_name, value);

		sprintf(addr_name, "ir_recovery_key_code%d", i);
		if (script_parser_fetch(FDT_IR_BOOT_SETTINGS_PATH, addr_name,
					(int *)&value, 1)) {
			break;
		}
		code_num++;
		code_store[i] = value;
		pr_notice("key_code = %s, value = %d\n", addr_name, value);
		pr_notice("code num = %d\n", code_num);
	}
*/
	return 0;
}

static int ir_code_detect_no_duplicate(struct sunxi_ir_data *ir_data, u32 code)
{
	int key_no_duplicate;
	if (script_parser_fetch(FDT_IR_BOOT_SETTINGS_PATH, "ir_key_no_duplicate",
				(int *)&key_no_duplicate, 1)) {
		pr_notice("ir_key_no_duplicate get fail, use defualt mode!\n");
		return 0;
	}

	if (key_no_duplicate == 0) {
		pr_notice("[ir recovery]: do not use key duplicate mode!\n");
		return 0;
	} else {
		pr_notice("[ir recovery]: use key duplicate mode!\n");
	}

	int i;
	for (i = 0; i < code_num; i++) {
		if ((code & 0xff) == code_store[i]) {
			pr_notice("In %s: code_store = %d\n", __func__,
				  code_store[i]);
			code_store[i] = 0;
			return 0;
		}
	}

	return -1;
}

static int ir_code_valid(struct sunxi_ir_data *ir_data, u32 code)
{
	u32 i, tmp;
	tmp = (code >> 8) & 0xffff;
	pr_notice("line:%d recved addr=0x%x key=0x%x\n", __LINE__, tmp,
		  (code & 0xff));
	for (i = 0; i < ir_data->ir_addr_cnt; i++) {
		pr_notice(
			"line:%d try ir_addr[%d] =0x%x, ir_recoverykey[%d]=0x%x\n",
			__LINE__, i, ir_data->ir_addr[i], i,
			ir_data->ir_recoverykey[i]);
		if (ir_data->ir_addr[i] == tmp) {
			if ((code & 0xff) == ir_data->ir_recoverykey[i]) {
				return ir_code_detect_no_duplicate(ir_data,
								   code);
			}
		}
	}

	pr_notice("ir boot recovery not detect\n");
	return -1;
}

int ir_boot_recovery_mode_detect(void)
{
	u32 code;
	if (ir_boot_recovery_mode) {
		code = sunxi_ir_raw.scancode;
		pr_notice("line:%d code=0x%x\n", __LINE__, code);
		if ((code != 0) && (!ir_code_valid(&ir_data, code))) {
			pr_notice("ir boot recovery detect valid.\n");
			return 0;
		}
	}

	return -1;
}

static void ir_detect_overtime(void *p)
{
	struct timer_list *timer_t;

	timer_t = (struct timer_list *)p;
	/*ir_disable();*/
	del_timer(timer_t);

	gd->ir_detect_status = IR_DETECT_END;
}

int check_ir_boot_recovery(void)
{
	if (ir_sys_cfg())
		return 0;

	if (ir_setup())
		return 0;

	gd->ir_detect_status  = IR_DETECT_NULL;
	ir_boot_recovery_mode = 1;
	ir_timer_t.data       = (unsigned long)&ir_timer_t;
	ir_timer_t.expires    = ir_detect_time;
	ir_timer_t.function   = ir_detect_overtime;
	init_timer(&ir_timer_t);
	add_timer(&ir_timer_t);
	return 0;
}

int get_ir_work_mode(int *cnt)
{
	int i;
	s32 tmp = (sunxi_ir_raw.scancode >> 8) & 0xffff;
	*cnt = ir_data.ir_addr_cnt;
	for (i = 0; i < ir_data.ir_addr_cnt; i++) {
		if (ir_data.ir_addr[i] == tmp) {
			if ((sunxi_ir_raw.scancode & 0xff) == ir_data.ir_recoverykey[i])
				break;
		}
	}
	return i%4;
}

extern int ir_nec_decode(struct ir_raw_buffer *ir_raw, struct ir_raw_event ev);
extern int get_ir_work_mode(int *cnt);
unsigned long ir_packet_handle(struct ir_raw_buffer *ir_raw)
{
	int i = 0, ret = 0;
	int ir_press_times = 0;
	int ir_work_mode   = 0;
	static uint ir_detect_count;

	ret = script_parser_fetch(FDT_IR_BOOT_SETTINGS_PATH, "ir_press_times",
				  (int *)&ir_press_times, sizeof(int) / 4);
	if (ret) {
		printf("ir_press_times not set, use default press time : 1\n");
		ir_press_times = 1;
	}

	for (i = 0; i < ir_raw->dcnt; i++) {
		ir_nec_decode(ir_raw, ir_raw->raw[i]);
	}

	ret = ir_boot_recovery_mode_detect();
	if (!ret) {
		ir_detect_count++;
	}

	if ((ir_detect_count == ir_press_times)
		&& (gd->ir_detect_status != IR_DETECT_OK)) {
		gd->ir_detect_status = IR_DETECT_OK;
#if 1
		ret = script_parser_fetch(FDT_IR_BOOT_SETTINGS_PATH, "ir_work_mode",
					  (int *)&ir_work_mode,
					  sizeof(int) / 4);
		if (ret) {
#else
		ir_work_mode = get_ir_work_mode(&ret);
		if (ret == ir_work_mode) {
#endif
			printf("ir_work_mode not set, use default mode: android recovery mode.\n");
			gd->key_pressd_value = BOOT_RECOVERY_VALUE;
		} else {
			switch (ir_work_mode) {
			case EFEX_MODE_CONFIG:
				gd->key_pressd_value = EFEX_VALUE;
				break;
			case ONEKEY_SPRITE_RECOVERY_MODE_CONFIG:
				gd->key_pressd_value = SPRITE_RECOVERY_VALUE;
				break;
			case RECOVERY_MODE_CONFIG:
				gd->key_pressd_value = BOOT_RECOVERY_VALUE;
				break;
			case FACTORY_MODE_CONFIG:
				gd->key_pressd_value = BOOT_FACTORY_VALUE;
				break;
			}
		}
	}
	pr_notice("line:%d gd->key_pressd_value=0x%lx\n", __LINE__,
		  gd->key_pressd_value);
	return 0;
}
