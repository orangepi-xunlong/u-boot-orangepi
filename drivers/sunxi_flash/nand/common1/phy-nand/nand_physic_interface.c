/* SPDX-License-Identifier: GPL-2.0 */
/*
************************************************************************************************************************
*                                                      eNand
*                                           Nand flash driver scan module
*
*                             Copyright(C), 2008-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : nand_chip_interface.c
*
* Author :
*
* Version : v0.1
*
* Date : 2013-11-20
*
* Description :
*
* Others : None at present.
*
*
*
************************************************************************************************************************
*/
#define _NPHYI_COMMON_C_
#include <stdio.h>
#include <sunxi_nand.h>
#include "nand_physic_interface.h"
/*#include "nand-partition/phy.h"*/
#include <sunxi_nand_partitions.h>
#include "rawnand/rawnand_chip.h"
#include "rawnand/controller/ndfc_base.h"
/*#include "rawnand/rawnand.h"*/
#include "rawnand/rawnand_base.h"
#include "rawnand/rawnand_cfg.h"
#include "nand.h"
#include "nand_boot.h"
#include "spinand/spinand.h"
#include "nand_weak.h"
#include "../nand_osal_uboot.h"
#include <sunxi_nand_partitions.h>
/*#include <linux/mtd/aw-spinand-nftl.h>*/
//#define POWER_OFF_DBG
__u32 storage_type;

struct _nand_info aw_nand_info = {0};

#ifndef POWER_OFF_DBG
unsigned int power_off_dbg_enable;
#else
unsigned int power_off_dbg_enable = 1;
#endif
/* current logical write block */
unsigned short cur_w_lb_no;
/* current physical write block */
unsigned short cur_w_pb_no;
/* current physical write page */
unsigned short cur_w_p_no;
/* current logical erase block */
unsigned short cur_e_lb_no;

