/*
 * ./cmd_sunxi_bmp.c
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*
 * BMP handling routines
 */

#include <common.h>
#include <bmp_layout.h>
#include <command.h>
#include <malloc.h>
#include <sunxi_bmp.h>
#include <sunxi_board.h>
#include <sunxi_advert.h>
#include <sys_partition.h>
#include <fdt_support.h>
#include <boot_gui.h>
#include <lzma/LzmaTools.h>

extern int sunxi_partition_get_partno_byname(const char *part_name);
static int sunxi_bmp_probe_info(unsigned long addr);
int sunxi_advert_display(char *fatname, char *filename);
int sunxi_advert_logo_load(char *fatname, char *filename);

DECLARE_GLOBAL_DATA_PTR;
/*
 * Allocate and decompress a BMP image using gunzip().
 *
 * Returns a pointer to the decompressed image data. Must be freed by
 * the caller after use.
 *
 * Returns NULL if decompression failed, or if the decompressed data
 * didn't contain a valid BMP signature.
 */

static int do_sunxi_bmp_info(cmd_tbl_t *cmdtp, int flag, int argc,
			     char *const argv[])
{
	uint addr;
	int ret = 0;

	if (argc == 2) {
		/* use argument only*/
		addr = simple_strtoul(argv[1], NULL, 16);
		debug("bmp addr=%x\n", addr);
	} else if (argc == 3) {
		char load_addr[16];
		char filename[32];
		char *const bmp_argv[6] = {"ext2load", "mmc", "0",
					   load_addr, filename,      NULL};

		addr = simple_strtoul(argv[1], NULL, 16);
		memcpy(load_addr, argv[1], 9);
		memset(filename, 0, 32);
		memcpy(filename, argv[2], strlen(argv[2]));

		if (do_ext2load(NULL, 0, 5, bmp_argv)) {
			printf("sunxi bmp info error : unable to open bmp file %s\n",
			       argv[2]);

			return cmd_usage(cmdtp);
		}
	} else {
		return cmd_usage(cmdtp);
	}

	ret = sunxi_bmp_probe_info(addr);
	return ret;
}

U_BOOT_CMD(
	sunxi_bmp_info,	3,	1,	do_sunxi_bmp_info,
	"manipulate BMP image data",
	"only one para : the address where the bmp stored\n"
);

static int do_sunxi_bmp_display(cmd_tbl_t *cmdtp, int flag, int argc,
				char *const argv[])
{
	uint addr;

	if (argc == 3) {
		char load_addr[16];
		char filename[32];
		char *const bmp_argv[6] = {"ext2load", "mmc", "0",
					   load_addr, filename,      NULL};

		addr = simple_strtoul(argv[1], NULL, 16);
		memcpy(load_addr, argv[1], 9);
		memset(filename, 0, 32);
		memcpy(filename, argv[2], strlen(argv[2]));

		if (do_ext2load(NULL, 0, 5, bmp_argv)) {
			printf("sunxi bmp info error : unable to open bmp file %s\n",
			       argv[2]);
			return cmd_usage(cmdtp);
		}
	} else {
		return cmd_usage(cmdtp);
	}
	return show_bmp_on_fb((char *)addr, FB_ID_0);
	printf("Decode bmp error\n");

	return -1;
}

U_BOOT_CMD(
	sunxi_bmp_show,	3,	1,	do_sunxi_bmp_display,
	"manipulate BMP image data",
	"sunxi_bmp_display addr [de addr]\n"
	"parameters 1 : the address where the bmp stored\n"
	"parameters 2 : bmp file name\n"
	"example: sunxi_bmp_show 40000000 /boot/boot.bmp\n"
);

