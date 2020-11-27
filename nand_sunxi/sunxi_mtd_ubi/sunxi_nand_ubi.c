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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.
 */
#include "sunxi_nand_ubi.h"

#ifdef CONFIG_SUNXI_UBIFS

UBI_MBR ubi_mbr;
char sunxi_mtd_w_buf[MTD_W_BUF_SIZE];

static unsigned int y = 1U;

unsigned int rand_r(unsigned int *seedp)
{
	*seedp ^= (*seedp << 13);
	*seedp ^= (*seedp >> 17);
	*seedp ^= (*seedp << 5);

	return *seedp;
}

unsigned int rand(void)
{
	return rand_r(&y);
}

void srand(unsigned int seed)
{
	y = seed;
}

static void randomize(unsigned int *buf, unsigned int len, int seed)
{
	int i;

	srand(seed);
	for (i = 0; i < len; i++)
		*buf++ = rand();
}

static int get_partno_in_mbr(PARTITION_MBR *nand_mbr, uint lba, uint sec_cnt)
{
	int i;
	uint addr, len;

	if (!nand_mbr->PartCount) {
		printf("Warning: nand mbr does not exist. lba = 0x%0x, sec_cnt = 0x%0x, %d, %s\n",
			lba, sec_cnt, __LINE__, __func__);
		return -1;
	}

	for (i = 0; i < nand_mbr->PartCount; i++) {
		addr = nand_mbr->array[i].addr;
		len = nand_mbr->array[i].len;
		if ((lba >= addr) && (lba <= (addr + len)) &&
			((lba + sec_cnt) <= (addr + len)))
			return i;
	}

	printf("Err: get partno err !\n");
	printf("lba=%0x, sec_cnt=%0x\n", lba, sec_cnt);
	printf("total part: %d\n", nand_mbr->PartCount);
	printf("part_no addr len user_type part_name\n");
	for (i = 0; i < nand_mbr->PartCount; i++) {
		printf("%d, 0x%x, 0x%x, 0x%x, %s\n",
			i, nand_mbr->array[i].addr,
			nand_mbr->array[i].len,
			nand_mbr->array[i].user_type,
			nand_mbr->array[i].classname);
	}

	return -1;
}

static uint get_offset_in_partition(PARTITION_MBR *nand_mbr,
	uint partno, uint addr)
{
	return addr - nand_mbr->array[partno].addr;
}

static int sunxi_init_nand_mbr(PARTITION_MBR *input_mbr,
	char *in_buf, PARTITION_MBR *output_mbr)
{
	int i;
	sunxi_mbr_t *rd_nand_mbr = (sunxi_mbr_t *)in_buf;
	PARTITION_MBR *o_mbr = output_mbr;
	PARTITION_MBR *i_mbr = input_mbr;

	memset(o_mbr, 0x00, sizeof(PARTITION_MBR));

	if (in_buf) {
		if (!strncmp((const char *)rd_nand_mbr->magic,
			SUNXI_MBR_MAGIC, 8)) {
			int crc = 0;
			crc = crc32(0,
				(const unsigned char *)&rd_nand_mbr->version,
				SUNXI_MBR_SIZE - 4);
			if (crc != rd_nand_mbr->crc32) {
				printf("Err: crc verify err !\n");
				printf("cal_crc=%0x, rd_crc=%0x\n",
					crc, rd_nand_mbr->crc32);
				return -1;
			}
		}

		if (!rd_nand_mbr->PartCount)
			return -1;

		/* the first part is MBR */
		o_mbr->PartCount = 1;
		o_mbr->array[0].addr = 0x00;
		o_mbr->array[0].len = rd_nand_mbr->array[0].addrlo;
		strcpy((char *)o_mbr->array[0].classname, "mbr");

		o_mbr->PartCount += rd_nand_mbr->PartCount;

		for (i = 0; i < rd_nand_mbr->PartCount; i++) {
			o_mbr->array[i + 1].addr = rd_nand_mbr->array[i].addrlo;
			o_mbr->array[i + 1].len = rd_nand_mbr->array[i].lenlo;
			strncpy((char *)o_mbr->array[i + 1].classname,
				(const char *)rd_nand_mbr->array[i].name, 16);
		}
	} else {
		if (i_mbr == NULL)
			return -1;

		o_mbr->PartCount = i_mbr->PartCount;

		for (i = 0; i < o_mbr->PartCount; i++) {
			o_mbr->array[i].addr = i_mbr->array[i].addr;
			o_mbr->array[i].len = i_mbr->array[i].len;
			strncpy((char *)o_mbr->array[i].classname,
				(const char *)i_mbr->array[i].classname, 16);
		}
	}
	return 0;
}

static int sunxi_find_nand_mbr_num(PARTITION_MBR *nand_mbr, char *name)
{
	int i;

	if (name == NULL)
		return -1;

	for (i = 0; i < nand_mbr->PartCount; i++) {
		if (!strcmp((char *)nand_mbr->array[i].classname, name))
			return i;
	}
	return -1;
}

static int sunxi_find_ubi_vol_num(PARTITION_MBR *ubi_mbr, char *name)
{
	int i;

	if (name == NULL)
		return -1;

	for (i = 0; i < ubi_mbr->PartCount; i++) {
		if (!strcmp((char *)ubi_mbr->array[i].classname, name))
			return i;
	}
	return -1;
}

static int sunxi_find_mtd_num(MTD_MBR *mtd_mbr, char *name)
{
	int i;

	if (name == NULL)
		return -1;

	for (i = 0; i < mtd_mbr->part_cnt; i++)
		if (!strcmp(mtd_mbr->part[i].name, name))
			return i;

	return -1;
}

