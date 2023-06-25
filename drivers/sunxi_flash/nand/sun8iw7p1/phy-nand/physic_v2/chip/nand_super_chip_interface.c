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
#define _NSCI_C_

#include "../nand_physic_inc.h"

int PHY_VirtualPageRead(unsigned int nDieNum, unsigned int nBlkNum,
		unsigned int nPage, uint64 SectBitmap, void *pBuf,
		void *pSpare)
{
	return nand_physic_read_super_page(nDieNum, nBlkNum, nPage, SectBitmap,
			pBuf, pSpare);
}
int PHY_VirtualPageWrite(unsigned int nDieNum, unsigned int nBlkNum,
		unsigned int nPage, uint64 SectBitmap, void *pBuf,
		void *pSpare)
{
	return nand_physic_write_super_page(nDieNum, nBlkNum, nPage, SectBitmap,
			pBuf, pSpare);
}
int PHY_VirtualBlockErase(unsigned int nDieNum, unsigned int nBlkNum)
{
	return nand_physic_erase_super_block(nDieNum, nBlkNum);
}
int PHY_VirtualBadBlockCheck(unsigned int nDieNum, unsigned int nBlkNum)
{
	return nand_physic_super_bad_block_check(nDieNum, nBlkNum);
}
int PHY_VirtualBadBlockMark(unsigned int nDieNum, unsigned int nBlkNum)
{
	return nand_physic_super_bad_block_mark(nDieNum, nBlkNum);
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_physic_erase_super_block(unsigned int chip, unsigned int block)
{
	int ret;
	struct _nand_super_chip_info *nsci;
	struct _nand_physic_op_par npo;

	npo.chip	= chip;
	npo.block       = block;
	npo.page	= 0;
	npo.sect_bitmap = 0;
	npo.mdata       = NULL;
	npo.sdata       = NULL;
	npo.slen	= 0;
	nsci		= nsci_get_from_nssi(g_nssi, chip);
	ret		= nsci->nand_physic_erase_super_block(&npo);

	return ret;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_physic_read_super_page(unsigned int chip, unsigned int block,
		unsigned int page, unsigned int bitmap,
		unsigned char *mbuf, unsigned char *sbuf)
{
	int ret;
	struct _nand_super_chip_info *nsci;
	struct _nand_physic_op_par npo;

	nsci = nsci_get_from_nssi(g_nssi, chip);

	npo.chip	= chip;
	npo.block       = block;
	npo.page	= page;
	npo.sect_bitmap = bitmap;
	npo.mdata       = mbuf;
	npo.sdata       = sbuf;
	npo.slen	= nsci->spare_bytes;
	ret		= nsci->nand_physic_read_super_page(&npo);

	return ret;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_physic_write_super_page(unsigned int chip, unsigned int block,
		unsigned int page, unsigned int bitmap,
		unsigned char *mbuf, unsigned char *sbuf)
{
	int ret;
	struct _nand_super_chip_info *nsci;
	struct _nand_physic_op_par npo;

	nsci = nsci_get_from_nssi(g_nssi, chip);

	npo.chip	= chip;
	npo.block       = block;
	npo.page	= page;
	npo.sect_bitmap = bitmap;
	npo.mdata       = mbuf;
	npo.sdata       = sbuf;
	npo.slen	= nsci->spare_bytes;
	ret		= nsci->nand_physic_write_super_page(&npo);

	return ret;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_physic_super_bad_block_check(unsigned int chip, unsigned int block)
{
	int ret;
	struct _nand_super_chip_info *nsci;
	struct _nand_physic_op_par npo;

	nsci = nsci_get_from_nssi(g_nssi, chip);

	npo.chip	= chip;
	npo.block       = block;
	npo.page	= 0;
	npo.sect_bitmap = 0;
	npo.mdata       = NULL;
	npo.sdata       = NULL;
	npo.slen	= 0;
	ret		= nsci->nand_physic_super_bad_block_check(&npo);

	return ret;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int nand_physic_super_bad_block_mark(unsigned int chip, unsigned int block)
{
	int ret;
	struct _nand_super_chip_info *nsci;
	struct _nand_physic_op_par npo;

	nsci = nsci_get_from_nssi(g_nssi, chip);

	npo.chip	= chip;
	npo.block       = block;
	npo.page	= 0;
	npo.sect_bitmap = 0;
	npo.mdata       = NULL;
	npo.sdata       = NULL;
	npo.slen	= 0;
	ret		= nsci->nand_physic_super_bad_block_mark(&npo);

	return ret;
}
