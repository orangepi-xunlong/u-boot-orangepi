/*
 *
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <malloc.h>
#include <sunxi_mbr.h>
#include <part_efi.h>
#include <sunxi_flash.h>
#include <sys_partition.h>
#include <sprite_download.h>

#define  SUNXI_SPRITE_PROTECT_DATA_MAX    (16)
#define  SUNXI_SPRITE_PROTECT_PART        "private"

struct private_part_info
{
	uint part_sectors;
	char *part_buf;
	char part_name[32];
};

struct private_part_info  part_info[SUNXI_SPRITE_PROTECT_DATA_MAX];


int sunxi_sprite_store_part_data(void *buffer)
{
	int i, j, index;
	char part_name[PARTNAME_SZ] = {0};
	u32  part_len = 0;
	u32  part_start = 0;
	gpt_header *gpt_head = (gpt_header *)(buffer + GPT_HEAD_OFFSET);
	gpt_entry  *entry    = (gpt_entry*)(buffer + GPT_ENTRY_OFFSET);

	/* check GPT first*/
	if(gpt_head->signature != GPT_HEADER_SIGNATURE) {
		printf("not GPT table\n");
		return -1;
	}

	for(i=0, j =0; i<gpt_head->num_partition_entries; i++)
	{
		memset(part_name, 0x0, sizeof(part_name));
		for(index = 0; index < PARTNAME_SZ; index++ ) {
			part_name[index] = (char)(entry[i].partition_name[index]);
		}
		printf("part %d name %s\n", i, part_name);
		printf("keydata = 0x%x\n", entry[i].attributes.fields.keydata);
		if( (!strcmp((const char *)part_name, SUNXI_SPRITE_PROTECT_PART))
			|| entry[i].attributes.fields.keydata == 0x1)
		{
			int storage_type = get_boot_storage_type();
			int logic_offset;
			if (storage_type == STORAGE_EMMC || storage_type == STORAGE_EMMC3
				|| storage_type == STORAGE_SD || storage_type == STORAGE_EMMC0) {
				logic_offset = 40960;
			} else {
				logic_offset = 0;
			}
			part_start =  entry[i].starting_lba - logic_offset;
			part_len = entry[i].ending_lba - entry[i].starting_lba + 1;
			printf("find keypart %s\n", part_name);
			printf("keypart read start: 0x%x, sectors 0x%x\n", part_start, part_len);

			part_info[j].part_buf = (char *)memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN(part_len * 512, CONFIG_SYS_CACHELINE_SIZE));
			if(!part_info[j].part_buf)
			{
				printf("cant malloc memory for part %s, sectors 0x%x\n", part_name, part_len);

				goto __sunxi_sprite_store_part_data_fail;
			}
			if(!sunxi_sprite_read(part_start, part_len, (void *)part_info[j].part_buf))
			{
				printf("read private data error\n");

				goto __sunxi_sprite_store_part_data_fail;
			}
			printf("keypart part %s read  0x%x, sectors 0x%x\n", part_name, part_start, part_len);

			part_info[j].part_sectors = part_len;
			strcpy(part_info[j].part_name, (const char *)part_name);

			j ++;
		}
	}
	if(!j)
	{
		printf("there is no keypart part on local flash\n");
	}

	return 0;

__sunxi_sprite_store_part_data_fail:
	for(i=0;i<j;i++)
	{
		if(part_info[i].part_buf)
		{
			free(part_info[i].part_buf);
		}
		else
		{
			break;
		}
	}

	return -1;
}

int sunxi_sprite_restore_part_data(void *buffer)
{
	int i, j = 0, index;
	char part_name[PARTNAME_SZ] = { 0 };
	u32 part_len		    = 0;
	u32 part_start		    = 0;
	gpt_header *gpt_head	= (gpt_header *)(buffer + GPT_HEAD_OFFSET);
	gpt_entry *entry	    = (gpt_entry *)(buffer + GPT_ENTRY_OFFSET);
	uint down_sectors;

	/* check GPT first*/
	if (gpt_head->signature != GPT_HEADER_SIGNATURE) {
		printf("not GPT table\n");
		goto __sunxi_sprite_restore_part_data_fail;
	}

	while (part_info[j].part_buf) {
		down_sectors = part_info[j].part_sectors;
		for (i = 0; i < gpt_head->num_partition_entries; i++) {
			memset(part_name, 0x0, sizeof(part_name));
			for (index = 0; index < PARTNAME_SZ; index++) {
				part_name[index] =
					(char)(entry[i].partition_name[index]);
			}
			int storage_type = get_boot_storage_type();
			int logic_offset;
			if (storage_type == STORAGE_EMMC || storage_type == STORAGE_EMMC3
				|| storage_type == STORAGE_SD || storage_type == STORAGE_EMMC0) {
				logic_offset = 40960;
			} else {
				logic_offset = 0;
			}
			part_start =  entry[i].starting_lba - logic_offset;
			part_len = entry[i].ending_lba - entry[i].starting_lba + 1;
			if (!strcmp((const char *)part_name,
				    part_info[j].part_name)) {
				if (part_len < down_sectors) {
					printf("restore part %s too small, part_len:%d down_sectors:%d\n",
					       part_name, part_len,
					       down_sectors);
					down_sectors = part_len;
				}
				printf("keypart write start: 0x%x, sectors 0x%x\n",
				       part_start, down_sectors);
				if (!sunxi_sprite_write(
					    part_start, down_sectors,
					    (void *)part_info[j].part_buf)) {
					printf("sunxi sprite error : write private data error\n");

					goto __sunxi_sprite_restore_part_data_fail;
				}
			}
		}
		j++;
	}

	if (!j)
		printf("there is no private part need rewrite\n");

	return 0;

__sunxi_sprite_restore_part_data_fail:
	for (i = 0; i < j; i++) {
		if (part_info[i].part_buf)
			free(part_info[i].part_buf);
		else
			break;
	}

	return -1;
}

