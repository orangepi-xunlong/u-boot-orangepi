// SPDX-License-Identifier: GPL-2.0

#include <linux/kernel.h>
#include <sunxi_board.h>
#include <common.h>
#ifdef CONFIG_AW_MTD_SPINAND
#include <linux/mtd/aw-spinand.h>
#endif
#ifdef CONFIG_AW_MTD_RAWNAND
#include <linux/mtd/aw-rawnand.h>
#endif
#include <sunxi_mbr.h>
#include <ubi_uboot.h>
#include <private_uboot.h>
#include <linux/mtd/mtd.h>
#include <linux/sizes.h>
#include <linux/mtd/aw-ubi.h>
/*
* undef crc32 -> crc32_le in linux/crc32.h
* crc32 for sunxi_mbr
*/
#undef crc32
#include <u-boot/crc.h>
#ifdef CONFIG_AW_MTD_SPINAND
#include "spinand/sunxi-spinand.h"
#endif


#define GAP_BUF_MAX_SECTS	(128)
#define TYPE_STATIC_VOLUME	(1 << 0)

struct ubi_nand_vol {
	char name[PART_NAME_MAX_SIZE];
	unsigned int addr;
	unsigned int sects;
	unsigned int type;
};

struct ubi_mbr {
	unsigned int part_cnt;
	struct ubi_nand_vol vols[NAND_MAX_PART_CNT];
};

struct ubi_vol_status {
#define INIT_UBI_VOL	0
#define CREATE_UBI_VOL	1
#define WRITE_UBI_VOL	2
	int state;
#define DYNAMIC_VOL	0
#define STATIC_VOL	1
	int type;
	unsigned int plan_wr_sects;
	unsigned int written_sects;

	struct ubi_nand_vol *vol;
};

struct ubi_status {
	int last_partno;
	struct ubi_vol_status vols[NAND_MAX_PART_CNT];
};

struct ubi_info {
	/*
	 * saving mbr with follow feature:
	 * 1. copy from sunxi_mbr_t
	 * 2. add mbr volume from sunxi_mbr_t
	 * 3. volume addr relative to user space address.
	 *    it means the first volume mbr's addr is 0x00
	 * 4. volume sects do not align
	 */
	struct ubi_mbr nand_mbr;
	/*
	 * saving mbr with follow feature:
	 * 1. copy from nand_mbr as above.
	 * 2. volume addr relative to the whole flash address.
	 *    it means the first volume mbr's addr is NOT 0x00.
	 *    the first volume always be the start of mtd device.
	 * 3. volume sects do not align
	 */
	struct ubi_mbr ubi_mbr;
	struct ubi_status ubi_status;
	struct ubi_mtd_info mtd_info;

	char last_name[PART_NAME_MAX_SIZE];
	int last_partno;
	unsigned int last_offset;
	char mtdids[20];
	char mtdparts[512];
	unsigned int ubootblks;

	unsigned int last_vol_sects;
};
static struct ubi_info g_ubi_info;

extern int sunxi_mbr_convert_to_gpt(void *sunxi_mbr_buf, char *gpt_buf, int storage_type);

static unsigned int to_align(unsigned int size, unsigned int limit)
{
	if (limit)
		return (size + limit - 1) / limit * limit;
	return size;
}

static inline struct ubi_info *get_ubi_info(void)
{
	return &g_ubi_info;
}
#define ubi_to_nand_mbr(ubi_info) (&(ubi_info)->nand_mbr)
#define ubi_to_ubi_mbr(ubi_info) (&(ubi_info)->ubi_mbr)
#define ubi_to_ubi_status(ubi_info) (&(ubi_info)->ubi_status)
#define ubi_to_mtd(ubi_info) (&(ubi_info)->mtd_info)
#if 0
static void randomize(unsigned int *buf, unsigned int len, int seed)
{
	int i;

	srand(seed);
	for (i = 0; i < len; i++)
		*buf++ = rand();
}
#endif

static void print_ubi_mbr(struct ubi_mbr *mbr)
{
	int i;

	pr_info("%-8s %-10s %-10s %-10s %s\n", "partno", "addr", "sects",
			"type", "name");
	for (i = 0; i < mbr->part_cnt; i++) {
		struct ubi_nand_vol *vol = &mbr->vols[i];

		pr_info("%-8d 0x%08x 0x%08x 0x%08x %s\n", i,
				vol->addr, vol->sects, vol->type, vol->name);
	}
}

static int get_partno_by_addr(unsigned int offset, unsigned int sec_cnt)
{
	int i;
	unsigned int addr, sects;
	struct ubi_info *ubinfo = get_ubi_info();
	struct ubi_mbr *nand_mbr = ubi_to_nand_mbr(ubinfo);

	if (!nand_mbr->part_cnt) {
		pr_err("not found nand_mbr\n");
		return -ENODEV;
	}

	for (i = 0; i < nand_mbr->part_cnt; i++) {
		addr = nand_mbr->vols[i].addr;
		sects = nand_mbr->vols[i].sects;
		if (offset >= addr + sects)
			continue;
		if (offset + sec_cnt > addr + sects) {
			pr_err("offset 0x%x with sects 0x%x over volume %s [0x%x 0x%x)\n",
					offset, sec_cnt,
					nand_mbr->vols[i].name, addr,
					addr + sects);
			break;
		}
		return i;
	}

	pr_err("get partno from nand_mbr failed: offset 0x%x sects %u\n",
			offset, sec_cnt);
	print_ubi_mbr(nand_mbr);
	return -EINVAL;
}

static int get_volnum_by_name(char *name)
{
	int i;
	struct ubi_info *ubinfo = get_ubi_info();
	struct ubi_mbr *ubi_mbr = ubi_to_ubi_mbr(ubinfo);;

	for (i = 0; i < ubi_mbr->part_cnt; i++) {
		if (name == NULL)
			continue;

		if (!strcmp((char *)ubi_mbr->vols[i].name, name))
			return i;
	}
	return -ENODEV;
}

static int get_mtd_num_by_name(char *name)
{
	int i;
	struct ubi_info *ubinfo = get_ubi_info();
	struct ubi_mtd_info *mtd_info = ubi_to_mtd(ubinfo);

	if (name == NULL)
		return -EINVAL;

	for (i = 0; i < mtd_info->part_cnt; i++)
		if (!strcmp(mtd_info->part[i].name, name))
			return i;

	return -EINVAL;
}

static int get_mtd_num_to_attach(void)
{
	int num;

	num = sunxi_get_mtd_num_parts();
	num = num ? num - 1 : -1;

	pr_info("ubi attach the last part of mtd device: NO.%d\n", num);

	return num;
}

static inline unsigned int offset_on_part(int partno, unsigned int addr)
{
	struct ubi_info *ubinfo = get_ubi_info();
	struct ubi_mbr *nand_mbr = ubi_to_nand_mbr(ubinfo);

	return addr - nand_mbr->vols[partno].addr;
}

static int check_sunxi_mbr(sunxi_mbr_t *sunxi_mbr)
{
	if (strncmp((const char *)sunxi_mbr->magic, SUNXI_MBR_MAGIC, 8)) {
		pr_err("mbr magic error: %.8s wanted %s\n",
			(char *)sunxi_mbr->magic, SUNXI_MBR_MAGIC);
		return -EINVAL;
	}

	if (WORK_MODE_BOOT == get_boot_work_mode()) {
		unsigned int crc = 0;
		crc = crc32(0, (const unsigned char *)&sunxi_mbr->version,
				SUNXI_MBR_SIZE - 4);
		if (crc != sunxi_mbr->crc32) {
			pr_err("mbr crc verify failed: wanted 0x%x get 0x%x\n",
				sunxi_mbr->crc32, crc);
			return -EINVAL;
		}
	}

	if (!sunxi_mbr->PartCount) {
		pr_err("partition count is zero, something error\n");
		return -EINVAL;
	}
	return 0;
}

static void adjust_sunxi_mbr(sunxi_mbr_t *org_mbr, unsigned int last_vol_sects)
{
	int i		       = 0;
	sunxi_mbr_t *tmp_mbr   = org_mbr;
	unsigned int partcount = org_mbr->PartCount;

	if (last_vol_sects == org_mbr->array[partcount - 1].lenlo)
		return;

	org_mbr->array[partcount - 1].lenlo = last_vol_sects;
	org_mbr->crc32 = crc32(0, (const unsigned char *)&org_mbr->version,
			SUNXI_MBR_SIZE - 4);
	for (i = 1; i < SUNXI_MBR_COPY_NUM; i++) {
		tmp_mbr++;
		tmp_mbr->array[partcount - 1].lenlo =
			org_mbr->array[partcount - 1].lenlo;
		tmp_mbr->crc32 = org_mbr->crc32;
	}
}

