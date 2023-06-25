/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef  _SUNXI_NAND_H
#define  _SUNXI_NAND_H

#include <stdbool.h>

#define NOSPACE 0xFFFFFFFF
#define MAX_BLOCKS (16384)
#define BITS_MAX_BLOCKS (MAX_BLOCKS >> 3)

struct erase_status {
	/*whole chip don't care secure storage and phy storage area*/
	bool erase_whole_chip;
	/*start_block=chip*block_per_chip*/
	int start_block;
	/*end_block=chip*block_per_chip*/
	int end_block;
	/*when erase chip, maybe use nftl_erase_zone when the chip is not blank,
	 * when erase chip use physic erase, the flag phy_erase is true*/
	bool phy_erase;
#define SINGLE_ERASE (1)
#define MULTI_ERASE  (2)
	int erase_method;

};

enum size_type {
	BYTE,
	SECTOR,
	PAGE,
	BLOCK
};

struct secure_status {
#define SECURE_STORAGE_FIRST_BUILD (1)
	/*sector storage state*/
	int state;
};

struct boot_info_status {
#define BOOT_INFO_OK (1)
#define BOOT_INFO_NOEXIST (2)
	int state;
};

struct uboot_status {
#define UBOOT_IN_BOOT (0)
#define UBOOT_IN_PRODUCT (1)
	int state;
	struct secure_status nand_secstate;
	struct boot_info_status nand_bootinfo_state;
	struct erase_status nand_erase_state;
};
extern struct uboot_status ubootsta;

struct _nand_partition_page {
	unsigned short Page_NO;
	unsigned short Block_NO;
};

struct _physic_par {
	struct _nand_partition_page phy_page;
	unsigned short page_bitmap;
	unsigned char *main_data_addr;
	unsigned char *spare_data_addr;
};

struct nand_cfg {
	unsigned int phy_interface_cfg;

	unsigned int phy_support_two_plane;
	unsigned int phy_nand_support_vertical_interleave;
	unsigned int phy_support_dual_channel;

	unsigned int phy_wait_rb_before;
	unsigned int phy_wait_rb_mode;
	unsigned int phy_wait_dma_mode;
};

#define NAND_MAX_ID_LEN 8
#define NAND_MIN_ID_LEN 4

#define OOB_BUF_SIZE 32

#define SUPPORT_SUPER_STANDBY

#define in_container_of(ptr, type, member) \
    (type *)((char *)(ptr) - (char *) &((type *)0)->member)

typedef enum _nand_if_type {
	SDR = 0,
	ONFI_DDR = 0x2,
	ONFI_DDR2 = 0x12,
	TOG_DDR = 0x3,
	TOG_DDR2 = 0x13,
} nand_if_type;

enum NDFC_ENCODE {
	BCH = 0,
	LDPC,
};

enum DDR_INFO_NO_PARAS {
	DDR_INFO_PARAS0_DEF = 0,
	DDR_INFO_PARAS1_DRV_2 = 1,
};


/*
 *struct nand_chips_ops {
 *        int (*nand_chips_init)(struct nand_chip_info *chip);
 *        void (*nand_chips_cleanup)(struct nand_chip_info *chip);
 *        int (*nand_chips_standby)(struct nand_chip_info *chip);
 *        int (*nand_chips_resume)(struct nand_chip_info *chip);
 *};
 */

