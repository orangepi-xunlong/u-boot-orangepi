/*
 * (C) Copyright 2007-2017
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * tuchengmao <tuchengmao@allwinnertech.com>
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
#include <common.h>
#include <command.h>
#include <exports.h>
#include <memalign.h>
#include <mtd.h>
#include <nand.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/err.h>
#include <ubi_uboot.h>
#include <linux/errno.h>
#include <mtd/ubi-user.h>
#include <linux/crc32.h>

#include "../drivers/mtd/ubi/ubi-media.h"
#include "../drivers/mtd/ubi/ubi.h"
#include "ubi_simu.h"

#define INIT_EC_CNT (1)
#define VOL_ALIGN_SIZE (1)

#if 0
#define sim_dbg(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#else
#define sim_dbg(fmt, ...)
#endif

static struct ubi_device sim_ubi;
static struct ubi_device *p_ubi = &sim_ubi;
struct ubi_volume *w_vol = NULL;
static unsigned char g_bbt[2048] = { 0 }; //bad block table
static int vol_idx = 0;
static int cur_pnum_idx = 0;
static unsigned long long seq_num = 0;

static struct ubi_volume *ubi_simu_find_volume(char *volume)
{
	struct ubi_volume *vol = NULL;
	int i = 0;

	for (i = 0; i < p_ubi->vtbl_slots; i++) {
		vol = p_ubi->volumes[i];
		if (vol && !strcmp(vol->name, volume))
			return vol;
	}

	printf("Volume %s not found!\n", volume);
	return NULL;
}

static int get_pnum(void)
{
	while ((cur_pnum_idx < p_ubi->peb_count) && g_bbt[cur_pnum_idx])
		cur_pnum_idx++;
	if (cur_pnum_idx >= p_ubi->peb_count) {
		sim_dbg("no available peb");
		return -1;
	}
	sim_dbg("pnum: %d", cur_pnum_idx);
	return cur_pnum_idx++;
}

static unsigned long long next_seq_num(void)
{
	return seq_num++;
}

static int erase_peb(int pnum)
{
	struct erase_info ei;

	memset(&ei, 0, sizeof(ei));
	ei.mtd  = p_ubi->mtd;
	ei.addr = (loff_t)pnum * p_ubi->peb_size;
	ei.len  = p_ubi->peb_size;
	if (mtd_erase(p_ubi->mtd, &ei)) {
		sim_dbg("erase %d fail", pnum);
		return -1;
	}
	return 0;
}

static void fill_ec_hdr(void)
{
	unsigned int crc = 0;
	struct ubi_ec_hdr *ech = (struct ubi_ec_hdr *)p_ubi->peb_buf;
	memset(ech, 0, p_ubi->ec_hdr_alsize);

	ech->ec = cpu_to_be64(INIT_EC_CNT);
	ech->magic = cpu_to_be32(UBI_EC_HDR_MAGIC);
	ech->version = UBI_VERSION;
	ech->vid_hdr_offset = cpu_to_be32(p_ubi->vid_hdr_offset);
	ech->data_offset = cpu_to_be32(p_ubi->leb_start);
	ech->image_seq = cpu_to_be32(p_ubi->image_seq);
	crc = crc32(UBI_CRC32_INIT, ech, UBI_EC_HDR_SIZE_CRC);
	ech->hdr_crc = cpu_to_be32(crc);
}

static void fill_vid_hdr(int lnum, int vol_type, int vol_id)
{
	unsigned int crc = 0;
	struct ubi_vid_hdr *vch = (struct ubi_vid_hdr *)(p_ubi->peb_buf + p_ubi->vid_hdr_offset);

	memset(vch, 0, p_ubi->vid_hdr_alsize);
	vch->vol_type = vol_type;
	vch->vol_id = cpu_to_be32(vol_id);
	vch->compat = (vol_id == UBI_LAYOUT_VOLUME_ID) ? UBI_LAYOUT_VOLUME_COMPAT : 0;
	vch->data_size = vch->used_ebs = vch->data_pad = cpu_to_be32(0);
	vch->lnum = cpu_to_be32(lnum);
	vch->sqnum = cpu_to_be64(next_seq_num());

	vch->magic = cpu_to_be32(UBI_VID_HDR_MAGIC);
	vch->version = UBI_VERSION;
	crc = crc32(UBI_CRC32_INIT, vch, UBI_VID_HDR_SIZE_CRC);
	vch->hdr_crc = cpu_to_be32(crc);
}

static void show_mtd_info(struct mtd_info *mtd)
{
	pr_err("mtd size: %lld\n", mtd->size);
	pr_err("erasesize: %d\n", mtd->erasesize);
	pr_err("writesize: %d\n", mtd->writesize);
	pr_err("writebufsize: %d\n", mtd->writebufsize);
	pr_err("subpage_sft: %d\n", mtd->subpage_sft);
	pr_err("mtd: %p, %s\n", mtd, mtd->name);
}

static int wr_vol_table(void)
{
	int i = 0;
	int pnum = -1;
	struct ubi_vtbl_record *layout_vol = (struct ubi_vtbl_record *)(p_ubi->peb_buf + p_ubi->leb_start);

	/* filling empty entries */
	sim_dbg("vol_idx: %d", vol_idx);
	for (i = vol_idx; i < p_ubi->vtbl_slots; i++)
		layout_vol[i].crc = cpu_to_be32(0xf116c36b);

	/* write internal layout volume peb */
	for (i = 0; i < UBI_LAYOUT_VOLUME_EBS; i++) {
		pnum = get_pnum();
		erase_peb(pnum);
		fill_ec_hdr();
		fill_vid_hdr(i, UBI_LAYOUT_VOLUME_TYPE, UBI_LAYOUT_VOLUME_ID);
		/* Write the layout volume contents */
		if (ubi_io_write(p_ubi, p_ubi->peb_buf, pnum, 0,
				 p_ubi->leb_start + p_ubi->vtbl_size))
			sim_dbg("wr layout volume %d@%d fail", i,
				UBI_LAYOUT_VOLUME_ID);
	}

