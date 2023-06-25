//SPDX-License-Identifier:	GPL-2.0+
/*
 ************************************************************************************************************************
 *                                                      eNand
 *                                           Nand flash driver scan module
 *
 *                             Copyright(C), 2008-2009, SoftWinners Microelectronic Co., Ltd.
 *                                                  All Rights Reserved
 *
 * File Name : nand_type.h
 *
 * Author :
 *
 * Version : v0.1
 *
 * Date : 2013-11-20
 *
 * Description :
 *
 * Others : None at present.
 *
 *
 *
 ************************************************************************************************************************
 */

#ifndef _NAND_TYPE_H_
#define _NAND_TYPE_H_

#include "nand_drv_cfg.h"

#define OOB_BUF_SIZE 32

//==============================================================================
//  define the data structure for physic layer module
//==============================================================================
typedef struct {
	unsigned int ChannelCnt;
	unsigned int
		ChipCnt; //the count of the total nand flash chips are currently connecting on the CE pin
	unsigned int
		ChipConnectInfo; //chip connect information, bit == 1 means there is a chip connecting on the CE pin
	unsigned int RbCnt;
	unsigned int
		RbConnectInfo; //the connect  information of the all rb  chips are connected
	unsigned int RbConnectMode; //the rb connect  mode
	unsigned int
		BankCntPerChip; //the count of the banks in one nand chip, multiple banks can support Inter-Leave
	unsigned int
		DieCntPerChip; //the count of the dies in one nand chip, block management is based on Die
	unsigned int
		PlaneCntPerDie; //the count of planes in one die, multiple planes can support multi-plane operation
	unsigned int
		SectorCntPerPage; //the count of sectors in one single physic page, one sector is 0.5k
	unsigned int
		PageCntPerPhyBlk; //the count of physic pages in one physic block
	unsigned int
		BlkCntPerDie; //the count of the physic blocks in one die, include valid block and invalid block
	unsigned int
		OperationOpt; //the mask of the operation types which current nand flash can support support
	unsigned int
		FrequencePar; //the parameter of the hardware access clock, based on 'MHz'
	unsigned int
		EccMode; //the Ecc Mode for the nand flash chip, 0: bch-16, 1:bch-28, 2:bch_32
	unsigned char
		NandChipId[8]; //the nand chip id of current connecting nand chip
	unsigned int
		ValidBlkRatio; //the ratio of the valid physical blocks, based on 1024
	unsigned int good_block_ratio; //good block ratio get from hwscan
	unsigned int ReadRetryType; //the read retry type
	unsigned int DDRType;
	unsigned int Reserved[32];
} boot_nand_para_t;

//==============================================================================
//define the optional physical operation parameter
//==============================================================================
struct _optional_phy_op_par {
	unsigned char multi_plane_read_cmd
		[2]; //the command for multi-plane read, the sequence is [0] -ADDR- [0] -ADDR- [1] - DATA
	unsigned char multi_plane_write_cmd
		[2]; //the command for multi-plane program, the sequence is 80 -ADDR- DATA - [0] - [1] -ADDR- DATA - 10/15
	unsigned char multi_plane_copy_read_cmd
		[3]; //the command for multi-plane page copy-back read, the sequence is [0] -ADDR- [1] -ADDR- [2]
	unsigned char multi_plane_copy_write_cmd
		[3]; //the command for multi-plane page copy-back program, the sequence is [0] -ADDR- [1] - [2] -ADDR- 10
	unsigned char
		multi_plane_status_cmd; //the command for multi-plane operation status read, the command may be 0x70/0x71/0x78/...
	unsigned char
		inter_bnk0_status_cmd; //the command for inter-leave bank0 operation status read, the command may be 0xf1/0x78/...
	unsigned char
		inter_bnk1_status_cmd; //the command for inter-leave bank1 operation status read, the command may be 0xf2/0x78/...
	unsigned char
		bad_block_flag_position; //the flag that marks the position of the bad block flag,0x00-1stpage/ 0x01-1st&2nd page/ 0x02-last page/ 0x03-last 2 page
	unsigned short
		multi_plane_block_offset; //the value of the block number offset between the left-plane block and the right pane block
};

