/* SPDX-License-Identifier: GPL-2.0 */
/*****************************************************************************/
#define _SECURE_STORAGE_C_
/*****************************************************************************/
#include <linux/string.h>
#include "nand_secure_storage.h"
#include "nand_boot.h"
#include "nand_physic_interface.h"
#include "rawnand/rawnand_cfg.h"
#include "rawnand/rawnand_debug.h"
#include "../nand_osal_uboot.h"
#include <asm/byteorder.h>
#include <sunxi_nand_boot.h>

/*#define CHECK_BACKFILL*/


extern int get_default_uboot_block(unsigned int *start_block, unsigned int *next_block);
#define ITEM_LEN 4096

#pragma pack(1)
struct nand_secure_storage_oob {
	unsigned char badflag;
	unsigned short magic; /*big end cpu_to_be16(0xaa5c)*/
	unsigned int check_sum; /* big end, init val cpu_to_be32(0x1234)*/
	/* blockA_gap = nand_secure_storage_block - uboot_next_block
	 * blockB_gap = nand_secure_storage_block_bak - blockA_gap
	 * if gap is big than 15 ,may be this flash is too bad, can't be used*/
	unsigned int blockA_gap:4;
	unsigned int blockB_gap:4;
};
#pragma pack()
struct nand_secure_storage {
	bool nphy_init;
	unsigned int blockA;
	bool blockA_init;
	unsigned int blockB;
	bool blockB_init;
	unsigned int blockA_gap;
	unsigned int blockB_gap;
	unsigned int need_backfill;
	/*if pagesize is 2k, should two pages store one item*/
	unsigned char *pages[MAX_PAGE_CNT_PER_ITEM * MAX_SECURE_STORAGE_ITEM];
	unsigned char oobs[MAX_PAGE_CNT_PER_ITEM * MAX_SECURE_STORAGE_ITEM][64];
	/*0: valid item 1: empty item 2: error item*/
	uint8_t  pages_right[MAX_PAGE_CNT_PER_ITEM * MAX_SECURE_STORAGE_ITEM];
	/*begin to write flag*/
	bool begin;
	int writen_pages;
	bool writing;
};

static struct nand_secure_storage gs;

int nand_secure_storage_block;
int nand_secure_storage_block_bak;

int nand_secure_storage_backfill(void);

unsigned int nand_get_secure_block_start(void)
{
	unsigned int uboot_start_block = 0;
	unsigned int uboot_next_block = 0;

	get_default_uboot_block(&uboot_start_block, &uboot_next_block);

	return uboot_next_block;
}

/*****************************************************************************
*Name         :nand_is_support_secure_storage
*Description  : judge if the flash support secure storage
*Parameter    : null
*Return       : 0:support  -1:don't support
*Note         :
*****************************************************************************/
#if 0
int nand_is_support_secure_storage(void)
{
	__u32 block, page_size;
	__u8 spare[32];
	__u8 *mbuf;

	page_size = nand_get_pae_size();
	mbuf = (__u8 *)NAND_Malloc(page_size);

	block = 7;
	while (block < 50) {
		nand_physic_read_page(0, block, 0, mbuf, spare);
		if ((spare[0] == 0xff) && (spare[1] == 0xaa) && (spare[2] == 0xbb)) {
			if (spare[3] == 0x01) {
				PRINT_DBG("nand:found factory_bad_block(new version) table in block:%d!\n", block);
				PRINT_DBG("nand:support secure storage\n");
				NAND_Free((void *)mbuf, page_size);
				return 0;
			} else if (spare[3] == 0xff) {
				PRINT_DBG("found factory_bad_block(old version) table in block:%d!\n", block);
				PRINT_DBG("nand:don't support secure storage\n");
				NAND_Free((void *)mbuf, page_size);
				return -1;
			}
		}
		block++;
	}
	PRINT_DBG("new nand flash\n");
	PRINT_DBG("nand:support secure storage\n");
	NAND_Free((void *)mbuf, page_size);
	return 0;
}
#else
int nand_is_support_secure_storage(void)
{
	return 0;
}

