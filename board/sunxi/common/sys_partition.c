/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <mmc.h>
#include <sunxi_mbr.h>
#include <boot_type.h>
#include <u-boot/crc.h>
#include <sys_partition.h>

DECLARE_GLOBAL_DATA_PTR;

#if defined(DISABLE_SUNXI_MBR) && defined(CONFIG_SUN8IW12P1_NOR)

typedef struct sunxi_env_partition_t
{
	unsigned  char      name[16];
	unsigned  int       addrlo;
	unsigned  int       lenlo;
	unsigned  int       ro;
}sunxi_env_partition;

typedef struct sunxi_env_mtd_t
{
	unsigned  char			device_name[16];
	unsigned  int			device_id;
	unsigned  int			PartCount;
	sunxi_env_partition     array[SUNXI_MBR_MAX_PART_COUNT];
}sunxi_env_mtd;

static sunxi_env_mtd env_mtd;
static int  env_mtd_status;

static void update_UDISK(void)
{
	int partcount;
	int size;
	int flag = 0;
	char mtdparts[128]= {0};
	char tmp[20];

	sprintf(mtdparts, "%s.%d:", env_mtd.device_name,env_mtd.device_id);
	for(partcount = env_mtd.PartCount-1; partcount >=0; partcount--)
	{
		if(!strncmp((const char*)env_mtd.array[partcount].name, "UDISK", sizeof("UDKSK")))
		{
			if(env_mtd.array[partcount].lenlo == 0)
			{
				printf("find DISK size is 0,update it\n");
				size = sunxi_flash_size();
				if(size < env_mtd.array[partcount].addrlo)
					env_mtd.array[partcount].lenlo = 256;
				else
					env_mtd.array[partcount].lenlo = size - env_mtd.array[partcount].addrlo;
				flag =1;
				break;
			}
			else
				break;
		}
	}

	if(flag ==1)
	{
		for(partcount=0; partcount<env_mtd.PartCount; partcount++)
		{

			if(partcount == 0)
			{
					if(env_mtd.array[partcount].ro == 1)
							sprintf(tmp, "%dK(%s)ro",(env_mtd.array[partcount].lenlo *512/1024),
																env_mtd.array[partcount].name);
					else
							sprintf(tmp, "%dK(%s)",(env_mtd.array[partcount].lenlo *512/1024),
																env_mtd.array[partcount].name);
			}
			else
			{
					if(env_mtd.array[partcount].ro == 1)
							sprintf(tmp, ",%dK(%s)ro",(env_mtd.array[partcount].lenlo *512/1024),
																	env_mtd.array[partcount].name);
					else
							sprintf(tmp, ",%dK(%s)",(env_mtd.array[partcount].lenlo *512/1024),
																	env_mtd.array[partcount].name);
			}
			strcat(mtdparts, tmp);
		}

		setenv("mtdparts",mtdparts);
		saveenv();
	}
}
static int sunxi_partition_parse(char *cmdline)
{
		char *src;
		char *des;
		char tmp[5];
		int partcount;
		int count;
		int size;

		src = cmdline;
		//name
		src = cmdline;
		des = strchr(src, '.');
		count = (int)(des-src);
		strncpy((char *)(env_mtd.device_name), src,count);
		//printf("the device.name = %s\n",env_mtd.device_name);
		src = des;

		//id
		memset(tmp, 0, 5);
		des = strchr(src, ':');
		count = (int)(des-src);
		//printf("count %d\n",count);
		strncpy(tmp, (src+1), (count-1));
		env_mtd.device_id = (int)simple_strtoul(tmp, NULL, 10);
		//printf("env_mtd.device_id = %d\n",env_mtd.device_id);
		src = des;

		//partition
		for(partcount=0; *(des+1)!='\0'; partcount++)
		{
			//size
			memset(tmp, 0, 5);
			des = strchr(src, '(');
			count = (int)(des-src);
			//printf("count %d\n",count);
			strncpy(tmp,src+1,count-2);

			size = (int)simple_strtoul(tmp, NULL, 10);
			//printf("size : %d\n",size);
			if(*(des-1) == 'K')
				env_mtd.array[partcount].lenlo = size *1024 / 512;
			else if(*(des-1) == 'M')
				env_mtd.array[partcount].lenlo = size *1024 * 1024 / 512;
			if(partcount == 0)
				env_mtd.array[partcount].addrlo = 0;
			else
				env_mtd.array[partcount].addrlo = env_mtd.array[partcount-1].addrlo
													+ env_mtd.array[partcount-1].lenlo;
			//partition_name
			src = des;
			des = strchr(src, ')');
			count = (int)(des-src);
			strncpy((char *)(env_mtd.array[partcount].name),src+1,count-1);

			//only-read ?
			src = des;
			des = strchr(src, ',');
			if(des == NULL)
			{
				env_mtd.array[partcount].ro = 0;
				break;
			}
			count = (int)(des-src);
			if(!(count-1) && !strncmp((src+1), "ro",2))
				env_mtd.array[partcount].ro = 1;
			else
				env_mtd.array[partcount].ro = 0;

			src = des;

			//printf("name:%s, size:%d, offset:%x, ro:%d\n",env_mtd.array[partcount].name,env_mtd.array[partcount].lenlo,
			//											env_mtd.array[partcount].addrlo,env_mtd.array[partcount].ro);
		}
		if(!partcount)
		{
			return -1;
		}
		env_mtd.PartCount = partcount + 1;
		printf("partition_count:%d\n",env_mtd.PartCount);
		return 0;
}