#if 0
	/* read and check */
	for (i=0;i<UBI_LAYOUT_VOLUME_EBS;i++) {
		unsigned char g_vid_hdr[p_ubi->vid_hdr_alsize] = {0};
		int j = 0;
		if (ubi_io_read_vid_hdr(p_ubi, i, g_vid_hdr, 1))
			sim_dbg("rd %d@%d fail", g_vid_hdr->lnum, g_vid_hdr->vol_id);
		//ubi_dump_vid_hdr(g_vid_hdr);
		/* read the layout volume contents */
		memset(layout_vol, 0, p_ubi->vtbl_size);
		if (ubi_io_read_data(p_ubi, layout_vol, pnum, 0, p_ubi->vtbl_size))
			sim_dbg("rd layout volume %d fail", g_vid_hdr->lnum);
		for(j=0;j<p_ubi->vtbl_slots;j++) {
			if (0xf116c36b == be32_to_cpu(layout_vol[j].crc))
				continue;
			else
				; //ubi_dump_vtbl_record(layout_vol + j, j);
		}
	}
#endif
	return 0;
}

int ubi_simu_create_vol(char *volume, int64_t size, int dynamic, int vol_id)
{
	int i = 0;
	unsigned int crc = 0;
	struct ubi_volume *vol = NULL;
	struct ubi_vtbl_record *layout_vol =
		(struct ubi_vtbl_record *)(p_ubi->peb_buf + p_ubi->leb_start);

	sim_dbg("%s %s %lld %d %d, vol_idx: %d\n", __func__, volume, size, dynamic, vol_id, vol_idx);

	vol = malloc(sizeof(struct ubi_volume));
	if (!vol) {
		sim_dbg("malloc %d fail", vol_idx);
		goto fail_exit;
	}
	memset(vol, 0, sizeof(*vol));

	/* Calculate how many eraseblocks are requested */
	vol->usable_leb_size = p_ubi->leb_size - p_ubi->leb_size % VOL_ALIGN_SIZE;
	vol->reserved_pebs = div_u64(size + vol->usable_leb_size - 1, vol->usable_leb_size);
	/* Reserve physical eraseblocks */
	if (vol->reserved_pebs > p_ubi->avail_pebs) {
		sim_dbg("not enough PEBs, only %d available", p_ubi->avail_pebs);
		goto fail_exit;
	}
	p_ubi->avail_pebs -= vol->reserved_pebs;
	p_ubi->rsvd_pebs += vol->reserved_pebs;
	vol->vol_id = vol_idx;
	vol->alignment = VOL_ALIGN_SIZE;
	vol->data_pad = p_ubi->leb_size % vol->alignment;
	vol->vol_type = dynamic ? UBI_DYNAMIC_VOLUME : UBI_STATIC_VOLUME;
	vol->name_len = strlen(volume);
	memcpy(vol->name, volume, vol->name_len);
	vol->ubi = p_ubi;

	vol->eba_tbl = malloc(vol->reserved_pebs * sizeof(int));
	if (!vol->eba_tbl) {
		sim_dbg("alloc eba_tbl fail");
		goto fail_exit;
	}
	for (i = 0; i < vol->reserved_pebs; i++)
		vol->eba_tbl[i] = UBI_LEB_UNMAPPED;

	/* filling ubi_vtbl_record for each volume */
	memset(&layout_vol[vol_idx], 0, UBI_VTBL_RECORD_SIZE);
	layout_vol[vol_idx].reserved_pebs = cpu_to_be32(vol->reserved_pebs);
	layout_vol[vol_idx].alignment = cpu_to_be32(vol->alignment);
	layout_vol[vol_idx].data_pad = cpu_to_be32(vol->data_pad);
	layout_vol[vol_idx].name_len = cpu_to_be16(vol->name_len);
	layout_vol[vol_idx].vol_type = dynamic ? UBI_VID_DYNAMIC : UBI_VID_STATIC;
	memcpy(layout_vol[vol_idx].name, vol->name, vol->name_len);

	/*
	*   add UBI_VTBL_AUTORESIZE_FLG for the last UDISK volume
	*   in case that we calculate size by mistake.
	*/
	if (0 == strncmp(volume, "UDISK", 5))
		layout_vol[vol_idx].flags = UBI_VTBL_AUTORESIZE_FLG;
	crc = crc32(UBI_CRC32_INIT, layout_vol + vol_idx, UBI_VTBL_RECORD_SIZE_CRC);
	layout_vol[vol_idx].crc = cpu_to_be32(crc);
	p_ubi->volumes[vol_idx] = vol;
	p_ubi->vol_count++;
	vol_idx++;

	if (0 == strncmp(volume, "UDISK", 5)) {
		sim_dbg("it is time to write layout volume table");
		wr_vol_table();
	}
	return 0;
fail_exit:
	if (vol->eba_tbl)
		free(vol->eba_tbl);
	if (vol)
		free(vol);
	return -1;
}

