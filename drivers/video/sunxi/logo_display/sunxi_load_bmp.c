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

extern int sunxi_partition_get_partno_byname(const char *part_name);

/*this function will creat memory address that was be use to logo buffer
 *the return value meant:
 *1:CONFIG_SUNXI_LOGBUFFER that in top of dram and size you can be set
 *in sun8iw16p1.h
 *2:SUNXI_DISPLAY_FRAME_BUFFER_ADDR that you can set a physical address
 *you want and the address & size can be set in sun8iw16p1.h
 *3:the address will malloc.
 */
static int malloc_bmp_mem(char **buff, int size)
{
	int ret;
#if defined(CONFIG_SUNXI_LOGBUFFER)
	*buff = (void *)(CONFIG_SYS_SDRAM_BASE + gd->ram_size - SUNXI_DISPLAY_FRAME_BUFFER_SIZE);
	ret = 1;
#elif defined(SUNXI_DISPLAY_FRAME_BUFFER_ADDR)
	*buff = (void *)(SUNXI_DISPLAY_FRAME_BUFFER_ADDR);
	ret = 2;
#else
	*buff = memalign(CONFIG_SYS_CACHELINE_SIZE, (size + 512 + 0x1000));
	if (NULL == bmp_buff) {
	    pr_error("bmp buffer malloc error!\n");
	    return -1;
	}
	ret = 3;
#endif
	return ret;
}

static int board_display_update_para_for_kernel(char *name, int value)
{
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
	char disp_reserve[80];

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
	snprintf(disp_reserve, 80, "%d,0x%x", rblock * 512, (unsigned int)align_addr);
	env_set("disp_reserve", disp_reserve);
	ret = sunxi_flash_read(start_block, rblock, align_addr);
	board_display_update_para_for_kernel("fb_base", (uint)align_addr);
	return 0;

}

