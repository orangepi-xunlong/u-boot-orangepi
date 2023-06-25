//SPDX-License-Identifier:	GPL-2.0+
/*
 ************************************************************************************************************************
 *                                                      eNand
 *                                           Nand flash driver scan module
 *
 *                             Copyright(C), 2008-2009, SoftWinners Microelectronic Co., Ltd.
 *                                                  All Rights Reserved
 *
 * File Name : nand_chip_op.c
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
#define _NC_C_
#include "../nand_physic_inc.h"

extern const u8 rand_factor_1st_spare_data[4][128];

/*****************************************************************************
 *Name         : _dma_config_start
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
int ndfc_dma_config_start(struct _nand_controller_info *nctri, u8 rw,
		void *buff_addr, u32 len)
{
	u32 reg_val;

	NAND_CleanFlushDCacheRegion(buff_addr, len);

	if (nctri->dma_type == 1) {//MBUS DMA
		nctri->dma_addr =
			(unsigned long)NAND_DMASingleMap(rw, buff_addr, len);
		//set mbus dma mode
		reg_val = *nctri->nreg.reg_ctl;
		reg_val &= ~(0x1U << 15);
		reg_val |= 0x1U << 14;
		*nctri->nreg.reg_ctl	   = reg_val;
		*nctri->nreg.reg_mbus_dma_addr = nctri->dma_addr;
		*nctri->nreg.reg_dma_cnt       = len;
	} else if (nctri->dma_type == 0) {//General DMA
		reg_val = *nctri->nreg.reg_ctl;
		reg_val |= 0x1U << 15;
		reg_val |= 0x1U << 14;
		*nctri->nreg.reg_ctl     = reg_val;
		*nctri->nreg.reg_dma_cnt = len;

		nctri->dma_addr =
			(unsigned long)NAND_DMASingleMap(rw, buff_addr, len);
		nand_dma_config_start(rw, nctri->dma_addr, len);
	} else {
		PHY_ERR("_dma_config_start, wrong dma mode, %d\n",
				nctri->dma_type);
		return ERR_NO_51;
	}

	return 0;
}

/*****************************************************************************
 *Name         : _wait_dma_end
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
s32 ndfc_wait_dma_end(struct _nand_controller_info *nctri, u8 rw,
		void *buff_addr, u32 len)
{
	s32 ret;

	if (nctri->write_wait_dma_mode != 0) {
		ndfc_enable_dma_int(nctri);
		if (nctri->dma_ready_flag == 1) {
			goto wait_dma_ready_int;
		}
		if (nand_dma_wait_time_out(nctri->channel_id,
					&nctri->dma_ready_flag) == 0) {
			PHY_ERR("_wait_dma_ready_int int timeout, ch: 0x%x, sta: 0x%x\n",
					nctri->channel_id, *nctri->nreg.reg_sta);
		}
	}

wait_dma_ready_int:
	nctri->dma_ready_flag = 0;
	ret = wait_reg_status(nctri->nreg.reg_sta, NDFC_DMA_INT_FLAG,
			NDFC_DMA_INT_FLAG, 0xfffff);
	if (ret != 0) {
		PHY_ERR("nand _wait_dma_end timeout, NandIndex: 0x%x, rw: 0x%x, status:0x%x\n",
				nctri->channel_id, (u32)rw, *nctri->nreg.reg_sta);
		//PHY_ERR("DMA addr: 0x%x, DMA len: 0x%x\n", *nctri->nreg.reg_mbus_dma_addr, *nctri->nreg.reg_dma_cnt);
		//show_nctri(nctri);
		ndfc_print_save_reg(nctri);
		ndfc_print_reg(nctri);
		//return ret;
		ret = wait_reg_status(nctri->nreg.reg_sta, NDFC_DMA_INT_FLAG,
				NDFC_DMA_INT_FLAG, 0xffffff);
		PHY_ERR("ret !!!!:0x%x\n", ret);
		if (ret == 0) {
			ndfc_print_reg(nctri);
		}
	}

	ndfc_clear_dma_int(nctri);
	ndfc_disable_dma_int(nctri);

	*nctri->nreg.reg_sta = (*nctri->nreg.reg_sta & NDFC_DMA_INT_FLAG);
	NAND_DMASingleUnmap(rw, (void *)(unsigned long)nctri->dma_addr, len);
	NAND_InvaildDCacheRegion(rw, (unsigned long)nctri->dma_addr, len);

	return ret;
}

/*****************************************************************************
 *Name         : ndfc_check_read_data_sta
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
s32 ndfc_check_read_data_sta(struct _nand_controller_info *nctri,
		u32 eblock_cnt)
{
	s32 i, flag;

	//check all '1' or all '0' status

	if ((*nctri->nreg.reg_ecc_sta >> 16) == ((0x1U << eblock_cnt) - 1)) {
		flag = *nctri->nreg.reg_pat_id & 0x3;
		for (i = 1; i < eblock_cnt; i++) {
			if (flag != (*nctri->nreg.reg_pat_id >> (i * 2) & 0x3))
				break;
		}

		if (i == eblock_cnt) {
			if (flag) {
				//PHY_DBG("read data sta, all '1'\n");
				return 4; //all input bits are '1'
			} else {
				//PHY_DBG("read data sta, all '0'\n");
				return 3; //all input bit are '0'
			}
		} else {
			PHY_DBG("read data sta, some ecc blocks are all '1', others are all '0' %x %x\n",
					*nctri->nreg.reg_ecc_sta,
					*nctri->nreg.reg_pat_id);
			return 2;
		}
	} else if ((*nctri->nreg.reg_ecc_sta >> 16) == 0x0) {
		return 0;
	} else {
		//PHY_ERR("!!!!read data sta, only some ecc blocks are all '1' or all '1', 0x%x 0x%x\n", *nctri->nreg.reg_ecc_sta, *nctri->nreg.reg_pat_id);
		return 1;
	}
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
extern int is_nouse_page(u8 *buf);

s32 ndfc_get_bad_blk_flag(u32 page_no, u32 dis_random, u32 slen, u8 *sbuf)
{
	u8 _1st_spare_byte[16];
	u32 i, len, num;

	num = page_no % 128;
	len = slen;

	if (len > 16)
		len = 16;

	//PHY_DBG("ndfc get_bad_blk_flag, %d %d %d 0x%x\n", page_no, dis_random, slen, sbuf);
	if (sbuf == NULL) {
		PHY_ERR("ndfc get_bad_blk_flag, input parameter error!\n");
		return ERR_NO_50;
	}

	for (i = 0; i < 16; i++) {
		_1st_spare_byte[i] = 0xff;
	}

	if (dis_random) {
		for (i = 0; i < len; i++) {
			_1st_spare_byte[i] =
				sbuf[i] ^
				rand_factor_1st_spare_data[i % 4][num];
		}
	} else {
		for (i = 0; i < len; i++) {
			_1st_spare_byte[i] = sbuf[i];
		}
	}

	//if (_1st_spare_byte[0] == 0xff)
	if (is_nouse_page(_1st_spare_byte) == 1) {
		//PHY_DBG("good page flag, page %d\n", page_no);
		//		if((page_no != 0)&&(page_no != 255))
		//		{
		//		    PHY_ERR("blank page:%d\n",page_no);
		//		}

		return 0;
	} else {
		//PHY_DBG("bad page flag, page %d\n", page_no);
		return ERR_NO_49;
	}
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
s32 ndfc_check_ecc(struct _nand_controller_info *nctri, u32 eblock_cnt)
{
	u32 i, ecc_limit, cfg;
	u32 ecc_cnt_w[4];
	u8 ecc_cnt;
	/*u8 ecc_tab[12] = {16, 24, 28, 32, 40, 48, 56, 60, 64, 72};*/
	u8 ecc_limit_tab[12] = { 13, 20, 23, 27, 35, 42, 50, 54, 58, 66 };

	//	max_ecc_bit_cnt = ecc_tab[(*nctri->nreg.reg_ecc_ctl >> 12) & 0x0f];
	ecc_limit = ecc_limit_tab[(*nctri->nreg.reg_ecc_ctl >> 12) & 0x0f];

	//check ecc errors
	cfg = (1 << eblock_cnt) - 1;
	//PHY_ERR("check ecc: %d 0x%x 0x%x\n", nctri->nci->randomizer, *nctri->nreg.reg_ecc_ctl, *nctri->nreg.reg_ecc_sta);
	if ((*nctri->nreg.reg_ecc_sta & cfg) != 0) {
		return ERR_ECC;
	}

	//check ecc limit
	ecc_cnt_w[0] = *nctri->nreg.reg_err_cnt0;
	ecc_cnt_w[1] = *nctri->nreg.reg_err_cnt1;
	ecc_cnt_w[2] = *nctri->nreg.reg_err_cnt2;
	ecc_cnt_w[3] = *nctri->nreg.reg_err_cnt3;

	for (i = 0; i < eblock_cnt; i++) {
		ecc_cnt = (u8)(ecc_cnt_w[i >> 2] >> ((i % 4) << 3));
		if (ecc_cnt > ecc_limit) {
			//PHY_ERR("ecc limit: 0x%x 0x%x 0x%x 0x%x   %d  %d  %d\n", ecc_cnt_w[0],ecc_cnt_w[1],ecc_cnt_w[2],ecc_cnt_w[3],ecc_cnt,ecc_limit,max_ecc_bit_cnt);
			return ECC_LIMIT;
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
s32 ndfc_update_ecc_sta_and_spare_data(struct _nand_physic_op_par *npo,
		s32 ecc_sta, unsigned char *sbuf)
{
	unsigned char *buf;
	unsigned int len;
	s32 ret_ecc_sta, ret1 = 0;
	struct _nand_chip_info *nci = nci_get_from_nsi(g_nsi, npo->chip);

	if (npo->slen != 0) {
		buf = npo->sdata;
		len = npo->slen;
	} else {
		buf = sbuf;
		len = nci->sdata_bytes_per_page;
	}

	if (npo->slen > nci->sdata_bytes_per_page) {
		len = nci->sdata_bytes_per_page;
	}

	//check input data status
	ret1 = ndfc_check_read_data_sta(
			nci->nctri, nci->sector_cnt_per_page >> nci->ecc_sector);

	ret_ecc_sta = ecc_sta;

	if (nci->randomizer) {
		if (ecc_sta == ERR_ECC) {
			//PHY_DBG("update ecc, %d %d %d 0x%x\n", npo->page, nci->randomizer, npo->slen, npo->sdata);
			if (ndfc_get_bad_blk_flag(npo->page, nci->randomizer,
						len, buf) == 0) {
				//when the 1st byte of spare data is 0xff after randomization, this page is blank
				//PHY_DBG("randomizer blank page\n");
				MEMSET(buf, 0xff, len);
				ret_ecc_sta = 0;
			}
		}

		if (ret1 == 3) {
			//all data bits are '0', no ecc error, this page is a bad page
			//PHY_DBG("randomizer all bit 0\n");
			MEMSET(buf, 0x0, len);
			ret_ecc_sta = ERR_ECC;
		}
	} else {
		if (ret1 == 3) {
			//all data bits are '0', don't do ecc(ecc exception), this page is a bad page
			//PHY_DBG("no randomizer all bit 0\n");
			ret_ecc_sta = ERR_ECC;
		} else if (ret1 == 4) {
			//all data bits are '1', don't do ecc(ecc exception), this page is a good page
			//PHY_DBG("no randomizer all bit 1\n");
			ret_ecc_sta = 0;
		}
	}

	return ret_ecc_sta;
}
