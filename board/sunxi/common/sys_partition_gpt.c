/*
 * Copyright (C) 2016 Allwinner.
 * wangwei <wangwei@allwinnertech.com>
 *
 * SUNXI GPT Partition
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <mmc.h>
#include <sunxi_mbr.h>
#include <boot_type.h>
#include <u-boot/crc.h>
#include <gpt.h>
#include <sunxi_board.h>
#include <malloc.h>
#include <sys_partition.h>

DECLARE_GLOBAL_DATA_PTR;


typedef struct
{
	char  *partition_buf;
	uint   partition_buf_size;
	uint   partition_logic_offset;
	uint   partition_init_flag;
	int    partition_type;

	int  (*partition_get_total_num)(void);
	int  (*partition_get_name)(int index, char *buf);
	uint (*partition_get_offset)(int part_index);
	uint (*partition_get_size)(int part_index);
	uint (*partition_get_offset_byname)(const char *part_name);
	uint (*partition_get_size_byname)(const char *part_name);
	int  (*partition_get_info_byname)(const char *part_name, uint *part_offset, uint *part_size);
	void* (*partition_fetch_mbr)(void);
	int (*partition_refresh)(void *buf, uint bytes);
	int (*partition_get_partno_byname)(const char *part_name);
} sunxi_partition_op_t;

extern int card0_convert_to_gpt(void *sunxi_mbr_buf, int update_sunxi_mbr);
int sunxi_is_gpt_valid(void *gpt_head);
int sunxi_is_awpart_valid(char* buffer);

int check_card0_partition(char* sunxi_mbr_buf);
int char8_char16_compare(const char *char8, const efi_char16_t *char16, size_t char16_len);


static sunxi_partition_op_t  *partition_op = NULL;


int gpt_partition_get_total_num(void)
{
	gpt_header     *gpt_head  = (gpt_header*)(partition_op->partition_buf+GPT_HEAD_OFFSET);
	gpt_entry      *entry  = (gpt_entry*)(partition_op->partition_buf + GPT_ENTRY_OFFSET);
	int i = 0;

	if(!partition_op->partition_init_flag)
	{
		return 0;
	}

	for(i=0;i<gpt_head->num_partition_entries;i++)
	{
		if( (entry[i].starting_lba == 0) && (entry[i].ending_lba == 0))
		{
			/*the last usable entry*/
			return i;
		}
	}

	return gpt_head->num_partition_entries;
}

int gpt_partition_get_name(int part_index, char *buf)
{
	int			i;

	char   char8_name[PARTNAME_SZ] = {0};
	gpt_header     *gpt_head  = (gpt_header*)(partition_op->partition_buf + GPT_HEAD_OFFSET);
	gpt_entry      *entry  = (gpt_entry*)(partition_op->partition_buf + GPT_ENTRY_OFFSET);

	if((!partition_op->partition_init_flag) || (part_index >= gpt_head->num_partition_entries))
	{
		memset(buf, 0, 16);
		return 0;
	}

	for(i=0;i < PARTNAME_SZ; i++ )
	{
		char8_name[i] = (char)(entry[part_index].partition_name[i]);
	}
	strcpy(buf, char8_name);

	return 0;
}

uint gpt_partition_get_offset(int part_index)
{
	gpt_header     *gpt_head  = (gpt_header*)(partition_op->partition_buf + GPT_HEAD_OFFSET);
	gpt_entry       *entry  = (gpt_entry*)(partition_op->partition_buf + GPT_ENTRY_OFFSET);

	if((!partition_op->partition_init_flag) || (part_index >= gpt_head->num_partition_entries))
	{
		return 0;
	}

	return (uint)(entry[part_index].starting_lba-partition_op->partition_logic_offset);
}

uint gpt_partition_get_size(int part_index)
{
	gpt_entry      *entry  = (gpt_entry*)(partition_op->partition_buf + GPT_ENTRY_OFFSET);
	gpt_header     *gpt_head  = (gpt_header*)(partition_op->partition_buf + GPT_HEAD_OFFSET);
	if((!partition_op->partition_init_flag) || (part_index >= gpt_head->num_partition_entries))
	{
		return 0;
	}
	return (uint)(entry[part_index].ending_lba - entry[part_index].starting_lba+1);
}


