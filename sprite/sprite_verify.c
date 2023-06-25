/*
 *  * Copyright 2000-2009
 *   * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *    *
 *     * SPDX-License-Identifier:	GPL-2.0+
 *     */
#include <config.h>
#include <common.h>
#include <malloc.h>
#include <sunxi_mbr.h>
#include <sunxi_board.h>
#include <sunxi_flash.h>
#include "sparse/sparse.h"
#ifdef CONFIG_SUNXI_CE_DRIVER
#include <asm/arch/ce.h>
#endif

#if defined(CONFIG_SUNXI_SPINOR)
#define VERIFY_ONCE_BYTES (2 * 1024 * 1024)
#else
#define VERIFY_ONCE_BYTES (8 * 1024 * 1024)
#endif

#define VERIFY_ONCE_SECTORS (VERIFY_ONCE_BYTES / 512)

uint add_sum(void *buffer, uint length)
{
	unsigned int *buf;
	unsigned int count;
	unsigned int sum;

	count = length >> 2;
	sum   = 0;
	buf   = (unsigned int *)buffer;
	while (count--) {
		sum += *buf++;
	};

	switch (length & 0x03) {
	case 0:
		return sum;
	case 1:
		sum += (*buf & 0x000000ff);
		break;
	case 2:
		sum += (*buf & 0x0000ffff);
		break;
	case 3:
		sum += (*buf & 0x00ffffff);
		break;
	}
	return sum;
}

uint sunxi_sprite_part_rawdata_verify(uint base_start, long long base_bytes)
{
	uint checksum = 0;
	uint unaligned_bytes, last_time_bytes;
	uint rest_sectors;
	uint crt_start;
	char *tmp_buf = NULL;

	tmp_buf = (char *)memalign(64, VERIFY_ONCE_BYTES);
	if (!tmp_buf) {
		printf("sunxi sprite err: unable to malloc memory for verify\n");

		return 0;
	}
	crt_start       = base_start;
	rest_sectors    = (uint)((base_bytes + 511) >> 9);
	unaligned_bytes = (uint)base_bytes & 0x1ff;

	debug("read total sectors %d\n", rest_sectors);
	debug("read part start %d\n", crt_start);
	while (rest_sectors >= VERIFY_ONCE_SECTORS) {
		if (sunxi_sprite_read(crt_start, VERIFY_ONCE_SECTORS,
				      tmp_buf) != VERIFY_ONCE_SECTORS) {
			printf("sunxi sprite: read flash error when verify\n");
			checksum = 0;

			goto __rawdata_verify_err;
		}
		crt_start += VERIFY_ONCE_SECTORS;
		rest_sectors -= VERIFY_ONCE_SECTORS;
		checksum += add_sum(tmp_buf, VERIFY_ONCE_BYTES);
		debug("check sum = 0x%x\n", checksum);
	}
	if (rest_sectors) {
		if (sunxi_sprite_read(crt_start, rest_sectors, tmp_buf) !=
		    rest_sectors) {
			printf("sunxi sprite: read flash error when verify\n");
			checksum = 0;

			goto __rawdata_verify_err;
		}
		if (unaligned_bytes) {
			last_time_bytes =
				(rest_sectors - 1) * 512 + unaligned_bytes;
		} else {
			last_time_bytes = rest_sectors * 512;
		}
		checksum += add_sum(tmp_buf, last_time_bytes);
		debug("check sum = 0x%x\n", checksum);
	}

__rawdata_verify_err:
	if (tmp_buf) {
		free(tmp_buf);
	}

	return checksum;
}

uint sunxi_sprite_part_sparsedata_verify(void)
{
	return unsparse_checksum();
}

uint sunxi_sprite_generate_checksum(void *buffer, uint length, uint src_sum)
{
	return sunxi_generate_checksum(buffer, length, 1, src_sum);
}

int sunxi_sprite_verify_checksum(void *buffer, uint length, uint src_sum)
{
	return sunxi_verify_checksum(buffer, length, src_sum);
}

static void __mbr_map_dump(u8 *buf)
{
	sunxi_mbr_t *mbr_info = (sunxi_mbr_t *)buf;
	sunxi_partition *part_info;
	u32 i;
	char buffer[32];

	printf("*************MBR DUMP***************\n");
	printf("total mbr part %d\n", mbr_info->PartCount);
	printf("\n");
	for (part_info = mbr_info->array, i = 0; i < mbr_info->PartCount;
	     i++, part_info++) {
		memset(buffer, 0, 32);
		memcpy(buffer, part_info->name, 16);
		printf("part[%d] name      :%s\n", i, buffer);
		memset(buffer, 0, 32);
		memcpy(buffer, part_info->classname, 16);
		printf("part[%d] classname :%s\n", i, buffer);
		printf("part[%d] addrlo    :0x%x\n", i, part_info->addrlo);
		printf("part[%d] lenlo     :0x%x\n", i, part_info->lenlo);
		printf("part[%d] user_type :%d\n", i, part_info->user_type);
		printf("part[%d] keydata   :%d\n", i, part_info->keydata);
		printf("part[%d] ro        :%d\n", i, part_info->ro);
		printf("\n");
	}
}

