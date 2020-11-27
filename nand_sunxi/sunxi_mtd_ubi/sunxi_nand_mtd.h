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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.
 */
#ifndef __SUNXI_NAND_MTD_H__
#define __SUNXI_NAND_MTD_H__

#include <nand.h>

/* #define SUNXI_NAND_MTD_DEBUG */

/* reach the limit of the ability of ECC */
#define ECC_LIMIT               10

#define ENOTSUPP                524     /* Operation is not supported */
#define THIS_MODULE             0
#define SUNXI_NAND_W_FLAG       0x01

#define SUNXI_MAX_SECS_PER_PAGE 64
#define SUNXI_MAX_PAGE_SIZE     (SUNXI_MAX_SECS_PER_PAGE * 1024)

#define BOOT0_1_USED_PHY_NAND_BLK_CNT   8

#ifndef __int64
    #define __int64 __u64
#endif
#ifndef uchar
typedef unsigned char   uchar;
#endif
#ifndef uint16
typedef unsigned short  uint16;
#endif
#ifndef uint32
typedef unsigned int    uint32;
#endif
#ifndef sint32
typedef  int            sint32;
#endif
#ifndef uint64
typedef __int64         uint64;
#endif
#ifndef sint16
typedef short           sint16;
#endif
#ifndef UINT8
typedef unsigned char   UINT8;
#endif
#ifndef UINT32
typedef unsigned int    UINT32;
#endif
#ifndef SINT32
typedef  signed int     SINT32;
#endif

typedef struct {
	__u32	ChannelCnt;
	__u32	ChipCnt;
	__u32	ChipConnectInfo;
	__u32	RbCnt;
	__u32	RbConnectInfo;
	__u32	RbConnectMode;
	__u32	BankCntPerChip;
	__u32	DieCntPerChip;
	__u32	PlaneCntPerDie;
	__u32	SectorCntPerPage;
	__u32	PageCntPerPhyBlk;
	__u32	BlkCntPerDie;
	__u32	OperationOpt;
	__u32	FrequencePar;
	__u32	EccMode;
	__u8	NandChipId[8];
	__u32	ValidBlkRatio;
	__u32	good_block_ratio;
	__u32	ReadRetryType;
	__u32	DDRType;
	__u32	Reserved[22];
} boot_nand_para_t;


struct _nand_super_storage_info {
	unsigned int	super_chip_cnt;
	unsigned int	super_block_nums;
	unsigned int	support_two_plane;
	unsigned int	support_v_interleave;
	unsigned int	support_dual_channel;
	struct			_nand_super_chip_info *nsci;
};

struct _nand_phy_info_par {
	char			nand_id[8];
	unsigned int	die_cnt_per_chip;
	unsigned int	sect_cnt_per_page;
	unsigned int	page_cnt_per_blk;
	unsigned int	blk_cnt_per_die;
	unsigned int	operation_opt;
	unsigned int	valid_blk_ratio;
	unsigned int	access_freq;
	unsigned int	ecc_mode;
	unsigned int	read_retry_type;
	unsigned int	ddr_type;
	unsigned int	option_phyisc_op_no;
	unsigned int	ddr_info_no;
	unsigned int	id_number;
	unsigned int	max_blk_erase_times;
	unsigned int	driver_no;
	unsigned int	access_high_freq;
};


struct _nand_physic_op_par {
	unsigned int	chip;
	unsigned int	block;
	unsigned int	page;
	unsigned int	sect_bitmap;
	unsigned char	*mdata;
	unsigned char	*sdata;
	unsigned int	slen;
};

struct _nand_chip_info {
	struct _nand_chip_info *nsi_next;
	struct _nand_chip_info *nctri_next;

	char			id[8];
	unsigned int	chip_no;
	unsigned int	nctri_chip_no;

	unsigned int	blk_cnt_per_chip;
	unsigned int	sector_cnt_per_page;
	unsigned int	page_cnt_per_blk;
	unsigned int	page_offset_for_next_blk;

	unsigned int	randomizer;
	unsigned int	read_retry;
	unsigned char	readretry_value[128];
	unsigned int	retry_count;
	unsigned int	lsb_page_type;

	unsigned int	ecc_mode;
	unsigned int	max_erase_times;
	unsigned int	driver_no;

	unsigned int	interface_type;
	unsigned int	frequency;
	unsigned int	timing_mode;

    /* nand flash support to change timing mode
	 * according to ONFI 3.0
	 */
	unsigned int	support_change_onfi_timing_mode;

	/* nand flash support to set ddr2 specific feature
	 * according to ONFI 3.0 or Toggle DDR2
	 */
	unsigned int	support_ddr2_specific_cfg;

    /* nand flash support to set io driver strength according to
	 * ONFI 2.x/3.0 or Toggle DDR1/DDR2
	 */
	unsigned int	support_io_driver_strength;

	unsigned int	support_vendor_specific_cfg;
	unsigned int	support_onfi_sync_reset;

	/* nand flash support toggle interface only,
	 * and do not support switch between legacy and toggle
	 */
	unsigned int	support_toggle_only;

	unsigned int	page_addr_bytes;

