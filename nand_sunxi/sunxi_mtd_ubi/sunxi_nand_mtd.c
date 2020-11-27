/*
 * (C) Copyright 2007-2017
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * zhongguizhao <zhongguizhao@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.
 */
#include <common.h>
#include <malloc.h>
#include "sunxi_nand_mtd.h"
#include <linux/mtd/mtd.h>
#include "sunxi_nand_driver.h"

#ifdef CONFIG_SUNXI_UBIFS

static char g_fix_sunxi_nand_default;

static char ecc_mode_hw[11] = {16, 24, 28, 32, 40, 48, 56, 60, 64, 72, 80};
static char ecc_limit[11] =   {13, 20, 23, 27, 35, 42, 50, 54, 58, 66, 70};

static char nand_para_buf[256];
boot_nand_para_t *g_nand_para = (boot_nand_para_t *)nand_para_buf;
static struct nand_chip nand_chip;


static int sunxi_nand_bad_block_check(unsigned int chip, unsigned int block)
{
	if (g_fix_sunxi_nand_default)
		return nand_physic_bad_block_check(chip, block);
	else
		return PHY_VirtualBadBlockCheck(chip, block);
}

static int sunxi_nand_bad_block_mark(unsigned int chip, unsigned int block)
{
	if (g_fix_sunxi_nand_default)
		return nand_physic_bad_block_mark(chip, block);
	else
		return PHY_VirtualBadBlockMark(chip, block);
}

static int sunxi_nand_erase_block(unsigned int chip, unsigned int block)
{
	if (g_fix_sunxi_nand_default)
		return nand_physic_erase_block(chip, block);
	else
		return PHY_VirtualBlockErase(chip, block);
}

static int sunxi_nand_rd_page(uint chip, uint block, uint page,
	uint64_t bitmap, uint8_t *mbuf, uint8_t *sbuf)
{
	if (g_fix_sunxi_nand_default)
		return nand_physic_read_page(chip, block, page,
			(unsigned int)bitmap, mbuf, sbuf);
	else
		return PHY_VirtualPageRead(chip, block, page,
			bitmap, mbuf, sbuf);
}

static int sunxi_nand_wr_page(uint chip, uint block, uint page,
	uint64_t bitmap, uint8_t *mbuf, uint8_t *sbuf)
{
	if (g_fix_sunxi_nand_default)
		return nand_physic_write_page(chip, block, page,
			(unsigned int)bitmap, mbuf, sbuf);
	else
		return PHY_VirtualPageWrite(chip, block, page,
			bitmap, mbuf, sbuf);
}

static void pr_buf(uint8_t *buf, uint len)
{
	int i;

	for (i = 0; i < len; i++) {
		if ((i > 0) && !(i % 16))
			printf("\n");

		printf("%02x ", buf[i]);
	}
	printf("\n");
}

/**
 *
 * return data is  2^n
 */
static uint64_t align_data(uint64_t dat)
{
	int i;
	int no = 0;
	uint64_t data = dat;

	for (i = 0; i < 64; i++) {
		if (data & 0x01)
			no = i;
		data >>= 1;
	}
	return dat & ((uint64_t)1 << no);
}
static int check_offs_len(struct mtd_info *mtd,
					loff_t ofs, uint64_t len)
{
	struct nand_chip *chip = mtd->priv;
	int ret = 0;

	/* Start address must align on block boundary */
	if (ofs & ((1 << chip->phys_erase_shift) - 1)) {
		MTDDEBUG(MTD_DEBUG_LEVEL0, "%s: Unaligned address\n", __func__);
		ret = -EINVAL;
	}

	/* Length must align on block boundary */
	if (len & ((1 << chip->phys_erase_shift) - 1)) {
		MTDDEBUG(MTD_DEBUG_LEVEL0, "%s: Length not block aligned\n",
					__func__);
		ret = -EINVAL;
	}

	return ret;
}

static uint get_blkcnt_perchip(void)
{
	if (g_fix_sunxi_nand_default)
		return g_nand_para->BlkCntPerDie;
	else
		return g_nssi->nsci->blk_cnt_per_super_chip;
}

static uint get_ph_blk_cnt_per_die(void)
{
	return g_nand_para->BlkCntPerDie;
}

static uint get_super_blk_cnt_per_die(void)
{
	return g_nssi->nsci->blk_cnt_per_super_chip;
}

/*
 * static uint64_t get_phy_blksize(void)
 * {
 *	uint32_t pg_cnt, sec_cnt;
 *
 *	pg_cnt = g_nand_para->PageCntPerPhyBlk;
 *	sec_cnt = g_nand_para->SectorCntPerPage;
 *
 *	return (pg_cnt * sec_cnt) << 9;
 * }
 */

/**
 * return: 0 --valid blk; -1 --reserved for boot0 or boot1
 */
