/*
 * Copyright (c) 2016 Allwinnertech Co., Ltd.
 * Author: zhouhuacai <zhouhuacai@allwinnertech.com>
 *
 * Define the function interface for the normal efuse read/write.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * SPDX-License-Identifier:     GPL-2.0+
*/

#include "common.h"
#include <efuse_map.h>
#include <asm/arch/sid.h>
#include <sunxi_board.h>

//#define EFUSE_DEBUG

static efuse_key_map_t key_imformatiom[] =
{
	{"sensor", EFUSE_THERMAL_SENSOR, SID_THERMAL_SIZE, -1, -1, {0}},
	{"reserved", EFUSE_RESERVED, EFUSE_RESERVED_SIZE, -1, -1, {0}},
	{{0} , 0, 0, 0, 0,{0}}
};

static void sid_program_key(uint key_index, uint key_value)
{
	uint reg_val;

	writel(key_value, SID_PRKEY);

	reg_val = readl(SID_PRCTL);
	reg_val &= ~((0x1ff<<16)|0x3);
	reg_val |= key_index<<16;
	writel(reg_val, SID_PRCTL);

	reg_val &= ~((0xff<<8)|0x3);
	reg_val |= (SID_OP_LOCK<<8) | 0x1;
	writel(reg_val, SID_PRCTL);

	while(readl(SID_PRCTL)&0x1){};

	reg_val &= ~((0x1ff<<16)|(0xff<<8)|0x3);
	writel(reg_val, SID_PRCTL);

	return;
}

static uint sid_read_key(uint key_index)
{
	uint key_val;

	memcpy((void*)&key_val,(void*)(SUNXI_SID_SRAM+key_index),4);

	return key_val;
}

int sunxi_efuse_write(void *key_buf)
{
	efuse_key_info_t  *key_list = NULL;
	unsigned long key_data_addr;
	int i;
	unsigned int key_start_addr;
	unsigned int *key_once_data = 0;
	unsigned int key_data_remain_size;
	unsigned int verify_buf[128];

	efuse_key_map_t *key_map = key_imformatiom;

	if (key_buf == NULL)
	{
		printf("[efuse] error: buf is null\n");
		return -1;
	}
	key_list = (efuse_key_info_t  *)key_buf;
	key_data_addr = ((unsigned long)key_list->key_data) & 0xffffffff ;

#ifdef EFUSE_DEBUG
		printf("^^^^^^^printf key_buf^^^^^^^^^^^^\n");
		printf("key name=%s\n", key_list->name);
		printf("key len=%d\n", key_list->len);
		printf("key data:%p\n", (void*)key_data_addr);
		sunxi_dump((void *)key_data_addr, key_list->len);
		printf("###################\n");
#endif

	/* check the key name,only the  key name match in key_map can be burn */
	for (; key_map != NULL; key_map++)
	{
		if (!memcmp(key_list->name, key_map->name, strlen(key_map->name)))
		{
			printf(" burn key start\n");
			printf("burn key start\n");
			printf("key name = %s\n", key_map->name);
			printf("key index = 0x%x\n", key_map->key_index);

			/* check if there is enough space to stroe the key*/
			if ((key_map->store_max_bit / 8) < key_list->len)
			{
				printf("[efuse] error: not enough space to store the key, efuse size(%d), data size(%d)\n",
					  key_map->store_max_bit/8, key_list->len);

				return -1;
			}
			break;
		}
	}

	if (key_map == NULL)
	{
		printf("[efuse] error: can't burn the key (unknow)\n");

		return -1;
	}
	/* burn efuse key */
	key_start_addr = key_map->key_index;
	key_data_remain_size = key_list->len;
	memset(verify_buf, 0, 512);
	memcpy(verify_buf,(void *)key_data_addr,key_data_remain_size);
	key_once_data = verify_buf;
	//flush_cache((uint)pbuf, byte_cnt);
	for(;key_data_remain_size >= 4; key_data_remain_size-=4, key_start_addr += 4, key_once_data ++)
	{
		printf("key_data_remain_size=%d\n", key_data_remain_size);
		printf("key data=0x%x, addr=0x%p\n", *key_once_data, key_once_data);
		sid_program_key(key_start_addr, *key_once_data);

		printf("[efuse] addr = 0x%x, data = 0x%x\n", key_start_addr, *key_once_data);
	}

	if(key_data_remain_size)
	{
		if(key_data_remain_size == 1)
		{
			*key_once_data &= 0x000000ff;
		}
		else if(key_data_remain_size == 2)
		{
			*key_once_data &= 0x0000ffff;
		}
		else if(key_data_remain_size == 3)
		{
			*key_once_data &= 0x00ffffff;
		}
		sid_program_key(key_start_addr, *key_once_data);

		printf("[efuse] addr = 0x%x, data = 0x%x\n", key_start_addr, *key_once_data);
	}
	/* read back the key*/
	key_start_addr = key_map->key_index;
	key_data_remain_size = key_list->len;

	memset(verify_buf, 0, 512);
	if(key_data_remain_size & 3)
		key_data_remain_size = (key_data_remain_size + 3) & (~3);
	for(i=0;i<key_data_remain_size/4; i++)
	{
		verify_buf[i] = sid_read_key(key_start_addr);
		key_start_addr += 4;
	}
	//compare the key
	if(memcmp(verify_buf, (const void *)key_data_addr, key_list->len))
	{
		printf("compare burned key with memory data failed\n");
		printf("memory data:\n");
		sunxi_dump((void *)key_data_addr, key_list->len);
		printf("burned key:\n");
		sunxi_dump(verify_buf, key_list->len);

		return -1;
	}
	printf(" burn key end\n");

	return 0;
}

int sunxi_efuse_read(void *key_name, void *read_buf,int *len)
{
	efuse_key_map_t *key_map = key_imformatiom;
	unsigned int key_start_addr;
	int check_buf[128];
	unsigned int key_data_remain_size;
	int i;

	/* check the key name,only the  key name match in key_map can be read */
	for (; key_map != NULL; key_map++)
	{
		if (!memcmp(key_name, key_map->name, strlen(key_map->name)))
		{
			printf("read key start\n");
			printf("key name = %s\n", key_map->name);
			printf("key index = 0x%x\n", key_map->key_index);
			break;
		}
	}

	if (key_map == NULL)
	{
		printf("[efuse] error: can't read the key (unknow)\n");

		return -1;
	}

	key_start_addr = key_map->key_index;
	key_data_remain_size = key_map->store_max_bit / 8;

	memset(check_buf, 0, 512);
	if(key_data_remain_size & 3)
		key_data_remain_size = (key_data_remain_size + 3) & (~3);
	for(i=0;i<key_data_remain_size/4; i++)
	{
		check_buf[i] = sid_read_key(key_start_addr);
		key_start_addr += 4;
	}

	*len = key_data_remain_size;
	sunxi_dump(check_buf, key_map->store_max_bit / 8);
	memcpy((void *)read_buf, check_buf, key_map->store_max_bit / 8);
	flush_dcache_range((unsigned long)read_buf, key_map->store_max_bit / 8);

	return 0;
}

