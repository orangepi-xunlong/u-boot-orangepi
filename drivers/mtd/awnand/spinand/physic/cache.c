// SPDX-License-Identifier: GPL-2.0

#define pr_fmt(fmt) "sunxi-spinand-phy: " fmt

#include <common.h>
#include <linux/kernel.h>
#include <linux/mtd/aw-spinand.h>
#include <spi-mem.h>
#include <linux/mtd/spinand.h>

#include "physic.h"
#if IS_ENABLED(CONFIG_AW_SPINAND_ENABLE_PHY_CRC16)
#include "crc16.h"
#endif

#define INVALID_CACHE ((unsigned int)(-1))
static void update_cache_info(struct aw_spinand_cache *cache,
		struct aw_spinand_chip_request *req)
{
	if (req && (req->databuf || req->oobbuf)) {
		cache->block = req->block;
		cache->page = req->page;
		cache->area = INVALID_CACHE_ALL_AREA;
		if (req->databuf)
			cache->area = VALID_CACHE_DATA;
		if (req->oobbuf)
			cache->area = VALID_CACHE_OOB;
		pr_debug("update cache: phy blk %u page %u with%s%s\n",
				req->block, req->page,
				req->databuf ? " data" : "",
				req->oobbuf ? " oob" : "");
	} else {
		cache->block = INVALID_CACHE;
		cache->page = INVALID_CACHE;
		cache->area = INVALID_CACHE_ALL_AREA;
		pr_debug("clear cache\n");
	}
}

/* the request must bases on single physical page/block */
static int aw_spinand_cache_copy_to_cache(struct aw_spinand_chip *chip,
		struct aw_spinand_chip_request *req)
{
	unsigned int len;
	struct aw_spinand_cache *cache = chip->cache;

	memset(cache->databuf, 0xFF, cache->data_maxlen);
	if (req->databuf && req->datalen) {
		len = min(req->datalen, cache->data_maxlen - req->pageoff);
		memcpy(cache->databuf + req->pageoff, req->databuf, len);
	}

	memset(cache->oobbuf, 0xFF, cache->oob_maxlen);
	if (req->oobbuf && req->ooblen) {
		if (req->mode != AW_SPINAND_MTD_OPS_RAW) {
			int ret;
			struct aw_spinand_ecc *ecc = chip->ecc;
			struct aw_spinand_info *info = chip->info;
			struct aw_spinand_phy_info *pinfo = info->phy_info;

			len = min3(req->ooblen, cache->oob_maxlen,
					(unsigned int)AW_OOB_SIZE_PER_PHY_PAGE);
			ret = ecc->copy_to_oob(pinfo->EccProtectedType,
					cache->oobbuf, req->oobbuf, len);
			if (unlikely(ret))
				return ret;
		} else {
			len = min3(req->ooblen, cache->oob_maxlen,
					(unsigned int)AW_OOB_SIZE_PER_PHY_PAGE);
			memcpy(cache->oobbuf, req->oobbuf, len);

		}
	}

	/* we must update cache information when update cache buffer */
	update_cache_info(cache, req);
	return 0;
}

/* the request must bases on single physical page/block */
static int aw_spinand_cache_copy_from_cache(struct aw_spinand_chip *chip,
		struct aw_spinand_chip_request *req)
{
	unsigned int len;
	struct aw_spinand_cache *cache = chip->cache;

	WARN_ON(!cache->match_cache(chip, req));

	if (req->datalen && req->databuf) {
		len = min(req->datalen, cache->data_maxlen - req->pageoff);
		memcpy(req->databuf, cache->databuf + req->pageoff, len);
	}

	if (req->oobbuf && req->ooblen) {
		if (req->mode != AW_SPINAND_MTD_OPS_RAW) {
			int ret;
			struct aw_spinand_ecc *ecc = chip->ecc;
			struct aw_spinand_info *info = chip->info;
			struct aw_spinand_phy_info *pinfo = info->phy_info;

			len = min3(req->ooblen, cache->oob_maxlen,
					(unsigned int)AW_OOB_SIZE_PER_PHY_PAGE);
			ret = ecc->copy_from_oob(pinfo->EccProtectedType,
					req->oobbuf, cache->oobbuf, len);
			if (unlikely(ret))
				return ret;
		} else {
			len = min3(req->ooblen, cache->oob_maxlen,
					(unsigned int)AW_OOB_SIZE_PER_PHY_PAGE);
			memcpy(req->oobbuf, cache->oobbuf, len);

		}
	}

