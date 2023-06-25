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
#define _NPI_C_

#include "nand_physic_inc.h"

extern int NAND_ReleaseDMA(__u32 nand_index);
extern int NAND_ReleaseVoltage(void);
extern int nand_exit_temp_buf(struct _nand_temp_buf *nand_temp_buf);

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_physic_init(void)
{
	PHY_DBG("nand_physic_init\n");

	if (init_parameter() != 0) {
		PHY_ERR("nand_physic_init init_parameter error\n");
		return NAND_OP_FALSE;
	}

	if (nand_build_nsi(g_nsi, g_nctri) != 0) {
		PHY_ERR("nand_physic_init nand_build_nsi error\n");
		return NAND_OP_FALSE;
	}

	if (check_nctri(g_nctri) != 0) {
		PHY_ERR("nand_physic_init check_nctri error\n");
		return NAND_OP_FALSE;
	}

	set_nand_script_frequence();

	if (update_nctri(g_nctri) != 0) {
		PHY_ERR("nand_physic_init update_nctri error\n");
		return NAND_OP_FALSE;
	}

	nand_physic_special_init();

	physic_info_read();

	if (nand_build_nssi(g_nssi, g_nctri) != 0) {
		PHY_ERR("nand_physic_init nand_build_nssi error\n");
		return NAND_OP_FALSE;
	}

	nand_build_storage_info();

	show_static_info();

	nand_code_info();

	//nand_phy_test();

	PHY_DBG("nand_physic_init end\n");

	return NAND_OP_TRUE;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_physic_exit(void)
{
	struct _nand_controller_info *nctri = g_nctri;

	PHY_DBG("nand_physic_exit\n");

	if (nand_physic_special_exit != NULL)
		nand_physic_special_exit();

	while (nctri != NULL) {
		NAND_ClkRelease(nctri->channel_id);
		NAND_PIORelease(nctri->channel_id);
		if (nctri->dma_type == DMA_MODE_GENERAL_DMA) {
			NAND_ReleaseDMA(nctri->channel_id);
		}
		nctri = nctri->next;
	}

	NAND_ReleaseVoltage();

	g_nsi			 = NULL;
	g_nssi			 = NULL;
	g_nctri			 = NULL;
	g_nand_storage_info      = NULL;
	nand_physic_special_init = NULL;
	nand_physic_special_exit = NULL;
	nand_physic_temp1	= 0;
	nand_physic_temp2	= 0;
	nand_physic_temp3	= 0;
	nand_physic_temp4	= 0;
	nand_physic_temp5	= 0;
	nand_physic_temp6	= 0;
	nand_physic_temp7	= 0;
	nand_physic_temp8	= 0;

	nand_permanent_data.magic_data	= MAGIC_DATA_FOR_PERMANENT_DATA;
	nand_permanent_data.support_two_plane = 0;
	nand_permanent_data.support_vertical_interleave = 0;
	nand_permanent_data.support_dual_channel	= 0;

	nand_exit_temp_buf(&ntf);

	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int init_parameter(void)
{
	int i;
	struct _nand_controller_info *nctri;

	g_nsi		    = NULL;
	g_nssi		    = NULL;
	g_nctri		    = NULL;
	g_nand_storage_info = NULL;

	//g_nsi = (struct _nand_storage_info *)MALLOC(sizeof(struct _nand_storage_info));
	g_nsi = &g_nsi_data;
	if (g_nsi == NULL) {
		PHY_ERR("init_parameter, no memory for g_nsi\n");
		return NAND_OP_FALSE;
	}
	MEMSET(g_nsi, 0, sizeof(struct _nand_storage_info));

	//g_nssi = (struct _nand_super_storage_info *)MALLOC(sizeof(struct _nand_super_storage_info));
	g_nssi = &g_nssi_data;
	if (g_nssi == NULL) {
		PHY_ERR("init_parameter, no memory for g_nssi\n");
		return NAND_OP_FALSE;
	}
	MEMSET(g_nssi, 0, sizeof(struct _nand_super_storage_info));

	for (i = 0; i < MAX_CHANNEL; i++) {
		//nctri = (struct _nand_controller_info *)MALLOC(sizeof(struct _nand_controller_info));
		nctri = &g_nctri_data[i];
		if (nctri == NULL) {
			PHY_ERR("init_parameter, no memory for g_nctri\n");
			return NAND_OP_FALSE;
		}
		MEMSET(nctri, 0, sizeof(struct _nand_controller_info));

		add_to_nctri(nctri);

		if (init_nctri(nctri)) {
			PHY_ERR("nand_physic_init, init nctri error\n");
			return NAND_OP_FALSE;
		}
	}

	//g_nand_storage_info = (struct __NandStorageInfo_t *)MALLOC(sizeof(struct __NandStorageInfo_t));
	g_nand_storage_info = &g_nand_storage_info_data;
	if (g_nand_storage_info == NULL) {
		PHY_ERR("init_parameter, no memory for g_nand_storage_info\n");
		return NAND_OP_FALSE;
	}

	function_read_page_end = m0_read_page_end_not_retry;

	nand_init_temp_buf(&ntf);

	return NAND_OP_TRUE;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_init_temp_buf(struct _nand_temp_buf *nand_temp_buf)
{
	int i;

	MEMSET(nand_temp_buf, 0, sizeof(struct _nand_temp_buf));

	for (i = 0; i < NUM_16K_BUF; i++) {
		nand_temp_buf->nand_temp_buf16k[i] = (u8 *)MALLOC(16384);
		if (nand_temp_buf->nand_temp_buf16k[i] == NULL) {
			PHY_ERR("no memory for nand_init_temp_buf\n");
			return NAND_OP_FALSE;
		}
	}

	for (i = 0; i < NUM_32K_BUF; i++) {
		nand_temp_buf->nand_temp_buf32k[i] = (u8 *)MALLOC(32768);
		if (nand_temp_buf->nand_temp_buf32k[i] == NULL) {
			PHY_ERR("no memory for nand_init_temp_buf\n");
			return NAND_OP_FALSE;
		}
	}

	for (i = 0; i < NUM_64K_BUF; i++) {
		nand_temp_buf->nand_temp_buf64k[i] = (u8 *)MALLOC(65536);
		if (nand_temp_buf->nand_temp_buf64k[i] == NULL) {
			PHY_ERR("no memory for nand_init_temp_buf\n");
			return NAND_OP_FALSE;
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
int nand_exit_temp_buf(struct _nand_temp_buf *nand_temp_buf)
{
	int i;

	for (i = 0; i < NUM_16K_BUF; i++) {
		FREE(nand_temp_buf->nand_temp_buf16k[i], 16384);
	}

	for (i = 0; i < NUM_32K_BUF; i++) {
		FREE(nand_temp_buf->nand_temp_buf32k[i], 32768);
	}

	for (i = 0; i < NUM_64K_BUF; i++) {
		FREE(nand_temp_buf->nand_temp_buf64k[i], 65536);
	}
	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :the return pointer cannot be modified! must call by pair!
 *****************************************************************************/
u8 *nand_get_temp_buf(unsigned int size)
{
	unsigned int i;

	if (size <= 16384) {
		for (i = 0; i < NUM_16K_BUF; i++) {
			if (ntf.used_16k[i] == 0) {
				ntf.used_16k[i] = 1;
				return ntf.nand_temp_buf16k[i];
			}
		}
	}

	if (size <= 32768) {
		for (i = 0; i < NUM_32K_BUF; i++) {
			if (ntf.used_32k[i] == 0) {
				ntf.used_32k[i] = 1;
				return ntf.nand_temp_buf32k[i];
			}
		}
	}

	if (size <= 65536) {
		for (i = 0; i < NUM_64K_BUF; i++) {
			if (ntf.used_64k[i] == 0) {
				ntf.used_64k[i] = 1;
				return ntf.nand_temp_buf64k[i];
			}
		}
	}

	for (i = 0; i < NUM_NEW_BUF; i++) {
		if (ntf.used_new[i] == 0) {
			PHY_ERR("get memory :%d. \n", size);
			ntf.used_new[i]     = 1;
			ntf.nand_new_buf[i] = (u8 *)MALLOC(size);
			if (ntf.nand_new_buf[i] == NULL) {
				PHY_ERR("malloc fail\n");
				ntf.used_new[i] = 0;
				return NULL;
			}
			return ntf.nand_new_buf[i];
		}
	}

	PHY_ERR("get memory fail %d. \n", size);
	//while(1);
	return NULL;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_free_temp_buf(unsigned char *buf, unsigned int size)
{
	int i;

	for (i = 0; i < NUM_16K_BUF; i++) {
		if (ntf.nand_temp_buf16k[i] == buf) {
			ntf.used_16k[i] = 0;
			return 0;
		}
	}

	for (i = 0; i < NUM_32K_BUF; i++) {
		if (ntf.nand_temp_buf32k[i] == buf) {
			ntf.used_32k[i] = 0;
			return 0;
		}
	}

	for (i = 0; i < NUM_64K_BUF; i++) {
		if (ntf.nand_temp_buf64k[i] == buf) {
			ntf.used_64k[i] = 0;
			return 0;
		}
	}

	for (i = 0; i < NUM_NEW_BUF; i++) {
		if (ntf.nand_new_buf[i] == buf) {
			ntf.used_new[i] = 0;
			FREE(ntf.nand_new_buf[i], size);
			return 0;
		}
	}

	PHY_ERR("free memory fail %d. \n", size);
	//while(1);
	return -1;
}