/**
 * struct nand_operation_instr:
 * @read_instr: read page instr serial eg. 00h[0]-addr-30h[1]
 * @multi_plane_read_instr: read page multi plane instr serial eg. 00h-addr-32h
 *	a complete read page multi plane instr serial is:
 *	00h[0]-addr-32h[1]--00h[2]-addr-30h[3]
 * @write_instr: program page instr serial eg. 80h[0]-addr-10h[1]
 * @multi_plane_write_instr: write page multi plane instr serial eg.
 *	a complete program page multi plane instr serial is:
 *	80h[0]-addr-11h[1]--80h[2]-addr-10h[3]
 * @copy_back_read_instr: copy back read instr serial eg. 00h[0]-addr-35h[1]
 * @copy_back_multi_plane_read_instr_instr[4]:
 *	a complete copy back read page multi plane instr serial is:
 *	00h[0]-addr-32h[1]--00h[2]-addr-35h[3]
 * @copy_back_write_instr: copy back program page instr serial eg. 85h[0]-addr-10h[1]
 * @copy_back_multi_plane_write_instr_instr[4]: write page multi plane instr serial eg.
 *	a complete copy back read page multi plane instr serial is:
 *	85h[0]-addr-11h[1]--85h[2]-addr-10h[3]
 * @read_status_instr: read die status command eg. 70h, for universal operation
 * @read_status_enhanced_instr: read die statu command eg. 78h, for multi
 *				plane operation
 * @read_inter_bnk0_status_instr: inter-leave bank0 operation read status,
 *				eg. f1h
 * @read_inter_bnk1_status_instr: inter-leave bank1 operation read status,
 *				eg. f2h
 */
struct nand_operation_instr {
	unsigned char read_instr[2];
	unsigned char multi_plane_read_instr[4];
	unsigned char write_instr[2];
	unsigned char multi_plane_write_instr[4];
	unsigned char copy_back_read_instr[2];
	unsigned char copy_back_multi_plane_read_instr[4];
	unsigned char copy_back_write_instr[2];
	unsigned char copy_back_multi_plane_write_instr[4];
	unsigned char read_status_instr;
	unsigned char read_status_enhanced_instr;
	unsigned char read_inter_bnk0_status_instr;
	unsigned char read_inter_bnk1_status_instr;
};

/**
 * nand_phy_op_par: nand flash operation parameter
 * @inst: operation command
 */
struct nand_phy_op_par {
	struct nand_operation_instr instr;
};

/**
 * ndfc_init_ddr_info
 *
 */
struct ndfc_init_ddr_info {
	unsigned int en_dqs_c;
	unsigned int en_re_c;
	unsigned int odt;
	unsigned int en_ext_verf;
	unsigned int dout_re_warmup_cycle;
	unsigned int din_dqs_warmup_cycle;
	unsigned int output_driver_strength;
	unsigned int rb_pull_down_strength;
};

/*
 * Different manufacturers nand flash readretry is different
 * slc nand flash chose NAND_READRETRY_NO
 * generical mlc nand flash wanted to used readretry
 */
enum nand_readretry_type {
	NAND_READRETRY_NO = 0,
	NAND_READRETRY_HYNIX_16NM,
	NAND_READRETRY_HYNIX_20NM,
	NAND_READRETRY_HYNIX_26NM,
	NAND_READRETRY_MICRON,
	NAND_READRETRY_SAMSUNG,
	NAND_READRETRY_TOSHIBA,
	NAND_READRETRY_SANDISK,
	NAND_READRETRY_SANDISK_A19,
};

/**
 * bad block flag position
 */
typedef enum {
	FIRST_PAGE = 0,
	FIRST_TWO_PAGES,
	LAST_PAGE,
	LAST_TWO_PAGES,
} bad_position_t;

/**
 * _nand_physic_op_par:
 * @chip: chip no. begin with 0
 * @block: block no in chip. begin with 0
 * @page: page no. in one block. begin with 0
 * @sect_bitmap: req data len in sector. minimum is 2 or 4(one ecc block size,
 *						reference ndfc spec)
 * @sdata: spare data buffer
 * @seln: req spare data len
 *
 */
struct _nand_physic_op_par {
	unsigned int chip;
	unsigned int block;
	unsigned int page;
	unsigned int sect_bitmap;
	unsigned char *mdata;
	unsigned char *sdata;
	unsigned int slen;
};

/**
 * onfi_cfg_t:
 * @support_ddr2_specific_cfg:
 *	nand flash support to set ddr2 specific feature according to ONFI 3.0
 * @support_change_onfi_timing_mode:
 *	nand flash support to change timing mode according to ONFI 3.0
 * @support_io_driver_strength:
 *	nand flash support to set io driver strength according to ONFI 2.x/3.0
 * @support_rb_pull_down_strength:
 *	nand flash support to set io RB strength according to ONFI 2.x/3.0
 */