//==============================================================================
//define  physical parameter
//==============================================================================
struct _nfc_init_ddr_info {
	unsigned int en_dqs_c;
	unsigned int en_re_c;
	unsigned int odt;
	unsigned int en_ext_verf;
	unsigned int dout_re_warmup_cycle;
	unsigned int din_dqs_warmup_cycle;
	unsigned int output_driver_strength;
	unsigned int rb_pull_down_strength;
};

struct boot_ndfc_cfg {
	u8 page_size_kb;
	u8 ecc_mode;
	u8 sequence_mode;
	u8 res[5];
};

//==============================================================================
//define the nand flash storage system information
//==============================================================================
struct __NandStorageInfo_t {
	unsigned int ChannelCnt;
	unsigned int
		ChipCnt; //the count of the total nand flash chips are currently connecting on the CE pin
	unsigned int
		ChipConnectInfo; //chip connect information, bit == 1 means there is a chip connecting on the CE pin
	unsigned int RbCnt;
	unsigned int
		RbConnectInfo; //the connect  information of the all rb  chips are connected
	unsigned int RbConnectMode; //the rb connect  mode
	unsigned int
		BankCntPerChip; //the count of the banks in one nand chip, multiple banks can support Inter-Leave
	unsigned int
		DieCntPerChip; //the count of the dies in one nand chip, block management is based on Die
	unsigned int
		PlaneCntPerDie; //the count of planes in one die, multiple planes can support multi-plane operation
	unsigned int
		SectorCntPerPage; //the count of sectors in one single physic page, one sector is 0.5k
	unsigned int
		PageCntPerPhyBlk; //the count of physic pages in one physic block
	unsigned int
		BlkCntPerDie; //the count of the physic blocks in one die, include valid block and invalid block
	unsigned int
		OperationOpt; //the mask of the operation types which current nand flash can support support
	unsigned int
		FrequencePar; //the parameter of the hardware access clock, based on 'MHz'
	unsigned int
		EccMode; //the Ecc Mode for the nand flash chip, 0: bch-16, 1:bch-28, 2:bch_32
	unsigned char
		NandChipId[8]; //the nand chip id of current connecting nand chip
	unsigned int
		ValidBlkRatio; //the ratio of the valid physical blocks, based on 1024
	unsigned int ReadRetryType; //the read retry type
	unsigned int DDRType;
	struct _optional_phy_op_par
		OptPhyOpPar; //the parameters for some optional operation
};

//==============================================================================
//define the nand flash physical information parameter type, for id table
//==============================================================================
struct _nand_phy_info_par {
	char nand_id[8]; //the ID number of the nand flash chip

	unsigned int
		die_cnt_per_chip; //the count of the die in one nand flash chip
	unsigned int
		sect_cnt_per_page; //the count of the sectors in one single physical page
	unsigned int
		page_cnt_per_blk; //the count of the pages in one single physical block
	unsigned int
		blk_cnt_per_die; //the count fo the physical blocks in one nand flash die
	unsigned int
		operation_opt; //the bitmap that marks which optional operation that the nand flash can support
	unsigned int valid_blk_ratio; //no use
	unsigned int
		access_freq; //the highest access frequence of the nand flash chip, based on mhz
	unsigned int
		ecc_mode; //the ecc mode for the nand flash chip, 0: bch-16, 1:bch-28, 2:bch_28 3:32 4:40 5:48 6:56 7:60 8:64 9:72
	unsigned int read_retry_type; //related to driver_no
	unsigned int
		ddr_type; //0:sdr 1:null 2:onfi nvddr1 3:toggle ddr1 0x12: onfi ddr2 0x13: toggle ddr2
	unsigned int
		option_phyisc_op_no; //the pointer point to the optional operation parameter
	unsigned int ddr_info_no;
	unsigned int id_number;
	unsigned int max_blk_erase_times;
	unsigned int driver_no;
	unsigned int access_high_freq;
};