static int get_ubi_attach_mtd_num(void)
{
	int num;

	num = sunxi_get_mtd_num_parts();
	num = num ? num - 1 : -1;

	debug("ubi attach the last mtdx, mtdx_num = %d\n", num);

	return num;
}

static void sunxi_init_partition_wr_size(UBI_MBR *ubi_mbr)
{
	/*
	 * mbr, bootloader, env, boot, system, misc, revovery
	 * u32 total_wr_len[64] = {0x0080, 0x41da, 0x100, 0x9324,  \
	 * 0x0000, 0x0000, 0x9d34};
	 */

#ifndef EN_BURN_MAX_LEN
	u32 total_wr_len[64] = {0};
#endif

	int i;
	int mtd_num, ubi_num;
	u32	len;
	char	*name = NULL;
	MTD_MBR	*mtd_mbr;
	PARTITION_MBR	*u_mbr;
	PARTITION_MBR	*n_mbr;
	UBI_MBR_STATUS	*u_status;

	u_mbr = &ubi_mbr->u_mbr;
	u_status = &ubi_mbr->u_status;
	mtd_mbr = &ubi_mbr->mtd_mbr;
	n_mbr = &ubi_mbr->n_mbr;

	for (i = 0; i < n_mbr->PartCount; i++) {
		name = (char *)n_mbr->array[i].classname;
		mtd_num = sunxi_find_mtd_num(mtd_mbr, name);
		ubi_num = sunxi_find_ubi_vol_num(u_mbr, name);

#ifndef EN_BURN_MAX_LEN
		len = n_mbr->array[i].len;
		len = total_wr_len[i] ? MIN(len, total_wr_len[i]) : len;
#endif

		if (mtd_num != -1) {

#ifdef EN_BURN_MAX_LEN
			len = mtd_mbr->part[mtd_num].size;
#endif

			mtd_mbr->part[mtd_num].plan_wr_size = len;
			mtd_mbr->part[mtd_num].written_size = 0;
		} else if (ubi_num != -1) {

#ifdef EN_BURN_MAX_LEN
			len = u_mbr->array[ubi_num].len;
#endif

			u_status->part[ubi_num].plan_wr_size = len;
			u_status->part[ubi_num].written_size = 0;
		} else {
			printf("Warning:%s is not in mtd/ubi partition!\n",
				name);
		}
	}

}

static void sunxi_init_mtd_info(MTD_MBR *mtd_mbr)
{
	int i;
	char *name;

	memset(mtd_mbr, 0x00, sizeof(MTD_MBR));

	mtd_mbr->w_buf = sunxi_mtd_w_buf;
	mtd_mbr->last_partnum = -1;
	mtd_mbr->pagesize =  get_mtd_pgsize();
	mtd_mbr->blksize = get_mtd_blksize();
	mtd_mbr->part_cnt = sunxi_get_mtd_num_parts();

	for (i = 0; i < mtd_mbr->part_cnt; i++) {
		mtd_mbr->part[i].num = i;
		name = sunxi_get_mtdparts_name(i);
		strncpy(mtd_mbr->part[i].name, (const char *)name, 16);
		mtd_mbr->part[i].offset = sunxi_get_mtdpart_offset(i);
		mtd_mbr->part[i].size = sunxi_get_mtdpart_size(i);
	}
}