	unsigned int	sdata_bytes_per_page;
	unsigned int	ecc_sector;

	struct _nand_super_chip_info *nsci;
	struct _nand_controller_info *nctri;
	struct _nand_phy_info_par *npi;
	struct _optional_phy_op_par *opt_phy_op_par;
	struct _nfc_init_ddr_info *nfc_init_ddr_info;

	int (*nand_physic_erase_block)(struct _nand_physic_op_par *npo);
	int (*nand_physic_read_page)(struct _nand_physic_op_par *npo);
	int (*nand_physic_write_page)(struct _nand_physic_op_par *npo);
	int (*nand_physic_bad_block_check)(struct _nand_physic_op_par *npo);
	int (*nand_physic_bad_block_mark)(struct _nand_physic_op_par *npo);

	int (*nand_read_boot0_page)(struct _nand_chip_info *nci,
		struct _nand_physic_op_par *npo);
	int (*nand_write_boot0_page)(struct _nand_chip_info *nci,
		struct _nand_physic_op_par *npo);

	int (*nand_read_boot0_one)(unsigned char *buf, unsigned int len,
		unsigned int counter);
	int (*nand_write_boot0_one)(unsigned char *buf, unsigned int len,
		unsigned int counter);
	int (*is_lsb_page)(__u32 page_num);
};

struct _nand_super_chip_info {
	struct _nand_super_chip_info *nssi_next;

	unsigned int	chip_no;
	unsigned int	blk_cnt_per_super_chip;
	unsigned int	sector_cnt_per_super_page;
	unsigned int	page_cnt_per_super_blk;
	unsigned int	page_offset_for_next_super_blk;
	/* unsigned int	multi_plane_block_offset; */

	unsigned int	spare_bytes;
	unsigned int	channel_num;

	unsigned int	two_plane;
	unsigned int	vertical_interleave;
	unsigned int	dual_channel;

	unsigned int	driver_no;

	struct _nand_chip_info *nci_first;
	struct _nand_chip_info *v_intl_nci_1;
	struct _nand_chip_info *v_intl_nci_2;
	struct _nand_chip_info *d_channel_nci_1;
	struct _nand_chip_info *d_channel_nci_2;

	int (*nand_physic_erase_super_block)(struct _nand_physic_op_par *npo);
	int (*nand_physic_read_super_page)(struct _nand_physic_op_par *npo);
	int (*nand_physic_write_super_page)(struct _nand_physic_op_par *npo);
	int (*nand_physic_super_bad_block_check)
		(struct _nand_physic_op_par *npo);
	int (*nand_physic_super_bad_block_mark)
		(struct _nand_physic_op_par *npo);
};

extern int nand_register(int devnum);
extern int nand_physic_bad_block_check(unsigned int chip, unsigned int block);
extern int nand_physic_bad_block_mark(unsigned int chip, unsigned int block);
extern int nand_physic_erase_block(unsigned int chip, unsigned int block);
extern int nand_physic_read_page(unsigned int chip, unsigned int block,
	unsigned int page, unsigned int bitmap,
	unsigned char *mbuf, unsigned char *sbuf);
extern int nand_physic_write_page(unsigned int chip, unsigned int block,
	unsigned int page, unsigned int bitmap,
	unsigned char *mbuf, unsigned char *sbuf);
extern int PHY_VirtualPageRead(unsigned int nDieNum, unsigned int nBlkNum,
	unsigned int nPage, uint64 SectBitmap, void *pBuf, void *pSpare);
extern int PHY_VirtualPageWrite(unsigned int nDieNum, unsigned int nBlkNum,
	unsigned int nPage, uint64 SectBitmap, void *pBuf, void *pSpare);
extern int PHY_VirtualBlockErase(unsigned int nDieNum, unsigned int nBlkNum);
extern int PHY_VirtualBadBlockCheck(unsigned int nDieNum, unsigned int nBlkNum);
extern int PHY_VirtualBadBlockMark(unsigned int nDieNum, unsigned int nBlkNum);
extern void show_nssi(void);
extern void debug_fix_blk_cnt(void);
extern int NAND_PhyInit(void);
extern int NAND_PhyExit(void);
extern int mtd_read(struct mtd_info *mtd, loff_t from, size_t len,
	size_t *retlen, u_char *buf);
extern int mtd_write(struct mtd_info *mtd, loff_t to, size_t len,
	size_t *retlen, const u_char *buf);
extern int sunxi_nand_phy_init(void);
extern int sunxi_nand_uboot_init(int boot_mode);
extern  int nand_get_param(boot_nand_para_t *nand_param);
extern int sunxi_get_mtd_ubi_mode_status(void);

uint64_t get_mtd_size(void);
uint32_t get_mtd_blksize(void);
uint32_t get_mtd_pgsize(void);
int sunxi_mtd_read(loff_t from, size_t len, size_t *retlen, u_char *buf);
int sunxi_mtd_write(loff_t to, size_t len, size_t *retlen, u_char *buf);

extern struct _nand_super_storage_info *g_nssi;
extern struct _nand_info aw_nand_info;

#endif