static int init_nand_mbr(sunxi_mbr_t *sunxi_mbr, struct ubi_mbr *nand_mbr)
{
	int i, ret;

	if (!sunxi_mbr) {
		pr_err("invalid sunxi_mbr\n");
		return -EINVAL;
	}

	ret = check_sunxi_mbr(sunxi_mbr);
	if (ret)
		return ret;

	memset(nand_mbr, 0x00, sizeof(*nand_mbr));

	if (!sunxi_mbr->PartCount) {
		pr_err("no partition on sunxi_mbr\n");
		return -EINVAL;
	}

	/* the first part is MBR */
	nand_mbr->part_cnt = 1;
	nand_mbr->vols[0].addr = 0x00;
	nand_mbr->vols[0].sects = sunxi_mbr->array[0].addrlo;
	nand_mbr->vols[0].type |= TYPE_STATIC_VOLUME;
	strcpy((char *)nand_mbr->vols[0].name, "mbr");

	nand_mbr->part_cnt += sunxi_mbr->PartCount;

	for (i = 0; i < sunxi_mbr->PartCount; i++) {
		nand_mbr->vols[i + 1].addr = sunxi_mbr->array[i].addrlo;
		nand_mbr->vols[i + 1].sects = sunxi_mbr->array[i].lenlo;
		strncpy((char *)nand_mbr->vols[i + 1].name,
			(const char *)sunxi_mbr->array[i].name,
			PART_NAME_MAX_SIZE);
		nand_mbr->vols[i + 1].type = sunxi_mbr->array[i].user_type;
	}

	pr_info("MBR info (unalign):\n");
	print_ubi_mbr(nand_mbr);
	return 0;
}

static int init_mtd_info(struct ubi_mtd_info *mtd_info)
{
	int ret = -EINVAL;
	int i = 0;
	struct ubi_mtd_part *mtd_part;

#ifdef CONFIG_AW_MTD_SPINAND
	if (support_spinand())
		ret = spinand_init_mtd_info(mtd_info);
#endif
#ifdef CONFIG_AW_MTD_RAWNAND
	if (support_rawnand())
		ret = rawnand_init_mtd_info(mtd_info);
#endif

	if (ret) {
		pr_err("%s-%d _init_mtd_info fail\n", __func__, __LINE__);
		return ret;
	}

	pr_info("MTD info (%d)\n", mtd_info->part_cnt);
	pr_info("pagesize: 0x%x\n", mtd_info->pagesize);
	pr_info("blksize: 0x%x\n", mtd_info->blksize);
	pr_info("%-4s %-10s %-10s %s\n", "num", "offset", "bytes", "name");

	for (i = 0; i < mtd_info->part_cnt; i++) {
		mtd_part = &mtd_info->part[i];
		pr_info("%-4d 0x%08x 0x%08x %s\n",
			mtd_part->partno, mtd_info->part[i].offset,
			mtd_part->bytes, mtd_part->name);
	}
	return 0;
}
static void init_ubi_mbr(struct ubi_info *ubinfo)
{
	int ret;
	int i, k;
	int mtdnum;
	int32_t n_cur_part_len;
	int32_t leb_sects;
	int32_t ubi_min_vol_sects;
	struct ubi_mbr *ubi_mbr = ubi_to_ubi_mbr(ubinfo);
	struct ubi_mbr *nand_mbr = ubi_to_nand_mbr(ubinfo);
	struct ubi_status *ubi_status = ubi_to_ubi_status(ubinfo);
	struct ubi_mtd_info *mtd_info = ubi_to_mtd(ubinfo);

	memset(ubi_mbr, 0x00, sizeof(struct ubi_mbr));
	memset(ubi_status, 0x00, sizeof(struct ubi_status));
	ubi_status->last_partno = -1;

	mtdnum = get_mtd_num_to_attach();

	/*
	 * multiplane with subpage, make it need only on super page to save
	 * EC header and VID header
	 */
	leb_sects = to_sects(mtd_info->blksize - 1 * mtd_info->pagesize);
	ubi_min_vol_sects = leb_sects;

	for (i = 0, k = 0; i < nand_mbr->part_cnt; i++) {
		ret = get_mtd_num_by_name((char *)nand_mbr->vols[i].name);
		if (ret >= 0)
			continue;

		memcpy(ubi_mbr->vols[k].name, nand_mbr->vols[i].name,
			sizeof(nand_mbr->vols[i].name));
		ubi_mbr->vols[k].type = nand_mbr->vols[i].type;

		if (k)
			ubi_mbr->vols[k].addr = ubi_mbr->vols[k - 1].addr +
				ubi_mbr->vols[k - 1].sects;
		else
			ubi_mbr->vols[k].addr = mtdnum >= 0 ?
			(unsigned int)to_sects(mtd_info->part[mtdnum].offset) : -1;

		/* align leb_size */
		n_cur_part_len = to_align(nand_mbr->vols[i].sects, leb_sects);
		if (n_cur_part_len)
			ubi_mbr->vols[k].sects = max_t(unsigned int,
					n_cur_part_len, ubi_min_vol_sects);
		else
			ubi_mbr->vols[k].sects = 0;

		k++;
		ubi_mbr->part_cnt++;
	}

	pr_info("MBR info (align):\n");
	print_ubi_mbr(ubi_mbr);
}

static int create_mtdparts_do(void)
{
	int ret = 0;
	char cmd[2][20];
	char *argv[2];

	argv[0] = cmd[0];
	argv[1] = cmd[1];

	memset(cmd, 0x00, sizeof(cmd));
	sprintf(argv[0], "mtdparts");
	sprintf(argv[1], "default");
	ret = sunxi_do_mtdparts(0, 2, argv);
	if (!ret) {
		debug("mtdparts info:\n");
		sunxi_do_mtdparts(0, 1, argv);
	}
	return ret;
}