static void init_sunxi_mtd_ubi_mbr(PARTITION_MBR *nand_mbr, UBI_MBR *ubi_mbr)
{
	int i, k;
	int attatch_mtd_num;
	uint64_t n_cur_part_len;
	uint64_t leb_size;		/* assigned by sectors */
	uint32_t ubi_min_volsize;	/* assigned by sectors */
	PARTITION_MBR	*u_mbr;
	UBI_MBR_STATUS	*u_status;
	MTD_MBR		*mtd_mbr;

	u_mbr = &ubi_mbr->u_mbr;
	u_status = &ubi_mbr->u_status;
	mtd_mbr = &ubi_mbr->mtd_mbr;

	memset(u_mbr, 0x00, sizeof(PARTITION_MBR));
	memset(u_status, 0x00, sizeof(UBI_MBR_STATUS));
	u_status->last_partnum = -1;

	attatch_mtd_num = get_ubi_attach_mtd_num();
	if ((attatch_mtd_num != -1) && nand_mbr->PartCount) {
		/* ubi attatch the last mtdx. */
		u_status->ath_mtd.num = attatch_mtd_num;
		strcpy(u_status->ath_mtd.name,
			mtd_mbr->part[attatch_mtd_num].name);
		u_status->ath_mtd.size = mtd_mbr->part[attatch_mtd_num].size;
	}

	leb_size = (mtd_mbr->blksize - 2 * mtd_mbr->pagesize) >> 9;
	ubi_min_volsize = 17 * leb_size;

	for (i = 0, k = 0; i < nand_mbr->PartCount; i++) {
		if (sunxi_find_mtd_num(mtd_mbr,
			(char *)nand_mbr->array[i].classname) != -1)
			continue;

		memcpy(u_mbr->array[k].classname, nand_mbr->array[i].classname,
			sizeof(nand_mbr->array[i].classname));

		if (k)
			u_mbr->array[k].addr = u_mbr->array[k - 1].addr +
				u_mbr->array[k - 1].len;
		else
			u_mbr->array[k].addr = attatch_mtd_num != -1 ?
			(uint)(mtd_mbr->part[attatch_mtd_num].offset >> 9) : -1;

		/* align leb_size */
		n_cur_part_len = (nand_mbr->array[i].len + leb_size - 1)
			/ leb_size * leb_size;
		if (n_cur_part_len)
			u_mbr->array[k].len = MAX(n_cur_part_len,
			ubi_min_volsize);
		else
			u_mbr->array[k].len = 0;

		k++;
		u_mbr->PartCount++;
	}

	sunxi_init_partition_wr_size(ubi_mbr);

	printf("MTD info (%d)\n", mtd_mbr->part_cnt);
	printf("pagesize: 0x%llx\n", mtd_mbr->pagesize);
	printf("blksize: 0x%llx\n", mtd_mbr->blksize);
	printf("num  offset     size        plan_wr_size written_size name\n");

	for (i = 0; i < mtd_mbr->part_cnt; i++)
		printf("%-4d 0x%08llx 0x%08llx 0x%08x   0x%08x   %s\n",
			mtd_mbr->part[i].num, mtd_mbr->part[i].offset,
			mtd_mbr->part[i].size, mtd_mbr->part[i].plan_wr_size,
			mtd_mbr->part[i].written_size, mtd_mbr->part[i].name);

	printf("MBR info: part_no addr        len         user_type   part_name\n");
	for (i = 0; i < nand_mbr->PartCount; i++)
		printf("MBR:      %2d,     0x%08x, 0x%08x, 0x%08x, %s\n",
		i, nand_mbr->array[i].addr, nand_mbr->array[i].len,
		nand_mbr->array[i].user_type, nand_mbr->array[i].classname);

	for (i = 0; i < u_mbr->PartCount; i++)
		printf("UBI_MBR:  %2d,     0x%08x, 0x%08x, 0x%08x, %s\n",
		i, u_mbr->array[i].addr, u_mbr->array[i].len,
		u_mbr->array[i].user_type, u_mbr->array[i].classname);

	printf("\nubi_vol_plan_wrsize:\n");
	printf("num  size       name\n");
	for (i = 0; i < u_mbr->PartCount; i++)
		printf("%-4d 0x%08x %s\n", i,
		u_status->part[i].plan_wr_size, u_mbr->array[i].classname);
	printf("\n");


}

static int sunxi_mtdparts_default(void)
{
	int ret = 0;
	char cmd[2][20];
	char *argv[2];
	int i;

	debug("%d, %s.\n", __LINE__, __func__);

	for (i = 0; i < 2; i++)
		argv[i] = cmd[i];

	memset(cmd, 0x00, sizeof(cmd));
	sprintf(&cmd[0][0], "mtdparts");
	sprintf(&cmd[1][0], "default");
	argv[0] = &cmd[0][0];
	argv[1] = &cmd[1][0];
	ret = sunxi_do_mtdparts(0, 2, argv);
	if (!ret) {
		debug("mtdparts info:\n");
		sunxi_do_mtdparts(0, 1, argv);
	}
	return ret;
}

static int sunxi_ubi_part_mtd(u16 mtd_num)
{
	int ret;
	char cmd[6][20];
	char *argv[6];
	char *mtd_name;
	int i;

	mtd_name = sunxi_get_mtdparts_name(mtd_num);
	if (mtd_name == NULL) {
		printf("mtd_name is NULL !!!\n");
		return -1;
	}
	for (i = 0; i < 6; i++)
		argv[i] = cmd[i];

	memset(cmd, 0x00, sizeof(cmd));
	sprintf(&cmd[0][0], "ubi");
	sprintf(&cmd[1][0], "part");
	sprintf(&cmd[2][0], "%s", mtd_name);

	/* ubi part [part] [offset] */
	ret = sunxi_do_ubi(0, 3, argv);
	if (ret)
		printf("ubi part %s err !\n", mtd_name);

	return ret;

}

static int sunxi_get_ubi_vol_num(UBI_MBR *ubi_mbr, char *name)
{
	int i;
	PARTITION_MBR *u_mbr;

	u_mbr = &ubi_mbr->u_mbr;

	for (i = 0; i < u_mbr->PartCount; i++) {
		if (name == NULL)
			continue;

		if (!strcmp((char *)u_mbr->array[i].classname, name))
			return i;
	}
	return -1;
}

static int sunxi_ubi_vol_chk(UBI_MBR *ubi_mbr, char *name)
{
	char cmd[6][20];
	char *argv[6];
	int i;

	if (sunxi_get_ubi_vol_num(ubi_mbr, name) == -1) {
		printf("Err: %s is not in ubi mbr !!! %s, %d.\n",
			name, __func__, __LINE__);
		return -1;
	}

	for (i = 0; i < 6; i++)
		argv[i] = cmd[i];

	memset(cmd, 0x00, sizeof(cmd));
	sprintf(&cmd[0][0], "ubi");
	sprintf(&cmd[1][0], "check");
	sprintf(&cmd[2][0], "%s", name);

	return sunxi_do_ubi(0, 3, argv);
}

static int sunxi_ubi_vol_remove(char *vol_name)
{
	char cmd[6][20];
	char *argv[6];
	int i;
	int ret;

	for (i = 0; i < 6; i++)
		argv[i] = cmd[i];

	memset(cmd, 0x00, sizeof(cmd));
	sprintf(&cmd[0][0], "ubi");
	sprintf(&cmd[1][0], "remove");
	sprintf(&cmd[2][0], "%s", vol_name);

	ret = sunxi_do_ubi(0, 3, argv);
	if (ret)
		printf("Err:%s remove fail!\n", vol_name);

	return ret;

}