	return 0;
}

static bool aw_spinand_cache_match_cache(struct aw_spinand_chip *chip,
		struct aw_spinand_chip_request *req)
{
	struct aw_spinand_cache *cache = chip->cache;

	if (cache->block == INVALID_CACHE || cache->page == INVALID_CACHE)
		return false;
	if (req->block != cache->block || req->page != cache->page)
		return false;
	if (req->databuf && cache->area != VALID_CACHE_DATA)
		return false;
	if (req->oobbuf && cache->area != VALID_CACHE_OOB)
		return false;
	return true;
}

static int aw_spinand_cahce_write_to_cache_do(struct aw_spinand_chip *chip,
		void *buf, unsigned int len, unsigned int column)
{
	int ret;
	struct spi_mem_op op = SPINAND_PROG_LOAD(false, column, NULL, 0);

	op.data.buswidth = chip->tx_bit;
	if (chip->tx_bit == SPI_NBITS_QUAD)
		op.cmd.opcode = SPI_NAND_PP_X4;
	else
		op.cmd.opcode = SPI_NAND_PP;

	while (len) {
		op.data.buf.out = buf;
		op.data.nbytes = len;

		ret = spi_mem_adjust_op_size(chip->slave, &op);
		if (ret)
			return ret;

		ret = spi_mem_exec_op(chip->slave, &op);
		if (ret)
			return ret;

		buf += op.data.nbytes;
		len -= op.data.nbytes;
		op.addr.val += op.data.nbytes;
	}
	return 0;
}

static int write_to_cache_half_page_twice(struct aw_spinand_chip *chip,
		struct aw_spinand_chip_request *req)
{
	int ret;
	struct aw_spinand_info *info = chip->info;
	struct aw_spinand_cache *cache = chip->cache;
	unsigned int column = 0, half_page_size;

	half_page_size = info->phy_page_size(chip) >> 1;
	/* the first half page */
	ret = aw_spinand_cahce_write_to_cache_do(chip, cache->databuf,
			half_page_size, column);
	if (ret)
		return ret;

	/* the secend half page */
	column += half_page_size;
	return aw_spinand_cahce_write_to_cache_do(chip,
			cache->databuf + half_page_size,
			half_page_size + info->phy_oob_size(chip), column);
}

static int write_to_cache_whole_page_once(struct aw_spinand_chip *chip,
		struct aw_spinand_chip_request *req)
{
	struct aw_spinand_info *info = chip->info;
	struct aw_spinand_cache *cache = chip->cache;

	/* write the whole page */
	return aw_spinand_cahce_write_to_cache_do(chip, cache->databuf,
			info->phy_page_size(chip) + info->phy_oob_size(chip),
			0);
}

#if IS_ENABLED(CONFIG_AW_SPINAND_ENABLE_PHY_CRC16)
static int aw_spinand_chip_verify_crc(struct aw_spinand_chip *chip,
		struct aw_spinand_chip_request *req)
{
	struct aw_spinand_cache *cache = chip->cache;
	__u16 crc = 0, crc_verify = 0;

	if (!req->databuf || !req->oobbuf)
		return 0;

	crc_verify = ((unsigned char *)req->oobbuf)[AW_CRC16_OOB_OFFSET] << 8;
	crc_verify += ((unsigned char *)req->oobbuf)[AW_CRC16_OOB_OFFSET + 1];

	crc = crc16(0, cache->databuf, cache->data_maxlen);

	/*
	 * If databuf are all 0xFF, the CRC16 value will be 0xA041.
	 * In this case, we should return directly
	 */
	if (crc_verify == (__u16)0xFFFF && crc == (__u16)0xA041)
		return 0;