static int ubi_attach_mtd_do(int mtdnum)
{
	int ret;
	char cmd[6][20];
	char *argv[6];
	char *mtd_name;
	int i;

	mtd_name = sunxi_get_mtdparts_name(mtdnum);
	if (mtd_name == NULL) {
		pr_err("mtd_name is NULL !!!\n");
		return -EINVAL;
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
		pr_err("ubi part %s err !\n", mtd_name);

	return ret;

}

static int check_volume_existed_do(char *name)
{
	char cmd[6][20];
	char *argv[6];
	int i;

	if (get_volnum_by_name(name) < 0) {
		pr_err("not found volume %s in mbr !!!\n", name);
		return -ENODEV;
	}

	for (i = 0; i < 6; i++)
		argv[i] = cmd[i];

	memset(cmd, 0x00, sizeof(cmd));
	sprintf(&cmd[0][0], "ubi");
	sprintf(&cmd[1][0], "check");
	sprintf(&cmd[2][0], "%s", name);

	return sunxi_do_ubi(0, 3, argv);
}

static void check_and_adjust_ubi_mbr(struct ubi_nand_vol *vol, struct ubi_mbr *ubi_mbr, int partno)
{
	int volumesize;
	int i = 0;
	char cmd[3][20];
	char *argv[3];

	for (i = 0; i < 3; i++)
		argv[i] = cmd[i];

	memset(cmd, 0x00, sizeof(cmd));
	sprintf(argv[0], "ubi");
	sprintf(argv[1], "get");
	sprintf(argv[2], "%s", vol->name);

	/* ubi create vloume size type */
	volumesize = sunxi_do_ubi(0, 3, argv);
	if (volumesize != -1) {
		if (to_sects(volumesize) != ubi_mbr->vols[partno].sects) {
			pr_info("existed volume %s size is %d(sects) , inside ubi part size is %d(sects),"
					"existed volume size to update inside ubi part size\n",
					vol->name, to_sects(volumesize), ubi_mbr->vols[partno].sects);
			ubi_mbr->vols[partno].sects = to_sects(volumesize);
		}
	}

}


static int create_ubi_volume_do(struct ubi_nand_vol *vol)
{
	int i, ret;
	char cmd[5][20];
	char *argv[5];

	for (i = 0; i < 5; i++)
		argv[i] = cmd[i];

	ret = check_volume_existed_do((char *)vol->name);
	if (!ret) {
		pr_info("volume %s existed, not create again\n", vol->name);
		return ret;
	}

	memset(cmd, 0x00, sizeof(cmd));
	sprintf(argv[0], "ubi");
	sprintf(argv[1], "create");
	sprintf(argv[2], "%s", vol->name);
	sprintf(argv[3], "0x%016x", to_bytes(vol->sects));
	if (vol->type & TYPE_STATIC_VOLUME)
		sprintf(&cmd[4][0], "s");
	else
		sprintf(&cmd[4][0], "d");

	/* ubi create vloume size type */
	ret = sunxi_do_ubi(0, 5, argv);
	if (ret) {
		pr_err("'ubi create %s %s %s' failed, return %d\n",
			argv[2], argv[3], argv[4], ret);
		return ret;
	}
	return 0;
}

static int create_ubi_volume(struct ubi_info *ubinfo)
{
	int ret = -EINVAL;
	int i;
	char *mtd_name;
	int mtd_num;
	unsigned int mtd_bytes;
	unsigned int vol_total_bytes = 0;
	struct ubi_mbr *ubi_mbr = ubi_to_ubi_mbr(ubinfo);
	struct ubi_mbr *nand_mbr = ubi_to_nand_mbr(ubinfo);
	struct ubi_status *ubi_status = ubi_to_ubi_status(ubinfo);

	if (ubi_mbr == NULL) {
		pr_err("UBI mbr is NULL !!!\n");
		return -EINVAL;
	}

	if (!ubi_mbr->part_cnt)
		return 0;

	mtd_num = get_mtd_num_to_attach();
	if (mtd_num < 0) {
		pr_err("invalid mtd num %d\n", mtd_num);
		return -EINVAL;
	}

	mtd_name = sunxi_get_mtdparts_name(mtd_num);
	mtd_bytes = sunxi_get_mtdpart_size(mtd_num);
	if (mtd_name == NULL) {
		pr_err("mtd_name is NULL !!!\n");
		return -EINVAL;
	}

	for (i = 0; i < ubi_mbr->part_cnt; i++)
		vol_total_bytes += to_bytes(ubi_mbr->vols[i].sects);

	if (vol_total_bytes > mtd_bytes) {
		pr_err("ubi volume total size is larger than mtd size.\n"
			"ubi_vol_total_bytes : 0x%x, mtd_bytes: 0x%x\n",
			vol_total_bytes, mtd_bytes);
		return -EINVAL;
	}

	pr_info("ubi attatch mtd, name: %s\n\n", mtd_name);

	ret = ubi_attach_mtd_do(mtd_num);
	if (ret)
		return ret;

	for (i = 0; i < ubi_mbr->part_cnt; i++) {
		ret = create_ubi_volume_do(&ubi_mbr->vols[i]);
		if (ret)
			return ret;
		ubi_status->vols[i].state = CREATE_UBI_VOL;
	}

	/*
	 * reset the last volume size
	 * The last volume always be 0 for aw sys_partition.
	 * It means that the last volume should resize to the max size.
	 * However, the max size is defined by ubi, so we insert a function
	 * @check_add_adjust_ubi_mbr to check and get the last volume size,
	 */
	check_and_adjust_ubi_mbr(&ubi_mbr->vols[i - 1], nand_mbr, i - 1);
	check_and_adjust_ubi_mbr(&ubi_mbr->vols[i -1], ubi_mbr, i -1);
	if (ubinfo->last_vol_sects == 0)
		ubinfo->last_vol_sects = nand_mbr->vols[i - 1].sects;
	return ret;
}

static int read_ubi_volume_do(char *vol_name, unsigned int sectors, void *buf)
{
	size_t bytes;
	char cmd[6][20];
	char *argv[6];
	int i;

	bytes = to_bytes(sectors);

	for (i = 0; i < 6; i++)
		argv[i] = cmd[i];

	memset(cmd, 0x00, sizeof(cmd));
	sprintf(&cmd[0][0], "ubi");
	sprintf(&cmd[1][0], "read");
	sprintf(&cmd[2][0], "0x%016x", (unsigned int)(ulong)buf);
	sprintf(&cmd[3][0], "%s", vol_name);
	sprintf(&cmd[4][0], "0x%016x", (unsigned int)(ulong)bytes);

	return sunxi_do_ubi(0, 5, argv);
}

static int write_ubi_volume_do(char *vol_name, void *buf, unsigned int bytes,
		unsigned int full_bytes)
{
	char cmd[6][20];
	char *argv[6];
	int i;

	memset(cmd, 0x00, sizeof(cmd));

	for (i = 0; i < 6; i++)
		argv[i] = cmd[i];

	/* ubi write.part address volume size [fullsize] */
	sprintf(&cmd[0][0], "ubi");
	sprintf(&cmd[1][0], "write.part");
	sprintf(&cmd[2][0], "0x%016x", (unsigned int)(ulong)buf);
	sprintf(&cmd[3][0], "%s", vol_name);
	sprintf(&cmd[4][0], "0x%016x", bytes);
	sprintf(&cmd[5][0], "0x%016x", full_bytes);

	if (full_bytes)
		return sunxi_do_ubi(0, 6, argv);
	else
		return sunxi_do_ubi(0, 5, argv);
}

static int write_ubi_volume(struct ubi_info *ubinfo, char *name,
		unsigned int sectors, void *buf)
{
	int ret;
	size_t full_bytes, bytes;
	unsigned int plan_wr_sects, written_sects;
	int num;
	struct ubi_status *ubi_status = ubi_to_ubi_status(ubinfo);
	char *last_name = ubinfo->last_name;

	num = get_volnum_by_name(name);
	if (num < 0) {
		pr_err("not found volume named %s\n", name);
		return -EINVAL;
	}

	plan_wr_sects = ubi_status->vols[num].plan_wr_sects;
	written_sects = ubi_status->vols[num].written_sects;
	bytes = to_bytes(sectors);

	if ((written_sects + sectors) > plan_wr_sects) {
		pr_err("write %u sectors over limit %u sectors, resize it\n",
				sectors, plan_wr_sects - written_sects);
		bytes = to_bytes(plan_wr_sects - written_sects);
	}

	full_bytes = 0;
	if (strcmp(last_name, name)) {
		if (ubi_status->vols[num].state != CREATE_UBI_VOL) {
			int vol_num;
			struct ubi_mbr *ubi_mbr = ubi_to_ubi_mbr(ubinfo);
			struct ubi_nand_vol *vol;

			vol_num = get_volnum_by_name(name);
			if (num < 0) {
				pr_err("not found volume named %s\n", name);
				return -EINVAL;
			}
			vol = &ubi_mbr->vols[vol_num];

			if (check_volume_existed_do(name)) {
				ret = create_ubi_volume_do(vol);
				if (ret)
					return ret;
			}

		}

		strcpy(last_name, name);
		full_bytes = to_bytes(plan_wr_sects);
	}
	ret = write_ubi_volume_do(name, buf, bytes, full_bytes);
	if (ret)
		pr_err("write volume %s with bytes %u full_bytes %u failed\n",
				name, bytes, full_bytes);

	ubi_status->last_partno = num;
	ubi_status->vols[num].written_sects += sectors;
	ubi_status->vols[num].state = WRITE_UBI_VOL;

	return ret;
}

static int read_mtd_do(loff_t from, size_t len, size_t *retlen, u_char *buf)
{
	struct mtd_info *mtd = NULL;
#ifdef CONFIG_AW_MTD_SPINAND
	if (support_spinand()) {
		struct aw_spinand *spinand = get_spinand();
		mtd = spinand_to_mtd(spinand);
	}
#endif
#ifdef CONFIG_AW_MTD_RAWNAND
	if (support_rawnand()) {
		struct aw_nand_chip *chip = get_rawnand();
		mtd = awnand_chip_to_mtd(chip);
	}
#endif

	return mtd ? mtd_read(mtd, from, len, retlen, buf) : -EINVAL;
}

static int write_mtd_do(loff_t to, size_t len, size_t *retlen, u_char *buf)
{
	struct mtd_info *mtd = NULL;

#ifdef CONFIG_AW_MTD_SPINAND
	if (support_spinand()) {
		struct aw_spinand *spinand = get_spinand();
		mtd = spinand_to_mtd(spinand);
	}
#endif

#ifdef CONFIG_AW_MTD_RAWNAND
	if (support_rawnand()) {
		struct aw_nand_chip *chip = get_rawnand();
		mtd = awnand_chip_to_mtd(chip);
	}
#endif

	return mtd ? mtd_write(mtd, to, len, retlen, buf) : -EINVAL;
}

static int flush_mtd_dev(struct ubi_mtd_info *mtd_info)
{
	u_char *upd_buf;
	int num;
	unsigned int col;
	size_t retlen;
	unsigned int to, len, pgsize, offset;
	unsigned int plan_wr_bytes, written_bytes, upd_received;
	int ret;

	num = mtd_info->last_partno;
	if (num < 0)
		return 0;

	offset = mtd_info->part[num].offset;
	pgsize = mtd_info->pagesize;
	upd_buf = (u_char *)mtd_info->wbuf;

	upd_received = to_bytes(mtd_info->part[num].written_sects);
	written_bytes = upd_received / pgsize * pgsize;

	plan_wr_bytes = to_bytes(mtd_info->part[num].plan_wr_sects);
	plan_wr_bytes = to_align(plan_wr_bytes, pgsize);

	if (written_bytes >= plan_wr_bytes)
		return 0;

	len = plan_wr_bytes - written_bytes;
	col = upd_received % pgsize;
	if (col)
		memset(upd_buf + col, 0xff, pgsize - col);
	else
		memset(upd_buf, 0xff, pgsize);

	while (len) {
		to = offset + written_bytes;
		ret = write_mtd_do(to, pgsize, &retlen, upd_buf);
		if (ret) {
			pr_err("mtd write failed with size:%x, ret:%u\n",
				pgsize, retlen);
			return ret;
		}

		memset(upd_buf, 0xff, pgsize);
		written_bytes += pgsize;
		len -= pgsize;
	}
	mtd_info->part[num].written_sects = to_sects(written_bytes);

	pr_info("volume %s total written :0x%x\n", mtd_info->part[num].name,
		to_sects(written_bytes));

	return 0;
}

static int read_mtd_part(struct ubi_mtd_info *mtd_info, int num,
	unsigned int start, unsigned int sectors, void *buffer)
{
	size_t len, retlen;
	unsigned int offset;
	unsigned int partsize;
	unsigned int from;

	partsize = mtd_info->part[num].bytes;
	offset = mtd_info->part[num].offset;
	len = to_bytes(sectors);
	from = offset + to_bytes(start);

	if (to_bytes(start + sectors) > partsize) {
		pr_err("%s,out of mtdsize(0x%x).\n", __func__, partsize);
		pr_err("start:%0x, sectors:%0x.\n", start, sectors);
		return -EINVAL;
	}

	flush_mtd_dev(mtd_info);

	return read_mtd_do(from, len, &retlen, buffer);

}

static int write_mtd_part(struct ubi_mtd_info *mtd_info, int num,
	unsigned int sectors, void *buffer)
{
	static int last_num = -1;
	size_t len, retlen;
	unsigned int col, rest_pgsize;
	unsigned int to, offset, pgsize, upd_received;
	u_char *src_buf, *upd_buf;

	if (num != last_num) {
		pr_err("First mtd write. part_name : %s, wr_sectors: 0x%0x\n",
			mtd_info->part[num].name, sectors);
		pr_err("%s, cur_num : 0x%x. last_num : 0x%x\n", __func__,
			num, last_num);

		/* the first write mtdx, then flush the last_num mtdx */
		flush_mtd_dev(mtd_info);

		last_num = num;
		mtd_info->part[num].written_sects = 0;
		mtd_info->last_partno = num;
	}

	offset = mtd_info->part[num].offset;
	upd_received = to_bytes(mtd_info->part[num].written_sects);
	src_buf = (u_char *)buffer;
	upd_buf = (u_char *)mtd_info->wbuf;
	pgsize = mtd_info->pagesize;
	if (pgsize > MTD_W_BUF_SIZE) {
		pr_err("Err: pgsize(0x%x) is larger than bufsize(64KB).\n", pgsize);
		return -EINVAL;
	}

	len = to_bytes(sectors);
	col = upd_received % pgsize;
	rest_pgsize = pgsize - col;
	if (col) {
		if (len >= rest_pgsize) {
			pr_debug("col:0x%x, rest_pgsize:%x, len:%x\n",
				col, rest_pgsize, len);
			pr_debug("mtd writing :0x%x\n", rest_pgsize >> 9);

			memcpy(upd_buf + col, src_buf, rest_pgsize);
			to = offset + upd_received / pgsize * pgsize;
			if (write_mtd_do(to, pgsize, &retlen, upd_buf)) {
				pr_err("%d, %s, Err:wr_size:%x, written:%0x\n",
					__LINE__, __func__, pgsize, retlen);
				return -EINVAL;
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
			pr_debug("mtd writing :0x%x\n", to_bytes(pgsize));

			memcpy(upd_buf, src_buf, pgsize);
			to = offset + upd_received;
			if (write_mtd_do(to, pgsize, &retlen, upd_buf)) {
				pr_err("%d, %s,Err: wr_size:%x, written:%0x\n",
					__LINE__, __func__, pgsize, retlen);
				return -EINVAL;
			}

			src_buf += pgsize;
			len -= pgsize;
			upd_received += pgsize;
		} else {
			debug("mtd copy to buf: 0x%x\n", (unsigned int)(len >> 9));

			memcpy(upd_buf, src_buf, len);
			upd_received += len;
			len = 0;
		}
	}
	mtd_info->part[num].written_sects = to_sects(upd_received);

	pr_debug("%s total written :0x%x\n", mtd_info->part[num].name,
			to_sects(upd_received));

	return 0;
}

static int fill_gap_do(struct ubi_info *ubinfo, char *vol_name,
		int64_t gap_size, char fill_data)
{
	char *gap_buf;
	int i, ret = 0;

	struct ubi_device *ubi = ubi_devices[0];
	struct ubi_status *ubi_status = ubi_to_ubi_status(ubinfo);

	int last_volnum = get_volnum_by_name(vol_name);
	if (last_volnum < 0)
		return -EINVAL;

	int leb_sects = to_sects(ubi->leb_size);
	int64_t  remain = 0;
	fill_data = 0x55;

	if (!gap_size)
		return 0;

	pr_info("fill gap start: volume %s sects 0x%x\n",
			vol_name, (unsigned int )gap_size);

	gap_buf = (void *)malloc(to_bytes(GAP_BUF_MAX_SECTS));
	if (!gap_buf) {
		pr_err("Err: malloc memory fail, line: %d, %s\n",
			__LINE__,  __func__);
		return -ENOMEM;
	}

	for (i = 0; i < to_bytes(GAP_BUF_MAX_SECTS); i++) {
		if (i % 2 == 0)
			gap_buf[i] = 0x55;
		else if (i % 2 == 1)
			gap_buf[i] = 0xaa;
	}

	if (ubi_status->vols[last_volnum].written_sects % leb_sects) {
		remain = (leb_sects - (ubi_status->vols[last_volnum].written_sects % leb_sects));

		/*to fill the last used peb of the volume*/
		for (i = remain; i > 0 ; i -= GAP_BUF_MAX_SECTS) {
			ret = write_ubi_volume(ubinfo, vol_name,
					min_t(unsigned int, i, GAP_BUF_MAX_SECTS), gap_buf);
			if (ret) {
				pr_err("fill gap fail with %d back!\n", ret);
				free(gap_buf);
				return ret;
			}
		}
	}

	/*data pattern 0xff don't really write to flash*/
	memset(gap_buf, 0xff, GAP_BUF_MAX_SECTS << 9);
	for (i = gap_size - remain; i > 0; i -= GAP_BUF_MAX_SECTS) {
		ret = write_ubi_volume(ubinfo, vol_name,
			min_t(unsigned int, i, GAP_BUF_MAX_SECTS), gap_buf);
		if (ret) {
			pr_err("fill gap fail with %d back!\n", ret);
			free(gap_buf);
			return ret;
		}
	}

	pr_info("fill gap end: volume %s\n", vol_name);
	free(gap_buf);
	return 0;
}

static int fill_gap(struct ubi_info *ubinfo, int partno)
{
	int ret = 0;
	int64_t gap = 0;
	char *name;
	struct ubi_status *ubi_status = ubi_to_ubi_status(ubinfo);
	struct ubi_mbr *nand_mbr = ubi_to_nand_mbr(ubinfo);
	unsigned last_volnum;

	if (partno < 0)
		return 0;

	name = (char *)nand_mbr->vols[partno].name;
	last_volnum = get_volnum_by_name(name);
	if (last_volnum < 0)
		return -EINVAL;

	gap = ubi_status->vols[last_volnum].plan_wr_sects -
		ubi_status->vols[last_volnum].written_sects;

	if (gap)
		ret = fill_gap_do(ubinfo, name, gap, 0x00);
	return ret;
}

static int read_ubi_volume(struct ubi_info *ubinfo, char *name,
		unsigned int offs, unsigned int sectors, void *buf)
{
	size_t full_size, size;
	int num;
	loff_t offp;
	struct ubi_mbr *ubi_mbr = ubi_to_ubi_mbr(ubinfo);

	num = get_volnum_by_name(name);
	if (num < 0) {
		pr_err("volume %s is not in ubi mbr !!!\n", name);
		return -ENODEV;
	}

	offp = to_bytes((loff_t)offs);
	full_size = to_bytes((size_t)ubi_mbr->vols[num].sects);
	size = (size_t)sectors << 9;
	if (size > full_size) {
		pr_warn("Warning: par overflow.");
		pr_warn("size(byte):%0x, full_size(byte):%0x\n",
				size, full_size);
		size = full_size;
	}

	/*
	 * aw burning tools will check the written data if finish downloading
	 * image, we should fill full to volume.
	 */
	if (num == ubinfo->last_partno) {
		int ret;

		ret = fill_gap(ubinfo, num);
		if (ret) {
			pr_err("ubi flush fail!!!\n");
			return ret;
		}
	}

	return sunxi_ubi_volume_read(name, offp, (char *)buf, size);
}

static int init_ubi_info(struct ubi_info *ubinfo,
		sunxi_mbr_t *org_mbr)
{
	int ret;
	struct ubi_mtd_info *mtd_info = ubi_to_mtd(ubinfo);
	struct ubi_mbr *nand_mbr = ubi_to_nand_mbr(ubinfo);

	ret = init_mtd_info(mtd_info);
	if (ret)
		return ret;

	ret = init_nand_mbr(org_mbr, nand_mbr);
	if (ret)
		return ret;

	init_ubi_mbr(ubinfo);
	ubinfo->last_offset = ubinfo->last_partno = -1;
	return 0;
}

static int init_ubi(struct ubi_info *ubinfo,
		sunxi_mbr_t *org_mbr)
{
	int ret;

	ret = create_mtdparts_do();
	if (ret) {
		pr_err("create mtd partitions failed\n");
		return ret;
	}

	ret = init_ubi_info(ubinfo, org_mbr);
	if (ret)
		return ret;

	ret = create_ubi_volume(ubinfo);
	if (ret)
		return ret;


	return 0;
}


#define PART_ENV_BUF_SIZE 1024

static unsigned int get_gpt_from_mbr_vol(unsigned int start,
		unsigned int sectors, void *buffer)
{
	int mtd_num;
	int ret = 0;
	char *mbr_buf = NULL, *gpt_buf = NULL;
	struct ubi_info *ubinfo = get_ubi_info();
	struct ubi_mbr *nand_mbr = ubi_to_nand_mbr(ubinfo);
	struct ubi_mtd_info *mtd_info = ubi_to_mtd(ubinfo);

	gpt_buf = malloc(8 * 1024);
	if (!gpt_buf) {
		pr_err("malloc 8k gpt_buf fail\n");
		goto err;
	}
	memset(gpt_buf, 0, 8 * 1024);

	mtd_num = get_mtd_num_by_name("mbr");
	if (mtd_num >= 0) {
		ret = read_mtd_part(mtd_info, mtd_num, start, sectors,
				buffer);
	} else {
		mbr_buf = (char *)malloc(to_bytes(MBR_SECTORS));
		if (!mbr_buf) {
			pr_err("allocate for mbr buf failed\n");
			goto free_gpt_buf;
		}
		ret = read_ubi_volume_do("mbr", MBR_SECTORS, mbr_buf);
		if (ret) {
			pr_err("read volume mbr failed return %d\n", ret);
			goto free_mbr_buf;
		}

		ret = check_sunxi_mbr((sunxi_mbr_t *)mbr_buf);
		if (ret)
			goto free_mbr_buf;
		sunxi_mbr_convert_to_gpt(mbr_buf, (char *)gpt_buf, STORAGE_SPI_NAND);
		memcpy(buffer, gpt_buf + to_bytes(start), to_bytes(sectors));
	}

	if (!ret && !start && !nand_mbr->part_cnt) {
		ret = init_ubi_info(ubinfo, (sunxi_mbr_t *)mbr_buf);
		if (ret)
			goto free_mbr_buf;
	}

	ret = sectors;
free_mbr_buf:
	if (mbr_buf)
		free(mbr_buf);
free_gpt_buf:
	if (gpt_buf)
		free(gpt_buf);
err:
	return ret;
}

/*
 * The last volume always be 0 for aw sys_partition.
 * It means that the last volume should resize to the max size.
 * However, the max size is defined by ubi, so we insert a function
 * @mtd_set_last_vol_sects to get the last volume size,
 */
int mtd_set_last_vol_sects(unsigned int sects)
{
	struct ubi_info *ubinfo = get_ubi_info();

	ubinfo->last_vol_sects = sects;
	pr_info("reset last volume size to 0x%x\n", sects);
	return 0;
}

#ifdef CONFIG_AW_MTD_SPINAND
uint64_t spinand_sys_part_offset(void)
{
	struct aw_spinand *spinand = get_spinand();
	struct aw_spinand_chip *chip = spinand_to_chip(spinand);
	struct aw_spinand_info *info = chip->info;
	unsigned int phy_blk_bytes = info->phy_block_size(chip);
	unsigned int uboot_end;
	uint64_t offset = 0;

	spinand_uboot_blknum(NULL, &uboot_end);
	offset += uboot_end * phy_blk_bytes;
	offset += AW_SPINAND_RESERVED_PHY_BLK_FOR_SECURE_STORAGE * phy_blk_bytes;
#if IS_ENABLED(CONFIG_AW_SPINAND_PSTORE_MTD_PART)
	offset += AW_SPINAND_RESERVED_FOR_PSTORE_KB * SZ_1K;;
#endif
#if IS_ENABLED(CONFIG_RAW_KERNEL)
	unsigned int blk_bytes = info->block_size(chip);
	/* kernel */
	if (CONFIG_KERNEL_SIZE_BYTE) {
		offset += ALIGN(CONFIG_KERNEL_SIZE_BYTE, blk_bytes);
	}
#endif

	return offset;
}

static void set_env_mtdparts_spinand(void)
{
	struct ubi_info *ubinfo = get_ubi_info();
	struct aw_spinand *spinand = get_spinand();
	struct aw_spinand_chip *chip = spinand_to_chip(spinand);
	struct aw_spinand_info *info = chip->info;
	unsigned int phy_blk_bytes = info->phy_block_size(chip);
	int wlen;
	struct ubi_mtd_part mtd_part;
	unsigned int uboot_start, uboot_end, offset = 0;

	spinand_uboot_blknum(&uboot_start, &uboot_end);
	ubinfo->ubootblks = uboot_end - uboot_start;
	wlen = snprintf(ubinfo->mtdparts, 512, "mtdparts=nand:");

	spinand->sec_sto.chip = chip;
	spinand->sec_sto.startblk = uboot_end;
	spinand->sec_sto.endblk = uboot_end +
		AW_SPINAND_RESERVED_PHY_BLK_FOR_SECURE_STORAGE;

	/* boot0 */
	snprintf((char *)&mtd_part.name, PART_NAME_MAX_SIZE, "boot0");
	mtd_part.offset = 0;
	mtd_part.bytes = uboot_start * phy_blk_bytes;
	offset += mtd_part.bytes;
	wlen += snprintf(ubinfo->mtdparts + wlen, 512 - wlen,
			"%uk@%u(%s)ro,", mtd_part.bytes / SZ_1K,
			mtd_part.offset, mtd_part.name);
	/* uboot */
	snprintf((char *)&mtd_part.name, PART_NAME_MAX_SIZE, "uboot");
	mtd_part.offset = offset;
	mtd_part.bytes = uboot_end * phy_blk_bytes - mtd_part.offset;
	offset += mtd_part.bytes;
	wlen += snprintf(ubinfo->mtdparts + wlen, 512 - wlen,
			"%uk@%u(%s)ro,", mtd_part.bytes / SZ_1K,
			mtd_part.offset, mtd_part.name);
	/* secure storage */
	snprintf((char *)&mtd_part.name, PART_NAME_MAX_SIZE, "secure_storage");
	mtd_part.offset = offset;
	mtd_part.bytes = AW_SPINAND_RESERVED_PHY_BLK_FOR_SECURE_STORAGE;
	mtd_part.bytes *= phy_blk_bytes;
	offset += mtd_part.bytes;
	wlen += snprintf(ubinfo->mtdparts + wlen, 512 - wlen,
			"%uk@%u(%s)ro,", mtd_part.bytes / SZ_1K,
			mtd_part.offset, mtd_part.name);
#if IS_ENABLED(CONFIG_AW_SPINAND_PSTORE_MTD_PART)
	/* pstore */
	snprintf((char *)&mtd_part.name, PART_NAME_MAX_SIZE, "pstore");
	mtd_part.offset = offset;
	mtd_part.bytes = AW_SPINAND_RESERVED_FOR_PSTORE_KB * SZ_1K;
	offset += mtd_part.bytes;
	wlen += snprintf(ubinfo->mtdparts + wlen, 512 - wlen,
			"%uk@%u(%s)ro,", mtd_part.bytes / SZ_1K,
			mtd_part.offset, mtd_part.name);
#endif
	/* user data */
	wlen += snprintf(ubinfo->mtdparts + wlen, 512 - wlen, "-(sys)");

	pr_info("mtdparts: %s\n", ubinfo->mtdparts);
	snprintf(ubinfo->mtdids, 20, "nand0=nand");
	sunxi_set_defualt_mtdpart(ubinfo->mtdids, ubinfo->mtdparts);
}

int spinand_mtd_flush_last_volume(void)
{
	struct ubi_info *ubinfo = get_ubi_info();

	fill_gap(ubinfo, ubinfo->last_partno);
	ubinfo->last_partno = -1;
	*ubinfo->last_name = 0;
	return 0;
}

unsigned int spinand_mtd_write_ubi(unsigned int start, unsigned int sectors,
		void *buffer)
{
	char *name = NULL;
	int ret;
	int num, mtd_num;
	struct ubi_info *ubinfo = get_ubi_info();
	struct ubi_mbr *nand_mbr = ubi_to_nand_mbr(ubinfo);
	struct ubi_mtd_info *mtd_info = ubi_to_mtd(ubinfo);
	struct ubi_status *ubi_status = ubi_to_ubi_status(ubinfo);
	unsigned int offset;

	/*
	 * The start sectors [0-80) is used for mbr. Write to this area means
	 * spinand ubi haven't initialized, we do it now.
	 */

	if (!start && (sectors <= MBR_SECTORS)) {
		if (sectors != MBR_SECTORS) {
			pr_err("the buffer size for mbr is wrongly: %d sectors\n", sectors);
			goto err;
		}

		ret = init_ubi(ubinfo, buffer);
		if (ret) {
			pr_err("initialize sunxi spinand ubi failed\n");
			goto err;
		}
		/*
		 * After creating all volumes, we got the actucal size of last
		 * partition(UDISK), it`s time to correct the corresponding
		 * partition entry info in sunxi_mbr
		 */
		if (WORK_MODE_BOOT != get_boot_work_mode())
			adjust_sunxi_mbr(buffer, ubinfo->last_vol_sects);
	}

	num = get_partno_by_addr(start, sectors);
	if (!sectors || num < 0)
		goto err;

	/*
	 * new start for a volume, it means this volume has data to download.
	 * we should set ubi_status
	 */
	ret = offset_on_part(num, start);
	if (!ret) {
		/*
		 * the last volume has 0 size default for aw platform with
		 * sys_partition. But some customers need to download image to
		 * the last volume, which need the real size of the last volume.
		 */
		if (num + 1 == nand_mbr->part_cnt)
			ubi_status->vols[num].plan_wr_sects =
				ubinfo->last_vol_sects;
		else
			ubi_status->vols[num].plan_wr_sects =
				nand_mbr->vols[num].sects;
		ubi_status->vols[num].written_sects = 0;
	}

	name = (char *)nand_mbr->vols[num].name;
	mtd_num = get_mtd_num_by_name(name);
	if (mtd_num >= 0) {
		ret = write_mtd_part(mtd_info, mtd_num, sectors, buffer);
		return ret ? 0 : sectors;
	}

	ret = check_volume_existed_do(name);
	if (ret) {
		pr_err("Warning: ubi volume does not exist!\n");
		goto err;
	}

	offset = offset_on_part(num, start);
	if (num == ubinfo->last_partno) {
		if (offset != ubinfo->last_offset) {
			pr_err("offset 0x%x smaller than last offset 0x%x\n",
					ubinfo->last_offset, offset);
			goto err;
		}
	} else {
		/* write to new volume */
		if (offset) {
			pr_err("offset should be 0 but 0x%x when write to new volume %s\n",
					offset, (char *)nand_mbr->vols[num].name);
			return -EINVAL;
		}
		ret = fill_gap(ubinfo, ubinfo->last_partno);
		if (ret != 0)
			pr_warn("fill gap fail, part:%s, start:%0x, sectors:%0x\n",
				name, start, sectors);
	}
	ubinfo->last_offset = offset + sectors;
	ubinfo->last_partno = num;

	pr_debug("write to volume %s: offset 0x%x sects 0x%x\n", name,
			start, sectors);

	ret = write_ubi_volume(ubinfo, name, sectors, buffer);

	return ret ? 0 : sectors;
err:
	return 0;
}


unsigned int spinand_mtd_read_ubi(unsigned int start, unsigned int sectors,
		void *buffer)
{
	char *name = NULL;
	unsigned int offset;
	int ret = 0;
	int num, mtd_num;
	struct ubi_info *ubinfo = get_ubi_info();
	struct ubi_mbr *nand_mbr = ubi_to_nand_mbr(ubinfo);
	struct ubi_mtd_info *mtd_info = ubi_to_mtd(ubinfo);

	if ((start < MBR_SECTORS) &&
			((start + sectors) <= MBR_SECTORS)) {
		return get_gpt_from_mbr_vol(start, sectors, buffer);
	}

	num = get_partno_by_addr(start, sectors);
	if (!sectors || num < 0)
		return 0;

	name   = (char *)nand_mbr->vols[num].name;
	offset = offset_on_part(num, start);

	mtd_num = get_mtd_num_by_name(name);
	if (mtd_num >= 0) {
		ret = read_mtd_part(mtd_info, mtd_num, offset, sectors,
					  buffer);
		return ret < 0 ? 0 : sectors;
	}

	ret = check_volume_existed_do(name);
	if (ret) {
		debug("Warning: ubi volume does not exist!\n");
		return 0;
	}

	ret = read_ubi_volume(ubinfo, name, offset, sectors, buffer);
	return ret ? 0 : sectors;
}


int spinand_mtd_update_ubi_env(void)
{
	struct ubi_info *ubinfo = get_ubi_info();
	struct ubi_mbr *ubi_mbr = ubi_to_ubi_mbr(ubinfo);
	char parts_set[PART_ENV_BUF_SIZE], *ppset;
	char buf[PART_ENV_BUF_SIZE];
	char *nand_root = NULL;
	char *rootname;
	int i;

#define BLOCK_DEVICE "/dev/ubiblock"
#define BLOCK_DEVICE_NUM "0_x"

	rootname = env_get("root_partition");
	if (rootname)
		pr_info("root_partition is %s\n", rootname);

	nand_root = env_get("nand_root");
	if (rootname)
		pr_info("nand_root is %s\n", nand_root);

	memset(parts_set, 0, PART_ENV_BUF_SIZE);
	ppset = parts_set;
	for (i = 0; i < ubi_mbr->part_cnt; i++) {
		memset(buf, 0, PART_ENV_BUF_SIZE);

		/* set parts_set */
		snprintf(buf, PART_ENV_BUF_SIZE, "ubi0_%d", i);
		ppset += snprintf(ppset,
				PART_ENV_BUF_SIZE - (ppset - parts_set),
				"%s@%s:", ubi_mbr->vols[i].name, buf);

		/* set root */
		if (rootname && !strcmp(rootname, ubi_mbr->vols[i].name)) {
			if (!strncmp(nand_root, BLOCK_DEVICE, strlen(BLOCK_DEVICE))) {
				/*rootfs mount as ext4、squashfs ...*/
				snprintf(buf, PART_ENV_BUF_SIZE,
						"/dev/ubiblock0_%d", i);
#ifdef CONFIG_SUNXI_DM_VERITY
				pr_info("set verity_dev to %s\n", buf);
				env_set("verity_dev", buf);
				pr_info("set root to /dev/dm-0\n");
				env_set("nand_root", "/dev/dm-0");
#else
				pr_info("set root to %s\n", buf);
				env_set("nand_root", buf);
#endif
			} else {
				/*rootfs mount as ubifs*/
				snprintf(buf, PART_ENV_BUF_SIZE,
						"ubi0_%d", i);
				pr_info("set root to %s\n", buf);
				env_set("nand_root", buf);
			}
		}
	}

	env_set("partitions", parts_set);


	snprintf(buf, PART_ENV_BUF_SIZE, "%u", ubinfo->ubootblks);
	pr_info("set aw-ubi-spinand.ubootblks to %u\n", ubinfo->ubootblks);
	env_set("aw-ubi-spinand.ubootblks", buf);

	snprintf(buf, PART_ENV_BUF_SIZE, "%d", get_mtd_num_to_attach());
	pr_info("set ubi_attach_mtdnum to %d\n", get_mtd_num_to_attach());
	env_set("ubi_attach_mtdnum", buf);

	pr_info("update ubi part info ok\n");
	return 0;
}

/*
* ubi->good_peb_count = ubi->peb_count - ubi->bad_peb_count; @ubi_attach
* @ubi_read_volume_table
* ubi->avail_pebs = ubi->good_peb_count - ubi->corr_peb_count
* ubi->avail_pebs -= all vol->reserved_pebs; @init_volumes
* ubi->avail_pebs -= UBI_LAYOUT_VOLUME_EBS; @init_volumes, 2
* ubi->avail_pebs -= WL_RESERVED_PEBS; @ubi_wl_init, 1
* ubi->avail_pebs -= EBA_RESERVED_PEBS; @ubi_eba_init, 1
* ubi->avail_pebs -= ubi->beb_rsvd_pebs; @ubi_eba_init
*/
int spinand_ubi_user_volumes_size(void)
{
	int pebs = 0;
	int leb_size = 0;
	int vol_pebs = 0;
	int i = 0;
	struct ubi_device *ubi = ubi_devices[0];

	if (ubi) {
		leb_size = ubi->leb_size;
		pebs = ubi->rsvd_pebs - ubi->beb_rsvd_pebs - 2 - 1 - 1;
		/*UBI_INT_VOL_COUNT: 1*/
		for (i = 0; i < (ubi->vol_count - 1); i++) {
			vol_pebs += ubi->volumes[i]->reserved_pebs;
		}
		if (vol_pebs != pebs)
			pr_info("warning vol_pebs: %d, %d\n", vol_pebs, pebs);
	}
	pr_info("logical area info: %d %d last_lba: %d\n", pebs, leb_size,
		pebs * (leb_size >> 9) - 1);
	return leb_size * pebs;
}

int spinand_mtd_attach_mtd(void)
{
	int num;
	struct ubi_info *ubinfo = get_ubi_info();
	struct ubi_mtd_info *mtd_info = ubi_to_mtd(ubinfo);

	/* support to configure size of mtd part 0 (boot_parts) */
	set_env_mtdparts_spinand();

	if (create_mtdparts_do())
		return -EINVAL;

	init_mtd_info(mtd_info);
	num = get_mtd_num_to_attach();
	if (num < 0)
		return -EINVAL;

	if (ubi_attach_mtd_do(num))
		return -EINVAL;

	return 0;

}

#endif /*CONFIG_AW_MTD_SPINAND*/

#ifdef CONFIG_AW_MTD_RAWNAND

static void set_env_mtdparts_rawnand(void)
{
	struct ubi_info *ubinfo = get_ubi_info();
	struct aw_nand_chip *chip = get_rawnand();

	struct aw_nand_sec_sto *rawnand_sec_sto = get_rawnand_sec_sto();
	unsigned int phy_blk_bytes = chip->erasesize;
	int wlen;
	struct ubi_mtd_part mtd_part;
	unsigned int uboot_start, uboot_end, offset = 0;

	rawnand_uboot_blknum(&uboot_start, &uboot_end);
	ubinfo->ubootblks = uboot_end - uboot_start;
	wlen = snprintf(ubinfo->mtdparts, 512, "mtdparts=nand:");

	rawnand_sec_sto->startblk = uboot_end;
	rawnand_sec_sto->endblk = uboot_end +
		AW_RAWNAND_RESERVED_PHY_BLK_FOR_SECURE_STORAGE;

	/* boot0 */
	snprintf((char *)&mtd_part.name, PART_NAME_MAX_SIZE, "boot0");
	mtd_part.offset = 0;
	mtd_part.bytes = uboot_start * phy_blk_bytes;
	offset += mtd_part.bytes;
	wlen += snprintf(ubinfo->mtdparts + wlen, 512 - wlen,
			"%uk@%u(%s)ro,", mtd_part.bytes / SZ_1K,
			mtd_part.offset, mtd_part.name);
	/* uboot */
	snprintf((char *)&mtd_part.name, PART_NAME_MAX_SIZE, "uboot");
	mtd_part.offset = offset;
	mtd_part.bytes = uboot_end * phy_blk_bytes - mtd_part.offset;
	offset += mtd_part.bytes;
	wlen += snprintf(ubinfo->mtdparts + wlen, 512 - wlen,
			"%uk@%u(%s)ro,", mtd_part.bytes / SZ_1K,
			mtd_part.offset, mtd_part.name);
	/* secure storage */
	snprintf((char *)&mtd_part.name, PART_NAME_MAX_SIZE, "secure_storage");
	mtd_part.offset = offset;
	mtd_part.bytes = AW_RAWNAND_RESERVED_PHY_BLK_FOR_SECURE_STORAGE;
	mtd_part.bytes *= phy_blk_bytes;
	offset += mtd_part.bytes;
	wlen += snprintf(ubinfo->mtdparts + wlen, 512 - wlen,
			"%uk@%u(%s)ro,", mtd_part.bytes / SZ_1K,
			mtd_part.offset, mtd_part.name);
#if IS_ENABLED(CONFIG_AW_RAWNAND_PSTORE_MTD_PART)
	/* pstore */
	snprintf((char *)&mtd_part.name, PART_NAME_MAX_SIZE, "pstore");
	mtd_part.offset = offset;
	mtd_part.bytes = AW_SPINAND_RESERVED_FOR_PSTORE_KB * SZ_1K;
	offset += mtd_part.bytes;
	wlen += snprintf(ubinfo->mtdparts + wlen, 512 - wlen,
			"%uk@%u(%s)ro,", mtd_part.bytes / SZ_1K,
			mtd_part.offset, mtd_part.name);
#endif
	/* user data */
	wlen += snprintf(ubinfo->mtdparts + wlen, 512 - wlen, "-(sys)");

	pr_info("mtdparts: %s\n", ubinfo->mtdparts);
	snprintf(ubinfo->mtdids, 20, "nand0=nand");
	sunxi_set_defualt_mtdpart(ubinfo->mtdids, ubinfo->mtdparts);
}

int rawnand_mtd_flush_last_volume(void)
{
	struct ubi_info *ubinfo = get_ubi_info();

	fill_gap(ubinfo, ubinfo->last_partno);
	ubinfo->last_partno = -1;
	*ubinfo->last_name = 0;
	return 0;
}

unsigned int rawnand_mtd_write_ubi(unsigned int start, unsigned int sectors,
		void *buffer)
{
	char *name = NULL;
	int ret;
	int num, mtd_num;
	struct ubi_info *ubinfo = get_ubi_info();
	struct ubi_mbr *nand_mbr = ubi_to_nand_mbr(ubinfo);
	struct ubi_mtd_info *mtd_info = ubi_to_mtd(ubinfo);
	struct ubi_status *ubi_status = ubi_to_ubi_status(ubinfo);
	unsigned int offset;

	/*
	 * The start sectors [0-80) is used for mbr. Write to this area means
	 * rawnand ubi haven't initialized, we do it now.
	 */
	if (!start && (sectors <= MBR_SECTORS)) {
		if (sectors != MBR_SECTORS) {
			pr_err("the buffer size for mbr is wrongly: %d sectors\n", sectors);
			goto err;
		}

		ret = init_ubi(ubinfo, buffer);
		if (ret) {
			pr_err("initialize sunxi rawnand ubi failed\n");
			goto err;
		}
		/*
		 * After creating all volumes, we got the actucal size of last
		 * partition(UDISK), it`s time to correct the corresponding
		 * partition entry info in sunxi_mbr
		 */
		printf("WORK_MODE_BOOT@%d get_boot_work_mode@%d\n",
				WORK_MODE_BOOT, get_boot_work_mode());
		if (WORK_MODE_BOOT != get_boot_work_mode())
			adjust_sunxi_mbr(buffer, ubinfo->last_vol_sects);
	}

	num = get_partno_by_addr(start, sectors);

	if (!sectors || num < 0)
		goto err;

	/*
	 * new start for a volume, it means this volume has data to download.
	 * we should set ubi_status
	 */
	ret = offset_on_part(num, start);
	if (!ret) {
		/*
		 * the last volume has 0 size default for aw platform with
		 * sys_partition. But some customers need to download image to
		 * the last volume, which need the real size of the last volume.
		 */
		if (num + 1 == nand_mbr->part_cnt)
			ubi_status->vols[num].plan_wr_sects =
				ubinfo->last_vol_sects;
		else
			ubi_status->vols[num].plan_wr_sects =
				nand_mbr->vols[num].sects;
		ubi_status->vols[num].written_sects = 0;
	}

	name = (char *)nand_mbr->vols[num].name;
	mtd_num = get_mtd_num_by_name(name);
	if (mtd_num >= 0) {
		ret = write_mtd_part(mtd_info, mtd_num, sectors, buffer);
		return ret ? 0 : sectors;
	}

	ret = check_volume_existed_do(name);
	if (ret) {
		pr_err("Warning: ubi volume does not exist!\n");
		goto err;
	}

	offset = offset_on_part(num, start);
	if (num == ubinfo->last_partno) {
		if (offset != ubinfo->last_offset) {
			pr_err("offset 0x%x smaller than last offset 0x%x\n",
					ubinfo->last_offset, offset);
			goto err;
		}
	} else {
		/* write to new volume */
		if (offset) {
			pr_err("offset should be 0 but 0x%x when write to new volume %s\n",
					offset, (char *)nand_mbr->vols[num].name);
			return -EINVAL;
		}
		ret = fill_gap(ubinfo, ubinfo->last_partno);
		if (ret != 0)
			pr_warn("fill gap fail, part:%s, start:%0x, sectors:%0x\n",
				name, start, sectors);
		/*
		 *pr_warn("don't fill gap, part:%s, start:%0x, sectors:%0x\n",
		 *                name, start, sectors);
		 */
	}
	ubinfo->last_offset = offset + sectors;
	ubinfo->last_partno = num;

	pr_debug("write to volume %s: offset 0x%x sects 0x%x\n", name,
			start, sectors);

	ret = write_ubi_volume(ubinfo, name, sectors, buffer);

	return ret ? 0 : sectors;
err:
	return 0;
}

unsigned int rawnand_mtd_read_ubi(unsigned int start, unsigned int sectors,
		void *buffer)
{
	char *name = NULL;
	unsigned int offset;
	int ret = 0;
	int num, mtd_num;
	struct ubi_info *ubinfo = get_ubi_info();
	struct ubi_mbr *nand_mbr = ubi_to_nand_mbr(ubinfo);
	struct ubi_mtd_info *mtd_info = ubi_to_mtd(ubinfo);

	if ((start < MBR_SECTORS) &&
			((start + sectors) <= MBR_SECTORS)) {
		return get_gpt_from_mbr_vol(start, sectors, buffer);
	}

	num = get_partno_by_addr(start, sectors);
	if (!sectors || num < 0)
		return 0;

	name   = (char *)nand_mbr->vols[num].name;
	offset = offset_on_part(num, start);

	mtd_num = get_mtd_num_by_name(name);
	if (mtd_num >= 0) {
		ret = read_mtd_part(mtd_info, mtd_num, offset, sectors,
					  buffer);
		return ret < 0 ? 0 : sectors;
	}

	ret = check_volume_existed_do(name);
	if (ret) {
		debug("Warning: ubi volume does not exist!\n");
		return 0;
	}

	ret = read_ubi_volume(ubinfo, name, offset, sectors, buffer);
	return ret ? 0 : sectors;
}

int rawnand_mtd_update_ubi_env(void)
{
	struct ubi_info *ubinfo = get_ubi_info();
	struct ubi_mbr *ubi_mbr = ubi_to_ubi_mbr(ubinfo);
	char parts_set[PART_ENV_BUF_SIZE], *ppset;
	char buf[PART_ENV_BUF_SIZE];
	char *nand_root = NULL;
	char *rootname;
	int i;

#define BLOCK_DEVICE "/dev/ubiblock"
#define BLOCK_DEVICE_NUM "0_x"

	rootname = env_get("root_partition");
	if (rootname)
		pr_info("root_partition is %s\n", rootname);

	nand_root = env_get("nand_root");
	if (rootname)
		pr_info("nand_root is %s\n", nand_root);

	memset(parts_set, 0, PART_ENV_BUF_SIZE);
	ppset = parts_set;
	for (i = 0; i < ubi_mbr->part_cnt; i++) {
		memset(buf, 0, PART_ENV_BUF_SIZE);

		/* set parts_set */
		snprintf(buf, PART_ENV_BUF_SIZE, "ubi0_%d", i);
		ppset += snprintf(ppset,
				PART_ENV_BUF_SIZE - (ppset - parts_set),
				"%s@%s:", ubi_mbr->vols[i].name, buf);

		/* set root */
		if (rootname && !strcmp(rootname, ubi_mbr->vols[i].name)) {
			if (!strncmp(nand_root, BLOCK_DEVICE, strlen(BLOCK_DEVICE))) {
				/*rootfs mount as ext4、squashfs ...*/
				snprintf(buf, PART_ENV_BUF_SIZE,
						"/dev/ubiblock0_%d", i);
#ifdef CONFIG_SUNXI_DM_VERITY
				pr_info("set verity_dev to %s\n", buf);
				env_set("verity_dev", buf);
				pr_info("set root to /dev/dm-0\n");
				env_set("nand_root", "/dev/dm-0");
#else
				pr_info("set root to %s\n", buf);
				env_set("nand_root", buf);
#endif

			} else {
				/*rootfs mount as ubifs*/
				snprintf(buf, PART_ENV_BUF_SIZE,
						"ubi0_%d", i);
				pr_info("set root to %s\n", buf);
				env_set("nand_root", buf);
			}
		}
	}

	env_set("partitions", parts_set);


	snprintf(buf, PART_ENV_BUF_SIZE, "%u", ubinfo->ubootblks);
	pr_info("set aw-ubi-rawnand.ubootblks to %u\n", ubinfo->ubootblks);
	env_set("aw-ubi-rawnand.ubootblks", buf);

	snprintf(buf, PART_ENV_BUF_SIZE, "%d", get_mtd_num_to_attach());
	pr_info("set ubi_attach_mtdnum to %d\n", get_mtd_num_to_attach());
	env_set("ubi_attach_mtdnum", buf);

	pr_info("update ubi part info ok\n");
	return 0;
}


/*
* ubi->good_peb_count = ubi->peb_count - ubi->bad_peb_count; @ubi_attach
* @ubi_read_volume_table
* ubi->avail_pebs = ubi->good_peb_count - ubi->corr_peb_count
* ubi->avail_pebs -= all vol->reserved_pebs; @init_volumes
* ubi->avail_pebs -= UBI_LAYOUT_VOLUME_EBS; @init_volumes, 2
* ubi->avail_pebs -= WL_RESERVED_PEBS; @ubi_wl_init, 1
* ubi->avail_pebs -= EBA_RESERVED_PEBS; @ubi_eba_init, 1
* ubi->avail_pebs -= ubi->beb_rsvd_pebs; @ubi_eba_init
*/
int rawnand_ubi_user_volumes_size(void)
{
	int pebs = 0;
	int leb_size = 0;
	int vol_pebs = 0;
	int i = 0;
	struct ubi_device *ubi = ubi_devices[0];

	if (ubi) {
		leb_size = ubi->leb_size;
		pebs = ubi->rsvd_pebs - ubi->beb_rsvd_pebs - 2 - 1 - 1;
		/*UBI_INT_VOL_COUNT: 1*/
		for (i = 0; i < (ubi->vol_count - 1); i++) {
			vol_pebs += ubi->volumes[i]->reserved_pebs;
		}
		if (vol_pebs != pebs)
			pr_info("warning vol_pebs: %d, %d\n", vol_pebs, pebs);
	}
	pr_info("logical area info: %d %d last_lba: %d\n", pebs, leb_size,
		pebs * (leb_size >> 9) - 1);
	return leb_size * pebs;
}

int rawnand_mtd_attach_mtd(void)
{
	int num;
	struct ubi_info *ubinfo = get_ubi_info();
	struct ubi_mtd_info *mtd_info = ubi_to_mtd(ubinfo);

	/* support to configure size of mtd part 0 (boot_parts) */
	set_env_mtdparts_rawnand();

	if (create_mtdparts_do())
		return -EINVAL;

	init_mtd_info(mtd_info);
	num = get_mtd_num_to_attach();
	if (num < 0)
		return -EINVAL;

	if (ubi_attach_mtd_do(num))
		return -EINVAL;

	return 0;

}
#endif /*CONFIG_AW_MTD_RAWNAND*/

