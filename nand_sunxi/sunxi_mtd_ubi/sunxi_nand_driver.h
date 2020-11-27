#ifndef __SUNXI_NAND_DRIVER_H__
#define __SUNXI_NAND_DRIVER_H__

#include "sunxi_nand_ubi.h"

#define MAX_PART_COUNT_PER_FTL      24
#define MAX_PARTITION               4
#define PARTITION_NAME_SIZE         16

/* for simplie(boot0) */
struct _nand_super_block {
	unsigned short  Block_NO;
	unsigned short  Chip_NO;
};

struct _nand_disk {
	unsigned int        size;
	/* unsigned int offset; */
	unsigned int        type;
	unsigned  char      name[PARTITION_NAME_SIZE];
};

struct _partition {
	struct          _nand_disk nand_disk[MAX_PART_COUNT_PER_FTL];
	unsigned int    size;
	unsigned int    cross_talk;
	unsigned int    attribute;
	struct          _nand_super_block start;
	struct          _nand_super_block end;
	/* unsigned int  offset; */
};

struct _nand_lib_cfg {
	unsigned int    phy_interface_cfg;

	unsigned int    phy_support_two_plane;
	unsigned int    phy_nand_support_vertical_interleave;
	unsigned int    phy_support_dual_channel;

	unsigned int    phy_wait_rb_before;
	unsigned int    phy_wait_rb_mode;
	unsigned int    phy_wait_dma_mode;
};

/* for globle*/
struct _nand_info {
	unsigned short                  type;
	unsigned short                  SectorNumsPerPage;
	unsigned short                  BytesUserData;
	unsigned short                  PageNumsPerBlk;
	unsigned short                  BlkPerChip;
	unsigned short                  ChipNum;
	unsigned short                  FirstBuild;
	unsigned short                  new_bad_page_addr;
	unsigned long long              FullBitmap;
	struct _nand_super_block        mbr_block_addr;
	struct _nand_super_block        bad_block_addr;
	struct _nand_super_block        new_bad_block_addr;
	struct _nand_super_block        no_used_block_addr;
	/*	struct _bad_block_lis			*new_bad_block_addr; */
	struct _nand_super_block		*factory_bad_block;
	struct _nand_super_block		*new_bad_block;
	unsigned char					*temp_page_buf;
	unsigned char					*mbr_data;
	struct _nand_phy_partition		*phy_partition_head;
	struct _partition               partition[MAX_PARTITION];
	struct _nand_lib_cfg            nand_lib_cfg;
	unsigned short                  partition_nums;
	unsigned short                  cache_level;
	unsigned short                  capacity_level;

	unsigned short                  mini_free_block_first_reserved;
	unsigned short                  mini_free_block_reserved;

	unsigned int                    MaxBlkEraseTimes;
	unsigned int                    EnableReadReclaim;

	unsigned int                    read_claim_interval;
};

struct _physic_nand_info {
	unsigned short      type;
	unsigned short      SectorNumsPerPage;
	unsigned short      BytesUserData;
	unsigned short      PageNumsPerBlk;
	unsigned short      BlkPerChip;
	unsigned short      ChipNum;
	__int64             FullBitmap;
};

#define MAGIC_DATA_FOR_PERMANENT_DATA     (0xa5a5a5a5)
struct _nand_permanent_data {
	unsigned int magic_data;
	unsigned int support_two_plane;
	unsigned int support_vertical_interleave;
	unsigned int support_dual_channel;
	unsigned int reserved[64-4];
};

#define MAX_CHIP_PER_CHANNEL	4
#define MAX_CMD_PER_LIST		16

struct _nctri_cmd {
	u32   cmd_valid;

	u32   cmd;
	u32   cmd_send;
	u32   cmd_wait_rb;

	u8    cmd_addr[MAX_CMD_PER_LIST];
	u32   cmd_acnt;