	/*
	 * CRC16 is used for debug
	 * So, we just print warning, do not do anything now
	 */
	if (crc != crc_verify) {
		pr_warn("phy blk %u page %u crc16 check failed: want 0x%04x get 0x%04x\n",
				req->block, req->page, crc_verify, crc);
		aw_spinand_hexdump(KERN_WARNING, "oob: ", req->oobbuf,
				req->ooblen);
		aw_spinand_hexdump(KERN_WARNING, "data: ", cache->databuf,
				cache->data_maxlen);
		return -EINVAL;
	}
	pr_debug("phy blk %u page %u crc16 check pass\n", req->block,
		   req->page);
	return 0;
}

static int aw_spinand_chip_update_crc(struct aw_spinand_chip *chip,
		struct aw_spinand_chip_request *req)
{
	struct aw_spinand_info *info = chip->info;
	__u16 crc = 0;

	crc = crc16(0, req->databuf, info->phy_page_size(chip));

	((char *)req->oobbuf)[AW_CRC16_OOB_OFFSET] = crc >> 8;
	((char *)req->oobbuf)[AW_CRC16_OOB_OFFSET + 1] = crc & 0xFF;

	pr_debug("record blk %u page %u crc16 0x%04x\n", req->block, req->page, crc);
	return 0;
}
#endif

/*
 * 3 step:
 *  a) copy data from req to cache->databuf/oobbuf
 *  b) update cache->block/page
 *  c) send write cache command to spinand
 */
static int aw_spinand_cache_write_to_cache(struct aw_spinand_chip *chip,
		struct aw_spinand_chip_request *req)
{
	int ret;
	const char *manufacture;
	struct aw_spinand_info *info = chip->info;
	struct aw_spinand_cache *cache = chip->cache;
#if IS_ENABLED(CONFIG_AW_SPINAND_ENABLE_PHY_CRC16)
	unsigned char oob[AW_OOB_SIZE_PER_PHY_PAGE] = {0xFF};
#endif

#if IS_ENABLED(CONFIG_AW_SPINAND_ENABLE_PHY_CRC16)
	if (!req->oobbuf) {
		req->oobbuf = oob;
		req->ooblen = AW_OOB_SIZE_PER_PHY_PAGE;
	}
	aw_spinand_chip_update_crc(chip, req);
#endif

	ret = cache->copy_to_cache(chip, req);
	if (ret)
		goto err;

	manufacture = info->manufacture(chip);
	if (!strcmp(manufacture, "Winbond"))
		/*
		 * Winbond spinand has a feature:
		 * Data cache in spinand is divide to 2 parts to inprove speed.
		 * We work with winbond engineers, however, still have no idea
		 * why the spinand will get wrong data, always 2 bytes 0xFF on
		 * the head of the 2rd part on cache.
		 * To fix it, we are recommended to write twice, half of data
		 * each time
		 */
		ret = write_to_cache_half_page_twice(chip, req);
	else
		ret = write_to_cache_whole_page_once(chip, req);
	if (ret)
		goto err;

	/* we must update cache information when update cache buffer */
	update_cache_info(cache, req);
#if IS_ENABLED(CONFIG_AW_SPINAND_ENABLE_PHY_CRC16)
	if (req->oobbuf == oob) {
		req->oobbuf = NULL;
		req->ooblen = req->oobleft = 0;
	}
#endif
	return 0;
err:
	/*
	 * the cache buffer is invalid now as we do not know
	 * what had done to cache buffer
	 */
	update_cache_info(cache, NULL);
#if IS_ENABLED(CONFIG_AW_SPINAND_ENABLE_PHY_CRC16)
	if (req->oobbuf == oob) {
		req->oobbuf = NULL;
		req->ooblen = req->oobleft = 0;
	}
#endif
	return ret;
}

/*
 * 3 step:
 *  a) copy data from req to cache->databuf/oobbuf
 *  b) update cache->block/page
 *  c) send write cache command to spinand
 */