static int sunxi_ubi_vol_create(UBI_MBR *ubi_mbr, int mtd_num)
{
	int ret = -1;
	char cmd[6][20];
	char *argv[6];
	int i;
	char *vol_name;
	size_t vol_size;
	char type;
	char *mtd_name;
	u64 mtd_size;
	u64 vol_total_size = 0;
	PARTITION_MBR *u_mbr;
	UBI_MBR_STATUS *u_status;

	if (ubi_mbr == NULL) {
		printf("UBI mbr is NULL !!!\n");
		return -1;
	}

	u_mbr = &ubi_mbr->u_mbr;
	u_status = &ubi_mbr->u_status;

	if (!u_mbr->PartCount)
		return 0;

	if (mtd_num != -1) {
		mtd_name = sunxi_get_mtdparts_name((u16)mtd_num);
		mtd_size = sunxi_get_mtdpart_size((u16)mtd_num);
		if (mtd_name == NULL) {
			printf("mtd_name is NULL !!!\n");
			return -1;
		}

		for (i = 0; i < u_mbr->PartCount; i++)
			vol_total_size += u_mbr->array[i].len << 9;

		if (vol_total_size > mtd_size) {
			printf("ubi volume total size is larger than mtd size.\n"
				"ubi_vol_total_size: 0x%llx, mtd_size: 0x%llx\n",
				vol_total_size, mtd_size);
			return -1;
		}

		printf("ubi attatch mtd, name: %s\n\n", mtd_name);

		ret = sunxi_ubi_part_mtd((u16)mtd_num);
		if (ret)
			return ret;
	}

	for (i = 0; i < 6; i++)
		argv[i] = cmd[i];

	for (i = 0; i < u_mbr->PartCount; i++) {
		vol_name = (char *)u_mbr->array[i].classname;
		vol_size = (size_t)u_mbr->array[i].len << 9;
		type = u_status->part[i].type;

		if (!sunxi_ubi_vol_chk(ubi_mbr, vol_name)) {
			printf("%s existed, ignore it.\n", vol_name);
			continue;
		}

		u_status->part[i].w_state = CREATE_UBI_VOL;

		memset(cmd, 0x00, sizeof(cmd));
		sprintf(&cmd[0][0], "ubi");
		sprintf(&cmd[1][0], "create");
		sprintf(&cmd[2][0], "%s", vol_name);
		sprintf(&cmd[3][0], "0x%016x", vol_size);
		if (type)
			sprintf(&cmd[4][0], "s");
		else
			sprintf(&cmd[4][0], "d");

		debug("%s %s %s %s %s\n", argv[0], argv[1],
			argv[2], argv[3], argv[4]);

		/* ubi create vloume size type */
		ret = sunxi_do_ubi(0, 5, argv);
		if (ret) {
			printf("ubi create %s err! vol_size:%0x\n",
				vol_name, vol_size);
			break;
		}
	}
	return ret;
}

/*
 * input: 0 ; 1
 *
 * static int sunxi_ubi_vol_info(uint layout)
 * {
 *	char cmd[6][20];
 *	char *argv[6];
 *	int i;
 *
 *	for (i = 0; i < 6; i++)
 *		argv[i] = cmd[i];
 *
 *	memset(cmd, 0x00, sizeof(cmd));
 *	sprintf(&cmd[0][0], "ubi");
 *	sprintf(&cmd[1][0], "info");
 *	sprintf(&cmd[2][0], "l");
 *
 *	if (layout)
 *		return sunxi_do_ubi(0, 3, argv);
 *	else
 *		return sunxi_do_ubi(0, 2, argv);
 * }
 */

static int sunxi_boot_ubi_read_mbr(char *vol_name, uint sectors, void *buf)
{
	size_t size;
	char cmd[6][20];
	char *argv[6];
	int i;

	size = sectors << 9;
	if (sectors > 0x80)
		printf("Warning: par overflow !!! size(byte): 0x%0x %s\n",
			size, __func__);

	for (i = 0; i < 6; i++)
		argv[i] = cmd[i];

	memset(cmd, 0x00, sizeof(cmd));
	sprintf(&cmd[0][0], "ubi");
	sprintf(&cmd[1][0], "read");
	sprintf(&cmd[2][0], "0x%016x", (size_t)buf);
	sprintf(&cmd[3][0], "%s", vol_name);
	sprintf(&cmd[4][0], "0x%016x", size);

	return sunxi_do_ubi(0, 5, argv);

}