	u32   cmd_trans_data_nand_bus;
	u32   cmd_swap_data;
	u32   cmd_swap_data_dma;
	u32   cmd_direction;
	u32   cmd_mdata_len;
    /* u32   cmd_mdata_addr; */
	u8    *cmd_mdata_addr;
};

struct _nctri_cmd_seq {
	u32   cmd_type;
	u32   ecc_layout;
	u32   row_addr_auto_inc;
	struct _nctri_cmd nctri_cmd[MAX_CMD_PER_LIST];
	u32   re_start_cmd;
	u32   re_end_cmd;
	u32   re_cmd_times;
};


struct _nand_controller_reg {
	volatile u32 *reg_ctl;
	volatile u32 *reg_sta;
	volatile u32 *reg_int;
	volatile u32 *reg_timing_ctl;
	volatile u32 *reg_timing_cfg;
	volatile u32 *reg_addr_low;
	volatile u32 *reg_addr_high;
	volatile u32 *reg_sect_num;
	volatile u32 *reg_byte_cnt;
	volatile u32 *reg_cmd;
	volatile u32 *reg_read_cmd_set;
	volatile u32 *reg_write_cmd_set;
	volatile u32 *reg_io;
	volatile u32 *reg_ecc_ctl;
	volatile u32 *reg_ecc_sta;
	volatile u32 *reg_debug;
	volatile u32 *reg_err_cnt0;
	volatile u32 *reg_err_cnt1;
	volatile u32 *reg_err_cnt2;
	volatile u32 *reg_err_cnt3;
	volatile u32 *reg_user_data_base;
	volatile u32 *reg_efnand_sta;
	volatile u32 *reg_spare_area;
	volatile u32 *reg_pat_id;
	volatile u32 *reg_mbus_dma_addr;
	volatile u32 *reg_dma_cnt;
	volatile u32 *reg_ram0_base;
	volatile u32 *reg_ram1_base;
	volatile u32 *reg_dma_sta;
};


struct _nand_controller_reg_bak {
	u32 reg_ctl;
	u32 reg_sta;
	u32 reg_int;
	u32 reg_timing_ctl;
	u32 reg_timing_cfg;
	u32 reg_addr_low;
	u32 reg_addr_high;
	u32 reg_sect_num;
	u32 reg_byte_cnt;
	u32 reg_cmd;
	u32 reg_read_cmd_set;
	u32 reg_write_cmd_set;
	u32 reg_io;
	u32 reg_ecc_ctl;
	u32 reg_ecc_sta;
	u32 reg_debug;
	u32 reg_err_cnt0;
	u32 reg_err_cnt1;
	u32 reg_err_cnt2;
	u32 reg_err_cnt3;
	u32 reg_user_data_base;
	u32 reg_efnand_sta;
	u32 reg_spare_area;
	u32 reg_pat_id;
	u32 reg_mbus_dma_addr;
	u32 reg_dma_cnt;
	u32 reg_ram0_base;
	u32 reg_ram1_base;
	u32 reg_dma_sta;
};


struct _nand_controller_info {
    #define MAX_ECC_BLK_CNT     16

	struct _nand_controller_info *next;
	u32 type;                   /* ndfc type */
	u32 channel_id;             /* 0: channel 0; 1: channel 1; */
	u32 chip_cnt;
	u32 chip_connect_info;
	u32 rb_connect_info;
	u32 max_ecc_level;

	struct _nctri_cmd_seq nctri_cmd_seq;

	u32 ce[8];
	u32 rb[8];
	u32 dma_type;                       /* 0: general dma; 1: mbus dma; */
	u32 dma_addr;

	u32 current_op_type;                /* 1: write operation; 0: others */

	/* 1: before send cmd io; 0: after send cmd io; */
	u32 write_wait_rb_before_cmd_io;

	/* 0: query mode; 1: interrupt mode */
	u32 write_wait_rb_mode;

