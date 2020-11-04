/*
 * (C) Copyright 2007-2019
 * Allwinner Technology Co., Ltd. <liuli.allwinnertech.com>
 * <Liuli@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 */
#include <common.h>
#include <malloc.h>
#include <sys_config.h>
#include <sunxi_eink.h>
#include <bmp_layout.h>
#include <fdt_support.h>
#include <sunxi_bmp.h>

#include "../disp2/eink200/include/eink_sys_source.h"

DECLARE_GLOBAL_DATA_PTR;

extern long eink_ioctl(struct file *file, unsigned int cmd, void *arg);

/*
 * Subroutine:  bmp_display
 *
 * Description: Display bmp file located in memory
 *
 * Inputs:	addr		address of the bmp file
 *
 * Return:      None
 *
 */
int sunxi_bmp_decode(unsigned long addr, sunxi_bmp_store_t *bmp_info)
{
	char *tmp_buffer;
	char *bmp_data;
	int zero_num = 0;
	struct bmp_image *bmp = (struct bmp_image *)addr;
	int x, y, bmp_bpix;
	int tmp;

	if ((bmp->header.signature[0] != 'B') ||
	    (bmp->header.signature[1] != 'M')) {
		printf("this is not a bmp picture\n");

		return -1;
	}
	debug("bmp dectece\n");

	bmp_bpix = bmp->header.bit_count / 8;
	if ((bmp_bpix != 3) && (bmp_bpix != 4)) {
		printf("no support bmp picture without bpix 24 or 32\n");

		return -1;
	}
	if (bmp_bpix == 3) {
		zero_num = (4 - ((3 * bmp->header.width) % 4)) & 3;
	}
	debug("bmp bitcount %d\n", bmp->header.bit_count);
	x = bmp->header.width;
	y = (bmp->header.height & 0x80000000) ? (-bmp->header.height)
					      : (bmp->header.height);
	debug("bmp x = %x, bmp y = %x\n", x, y);

	tmp = bmp->header.height;
	if (0 == (bmp->header.height & 0x80000000))
		bmp->header.height = (-bmp->header.height);
	memcpy(bmp_info->buffer, bmp, sizeof(struct bmp_header));
	bmp_info->buffer += sizeof(struct bmp_header);
	bmp->header.height = tmp;

	tmp_buffer = (char *)bmp_info->buffer;
	bmp_data = (char *)(addr + bmp->header.data_offset);
	if (bmp->header.height & 0x80000000) {
		if (zero_num == 0) {
			memcpy(tmp_buffer, bmp_data, x * y * bmp_bpix);
		} else {
			int i, line_bytes, real_line_byte;
			char *src;

			line_bytes = (x * bmp_bpix) + zero_num;
			real_line_byte = x * bmp_bpix;
			for (i = 0; i < y; i++) {
				src = bmp_data + i * line_bytes;
				memcpy(tmp_buffer, src, real_line_byte);
				tmp_buffer += real_line_byte;
			}
		}
	} else {
		uint i, line_bytes, real_line_byte;
		char *src;

		line_bytes = (x * bmp_bpix) + zero_num;
		real_line_byte = x * bmp_bpix;
		for (i = 0; i < y; i++) {
			src = bmp_data + (y - i - 1) * line_bytes;
			memcpy(tmp_buffer, src, real_line_byte);
			tmp_buffer += real_line_byte;
		}
	}
	bmp_info->x = x;
	bmp_info->y = y;
	bmp_info->bit = bmp->header.bit_count;
	// flush_cache((uint)bmp_info->buffer, x * y * bmp_bpix);
	flush_cache((uint)bmp_info->buffer - sizeof(struct bmp_header),
		    x * y * bmp_bpix + sizeof(struct bmp_header));

	return 0;
}

int bmp_buffer_change2Gray(sunxi_bmp_store_t *bmp_info, char *dst_buf)
{
	int ret = 1;

	if (bmp_info->bit == 24) {
		int i, j;
		char *tmp;

		tmp = bmp_info->buffer;
		for (i = 0; i < bmp_info->y; i++) {
			for (j = 0; j < bmp_info->x; j++) {
				*dst_buf = (unsigned char)(0.299 * tmp[0] + 0.587 * tmp[1] +  0.114 * tmp[2]);  /* set Y */
				dst_buf++;
				tmp += 3;
			}
		}
	} else if (bmp_info->bit == 32) {
		int i, j;
		char *tmp;

		tmp = bmp_info->buffer;
		for (i = 0; i < bmp_info->y; i++) {
			for (j = 0; j < bmp_info->x; j++) {
				*dst_buf = (unsigned char)(0.299 * tmp[0] + 0.587 * tmp[1] +  0.114 * tmp[2]);    /* set Y */
				dst_buf++;
				tmp += 4;
			}
		}
	} else
		ret = -1;

	return ret;
}

