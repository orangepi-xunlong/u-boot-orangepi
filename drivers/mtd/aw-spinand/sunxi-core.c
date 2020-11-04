// SPDX-License-Identifier: GPL-2.0

#define pr_fmt(fmt) "sunxi-spinand: " fmt

#include <linux/kernel.h>
#include <linux/mtd/aw-spinand.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/mtd.h>
#include <linux/printk.h>
#include <linux/libfdt.h>
#include <common.h>
#include <spi-mem.h>
#include <spi.h>
#include <fdt_support.h>

#include "sunxi-spinand.h"

static struct aw_spinand *g_spinand;

static void aw_spinand_cleanup(struct aw_spinand *spinand)
{
	aw_spinand_chip_exit(&spinand->chip);
}

static int aw_spinand_erase(struct mtd_info *mtd, struct erase_info *einfo)
{
	unsigned int len;
	int ret;
	struct aw_spinand_chip_request req = {0};
	struct aw_spinand *spinand = mtd_to_spinand(mtd);
	struct aw_spinand_chip *chip = spinand_to_chip(spinand);
	struct aw_spinand_chip_ops *ops = chip->ops;
	struct aw_spinand_info *info = chip->info;

	pr_debug("calling erase\n");

	len = (unsigned int)einfo->len;
	ret = addr_to_req(chip, &req, einfo->addr);
	if (ret)
		return ret;

	while (len) {
		ret = ops->erase_block(chip, &req);
		if (ret) {
			einfo->state = MTD_ERASE_FAILED;
			einfo->fail_addr = einfo->addr;
			pr_err("erase block %u in addr 0x%x failed: %d\n",
					req.block, (unsigned int)einfo->addr,
					ret);
			return ret;
		}

		req.block++;
		len -= info->block_size(chip);
	}

	einfo->state = MTD_ERASE_DONE;
	return 0;
}

static void aw_spinand_init_mtd_info(struct aw_spinand_chip *chip,
		struct mtd_info *mtd)
{
	struct aw_spinand_info *info = chip->info;

	mtd->type = MTD_NANDFLASH;
	mtd->flags = MTD_CAP_NANDFLASH;
	mtd->owner = THIS_MODULE;
	mtd->erasesize = info->block_size(chip);
	mtd->writesize = info->page_size(chip);
	mtd->oobsize = info->oob_size(chip);
#if SIMULATE_MULTIPLANE
	mtd->oobavail = AW_OOB_SIZE_PER_PHY_PAGE * 2;
	mtd->subpage_sft = 1;
#else
	mtd->oobavail = AW_OOB_SIZE_PER_PHY_PAGE;
	mtd->subpage_sft = 0;
#endif
	mtd->writebufsize = info->page_size(chip);
	mtd->size = info->total_size(chip);
	mtd->name = "nand0";
	mtd->ecc_strength = ECC_ERR;
	mtd->bitflip_threshold = ECC_LIMIT;
}

static inline void aw_spinand_req_init(struct aw_spinand *spinand,
		loff_t offs, struct mtd_oob_ops *mtd_ops,
		struct aw_spinand_chip_request *req)
{
	struct aw_spinand_chip *chip = spinand_to_chip(spinand);
	struct aw_spinand_info *info = chip->info;
	struct mtd_info *mtd = spinand_to_mtd(spinand);

	addr_to_req(chip, req, offs);
	req->databuf = mtd_ops->datbuf;
	req->oobbuf = mtd_ops->oobbuf;
	req->pageoff = offs & (info->page_size(chip) - 1);
	/* read size once */
	req->ooblen = min_t(unsigned int, mtd_ops->ooblen, mtd->oobavail);
	req->datalen = min(info->page_size(chip) - req->pageoff,
			mtd_ops->len);
	/* the total size to read */
	req->dataleft = mtd_ops->len;
	req->oobleft = mtd_ops->ooblen;
}

static inline void aw_spinand_req_next(struct aw_spinand *spinand,
		struct aw_spinand_chip_request *req)
{
	struct aw_spinand_chip *chip = spinand_to_chip(spinand);
	struct aw_spinand_info *info = chip->info;
	struct mtd_info *mtd = spinand_to_mtd(spinand);
	unsigned int pages_per_blk;
	const char *str;

	pages_per_blk = info->block_size(chip) >> spinand->page_shift;
	req->page++;
	if (req->page >= pages_per_blk) {
		req->page = 0;
		req->block++;
	}

	if (req->dataleft < req->datalen) {
		str = "[phy]request: dataleft < datalen";
		goto bug;
	}

	if (req->oobleft < req->ooblen) {
		str = "[phy]request: oobleft < ooblen";
		goto bug;
	}

	req->dataleft -= req->datalen;
	req->databuf += req->datalen;
	req->oobleft -= req->ooblen;
	req->oobbuf += req->ooblen;
	req->pageoff = 0;
	req->datalen = min(info->page_size(chip), req->dataleft);
	req->ooblen = min_t(unsigned int, req->oobleft, mtd->oobavail);
	return;

bug:
	aw_spinand_reqdump(pr_err, str, req);
	BUG_ON(1);
	return;
}

