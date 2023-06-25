/*
 * (C) Copyright 2019
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * cuizhikui <cuizhikui@allwinnertech.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */
#include <common.h>
#include <config.h>
#include <command.h>
#include <sunxi_board.h>
#include <malloc.h>
#include <memalign.h>
#include <sunxi_flash.h>
#include <part.h>
#include <private_boot0.h>

extern __s32 check_sum(__u32 *mem_base, __u32 size);
#define STRING(x) #x
#define readline(x) cli_readline(x)

#define RR_TAB_BLOCK_START (0)
#define RR_TAB_PAGE_START (0)
#define RR_TAB_PAGE_END (4)
#define PAGE_TAB_PAGE_START (4)
#define PAGE_TAB_PAGE_END (8)

#define DUMP_NAND_ONFO                                                         \
	"\033[0;36m<obtain nand chip inof>: [parm1:nand_info]\033[0m\n"
#define DUMP_NAND_INFO_HELP                                                    \
	"  For example:want to obtain nand chip info,usage:sunxi_nand_test nand_info\n"

#define DUMP_NAND_PAGES                                                        \
	"\033[0;36m<dump continuous phy pages>: [parm1:dump_phy_pages] [parm2:chip] [parm3:block] [parm4:start_page] [parm5:end_page] [parm6:pmem] [parm7:printf_flag]\033[0m\n"
#define DUMP_NAND_PAGES_HELP                                                   \
	"  For example:want to dump chip 0 block 1 's page 2 to page 5 data to 0x45000000 and don't display in terminal,usage:sunxi_nand_test dump_phy_pages 0 1 2 5 0x45000000 no\n"

#define DUMP_BLOCKS                                                            \
	"\033[0;36m<dump continuous blocks>: [parm1:dump_phy_blocks] [parm2:chip num] [parm3:start block num] [parm4:end block num] [parm5:pmem] [parm6:printf_flag]\033[0m\n"
#define DUMP_BLOCKS_HELP                                                       \
	"  For example:want to dump chip 0 block 1 to block 5 data to 0x45000000 and don't display in terminal usage:sunxi_nand_test dump_phy_blocks 0 1 5 0x45000000 no\n"

#define DUMP_LOGIC_DATA                                                        \
	"\033[0;36m<dump logic data>: [parm1:dump_logic_data] [parm2:start_sector] [parm3:end_sector] [parm4:pmem] [parm5:printf_flag]\033[0m\n"
#define DUMP_LOGIC_DATA_HELP                                                   \
	"  For example:want to dump sector 2 to 4 data to 0x45000000 and don't display in terminal,usage:sunxi_nand_test dump_logic_data 2 4 0x45000000 no\n"

#define WRITE_NAND_PAGES                                                        \
	"\033[0;36m<write continuous phy pages>: [parm1:write_phy_pages] [parm2:chip] [parm3:block] [parm4:start_page] [parm5:end_page] [parm6:pmem] [parm7:printf_flag]\033[0m\n"
#define WRITE_NAND_PAGES_HELP                                                   \
	"  For example:want to write data in 0x45000000 to chip 0 block 1 's page 2 to page 5 and don't display in terminal,usage:sunxi_nand_test write_phy_pages 0 1 2 5 0x45000000 no\n"

#define WRITE_BLOCKS                                                            \
	"\033[0;36m<write continuous blocks>: [parm1:write_phy_blocks] [parm2:chip num] [parm3:start block num] [parm4:end block num] [parm5:pmem] [parm6:printf_flag]\033[0m\n"
#define WRITE_BLOCKS_HELP                                                       \
	"  For example:want to write data in 0x45000000 to chip 0 block 1 to block 5 and don't display in terminal usage:sunxi_nand_test write_phy_blocks 0 1 5 0x45000000 no\n"

#define ERASE_BLOCKS                                                            \
	"\033[0;36m<erase continuous blocks>: [parm1:erase_phy_blocks] [parm2:chip num] [parm3:start block num] [parm4:end block num] [parm5:printf_flag]\033[0m\n"
#define ERASE_BLOCKS_HELP                                                       \
	"  For example:want to erase chip 0 block 1 to block 5 and don't display in terminal usage:sunxi_nand_test erase_phy_blocks 0 1 5 no\n"

#define DUMP_HISTORY_DATA                                                      \
	"\033[0;36m<dump history data>: [parm1:dump_history_data] [parm2:start_sector] [parm3:sector num]\033[0m\n"
#define DUMP_HISTORY_DATA_HELP                                                 \
	"  For example:want to dump sector 200' history data,usage:sunxi_nand_test dump_history_data 200 100\n"

#define DUMP_READ_RETRY_TABLE                                                  \
	"\033[0;36m<dump read retry table>: [parm1:dump_read_retry_table]\033[0m\n"
#define DUMP_READ_RETRY_TABLE_HELP                                             \
	"  For example:want to dump read retry table,usage:sunxi_nand_test dump_read_retry_table\n"

#define DUMP_PAGE_TABLE                                                        \
	"\033[0;36m<dump page table>: [parm1:dump_page_table]\033[0m\n"
#define DUMP_PAGE_TABLE_HELP                                                   \
	"  For example:want to dump page table,usage:sunxi_nand_test dump_page_table\n"

#define DUMP_BOOT0 "\033[0;36m<dump boot0>: [parm1:dump_boot0]\033[0m\n"
#define DUMP_BOOT0_HELP                                                        \
	"  For example:want to dump boot0 to 0x45000000 and don't display in terminal,usage:sunxi_nand_test dump_boot0 0x45000000 no\n"

#define DUMP_BAD_TABLE                                                         \
	"\033[0;36m<dump bad table>: [parm1:dump_bad_table]\033[0m\n"