static int chk_boot0_boot1_reserved_blks(int blk_num)
{
	int mul;
	int boot_num;

	boot_num = BOOT0_1_USED_PHY_NAND_BLK_CNT;
	mul = get_ph_blk_cnt_per_die() / get_super_blk_cnt_per_die();

	if (mul * blk_num < boot_num) {
		printf("mtd ignore:");
		printf("%d is in boot0 or boot1 (0 - %d)\n",
			blk_num, boot_num-1);
		return 1;
	}

	return 0;
}

/**
 *
 *
 *
 *
 *return :0 -ok; 1-bank page.
 */
static int sunxi_nand_chk_bank_page(uint8_t *sbuf, uint len)
{
	uint8_t cmp_buf[SUNXI_MAX_SECS_PER_PAGE * 4];

	if (!sbuf || !len)
		return 0;

	memset(cmp_buf, 0xff, sizeof(cmp_buf));
	if (!memcmp(sbuf, cmp_buf, MIN(len, sizeof(cmp_buf))))
		return 1;
	else
		return 0;
}

/**
 * nand_erase - [MTD Interface] erase block(s)
 * @mtd: MTD device structure
 * @instr: erase instruction
 *
 * Erase one ore more blocks.
 */
static int sunxi_nand_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	int blk_num, ret, chipnr;
	loff_t len;
	uint64_t addr;
	struct nand_chip *chip = mtd->priv;

#ifdef	SUNXI_NAND_MTD_DEBUG
	tick_printf("line: %d, %s\n", __LINE__,  __func__);
#endif

	MTDDEBUG(MTD_DEBUG_LEVEL3,
		"%s: start = 0x%012llx, len = %llu\n",
		__func__, (unsigned long long)instr->addr,
		(unsigned long long)instr->len);

	addr = instr->addr;

	if (check_offs_len(mtd, addr, instr->len))
		return -EINVAL;

	chipnr = (uint)(addr >> chip->chip_shift);
	blk_num = (uint)((addr & (chip->chipsize - 1))
		>> chip->phys_erase_shift);
	len = instr->len;

#ifdef SUNXI_NAND_MTD_DEBUG
	tick_printf("erase chip:%0d, blk:%0d , len:%08x, %08x\n",
		chipnr, blk_num, (uint32)(len >> 32), (uint32)len);
#endif

	while (len) {
		if (chipnr >= chip->numchips) {
			tick_printf("Err: chipnr overflow.");
			tick_printf("chipnr=%d, chips=%d, line:%d, %s\n",
				chipnr, chip->numchips, __LINE__,  __func__);
			ret = -EINVAL;
			break;
		}

		if (blk_num >= get_blkcnt_perchip()) {
			tick_printf("warning: blk_num is overflow.");
			tick_printf("blk:%d\n", blk_num);
			ret = 0;
		} else {
			ret = sunxi_nand_erase_block(chipnr, blk_num);
			if (ret) {
				instr->state = MTD_ERASE_FAILED;
				tick_printf("Erase blk fail. block:%x\n",
					blk_num);
				goto erase_exit;
			}
		}

		len -= (1 << chip->phys_erase_shift);
		blk_num++;
		if (len && !((blk_num << chip->page_shift) & chip->pagemask)) {
			chipnr++;
			blk_num = 0;
		}
	}
	instr->state = MTD_ERASE_DONE;

erase_exit:

	ret = instr->state == MTD_ERASE_DONE ? 0 : -EIO;

	/* Do call back function */
	/*
	 * if (!ret)
	 *	mtd_erase_callback(instr);
	 */

#ifdef SUNXI_NAND_MTD_DEBUG
	tick_printf("exit: ret=%0x, line:%d, %s\n",
		ret, __LINE__,  __func__);
#endif

	return ret;
}

/**
 * nand_do_read_ops - [INTERN] Read data with ECC
 * @mtd: MTD device structure
 * @from: offset to read from
 * @ops: oob ops structure
 *
 * Internal function. Called with chip held.
 */
static int nand_do_read_ops(struct mtd_info *mtd, loff_t from,
			    struct mtd_oob_ops *ops)
{
	struct nand_chip *chip = mtd->priv;
	uint page, blk_num, chipnr, page_size, pages_per_blk;
	uint col, copy_len, copy_oob_len;
	uint64_t sec_bitmap;
	uint8_t *mbuf;
	uint8_t sbuf[SUNXI_MAX_SECS_PER_PAGE * 4];
	uint8_t *oob, *buf;
	int ret = -ENOTSUPP;
	uint64_t addr;
	uint readlen = ops->len;

#ifdef SUNXI_NAND_MTD_DEBUG
	tick_printf("line: %d, %s\n", __LINE__,  __func__);
#endif

	addr = from;
	ops->oobretlen = 0;
	ops->retlen = 0;

	if (!readlen)
		return 0;

	mbuf = (void *)malloc(SUNXI_MAX_PAGE_SIZE);
	if (!mbuf) {
		tick_printf("Err: malloc memory fail, line: %d, %s\n",
			__LINE__,  __func__);
		return -ENOMEM;
	}
	if ((1 << chip->page_shift) > SUNXI_MAX_PAGE_SIZE) {
		tick_printf("Err: pagesize > mbuf_size.");
		tick_printf("pagesize=0x%0x, line: %d, %s\n",
			1 << chip->page_shift, __LINE__,  __func__);
		return -ENOMEM;
	}
	if (mtd->oobsize > sizeof(sbuf)) {
		tick_printf("Err: oobsize > sbuf_size.");
		tick_printf("oobsize=0x%0x, sbuf_size=0x%0x, line: %d, %s\n",
			mtd->oobsize, sizeof(sbuf), __LINE__,	__func__);
		return -ENOMEM;
	}