static int wr_peb(struct ubi_volume *vol, int lnum, void *buf, int len)
{
	int pnum = 0;

	if (len != vol->usable_leb_size)
		pr_info("%s lnum: %d, len: %d\n", vol->name, lnum, len);

	if (vol->vol_type == UBI_DYNAMIC_VOLUME) {
		/* For dynamic volume, this function checks if the data contains 0xFF bytes
		 * at the end. If yes, the 0xFF bytes are cut and not written. So if the whole
		 * buffer contains only 0xFF bytes, the LEB is left unmapped.
		 * The reason why we skip the trailing 0xFF bytes in case of dynamic volume is
		 * that we want to make sure that more data may be appended to the logical
		 * eraseblock in future. Indeed, writing 0xFF bytes may have side effects and
		 * this PEB won't be writable anymore. So if one writes the file-system image
		 * to the UBI volume where 0xFFs mean free space - UBI makes sure this free
		 * space is writable after the update.
		 */
		int l = ALIGN(len, p_ubi->min_io_size);

		memset(buf + len, 0xFF, l - len);
		len = ubi_calc_data_len(p_ubi, buf, l);
		if (len == 0) {
			pr_warn("all %d bytes contain 0xFF - skip\n", len);
			return 0;
		}
	} else {
		/*
		 * When writing static volume, and this is the last logical
		 * eraseblock, the length (@len) does not have to be aligned to
		 * the minimal flash I/O unit. The 'ubi_eba_write_leb_st()'
		 * function accepts exact (unaligned) length and stores it in
		 * the VID header. And it takes care of proper alignment by
		 * padding the buffer. Here we just make sure the padding will
		 * contain zeros, not random trash.
		 */
		memset(buf + len, 0, vol->usable_leb_size - len);
	}

	pnum = get_pnum();

	if (pnum == -1) {
		pr_err("%s fail\n", __func__);
		return -1;
	}
	vol->eba_tbl[lnum] = pnum;
	erase_peb(pnum);
	fill_ec_hdr();
	fill_vid_hdr(lnum, (vol->vol_type == UBI_DYNAMIC_VOLUME) ?
			UBI_VID_DYNAMIC : UBI_VID_STATIC, vol->vol_id);

	if (vol->vol_type == UBI_STATIC_VOLUME) {
		struct ubi_vid_hdr *vch = (struct ubi_vid_hdr *)(p_ubi->peb_buf + p_ubi->vid_hdr_offset);

		vch->data_size = cpu_to_be32(len);
		vch->used_ebs = cpu_to_be32(vol->upd_ebs);
		vch->data_crc = cpu_to_be32(crc32(UBI_CRC32_INIT, buf, len));

		/* re-calc vid hdr crc */
		vch->hdr_crc = cpu_to_be32(crc32(UBI_CRC32_INIT, vch, UBI_VID_HDR_SIZE_CRC));

		sim_dbg("size: %d, ebs: %d, data_crc: %x\n", len, vol->upd_ebs, crc32(UBI_CRC32_INIT, buf, len));

		if (lnum == vol->upd_ebs - 1)
			/* If this is the last LEB @len may be unaligned */
			len = ALIGN(len, p_ubi->min_io_size);
		else
			ubi_assert(!(len & (p_ubi->min_io_size - 1)));
	}

	/* Write ec_hdr, vid_hdr and data */
	if (ubi_io_write(p_ubi, p_ubi->peb_buf, pnum, 0, p_ubi->leb_start + len)) {
		pr_err("wr %d@%d fail", lnum, vol->vol_id);
		return -1;
	}
	return 0;
}