typedef struct {
	unsigned char support_ddr2_specific_cfg;
	unsigned char support_change_onfi_timing_mode;
	unsigned char support_io_driver_strength;
	unsigned char support_rb_pull_down_strength ;
} onfi_cfg_t;

/**
 * toggle_cfg_t:
 * @support_specific_setting:
 *	nand flash support to set ddr2 specific feature according to Toggle DDR2
 * @support_io_driver_strength_setting:
 *	nand flash support to set io driver strength according to Toggle DDR1/DDR2
 * @support_vendor_specific_setting:
 *	nand flash support to set vendor's specific configuration to change interface
 */
typedef struct {
	unsigned char support_specific_setting;
	unsigned char support_io_driver_strength_setting;
	unsigned char support_vendor_specific_setting;
} toggle_cfg_t;

typedef struct {
	onfi_cfg_t onfi_cfg;
	toggle_cfg_t toggle_cfg;
} itf_cfg_t;
/**
 * nand_chip_info:
 * @id: the chip identity
 * @nctri_chip_no: the chip num in nctri(channel)
 * @ blk_cnt_per_chip: the count of blocks in one chip
 * @sector_cnt_per_page: the count of sectors in one single physic page,
 *			one sector is 0.5k
 * @page_cnt_per_blk:the count of physic pages in one physic block
 * @page_offset_for_next_blk: the count of physic pages in one physic block
 * @randomizer: is open randomizer flag
 * @ecc_mode: the Ecc Mode for the nand flash chip, 0: bch-16, 1:bch-28, 2:bch_32
 * @interface_type: 0x0: sdr; 0x2: nvddr; 0x3: tgddr; 0x12: nvddr2; 0x13: tgddr2
 * @frequency: the parameter of the hardware access clock, based on 'MHz':
 * @timing_mode: current timing mode
 * @support_onfi_sync_reset: nand flash support onfi's sync reset
 * @support_toggle_only: nand flash support toggle interface only,
 *			and do not support switch between legacy and toggle
 * @ecc_sector: sector size, 512 or 1024
 * @random_cmd2_send_flag: special nand cmd for some nand in batch cmd, only for write
 * @random_addr_num: random col addr num in batch cmd
 * @nand_real_page_size: real physic page size
 * @opt_phy_op_par: the parameters for some optional operation
 * @info: device information in id table
 */
struct nand_chip_info {
	struct nand_chip_info *nsi_next;
	struct nand_chip_info *nctri_next;

	char id[8];
	unsigned int chip_no;
	unsigned int nctri_chip_no;

	unsigned int blk_cnt_per_chip;
	unsigned int sector_cnt_per_page;
	unsigned int page_cnt_per_blk;
	unsigned int page_offset_for_next_blk;

	unsigned int randomizer;
	unsigned int read_retry;
	unsigned char readretry_value[128];
	unsigned int retry_count;
	unsigned int lsb_page_type;

	unsigned int ecc_mode;
	unsigned int max_erase_times;
	unsigned int driver_no;

	unsigned int interface_type;
	unsigned int frequency;
	unsigned int timing_mode;

	unsigned int support_onfi_sync_reset;
	unsigned int support_toggle_only;
	bad_position_t bad_block_flag_position;
	unsigned int multi_plane_block_offset;
	unsigned int page_addr_bytes;

	unsigned int sdata_bytes_per_page;
	unsigned int ecc_sector;
	unsigned int random_cmd2_send_flag;
	unsigned int random_addr_num;
	unsigned int nand_real_page_size;
	unsigned int sharedpage_pairedwrite;
	unsigned int sharedpage_offset;
	itf_cfg_t itf_cfg;
	struct nand_super_chip_info *nsci;
	struct nand_controller_info *nctri;
	struct sunxi_nand_flash_device *npi;
	struct nand_phy_op_par *opt_phy_op_par;
	struct nfc_init_ddr_info *nfc_init_ddr_info;
	struct sunxi_nand_flash_device *info;
	struct nand_dev_ops *dev_ops;