int sunxi_sprite_probe_prvt(void  *buffer)
{
	int i;

	sunxi_mbr_t  *mbr = (sunxi_mbr_t *)buffer;
	for(i=0;i<mbr->PartCount;i++)
	{
		if( (!strcmp((const char *)mbr->array[i].name, SUNXI_SPRITE_PROTECT_PART)) || (mbr->array[i].keydata == 0x8000))
		{
			printf("private part exist\n");

			return 1;
		}
	}

	return 0;
}

int sunxi_sprite_erase_private_key(void *buffer)
{
	int count       = 0;
	int flash_start = 0, flash_sectors = 0;
	int i = 0, j = 0, len = 1024 * 1024;
	sunxi_mbr_t *mbr = (sunxi_mbr_t *)buffer;
	char *fill_zero  = NULL;

	char char8_name[PARTNAME_SZ] = { 0 };
	gpt_header *gpt_head	 = (gpt_header *)(buffer + GPT_HEAD_OFFSET);

	gpt_entry *entry  = (gpt_entry *)(buffer + GPT_ENTRY_OFFSET);
	u32 logic_offset  = 0;
	int total_sectors = 0;
	int storage_type  = 0;
	__le64 total_len = 0, other_len = 0;

	flash_sectors = len / 512;
	if (gpt_head->signature == GPT_HEADER_SIGNATURE) {
		storage_type = get_boot_storage_type();
		sunxi_get_logical_offset_param(storage_type, &logic_offset,
					       &total_sectors);

		for (i = 0; i < gpt_head->num_partition_entries; i++) {
			for (j = 0; j < PARTNAME_SZ; j++) {
				char8_name[j] =
					(char)(entry[i].partition_name[j]);
			}
			if ((!strcmp(char8_name, SUNXI_SPRITE_PROTECT_PART)) ||
			    (entry[i].attributes.fields.keydata == 1)) {
				printf("private part exist!!!!!\n");
				total_len = entry[i].ending_lba -
					    entry[i].starting_lba + 1;
				count     = total_len / flash_sectors;
				other_len = total_len & (flash_sectors - 1);
				flash_start =
					entry[i].starting_lba - logic_offset;
				break;
			}
		}
		if (i >= gpt_head->num_partition_entries) {
			printf("private part is not exit!!!!! \n");
			return -2;
		}
	} else {
		for (i = 0; i < mbr->PartCount; i++) {
			if ((!strcmp((const char *)mbr->array[i].name,
				     SUNXI_SPRITE_PROTECT_PART)) ||
			    (mbr->array[i].keydata == 0x8000)) {
				printf("private part exist\n");
				total_len   = mbr->array[i].lenlo;
				count       = total_len / flash_sectors;
				other_len   = total_len & (flash_sectors - 1);
				flash_start = mbr->array[i].addrlo;
				break;
			}
		}

		if (i >= mbr->PartCount) {
			printf("private part is not exit \n");
			return -2;
		}
	}
	fill_zero = (char *)memalign(CONFIG_SYS_CACHELINE_SIZE,
				     ALIGN(len, CONFIG_SYS_CACHELINE_SIZE));
	if (fill_zero == NULL) {
		printf("no enough memory to malloc \n");
		return -1;
	}

	memset(fill_zero, 0x0, len);
	for (i = 0; i < count; i++) {
		if (!sunxi_sprite_write(flash_start + i * flash_sectors,
					flash_sectors, (void *)fill_zero)) {
			printf("write flash from 0x%x, sectors 0x%x failed\n",
			       flash_start + i * flash_sectors, flash_sectors);
			return -1;
		}
	}
	if (sunxi_sprite_write(flash_start + i * flash_sectors, other_len,
				(void *)fill_zero) != other_len) {
		printf("write flash from 0x%x, other_len 0x%llx failed\n",
		       flash_start + i * flash_sectors, other_len);
		return -1;
	}
	printf("erase private key successed \n");
	return 0;
}