//==============================================================================
//define the paramter structure for physic operation function
//==============================================================================
struct _nand_physic_op_par {
	unsigned int chip;
	unsigned int block;
	unsigned int page;
	unsigned int sect_bitmap;
	unsigned char *mdata;
	unsigned char *sdata;
	unsigned int slen;
};

struct _nand_chip_info;

//==============================================================================

//==============================================================================
struct _nand_physic_function {
	int (*nand_physic_erase_block)(struct _nand_physic_op_par *npo);
	int (*nand_physic_read_page)(struct _nand_physic_op_par *npo);
	int (*nand_physic_write_page)(struct _nand_physic_op_par *npo);
	int (*nand_physic_bad_block_check)(struct _nand_physic_op_par *npo);
	int (*nand_physic_bad_block_mark)(struct _nand_physic_op_par *npo);

	int (*nand_physic_erase_super_block)(struct _nand_physic_op_par *npo);
	int (*nand_physic_read_super_page)(struct _nand_physic_op_par *npo);
	int (*nand_physic_write_super_page)(struct _nand_physic_op_par *npo);
	int (*nand_physic_super_bad_block_check)(
			struct _nand_physic_op_par *npo);
	int (*nand_physic_super_bad_block_mark)(struct _nand_physic_op_par *npo);

	int (*nand_read_boot0_page)(struct _nand_chip_info *nci,
			struct _nand_physic_op_par *npo);
	int (*nand_write_boot0_page)(struct _nand_chip_info *nci,
			struct _nand_physic_op_par *npo);

	int (*nand_read_boot0_one)(unsigned char *buf, unsigned int len,
			unsigned int counter);
	int (*nand_write_boot0_one)(unsigned char *buf, unsigned int len,
			unsigned int counter);

	int (*nand_physic_special_init)(void);
	int (*nand_physic_special_exit)(void);
	int (*is_lsb_page)(__u32 page_num);
};

//==============================================================================
//define the nand flash chip information
//==============================================================================
struct _nand_chip_info {
	struct _nand_chip_info *nsi_next;
	struct _nand_chip_info *nctri_next;

	char id[8];
	unsigned int chip_no;
	unsigned int nctri_chip_no;

	unsigned int blk_cnt_per_chip;
	unsigned int
		sector_cnt_per_page; //the count of sectors in one single physic page, one sector is 0.5k
	unsigned int
		page_cnt_per_blk; //the count of physic pages in one physic block
	unsigned int
		page_offset_for_next_blk; //the count of physic pages in one physic block

	unsigned int randomizer;
	unsigned int read_retry;
	unsigned char readretry_value[128];
	unsigned int retry_count;
	unsigned int lsb_page_type;

	unsigned int
		ecc_mode; //the Ecc Mode for the nand flash chip, 0: bch-16, 1:bch-28, 2:bch_32
	unsigned int max_erase_times;
	unsigned int driver_no;

	unsigned int
		interface_type; //0x0: sdr; 0x2: nvddr; 0x3: tgddr; 0x12: nvddr2; 0x13: tgddr2
	unsigned int
		frequency; //the parameter of the hardware access clock, based on 'MHz'
	unsigned int timing_mode; //current timing mode

	unsigned int
		support_change_onfi_timing_mode; //nand flash support to change timing mode according to ONFI 3.0
	unsigned int
		support_ddr2_specific_cfg; //nand flash support to set ddr2 specific feature according to ONFI 3.0 or Toggle DDR2
	unsigned int
		support_io_driver_strength; //nand flash support to set io driver strength according to ONFI 2.x/3.0 or Toggle DDR1/DDR2
	unsigned int
		support_vendor_specific_cfg; //nand flash support to set vendor's specific configuration
	unsigned int
		support_onfi_sync_reset; //nand flash support onfi's sync reset
	unsigned int
		support_toggle_only; //nand flash support toggle interface only, and do not support switch between legacy and toggle

