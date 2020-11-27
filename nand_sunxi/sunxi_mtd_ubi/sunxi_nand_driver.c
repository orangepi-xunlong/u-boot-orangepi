/*
 * (C) Copyright 2007-2017
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * zhongguizhao <zhongguizhao@allwinnertech.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <malloc.h>
#include <linux/mtd/mtd.h>
#include "sunxi_nand_mtd.h"
#include "sunxi_nand_driver.h"

#ifdef CONFIG_SUNXI_UBIFS

static struct _nand_info *g_nand_info;
static char nand_para_store[256];
static int  flash_scaned;
static int nand_partition_num;
static int  nand_open_times;

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
static struct _nand_info *sunxi_nand_hw_init(void)
{
	int ret;

	nand_cfg_setting();

	ret = nand_physic_init();
	if (ret != 0) {
		PHY_ERR("nand_physic_init error %d\n", ret);
		return NULL;
	}

	aw_nand_info.type = 0;
	aw_nand_info.SectorNumsPerPage =
		g_nssi->nsci->sector_cnt_per_super_page;

	aw_nand_info.BytesUserData = g_nssi->nsci->spare_bytes;
	aw_nand_info.BlkPerChip = g_nssi->nsci->blk_cnt_per_super_chip;
	aw_nand_info.ChipNum = g_nssi->super_chip_cnt;
	aw_nand_info.PageNumsPerBlk = g_nssi->nsci->page_cnt_per_super_blk;
	/* aw_nand_info.FullBitmap = FULL_BITMAP_OF_SUPER_PAGE;
	 * aw_nand_info.MaxBlkEraseTimes = 2000;
	 */
	aw_nand_info.MaxBlkEraseTimes =
		g_nssi->nsci->nci_first->max_erase_times;

	aw_nand_info.EnableReadReclaim = 1;

	if (sunxi_get_mtd_ubi_mode_status()) {
		PHY_DBG("NandHwInit end\n");
		return &aw_nand_info;
	}

	nand_secure_storage_init();
	PHY_DBG("NandHwInit end\n");
	return &aw_nand_info;
}

