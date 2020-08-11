/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 * SPDX-License-Identifier:	GPL-2.0+ BSD-2-Clause
 */
#include <common.h>
#include "libfdt.h"
#include "fdt_support.h"
#include "fdt_acc.h"
#include "common.h"
//返回对应的name的head node的偏移量
//非0表示找到，0值表示找不到
DECLARE_GLOBAL_DATA_PTR;

int fdtfast_search_head(char *head_base, char *dtb_base, char *head_buff, char *name)
{
	struct head_node *head = NULL, *next_head;
	char *tmp_name = name, *next_end_char;
	uint  full_string_sum = 0, full_string_len = 0;
	uint  at_flag = 0;
	int i;

	//取出首节点
	head = (struct head_node *)head_buff;

	//跳过name的/
	while(*tmp_name == '/')
		tmp_name ++;
	//找到下一个/，或者空字符
	next_end_char = tmp_name;
	while(1) {
		if (*next_end_char == '/')
			break;
		if (*next_end_char == '\0')
			break;
		if (*next_end_char == '@')
			at_flag = 1;

		full_string_sum += *next_end_char;
		full_string_len ++;

		next_end_char   ++;
	}
//printf("%s %d\n", __FILE__, __LINE__);
	//printf("string_sum=%d\n", full_string_sum);
	for(i=0;i<head->head_count;i++) {

		next_head = (struct head_node *)(head->head_offset + head_base) + i;

		//printf("next_head->name_offset=0x%x\n", next_head->name_offset);
		//比较内存内容
		//printf("offset=0x%x\n", (uint)((unsigned long)next_head - (unsigned long)head_base));
		//printf("i=%d\n", i);
		if (!memcmp(tmp_name, next_head->name_offset + dtb_base, full_string_len)) {
			//完整字符串相同，检查节点是不是重复出现
			if ((next_head->repeate_count) && (at_flag))
				continue;		//节点名字重复出现，丢弃
			//节点没有重复出现，继续处理
			if (*next_end_char != '\0')
				return fdtfast_search_head(head_base, dtb_base, (char *)next_head, next_end_char);
			else
				return (int)((unsigned long)next_head - (unsigned long)head_base);
		}
	}

	return 0;

}


struct fdt_property *fdtfast_search_prop(char *dtbfast_buffer, char *fdt_buffer, uint32_t offset, char *name)
{
	struct fdt_header *fdt = (struct fdt_header *)fdt_buffer;
	struct prop_node *node;
	struct head_node *head;

	char *tmp_name = name;
	uint32_t  string_sum = 0, i;
	char *fdt_string_base, *fdt_string_off;

	head = (struct head_node *)(dtbfast_buffer + offset + sizeof(struct dtbfast_header));
	while(*tmp_name)
		string_sum += *tmp_name++;

	fdt_string_base = fdt_buffer + cpu_to_be32(fdt->off_dt_strings);
	for(i=0;i<head->data_count;i++) {
		node = (struct prop_node *)(dtbfast_buffer + head->data_offset + sizeof(struct dtbfast_header)) + i;
		if (string_sum == node->name_sum) {
			fdt_string_off = fdt_string_base + node->name_offset;
			if (!strcmp(name, fdt_string_off)) {
				return (struct fdt_property *)(fdt_buffer + cpu_to_be32(fdt->off_dt_struct) + node->offset);
			}
		}
	}

	return NULL;
}

int fdtfast_path_offset(const void *fdt, const char *path)
{
	char *dtbfast_buffer = gd->fdtfast_blob;
	char *dtb_buffer = (char *)fdt;
	uint32_t base_header_size = sizeof(struct dtbfast_header);
	struct fdt_header *fdt_head;
	uint32_t offset;

	fdt_head = (struct fdt_header *)dtb_buffer;
	if (path == NULL)
		return 0;

//	printf("path=%s\n", path);
	//如果不是以"/"开始，表示这是一个aliase，需要从aliase节点中找
	if (path[0] != '/') {
		struct fdt_property *prop_node;

		offset = fdtfast_search_head(dtbfast_buffer + base_header_size,
										dtb_buffer + cpu_to_be32(fdt_head->off_dt_struct),
										dtbfast_buffer + base_header_size, "/aliases");
//printf("%s %d\n", __FILE__, __LINE__);
		if (offset == 0) {
			printf("dtbfast path offset err: cant find aliases\n");

			return 0;
		}

		prop_node = fdtfast_search_prop(dtbfast_buffer, dtb_buffer, offset, (char *)path);
//printf("%s %d\n", __FILE__, __LINE__);
		return fdtfast_search_head(dtbfast_buffer + base_header_size,
										dtb_buffer + cpu_to_be32(fdt_head->off_dt_struct),
										dtbfast_buffer + base_header_size,
										prop_node->data);
	}
	else
		return fdtfast_search_head(dtbfast_buffer + base_header_size, dtb_buffer +
										cpu_to_be32(fdt_head->off_dt_struct),
										dtbfast_buffer + base_header_size, (char *)path);
}