static int sunxi_ubi_vol_write(UBI_MBR *ubi_mbr,
	char *name, uint sectors, void *buf)
{
	int ret;
	size_t full_size, size;
	uint32 plan_wr_size, written_size;
	static char last_name[64] = {0};
	char cmd[6][20];
	char *argv[6];
	int i;
	int num;
	UBI_MBR_STATUS *u_status;

	u_status = &ubi_mbr->u_status;

	num = sunxi_get_ubi_vol_num(ubi_mbr, name);
	if (num == -1) {
		printf("Err: %s is not in ubi mbr !!! %s, %d.\n",
			name, __func__, __LINE__);
		return -1;
	}

	for (i = 0; i < 6; i++)
		argv[i] = cmd[i];

	plan_wr_size = u_status->part[num].plan_wr_size;
	written_size = u_status->part[num].written_size;

	full_size = plan_wr_size << 9;
	size = sectors << 9;

	if ((written_size + sectors) > plan_wr_size) {
		printf("Warning: par overflow !");
		printf("partno:%d, plan:%0x, written:%0x, wrting:%0x\n",
			num, plan_wr_size, written_size, sectors);
		size = (plan_wr_size - written_size) << 9;
	}

	if (strlen(name) > sizeof(last_name)) {
		printf("Err:The len of vol_name too large.\n");
		printf("name: %s %s\n", name, __func__);

		return -1;
	}

	/* ubi write.part address volume size [fullsize] */
	memset(cmd, 0x00, sizeof(cmd));
	sprintf(&cmd[0][0], "ubi");
	sprintf(&cmd[1][0], "write.part");
	sprintf(&cmd[2][0], "0x%016x", (size_t)buf);
	sprintf(&cmd[3][0], "%s", name);
	sprintf(&cmd[4][0], "0x%016x", size);
	sprintf(&cmd[5][0], "0x%016x", full_size);

	if (strcmp(last_name, name)) {

		printf("First ubi write,part_name:%s, wr_sectors:%0x\n",
			name, sectors);

		if (u_status->part[num].w_state != CREATE_UBI_VOL) {
			if (!sunxi_ubi_vol_chk(ubi_mbr, name)) {
				printf("%s existed.\n", name);
				printf("re-build it, removing.\n");
			}

			sunxi_ubi_vol_remove(name);

			if (sunxi_ubi_vol_create(ubi_mbr, -1))
				return -1;
		}

		strcpy(last_name, name);
		ret = sunxi_do_ubi(0, 6, argv);
	} else {
		ret = sunxi_do_ubi(0, 5, argv);
	}
	u_status->last_partnum = num;
	u_status->part[num].written_size += sectors;
	u_status->part[num].w_state = WRITTEN_UBI_VOL;

	if (ret)
		printf("Err: ret = 0x%0x %s\n", ret, __func__);

	return ret;
}

static int sunxi_mtd_flush(MTD_MBR *mtd_mbr)
{
	u_char *upd_buf;
	int num;
	uint col;
	size_t retlen;
	u64	to, len, pgsize, offset;
	u64 plan_wr_bytes, written_bytes, upd_received;

	num = mtd_mbr->last_partnum;
	if ((num == -1))
		return 0;

	offset = mtd_mbr->part[num].offset;
	pgsize = mtd_mbr->pagesize;
	upd_buf = (u_char *)mtd_mbr->w_buf;

	upd_received = (u64)mtd_mbr->part[num].written_size << 9;
	written_bytes = upd_received / pgsize * pgsize;

	plan_wr_bytes = (u64)mtd_mbr->part[num].plan_wr_size << 9;
	plan_wr_bytes = (plan_wr_bytes + (pgsize - 1)) / pgsize * pgsize;

	if (written_bytes >= plan_wr_bytes)
		return 0;

	debug("%s, written: 0x%llx, plan: 0x%llx\n", __func__,
		written_bytes, plan_wr_bytes);

	len = plan_wr_bytes - written_bytes;
	col = upd_received % pgsize;
	if (col)
		memset(upd_buf + col, 0xff, pgsize - col);
	else
		memset(upd_buf, 0xff, pgsize);

	debug("len: 0x%llx, col: 0x%0x, rest_bytes: 0x%0llx\n",
		len, col, pgsize - col);

	while (len) {
		debug("mtd writing :0x%llx\n", pgsize >> 9);

		to = offset + written_bytes;
		if (sunxi_mtd_write(to, pgsize, &retlen, upd_buf)) {
			printf("%d, %s, Err:wr_size:%llx, written:%0x\n",
				__LINE__, __func__, pgsize, retlen);
			return -1;
		}

		memset(upd_buf, 0xff, pgsize);
		written_bytes += pgsize;
		len -= pgsize;
	}
	mtd_mbr->part[num].written_size = written_bytes >> 9;

	debug("%s total written :0x%llx\n", mtd_mbr->part[num].name,
		written_bytes >> 9);

	return 0;
}
static int sunxi_mtd_part_read(MTD_MBR *mtd_mbr, int num,
	uint start, uint sectors, void *buffer)
{
	size_t len, retlen;
	u64 offset;
	u64 partsize;
	u64 from;

	partsize = mtd_mbr->part[num].size;
	offset = mtd_mbr->part[num].offset;
	len = sectors << 9;
	from = offset + (start << 9);

	if (((start + sectors) << 9) > partsize) {
		printf("%s,out of mtdsize(0x%llx).\n", __func__, partsize);
		printf("start:%0x, sectors:%0x.\n", start, sectors);
		return -1;
	}

	sunxi_mtd_flush(mtd_mbr);

	return sunxi_mtd_read(from, len, &retlen, buffer);

}
static int sunxi_mtd_part_write(MTD_MBR *mtd_mbr, int num,
	uint sectors, void *buffer)
{
	static int last_num = -1;
	size_t len, retlen;
	uint col, rest_pgsize;
	u64 to, offset, pgsize, upd_received;
	u_char *src_buf, *upd_buf;

	if (num != last_num) {
		printf("First mtd write. part_name : %s, wr_sectors: 0x%0x\n",
			mtd_mbr->part[num].name, sectors);
		printf("%s, cur_num : 0x%x. last_num : 0x%x\n", __func__,
			num, last_num);

		/* the first write mtdx, then flush the last_num mtdx */
		sunxi_mtd_flush(mtd_mbr);

		last_num = num;
		mtd_mbr->part[num].written_size = 0;
		mtd_mbr->last_partnum = num;
	}

	offset = mtd_mbr->part[num].offset;
	upd_received = mtd_mbr->part[num].written_size << 9;
	src_buf = (u_char *)buffer;
	upd_buf = (u_char *)mtd_mbr->w_buf;
	pgsize = mtd_mbr->pagesize;
	if (pgsize > MTD_W_BUF_SIZE) {
		printf("Err: pgsize(0x%llx) is larger than bufsize(64KB).\n",
			pgsize);
		return -1;
	}

	len = sectors << 9;
	col = upd_received % pgsize;
	rest_pgsize = pgsize - col;
	if (col) {
		if (len >= rest_pgsize) {
			debug("col:0x%x, rest_pgsize:%x, len:%x\n",
				col, rest_pgsize, len);
			debug("mtd writing :0x%x\n", rest_pgsize >> 9);

			memcpy(upd_buf + col, src_buf, rest_pgsize);
			to = offset + upd_received / pgsize * pgsize;
			if (sunxi_mtd_write(to, pgsize, &retlen, upd_buf)) {
				printf("%d, %s, Err:wr_size:%llx, written:%0x\n",
					__LINE__, __func__, pgsize, retlen);
				return -1;
			}

			src_buf += rest_pgsize;
			len -= rest_pgsize;
			upd_received += rest_pgsize;
		} else {
			memcpy(upd_buf + col, src_buf, len);
			len = 0;
			upd_received += len;
		}
	}

	while (len) {
		if (len >= pgsize) {
			debug("mtd writing :0x%llx\n", pgsize >> 9);

			memcpy(upd_buf, src_buf, pgsize);
			to = offset + upd_received;
			if (sunxi_mtd_write(to, pgsize, &retlen, upd_buf)) {
				printf("%d, %s,Err: wr_size:%llx, written:%0x\n",
					__LINE__, __func__, pgsize, retlen);
				return -1;
			}

			src_buf += pgsize;
			len -= pgsize;
			upd_received += pgsize;
		} else {
			debug("mtd copy to buf: 0x%x\n", len >> 9);

			memcpy(upd_buf, src_buf, len);
			upd_received += len;
			len = 0;
		}
	}
	mtd_mbr->part[num].written_size = upd_received >> 9;

	debug("%s total written :0x%llx\n", mtd_mbr->part[num].name,
		upd_received >> 9);

	return 0;
}