#define DUMP_BAD_TABLE_HELP                                                    \
	"  For example:want to dump bad block table,usage:sunxi_nand_test dump_bad_table\n"

#define DUMP_READ_WRITE_PERFORMANCE                                            \
	"\033[0;36m<read write performance>: [parm1:performance]\033[0m\n"
#define DUMP_READ_WRITE_PERFORMANCE_HELP                                       \
	"  For example:want to test performance,usage:sunxi_nand_test performance\n"

#define CHECK_READ_WRITE_FUNCTION                                              \
	"\033[0;36m<check read-write function>: [parm1:check read-write]\033[0m\n"
#define CHECK_READ_WRITE_FUNCTION_HELP                                         \
	"  For example:want to check read-write function is normal,usage:sunxi_nand_test read-write\n"
extern int sunxi_flash_get_boot0_size(void);

struct sunxi_nand_info {
	unsigned char id[8];
	int chip_cnt;
	int die_cnt_per_chip;
	int blks_per_die;
	unsigned int page_cnt_per_blk;
	unsigned int sects_per_page;
	unsigned int multi_program_flag;
	int super_chip_cnt;
	unsigned int multi_plane_flag;
	unsigned int support_v_interleave;
	unsigned int support_dual_channel;
	unsigned int blk_cnt_per_super_chip;
	unsigned int sector_cnt_per_super_page;
	unsigned int page_cnt_per_super_blk;
	unsigned int page_offset_for_next_super_blk;
	unsigned int spare_bytes;
	unsigned int multi_plane_block_offset;
};

static void dumphex(u8 *mem, unsigned int len)
{
	unsigned int i = 0;

	for (i = 0; i < len; i += 16) {
		if (i % 16 == 0)
			printf("%08x:", i);
		printf("%02x %02x %02x %02x %02x %02x %02x %02x "
				"%02x %02x %02x %02x %02x %02x %02x %02x\n",
				mem[i + 0], mem[i + 1], mem[i + 2], mem[i + 3],
				mem[i + 4], mem[i + 5], mem[i + 6], mem[i + 7],
				mem[i + 8], mem[i + 9], mem[i + 10], mem[i + 11],
				mem[i + 12], mem[i + 13], mem[i + 14], mem[i + 15]);
	}
	printf("\n");
}

static void dumphex_s_e(u8 *mem, unsigned int start, unsigned int end)
{
	unsigned int i = 0;

	for (i = start; i < end; i++) {
		if (i % 16 == 0)
			printf("%08x:", i);
		printf("%02x ", mem[i]);

		if (i % 16 == 15)
			printf("\n");
	}
	printf("\n");
}

static void display_page_table(unsigned int *buf)
{
	boot_file_head_t *bfn	 = (boot_file_head_t *)buf;
	unsigned int page_cnt_per_blk = nand_get_chip_block_size(PAGE);
	int page_cnt		      = bfn->platform[0];
	int copy_cnt		      = bfn->platform[1];
	int i, j;
	int offset = 0;
	printf("\033[0;31mpage table:\033[0m\n");
	if (copy_cnt == 1) {
		for (j = 0; j < page_cnt; j++) {
			/*16=head_size/sizeof(int)=NDFC_PAGE_TAB_HEAD_SIZE(64)/4*/
			printf("%4d ", *(buf + j + 16));
		}
		printf("\n");

	} else {
		for (i = 0; i < copy_cnt; i++) {
			printf("\033[0;34m%d copy in:\033[0m\n", i + 1);
			for (j = 0; j < page_cnt; j++) {
				offset = i + j * copy_cnt;
				/*16=head_size/sizeof(int)=NDFC_PAGE_TAB_HEAD_SIZE(64)/4*/
				if (j % 4 == 0)
					printf("\n");
				printf("b@%02dp%04d ",
						*(buf + offset + 16) / page_cnt_per_blk,
						*(buf + offset + 16) % page_cnt_per_blk);
			}
			printf("\n");
		}
	}
}