	int (*nand_physic_erase_block)(struct _nand_physic_op_par *npo);
	int (*nand_physic_read_page)(struct _nand_physic_op_par *npo);
	int (*nand_physic_write_page)(struct _nand_physic_op_par *npo);
	int (*nand_physic_bad_block_check)(struct _nand_physic_op_par *npo);
	int (*nand_physic_bad_block_mark)(struct _nand_physic_op_par *npo);

	int (*nand_read_boot0_page)(struct nand_chip_info *nci, struct _nand_physic_op_par *npo);
	int (*nand_write_boot0_page)(struct nand_chip_info *nci, struct _nand_physic_op_par *npo);

	int (*nand_read_boot0_one)(unsigned char *buf, unsigned int len, unsigned int counter);
	int (*nand_write_boot0_one)(unsigned char *buf, unsigned int len, unsigned int counter);
	int (*is_lsb_page)(unsigned int page_num);
};

/*
 * define the nand flash chip information
 * @sector_cnt_per_super_page: the count of sectors in one single physic page, one sector is 0.5k
 * @page_cnt_per_super_blk: the count of super pages in one super block
 * @page_offset_for_next_super_blk: the count of siper pages in one super block
 * @spare_bytes: oob size
 * @two_plane: nsci is support two plane operation flag
 * @vertical_interleave: nsci is support vertical_interleave flag
 * @dual_channel: nsci is support dual channel flag
 * @nci_first: first nci belong to this super chip
 * @v_intl_nci_1: first nci in interleave operation
 * @v_intl_nci_2: second nci in interleave operation
 * @d_channel_nci_1: first nci in dual_channel operation
 * @d_channel_nci_2: second nci in dual_channel operation
 */
struct nand_super_chip_info {
	struct nand_super_chip_info *nssi_next;

	unsigned int chip_no;
	unsigned int blk_cnt_per_super_chip;
	unsigned int sector_cnt_per_super_page;
	unsigned int page_cnt_per_super_blk;
	unsigned int page_offset_for_next_super_blk;

	unsigned int spare_bytes;
	unsigned int channel_num;

	unsigned int two_plane;
	unsigned int vertical_interleave;
	unsigned int dual_channel;

	unsigned int driver_no;

	struct nand_chip_info *nci_first;
	struct nand_chip_info *v_intl_nci_1;
	struct nand_chip_info *v_intl_nci_2;
	struct nand_chip_info *d_channel_nci_1;
	struct nand_chip_info *d_channel_nci_2;

	int (*nand_physic_erase_super_block)(struct _nand_physic_op_par *npo);
	int (*nand_physic_read_super_page)(struct _nand_physic_op_par *npo);
	int (*nand_physic_write_super_page)(struct _nand_physic_op_par *npo);
	int (*nand_physic_super_bad_block_check)(struct _nand_physic_op_par *npo);
	int (*nand_physic_super_bad_block_mark)(struct _nand_physic_op_par *npo);
};

/* rawnand_storage_info_t
 * define the nand flash storage system information
 * @ChipCnt: the count of the total nand flash chips are currently connecting on the CE pin
 * @ChipConnectInfo: chip connect information, bit == 1 means there is a chip connecting on the CE pin
 * @RbConnectInfo: the connect  information of the all rb  chips are connected
 * @RbConnectMode: the rb connect  mode
 * @BankCntPerChip: the count of the banks in one nand chip, multiple banks can support Inter-Leave
 * @DieCntPerChip: the count of the dies in one nand chip, block management is based on Die
 * @PlaneCntPerDie: the count of planes in one die, multiple planes can support multi-plane operation
 * @SectorCntPerPage: the count of sectors in one single physic page, one sector is 0.5k
 * @PageCntPerPhyBlk: the count of physic pages in one physic block
 * @BlkCntPerDie: the count of the physic blocks in one die, include valid block and invalid block
 * @OperationOpt: the mask of the operation types which current nand flash can support support
 * @FrequencePar: the parameter of the hardware access clock, based on 'MHz'
 * @EccMode: the Ecc Mode for the nand flash chip, 0: bch-16, 1:bch-28, 2:bch_32
 * @NandChipId[8]: the nand chip id of current connecting nand chip
 * @ValidBlkRatio: the ratio of the valid physical blocks, based on 1024
 * @ReadRetryType: the read retry type
 * @DDRType: interface type (SDR/ONFI DDR/TOGGLE DDR)
 * @random_cmd2_send_flag: special nand cmd for some nand in batch cmd, only for write
 * @random_addr_num: random col addr num in batch cmd
 * @nand_real_page_size: real physic page size
 * @OptPhyOpPar: the parameters for some optional operation
 */