static int sunxi_ubi_vol_fill_full(UBI_MBR *ubi_mbr, char *vol_name,
	int64_t gap_size, char fill_data)
{
	char *gap_buf;
	int64_t i;

	if (!gap_size)
		return 0;

	gap_buf = (void *)malloc(GAP_BUF_MAX_SIZE << 9);
	if (!gap_buf) {
		printf("Err: malloc memory fail, line: %d, %s\n",
			__LINE__,  __func__);
		return -1;
	}

	if ((fill_data == 0x00) || (fill_data == 0xff))
		randomize((unsigned int *)gap_buf,
		(GAP_BUF_MAX_SIZE << 9) / sizeof(unsigned int), 0x55aa);
	else
		memset(gap_buf, fill_data, GAP_BUF_MAX_SIZE << 9);

	for (i = gap_size; i > 0; i -= GAP_BUF_MAX_SIZE) {
		if (sunxi_ubi_vol_write(ubi_mbr, vol_name,
			MIN(i, GAP_BUF_MAX_SIZE), gap_buf)) {
			free(gap_buf);
			printf("Err: fill gap fail! line: %d, %s\n",
				__LINE__,  __func__);
			return -1;
		}
	}
	free(gap_buf);

	return 0;

}

static int fill_gap(PARTITION_MBR *nand_mbr, UBI_MBR *ubi_mbr,
	uint lba, uint sectors)
{
	int ret = 0;
	int last_u_partno, n_partno;
	uint offset;
	int64_t gap = 0;
	char *name;

	static int last_n_partno = -1;
	static uint last_offset;
	UBI_MBR_STATUS *u_status;

	u_status = &ubi_mbr->u_status;

	n_partno = get_partno_in_mbr(nand_mbr, lba, sectors);
	if (n_partno == -1) {
		printf("Err: lba is not invalid !!! lba=%0x, sectors=%0x\n",
			lba, sectors);
		return -1;
	}

	name = (char *)nand_mbr->array[n_partno].classname;
	if (sunxi_get_ubi_vol_num(ubi_mbr, name) == -1) {
		printf("%s, %d.Warning: ubi vol does not exist.\n",
			__func__, __LINE__);
		return -1;
	}

	offset = get_offset_in_partition(nand_mbr, n_partno, lba);
	if (n_partno != last_n_partno) {
		/* write another vol, so fill with last_vol */
		if (last_n_partno != -1) {
			name = (char *)nand_mbr->array[last_n_partno].classname;
			last_u_partno = sunxi_get_ubi_vol_num(ubi_mbr, name);
			if (last_u_partno == -1)
				return -1;

			gap = u_status->part[last_u_partno].plan_wr_size -
				u_status->part[last_u_partno].written_size;

			if (gap) {
				printf("last_part: %s,cur_part: %s.Auto fill data to the end.\n",
				name,
				(char *)nand_mbr->array[n_partno].classname);
				printf("gap_cnt: 0x%0x\n", (uint)gap);
				printf("input:start_lba=%0x, sectors_cnt=%0x\n",
					lba, sectors);

				ret = sunxi_ubi_vol_fill_full(ubi_mbr, name,
					gap, 0x00);
			}

			if (offset) {
				printf("Err: First offset in writing ubi(%s) is not from 0x00\n",
				(char *)nand_mbr->array[n_partno].classname);
				return -1;
			}
		}

		/* first write current vol */
		last_n_partno = n_partno;
		last_offset = offset + sectors;

	} else {
		if (offset == last_offset) {
			last_offset = offset + sectors;
			return 0;
		}

		if (offset < last_offset) {
			printf("%s, %d. Err: offset smaller than last_offset.\n",
				__func__, __LINE__);
			printf("last_offset = 0x%x, cur_offset = 0x%0x\n",
				last_offset, offset);
			return -1;
		}

		gap = offset - last_offset;
		name = (char *)nand_mbr->array[n_partno].classname;

		printf("%s, Warning: lba not seq !!!\n", __func__);
		printf("last_partno=%d, cur_partno=%d , name=%s\n",
			last_n_partno, n_partno, name);
		printf("last_offset=%x, cur_offset=%0x\n", last_offset, offset);
		printf("gap_cnt: 0x%0x\n", (uint)gap);
		printf("input:start_lba=%0x, sectors_cnt=%0x\n", lba, sectors);

		ret = sunxi_ubi_vol_fill_full(ubi_mbr, name, gap, 0x00);

		last_offset = offset + sectors;
	}
	return ret;
}

