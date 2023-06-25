//SPDX-License-Identifier:	GPL-2.0+
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
#define _UBOOTT_C_

#include "nand_physic_inc.h"

extern int nand_secure_storage_block_bak;
extern int nand_secure_storage_block;
extern __u32 NAND_GetLsbPages(void);
extern __u32 NAND_UsedLsbPages(void);

extern int printf(const char *fmt, ...);
/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_write_nboot_data(unsigned char *buf, unsigned int len)
{
	int ret = 1;
	unsigned int i;
	/*unsigned char *mbuf;*/

	PHY_ERR("burn boot0 normal mode!\n");

	for (i = 0; i < aw_nand_info.boot->uboot_start_block; i++) {
		ret &= g_nsi->nci->nand_write_boot0_one(buf, len, i);
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
int nand_read_nboot_data(unsigned char *buf, unsigned int len)
{
	int ret;
	unsigned int i;

	PHY_ERR("read boot0 normal mode!\n");

	for (i = 0; i < NAND_BOOT0_BLK_CNT; i++) {
		ret = g_nsi->nci->nand_read_boot0_one(buf, len, i);
		if (ret == 0) {
			return 0;
		} else {
			PHY_ERR("read boot0 one fail %d!\n", i);
		}
	}

	PHY_ERR("read boot0 fail!\n");
	return ERR_NO_106;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int check_nboot_table(unsigned char *buf, unsigned int len,
		unsigned int counter)
{
	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_check_nboot(unsigned char *buf, unsigned int len)
{
	int i;
	int num = 0;
	int flag[NAND_BOOT0_BLK_CNT];
	int table_flag[NAND_BOOT0_BLK_CNT];

	for (i = 0; i < NAND_BOOT0_BLK_CNT; i++) {
		table_flag[i] = check_nboot_table(buf, len, i);
		flag[i]       = g_nsi->nci->nand_read_boot0_one(buf, len, i);
	}

	if (flag[NAND_BOOT0_BLK_CNT - 1] != 0) {
		for (i = 0; i < NAND_BOOT0_BLK_CNT; i++) {
			if (flag[i] == 0) {
				if (g_nsi->nci->nand_read_boot0_one(buf, len,
							i) == 0) {
					num = 1;
					break;
				}
			}
		}

		if (num == 0) {
			PHY_ERR("no boot0 ok!\n");
			return ERR_NO_102;
		}
	}

	for (i = 0; i < NAND_BOOT0_BLK_CNT; i++) {
		if (table_flag[i] == -1) {
			flag[i] = -1;
		}
	}

	if (flag[0] > 0) {
		flag[0] = 0;
	}

	for (i = 0; i < NAND_BOOT0_BLK_CNT; i++) {
		if (flag[i] != 0) {
			PHY_ERR("recover boot0 %d!\n", i);
			g_nsi->nci->nand_write_boot0_one(buf, len, i);
		}
	}
	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_write_uboot_one_in_block(unsigned char *buf, unsigned int len,
		struct _boot_info *info_buf,
		unsigned int info_len, unsigned int counter)
{
	int ret;
	unsigned int j, boot_len, page_index, t_len;
	unsigned char oob_buf[64];
	struct _nand_physic_op_par lnpo;
	struct _nand_chip_info *nci;
	int uboot_flag, info_flag;
	unsigned char *kernel_buf;
	unsigned char *ptr;

	PHY_ERR("burn uboot in one block!\n");

	nci = g_nctri->nci;

	uboot_flag = 0;
	info_flag  = 0;
	boot_len   = 0;
	t_len      = 0;

	kernel_buf = nand_get_temp_buf(nci->sector_cnt_per_page << 9);

	MEMSET(oob_buf, 0xff, 64);

	nand_get_version(oob_buf);
	if ((oob_buf[0] != 0xff) || (oob_buf[1] != 0x00)) {
		PHY_ERR("get flash driver version error!");
		nand_free_temp_buf(kernel_buf, nci->sector_cnt_per_page << 9);
		return ERR_NO_105;
	}
	/*
	   if(len%(nci->sector_cnt_per_page<<9))
	   {
	   PHY_ERR("uboot length check error!\n");
	   nand_free_temp_buf(kernel_buf,nci->sector_cnt_per_page<<9);
	   return ERR_NO_104;
	   }
	 */
	lnpo.chip  = 0;
	lnpo.block = info_buf->uboot_start_block + counter;
	lnpo.page  = 0;

	nand_wait_all_rb_ready();

	ret = nci->nand_physic_erase_block(&lnpo);
	if (ret) {
		PHY_ERR("Fail in erasing block %d!\n", lnpo.block);
		//return -1;
	}
	page_index = 0;
	for (j = 0; j < nci->page_cnt_per_blk; j++) {
		lnpo.chip  = 0;
		lnpo.block = info_buf->uboot_start_block + counter;
		if (0 == nci->is_lsb_page(j)) {
			continue;
		}
		lnpo.page	= j;
		lnpo.sect_bitmap = nci->sector_cnt_per_page;
		lnpo.sdata       = oob_buf;
		lnpo.slen	= 64;

		if (uboot_flag == 0) {
			boot_len = page_index * (nci->sector_cnt_per_page << 9);
			MEMCPY(kernel_buf, buf + boot_len,
					nci->sector_cnt_per_page << 9);
			ptr = kernel_buf;
			if (((len - boot_len) <=
						(nci->sector_cnt_per_page << 9)) &&
					((len - boot_len) > 0)) {
				uboot_flag = page_index + 1;
			}
		} else if (info_flag == 0) {
			PHY_ERR("uboot info: page %d in block %d.\n", lnpo.page,
					lnpo.block);
			t_len = (page_index - uboot_flag) *
				(nci->sector_cnt_per_page << 9);
			ptr = (unsigned char *)info_buf;
			ptr += t_len;
			if (((info_len - t_len) <=
						(nci->sector_cnt_per_page << 9)) &&
					((info_len - t_len) > 0)) {
				info_flag = page_index;
			}
		} else {
			ptr = kernel_buf;
		}

		lnpo.mdata = ptr;

		nand_wait_all_rb_ready();

		if (nci->nand_physic_write_page(&lnpo) != 0) {
			PHY_ERR("Warning. Fail in writing page %d in block %d.\n",
					lnpo.page, lnpo.block);
		}
		page_index++;
	}

	nand_wait_all_rb_ready();
	nand_free_temp_buf(kernel_buf, nci->sector_cnt_per_page << 9);
	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_write_uboot_one_in_many_block(unsigned char *buf, unsigned int len,
		struct _boot_info *info_buf,
		unsigned int info_len,
		unsigned int counter)
{
	int ret;
	unsigned int j, boot_len, page_index, t_len, total_pages,
		     pages_per_block, good_block_offset, blocks_one_uboot,
		     uboot_block_offset, m;
	unsigned int write_blocks;
	unsigned char oob_buf[64];
	struct _nand_physic_op_par lnpo;
	struct _nand_chip_info *nci;
	int uboot_flag, info_flag;
	unsigned char *kernel_buf;
	unsigned char *ptr;

	PHY_ERR("burn uboot in many block %d!\n", counter);

	nci = g_nctri->nci;

	uboot_flag = 0;
	info_flag  = 0;
	boot_len   = 0;
	t_len      = 0;

	kernel_buf = nand_get_temp_buf(nci->sector_cnt_per_page << 9);

	MEMSET(oob_buf, 0xff, 64);

	nand_get_version(oob_buf);
	if ((oob_buf[0] != 0xff) || (oob_buf[1] != 0x00)) {
		PHY_ERR("get flash driver version error!");
		nand_free_temp_buf(kernel_buf, nci->sector_cnt_per_page << 9);
		return ERR_NO_105;
	}
	/*
	   if(len%(nci->sector_cnt_per_page<<9))
	   {
	   PHY_ERR("uboot length check error!\n");
	   nand_free_temp_buf(kernel_buf,nci->sector_cnt_per_page<<9);
	   return ERR_NO_104;
	   }

	   total_len = len + info_len;
	   if(total_len%(nci->sector_cnt_per_page<<9))
	   {
	   PHY_ERR("uboot length check error!\n");
	   nand_free_temp_buf(kernel_buf,nci->sector_cnt_per_page<<9);
	   return ERR_NO_104;
	   }
	 */
	total_pages = (len + (nci->sector_cnt_per_page << 9) - 1) /
		(nci->sector_cnt_per_page << 9);
	total_pages += (info_len + (nci->sector_cnt_per_page << 9) - 1) /
		(nci->sector_cnt_per_page << 9);
	pages_per_block  = NAND_GetLsbPages();
	blocks_one_uboot = total_pages / pages_per_block;

	if (total_pages % pages_per_block) {
		blocks_one_uboot++;
	}

	good_block_offset = blocks_one_uboot * counter;

	for (m = 0, j = info_buf->uboot_start_block;
			j < info_buf->uboot_next_block; j++) {
		if (m == good_block_offset) {
			break;
		}
		lnpo.chip	= 0;
		lnpo.block       = j;
		lnpo.page	= 0;
		lnpo.sect_bitmap = 0;
		lnpo.mdata       = NULL;
		lnpo.sdata       = NULL;
		ret		 = nci->nand_physic_bad_block_check(&lnpo);
		if (ret == 0) {
			m++;
		}
	}

	uboot_block_offset = j;

	if ((uboot_block_offset + blocks_one_uboot) >
			info_buf->uboot_next_block) {
		nand_free_temp_buf(kernel_buf, nci->sector_cnt_per_page << 9);
		return 0;
	}

	/////////////////////////////////////////////////////

	uboot_flag   = 0;
	info_flag    = 0;
	boot_len     = 0;
	page_index   = 0;
	write_blocks = 0;

	for (j = uboot_block_offset; j < info_buf->uboot_next_block; j++) {
		lnpo.chip  = 0;
		lnpo.block = j;
		lnpo.page  = 0;
		nand_wait_all_rb_ready();
		ret = nci->nand_physic_bad_block_check(&lnpo);
		if (ret != 0) {
			continue;
		}

		ret = nci->nand_physic_erase_block(&lnpo);
		if (ret != 0) {
			PHY_ERR("Fail in erasing block %d!\n", lnpo.block);
			//return ret;
		}

		write_blocks++;

		PHY_ERR("write uboot many block %d!\n", lnpo.block);

		for (m = 0; m < nci->page_cnt_per_blk; m++) {
			lnpo.chip	= 0;
			lnpo.block       = j;
			lnpo.page	= m;
			lnpo.sect_bitmap = nci->sector_cnt_per_page;
			lnpo.sdata       = oob_buf;
			lnpo.slen	= 64;
			if (nci->is_lsb_page(m) != 0) {
				if (uboot_flag == 0) {
					boot_len =
						page_index *
						(nci->sector_cnt_per_page << 9);
					MEMCPY(kernel_buf, buf + boot_len,
							nci->sector_cnt_per_page << 9);
					ptr = kernel_buf;
					if (((len - boot_len) <=
								(nci->sector_cnt_per_page << 9)) &&
							((len - boot_len) > 0)) {
						uboot_flag = page_index + 1;
					}
				} else if (info_flag == 0) {
					PHY_ERR("uboot info: page %d in block %d.\n",
							lnpo.page, lnpo.block);
					t_len = (page_index - uboot_flag) *
						(nci->sector_cnt_per_page << 9);
					ptr = (unsigned char *)info_buf;
					ptr += t_len;
					if (((info_len - t_len) <=
								(nci->sector_cnt_per_page << 9)) &&
							((info_len - t_len) > 0)) {
						info_flag = page_index;
					}
				} else {
					ptr = kernel_buf;
				}

				lnpo.mdata = ptr;
				nand_wait_all_rb_ready();
				if (nci->nand_physic_write_page(&lnpo) != 0) {
					PHY_ERR("Warning. Fail in writing page %d in block %d.\n",
							lnpo.page, lnpo.block);
				}
				page_index++;
			}
		}

		if (blocks_one_uboot == write_blocks) {
			break;
		}
	}
	nand_wait_all_rb_ready();
	nand_free_temp_buf(kernel_buf, nci->sector_cnt_per_page << 9);
	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_write_uboot_one(unsigned char *buf, unsigned int len,
		struct _boot_info *info_buf, unsigned int info_len,
		unsigned int counter)
{
	int ret;
	struct _nand_chip_info *nci;
	int real_len;

	PHY_ERR("burn uboot one!\n");

	nci = g_nctri->nci;

	real_len = add_len_to_uboot_tail(len);

	cal_sum_physic_info(phyinfo_buf, PHY_INFO_SIZE);

	//print_physic_info(phyinfo_buf);

	if (((0 == nci->lsb_page_type) && (real_len <= NAND_GetPhyblksize())) ||
			((0 != nci->lsb_page_type) && (real_len <= NAND_GetLsbblksize()))) {
		ret = nand_write_uboot_one_in_block(buf, len, phyinfo_buf,
				PHY_INFO_SIZE, counter);
	} else {
		ret = nand_write_uboot_one_in_many_block(
				buf, len, phyinfo_buf, PHY_INFO_SIZE, counter);
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
int nand_write_uboot_data(unsigned char *buf, unsigned int len)
{
	int ret = 0;
	unsigned int i;
	unsigned int uboot_block_cnt;

	uboot_block_cnt = aw_nand_info.boot->uboot_next_block -
		aw_nand_info.boot->uboot_start_block;

	for (i = 0; i < uboot_block_cnt; i++) {
		ret &= nand_write_uboot_one(buf, len, phyinfo_buf,
				PHY_INFO_SIZE, i);
	}

	return ret;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail 1: ecc limit
 *Note         :
 *****************************************************************************/
int nand_read_uboot_one_in_block(unsigned char *buf, unsigned int len,
		unsigned int counter)
{
	int ret = 0, data_error = 0, ecc_limit = 0;
	unsigned int j, page_index, total_pages;
	unsigned char oob_buf[64];
	struct _nand_physic_op_par lnpo;
	struct _nand_chip_info *nci;
	/*int uboot_flag;*/
	unsigned char *ptr;

	PHY_DBG("read uboot in one block %d!\n", counter);

	nci = g_nctri->nci;

	/*uboot_flag = 0;*/
	/*info_flag  = 0;*/
	/*boot_len   = 0;*/
	/*t_len      = 0;*/

	ptr = nand_get_temp_buf(nci->sector_cnt_per_page << 9);

	MEMSET(oob_buf, 0xff, 64);

	//	if(len%(nci->sector_cnt_per_page<<9))
	//	{
	//		PHY_ERR("uboot length check error!\n");
	//		nand_free_temp_buf(ptr,nci->sector_cnt_per_page<<9);
	//		return 0;
	//	}

	total_pages = len / (nci->sector_cnt_per_page << 9);
	if (len % (nci->sector_cnt_per_page << 9))
		total_pages++;

	for (j = 0, page_index = 0; j < nci->page_cnt_per_blk; j++) {
		lnpo.chip  = 0;
		lnpo.block = phyinfo_buf->uboot_start_block + counter;
		if (0 == nci->is_lsb_page(j)) {
			continue;
		}
		lnpo.page	= j;
		lnpo.sect_bitmap = nci->sector_cnt_per_page;
		lnpo.sdata       = oob_buf;
		lnpo.slen	= 64;
		lnpo.mdata       = ptr;

		nand_wait_all_rb_ready();

		ret = nci->nand_physic_read_page(&lnpo);
		if (ret == 0) {
			;
		} else if (ret == ECC_LIMIT) {
			ecc_limit = 1;
			PHY_ERR("Warning. Fail in read page %d in block %d \n",
					lnpo.page, lnpo.block);
		} else {
			data_error = 1;
			break;
		}

		if ((oob_buf[0] != 0xff) || (oob_buf[1] != 0x00)) {
			PHY_ERR("get uboot flash driver version error!\n");
			data_error = 1;
			break;
		}

		MEMCPY(buf + page_index * (nci->sector_cnt_per_page << 9), ptr,
				nci->sector_cnt_per_page << 9);

		page_index++;

		if (total_pages == page_index) {
			break;
		}
	}

	nand_wait_all_rb_ready();
	nand_free_temp_buf(ptr, nci->sector_cnt_per_page << 9);

	if (data_error == 1) {
		ret = -1;
	} else if (ecc_limit == 1) {
		ret = 1;
	} else {
		ret = 0;
	}
	return ret;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail  1: ecc limit
 *Note         :
 *****************************************************************************/
int nand_read_uboot_one_in_many_block(unsigned char *buf, unsigned int len,
		unsigned int counter)
{
	int ret = 0, data_error = 0, ecc_limit = 0;
	unsigned int j, page_index, total_len, total_pages, pages_per_block,
		     good_block_offset, blocks_one_uboot, uboot_block_offset, m;
	unsigned char oob_buf[64];
	struct _nand_physic_op_par lnpo;
	struct _nand_chip_info *nci;
	/*int uboot_flag, info_flag;*/
	unsigned char *kernel_buf;
	/*unsigned char *ptr;*/

	PHY_DBG("read uboot in many block %d!\n", counter);

	nci = g_nctri->nci;

	/*uboot_flag = 0;*/
	/*info_flag  = 0;*/
	/*boot_len   = 0;*/
	/*t_len      = 0;*/

	kernel_buf = nand_get_temp_buf(nci->sector_cnt_per_page << 9);

	MEMSET(oob_buf, 0xff, 64);

	nand_get_version(oob_buf);
	if ((oob_buf[0] != 0xff) || (oob_buf[1] != 0x00)) {
		PHY_ERR("get flash driver version error!");
		nand_free_temp_buf(kernel_buf, nci->sector_cnt_per_page << 9);
		return ERR_NO_105;
	}

	//	if(len%(nci->sector_cnt_per_page<<9))
	//	{
	//		PHY_ERR("uboot length check error!\n");
	//		nand_free_temp_buf(kernel_buf,nci->sector_cnt_per_page<<9);
	//		return ERR_NO_104;
	//	}

	total_len   = len;
	total_pages = total_len / (nci->sector_cnt_per_page << 9);
	if (total_len % (nci->sector_cnt_per_page << 9))
		total_pages++;

	pages_per_block  = NAND_GetLsbPages();
	blocks_one_uboot = total_pages / pages_per_block;

	if (total_pages % pages_per_block) {
		blocks_one_uboot++;
	}

	good_block_offset = blocks_one_uboot * counter;

	for (m = 0, j = phyinfo_buf->uboot_start_block;
			j < phyinfo_buf->uboot_next_block; j++) {
		if (m == good_block_offset) {
			break;
		}
		lnpo.chip	= 0;
		lnpo.block       = j;
		lnpo.page	= 0;
		lnpo.sect_bitmap = 0;
		lnpo.mdata       = NULL;
		lnpo.sdata       = NULL;
		ret		 = nci->nand_physic_bad_block_check(&lnpo);
		if (ret == 0) {
			m++;
		}
	}

	uboot_block_offset = j;

	if ((uboot_block_offset + blocks_one_uboot) >
			phyinfo_buf->uboot_next_block) {
		nand_free_temp_buf(kernel_buf, nci->sector_cnt_per_page << 9);
		PHY_ERR("ERROR! More than the location of storage uboot");
		return -1;
	}

	/////////////////////////////////////////////////////

	/*uboot_flag = 0;*/
	/*info_flag  = 0;*/
	/*boot_len   = 0;*/
	page_index = 0;

	for (j = uboot_block_offset; j < phyinfo_buf->uboot_next_block; j++) {
		lnpo.chip  = 0;
		lnpo.block = j;
		lnpo.page  = 0;
		nand_wait_all_rb_ready();
		ret = nci->nand_physic_bad_block_check(&lnpo);
		if (ret != 0) {
			continue;
		}

		PHY_DBG("read uboot many block %d!\n", lnpo.block);

		for (m = 0; m < nci->page_cnt_per_blk; m++) {
			lnpo.chip	= 0;
			lnpo.block       = j;
			lnpo.page	= m;
			lnpo.sect_bitmap = nci->sector_cnt_per_page;
			lnpo.sdata       = oob_buf;
			lnpo.slen	= 64;
			if (nci->is_lsb_page(m) != 0) {
				lnpo.mdata = kernel_buf;
				nand_wait_all_rb_ready();
				ret = nci->nand_physic_read_page(&lnpo);
				if (ret == 0) {
					;
				} else if (ret == ECC_LIMIT) {
					ecc_limit = 1;
					PHY_ERR("Warning. Fail in read page %d in block %d \n",
							lnpo.page, lnpo.block);
				} else {
					PHY_ERR("error read page: %d in block %d \n",
							lnpo.page, lnpo.block);
					data_error = 1;
					break;
				}

				if ((oob_buf[0] != 0xff) ||
						(oob_buf[1] != 0x00)) {
					PHY_ERR("get uboot flash driver version error!\n");
					data_error = 1;
					break;
				}

				MEMCPY(buf +
						page_index *
						(nci->sector_cnt_per_page
						 << 9),
						lnpo.mdata,
						nci->sector_cnt_per_page << 9);
				page_index++;
				if (total_pages == page_index) {
					break;
				}
			}
		}

		if (total_pages == page_index) {
			break;
		}
		if (data_error == 1) {
			break;
		}
	}

	nand_wait_all_rb_ready();
	nand_free_temp_buf(kernel_buf, nci->sector_cnt_per_page << 9);

	if (data_error == 1) {
		ret = -1;
	} else if (ecc_limit == 1) {
		ret = 1;
	} else {
		ret = 0;
	}
	return ret;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  <0:fail 1: ecc limit
 *Note         :
 *****************************************************************************/
int nand_read_uboot_one(unsigned char *buf, unsigned int len,
		unsigned int counter)
{
	int ret = 0;
	struct _nand_chip_info *nci;

	nci = g_nctri->nci;

	PHY_DBG("read uboot one %d!\n", counter);

	if (((0 == nci->lsb_page_type) && (len <= NAND_GetPhyblksize())) ||
			((0 != nci->lsb_page_type) && (len <= NAND_GetLsbblksize()))) {
		ret = nand_read_uboot_one_in_block(buf, len, counter);
	} else {
		ret = nand_read_uboot_one_in_many_block(buf, len, counter);
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
int nand_read_uboot_data(unsigned char *buf, unsigned int len)
{
	int ret = 0, uboot_size;
	unsigned int i;
	unsigned int uboot_block_cnt;

	uboot_size = nand_get_uboot_total_len();
	if (len >= uboot_size)
		len = uboot_size;
	else {
		PHY_ERR("Error! Read Uboot Size too Small\n");
		PHY_ERR("Size = %dByte, Uboot_Size = %dByte\n", len,
				uboot_size);
	}

	PHY_DBG("read uboot!\n");

	uboot_block_cnt = aw_nand_info.boot->uboot_next_block -
		aw_nand_info.boot->uboot_start_block;

	for (i = 0; i < uboot_block_cnt; i++) {
		ret = nand_read_uboot_one(buf, len, i);
		if (ret >= 0) {
			return 0;
		}
	}
	return -1;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         : must after uboot_info_init
 *****************************************************************************/
int nand_check_uboot(unsigned char *buf, unsigned int len)
{
	int i;
	int num = 0;
	int uboot_len;
	int counter, first_boot_flag;
	int flag[30];
	int boot_flag[30];
	//	char sbuf[64];
	struct _boot_info *boot;

	counter = uboot_info.copys;

	for (i = 0; i < counter; i++) {
		boot_flag[i] = -1;
		flag[i]      = nand_read_uboot_one(buf, len, i);
		if (flag[i] >= 0) {
			boot = (struct _boot_info
					*)(buf +
						uboot_info.byte_offset_for_nand_info);
			if (boot->magic == PHY_INFO_MAGIC) {
				boot_flag[i] = 0;
			} else {
				PHY_ERR("boot magic error %x\n", boot->magic);
			}
		}
	}

	if (flag[counter - 1] < 0) {
		for (i = 0; i < counter; i++) {
			if (flag[i] >= 0) {
				if (nand_read_uboot_one(buf, len, i) == 0) {
					num = 1;
					break;
				}
			}
		}
		if (num == 0) {
			PHY_ERR("no uboot ok\n");
			return -1;
		}
	}

	if (flag[0] > 0) {
		flag[0] = 0;
	}

	first_boot_flag = 1;
	for (i = 0; i < counter; i++) {
		if (boot_flag[i] == 0) {
			first_boot_flag = 0;
		}
	}

	if (first_boot_flag == 1) {
		PHY_ERR("first boot flag\n");
		boot = (struct _boot_info
				*)(buf + uboot_info.byte_offset_for_nand_info);
		if (boot->magic != PHY_INFO_MAGIC) {
			PHY_ERR("recover uboot data\n");
			cal_sum_physic_info(aw_nand_info.boot, PHY_INFO_SIZE);
			MEMCPY(boot, aw_nand_info.boot, PHY_INFO_SIZE);
		}
	}

	uboot_len = uboot_info.uboot_len;

	for (i = 0; i < (counter / 2); i++) {//if error; change
		if ((flag[i] < 0) || (first_boot_flag == 1)) {
			PHY_ERR("recover1  uboot %d.\n", i);
			nand_write_uboot_one(buf, uboot_len, aw_nand_info.boot,
					PHY_INFO_SIZE, i);
		} else if ((boot_flag[i] < 0) && (first_boot_flag == 0)) {
			PHY_ERR("recover2  uboot %d.\n", i);
			nand_write_uboot_one(buf, uboot_len, aw_nand_info.boot,
					PHY_INFO_SIZE, i);
		} else {
			;
		}
	}

	for (i = (counter / 2); i < counter; i++) {//if ecc limit ; change
		if ((flag[i] != 0) || (first_boot_flag == 1)) {
			PHY_ERR("recover3  uboot %d.\n", i);
			nand_write_uboot_one(buf, uboot_len, aw_nand_info.boot,
					PHY_INFO_SIZE, i);
		} else if ((boot_flag[i] < 0) && (first_boot_flag == 0)) {
			PHY_ERR("recover4  uboot %d.\n", i);
			nand_write_uboot_one(buf, uboot_len, aw_nand_info.boot,
					PHY_INFO_SIZE, i);
		} else {
			;
		}
	}

	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_get_uboot_total_len(void)
{
	int len;

	uboot_info_init(&uboot_info);

	len = uboot_info.total_len;
	if ((len == 0) || (len >= 8 * 1024 * 1024)) {
		PHY_ERR("not uboot: %d\n", len);
		return 0;
	}
	return len;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_get_nboot_total_len(void)
{
	int len = 0;

	return len;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :NAND_GetParam
 *****************************************************************************/
int nand_get_param(boot_nand_para_t *nand_param)
{
	int i;
	struct _nand_chip_info *nci;

	nci = g_nctri->nci;

	nand_param->ChannelCnt       = g_nand_storage_info->ChannelCnt;
	nand_param->ChipCnt	  = g_nand_storage_info->ChipCnt;
	nand_param->ChipConnectInfo  = g_nand_storage_info->ChipConnectInfo;
	nand_param->RbCnt	    = g_nand_storage_info->ChipConnectInfo;
	nand_param->RbConnectInfo    = g_nand_storage_info->RbConnectInfo;
	nand_param->RbConnectMode    = g_nand_storage_info->RbConnectMode;
	nand_param->BankCntPerChip   = g_nand_storage_info->BankCntPerChip;
	nand_param->DieCntPerChip    = g_nand_storage_info->DieCntPerChip;
	nand_param->PlaneCntPerDie   = g_nand_storage_info->PlaneCntPerDie;
	nand_param->SectorCntPerPage = g_nand_storage_info->SectorCntPerPage;
	nand_param->PageCntPerPhyBlk = g_nand_storage_info->PageCntPerPhyBlk;
	nand_param->BlkCntPerDie     = g_nand_storage_info->BlkCntPerDie;
	nand_param->OperationOpt     = g_nand_storage_info->OperationOpt;
	nand_param->FrequencePar     = g_nand_storage_info->FrequencePar;
	nand_param->EccMode	  = g_nand_storage_info->EccMode;
	nand_param->ValidBlkRatio    = g_nand_storage_info->ValidBlkRatio;
	nand_param->good_block_ratio = 0;
	nand_param->ReadRetryType    = g_nand_storage_info->ReadRetryType;
	nand_param->DDRType	  = g_nand_storage_info->DDRType;

	//boot0 not support DDR mode, only SDR or toggle!!!!
	if ((nand_param->DDRType == 0x2) || (nand_param->DDRType == 0x12)) {
		PHY_ERR("set ddrtype 0, freq 30Mhz\n");
		nand_param->DDRType      = 0;
		nand_param->FrequencePar = 30;
	} else if ((nci->support_toggle_only == 0) &&
			((nand_param->DDRType == 0x3) ||
			 (nand_param->DDRType == 0x13))) {
		PHY_ERR("set ddrtype 0\n");
		nand_param->DDRType      = 0;
		nand_param->FrequencePar = 30;
	} else if (nand_param->DDRType == 0x13) {
		PHY_ERR("set ddrtype 3, freq 30Mhz\n");
		nand_param->DDRType      = 3;
		nand_param->FrequencePar = 20;
	} else {
		;
	}

	for (i				  = 0; i < 8; i++)
		nand_param->NandChipId[i] = g_nand_storage_info->NandChipId[i];

	PHY_ERR("nand get param end 0x%x\n", nand_param->DDRType);
	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int NAND_UpdatePhyArch(void)
{
	int ret;
	unsigned int id_number_ctl, para;
	unsigned char *nand_permanent_data_tmp = NULL;

	nand_permanent_data_tmp = (unsigned char *)NAND_Malloc(
			sizeof(nand_permanent_data)); // for cache align
	if (nand_permanent_data_tmp == NULL) {
		PHY_ERR("nand_permanent_data_tmp malloc fail!\n");
		return -1;
	}

	g_nssi->support_two_plane    = 0;
	g_nssi->support_v_interleave = 1;
	g_nssi->support_dual_channel = 1;

	nand_permanent_data.magic_data	= MAGIC_DATA_FOR_PERMANENT_DATA;
	nand_permanent_data.support_two_plane = 0;
	nand_permanent_data.support_vertical_interleave = 1;
	nand_permanent_data.support_dual_channel	= 1;

	PHY_ERR("NAND UpdatePhyArch\n");

	g_nssi->support_two_plane = g_phy_cfg->phy_support_two_plane;
	nand_permanent_data.support_two_plane = g_nssi->support_two_plane;

	g_nssi->support_v_interleave =
		g_phy_cfg->phy_nand_support_vertical_interleave;
	nand_permanent_data.support_vertical_interleave =
		g_nssi->support_v_interleave;

	g_nssi->support_dual_channel = g_phy_cfg->phy_support_dual_channel;
	nand_permanent_data.support_dual_channel = g_nssi->support_dual_channel;

	id_number_ctl = NAND_GetNandIDNumCtrl();
	if ((id_number_ctl & 0x0e) != 0) {
		para = NAND_GetNandExtPara(1);
		if ((para != 0xffffffff) &&
				(id_number_ctl & 0x02)) {//get script success
			if (((para & 0xffffff) ==
						g_nctri->nci->npi->id_number) ||
					((para & 0xffffff) == 0xeeeeee)) {
				PHY_ERR("script support_two_plane %d\n", para);
				g_nssi->support_two_plane = (para >> 24) & 0xff;
				if (g_nssi->support_two_plane == 1) {
					g_nssi->support_two_plane = 1;
				} else {
					g_nssi->support_two_plane = 0;
				}
				nand_permanent_data.support_two_plane =
					g_nssi->support_two_plane;
			}
		}

		para = NAND_GetNandExtPara(2);
		if ((para != 0xffffffff) &&
				(id_number_ctl & 0x04)) {//get script success
			if (((para & 0xffffff) ==
						g_nctri->nci->npi->id_number) ||
					((para & 0xffffff) == 0xeeeeee)) {
				PHY_ERR("script support_v_interleave %d\n",
						para);
				g_nssi->support_v_interleave =
					(para >> 24) & 0xff;
				nand_permanent_data.support_vertical_interleave =
					g_nssi->support_v_interleave;
			}
		}

		para = NAND_GetNandExtPara(3);
		if ((para != 0xffffffff) &&
				(id_number_ctl & 0x08)) {//get script success
			if (((para & 0xffffff) ==
						g_nctri->nci->npi->id_number) ||
					((para & 0xffffff) == 0xeeeeee)) {
				PHY_ERR("script support_dual_channel %d\n",
						para);
				g_nssi->support_dual_channel =
					(para >> 24) & 0xff;
				nand_permanent_data.support_dual_channel =
					g_nssi->support_dual_channel;
			}
		}
	}

	NAND_Memcpy(nand_permanent_data_tmp, &nand_permanent_data,
			sizeof(nand_permanent_data));

	ret = set_nand_structure((void *)nand_permanent_data_tmp);

	NAND_Free(nand_permanent_data_tmp, sizeof(nand_permanent_data));
	return ret;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int nand_is_blank(void)
{
	int i, ret;
	int block_blank_flag = 1;
	int read_retry_mode;
	int start_blk = 4, blk_cnt = 30;
	unsigned char oob[64];
	struct _nand_physic_op_par npo;
	struct _nand_chip_info *nci;

	nci		= nci_get_from_nsi(g_nsi, 0);
	read_retry_mode = (nci->npi->read_retry_type >> 16) & 0xff;

	if ((read_retry_mode != 0x32) && (read_retry_mode != 0x33) &&
			(read_retry_mode != 0x34) && (read_retry_mode != 0x35))
		return 0;

	for (i = start_blk; i < (start_blk + blk_cnt); i++) {
		npo.chip	= 0;
		npo.block       = i;
		npo.page	= 0;
		npo.sect_bitmap = nci->sector_cnt_per_page;
		npo.mdata       = NULL;
		npo.sdata       = oob;
		npo.slen	= 64;

		ret = nci->nand_physic_read_page(&npo);
		if (ret >= 0) {
			if ((oob[0] == 0xff) && (oob[1] == 0xaa) &&
					(oob[2] == 0x5c)) {
				block_blank_flag = 0;
				break;
			}

			if ((oob[0] == 0xff) && (oob[1] == 0x00)) {
				block_blank_flag = 0;
				break;
			}
		}
	}

	if (block_blank_flag == 1)
		PHY_DBG("nand is blank!!\n");
	else
		PHY_DBG("nand has valid data!!\n");

	return block_blank_flag;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int nand_check_bad_block_before_first_erase(struct _nand_physic_op_par *npo)
{
	int ret;
	int read_retry_mode;
	struct _nand_chip_info *nci;

	nci		= nci_get_from_nsi(g_nsi, npo->chip);
	read_retry_mode = (nci->npi->read_retry_type >> 16) & 0xff;
	if ((read_retry_mode != 0x34) && (read_retry_mode != 0x35)) {
		PHY_DBG("not slc program\n");
		return 1;
	}

	ret = m9_check_bad_block_first_burn(npo);

	return ret;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_erase_chip(unsigned int chip, unsigned int start_block,
		unsigned int end_block, unsigned int force_flag)
{
	int ret, i;
	int read_retry_mode;
	struct _nand_physic_op_par npo;
	struct _nand_chip_info *nci;

	nci		= nci_get_from_nsi(g_nsi, chip);
	read_retry_mode = (nci->npi->read_retry_type >> 16) & 0xff;

	if ((end_block >= nci->blk_cnt_per_chip) || (end_block == 0))
		end_block = nci->blk_cnt_per_chip;

	if (start_block > end_block)
		return 0;

	for (i = start_block; i < end_block; i++) {
		npo.chip	= nci->chip_no;
		npo.block       = i;
		npo.page	= 0;
		npo.sect_bitmap = nci->sector_cnt_per_page;
		npo.mdata       = NULL;
		npo.sdata       = NULL;

		if ((read_retry_mode == 0x32) || (read_retry_mode == 0x33)) {
			ret = 0;
		} else if ((read_retry_mode == 0x34) ||
				(read_retry_mode == 0x35)) {
			ret = nand_check_bad_block_before_first_erase(&npo);
			if (ret != 0)
				PHY_DBG("find a bad block %d %d\n", npo.chip,
						npo.block);
			if (force_flag == 1)
				ret = 0;
		} else {
			ret = nci->nand_physic_bad_block_check(&npo);
			if (force_flag == 1)
				ret = 0;
		}

		if (ret == 0) {
			ret = nci->nand_physic_erase_block(&npo);
			nand_wait_all_rb_ready();

			if (ret != 0)
				nci->nand_physic_bad_block_mark(&npo);
		}
	}

	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
void nand_erase_special_block(void)
{
	int i, j, flag, ret;
	struct _nand_physic_op_par npo;
	u8 oob[64];
	struct _nand_chip_info *nci;

	nci = nci_get_from_nsi(g_nsi, 0);

	for (j = 0; j < g_nsi->chip_cnt; j++) {
		for (i = 7; i < 50; i++) {
			flag = 0;

			npo.chip	= j;
			npo.block       = i;
			npo.page	= 0;
			npo.sect_bitmap = nci->sector_cnt_per_page;
			npo.mdata       = NULL;
			npo.sdata       = NULL;

			ret = nci->nand_physic_bad_block_check(&npo);
			if (ret != 0) {
				npo.chip	= j;
				npo.block       = i;
				npo.page	= 0;
				npo.sect_bitmap = nci->sector_cnt_per_page;
				npo.mdata       = NULL;
				npo.sdata       = oob;
				npo.slen	= 16;
				ret = nci->nand_physic_read_page(&npo);

				if ((oob[1] == 0x78) && (oob[2] == 0x69) &&
						(oob[3] == 0x87) && (oob[4] == 0x41) &&
						(oob[5] == 0x52) && (oob[6] == 0x43) &&
						(oob[7] == 0x48)) {
					flag = 1;
					PHY_ERR("this is nand struct block %d\n",
							i);
				}

				if ((oob[1] == 0x50) && (oob[2] == 0x48) &&
						(oob[3] == 0x59) && (oob[4] == 0x41) &&
						(oob[5] == 0x52) && (oob[6] == 0x43) &&
						(oob[7] == 0x48)) {
					flag = 1;
					PHY_ERR("this is old nand struct block %d\n",
							i);
				}

				if ((oob[0] == 0x00) && (oob[1] == 0x4F) &&
						(oob[2] == 0x4F) && (oob[3] == 0x42)) {
					flag = 1;
					PHY_ERR("this is hynix otp data save block %d\n",
							i);
				}

				if (flag == 1) {
					ret = nci->nand_physic_erase_block(
							&npo);
					nand_wait_all_rb_ready();
					if (ret != 0) {
						nci->nand_physic_bad_block_mark(
								&npo);
					}
				}
			}
		}
	}

	return;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_uboot_erase_all_chip(UINT32 force_flag)
{
	int i, start, end, secure_block_start;
	int uboot_start_block, uboot_next_block;

	uboot_start_block  = get_uboot_start_block();
	uboot_next_block   = get_uboot_next_block();
	secure_block_start = uboot_next_block;

	for (i = 0; i < g_nsi->chip_cnt; i++) {
		start = 0;
		end   = 0xffff;
		if (i == 0) {
			if ((force_flag == 1) || (nand_is_blank()) == 1)
				start = secure_block_start;
			else
				start = nand_secure_storage_first_build(
						secure_block_start);
		}
		nand_erase_chip(i, start, end, force_flag);
	}

	nand_erase_special_block();

	clean_physic_info();

	aw_nand_info.boot->uboot_start_block = uboot_start_block;
	aw_nand_info.boot->uboot_next_block  = uboot_next_block;
#ifndef DISABLE_CRC
	aw_nand_info.boot->enable_crc = ENABLE_CRC_MAGIC;
#endif
	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_dragonborad_test_one(unsigned char *buf, unsigned char *oob,
		unsigned int blk_num)
{
	int ret;
	unsigned int j;
	unsigned char oob_buf[64];
	struct _nand_physic_op_par lnpo;
	struct _nand_chip_info *nci;

	//    PHY_ERR("dragonborad test in one block!\n");

	nci = g_nctri->nci;

	lnpo.chip  = 0;
	lnpo.block = blk_num;
	lnpo.page  = 0;

	nand_wait_all_rb_ready();

	ret = nci->nand_physic_erase_block(&lnpo);
	if (ret) {
		PHY_ERR("Fail in erasing block %d!\n", lnpo.block);
		return 0;
	}

	for (j = 0; j < nci->page_cnt_per_blk; j++) {
		lnpo.chip	= 0;
		lnpo.block       = blk_num;
		lnpo.page	= j;
		lnpo.sect_bitmap = nci->sector_cnt_per_page;
		lnpo.sdata       = oob;
		lnpo.slen	= 64;

		lnpo.mdata = buf;

		nand_wait_all_rb_ready();

		if (nci->nand_physic_write_page(&lnpo) != 0) {
			PHY_ERR("Warning. Fail in writing page %d in block %d.\n",
					lnpo.page, lnpo.block);
		}
	}
	for (j = 0; j < nci->page_cnt_per_blk; j++) {
		lnpo.chip	= 0;
		lnpo.block       = blk_num;
		lnpo.page	= j;
		lnpo.sdata       = (unsigned char *)oob_buf;
		lnpo.sect_bitmap = nci->sector_cnt_per_page;
		lnpo.slen	= 64;
		lnpo.mdata       = buf;

		MEMSET(oob_buf, 0xff, 64);

		if (nci->nand_physic_read_page(&lnpo) != 0) {
			PHY_ERR("Warning. Fail in read page %d in block %d.\n",
					j, lnpo.block);
			return -1;
		}
		if ((oob_buf[0] != oob[0]) || (oob_buf[1] != oob[1]) ||
				(oob_buf[2] != oob[2]) || (oob_buf[3] != oob[3])) {
			PHY_ERR("oob data error\n!");
			return -1;
		}
	}

	nand_wait_all_rb_ready();
	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok   1:ecc limit     -1:fail
 *Note         :
 *****************************************************************************/
int change_uboot_start_block(struct _boot_info *info, unsigned int start_block)
{
	if (nand_secure_storage_block != 0) {
		if (info->storage_info.data.support_two_plane == 1) {
			info->uboot_start_block = get_uboot_start_block();
			info->uboot_next_block  = get_uboot_next_block();
			info->logic_start_block =
				(nand_secure_storage_block_bak / 2) + 1 +
				PHYSIC_RECV_BLOCK / 2;
			info->no_use_block = info->logic_start_block;
		} else {
			info->uboot_start_block = get_uboot_start_block();
			info->uboot_next_block  = get_uboot_next_block();
			info->logic_start_block =
				nand_secure_storage_block_bak + 1 +
				PHYSIC_RECV_BLOCK;
			info->no_use_block = info->logic_start_block;
		}
		return 0;
	} else {
		//first build
		PHY_ERR("change_uboot_start_block error!!!! \n");
		return -1;
	}
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok   1:ecc limit     -1:fail
 *Note         :
 *****************************************************************************/
int get_uboot_start_block(void)
{
	__u32 uboot_block_size;
	__u32 uboot_start_block;

	uboot_block_size = NAND_GetLsbblksize();

	/*small nand:block size < 1MB*/
	if (uboot_block_size <= 0x40000) {//256K
		uboot_start_block = UBOOT_START_BLOCK_SMALLNAND;
	} else {
		uboot_start_block = NAND_UBOOT_BLK_START;
	}

	if (aw_nand_info.boot->uboot_start_block != 0) {
		uboot_start_block = aw_nand_info.boot->uboot_start_block;
	}

	return uboot_start_block;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok   1:ecc limit     -1:fail
 *Note         :
 *****************************************************************************/
int get_uboot_next_block(void)
{
	__u32 uboot_block_size;
	__u32 uboot_start_block;
	__u32 uboot_next_block;
	/*struct _nand_chip_info *nci;*/

	/*nci = g_nctri->nci;*/

	uboot_block_size = NAND_GetLsbblksize();

	/*small nand:block size < 1MB*/
	if (uboot_block_size <= 0x20000) {//128K
		uboot_start_block = UBOOT_START_BLOCK_SMALLNAND;
		uboot_next_block  = uboot_start_block + 40;
	} else if (uboot_block_size <= 0x40000) {//256k
		uboot_start_block = UBOOT_START_BLOCK_SMALLNAND;
		uboot_next_block  = uboot_start_block + 20;
	} else {
		uboot_start_block = NAND_UBOOT_BLK_START;
		uboot_next_block  = uboot_start_block + NAND_UBOOT_BLK_CNT;
	}

	if (aw_nand_info.boot->uboot_next_block != 0) {
		uboot_next_block = aw_nand_info.boot->uboot_next_block;
	}

	return uboot_next_block;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok   1:ecc limit     -1:fail
 *Note         :
 *****************************************************************************/
void print_uboot_info(struct _uboot_info *uboot)
{
	int i;
	PHY_DBG("uboot->use_lsb_page %d \n", uboot->use_lsb_page);
	PHY_DBG("uboot->copys %d \n", uboot->copys);
	PHY_DBG("uboot->uboot_len %d \n", uboot->uboot_len);
	PHY_DBG("uboot->total_len %d \n", uboot->total_len);
	PHY_DBG("uboot->uboot_pages %d \n", uboot->uboot_pages);
	PHY_DBG("uboot->total_pages %d \n", uboot->total_pages);
	PHY_DBG("uboot->blocks_per_total %d \n", uboot->blocks_per_total);
	PHY_DBG("uboot->page_offset_for_nand_info %d \n",
			uboot->page_offset_for_nand_info);
	PHY_DBG("uboot->uboot_block:\n");
	for (i = 0; i < 60; i++) {
		if (uboot->uboot_block[i] != 0)
			PHY_DBG("%d ", uboot->uboot_block[i]);
	}
	PHY_DBG("\n");
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok   1:ecc limit     -1:fail
 *Note         :
 *****************************************************************************/
int uboot_info_init(struct _uboot_info *uboot)
{
	int ret, blcok_offset;
	char *uboot_buf;
	unsigned int i;
	unsigned int jump_size;
	unsigned int size_per_page;
	/*unsigned int uboot_size;*/
	unsigned int lsb_pages;
	struct _nand_chip_info *nci;
	struct _nand_physic_op_par npo;
	int uboot_find = 0;
	int uboot_ptr  = 0;

	nci	   = g_nctri->nci;
	size_per_page = nci->sector_cnt_per_page * 512;
	MEMSET(uboot, 0, sizeof(struct _uboot_info));

	uboot_buf = (char *)nand_get_temp_buf(size_per_page);
	if (uboot_buf == NULL) {
		PHY_ERR("uboot_buf malloc fail\n");
		return 1;
	}

	uboot->use_lsb_page = NAND_UsedLsbPages();
	blcok_offset	= 0;

	for (i = aw_nand_info.boot->uboot_start_block;
			i < aw_nand_info.boot->uboot_next_block; i++) {
		npo.chip	= 0;
		npo.block       = i;
		npo.page	= 0;
		npo.sect_bitmap = nci->sector_cnt_per_page;
		npo.mdata       = NULL;
		npo.sdata       = NULL;
		ret		= nci->nand_physic_bad_block_check(&npo);
		if (ret == 0) {
			uboot->uboot_block[uboot_ptr] = i;
			uboot_ptr++;
		}

		ret = is_uboot_block(i, uboot_buf);
		if (ret == 1) {
			if (blcok_offset == 0) {
				uboot_find       = 1;
				uboot->uboot_len = get_uboot_offset(uboot_buf);
				uboot->uboot_pages =
					uboot->uboot_len / size_per_page;
				if (uboot->uboot_len % size_per_page) {
					uboot->uboot_pages++;
				}
				jump_size = size_per_page * uboot->uboot_pages -
					uboot->uboot_len;

				uboot->total_len = uboot->uboot_len +
					jump_size + PHY_INFO_SIZE;
				uboot->total_pages =
					uboot->total_len / size_per_page;
				uboot->page_offset_for_nand_info =
					uboot->uboot_pages;
				uboot->byte_offset_for_nand_info =
					size_per_page * uboot->uboot_pages;
				if (uboot->use_lsb_page == 0) {
					uboot->blocks_per_total =
						uboot->total_pages /
						nci->page_cnt_per_blk;
					if (uboot->total_pages %
							nci->page_cnt_per_blk) {
						uboot->blocks_per_total++;
					}
				} else {
					lsb_pages = NAND_GetLsbPages();
					uboot->blocks_per_total =
						uboot->total_pages / lsb_pages;
					if (uboot->total_pages % lsb_pages) {
						uboot->blocks_per_total++;
					}
				}
			}
			blcok_offset++;
			//uboot->copys++;
			//i += uboot->blocks_per_total - 1;
		}
	}

	if (uboot_find == 1) {
		uboot->copys = uboot_ptr / uboot->blocks_per_total;
	} else {
		uboot->copys = 0;
	}

	print_uboot_info(uboot);

	nand_free_temp_buf((unsigned char *)uboot_buf, size_per_page);
	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int init_phyinfo_buf(void)
{
	phyinfo_buf = (struct _boot_info *)MALLOC(PHY_INFO_SIZE);
	if (phyinfo_buf == NULL) {
		PHY_ERR("init_phyinfo_buf fail\n");
		return -1;
	}
	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int get_normal_uboot_offset(void *buf)
{
	int length;
	struct normal_uboot_head_info *temp_buf;

	temp_buf = (struct normal_uboot_head_info *)buf;

	length = temp_buf->length;

	return length;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int get_secure_uboot_offset(void *buf)
{
	int length;
	struct secure_uboot_head_info *temp_buf;

	temp_buf = (struct secure_uboot_head_info *)buf;

	length = temp_buf->valid_len;

	return length;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int get_uboot_offset(void *buf)
{
	int *toc;
	toc = (int *)buf;
	return toc[TOC_LEN_OFFSET_BY_INT];
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int is_normal_uboot_magic(void *buf)
{
	int ret;
	struct normal_uboot_head_info *temp_buf;

	temp_buf = (struct normal_uboot_head_info *)buf;

	//	PHY_ERR("%c %c %c %c %c\n",temp_buf->magic[0],temp_buf->magic[1],temp_buf->magic[2],temp_buf->magic[3],temp_buf->magic[4]);
	if ((temp_buf->magic[0] == 'u') && (temp_buf->magic[1] == 'b') &&
			(temp_buf->magic[2] == 'o') && (temp_buf->magic[3] == 'o') &&
			(temp_buf->magic[4] == 't'))
		ret = 1;
	else
		ret = 0;

	return ret;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int is_secure_uboot_magic(void *buf)
{
	int ret;
	struct secure_uboot_head_info *temp_buf;

	temp_buf = (struct secure_uboot_head_info *)buf;

	if (temp_buf->magic == TOC_MAIN_INFO_MAGIC)
		ret = 1;
	else
		ret = 0;

	return ret;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int is_uboot_magic(void *buf)
{
	int *toc;
	toc = (int *)buf;

	return (toc[TOC_MAGIC_OFFSET_BY_INT] == TOC_MAIN_INFO_MAGIC) ? 1 : 0;
}

/********************************************************************************
 * name	: check_magic
 * func	: check the magic of boot0
 *
 * argu	: @mem_base	: the start address of boot0;
 *         @magic	: the standard magic;
 *
 * return: CHECK_IS_CORRECT 	: magic is ok;
 *         CHECK_IS_WRONG	: magic is wrong;
 ********************************************************************************/
int check_magic_physic_info(unsigned int *mem_base, char *magic)
{
	unsigned int i;
	struct _boot_info *bfh;
	unsigned int sz;
	unsigned char *p;

	bfh = (struct _boot_info *)mem_base;
	p   = (unsigned char *)bfh->magic;
	for (i = 0, sz = sizeof(bfh->magic); i < sz; i++) {
		if (*p++ != *magic++)
			return -1;
	}

	return 0;
}

/********************************************************************************
 * name	: check_sum
 * func	: check data using check sum.
 *
 * argu	: @mem_base	: the start address of data, it must be 4-byte aligned;
 *         @size		: the size of data;
 *
 * return: CHECK_IS_CORRECT 	: the data is right;
 *         CHECK_IS_WRONG	: the data is wrong;
 ********************************************************************************/
int check_sum_physic_info(unsigned int *mem_base, unsigned int size)
{
	unsigned int *buf;
	unsigned int count;
	unsigned int src_sum;
	unsigned int sum;
	struct _boot_info *bfh;

	bfh = (struct _boot_info *)mem_base;

	/* generate check sum */
	src_sum = bfh->sum; // get check_sum field from the head of boot0;
	bfh->sum =
		UBOOT_STAMP_VALUE; // replace the check_sum field of the boot0 head with STAMP_VALUE

	count = size >> 2; // unit, 4byte
	sum   = 0;
	buf   = (unsigned int *)mem_base;
	do {
		sum += *buf++; // calculate check sum
		sum += *buf++;
		sum += *buf++;
		sum += *buf++;
	} while ((count -= 4) > (4 - 1));

	while (count-- > 0)
		sum += *buf++;

	bfh->sum = src_sum; // restore the check_sum field of the boot0 head

	//msg("sum:0x%x - src_sum:0x%x\n", sum, src_sum);
	if (sum == src_sum)
		return 0; // ok
	else
		return -1; // err
}
/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
unsigned int cal_sum_physic_info(struct _boot_info *mem_base, unsigned int size)
{
	unsigned int count, sum;
	struct _boot_info *temp_buf;
	unsigned int *buf;

	temp_buf      = mem_base;
	temp_buf->sum = UBOOT_STAMP_VALUE;

	count = size >> 2;
	sum   = 0;
	buf   = (unsigned int *)mem_base;
	do {
		sum += *buf++;
		sum += *buf++;
		sum += *buf++;
		sum += *buf++;
	} while ((count -= 4) > (4 - 1));

	while (count-- > 0)
		sum += *buf++;

	temp_buf->sum = sum;

	return sum;
}
/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int physic_info_get_offset(unsigned int *pages_offset)
{
	unsigned int block;
	unsigned int size_per_page;
	unsigned int uboot_size;
	unsigned char sdata[64];
	int ret;
	unsigned int cnt;
	void *uboot_buf = NULL;
	struct _nand_physic_op_par lnpo;
	struct _nand_chip_info *nci = g_nctri->nci;

	PHY_DBG("physic_info_get_offset start!!\n");

	size_per_page = nci->sector_cnt_per_page << 9;

	uboot_buf = nand_get_temp_buf(size_per_page);
	if (uboot_buf == NULL) {
		PHY_ERR("uboot_buf malloc fail\n");
		return -1;
	}

	cnt = 0;
	for (block = NAND_UBOOT_BLK_START; block < UBOOT_MAX_BLOCK_NUM;
			block++) {
		lnpo.chip	= 0;
		lnpo.block       = block;
		lnpo.page	= 0;
		lnpo.sdata       = sdata;
		lnpo.sect_bitmap = nci->sector_cnt_per_page;
		lnpo.slen	= 64;
		lnpo.mdata       = uboot_buf;

		ret = nci->nand_physic_read_page(&lnpo);
		if (ret == ERR_ECC) {
			PHY_ERR("ecc err:chip %d block %d page %d\n", lnpo.chip,
					lnpo.block, lnpo.page);
			continue;
		}

		if (1 == (is_uboot_magic(uboot_buf))) {//
			cnt++;
			uboot_size = get_uboot_offset(uboot_buf);
			break;
		}
	}

	if (cnt) {
		*pages_offset = uboot_size / size_per_page;
		if (uboot_size % size_per_page)
			*pages_offset = (*pages_offset) + 1;
	}
	if (uboot_buf) {
		nand_free_temp_buf(uboot_buf, size_per_page);
	}

	if (cnt)
		return 0;
	else
		return 1;
}
/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int physic_info_get_one_copy(unsigned int start_block,
		unsigned int pages_offset,
		unsigned int *block_per_copy, unsigned int *buf)
{
	unsigned int page, block;
	unsigned int flag;
	unsigned int size_per_page, pages_per_block, lsbblock_size;
	unsigned char sdata[64];
	int ret;
	unsigned int phyinfo_page_cnt, page_cnt;
	unsigned int badblk_num = 0;
	unsigned int pages_per_phyinfo;
	void *tempbuf = NULL;
	struct _nand_physic_op_par lnpo;
	struct _nand_chip_info *nci = g_nctri->nci;

	PHY_DBG("physic_info_get_one_copy start!!\n");

	size_per_page   = nci->sector_cnt_per_page << 9;
	pages_per_block = nci->page_cnt_per_blk;

	pages_per_phyinfo = PHY_INFO_SIZE / size_per_page;
	if (PHY_INFO_SIZE % size_per_page)
		pages_per_phyinfo++;


	tempbuf = nand_get_temp_buf(64 * 1024);
	if (tempbuf == NULL) {
		PHY_ERR("tempbuf malloc fail\n");
		return -1;
	}

	page_cnt	 = 0;
	phyinfo_page_cnt = 0;
	flag		 = 0;
	for (block = start_block;; block++) {
		lnpo.chip	= 0;
		lnpo.block       = block;
		lnpo.page	= 0;
		lnpo.sdata       = sdata;
		lnpo.sect_bitmap = 0;
		lnpo.slen	= 64;
		lnpo.mdata       = NULL;

		ret = nci->nand_physic_read_page(&lnpo);
		if ((sdata[0] == 0x0)) {
			badblk_num++;
			PHY_DBG("bad block:chip %d block %d\n", lnpo.chip,
					lnpo.block);
			continue;
		}
		for (page = 0; page < pages_per_block; page++) {
			if (nci->is_lsb_page(page) == 1) {
				if (page_cnt >= pages_offset) {
					lnpo.chip  = 0;
					lnpo.block = block;
					lnpo.page  = page;
					lnpo.sdata = sdata;
					lnpo.sect_bitmap =
						nci->sector_cnt_per_page;
					lnpo.slen = 64;
					lnpo.mdata =
						(void *)((char *)tempbuf +
								phyinfo_page_cnt *
								size_per_page);
					PHY_DBG("block %d page %d\n",
							lnpo.block, lnpo.page);
					ret = nci->nand_physic_read_page(&lnpo);
					if (ret == ERR_ECC) {
						PHY_ERR("ecc err:chip %d block %d page %d\n",
								lnpo.chip, lnpo.block,
								lnpo.page);
						break;
					}

					phyinfo_page_cnt++;
					if (phyinfo_page_cnt ==
							pages_per_phyinfo) {
						flag = 1;
						break;
					}
				}
				page_cnt++;
			}
		}

		if (ret == ERR_ECC)
			break;

		if (flag == 1)
			break;
	}

	MEMCPY(buf, tempbuf, PHY_INFO_SIZE);

	lsbblock_size = NAND_GetLsbblksize();

	*block_per_copy = (pages_offset + pages_per_phyinfo) * size_per_page /
		lsbblock_size +
		badblk_num;
	if (((pages_offset + pages_per_phyinfo) * size_per_page) %
			lsbblock_size)
		*block_per_copy = (*block_per_copy) + 1;

	if (tempbuf) {
		nand_free_temp_buf(tempbuf, 64 * 1024);
	}

	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int clean_physic_info(void)
{
	MEMSET((char *)phyinfo_buf, 0, PHY_INFO_SIZE);
	phyinfo_buf->len   = PHY_INFO_SIZE;
	phyinfo_buf->magic = PHY_INFO_MAGIC;
	PHY_ERR("clean physic info uboot\n");
	return 0;
}
/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int physic_info_read(void)
{
	/*unsigned int copy;*/
	int ret, ret_sum, ret_flag, tail_flag = 0;
	unsigned int pages_offset;
	unsigned int start_block, block_per_copy;
	char *temp_buf;
	static int flag;
	unsigned int uboot_blk_size;
	struct _nand_physic_op_par npo;
	struct _nand_chip_info *nci;

	nci = nci_get_from_nsi(g_nsi, 0);

	uboot_blk_size = NAND_GetLsbblksize();

	PHY_ERR("physic_info_read start!!\n");

	if (flag == 1) {
		PHY_DBG("physic_info_read already!!\n");
		return 0;
	}

	if (init_phyinfo_buf() != 0) {
		return -1;
	}

	ret = physic_info_get_offset(&pages_offset);
	if (ret) {
		MEMSET((char *)phyinfo_buf, 0, PHY_INFO_SIZE);
		phyinfo_buf->len   = PHY_INFO_SIZE;
		phyinfo_buf->magic = PHY_INFO_MAGIC;
#ifndef DISABLE_CRC
		phyinfo_buf->enable_crc = ENABLE_CRC_MAGIC;
#endif
		PHY_ERR("can't find uboot head\n");
		flag = 1;
		return ret;
	}

	temp_buf = (char *)nand_get_temp_buf(g_nctri->nci->sector_cnt_per_page
			<< 9);

	ret_flag = -1;

	for (start_block = NAND_UBOOT_BLK_START;
			start_block <= UBOOT_MAX_BLOCK_NUM; start_block++) {
		if (uboot_blk_size > 0x40000) {// not small nand
			if ((start_block == 7) &&
					(tail_flag == 1)) {//nand2.x ota erase blk 7
				npo.chip	= 0;
				npo.block       = 7;
				npo.page	= 0;
				npo.sect_bitmap = nci->sector_cnt_per_page;
				npo.mdata       = NULL;
				npo.sdata       = NULL;

				nci->nand_physic_erase_block(&npo);
				PHY_DBG("ERASE BLOCK 7 ...\n");
				MEMSET((char *)phyinfo_buf, 0, PHY_INFO_SIZE);
				phyinfo_buf->len   = PHY_INFO_SIZE;
				phyinfo_buf->magic = PHY_INFO_MAGIC;
				ret_flag	   = 1;
				flag		   = 1;
				break;
			}
		}
		ret = is_uboot_block(start_block, temp_buf);
		if (ret != 1) {
			continue;
		}

		physic_info_get_one_copy(start_block, pages_offset,
				&block_per_copy,
				(unsigned int *)phyinfo_buf);

		ret_sum = check_sum_physic_info((unsigned int *)phyinfo_buf,
				PHY_INFO_SIZE);
		if (ret_sum == 0) {
			PHY_ERR("physic info copy is ok\n");
			ret_flag = 0;
			flag     = 1;

			printf("phyinfo_buf->enable_crc %x\n",
					phyinfo_buf->enable_crc);
			break;
		} else {
			PHY_ERR("physic info copy is bad\n");
			MEMSET((char *)phyinfo_buf, 0, PHY_INFO_SIZE);
			phyinfo_buf->len   = PHY_INFO_SIZE;
			phyinfo_buf->magic = PHY_INFO_MAGIC;
			flag		   = 1;
			tail_flag	  = 1;
		}
	}

	nand_free_temp_buf((unsigned char *)temp_buf,
			g_nctri->nci->sector_cnt_per_page << 9);

	return ret_flag;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int is_phyinfo_empty(struct _boot_info *info)
{
	if ((info->magic == PHY_INFO_MAGIC) && (info->len == PHY_INFO_SIZE) &&
			(info->sum == 0)) {
		return 1;
	}
	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int add_len_to_uboot_tail(unsigned int uboot_size)
{
	unsigned int size_per_page, pages_per_uboot;
	unsigned int jump_size;

	size_per_page = g_nctri->nci->sector_cnt_per_page << 9;

	pages_per_uboot = uboot_size / size_per_page;
	if (uboot_size % size_per_page)
		pages_per_uboot++;

	jump_size = size_per_page * pages_per_uboot - uboot_size;

	return (uboot_size + jump_size + PHY_INFO_SIZE);
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int print_physic_info(struct _boot_info *info)
{
	unsigned int i;
	for (i = 1; i < (PHY_INFO_SIZE / 4) + 1; i++) {
		if ((i % 8) == 0)
			PHY_ERR("\n");
		PHY_ERR(" %08x", *((unsigned int *)info + i - 1));
	}
	PHY_ERR("end!\n");
	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int physic_info_add_to_uboot_tail(unsigned int *buf_dst,
		unsigned int uboot_size)
{
	unsigned int real_len;

	real_len = add_len_to_uboot_tail(uboot_size);

	cal_sum_physic_info(phyinfo_buf, PHY_INFO_SIZE);

	PHY_ERR("physic_info_add_to_uboot_tail start2 %x!!\n", phyinfo_buf);

	MEMCPY((char *)buf_dst + (real_len - PHY_INFO_SIZE), phyinfo_buf,
			PHY_INFO_SIZE);

	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
unsigned int is_uboot_block(unsigned int block, char *uboot_buf)
{
	int ret = 0;
	unsigned char sdata[64];
	struct _nand_physic_op_par lnpo;
	struct _nand_chip_info *nci = g_nctri->nci;
	lnpo.chip		    = 0;
	lnpo.block		    = block;
	lnpo.page		    = 0;
	lnpo.sdata		    = sdata;
	lnpo.sect_bitmap	    = nci->sector_cnt_per_page;
	lnpo.slen		    = 64;
	lnpo.mdata		    = (unsigned char *)uboot_buf;

	ret = nci->nand_physic_read_page(&lnpo);

	if (1 == (is_uboot_magic((void *)uboot_buf))) {
		return 1;
	} else {
		return 0;
	}

	return ret;
}
#if 1

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int rawnand_is_blank(void)
{
	int i, ret;
	int block_blank_flag = 1;
	int read_retry_mode;
	int start_blk = 4, blk_cnt = 30;
	unsigned char oob[64];
	struct _nand_physic_op_par npo;
	struct _nand_chip_info *nci;

	nci		= nci_get_from_nsi(g_nsi, 0);
	read_retry_mode = (nci->npi->read_retry_type >> 16) & 0xff;

	if ((read_retry_mode != 0x32) && (read_retry_mode != 0x33) &&
			(read_retry_mode != 0x34) && (read_retry_mode != 0x35))
		return 0;

	for (i = start_blk; i < (start_blk + blk_cnt); i++) {
		npo.chip	= 0;
		npo.block       = i;
		npo.page	= 0;
		npo.sect_bitmap = nci->sector_cnt_per_page;
		npo.mdata       = NULL;
		npo.sdata       = oob;
		npo.slen	= 64;

		ret = nci->nand_physic_read_page(&npo);
		if (ret >= 0) {
			if ((oob[0] == 0xff) && (oob[1] == 0xaa) &&
					(oob[2] == 0x5c)) {
				block_blank_flag = 0;
				break;
			}

			if ((oob[0] == 0xff) && (oob[1] == 0x00)) {
				block_blank_flag = 0;
				break;
			}
		}
	}

	if (block_blank_flag == 1)
		PHY_DBG("nand is blank!!\n");
	else
		PHY_DBG("nand has valid data!!\n");

	return block_blank_flag;
}

int nand_get_param_for_uboottail(void *nand_param)
{
	int ret = 0;

	nand_get_param(nand_param);

	return ret;
}
#endif

/*
   enable crc check for winbond power down fail issue
   calc crc when nftl write.
   check crc when nftl build PA->LA mapping table
 */
int is_physic_info_enable_crc(void)
{
	return (phyinfo_buf->enable_crc == ENABLE_CRC_MAGIC);
}