int sunxi_partition_get_total_num(void) //get num of fenqu
{
	if(!env_mtd_status)
	{
		return 0;
	}
	return env_mtd.PartCount;
}

int sunxi_partition_get_name(int index, char *buf)
{

	if(env_mtd_status)
	{
		strncpy(buf, (const char *)env_mtd.array[index].name, 16);
	}
	else
	{
		memset(buf, 0, 16);
	}

	return 0;
}

uint sunxi_partition_get_offset(int part_index)
{


	if((!env_mtd_status) || (part_index >= env_mtd.PartCount))
	{
		return 0;
	}

	return env_mtd.array[part_index].addrlo;
}

uint sunxi_partition_get_size(int part_index)
{

	if((!env_mtd_status) || (part_index >= env_mtd.PartCount))
	{
		return 0;
	}

	return env_mtd.array[part_index].lenlo;
}

uint sunxi_partition_get_offset_byname(const char *part_name)
{
	int			i;

	if(!env_mtd_status)
	{
		return 0;
	}
	for(i=0; i<env_mtd.PartCount; i++)
	{
		if(!strcmp(part_name, (const char *)env_mtd.array[i].name))
		{
			return env_mtd.array[i].addrlo;
		}
	}

	return 0;
}

int sunxi_partition_get_partno_byname(const char *part_name)
{
	int			i;

	if(!env_mtd_status)
	{
		return -1;
	}
	for(i=0; i<env_mtd.PartCount; i++)
	{
		if(!strcmp(part_name, (const char *)env_mtd.array[i].name))
		{
			return i;
		}
	}

	return -1;
}


uint sunxi_partition_get_size_byname(const char *part_name)
{
	int			i;

	if(!env_mtd_status)
	{
		return 0;
	}
	for(i=0;i<env_mtd.PartCount;i++)
	{
		if(!strcmp(part_name, (const char *)env_mtd.array[i].name))
		{
			return env_mtd.array[i].lenlo;
		}
	}

	return 0;
}

/* get the partition info, offset and size
 * input: partition name
 * output: part_offset and part_size (in byte)
 */
int sunxi_partition_get_info_byname(const char *part_name, uint *part_offset, uint *part_size)
{
	int			i;

	if(!env_mtd_status)
	{
		return -1;
	}

	for(i=0; i<env_mtd.PartCount; i++)
	{
		if(!strcmp(part_name, (const char *)env_mtd.array[i].name))
		{
			*part_offset = env_mtd.array[i].addrlo;
			*part_size = env_mtd.array[i].lenlo;
			return 0;
		}
	}

	return -1;
}

void *sunxi_partition_fetch_mbr(void)
{
	printf("This uboot not support mbr.\n");
	return NULL;
}