static int sunxi_ubi_vol_flush(UBI_MBR *ubi_mbr, char *cur_partname)
{
	char *last_partname = NULL;
	uint gap_lba, gap_size;
	uint plan_size, written_size;
	int last_partnum, num;
	PARTITION_MBR *n_mbr;
	PARTITION_MBR *u_mbr;
	UBI_MBR_STATUS *u_status;

	n_mbr = &ubi_mbr->n_mbr;
	u_mbr = &ubi_mbr->u_mbr;
	u_status = &ubi_mbr->u_status;

	last_partnum = u_status->last_partnum;
	plan_size = u_status->part[last_partnum].plan_wr_size;
	written_size = u_status->part[last_partnum].written_size;
	last_partname = (char *)u_mbr->array[last_partnum].classname;

	debug("last_partnum: 0x%0x, plan_size: 0x%0x, written_size: 0x%0x\n",
		last_partnum, plan_size, written_size);

	if (!strcmp(last_partname, cur_partname) &&
		(written_size < plan_size)) {
		gap_size = plan_size - written_size;
		num = sunxi_find_nand_mbr_num(n_mbr, last_partname);
		if (num == -1)
			return -1;

		/* last lba */
		gap_lba = n_mbr->array[num].addr + written_size;
		gap_lba += gap_size;

		if (gap_size) {
			printf("%s, partname:%s, gap_lba:%0x, gap_size:%0x\n",
				__func__, cur_partname,
				gap_lba-gap_size, gap_size);

			sunxi_ubi_vol_fill_full(ubi_mbr, cur_partname,
				gap_size, 0x00);
		}
	}

	return 0;
}

static int sunxi_ubi_vol_read(UBI_MBR *ubi_mbr, char *name,
	uint offs, uint sectors, void *buf)
{
	size_t full_size, size;
	int num;
	loff_t offp;
	PARTITION_MBR *u_mbr;

	num = sunxi_get_ubi_vol_num(ubi_mbr, name);
	if (num == -1) {
		printf("Err: %s is not in ubi mbr !!! %s, %d.\n",
			name, __func__, __LINE__);
		return -1;
	}

	u_mbr = &ubi_mbr->u_mbr;
	offp = (loff_t)offs << 9;
	full_size = (size_t)u_mbr->array[num].len << 9;
	size = (size_t)sectors << 9;
	if (size > full_size) {
		printf("Warning: par overflow.");
		printf("size(byte):%0x, full_size(byte):%0x\n",
			size, full_size);
		size = full_size;
	}

	if (sunxi_ubi_vol_flush(ubi_mbr, name)) {
		printf("%s, Err: ubi flush fail!!!\n", __func__);
		return -1;
	}

	return sunxi_ubi_volume_read(name, offp, (char *)buf, size);
}

uint sunxi_nand_mtd_ubi_cap(void)
{
	return (uint)(get_mtd_size() >> 9);
}

uint sunxi_nand_mtd_ubi_write(uint start, uint sectors, void *buffer)
{
	char *name = NULL;
	int ret;
	int num, mtd_num;
	MTD_MBR *gb_mtd_mbr;
	PARTITION_MBR *gb_n_mbr;
	PARTITION_MBR *gb_nand_mbr;
	UBI_MBR *gb_ubi_mbr;

	debug("line: %d, %s. start = 0x%0x, sectors = 0x%0x\n",
		__LINE__, __func__, start, sectors);

	gb_nand_mbr = &nand_mbr;
	gb_ubi_mbr = &ubi_mbr;
	gb_n_mbr = &ubi_mbr.n_mbr;
	gb_mtd_mbr = &ubi_mbr.mtd_mbr;

	if (!start && (sectors <= 0x80)) {
		if (sunxi_mtdparts_default())
			return 0;

		num = get_ubi_attach_mtd_num();
		if (num == -1)
			return 0;

		sunxi_init_mtd_info(gb_mtd_mbr);

		if (sunxi_init_nand_mbr(gb_nand_mbr, NULL, gb_n_mbr))
			return 0;

		init_sunxi_mtd_ubi_mbr(gb_n_mbr, gb_ubi_mbr);

		if (sunxi_ubi_vol_create(gb_ubi_mbr, num))
			return 0;
	}

	num = get_partno_in_mbr(gb_n_mbr, start, sectors);
	if (!sectors || num == -1)
		return 0;

	name = (char *)gb_n_mbr->array[num].classname;

	mtd_num = sunxi_find_mtd_num(gb_mtd_mbr, name);
	if (mtd_num != -1) {
		ret = sunxi_mtd_part_write(gb_mtd_mbr, mtd_num,
			sectors, buffer);
		return ret ? 0 : sectors;
	}

	ret = sunxi_ubi_vol_chk(gb_ubi_mbr, name);
	if (ret) {
		printf("Warning: ubi volume does not exist!\n");
		return 0;
	}

	if (fill_gap(gb_n_mbr, gb_ubi_mbr, start, sectors) == -1)
		printf("Warning: fill gap fail, part:%s, start:%0x, sectors:%0x\n",
			name, start, sectors);

	ret = sunxi_ubi_vol_write(gb_ubi_mbr, name, sectors, buffer);

	return ret ? 0 : sectors;
}