static inline bool aw_spinand_req_end(struct aw_spinand *spinand,
		struct aw_spinand_chip_request *req)
{
	if (req->dataleft || req->oobleft)
		return false;
	return true;
}

/**
 * aw_spinand_for_each_req - Iterate over all NAND pages contained in an
 *				MTD I/O request
 * @spinand: SPINAND device
 * @start: start address to read/write from
 * @mtd_ops: MTD I/O request
 * @req: sunxi chip request
 *
 * Should be used for iterate over pages that are contained in an MTD request
 */
#define aw_spinand_for_each_req(spinand, start, mtd_ops, req)	\
	for (aw_spinand_req_init(spinand, start, mtd_ops, req);	\
		!aw_spinand_req_end(spinand, req);			\
		aw_spinand_req_next(spinand, req))

static int aw_spinand_read_oob(struct mtd_info *mtd, loff_t from,
		struct mtd_oob_ops *ops)
{
	int ret = 0;
	unsigned int max_bitflips = 0;
	struct aw_spinand_chip_request req = {0};
	struct aw_spinand *spinand = mtd_to_spinand(mtd);
	struct aw_spinand_chip *chip = spinand_to_chip(spinand);
	struct aw_spinand_chip_ops *chip_ops = chip->ops;
	bool ecc_failed = false;

	if (from < 0 || from >= mtd->size || ops->len > mtd->size - from)
		return -EINVAL;
	if (!ops->len)
		return 0;

	pr_debug("calling read with oob: from 0x%llx datalen %d ooblen %d\n",
			from, ops->len, ops->ooblen);

	aw_spinand_for_each_req(spinand, from, ops, &req) {
		aw_spinand_reqdump(pr_debug, "do super read", &req);

		ret = chip_ops->read_page(chip, &req);
		if (ret < 0) {
			pr_err("read single page failed: %d\n", ret);
			break;
		} else if (ret == ECC_LIMIT) {
			ret = mtd->bitflip_threshold;
			mtd->ecc_stats.corrected += ret;
			max_bitflips = max_t(unsigned int, max_bitflips, ret);
			pr_debug("ecc limit: block: %u page: %u\n",
					req.block, req.page);
		} else if (ret == ECC_ERR) {
			ecc_failed = true;
			mtd->ecc_stats.failed++;
			pr_err("ecc err: block: %u page: %u\n",
					req.block, req.page);
		}

		ret = 0;
		ops->retlen += req.datalen;
		ops->oobretlen += req.ooblen;
	}

	if (ecc_failed && !ret)
		ret = -EBADMSG;

	pr_debug("exitng read with oob\n");
	return ret ? ret : max_bitflips;
}

static int aw_spinand_read(struct mtd_info *mtd,
	loff_t from, size_t len, size_t *retlen, uint8_t *buf)
{
	struct mtd_oob_ops ops = {0};
	int ret;

	pr_debug("calling read\n");

	ops.len = len;
	ops.datbuf = buf;
	ops.oobbuf = NULL;
	ops.ooblen = ops.ooboffs = ops.oobretlen = 0;
	ret = aw_spinand_read_oob(mtd, from, &ops);
	*retlen = ops.retlen;
	pr_debug("exiting read\n");
	return ret;
}

static int aw_spinand_write_oob(struct mtd_info *mtd, loff_t to,
		struct mtd_oob_ops *ops)
{
	int ret = 0;
	struct aw_spinand_chip_request req = {0};
	struct aw_spinand *spinand = mtd_to_spinand(mtd);
	struct aw_spinand_chip *chip = spinand_to_chip(spinand);
	struct aw_spinand_info *info = chip->info;
	struct aw_spinand_chip_ops *chip_ops = chip->ops;

	if (to < 0 || to >= mtd->size || ops->len > mtd->size - to)
		return -EOVERFLOW;
	if (!ops->len)
		return 0;
	/*
	 * if enable SIMULATE_MULTIPLANE, will eanble subpage too.
	 * This means that ubi will write header to physic block page.
	 * So, we should check alignment for physic page rather super page.
	 */
	if (ops->len & (info->phy_page_size(chip) - 1))
		return -EINVAL;

	pr_debug("calling write with oob: to 0x%llx datalen %d ooblen %d\n",
			to, ops->len, ops->ooblen);

	aw_spinand_for_each_req(spinand, to, ops, &req) {
		aw_spinand_reqdump(pr_debug, "do super write", &req);

		ret = chip_ops->write_page(chip, &req);
		if (ret < 0) {
			pr_err("write single page failed: block %d, page %d, ret %d\n",
					req.block, req.page, ret);
			break;
		}

		ops->retlen += req.datalen;
		ops->oobretlen += req.ooblen;
	}

	pr_debug("exiting write with oob\n");
	return ret;
}

