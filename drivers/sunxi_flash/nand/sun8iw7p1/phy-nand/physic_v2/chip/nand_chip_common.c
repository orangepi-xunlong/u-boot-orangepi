//SPDX-License-Identifier:	GPL-2.0+
/*
 ************************************************************************************************************************
 *                                                      eNand
 *                                           Nand flash driver scan module
 *
 *                             Copyright(C), 2008-2009, SoftWinners Microelectronic Co., Ltd.
 *                                                  All Rights Reserved
 *
 * File Name : nand_chip_function.c
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

#define _NCC_C_

#include "../nand_physic_inc.h"
#include "../../../osal/nand_osal.h"
#include <stdio.h>

extern struct _nand_phy_info_par nand_tbl[];
extern struct _optional_phy_op_par physic_arch_para[];
extern struct _nfc_init_ddr_info def_ddr_info[];

extern int fdt_getprop_u32(const void *fdt, int nodeoffset, const char *name,
		unsigned int *val);
extern int fdt_path_offset(const void *fdt, const char *path);
extern void nand_enable_vcc_3v(void);
extern void nand_enable_vccq_1p8v(void);

int g_need_vccq_1p8v;
struct fdt_header;
extern struct fdt_header *working_fdt;

int nand_script_parser_fetch(char *node_path, char *prop_name,
		unsigned int *value, int count)
{
	int ret = 0;
	//get property vaule by handle
	int nodeoffset = fdt_path_offset(working_fdt, node_path);
	if (nodeoffset < 0) {
		printf("fdt to get %s nodeoffset fail\n", node_path);
		return -1;
	}

	ret = fdt_getprop_u32(working_fdt, nodeoffset, prop_name, value);
	if (ret < 0) {
		printf("get %s valu fail\n", prop_name);
		return -1;
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
int nci_add_to_nsi(struct _nand_storage_info *nsi, struct _nand_chip_info *node)
{
	struct _nand_chip_info *nci;

	node->chip_no = 0;
	if (nsi->nci == NULL) {
		nsi->nci = node;
		return NAND_OP_TRUE;
	}

	nci	   = nsi->nci;
	node->chip_no = 1;
	while (nci->nsi_next != NULL) {
		nci = nci->nsi_next;
		node->chip_no++;
	}
	nci->nsi_next = node;

	PHY_DBG("nci_add_to_nsi: %d 0x%x\n", node->chip_no, node);
	return NAND_OP_TRUE;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nci_add_to_nctri(struct _nand_controller_info *nctri,
		struct _nand_chip_info *node)
{
	struct _nand_chip_info *nci;

	node->nctri_chip_no = 0;
	if (nctri->nci == NULL) {
		nctri->nci = node;
		return NAND_OP_TRUE;
	}

	nci		    = nctri->nci;
	node->nctri_chip_no = 1;
	while (nci->nctri_next != NULL) {
		nci = nci->nctri_next;
		node->nctri_chip_no++;
	}
	nci->nctri_next = node;

	PHY_DBG("nci_add_to_nctri: %d 0x%x\n", node->nctri_chip_no, node);
	return NAND_OP_TRUE;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
struct _nand_chip_info *nci_get_from_nctri(struct _nand_controller_info *nctri,
		unsigned int num)
{
	int i;
	struct _nand_chip_info *nci;

	for (i = 0, nci = nctri->nci; i < num; i++)
		nci = nci->nctri_next;

	return nci;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
struct _nand_chip_info *nci_get_from_nsi(struct _nand_storage_info *nsi,
		unsigned int num)
{
	int i;
	struct _nand_chip_info *nci;

	for (i = 0, nci = nsi->nci; i < num; i++)
		nci = nci->nsi_next;

	return nci;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
struct _nand_phy_info_par *search_id(char *id)
{
	int i, j, ok_bytes, ok_ptr, nums;
	struct _nand_phy_info_par *id_ptr = &nand_tbl[0];

	ok_ptr   = 0xffff;
	ok_bytes = 0;

	for (j = 0; id_ptr->nand_id[0] != 0xff; j++, id_ptr++) {
		for (i = 0, nums = 0; i < 6; i++) {
			if (!((id_ptr->nand_id[i] == id[i]) ||
						(id_ptr->nand_id[i] == 0xff))) {
				break;
			}
			if (id_ptr->nand_id[i] != 0xff) {
				nums++;
			}
		}
		if (i != 6) {
			continue;
		}
		if (nums > ok_bytes) {
			ok_bytes = nums;
			ok_ptr   = j;
		}
		if (ok_bytes == 6) {
			break;
		}
	}

	if (ok_ptr != 0xffff) {
		id_ptr = &nand_tbl[0];
		return (id_ptr + ok_ptr);
	}

	return NULL;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int init_nci_from_id(struct _nand_chip_info *nci,
		struct _nand_phy_info_par *npi)
{
	MEMCPY(nci->id, npi->nand_id, 8);

	nci->npi	       = npi;
	nci->opt_phy_op_par    = &physic_arch_para[npi->option_phyisc_op_no];
	nci->nfc_init_ddr_info = &def_ddr_info[npi->ddr_info_no];

	nci->blk_cnt_per_chip    = npi->die_cnt_per_chip * npi->blk_cnt_per_die;
	nci->sector_cnt_per_page = npi->sect_cnt_per_page;
	nci->page_cnt_per_blk    = npi->page_cnt_per_blk;
	nci->page_offset_for_next_blk = npi->page_cnt_per_blk;
	nci->ecc_mode		      = npi->ecc_mode;
	nci->max_erase_times	  = npi->max_blk_erase_times;
	nci->driver_no		      = npi->driver_no;
	nci->randomizer    = (npi->operation_opt & NAND_RANDOM) ? 1 : 0;
	nci->lsb_page_type = ((npi->operation_opt) & NAND_LSB_PAGE_TYPE) >> 12;

	/* nand interface */
	nci->interface_type =
		npi->ddr_type; //0x0: sdr; 0x2: nvddr; 0x3: tgddr; 0x12: nvddr2; 0x13: tgddr2;
	if (g_phy_cfg->phy_interface_cfg == 1) {
		if ((nci->interface_type == 0x3) ||
				(nci->interface_type == 0x13)) {
			nci->interface_type = 0x03;
		} else if ((nci->interface_type == 0x2) ||
				(nci->interface_type == 0x12)) {
			nci->interface_type = 0x02;
		} else {
			nci->interface_type = 0x00;
		}
	}

	nci->frequency = npi->access_freq;
	//if(nci->frequency > 40)
	//{
	//    nci->frequency = 40;
	//}

	nci->timing_mode = 0x0;
	nci->support_change_onfi_timing_mode =
		(npi->operation_opt & NAND_TIMING_MODE) ? 1 : 0;
	nci->support_ddr2_specific_cfg =
		(npi->operation_opt & NAND_DDR2_CFG) ? 1 : 0;
	nci->support_io_driver_strength =
		(npi->operation_opt & NAND_IO_DRIVER_STRENGTH) ? 1 : 0;
	nci->support_vendor_specific_cfg =
		(npi->operation_opt & NAND_VENDOR_SPECIFIC_CFG) ? 1 : 0;
	nci->support_onfi_sync_reset =
		(npi->operation_opt & NAND_ONFI_SYNC_RESET_OP) ? 1 : 0;
	nci->support_toggle_only =
		(npi->operation_opt & NAND_TOGGLE_ONLY) ? 1 : 0;

	nci->ecc_sector = 1;
	nci->sdata_bytes_per_page =
		(nci->sector_cnt_per_page >> nci->ecc_sector) * 4;

	nci->nand_physic_erase_block =
		nand_physic_function[nci->driver_no].nand_physic_erase_block;
	nci->nand_physic_read_page =
		nand_physic_function[nci->driver_no].nand_physic_read_page;
	nci->nand_physic_write_page =
		nand_physic_function[nci->driver_no].nand_physic_write_page;
	nci->nand_physic_bad_block_check =
		nand_physic_function[nci->driver_no].nand_physic_bad_block_check;
	nci->nand_physic_bad_block_mark =
		nand_physic_function[nci->driver_no].nand_physic_bad_block_mark;
	nci->nand_read_boot0_page =
		nand_physic_function[nci->driver_no].nand_read_boot0_page;
	nci->nand_write_boot0_page =
		nand_physic_function[nci->driver_no].nand_write_boot0_page;
	nci->nand_read_boot0_one =
		nand_physic_function[nci->driver_no].nand_read_boot0_one;
	nci->nand_write_boot0_one =
		nand_physic_function[nci->driver_no].nand_write_boot0_one;

	nci->is_lsb_page = nand_physic_function[nci->driver_no].is_lsb_page;

	nand_physic_special_init =
		nand_physic_function[nci->driver_no].nand_physic_special_init;
	nand_physic_special_exit =
		nand_physic_function[nci->driver_no].nand_physic_special_exit;

	//update according to external configuration
	if ((!SUPPORT_CHANGE_ONFI_TIMING_MODE) ||
			(nci->support_onfi_sync_reset == 0)) {
		/* if don't support onfi sync reset op, forbid changing onfi timing mode */
		nci->support_change_onfi_timing_mode = 0;
	}

	return NAND_OP_TRUE;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_build_nsi(struct _nand_storage_info *nsi,
		struct _nand_controller_info *nctri)
{
	int channel, chips;
	char id[8]		    = { 0 };
	int ret			    = 0;
	unsigned int support_3D_MLC = 0x55aaaa55;
	unsigned int nand_vccq_1p8v = 0x55aaaa55;
	unsigned int delay1;