	unsigned int page_addr_bytes;

	unsigned int sdata_bytes_per_page;
	unsigned int ecc_sector; //0:512 1:1024

	struct _nand_super_chip_info *nsci;
	struct _nand_controller_info *nctri;
	struct _nand_phy_info_par *npi;
	struct _optional_phy_op_par
		*opt_phy_op_par; //the parameters for some optional operation
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

//==============================================================================
//define the nand flash chip information
//==============================================================================
struct _nand_super_chip_info {
	struct _nand_super_chip_info *nssi_next;

	unsigned int chip_no;
	unsigned int blk_cnt_per_super_chip;
	unsigned int
		sector_cnt_per_super_page; //the count of sectors in one single physic page, one sector is 0.5k
	unsigned int
		page_cnt_per_super_blk; //the count of physic pages in one physic block
	unsigned int
		page_offset_for_next_super_blk; //the count of physic pages in one physic block
	//    unsigned int       multi_plane_block_offset;

	unsigned int spare_bytes;
	unsigned int channel_num;

	unsigned int two_plane; //if nsci support two_plane
	unsigned int vertical_interleave; //if nsci support vertical_interleave
	unsigned int dual_channel; //if nsci support dual_channel

	unsigned int driver_no;

	struct _nand_chip_info *nci_first; //first nci belong to this super chip
	struct _nand_chip_info *v_intl_nci_1; //first nci in interleave operation
	struct _nand_chip_info *v_intl_nci_2; //second nci in interleave operation
	struct _nand_chip_info
		*d_channel_nci_1; //first nci in dual_channel operation
	struct _nand_chip_info
		*d_channel_nci_2; //second nci in dual_channel operation