int sunxi_nand_info_dump(void *buffer)
{
	unsigned char id[8]		= { 0 };
	int chip_cnt			= nand_get_chip_cnt();
	int die_cnt_per_chip		= nand_get_chip_die_cnt();
	int blks_per_die		= nand_get_chip_die_size(BLOCK);
	unsigned int page_cnt_per_blk   = nand_get_chip_block_size(PAGE);
	unsigned int sects_per_page     = nand_get_chip_page_size(SECTOR);
	unsigned int multi_program_flag = nand_get_muti_program_flag();
	int super_chip_cnt		= nand_get_super_chip_cnt();
	unsigned int multi_plane_flag   = nand_get_twoplane_flag();
	unsigned int support_v_interleave =
		nand_get_support_v_interleave_flag();
	unsigned int support_dual_channel   = nand_get_support_dual_channel();
	unsigned int blk_cnt_per_super_chip = nand_get_super_chip_size();
	unsigned int sector_cnt_per_super_page =
		nand_get_super_chip_page_size();
	unsigned int page_cnt_per_super_blk = nand_get_super_chip_block_size();
	unsigned int page_offset_for_next_super_blk =
		nand_get_super_chip_pages_offset_to_block();
	unsigned int spare_bytes = nand_get_super_chip_spare_size();
	unsigned int multi_plane_block_offset =
		nand_get_chip_multi_plane_block_offset();

	nand_get_chip_id(id, sizeof(id));

	struct sunxi_nand_info *nand_info = malloc_cache_aligned(sizeof(struct sunxi_nand_info));
	if (nand_info == NULL) {
		printf("nand_info malloc err\n");
		return -1;
	}

	nand_get_chip_id(nand_info->id, sizeof(nand_info->id));
	nand_info->chip_cnt			= nand_get_chip_cnt();
	nand_info->die_cnt_per_chip		= nand_get_chip_die_cnt();
	nand_info->blks_per_die		= nand_get_chip_die_size(BLOCK);
	nand_info->page_cnt_per_blk   = nand_get_chip_block_size(PAGE);
	nand_info->sects_per_page     = nand_get_chip_page_size(SECTOR);
	nand_info->multi_program_flag = nand_get_muti_program_flag();
	nand_info->super_chip_cnt		= nand_get_super_chip_cnt();
	nand_info->multi_plane_flag   = nand_get_twoplane_flag();
	nand_info->support_v_interleave =
		nand_get_support_v_interleave_flag();
	nand_info->support_dual_channel   = nand_get_support_dual_channel();
	nand_info->blk_cnt_per_super_chip = nand_get_super_chip_size();
	nand_info->sector_cnt_per_super_page =
		nand_get_super_chip_page_size();
	nand_info->page_cnt_per_super_blk = nand_get_super_chip_block_size();
	nand_info->page_offset_for_next_super_blk =
		nand_get_super_chip_pages_offset_to_block();
	nand_info->spare_bytes = nand_get_super_chip_spare_size();
	nand_info->multi_plane_block_offset =
		nand_get_chip_multi_plane_block_offset();
	if (buffer) {
		memcpy(buffer, nand_info, sizeof(struct sunxi_nand_info));
		printf("\033[0;34m-\033[0m \033[0;36m%-30s:%5d \033[0;34m%20c-\033[0m",
				"total chips", ((struct sunxi_nand_info *)buffer)->chip_cnt, 0);
		printf("\n");
	}


	free(nand_info);
	nand_info = NULL;

	printf("\033[0;34m-------------------------\033[0m\033[0;31m nand info\033[0m\033[0;34m-----------------------------\033[0m\n");
	printf("\033[0;34m------------------------- physic layer -------------------------\033[0m");
	printf("\n");

	printf("\033[0;34m-\033[0m \033[0;36m %-30s: %x %x %x %x %x %x %x %x  \033[0;34m-\033[0m",
			"id", id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);
	printf("\n");

	printf("\033[0;34m-\033[0m \033[0;36m%-30s:%5d \033[0;34m%20c-\033[0m",
			"total chips", chip_cnt, 0);
	printf("\n");

	printf("\033[0;34m-\033[0m \033[0;36m%-30s:%5d \033[0;34m%20c-\033[0m",
			"total dies", die_cnt_per_chip, 0);
	printf("\n");

	printf("\033[0;34m-\033[0m \033[0;36m%-30s:%5d \033[0;34m%20c-\033[0m",
			"total blocks", blks_per_die, 0);
	printf("\n");

	printf("\033[0;34m-\033[0m \033[0;36m%-30s:%5d \033[0;34m%20c-\033[0m",
			"pages_per_block", page_cnt_per_blk, 0);
	printf("\n");

	printf("\033[0;34m-\033[0m \033[0;36m%-30s:%5d \033[0;34m%20c-\033[0m",
			"sectors_per_page", sects_per_page, 0);
	printf("\n");

	printf("\033[0;34m-\033[0m \033[0;36m%-30s:%s \033[0;34m%20c-\033[0m",
			"support_multi_program", multi_program_flag ? " yes" : " no", 0);
	printf("\n");

	printf("\033[0;34m------------------------- logical layer ------------------------\033[0m");
	printf("\n");

	printf("\033[0;34m-\033[0m \033[0;36m%-30s:%5d \033[0;34m%20c-\033[0m",
			"super_chip_cnt", super_chip_cnt, 0);
	printf("\n");

	printf("\033[0;34m-\033[0m \033[0;36m%-30s:%s \033[0;34m%20c-\033[0m",
			"support two plane", multi_plane_flag ? " yes" : " no", 0);
	printf("\n");

	printf("\033[0;34m-\033[0m \033[0;36m%-30s:%s \033[0;34m%20c-\033[0m",
			"support v interleave", support_v_interleave ? " yes" : " no",
			0);
	printf("\n");

	printf("\033[0;34m-\033[0m \033[0;36m%-30s:%s \033[0;34m%20c-\033[0m",
			"support dual channel", support_dual_channel ? " yes" : " no",
			0);
	printf("\n");

	printf("\033[0;34m-\033[0m \033[0;36m%-30s:%5d \033[0;34m%20c-\033[0m",
			"blk_cnt_per_super_chip", blk_cnt_per_super_chip, 0);
	printf("\n");

	printf("\033[0;34m-\033[0m \033[0;36m%-30s:%5d \033[0;34m%20c-\033[0m",
			"sector_cnt_per_super_page", sector_cnt_per_super_page, 0);
	printf("\n");

	printf("\033[0;34m-\033[0m \033[0;36m%-30s:%5d \033[0;34m%20c-\033[0m",
			"page_cnt_per_super_blk", page_cnt_per_super_blk, 0);
	printf("\n");

	printf("\033[0;34m-\033[0m \033[0;36m%-30s:%5d \033[0;34m%20c-\033[0m",
			"page_offset_for_next_super_blk", page_offset_for_next_super_blk,
			0);
	printf("\n");

	printf("\033[0;34m-\033[0m \033[0;36m%-30s:%5d \033[0;34m%20c-\033[0m",
			"spare_bytes", spare_bytes, 0);
	printf("\n");

	printf("\033[0;34m-\033[0m \033[0;36m%-30s:%5d \033[0;34m%20c-\033[0m",
			"multi plane block address offset", multi_plane_block_offset, 0);
	printf("\n");

	printf("\033[0;34m-     ----------------------------------------------------     -\033[0m\n");

	printf("\033[0;34m----------------------------------------------------------------\033[0m\n");
	return 0;
}

