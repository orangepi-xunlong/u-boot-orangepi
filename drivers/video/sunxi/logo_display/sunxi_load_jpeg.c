/*
 * sunxi_load_jpeg/sunxi_load_jpeg.c
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
#include <common.h>
#include <malloc.h>
#include <sys_config.h>
#include <fdt_support.h>

#include <sys_partition.h>
#include <tinyjpeg.h>
#include <bmp_layout.h>
#include <boot_gui.h>
#include <bmp_layout.h>

struct boot_fb_private {
	char *base;
	int width;
	int height;
	int bpp;
	int stride;
	int id;
};

#ifdef CONFIG_SUNXI_SPINOR_JPEG
static struct boot_fb_private boot_fb;
#endif

#ifdef CONFIG_BOOT_GUI
static int show_bmp_on_fb(char *bmp_head_addr, unsigned int fb_id)
{
	struct bmp_image *bmp = (struct bmp_image *)bmp_head_addr;
	struct canvas *cv = NULL;
	char *src_addr;
	int src_width, src_height, src_stride, src_cp_bytes;
	char *dst_addr_b, *dst_addr_e;
	rect_t dst_crop;
	int need_set_bg = 0;

	cv = fb_lock(fb_id);
	if ((NULL == cv) || (NULL == cv->base)) {
		printf("cv=%p, base= %p\n", cv, (cv != NULL) ? cv->base : 0x0);
		goto err_out;
	}
	if ((bmp->header.signature[0] != 'B') ||
	    (bmp->header.signature[1] != 'M')) {
		printf("this is not a bmp picture\n");
		goto err_out;
	}
	if ((24 != bmp->header.bit_count) && (32 != bmp->header.bit_count)) {
		printf("no support %dbit bmp\n", bmp->header.bit_count);
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
	if (0 != need_set_bg) {
		if (src_width != cv->width) {
			debug("memset full fb\n");
			memset((void *)(cv->base), 0, cv->stride * cv->height);
		} else if (0 != dst_crop.top) {
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
		if ((24 == bmp->header.bit_count) && (32 == cv->bpp)) {
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
	if (0 != need_set_bg) {
		if ((cv->height != dst_crop.bottom) &&
		    (src_width == cv->width)) {
			debug("memset bottom fb\n");
			memset(
			    (void *)(cv->base + cv->stride * dst_crop.bottom),
			    0, cv->stride * (cv->height - dst_crop.bottom));
		}
	}

	if (32 == cv->bpp)
		fb_set_alpha_mode(fb_id, FB_GLOBAL_ALPHA_MODE, 0xFF);

	fb_unlock(fb_id, NULL, 1);
	save_disp_cmd();

	return 0;

err_out:
	if (NULL != cv)
		fb_unlock(fb_id, NULL, 0);
	return -1;
}
#endif

static int add_bmp_header(struct boot_fb_private *fb)
{
	struct bmp_header bmp_header;
	memset(&bmp_header, 0, sizeof(struct bmp_header));
	bmp_header.signature[0] = 'B';
	bmp_header.signature[1] = 'M';
	bmp_header.data_offset = sizeof(struct bmp_header);
	bmp_header.image_size = fb->stride * fb->height;
	bmp_header.file_size = bmp_header.image_size + sizeof(struct bmp_header);
	bmp_header.size = sizeof(struct bmp_header) - 14;
	bmp_header.width = fb->width;
	bmp_header.height = -fb->height;
	bmp_header.planes = 1;
	bmp_header.bit_count = fb->bpp;
	memcpy(fb->base, &bmp_header, sizeof(struct bmp_header));
	return 0;
}


static int read_jpeg(const char *filename, char *buf, unsigned int buf_size)
{
#ifdef CONFIG_CMD_FAT
	char *argv[6];
	char part_num[16] = {0};
	char len[16] = {0};
	char read_addr[32] = {0};
	char file_name[32] = {0};
	int partno = -1;

	partno = sunxi_partition_get_partno_byname("bootloader"); /*android*/
	if (partno < 0) {
		partno = sunxi_partition_get_partno_byname("boot-resource"); /*linux*/
		if (partno < 0) {
			printf("Get bootloader and boot-resource partition number fail!\n");
			return -1;
		}
	}

	snprintf(part_num, 16, "0:%x", partno);
	snprintf(len, 16, "%ld", (ulong)buf_size);
	snprintf(read_addr, 32, "%lx", (ulong)buf);
	strncpy(file_name, filename, 32);
	argv[0] = "fatload";
	argv[1] = "sunxi_flash";
	argv[2] = part_num;
	argv[3] = read_addr;
	argv[4] = file_name;
	argv[5] = len;
	if (!do_fat_fsload(0, 0, 5, argv))
		return env_get_hex("filesize", 0);
	else
		return 0;
#else
	int size;
	unsigned int start_block, nblock;
	sunxi_partition_get_info_byname(filename, &start_block, &nblock);
	size = sunxi_flash_read(start_block, nblock, (void *)buf);
	return size > 0 ? (size << 10) : 0;

#endif
}

static int get_disp_fdt_node(void)
{
	int fdt_node = -1;

	if (0 <= fdt_node)
		return fdt_node;
	/* notice: make sure we use the only one nodt "disp". */
	fdt_node = fdt_path_offset(working_fdt, "disp");
	assert(fdt_node >= 0);
	return fdt_node;
}