int sunxi_partition_refresh(void *buf, uint bytes)
{
	printf("This uboot not support mbr.\n");
	printf("if you wan Modify partition data, please checking env \n");
	return -1;
}

int sunxi_partition_get_type(void)
{
	return PART_TYPE_AW;
}


int sunxi_partition_init(void)
{
	char *str = getenv("mtdparts");

	if(str == NULL)
	{
		printf("error:can't find mtdparts in env.\n");
		debug("get partition error\n");
		env_mtd_status = 0;
	}
	printf("%s\n",str);
	if(!sunxi_partition_parse(str))
	{
		env_mtd_status = 1;
	}
	update_UDISK();
	return 0;
}

#else


static char mbr_buf[SUNXI_MBR_SIZE];
static int  mbr_status;


int sunxi_partition_get_total_num(void)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)mbr_buf;
	if(!mbr_status)
	{
		return 0;
	}

	return mbr->PartCount;
}

int sunxi_partition_get_name(int index, char *buf)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)mbr_buf;

	if(mbr_status)
	{
		strncpy(buf, (const char *)mbr->array[index].name, 16);
	}
	else
	{
		memset(buf, 0, 16);
	}

	return 0;
}

uint sunxi_partition_get_offset(int part_index)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)mbr_buf;

	if((!mbr_status) || (part_index >= mbr->PartCount))
	{
		return 0;
	}

	return mbr->array[part_index].addrlo;
}

uint sunxi_partition_get_size(int part_index)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)mbr_buf;

	if((!mbr_status) || (part_index >= mbr->PartCount))
	{
		return 0;
	}

	return mbr->array[part_index].lenlo;
}

uint sunxi_partition_get_offset_byname(const char *part_name)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)mbr_buf;
	int			i;

	if(!mbr_status)
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

int sunxi_partition_get_partno_byname(const char *part_name)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)mbr_buf;
	int			i;

	if(!mbr_status)
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


uint sunxi_partition_get_size_byname(const char *part_name)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)mbr_buf;
	int			i;

	if(!mbr_status)
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
int sunxi_partition_get_info_byname(const char *part_name, uint *part_offset, uint *part_size)
{
	sunxi_mbr_t        *mbr  = (sunxi_mbr_t*)mbr_buf;
	int			i;

	if(!mbr_status)
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

void *sunxi_partition_fetch_mbr(void)
{
	if(!mbr_status)
	{
		return NULL;
	}

	return mbr_buf;
}

int sunxi_partition_refresh(void *buf, uint bytes)
{
	if(!mbr_status)
	{
		return -1;
	}
	if(bytes != SUNXI_MBR_SIZE)
	{
		return -1;
	}

	memcpy(mbr_buf, buf, bytes);

	return 0;
}

int sunxi_partition_get_type(void)
{
	return PART_TYPE_AW;
}


int sunxi_partition_init(void)
{
	sunxi_mbr_t    *mbr;
	int mbr_offset = 0;
	int i = 0;

	for (i = 0; i < SUNXI_MBR_COPY_NUM; i++)
	{
		if (!sunxi_flash_read(mbr_offset, SUNXI_MBR_SIZE >> 9, mbr_buf))
		{
			printf("read mbr copy[%d] failed\n", i);
			mbr_offset += (SUNXI_MBR_SIZE >> 9);
			continue;
		}

		mbr = (sunxi_mbr_t*)mbr_buf;
		if (!strncmp((const char*)mbr->magic, SUNXI_MBR_MAGIC, 8))
		{
			int crc = 0;

			crc = crc32(0, (const unsigned char *)&mbr->version, SUNXI_MBR_SIZE-4);
			if (crc == mbr->crc32)
			{
				printf("used mbr [%d], count = %d\n", i, mbr->PartCount);
				mbr_status = 1;
				gd->lockflag = mbr->lockflag;

				return mbr->PartCount;
			}
		}

		printf("crc mbr copy[%d] failed\n", i);
		mbr_offset += (SUNXI_MBR_SIZE >> 9);
	}
	mbr_status = 0;
	debug("mbr crc error\n");

	return 0;
}

#endif