int fdtfast_setprop_string(const void *fdt, uint32_t offset, const char *name, const void *val)
{
	struct fdt_property *prop_node;
	char *dtbfast_buffer = gd->fdtfast_blob;
	char  *data, *src;
	int   i=0;

//printf("%s %d\n", __FILE__, __LINE__);
	prop_node = fdtfast_search_prop(dtbfast_buffer, (char *)fdt, offset, (char *)name);
	if (prop_node == NULL) {
		printf("can not find the string named %s\n", name);

		return -1;
	}

	if (cpu_to_be32(prop_node->len) < 4) {
		printf("the name len is too short\n");

		return -1;
	}
	data = (char *)prop_node->data;
	src  = (char *)val;

	while(src[i]) {
		data[i] = src[i];
		i ++;
	}
	while(i < cpu_to_be32(prop_node->len))
		data[i++] = '\0';


	return 0;
}


int fdtfast_set_node_status(void *fdt, int nodeoffset, enum fdt_status status, unsigned int error_code)
{
	//char buf[16];
	int ret = 0;

	if (nodeoffset < 0)
		return nodeoffset;
//printf("%s %d\n", __FILE__, __LINE__);
	switch (status) {
	case FDT_STATUS_OKAY:
		ret = fdtfast_setprop_string(fdt, nodeoffset, "status", "okay");
		break;
	case FDT_STATUS_DISABLED:
		ret = fdtfast_setprop_string(fdt, nodeoffset, "status", "bad");
		break;
	case FDT_STATUS_FAIL:
		ret = fdtfast_setprop_string(fdt, nodeoffset, "status", "fail");
		break;
	//case FDT_STATUS_FAIL_ERROR_CODE:
	//	sprintf(buf, "fail-%d", error_code);
	//	ret = fdtfast_setprop_string(fdt, nodeoffset, "status", buf);
	//	break;
	default:
		printf("Invalid fdt status: %x\n", status);
		ret = -1;
		break;
	}

	return ret;
}


int fdtfast_setprop_u32(void *fdt, int nodeoffset, const char *name,
				  uint32_t val)
{
	struct fdt_property *prop_node;
	char *dtbfast_buffer = gd->fdtfast_blob;
	uint32_t  *data;

	prop_node = fdtfast_search_prop(dtbfast_buffer, (char *)fdt, nodeoffset, (char *)name);
	if (prop_node == NULL) {
		printf("can not find the string named %s\n", name);

		return -1;
	}

	if (cpu_to_be32(prop_node->len) != 4) {
		printf("the name len is invalid\n");

		return -1;
	}
	data = (uint32_t *)prop_node->data;
	*data = val;

	return 0;
}

int fdtfast_getprop_u32(const void *fdt, int nodeoffset,
			const char *name, uint32_t *val)
{
	struct fdt_property *prop_node;
	char *dtbfast_buffer = gd->fdtfast_blob;
	int len, j;
	uint32_t *p;

	if (val == NULL) {
		printf("the input buff is empty\n");

		return -1;
	}

	prop_node = fdtfast_search_prop(dtbfast_buffer, (char *)fdt, nodeoffset, (char *)name);
	if (prop_node == NULL) {
		printf("can not find the string named %s\n", name);

		return -1;
	}

	len = cpu_to_be32(prop_node->len);
	if (len & 3) {
		printf("the name len %d is invalid\n", len);

		return -1;
	}

	for (j = 0, p = (uint32_t *)prop_node->data; j < len/4; j++)
	{
		*val = fdt32_to_cpu(p[j]);
		val++;
	}

	return len/4;
}

int fdtfast_getprop_string(const void *fdt, int nodeoffset,
			const char *name, char **val)
{
	struct fdt_property *prop_node;
	char *dtbfast_buffer = gd->fdtfast_blob;
	int len;

	if (val == NULL) {
		printf("the input buff is empty\n");

		return -1;
	}

	prop_node = fdtfast_search_prop(dtbfast_buffer, (char *)fdt, nodeoffset, (char *)name);
	if (prop_node == NULL) {
		printf("can not find the string named %s\n", name);

		return -1;
	}
	len = cpu_to_be32(prop_node->len);

	*val = prop_node->data;

	return len;
}

int fdtfast_getprop_gpio(const void *fdt, int nodeoffset,
		const char* prop_name,	user_gpio_set_t* gpio_list)
{
	int ret ;
	u32 data[10];

	memset(data, 0, sizeof(data));
	ret = fdtfast_getprop_u32(fdt, nodeoffset, prop_name, data);
	if (ret < 0) {
		printf("can not find gpio\n");

		return -1;
	}

	strcpy(gpio_list->gpio_name, prop_name);

	gpio_list->port = data[1] + 1;  //0: PA
	gpio_list->port_num = data[2];
	gpio_list->mul_sel = data[3];
	gpio_list->pull = data[4];
	gpio_list->drv_level = data[5];
	gpio_list->data = data[6];

	debug("name = %s, port = %x,portnum=%x,mul_sel=%x,pull=%x drive= %x, data=%x\n",
			gpio_list->gpio_name,
			gpio_list->port,
			gpio_list->port_num,
			gpio_list->mul_sel,
			gpio_list->pull,
			gpio_list->drv_level,
			gpio_list->data);

	return 0;

}