typedef struct rawnand_storage_info {
	unsigned int ChannelCnt;
	unsigned int ChipCnt;
	unsigned int ChipConnectInfo;
	unsigned int RbCnt;
	unsigned int RbConnectInfo;
	unsigned int RbConnectMode;
	unsigned int BankCntPerChip;
	unsigned int DieCntPerChip;
	unsigned int PlaneCntPerDie;
	unsigned int SectorCntPerPage;
	unsigned int PageCntPerPhyBlk;
	unsigned int BlkCntPerDie;
	unsigned int OperationOpt;
	unsigned int FrequencePar;
	unsigned int EccMode;
	unsigned char NandChipId[8];
	unsigned int ValidBlkRatio;
	unsigned int ReadRetryType;
	unsigned int DDRType;
	unsigned int random_cmd2_send_flag;
	unsigned int random_addr_num;
	unsigned int nand_real_page_size;
	struct nand_phy_op_par OptPhyOpPar;
} rawnand_storage_info_t;


/*
 * _nand_storage_info:
 *		define the nand flash storage system information
 * @chip_cnt: the count of chip
 */
struct _nand_storage_info {
	unsigned int chip_cnt;
	unsigned int block_nums;
	struct nand_chip_info *nci;
};

/*
 * _nand_super_storage_inf:
 *	define the nand flash storage system information
 */
struct _nand_super_storage_info {
	unsigned int super_chip_cnt;
	unsigned int super_block_nums;
	unsigned int support_two_plane;
	unsigned int support_v_interleave;
	unsigned int support_dual_channel;
	struct nand_super_chip_info *nsci;
	unsigned int plane_cnt;
};


struct onfi_ops_t {
		int (*ddr2_cfg) (struct nand_chip_info *nci);
		int (*driver_strength) (struct nand_chip_info *nci);
		int (*rb_strength) (struct nand_chip_info *nci);
		int (*timing_mode)(struct nand_chip_info *nci, nand_if_type if_type, unsigned int timing_mode);
};

struct toggle_ops_t {
		/*set to DDR / DDR2*/
		int (*specific_setting) (struct nand_chip_info *nci);
		int (*driver_strength) (struct nand_chip_info *nci);
		/*switch to sdr or ddr*/
		int (*vendor_specific_setting) (struct nand_chip_info *nci);
};

struct itf_ops_t {
		struct onfi_ops_t onfi;
		struct toggle_ops_t toggle;
};


