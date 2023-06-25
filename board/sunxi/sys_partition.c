
// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2018-2020
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 *
 */
#include <common.h>
#include <sys_partition.h>
#include <sunxi_flash.h>
#include <memalign.h>
#include <sunxi_mbr.h>
#include <sunxi_board.h>
#include <android_misc.h>
#include <android_ab.h>

#ifndef CONFIG_ENABLE_MTD_CMDLINE_PARTS_BY_ENV
static sunxi_mbr_t *mbr ;
#endif

extern struct bootloader_control ab_message;
#define MMC_LOGICAL_OFFSET   (20 * 1024 * 1024/512)
DECLARE_GLOBAL_DATA_PTR;

#if defined (CONFIG_ENABLE_MTD_CMDLINE_PARTS_BY_ENV)
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

int sunxi_partition_parse_get_info(int index, disk_partition_t *info)
{
	int  find_flag = 0;
	char *src;
	char *des;
	char tmp[5];
	int partcount;
	int count;
	int size;
	char *cmdline = env_get("sunxi_mtdparts");

	if(cmdline == NULL)
	{
		printf("error:can't find mtdparts in env.\n");
		debug("get partition error\n");
		return -1;
	}
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
		if((index-1) == partcount)
		{
			find_flag = 1;
			strncpy((char *)info->name, (const char *)env_mtd.array[partcount].name, PART_NAME_LEN);
			info->start = env_mtd.array[partcount].addrlo;
			info->size = env_mtd.array[partcount].lenlo;
			//printf("name:%s, size:%d, offset:%x, ro:%d\n",env_mtd.array[partcount].name,env_mtd.array[partcount].lenlo,env_mtd.array[partcount].addrlo,env_mtd.array[partcount].ro);
			break ; //dont check all
		}
	}

	if(!find_flag) {
		return -1;
	}
/*
	env_mtd.PartCount = partcount + 1;
	printf("partition_count:%d\n",env_mtd.PartCount);
*/
	return 0;
}
#endif

int sunxi_partition_init(void)
{
#ifndef CONFIG_ENABLE_MTD_CMDLINE_PARTS_BY_ENV
	if (mbr == NULL) {
		mbr = memalign(ARCH_DMA_MINALIGN, SUNXI_MBR_SIZE);
		if (mbr == NULL) {
			printf("unable to allocate TDs\n");
			return -1;
		}

		if (sunxi_flash_read(0, SUNXI_MBR_SIZE / 512, (void *)mbr) <=
		    0) {
			printf("line:%d:sunxi_flash_read fail\n", __LINE__);
			return -1;
		}
		if (!strncmp((const char *)mbr->magic, SUNXI_MBR_MAGIC, 8)) {
			int crc = 0;
			crc     = crc32(0, (const unsigned char *)&mbr->version,
				    SUNXI_MBR_SIZE - 4);
			if (crc != mbr->crc32) {
				return -1;
			}
			gd->lockflag = mbr->lockflag;
		}
	}
#endif
	return 0;
}

int sunxi_probe_partition_map(void)
{
#ifndef CONFIG_ENABLE_MTD_CMDLINE_PARTS_BY_ENV
	struct blk_desc *desc;
	desc = blk_get_devnum_by_typename("sunxi_flash", 0);
	if (desc == NULL) {
		pr_err("%s: get desc fail\n", __func__);
		return -1;
	}
	if (part_init_info_map(desc) < 0)
		return -1;
	else
#endif
		return 0;

}

int sunxi_replace_android_ab_system(char *intput_part_name, char *output_part_name)
{
	strncpy(output_part_name, intput_part_name, strlen(intput_part_name));
#ifdef CONFIG_ANDROID_AB
	int i, ret;
	static int part_cur_len;
	static char ab_partition[16][16] = {"bootloader", "env", "boot", "vendor_boot", "dtbo", "vbmeta", "vbmeta_system", "vbmeta_vendor"};
	char *ab_part_list = env_get("ab_partition_list");
	if ((gd->env_has_init) && (ab_part_list != NULL) && (part_cur_len != strlen(ab_part_list))) {
		memset(ab_partition, 0, sizeof(ab_partition));
		sunxi_parsed_specific_string(ab_part_list, ab_partition, ',', ' ');
		part_cur_len = strlen(ab_part_list);
	}
	ret = ab_select_slot_from_partname("misc");
	/* tick_printf("output_part_name:%s\t intput_part_name:%s\n", output_part_name, intput_part_name); */
	for (i = 0; i < sizeof(ab_partition)/sizeof(ab_partition[0]); i++) {
		if (!strncmp(intput_part_name, ab_partition[i], max(strlen(intput_part_name), strlen(ab_partition[i])))) {
			if (ret == 1) {
				sprintf(output_part_name, "%s_b", intput_part_name);
			} else {
				sprintf(output_part_name, "%s_a", intput_part_name);
			}
			break;
		}
	}
#endif
	/* tick_printf("output_part_name:%s\t intput_part_name:%s\n", output_part_name, intput_part_name); */
	return 0;
}

int sunxi_partition_get_partno_byname(const char *part_name)
{
	int i;
	struct blk_desc *desc;
	int ret;
	disk_partition_t info;
	char temp_part_name[16] = {0};

	desc = blk_get_devnum_by_typename("sunxi_flash", 0);
	if (desc == NULL) {
		printf("%s: get desc fail\n", __func__);
		ret = -ENODEV;
	}
	sunxi_replace_android_ab_system((char *)part_name, temp_part_name);

	for (i = 1;; i++) {
		ret = part_get_info(desc, i, &info);
		debug("%s: try part %d, ret = %d\n", __func__, i, ret);
		if (ret < 0) {
			printf("partno erro : can't find partition %s\n", part_name);
			return ret;
		}
		if (!strncmp((const char *)info.name, temp_part_name, sizeof(info.name))) {
			return i;
		} else if (!strncmp((const char *)info.name, part_name, sizeof(info.name))) {
			return i;
		}
	}

	return -1;
}

