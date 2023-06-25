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
#include <bmp_layout.h>
#include <fdt_support.h>
#include <sunxi_bmp.h>

#include <sunxi_eink.h>
#include "../disp2/eink200/include/eink_sys_source.h"

DECLARE_GLOBAL_DATA_PTR;

extern long eink_ioctl(struct file *file, unsigned int cmd, void *arg);
extern int do_fat_size(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

typedef struct rect {
	int left;
	int top;
	int right;
	int bottom;
} rect_t;

static struct eink_fb_info_t *g_fb_inst;


static int eink_bmp_decode(sunxi_bmp_store_t *bmp_info, struct raw_rgb_t *p_rgb)
{
	struct bmp_image *bmp = (struct bmp_image *)bmp_info->buffer;
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

	bmp_info->bit = bmp->header.bit_count;
	bmp_info->x = bmp->header.width;

	src_width = bmp->header.width;
	if (bmp->header.height & 0x80000000)
		src_height = -bmp->header.height;
	else
		src_height = bmp->header.height;
	bmp_info->y = src_height;

	if ((src_width > p_rgb->width) || (src_height > p_rgb->height)) {
		printf("no support big size bmp[%dx%d] on fb[%dx%d]\n",
		       src_width, src_height, p_rgb->width, p_rgb->height);
		goto OUT;
	}
	memset(p_rgb->addr, 0, p_rgb->file_size);

	src_cp_bytes = src_width * bmp->header.bit_count >> 3;
	src_stride = ((src_width * bmp->header.bit_count + 31) >> 5) << 2;
	src_addr = (char *)(bmp_info->buffer + bmp->header.data_offset);
	if (!(bmp->header.height & 0x80000000)) {
		src_addr += (src_stride * (src_height - 1));
		src_stride = -src_stride;
	}

	dst_crop.left = (p_rgb->width - src_width) >> 1;
	dst_crop.right = dst_crop.left + src_width;
	dst_crop.top = (p_rgb->height - src_height) >> 1;
	dst_crop.bottom = dst_crop.top + src_height;
	dst_addr_b = (char *)p_rgb->addr + p_rgb->stride * dst_crop.top +
		     (dst_crop.left * p_rgb->bpp >> 3);
	dst_addr_e = dst_addr_b + p_rgb->stride * src_height;

	if (p_rgb->bpp == bmp->header.bit_count) {
		for (; dst_addr_b != dst_addr_e; dst_addr_b += p_rgb->stride) {
			memcpy((void *)dst_addr_b, (void *)src_addr,
			       src_cp_bytes);
			src_addr += src_stride;
		}
	} else {
		if ((bmp->header.bit_count == 24) && (p_rgb->bpp == 32)) {
			for (; dst_addr_b != dst_addr_e;
			     dst_addr_b += p_rgb->stride) {
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
			ret = -1;
			printf("no support %dbit bmp picture on %dbit fb\n",
			       bmp->header.bit_count, p_rgb->bpp);
		}
	}
	ret = 0;
OUT:
	return ret;
}

static int __de_format_convert(struct eink_fb_info_t *p_info)
{

	unsigned long args[4] = { 0 };
	int ret = -1;


	args[0] = (unsigned long)&p_info->configs;
	args[1] = 1;
	args[2] = (unsigned long)&p_info->last_img;
	args[3] = (unsigned long)&p_info->cur_img;

	ret = eink_ioctl(NULL, EINK_WRITE_BACK_IMG, (void *)args);
	if (ret) {
		pr_err("EINK_WRITE_BACK_IMG: fail:%d!\n", ret);
	}

	return ret;
}

int sunxi_eink_get_bmp_buffer(char *name, sunxi_bmp_store_t *bmp_info)
{
	int ret = -1;
	int partno = -1;
	char bmp_addr[32] = {0}, part_info[16] = {0}, size[32] = {0};
	char *bmp_argv[6];
	unsigned long file_size = 0;

	if (!bmp_info) {
		pr_err("sunxi_Eink_Get_bmp_buffer: bmp_gray_buffor is null for %s\n", name);
		return -1;
	}

	partno = sunxi_partition_get_partno_byname("bootloader");
	if (partno < 0) {
		partno = sunxi_partition_get_partno_byname("boot-resource");
		if (partno < 0) {
			pr_err("[%s]:Get bootloader or boot-resource partition number fail!\n", __func__);
			return -1;
		}
	}


	snprintf(part_info, 16, "0:%x", partno);

	bmp_argv[0] = "fatsize";
	bmp_argv[1] = "sunxi_flash";
	bmp_argv[2] = part_info;
	bmp_argv[3] = name;
	bmp_argv[4] = NULL;
	bmp_argv[5] = NULL;
	if (!do_fat_size(0, 0, 4, bmp_argv)) {
		file_size = env_get_hex("filesize", 0);
		snprintf(size, 32,  "%lx", file_size);
		bmp_info->buffer = (void *)malloc_aligned(file_size, ARCH_DMA_MINALIGN);
		if (!bmp_info->buffer) {
			pr_err("Malloc %lu byte bmp buffer fail!\n", file_size);
			goto OUT;
		}
	} else {
		pr_err("Get %s's size fail!\n", name);
		goto OUT;
	}

	snprintf(bmp_addr, 32,  "%lx", (ulong)bmp_info->buffer);

	bmp_argv[0] = "fatload";
	bmp_argv[1] = "sunxi_flash";
	bmp_argv[2] = part_info;
	bmp_argv[3] = bmp_addr;
	bmp_argv[4] = name;
	bmp_argv[5] = size;

	ret = do_fat_fsload(0, 0, 5, bmp_argv);
	if (ret) {
		pr_err("[%s]:error : unable to open logo file %s\n", __func__, bmp_argv[4]);
	}

OUT:
	return  ret;
}

static int __eink_update(struct eink_fb_info_t *p_info)
{
	int ret = -1;
	unsigned long arg[4] = {0};
	static bool first_update;

	if (first_update == false) {
		first_update = true;
		p_info->cur_img.upd_win.left = 0;
		p_info->cur_img.upd_win.right = p_info->p_rgb->width - 1;
		p_info->cur_img.upd_win.top = 0;
		p_info->cur_img.upd_win.bottom = p_info->p_rgb->height - 1;
	}

	if (p_info->cur_img.upd_all_en) {
		p_info->cur_img.upd_win.left = 0;
		p_info->cur_img.upd_win.right = p_info->p_rgb->width - 1;
		p_info->cur_img.upd_win.top = 0;
		p_info->cur_img.upd_win.bottom = p_info->p_rgb->height - 1;
	}

	if (!p_info->cur_img.upd_win.right || !p_info->cur_img.upd_win.bottom)
		return 0;

	/*pr_err("upd win:%d x %d mode 0x%x\n",*/
	       /*p_info->cur_img.upd_win.right -*/
	       /*p_info->cur_img.upd_win.left,*/
	       /*p_info->cur_img.upd_win.bottom -*/
	       /*p_info->cur_img.upd_win.top,*/
	       /*p_info->cur_img.upd_mode);*/

	arg[0] = (unsigned long)&p_info->cur_img;

	ret = eink_ioctl(NULL, EINK_UPDATE_IMG, (void *)arg);
	if (ret != 0) {
		pr_err("update eink image fail\n");
	}

	return ret;
}

static void __eink_update_last_buf(struct eink_fb_info_t *p_info)
{
	char *temp_addr = NULL;
	//switch cur_img and last_img
	temp_addr = p_info->last_buf_addr;
	p_info->last_buf_addr = p_info->gray_buffer_addr;
	p_info->gray_buffer_addr = temp_addr;
	p_info->cur_img.vaddr = (void *)p_info->gray_buffer_addr;
	p_info->cur_img.paddr = (void *)p_info->gray_buffer_addr;
	p_info->last_img.vaddr = (void *)p_info->last_buf_addr;
	p_info->last_img.paddr = (void *)p_info->last_buf_addr;
}

static int __eink_display(struct eink_fb_info_t *p_info)
{
	int ret = -1;

	ret = p_info->de_format_convert(p_info);
	if (ret) {
		pr_err("de_format_convert fail!\n");
	} else {
		ret = p_info->eink_update(p_info);
		__eink_update_last_buf(p_info);
		p_info->wait_pipe_finish(p_info);
	}
	return ret;
}

int sunxi_bmp_display(char *name)

{
	int ret = -1;
	sunxi_bmp_store_t bmp_info;

	if (!g_fb_inst) {
		pr_err("eink_framebuffer_init not init yet!\n");
		goto OUT;
	}

	memset(&bmp_info, 0, sizeof(sunxi_bmp_store_t));


	ret = sunxi_eink_get_bmp_buffer(name, &bmp_info);
	if (ret) {
		goto OUT;
	}

	ret = eink_bmp_decode(&bmp_info, g_fb_inst->p_rgb);
	if (ret) {
		goto OUT;
	}

	if (bmp_info.buffer) {
		free_aligned(bmp_info.buffer);
	}

	g_fb_inst->update_all_en(g_fb_inst, 1);

	ret = g_fb_inst->de_format_convert(g_fb_inst);
	if (ret) {
		pr_err("de format conver fail!\n");
		goto OUT;
	}
	//wait last update finish
	g_fb_inst->wait_pipe_finish(g_fb_inst);

	ret = g_fb_inst->eink_update(g_fb_inst);
	__eink_update_last_buf(g_fb_inst);

OUT:
	return 0;
}

static int __setup_layer(struct eink_fb_info_t *p_fbinfo)
{
	int ret = -1;

	if (!p_fbinfo)
		return ret;

	p_fbinfo->configs.enable = 1;
	p_fbinfo->configs.info.mode = LAYER_MODE_BUFFER;
	p_fbinfo->configs.info.zorder = 0;
	p_fbinfo->configs.info.alpha_mode = 1;
	p_fbinfo->configs.info.alpha_value = 255;
	p_fbinfo->configs.info.screen_win.width = p_fbinfo->p_rgb->width;
	p_fbinfo->configs.info.screen_win.height = p_fbinfo->p_rgb->height;
	p_fbinfo->configs.info.fb.addr[0] = (unsigned long)(p_fbinfo->p_rgb->addr);
	p_fbinfo->configs.info.fb.size[0].width = p_fbinfo->p_rgb->width;
	p_fbinfo->configs.info.fb.size[0].height = p_fbinfo->p_rgb->height;

	if (p_fbinfo->p_rgb->bpp == 24)
		p_fbinfo->configs.info.fb.format = DISP_FORMAT_RGB_888;
	else if (p_fbinfo->p_rgb->bpp == 32)
		p_fbinfo->configs.info.fb.format = DISP_FORMAT_ARGB_8888;
	else
		p_fbinfo->configs.info.fb.format = DISP_FORMAT_RGB_888;

	p_fbinfo->configs.info.fb.crop.width = ((long long)p_fbinfo->p_rgb->width << 32);
	p_fbinfo->configs.info.fb.crop.height = ((long long)p_fbinfo->p_rgb->height << 32);

	p_fbinfo->cur_img.win_calc_en = 1;
	p_fbinfo->cur_img.upd_all_en = 1;
	p_fbinfo->cur_img.upd_mode = EINK_GC16_MODE;
	p_fbinfo->cur_img.upd_win.left = 0;
	p_fbinfo->cur_img.upd_win.right = p_fbinfo->p_rgb->width - 1;
	p_fbinfo->cur_img.upd_win.top = 0;
	p_fbinfo->cur_img.upd_win.bottom = p_fbinfo->p_rgb->height - 1;
	p_fbinfo->cur_img.size.width = p_fbinfo->p_rgb->width;
	p_fbinfo->cur_img.size.height = p_fbinfo->p_rgb->height;
	p_fbinfo->cur_img.size.align = 4;
	p_fbinfo->cur_img.pitch = EINKALIGN(p_fbinfo->p_rgb->width, p_fbinfo->cur_img.size.align);
	p_fbinfo->cur_img.out_fmt = EINK_Y8;
	p_fbinfo->cur_img.dither_mode = QUANTIZATION;
	p_fbinfo->cur_img.vaddr = (void *)p_fbinfo->gray_buffer_addr;
	p_fbinfo->cur_img.paddr = (void *)p_fbinfo->gray_buffer_addr;
	memcpy(&p_fbinfo->last_img, &p_fbinfo->cur_img, sizeof(struct eink_img));
	p_fbinfo->last_img.vaddr = (void *)p_fbinfo->last_buf_addr;
	p_fbinfo->last_img.paddr = (void *)p_fbinfo->last_buf_addr;

	return 0;
}
static int __set_update_mode(struct eink_fb_info_t *p_info, enum upd_mode update_mode)
{
	p_info->cur_img.upd_mode = update_mode;
	return 0;
}

static int __update_all_en(struct eink_fb_info_t *p_info, int en)
{
	p_info->cur_img.upd_all_en = en;
	return 0;
}

void __wait_pipe_finish(struct eink_fb_info_t *p_info)
{
	int cur_wait_time = 0;
	while (1) {
		if (eink_ioctl(NULL, EINK_WAIT_PIPE_FINISH, (void *)NULL)) {
			/*pr_err("wait %d ms ...\n", cur_wait_time);*/
			break;
		}
		udelay(2 * 1000);
		cur_wait_time += 2;
		if (cur_wait_time >= 600) {
			pr_err("Wait pipe finish time out\n");
			break;
		}
	}
}

int eink_framebuffer_init(void)
{
	int value = 0, ret = -1;
	struct eink_fb_info_t *p_fbinfo = NULL;

	if (g_fb_inst) {
		pr_err("eink_framebuffer_init alread init!\n");
		goto OUT;
	}

	p_fbinfo = malloc(sizeof(struct eink_fb_info_t));
	if (!p_fbinfo) {
		pr_err("Malloc eink_fb_info_t fail!\n");
		goto OUT;
	}

	memset(p_fbinfo, 0, sizeof(struct eink_fb_info_t));

	p_fbinfo->p_rgb = malloc(sizeof(struct raw_rgb_t));
	if (!p_fbinfo->p_rgb) {
		pr_err("malloc struct raw_rgb_t fail!\n");
		goto FREE_FBINFO;
	}

	ret = eink_sys_script_get_item("eink", "eink_width", &value, 1);
	if (ret == 1)
		p_fbinfo->p_rgb->width = value;

	ret = eink_sys_script_get_item("eink", "eink_height", &value, 1);
	if (ret == 1)
		p_fbinfo->p_rgb->height = value;

	if (!p_fbinfo->p_rgb->width || !p_fbinfo->p_rgb->height) {
		pr_err("zero eink width or height!\n");
		goto FREE_RGB;
	}

	p_fbinfo->p_rgb->bpp = 32;
	p_fbinfo->p_rgb->stride =
		p_fbinfo->p_rgb->width * p_fbinfo->p_rgb->bpp / 8;
	p_fbinfo->p_rgb->file_size =
		p_fbinfo->p_rgb->stride * p_fbinfo->p_rgb->height;
	p_fbinfo->p_rgb->addr = (void *)malloc_aligned(p_fbinfo->p_rgb->file_size,
						      ARCH_DMA_MINALIGN);
	if (!p_fbinfo->p_rgb->addr) {
		pr_err("Malloc raw rgb buffer fail!\n");
		goto FREE_RGB;
	}

	p_fbinfo->gray_buffer_addr = (char *)malloc_aligned(
		p_fbinfo->p_rgb->width * p_fbinfo->p_rgb->height,
		ARCH_DMA_MINALIGN);
	if (!p_fbinfo->gray_buffer_addr) {
		pr_err("Malloc gray buffer fail!\n");
		goto FREE_RGB_BUF;
	}
	p_fbinfo->last_buf_addr = (void *)malloc_aligned(p_fbinfo->p_rgb->width * p_fbinfo->p_rgb->height,
						      ARCH_DMA_MINALIGN);
	if (!p_fbinfo->last_buf_addr) {
		pr_err("Malloc last buffer addr fail!\n");
		goto FREE_GRAY;
	}

	__setup_layer(p_fbinfo);
	p_fbinfo->de_format_convert = __de_format_convert;
	p_fbinfo->eink_update = __eink_update;
	p_fbinfo->set_update_mode = __set_update_mode;
	p_fbinfo->update_all_en = __update_all_en;
	p_fbinfo->eink_display = __eink_display;
	p_fbinfo->wait_pipe_finish = __wait_pipe_finish;
	g_fb_inst = p_fbinfo;
	ret = 0;

	return ret;

FREE_GRAY:
	if (p_fbinfo->gray_buffer_addr)
		free(p_fbinfo->gray_buffer_addr);
FREE_RGB_BUF:
	if (p_fbinfo->p_rgb->addr)
		free(p_fbinfo->p_rgb->addr);
FREE_RGB:
	if (p_fbinfo->p_rgb)
		free(p_fbinfo->p_rgb);
FREE_FBINFO:
	if (p_fbinfo) {
		free(p_fbinfo);
	}
OUT:
	return ret;
}

struct eink_fb_info_t *eink_get_fb_inst(void)
{
	return g_fb_inst;
}

int eink_framebuffer_exit(void)
{
	if (g_fb_inst) {
		if (g_fb_inst->p_rgb) {
			if (g_fb_inst->p_rgb->addr) {
				free_aligned(g_fb_inst->p_rgb->addr);
			}
			free(g_fb_inst->p_rgb);
		}
		if (g_fb_inst->gray_buffer_addr)
			free_aligned(g_fb_inst->gray_buffer_addr);
		if (g_fb_inst->last_buf_addr)
			free_aligned(g_fb_inst->last_buf_addr);
		free(g_fb_inst);
		g_fb_inst = NULL;
		return 0;
	}
	return 1;
}
static int do_sunxi_bmp_display(cmd_tbl_t *cmdtp, int flag, int argc,
				char *const argv[])
{
	char filename[32] = {0};
	int ret = -1;

	if (argc == 2) {
		snprintf(filename, 32, "%s", argv[1]);
		ret = sunxi_bmp_display(filename);
	} else {
		return cmd_usage(cmdtp);
	}

	return ret;
}


U_BOOT_CMD(
	sunxi_bmp_show,	2,	1,	do_sunxi_bmp_display,
	"manipulate BMP image data",
	"sunxi_bmp_show name\n"
	"parameters 1 : bmp file name\n"
	"example: sunxi_bmp_show bat/bempty.bmp\n"
);