static void sunxi_nand_phy_pages_dump(int chip, int block, int start_page,
		int end_page, void *pmem,
		unsigned int print_flag, int mem_flag)
{
	int i			= 0;
	int ret			= 0;
	unsigned long len       = 0;
	unsigned int page_size  = nand_get_chip_page_size(BYTE);
	unsigned char *mbuf     = malloc_align(page_size, 64);
	unsigned char spare[16] = {};

	if (!mbuf) {
		printf("malloc buf fail\n");
		return;
	}
	/*main data*/
	for (i = start_page; i < end_page; i++) {
		memset(mbuf, 0, page_size);
		ret = nand_physic_read_page(chip, block, i, page_size >> 9,
				mbuf, NULL);
		if (ret < 0) {
			printf("read chip@%d block@%d page@%d main data fail\n",
					chip, block, i);
			memset(mbuf, 0, page_size);
		}

		memcpy(pmem + len, mbuf, page_size);
		if (print_flag)
			dumphex_s_e(mbuf, i * page_size, (i + 1) * page_size);

		len = len + page_size;
	}

	/*spare data*/
	for (i = start_page; i < end_page + 1; i++) {
		memset(spare, 0, 16);
		ret = nand_physic_read_page(chip, block, i, 0, NULL, spare);
		if (ret < 0) {
			printf("read chip@%d block@%d page@%d spare fail\n",
					chip, block, i);
			memset(spare, 0, 16);
		}
		if (print_flag) {
			printf("chip@%d block@%d page@%d spare\n", chip, block,
					i);
			dumphex(spare, 16);
		}
	}

	if (mem_flag) {
		printf("\033[0;36mstart pmem = 0x%x \033[0m\n",
				(unsigned int)((unsigned int *)pmem));
		printf("\033[0;36mend   pmem = 0x%x \033[0m\n",
				(unsigned int)(((unsigned char *)pmem) + len));
	}
}

static void sunxi_nand_phy_pages_write(int chip, int block, int start_page,
		int end_page, void *pmem,
		unsigned int print_flag, int mem_flag)
{
	int i			= 0;
	int ret			= 0;
	unsigned long len       = 0;
	unsigned int page_size  = nand_get_chip_page_size(BYTE);
	unsigned char *mbuf;

	/*main data*/
	for (i = start_page; i < end_page + 1; i++) {
		mbuf = pmem + len;
		ret = nand_physic_write_page(chip, block, i, page_size >> 9,
				mbuf, NULL);
		if (ret < 0) {
			printf("write chip@%d block@%d page@%d main data fail\n",
					chip, block, i);
		}
	}
}

static void sunxi_nand_phy_blocks_dump(int chip, int start_block, int end_block,
		void *pmem, unsigned int print_flag)
{
	int b = 0;
	/*int p = 0;*/
	unsigned long len = 0;

	int page_num		= nand_get_chip_block_size(PAGE);
	//	unsigned int page_size  = nand_get_chip_page_size(BYTE);
	unsigned int block_size = nand_get_chip_block_size(BYTE);
	/*	unsigned char *mbuf     = malloc_align(page_size, 64);
		if (!mbuf) {
		printf("malloc mbuf fail\n");
		return;
		}
		*/
	for (b = start_block; b < end_block + 1; b++) {
		sunxi_nand_phy_pages_dump(chip, b, 0, page_num - 1, pmem + len,
				print_flag, 0);

		len = len + block_size;
	}

	printf("\033[0;36mstart pmem = 0x%x \033[0m\n",
			(unsigned int)((unsigned int *)pmem));
	printf("\033[0;36mend  pmem = 0x%x \033[0m\n",
			(unsigned int)((unsigned char *)pmem + len + block_size));
	return;
}

static void sunxi_nand_phy_blocks_write(int chip, int start_block, int end_block,
		void *pmem, unsigned int print_flag)
{
	int b = 0;
	unsigned long len = 0;

	int page_num		= nand_get_chip_block_size(PAGE);
	unsigned int block_size = nand_get_chip_block_size(BYTE);

	for (b = start_block; b < end_block + 1; b++) {
		sunxi_nand_phy_pages_write(chip, b, 0, page_num - 1, pmem + len,
				print_flag, 0);

		len = len + block_size;
	}
	return;
}
void sunxi_nand_phy_page(int start_page, int nbyte, void *pmem)
{
	int blocks = nand_get_chip_die_size(BLOCK);
	int npages = nbyte / nand_get_chip_page_size(BYTE);
	int chip = 0;
	int start_block, st_page;
	start_block = start_page / nand_get_chip_block_size(PAGE);
	st_page = start_page % nand_get_chip_block_size(PAGE);
	if (start_block > blocks)
		chip = 1;

	sunxi_nand_phy_pages_dump(chip, start_block, st_page, npages+st_page, pmem, 0, 0);

	return;
}

static void sunxi_nand_logic_data_dump(int start_sector, int end_sector,
		void *pmem, unsigned int print_flag)
{
	int sector_num = end_sector - start_sector + 1;
	int ret	= 0;

	ret = sunxi_flash_read(start_sector, sector_num, pmem);
	if (!ret) {
		printf("read logic data from %d to %d err\n", start_sector,
				end_sector);
	}

	if (print_flag) {
		printf("start sector@%d end sector@%d logic data:\n",
				start_sector, end_sector);
		dumphex(pmem, sector_num << 9);
	}

	printf("\033[0;36mstart pmem = 0x%x \033[0m\n",
			(unsigned int)(unsigned int *)pmem);
	printf("\033[0;36mend   pmem = 0x%x \033[0m\n",
			(unsigned int)(((unsigned char *)pmem) +
				((sector_num + 1) << 9)));
	return;
}