int gpt_partition_get_partno_byname(const char *part_name)
{
	int			   i;
	gpt_header     *gpt_head  = (gpt_header*)(partition_op->partition_buf + GPT_HEAD_OFFSET);
	gpt_entry      *entry  = (gpt_entry*)(partition_op->partition_buf + GPT_ENTRY_OFFSET);

	if(!partition_op->partition_init_flag)
	{
		return -1;
	}

	for(i=0;i<gpt_head->num_partition_entries;i++)
	{
		if(!char8_char16_compare(part_name, entry[i].partition_name, PARTNAME_SZ))
		{
			return i;
		}
	}

	return -1;
}

/* get the partition info, offset and size
 * input: partition name
 * output: part_offset and part_size (in byte)
 */
int gpt_partition_get_info_byname(const char *part_name, uint *part_offset, uint *part_size)
{
	int			i;
	gpt_header  *gpt_head  = (gpt_header*)(partition_op->partition_buf + GPT_HEAD_OFFSET);
	gpt_entry   *entry  = (gpt_entry*)(partition_op->partition_buf + GPT_ENTRY_OFFSET);

	if(!partition_op->partition_init_flag)
	{
		return -1;
	}

	for(i=0;i<gpt_head->num_partition_entries;i++)
	{
		if(!char8_char16_compare(part_name, entry[i].partition_name, PARTNAME_SZ))
		{
			*part_offset =  (uint)(entry[i].starting_lba - partition_op->partition_logic_offset);
			*part_size = (uint)(entry[i].ending_lba - entry[i].starting_lba+1);
			return 0;
		}
	}

	return -1;
}

uint gpt_partition_get_offset_byname(const char *part_name)
{
	uint part_offset = 0, part_size = 0;
	gpt_partition_get_info_byname(part_name,&part_offset, &part_size);
	return part_offset;
}


uint gpt_partition_get_size_byname(const char *part_name)
{
	uint part_offset = 0, part_size = 0;
	gpt_partition_get_info_byname(part_name,&part_offset, &part_size);
	return part_size;
}



void *gpt_partition_fetch_mbr(void)
{
	if(!partition_op->partition_init_flag)
	{
		return NULL;
	}

	return partition_op->partition_buf;
}

int gpt_partition_refresh(void *buf, uint bytes)
{
	if(!partition_op->partition_init_flag)
	{
		return -1;
	}
	if(bytes != SUNXI_GPT_SIZE)
	{
		return -1;
	}

	memcpy(partition_op->partition_buf, buf, bytes);

	return 0;
}



int aw_partition_get_total_num(void)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)partition_op->partition_buf;
	if(!partition_op->partition_init_flag)
	{
		return 0;
	}

	return mbr->PartCount;
}

int aw_partition_get_name(int index, char *buf)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)partition_op->partition_buf;

	if(partition_op->partition_init_flag)
	{
		strncpy(buf, (const char *)mbr->array[index].name, 16);
	}
	else
	{
		memset(buf, 0, 16);
	}

	return 0;
}

uint aw_partition_get_offset(int part_index)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)partition_op->partition_buf;

	if((!partition_op->partition_init_flag) || (part_index >= mbr->PartCount))
	{
		return 0;
	}

	return mbr->array[part_index].addrlo;
}

uint aw_partition_get_size(int part_index)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)partition_op->partition_buf;

	if((!partition_op->partition_init_flag) || (part_index >= mbr->PartCount))
	{
		return 0;
	}

	return mbr->array[part_index].lenlo;
}

uint aw_partition_get_offset_byname(const char *part_name)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)partition_op->partition_buf;
	int			i;

	if(!partition_op->partition_init_flag)
	{
		return 0;
	}
	for(i=0;i<mbr->PartCount;i++)
	{
		if(!strcmp(part_name, (const char *)mbr->array[i].name))
		{
			return mbr->array[i].addrlo;
		}
	}

	return 0;
}

int aw_partition_get_partno_byname(const char *part_name)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)partition_op->partition_buf;
	int			i;

	if(!partition_op->partition_init_flag)
	{
		return -1;
	}
	for(i=0;i<mbr->PartCount;i++)
	{
		if(!strcmp(part_name, (const char *)mbr->array[i].name))
		{
			return i;
		}
	}

	return -1;
}


uint aw_partition_get_size_byname(const char *part_name)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)partition_op->partition_buf;
	int			i;

	if(!partition_op->partition_init_flag)
	{
		return 0;
	}
	for(i=0;i<mbr->PartCount;i++)
	{
		if(!strcmp(part_name, (const char *)mbr->array[i].name))
		{
			return mbr->array[i].lenlo;
		}
	}

	return 0;
}

/* get the partition info, offset and size
 * input: partition name
 * output: part_offset and part_size (in byte)
 */
