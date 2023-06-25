/*
 * decode_pic.c
 *
 * Copyright (C) 2021 tracyone
 *
 * Contacts: tracyone <tracyone@live.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "decode_pic.h"
#include <tinyjpeg.h>
#include <common.h>
#include <bmp_layout.h>
#include <malloc.h>
#include <sys_partition.h>

typedef struct rect {
	int left;
	int top;
	int right;
	int bottom;
} rect_t;

#if defined(FASTLOGO_DEBUG) && defined(CONFIG_FAT_WRITE)
extern int do_fat_fswrite(cmd_tbl_t *cmdtp, int flag, int argc,
			  char *const argv[]);
int save_file_to_local(struct raw_pic_t *p_raw)
{
	int partno = -1, ret = -1;
	char *argv[6], file_addr[32];
	char part_info[16] = { 0 }, size[32] = { 0 };
	char file_name[32] = { 0 };

	partno = sunxi_partition_get_partno_byname("bootloader");
	if (partno < 0) {
		pr_err("boot-resource is not found!\n");
		goto OUT;
	}
	snprintf(part_info, 16, "0:%x", partno);
	snprintf(file_name, 32, "rawfile_%ux%u.rgb", p_raw->width,
		 p_raw->height);
	sprintf(file_addr, "%lx", (unsigned long)p_raw->addr);
	snprintf(size, 16, "%lx", (unsigned long)p_raw->file_size);
	argv[0] = "fatwrite";
	argv[1] = "sunxi_flash";
	argv[2] = part_info;
	argv[3] = file_addr;
	argv[4] = file_name;
	argv[5] = size;
	if (do_fat_fswrite(0, 0, 6, argv)) {
		pr_err("do_fat_fswrite fail!\n");
	} else
		ret = 0;

OUT:
	return ret;
}
#endif

static int __print_info(struct raw_pic_t *p_raw)
{
	pr_warn("======== Picture info ==========\n");
	pr_warn("size:[%ux%u], stride:%u\n", p_raw->width, p_raw->height, p_raw->stride);
	pr_warn("bpp:%u\n", p_raw->bpp);
	pr_warn("buf:%p\n", p_raw->addr);
	return 0;
}

static int __free_raw_pic(struct raw_pic_t *p_raw)
{
	if (p_raw->addr)
		free(p_raw->addr);
	free(p_raw);
	return 0;
}

static int __bmp_decode(struct file_info_t *p_file, struct raw_pic_t *p_pic)
{
	struct bmp_image *bmp = (struct bmp_image *)p_file->file_addr;
	char *src_addr;
	int src_width, src_height, src_stride, src_cp_bytes, ret = -1;
	char *dst_addr_b, *dst_addr_e;
	rect_t dst_crop;

	if ((bmp->header.signature[0] != 'B') ||
	    (bmp->header.signature[1] != 'M')) {
		pr_err("this is not a bmp picture\n");
		goto OUT;
	}

	if (bmp->header.bit_count != 24 && bmp->header.bit_count != 32) {
	    pr_err("no support %d bit bmp\n", bmp->header.bit_count);
	    goto OUT;
	}

	src_width = bmp->header.width;
	if (bmp->header.height & 0x80000000)
		src_height = -bmp->header.height;
	else
		src_height = bmp->header.height;

	if ((src_width > p_pic->width) || (src_height > p_pic->height)) {
		printf("no support big size bmp[%dx%d] on fb[%dx%d]\n",
		       src_width, src_height, p_pic->width, p_pic->height);
		goto OUT;;
	}

	src_cp_bytes = src_width * bmp->header.bit_count >> 3;
	src_stride = ((src_width * bmp->header.bit_count + 31) >> 5) << 2;
	src_addr = (char *)(p_file->file_addr + bmp->header.data_offset);
	if (!(bmp->header.height & 0x80000000)) {
		src_addr += (src_stride * (src_height - 1));
		src_stride = -src_stride;
	}

	dst_crop.left = (p_pic->width - src_width) >> 1;
	dst_crop.right = dst_crop.left + src_width;
	dst_crop.top = (p_pic->height - src_height) >> 1;
	dst_crop.bottom = dst_crop.top + src_height;
	dst_addr_b = (char *)p_pic->addr + p_pic->stride * dst_crop.top +
		     (dst_crop.left * p_pic->bpp >> 3);
	dst_addr_e = dst_addr_b + p_pic->stride * src_height;

	if (p_pic->bpp == bmp->header.bit_count) {
		for (; dst_addr_b != dst_addr_e; dst_addr_b += p_pic->stride) {
			memcpy((void *)dst_addr_b, (void *)src_addr,
			       src_cp_bytes);
			src_addr += src_stride;
		}
	} else {
		if ((bmp->header.bit_count == 24) && (p_pic->bpp == 32)) {
			for (; dst_addr_b != dst_addr_e;
			     dst_addr_b += p_pic->stride) {
				int *d = (int *)dst_addr_b;
				char *c_b = src_addr;
				char *c_end = c_b + src_cp_bytes;

				for (; c_b < c_end;) {
					*d++ = 0xFF000000 |
					       ((*(c_b + 2)) << 16) |
					       ((*(c_b + 1)) << 8) | (*c_b);
					c_b += 3;
				}
				src_addr += src_stride;
			}
		} else {
			printf("no support %dbit bmp picture on %dbit fb\n",
			       bmp->header.bit_count, p_pic->bpp);
		}
	}
	ret = 0;
OUT:
	return ret;
}

static int __jpeg_decode(struct file_info_t *p_file, struct raw_pic_t *p_pic)
{
#if defined(CONFIG_SUNXI_FASTLOGO_JPEG)
	struct jdec_private *jdec;
	unsigned int width, height;
	int output_format = -1;

	jdec = tinyjpeg_init();
	if (jdec == NULL) {
		pr_err("tinyjpeg_init failed\n");
		return -1;
	}

	if (tinyjpeg_parse_header(jdec, p_file->file_addr,
				  (unsigned int)p_file->file_size) < 0) {
		pr_err("tinyjpeg_parse_header failed: %s\n",
		       tinyjpeg_get_errorstring(jdec));
		goto FREE;
	}
	/* Get the size of the image, request the same size fb */
	tinyjpeg_get_size(jdec, &width, &height);

	if (p_pic->width < width || p_pic->height < height) {
	    pr_err("bootlogo size [%ux%u] greater then [%ux%u]\n", width, height,
		   p_pic->width, p_pic->height);
	    goto FREE;
	}


	tinyjpeg_set_components(jdec, (unsigned char **)&(p_pic->addr), 1);

	if (32 == p_pic->bpp)
		output_format = TINYJPEG_FMT_BGRA32;
	else if (24 == p_pic->bpp)
		output_format = TINYJPEG_FMT_BGR24;

	if (tinyjpeg_decode(jdec, output_format) < 0) {
		printf("tinyjpeg_decode failed: %s\n",
		       tinyjpeg_get_errorstring(jdec));
		goto FREE;
	}

	return 0;
