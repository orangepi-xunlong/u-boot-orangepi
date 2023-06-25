/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __SUNXI_SPINAND_H
#define __SUNXI_SPINAND_H

#include <linux/kernel.h>
#include <linux/compat.h>
#include <linux/mtd/mtd.h>
#include <hexdump.h>
/* to print more log for uboot */
#undef CONFIG_LOGLEVEL
#define CONFIG_LOGLEVEL 7

/*
 * spinand do not support multiplane. In order to adapt to aw nand
 * we simulate multiplane.
 *
 * Merge pages in two adjacent blocks with the same page num to super page.
 * Merge adjacent blocs to super block.
 *
 *   phy-block0   phy-block1    = super block 0
 * |------------|------------|
 * | phy-page 0 | phy-page 0 |  = super page 0 on super block 0
 * | phy-page 1 | phy-page 1 |  = super page 1 on super block 0
 * |     ...    |     ...    |
 * |------------|------------|
 *
 */
#define SIMULATE_MULTIPLANE (1)
#define AW_OOB_SIZE_PER_PHY_PAGE (16)
#define AW_SPINAND_RESERVED_PHY_BLK_FOR_SECURE_STORAGE 8
#define AW_SPINAND_RESERVED_FOR_PSTORE_KB 512
/**
 * In order to fix for nftl nand, make they has the same address
 * for saving crc16
 */
#define AW_CRC16_OOB_OFFSET (12)

/* ecc status */
#define ECC_GOOD	(0 << 4)
#define ECC_LIMIT	(1 << 4)
#define ECC_ERR		(2 << 4)

#define SPI_NBITS_SINGLE 1
#define SPI_NBITS_DUAL 2
#define SPI_NBITS_QUAD 4

#define SPI_SELECT_ODDNUM_BLACK 0x10

