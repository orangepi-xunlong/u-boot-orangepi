/*
 * sunxi_ephy.c: Allwinnertech External Phy u-boot driver
 * (C) Copyright 2013-2017
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Author: Huang zhenwei <huangzhenwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <linux/types.h>
#include <common.h>
#include <asm/io.h>
#include <net.h>
#include <malloc.h>
#include <linux/mii.h>
#include <netdev.h>
#include <errno.h>
#include <sys_config.h>
#include <linux/string.h>
#include <fdt_support.h>
#include <miiphy.h>
#include <phy.h>
#include <power/sunxi/pmu.h>
#include "asm/io.h"
#include <i2c.h>
#include <fdt_support.h>

#define EXTEPHY_CTRL0 0x0014
#define EXTEPHY_CTRL1 0x0016

#define EPHY_CTRL 0x6000
#define EPHY_SID 0x8004

static phy_interface_t g_phy_intf = PHY_INTERFACE_MODE_NONE;
static u32 ac200_twi_addr = -1;

static s32 aw1683_wr_reg(__u16 sub_addr, __u16 data)
{

	__s8 ret = -1;
	__u32 tmpData = 0;

	__u8 *pexcTmp = NULL;
	__u8  excTmp = 0;
	pexcTmp = (__u8 *)&data;
	excTmp = pexcTmp[0];
	pexcTmp[0] = pexcTmp[1];
	pexcTmp[1] = excTmp;

	/* ac200_twi_addr */
	tmpData = (u32)(sub_addr>>8) & 0xff;

	pexcTmp = (__u8 *)&tmpData;
	excTmp = pexcTmp[0];
	pexcTmp[0] = pexcTmp[1];
	pexcTmp[1] = excTmp;

	ret = i2c_write((uchar)ac200_twi_addr, (__u32)0xfe, 1, (__u8 *)&tmpData, 2);

	tmpData = sub_addr & 0xff;

	ret = i2c_write((uchar)ac200_twi_addr, tmpData, 1, (__u8 *)&data, 2);

	return ret;
}

static s32 aw1683_rd_reg(__u16 sub_addr, __u16 *data)
{

	__s8 ret = -1;
	__u32 tmpData = 0;
	__u32 i2cFixAddr = 0xfe;
	__u8 *pexcTmp = NULL;
	__u8  excTmp = 0;

	tmpData = (u32)(sub_addr>>8) & 0xff;
	pexcTmp = (__u8 *)&tmpData;
	excTmp = pexcTmp[0];
	pexcTmp[0] = pexcTmp[1];
	pexcTmp[1] = excTmp;

	ret = i2c_write((uchar)ac200_twi_addr, i2cFixAddr, 1, (__u8 *)&tmpData, 2);

	tmpData = (u32)(sub_addr & 0xff);

	ret = i2c_read((uchar)ac200_twi_addr, tmpData, 1, (__u8 *)data, 2);

	pexcTmp = (__u8 *)data;
	excTmp = pexcTmp[0];
	pexcTmp[0] = pexcTmp[1];
	pexcTmp[1] = excTmp;

	return ret;
}

static int ephy_sys_init(void)
{
	char *phy_mode = NULL;
	int nodeoffset;
	unsigned char i;

	/* config for gmac */
	nodeoffset = fdt_path_offset(working_fdt, "gmac0");
	if (nodeoffset < 0) {
		printf("%s:%d: get nodeerror\n", __func__, __LINE__);
		return -1;
	}

	/* config PHY mode */
	if (fdt_getprop_string(working_fdt, nodeoffset, "phy-mode", &phy_mode) < 0) {
		printf("get phy-mode fail!");
		return -1;
	}
	for (i = 0; i < ARRAY_SIZE(phy_interface_strings); i++) {
		if (!strcmp(phy_interface_strings[i], (const char *)phy_mode))
			break;
	}
	if (i == PHY_INTERFACE_MODE_NONE)
		return -1;
	else
		g_phy_intf = i;

	/* config for ac200 */
	nodeoffset = fdt_path_offset(working_fdt, "/soc/ac200");
	if (nodeoffset < 0) {
		printf("%s:%d: get nodeerror\n", __func__, __LINE__);
		return -1;
	}
	if (fdt_getprop_u32(working_fdt, nodeoffset, "tv_twi_addr", &ac200_twi_addr) < 0) {
		printf("get ac200_twi_addr fail!");
		return -1;
	}

	return 0;
}

static int ephy_config_init(void)
{
	u16 data = 0;

	aw1683_rd_reg(EPHY_CTRL, &data);
	if (g_phy_intf == PHY_INTERFACE_MODE_RMII)
		data |= (1 << 11);
	else
		data &= (~(1 << 11));
	aw1683_wr_reg(EPHY_CTRL, data | (1 << 11));

	return 0;

}

static int sunxi_ephy_enable(void)
{
	u16 data = 0;
	u16 data_sid = 0;
	u16 try_count = 0;

	aw1683_wr_reg(0x0002, 0x0001);	/* close chip reset */
	aw1683_rd_reg(0x0002, &data);
	for (try_count = 0; try_count < 10 && data != 0x0001; ++try_count) {
		aw1683_wr_reg(0x0002, 0x0001); /* close chip reset */
		aw1683_rd_reg(0x0002, &data);
	}
	if (data != 0x0001) {
		printf("close chip reset failed after %d times\n", try_count);
		return -EINVAL;
	}

	aw1683_rd_reg(EXTEPHY_CTRL0, &data);
	data |= 0x03;
	aw1683_wr_reg(EXTEPHY_CTRL0, data);
	aw1683_rd_reg(EXTEPHY_CTRL1, &data);
	data |= 0x0f;
	aw1683_wr_reg(EXTEPHY_CTRL1, data);
	aw1683_wr_reg(EPHY_CTRL, 0x06);

	/*for ephy */
	aw1683_rd_reg(EPHY_CTRL, &data);
	aw1683_rd_reg(EPHY_SID, &data_sid);
	data &= ~(0xf<<12);
	data |= (0x0F & (0x03 + data_sid))<<12;
	aw1683_wr_reg(EPHY_CTRL, data);

	return 0;
}

int ephy_init(void)
{
	if (ephy_sys_init()) {
		printf("ephy_sys_init fail!\n");
		return -EINVAL;
	}

	if (sunxi_ephy_enable()) {
		printf("sunxi_ephy_enable fail!\n");
		return -EINVAL;
	}

	ephy_config_init();

	return 0;
}

