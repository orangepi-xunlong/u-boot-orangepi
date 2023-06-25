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
#define _NSCF_C_

#include "../nand_physic_inc.h"

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_reset_super_chip(struct _nand_super_chip_info *nsci,
		unsigned int super_chip_no)
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
int nand_read_super_chip_status(struct _nand_super_chip_info *nsci,
		unsigned int super_chip_no)
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
int is_super_chip_rb_ready(struct _nand_super_chip_info *nsci,
		unsigned int super_chip_no)
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
int nand_wait_all_rb_ready(void)
{
	int ret				    = 0;
	struct _nand_controller_info *nctri = g_nctri;

	while (nctri != NULL) {
		ret |= ndfc_wait_all_rb_ready(nctri);
		nctri = nctri->next;
	}
	return ret;
}