FREE:
	tinyjpeg_free(jdec);
#endif
	return -1;
}

int decode_pic2(struct file_info_t *p_in_file, struct raw_pic_t *p_pic,
		enum decode_type type)
{
	int ret = -1;

	if (!p_in_file || !p_pic || !p_pic->addr) {
		pr_err("NULL pointer!\n");
		return ret;
	}

	if (type == BMP_DECODE_TYPE) {
		ret = __bmp_decode(p_in_file, p_pic);
	} else if (type == JPEG_DECODE_TYPE) {
		ret = __jpeg_decode(p_in_file, p_pic);
	} else {
		memcpy(p_pic->addr, p_in_file->file_addr, p_in_file->file_size);
		ret = 0;
	}

	return ret;
}

struct raw_pic_t *decode_pic(struct file_info_t *p_in_file,
			     struct decode_out_arg *p_out_arg)
{
	struct raw_pic_t *p_pic = NULL;
	int ret = -1;

	if (!p_in_file || !p_out_arg) {
		pr_err("Null pointer\n");
		goto OUT;
	}

	p_pic = (struct raw_pic_t *)malloc(sizeof(struct raw_pic_t));
	if (!p_pic) {
		pr_err("NULL pointer(%p)\n", p_pic);
		goto OUT;
	}
	p_pic->print_info = __print_info;
	p_pic->free_raw_pic = __free_raw_pic;
	p_pic->width = p_out_arg->width;
	p_pic->height = p_out_arg->height;
	p_pic->bpp = p_out_arg->bpp;
	p_pic->stride = p_out_arg->stride;
	p_pic->file_size =
		ALIGN(p_pic->stride * p_pic->height, 4096);

	p_pic->addr = memalign(4096,
				p_pic->file_size);
	if (!p_pic->addr) {
		pr_err("Malloc pic addr fail!\n");
		goto FREE;
	}
	memset(p_pic->addr, 0, p_pic->file_size);

	ret = decode_pic2(p_in_file, p_pic, p_out_arg->type);
	if (ret)
		goto FREE_RAW;
#if defined(FASTLOGO_DEBUG) && defined(CONFIG_FAT_WRITE)
	save_file_to_local(p_pic);
#endif

OUT:
	return p_pic;
FREE_RAW:
	free(p_pic->addr);
FREE:
	free(p_pic);
	return NULL;

}

/*End of File*/