static int gpt_show_partition_info(char *buf)
{
	int i, j;

	char char8_name[PARTNAME_SZ] = { 0 };
	gpt_header *gpt_head	 = (gpt_header *)(buf + GPT_HEAD_OFFSET);
	gpt_entry *entry	     = (gpt_entry *)(buf + GPT_ENTRY_OFFSET);

	for (i = 0; i < gpt_head->num_partition_entries; i++) {
		for (j = 0; j < PARTNAME_SZ; j++) {
			char8_name[j] = (char)(entry[i].partition_name[j]);
		}
		printf("GPT:%-12s: %-12llx  %-12llx\n", char8_name,
		       entry[i].starting_lba, entry[i].ending_lba);
	}

	return 0;
}

int sunxi_sprite_read_mbr(void *buffer, uint mbr_copy)
{
	uint sectors;

	sectors = 1 * SUNXI_MBR_SIZE / 512;
	if (sectors != sunxi_sprite_read(0, sectors, buffer))
		return -1;

	return 0;
}

int sunxi_sprite_verify_mbr(void *buffer)
{
	sunxi_mbr_t *local_mbr;
	char *tmp_buf = (char *)buffer;
	int i;
	int mbr_num = SUNXI_MBR_COPY_NUM;
	tmp_buf     = buffer;

	gpt_header *gpt_head = (gpt_header *)(buffer + GPT_HEAD_OFFSET);
	/* check GPT first*/
	if (gpt_head->signature == GPT_HEADER_SIGNATURE) {
		u32 calc_crc32   = 0;
		u32 backup_crc32 = 0;

		backup_crc32	   = gpt_head->header_crc32;
		gpt_head->header_crc32 = 0;
		calc_crc32 = crc32(0, (const unsigned char *)gpt_head,
				   sizeof(gpt_header));
		gpt_head->header_crc32 = backup_crc32;
		if (calc_crc32 != backup_crc32) {
			printf("the GPT table is bad\n");
			return -1;
		}
		gpt_show_partition_info(buffer);
		return 0;
	}

	/* check mbr */
	if (get_boot_storage_type() == STORAGE_NOR) {
		mbr_num = 1;
	}

	for (i = 0; i < mbr_num; i++) {
		local_mbr = (sunxi_mbr_t *)tmp_buf;
		if (crc32(0, (const unsigned char *)(tmp_buf + 4),
			  SUNXI_MBR_SIZE - 4) != local_mbr->crc32) {
			printf("the %d mbr table is bad\n", i);

			return -1;
		} else {
			printf("the %d mbr table is ok\n", i);
			tmp_buf += SUNXI_MBR_SIZE;
		}
	}
#if 1
	__mbr_map_dump(buffer);
#endif

	return 0;
}

int sunxi_sprite_verify_dlmap(void *buffer)
{
	sunxi_download_info *local_dlmap;
	char *tmp_buf = (char *)buffer;

	tmp_buf     = buffer;
	local_dlmap = (sunxi_download_info *)tmp_buf;
	if (crc32(0, (const unsigned char *)(tmp_buf + 4),
		  SUNXI_MBR_SIZE - 4) != local_dlmap->crc32) {
		printf("downlaod map is bad\n");

		return -1;
	}

	return 0;
}

#ifdef CONFIG_SUNXI_DIGEST_TEST
int do_sunxi_digest_test(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	__maybe_unused int i;
	if (argc < 3) {
		goto usage;
	}
	int end_time, start_time = get_timer_masked();
	uint crc32_val = crc32(0, (const unsigned char *)simple_strtoul(argv[1], NULL, 16), simple_strtoul(argv[2], NULL, 16));
	end_time = get_timer_masked();
	tick_printf("crc32:0x%x\ttime=%dms \n", crc32_val, end_time-start_time);
	start_time = get_timer_masked();
	int checksum = sunxi_generate_checksum((void *)simple_strtoul(argv[1], NULL, 16),
			simple_strtoul(argv[2], NULL, 16), (argc < 4 ? 1 : simple_strtoul(argv[3], NULL, 16)), STAMP_VALUE);
	end_time = get_timer_masked();
	tick_printf("div:%d checksum:0x%x\ttime=%dms \n", (argc < 4 ? 1 : simple_strtoul(argv[3], NULL, 16)), checksum, end_time-start_time);
#ifdef CONFIG_SUNXI_CE_DRIVER
	start_time = get_timer_masked();
	u8 hash_of_file[32] = {0};
	sunxi_ss_open();
	if (sunxi_sha_calc(hash_of_file, 32, (unsigned char *)simple_strtoul(argv[1], NULL, 16), simple_strtoul(argv[2], NULL, 16))) {
		goto usage;
	}
	end_time = get_timer_masked();
	tick_printf("sha256:\ttime=%dms \n", end_time-start_time);
	for (i = 0; i < 32; i++)
		printf("%02x", hash_of_file[i]);
	printf("\n");
#endif
#ifdef CONFIG_CMD_SHA1SUM
	char temp_buf[64] = {0};
	start_time = get_timer_masked();
	sprintf((char *)temp_buf, "sha1sum 0x%lx 0x%lx", simple_strtoul(argv[1], NULL, 16), simple_strtoul(argv[2], NULL, 16));
	run_command(temp_buf, 0);
	end_time = get_timer_masked();
	tick_printf("sha1:\ttime=%dms \n", end_time-start_time);
#endif
	return 0;
usage:
	return cmd_usage(cmdtp);
}

U_BOOT_CMD(sunxi_digest_test, 6, 1, do_sunxi_digest_test, "sunxi_digest_test sub-system",
	   "sunxi_digest_test <mem_addr> <size> [div]\n");
#endif
