//SPDX-License-Identifier:	GPL-2.0+
/*****************************************************************************/
#define _SECURE_STORAGE_C_
/*****************************************************************************/
#include "nand_physic_inc.h"

/*****************************************************************************/

#define MAX_SECURE_STORAGE_ITEM 32

#define MIN_SECURE_STORAGE_BLOCK_NUM 8
#define MAX_SECURE_STORAGE_BLOCK_NUM 100
#if 1
#define SSB_DBG(...)
#else
#define SSB_DBG tick_printf
#endif

#define SSB_OPT

int nand_secure_storage_block;
int nand_secure_storage_block_bak;

__u32 NAND_GetPageCntPerBlk(void)
{
	return g_nctri->nci->page_cnt_per_blk;
}

__u32 NAND_GetPageSize(void)
{
	return ((g_nctri->nci->sector_cnt_per_page) * 512);
}

/*****************************************************************************
 *Name         :nand_is_support_secure_storage
 *Description  : judge if the flash support secure storage
 *Parameter    : null
 *Return       : 0:support  -1:don't support
 *Note         :
 *****************************************************************************/
int nand_is_support_secure_storage(void)
{
	return 0;
}

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

	page_size	= NAND_GetPageSize();
	page_cnt_per_blk = NAND_GetPageCntPerBlk();
	mbuf		 = MALLOC(page_size);

	MEMSET(mbuf, 0, page_size);
	MEMSET(spare, 0xff, 64);
	spare[1] = 0xaa;
	spare[2] = 0x5c;
	spare[3] = 0x00;
	spare[4] = 0x00;
	spare[5] = 0x12;
	spare[6] = 0x34;

	if (nand_physic_erase_block(0, block) == 0) {
		for (i = 0; i < page_cnt_per_blk; i++) {
			nand_physic_write_page(0, block, i,
					g_nsi->nci->sector_cnt_per_page,
					mbuf, spare);
		}
		mbuf[0] = 0X11;
		ret     = nand_secure_storage_read_one(block, 0, mbuf, 2048);
	}

	FREE(mbuf, page_size);

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
	SSB_DBG("%s %d in\n", __func__, __LINE__);
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
	unsigned char *mbuf;
	int page_size, ret = 0;
	unsigned char spare[64];

	page_size = NAND_GetPageSize();
	mbuf      = MALLOC(page_size);

	nand_physic_read_page(0, block, 0, g_nsi->nci->sector_cnt_per_page,
			mbuf, spare);
	if ((spare[0] == 0xff) && (spare[1] == 0xaa) && (spare[2] == 0x5c)) {
		ret = 1;
	}

	FREE(mbuf, page_size);

	return ret;
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
	int ret;

	nand_secure_storage_block     = 0;
	nand_secure_storage_block_bak = 0;

	if (nand_is_support_secure_storage() != 0)
		return 0;

	nBlkNum = 8;
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

	if ((nand_secure_storage_block < MIN_SECURE_STORAGE_BLOCK_NUM) ||
			(nand_secure_storage_block >= nand_secure_storage_block_bak)) {
		nand_secure_storage_block     = 0;
		nand_secure_storage_block_bak = 0;
		PHY_ERR("nand secure storage fail: %d,%d\n",
				nand_secure_storage_block,
				nand_secure_storage_block_bak);
		ret = -1;
	} else {
		if (flag != 0) {
			nand_secure_storage_update();
		}
		ret = 0;
		PRINT_DBG("nand secure storage ok: %d,%d\n",
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
		PRINT_DBG("start block:%d\n",
				nand_secure_storage_block_bak + 1);
		return nand_secure_storage_block_bak + 1;
	}

	nBlkNum = start_block;

	for (; nBlkNum < MAX_SECURE_STORAGE_BLOCK_NUM; nBlkNum++) {
		if (nand_physic_bad_block_check(0, nBlkNum) == 0) {
			ret = is_nand_secure_storage_block(nBlkNum);
			if (ret != 1) {
				nand_secure_storage_write_init(nBlkNum);
			}
			nand_secure_storage_block = nBlkNum;
			break;
		}
	}

	for (nBlkNum += 1; nBlkNum < MAX_SECURE_STORAGE_BLOCK_NUM; nBlkNum++) {
		if (nand_physic_bad_block_check(0, nBlkNum) == 0) {
			ret = is_nand_secure_storage_block(nBlkNum);
			if (ret != 1) {
				nand_secure_storage_write_init(nBlkNum);
			}
			nand_secure_storage_block_bak = nBlkNum;
			break;
		}
	}

	if ((nand_secure_storage_block < MIN_SECURE_STORAGE_BLOCK_NUM) ||
			(nand_secure_storage_block >= nand_secure_storage_block_bak)) {
		PHY_ERR("nand secure storage first build  fail: %d,%d\n",
				nand_secure_storage_block,
				nand_secure_storage_block_bak);
		nand_secure_storage_block     = 0;
		nand_secure_storage_block_bak = 0;
		block			      = start_block + 2;
	} else {
		block = nand_secure_storage_block_bak + 1;
		PRINT_DBG("nand secure storage first build  ok: %d,%d\n",
				nand_secure_storage_block,
				nand_secure_storage_block_bak);
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
	unsigned int page_cnt_per_blk = NAND_GetPageCntPerBlk();

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

	p	 = (unsigned int *)mbuf;
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

	page_cnt_per_blk = NAND_GetPageCntPerBlk();

	NAND_PhysicLock();
	SSB_DBG("%s %d item %d in\n", __func__, __LINE__, item);
	for (; item < page_cnt_per_blk; item += MAX_SECURE_STORAGE_ITEM) {
		MEMSET(spare, 0, 64);
		nand_physic_read_page(0, block, item,
				g_nsi->nci->sector_cnt_per_page, mbuf,
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
			PHY_ERR("spare_sum:0x%x,check_sum:0x%x\n", read_sum,
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
	int page_size, ret = -1, i;
	int pages_cnt_per_item = 0;
	unsigned int len_bak, item_tmp = item;

	if (nand_is_support_secure_storage())
		return 0;

	NAND_PhysicLock();
	SSB_DBG("%s %d %d %d in\n", __func__, __LINE__, item, len);
	if (len % 1024) {
		PHY_ERR("error! len = %d, agali 1024Bytes\n", len);
		ret = -1;
		goto out;
	}

	page_size = NAND_GetPageSize();
	len_bak   = len;
	if (len <= page_size)
		pages_cnt_per_item = 1;
	else {
		pages_cnt_per_item = len / page_size;
		len		   = page_size;
	}

	mbuf = MALLOC(page_size);
	if (!mbuf) {
		PHY_ERR("malloc error! %s[%d]\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto out;
	}
	PHY_DBG("nand_secure_storage_read item = %d, len = %d, pages_cnt_per_item = %d, blk = %d, blk_bak = %d\n",
			item, len_bak, pages_cnt_per_item, nand_secure_storage_block,
			nand_secure_storage_block_bak);
	for (i = 0; i < pages_cnt_per_item; i++) {
		item = item_tmp * pages_cnt_per_item + i;
		ret  = nand_secure_storage_read_one(nand_secure_storage_block,
				item, mbuf, len);
		if (ret != 0)
			ret = nand_secure_storage_read_one(
					nand_secure_storage_block_bak, item, mbuf, len);

		if (ret == 0)
			if (pages_cnt_per_item == 1)
				MEMCPY(buf, mbuf, len);
			else
				MEMCPY(buf + i * len, mbuf, len);
		else {
			//PHY_ERR("nand_secure_storage_read fail, memset 0x00\n");
			if (pages_cnt_per_item == 1)
				MEMSET(buf, 0, len);
			else
				MEMSET(buf + i * len, 0, len);
		}
	}

	FREE(mbuf, page_size);
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
	int ret		    = -1;
	unsigned char *mbuf = NULL;
	unsigned int page_size, i, check_sum, page, page_cnt_per_blk, len_bak;
	unsigned char *spare   = NULL;
	unsigned char *tmp_buf = NULL;

	int pages_cnt_per_item = 0;

	if (nand_is_support_secure_storage())
		return 0;

	NAND_PhysicLock();
	SSB_DBG("%s %d item %d len %d\n", __func__, __LINE__, item, len);
	nand_secure_storage_update();
	page_size	= NAND_GetPageSize();
	page_cnt_per_blk = NAND_GetPageCntPerBlk();

	if (len % 1024) {
		PHY_ERR("error! len = %d, agali 1024Bytes\n", len);
		ret = -1;
		goto out;
	}

	len_bak = len;
	if (len <= page_size)
		pages_cnt_per_item = 1;
	else {
		pages_cnt_per_item = len / page_size;
		len		   = page_size;
	}

	tmp_buf = MALLOC(MAX_SECURE_STORAGE_ITEM * pages_cnt_per_item *
			(page_size + 64));
	if (!tmp_buf) {
		PHY_ERR("malloc error! %s[%d]\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto out;
	}

	//for debug
	//	PHY_DBG("buf = 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
	//		buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
	//
	PHY_ERR("nand_secure_storage_write, item = %d, len = %d, pages_cnt_per_item = %d, blk = %d, blk_bak = %d\n",
			item, len_bak, pages_cnt_per_item, nand_secure_storage_block,
			nand_secure_storage_block_bak);
	SSB_DBG("Write 1st block\n");
	nand_physic_erase_block(0, nand_secure_storage_block_bak);
	for (i = 0; i < MAX_SECURE_STORAGE_ITEM * pages_cnt_per_item; i++) {
		mbuf  = tmp_buf + i * (page_size + 64);
		spare = mbuf + page_size;
		if ((i / pages_cnt_per_item) != item) {
			//PHY_DBG("read after old writed\n");
			nand_physic_read_page(0, nand_secure_storage_block, i,
					g_nsi->nci->sector_cnt_per_page,
					mbuf, spare);
		} else {
			//PHY_DBG("new writed\n");
			MEMSET(mbuf, 0, page_size);
			if (pages_cnt_per_item == 1)
				MEMCPY(mbuf, buf, len);
			else
				MEMCPY(mbuf,
						buf +
						page_size *
						(i % pages_cnt_per_item),
						page_size);
			check_sum = nand_secure_check_sum(mbuf, len);

			MEMSET(spare, 0xff, 64);
			spare[1] = 0xaa;
			spare[2] = 0x5c;
			spare[3] = (unsigned char)(check_sum >> 24);
			spare[4] = (unsigned char)(check_sum >> 16);
			spare[5] = (unsigned char)(check_sum >> 8);
			spare[6] = (unsigned char)(check_sum);
		}
		//PHY_DBG("i = %d, spare 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
		//		i, spare[0], spare[1], spare[2], spare[3], spare[4], spare[5], spare[6], spare[7]);
		if (nand_physic_write_page(0, nand_secure_storage_block_bak, i,
					g_nsi->nci->sector_cnt_per_page,
					mbuf, spare))
			PHY_ERR("nand_secure_storage_write fail\n");
	}
	SSB_DBG("Fill 1st block\n");
	for (i = MAX_SECURE_STORAGE_ITEM * pages_cnt_per_item;
			i < page_cnt_per_blk; i++) {
		page = i % (MAX_SECURE_STORAGE_ITEM * pages_cnt_per_item);
#ifndef SSB_OPT
		nand_physic_read_page(0, nand_secure_storage_block_bak, page,
				g_nsi->nci->sector_cnt_per_page, mbuf,
				spare);
#else
		mbuf  = tmp_buf + page * (page_size + 64);
		spare = mbuf + page_size;
#endif
		nand_physic_write_page(0, nand_secure_storage_block_bak, i,
				g_nsi->nci->sector_cnt_per_page, mbuf,
				spare);
	}

	SSB_DBG("Write 2nd block\n");
#ifndef SSB_OPT
	nand_secure_storage_repair(2);
#else
	nand_physic_erase_block(0, nand_secure_storage_block);
	for (i = 0; i < page_cnt_per_blk; i++) {
		page  = i % (MAX_SECURE_STORAGE_ITEM * pages_cnt_per_item);
		mbuf  = tmp_buf + page * (page_size + 64);
		spare = mbuf + page_size;
		SSB_DBG("%s %d wp\n", __func__, __LINE__);
		nand_physic_write_page(0, nand_secure_storage_block, i,
				g_nsi->nci->sector_cnt_per_page, mbuf,
				spare);
		SSB_DBG("%s %d wpe\n", __func__, __LINE__);
	}
#endif
	FREE(tmp_buf,
			MAX_SECURE_STORAGE_ITEM * pages_cnt_per_item * (page_size + 64));
	ret = 0;
out:
	NAND_PhysicUnLock();
	SSB_DBG("%s %d out\n", __func__, __LINE__);
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
	int i, page_size, ret = -1, ret1, ret2;

	page_size = NAND_GetPageSize();
	mbuf      = MALLOC(page_size);

	for (i = 0; i < MAX_SECURE_STORAGE_ITEM; i++) {
		ret1 = nand_secure_storage_read_one(nand_secure_storage_block,
				i, mbuf, page_size);
		ret2 = nand_secure_storage_read_one(
				nand_secure_storage_block_bak, i, mbuf, page_size);
		if (ret1 != ret2) {
			break;
		}
		if (ret1 < 0) {
			break;
		}
	}

	if ((ret1 < 0) && (ret2 < 0)) {
		ret = -1;
		PHY_ERR("nand secure storage check fail:%d\n", i);
		goto ss_check_out;
	}

	if (ret1 == ret2) {
		ret = 0;
		goto ss_check_out;
	}

	if ((ret1 == 0) || (ret2 < 0)) {
		ret = 1;
		//block_d = nand_secure_storage_block_bak;
		//block_s = nand_secure_storage_block;
	}

	if ((ret2 == 0) || (ret1 < 0)) {
		ret = 2;
		//block_d = nand_secure_storage_block;
		//block_s = nand_secure_storage_block_bak;
	}

ss_check_out:

	FREE(mbuf, page_size);
	SSB_DBG("%s %d %d out\n", __func__, __LINE__, ret);

	return ret;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int nand_secure_storage_repair(int flag)
{
	int ret		     = 0;
	unsigned int block_s = nand_secure_storage_block;
	unsigned int block_d = nand_secure_storage_block_bak;

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

	SSB_DBG("%s %d flag:%d\n", __func__, __LINE__, flag);
	ret |= nand_physic_block_copy(0, block_s, 0, block_d);
	SSB_DBG("%s %d\n", __func__, __LINE__);

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

	while (1) {
		ret = nand_secure_storage_check();
		if (ret == 0) {
			break;
		}

		retry++;
		if (ret < 0) {
			PHY_ERR("secure storage fail 1\n");
			return -1;
		}
		if (ret > 0) {
			PHY_ERR("secure storage repair:%d\n", ret);
			nand_secure_storage_repair(ret);
		}
		if (retry > 3) {
			return -1;
		}
	}
	PRINT_DBG("secure storage updata ok!\n");
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
	unsigned char *mbuf = NULL;
	int i, ret = -1;
#define SECURE_BUFF_LEN 4096

	PHY_ERR("%s[%d], item = %d\n", __FUNCTION__, __LINE__, item);
	if (item >= MAX_SECURE_STORAGE_ITEM) {
		return 0;
	}

	/*
	   nand_secure_storage_block = 0;
	   nand_secure_storage_block_bak = 1;
	 */
	mbuf = MALLOC(SECURE_BUFF_LEN);
	if (!mbuf) {
		PHY_ERR("%d[%s]mallco fail\n", __FUNCTION__, __LINE__);
		return -1;
	}

	for (i = 0; i < MAX_SECURE_STORAGE_ITEM; i++) {
		ret = nand_secure_storage_read(i, mbuf, SECURE_BUFF_LEN);
		PHY_ERR("read secure storage:%d ret %d buf :0x%x,0x%x,0x%x,0x%x,\n",
				i, ret, mbuf[0], mbuf[1], mbuf[2], mbuf[3]);
	}

	MEMSET(mbuf, 0, SECURE_BUFF_LEN);
	mbuf[0] = 0x00 + item;
	mbuf[1] = 0x11 + item;
	mbuf[2] = 0x22 + item;
	mbuf[3] = 0x33 + item;
	if (SECURE_BUFF_LEN > 2048) {
		mbuf[2047] = 0x44 + item;
		mbuf[2048] = 0x55 + item;
		mbuf[2049] = 0x66 + item;
		mbuf[2050] = 0x77 + item;
	}
	if (nand_secure_storage_write(item, mbuf, SECURE_BUFF_LEN))
		PHY_ERR("write secure storage error\n");

	for (i = 0; i < MAX_SECURE_STORAGE_ITEM; i++) {
		ret = nand_secure_storage_read(i, mbuf, SECURE_BUFF_LEN);
		PHY_ERR("write after read secure storage:%d ret %d buf :0x%x,0x%x,0x%x,0x%x,\n",
				i, ret, mbuf[0], mbuf[1], mbuf[2], mbuf[3]);
	}

	FREE(mbuf, SECURE_BUFF_LEN);

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
	int page_size, ret;

	page_size = NAND_GetPageSize();
	mbuf      = MALLOC(page_size);

	if (block == 0)
		block = nand_secure_storage_block;
	if (block == 1)
		block = nand_secure_storage_block_bak;

	ret = nand_secure_storage_read_one(block, item, mbuf, 2048);

	PHY_ERR("nand_secure_storage_test_read item:%d ret:%d buf:0x%x,0x%x,0x%x,0x%x,\n",
			item, ret, mbuf[0], mbuf[1], mbuf[2], mbuf[3]);

	FREE(mbuf, page_size);
	return 0;
}