static int aw_spinand_write(struct mtd_info *mtd, loff_t to,
	size_t len, size_t *retlen, const u_char *buf)
{
	struct mtd_oob_ops ops = {0};
	int ret;

	pr_debug("calling write\n");

	ops.len = len;
	ops.datbuf = (uint8_t *)buf;
	ops.oobbuf = NULL;
	ops.ooblen = ops.ooboffs = ops.oobretlen = 0;
	ret = aw_spinand_write_oob(mtd, to, &ops);
	*retlen = ops.retlen;

	pr_debug("exiting write\n");
	return ret;
}

static int aw_spinand_block_isbad(struct mtd_info *mtd, loff_t offs)
{
	int ret;
	struct aw_spinand_chip_request req = {0};
	struct aw_spinand *spinand = mtd_to_spinand(mtd);
	struct aw_spinand_chip *chip = spinand_to_chip(spinand);
	struct aw_spinand_chip_ops *ops = chip->ops;

	pr_debug("calling isbad\n");

	ret = addr_to_req(chip, &req, offs);
	if (ret)
		return ret;

	ret = ops->is_bad(chip, &req);
	pr_debug("exting isbad: block %u is %s\n", req.block,
			ret == true ? "bad" : "good");
	return ret;
}

static int aw_spinand_block_markbad(struct mtd_info *mtd, loff_t offs)
{
	int ret;
	struct aw_spinand_chip_request req = {0};
	struct aw_spinand *spinand = mtd_to_spinand(mtd);
	struct aw_spinand_chip *chip = spinand_to_chip(spinand);
	struct aw_spinand_chip_ops *ops = chip->ops;

	pr_debug("calling markbad\n");

	ret = addr_to_req(chip, &req, offs);
	if (ret)
		return ret;

	ret = ops->mark_bad(chip, &req);
	pr_info("exting markbad: mark block %d as bad\n", req.block);
	return ret;
}

/* write when on panic */
static int aw_spinand_panic_write(struct mtd_info *mtd, loff_t to,
		size_t len, size_t *retlen, const u_char *buf)
{
	return 0;
}

static int aw_spinand_mtd_init(struct aw_spinand *spinand)
{
	struct mtd_info *mtd = spinand_to_mtd(spinand);
	struct aw_spinand_chip *chip = &spinand->chip;

	mtd = spinand_to_mtd(spinand);
	mtd_set_of_node(mtd, mtd->dev->node.np);

	aw_spinand_init_mtd_info(chip, mtd);

	mtd->_erase = aw_spinand_erase;
	mtd->_read = aw_spinand_read;
	mtd->_read_oob = aw_spinand_read_oob;
	mtd->_write = aw_spinand_write;
	mtd->_write_oob = aw_spinand_write_oob;
	mtd->_block_isbad = aw_spinand_block_isbad;
	mtd->_block_markbad = aw_spinand_block_markbad;
	mtd->_panic_write = aw_spinand_panic_write;
	return 0;
}

int aw_spinand_probe(struct udevice *dev)
{
	struct aw_spinand *spinand;
	struct aw_spinand_chip *chip;
	struct spi_slave *slave = dev->parent_priv;
	struct mtd_info *mtd = dev->uclass_priv;
	int ret;

	if (g_spinand) {
		pr_info("aw spinand already probe\n");
		return -EBUSY;
	}

	pr_info("AW SPINand MTD Layer Version: %x.%x %x\n",
			AW_MTD_SPINAND_VER_MAIN, AW_MTD_SPINAND_VER_SUB,
			AW_MTD_SPINAND_VER_DATE);

	spinand = malloc(sizeof(*spinand));
	if (!spinand)
		return -ENOMEM;
	memset(spinand, 0, sizeof(*spinand));
	chip = spinand_to_chip(spinand);
	if (!mtd)
		mtd = spinand_to_mtd(spinand);
	mtd->priv = spinand;

	ret = aw_spinand_chip_init(slave, chip);
	if (ret)
		return ret;

	spinand->sector_shift = ffs(chip->info->sector_size(chip)) - 1;
	spinand->page_shift = ffs(chip->info->page_size(chip)) - 1;
	spinand->block_shift = ffs(chip->info->block_size(chip)) - 1;

	ret = aw_spinand_mtd_init(spinand);
	if (ret)
		goto err_spinand_cleanup;

	ret = add_mtd_device(mtd);
	if (ret)
		goto err_spinand_cleanup;

	/* save spinand to global variable, usefull for @sunxi_mtd_#### */
	g_spinand = spinand;
	return 0;

err_spinand_cleanup:
	aw_spinand_cleanup(spinand);
	return ret;
}

void aw_spinand_exit(struct aw_spinand *spinand)
{
	aw_spinand_cleanup(spinand);
	g_spinand = NULL;
}

static const struct udevice_id aw_spinand_ids[] = {
	{ .compatible = "spi-nand" },
	{ /* sentinel */ },
};

U_BOOT_DRIVER(aw_spinand) = {
	.name = "spi_nand",
	.id = UCLASS_MTD,
	.of_match = aw_spinand_ids,
	.priv_auto_alloc_size = sizeof(struct aw_spinand),
	.probe = aw_spinand_probe,
};

inline struct aw_spinand *get_spinand(void)
{
	return g_spinand;
}