/**
* sunxi_nand_flash_device nand flash device id structure
* @name : a human-readable name of the NAND chip
* @mfr : manufacturer ID (the first byte of nand_id[])
* @dev_id : device ID (the sencond byte of nand_id[])
* @nand_id : the ID number of the nand flash chip
* @die_cnt_per_chip :  the count of the die in one nand flash chip
* @sect_cnt_per_page : the count of the sectors in one single physical page
* @page_cnt_per_blk : the count of the pages in one single physical block
* @blk_cnt_per_die : the count fo the physical blocks in one nand flash die
* @operation_opt : the bitmap that marks which optional operation that the nand flash can support
* @valid_blk_ratio : no use
* @access_freq : the access frequence of the nand flash chip, based on mhz
* @ecc_mode : the ecc mode for the nand flash chip, 0: bch-16, 1:bch-28, 2:bch_28 3:32 4:40 5:48 6:56 7:60 8:64 9:72
* @read_retry_type : relate to readretry function
* @ddr_type : 0:sdr 1:null 2:onfi nvddr1 3:toggle ddr1 0x12: onfi ddr2 0x13: toggle ddr2
* @ddr_opt : some ddr interface param config option
* @bad_block_flag_position : manufacture bad block mark flag
* @multi_plane_block_offset : abut plane block offset
* @cmd_set_no : the flash operation command set number
* @ddr_info_no : ddr interface init information struct no.
* @selected_write_boot0_no : boot0 interface choose
* @selected_readretry_no : read retry interface choose
* @id_number : drive no
* @access_high_freq : maximum frequerence ,keep same with access_freq.
* @max_blk_erase_times : maximum erasure times
* @random_cmd2_send_flag : special nand cmd for some nand in batch cmd,only for write  ,no use ,not reflected in id table
* @random_addr_num : random col addr num in batch cmd, no use ,not reflected in id table
* @nand_real_page_size : real physic page size ,no use ,not reflected in id table
*/
struct sunxi_nand_flash_device {
	char *name;
	union {
		struct {
			unsigned char mfr_id;
			unsigned char dev_id;
		};
		unsigned char id[NAND_MAX_ID_LEN];
	};
	unsigned int die_cnt_per_chip;
	unsigned int sect_cnt_per_page;
	unsigned int page_cnt_per_blk;
	unsigned int blk_cnt_per_die;
	unsigned long long operation_opt;
	unsigned int valid_blk_ratio;
	unsigned int access_freq;
	unsigned int ecc_mode;
	unsigned int read_retry_type;
	unsigned int ddr_type;
	unsigned int ddr_opt;
	bad_position_t bad_block_flag_position;
	unsigned int multi_plane_block_offset;
	unsigned int cmd_set_no;
	unsigned int ddr_info_no;
	unsigned int selected_write_boot0_no;
	enum nand_readretry_type selected_readretry_no;
	unsigned int id_number;
	unsigned int max_blk_erase_times;
	unsigned int access_high_freq;
	unsigned int random_cmd2_send_flag;
	unsigned int random_addr_num;
	unsigned int nand_real_page_size;
	unsigned int sharedpage_offset;
};

/**
* nfc_init_ddr_info: ddr interface init's parameter
*/
struct nfc_init_ddr_info {
	unsigned int en_dqs_c;
	unsigned int en_re_c;
	unsigned int odt;
	unsigned int en_ext_verf;
	unsigned int dout_re_warmup_cycle;
	unsigned int din_dqs_warmup_cycle;
	unsigned int output_driver_strength;
	unsigned int rb_pull_down_strength;
};

struct _nand_super_block {
	unsigned short Block_NO;
	unsigned short Chip_NO;
};
/*
 * define  physical parameter to flash  128bytes
 */
#define MAGIC_DATA_FOR_PERMANENT_DATA (0xa5a5a5a5)
struct _nand_permanent_data {
	unsigned int magic_data;
	unsigned int support_two_plane;
	unsigned int support_vertical_interleave;
	unsigned int support_dual_channel;
	unsigned int reserved[64 - 4];
};

/*
 * _nand_temp_buf:
 *	define  physical temp buf
 */
struct _nand_temp_buf {

#define NUM_16K_BUF 2
#define NUM_32K_BUF 4
#define NUM_64K_BUF 1
#define NUM_NEW_BUF 4

	unsigned int used_16k[NUM_16K_BUF];
	unsigned int used_16k_flag[NUM_16K_BUF];
	unsigned char *nand_temp_buf16k[NUM_16K_BUF];

	unsigned int used_32k[NUM_32K_BUF];
	unsigned int used_32k_flag[NUM_32K_BUF];
	unsigned char *nand_temp_buf32k[NUM_32K_BUF];

	unsigned int used_64k[NUM_64K_BUF];
	unsigned int used_64k_flag[NUM_64K_BUF];
	unsigned char *nand_temp_buf64k[NUM_64K_BUF];

	unsigned int used_new[NUM_NEW_BUF];
	unsigned int used_new_flag[NUM_NEW_BUF];
	unsigned char *nand_new_buf[NUM_NEW_BUF];
};