	struct _nand_phy_info_par *nand_id_tbl;
	struct _nand_chip_info *nci;
	struct _nand_chip_info lnci;

	for (channel = 0; channel < MAX_CHANNEL; channel++) {
		if (nctri == NULL) {
			PHY_ERR("invalid nctri!\n");
			return NAND_OP_FALSE;
		}

		nand_enable_vcc_3v();
		for (chips = 0; chips < MAX_CHIP_PER_CHANNEL; chips++) {
			MEMSET(&lnci, 0, sizeof(struct _nand_chip_info));
			lnci.nctri   = nctri;
			lnci.chip_no = nsi->chip_cnt;

			if (nand_first_reset_chip(&lnci, chips) !=
					NAND_OP_TRUE) {
				PHY_ERR("nand_reset_chip error %d\n",
						lnci.chip_no);
			}

			if (g_need_vccq_1p8v == 0) {
				ret = nand_script_parser_fetch("nand0",
						"nand_vccq_1p8v",
						&nand_vccq_1p8v,
						1);
				if (ret < 0) {
					PHY_DBG("nand not need vccq 1.8v\n");
				} else if (nand_vccq_1p8v == 1) {
					nand_enable_vccq_1p8v();
					//PHY_DBG("l04a_vccq_enable 2\n");
					for (delay1 = 150; delay1 > 0; delay1--)
						;
					//PHY_DBG("l04a_vccq_enable\n");
					g_need_vccq_1p8v = 1;
				}
			}
#if 1 /* reset and read id twice for L04a BUG */
			/*reset L04a and read id firstly*/
			if (nand_first_reset_chip(&lnci, chips) !=
					NAND_OP_TRUE) {
				PHY_ERR("nand_reset_chip error %d\n",
						lnci.chip_no);
			}
			for (delay1 = 150; delay1 > 0; delay1--)
				;
			if (nand_first_read_id(&lnci, chips, id) !=
					NAND_OP_TRUE) {
				PHY_ERR("nand_read_id error %d\n",
						lnci.chip_no);
				//break;
			}
#endif
			/*reset L04a and read id secondly*/
			if (nand_first_reset_chip(&lnci, chips) !=
					NAND_OP_TRUE) {
				PHY_ERR("nand_reset_chip error %d\n",
						lnci.chip_no);
				break;
			}
			for (delay1 = 150; delay1 > 0; delay1--)
				;
			//PHY_ERR("[L04a] read id 2 \n");
			if (nand_first_read_id(&lnci, chips, id) !=
					NAND_OP_TRUE) {
				PHY_ERR("nand_read_id error %d\n",
						lnci.chip_no);
				break;
			}

			/*
			 *此处增加这是为了对于客户，3D MLC区别对待
			 */

			ret = nand_script_parser_fetch("nand0",
					"nand0_support_3D_MLC",
					&support_3D_MLC, 1);
			if (ret < 0 || support_3D_MLC != 0x04A) {
				PHY_ERR("ret = %d, support_3D_MLC = %x\n", ret,
						support_3D_MLC);
				if ((id[0] == 0x2c) && (id[1] == 0x64) &&
						(id[2] == 0x44) && (id[3] == 0x32) &&
						(id[4] == 0xa5)) {//this is L04A
					PHY_DBG("nand not support! channel %d chip %d: %02x %02x %02x %02x %02x %02x %02x %02x\n",
							channel, chips, id[0], id[1],
							id[2], id[3], id[4], id[5],
							id[6], id[7]);
					return NAND_OP_FALSE;
				}
			} else {
				PHY_DBG("ret = %d, support_3D_MLC = %x\n", ret,
						support_3D_MLC);
			}

			nand_id_tbl = search_id(id);
			if (nand_id_tbl == NULL) {
				PHY_DBG("nand not support! channel %d chip %d: %02x %02x %02x %02x %02x %02x %02x %02x\n",
						channel, chips, id[0], id[1], id[2],
						id[3], id[4], id[5], id[6], id[7]);
				continue;
			} else {
				PHY_DBG("channel %d chip %d: %02x %02x %02x %02x %02x %02x %02x %02x\n",
						channel, chips, id[0], id[1], id[2],
						id[3], id[4], id[5], id[6], id[7]);
			}

			if (init_nci_from_id(&lnci, nand_id_tbl) !=
					NAND_OP_TRUE) {
				PHY_ERR("init_nci_from_id error %d\n",
						lnci.chip_no);
				break;
			}

			//nci = (struct _nand_chip_info *)MALLOC(sizeof(struct _nand_chip_info));
			nci = &nci_data[channel * MAX_CHIP_PER_CHANNEL + chips];
			MEMCPY(nci, &lnci, sizeof(struct _nand_chip_info));

			nsi->chip_cnt++;
			nci_add_to_nsi(nsi, nci);

			nctri->chip_cnt++;
			nctri->chip_connect_info |= (0x1U << chips);
			nci_add_to_nctri(nctri, nci);
		}

		// walk to next channel
		nctri = nctri->next;
	}