#define SPINAND_MSG_EN	(1U)
#define SPINAND_MSG(d, fmt, args...) \
	do {if ((d)->msglevel & SPINAND_MSG_EN) \
	pr_err("[SPINAND]: "fmt, ##args); } while (0)

struct aw_spinand_ecc;
struct aw_spinand_info;
struct aw_spinand_phy_info;
struct aw_spinand_chip_ops;

struct aw_spinand_chip {
	struct aw_spinand_chip_ops *ops;
	struct aw_spinand_ecc *ecc;
	struct aw_spinand_cache *cache;
	struct aw_spinand_info *info;
	struct aw_spinand_bbt *bbt;
	struct spi_slave *slave;
	unsigned int rx_bit;
	unsigned int tx_bit;
	unsigned int freq;
	void *priv;
};

struct aw_spinand_chip_request {
	unsigned int block;
	unsigned int page;
	unsigned int pageoff;
	unsigned int ooblen;
	unsigned int datalen;
	void *databuf;
	void *oobbuf;

	unsigned int oobleft;
	unsigned int dataleft;

#define AW_SPINAND_MTD_OPS_RAW (2)
	int mode;
};

struct aw_spinand_chip_ops {
	int (*get_block_lock)(struct aw_spinand_chip *chip,
			unsigned char *reg_val);
	int (*set_block_lock)(struct aw_spinand_chip *chip,
			unsigned char reg_val);
	int (*get_otp)(struct aw_spinand_chip *chip, unsigned char *reg_val);
	int (*set_otp)(struct aw_spinand_chip *chip, unsigned char reg_val);
	int (*get_bft)(struct aw_spinand_chip *chip, unsigned char *reg_val);
	int (*get_driver_level)(struct aw_spinand_chip *chip,
			unsigned char *reg_val);
	int (*set_driver_level)(struct aw_spinand_chip *chip,
			unsigned char reg_val);
	int (*reset)(struct aw_spinand_chip *chip);
	int (*read_status)(struct aw_spinand_chip *chip, unsigned char *status);
	int (*read_id)(struct aw_spinand_chip *chip, void *id, int len,
			int dummy);
	int (*read_reg)(struct aw_spinand_chip *chip, unsigned char cmd,
			unsigned char reg, unsigned char *val);
	int (*write_reg)(struct aw_spinand_chip *chip, unsigned char cmd,
			unsigned char reg, unsigned char val);
	int (*is_bad)(struct aw_spinand_chip *chip,
			struct aw_spinand_chip_request *req);
	int (*mark_bad)(struct aw_spinand_chip *chip,
			struct aw_spinand_chip_request *req);
	int (*erase_block)(struct aw_spinand_chip *chip,
			struct aw_spinand_chip_request *req);
	int (*write_page)(struct aw_spinand_chip *chip,
			struct aw_spinand_chip_request *req);
	int (*read_page)(struct aw_spinand_chip *chip,
			struct aw_spinand_chip_request *req);
	int (*phy_is_bad)(struct aw_spinand_chip *chip,
			struct aw_spinand_chip_request *req);
	int (*phy_mark_bad)(struct aw_spinand_chip *chip,
			struct aw_spinand_chip_request *req);
	int (*phy_erase_block)(struct aw_spinand_chip *chip,
			struct aw_spinand_chip_request *req);
	int (*phy_write_page)(struct aw_spinand_chip *chip,
			struct aw_spinand_chip_request *req);
	int (*phy_read_page)(struct aw_spinand_chip *chip,
			struct aw_spinand_chip_request *req);
	int (*phy_copy_block)(struct aw_spinand_chip *chip,
			unsigned int from_blk, unsigned int to_blk);
};

/*different manufacture spinand's ecc status location maybe not the same*/
enum ecc_status_shift {
	ECC_STATUS_SHIFT_0 = 0,
	ECC_STATUS_SHIFT_1,
	ECC_STATUS_SHIFT_2,
	ECC_STATUS_SHIFT_3,
	ECC_STATUS_SHIFT_4,
	ECC_STATUS_SHIFT_5,
	ECC_STATUS_SHIFT_6,
	ECC_STATUS_SHIFT_7,
};

enum ecc_limit_err {
	ECC_TYPE_ERR = 0,
	BIT3_LIMIT2_TO_6_ERR7,
	BIT2_LIMIT1_ERR2,
	BIT2_LIMIT1_ERR2_LIMIT3,
	BIT4_LIMIT3_TO_4_ERR15,
	BIT3_LIMIT3_TO_4_ERR7,
	BIT3_LIMIT5_ERR2,
	BIT4_LIMIT5_TO_7_ERR8_LIMIT_12,
	BIT4_LIMIT5_TO_8_ERR9_TO_15,
};

enum ecc_oob_protected {
	ECC_PROTECTED_TYPE = 0,
	/* all spare data are under ecc protection */
	SIZE16_OFF0_LEN16,
	SIZE16_OFF4_LEN12,
	SIZE16_OFF4_LEN4_OFF8,
	/*compatible with GD5F1GQ4UBYIG@R6*/
	SIZE16_OFF4_LEN8_OFF4,
	SIZE16_OFF32_LEN16,
	/*compatible with XTX*/
	SIZE16_OFF8_LEN16,
};

struct aw_spinand_phy_info {
	const char *Model;
#define MAX_ID_LEN 8
	unsigned char NandID[MAX_ID_LEN];
	unsigned int DieCntPerChip;
	unsigned int BlkCntPerDie;
	unsigned int PageCntPerBlk;
	unsigned int SectCntPerPage;
	unsigned int OobSizePerPage;
#define BAD_BLK_FLAG_MARK			0x03
#define BAD_BLK_FLAG_FRIST_1_PAGE		0x00
#define BAD_BLK_FLAG_FIRST_2_PAGE		0x01
#define BAD_BLK_FLAG_LAST_1_PAGE		0x02
#define BAD_BLK_FLAG_LAST_2_PAGE		0x03
	int BadBlockFlag;
#define SPINAND_DUAL_READ			BIT(0)
#define SPINAND_QUAD_READ			BIT(1)
#define SPINAND_QUAD_PROGRAM			BIT(2)
#define SPINAND_QUAD_NO_NEED_ENABLE		BIT(3)
#define SPINAND_TWO_PLANE_SELECT		BIT(7)
#define SPINAND_ONEDUMMY_AFTER_RANDOMREAD	BIT(8)
	int OperationOpt;
	int MaxEraseTimes;
#define HAS_EXT_ECC_SE01			BIT(0)
#define HAS_EXT_ECC_STATUS			BIT(1)
	int EccFlag;
	enum ecc_status_shift ecc_status_shift;
	enum ecc_limit_err EccType;
	enum ecc_oob_protected EccProtectedType;
};

struct aw_spinand_info {
	const char *(*model)(struct aw_spinand_chip *chip);
	const char *(*manufacture)(struct aw_spinand_chip *chip);
	void (*nandid)(struct aw_spinand_chip *chip, unsigned char *id, int cnt);
	unsigned int (*die_cnt)(struct aw_spinand_chip *chip);
	unsigned int (*oob_size)(struct aw_spinand_chip *chip);
	unsigned int (*sector_size)(struct aw_spinand_chip *chip);
	unsigned int (*page_size)(struct aw_spinand_chip *chip);
	unsigned int (*block_size)(struct aw_spinand_chip *chip);
	unsigned int (*phy_oob_size)(struct aw_spinand_chip *chip);
	unsigned int (*phy_page_size)(struct aw_spinand_chip *chip);
	unsigned int (*phy_block_size)(struct aw_spinand_chip *chip);
	unsigned int (*total_size)(struct aw_spinand_chip *chip);
	int (*operation_opt)(struct aw_spinand_chip *chip);
	int (*max_erase_times)(struct aw_spinand_chip *chip);

	/* private data */
	struct aw_spinand_phy_info *phy_info;
};

int addr_to_req(struct aw_spinand_chip *chip, struct aw_spinand_chip_request *req,
		unsigned int addr);
int aw_spinand_chip_init(struct spi_slave *salve, struct aw_spinand_chip *chip);
void aw_spinand_chip_exit(struct aw_spinand_chip *chip);

#define aw_spinand_hexdump(level, prefix, buf, len)			\
	print_hex_dump(prefix, DUMP_PREFIX_OFFSET, 16, 1,		\
			buf, len, true)
#define aw_spinand_reqdump(func, note, req)				\
	do {								\
		func("%s(%d): %s\n", __func__, __LINE__, note);		\
		func("\tblock: %u\n", (req)->block);			\
		func("\tpage: %u\n", (req)->page);			\
		func("\tpageoff: %u\n", (req)->pageoff);		\
		if ((req)->databuf)					\
			func("\tdatabuf: 0x%p\n", (req)->databuf);	\
		else							\
			func("\tdatabuf: NULL\n");			\
		func("\tdatalen: %u\n", (req)->datalen);		\
		func("\tdataleft: %u\n", (req)->dataleft);		\
		if ((req)->oobbuf)					\
			func("\toobbuf: 0x%p\n", (req)->oobbuf);	\
		else							\
			func("\toobbuf: NULL\n");			\
		func("\tooblen: %u\n", (req)->ooblen);			\
		func("\toobleft: %u\n", (req)->oobleft);		\
		func("\n");						\
	} while (0)

struct aw_spinand_sec_sto {
	/* the follow three must be initialized by the caller before read/write */
	unsigned int startblk;
	unsigned int endblk;
	struct aw_spinand_chip *chip;

	unsigned int blk[2];
	int init_end;
};

int aw_spinand_secure_storage_read(struct aw_spinand_sec_sto *sec_sto,
		int item, char *buf, unsigned int len);
int aw_spinand_secure_storage_write(struct aw_spinand_sec_sto *sec_sto,
		int item, char *buf, unsigned int len);

struct aw_spinand {
	struct aw_spinand_chip chip;
	struct mtd_info mtd;
	int sector_shift;
	int page_shift;
	int block_shift;
	int phy_page_shift;
	int phy_block_shift;
	struct aw_spinand_sec_sto sec_sto;
	int msglevel;
	unsigned int right_sample_delay;
	unsigned int right_sample_mode;
};

extern int aw_spinand_probe(struct udevice *dev);
extern void aw_spinand_exit(struct aw_spinand *spinand);
extern int spinand_mtd_init(void);
extern int spinand_mtd_attach_mtd(void);
extern int spinand_mtd_exit(void);
extern unsigned spinand_mtd_size(void);
extern bool support_spinand(void);
extern struct aw_spinand *get_spinand(void);
extern int spinand_mtd_download_boot0(unsigned int len, void *buf);
extern int spinand_mtd_download_uboot(unsigned int len, void *buf);
extern int spinand_mtd_flush(void);
extern int spinand_mtd_write_end(void);
extern int spinand_mtd_get_flash_info(void *info, unsigned int len);
extern unsigned int spinand_mtd_blksize(void);
extern unsigned int spinand_mtd_pagesize(void);
extern int spinand_mtd_read(unsigned int start, unsigned int sects, void *buf);
extern int spinand_mtd_write(unsigned int start, unsigned int sects, void *buf);
extern int spinand_mtd_erase(int force);
extern int spinand_mtd_force_erase(void);
extern int spinand_mtd_update_ubi_env(void);
extern int spinand_mtd_set_last_vol_sects(unsigned int sects);
extern int spinand_mtd_secure_storage_read(int item, char *buf,
		unsigned int len);
extern int spinand_mtd_secure_storage_write(int item, char *buf,
		unsigned int len);
extern uint64_t spinand_sys_part_offset(void);
#endif
