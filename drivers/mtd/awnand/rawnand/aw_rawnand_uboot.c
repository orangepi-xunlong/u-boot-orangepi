/**
 * SPDX-License-Identifier: GPL-2.0+
 * aw_rawnand_uboot.c
 *
 * (C) Copyright 2020 - 2021
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * cuizhikui <cuizhikui@allwinnertech.com>
 *
 */
#include <common.h>
#include <linux/errno.h>
#include <malloc.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/aw-rawnand.h>
#include <fdt_support.h>
#include <linux/mtd/aw-ubi.h>


#define VERSION "v1.28 2021-12-13 16:13"

void DISPLAY_VERSION(void)
{
	printf("awnand-mtd-version:%s\n", VERSION);
}


static int enable_rawnand = -1;


inline bool support_rawnand(void)
{
	int raw_node = 0;

	if (enable_rawnand != -1)
		return enable_rawnand;

	raw_node = fdt_path_offset(working_fdt, "nand0");
	if (raw_node < 0) {
		awrawnand_err("get raw-nand node from fdt failed\n");
		return false;
	}
#ifdef CONFIG_SUNXI_UBIFS
	enable_rawnand = true;
#else
	enable_rawnand = false;
#endif
	return enable_rawnand;
}
EXPORT_SYMBOL_GPL(support_rawnand);

/*usually small capacity nand is slc nand*/
static int get_smallnand_uboot_start_block_num(void)
{
	return UBOOT_START_BLOCK_SMALLNAND;
}

/*usually big capacity nand is mlc nand*/
static int get_bignand_uboot_start_block_num(void)
{
	return UBOOT_START_BLOCK_BIGNAND;
}

void rawnand_uboot_blknum(unsigned int *start, unsigned int *end)
{
	uint32_t uboot_block_size = 0;
	uint32_t uboot_start_block = 0;
	uint32_t uboot_next_block = 0;
	uint32_t page_cnt_per_blk = 0;

	struct aw_nand_chip *chip = get_rawnand();

	page_cnt_per_blk = 1 << chip->pages_per_blk_shift;
	uboot_block_size = chip->erasesize;

	/*small nand:block size < 1MB;  reserve 4M for uboot*/
	if (uboot_block_size <= 0x20000) { //128K
		uboot_start_block = get_smallnand_uboot_start_block_num();
		uboot_next_block = uboot_start_block + 32;
	} else if (uboot_block_size <= 0x40000) { //256k
		uboot_start_block = get_smallnand_uboot_start_block_num();
		uboot_next_block = uboot_start_block + 16;
	} else if (uboot_block_size <= 0x80000) { //512k
		uboot_start_block = get_smallnand_uboot_start_block_num();
		uboot_next_block = uboot_start_block + 8;
	} else if (uboot_block_size <= 0x100000 && page_cnt_per_blk <= 128) { //1M
		uboot_start_block = get_smallnand_uboot_start_block_num();
		uboot_next_block = uboot_start_block + 4;
	}
	/* big nand;  reserve at least 20M for uboot */
	else if (uboot_block_size <= 0x100000 && page_cnt_per_blk > 128) { //BIGNAND 1M
		uboot_start_block = get_bignand_uboot_start_block_num();
		uboot_next_block = uboot_start_block + 20;
	} else if (uboot_block_size <= 0x200000) { //BIGNAND 2M
		uboot_start_block = get_bignand_uboot_start_block_num();
		uboot_next_block = uboot_start_block + 10;
	} else {
		uboot_start_block = get_bignand_uboot_start_block_num();
		uboot_next_block = uboot_start_block + 8;
	}

	if (start) {
		*start = uboot_start_block;
		awrawnand_dbg("uboot_start@%u\n", *start);
	}
	if (end) {
		*end = uboot_next_block;
		awrawnand_dbg("uboot_end@%u\n", *end);
	}

}
EXPORT_SYMBOL_GPL(rawnand_uboot_blknum);

static int rawnand_erase_block(int start, int end)
{
	int ret = 0;
	int page = 0;
	struct aw_nand_chip *chip = get_rawnand();
	struct mtd_info *mtd = awnand_chip_to_mtd(chip);
	int page_in_chip = 0;
	int chipnr = 0;

	awrawnand_info("erase block@%d to block@%d\n", start, end);

#if SIMULATE_MULTIPLANE
	int real_start_blk = start >> 1;
	int real_end_blk = end >> 1;
	page = real_start_blk << chip->simu_pages_per_blk_shift;
	page_in_chip = page & chip->simu_chip_pages_mask;
	chipnr = real_start_blk >> (chip->simu_chip_shift - chip->simu_erase_shift);
#else
	int real_start_blk = start;
	int real_end_blk = end;
	page = real_start_blk << chip->pages_per_blk_shift;
	page_in_chip = page & chip->chip_pages_mask;
	chipnr = real_start_blk >> (chip->chip_shift - chip->erase_shift);
#endif

	printf("erase real block@%d to real block@%d \n", real_start_blk, real_end_blk);
	chip->select_chip(mtd, chipnr);

	int blk = real_start_blk;
	do {
#if SIMULATE_MULTIPLANE
		ret = chip->multi_erase(mtd, page_in_chip);
#else
		ret = chip->erase(mtd, page_in_chip);
#endif
		if (ret) {
			awrawnand_err("erase block@%d fail\n", blk);
		}
		blk++;
#if SIMULATE_MULTIPLANE
		page = blk << chip->simu_pages_per_blk_shift;
		page_in_chip = page & chip->simu_chip_pages_mask;
#else
		page = blk << chip->pages_per_blk_shift;
		page_in_chip = page & chip->chip_pages_mask;
#endif
		if (!page_in_chip) {
			chip->select_chip(mtd, -1);
			chip->select_chip(mtd, chipnr);
		}

	} while (blk < real_end_blk);

	chip->select_chip(mtd, -1);

	return ret;
}

