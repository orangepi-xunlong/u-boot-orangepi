// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * oujiayu <oujiayu@allwinnertech.com>
 *
 */
#include <common.h>
#include <config.h>
#include <command.h>
#include <asm/io.h>
#include <fdt_support.h>
#include <fdtdec.h>
#include <environment.h>
#include "sunxi_lradc_vol.h"

DECLARE_GLOBAL_DATA_PTR;

#define FDT_PATH_KEYBOARD	"/soc/keyboard"
#define FDT_REG			"reg"
#define FDT_KEY_CNT		"key_cnt"
#define ENV_LRADC_VOL		"lradc_vol"
#define VOL_NUM			13
static u32 key_cnt = 1;

static u32 lradc_read_data(u32 reg_base)
{
	u32 reg_val;

	reg_val = readl(reg_base + LRADC_DATA0);

	return reg_val;
}

static int lradc_data_init(void)
{
	int nodeoffset;

	nodeoffset = fdt_path_offset(working_fdt, FDT_PATH_KEYBOARD);
	if (nodeoffset < 0) {
		printf("find dts keytboard err!\n");
		return -1;
	}

	fdt_getprop_u32(working_fdt, nodeoffset, FDT_KEY_CNT, &key_cnt);
	if (!key_cnt) {
		printf("find dts key_cnt err!!!\n");
		return -1;
	}
	return 0;
}

int sunxi_read_lradc_vol(void)
{
	int i;
	int nodeoffset;
	u32 reg_base;
	const void *blob = gd->fdt_blob;
	u32 real_vol = 0;
	u32 val[2] = {0, 0};
	u32 key_vol[VOL_NUM];
	u32 key_ch[VOL_NUM];
	char *s;
	char node_name[32] = {0};

	lradc_data_init();
	//reg_base = lradc_reg_init();

	nodeoffset = fdt_path_offset(working_fdt, FDT_PATH_KEYBOARD);
	for (i = 0; i < key_cnt; i++) {
		sprintf(node_name, "key%d", i);
		fdt_getprop_u32(working_fdt, nodeoffset, node_name, val);
		key_vol[i] = val[0];
		key_ch[i] = val[1];
	}
	reg_base = fdtdec_get_addr(blob, nodeoffset, FDT_REG);
	if (!reg_base) {
		printf("find dts reg_base err!!!\n");
		return -1;
	}

#ifdef SUNXI_LRADC_VOL_DEBUG
	for (i = 0; i < key_cnt; i++) {
		printf("key_vol[%d] is %d ..\n", i, key_vol[i]);
		printf("key_ch[%d] is %d ..\n", i, key_ch[i]);
	}
#endif
	s = env_get(ENV_LRADC_VOL);
	real_vol = lradc_read_data(reg_base);
	real_vol = (1350 / 63 * real_vol);
	for (i = 0; i < key_cnt; i++) {
		if (i == 0) {
			if (real_vol <= key_vol[i] && real_vol > 0) {
				sprintf(s, "%d", key_ch[i]);
				env_set(ENV_LRADC_VOL, s);
			}
		} else {
			if (real_vol <= key_vol[i] &&
					real_vol > key_vol[(i - 1)]) {
				sprintf(s, "%d", key_ch[i]);
				env_set(ENV_LRADC_VOL, s);
			}
		}
	}
	return 0;
}