	buf = ops->datbuf;
	oob = ops->oobbuf;
	if (oob)
		copy_oob_len = MIN(ops->ooblen, mtd->oobsize);
	else
		copy_oob_len = mtd->oobsize;

	chipnr = (uint)(addr >> chip->chip_shift);
	blk_num = (uint)((addr & (chip->chipsize - 1))
		>> chip->phys_erase_shift);
	page_size = 1 << chip->page_shift;
	pages_per_blk = 1 <<
		(chip->phys_erase_shift - chip->page_shift);
	page = ((int)(addr >> chip->page_shift) &
		chip->pagemask) % pages_per_blk;
	sec_bitmap = page_size >> 9;

	col = addr % page_size;

#ifdef SUNXI_NAND_MTD_DEBUG
	tick_printf("src from: 0x%08x, %08x\n",
		(uint32)(from >> 32), (uint32)from);
	tick_printf("page_size:0x%0x, pages_per_blk:0x%0x\n",
		page_size, pages_per_blk);
	tick_printf("addr: 0x%08x,%08x\n", (uint32)(addr >> 32), (uint32)addr);
	tick_printf("readlen: 0x%0x\n", readlen);
	tick_printf("chip:%0d, blk:%0d, page:%0d, col:%0d\n",
		chipnr, blk_num, page, col);
#endif

	while (readlen) {
		if (chipnr >= chip->numchips) {
			tick_printf("Err: chipnr overflow.");
			tick_printf("chipnr=%d, chips=%d, line:%d, %s\n",
				chipnr, chip->numchips, __LINE__,  __func__);
			ret = -EINVAL;
			break;
		}
		if (readlen >= (page_size - col))
			copy_len = page_size - col;
		else
			copy_len = readlen;

		ret = sunxi_nand_rd_page(chipnr, blk_num, page,
			sec_bitmap, mbuf, sbuf);
		if (ret && (ret != ECC_LIMIT)) {
			tick_printf("Err: Read data abort.");
			tick_printf("chipnr=%d, blk_num=%d, page=%d, col=%d\n",
				chipnr, blk_num, page, col);
			tick_printf("Err data is(first 16 bytes):\n");
			pr_buf(mbuf + col, MIN(copy_len, 16));
			break;
		}
		if (ret == ECC_LIMIT) {
			ret = 0;
			tick_printf("Warning: Read data ECC_LIMIT.");
			tick_printf("chipnr=%d, blk_num=%d, page=%d\n",
				chipnr, blk_num, page);
		}

		if (sunxi_nand_chk_bank_page(sbuf, copy_oob_len)) {
			memset(mbuf, 0xff, page_size);
#ifdef SUNXI_NAND_MTD_DEBUG
			tick_printf("Warning: Bank page.");
			tick_printf("chipnr=%d, blk_num=%d, page=%d\n",
				chipnr, blk_num, page);
#endif
		}

		if (buf) {
			memcpy(buf, mbuf + col, copy_len);
			buf += copy_len;
		}
		if (oob) {
			memcpy(oob, sbuf, copy_oob_len);
			ops->oobretlen = copy_oob_len;
		}
		readlen -= copy_len;
		col = 0;
		page++;
	    if (page >= pages_per_blk) {
			blk_num++;
			page = 0;
			if (blk_num >= (int)(chip->chipsize >>
				chip->phys_erase_shift)) {
				blk_num = 0;
				chipnr++;
			}
		}
	}
	ops->retlen = ops->len - readlen;
	free(mbuf);

#ifdef SUNXI_NAND_MTD_DEBUG
	tick_printf("exit: ret = 0x%0x, line: %d, %s\n",
		ret, __LINE__,  __func__);
#endif

	return ret;
}

/**
 * nand_do_read_oob - [INTERN] NAND read out-of-band
 * @mtd: MTD device structure
 * @from: offset to read from
 * @ops: oob operations description structure
 *
 * NAND read out-of-band data from the spare area.
 */
static int nand_do_read_oob(struct mtd_info *mtd, loff_t from,
			    struct mtd_oob_ops *ops)
{
	struct nand_chip *chip = mtd->priv;

	if (!ops->datbuf)
		ops->len = 1 << chip->page_shift;

	return nand_do_read_ops(mtd, from, ops);
}

/**
 * nand_read - [MTD Interface] MTD compatibility function for nand_do_read_ecc
 * @mtd: MTD device structure
 * @from: offset to read from
 * @len: number of bytes to read
 * @retlen: pointer to variable to store the number of read bytes
 * @buf: the databuffer to put data
 *
 * Get hold of the chip and call nand_do_read.
 */