	u32 write_wait_dma_mode;
	u32 rb_ready_flag;
	u32 dma_ready_flag;
	u32 dma_channel;
	u32 nctri_flag;
	u32 ddr_timing_ctl[MAX_CHIP_PER_CHANNEL];
	u32 ddr_scan_blk_no[MAX_CHIP_PER_CHANNEL];

	struct _nand_controller_reg nreg;
	struct _nand_controller_reg_bak nreg_bak;
	struct _nand_chip_info *nci;

};

extern int printf(const char *fmt, ...);
extern int NAND_Print_DBG(const char *str, ...);

#define NAND_Print(fmt, args...) printf(fmt, ##args)
#define PRINT(...)  NAND_Print(__VA_ARGS__)
#define PRINT_DBG(...)  NAND_Print_DBG(__VA_ARGS__)

#if 1
#define PHY_DBG(...)    PRINT_DBG(__VA_ARGS__)
#else
#define PHY_DBG(...)
#endif

#if 1
#define PHY_ERR(...)    PRINT(__VA_ARGS__)
#else
#define PHY_ERR(...)
#endif

extern void nand_cfg_setting(void);
extern int nand_physic_init(void);
extern __u32 NAND_GetNandIDNumCtrl(void);
extern __u32 NAND_GetNandExtPara(__u32 para_num);
extern int  NAND_EraseBootBlocks(void);
extern int NAND_PhyInit(void);
extern int  NAND_EraseChip_force(void);
extern int NAND_VersionCheck(void);
extern int  NAND_EraseBootBlocks(void);
extern int nand_is_blank(void);
extern int  NAND_EraseChip(void);
extern int NAND_PhyExit(void);
extern int nand_secure_storage_init(void);
extern int set_nand_structure(void *phy_arch);
extern int NAND_BurnBoot0(uint length, void *buffer);
extern int NAND_BurnUboot(uint length, void *buffer);
extern int NAND_set_boot_mode(__u32 boot);
extern int nand_get_param(boot_nand_para_t *nand_param);
extern __u32 NAND_GetNandCapacityLevel(void);
extern int nand_info_init(struct _nand_info *nand_info, uchar chip,
	uint16 start_block, uchar *mbr_data);
extern uint32 get_phy_partition_num(struct _nand_info *nand_info);
extern int nftl_build_one(struct _nand_info *nand_info, uint32 num);
extern void set_capacity_level(struct _nand_info *nand_info,
	unsigned short capacity_level);
extern int nftl_build_all(struct _nand_info *nand_info);
extern int NAND_UbootExit(void);
extern int fdt_path_offset(const void *fdt, const char *path);
extern int fdt_getprop_u32(const void *fdt, int nodeoffset,
	const char *prop, uint32_t *val);
extern int NandHwExit(void);

int sunxi_nand_uboot_exit(int force);
int sunxi_nand_download_uboot(uint length, void *buffer);
int sunxi_nand_uboot_erase(int user_erase);
int sunxi_nand_download_boot0(uint length, void *buffer);
int sunxi_nand_uboot_probe(void);
int sunxi_nand_uboot_force_erase(void);
uint sunxi_nand_uboot_get_flash_info(void *buffer, uint length);
int sunxi_nand_uboot_init(int boot_mode);
int sunxi_nand_phy_init(void);
int sunxi_open_ubifs_interface(void);
int sunxi_get_mtd_ubi_mode_status(void);
void sunxi_disable_mtd_ubi_mode(void);
void sunxi_enable_mtd_ubi_mode(void);

extern struct _nand_info aw_nand_info;
extern struct _nand_super_storage_info *g_nssi;
extern struct _nand_permanent_data  nand_permanent_data;
extern struct _nand_lib_cfg *g_phy_cfg;
extern struct _nand_controller_info *g_nctri;
extern struct fdt_header *working_fdt;

extern int sunxi_mtd_ubi_mode;
extern PARTITION_MBR nand_mbr;
extern int  mbr_burned_flag;
#endif
