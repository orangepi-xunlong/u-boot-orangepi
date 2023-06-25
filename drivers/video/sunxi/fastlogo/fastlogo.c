/*
 * fastlogo.c
 *
 * Copyright (c) 2007-2021 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
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

#include <fastlogo.h>
#include "parse_reg.h"
#include "load_file.h"
#include "decode_pic.h"
#include <common.h>
#include <fdt_support.h>
#include <securestorage.h>
#include <stdlib.h>
#include <asm/global_data.h>
DECLARE_GLOBAL_DATA_PTR;

static struct fastlogo_t *g_fastlogo_inst;

static int __get_project_id(__u32 *project_id)
{
	char projectID_name[64];
	int projectID_len;

	if (sunxi_secure_object_read("mips_projectID", projectID_name, 64,
				     &projectID_len)) {
		*project_id = 1;
	} else {
		*project_id = strtoul(projectID_name, NULL, 16);
	}

	return 0;
}

static enum decode_type __file_type(const char path[])
{
	const char *result;
	int i, n;

	n = strlen(path);
	i = n - 1;
	while ((i >= 0) && (path[i] != '.') &&
	       (path[i] != '/') & (path[i] != '\\')) {
		i--;
	}
	if ((i > 0) && (path[i] == '.') && (path[i - 1] != '/') &&
	    (path[i - 1] != '\\')) {
		result = path + i;
	} else {
		result = path + n;
	}

	if (!strcmp(result, ".bmp")) {
		return BMP_DECODE_TYPE;
	}

	if (!strcmp(result, ".jpg") || !strcmp(result, ".jpeg")) {
		return JPEG_DECODE_TYPE;
	}
	return INVALID_DECODE_TYPE;
}


static int __save_fb_para(struct fastlogo_t *p_fastlogo)
{
	int ret = -1;
	char fb_paras[128];
	int fdt_node = -1;

	fdt_node = fdt_path_offset(working_fdt, "/soc/tvtop");
	if (fdt_node < 0) {
		pr_err("Get /soc/tvtop fail!\n");
		return fdt_node;
	}

	sprintf(fb_paras, "%p,%x,%x,%x,%x", p_fastlogo->p_decoded_pic->addr,
		p_fastlogo->p_decoded_pic->width,
		p_fastlogo->p_decoded_pic->height,
		p_fastlogo->p_decoded_pic->bpp,
		p_fastlogo->p_decoded_pic->stride);

	ret = fdt_setprop_string(working_fdt, fdt_node, "fastlogo", fb_paras);
	if (ret) {
		pr_err("sunxi_fdt_getprop_store_string :%s fail!\n", fb_paras);
	}
	return ret;
}


static int __reserve_memory(struct fastlogo_t *p_fastlogo)
{
	int ret = -1;

	ret = fdt_add_mem_rsv(
		working_fdt, (unsigned long)p_fastlogo->p_decoded_pic->addr,
		(unsigned long)p_fastlogo->p_decoded_pic->file_size);
	if (ret) {
		pr_err("Reserve memory:0x.8x size:%u fail:%d\n",
		       (unsigned long)p_fastlogo->p_decoded_pic->addr,
		       (unsigned long)p_fastlogo->p_decoded_pic->file_size,
		       ret);
	}
	ret = __save_fb_para(p_fastlogo);

	return ret;
}

static int __display_fastlogo(struct fastlogo_t *p_fastlogo)
{
	struct reg_data_info_t info;
	__u32 i = 0;
	int ret = -1;

	memset(&info, 0, sizeof(struct reg_data_info_t));

	for (i = 0; i < p_fastlogo->p_parse_reg->table_type_number; ++i) {
	    ret = p_fastlogo->p_parse_reg->get_reg(p_fastlogo->p_parse_reg,
					     p_fastlogo->project_id,
					     (enum logo_table_type)i, &info);
	    if (ret) {
		    pr_err("Get reg of type:%u fail!\n", i);
		    break;
	    }
	    p_fastlogo->p_parse_reg->write_reg(&info);
	    memset(&info, 0, sizeof(struct reg_data_info_t));
	}


	return ret;
}

static int __destroy_fastlogo(struct fastlogo_t *p_fastlogo)
{
	/*destry every thing except decoded pic*/
	if (p_fastlogo->p_reg_bin)
		p_fastlogo->p_reg_bin->unload_file(p_fastlogo->p_reg_bin);
	if (p_fastlogo->p_logo)
		p_fastlogo->p_logo->unload_file(p_fastlogo->p_logo);
	if (p_fastlogo->p_parse_reg)
		p_fastlogo->p_parse_reg->destroy_parse_reg_t(
		    p_fastlogo->p_parse_reg);
	if (p_fastlogo)
		free(p_fastlogo);
	return 0;
}

static int __get_framebuffer_info(struct fastlogo_t *p_fastlogo, __u32 *w,
				    __u32 *h, char **buf)
{
	if (p_fastlogo->p_decoded_pic) {
		*w = p_fastlogo->p_decoded_pic->width;
		*h = p_fastlogo->p_decoded_pic->height;
		*buf = (char *)p_fastlogo->p_decoded_pic->addr;
		return 0;
	}

	return -1;
}