__u32 get_storage_type_from_init(void)
{
	return storage_type;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
struct _nand_info *NandHwInit(void)
{
	struct _nand_info *nand_info_temp;

	if (get_storage_type() == 1) {
		nand_info_temp = RawNandHwInit();
	} else if (get_storage_type() == 2) {
		nand_info_temp = spinand_hardware_init();
	} else {
		storage_type = 1;
		nand_info_temp = RawNandHwInit();
		if (nand_info_temp == NULL) {
			storage_type = 2;
			nand_info_temp = spinand_hardware_init();
		}
	}

	return nand_info_temp;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
int NandHwExit(void)
{
	int ret = 0;

	if (get_storage_type() == 1) {
		ret = RawNandHwExit();
	} else if (get_storage_type() == 2) {
		ret = spinand_hardware_exit();
	} else {
		RawNandHwExit();
		spinand_hardware_exit();
	}

	return ret;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
int NandHwSuperStandby(void)
{
	int ret = 0;

	if (get_storage_type() == 1) {
		ret = RawNandHwSuperStandby();
	} else if (get_storage_type() == 2) {
		ret = 0; //spi host layer achieve
	}

	return ret;
}
/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
int NandHwSuperResume(void)
{
	int ret = 0;

	if (get_storage_type() == 1) {
		ret = RawNandHwSuperResume();
	} else if (get_storage_type() == 2) {
		ret = 0; //spi host layer achieve
	}

	return ret;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
int NandHwNormalStandby(void)
{
	int ret = 0;

	if (get_storage_type() == 1) {
		ret = RawNandHwNormalStandby();
	} else if (get_storage_type() == 2) {
		ret = 0; //spi host layer achieve
	}

	return ret;
}
/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
int NandHwNormalResume(void)
{
	int ret = 0;

	if (get_storage_type() == 1) {
		ret = RawNandHwNormalResume();
	} else if (get_storage_type() == 2) {
		ret = 0; /*spi host layer achieve resume*/
	}

	return ret;
}
/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
int NandHwShutDown(void)
{
	int ret = 0;

	if (get_storage_type() == 1) {
		ret = RawNandHwShutDown();
	} else if (get_storage_type() == 2) {
		ret = 0; /*spi host layer achieve suspend*/
	}

	return ret;
}
#if 1
/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
void *nand_get_channel_base_addr(u32 no)
{
	if (get_storage_type() == 1) {
		if (no != 0)
			return (void *)RAWNAND_GetIOBaseAddrCH1();
		else
			return (void *)RAWNAND_GetIOBaseAddrCH0();
	} else if (get_storage_type() == 2) {
		return NULL;
	}
	return NULL;
}
#endif
/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
u32 NAND_GetLogicPageSize(void)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (nand_use_ubi())
		return 16384;
#endif

	return (nand_get_super_chip_page_size() << 9);
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
int nand_physic_erase_block(unsigned int chip, unsigned int block)
{
	if (get_storage_type() == 1)
		return rawnand_physic_erase_block(chip, block);
	else if (get_storage_type() == 2)
		return spinand_nftl_erase_single_block(chip, block);
	return 0;
}

/**
 * nand_physic_read_boot0_page:
 * @chip : 0
 * @block: 0~4/0~8
 * @bitmap: not care
 * @mbuf: pagesize
 * @sbuf: 64
 */
int nand_physic_read_boot0_page(unsigned int chip, unsigned int block, unsigned int page, unsigned int bitmap, unsigned char *mbuf, unsigned char *sbuf)
{
	if (get_storage_type() == 1)
		return rawnand_physic_read_boot0_page(chip, block, page, bitmap, mbuf, sbuf);
	else if (get_storage_type() == 2)
		return 0;
	return 0;
}
/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
int nand_physic_read_page(unsigned int chip, unsigned int block, unsigned int page, unsigned int bitmap, unsigned char *mbuf, unsigned char *sbuf)
{
	if (get_storage_type() == 1)
		return rawnand_physic_read_page(chip, block, page, bitmap, mbuf, sbuf);
	else if (get_storage_type() == 2)
		return spinand_nftl_read_single_page(chip, block, page, bitmap, mbuf, sbuf);
	return 0;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
int nand_physic_write_page(unsigned int chip, unsigned int block, unsigned int page, unsigned int bitmap, unsigned char *mbuf, unsigned char *sbuf)
{
	if (get_storage_type() == 1)
		return rawnand_physic_write_page(chip, block, page, bitmap, mbuf, sbuf);
	else if (get_storage_type() == 2)
		return spinand_nftl_write_single_page(chip, block, page, bitmap, mbuf, sbuf);
	return 0;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
int nand_physic_bad_block_check(unsigned int chip, unsigned int block)
{
	if (get_storage_type() == 1)
		return rawnand_physic_bad_block_check(chip, block);
	else if (get_storage_type() == 2)
		return spinand_nftl_single_badblock_check(chip, block);
	return 0;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
int nand_physic_bad_block_mark(unsigned int chip, unsigned int block)
{
	if (get_storage_type() == 1)
		return rawnand_physic_bad_block_mark(chip, block);
	else if (get_storage_type() == 2)
		return spinand_nftl_single_badblock_mark(chip, block);
	return 0;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
int nand_physic_block_copy(unsigned int chip_s, unsigned int block_s, unsigned int chip_d, unsigned int block_d, unsigned int copy_nums)
{
	if (get_storage_type() == 1)
		return rawnand_physic_block_copy(chip_s, block_s, chip_d, block_d, copy_nums);
	else if (get_storage_type() == 2)
		return spinand_nftl_single_block_copy(chip_s, block_s, chip_d, block_d);
	return 0;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
int read_virtual_page(unsigned int nDieNum, unsigned int nBlkNum, unsigned int nPage, uint64 SectBitmap, void *pBuf, void *pSpare)
{
	if (get_storage_type() == 1)
		return rawnand_physic_read_super_page(nDieNum, nBlkNum, nPage, SectBitmap, pBuf, pSpare);
	else if (get_storage_type() == 2)
		return spinand_nftl_read_super_page(nDieNum, nBlkNum, nPage, SectBitmap, pBuf, pSpare);
	return 0;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
int write_virtual_page(unsigned int nDieNum, unsigned int nBlkNum, unsigned int nPage, uint64 SectBitmap, void *pBuf, void *pSpare)
{
	if (get_storage_type() == 1)
		return rawnand_physic_write_super_page(nDieNum, nBlkNum, nPage, SectBitmap, pBuf, pSpare);
	else if (get_storage_type() == 2)
		return spinand_nftl_write_super_page(nDieNum, nBlkNum, nPage, SectBitmap, pBuf, pSpare);
	return 0;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
int erase_virtual_block(unsigned int nDieNum, unsigned int nBlkNum)
{
	if (get_storage_type() == 1)
		return rawnand_physic_erase_super_block(nDieNum, nBlkNum);
	else if (get_storage_type() == 2)
		return spinand_nftl_erase_super_block(nDieNum, nBlkNum);
	return 0;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
int virtual_badblock_check(unsigned int nDieNum, unsigned int nBlkNum)
{
	if (get_storage_type() == 1)
		return rawnand_physic_super_bad_block_check(nDieNum, nBlkNum);
	else if (get_storage_type() == 2)
		return spinand_nftl_super_badblock_check(nDieNum, nBlkNum);
	return 0;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : 0:ok  -1:fail
*Note         :
*****************************************************************************/
int virtual_badblock_mark(unsigned int nDieNum, unsigned int nBlkNum)
{
	if (get_storage_type() == 1)
		return rawnand_physic_super_bad_block_mark(nDieNum, nBlkNum);
	else if (get_storage_type() == 2)
		return spinand_nftl_super_badblock_mark(nDieNum, nBlkNum);
	return 0;
}

__u32 nand_get_lsb_block_size(void)
{
	if (get_storage_type() == 1)
		return rawnand_get_lsb_block_size();
	else if (get_storage_type() == 2)
		return spinand_nftl_get_single_block_size(BYTE);
	return 0;
}

__u32 nand_get_lsb_pages(void)
{
	if (get_storage_type() == 1)
		return rawnand_get_lsb_pages();
	else if (get_storage_type() == 2)
		return spinand_nftl_get_single_block_size(PAGE);
	return 0;
}

__u32 nand_get_phy_block_size(void)
{
	if (get_storage_type() == 1)
		return rawnand_get_phy_block_size();
	else if (get_storage_type() == 2)
		return spinand_nftl_get_single_block_size(BYTE);
	return 0;
}

__u32 nand_used_lsb_pages(void)
{
	if (get_storage_type() == 1)
		return rawnand_used_lsb_pages();
	else if (get_storage_type() == 2)
		return spinand_lsb_page_cnt_per_block();
	return 0;
}

u32 nand_get_pageno(u32 lsb_page_no)
{
	if (get_storage_type() == 1)
		return rawnand_get_pageno(lsb_page_no);
	else if (get_storage_type() == 2)
		return spinand_get_page_num(lsb_page_no);
	return 0;
}

__u32 nand_get_page_cnt_per_block(void)
{
	if (get_storage_type() == 1)
		return rawnand_get_page_cnt_per_block();
	else if (get_storage_type() == 2)
		return spinand_nftl_get_single_block_size(PAGE);
	return 0;
}

__u32 nand_get_pae_size(void)
{
	if (get_storage_type() == 1)
		return rawnand_get_pae_size();
	else if (get_storage_type() == 2)
		return spinand_nftl_get_single_page_size(BYTE);
	return 0;
}

__u32 nand_get_twoplane_flag(void)
{
	if (get_storage_type() == 1)
		return rawnand_get_twoplane_flag();
	else if (get_storage_type() == 2)
		return spinand_nftl_get_multi_plane_flag();
	return 0;
}

unsigned int nand_get_muti_program_flag(void)
{
	if (get_storage_type() == 1) {
		if (!g_nsi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_muti_program_flag(g_nsi->nci);
	 } else if (get_storage_type() == 2)
		return 0;
	return 0;
}

unsigned int nand_get_support_v_interleave_flag(void)
{
	if (get_storage_type() == 1) {
		if (!g_nssi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_support_v_interleave_flag(g_nssi->nsci);
	} else if (get_storage_type() == 2)
		return 0;
	return 0;
}

char *nand_get_chip_name(void)
{
	if (get_storage_type() == 1) {
		if (!g_nsi) {
			printf("rawnand hw not init\n");
			return NULL;
		}
		return rawnand_get_chip_name(g_nsi->nci);
	} else if (get_storage_type() == 2)
		return NULL;

	return NULL;
}

void nand_get_chip_id(unsigned char *id, int cnt)
{
	if (get_storage_type() == 1) {
		if (!g_nsi) {
			printf("rawnand hw not init\n");
			return;
		}
		return rawnand_get_chip_id(g_nsi->nci, id, cnt);
	} else if (get_storage_type() == 2)
		return;
	return;
}

unsigned int nand_get_chip_die_cnt(void)
{
	if (get_storage_type() == 1) {
		if (!g_nsi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_chip_die_cnt(g_nsi->nci);
	} else if (get_storage_type() == 2)
		return 0;
	return 0;
}


int nand_get_chip_page_size(enum size_type type)
{
	if (get_storage_type() == 1) {
		if (!g_nsi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_chip_page_size(g_nsi->nci, type);
	} else if (get_storage_type() == 2)
		return 0;

	return 0;
}

int nand_get_chip_block_size(enum size_type type)
{
	if (get_storage_type() == 1) {
		if (!g_nsi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_chip_block_size(g_nsi->nci, type);
	} else if (get_storage_type() == 2)
		return 0;

	return 0;
}

int nand_get_chip_die_size(enum size_type type)
{
	if (get_storage_type() == 1) {
		if (!g_nsi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_chip_die_size(g_nsi->nci, type);
	} else if (get_storage_type() == 2)
		return 0;

	return 0;
}

unsigned long long nand_get_chip_opt(void)
{
	if (get_storage_type() == 1) {
		if (!g_nsi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_chip_opt(g_nsi->nci);
	} else if (get_storage_type() == 2)
		return 0;

	return 0;
}

unsigned int nand_get_chip_ddr_opt(void)
{
	if (get_storage_type() == 1) {
		if (!g_nsi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_chip_ddr_opt(g_nsi->nci);
	} else if (get_storage_type() == 2)
		return 0;

	return 0;
}
unsigned int nand_get_chip_ecc_mode(void)
{
	if (get_storage_type() == 1) {
		if (!g_nsi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_chip_ecc_mode(g_nsi->nci);
	} else if (get_storage_type() == 2)
		return 0;

	return 0;
}

unsigned int nand_get_chip_freq(void)
{
	if (get_storage_type() == 1) {
		if (!g_nsi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_chip_freq(g_nsi->nci);
	} else if (get_storage_type() == 2)
		return 0;

	return 0;
}

unsigned int nand_get_chip_cnt(void)
{
	if (get_storage_type() == 1) {
		if (!g_nsi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_chip_cnt(g_nsi->nci);
	} else if (get_storage_type() == 2)
		return 0;

	return 0;
}

unsigned int nand_get_chip_multi_plane_block_offset(void)
{
	if (get_storage_type() == 1) {
		if (!g_nsi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_chip_multi_plane_block_offset(g_nsi->nci);
	} else if (get_storage_type() == 2)
		return 0;

	return 0;
}

unsigned int nand_get_super_chip_cnt(void)
{
	if (get_storage_type() == 1) {
		if (!g_nssi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_super_chip_cnt(g_nssi->nsci);
	} else if (get_storage_type() == 2)
		return 0;

	return 0;
}

/**
 * nand_get_super_chip_page_size:
 *
 * @return page size in sector
 */
unsigned int nand_get_super_chip_page_size(void)
{
	if (get_storage_type() == 1) {
		if (!g_nssi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_super_chip_page_size(g_nssi->nsci);
	} else if (get_storage_type() == 2)
		return 0;

	return 0;
}

/**
 * nand_get_super_chip_spare_size:
 *
 * @return the super chip spare size
 */
unsigned int nand_get_super_chip_spare_size(void)
{
	if (get_storage_type() == 1) {
		if (!g_nssi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_super_chip_spare_size(g_nssi->nsci);
	} else if (get_storage_type() == 2)
		return 0;

	return 0;
}
/**
 * nand_get_super_chip_block_size:
 *
 * @return block size in page
 */
unsigned int nand_get_super_chip_block_size(void)
{
	if (get_storage_type() == 1) {
		if (!g_nssi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_super_chip_block_size(g_nssi->nsci);
	} else if (get_storage_type() == 2)
		return 0;

	return 0;
}

/**
 * nand_get_super_chip_page_offset_to_block:
 *
 * @return the page offset for next super block
 */
unsigned int nand_get_super_chip_pages_offset_to_block(void)
{
	if (get_storage_type() == 1) {
		if (!g_nssi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_super_chip_page_offset_to_block(g_nssi->nsci);
	} else if (get_storage_type() == 2)
		return 0;

	return 0;
}

/**
 * nand_get_support_dual_channel:
 *
 * @return 1 if support dual channel ,otherwise return 0
 */
unsigned int nand_get_support_dual_channel(void)
{
	if (get_storage_type() == 1) {
		if (!g_nssi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_support_dual_channel();
	} else if (get_storage_type() == 2)
		return 0;

	return 0;
}

/**
 * nand_get_super_chip_size:
 *
 * @return super chip size in block
 */
unsigned int nand_get_super_chip_size(void)
{
	if (get_storage_type() == 1) {
		if (!g_nssi) {
			printf("rawnand hw not init\n");
			return 0;
		}
		return rawnand_get_super_chip_size(g_nssi->nsci);
	} else if (get_storage_type() == 2)
		return 0;

	return 0;
}

/**
 * nand_chips_init: rawnand physic init
 *
 * @chip: chip info
 * @info: nand info
 *
 *
 */
static int nand_chips_init(struct nand_chip_info *chip)
{
	return 0;
}

/**
 * nand_chips_cleanup: rawnand exit
 *
 * @chip: chip info
 *
 *
 */
static void nand_chips_cleanup(struct nand_chip_info *chip)
{
	return;
}

#ifdef SUPPORT_SUPER_STANDBY
/**
 * nand_chips_super_standby: rawnand super standby
 *
 * @chip: chip info
 *
 *
 */
static int nand_chips_super_standby(struct nand_chip_info *chip)
{
	return 0;
}

/**
 * nand_chips_super_resume: rawnand super resume
 *
 * @chip: chip info
 *
 *
 */
static int nand_chips_super_resume(struct nand_chip_info *chip)
{
	return 0;
}

#else
/**
 * nand_chips_normal_standby: rawnand normal standby
 *
 * @chip: chip info
 *
 *
 */
static int nand_chips_normal_standby(struct nand_chip_info *chip)
{
	return 0;
}

/**
 * nand_chips_normal_resume: rawnand normal resume
 *
 * @chip: chip info
 *
 *
 */
static int nand_chips_normal_resume(struct nand_chip_info *chip)
{
	return 0;
}
#endif

struct nand_chips_ops chips_ops = {
    .nand_chips_init = nand_chips_init,
    .nand_chips_cleanup = nand_chips_cleanup,
#ifdef SUPPORT_SUPER_STANDBY
    .nand_chips_standby = nand_chips_super_standby,
    .nand_chips_resume = nand_chips_super_resume,
#else
    .nand_chips_standby = nand_chips_normal_standby,
    .nand_chips_resume = nand_chips_normal_resume,
#endif
};