static int wr_vol(struct ubi_volume *vol, void *buf, size_t count)
{
	int lnum, len;
	u32 offs = 0;

	//sim_dbg("write %d of %lld bytes, %lld already passed", count, vol->upd_bytes, vol->upd_received);
	lnum = div_u64_rem(vol->upd_received, vol->usable_leb_size, &offs);
	if (vol->upd_received + count > vol->upd_bytes)
		count = vol->upd_bytes - vol->upd_received;

	/*
	 * When updating volumes, we accumulate whole logical eraseblock of
	 * data and write it at once.
	 */
	if (offs != 0) {
		/*
		 * This is a write to the middle of the logical eraseblock. We
		 * copy the data to our update buffer and wait for more data or
		 * flush it if the whole eraseblock is written or the update
		 * is finished.
		 */
		len = vol->usable_leb_size - offs;
		if (len > count)
			len = count;

		memcpy(vol->upd_buf + offs, buf, len);

		if (offs + len == vol->usable_leb_size ||
		    vol->upd_received + len == vol->upd_bytes)
			wr_peb(vol, lnum, vol->upd_buf, offs + len);
		vol->upd_received += len;
		count -= len;
		buf += len;
		lnum += 1;
	}

	/*
	 * If we've got more to write, let's continue. At this point we know we
	 * are starting from the beginning of an eraseblock.
	 */
	while (count) {
		if (count > vol->usable_leb_size)
			len = vol->usable_leb_size;
		else
			len = count;

		memcpy(vol->upd_buf, buf, len);
		if (len == vol->usable_leb_size || vol->upd_received + len == vol->upd_bytes)
			wr_peb(vol, lnum, vol->upd_buf, len);

		vol->upd_received += len;
		count -= len;
		lnum += 1;
		buf += len;
	}

	if (vol->upd_received == vol->upd_bytes)
		sim_dbg("********wr over for %s**********", vol->name);
	return 0;
}

int ubi_simu_volume_continue_write(char *volume, void *buf, size_t size)
{
	struct ubi_volume *vol = ubi_simu_find_volume(volume);

	sim_dbg("%s %s %p %d\n", __func__, volume, buf, size);
	if (!vol)
		return -1;
	if (vol != w_vol) {
		sim_dbg("warning diff %s %s", volume, w_vol->name);
		while (1)
			;
	}
	return wr_vol(vol, buf, size);
}

int ubi_simu_volume_begin_write(char *volume, void *buf, size_t size,
				size_t full_size)
{
	struct ubi_volume *vol = NULL;

	sim_dbg("%s %s %p %d %d\n", __func__, volume, buf, size, full_size);
	vol = ubi_simu_find_volume(volume);

	if (!vol)
		return -1;

	if (vol != w_vol) {
		sim_dbg("last wr status: %lld %lld", vol->upd_received, vol->upd_bytes);
		/* flush last vol */
		if (vol->upd_received != vol->upd_bytes)
			sim_dbg("********************should flush first*****************\n");
	}
	w_vol = vol;
	vol->upd_bytes = full_size;
	vol->upd_ebs = div_u64(full_size + vol->usable_leb_size - 1, vol->usable_leb_size);
	vol->used_bytes = vol->upd_ebs * vol->usable_leb_size;
	vol->upd_received = 0;
	vol->upd_buf = p_ubi->peb_buf + p_ubi->leb_start;
	sim_dbg("%s upd_bytes:%lld, used_bytes:%lld", __func__, vol->upd_bytes, vol->used_bytes);
	return wr_vol(vol, buf, size);
}

