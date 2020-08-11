/*This file/function use show bootlogo in SPINOR Flash
 *This function was clip from cmd_sunxi_bmp.c Becaus SPINOR
 *Flash size is to less ,so we don't need other function when
 *we only want to show bootlogo.
 */

#include <common.h>
#include <bmp_layout.h>
#include <command.h>
#include <malloc.h>
#include <sunxi_bmp.h>
#include <sunxi_board.h>
#include <sys_partition.h>
#include <fdt_support.h>

#ifdef CONFIG_SUN8IW12P1_NOR
extern int sunxi_partition_get_partno_byname(const char *part_name);

#ifndef CONFIG_BOOTLOGO_DISABLE
static int board_display_update_para_for_kernel(char *name, int value)
{
#ifndef CONFIG_SUNXI_MULITCORE_BOOT
	int node;
	int ret = -1;

	node = fdt_path_offset(working_fdt,"disp");
	if (node < 0) {
		pr_error("%s:disp_fdt_nodeoffset %s fail\n", __func__,"disp");
		goto exit;
	}

	ret = fdt_setprop_u32(working_fdt, node, name, (uint32_t)value);
	if ( ret < 0)
		pr_error("fdt_setprop_u32 %s.%s(0x%x) fail.err code:%s\n", "disp", name, value,fdt_strerror(ret));
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
	bmp_header_t *bmp_header_info = NULL;
	char *bmp_buff = NULL;
	char *align_addr = NULL;
	u32 start_block = 0;
	u32 rblock = 0;
	char tmp[512];
	u32 patition_size = 0;
	int ret = -1;

	 /*read logo.bmp header info*/
	start_block = sunxi_partition_get_offset_byname((const char *)partition_name);
	if(!start_block) {
		pr_error("cant find part named %s\n", (char *)partition_name);
		return -1;
	}
	patition_size = sunxi_partition_get_size_byname((const char *)partition_name);
	ret = sunxi_flash_read(start_block, 1, tmp);
	debug("The flash read ret:%d\n",ret);
	bmp_header_info = (bmp_header_t *)tmp;

	/*checking the data whether if a BMP file*/
	if(bmp_header_info->signature[0] != 'B' || bmp_header_info->signature[1] != 'M')
	{
		pr_error("the data in bootlogo partition was not a BMP file,can't not show!\n");
		return -1;

	}

	/*malloc some memroy for bmp buff*/
	if(patition_size > ((bmp_header_info->file_size / 512) + 1)){
	    rblock = (bmp_header_info->file_size / 512) + 1;
	}else{
	    rblock = patition_size;
	}

	bmp_buff = malloc((bmp_header_info->file_size + 512 + 0x1000));
	if(NULL == bmp_buff){
	    pr_error("bmp buffer malloc error!\n");
	    return -1;
	}
	    /*4K aligned*/
	align_addr = (char *)((unsigned int)bmp_buff +
			(0x1000U - (0xfffu & (unsigned int)bmp_buff)));

	/*read logo.bmp all info*/
	ret = sunxi_flash_read(start_block, rblock, align_addr);
	board_display_update_para_for_kernel("fb_base", (uint)align_addr);
	return 0;

}
#else
int read_bmp_to_kernel(char *partition_name)
{
	printf("cancle bootlogo show! \n");
	return 0;
}
#endif
#endif

