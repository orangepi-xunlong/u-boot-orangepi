/* SPDX-License-Identifier: GPL-2.0*/

#ifndef __RAWNAND_READRETRY_H__
#define __RAWNAND_READRETRY_H__
//#include "rawnand.h"
#include <sunxi_nand.h>

typedef unsigned char uint8_t;

extern uint8_t hynix16nm_read_retry_mode;
extern uint8_t hynix20nm_read_retry_mode;

extern int generic_special_init(void);
extern int generic_special_exit(void);
extern int generic_is_lsb_page(unsigned int page_num);
extern int hynix16nm_write_page_FF(struct _nand_physic_op_par *npo, unsigned int page_size_k);
extern signed int hynix16nm_set_default_param(struct nand_chip_info *nci);
extern int hynix16nm_special_init(void);
extern int hynix16nm_special_exit(void);
extern int hynix16nm_is_lsb_page(unsigned int page_num);
extern int hynix20nm_lsb_init(struct nand_chip_info *nci);
extern int hynix20nm_lsb_enable(struct nand_chip_info *nci);
extern int hynix20nm_lsb_disable(struct nand_chip_info *nci);
extern int hynix20nm_lsb_exit(struct nand_chip_info *nci);
extern signed int hynix20nm_set_default_param(struct nand_chip_info *nci);
extern int hynix20nm_special_init(void);
extern int hynix20nm_special_exit(void);
extern int hynix20nm_is_lsb_page(unsigned int page_num);
extern signed int hynix26nm_lsb_init(struct nand_chip_info *nci);
extern signed int hynix26nm_lsb_enable(struct nand_chip_info *nci);
extern signed int hynix26nm_lsb_disable(struct nand_chip_info *nci);
extern signed int hynix26nm_lsb_exit(struct nand_chip_info *nci);
extern int hynix26nm_special_init(void);
extern int hynix26nm_special_exit(void);
extern int hynix26nm_is_lsb_page(unsigned int page_num);
extern signed int micron_readretry_init(struct nand_chip_info *nci);
extern signed int micron_readretry_exit(struct nand_chip_info *nci);
extern signed int micron_set_readretry(struct nand_chip_info *nci);
extern int micron_special_init(void);
extern int micron_special_exit(void);
extern int micron_0x40_is_lsb_page(unsigned int page_num);
extern int micron_0x41_is_lsb_page(unsigned int page_num);
extern int micron_0x42_is_lsb_page(unsigned int page_num);
extern int samsung_special_init(void);
extern int samsung_special_exit(void);
extern int samsung_is_lsb_page(unsigned int page_num);
extern int sandisk_A19_special_init(void);
extern int sandisk_A19_special_exit(void);
extern int sandisk_A19_check_bad_block_first_burn(struct _nand_physic_op_par *npo);
extern int sandisk_special_init(void);
extern int sandisk_special_exit(void);
extern int sandisk_is_lsb_page(unsigned int page_num);
extern int toshiba_special_init(void);
extern int toshiba_special_exit(void);
extern int toshiba_is_lsb_page(unsigned int page_num);

#if 0
typedef int (*lsb_page_t)(unsigned int no);
extern lsb_page_t chose_lsb_func(unsigned int no);
#endif

struct rawnand_readretry {
	int (*nand_readretry_op_init)(void);
	int (*nand_readretry_op_exit)(void);
};
extern struct rawnand_readretry rawnand_readretry[];
#endif /*RAWNAND_READRETRY_H*/