#endif

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         :
*****************************************************************************/
int nand_secure_storage_write_init(unsigned int block)
{
	int ret = -1;
	unsigned char *mbuf;
	unsigned int page_size, i, page_cnt_per_blk;
	unsigned char spare[64];
	struct nand_secure_storage_oob *nsoob =
		(struct nand_secure_storage_oob *)spare;

	page_size = nand_get_pae_size();
	page_cnt_per_blk = nand_get_page_cnt_per_block();
	mbuf = NAND_Malloc(page_size);
	if (mbuf == NULL) {
		pr_err("malloc main buffer fail in nand secure storage write init\n");
	}

	memset(mbuf, 0, page_size);
	memset(spare, 0xff, 64);
	/*
	 *spare[1] = 0xaa;
	 *spare[2] = 0x5c;
	 *spare[3] = 0x00;
	 *spare[4] = 0x00;
	 *spare[5] = 0x12;
	 *spare[6] = 0x34;
	 */
	nsoob->badflag = 0xff,
	nsoob->magic = cpu_to_be16(0xaa5c);
	nsoob->check_sum = cpu_to_be32(0x1234);
	nsoob->blockA_gap = gs.blockA - nand_get_secure_block_start();
	nsoob->blockB_gap = gs.blockB - gs.blockA;


	if (nand_physic_erase_block(0, block) == 0) {
		for (i = 0; i < page_cnt_per_blk; i++) {
			nand_physic_write_page(0, block, i, page_size / 512,
					       mbuf, spare);
		}
		mbuf[0] = 0X11;
		ret = nand_secure_storage_read_one(block, 0, mbuf, 2048);
	}

	NAND_Free(mbuf, page_size);
	return ret;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         :
*****************************************************************************/
int nand_secure_storage_clean_data(unsigned int num)
{
	if (num == 0)
		nand_physic_erase_block(0, nand_secure_storage_block);
	if (num == 1)
		nand_physic_erase_block(0, nand_secure_storage_block_bak);
	return 0;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         :
*****************************************************************************/
int nand_secure_storage_clean(void)
{
	nand_secure_storage_clean_data(0);
	nand_secure_storage_clean_data(1);
	return 0;
}
/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         :
*****************************************************************************/
int is_nand_secure_storage_build(void)
{
	if ((nand_secure_storage_block_bak > MIN_SECURE_STORAGE_BLOCK_NUM) &&
	    (nand_secure_storage_block_bak < MAX_SECURE_STORAGE_BLOCK_NUM)) {
		return 1;
	} else {
		return 0;
	}
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         :
*****************************************************************************/
int is_nand_secure_storage_block(unsigned int block)
{
	/*unsigned char *mbuf;*/
	int ret = 0;
	unsigned char spare[64];
	struct nand_secure_storage_oob *nsoob =
		(struct nand_secure_storage_oob *)spare;

	/*unsigned int page_cnt_per_blk = nand_get_page_cnt_per_block();*/

	ret = nand_physic_read_page(0, block, 0, 0, NULL, spare);
	if (ret != 0)
		pr_err("read b@%d p@0 fail\n", block);


	if ((spare[0] == 0xff) && (spare[1] == 0xaa) && (spare[2] == 0x5c)) {

		gs.blockA_gap = nsoob->blockA_gap;
		gs.blockB_gap = nsoob->blockB_gap;

		ret = 1;
	}

	nsoob = NULL;
	return ret;
}

/*check wheter need to backfill. and check result save in gs.need_fillback*/
int nand_secure_check_is_need_backfill(void)
{
	unsigned int blockA = gs.blockA;
	unsigned int blockB = gs.blockB;
	int ret = 0;
	unsigned char spare[64];
	unsigned int page_cnt_per_blk = nand_get_page_cnt_per_block();
	unsigned int read_sum = 0;
	int blockAflag = 0;


	if (blockB <= blockA) {
		pr_err("nand secure block err blockA:%u blockB:%u\n", gs.blockA, gs.blockB);
		return 0;
	}

	/*check blockA*/
	ret = nand_physic_read_page(0, blockA, 0, 0, NULL, spare);
	if (ret != 0)
		pr_err("read b@%d p@%d fail\n", blockA, 0);

	read_sum = ((spare[3] << 24) | (spare[4] << 16) | (spare[5] << 8) | (spare[6] << 0));

	/*first burn key, would write full blockA*/
	if (spare[0] == 0xff && read_sum != 0x1234) {
		blockAflag = 1;
	} else {
		blockAflag = 0;
		gs.need_backfill = 0;
	}

	/*if blockA is full and not need fillback, then check blockB*/
	if (blockAflag) {
		ret = nand_physic_read_page(0, blockB, page_cnt_per_blk - 1, 0, NULL, spare);
		if (ret != 0)
			pr_err("read b@%d p@%d fail\n", blockB, page_cnt_per_blk - 1);

		read_sum = ((spare[3] << 24) | (spare[4] << 16) | (spare[5] << 8) | (spare[6] << 0));

		if (spare[0] == 0xff && spare[1] == 0xff && spare[2] == 0xff && spare[3] == 0xff) {
			gs.need_backfill = 1;
		} else
			gs.need_backfill = 0;
	}

	return gs.need_backfill;
}
/**
 * blockA: nand_secure_storage_block
 * blockB: nand_secure_storage_block_bak
 **/
int nand_secure_check_and_correct_blockAB_number(unsigned int blockA, unsigned int blockB)
{
	/*powerfail while erase nand_secure_storage_block or
	 * erase nand_secure_storage_block_bak in fillback process
	 * maybe would cause this scene*/
	if ((blockA >= MIN_SECURE_STORAGE_BLOCK_NUM) && (blockA >= blockB)) {

		gs.blockA = gs.blockA_gap + nand_get_secure_block_start();
		gs.blockB = gs.blockB_gap + gs.blockA;

		nand_secure_storage_block = gs.blockA;
		nand_secure_storage_block_bak = gs.blockB;
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
int nand_secure_storage_init(int flag)
{
	unsigned int nBlkNum;
	int ret = 0;

	nand_secure_storage_block = 0;
	nand_secure_storage_block_bak = 0;

	if (nand_is_support_secure_storage() != 0)
		return 0;


	nBlkNum = nand_get_secure_block_start();


	pr_debug("%s %d nBlkNum:%d\n", __func__, __LINE__, nBlkNum);

	for (; nBlkNum < MAX_SECURE_STORAGE_BLOCK_NUM; nBlkNum++) {
		ret = is_nand_secure_storage_block(nBlkNum);
		if (ret == 1) {
			nand_secure_storage_block = nBlkNum;
			break;
		}
	}

	for (nBlkNum += 1; nBlkNum < MAX_SECURE_STORAGE_BLOCK_NUM; nBlkNum++) {
		ret = is_nand_secure_storage_block(nBlkNum);
		if (ret == 1) {
			nand_secure_storage_block_bak = nBlkNum;
			break;
		}
	}

	gs.blockA = nand_secure_storage_block;
	gs.blockB = nand_secure_storage_block_bak;


	pr_info("%s %d NSSB:%d\n", __func__, __LINE__,
			nand_secure_storage_block);
	pr_info("%s %d NSSBBbak:%d\n", __func__, __LINE__,
			nand_secure_storage_block_bak);
	pr_debug("%s %d MIN_SECURE_STORAGE_BLOCK_NUM:%d\n", __func__, __LINE__,
			MIN_SECURE_STORAGE_BLOCK_NUM);


	/*powerfail while erase nand_secure_storage_block or
	 * erase nand_secure_storage_block_bak in fillback process
	 * maybe need to check and correct them*/
	nand_secure_check_and_correct_blockAB_number(nand_secure_storage_block,
			nand_secure_storage_block_bak);

	if (nand_secure_check_is_need_backfill()) {
		printf("%s need to backfill\n", __func__);
		ret = nand_secure_storage_backfill();
	}

	if ((nand_secure_storage_block < MIN_SECURE_STORAGE_BLOCK_NUM) ||
	    (nand_secure_storage_block >= nand_secure_storage_block_bak)) {
		nand_secure_storage_block = 0;
		nand_secure_storage_block_bak = 0;
		pr_err("nand secure storage fail: %d,%d\n",
			    nand_secure_storage_block,
			    nand_secure_storage_block_bak);
		ret = -1;
	} else {
		if (flag != 0) {
			nand_secure_storage_update();
		}
		ret = 0;
		pr_debug("nand secure storage ok: %d,%d\n",
			    nand_secure_storage_block,
			    nand_secure_storage_block_bak);
	}

	return ret;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         : only called when first build
*****************************************************************************/
int nand_secure_storage_first_build(unsigned int start_block)
{
	int block = -1;
	unsigned int nBlkNum;
	int ret;

	if (nand_is_support_secure_storage() != 0)
		return start_block;

	if ((nand_secure_storage_block_bak > MIN_SECURE_STORAGE_BLOCK_NUM) &&
	    (nand_secure_storage_block_bak < MAX_SECURE_STORAGE_BLOCK_NUM)) {
		pr_debug("start block:%d\n",
			    nand_secure_storage_block_bak + 1);
		return nand_secure_storage_block_bak + 1;
	}

	nBlkNum = start_block;

	for (; nBlkNum < MAX_SECURE_STORAGE_BLOCK_NUM; nBlkNum++) {
		if (nand_physic_bad_block_check(0, nBlkNum) == 0) {
			ret = is_nand_secure_storage_block(nBlkNum);
			if (ret != 1) {
				/*nand_secure_storage_write_init(nBlkNum);*/
				gs.blockA_init = false;
			}
			nand_secure_storage_block = nBlkNum;
			gs.blockA = nand_secure_storage_block;
			break;
		}
	}

	for (nBlkNum += 1; nBlkNum < MAX_SECURE_STORAGE_BLOCK_NUM; nBlkNum++) {
		if (nand_physic_bad_block_check(0, nBlkNum) == 0) {
			ret = is_nand_secure_storage_block(nBlkNum);
			if (ret != 1) {
				/*nand_secure_storage_write_init(nBlkNum);*/
				gs.blockB_init = false;
			}
			nand_secure_storage_block_bak = nBlkNum;
			gs.blockB = nand_secure_storage_block_bak;
			break;
		}
	}
	pr_info("%s gs.blockA:%u-%d\n", __func__, gs.blockA, nand_secure_storage_block);
	pr_info("%s gs.blockB:%u-%d\n", __func__, gs.blockB, nand_secure_storage_block_bak);

	if (!gs.blockA_init)
		nand_secure_storage_write_init(gs.blockA);

	if (!gs.blockB_init)
		nand_secure_storage_write_init(gs.blockB);

	if ((nand_secure_storage_block < MIN_SECURE_STORAGE_BLOCK_NUM) ||
	    (nand_secure_storage_block >= nand_secure_storage_block_bak)) {
		pr_debug("nand secure storage firsr build  fail: %d,%d\n",
			    nand_secure_storage_block,
			    nand_secure_storage_block_bak);
		nand_secure_storage_block = 0;
		nand_secure_storage_block_bak = 0;
		block = start_block + 2;
	} else {
		block = nand_secure_storage_block_bak + 1;
		pr_debug("nand secure storage firsr build  ok: %d,%d\n",
			    nand_secure_storage_block,
			    nand_secure_storage_block_bak);
		ubootsta.nand_secstate.state = SECURE_STORAGE_FIRST_BUILD;
	}
	return block;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         : api
*****************************************************************************/
int get_nand_secure_storage_max_item(void)
{
	unsigned int page_cnt_per_blk = nand_get_page_cnt_per_block();

	if (MAX_SECURE_STORAGE_ITEM < page_cnt_per_blk) {
		return MAX_SECURE_STORAGE_ITEM;
	}
	return page_cnt_per_blk;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         :
*****************************************************************************/
unsigned int nand_secure_check_sum(unsigned char *mbuf, unsigned int len)
{
	unsigned int check_sum, i;
	unsigned int *p;

	p = (unsigned int *)mbuf;
	check_sum = 0x1234;
	len >>= 2;

	for (i = 0; i < len; i++) {
		check_sum += p[i];
	}
	return check_sum;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         :
*****************************************************************************/
int nand_secure_storage_read_one(unsigned int block, int item,
				 unsigned char *mbuf, unsigned int len)
{
	unsigned char spare[64];
	unsigned int check_sum, read_sum, page_cnt_per_blk;
	int page_size;

	page_size = nand_get_pae_size();

	page_cnt_per_blk = nand_get_page_cnt_per_block();

	for (; item < page_cnt_per_blk; item += MAX_SECURE_STORAGE_ITEM) {
		memset(spare, 0, 64);
		nand_physic_read_page(0, block, item, page_size / 512, mbuf,
				      spare);

		if ((spare[1] != 0xaa) || (spare[2] != 0x5c)) {
			continue;
		}

		check_sum = nand_secure_check_sum(mbuf, len);

		read_sum = spare[3];
		read_sum <<= 8;
		read_sum |= spare[4];
		read_sum <<= 8;
		read_sum |= spare[5];
		read_sum <<= 8;
		read_sum |= spare[6];

		if (read_sum == check_sum) {
			if (read_sum == 0x1234) {
				return 1;
			}
			return 0;
		} else {
			pr_err("spare_sum:0x%x,check_sum:0x%x\n", read_sum,
				    check_sum);
		}
	}
	return -1;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         :api
*****************************************************************************/
int nand_secure_storage_read(int item, unsigned char *buf, unsigned int len)
{
	unsigned char *mbuf = NULL;
	int page_size = 0, ret = 0, i = 0;
	int pages_cnt_per_item = 0;
	unsigned int item_tmp = item;

	if (nand_is_support_secure_storage())
		return 0;

	NAND_PhysicLock();

	if (!gs.nphy_init) {
		if (!NandHwInit()) {
			pr_err("%s nand hw init fail\n", __func__);
			ret = -1;
			goto out;
		}
		gs.nphy_init = true;
	}
	if (len % 1024) {
		pr_err("error! len = %d, agali 1024Bytes\n", len);
		ret = -1;
		goto out;
	}

	page_size = nand_get_pae_size();
	if (len <= page_size)
		pages_cnt_per_item = 1;
	else {
		pages_cnt_per_item = len / page_size;
		len = page_size;
	}

	mbuf = NAND_Malloc(page_size);
	if (!mbuf) {
		pr_err("malloc error! %s[%d]\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto out;
	}

	for (i = 0; i < pages_cnt_per_item; i++) {
		item = item_tmp * pages_cnt_per_item + i;
		ret = nand_secure_storage_read_one(nand_secure_storage_block,
						   item, mbuf, len);
		if (ret == -1)
			ret = nand_secure_storage_read_one(
			    nand_secure_storage_block_bak, item, mbuf, len);

		if (ret == 0) {
			if (pages_cnt_per_item == 1)
				memcpy(buf, mbuf, len);
			else
				memcpy(buf + i * len, mbuf, len);
		} else {
			if (pages_cnt_per_item == 1)
				memset(buf, 0, len);
			else
				memset(buf + i * len, 0, len);
		}
	}

	NAND_Free(mbuf, page_size);
out:
	NAND_PhysicUnLock();

	return ret;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         :api
*****************************************************************************/
int nand_secure_storage_write(int item, unsigned char *buf, unsigned int len)
{
	int ret = -1; //, retry = 0;
	unsigned char *mbuf = NULL;
	unsigned int page_size, i, check_sum, page, page_cnt_per_blk = 0;
	unsigned char spare[64];
	int pages_cnt_per_item = 0;

	if (nand_is_support_secure_storage())
		return 0;

	NAND_PhysicLock();

	if (!gs.nphy_init) {
		if (!NandHwInit()) {
			pr_err("%s nand hw init fail\n", __func__);
			ret = -1;
			goto out;
		}
		gs.nphy_init = true;
	}

	nand_secure_storage_update();

	page_size = nand_get_pae_size();
	page_cnt_per_blk = nand_get_page_cnt_per_block();

	if (len % 1024) {
		pr_err("error! len = %d, agali 1024Bytes\n", len);
		ret = -1;
		goto out;
	}

	if (len <= page_size)
		pages_cnt_per_item = 1;
	else {
		pages_cnt_per_item = len / page_size;
		len = page_size;
	}

	mbuf = NAND_Malloc(page_size);
	if (!mbuf) {
		pr_err("malloc error! %s[%d]\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto out;
	}

	nand_physic_erase_block(0, nand_secure_storage_block_bak);
	for (i = 0; i < MAX_SECURE_STORAGE_ITEM * pages_cnt_per_item; i++) {
		if ((i / pages_cnt_per_item) != item) {
			nand_physic_read_page(0, nand_secure_storage_block, i,
					      page_size / 512, mbuf, spare);
		} else {
			memset(mbuf, 0, page_size);
			if (pages_cnt_per_item == 1)
				memcpy(mbuf, buf, len);
			else
				memcpy(mbuf,
				       buf +
					   page_size *
					       (i % pages_cnt_per_item),
				       page_size);
			check_sum = nand_secure_check_sum(mbuf, len);

			memset(spare, 0xff, 64);
			spare[1] = 0xaa;
			spare[2] = 0x5c;
			spare[3] = (unsigned char)(check_sum >> 24);
			spare[4] = (unsigned char)(check_sum >> 16);
			spare[5] = (unsigned char)(check_sum >> 8);
			spare[6] = (unsigned char)(check_sum);
		}
		if (nand_physic_write_page(0, nand_secure_storage_block_bak, i,
					   page_size / 512, mbuf, spare))
			pr_err("nand_secure_storage_write fail\n");
	}

	for (i = MAX_SECURE_STORAGE_ITEM * pages_cnt_per_item;
	     i < page_cnt_per_blk; i++) {
		page = i % (MAX_SECURE_STORAGE_ITEM * pages_cnt_per_item);
		nand_physic_read_page(0, nand_secure_storage_block_bak, page,
				      page_size / 512, mbuf, spare);
		nand_physic_write_page(0, nand_secure_storage_block_bak, i,
				       page_size / 512, mbuf, spare);
	}

	nand_secure_storage_repair(2, page_cnt_per_blk);

	NAND_Free(mbuf, page_size);
	ret = 0;
out:
	NAND_PhysicUnLock();
	return ret;
}

int nand_secure_storage_read_one_pre(unsigned int block, int p,
				 unsigned char *mbuf, unsigned int len, unsigned char *spare,
				 unsigned int page_cnt_one_item)
{
	unsigned int check_sum, read_sum, page_cnt_per_blk;
	int page_size;

	page_size = nand_get_pae_size();

	page_cnt_per_blk = nand_get_page_cnt_per_block();

	for (; p < page_cnt_per_blk; p += MAX_SECURE_STORAGE_ITEM) {
		memset(spare, 0, 64);
		nand_physic_read_page(0, block, p, page_size / 512, mbuf,
				      spare);
		if ((spare[1] != 0xaa) || (spare[2] != 0x5c)) {
			continue;
		}

		check_sum = nand_secure_check_sum(mbuf, len);

		read_sum = spare[3];
		read_sum <<= 8;
		read_sum |= spare[4];
		read_sum <<= 8;
		read_sum |= spare[5];
		read_sum <<= 8;
		read_sum |= spare[6];

		if (read_sum == check_sum) {
			if (read_sum == 0x1234) {
				gs.pages_right[p%(page_cnt_one_item*MAX_SECURE_STORAGE_ITEM)] = 1;
				return 1;
			}
			gs.pages_right[p%(page_cnt_one_item*MAX_SECURE_STORAGE_ITEM)] = 0;
			return 0;
		} else {
			pr_err("spare_sum:0x%x,check_sum:0x%x\n", read_sum,
				    check_sum);
		}
	}

	gs.pages_right[p%(page_cnt_one_item*MAX_SECURE_STORAGE_ITEM)] = 2;
	return -1;
}

int nand_secure_storage_fast_write(int item, unsigned char *buf,
		unsigned int len)
{

	int ret = -1;
	unsigned int page_size, i, check_sum;
	/*unsigned char spare[64];*/
	int pages_cnt_per_item = 0;

	if (nand_is_support_secure_storage())
		return 0;

	NAND_PhysicLock();

	if (!gs.nphy_init) {
		if (!NandHwInit()) {
			pr_err("%s nand hw init fail\n", __func__);
			ret = -1;
			goto out;
		}
		gs.nphy_init = true;
	}
	gs.writing = true;


	page_size = nand_get_pae_size();

	/*page_cnt_per_blk = nand_get_page_cnt_per_block();*/

	if (len % 1024) {
		pr_err("error! len = %d, agali 1024Bytes\n", len);
		ret = -1;
		goto out;
	}

	if (len <= page_size)
		pages_cnt_per_item = 1;
	else {
		pages_cnt_per_item = len / page_size;
		len = page_size;
	}

	if (pages_cnt_per_item > MAX_PAGE_CNT_PER_ITEM) {
		pr_err("error! page count per item too big %d\n", pages_cnt_per_item);
		ret = -1;
		goto out;
	}


	if (!gs.begin) {
		gs.begin = true;
		for (i = 0; i < pages_cnt_per_item * MAX_SECURE_STORAGE_ITEM; i++) {
			gs.pages[i] = nand_malloc(page_size);
			if (gs.pages[i] == NULL) {
				pr_err("nand malloc fail in fl\n");
				ret = -1;
				goto out2;
			}
			memset(gs.pages[i], 0, page_size);
		}

		pr_info("%s-%d A:%d B:%d\n", __func__, __LINE__,
				nand_secure_storage_block, nand_secure_storage_block_bak);
		for (i = 0; i < MAX_SECURE_STORAGE_ITEM * pages_cnt_per_item; i++) {
			nand_secure_storage_read_one_pre(nand_secure_storage_block,
					i, gs.pages[i], len, gs.oobs[i], pages_cnt_per_item);
			/*not valid item*/
			if (gs.pages_right[i] != 0 && gs.pages_right[i] != 1) {
				nand_secure_storage_read_one_pre(
						nand_secure_storage_block_bak, i, gs.pages[i], len, gs.oobs[i], pages_cnt_per_item);
			}
			if (gs.pages_right[i] == 2)
				pr_debug("%s pre read page @%d is err\n", __func__, i);
		}
	}

	for (i = 0; i < MAX_SECURE_STORAGE_ITEM * pages_cnt_per_item; i++) {
		if ((i / pages_cnt_per_item) == item) {
			memset(gs.pages[i], 0, page_size);
			if (pages_cnt_per_item == 1)
				memcpy(gs.pages[i], buf, len);
			else {
				memcpy(gs.pages[i], buf + page_size * (i % pages_cnt_per_item),
						page_size);
			}
			check_sum = nand_secure_check_sum(gs.pages[i], len);

			memset(gs.oobs[i], 0xff, 64);

			struct nand_secure_storage_oob *nsoob =
				(struct nand_secure_storage_oob *)gs.oobs[i];
			/*
			 *spare[1] = 0xaa;
			 *spare[2] = 0x5c;
			 *spare[3] = (unsigned char)(check_sum >> 24);
			 *spare[4] = (unsigned char)(check_sum >> 16);
			 *spare[5] = (unsigned char)(check_sum >> 8);
			 *spare[6] = (unsigned char)(check_sum);
			 */
			nsoob->badflag = 0xff;
			nsoob->magic = cpu_to_be16(0xaa5c);
			nsoob->check_sum = cpu_to_be32(check_sum);
			nsoob->blockA_gap = gs.blockA - nand_get_secure_block_start();
			nsoob->blockB_gap = gs.blockB - gs.blockA;
		}

	}

/*	printf("write item %d\n", item);*/
	ret = 0;

	return ret;
out2:

	for (i = 0; i < pages_cnt_per_item * MAX_SECURE_STORAGE_ITEM; i++) {
		if (gs.pages[i] != NULL) {
			NAND_Free(gs.pages[i], page_size);
		}
	}
out:
	NAND_PhysicUnLock();
	return ret;

}

int nand_secure_storage_flush(void)
{
	int ret = 0;
	int p = 0;
	int i = 0;
	int cnt = 0;
	int pages_cnt_per_item = 0;
	int page_cnt = nand_get_page_cnt_per_block();
	unsigned int page_size = nand_get_pae_size();

	/*page_cnt_per_blk = nand_get_page_cnt_per_block();*/

	if (ITEM_LEN <= page_size)
		pages_cnt_per_item = 1;
	else {
		pages_cnt_per_item = ITEM_LEN / page_size;
	}
/*
	for (i = 0; i < (MAX_SECURE_STORAGE_ITEM * pages_cnt_per_item); i++) {
		printf("i@%d: %02x %02x %02x %02x %02x %02x %02x %02x\n",
			   i, gs.oobs[i][0], gs.oobs[i][1],  gs.oobs[i][2],  gs.oobs[i][3],
			    gs.oobs[i][4],  gs.oobs[i][5],  gs.oobs[i][6],  gs.oobs[i][7]);
	}
*/

retry:
	ret = nand_physic_erase_block(0, nand_secure_storage_block);
	if (!ret) {
		for (p = 0; p < page_cnt; p++) {
			i = p % (MAX_SECURE_STORAGE_ITEM * pages_cnt_per_item);
			ret = nand_physic_write_page(0, nand_secure_storage_block, p,
					page_size / 512, gs.pages[i], gs.oobs[i]);
		}
	} else {
		cnt++;
		if (cnt < 3)
			goto retry;
		else
			pr_err("erase blockA@%d fail\n", nand_secure_storage_block);
	}
	/*erase blockB , guarantee backfill it when power on again, keep up to date*/
	ret = nand_physic_erase_block(0, nand_secure_storage_block_bak);

	if (ret)
		pr_err("%s flush storage fail\n", __func__);
	else
		pr_err("flush storage success\n");

	gs.begin = false;

	return ret;
}

#if 0

int nand_secure_storage_fast_write(int item, unsigned char *buf,
		unsigned int len)
{
	int ret = -1;
	unsigned char *mbuf = NULL;
	unsigned int page_size, i, check_sum;
	unsigned char spare[64];
	int pages_cnt_per_item = 0;
	struct nand_secure_storage_oob *nsoob =
		(struct nand_secure_storage_oob *)spare;

	if (nand_is_support_secure_storage())
		return 0;

	NAND_PhysicLock();

	if (!gs.nphy_init) {
		if (!NandHwInit()) {
			pr_err("%s nand hw init fail\n", __func__);
			ret = -1;
			goto out;
		}
		gs.nphy_init = true;
	}

	nand_secure_storage_update();

	page_size = nand_get_pae_size();
	/*page_cnt_per_blk = nand_get_page_cnt_per_block();*/

	if (len % 1024) {
		pr_err("error! len = %d, agali 1024Bytes\n", len);
		ret = -1;
		goto out;
	}

	if (len <= page_size)
		pages_cnt_per_item = 1;
	else {
		pages_cnt_per_item = len / page_size;
		len = page_size;
	}

	mbuf = NAND_Malloc(page_size);
	if (!mbuf) {
		pr_err("malloc error! %s[%d]\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto out;
	}

	nand_physic_erase_block(0, nand_secure_storage_block_bak);
	for (i = 0; i < MAX_SECURE_STORAGE_ITEM * pages_cnt_per_item; i++) {
		if ((i / pages_cnt_per_item) != item) {
			nand_physic_read_page(0, nand_secure_storage_block, i,
					page_size / 512, mbuf, spare);
		} else {
			memset(mbuf, 0, page_size);
			if (pages_cnt_per_item == 1)
				memcpy(mbuf, buf, len);
			else
				memcpy(mbuf,
						buf +
					   page_size *
					       (i % pages_cnt_per_item),
				       page_size);
			check_sum = nand_secure_check_sum(mbuf, len);

			memset(spare, 0xff, 64);
			/*
			 *spare[1] = 0xaa;
			 *spare[2] = 0x5c;
			 *spare[3] = (unsigned char)(check_sum >> 24);
			 *spare[4] = (unsigned char)(check_sum >> 16);
			 *spare[5] = (unsigned char)(check_sum >> 8);
			 *spare[6] = (unsigned char)(check_sum);
			 */
			nsoob->badflag = 0xff;
			nsoob->magic = cpu_to_be16(0xaa5c);
			nsoob->check_sum = cpu_to_be32(check_sum);
			nsoob->blockA_gap = gs.blockA - nand_get_secure_block_start();
			nsoob->blockB_gap = gs.blockB - gs.blockA;
		}

		if (nand_physic_write_page(0, nand_secure_storage_block_bak, i,
					page_size / 512, mbuf, spare))
			pr_err("nand_secure_storage_write fail\n");
	}

	nand_secure_storage_repair(2, MAX_SECURE_STORAGE_ITEM * pages_cnt_per_item);

	NAND_Free(mbuf, page_size);
	ret = 0;
out:
	NAND_PhysicUnLock();
	return ret;
}
#endif

#ifdef CHECK_BACKFILL
int nand_secure_storage_backfill_check(void)
{

	unsigned int page_cnt_per_blk = nand_get_page_cnt_per_block();
	unsigned int page_size = nand_get_pae_size();
	unsigned char *mbuf1 = nand_malloc(page_size);
	unsigned char *mbuf2 = nand_malloc(page_size);
	unsigned char oob1[8], oob2[8];
	unsigned int len = ITEM_LEN;
	int ret = 0;
	int p = 0;
	int item = 0;
	int copy = 0;
	int copys = 0;
	if (mbuf1 == NULL || mbuf2 == NULL) {
		pr_err("%s malloc mbuf1 fail\n", __func__);
		ret = -1;
		goto out;
	}

	/*check main and backup data area whether consistent*/
	for (p = 0; p < page_cnt_per_blk; p++) {

		memset(mbuf1, 0x00, page_size);
		memset(mbuf2, 0x00, page_size);
		memset(oob1, 0x00, 8);
		memset(oob2, 0x00, 8);

		ret = nand_physic_read_page(0, nand_secure_storage_block, p,
				page_size / 512, mbuf1, oob1);
		if (ret != 0)
			pr_err("read b@%d p@%d fail\n", nand_secure_storage_block, p);

		ret = nand_physic_read_page(0, nand_secure_storage_block_bak, p,
				page_size / 512, mbuf2, oob2);
		if (ret != 0)
			pr_err("read b@%d p@%d fail\n", nand_secure_storage_block_bak, p);

		if (memcmp(mbuf1, mbuf2, page_size) || memcmp(oob1, oob2, 8))
			pr_err("b@%d b@%d p@%d is different\n", nand_secure_storage_block,
					nand_secure_storage_block_bak, p);
	};

	copys = page_cnt_per_blk / MAX_SECURE_STORAGE_ITEM;

	/*check main data layout*/
	for (item = 0; item < MAX_SECURE_STORAGE_ITEM; item++) {
		memset(mbuf1, 0x00, page_size);
		memset(oob1, 0x00, 8);
		p = item;
		ret = nand_physic_read_page(0, nand_secure_storage_block, p,
				page_size / 512, mbuf1, oob1);
		if (ret != 0)
			pr_err("copy@0'item@%d is \033[0;36mbad\033[0m in b@%d\n", copy, item, nand_secure_storage_block);
		unsigned int check_sum = 0;
		struct nand_secure_storage_oob *nsoob = (struct nand_secure_storage_oob *)oob1;

		if (len < page_size)
			check_sum = nand_secure_check_sum(mbuf1, len);
		else
			check_sum = nand_secure_check_sum(mbuf1, page_size);

		if (nsoob->check_sum == cpu_to_be32(check_sum))
			printf("copy@0'item@%d is good in b@%d\n", item, nand_secure_storage_block);

		for (copy = 1; copy < copys; copy++) {
			memset(mbuf2, 0x00, page_size);
			memset(oob2, 0x00, 8);
			p = item + copy * MAX_SECURE_STORAGE_ITEM;
			ret = nand_physic_read_page(0, nand_secure_storage_block, p,
					page_size / 512, mbuf2, oob2);
			if (ret != 0)
				pr_err("read copy@%d'item@%d fail in b@%u\n",
						copy, item, nand_secure_storage_block);

			if (memcmp(mbuf1, mbuf2, page_size) || memcmp(oob1, oob2, 8))
				pr_err("copy@%d'item@%d is \033[0;36mbad\033[0m in b@%d\n",
						copy, item, nand_secure_storage_block);
			else
				printf("copy@%d'item@%d is good in b@%d\n",
						copy, item, nand_secure_storage_block);
		}
	}

	/*check backup data layout*/
	for (item = 0; item < MAX_SECURE_STORAGE_ITEM; item++) {
		memset(mbuf1, 0x00, page_size);
		memset(oob1, 0x00, 8);
		p = item;
		ret = nand_physic_read_page(0, nand_secure_storage_block_bak, p,
				page_size / 512, mbuf1, oob1);
		if (ret != 0)
			pr_err("read copy@0'item@%d is \033[0;36mbad\033[0m in b@%d\n",
					item, nand_secure_storage_block_bak);

		unsigned int check_sum = 0;
		struct nand_secure_storage_oob *nsoob = (struct nand_secure_storage_oob *)oob1;

		if (len < page_size)
			check_sum = nand_secure_check_sum(mbuf1, len);
		else
			check_sum = nand_secure_check_sum(mbuf1, page_size);

		if (nsoob->check_sum == cpu_to_be32(check_sum))
			printf("copy@0'item@%d is good in b@%d\n", item, nand_secure_storage_block_bak);

		for (copy = 1; copy < copys; copy++) {
			memset(mbuf2, 0x00, page_size);
			memset(oob2, 0x00, 8);
			p = item + copy * MAX_SECURE_STORAGE_ITEM;
			ret = nand_physic_read_page(0, nand_secure_storage_block_bak, p,
					page_size / 512, mbuf2, oob2);
			if (ret != 0)
				pr_err("read copy@%d'item@%d in b@%d fail\n", copy, item, nand_secure_storage_block_bak);

			if (memcmp(mbuf1, mbuf2, page_size) || memcmp(oob1, oob2, 8))
				pr_err("copy@%d'item@%d is \033[0;36mbad\033[0m in b@%d\n",
						copy, item, nand_secure_storage_block_bak);
			else
				printf("copy@%d'item@%d is good in b@%d\n",
						copy, item, nand_secure_storage_block_bak);
		}
	}
out:
	return ret;
}
#endif
/*use the first keys to fill whole secure storage*/
#if 0
int nand_secure_storage_backfill(void)
{
	int ret = -1, retry = 0;
	unsigned int page_size, i, page_cnt_per_blk = 0;
	int pages_cnt_per_item = 0;
	struct nand_secure_storage ns;
	struct nand_secure_storage_oob *nsoob;
	unsigned int len = ITEM_LEN;
	int item_page = 0;
	unsigned int check_sum = 0;
	/*unsigned int read_sum = 0;*/

	if (nand_is_support_secure_storage())
		return 0;

	NAND_PhysicLock();

	if (len % 1024) {
		pr_err("error! len = %d, agali 1024Bytes\n", len);
		ret = -1;
		goto out;
	}


	page_size = nand_get_pae_size();

	if (len <= page_size) {
		pages_cnt_per_item = 1;
	} else {
		pages_cnt_per_item = len / page_size;
		len = page_size;
	}

	/*ns.oob = (struct nand_secure_storage_oob *)spare;*/

	if (pages_cnt_per_item > MAX_PAGE_CNT_PER_ITEM) {
		pr_err("err pags cnt per item more than 2\n");
		ret = -1;
		goto out;
	}

	for (i = 0; i < pages_cnt_per_item * MAX_SECURE_STORAGE_ITEM; i++) {
		ns.pages[i] = nand_malloc(page_size);
		if (ns.pages[i] == NULL) {
			pr_err("nand malloc fail in fl\n");
			ret = -1;
			goto out;
		}
		memset(ns.pages[i], 0, page_size);
	}
	/*read item*/
	for (i = 0; i < pages_cnt_per_item * MAX_SECURE_STORAGE_ITEM; i++) {
retry0:
		ret = nand_physic_read_page(0, nand_secure_storage_block, i,
				page_size / 512, ns.pages[i], ns.oobs[i]);

		nsoob = (struct nand_secure_storage_oob *)ns.oobs[i];

		check_sum = nand_secure_check_sum(ns.pages[i], len);

		if (nsoob->check_sum != cpu_to_be32(check_sum)) {
			if (nsoob->check_sum != 0xffffffff) {
				pr_err("read secure storage b@%d p@%d fail, try to back secure storage b@%d p@%d\n",
						nand_secure_storage_block, i, nand_secure_storage_block_bak, i);
			}

			memset(ns.pages[i], 0, page_size);
			memset(ns.oobs[i], 0, 64);
			retry++;

			ret = nand_physic_read_page(0,
					nand_secure_storage_block_bak, i,
					page_size / 512, ns.pages[i], ns.oobs[i]);

			nsoob = (struct nand_secure_storage_oob *)ns.oobs[i];

			check_sum = nand_secure_check_sum(ns.pages[i], len);
			if (nsoob->check_sum != cpu_to_be32(check_sum)) {
				if (retry < 3) {
					goto retry0;
				} else {
					if (nsoob->check_sum != 0xffffffff)
						pr_err("item@%d is bad\n", i);
					else
						pr_err("item@%d is empty\n", i);
					retry = 0;
				}
			} else {
				pr_info("back secure storage b@%d p@%d is good\n",
						nand_secure_storage_block_bak, i);
			}
		}
	}

	page_cnt_per_blk = nand_get_page_cnt_per_block();

	retry = 0;

	nand_physic_erase_block(0, nand_secure_storage_block);
	for (i = 0; i < page_cnt_per_blk; i++) {
		item_page = i % (pages_cnt_per_item * MAX_SECURE_STORAGE_ITEM);

		if (nand_physic_write_page(0, nand_secure_storage_block, i,
					page_size / 512, ns.pages[item_page], ns.oobs[item_page])) {
			pr_err("write secure storage b@%d p@%d fail in bf\n",
						nand_secure_storage_block, i);
		}
	}

	nand_physic_erase_block(0, nand_secure_storage_block_bak);
	for (i = 0; i < page_cnt_per_blk; i++) {
		item_page = i % (pages_cnt_per_item * MAX_SECURE_STORAGE_ITEM);

		if (nand_physic_write_page(0, nand_secure_storage_block_bak, i,
					page_size / 512, ns.pages[item_page], ns.oobs[item_page])) {
			pr_err("write secure storage b@%d p@%d fail in bf\n",
						nand_secure_storage_block_bak, i);
		}
	}

#ifdef CHECK_BACKFILL
	nand_secure_storage_backfill_check();
#endif

	ret = 0;
	printf("nand secure storage backfill success\n");
out:

	for (i = 0; i < pages_cnt_per_item * MAX_SECURE_STORAGE_ITEM; i++) {
		if (ns.pages[i])
			nand_free(ns.pages[i]);
	}
	NAND_PhysicUnLock();

	return ret;
}
#endif
int nand_secure_storage_backfill(void)
{
	int ret = -1;
	unsigned int page_cnt_per_blk = 0;
	int cnt = 0;

	if (nand_is_support_secure_storage())
		return 0;

	page_cnt_per_blk = nand_get_page_cnt_per_block();
retry:
	/*copy blockA to blockB*/
	ret = nand_secure_storage_repair(1, page_cnt_per_blk);
	printf("%s ret@%d\n", __func__, ret);
	if (ret) {
		cnt++;
		if (cnt < 3)
			goto retry;
	}

#ifdef CHECK_BACKFILL
	nand_secure_storage_backfill_check();
#endif

	return ret;
}
/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       : -1;0;1;2
*Note         :
*****************************************************************************/
int nand_secure_storage_check(void)
{
	unsigned char *mbuf;
	int i, page_size, ret = -1, ret1 = -1, ret2 = -1;
	int pages_cnt_per_item = 0;

	page_size = nand_get_pae_size();
	mbuf = NAND_Malloc(page_size);
	if (mbuf == NULL) {
		pr_err("%s:malloc fail for mbuf\n", __func__);
	}

	if (ITEM_LEN <= page_size)
		pages_cnt_per_item = 1;
	else {
		pages_cnt_per_item = ITEM_LEN / page_size;
	}

	for (i = 0; i < pages_cnt_per_item * MAX_SECURE_STORAGE_ITEM; i++) {
		ret1 = nand_secure_storage_read_one(nand_secure_storage_block,
						    i, mbuf, page_size);
		ret2 = nand_secure_storage_read_one(nand_secure_storage_block_bak,
				i, mbuf, page_size);

		if (ret1 != ret2) {
			break;
		}
		if (ret1 < 0) {
			break;
		}
	}

	if ((ret1 < 0) && (ret2 < 0)) {
		ret = -1;
		pr_err("nand secure storage check fail:%d\n", i);
		goto ss_check_out;
	}

	if (ret1 == ret2) {
		ret = 0;
		goto ss_check_out;
	}

	if ((ret1 == 0) || (ret2 < 0)) {
		ret = 1;
	}

	if ((ret2 == 0) || (ret1 < 0)) {
		ret = 2;
	}

ss_check_out:

	NAND_Free(mbuf, page_size);

	return ret;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         :
*****************************************************************************/
int nand_secure_storage_repair(int flag, unsigned int copy_nums)
{
	int ret = 0;
	unsigned int block_s = 0, block_d = 0;

	if (flag == 0) {
		return 0;
	}

	if (flag == 1) {
		block_s = nand_secure_storage_block;
		block_d = nand_secure_storage_block_bak;
	}

	if (flag == 2) {
		block_s = nand_secure_storage_block_bak;
		block_d = nand_secure_storage_block;
	}


	ret |= nand_physic_erase_block(0, block_d);
	ret |= nand_physic_block_copy(0, block_s, 0, block_d, copy_nums);

	return ret;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         :
*****************************************************************************/
int nand_secure_storage_update(void)
{
	int ret, retry = 0;
	unsigned int page_cnt_per_blk = nand_get_page_cnt_per_block();

	while (1) {
		ret = nand_secure_storage_check();
		if (ret == 0) {
			break;
		}

		retry++;
		if (ret < 0) {
			pr_err("secure storage fail 1\n");
			return -1;
		}
		if (ret > 0) {
			pr_debug("secure storage repair:%d\n", ret);
			nand_secure_storage_repair(ret, page_cnt_per_blk);
		}

		if (retry > 3) {
			return -1;
		}
	}
	pr_debug("secure storage updata ok!\n");
	return 0;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         :
*****************************************************************************/
int nand_secure_storage_test(unsigned int item)
{
	unsigned char *mbuf;
	int i, page_size, ret = -1;

	if (item >= MAX_SECURE_STORAGE_ITEM) {
		return 0;
	}

	page_size = nand_get_pae_size();
	mbuf = NAND_Malloc(page_size);
	if (mbuf == NULL) {
		pr_err("%s:malloc fail for mbuf\n", __func__);
	}

	for (i = 0; i < MAX_SECURE_STORAGE_ITEM; i++) {
		ret = nand_secure_storage_read(i, mbuf, 2048);
		pr_debug("read secure storage:%d ret %d buf :0x%x,0x%x,0x%x,0x%x,\n",
			    i, ret, mbuf[0], mbuf[1], mbuf[2], mbuf[3]);
	}

	memset(mbuf, 0, 2048);
	mbuf[0] = 0x00 + item;
	mbuf[1] = 0x11 + item;
	mbuf[2] = 0x22 + item;
	mbuf[3] = 0x33 + item;
	nand_secure_storage_write(item, mbuf, 2048);

	for (i = 0; i < MAX_SECURE_STORAGE_ITEM; i++) {
		ret = nand_secure_storage_read(i, mbuf, 2048);
		pr_debug("read secure storage:%d ret %d buf :0x%x,0x%x,0x%x,0x%x,\n",
			    i, ret, mbuf[0], mbuf[1], mbuf[2], mbuf[3]);
	}

	NAND_Free(mbuf, page_size);

	return ret;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         :
*****************************************************************************/
int nand_secure_storage_test_read(unsigned int item, unsigned int block)
{
	unsigned char *mbuf;
	int page_size, ret = 0;
	//unsigned char spare[64];

	page_size = nand_get_pae_size();
	mbuf = NAND_Malloc(page_size);
	if (mbuf == NULL) {
		pr_err("%s:malloc fail for mbuf\n", __func__);
		return -1;
	}

	if (block == 0)
		block = nand_secure_storage_block;
	if (block == 1)
		block = nand_secure_storage_block_bak;

	ret = nand_secure_storage_read_one(block, item, mbuf, 2048);

	pr_debug("nand_secure_storage_test_read item:%d ret:%d buf:0x%x,0x%x,0x%x,0x%x,\n",
		    item, ret, mbuf[0], mbuf[1], mbuf[2], mbuf[3]);

	NAND_Free(mbuf, page_size);
	return ret;
}