int aw_partition_get_info_byname(const char *part_name, uint *part_offset, uint *part_size)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)partition_op->partition_buf;
	int			i;

	if(!partition_op->partition_init_flag)
	{
		return -1;
	}

	for(i=0;i<mbr->PartCount;i++)
	{
		if(!strcmp(part_name, (const char *)mbr->array[i].name))
		{
			*part_offset = mbr->array[i].addrlo;
			*part_size = mbr->array[i].lenlo;
			return 0;
		}
	}

	return -1;
}

void *aw_partition_fetch_mbr(void)
{
	if(!partition_op->partition_init_flag)
	{
		return NULL;
	}

	return partition_op->partition_buf;
}

int aw_partition_refresh(void *buf, uint bytes)
{
	if(!partition_op->partition_init_flag)
	{
		return -1;
	}
	if(bytes != SUNXI_MBR_SIZE)
	{
		return -1;
	}

	memcpy(partition_op->partition_buf, buf, bytes);

	return 0;
}



int sunxi_partition_get_total_num(void)
{
	return partition_op->partition_get_total_num();
}

int sunxi_partition_get_name(int index, char *buf)
{
	return partition_op->partition_get_name(index, buf);
}

uint sunxi_partition_get_offset(int part_index)
{
	return partition_op->partition_get_offset(part_index);
}

uint sunxi_partition_get_size(int part_index)
{
	return partition_op->partition_get_size(part_index);
}

uint sunxi_partition_get_offset_byname(const char *part_name)
{
	return partition_op->partition_get_offset_byname(part_name);
}

int sunxi_partition_get_partno_byname(const char *part_name)
{
	return partition_op->partition_get_partno_byname(part_name);
}


uint sunxi_partition_get_size_byname(const char *part_name)
{
	return partition_op->partition_get_size_byname(part_name);
}

/* get the partition info, offset and size
 * input: partition name
 * output: part_offset and part_size (in byte)
 */
int sunxi_partition_get_info_byname(const char *part_name, uint *part_offset, uint *part_size)
{
	return partition_op->partition_get_info_byname(part_name,part_offset,part_size);
}

void *sunxi_partition_fetch_mbr(void)
{
	return partition_op->partition_fetch_mbr();
}

int sunxi_partition_refresh(void *buf, uint bytes)
{
	return partition_op->partition_refresh(buf, bytes);
}

int sunxi_partition_get_type(void)
{
	return partition_op->partition_type;
}

int sunxi_partition_init(void)
{
	char *part_buff = NULL;
	int part_buff_size = SUNXI_GPT_SIZE > SUNXI_MBR_SIZE ? SUNXI_GPT_SIZE : SUNXI_MBR_SIZE;

	partition_op = (sunxi_partition_op_t *)malloc(sizeof(sunxi_partition_op_t));
	part_buff = malloc(part_buff_size);
	if(!partition_op || !part_buff)
	{
		printf("%s:malloc fail\n", __func__);
		return -1;
	}

	memset(partition_op, 0x0, sizeof(sunxi_partition_op_t));
	memset(part_buff, 0x0, part_buff_size);

	/* read partition table */
	if(!sunxi_flash_read(0, part_buff_size>>9, part_buff))
	{
		printf("read flash error\n");
		return -1;
	}

	if(sunxi_is_gpt_valid(part_buff + GPT_HEAD_OFFSET))
	{
		int storage_type = get_boot_storage_type();
		if(storage_type == STORAGE_EMMC || storage_type == STORAGE_EMMC3
			|| storage_type == STORAGE_SD)
		{
			partition_op->partition_logic_offset = CONFIG_MMC_LOGICAL_OFFSET;
		}
		partition_op->partition_buf = part_buff;
		partition_op->partition_buf_size = SUNXI_GPT_SIZE;
		partition_op->partition_init_flag = 1;
		partition_op->partition_type = PART_TYPE_GPT;

		partition_op->partition_get_total_num    = gpt_partition_get_total_num;
		partition_op->partition_get_name         = gpt_partition_get_name;
		partition_op->partition_get_offset       = gpt_partition_get_offset;
		partition_op->partition_get_size         = gpt_partition_get_size;
		partition_op->partition_get_offset_byname= gpt_partition_get_offset_byname;
		partition_op->partition_get_size_byname  = gpt_partition_get_size_byname;
		partition_op->partition_get_info_byname  = gpt_partition_get_info_byname;
		partition_op->partition_fetch_mbr        = gpt_partition_fetch_mbr;
		partition_op->partition_refresh          = gpt_partition_refresh;
		partition_op->partition_get_partno_byname= gpt_partition_get_partno_byname;

		printf("GPT partition init ok\n");
		return partition_op->partition_get_total_num();
	}
	else if(sunxi_is_awpart_valid(part_buff))
	{
		partition_op->partition_buf = part_buff;
		partition_op->partition_buf_size = SUNXI_MBR_SIZE;
		partition_op->partition_init_flag = 1;
		partition_op->partition_type = PART_TYPE_AW;

		partition_op->partition_get_total_num    = aw_partition_get_total_num;
		partition_op->partition_get_name         = aw_partition_get_name;
		partition_op->partition_get_offset       = aw_partition_get_offset;
		partition_op->partition_get_size         = aw_partition_get_size;
		partition_op->partition_get_offset_byname= aw_partition_get_offset_byname;
		partition_op->partition_get_size_byname  = aw_partition_get_size_byname;
		partition_op->partition_get_info_byname  = aw_partition_get_info_byname;
		partition_op->partition_fetch_mbr        = aw_partition_fetch_mbr;
		partition_op->partition_refresh          = aw_partition_refresh;
		partition_op->partition_get_partno_byname= aw_partition_get_partno_byname;
		printf("AW partition init ok\n");

#ifdef CONFIG_CONVERT_CARD0_TO_GPT
		if(0 == check_card0_partition(partition_op->partition_buf))
		{
			/*card0 conver sunxi-MBR to gpt success */
			partition_op->partition_type = PART_TYPE_GPT;
		}
#endif
		return partition_op->partition_get_total_num();
	}
	else
	{
		printf("partition init fail\n");

	}

	return 0;
}