static int rawnand_mtd_erase_boot(void)
{
	unsigned int start, end;

	start = 0;
	rawnand_uboot_blknum(NULL, &end);

	/* [start, end) */
	return rawnand_erase_block(start, end);
}



static int rawnand_mtd_erase_data(void)
{
	unsigned int start, end;
	struct aw_nand_chip *chip = get_rawnand();
	uint64_t total_size = chip->chips * chip->chipsize;

	rawnand_uboot_blknum(NULL, &start);
	start += AW_RAWNAND_RESERVED_PHY_BLK_FOR_SECURE_STORAGE;
	end = total_size >> chip->erase_shift;

	/* [start, end) */
	return rawnand_erase_block(start, end);
}

int rawnand_init_mtd_info(struct ubi_mtd_info *mtd_info)
{
	struct aw_nand_chip *chip = get_rawnand();
	struct mtd_info *mtd = awnand_chip_to_mtd(chip);
	int i;

	mtd_info->last_partno = -1;
	mtd_info->blksize = mtd->erasesize;
	mtd_info->pagesize = mtd->writesize;
	mtd_info->total_bytes = mtd->size;
	mtd_info->part_cnt = sunxi_get_mtd_num_parts();

	printf("mtd->size@%llu\n", mtd->size);

	printf("mtd_info blksize@%u\n", mtd_info->blksize);
	printf("mtd_info pagesize@%u\n", mtd_info->pagesize);
	printf("mtd_info total_bytes@%u\n", mtd_info->total_bytes);

	for (i = 0; i < mtd_info->part_cnt; i++) {
		const char *part_name = sunxi_get_mtdparts_name(i);
		struct ubi_mtd_part *part = &mtd_info->part[i];

		part->partno = i;
		strncpy(part->name, part_name, 16);
		part->offset = sunxi_get_mtdpart_offset(i);
		part->bytes = sunxi_get_mtdpart_size(i);
	}
	return 0;
}
EXPORT_SYMBOL_GPL(rawnand_init_mtd_info);


unsigned rawnand_mtd_size(void)
{
	return rawnand_ubi_user_volumes_size();
}
EXPORT_SYMBOL_GPL(rawnand_mtd_size);

int rawnand_mtd_erase(int flag)
{
	rawnand_mtd_erase_boot();
	if (flag)
		rawnand_mtd_erase_data();
	return 0;
}
EXPORT_SYMBOL_GPL(rawnand_mtd_erase);

int rawnand_mtd_force_erase(void)
{
	return rawnand_mtd_erase(1);
}
EXPORT_SYMBOL_GPL(rawnand_mtd_force_erase);

int rawnand_mtd_flush(void)
{
	struct aw_nand_chip *chip = get_rawnand();
	struct mtd_info *mtd = awnand_chip_to_mtd(chip);

	mtd_sync(mtd);
	return 0;
}
EXPORT_SYMBOL_GPL(rawnand_mtd_flush);

int rawnand_mtd_write_end(void)
{
	return rawnand_mtd_flush_last_volume();
}
EXPORT_SYMBOL_GPL(rawnand_mtd_write_end);

inline int rawnand_mtd_write(unsigned int start, unsigned int sects, void *buf)
{
	return rawnand_mtd_write_ubi(start, sects, buf);
}
EXPORT_SYMBOL_GPL(rawnand_mtd_write);

inline int rawnand_mtd_read(unsigned int start, unsigned int sects, void *buf)
{

	return rawnand_mtd_read_ubi(start, sects, buf);

}
EXPORT_SYMBOL_GPL(rawnand_mtd_read);


int rawnand_mtd_secure_storage_read(int item, char *buf, unsigned int len)
{
	struct aw_nand_sec_sto *sec_sto = get_rawnand_sec_sto();

	return aw_rawnand_secure_storage_read(sec_sto, item, buf, len);
}
EXPORT_SYMBOL_GPL(rawnand_mtd_secure_storage_read);

int rawnand_mtd_secure_storage_write(int item, char *buf, unsigned int len)
{
	struct aw_nand_sec_sto *sec_sto = get_rawnand_sec_sto();

	return aw_rawnand_secure_storage_write(sec_sto, item, buf, len);
}
EXPORT_SYMBOL_GPL(rawnand_mtd_secure_storage_write);


int rawnand_mtd_init(void)
{
	struct udevice *dev = get_udevice();

	return aw_rawnand_probe(dev);
}
EXPORT_SYMBOL_GPL(rawnand_mtd_init);

int rawnand_mtd_exit(void)
{
	struct udevice *dev = get_udevice();

	return aw_rawnand_remove(dev);
}
EXPORT_SYMBOL_GPL(rawnand_mtd_exit);

static const struct udevice_id of_rawnand_id[] = {
    { .compatible = "allwinner,sun50iw9-nand"},
    { .compatible = "allwinner,sun50iw10-nand"},
	{ .compatible = "allwinner,sun50iw11-nand"},
    { .compatible = "allwinner,sun8iw19-nand"},
    {/* sentinel */},
};


U_BOOT_DRIVER(aw_rawnand) = {
	.name = "aw_rawnand",
	.id = UCLASS_MTD,
	.of_match = of_rawnand_id,
	.priv_auto_alloc_size = sizeof(struct aw_nand_chip),
	.probe = aw_rawnand_probe,
	.remove = aw_rawnand_remove,
};