int show_bmp_on_fb(char *bmp_head_addr, unsigned int fb_id)
{
#if defined(CONFIG_BOOT_GUI)
	struct bmp_image *bmp = (struct bmp_image *)bmp_head_addr;
	struct canvas *cv = NULL;
	char *src_addr;
	int src_width, src_height, src_stride, src_cp_bytes;
	char *dst_addr_b, *dst_addr_e;
	rect_t dst_crop;
	int need_set_bg = 0;

	cv = fb_lock(fb_id);
	if ((!cv) || (!cv->base)) {
		printf("cv=%p, base= %p\n", cv, (cv) ? cv->base : 0x0);
		goto err_out;
	}
	if ((bmp->header.signature[0] != 'B') ||
	    (bmp->header.signature[1] != 'M')) {
		printf("this is not a bmp picture\n");
		goto err_out;
	}
	if ((bmp->header.bit_count != 24) && (bmp->header.bit_count != 32)) {
		printf("no support %d bit bmp\n", bmp->header.bit_count);
		goto err_out;
	}

	src_width = bmp->header.width;
	if (bmp->header.height & 0x80000000)
		src_height = -bmp->header.height;
	else
		src_height = bmp->header.height;
	if ((src_width > cv->width) || (src_height > cv->height)) {
		printf("no support big size bmp[%dx%d] on fb[%dx%d]\n",
		       src_width, src_height, cv->width, cv->height);
		goto err_out;
	}

	src_cp_bytes = src_width * bmp->header.bit_count >> 3;
	src_stride = ((src_width * bmp->header.bit_count + 31) >> 5) << 2;
	src_addr = (char *)(bmp_head_addr + bmp->header.data_offset);
	if (!(bmp->header.height & 0x80000000)) {
		src_addr += (src_stride * (src_height - 1));
		src_stride = -src_stride;
	}

	dst_crop.left = (cv->width - src_width) >> 1;
	dst_crop.right = dst_crop.left + src_width;
	dst_crop.top = (cv->height - src_height) >> 1;
	dst_crop.bottom = dst_crop.top + src_height;
	dst_addr_b = (char *)cv->base + cv->stride * dst_crop.top +
		     (dst_crop.left * cv->bpp >> 3);
	dst_addr_e = dst_addr_b + cv->stride * src_height;

	need_set_bg = cv->set_interest_region(cv, &dst_crop, 1, NULL);
	if (need_set_bg != 0) {
		if (src_width != cv->width) {
			debug("memset full fb\n");
			memset((void *)(cv->base), 0, cv->stride * cv->height);
		} else if (dst_crop.top != 0) {
			debug("memset top fb\n");
			memset((void *)(cv->base), 0,
			       cv->stride * dst_crop.top);
		}
	}
	if (cv->bpp == bmp->header.bit_count) {
		for (; dst_addr_b != dst_addr_e; dst_addr_b += cv->stride) {
			memcpy((void *)dst_addr_b, (void *)src_addr,
			       src_cp_bytes);
			src_addr += src_stride;
		}
	} else {
		if ((bmp->header.bit_count == 24) && (cv->bpp == 32)) {
			for (; dst_addr_b != dst_addr_e;
			     dst_addr_b += cv->stride) {
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
			       bmp->header.bit_count, cv->bpp);
		}
	}
	if (need_set_bg != 0) {
		if ((cv->height != dst_crop.bottom) &&
		    (src_width == cv->width)) {
			debug("memset bottom fb\n");
			memset(
			    (void *)(cv->base + cv->stride * dst_crop.bottom),
			    0, cv->stride * (cv->height - dst_crop.bottom));
		}
	}

	if (cv->bpp == 32)
		fb_set_alpha_mode(fb_id, FB_GLOBAL_ALPHA_MODE, 0xFF);

	fb_unlock(fb_id, NULL, 1);
	save_disp_cmd();

	return 0;

err_out:
	if (cv)
		fb_unlock(fb_id, NULL, 0);
	return -1;
#else
	printf("Fail to show bmp! You need to enable CONFIG_BOOT_GUI\n");
	return -1;
#endif
}

int sunxi_bmp_display(char *name)
{
	int ret = 0;
	char *argv[6];
	char bmp_head[32];
	char bmp_name[32];
	char *bmp_head_addr = (char *)CONFIG_SYS_SDRAM_BASE;

	if (bmp_head_addr) {
		sprintf(bmp_head, "%lx", (ulong)bmp_head_addr);
	} else {
		pr_error("sunxi bmp: alloc buffer for %s fail\n", name);
		return -1;
	}
	strncpy(bmp_name, name, sizeof(bmp_name));
	tick_printf("bmp_name=%s\n", bmp_name);

	argv[0] = "ext2load";
	argv[1] = "mmc";
	argv[2] = "0";
	argv[3] = bmp_head;
	argv[4] = bmp_name;
	argv[5] = NULL;

	if (do_ext2load(NULL, 0, 5, argv)) {
		pr_error("sunxi bmp info error : unable to open logo file %s\n",
		       argv[4]);
		return -1;
	}

	ret = show_bmp_on_fb(bmp_head_addr, FB_ID_0);
	if (ret != 0)
		pr_error("show bmp on fb failed !%d\n", ret);

	return ret;
}

int sunxi_bmp_dipslay_screen(sunxi_bmp_store_t bmp_info)
{
	return show_bmp_on_fb(bmp_info.buffer, FB_ID_0);
}

/*
 * Subroutine:  bmp_info
 *
 * Description: Show information about bmp file in memory
 *
 * Inputs:	addr		address of the bmp file
 *
 * Return:      None
 *
 */