int sunxi_is_gpt_valid(void *buffer)
{
	u32 calc_crc32 = 0;
	u32 backup_crc32 = 0;
	gpt_header *gpt_head = (gpt_header *)buffer;

	if(gpt_head->signature != GPT_HEADER_SIGNATURE)
	{
		debug("gpt magic error, %llx != %llx\n",gpt_head->signature, GPT_HEADER_SIGNATURE);
		return 0;
	}
	backup_crc32 = gpt_head->header_crc32;
	gpt_head->header_crc32 = 0;
	calc_crc32 = crc32(0,(const unsigned char *)gpt_head, sizeof(gpt_header));
	gpt_head->header_crc32 = backup_crc32;
	if(calc_crc32 == backup_crc32)
	{
		return 1;
	}

	printf("gpt crc error, 0x%x != 0x%x\n",backup_crc32, calc_crc32);
	return 0;

}

int sunxi_is_awpart_valid(char* buffer)
{
	sunxi_mbr_t    *mbr;
	mbr = (sunxi_mbr_t*)buffer;
	if (!strncmp((const char*)mbr->magic, SUNXI_MBR_MAGIC, 8))
	{
		int crc = 0;
		crc = crc32(0, (const unsigned char *)&mbr->version, SUNXI_MBR_SIZE-4);
		if (crc == mbr->crc32)
		{
			return 1;
		}
	}
	printf("aw mbr crc error\n");
	return 0;
}


int check_card0_partition(char* sunxi_mbr_buf)
{
	gpt_header  *gpt_head;
	int         ret = 0;
	char        buffer[1024];

	if(STORAGE_SD != get_boot_storage_type())
	{
		return -1;
	}
	if(WORK_MODE_BOOT != get_boot_work_mode())
	{
		return -1;
	}
	memset(buffer, 0x0, sizeof(buffer));

	/*read gpt header*/
	ret = sunxi_flash_phyread(1,1,buffer);
	if(!ret)
	{
		printf("%s: phy read fail\n",__func__);
		return -1;
	}
	gpt_head = (gpt_header *)buffer;
	if(!sunxi_is_gpt_valid(gpt_head))
	{
		/*conver primary_mbr and sunxi_mbr to GPT format.*/
		if(card0_convert_to_gpt(sunxi_mbr_buf, GPT_UPDATE_PRIMARY_MBR))
			return -1;
	}
	debug("Primary GPT entry LBA: %lld\n", gpt_head->partition_entry_lba);

	return 0;
}

int char8_char16_compare(const char *char8, const efi_char16_t *char16, size_t char16_len)
{
	char char8_str[PARTNAME_SZ] = {0};
	int  i = 0;

	if(char16_len > PARTNAME_SZ)
	{
		return -1;
	}
	for(i = 0; i < char16_len; i++)
	{
		char8_str[i] = char16[i];
	}

	return strcmp(char8,char8_str);
}