static int sunxi_nand_read(struct mtd_info *mtd, loff_t from, size_t len,
		     size_t *retlen, uint8_t *buf)
{
	struct mtd_oob_ops ops;
	int ret;

#ifdef SUNXI_NAND_MTD_DEBUG
	tick_printf("line: %d, %s\n", __LINE__,  __func__);
#endif

	ops.len = len;
	ops.datbuf = buf;
	ops.oobbuf = NULL;
	ret = nand_do_read_ops(mtd, from, &ops);
	*retlen = ops.retlen;
	return ret;
}

/**
 * nand_do_write_ops - [INTERN] NAND write with ECC
 * @mtd: MTD device structure
 * @to: offset to write to
 * @ops: oob operations description structure
 *
 * NAND write with ECC.
 */
static int nand_do_write_ops(struct mtd_info *mtd, loff_t to,
			     struct mtd_oob_ops *ops)
{
	struct nand_chip *chip = mtd->priv;
	uint page, blk_num, chipnr, page_size, pages_per_blk;
	uint copy_oob_len = 0;
	uint64_t sec_bitmap;
	uint8_t *mbuf;
	uint8_t sbuf[SUNXI_MAX_SECS_PER_PAGE * 4];
	uint8_t *oob, *buf;
	int ret = -ENOTSUPP;
	uint64_t addr;
	uint writelen = ops->len;

#ifdef SUNXI_NAND_MTD_DEBUG
	tick_printf("line: %d, %s\n", __LINE__,  __func__);
#endif

	addr = to;
	ops->retlen = 0;
	ops->oobretlen = 0;

	if (!writelen)
		return 0;

	if (writelen % (1 << chip->page_shift))
		return	-EINVAL;

	mbuf = (void *)malloc(SUNXI_MAX_PAGE_SIZE);
	if (!mbuf) {
		tick_printf("Err: malloc memory fail.\n");
		return -ENOMEM;
	}
	if ((1 << chip->page_shift) > SUNXI_MAX_PAGE_SIZE) {
		tick_printf("Err: pagesize > mbuf_size. pagesize = 0x%0x\n",
			(1 << chip->page_shift));
		return -ENOMEM;
	}
	if (mtd->oobsize > sizeof(sbuf)) {
		tick_printf("Err: oobsize > sbuf_size.");
		tick_printf("oobsize = 0x%0x, sbuf_size = 0x%0x\n",
			mtd->oobsize, sizeof(sbuf));
		return -ENOMEM;
	}

	memset(mbuf, 0xff, SUNXI_MAX_PAGE_SIZE);
	memset(sbuf, 0xff, sizeof(sbuf));

	buf = ops->datbuf;
	oob = ops->oobbuf;
	if (oob) {
		copy_oob_len = ops->ooblen < sizeof(sbuf) ?
			ops->ooblen : sizeof(sbuf);
		memcpy(sbuf, oob, copy_oob_len);
	}

	chipnr = (uint)(addr >> chip->chip_shift);
	blk_num =
	(uint)((addr & (chip->chipsize - 1)) >> chip->phys_erase_shift);
	page_size = 1 << chip->page_shift;
	pages_per_blk = (1 << chip->phys_erase_shift) / page_size;
	page = ((int)(addr >> chip->page_shift)
		& chip->pagemask) % pages_per_blk;

	sec_bitmap = page_size >> 9;

#ifdef SUNXI_NAND_MTD_DEBUG
	tick_printf("addr: 0x%08x,%08x\n",
	(uint32)((uint64_t)addr >> 32), (uint32)addr);
	tick_printf("writelen: 0x%0x\n", writelen);
	tick_printf("chipnum:%0d, blknum:%0d, page:%0d\n",
		chipnr, blk_num, page);
#endif

	while (writelen) {
		if (chipnr >= chip->numchips) {
			tick_printf("Err: chipnr overflow.");
			tick_printf("chipnr=%d, total_chips=%d\n",
				chipnr, chip->numchips);
			ret = -EINVAL;
			break;
		}
		if (buf) {
			memcpy(mbuf, buf, page_size);
			buf += page_size;
		}
		ret = sunxi_nand_wr_page(chipnr, blk_num, page,
			sec_bitmap, mbuf, sbuf);
		if (ret) {
			tick_printf("Err: write data abort.");
			tick_printf("chipnr=%d, blk_num=%d, page=%d\n",
				chipnr, blk_num, page);
			break;
		}
		ops->oobretlen = copy_oob_len;
		writelen -= page_size;
		page++;
	    if (page >= pages_per_blk) {
			blk_num++;
			page = 0;
			if (blk_num >= (int)(chip->chipsize >>
				chip->phys_erase_shift)) {
				blk_num = 0;
				chipnr++;
			}
		}
	}
	ops->retlen = ops->len - writelen;
	free(mbuf);

#ifdef SUNXI_NAND_MTD_DEBUG
	tick_printf("exit: ret = 0x%0x, line = %d, %s\n\n",
		ret, __LINE__,  __func__);
#endif

	return ret;
}

