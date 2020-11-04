
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

static sunxi_mbr_t *mbr ;
#define MMC_LOGICAL_OFFSET   (20 * 1024 * 1024/512)
DECLARE_GLOBAL_DATA_PTR;


int sunxi_partition_init(void)
{
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
	return 0;
}

int sunxi_partition_get_partno_byname(const char *part_name)
{
	int i;
	struct blk_desc *desc;
	int ret;
	disk_partition_t info;

	desc = blk_get_devnum_by_typename("sunxi_flash", 0);
	if (desc == NULL) {
		printf("%s: get desc fail\n", __func__);
		ret = -ENODEV;
	}

	for (i = 1;; i++) {
		ret = part_get_info(desc, i, &info);
		debug("%s: try part %d, ret = %d\n", __func__, i, ret);
		if (ret < 0) {
			printf("partno erro : can't find partition %s\n", part_name);
			return ret;
		}
		if (!strncmp((const char *)info.name, part_name, sizeof(info.name))) {
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
	for (i = 1;; i++) {
		ret = part_get_info(desc, i, info);
		debug("%s: try part %d, ret = %d\n", __func__, i, ret);
		if (ret < 0)
			return ret;

		if (!strncmp((const char *)info->name, str, sizeof(info->name)))
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
		|| storage_type == STORAGE_SD) {
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

	return (lbaint_t)(-1);
}
