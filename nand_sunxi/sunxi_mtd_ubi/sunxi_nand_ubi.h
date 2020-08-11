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
#ifndef __SUNXI_UBI_H__
#define __SUNXI_UBI_H__

#include <common.h>
#include <malloc.h>
#include <sunxi_mbr.h>
#include "sunxi_nand_mtd.h"
#include <../../../fs/ubifs/ubifs-media.h>

#define EN_BURN_MAX_LEN     1
#define MTD_W_BUF_SIZE      (64 * 1024)

/* 128 sectors */
#define GAP_BUF_MAX_SIZE    (128)

#define CREATE_UBI_VOL      1
#define WRITTEN_UBI_VOL     2

/* for partition */
#define MAX_PART_COUNT_PER_FTL  24
#define MAX_PARTITION           4
#define ND_MAX_PARTITION_COUNT  (MAX_PART_COUNT_PER_FTL*MAX_PARTITION)

/* part info */
typedef struct _NAND_PARTITION {
	unsigned  char		classname[16];
	unsigned  int		addr;
	unsigned  int		len;
	unsigned  int		user_type;
	unsigned  int		keydata;
	unsigned  int		ro;
} NAND_PARTITION;	 /* 36bytes */

/* mbr info */
typedef struct _PARTITION_MBR {
	unsigned  int	CRC;
	unsigned  int	PartCount;
	NAND_PARTITION	array[ND_MAX_PARTITION_COUNT];
} PARTITION_MBR;


typedef struct _UBI_PART_STATUS {
	/* w_state -- 0: initial ; 1:create vol; 2:writing vol */
	int		w_state;

	/* type -- 0: dynamic(default); 1:static. */
	int		type;

	uint32	plan_wr_size;		/* sectors */
	uint32	written_size;		/* sectors */
} UBI_PART_STATUS;

typedef struct _UBI_ATTATCH_MTD {
	u16 num;
	char *name;
	u64 size;
} UBI_ATTATCH_MTD;

typedef struct _UBI_MBR_STATUS {
	UBI_ATTATCH_MTD ath_mtd;
	int last_partnum;
	UBI_PART_STATUS part[ND_MAX_PARTITION_COUNT];
} UBI_MBR_STATUS;

typedef struct _MTD_PART {
	u16 num;        /* mtdx */
	char name[16];
	u64 offset;
	u64 size;
	u32 plan_wr_size;
	u32 written_size;
} MTD_PART;

typedef struct _MTD_MBR {
	char *w_buf;
	int last_partnum;
	u64 pagesize;
	u64 blksize;
	u16 part_cnt;
	MTD_PART part[ND_MAX_PARTITION_COUNT];
} MTD_MBR;

typedef struct _UBI_MBR {
	PARTITION_MBR n_mbr;        /* nand partition mbr */
	PARTITION_MBR u_mbr;        /* ubi volume mbr */
	UBI_MBR_STATUS u_status;    /* ubi volume status */
	MTD_MBR mtd_mbr;            /* mtd partition mbr */
} UBI_MBR;

extern int sunxi_mtd_read(loff_t from, size_t len, size_t *retlen, u_char *buf);
extern int sunxi_mtd_write(loff_t to, size_t len, size_t *retlen, u_char *buf);
extern int mtdparts_init(void);
extern int sunxi_do_ubi(int flag, int argc, char * const argv[]);
extern int sunxi_ubi_volume_read(char *volume, loff_t offp,
	char *buf, size_t size);
extern int sunxi_do_mtdparts(int flag, int argc, char * const argv[]);
extern char *sunxi_get_mtdparts_name(u16 mtd_partnum);
extern u64 sunxi_get_mtdpart_size(u16 mtd_partnum);
extern u64 sunxi_get_mtdpart_offset(u16 mtd_partnum);
extern u16 sunxi_get_mtd_num_parts(void);
extern uint64_t get_mtd_size(void);
extern uint32_t get_mtd_blksize(void);
extern uint32_t get_mtd_pgsize(void);

uint sunxi_nand_mtd_ubi_cap(void);
uint sunxi_nand_mtd_ubi_write(uint start, uint sectors, void *buffer);
uint sunxi_nand_mtd_ubi_read(uint start, uint sectors, void *buffer);
int sunxi_chk_ubifs_sb(void *sb_buf);

extern PARTITION_MBR nand_mbr;

#endif