int sunxi_eink_get_bmp_buffer(char *name, char *bmp_gray_buf)
{
	int ret;
	int partno = -1;
	sunxi_bmp_store_t bmp_info;
	char  bmp_addr[32] = {0};
	char  bmp_name[32];
	char *bmp_argv[6] = {"fatload", "sunxi_flash", "0:0", "00000000", bmp_name, NULL};

	partno = sunxi_partition_get_partno_byname("bootloader");
	if (partno < 0) {
		partno = sunxi_partition_get_partno_byname("boot-resource");
		if (partno < 0) {
			printf("[%s]:Get bootloader or boot-resource partition number fail!\n", __func__);
		} else
			//sprintf(bmp_argv[2], "%x:0", partno);
			sprintf(bmp_argv[2], "0:%x", partno);

		if (!bmp_gray_buf) {
			printf("sunxi_Eink_Get_bmp_buffer: bmp_gray_buffor is null for %s\n", name);
			return -1;
		}
	}
	EINK_INFO_MSG("partno = %d\n", partno);

	sprintf(bmp_addr, "%lx", (ulong)bmp_gray_buf);
	bmp_argv[3] = bmp_addr;

	memset(bmp_name, 0, 32);
	strcpy(bmp_name, name);
	if (do_fat_fsload(0, 0, 5, bmp_argv)) {
		printf("[%s]:error : unable to open logo file %s\n", __func__, bmp_argv[4]);
		return -1;
	}
	bmp_info.buffer = (void *)CONFIG_SYS_SDRAM_BASE;
	if (sunxi_bmp_decode((ulong)bmp_gray_buf, &bmp_info)) {
		debug("decode bmp fail\n");
		return -1;
	}

	ret = bmp_buffer_change2Gray(&bmp_info, bmp_gray_buf);

	return  ret;
}

int sunxi_eink_fresh_image(char *name, __u32 update_mode)

{
	unsigned long arg[4] = { 0 };
	uint cmd = 0;
	int ret;
	u32 width = 800;
	u32 height = 600;
	u32 buf_size = 0;
	char *bmp_buffer = NULL;
	char primary_key[25];
	s32 value = 0;

	struct eink_img cimage;

	sprintf(primary_key, "eink");

	ret = eink_sys_script_get_item(primary_key, "eink_width", &value, 1);
	if (ret == 1)
		width = value;

	ret = eink_sys_script_get_item(primary_key, "eink_height", &value, 1);
	if (ret == 1)
		height = value;

	buf_size = (width * height) << 2;

	bmp_buffer = (char *)malloc_aligned(buf_size, ARCH_DMA_MINALIGN);
	if (!bmp_buffer) {
		printf("fail to alloc memory for eink decode bmp.\n");
		return -1;
	}

	sunxi_eink_get_bmp_buffer(name, bmp_buffer);

	cimage.paddr = bmp_buffer;
	cimage.vaddr = bmp_buffer;
	cimage.force_fresh = 0;
	cimage.win_calc_en = 0;
	cimage.de_bypass_flag = 0;
	cimage.upd_all_en = 1;
	cimage.upd_mode = update_mode;
	cimage.size.height = height;
	cimage.size.width = width;
	cimage.size.align = 4;
	cimage.pitch = EINKALIGN(width, cimage.size.align);
	cimage.out_fmt = EINK_Y8;
	cimage.dither_mode = 0;
	cimage.upd_win.top = 0;
	cimage.upd_win.left = 0;
	cimage.upd_win.right = width - 1;
	cimage.upd_win.bottom = height - 1;

	arg[0] = (unsigned long)&cimage;
	arg[1] = 0;
	arg[2] = 0;
	arg[3] = 0;

	cmd = EINK_UPDATE_IMG;
	ret = eink_ioctl(NULL, cmd, (void *)arg);
	if (ret != 0) {
		printf("update eink image fail\n");
		return -1;
	}
	if (bmp_buffer) {
		free_aligned(bmp_buffer);
		bmp_buffer = NULL;
	}

	return 0;
}