int ubi_simu_volume_write(char *volume, void *buf, size_t size)
{
	sim_dbg("%s %s %p %d\n", __func__, volume, buf, size);
	return ubi_simu_volume_begin_write(volume, buf, size, size);
}

/*
* copy from get_bad_peb_limit
*/
static int calc_bad_peb_limit(const struct ubi_device *ubi, int max_beb_per1024)
{
	int limit, device_pebs;
	uint64_t device_size;

	if (!max_beb_per1024)
		return 0;

	/*
	 * Here we are using size of the entire flash chip and
	 * not just the MTD partition size because the maximum
	 * number of bad eraseblocks is a percentage of the
	 * whole device and bad eraseblocks are not fairly
	 * distributed over the flash chip. So the worst case
	 * is that all the bad eraseblocks of the chip are in
	 * the MTD partition we are attaching (ubi->mtd).
	 */
	device_size = mtd_get_device_size(ubi->mtd);
	device_pebs = mtd_div_by_eb(device_size, ubi->mtd);
	limit = mult_frac(device_pebs, max_beb_per1024, 1024);

	/* Round it up */
	if (mult_frac(limit, 1024, max_beb_per1024) < device_pebs)
		limit += 1;
	return limit;
}

struct ubi_device *ubi_simu_part(char *part_name, const char *vid_header_offset)
{
	struct mtd_info *mtd = NULL;
	int i		     = 0;

	pr_err("part_name: %s, vid_hdr_offset: %s, sizeof(void): %d\n", part_name, vid_header_offset, sizeof(void));
	mtd_probe_devices();
	mtd = get_mtd_device_nm(part_name);
	if (IS_ERR(mtd)) {
		printf("Partition %s not found!\n", part_name);
		return NULL;
	}
	put_mtd_device(mtd);
	show_mtd_info(mtd);

	memset(p_ubi, 0, sizeof(struct ubi_device));
	p_ubi->ubi_num = 0;
	strncpy(p_ubi->ubi_name, "ubi0", 4);
	p_ubi->mtd = mtd;

	/* duplicated from io_init */
	p_ubi->peb_size = p_ubi->mtd->erasesize;
	p_ubi->peb_count = mtd_div_by_eb(p_ubi->mtd->size, p_ubi->mtd);
	p_ubi->flash_size = p_ubi->mtd->size;

	p_ubi->bad_allowed = 1;
	p_ubi->bad_peb_limit = calc_bad_peb_limit(p_ubi, CONFIG_MTD_UBI_BEB_LIMIT);
	p_ubi->min_io_size = p_ubi->mtd->writesize;
	p_ubi->hdrs_min_io_size = p_ubi->mtd->writesize >> p_ubi->mtd->subpage_sft;
	p_ubi->max_write_size = p_ubi->mtd->writebufsize;

	/* Calculate default aligned sizes of EC and VID headers */
	p_ubi->ec_hdr_alsize = ALIGN(UBI_EC_HDR_SIZE, p_ubi->hdrs_min_io_size);
	p_ubi->vid_hdr_alsize = ALIGN(UBI_VID_HDR_SIZE, p_ubi->hdrs_min_io_size);
	p_ubi->vid_hdr_offset = p_ubi->vid_hdr_aloffset = p_ubi->ec_hdr_alsize;
	p_ubi->leb_start = p_ubi->vid_hdr_offset + UBI_VID_HDR_SIZE;
	p_ubi->leb_start = ALIGN(p_ubi->leb_start, p_ubi->min_io_size);
	p_ubi->leb_size = p_ubi->peb_size - p_ubi->leb_start;
	p_ubi->image_seq = 0;
	p_ubi->bad_allowed = 1;

