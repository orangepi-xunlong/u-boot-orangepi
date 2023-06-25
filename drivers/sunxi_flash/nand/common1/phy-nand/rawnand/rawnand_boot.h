/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __RAWNAND_BOOT_H__
#define __RAWNAND_BOOT_H__
//#include "../nand-partition/phy.h"
#include <sunxi_nand_partitions.h>
#include <private_boot0.h>


#define NDFC_PAGE_TAB_MAGIC "BT0.NTAB"
#define NDFC_PAGE_TAB_HEAD_SIZE (64)
#define NDFC_DMY_TAB_MAGIC "BT0.DMTB" //dummy table

struct boot_ndfc_cfg {
	u8 page_size_kb;
	u8 ecc_mode;
	u8 sequence_mode;
	u8 res[5];
};

typedef struct {
	unsigned int ChannelCnt;
	/*count of total nand chips are currently connecting on the CE pin*/
	unsigned int ChipCnt;
	/*chip connect info, bit=1 means one chip connecting on the CE pin*/
	unsigned int ChipConnectInfo;
	unsigned int RbCnt;
	/*connect info of all rb  chips are connected*/
	unsigned int RbConnectInfo;
	unsigned int RbConnectMode;	/*rb connect mode*/
	/*count of banks in one nand chip, multi banks can support Inter-Leave*/
	unsigned int BankCntPerChip;
	/*count of dies in one nand chip, block management is based on Die*/
	unsigned int DieCntPerChip;
	/*count of planes in one die, >1 can support multi-plane operation*/
	unsigned int PlaneCntPerDie;
	/*count of sectors in one single physic page, one sector is 0.5k*/
	unsigned int SectorCntPerPage;
	/*count of physic pages in one physic block*/
	unsigned int PageCntPerPhyBlk;
	/*count of physic blocks in one die, include valid and invalid blocks*/
	unsigned int BlkCntPerDie;
	/*mask of operation types which current nand flash can support support*/
	unsigned int OperationOpt;
	/*parameter of hardware access clock, based on 'MHz'*/
	unsigned int FrequencePar;
	/*Ecc Mode for nand chip, 0: bch-16, 1:bch-28, 2:bch_32*/
	unsigned int EccMode;
	/*nand chip id of current connecting nand chip*/
	unsigned char NandChipId[8];
	/*ratio of valid physical blocks, based on 1024*/
	unsigned int ValidBlkRatio;
	unsigned int good_block_ratio; /*good block ratio get from hwscan*/
	unsigned int ReadRetryType; /*read retry type*/
	unsigned int DDRType;
	unsigned int uboot_start_block;
	unsigned int uboot_next_block;
	unsigned int logic_start_block;
	unsigned int nand_specialinfo_page;
	unsigned int nand_specialinfo_offset;
	unsigned int physic_block_reserved;
	/*special nand cmd for some nand in batch cmd, only for write*/
	unsigned int random_cmd2_send_flag;
	/*random col addr num in batch cmd*/
	unsigned int random_addr_num;
	/*real physic page size*/
	unsigned int nand_real_page_size;
	unsigned int Reserved[23];
} boot_nand_para_t;
struct boot0_df_cfg_mode {
	s32 (*generic_read_boot0_page_cfg_mode)(struct nand_chip_info *nci, struct _nand_physic_op_par *npo, struct boot_ndfc_cfg cfg);
	s32 (*generic_write_boot0_page_cfg_mode)(struct nand_chip_info *nci, struct _nand_physic_op_par *npo, struct boot_ndfc_cfg cfg);
};

struct boot0_copy_df {
	u32 (*ndfc_blks_per_boot0_copy)(struct nand_chip_info *nci);
};

enum rq_write_boot0_type {
	NAND_WRITE_BOOT0_GENERIC = 0,
	NAND_WRITE_BOOT0_HYNIX_16NM_4G,
	NAND_WRITE_BOOT0_HYNIX_20NM,
	NAND_WRITE_BOOT0_HYNIX_26NM,
};

typedef int (*write_boot0_one_t)(unsigned char *buf, unsigned int len, unsigned int counter);

struct rawnand_boot0_ops_t {
	int (*write_boot0_page)(struct nand_chip_info *nci, struct _nand_physic_op_par *npo);
	int (*read_boot0_page)(struct nand_chip_info *nci, struct _nand_physic_op_par *npo);
	int (*write_boot0_one)(unsigned char *buf, unsigned int len, unsigned int counter);
	int (*read_boot0_one)(unsigned char *buf, unsigned int len, unsigned int counter);
};

void selected_write_boot0_one(enum rq_write_boot0_type rq);

//extern struct boot0_df_cfg_mode boot0_cfg_df;
extern struct boot0_copy_df boot0_copy_df;
extern struct rawnand_boot0_ops_t rawnand_boot0_ops;
extern write_boot0_one_t write_boot0_one[];

extern int nand_secure_storage_block_bak;
extern int nand_secure_storage_block;

int rawnand_write_boot0_one(unsigned char *buf, unsigned int len, unsigned int counter);
int rawnand_read_boot0_one(unsigned char *buf, unsigned int len, unsigned int counter);
int rawnand_write_uboot_one(unsigned char *buf, unsigned int len, struct _boot_info *info_buf, unsigned int info_len, unsigned int counter);
int rawnand_read_uboot_one(unsigned char *buf, unsigned int len, unsigned int counter);
int rawnand_get_param(void *nand_param);
int rawnand_get_param_for_uboottail(void *nand_param);
int RAWNAND_UpdatePhyArch(void);
int nand_check_bad_block_before_first_erase(struct _nand_physic_op_par *npo);
int rawnand_erase_chip(unsigned int chip, unsigned int start_block, unsigned int end_block, unsigned int force_flag);
void rawnand_erase_special_block(void);
int rawnand_uboot_erase_all_chip(UINT32 force_flag);
int rawnand_dragonborad_test_one(unsigned char *buf, unsigned char *oob, unsigned int blk_num);
int change_uboot_start_block(struct _boot_info *info, unsigned int start_block);
int rawnand_physic_info_get_one_copy(unsigned int start_block, unsigned int pages_offset, unsigned int *block_per_copy, unsigned int *buf);
int rawnand_add_len_to_uboot_tail(unsigned int uboot_size);
int set_hynix_special_info(void);
int rawnand_replace_boot0_with_toc0(void);

extern __u32 NAND_GetNandIDNumCtrl(void);
extern __u32 NAND_GetNandExtPara(__u32 para_num);
#endif /*RAWNAND_BOOT_H*/