uint sunxi_nand_mtd_ubi_read(uint start, uint sectors, void *buffer)
{
	char *name = NULL;
	uint offset;
	int ret;
	int num, mtd_num;
	int workmode = uboot_spare_head.boot_data.work_mode;
	MTD_MBR *mtd_mbr;
	PARTITION_MBR *n_mbr;
	PARTITION_MBR *gb_nand_mbr;
	UBI_MBR *gb_ubi_mbr;

	debug("line: %d, %s. start = 0x%0x, sectors = 0x%0x\n",
		__LINE__, __func__, start, sectors);

	gb_nand_mbr = &nand_mbr;
	gb_ubi_mbr = &ubi_mbr;
	n_mbr = &ubi_mbr.n_mbr;
	mtd_mbr = &ubi_mbr.mtd_mbr;

	/* After power on, nand_mbr.PartCount = 0 */
	if ((workmode == WORK_MODE_BOOT) && !gb_nand_mbr->PartCount
		&& !start && (sectors <= 0x80)) {
		if (sunxi_mtdparts_default())
			return 0;

		sunxi_init_mtd_info(mtd_mbr);

		num = get_ubi_attach_mtd_num();
		if (num == -1)
			return 0;

		if (sunxi_ubi_part_mtd(num))
			return 0;

		mtd_num = sunxi_find_mtd_num(mtd_mbr, "mbr");
		if (mtd_num != -1)
			ret = sunxi_mtd_part_read(mtd_mbr,
				mtd_num, start, sectors, buffer);
		else
			ret = sunxi_boot_ubi_read_mbr("mbr", sectors, buffer);

		if (!ret) {
			ret = sunxi_init_nand_mbr(NULL, buffer, n_mbr);
			init_sunxi_mtd_ubi_mbr(n_mbr, gb_ubi_mbr);
			return ret ? 0 : sectors;
		}
		return 0;
	}

	num = get_partno_in_mbr(n_mbr, start, sectors);
	if (!sectors || num == -1)
		return 0;

	name = (char *)n_mbr->array[num].classname;
	offset = get_offset_in_partition(n_mbr, num, start);

	mtd_num = sunxi_find_mtd_num(mtd_mbr, name);
	if (mtd_num != -1) {
		ret = sunxi_mtd_part_read(mtd_mbr,
			mtd_num, offset, sectors, buffer);
		return ret < 0 ? 0 : sectors;
	}

	ret = sunxi_ubi_vol_chk(gb_ubi_mbr, name);
	if (ret) {
		printf("Warning: ubi volume does not exist!\n");
		return 0;
	}

	ret = sunxi_ubi_vol_read(gb_ubi_mbr, name, offset, sectors, buffer);

	return ret ? 0 : sectors;
}

int sunxi_chk_ubifs_sb(void *sb_buf)
{
	struct ubifs_sb_node *sb;
	uint32_t mtd_blksize, mtd_pgsize, ubi_leb_size;
	uint32_t magic = UBIFS_NODE_MAGIC;
	int ret = 0;

	sb = (struct ubifs_sb_node *)sb_buf;

	mtd_pgsize = get_mtd_pgsize();
	mtd_blksize = get_mtd_blksize();
	ubi_leb_size = mtd_blksize - 2 * mtd_pgsize;

	if (sb->ch.magic != magic) {
		printf("Err: ubifs magic failed: 0x%0x != 0x%0x, %d, %s\n",
			sb->ch.magic, magic, __LINE__, __func__);
		printf("Please check ubifs super block for more information.\n");
		return 1;
	}

	printf("ubifs pack config:\n");
	printf("nand_super_page_size=%d\n", sb->min_io_size);
	printf("nand_super_blk_size=%d\n", sb->leb_size + 2 * sb->min_io_size);

	if (sb->min_io_size != mtd_pgsize) {
		printf("Err: ubifs min_io_size verify failed: %d != %d, %d, %s\n",
			sb->min_io_size, mtd_pgsize, __LINE__, __func__);

		ret = 2;
	}

	if (sb->leb_size != ubi_leb_size) {
		printf("Err: ubifs leb verify failed: %d != %d, %d, %s\n",
			sb->leb_size, ubi_leb_size, __LINE__, __func__);

		ret = 3;
	}

	if (ret) {
		printf("please use ubifs pack configure as follow:\n");
		printf("nand_super_page_size=%d\n", mtd_pgsize);
		printf("nand_super_blk_size=%d\n", mtd_blksize);
	} else
		printf("%s: ubifs pack configure is ok.\n\n", __func__);

	return ret;
}
#endif