	p_ubi->peb_buf = malloc(p_ubi->peb_size);
	if (!p_ubi->peb_buf) {
		sim_dbg("malloc peb_buf fail");
		return NULL;
	}
	memset(p_ubi->peb_buf, 0, p_ubi->peb_size);
	for (i = 0; i < p_ubi->peb_count; i++) {
		if (ubi_io_is_bad(p_ubi, i)) {
			g_bbt[i] = 1;
			sim_dbg("peb %d is bad block", i);
			p_ubi->bad_peb_count++;
			continue;
		}
	}
	p_ubi->avail_pebs = p_ubi->peb_count - p_ubi->bad_peb_count;
	p_ubi->beb_rsvd_level = p_ubi->bad_peb_limit - p_ubi->bad_peb_count;
	p_ubi->beb_rsvd_pebs = p_ubi->beb_rsvd_level;
	pr_err("beb_pebs: %d, beb_level: %d, limit: %d\n",
		p_ubi->beb_rsvd_pebs, p_ubi->beb_rsvd_level,
		p_ubi->bad_peb_limit);

	/*
	 * The number of supported volumes is limited by the eraseblock size
	 * and by the UBI_MAX_VOLUMES constant.
	 */
	p_ubi->vtbl_slots = p_ubi->leb_size / UBI_VTBL_RECORD_SIZE;
	if (p_ubi->vtbl_slots > UBI_MAX_VOLUMES)
		p_ubi->vtbl_slots = UBI_MAX_VOLUMES;

	p_ubi->vtbl_size = p_ubi->vtbl_slots * UBI_VTBL_RECORD_SIZE;
	pr_err("UBI_VTBL_RECORD_SIZE: %d, size: %d\n", UBI_VTBL_RECORD_SIZE,
		p_ubi->vtbl_size);
	p_ubi->vtbl_size = ALIGN(p_ubi->vtbl_size, p_ubi->min_io_size);
	pr_err("UBI_VTBL_RECORD_SIZE: %d, size: %d\n", UBI_VTBL_RECORD_SIZE,
		p_ubi->vtbl_size);
	/* 4: reserved 2 layout volumes, 1 wl and 1 atomic change */
	p_ubi->rsvd_pebs += p_ubi->beb_rsvd_pebs + 4;
	p_ubi->avail_pebs -= p_ubi->rsvd_pebs;

	ubi_devices[0] = p_ubi;
	pr_err("cnt: %d, bad_cnt:%d, avail: %d, rsvd: %d", p_ubi->peb_count,
		p_ubi->bad_peb_count, p_ubi->avail_pebs, p_ubi->rsvd_pebs);
	return p_ubi;
}

int sunxi_ubi_simu_volume_read(char *volume, loff_t offp, char *buf,
			       size_t size)
{
	int err = 0, lnum, off, len, tbuf_size;
	void *tbuf;
	unsigned long long tmp;
	struct ubi_volume *vol;

	sim_dbg("%s %s %lld %p %d\n", __func__, volume, offp, buf, size);
	vol = ubi_simu_find_volume(volume);
	if (vol == NULL)
		return -ENODEV;
	if (offp == vol->used_bytes)
		return 0;

	if (size == 0) {
		printf("simu No size specified -> Using max size (%lld)\n", vol->used_bytes);
		size = vol->used_bytes;
	}

	if (offp + size > vol->used_bytes)
		size = vol->used_bytes - offp;

	tbuf_size = vol->usable_leb_size;
	if (size < tbuf_size)
		tbuf_size = ALIGN(size, p_ubi->min_io_size);
	tbuf = malloc(tbuf_size);
	if (!tbuf) {
		printf("NO MEM\n");
		return -ENOMEM;
	}
	len = size > tbuf_size ? tbuf_size : size;

	tmp = offp;
	off = do_div(tmp, vol->usable_leb_size);
	lnum = tmp;
	do {
		if (off + len >= vol->usable_leb_size)
			len = vol->usable_leb_size - off;
		sim_dbg("%d -> %d, off: %d, %d", lnum, vol->eba_tbl[lnum], off, len);
		if (UBI_LEB_UNMAPPED == vol->eba_tbl[lnum])
			memset(tbuf, 0xff, len);
		else
			err = ubi_io_read_data(p_ubi, tbuf, vol->eba_tbl[lnum], off, len);
		off += len;
		if (off == vol->usable_leb_size) {
			lnum += 1;
			off -= vol->usable_leb_size;
		}

		size -= len;
		offp += len;

		memcpy(buf, tbuf, len);

		buf += len;
		len = size > tbuf_size ? tbuf_size : size;
	} while (size);

	free(tbuf);
	return err;
}