/**
 * nand_do_write_oob - [MTD Interface] NAND write out-of-band
 * @mtd: MTD device structure
 * @to: offset to write to
 * @ops: oob operation description structure
 *
 * NAND write out-of-band.
 */
static int nand_do_write_oob(struct mtd_info *mtd, loff_t to,
			     struct mtd_oob_ops *ops)
{
	struct nand_chip *chip = mtd->priv;

	if (!ops->datbuf)
		ops->len = 1 << chip->page_shift;

	return nand_do_write_ops(mtd, to, ops);
}

/**
 * nand_write - [MTD Interface] NAND write with ECC
 * @mtd: MTD device structure
 * @to: offset to write to
 * @len: number of bytes to write
 * @retlen: pointer to variable to store the number of written bytes
 * @buf: the data to write
 *
 * NAND write with ECC.
 */
static int sunxi_nand_write(struct mtd_info *mtd, loff_t to, size_t len,
			  size_t *retlen, const uint8_t *buf)
{
	struct mtd_oob_ops ops;
	int ret;
	uint8_t sbuf[SUNXI_MAX_SECS_PER_PAGE * 4];

#ifdef SUNXI_NAND_MTD_DEBUG
	tick_printf("line: %d, %s\n", __LINE__,  __func__);
#endif

	memset(sbuf, 0xff, sizeof(sbuf));
	sbuf[1] = SUNXI_NAND_W_FLAG;

	ops.len = len;
	ops.datbuf = (uint8_t *)buf;
	ops.oobbuf = sbuf;
	ops.ooblen = mtd->oobsize;
	ret = nand_do_write_ops(mtd, to, &ops);
	*retlen = ops.retlen;
	return ret;
}

/**
 * nand_read_oob - [MTD Interface] NAND read data and/or out-of-band
 * @mtd: MTD device structure
 * @from: offset to read from
 * @ops: oob operation description structure
 *
 * NAND read data and/or out-of-band data.
 */
static int sunxi_nand_read_oob(struct mtd_info *mtd, loff_t from,
			 struct mtd_oob_ops *ops)
{
	int ret = -ENOTSUPP;

#ifdef SUNXI_NAND_MTD_DEBUG
	tick_printf("line: %d, %s\n", __LINE__,  __func__);
#endif

	ops->retlen = 0;

	/* Do not allow reads past end of device */
	if (ops->datbuf && (from + ops->len) > mtd->size) {
		MTDDEBUG(MTD_DEBUG_LEVEL0,
			"%s: Attempt read beyond end of device\n",
			__func__);
		return -EINVAL;
	}

	if (!ops->datbuf)
		ret = nand_do_read_oob(mtd, from, ops);
	else
		ret = nand_do_read_ops(mtd, from, ops);
	return ret;
}

/**
 * nand_write_oob - [MTD Interface] NAND write data and/or out-of-band
 * @mtd: MTD device structure
 * @to: offset to write to
 * @ops: oob operation description structure
 */
static int sunxi_nand_write_oob(struct mtd_info *mtd, loff_t to,
			  struct mtd_oob_ops *ops)
{
	int ret = -ENOTSUPP;

#ifdef SUNXI_NAND_MTD_DEBUG
	tick_printf("line: %d, %s\n", __LINE__,  __func__);
#endif

	ops->retlen = 0;

	/* Do not allow writes past end of device */
	if (ops->datbuf && (to + ops->len) > mtd->size) {
		MTDDEBUG(MTD_DEBUG_LEVEL0,
			"%s: Attempt write beyond end of device\n",
			__func__);
		return -EINVAL;
	}

	if (!ops->datbuf)
		ret = nand_do_write_oob(mtd, to, ops);
	else
		ret = nand_do_write_ops(mtd, to, ops);

	return ret;
}

/**
 * nand_sync - [MTD Interface] sync
 * @mtd: MTD device structure
 *
 * Sync is actually a wait for chip ready function.
 */
static void sunxi_nand_sync(struct mtd_info *mtd)
{
#ifdef SUNXI_NAND_MTD_DEBUG
	tick_printf("line: %d, %s\n\n", __LINE__,  __func__);
#endif

}

/**
 * nand_block_isbad - [MTD Interface] Check if block at offset is bad
 * @mtd: MTD device structure
 * @offs: offset relative to mtd start
 */
static int sunxi_nand_block_isbad(struct mtd_info *mtd, loff_t offs)
{
	struct nand_chip *chip = mtd->priv;
	uint chipnr, blk_num;
	uint64_t addr;

#ifdef SUNXI_NAND_MTD_DEBUG
	tick_printf("line: %d, %s\n", __LINE__,  __func__);
#endif

	addr = offs;

	chipnr = (uint)(addr >> chip->chip_shift);
	blk_num =
	(uint)((addr & (chip->chipsize - 1)) >> chip->phys_erase_shift);

	if (blk_num >= get_blkcnt_perchip()) {
		tick_printf("Fix bad blk, blk_num: %d\n", blk_num);
		return 1;
	}

	if (chk_boot0_boot1_reserved_blks(blk_num))
		return 1;

#ifdef SUNXI_NAND_MTD_DEBUG
	tick_printf("offs: 0x%0x\n", (uint)offs);
	tick_printf("chipnr: %0d, blk_num: %0d\n", chipnr, blk_num);
#endif

	if (sunxi_nand_bad_block_check(chipnr, blk_num)) {
		tick_printf("Warning: find bad blk.");
		tick_printf("chipnr=%d, blk_nr=%d XXX\n", chipnr, blk_num);
		return 1;
	} else {

#ifdef SUNXI_NAND_MTD_DEBUG
		tick_printf("exit: line: %d, %s\n\n", __LINE__,  __func__);
#endif
		return 0;
	}
}