	if (nsi->chip_cnt == 0) {
		PHY_ERR(" no nand found !\n");
		return NAND_OP_FALSE;
	} else {
		return NAND_OP_TRUE;
	}
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok 1:find good block 2:old arch -1:fail
 *Note         :
 *****************************************************************************/
int read_nand_structure(void *phy_arch, unsigned int *good_blk_no)
{
	int i, retry = 3, ret, ret2 = -1;
	unsigned int b, chip = 0;
	unsigned int start_blk = 12, blk_cnt = 20;
	unsigned char oob[64];
	//unsigned char *oob = NULL;
	struct _nand_permanent_data *parch;
	struct _nand_physic_op_par npo;

#if 0
	oob = (unsigned char *)NAND_Malloc(64);
	if (oob == NULL) {
		PHY_ERR("malloc oob fail in %s\n", __func__);
		return -1;
	}
#endif
	parch = (struct _nand_permanent_data *)nand_get_temp_buf(64 * 1024);

	for (b = start_blk; b < start_blk + blk_cnt; b++) {
		for (i = 0; i < retry; i++) {
			npo.chip	= chip;
			npo.block       = b;
			npo.page	= i;
			npo.mdata       = (u8 *)parch;
			npo.sect_bitmap = g_nctri->nci->sector_cnt_per_page;
			npo.sdata       = oob;
			npo.slen	= g_nctri->nci->sdata_bytes_per_page;

			ret = g_nctri->nci->nand_physic_read_page(&npo);
			if (oob[0] == 0x00) {
				if (ret >= 0) {
					if ((ret >= 0) && (oob[1] == 0x78) &&
							(oob[2] == 0x69) &&
							(oob[3] == 0x87) &&
							(oob[4] == 0x41) &&
							(oob[5] == 0x52) &&
							(oob[6] == 0x43) &&
							(oob[7] == 0x48)) {
						MEMCPY(phy_arch, parch,
								sizeof(struct _nand_permanent_data));
						PHY_DBG("search nand structure %d  %d: get last physic arch ok 0x%x 0x%x!\n",
								npo.block, npo.page,
								parch->support_two_plane,
								parch->support_vertical_interleave);
						ret2 = 0;
						goto search_end;
					}
					if ((ret >= 0) && (oob[1] == 0x50) &&
							(oob[2] == 0x48) &&
							(oob[3] == 0x59) &&
							(oob[4] == 0x41) &&
							(oob[5] == 0x52) &&
							(oob[6] == 0x43) &&
							(oob[7] == 0x48)) {
						MEMCPY(phy_arch, parch,
								sizeof(struct _nand_permanent_data));
						PHY_DBG("search nand structure %d  %d: get old physic arch ok 0x%x 0x%x!\n",
								npo.block, npo.page,
								parch->support_two_plane,
								parch->support_vertical_interleave);
						ret2 = 2;
						goto search_end;
					}
				} else {
					PHY_DBG("search nand structure: bad block no physic arch info!\n");
				}
			} else if (oob[0] == 0xff) {
				if ((ret >= 0) && (i == 0)) {
					PHY_DBG("search nand structure: find a good block: %d, but no physic arch info.\n",
							npo.block);
					ret2 = 1;
					goto search_end;
				} else {
					PHY_DBG("search nand structure: blank block!\n");
				}
			} else {
				PHY_DBG("search nand structure: unkonwn %d!\n",
						oob[0]);
			}
		}
	}

search_end:

	if (b == (start_blk + blk_cnt)) {
		ret2	 = ERR_NO_58;
		*good_blk_no = 0;
	} else {
		*good_blk_no = b;
	}

	nand_free_temp_buf((u8 *)parch, 64 * 1024);

	return ret2;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int write_nand_structure(void *phy_arch, unsigned int good_blk_no,
		unsigned int blk_cnt)
{
	int retry = 3, ret, ret2 = ERR_NO_130;
	unsigned int b, p, ok, chip = 0;
	unsigned char oob[64];
	//	unsigned char *oob = NULL;
	struct _nand_physic_op_par npo;
#if 0
	oob = (unsigned char *)NAND_Malloc(64);
	if (oob == NULL) {
		PHY_ERR("oob malloc fail in %s\n", __func__);
		return -1;
	}
#endif
	PHY_DBG("write nand structure: write physic arch to blk %d...\n",
			good_blk_no);

	for (b = good_blk_no; b < good_blk_no + blk_cnt; b++) {
		npo.chip	= chip;
		npo.block       = b;
		npo.page	= 0;
		npo.mdata       = (unsigned char *)phy_arch;
		npo.sect_bitmap = g_nctri->nci->sector_cnt_per_page;
		npo.sdata       = oob;
		npo.slen	= g_nctri->nci->sdata_bytes_per_page;

		ret = g_nctri->nci->nand_physic_erase_block(
				&npo); //PHY_SimpleErase_CurCH(&nand_op);
		if (ret < 0) {
			PHY_ERR("write nand structure: erase chip %d, block %d error\n",
					npo.chip, npo.block);
			MEMSET(oob, 0, 64);

			for (p = 0; p < g_nctri->nci->page_cnt_per_blk; p++) {
				npo.page = p;
				ret      = g_nctri->nci->nand_physic_write_page(
						&npo);
				nand_wait_all_rb_ready();
				if (ret < 0) {
					PHY_ERR("write nand structure: mark bad block, write chip %d, block %d, page %d error\n",
							npo.chip, npo.block, npo.page);
				}
			}
		} else {
			PHY_DBG("write nand structure: erase block %d ok.\n",
					b);
			MEMSET(oob, 0x88, 64);
			oob[0] = 0x00; //bad block flag
			oob[1] = 0x78; //'N'
			oob[2] = 0x69; //'E'
			oob[3] = 0x87; //'W'
			oob[4] = 0x41; //'A'
			oob[5] = 0x52; //'R'
			oob[6] = 0x43; //'C'
			oob[7] = 0x48; //'H'

			for (ok = 0, p = 0; p < g_nctri->nci->page_cnt_per_blk;
					p++) {
				npo.page = p;
				ret      = g_nctri->nci->nand_physic_write_page(
						&npo);
				nand_wait_all_rb_ready();
				if (ret != 0) {
					PHY_ERR("write nand structure: write chip %d, block %d, page %d error\n",
							npo.chip, npo.block, npo.page);
				} else {
					if (p < retry) {
						ok = 1;
					}
				}
			}

			if (ok == 1) {
				ret2 = 0;
				break;
			}
		}
	}

	return ret2;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         : only for erase boot
 *****************************************************************************/
int set_nand_structure(void *phy_arch)
{
	int ret;
	unsigned good_blk_no = 12;

	void *data;
	struct _nand_permanent_data *parch;
	struct __NandStorageInfo_t *old_parch;

	data = (void *)nand_get_temp_buf(4096);

	ret = read_nand_structure(data, &good_blk_no);
	if (ret == 1) {
		PHY_ERR("search nand structure: ok arch\n");
	} else if (ret == 2) {
		PHY_ERR("search nand structure: old arch\n");
		old_parch = (struct __NandStorageInfo_t *)data;
		parch     = (struct _nand_permanent_data *)phy_arch;

		parch->magic_data	= MAGIC_DATA_FOR_PERMANENT_DATA;
		parch->support_two_plane = 0;
		parch->support_vertical_interleave = 1;
		parch->support_dual_channel	= 1;
		if (old_parch->PlaneCntPerDie == 2) {
			parch->support_two_plane = 1;
		}
	} else if (ret == 0) {
		PHY_ERR("never be here! store nand structure\n");
	} else {
		PHY_ERR("search nand structure: can not find good block: 12~112\n");
		nand_free_temp_buf((u8 *)data, 4096);
		return ret;
	}

	ret = write_nand_structure(phy_arch, good_blk_no, 20);
	if (ret != 0) {
		PHY_ERR("write nand structure fail1\n");
	}

	ret = read_nand_structure(data, &good_blk_no);
	if (ret != 0) {
		PHY_ERR("write nand structure: can not find nand structure: 12~112\n");
	}

	nand_free_temp_buf((u8 *)data, 4096);

	return 0;
}

__u32 NAND_GetLsbblksize(void)
{
	__u32 i, count = 0;
	struct _nand_chip_info *nci;

	nci = g_nctri->nci;

	for (i = 0; i < nci->page_cnt_per_blk; i++) {
		if (1 == nci->is_lsb_page(i))
			count++;
	}
	return count * (nci->sector_cnt_per_page << 9);
}

__u32 NAND_GetLsbPages(void)
{
	__u32 i, count = 0;
	struct _nand_chip_info *nci;

	nci = g_nctri->nci;

	for (i = 0; i < nci->page_cnt_per_blk; i++) {
		if (1 == nci->is_lsb_page(i))
			count++;
	}
	return count;
}

__u32 NAND_GetPhyblksize(void)
{
	struct _nand_chip_info *nci;

	nci = g_nctri->nci;
	return nci->page_cnt_per_blk * (nci->sector_cnt_per_page << 9);
}

__u32 NAND_UsedLsbPages(void)
{
	__u32 i, count = 0;
	struct _nand_chip_info *nci;

	nci = g_nctri->nci;

	for (i = 0; i < nci->page_cnt_per_blk; i++) {
		if (1 == nci->is_lsb_page(i))
			count++;
	}

	if (count == nci->page_cnt_per_blk) {
		return 0;
	}
	return 1;
}

u32 NAND_GetPageNo(u32 lsb_page_no)
{
	u32 i, count = 0;
	struct _nand_chip_info *nci;

	nci = g_nctri->nci;

	for (i = 0; i < nci->page_cnt_per_blk; i++) {
		if (1 == nci->is_lsb_page(i))
			count++;
		if (count == (lsb_page_no + 1))
			break;
	}
	return i;
}
