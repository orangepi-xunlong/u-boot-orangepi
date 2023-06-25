/*This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 *This file/function use show bootlogo in SPINOR Flash
 *This function was clip from cmd_sunxi_bmp.c Becaus SPINOR
 *Flash size is to less ,so we don't need other function when
 *we only want to show bootlogo.
 */

#include <common.h>
#include <bmp_layout.h>
#include <command.h>
#include <malloc.h>
#include <sunxi_bmp.h>
#include <bmp_layout.h>
#include <sunxi_board.h>
#include <sys_partition.h>
#include <fdt_support.h>

static void *bmp_addr;
extern int sunxi_partition_get_partno_byname(const char *part_name);

static int malloc_bmp_mem(char **buff, int size)
{
	*buff = memalign(CONFIG_SYS_CACHELINE_SIZE, (size + 512 + 0x1000));
	if (NULL == *buff) {
	    pr_error("buffer malloc error!\n");
	    return -1;
	}
	return 0;
}

static void update_disp_reserve(unsigned int size, void *addr)
{
	char disp_reserve[80];

	snprintf(disp_reserve, 80, "%d,0x%p", size, addr);
#if defined(CONFIG_CMD_FAT)
	env_set("disp_reserve", disp_reserve);
#else
	pr_error("You need to enable CONFIG_CMD_FAT to update disp_reserve\n");
#endif
}

int save_bmp_logo_to_kernel(void)
{
	char name[] = "fb_base";
	int value = (int)bmp_addr;
#ifndef CONFIG_SUNXI_MULITCORE_BOOT
	int node;
	int ret = -1;

	node = fdt_path_offset(working_fdt, "disp");
	if (node < 0) {
		pr_error("%s:disp_fdt_nodeoffset %s fail\n", __func__, "disp");
		goto exit;
	}

	ret = fdt_setprop_u32(working_fdt, node, name, (uint32_t)value);
	if (ret < 0)
		pr_error("fdt_setprop_u32 %s.%s(0x%x) fail.err code:%s\n",
				       "disp", name, value, fdt_strerror(ret));
	else
		ret = 0;

exit:
	return ret;
#else
	return sunxi_fdt_getprop_store(working_fdt, "disp", name, value);
#endif
}

int fat_read_logo_to_kernel(char *name)
{
	int ret = -1;
#if defined(CONFIG_CMD_FAT)
	char *argv[6];
	char bmp_head[32];
	char bmp_name[32];
	char part_info[16] = {0};
	int partno = -1;
	struct bmp_header *bmp_head_addr = (struct bmp_header *)CONFIG_SYS_SDRAM_BASE;
	char *bmp_buff = NULL;

	if (bmp_head_addr) {
		sprintf(bmp_head, "%lx", (ulong)bmp_head_addr);
	} else {
		pr_error("sunxi bmp: alloc buffer for %s fail\n", name);
		return -1;
	}
	strncpy(bmp_name, name, sizeof(bmp_name));
	tick_printf("bmp_name=%s\n", bmp_name);

	partno = sunxi_partition_get_partno_byname("boot-resource"); /*android*/
	if (partno < 0) {
		pr_error("Get boot-resource partition number fail!\n");
		return -1;
	}
	snprintf(part_info, 16, "0:%x", partno);

	argv[0] = "fatload";
	argv[1] = "sunxi_flash";
	argv[2] = part_info;
	argv[3] = bmp_head;
	argv[4] = bmp_name;
	argv[5] = NULL;

	if (do_fat_fsload(0, 0, 5, argv)) {
		pr_error("sunxi bmp info error : unable to open logo file %s\n",
		       argv[4]);
		return -1;
	}
	if (bmp_head_addr->signature[0] != 'B' ||
	    bmp_head_addr->signature[1] != 'M') {
		pr_error("This not a BMP file!\n");
		return 0;
	}
	ret = malloc_bmp_mem(&bmp_buff, bmp_head_addr->file_size);
	if (ret) {
		pr_error("Malloc fail!\n");
		return 0;
	}

	memcpy(bmp_buff, bmp_head_addr, bmp_head_addr->file_size);
	update_disp_reserve(bmp_head_addr->file_size, (void *)bmp_buff);
	bmp_addr = (void *)bmp_buff;
#endif
	return ret;
}

/*
 *get BMP file from partition ,so give it partition name
 */
int read_bmp_to_kernel(char *partition_name)
{
	struct bmp_header *bmp_header_info = NULL;
	char *bmp_buff = NULL;
	char *align_addr = NULL;
	u32 start_block = 0;
	u32 rblock = 0;
	char tmp[512];
	u32 patition_size = 0;
	int ret = -1;
	struct lzma_header *lzma_head = NULL;
	u32 file_size;

	sunxi_partition_get_info_byname(partition_name, &start_block, &patition_size);
	if (!start_block) {
		pr_error("cant find part named %s\n", (char *)partition_name);
		return -1;
	}

	ret = sunxi_flash_read(start_block, 1, tmp);
	bmp_header_info = (struct bmp_header *)tmp;
	lzma_head = (struct lzma_header *)tmp;
	file_size = bmp_header_info->file_size;

	/*checking the data whether if a BMP file*/
	if (bmp_header_info->signature[0] != 'B' ||
	    bmp_header_info->signature[1] != 'M') {
		if (lzma_head->signature[0] != 'L' ||
		    lzma_head->signature[1] != 'Z' ||
		    lzma_head->signature[2] != 'M' ||
		    lzma_head->signature[3] != 'A') {
			pr_error(
			    "file  is neither a bmp file nor a lzma file\n");
			return -1;
		} else
			file_size = lzma_head->file_size;
	}

	/*malloc some memroy for bmp buff*/
	if (patition_size > ((file_size / 512) + 1)) {
	    rblock = (file_size / 512) + 1;
	} else {
	    rblock = patition_size;
	}

	ret = malloc_bmp_mem(&bmp_buff, file_size);
	if (NULL == bmp_buff) {
	    pr_error("bmp buffer malloc error!\n");
	    return -1;
	}

	/*if malloc, will be 4K aligned*/
	if (ret == 3) {
		align_addr = (char *)((unsigned int)bmp_buff +
			(0x1000U - (0xfffu & (unsigned int)bmp_buff)));
	} else {
		align_addr = bmp_buff;
	}

	printf("the align_addr is %x \n", (unsigned int)align_addr);

	/*read logo.bmp all info*/
	update_disp_reserve(rblock * 512, (void *)align_addr);
	ret = sunxi_flash_read(start_block, rblock, align_addr);
	bmp_addr = (void *)align_addr;
	return 0;

}