static void sunxi_nand_phy_blocks_erase(int chip, int start_block, int end_block,
		unsigned int print_flag)
{
	int b = 0;
	int ret = 0;

	for (b = start_block; b < end_block + 1; b++) {
		ret = nand_physic_erase_block(chip, b);
		if (ret < 0) {
			printf("erase block %d err\n", b);
		}
	}

	return;
}


void sunxi_nand_logic_history_data_dump(int start_sector, int sector_num,
		void *pmem,
		unsigned int print_flag)
{
	//int sector_num = end_sector - start_sector + 1;
	int ret	= 0;

	ret = nand_uboot_read_history(start_sector, sector_num, pmem);
	/*ret = sunxi_flash_read(start_sector, sector_num, pmem);*/
	if (!ret) {
		printf("read logic data from %d to %d err\n", start_sector,
				start_sector + sector_num);
	}

	/*
	 *if (print_flag) {
	 *        printf("start sector@%d end sector@%d logic data:\n", start_sector, end_sector);
	 *        dumphex(pmem, sector_num << 9);
	 *}
	 */

	/*
	 *printf("\033[0;36mstart pmem = 0x%x \033[0m\n", (unsigned int)(unsigned int *)pmem);
	 *printf("\033[0;36mend   pmem = 0x%x \033[0m\n",
	 *                (unsigned int)(((unsigned char *)pmem) + ((sector_num + 1) << 9)));
	 */
	return;
}

static void sunxi_nand_retry_table_dump(void)
{
	unsigned int page_size  = nand_get_chip_page_size(BYTE);
	unsigned char *page_buf = malloc_align(page_size, 64);
	;
	unsigned char spare_data[64] = { 0 };
	int ret			     = 0;
	int i = 0, p = 0;
	boot_file_head_t *bfn = (boot_file_head_t *)page_buf;

	if (!page_buf) {
		printf("%s malloc buffer fail\n", __func__);
		return;
	}

	memset(page_buf, 0, page_size);

	for (p = RR_TAB_PAGE_START; p < RR_TAB_PAGE_END; p++) {
		memset(page_buf, 0, page_size);

		ret = nand_physic_read_page(0, RR_TAB_BLOCK_START, p, page_size,
				page_buf, spare_data);
		if (ret < 0) {
			printf("read rr fail in chip@%d block@%d page%d\n", 0,
					RR_TAB_BLOCK_START, p);
			continue;
		}

		if (check_sum((unsigned int *)page_buf, bfn->length)) {
			printf("page : %d not read retry table\n", p);
			continue;
		}
		printf("============= read retry table ==================\n");
		for (i = 0; i < sizeof(bfn->magic); i++) {
			printf("%c", bfn->magic[i]);
		}
		printf("\n");
		printf("rr table size: %d\n", bfn->length);
		printf("check sum: 0x%x\n", bfn->check_sum);
		printf("page size: %d(sectors)\n", bfn->platform[3]);
		printf("table:\n");

		dumphex(page_buf, bfn->length);
		memset(page_buf, 0, page_size);

		goto err;
	}

	printf("have no read retry table!\n");
err:
	free_align(page_buf);

	return;
}

static void sunxi_nand_page_table_dump(void *buffer)
{
	unsigned int page_size	 = nand_get_chip_page_size(BYTE);
	unsigned int pages_per_block   = nand_get_chip_block_size(PAGE);
	unsigned int uboot_start_block = get_uboot_start_block();
	unsigned char *page_buf	= malloc_align(page_size, 64);
	if (!page_buf) {
		printf("%s malloc buffer fail\n", __func__);
		return ;
	}
	unsigned char spare_data[64] = { 0 };
	boot_file_head_t *bfn	= (boot_file_head_t *)page_buf;
	int ret			     = 0;
	int b, p;
	int page_cnt;
	int copy_cnt;

	if (page_size < 8192 || pages_per_block < 128) {
		printf("have no page table");
		return ;
	}

	for (b = 0; b < uboot_start_block; b++) {
		for (p = PAGE_TAB_PAGE_START; p < PAGE_TAB_PAGE_END; p++) {
			memset(page_buf, 0, page_size);

			ret = nand_physic_read_boot0_page(0, b, p, 0, page_buf,
					spare_data);
			if (ret < 0) {
				printf("read page tab fail in chip@%d, block@%d"
						" page@%d",
						0, b, p);
				continue;
			}

			if (bfn->platform[1] == 1) {
				if (check_sum((unsigned int *)page_buf, 1024)) {
					continue;
				}

			} else {
				if (check_sum((unsigned int *)page_buf,
							bfn->length)) {
					continue;
				}
			}
			break;
		}
		break;
	}

	if (b == 7) {
		printf("page table is bad!\n");
		goto err;
	}
	page_cnt = bfn->platform[0];
	copy_cnt = bfn->platform[1];
	printf("========================== page table info ========================\n");
	printf("%-23s: %d\n", "one copy pages", page_cnt);
	printf("%-23s: %d\n", "total copy", copy_cnt);
	printf("storage size one page: %d\n", bfn->platform[3] * 512);
	display_page_table((unsigned int *)page_buf);
	if (buffer != NULL)
		memcpy(buffer, page_buf, page_size);
err:
	free_align(page_buf);

	return ;
}

void sunxi_nand_page_table(void *mem)
{
	sunxi_nand_page_table_dump(mem);
}

