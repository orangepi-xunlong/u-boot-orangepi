/*
 * rawnand.h
 *
 * Copyright (C) 2019 Allwinner.
 *
 * cuizhikui <cuizhikui@allwinnertech.com>
 *
 * SPDX-License-Identifier: GPL-2.0
 */
#ifndef __NAND_H__
#define __NAND_H__
#include <sunxi_nand.h>
//#include "rawnand/rawnand.h"

#define NAND_VERSION_0 0x03
#define NAND_VERSION_1 0x01

#define SUPPORT_SUPER_STANDBY


struct nand_chip_info;
struct _nand_info;

struct nand_chips_ops {
	int (*nand_chips_init)(struct nand_chip_info *chip);
	void (*nand_chips_cleanup)(struct nand_chip_info *chip);
	int (*nand_chips_standby)(struct nand_chip_info *chip);
	int (*nand_chips_resume)(struct nand_chip_info *chip);
};

extern char *rawnand_get_chip_name(struct nand_chip_info *chip);
extern void rawnand_get_chip_id(struct nand_chip_info *chip, unsigned char *id, int cnt);
extern unsigned int rawnand_get_chip_die_cnt(struct nand_chip_info *chip);
extern int rawnand_get_chip_page_size(struct nand_chip_info *chip,
		enum size_type type);
extern int rawnand_get_chip_block_size(struct nand_chip_info *chip,
		enum size_type type);
extern int rawnand_get_chip_block_size(struct nand_chip_info *chip,
		enum size_type type);
extern int rawnand_get_chip_die_size(struct nand_chip_info *chip,
		enum size_type type);
extern unsigned long long rawnand_get_chip_opt(struct nand_chip_info *chip);
extern unsigned int rawnand_get_chip_ecc_mode(struct nand_chip_info *chip);
extern unsigned int rawnand_get_chip_freq(struct nand_chip_info *chip);
extern unsigned int rawnand_get_chip_ddr_opt(struct nand_chip_info *chip);
extern unsigned int rawnand_get_chip_cnt(struct nand_chip_info *chip);
extern unsigned int rawnand_get_muti_program_flag(struct nand_chip_info *chip);
extern unsigned int rawnand_get_support_v_interleave_flag(struct nand_super_chip_info *schip);
extern unsigned int rawnand_get_super_chip_cnt(struct nand_super_chip_info *schip);
extern unsigned int rawnand_get_chip_multi_plane_block_offset(struct nand_chip_info *chip);
extern unsigned int rawnand_get_super_chip_spare_size(struct nand_super_chip_info *schip);
extern unsigned int rawnand_get_super_chip_page_offset_to_block(struct nand_super_chip_info *schip);
extern unsigned int rawnand_get_super_chip_page_size(struct nand_super_chip_info *schip);
extern unsigned int rawnand_get_super_chip_block_size(struct nand_super_chip_info *schip);
extern unsigned int rawnand_get_super_chip_size(struct nand_super_chip_info *schip);
extern unsigned int rawnand_get_support_dual_channel(void);


#endif /*NAND_H*/