#ifndef NULL
#define NULL (0)
#endif
#if 0
struct nand_id_tbl_info_ops {
	char *(*get_name)(struct nand_chip_info *chip);
	void (*get_id)(struct nand_chip_info *chip, unsigned char *id, int cnt);
	unsigned int (*get_die_cnt)(struct nand_chip_info *chip);
	unsigned int (*get_page_size)(struct nand_chip_info *chip,
				enum sizetype type);
	unsigned int (*get_block_size)(struct nand_chip_info *chip,
				enum sizetype type);
	unsigned int (*get_die_size)(struct nand_chip_info *chip,
				enum sizetype type);
	unsigned long long (*get_opt)(struct nand_chip_info chip);
	unsigned int (*get_freq)(struct nand_chip_info chip);
	unsigned int (*get_ecc_mode)(struct nand_chip_info chip);
	unsigned int (*get_readretry_type)(struct nand_chip_info chip);
	unsigned int (*get_ddr_type)(struct nand_chip_info chip);
	unsigned int (*get_id_number)(struct nand_chip_info chip);
	unsigned int (*get_max_erase_times)(struct nand_chip_info chip);
	unsigned int (*get_access_high_freq)(struct nand_chip_info chip);
};
#endif
extern struct itf_ops_t sansumg_itf_ops;
extern struct itf_ops_t sandisk_itf_ops;
extern struct itf_ops_t toshiba_itf_ops;
extern struct itf_ops_t hynix_itf_ops;
extern struct itf_ops_t micron_itf_ops;
extern struct itf_ops_t intel_itf_ops;

void rawnand_update_timings_ift_ops(int mfr_type);
int rawnand_async_to_onfi_ddr_or_ddr2_set(struct nand_chip_info *nci, nand_if_type ddr_type);
int rawnand_onfi_ddr_or_ddr2_to_async_set(struct nand_chip_info *nci, nand_if_type ddr_type,
	nand_if_type pre_ddr_type);
int rawnand_toggle_ddr_or_ddr2_to_async_set(struct nand_chip_info *nci, nand_if_type ddr_type,
	nand_if_type pre_ddr_type);
int rawnand_async_to_toggle_ddr_or_ddr2_set(struct nand_chip_info *nci, nand_if_type ddr_type);
int rawnand_toggle_ddr2_to_toggle_ddr_set(struct nand_chip_info *nci);
int rawnand_itf_unchanged_set(struct nand_chip_info *nci, nand_if_type ddr_type,
	nand_if_type pre_ddr_type);

int rawnand_chips_init(struct nand_chip_info *nci);
void rawnand_chip_special_init(enum nand_readretry_type type);
void rawnand_chip_special_exit(enum nand_readretry_type type);
int rawnand_sp_chips_init(struct nand_super_chip_info *nsci);
void rawnand_chips_cleanup(struct nand_chip_info *chip);
int rawnand_chips_super_standby(struct nand_chip_info *chip);
int rawnand_chips_super_resume(struct nand_chip_info *chip);
int rawnand_chips_normal_standby(struct nand_chip_info *chip);
int rawnand_chips_normal_resume(struct nand_chip_info *chip);

/* the interface for nand with nftl */
extern int nand_get_mbr(char* buffer, unsigned int len);
extern int nand_uboot_init(int boot_mode);
extern int nand_uboot_exit(int force);
extern int nand_uboot_probe(void);
extern unsigned int  nand_uboot_read_history(unsigned int start,
		unsigned int sectors, void *buffer);
extern unsigned int nand_uboot_read(unsigned int start, unsigned int sectors,
		void *buffer);
extern unsigned int nand_uboot_write(unsigned int start, unsigned int sectors,
		void *buffer);
extern int ubi_nand_get_flash_info(void *info, unsigned int len);
extern int nand_download_boot0(unsigned int length, void *buffer);
extern int nand_download_uboot(unsigned int length, void *buffer);
extern int nand_force_download_uboot(unsigned int length ,void *buffer);
extern int nand_write_boot0(void *buffer,unsigned int length);
extern int nand_read_boot0(void *buffer,unsigned int length);
extern int nand_uboot_erase(int user_erase);
extern unsigned int nand_uboot_get_flash_info(void *buffer, unsigned int length);
extern unsigned int nand_uboot_set_flash_info(void *buffer, unsigned int length);
extern unsigned int nand_uboot_get_flash_size(void);
extern int nand_uboot_flush(void);
extern int NAND_Uboot_Force_Erase(void);
extern int nand_secure_storage_read(int item, unsigned char *buf,
		unsigned int len);