	int (*nand_physic_erase_super_block)(struct _nand_physic_op_par *npo);
	int (*nand_physic_read_super_page)(struct _nand_physic_op_par *npo);
	int (*nand_physic_write_super_page)(struct _nand_physic_op_par *npo);
	int (*nand_physic_super_bad_block_check)(
			struct _nand_physic_op_par *npo);
	int (*nand_physic_super_bad_block_mark)(struct _nand_physic_op_par *npo);
};

//==============================================================================
//define the nand flash storage system information
//==============================================================================
struct _nand_storage_info {
	unsigned int chip_cnt;
	unsigned int block_nums;
	struct _nand_chip_info *nci;
};

//==============================================================================
//define the nand flash storage system information
//==============================================================================
struct _nand_super_storage_info {
	unsigned int super_chip_cnt;
	unsigned int super_block_nums;
	unsigned int support_two_plane;
	unsigned int support_v_interleave;
	unsigned int support_dual_channel;
	struct _nand_super_chip_info *nsci;
};

//==============================================================================
//define  physical parameter to flash  128bytes
//==============================================================================
#define MAGIC_DATA_FOR_PERMANENT_DATA (0xa5a5a5a5)
struct _nand_permanent_data {
	unsigned int magic_data;
	unsigned int support_two_plane;
	unsigned int support_vertical_interleave;
	unsigned int support_dual_channel;
	unsigned int reserved[64 - 4];
};

//==============================================================================
//define  physical temp buf
//==============================================================================
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

//define the mask for the nand flash optional operation
#define NAND_CACHE_READ (1 << 0) //nand flash support cache read operation
#define NAND_CACHE_PROGRAM                                                     \
	(1 << 1) //nand flash support page cache program operation
#define NAND_MULTI_READ                                                        \
	(1 << 2) //nand flash support multi-plane page read operation
#define NAND_MULTI_PROGRAM                                                     \
	(1 << 3) //nand flash support multi-plane page program operation
#define NAND_PAGE_COPYBACK                                                     \
	(1 << 4) //nand flash support page copy-back command mode operation
#define NAND_INT_INTERLEAVE                                                    \
	(1                                                                     \
	 << 5) //nand flash support internal inter-leave operation, it based multi-bank
#define NAND_EXT_INTERLEAVE                                                    \
	(1                                                                     \
	 << 6) //nand flash support external inter-leave operation, it based multi-chip
#define NAND_RANDOM (1 << 7) //nand flash support RANDOMIZER
#define NAND_READ_RETRY (1 << 8) //nand falsh support READ RETRY
#define NAND_READ_UNIQUE_ID (1 << 9) //nand falsh support READ UNIQUE_ID
#define NAND_PAGE_ADR_NO_SKIP                                                  \
	(1 << 10) //nand falsh page adr no skip is requiered
#define NAND_DIE_SKIP (1 << 11) //nand flash die adr skip
//#define NAND_LOG_BLOCK_MANAGE   (1<<12)             //nand flash log block type management
//#define NAND_FORCE_WRITE_SYNC    (1<<13)             //nand flash need check status after write or erase
//#define NAND_LOG_BLOCK_LSB_TYPE (0xff<<16)          //nand flash log block lsb page type
#define NAND_LSB_PAGE_TYPE (0xff << 12) //nand flash lsb page type....
#define NAND_MAX_BLK_ERASE_CNT                                                 \
	(1 << 20) //nand flash support the maximum block erase cnt
#define NAND_READ_RECLAIM                                                      \
	(1 << 21) //nand flash support to read reclaim Operation

#define NAND_TIMING_MODE                                                       \
	(1                                                                     \
	 << 24) //nand flash support to change timing mode according to ONFI 3.0
#define NAND_DDR2_CFG                                                          \
	(1                                                                     \
	 << 25) //nand flash support to set ddr2 specific feature according to ONFI 3.0 or Toggle DDR2
#define NAND_IO_DRIVER_STRENGTH                                                \
	(1                                                                     \
	 << 26) //nand flash support to set io driver strength according to ONFI 2.x/3.0 or Toggle DDR1/DDR2
#define NAND_VENDOR_SPECIFIC_CFG                                               \
	(1 << 27) //nand flash support to set vendor's specific configuration
#define NAND_ONFI_SYNC_RESET_OP (1 << 28) //nand flash support onfi's sync reset

#define NAND_TOGGLE_ONLY                                                       \
	(1                                                                     \
	 << 29) //nand flash support toggle interface only, and do not support switch between legacy and toggle
#define NAND_WITH_TWO_ROW_ADR (1 << 30) //nand flash has two row address only
#define NAND_PAIRED_PAGE_SYNC                                                  \
	(1                                                                     \
	 << 31) //nand flash support cache for lsb page data untill shared page data is writed. (only for 3D nand)

//define the return value

#define NAND_OP_TRUE (0) //define the successful return value
#define NAND_OP_FALSE (-1) //define the failed return value

#define ECC_LIMIT 10 //reach the limit of the ability of ECC
#define ERR_ECC (-2) //too much ecc error
#define ERR_TIMEOUT (-3) //hardware timeout
#define ERR_PARA (-4)

#define ERR_NO_10 (-10)
#define ERR_NO_11 (-11)
#define ERR_NO_12 (-12)
#define ERR_NO_13 (-13)
#define ERR_NO_14 (-14)
#define ERR_NO_15 (-15)
#define ERR_NO_16 (-16)
#define ERR_NO_17 (-17)
#define ERR_NO_18 (-18)
#define ERR_NO_19 (-19)
#define ERR_NO_20 (-20)
#define ERR_NO_21 (-21)
#define ERR_NO_22 (-22)
#define ERR_NO_23 (-23)
#define ERR_NO_24 (-24)
#define ERR_NO_25 (-25)
#define ERR_NO_26 (-26)
#define ERR_NO_27 (-27)
#define ERR_NO_28 (-28)
#define ERR_NO_29 (-29)
#define ERR_NO_30 (-30)
#define ERR_NO_31 (-31)
#define ERR_NO_32 (-32)
#define ERR_NO_33 (-33)
#define ERR_NO_34 (-34)
#define ERR_NO_35 (-35)
#define ERR_NO_36 (-36)
#define ERR_NO_37 (-37)
#define ERR_NO_38 (-38)
#define ERR_NO_39 (-39)
#define ERR_NO_40 (-40)
#define ERR_NO_41 (-41)
#define ERR_NO_42 (-42)
#define ERR_NO_43 (-43)
#define ERR_NO_44 (-44)
#define ERR_NO_45 (-45)
#define ERR_NO_46 (-46)
#define ERR_NO_47 (-47)
#define ERR_NO_48 (-48)
#define ERR_NO_49 (-49)
#define ERR_NO_50 (-50)
#define ERR_NO_51 (-51)
#define ERR_NO_52 (-52)
#define ERR_NO_53 (-53)
#define ERR_NO_54 (-54)
#define ERR_NO_55 (-55)
#define ERR_NO_56 (-56)
#define ERR_NO_57 (-57)
#define ERR_NO_58 (-58)
#define ERR_NO_59 (-59)
#define ERR_NO_60 (-60)
#define ERR_NO_61 (-61)
#define ERR_NO_62 (-62)
#define ERR_NO_63 (-63)
#define ERR_NO_64 (-64)
#define ERR_NO_65 (-65)
#define ERR_NO_66 (-66)
#define ERR_NO_67 (-67)
#define ERR_NO_68 (-68)
#define ERR_NO_69 (-69)
#define ERR_NO_70 (-70)
#define ERR_NO_71 (-71)
#define ERR_NO_72 (-72)
#define ERR_NO_73 (-73)
#define ERR_NO_74 (-74)
#define ERR_NO_75 (-75)
#define ERR_NO_76 (-76)
#define ERR_NO_77 (-77)
#define ERR_NO_78 (-78)
#define ERR_NO_79 (-79)
#define ERR_NO_80 (-80)
#define ERR_NO_81 (-81)
#define ERR_NO_82 (-82)
#define ERR_NO_83 (-83)
#define ERR_NO_84 (-84)
#define ERR_NO_85 (-85)
#define ERR_NO_86 (-86)
#define ERR_NO_87 (-87)
#define ERR_NO_88 (-88)
#define ERR_NO_89 (-89)
#define ERR_NO_90 (-90)
#define ERR_NO_91 (-91)
#define ERR_NO_92 (-92)
#define ERR_NO_93 (-93)
#define ERR_NO_94 (-94)
#define ERR_NO_95 (-95)
#define ERR_NO_96 (-96)
#define ERR_NO_97 (-97)
#define ERR_NO_98 (-98)
#define ERR_NO_99 (-99)
#define ERR_NO_100 (-100)
#define ERR_NO_101 (-101)
#define ERR_NO_102 (-102)
#define ERR_NO_103 (-103)
#define ERR_NO_104 (-104)
#define ERR_NO_105 (-105)
#define ERR_NO_106 (-106)
#define ERR_NO_107 (-107)
#define ERR_NO_108 (-108)
#define ERR_NO_109 (-109)
#define ERR_NO_110 (-110)
#define ERR_NO_111 (-111)
#define ERR_NO_112 (-112)
#define ERR_NO_113 (-113)
#define ERR_NO_114 (-114)
#define ERR_NO_115 (-115)
#define ERR_NO_116 (-116)
#define ERR_NO_117 (-117)
#define ERR_NO_118 (-118)
#define ERR_NO_119 (-119)
#define ERR_NO_120 (-120)
#define ERR_NO_121 (-121)
#define ERR_NO_122 (-122)
#define ERR_NO_123 (-123)
#define ERR_NO_124 (-124)
#define ERR_NO_125 (-125)
#define ERR_NO_126 (-126)
#define ERR_NO_127 (-127)
#define ERR_NO_128 (-128)
#define ERR_NO_129 (-129)
#define ERR_NO_130 (-130)
#define ERR_NO_131 (-131)
#define ERR_NO_132 (-132)
#define ERR_NO_133 (-133)

#endif //_NAND_TYPE_H_