static int sunxi_bmp_probe_info(unsigned long addr)
{
	struct bmp_image *bmp = (struct bmp_image *)addr;

	if ((bmp->header.signature[0] != 'B') ||
	    (bmp->header.signature[1] != 'M')) {
		pr_error("this is not a bmp picture\n");

		return -1;
	}
	debug("bmp picture dectede\n");

	tick_printf("Image size    : %d x %d\n", bmp->header.width,
	       (bmp->header.height & 0x80000000) ? (-bmp->header.height)
						 : (bmp->header.height));
	tick_printf("Bits per pixel: %d\n", bmp->header.bit_count);

	return 0;
}

static int fat_read_file_ex(char *fatname, char *filename, char *addr)
{
	char file_name[32];
	char fat_name[32];
	char partition[32];
	int partition_num = -1;
	char *bmp_buff = NULL;
	char bmp_addr[32] = {0};

	memset(file_name, 0, 32);
	strcpy(file_name, filename);

	memset(fat_name, 0, 32);
	strcpy(fat_name, fatname);

	partition_num = sunxi_partition_get_partno_byname(fat_name);
	if (partition_num < 0) {
		pr_error("[boot disp] can not find the partition %s\n", fat_name);
		return -1;
	}
	sprintf(partition, "0:%x", partition_num);
	bmp_buff = addr;
	if (!bmp_buff) {
		pr_error("sunxi bmp: alloc buffer fail\n");
		return -1;
	}
	char *bmp_argv[6] = {"fatload",  "sunxi_flash", "0:0",
			     "00000000", file_name,     NULL};
	bmp_argv[2] = partition;
	sprintf(bmp_addr, "%lx", (ulong)bmp_buff);
	bmp_argv[3] = bmp_addr;
	if (do_fat_fsload(0, 0, 5, bmp_argv)) {
		pr_error("sunxi bmp info error : unable to open logo file %s\n",
		       bmp_argv[1]);
		return -1;
	}
	return 0;
}

static int sunxi_advert_verify_head(struct __advert_head *adv_head)
{
	char *addr = (char *)CONFIG_SYS_SDRAM_BASE;

	if ((0 > fat_read_file_ex("Reserve0", "advert.crc", addr))) {
		return -1;
	};

	memcpy((u32 *)adv_head, (u32 *)addr, sizeof(*adv_head));

	if (memcmp((char *)(adv_head->magic), ADVERT_MAGIC,
		   strlen(ADVERT_MAGIC))) {
		pr_error("advert magic not equal,%s\n",
		       (char *)(adv_head->magic));
		return -1;
	}

	if ((adv_head->length > SUNXI_DISPLAY_FRAME_BUFFER_SIZE) ||
	    (adv_head->length == 0)) {
		pr_error("advert length=%d to big or to short\n",
		       adv_head->length);
		return -1;
	}

	return 0;
}

static __s32 check_sum(void *mem_base, __u32 size, __u32 src_sum)
{
	__u32 *buf = (__u32 *)mem_base;
	__u32 count = 0;
	__u32 sum = 0;
	__u32 last = 0;
	__u32 curlen = 0;
	__s32 i = 0;

	count = size >> 2;
	do {
		sum += *buf++;
		sum += *buf++;
		sum += *buf++;
		sum += *buf++;
	} while ((count -= 4) > (4 - 1));
	for (i = 0; i < count; i++)
		sum += *buf++;

	curlen = size % 4;
	if ((size & 0x03) != 0) {
		memcpy(&last, mem_base + size - curlen, curlen);
		sum += last;
	}

	if (sum == src_sum) {
		return 0;
	} else {
		pr_error("err: sum=%x; src_sum=%x\n", sum, src_sum);
		return -1;
	}
}

int sunxi_advert_display(char *fatname, char *filename)
{
	struct __advert_head advert_head;

	if (sunxi_advert_verify_head(&advert_head) < 0)
		return -1;

	if ((0 > fat_read_file_ex("Reserve0", "advert.bmp",
				  (char *)CONFIG_SYS_SDRAM_BASE)) ||
	    (0 > check_sum((u32 *)CONFIG_SYS_SDRAM_BASE, advert_head.length,
			   advert_head.check_sum))) {
		return -1;
	}

	return show_bmp_on_fb((char *)CONFIG_SYS_SDRAM_BASE, FB_ID_0);
}

int sunxi_bmp_decode_from_compress(unsigned char *dst_buf,
				   unsigned char *src_buf)
{
 __maybe_unused	unsigned long dst_len, src_len;

	src_len = *(uint *)(src_buf - 16);
#if defined(CONFIG_LZMA)
	return lzmaBuffToBuffDecompress(dst_buf, (SizeT *)&dst_len,
					(void *)src_buf, (SizeT)src_len);
#else
	pr_err("lzma decompress not supported\n");
	return -1;
#endif
}


int do_sunxi_logo(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	return sunxi_bmp_display("/boot/boot.bmp");
}

U_BOOT_CMD(
	logo,	1,	0,	do_sunxi_logo,
	"show default logo",
	"no args\n"
);