struct fastlogo_t *create_fastlogo_inst(char *logoname, char *logo_partition,
					char *regbin_name,
					char *regbin_partition)
{
	struct fastlogo_t *p_fastlogo = NULL;
	int ret = -1;

	if (!logoname || !regbin_name || !regbin_partition || !logo_partition) {
		pr_err("NULL pointer(%p %p %p %p)\n", logoname, regbin_name,
		       regbin_partition, logo_partition);
		goto OUT;
	}

	p_fastlogo = (struct fastlogo_t *)malloc(sizeof(struct fastlogo_t));
	if (!p_fastlogo) {
		pr_err("Malloc fastlogo_t fail!\n");
		goto OUT;
	}
	memset(p_fastlogo, 0, sizeof(struct fastlogo_t));

	p_fastlogo->p_reg_bin = load_file(regbin_name, regbin_partition);
	if (!p_fastlogo->p_reg_bin) {
		pr_err("load file:%s from %s fail!\n", regbin_name,
		       regbin_partition);
		goto FREE;
	}

	p_fastlogo->p_logo = load_file(logoname, logo_partition);
	if (!p_fastlogo->p_logo) {
		pr_err("load file:%s from %s fail!\n", logoname,
		       logo_partition);
		goto FREE_REG_BIN;
	}

	p_fastlogo->p_parse_reg =
	    create_parse_reg_t(p_fastlogo->p_reg_bin->file_addr);
	if (!p_fastlogo->p_parse_reg) {
	    pr_err("Invalid logo regbin:%s\n", regbin_name);
	    goto FREE_LOGO;
	}

	ret = __get_project_id(&p_fastlogo->project_id);
	if (ret) {
		pr_err("get project id fail!\n");
		goto FREE_LOGO;
	}

	ret = p_fastlogo->p_parse_reg->is_project_valid(p_fastlogo->p_parse_reg,
							p_fastlogo->project_id);
	if (ret == false) {
		pr_err("Invalid project id:%u\n", p_fastlogo->project_id);
		goto FREE_LOGO;
	}

	struct osd_buf_info info;
	memset(&info, 0, sizeof(struct osd_buf_info));
	ret = p_fastlogo->p_parse_reg->get_osd_buf_info(
		p_fastlogo->p_parse_reg, p_fastlogo->project_id, &info);
	if (ret) {
		pr_err("Get osd buf info fail!\n");
		goto FREE_LOGO;
	}

	struct decode_out_arg decode_out;
	decode_out.stride = info.stride;
	decode_out.bpp = info.bpp;
	decode_out.width = info.width;
	decode_out.height = info.height;
	decode_out.type = __file_type(logoname);
	p_fastlogo->p_decoded_pic = decode_pic(p_fastlogo->p_logo, &decode_out);

	if (!p_fastlogo->p_decoded_pic) {
		pr_err("Decode picture fail\n");
		goto FREE_LOGO;
	}

	p_fastlogo->p_parse_reg->update_osd_buf_addr(
		p_fastlogo->p_parse_reg, p_fastlogo->project_id,
		(unsigned long)p_fastlogo->p_decoded_pic->addr);

	p_fastlogo->display_fastlogo = __display_fastlogo;
	p_fastlogo->destroy_fastlogo = __destroy_fastlogo;
	p_fastlogo->reserve_memory = __reserve_memory;
	p_fastlogo->get_framebuffer_info = __get_framebuffer_info;
	g_fastlogo_inst = p_fastlogo;

	goto OUT;

FREE_LOGO:
	p_fastlogo->p_logo->unload_file(p_fastlogo->p_logo);
FREE_REG_BIN:
	p_fastlogo->p_reg_bin->unload_file(p_fastlogo->p_reg_bin);
FREE:
	free(p_fastlogo);
	p_fastlogo = NULL;
OUT:
	return p_fastlogo;
}

struct fastlogo_t *get_fastlogo_inst(void)
{
	return g_fastlogo_inst;
}

int sunxi_bmp_display(char *name)
{
	struct fastlogo_t *p_fastlogo = NULL;
	int ret = -1;

	p_fastlogo = get_fastlogo_inst();
	if (!p_fastlogo) {
		p_fastlogo =
			create_fastlogo_inst(name, "bootloader",
					     "LogoRegData.bin",
					     "bootloader");
		if (p_fastlogo) {
			ret = p_fastlogo->display_fastlogo(p_fastlogo);
		} else {
			pr_err("create_fastlogo_inst fail!\n");
		}
	} else {
		if (p_fastlogo->p_logo) {
			p_fastlogo->p_logo->unload_file(p_fastlogo->p_logo);
			p_fastlogo->p_logo = load_file(name, "bootloader");
			if (!p_fastlogo->p_logo) {
				pr_err("load file:%s fail!\n", name);
				goto OUT;
			}
			memset(p_fastlogo->p_decoded_pic->addr, 0, p_fastlogo->p_decoded_pic->file_size);
			ret = decode_pic2(p_fastlogo->p_logo, p_fastlogo->p_decoded_pic, __file_type(name));
		}
	}

OUT:
	return ret;
}


//End of File
