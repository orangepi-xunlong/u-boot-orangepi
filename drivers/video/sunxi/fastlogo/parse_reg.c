/*
 * parse_reg.c
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

#include "parse_reg.h"

static bool __is_header_valid(struct parse_reg_t *p_bin)
{
	if (!p_bin || !p_bin->p_header) {
		parse_reg_wrn("Null pointer!");
		return false;
	}

	if (p_bin->p_header->magic_number[0] == 'l' &&
	    p_bin->p_header->magic_number[1] == 'o' &&
	    p_bin->p_header->magic_number[2] == 'g' &&
	    p_bin->p_header->magic_number[3] == 'o') {
		return true;
	}
	return false;
}

bool __is_project_valid(struct parse_reg_t *p_bin, __u32 project_id)
{
    int i = 0;

    if (!p_bin) {
	parse_reg_wrn("null pointer!\n");
	goto OUT;
    }

    for (i = 0; i < p_bin->project_number; ++i) {
	if (p_bin->p_project[i].project_id == project_id)
	    return true;
    }

OUT:
    return false;
}

static int __print_info(struct parse_reg_t *p_bin)
{
    unsigned int i = 0;

    if (!p_bin) {
	parse_reg_wrn("Null pointer!\n");
	return -1;
    }

    parse_reg_wrn("Project_number:%u\n", p_bin->project_number);
    parse_reg_wrn("Project config table size:%u\n",
		  p_bin->p_header->project_config_table_size);
    parse_reg_wrn("Logo table type number:%u\n", p_bin->table_type_number);
    parse_reg_wrn("Logo table size:%u\n", p_bin->p_header->logo_table_size);
    parse_reg_wrn("Logo data size:%u\n", p_bin->p_header->logo_data_size);

    for (i = 0; i < NUMBER_OF_LOGO_TABLE_TYPE; ++i) {
	    p_bin->print_logo_table_info(p_bin, i);
    }
    for (i = 0; i < p_bin->project_number; ++i) {
	    p_bin->print_project_info(p_bin, p_bin->p_project[i].project_id);
    }

    return 0;
}

static struct project_info_t *__get_project_info(struct parse_reg_t *p_bin, __u32 project_id)
{
	int i = 0;

	for (i = 0; i < p_bin->project_number; ++i) {
	    if (p_bin->p_project[i].project_id == project_id)
		    return  &p_bin->p_project[i];
	}

	return NULL;
}

static struct logo_table_t *__get_logo_table_info(struct parse_reg_t *p_bin,
						  enum logo_table_type table_type)
{
	int i = 0;

	if (!p_bin) {
		parse_reg_wrn("null pointer!\n");
		goto OUT;
	}

	for (i = 0; i < p_bin->table_type_number; ++i) {
		if (p_bin->p_table[i].table_type == table_type)
			return &p_bin->p_table[i];
	}

OUT:
	return NULL;
}

static int __print_reg(struct reg_data_info_t *p_reg_info)
{
	int i = 0;

	if (!p_reg_info || !p_reg_info->reg_number || !p_reg_info->p_info ||
	    !p_reg_info->p_reg_set) {
	    parse_reg_wrn("Invalid reg info!\n");
	    return -1;
	}

	parse_reg_wrn("Start print type:%d regsiter, number:%u idx:%u\n",
		      p_reg_info->type, p_reg_info->reg_number,
		      p_reg_info->p_info->reg_data_index);
	for (i = 0; i < p_reg_info->reg_number; ++i) {
	    parse_reg_wrn(
		"Register:0x%.8x, Setting:0x%.8x, mask:0x%.8x, type:0x%.8x\n",
		p_reg_info->p_reg_set[i].reg_addr,
		p_reg_info->p_reg_set[i].reg_value,
		p_reg_info->p_reg_set[i].reg_mask,
		p_reg_info->p_reg_set[i].reg_type);
	}
	parse_reg_wrn("\n\n");

	return 0;
}

static int __get_reg(struct parse_reg_t *p_bin, __u32 project_id,
	       enum logo_table_type table_type, struct reg_data_info_t *p_reg_info)
{
	struct project_info_t *project = NULL;
	struct logo_table_t *table = NULL;
	struct reg_data_t *reg_data = NULL;
	unsigned int data_size = 0;
	int ret = -1;

	project = p_bin->get_project_info(p_bin, project_id);
	if (!project) {
		parse_reg_wrn("Get project:%u info fail!\n", project_id);
		goto OUT;
	}

	table = p_bin->get_logo_table_info(p_bin, table_type);
	if (!table) {
		parse_reg_wrn("Get logo table:%d fail!\n", table_type);
		goto OUT;
	}

	reg_data = (struct reg_data_t *)((unsigned long)p_bin->p_table +
				    table->table_offset);

	if (table->table_size <= sizeof(struct reg_data_t)) {
		parse_reg_wrn("No reg of this type(%d) was found!\n", table_type);
		ret = 0;
		goto OUT;
	}

	for (data_size = 0; data_size < table->table_size;) {
	    if (reg_data->reg_data_index == project->reg_data_idx[table_type]) {
		p_reg_info->reg_number = reg_data->reg_data_size / sizeof(struct logo_reg_setting_t);
		p_reg_info->type = table_type;
		p_reg_info->p_reg_set = (struct logo_reg_setting_t *)((unsigned long)reg_data + sizeof(struct reg_data_t));
		p_reg_info->p_info = reg_data;
		return 0;
	    }
	    data_size += (reg_data->reg_data_size + sizeof(struct reg_data_t));
	    reg_data = (struct reg_data_t *)((unsigned long)reg_data + (sizeof(struct reg_data_t) + reg_data->reg_data_size));
	}

OUT:
	return ret;
}

int __print_logo_table_info(struct parse_reg_t *p_bin, enum logo_table_type table_type)
{
	struct logo_table_t *table = NULL;
	struct reg_data_t *reg_data = NULL;
	unsigned int data_size = 0;

	table = p_bin->get_logo_table_info(p_bin, table_type);
	if (!table) {
		parse_reg_wrn("Get logo table fail!\n");
		goto OUT;
	}

	parse_reg_wrn("type:%d, offset:%u size:%u\n", table->table_type, table->table_offset, table->table_size);
	reg_data = (struct reg_data_t *)((unsigned long)p_bin->p_table +
					 table->table_offset);
	for (data_size = 0; data_size < table->table_size;) {
		parse_reg_wrn("idx:%u data size:%u\n", reg_data->reg_data_index, reg_data->reg_data_size);
		data_size += (reg_data->reg_data_size + sizeof(struct reg_data_t));
		reg_data = (struct reg_data_t *)((unsigned long)reg_data + (sizeof(struct reg_data_t) + reg_data->reg_data_size));
	}
OUT:
	return -1;
}

int __write_reg(struct reg_data_info_t *p_reg_info)
{
	int i = 0;
	__u32 temp_val = 0;

	if (!p_reg_info || !p_reg_info->reg_number || !p_reg_info->p_info ||
	    !p_reg_info->p_reg_set) {
		parse_reg_wrn("Null pointer!\n");
		return -1;
	}

	for (i = 0; i < p_reg_info->reg_number; ++i) {
		if (p_reg_info->p_reg_set[i].reg_type < REG_TYPE_DELAY) {
			temp_val = parse_reg_readl(p_reg_info->p_reg_set[i].reg_addr);
			temp_val = (temp_val &
				    ~p_reg_info->p_reg_set[i].reg_mask) |
				   (p_reg_info->p_reg_set[i].reg_value &
				    p_reg_info->p_reg_set[i]
					    .reg_mask); // mask value
			parse_reg_writel(temp_val,
					 p_reg_info->p_reg_set[i].reg_addr);
		} else if (p_reg_info->p_reg_set[i].reg_type == REG_TYPE_DELAY) {
			parse_reg_delayus(p_reg_info->p_reg_set[i].reg_value);
		} else if (p_reg_info->p_reg_set[i].reg_type == REG_TYPE_TOGGLE) {
		}
	}

	return 0;
}
int __print_project_info(struct parse_reg_t *p_bin, __u32 project_id)
{
	struct project_info_t *p_proj = NULL;
	unsigned long i = 0;

	if (!p_bin) {
		parse_reg_wrn("Null pointer!\n");
		return -1;
	}

	p_proj = p_bin->get_project_info(p_bin, project_id);
	if (!p_proj) {
		parse_reg_wrn("Null project pointer!\n");
		return -2;
	}

	parse_reg_wrn("proejct %u\n", p_proj->project_id);

	for (i = 0; i < sizeof(p_proj->reg_data_idx) / sizeof(__u16); ++i) {
		parse_reg_wrn("type:%lu,index:%u\n", i, p_proj->reg_data_idx[i]);
	}

	return 0;
}

int __get_osd_buf_info(struct parse_reg_t *p_bin, __u32 project_id,
		       struct osd_buf_info *p_info)
{
	struct reg_data_info_t info;
	int ret = -1, i = 0;
	unsigned long osd_base_addr = 0;
#define CH1_ATTR_REG_OFFSET  0x140
#define CH1_SIZE_REG_OFFSET  0x150
#define CH1_STRIDE_REG_OFFSET  0x170

	if (!p_bin || !p_info) {
		parse_reg_wrn("Null pointer!\n");
		goto OUT;
	}

	ret = p_bin->get_reg(p_bin, project_id, LOGO_OSD_REG, &info);
	if (ret) {
		parse_reg_wrn("get LOGO_OSD_REG of proejct(%u) fail\n", project_id);
		goto OUT;
	}

	memset(p_info, 0, sizeof(struct osd_buf_info));

	osd_base_addr = info.p_reg_set[0].reg_addr;
	for (i = 0; i < info.reg_number; ++i) {
	    if (info.p_reg_set[i].reg_addr ==
		(osd_base_addr + CH1_ATTR_REG_OFFSET)) {
		    p_info->fmt = (info.p_reg_set[i].reg_value >> 8) & 0xff;
		    if (p_info->fmt == OSD_RGB_888)
			    p_info->bpp = 24;
		    else if (p_info->fmt == OSD_RGA_8888)
			    p_info->bpp = 32;
		    else {
			    parse_reg_wrn("Unknow fmt:%d\n", p_info->fmt);
		    }
	    }
	    if (info.p_reg_set[i].reg_addr ==
		(osd_base_addr + CH1_SIZE_REG_OFFSET)) {
		    p_info->height = ((info.p_reg_set[i].reg_value >> 16) &  0x1fff) + 1;
		    p_info->width = ((info.p_reg_set[i].reg_value) & 0x1fff) + 1;
	    }
	    if (info.p_reg_set[i].reg_addr ==
		(osd_base_addr + CH1_STRIDE_REG_OFFSET)) {
		p_info->stride = (info.p_reg_set[i].reg_value) & 0xffff;
	    }
	    if (p_info->fmt && p_info->width && p_info->height &&
		p_info->stride)
		break;
	}

	if (i < info.reg_number)
		ret = 0;

OUT:
	return ret;
}

int __update_osd_buf_addr(struct parse_reg_t *p_bin, __u32 project_id,
			  unsigned long addr)
{
	struct reg_data_info_t info;
	int ret = -1, i = 0;
	unsigned long osd_base_addr = 0;
#define CH1_ADDR_REG_OFFSET  0x178
#define CH1_HIGH_ADDR_REG_OFFSET  0x17c

	if (!p_bin) {
		parse_reg_wrn("Null pointer!\n");
		goto OUT;
	}
	ret = p_bin->get_reg(p_bin, project_id, LOGO_OSD_REG, &info);
	if (ret) {
		parse_reg_wrn("get LOGO_OSD_REG of proejct(%u) fail\n", project_id);
		goto OUT;
	}

	osd_base_addr = info.p_reg_set[0].reg_addr;

	for (i = 0; i < info.reg_number; ++i) {
	    if (info.p_reg_set[i].reg_addr ==
		(osd_base_addr + CH1_ADDR_REG_OFFSET)) {
		    info.p_reg_set[i].reg_value = addr;
		    break;
	    }
	}
	if (i < info.reg_number)
		ret = 0;

OUT:
	return ret;
}

static int __destroy_parse_reg_t(struct parse_reg_t *p_bin)
{
	if (p_bin) {
		parse_reg_free(p_bin);
	}

	return 0;
}

struct parse_reg_t *create_parse_reg_t(void *buf_addr)
{
	struct parse_reg_t *p_reg = NULL;

	if (!buf_addr) {
		parse_reg_wrn("Null pointer!");
		return p_reg;
	}

	p_reg = parse_reg_malloc(sizeof(struct parse_reg_t));
	if (!p_reg) {
		parse_reg_wrn("Malloc parse_reg_t fail!\n");
		return p_reg;
	}
	memset((void *)p_reg, 0, sizeof(struct parse_reg_t));
	p_reg->p_header	= (struct logo_head_t *)buf_addr;
	p_reg->is_header_valid = __is_header_valid;
	if (!__is_header_valid(p_reg)) {
		parse_reg_wrn("Not valid logo data bin!\n");
		parse_reg_free(p_reg);
		return NULL;
	}
	p_reg->p_project =
	    (struct project_info_t *)((unsigned long)p_reg->p_header +
				      sizeof(struct logo_head_t));
	p_reg->project_number = p_reg->p_header->project_config_table_size /
				sizeof(struct project_info_t);

	p_reg->p_table =
	    (struct logo_table_t *)((unsigned long)p_reg->p_project +
				    p_reg->p_header->project_config_table_size);
	p_reg->table_type_number =
	    p_reg->p_header->logo_table_size / sizeof(struct logo_table_t);

	p_reg->print_info = __print_info;
	p_reg->is_project_valid = __is_project_valid;
	p_reg->get_reg = __get_reg;
	p_reg->get_project_info = __get_project_info;
	p_reg->get_logo_table_info = __get_logo_table_info;
	p_reg->print_logo_table_info = __print_logo_table_info;
	p_reg->print_reg = __print_reg;
	p_reg->write_reg = __write_reg;
	p_reg->print_project_info = __print_project_info;
	p_reg->update_osd_buf_addr = __update_osd_buf_addr;
	p_reg->get_osd_buf_info = __get_osd_buf_info;
	p_reg->destroy_parse_reg_t = __destroy_parse_reg_t;
	return p_reg;
}


//End of File