static int _sunxi_nand_block_markbad(struct mtd_info *mtd, loff_t ofs)
{
	struct nand_chip *chip = mtd->priv;
	uint chipnr, blk_num;
	uint64_t addr;

	addr = ofs;

	chipnr = (uint)(addr >> chip->chip_shift);
	blk_num = (uint)((addr & (chip->chipsize - 1))
		>> chip->phys_erase_shift);

	if (blk_num >= get_blkcnt_perchip()) {
		tick_printf("warning: offs is overflow, fix bad blk.");
		tick_printf("offs:0x%llx, blk_num: %d\n", ofs, blk_num);
		return 0;
	}

	return sunxi_nand_bad_block_mark(chipnr, blk_num);
}

/**
 * nand_block_markbad - [MTD Interface] Mark block at the given offset as bad
 * @mtd: MTD device structure
 * @ofs: offset relative to mtd start
 */
static int sunxi_nand_block_markbad(struct mtd_info *mtd, loff_t ofs)
{
	int ret;

#ifdef SUNXI_NAND_MTD_DEBUG
	tick_printf("line: %d, %s\n", __LINE__,  __func__);
#endif

	ret = sunxi_nand_block_isbad(mtd, ofs);
	if (ret) {
		/* If it was bad already, return success and do nothing */
		if (ret > 0)
			return 0;
		return ret;
	}

	return _sunxi_nand_block_markbad(mtd, ofs);
}

/**
 *
 * use one plane R/W/E
 */
static void sunxi_nand_init_default(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	uint chipcnt, blkcnt_perchip, pgcnt_perblk;
	uint seccnt_perpg, oobsize, pgsize, blksize;
	uint64_t chipsize, align_chipsize;
	/*
	 * int i;
	 * uint bb_cnt, chipnr;
	 * int ret;
	 */

	chipcnt = g_nand_para->ChipCnt;
	blkcnt_perchip = g_nand_para->BlkCntPerDie;
	pgcnt_perblk = g_nand_para->PageCntPerPhyBlk;
	seccnt_perpg = g_nand_para->SectorCntPerPage;
	oobsize = 16;

	pgsize = seccnt_perpg << 9;
	blksize = pgsize * pgcnt_perblk;
	chipsize = ((uint64)blksize) * blkcnt_perchip;

	if (sunxi_get_mtd_ubi_mode_status()) {
		tick_printf("chipcnt = %d\n", chipcnt);
		tick_printf("blkcnt_perchip = %d\n", blkcnt_perchip);
		tick_printf("pgcnt_perblk = %d\n", pgcnt_perblk);
		tick_printf("seccnt_perpg = %d\n", seccnt_perpg);
		tick_printf("oobsize = %d\n", oobsize);
		tick_printf("chipsize = %d MiB\n",
			(uint32_t)(chipsize >> (9 + 11)));
		tick_printf("blksize = %d MiB (%d secs)\n",
			blksize >> (9 + 11), blksize >> 9);

		/*
		 * for (chipnr = 0, bb_cnt = 0; chipnr < chipcnt; ++chipnr) {
		 *	for (i = 0; i < blkcnt_perchip; ++i) {
		 *		ret = sunxi_nand_bad_block_check(chipnr, i);
		 *		if (ret){
		 *			tick_printf("Bad  blk : "
		 *				"chip %d, %d X, ret = 0x%0x\n",
		 *				chipnr, i, ret);
		 *			bb_cnt++;
		 *		}
		 *	}
		 * }
		 * tick_printf("Bad blk cnt = %d\n", bb_cnt);
		 * tick_printf("Good blk cnt = %d\n",
		 * blkcnt_perchip * chipcnt - bb_cnt);
		 * tick_printf("Total good blk size = %d MiB\n",
		 *	seccnt_perpg * pgcnt_perblk *
		 *	(blkcnt_perchip - bb_cnt) * chipcnt / 2048);
		 */
	}

	chip->numchips = chipcnt;

	/* use extern blocks */
	align_chipsize = align_data(chipsize);
	chip->chipsize = chipsize > align_chipsize ?
		align_chipsize << 1 : align_chipsize;

	chip->page_shift = ffs(pgsize) - 1;
	chip->phys_erase_shift = ffs(blksize) - 1;

	if (chip->chipsize & 0xffffffff)
		chip->chip_shift = ffs((unsigned)chip->chipsize) - 1;
	else {
		chip->chip_shift = ffs((unsigned)(chip->chipsize >> 32));
		chip->chip_shift += 32 - 1;
	}
	chip->pagemask = (chip->chipsize >> chip->page_shift) - 1;
	chip->pagebuf = -1;

	/*
	 * tick_printf("super_chip->chipsize = 0x%08x,%08x ; %0dMiB\n",
	 *	(uint)((chip->chipsize >> 32) & 0xffffffff),
	 *	(uint)(chip->chipsize & 0xffffffff), chip->chipsize >> 20);
	 */

	tick_printf("chip_shift = %d\n", chip->chip_shift);

	mtd->size = chipsize * chip->numchips;
	mtd->erasesize = blksize;
	mtd->writesize = pgsize;
	mtd->oobsize = oobsize;
	mtd->oobavail = mtd->oobsize;
}