static int sunxi_nand_phy_exit(void)
{
	return NAND_PhyExit();
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         : must check NAND_UpdatePhyArch()
*****************************************************************************/
static int sunxi_nand_update_phy_arch(void)
{
	printf("null %s\n", __func__);
	return 0;

	/*
	 * int ret;
	 * unsigned int id_number_ctl,para;
	 *
	 * g_nssi->support_two_plane = 0;
	 * g_nssi->support_v_interleave = 1;
	 * g_nssi->support_dual_channel = 1;
	 *
	 * nand_permanent_data.magic_data = \
	 *	MAGIC_DATA_FOR_PERMANENT_DATA;
	 * nand_permanent_data.support_two_plane = 0;
	 * nand_permanent_data.support_vertical_interleave = 1;
	 * nand_permanent_data.support_dual_channel = 1;
	 *
	 * PHY_ERR("NAND UpdatePhyArch\n");
	 *
	 * g_nssi->support_two_plane = \
	 *	g_phy_cfg->phy_support_two_plane;
	 * nand_permanent_data.support_two_plane = \
	 *	g_nssi->support_two_plane;
	 *
	 * g_nssi->support_v_interleave = \
	 *	g_phy_cfg->phy_nand_support_vertical_interleave;
	 * nand_permanent_data.support_vertical_interleave = \
	 *	g_nssi->support_v_interleave;
	 *
	 * g_nssi->support_dual_channel = \
	 *	g_phy_cfg->phy_support_dual_channel;
	 * nand_permanent_data.support_dual_channel = \
	 *	g_nssi->support_dual_channel;
	 *
	 * id_number_ctl = NAND_GetNandIDNumCtrl();
	 * if ((id_number_ctl&0x0e) != 0)
	 * {
	 *	para = NAND_GetNandExtPara(1);
	 *	 if(( para != 0xffffffff)&&(id_number_ctl&0x02))
	 *	 {
	 *		 if(((para & 0xffffff) ==
	 *			g_nctri->nci->npi->id_number)
	 *			|| ((para & 0xffffff) == 0xeeeeee))
	 *		 {
	 *			 PHY_ERR("script support_two_plane %d\n",para);
	 *			 g_nssi->support_two_plane =
	 *				(para >> 24) & 0xff;
	 *			 if(g_nssi->support_two_plane == 1)
	 *			 {
	 *				 g_nssi->support_two_plane = 1;
	 *			 }
	 *			 else
	 *			 {
	 *				 g_nssi->support_two_plane = 0;
	 *			 }
	 *			 nand_permanent_data.support_two_plane =
	 *			g_nssi->support_two_plane;
	 *		 }
	 *	 }
	 *
	 *	 para = NAND_GetNandExtPara(2);
	 *	 if(( para != 0xffffffff)&&(id_number_ctl&0x04))
	 *	 {
	 *		 if(((para & 0xffffff) ==
	 *			g_nctri->nci->npi->id_number)
	 *			|| ((para & 0xffffff) == 0xeeeeee))
	 *		 {
	 *			 PHY_ERR("script support_v_interleave %d\n",
	 *			para);
	 *			 g_nssi->support_v_interleave =
	 *				(para >> 24) & 0xff;
	 *			 nand_permanent_data. \
	 *			support_vertical_interleave =
	 *			g_nssi->support_v_interleave;
	 *		 }
	 *	 }
	 *
	 *	 para = NAND_GetNandExtPara(3);
	 *	 if(( para != 0xffffffff)&&(id_number_ctl&0x08))
	 *	 {
	 *		 if(((para & 0xffffff) == g_nctri->nci->npi->id_number)
	 *		|| ((para & 0xffffff) == 0xeeeeee))
	 *		 {
	 *			 PHY_ERR("script support_dual_channel %d\n",
	 *			para);
	 *			 g_nssi->support_dual_channel =
	 *			(para >> 24) & 0xff;
	 *			 nand_permanent_data.support_dual_channel =
	 *			g_nssi->support_dual_channel;
	 *		 }
	 *	 }
	 * }
	 *
	 *
	 * if (sunxi_get_mtd_ubi_mode_status()) {
	 *	printf("%s, in mtd_ubi mode.\n", __func__);
	 *	return 0;
	 * }
	 *
	 * ret = set_nand_structure((void*)&nand_permanent_data);
	 *
	 * return ret;
	 */
}


static int _sunxi_nand_uboot_erase(int erase_flag)
{
	int nand_erased = 0;

	debug("erase_flag = %d\n", erase_flag);
	sunxi_nand_phy_init();

	printf("erase by flag %d\n", erase_flag);
	NAND_EraseBootBlocks();

	if (sunxi_get_mtd_ubi_mode_status())
		NAND_EraseChip_force();
	else
		NAND_EraseChip();

	sunxi_nand_update_phy_arch();
	nand_erased = 1;

	printf("NAND_Uboot_Erase\n");
	sunxi_nand_phy_exit();
	return nand_erased;

	/*
	 * int version_match_flag;
	 * int nand_erased = 0;
	 * debug("erase_flag = %d\n", erase_flag);
	 * sunxi_nand_phy_init();
	 *
	 * if(erase_flag)
	 * {
	 *	printf("erase by flag %d\n", erase_flag);
	 *	NAND_EraseBootBlocks();
	 *
	 *	if (sunxi_get_mtd_ubi_mode_status())
	 *		NAND_EraseChip_force();
	 *	else
	 *		NAND_EraseChip();
	 *
	 *	sunxi_nand_update_phy_arch();
	 *	nand_erased = 1;
	 * }
	 * else
	 * {
	 *	//nand_super_page_test(0,0,0);
	 *	version_match_flag = NAND_VersionCheck();
	 *	printf("nand version = %x\n", version_match_flag);
	 *	NAND_EraseBootBlocks();
	 *
	 *	if(nand_is_blank() == 1)
	 *		NAND_EraseChip();
	 *
	 *	if (version_match_flag > 0)
	 *	{
	 *		//NAND_EraseChip();
	 *		//NAND_UpdatePhyArch();
	 *		//nand_erased = 1;
	 *		debug("nand version check fail,"
	 *			"please select erase nand flash\n");
	 *		nand_erased =  -1;
	 *	}
	 * }
	 * printf("NAND_Uboot_Erase\n");
	 * sunxi_nand_phy_exit();
	 * return nand_erased;
	 */
}

static int _sunxi_nand_uboot_probe(void)
{
	int ret = 0;

	debug("NAND_UbootProbe start\n");

    /* logic init */
	ret = sunxi_nand_phy_init();
	sunxi_nand_phy_exit();

	debug("NAND_UbootProbe end: 0x%x\n", ret);

	return ret;
}

static int sunxi_nand_get_param_store(void *buffer, uint length)
{
	if (!flash_scaned) {
		printf("sunxi flash: force flash init to begin hardware scanning\n");
		sunxi_nand_phy_init();
		sunxi_nand_phy_exit();
		printf("sunxi flash: hardware scan finish\n");
	}
	memcpy(buffer, nand_para_store, length);

	return 0;
}

static int sunxi_nand_logic_init(int boot_mode)
{
	__s32  result = 0;
	__s32 ret = -1;
	__s32 i, nftl_num, capacity_level;
	struct _nand_info *nand_info;


	printf("%s, NB1 : enter NAND_LogicInit\n", __func__);

	nand_info = sunxi_nand_hw_init();

	capacity_level = NAND_GetNandCapacityLevel();
	set_capacity_level(nand_info, capacity_level);

	if (sunxi_get_mtd_ubi_mode_status()) {
		printf("NB1 : NAND_LogicInit ok.\n");
		return 0;
	}

	g_nand_info = nand_info;
	if (nand_info == NULL) {
		printf("NB1 : nand phy init fail\n");
		return ret;
	}

	if ((!boot_mode) && (nand_mbr.PartCount != 0)
		&& (mbr_burned_flag == 0)) {
		printf("burn nand partition table! mbr tbl: 0x%x, part_count:%d\n",
			(__u32)(&nand_mbr), nand_mbr.PartCount);
		result = nand_info_init(nand_info, 0, 8, (uchar *)&nand_mbr);
		mbr_burned_flag = 1;
	} else {
		printf("not burn nand partition table!\n");
		result = nand_info_init(nand_info, 0, 8, NULL);
	}

	if (result != 0) {
		printf("NB1 : nand_info_init fail\n");
		return -5;
	}

	if (boot_mode) {
		nftl_num = get_phy_partition_num(nand_info);
		printf("NB1 : nftl num: %d\n", nftl_num);
		if ((nftl_num < 1) || (nftl_num > 5)) {
			printf("NB1 : nftl num: %d error\n", nftl_num);
			return -1;
		}

		nand_partition_num = 0;
		for (i = 0; i < nftl_num - 1; i++) {
			nand_partition_num++;
			printf(" init nftl: %d\n", i);
			result = nftl_build_one(nand_info, i);
		}
	} else {
		result = nftl_build_all(nand_info);
		nand_partition_num = get_phy_partition_num(nand_info);
	}

	if (result != 0) {
		printf("NB1 : nftl_build_all fail\n");
		return -5;
	}

	printf("NB1 : NAND_LogicInit ok, result = 0x%x\n", result);

	return result;
}

static int _sunxi_nand_uboot_init(int boot_mode)
{
	int ret = 0;

	debug("NAND_UbootInit start\n");

	NAND_set_boot_mode(boot_mode);

	/* logic init */
	ret |= sunxi_nand_logic_init(boot_mode);
	if (!boot_mode) {
		if (!flash_scaned) {
			printf("%s, init nand_para_store\n", __func__);
			nand_get_param((boot_nand_para_t *)nand_para_store);
			flash_scaned = 1;
		}
	}

	debug("NAND_UbootInit end: 0x%x\n", ret);

	return ret;
}

static int sunxi_nand_logic_exit(void)
{
	printf("%s, NB1 : NAND_LogicExit\n", __func__);
	NandHwExit();
	g_nand_info = NULL;
	return 0;
}

static int _sunxi_nand_uboot_exit(void)
{
	int ret = 0;

	printf("%s\n", __func__);

	ret = sunxi_nand_logic_exit();

	return ret;
}

int sunxi_nand_phy_init(void)
{
	struct _nand_info *nand_phy_info;

	NAND_Print("NB1 : enter phy init\n");

    /* ClearNandStruct(); */

	nand_phy_info = sunxi_nand_hw_init();
	if (nand_phy_info == NULL) {
		printf("NB1 : nand phy init fail\n");
		return -1;
	}

	NAND_Print("%s, NB1 : nand phy init ok\n", __func__);

	return 0;
}

int sunxi_nand_uboot_erase(int user_erase)
{
	return _sunxi_nand_uboot_erase(user_erase);
}

int sunxi_nand_download_boot0(uint length, void *buffer)
{
	int ret;

	printf("%s\n", __func__);

	if (sunxi_nand_phy_init() == 0)
		ret = NAND_BurnBoot0(length, buffer);
	else
		ret = -1;

	sunxi_nand_phy_exit();

	return ret;
}

int sunxi_nand_download_uboot(uint length, void *buffer)
{
	int ret;

	printf("%s, nand_download_uboot\n", __func__);

	if (!sunxi_nand_phy_init()) {
		ret = NAND_BurnUboot(length, buffer);
		debug("nand burn uboot error ret = %d\n", ret);
	} else {
		debug("nand phyinit error\n");
		ret = -1;
	}

	sunxi_nand_phy_exit();

	return ret;
}

int sunxi_nand_uboot_probe(void)
{
	debug("nand_uboot_probe\n");
	return _sunxi_nand_uboot_probe();
}

int sunxi_nand_uboot_force_erase(void)
{
	printf("force erase, %s.\n", __func__);
	if (sunxi_nand_phy_init()) {
		printf("phy init fail\n");
		return -1;
	}

	NAND_EraseChip_force();
	sunxi_nand_phy_exit();

	return 0;
}

uint sunxi_nand_uboot_get_flash_info(void *buffer, uint length)
{
	return sunxi_nand_get_param_store(buffer, length);
}

int sunxi_get_mtd_ubi_mode_status(void)
{
	return sunxi_mtd_ubi_mode;
}

void sunxi_disable_mtd_ubi_mode(void)
{
	sunxi_mtd_ubi_mode = 0;
}

void sunxi_enable_mtd_ubi_mode(void)
{
	sunxi_mtd_ubi_mode = 1;
}

int sunxi_open_ubifs_interface(void)
{
	int ret;
	int en_ubifs;
	int nand_nodeoffset;

	nand_nodeoffset =  fdt_path_offset(working_fdt, "nand0");
	if (nand_nodeoffset < 0) {
		printf("nand0: get node offset error\n");
		return -1;
	}

	ret = fdt_getprop_u32(working_fdt, nand_nodeoffset,
			"nand0_ubifs", (uint32_t *)&en_ubifs);
	if (ret < 0) {
		printf("nand : %s fail, %x\n", __func__, en_ubifs);
		return -1;
	}

	if (en_ubifs)
		sunxi_enable_mtd_ubi_mode();
	else
		sunxi_disable_mtd_ubi_mode();

	printf("\nnand : %s  from script, %x\n", __func__, en_ubifs);
	printf("open ubifs interface by dts config.\n");
	printf("Sunxi nand is in ubifs mode.\n");

	return 0;
}

int sunxi_nand_uboot_init(int boot_mode)
{
	if (!nand_open_times) {
		nand_open_times++;
	    printf("%s, NAND_UbootInit\n", __func__);

	    return _sunxi_nand_uboot_init(boot_mode);
	}

	printf("%s, nand already init\n", __func__);
	nand_open_times++;

	return 0;
}

int sunxi_nand_uboot_exit(int force)
{
	if (nand_open_times == 0) {
		printf("nand not opened\n");
		return 0;
	}

	if (force)
		if (nand_open_times) {
			nand_open_times = 0;
			printf("%s, NAND_UbootExit\n", __func__);
			return _sunxi_nand_uboot_exit();
	    }

	printf("%s, nand not need closed\n", __func__);

	return 0;
}
#endif