int sunxi_partition_get_info_byname(const char *part_name, uint *part_offset,
				    uint *part_size)
{
	disk_partition_t info = { 0 };
	if (sunxi_partition_get_info(part_name, &info) == 0) {
		*part_offset = info.start;
		*part_size  = info.size;
		return 0;
	}
	return -1;
}

uint sunxi_partition_get_offset_byname(const char *part_name)
{
	disk_partition_t info = { 0 };
	if (sunxi_partition_get_info(part_name, &info) == 0) {
		return info.start;
	}
	return 0;
}

/* for blk read/write */
int sunxi_flash_try_partition(struct blk_desc *desc, const char *str,
			      disk_partition_t *info)
{
	int i, ret;
	char temp_part_name[16] = {0};
#if 0
	char *gpt_signature[512] = {0};
	printf("\n");
	debug("line:%d:gpt_header_size %d\n", __LINE__, sizeof(gpt_header));
	if (sunxi_flash_phyread(1, 1, (void *)gpt_signature) <= 0) {
		printf("line:%d:sunxi_flash_read fail\n", __LINE__);
	}
	if (strncmp((const char *)(gpt_signature), GPT_SIGNATURE, sizeof(GPT_SIGNATURE))) {
		debug("line:%d:get gpt partition fail\n", __LINE__);
		debug("line:%d:try to mbr partition\n", __LINE__);
		if (get_boot_storage_type() == STORAGE_SD) {
			u32 start, size;
			if (sunxi_partition_get_info_byname(str, &start, &size))
				return -1;
			strcpy((char *)info->name, str);
			info->start = start;
			info->size  = size;

			return 0;
		}
	}
	printf("line:%d:get gpt partition success\n", __LINE__);
#endif
	sunxi_replace_android_ab_system((char *)str, temp_part_name);

	for (i = 1;; i++) {
#if defined (CONFIG_ENABLE_MTD_CMDLINE_PARTS_BY_ENV) /*Get partitiones by env*/
		ret = sunxi_partition_parse_get_info(i, info);
#else /* Get partitiones by GPT */
		ret = part_get_info(desc, i, info);
		debug("%s: try part %d, ret = %d\n", __func__, i, ret);

#endif
		if (ret < 0)
			return ret;

		if (!strncmp((const char *)info->name, temp_part_name, sizeof(info->name)))
			break;
		else if (!strncmp((const char *)info->name, (char *)str, sizeof(info->name)))
			break;
	}
	return 0;
}

/* for sunxi_flash_read/write */
int sunxi_partition_get_info(const char *part_name, disk_partition_t *info)
{
	struct blk_desc *desc;
	int ret;
	int storage_type;
	int logic_offset;

	storage_type = get_boot_storage_type();
	if (get_boot_work_mode() == WORK_MODE_CARD_PRODUCT ||
		storage_type == STORAGE_EMMC || storage_type == STORAGE_EMMC3
		|| storage_type == STORAGE_SD || storage_type == STORAGE_EMMC0) {
		logic_offset = MMC_LOGICAL_OFFSET;
	} else {
		logic_offset = 0;
	}

	desc = blk_get_devnum_by_typename("sunxi_flash", 0);
	if (desc == NULL) {
		debug("%s: get desc fail\n", __func__);
		ret = -ENODEV;
		goto __err;
	}
	ret = sunxi_flash_try_partition(desc, part_name, info);
	if (ret < 0) {
		debug("%s: get partition info fail\n", __func__);
		ret = -ENODEV;
		goto __err;
	}
	debug("name:%s start:0x%x, size: 0x%x\n", info->name, (u32)info->start,
		(u32)info->size);
	/* conver gpt info to sunxi_part */
	/* gpt part use phy address */
	/* sunxi part use logic address */
	info->start -= logic_offset;

	return 0;
__err:
	return ret;

}

/* for card sprite mode*/
lbaint_t sunxi_partition_get_offset(int part_index)
{

	if (get_boot_work_mode() != WORK_MODE_CARD_PRODUCT) {
		printf("****not support*****\n");
		return (lbaint_t)(-1);
	}

#ifdef CONFIG_ENABLE_MTD_CMDLINE_PARTS_BY_ENV
		disk_partition_t info;
		int ret;
		ret = sunxi_partition_parse_get_info(part_index, &info);
		if (ret == 0) {
			debug(" mbr->array[%d].name=%s\n", part_index,
			       info.name);
			debug(" mbr->array[%d].lenlo=0x%x\n", part_index,
			       (u32)info.size);
			debug("mbr->array[%d].addrlo=0x%x\n", part_index,
			       (u32)info.start);
			return (lbaint_t)info.start;
		}
#else
	sunxi_partition_init();
	if (mbr->PartCount && part_index <= mbr->PartCount) {
		debug(" mbr->array[%d].name=%s\n", part_index,
		       mbr->array[part_index].name);
		debug(" mbr->array[%d].lenlo=%d\n", part_index,
		       mbr->array[part_index].lenlo);
		debug("mbr->array[%d].addrlo=%d\n", part_index,
		       mbr->array[part_index].addrlo);
		return (lbaint_t)mbr->array[part_index].addrlo;
	}

#endif
	return (lbaint_t)(-1);
}
