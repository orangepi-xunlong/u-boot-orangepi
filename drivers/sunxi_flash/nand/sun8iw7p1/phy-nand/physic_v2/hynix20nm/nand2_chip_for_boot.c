//SPDX-License-Identifier:	GPL-2.0+
/*
 ************************************************************************************************************************
 *                                                      eNand
 *                                           Nand flash driver scan module
 *
 *                             Copyright(C), 2008-2009, SoftWinners Microelectronic Co., Ltd.
 *                                                  All Rights Reserved
 *
 * File Name : nand_chip_for_boot.c
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
#define _NCFB2_C_

#include "../nand_physic_inc.h"

extern __u32 SECURE_FLAG;

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :for hynix 20nm nand
 *****************************************************************************/
int m2_read_boot0_page(struct _nand_chip_info *nci,
		struct _nand_physic_op_par *npo)
{
	struct boot_ndfc_cfg cfg;

	if (SECURE_FLAG == 0) {
		cfg.page_size_kb = 8;
		cfg.ecc_mode     = 4;
	} else {
		cfg.page_size_kb = 4;
		cfg.ecc_mode     = nci->nctri->max_ecc_level;
	}
	cfg.sequence_mode = 1;
	return m8_read_boot0_page_cfg_mode(nci, npo, &cfg);
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :for hynix 20nm nand
 *****************************************************************************/
int m2_write_boot0_page(struct _nand_chip_info *nci,
		struct _nand_physic_op_par *npo)
{
	struct boot_ndfc_cfg cfg;