static int aw_spinand_cache_read_from_cache(struct aw_spinand_chip *chip,
		struct aw_spinand_chip_request *req)
{
	unsigned int nbytes = 0;
	void *buf = NULL;
	int column = 0, ret;
	struct aw_spinand_info *info = chip->info;
	struct aw_spinand_cache *cache = chip->cache;
	struct spi_mem_op op = SPINAND_PAGE_READ_FROM_CACHE_OP(true, 0, 0, NULL, 0);
#if IS_ENABLED(CONFIG_AW_SPINAND_ENABLE_PHY_CRC16)
	unsigned char oob[AW_OOB_SIZE_PER_PHY_PAGE] = {0xFF};
#endif

#if IS_ENABLED(CONFIG_AW_SPINAND_ENABLE_PHY_CRC16)
	if (!req->oobbuf) {
		 req->oobbuf = oob;
		 req->ooblen = AW_OOB_SIZE_PER_PHY_PAGE;
	}
#endif

	if (req->datalen) {
		buf = cache->databuf;
		nbytes = cache->data_maxlen;
		column = 0;
	}

	if (req->ooblen) {
		nbytes += cache->oob_maxlen;
		/* just oob without data */
		if (!buf) {
			buf = cache->oobbuf;
			column = info->phy_page_size(chip);
		}
	}

	op.addr.val = column;
	op.data.buswidth = chip->rx_bit;
	if (chip->rx_bit == SPI_NBITS_QUAD)
		op.cmd.opcode = SPI_NAND_READ_X4;
	else if (chip->rx_bit == SPI_NBITS_DUAL)
		op.cmd.opcode = SPI_NAND_READ_X2;
	else
		op.cmd.opcode = SPI_NAND_READ_X1;

	while (nbytes) {
		op.data.buf.in = buf;
		op.data.nbytes = nbytes;

		ret = spi_mem_adjust_op_size(chip->slave, &op);
		if (ret)
			goto err;

		ret = spi_mem_exec_op(chip->slave, &op);
		if (ret)
			goto err;

		buf += op.data.nbytes;
		nbytes -= op.data.nbytes;
		op.addr.val += op.data.nbytes;

	}

	/* we must update cache information when update cache buffer */
	update_cache_info(cache, req);

	/* no need to update cache information as no change happened */
	ret = cache->copy_from_cache(chip, req);
	if (unlikely(ret))
		return ret;

#if IS_ENABLED(CONFIG_AW_SPINAND_ENABLE_PHY_CRC16)
	aw_spinand_chip_verify_crc(chip, req);
	if (req->oobbuf == oob) {
		 req->oobbuf = NULL;
		 req->ooblen = req->oobleft = 0;
	}
#endif

	return 0;

err:
	/*
	 * the cache buffer is invalid now as we do not know
	 * what spi system had done to cache buffer
	 */
	update_cache_info(cache, NULL);
	return ret;
}

/* see what these funcions do on somewhere defined struct aw_spinand_cache */
struct aw_spinand_cache aw_spinand_cache = {
	.match_cache = aw_spinand_cache_match_cache,
	.copy_to_cache = aw_spinand_cache_copy_to_cache,
	.copy_from_cache = aw_spinand_cache_copy_from_cache,
	.read_from_cache = aw_spinand_cache_read_from_cache,
	.write_to_cache = aw_spinand_cache_write_to_cache,
};

int aw_spinand_chip_cache_init(struct aw_spinand_chip *chip)
{
	struct aw_spinand_info *info = chip->info;
	struct aw_spinand_cache *cache = &aw_spinand_cache;

	/*
	 * this cache is only used for single physical page,
	 * no need to allocate super page for multiplane.
	 * If multiplane enabled, the read/write operation will be
	 * cut into 2 single page.
	 */
	cache->data_maxlen = info->phy_page_size(chip);
	cache->oob_maxlen = info->phy_oob_size(chip);
	cache->databuf = kzalloc(cache->data_maxlen + cache->oob_maxlen,
			GFP_KERNEL);
	if (!cache->databuf)
		goto err;

	cache->oobbuf = cache->databuf + cache->data_maxlen;
	chip->cache = cache;
	return 0;
err:
	pr_err("init cache failed\n");
	return -ENOMEM;
}

void aw_spinand_chip_cache_exit(struct aw_spinand_chip *chip)
{
	struct aw_spinand_cache *cache = chip->cache;

	kfree(cache->databuf);
	cache->databuf = cache->oobbuf = NULL;
	chip->cache = NULL;
}