static void sunxi_nand_init_superblk(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	uint chipcnt, blkcnt_perchip, pgcnt_perblk;
	uint seccnt_perpg, oobsize, pgsize, blksize;
	uint64_t chipsize, align_chipsize;
	/*
	 * uint i;
	 * uint bb_cnt, chipnr;
	 * int ret;
	 */

	chipcnt = g_nssi->super_chip_cnt;
	blkcnt_perchip = g_nssi->nsci->blk_cnt_per_super_chip;
	pgcnt_perblk = g_nssi->nsci->page_cnt_per_super_blk;
	seccnt_perpg = g_nssi->nsci->sector_cnt_per_super_page;
	oobsize = g_nssi->nsci->spare_bytes;

	pgsize = seccnt_perpg << 9;
	blksize = pgsize * pgcnt_perblk;
	chipsize = ((uint64)blksize) * blkcnt_perchip;

	if (sunxi_get_mtd_ubi_mode_status()) {
		tick_printf("line: %d, %s\n", __LINE__,  __func__);
		tick_printf("super_block_nums = %0d\n",
			g_nssi->super_block_nums);
		tick_printf("support_two_plane = %0d\n",
			g_nssi->nsci->two_plane);
		tick_printf("support_v_interleave = %0d\n",
			g_nssi->nsci->vertical_interleave);
		tick_printf("support_dual_channel  = %0d\n",
			g_nssi->nsci->dual_channel);

		tick_printf("super_chip_cnt = %0d\n", chipcnt);
		tick_printf("blk_cnt_per_super_chip  = %0d\n", blkcnt_perchip);
		tick_printf("page_cnt_per_super_blk  = %0d\n", pgcnt_perblk);
		tick_printf("sector_cnt_per_super_page	= %0d\n", seccnt_perpg);
		tick_printf("spare_bytes  = %0d\n", oobsize);

		tick_printf("super chip size = %d MiB\n",
			(uint32_t)(chipsize >> (9 + 11)));
		tick_printf("super blk size = %d MiB (%d secs)\n",
			blksize >> (9 + 11), blksize >> 9);

		/*
		 * search bad blks, cal capacity.
		 * for (chipnr = 0, bb_cnt = 0; chipnr < chipcnt; ++chipnr) {
		 *	for (i = 0; i < blkcnt_perchip; ++i) {
		 *		ret = sunxi_nand_bad_block_check(chipnr, i);
		 *		if (ret){
		 *			tick_printf("Bad  blk :");
		 *			tick_printf("chip %d, %d X\n",
		 *				chipnr, i);
		 *			bb_cnt++;
		 *		}
		 *	}
		 * }
		 * tick_printf("Bad blk cnt = %d\n", bb_cnt);
		 * tick_printf("Good blk cnt = %d\n",
		 * blkcnt_perchip * chipcnt - bb_cnt);
		 * tick_printf("Total good blk size = %d MiB\n",
		 *	seccnt_perpg * pgcnt_perblk *
		 *	(blkcnt_perchip - bb_cnt) * chipcnt / 2048);
		 */
	}

	chip->numchips = chipcnt;

	/* use extern blocks */
	align_chipsize = align_data(chipsize);
	chip->chipsize = chipsize > align_chipsize ?
		align_chipsize << 1 : align_chipsize;

	chip->page_shift = ffs(pgsize) - 1;
	chip->phys_erase_shift = ffs(blksize) - 1;

	if (chip->chipsize & 0xffffffff)
		chip->chip_shift = ffs((unsigned)chip->chipsize) - 1;
	else {
		chip->chip_shift = ffs((unsigned)(chip->chipsize >> 32));
		chip->chip_shift += 32 - 1;
	}
	chip->pagemask = (chip->chipsize >> chip->page_shift) - 1;
	chip->pagebuf = -1;

	/*
	 * tick_printf("super_chip->chipsize = 0x%08x,%08x ; %0dMiB\n",
	 *	(uint)((chip->chipsize >> 32) & 0xffffffff),
	 *	(uint)(chip->chipsize & 0xffffffff), chip->chipsize >> 20);
	 */

	tick_printf("super_chip->chip_shift = %d\n", chip->chip_shift);

	mtd->size = chipsize * chip->numchips;
	mtd->erasesize = blksize;
	mtd->writesize = pgsize;
	mtd->oobsize = oobsize;
	mtd->oobavail = mtd->oobsize;
}