extern int nand_secure_storage_write(int item, unsigned char *buf,
		unsigned int len);
extern int nand_secure_storage_fast_write(int item, unsigned char *buf,
		unsigned int len);

/* the interface for aw physic nand*/
extern int get_uboot_start_block(void);
extern int get_uboot_next_block(void);
extern int get_physic_block_reserved(void);
extern int nand_secure_storage_first_build(unsigned int start_block);
extern char *nand_get_chip_name(void);
extern void nand_get_chip_id(unsigned char *id, int cnt);
extern unsigned int nand_get_chip_die_cnt(void);
extern int nand_get_chip_page_size(enum size_type type);
extern int nand_get_chip_block_size(enum size_type type);
extern int nand_get_chip_die_size(enum size_type type);
extern unsigned long long nand_get_chip_opt(void);
extern unsigned int nand_get_chip_ddr_opt(void);
extern unsigned int nand_get_chip_ecc_mode(void);
extern unsigned int nand_get_chip_freq(void);
extern unsigned int nand_get_chip_cnt(void);
extern unsigned int  nand_get_twoplane_flag(void);
extern unsigned int nand_get_super_chip_cnt(void);
extern unsigned int nand_get_muti_program_flag(void);
extern unsigned int nand_get_support_v_interleave_flag(void);
extern unsigned int nand_get_super_chip_spare_size(void);
extern unsigned int nand_get_super_chip_page_size(void);
extern unsigned int nand_get_super_chip_block_size(void);
extern unsigned int nand_get_super_chip_pages_offset_to_block(void);
extern unsigned int nand_get_super_chip_size(void);
extern unsigned int nand_get_support_dual_channel(void);
extern unsigned int nand_get_chip_multi_plane_block_offset(void);

extern int nand_physic_read_boot0_page(unsigned int chip, unsigned int block,
		unsigned int page, unsigned int bitmap, unsigned char *mbuf,
		unsigned char *sbuf);
extern int nand_physic_read_page(unsigned int chip, unsigned int block,
		unsigned int page, unsigned int bitmap, unsigned char *mbuf,
		unsigned char *sbuf);
extern int nand_physic_write_page(unsigned int chip, unsigned int block,
		unsigned int page, unsigned int bitmap, unsigned char *mbuf,
		unsigned char *sbuf);
extern int nand_physic_erase_block(unsigned int chip, unsigned int block);
extern int nand_physic_bad_block_check(unsigned int chip, unsigned int block);

int nand_secure_storage_flush(void);


/* the interface for nand with ubi */
extern bool nand_use_ubi(void);
extern int ubi_nand_flush(void);
extern int ubi_nand_probe_uboot(void);
extern int ubi_nand_attach_mtd(void);
extern int ubi_nand_exit_uboot(int force);
extern int ubi_nand_init_uboot(int boot_mode);
extern unsigned int ubi_nand_size(void);
extern int ubi_nand_erase(int force);
extern int ubi_nand_force_erase(void);
extern int ubi_nand_read(unsigned int start, unsigned int sectors, void *buffer);
extern int ubi_nand_write(unsigned int start, unsigned int sectors, void *buf);
extern int ubi_nand_download_uboot(unsigned int len, void *buf);
extern int ubi_nand_download_boot0(unsigned int len, void *buf);
extern int ubi_nand_write_end(void);
extern int ubi_nand_update_ubi_env(void);
extern int ubi_nand_secure_storage_read(int item, void *buf, unsigned int len);
extern int ubi_nand_secure_storage_write(int item, void *buf, unsigned int len);

extern void nand_get_ubootstat(void);

extern int phy_block_is_bad_detect_by_erasechip(int chip, int block);
#endif

