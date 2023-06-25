/**
 * SPDX-License-Identifier: GPL-2.0+
 */
#ifndef __AW_UBI_H__
#define __AW_UBI_H__

/* convert APIs */
#define to_sects(size)	((size) >> 9)
#define to_bytes(size)	((size) << 9)
#define MTD_W_BUF_SIZE		(64 * 1024)
#define MBR_SECTORS	to_sects(SUNXI_MBR_SIZE * SUNXI_MBR_COPY_NUM)

#define NAND_MAX_PART_CNT	(4 * 24)
#define PART_NAME_MAX_SIZE	16


struct ubi_mtd_part {
	int partno;
	char name[PART_NAME_MAX_SIZE];
	unsigned int offset;
	unsigned int bytes;
	unsigned int plan_wr_sects;
	unsigned int written_sects;
};

struct ubi_mtd_info {
	char wbuf[MTD_W_BUF_SIZE];
	int last_partno;
	unsigned int pagesize;
	unsigned int blksize;
	unsigned int total_bytes;
	unsigned int part_cnt;
	struct ubi_mtd_part part[NAND_MAX_PART_CNT];
};

int mtd_set_last_vol_sects(unsigned int sects);

#ifdef CONFIG_AW_MTD_RAWNAND

int rawnand_ubi_user_volumes_size(void);
int rawnand_mtd_flush_last_volume(void);
unsigned int rawnand_mtd_write_ubi(unsigned int start, unsigned int sectors,
		void *buffer);
unsigned int rawnand_mtd_read_ubi(unsigned int start, unsigned int sectors,
		void *buffer);


#endif




#endif /*AW_UBI_H*/