uint64_t get_mtd_size(void)
{
	struct mtd_info *mtd = &nand_info[0];

	if (mtd)
		return mtd->size;
	else
		return 0;
}

uint32_t get_mtd_blksize(void)
{
	struct mtd_info *mtd = &nand_info[0];

	if (mtd)
		return mtd->erasesize;
	else
		return 0;
}

uint32_t get_mtd_pgsize(void)
{
	struct mtd_info *mtd = &nand_info[0];

	if (mtd)
		return mtd->writesize;
	else
		return 0;
}

int sunxi_mtd_read(loff_t from, size_t len, size_t *retlen, u_char *buf)
{
	struct mtd_info *mtd = &nand_info[0];

	return mtd_read(mtd, from, len, retlen, buf);
}

int sunxi_mtd_write(loff_t to, size_t len, size_t *retlen, u_char *buf)
{
	struct mtd_info *mtd = &nand_info[0];

	return mtd_write(mtd, to, len, retlen, buf);
}

int sunxi_nand_mtd_init(void)
{
	int workmode;
	int storage_type;
	int local_init_nand_flag = 0;

	struct mtd_info *mtd = &nand_info[0];
	struct nand_chip *chip = &nand_chip;

	workmode = uboot_spare_head.boot_data.work_mode;
	storage_type = uboot_spare_head.boot_data.storage_type;

	if ((storage_type == STORAGE_NAND) &&
		((workmode == WORK_MODE_BOOT)
		|| (workmode == WORK_MODE_SPRITE_RECOVERY)
		|| (workmode & WORK_MODE_PRODUCT))) {

		/* sunxi_nand_uboot_init(0); */

		if (g_nssi == NULL) {
			if (sunxi_nand_phy_init()) {
				tick_printf("Err: line: %d, %s\n",
					__LINE__,  __func__);
				return -1;
			}
			local_init_nand_flag = 1;
		}

		memset(mtd, 0, sizeof(struct mtd_info));
		memset(chip, 0, sizeof(struct nand_chip));
		mtd->priv = chip;
		nand_get_param(g_nand_para);

		tick_printf("line: %d, %s\n", __LINE__,  __func__);
		tick_printf("DDRType = %d\n", g_nand_para->DDRType);

		/*
		 * tick_printf("BankCntPerChip = %d\n",
		 * g_nand_para->BankCntPerChip);
		 * tick_printf("PlaneCntPerDie = %d\n",
		 * g_nand_para->PlaneCntPerDie);
		 */

		tick_printf("The nand flash ID :");
		tick_printf("%0x %0x %0x %0x %0x %0x %0x %0x\n",
			g_nand_para->NandChipId[0], g_nand_para->NandChipId[1],
			g_nand_para->NandChipId[2], g_nand_para->NandChipId[3],
			g_nand_para->NandChipId[4], g_nand_para->NandChipId[5],
			g_nand_para->NandChipId[6], g_nand_para->NandChipId[7]);

		tick_printf("Ecc mode = %d\n",
			g_nand_para->EccMode < sizeof(ecc_mode_hw) ?
			ecc_mode_hw[g_nand_para->EccMode] : 0);
		tick_printf("Ecc limit = %d\n",
			g_nand_para->EccMode < sizeof(ecc_mode_hw) ?
			ecc_limit[g_nand_para->EccMode] : 0);
		tick_printf("ReadRetryType = 0x%x\n",
			g_nand_para->ReadRetryType);

		if (g_fix_sunxi_nand_default)
			sunxi_nand_init_default(mtd);
		else
			sunxi_nand_init_superblk(mtd);

		mtd->type = MTD_NANDFLASH;
		mtd->flags = MTD_CAP_NANDFLASH;
		mtd->bitflip_threshold = 0;
		mtd->ecc_strength =
			g_nand_para->EccMode < sizeof(ecc_mode_hw) ?
			ecc_mode_hw[g_nand_para->EccMode] : 0;
		mtd->numeraseregions = 0;
		mtd->subpage_sft = 0;

		mtd->_erase = sunxi_nand_erase;
		mtd->_point = NULL;
		mtd->_unpoint = NULL;
		mtd->_read = sunxi_nand_read;
		mtd->_write = sunxi_nand_write;
		mtd->_read_oob = sunxi_nand_read_oob;
		mtd->_write_oob = sunxi_nand_write_oob;
		mtd->_sync = sunxi_nand_sync;
		mtd->_lock = NULL;
		mtd->_unlock = NULL;
		mtd->_block_isbad = sunxi_nand_block_isbad;
		mtd->_block_markbad = sunxi_nand_block_markbad;
		mtd->owner = THIS_MODULE;

		nand_register(0);

		if (local_init_nand_flag)
			NAND_PhyExit();

		tick_printf("sunxi mtd regist ok.\n");
	}

	tick_printf("sunxi_nand_mtd_init end.\n");

	return 0;
}
#endif