void sunxi_nand_bad_block_table_dump(void)
{
	int chip_cnt	 = nand_get_chip_cnt();
	int die_cnt_per_chip = nand_get_chip_die_cnt();
	int blks_per_die     = nand_get_chip_die_size(BLOCK);

	int cnt = 0;
	int c = 0, b = 0;
	int ret		 = 0;
	int total_blocks = die_cnt_per_chip * blks_per_die;
	unsigned short r[chip_cnt][total_blocks];

	memset(r, 0, sizeof(r));

	printf("c(chip) b(block)\n");
	for (c = 0; c < chip_cnt; c++) {
		for (b = 0; b < total_blocks; b++) {
			printf("c@%db@%04d", c, b);
			ret = nand_physic_bad_block_check(c, b);
			if (ret < 0) {
				printf(" is bad\n");
				cnt++;
				r[c][b] = 1;
			} else {
				printf(" is good\n");
			}
		}
	}
	printf("scan total %d chip, total %d block (%d bad blocks)\n", chip_cnt,
			chip_cnt * total_blocks, cnt);
	printf("bad block:\n");
	for (c = 0; c < chip_cnt; c++) {
		for (b = 0; b < total_blocks; b++) {
			if (r[c][b] == 1) {
				printf("chip@%d block@%04d ", c, b);
				if (b % 4 == 0)
					printf("\n");
			}
		}
	}

	return ;
}

void sunxi_nand_boot0_dump(void *mem, int start, int print_flag)
{
	int ret		       = 0;
	int max_len	= 64*1024;
	//unsigned int page_size = nand_get_chip_page_size(BYTE);
	unsigned char *buf;
	int len		      = 0;

	len = sunxi_flash_get_boot0_size();

	buf = malloc_align(len, 64);
	if (!buf) {
		printf("%s malloc buffer fail\n", __func__);
		return;
	}

	/*read complete boot0*/
	ret = nand_read_boot0(buf, len);
	if (ret < 0) {
		printf("%s %d read boot0 fail\n", __func__, __LINE__);
		return;
	}

	if (mem) {
		memcpy(mem, buf + start, max_len);
	}
	if (print_flag) {
		printf("boot0:\n");
		dumphex(buf, len);
	}

	printf("\033[0;36mstart pmem = 0x%x \033[0m\n",
			(unsigned int)((unsigned int *)mem));
	printf("\033[0;36mend   pmem = 0x%x \033[0m\n",
			(unsigned int)(((unsigned char *)mem) + len));

	free_align(buf);
	return;
}

void sunxi_nand_performance_test(int *read_speed, int *write_speed)
{
	int uboot_next_block = get_uboot_next_block();
	int secure_next_block =
		nand_secure_storage_first_build(uboot_next_block);
	int physic_reserved_block    = get_physic_block_reserved();
	unsigned int page_size       = nand_get_chip_page_size(BYTE);
	__maybe_unused unsigned int block_size      = nand_get_chip_block_size(BYTE);
	unsigned int pages_per_block = nand_get_chip_block_size(PAGE);
	unsigned char *page_buf      = malloc_align(page_size, 64);

	if (!page_buf) {
		printf("%s malloc buffer fail\n", __func__);
		return;
	}
	int ret = 0;
	int b, p = 0;
	int chip	       = 0;
	unsigned long long len = 0;
	ulong time	     = 0;
	//	int read_speed = 0, write_speed = 0;
	int r_bad[60];
	int i = 0;
	int j = 0;

	memset(r_bad, 0, sizeof(int) * 60);

	memset(page_buf, 0xa5, page_size);
	printf("tese base on block %d - %d\n", secure_next_block, secure_next_block + physic_reserved_block - 1);

	printf("erase test start ...\n");
	time = get_timer(0);
	for (b = secure_next_block;
			b < secure_next_block + physic_reserved_block; b++, i++) {
		ret = nand_physic_erase_block(chip, b);
		if (ret < 0) {
			printf("%s erase block %d fail\n", __func__, b);
			r_bad[i] = b;
			continue;
		}
	}
	if (write_speed) {
		printf("write test start ...\n");
		time = get_timer(0);
		for (b = secure_next_block;
				b < secure_next_block + physic_reserved_block; b++) {
			for (j = 0; j < i; j++) {
				if (r_bad[j] == b)
					break;
			}
			if (j != i)
				continue;

			for (p = 0; p < pages_per_block; p++) {
				ret = nand_physic_write_page(chip, b, p, page_size >> 9,
						page_buf, NULL);
				if (ret < 0) {
					printf("%s write block@%d page@%d fail\n",
							__func__, b, p);
					continue;
				}
				len += page_size;
			}
		}
		time = get_timer(time);

		printf("time = %lu ms\n", time);
		printf("write test end len : %llu\n", len);
		*write_speed = len * 1000 / time / 1024;
		printf("total test  \033[0;36m%llu \033[0mKB\n", len >> 10);
		printf("time = %lu ms\n", time);
		printf("speed = \033[0;31m%d KB/s \033[0;0m\n", *write_speed);
	}
	/*memset(page_buf, 0, page_size);*/
	len = 0;
	printf("------------------------------------------\n");

	if (read_speed) {
		printf("read test start ...\n");
		time = get_timer(0);
		for (b = secure_next_block;
				b < secure_next_block + physic_reserved_block; b++) {
			for (p = 0; p < pages_per_block; p++) {
				ret = nand_physic_read_page(chip, b, p, page_size >> 9,
						page_buf, NULL);
				if (ret < 0) {
					printf("%s read block@%d page@%d fail\n",
							__func__, b, p);
					continue;
				}
				len += page_size;
			}
		}

		time = get_timer(time);
		printf("read test end\n");
		*read_speed = (len * 1000) / time / 1024;
		printf("total test  \033[0;36m%llu \033[0mKB\n", len >> 10);
		printf("time = %lu ms\n", time);
		printf("speed = \033[0;31m%d KB/s \033[0;0m\n", *read_speed);
	}
	memset(page_buf, 0, page_size);

	for (b = secure_next_block;
			b < secure_next_block + physic_reserved_block; b++) {
		ret = nand_physic_erase_block(chip, b);
		if (ret < 0) {
			printf("%s erase block fail\n", __func__);
			continue;
		}
	}

	if (page_buf)
		free_align(page_buf);

	return;
}

