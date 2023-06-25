/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __SPINAND_PHYSIC_H
#define __SPINAND_PHYSIC_H

#include <linux/mtd/aw-spinand.h>
#include <linux/bitops.h>
#include <linux/printk.h>

#define AW_SPINAND_PHY_VER_MAIN		0x01
#define AW_SPINAND_PHY_VER_SUB		0x11
#define AW_SPINAND_PHY_VER_DATE		0x20211217

#define SECTOR_SHIFT 9

#ifndef BIT
#define BIT(nr)			(1UL << (nr))
#endif

/* spinand cmd num */
#define SPI_NAND_WREN		0x06
#define SPI_NAND_WRDI		0x04
#define SPI_NAND_GETSR		0x0f
#define SPI_NAND_SETSR		0x1f
#define SPI_NAND_PAGE_READ	0x13
#define SPI_NAND_FAST_READ_X1	0x0b
#define SPI_NAND_READ_X1	0x03
#define SPI_NAND_READ_X2	0x3b
#define SPI_NAND_READ_X4	0x6b
#define SPI_NAND_READ_DUAL_IO	0xbb
#define SPI_NAND_READ_QUAD_IO	0xeb
#define SPI_NAND_RDID		0x9f
#define SPI_NAND_PP		0x02
#define SPI_NAND_PP_X4		0x32
#define SPI_NAND_RANDOM_PP	0x84
#define SPI_NAND_RANDOM_PP_X4	0x34
#define SPI_NAND_PE		0x10
#define SPI_NAND_BE		0xd8
#define SPI_NAND_RESET		0xff
#define SPI_NAND_READ_INT_ECCSTATUS 0x7c

/* status register */
#define REG_STATUS		0xc0
#define GD_REG_EXT_ECC_STATUS	0xf0
#define STATUS_BUSY		BIT(0)
#define STATUS_ERASE_FAILED	BIT(2)
#define STATUS_PROG_FAILED	BIT(3)
#define STATUS_ECC_SHIFT	4

/* feature register */
#define REG_BLOCK_LOCK		0xa0

/* configuration register */
#define FORESEE_REG_ECC_CFG	0x90
#define REG_CFG			0xb0
#define CFG_OTP_ENABLE		BIT(6)
#define CFG_BUF_MODE		BIT(3)
#define CFG_ECC_ENABLE		BIT(4)
#define CFG_QUAD_ENABLE		BIT(0)

/* driver strength register */
#define REG_DRV			0xd0

struct aw_spinand_ecc {
	int (*copy_to_oob)(enum ecc_oob_protected type, unsigned char *to,
			unsigned char *from, unsigned int len);
	int (*copy_from_oob)(enum ecc_oob_protected type, unsigned char *to,
			unsigned char *from, unsigned int len);
	int (*check_ecc)(enum ecc_limit_err type, unsigned char status);
};

/* bbt: bad block table */
struct aw_spinand_bbt {
	unsigned long *bitmap;
	unsigned long *en_bitmap;

	int (*mark_badblock)(struct aw_spinand_chip *chip,
			unsigned int blknum, bool badblk);
#define BADBLOCK	(1)
#define NON_BADBLOCK	(0)
#define NOT_MARKED	(-1)
	int (*is_badblock)(struct aw_spinand_chip *chip, unsigned int blknum);
};

struct aw_spinand_cache {
#ifdef CONFIG_AW_SPINAND_NONSTANDARD_SPI_DRIVER
	/*
	 * the non standard spi driver send the cmd and data at the same time
	 * when write, so, we must allocate a buffer to temporary storage
	 * cmd and data.
	 */
	unsigned char *wbuf;
#endif
	unsigned char *databuf;
	unsigned char *oobbuf;
	unsigned int data_maxlen;
	unsigned int oob_maxlen;
	unsigned int block;
	unsigned int page;
#define INVALID_CACHE_ALL_AREA	0
#define VALID_CACHE_OOB		BIT(1)
#define VALID_CACHE_DATA	BIT(2)
	unsigned int area;

	/*
	 * If the structure cache already has the data before, just copy
	 * these data to req.
	 * @match_cache is helper to check whether the structure cache is
	 *   what req needed.
	 * @copy_to_cache is helper to copy req to structure cache.
	 * @copy_from_cache is helper to copy structure cache to req.
	 */
	bool (*match_cache)(struct aw_spinand_chip *chip,
			struct aw_spinand_chip_request *req);
	int (*copy_to_cache)(struct aw_spinand_chip *chip,
			struct aw_spinand_chip_request *req);
	int (*copy_from_cache)(struct aw_spinand_chip *chip,
			struct aw_spinand_chip_request *req);
	/*
	 * 3 step:
	 *  a) copy data from req to cache->databuf/oobbuf
	 *  b) update cache->block/page
	 *  c) send write cache command to spinand
	 */
	int (*write_to_cache)(struct aw_spinand_chip *chip,
			struct aw_spinand_chip_request *req);
	/*
	 * 3 step:
	 *  a) send read cache command to spinand
	 *  b) update cache->block/page
	 *  c) copy data from cache->databuf/oobbuf to req
	 */
	int (*read_from_cache)(struct aw_spinand_chip *chip,
			struct aw_spinand_chip_request *req);
};

extern int aw_spinand_chip_ecc_init(struct aw_spinand_chip *chip);
extern int aw_spinand_chip_ops_init(struct aw_spinand_chip *chip);
extern int aw_spinand_chip_detect(struct aw_spinand_chip *chip);
extern int aw_spinand_chip_bbt_init(struct aw_spinand_chip *chip);
extern void aw_spinand_chip_bbt_exit(struct aw_spinand_chip *chip);
extern int aw_spinand_chip_cache_init(struct aw_spinand_chip *chip);
extern void aw_spinand_chip_cache_exit(struct aw_spinand_chip *chip);

#endif