	if (SECURE_FLAG == 0) {
		cfg.page_size_kb = nci->sector_cnt_per_page / 2;
		cfg.ecc_mode     = 4;
	} else {
		cfg.page_size_kb = ((nci->sector_cnt_per_page) / 2) - 1;
		cfg.ecc_mode     = nci->nctri->max_ecc_level;
	}
	cfg.sequence_mode = 1;
	return m8_write_boot0_page_cfg_mode(nci, npo, &cfg);
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :Ã¿¸öpageÖ»Ð´16K
 *****************************************************************************/
int m2_read_boot0_one_nonsecure(unsigned char *buf, unsigned int len,
		unsigned int counter)
{
	__u32 i, j, count, total_pages, block;
	__u8 oob_buf[64];
	unsigned char *ptr = NULL;
	//	int ret;
	struct _nand_chip_info *nci;
	struct _nand_physic_op_par lnpo;

	nci = g_nctri->nci;

	PHY_ERR("m2_read_boot0_one_nonsecure!\n");

	for (i		   = 0; i < 64; i++)
		oob_buf[i] = 0xff;

	/* get nand driver version */
	nand_get_version(oob_buf);
	if ((oob_buf[0] != 0xff) || (oob_buf[1] != 0x00)) {
		PHY_ERR("get flash driver version error!");
		goto error;
	}

	block = NAND_BOOT0_BLK_START + counter;

	PHY_DBG("boot0 count %d!\n", counter);

	lnpo.chip  = 0;
	lnpo.block = block;

	nand_wait_all_rb_ready();

	ptr	 = nand_get_temp_buf(8 * 1024);
	total_pages = len / (8 * 1024);
	count       = 0;

	for (j = 0; j < (len / (8 * 1024)); j++) {
		lnpo.page   = j;
		lnpo.sdata  = oob_buf;
		lnpo.slen   = 64;
		lnpo.mdata  = ptr;
		SECURE_FLAG = 0;

		nand_wait_all_rb_ready();
		if (nci->nand_read_boot0_page(nci, &lnpo) < 0) {
			PHY_ERR("Warning. Fail in read page %d in block %d.\n",
					lnpo.page, lnpo.block);
			goto error;
		}
		if ((oob_buf[0] != 0xff) || (oob_buf[1] != 0x00)) {
			PHY_ERR("get flash driver version error!");
			goto error;
		}
		MEMCPY(buf + count * (8 * 1024), ptr, 8 * 1024);

		count++;
		if (total_pages == count) {
			break;
		}
	}
	nand_free_temp_buf(ptr, 8 * 1024);

	return 0;

error:
	PHY_ERR("m2_read_boot0_one_nonsecure error!");
	nand_free_temp_buf(ptr, 8 * 1024);

	return -1;
}

int m2_read_boot0_one_secure(unsigned char *buf, unsigned int len,
		unsigned int counter)
{
	__u32 j, k, m, err_flag, count, block;
	__u8 oob_buf[64];
	__u32 data_size_per_page;
	__u32 pages_per_block, copies_per_block;
	unsigned char *ptr;
	struct _nand_physic_op_par lnpo;
	struct _nand_chip_info *nci;

	nci = g_nctri->nci;

	data_size_per_page = 4096;
	pages_per_block    = nci->page_cnt_per_blk;
	copies_per_block   = pages_per_block / NAND_BOOT0_PAGE_CNT_PER_COPY_2;

	block = NAND_BOOT0_BLK_START + counter;
	ptr   = nand_get_temp_buf(data_size_per_page);

	PHY_ERR("read blk %x \n", block);
	for (j = 0; j < copies_per_block; j++) {
		err_flag = 0;
		count    = 0;
		for (k = 8; k < NAND_BOOT0_PAGE_CNT_PER_COPY_2; k++) {
			lnpo.chip  = 0;
			lnpo.block = block;
			lnpo.page  = j * NAND_BOOT0_PAGE_CNT_PER_COPY_2 + k;
			lnpo.sdata = oob_buf;
			lnpo.slen  = 64;

			for (m		   = 0; m < 64; m++)
				oob_buf[m] = 0x55;

			if (nci->is_lsb_page(lnpo.page)) {
				lnpo.mdata = ptr;

				nand_wait_all_rb_ready();
				SECURE_FLAG = 1;
				if (nci->nand_read_boot0_page(nci, &lnpo) < 0) {
					PHY_ERR("Warning. Fail in read page %d in block %d.\n",
							lnpo.page, lnpo.block);
					err_flag = 1;
					break;
				}
				if ((oob_buf[0] != 0xff) ||
						(oob_buf[1] != 0x00)) {
					PHY_ERR("get flash driver version error!");
					err_flag = 1;
					break;
				}
				MEMCPY(buf + count * data_size_per_page, ptr,
						data_size_per_page);

				count++;
				if (count == (len / data_size_per_page))
					break;
			}
		}
		if (err_flag == 0)
			break;
	}

	nand_wait_all_rb_ready();
	nand_free_temp_buf(ptr, data_size_per_page);

	if (err_flag == 1)
		return -1;

	return 0;
}

int m2_read_boot0_one(unsigned char *buf, unsigned int len,
		unsigned int counter)
{
	if (NAND_IS_Secure_sys() == 0)
		return (m2_read_boot0_one_nonsecure(buf, len, counter));
	else if (NAND_IS_Secure_sys() == 1)
		return (m2_read_boot0_one_secure(buf, len, counter));
	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int m2_write_boot0_one_nonsecure(unsigned char *buf, unsigned int len,
		unsigned int counter)
{
	int ret;
	__u32 k, count, block, chip_no;
	__u8 oob_buf[64];
	struct _nand_physic_op_par lnpo;
	struct _nand_chip_info *nci;

	PHY_ERR("m2_write_boot0_one_nonsecure\n");

	nci = g_nctri->nci;

	MEMSET(oob_buf, 0xff, 64);

	nand_get_version(oob_buf);

	// lsb enable
	PHY_DBG("lsb enalbe \n");
	PHY_DBG("read retry mode: 0x%x\n", m2_read_retry_mode);

	chip_no		   = nci->nctri_chip_no;
	nci->nctri_chip_no = 0;
	m2_lsb_init(nci);
	m2_lsb_enable(nci);
	nci->nctri_chip_no = chip_no;

	block = NAND_BOOT0_BLK_START + counter;

	PHY_ERR("block:%x \n", block);

	/*erase block*/
	lnpo.chip  = 0;
	lnpo.block = block;
	lnpo.page  = 0;

	nand_wait_all_rb_ready();

	ret = nci->nand_physic_erase_block(&lnpo);
	if (ret) {
		PHY_ERR("Fail in erasing block %d!\n", lnpo.block);
		goto error;
	}

	count = 0;
	for (k = 0; k < 4; k++) {
		lnpo.chip  = 0;
		lnpo.block = block;
		lnpo.page  = k;
		lnpo.sdata = oob_buf;
		lnpo.slen  = 64;

		if (nci->is_lsb_page(lnpo.page)) {
			lnpo.mdata = (__u8 *)(buf + count * (8 * 1024));
			count++;
			SECURE_FLAG = 0;

			nand_wait_all_rb_ready();
			if (nci->nand_write_boot0_page(nci, &lnpo) != 0) {
				PHY_ERR("Warning. Fail in writing page %d in block %d.\n",
						lnpo.page, lnpo.block);
			}
			if (count == (len + 8 * 1024 - 1) / (8 * 1024)) {
				count = 0;
			}
		}
	}
	nand_wait_all_rb_ready();

	chip_no		   = nci->nctri_chip_no;
	nci->nctri_chip_no = 0;
	m2_lsb_disable(nci);
	m2_lsb_exit(nci);
	nci->nctri_chip_no = chip_no;

	PHY_DBG("lsb disalbe \n");

	return 0;

error:
	chip_no		   = nci->nctri_chip_no;
	nci->nctri_chip_no = 0;
	m2_lsb_disable(nci);
	m2_lsb_exit(nci);
	nci->nctri_chip_no = chip_no;

	PHY_DBG("lsb disalbe \n");

	return -1;
}

int m2_write_boot0_one_secure(unsigned char *buf, unsigned int len,
		unsigned int counter)
{
	__u32 i, k, j, count, block, chip_no;
	__u8 oob_buf[64];
	__u32 tab_size, data_size_per_page;
	__u32 pages_per_block, copies_per_block;
	__u32 page_addr;
	__u32 *pos_data = NULL, *tab = NULL;
	//	__u8 *data_FF_buf=NULL;
	int ret;
	/*struct boot_ndfc_cfg cfg;*/
	struct _nand_chip_info *nci;
	struct _nand_physic_op_par lnpo;

	nci = g_nctri->nci;

	PHY_ERR("m2_write_boot0_one_secure\n");

	pos_data = (__u32 *)MALLOC(128 * 4 * BOOT0_MAX_COPY_CNT);
	if (!pos_data) {
		PHY_ERR("burn_boot0_lsb_FF_mode, malloc for pos_data failed.\n");
		goto error;
	}

	tab = (__u32 *)MALLOC(8 * 1024);
	if (!tab) {
		PHY_ERR("burn_boot0_lsb_FF_mode, malloc for tab failed.\n");
		goto error;
	}


	for (i = 0; i < 64; i++)
		oob_buf[i] = 0xff;

	/* get nand driver version */
	nand_get_version(oob_buf);
	if ((oob_buf[0] != 0xff) || (oob_buf[1] != 0x00)) {
		PHY_ERR("get flash driver version error!");
		goto error;
	}

	// lsb enable
	PHY_DBG("lsb enalbe \n");
	PHY_DBG("read retry mode: 0x%x\n", m2_read_retry_mode);

	chip_no		   = nci->nctri_chip_no;
	nci->nctri_chip_no = 0;
	m2_lsb_init(nci);
	m2_lsb_enable(nci);
	nci->nctri_chip_no = chip_no;

	data_size_per_page = 4096;
	pages_per_block    = nci->page_cnt_per_blk;
	copies_per_block   = pages_per_block / NAND_BOOT0_PAGE_CNT_PER_COPY_2;

	count = 0;
	for (i = NAND_BOOT0_BLK_START;
			i < (NAND_BOOT0_BLK_START + NAND_BOOT0_BLK_CNT); i++) {
		for (j = 0; j < copies_per_block; j++) {
			for (k = 8; k < NAND_BOOT0_PAGE_CNT_PER_COPY_2; k++) {
				page_addr = i * pages_per_block +
					j * NAND_BOOT0_PAGE_CNT_PER_COPY_2 +
					k;
				if (nci->is_lsb_page(
							(page_addr % pages_per_block))) {
					*((__u32 *)pos_data + count) =
						page_addr;
					count++;
					if (((count %
									(len / data_size_per_page)) ==
								0) &&
							(count != 0))
						break;
				}
			}
		}
	}

	_generate_page_map_tab(data_size_per_page,
			copies_per_block * NAND_BOOT0_BLK_CNT,
			len / data_size_per_page, pos_data, tab,
			&tab_size);

	block = NAND_BOOT0_BLK_START + counter;

	PHY_ERR("pagetab boot0 %x \n", block);

	lnpo.chip  = 0;
	lnpo.block = block;
	lnpo.page  = 0;
	nand_wait_all_rb_ready();

	ret = nci->nand_physic_erase_block(&lnpo);
	if (ret) {
		PHY_ERR("Fail in erasing block %d!\n", lnpo.block);
		//return ret;
	}

	for (j = 0; j < copies_per_block; j++) {
		count = 0;
		for (k = 0; k < NAND_BOOT0_PAGE_CNT_PER_COPY_2; k++) {
			lnpo.chip  = 0;
			lnpo.block = block;
			lnpo.page  = j * NAND_BOOT0_PAGE_CNT_PER_COPY_2 + k;
			lnpo.sdata = oob_buf;
			lnpo.slen  = 64;

			if (nci->is_lsb_page(lnpo.page)) {
				if (k < 8)
					lnpo.mdata = (__u8 *)tab;
				else {
					lnpo.mdata =
						(__u8 *)(buf +
								count * data_size_per_page);
					count++;
				}
				SECURE_FLAG = 1;

				nand_wait_all_rb_ready();
				if (nci->nand_write_boot0_page(nci, &lnpo) !=
						0) {
					PHY_ERR("Warning. Fail in writing page %d in block %d.\n",
							lnpo.page, lnpo.block);
				}
				if (count ==
						(len + data_size_per_page - 1) /
						data_size_per_page) {
					count = 0;
				}
			}
		}
	}

	chip_no		   = nci->nctri_chip_no;
	nci->nctri_chip_no = 0;
	m2_lsb_disable(nci);
	m2_lsb_exit(nci);
	nci->nctri_chip_no = chip_no;

	PHY_DBG("lsb disalbe \n");

	if (pos_data)
		FREE(pos_data, 128 * 4 * BOOT0_MAX_COPY_CNT);
	if (tab)
		FREE(tab, 8 * 1024);
	//	if(data_FF_buf)
	//		FREE(data_FF_buf,18048);
	return 0;

error:
	chip_no		   = nci->nctri_chip_no;
	nci->nctri_chip_no = 0;
	m2_lsb_disable(nci);
	m2_lsb_exit(nci);
	nci->nctri_chip_no = chip_no;

	PHY_DBG("lsb disalbe \n");

	if (pos_data)
		FREE(pos_data, 128 * 4 * BOOT0_MAX_COPY_CNT);
	if (tab)
		FREE(tab, 8 * 1024);
	//	if(data_FF_buf)
	//		FREE(data_FF_buf,18048);
	return -1;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int m2_write_boot0_one(unsigned char *buf, unsigned int len,
		unsigned int counter)
{
	if (NAND_IS_Secure_sys() == 0)
		return (m2_write_boot0_one_nonsecure(buf, len, counter));
	else if (NAND_IS_Secure_sys() == 1)
		return (m2_write_boot0_one_secure(buf, len, counter));
	return 0;
}