void sunxi_nand_wperf_test(int *write_speed)
{
	sunxi_nand_performance_test(NULL, write_speed);
}

void sunxi_nand_rperf_test(int *read_speed)
{
	sunxi_nand_performance_test(read_speed, NULL);
}

static void sunxi_nand_performance(void)
{
	int read_speed, write_speed;
	sunxi_nand_performance_test(&read_speed, &write_speed);
}

void sunxi_nand_test_read_write_normal(void)
{
	int uboot_next_block = get_uboot_next_block();
	int secure_next_block =
		nand_secure_storage_first_build(uboot_next_block);
	int physic_reserved_block = get_physic_block_reserved();
	unsigned int page_size    = nand_get_chip_page_size(BYTE);
	unsigned char *page_buf   = malloc_align(page_size, 64);

	unsigned char *r_page_buf = malloc_align(page_size, 64);

	unsigned int pages_per_block = nand_get_chip_block_size(PAGE);
	unsigned char spare[8]       = {};
	unsigned char r_spare[8]     = {};

	if (!page_buf || !r_page_buf) {
		printf("%s malloc buffer fail\n", __func__);
		return;
	}
	int ret = 0;
	int b, p = 0;
	int chip = 0;

	/*#define TEST (0x74657374)*/
	printf("uboot_next_block:%d\n", uboot_next_block);
	printf("secure_next_block:%d\n", secure_next_block);
	printf("physic_next_block:%d\n", physic_reserved_block);

	memset(page_buf, 0xA5, page_size);
	memset(r_page_buf, 0, page_size);
	memset(spare, 0xA5, sizeof(spare));
	memset(r_spare, 0, sizeof(spare));

	for (b = secure_next_block; b < secure_next_block + 1; b++) {
		printf("block@%d \n", b);
		ret = nand_physic_erase_block(chip, b);
		if (ret < 0) {
			printf("%s erase block fail\n", __func__);
			continue;
		}

		printf("pages_per_block@%d \n", pages_per_block);
		for (p = 0; p < pages_per_block; p++) {
			ret = nand_physic_write_page(chip, b, p, page_size >> 9,
					page_buf, spare);
			if (ret < 0) {
				printf("%s write block@%d page@%d fail\n",
						__func__, b, p);
				continue;
			}

			ret = nand_physic_read_page(chip, b, p, page_size >> 9,
					r_page_buf, r_spare);
			if (ret < 0) {
				printf("%s read block@%d page@%d fail\n",
						__func__, b, p);
				continue;
			}

			/*compare main data*/
			if (memcmp(page_buf, r_page_buf, page_size)) {
				printf("block@%d page@%d read & write main content is differrent\n",
						b, p);
				printf("write main data:\n");
				dumphex(page_buf, page_size);
				printf("read main data:\n");
				dumphex(r_page_buf, page_size);
			} else {
				printf("b@%d p@%d read & write main is same\n",
						b, p);
			}

			/*compare spare data*/
			if (memcmp(spare, r_spare, sizeof(spare))) {
				printf("block@%d page@%d read & write spare content is differrent\n",
						b, p);
				printf("write spare data:\n");
				dumphex(spare, page_size);
				printf("read spare data:\n");
				dumphex(r_spare, page_size);
			} else {
				printf("b@%d p@%d read & write spare is same\n",
						b, p);
			}
		}

		ret = nand_physic_erase_block(chip, b);
		if (ret < 0) {
			printf("%s erase block fail\n", __func__);
			continue;
		}
	}

	if (r_page_buf)
		free_align(r_page_buf);

	if (page_buf)
		free_align(page_buf);

	return;
}

void sunxi_nand_boot1_dump_for_efex(void *mem, int len)
{
	extern int nand_get_uboot_total_len(void);
	int ret		       = 0;
	unsigned int page_size = nand_get_chip_page_size(BYTE);
	int length = nand_get_uboot_total_len();
	printf("uboot total lenght@%d\n", length);
	unsigned char *boot1 = malloc_align(ALIGN(length, page_size), 64);
	static int received;

	memset(boot1, 0x00, ALIGN(length, page_size));
	ret = nand_read_uboot_data(boot1, length);
	if (ret != 0) {
		printf("read uboot fail\n");
		goto out;
	} else {
		memcpy(mem, boot1 + received, len);
		received += len;
	}

out:
	free_align(boot1);
	return;
}