static void disp_getprop_by_name(int node, const char *name, uint32_t *value,
				 uint32_t defval)
{
	if (fdt_getprop_u32(working_fdt, node, name, value) < 0) {
		printf("fetch script data disp.%s fail. using defval=%d\n",
		       name, defval);
		*value = defval;
	}
}

static int malloc_buf(char **buff, int size)
{
	int ret;
	*buff = memalign(CONFIG_SYS_CACHELINE_SIZE, (size + 512 + 0x1000));
	if (NULL == buff) {
		printf("bmp buffer malloc error!\n");
		return -1;
	}
	ret = 3;
	return ret;
}

static int request_fb(struct boot_fb_private *fb, unsigned int width,
		      unsigned int height)
{
	int format = 0;
	int fbsize = 0;

	disp_getprop_by_name(get_disp_fdt_node(), "fb0_format",
			     (uint32_t *)(&format), 0);
	if (8 == format) /* DISP_FORMAT_RGB_888 = 8; */
		fb->bpp = 24;
	else
		fb->bpp = 32;
	fb->width = width;
	fb->height = height;
	fb->stride = width * fb->bpp >> 3;
	fbsize = fb->stride * fb->height + sizeof(struct bmp_header); /*add bmp header len*/
	fb->id = 0;
	malloc_buf(&fb->base, fbsize);

	if (NULL != fb->base)
		memset((void *)(fb->base), 0x0, fbsize);

	return 0;
}

static int release_fb(struct boot_fb_private *fb)
{
	flush_cache((uint)fb->base, fb->stride * fb->height);
	return 0;
}

#ifdef CONFIG_SUNXI_SPINOR_JPEG
void save_jpg_logo_to_kernel(void)
{
	struct boot_fb_private *fb = &boot_fb;
	int node = get_disp_fdt_node();
	//int ret = 0;
	char fb_paras[128];
	sprintf(fb_paras, "%p,%x,%x,%x,%x",
		fb->base, fb->width, fb->height, fb->bpp, fb->stride);
	fdt_setprop_string(working_fdt, node, "boot_fb0", fb_paras);
	//ret = fdt_setprop_string(working_fdt, node, "boot_fb0", fb_paras);
	//printf("fdt_setprop_string disp.boot_fb0(%s). ret-code:%s\n",fb_paras, fdt_strerror(ret));
}
#endif

int sunxi_jpeg_display(const char *filename)
{
	int length_of_file;
	unsigned int width, height;
	unsigned char *buf = (unsigned char *)CONFIG_SYS_SDRAM_BASE;
	;
	struct jdec_private *jdec;
	struct boot_fb_private fb;
	int output_format = -1;

	/* Load the Jpeg into memory */
	length_of_file =
	    read_jpeg(filename, (char *)buf, SUNXI_DISPLAY_FRAME_BUFFER_SIZE);
	if (0 >= length_of_file) {
		printf("%s:%d length_of_file invalid:%d\n", __func__, __LINE__,
		       length_of_file);
		return -1;
	}

	/* tinyjpeg: init and parse header */
	jdec = tinyjpeg_init();
	if (jdec == NULL) {
		printf("tinyjpeg_init failed\n");
		return -1;
	}
	if (tinyjpeg_parse_header(jdec, buf, (unsigned int)length_of_file) <
	    0) {
		printf("tinyjpeg_parse_header failed: %s\n",
		       tinyjpeg_get_errorstring(jdec));
		return -1;
	}

	/* Get the size of the image, request the same size fb + bmp header*/
	tinyjpeg_get_size(jdec, &width, &height);
	memset((void *)&fb, 0x0, sizeof(fb));
	request_fb(&fb, width, height);
	if (NULL == fb.base) {
		printf("fb.base is null !!!");
		return -1;
	}
	char *tmp = fb.base + sizeof(struct bmp_header);
	tinyjpeg_set_components(jdec, (unsigned char **)&(tmp), 1);

	if (32 == fb.bpp)
		output_format = TINYJPEG_FMT_BGRA32;
	else if (24 == fb.bpp)
		output_format = TINYJPEG_FMT_BGR24;
	if (tinyjpeg_decode(jdec, output_format) < 0) {
		printf("tinyjpeg_decode failed: %s\n",
		       tinyjpeg_get_errorstring(jdec));
		return -1;
	}

#ifdef CONFIG_BOOT_GUI
	add_bmp_header(&fb);
	return show_bmp_on_fb(fb.base, FB_ID_0);
#elif CONFIG_SUNXI_SPINOR_JPEG
	char disp_reserve[80];
	snprintf(disp_reserve, 80, "%d,0x%x",
		(fb.stride * fb.height + sizeof(struct bmp_header)), (uint)tmp);
	env_set("disp_reserve", disp_reserve);
	fb.base = tmp ;
	add_bmp_header(&fb);

	memcpy(&boot_fb, &fb, sizeof(fb));
#endif

	release_fb(&fb);
	free(jdec);
	/*display*/
	return 0;
}

int do_sunxi_jpg_show(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	char filename[40] = {0};

	if (argc != 2) {
		return cmd_usage(cmdtp);
	}
	memcpy(filename, argv[1], strlen(argv[1]));
	return sunxi_jpeg_display(filename);
}

U_BOOT_CMD(
	sunxi_jpg_show,	2,	0,	do_sunxi_jpg_show,
	"show jpeg picture",
	"parameters 1 : jpg file name\n"
	"example: sunxi_jpg_show  bat/bootlogo.jpg\n"
);