int do_sunxi_nand_dump(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int chip		= 0;
	int block1		= 0;
	int block2		= 0;
	int page1		= 0;
	int page2		= 0;
	int sector1		= 0;
	int sector2		= 0;
	unsigned int print_flag = 0;

	void *pmem = NULL;
	ulong time	     = 0;
	time = get_timer(0);

	if (!strcmp(argv[1], "nand_info")) {
		sunxi_nand_info_dump(NULL);

	} else if (!strcmp(argv[1], "dump_phy_pages")) {
		if (argc != 8)
			return CMD_RET_USAGE;
		chip       = simple_strtoul(argv[2], 0, 0);
		block1     = simple_strtoul(argv[3], 0, 0);
		page1      = simple_strtoul(argv[4], 0, 0);
		page2      = simple_strtoul(argv[5], 0, 0);
		pmem       = (void *)simple_strtoul(argv[6], 0, 16);
		print_flag = strcmp(argv[7], "no") ? 1 : 0;

		sunxi_nand_phy_pages_dump(chip, block1, page1, page2, pmem,
				print_flag, 1);

	} else if (!strcmp(argv[1], "dump_phy_blocks")) {
		if (argc != 7)
			return CMD_RET_USAGE;
		chip       = simple_strtoul(argv[2], 0, 0);
		block1     = simple_strtoul(argv[3], 0, 0);
		block2     = simple_strtoul(argv[4], 0, 0);
		pmem       = (void *)simple_strtoul(argv[5], 0, 16);
		print_flag = strcmp(argv[6], "no") ? 1 : 0;
		sunxi_nand_phy_blocks_dump(chip, block1, block2, pmem,
				print_flag);
	} else if (!strcmp(argv[1], "dump_logic_data")) {
		if (argc != 6)
			return CMD_RET_USAGE;

		sector1    = simple_strtoul(argv[2], 0, 0);
		sector2    = simple_strtoul(argv[3], 0, 0);
		pmem       = (void *)simple_strtoul(argv[4], 0, 16);
		print_flag = strcmp(argv[5], "no") ? 1 : 0;

		sunxi_nand_logic_data_dump(sector1, sector2, pmem, print_flag);

	} else if (!strcmp(argv[1], "erase_phy_blocks")) {
		if (argc != 6)
			return CMD_RET_USAGE;
		chip       = simple_strtoul(argv[2], 0, 0);
		block1     = simple_strtoul(argv[3], 0, 0);
		block2     = simple_strtoul(argv[4], 0, 0);
		print_flag = strcmp(argv[5], "no") ? 1 : 0;
		sunxi_nand_phy_blocks_erase(chip, block1, block2, print_flag);

	} else if (!strcmp(argv[1], "write_phy_pages")) {
		if (argc != 8)
			return CMD_RET_USAGE;
		chip       = simple_strtoul(argv[2], 0, 0);
		block1     = simple_strtoul(argv[3], 0, 0);
		page1      = simple_strtoul(argv[4], 0, 0);
		page2      = simple_strtoul(argv[5], 0, 0);
		pmem       = (void *)simple_strtoul(argv[6], 0, 16);
		print_flag = strcmp(argv[7], "no") ? 1 : 0;

		sunxi_nand_phy_pages_write(chip, block1, page1, page2, pmem,
				print_flag, 1);


	} else if (!strcmp(argv[1], "write_phy_blocks")) {
		if (argc != 7)
			return CMD_RET_USAGE;
		chip       = simple_strtoul(argv[2], 0, 0);
		block1     = simple_strtoul(argv[3], 0, 0);
		block2     = simple_strtoul(argv[4], 0, 0);
		pmem       = (void *)simple_strtoul(argv[5], 0, 16);
		print_flag = strcmp(argv[6], "no") ? 1 : 0;
		sunxi_nand_phy_blocks_write(chip, block1, block2, pmem, print_flag);

	} else if (!strcmp(argv[1], "dump_history_data")) {
		if (argc != 4)
			return CMD_RET_USAGE;
		sector1		 = simple_strtoul(argv[2], 0, 0);
		sector2		 = simple_strtoul(argv[3], 0, 0);
		print_flag       = strcmp(argv[5], "no") ? 1 : 0;
		unsigned int len = (sector2 - sector1 + 1) << 9;
		pmem		 = malloc_align(len, 64);

		sunxi_nand_logic_history_data_dump(sector1, sector2, pmem,
				print_flag);

	} else if (!strcmp(argv[1], "dump_read_retry_table")) {
		if (argc != 2)
			return CMD_RET_USAGE;
		sunxi_nand_retry_table_dump();
	} else if (!strcmp(argv[1], "dump_page_table")) {
		if (argc != 2)
			return CMD_RET_USAGE;

		sunxi_nand_page_table_dump(NULL);
	} else if (!strcmp(argv[1], "dump_boot0")) {
		if (argc != 4)
			return CMD_RET_USAGE;
		pmem       = (void *)simple_strtoul(argv[2], 0, 16);
		print_flag = strcmp(argv[3], "no") ? 1 : 0;
		sunxi_nand_boot0_dump(pmem, 0, print_flag);

	} else if (!strcmp(argv[1], "dump_bad_table")) {
		if (argc != 2)
			return CMD_RET_USAGE;
		sunxi_nand_bad_block_table_dump();

	} else if (!strcmp(argv[1], "performance")) {
		printf("performance start ...");
		sunxi_nand_performance();

	} else if (!strcmp(argv[1], "read-write")) {
		printf("check read-write basic function is noraml?");
		sunxi_nand_test_read_write_normal();

	} else {
		return CMD_RET_USAGE;
	}

	printf("\n");
	time = get_timer(time);
	printf("sunxi_nand cmd time = %lu ms\n", time);
	return 0;
}

U_BOOT_CMD(
		sunxi_nand_test, CONFIG_SYS_MAXARGS, 1, do_sunxi_nand_dump,
		"sunxi_nand_test sub systerm",
		"\n" DUMP_NAND_ONFO DUMP_NAND_INFO_HELP DUMP_NAND_PAGES DUMP_NAND_PAGES_HELP
		DUMP_BLOCKS DUMP_BLOCKS_HELP DUMP_LOGIC_DATA DUMP_LOGIC_DATA_HELP
		ERASE_BLOCKS ERASE_BLOCKS_HELP
		WRITE_NAND_PAGES WRITE_NAND_PAGES_HELP
		WRITE_BLOCKS WRITE_BLOCKS_HELP
		DUMP_HISTORY_DATA DUMP_HISTORY_DATA_HELP DUMP_READ_RETRY_TABLE
		DUMP_READ_RETRY_TABLE_HELP DUMP_PAGE_TABLE DUMP_PAGE_TABLE_HELP
		DUMP_BAD_TABLE DUMP_BAD_TABLE_HELP DUMP_BOOT0
		DUMP_BOOT0_HELP DUMP_READ_WRITE_PERFORMANCE
		DUMP_READ_WRITE_PERFORMANCE_HELP
		CHECK_READ_WRITE_FUNCTION
		CHECK_READ_WRITE_FUNCTION_HELP
		"\n");
