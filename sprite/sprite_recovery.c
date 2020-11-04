/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Char <yanjianbo@allwinnertech.com>
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
 */
#include <config.h>
#include <common.h>
#include <malloc.h>
#include <sunxi_board.h>
#include "sprite_card.h"
#include "sprite_download.h"
#include "./firmware/imgdecode.h"
#include "./firmware/imagefile_new.h"
#include <sys_config.h>
#include <fdt_support.h>
#include "./cartoon/sprite_cartoon.h"
#include <sys_partition.h>

extern uint img_file_start;
extern int sunxi_sprite_deal_part_from_sysrevoery(sunxi_download_info *dl_map);
extern int __imagehd(HIMAGE tmp_himage);
extern int char8_char16_compare(const char *char8, const efi_char16_t *char16,
				size_t char16_len);

typedef struct tag_IMAGE_HANDLE {
	ImageHead_t ImageHead;
	ImageItem_t *ItemTable;
} IMAGE_HANDLE;

static HIMAGE Img_Open_from_sysrecovery(__u32 start)
{
	IMAGE_HANDLE *pImage = NULL;
	uint ItemTableSize;

	img_file_start = start;
	if (!img_file_start) {
		pr_err("sunxi sprite error: unable to get firmware start position\n");

		return NULL;
	}
	debug("img start = 0x%x\n", img_file_start);
	pImage = (IMAGE_HANDLE *)malloc(sizeof(IMAGE_HANDLE));
	if (NULL == pImage) {
		pr_err("sunxi sprite error: fail to malloc memory for img head\n");

		return NULL;
	}
	memset(pImage, 0, sizeof(IMAGE_HANDLE));

	//debug("try to read mmc start %d\n", img_file_start);
	if (!sunxi_flash_read(img_file_start, IMAGE_HEAD_SIZE / 512,
			      &pImage->ImageHead)) {
		pr_err("sunxi sprite error: read iamge head fail\n");

		goto _img_open_fail_;
	}
	debug("read mmc ok\n");

	if (memcmp(pImage->ImageHead.magic, IMAGE_MAGIC, 8) != 0) {
		pr_err("sunxi sprite error: iamge magic is bad\n");

		goto _img_open_fail_;
	}

	ItemTableSize     = pImage->ImageHead.itemcount * sizeof(ImageItem_t);
	pImage->ItemTable = (ImageItem_t *)malloc(ItemTableSize);
	if (NULL == pImage->ItemTable) {
		pr_err("sunxi sprite error: fail to malloc memory for item table\n");

		goto _img_open_fail_;
	}

	if (!sunxi_flash_read(img_file_start + (IMAGE_HEAD_SIZE / 512),
			      ItemTableSize / 512, pImage->ItemTable)) {
		pr_err("sunxi sprite error: read iamge item table fail\n");

		goto _img_open_fail_;
	}

	return pImage;

_img_open_fail_:
	if (pImage->ItemTable) {
		free(pImage->ItemTable);
	}
	if (pImage) {
		free(pImage);
	}

	return NULL;
}

static int sprite_erase_partition_by_name(char *part_name)
{
	int ret = -1;
	__u32 img_start;
	__u32 part_size;

	ret = sunxi_partition_get_info_byname(part_name, &img_start,
					      &part_size);
	if (ret) {
		pr_msg("sprite update error: no %s found\n", part_name);
		if (!strcmp(part_name, "data")) {
			ret = sunxi_partition_get_info_byname(
				"UDISK", &img_start, &part_size);
			if (ret) {
				pr_err("sprite update error: no udisk partition\n");
				return -1;
			}
		}
	}

	if (!ret) {
		__u32 tmp_size;
		__u32 tmp_start;
		char *src_buf = NULL;

		src_buf = (char *)malloc(1024 * 1024);
		if (!src_buf) {
			pr_err("sprite erase error: fail to get memory for tmpdata\n");
			return -1;
		}

		tmp_start = img_start;
		tmp_size  = part_size;
		pr_msg("data part size=%d\n", tmp_size);
		tick_printf("begin erase part %s\n", part_name);
		memset(src_buf, 0xff, 1024 * 1024);

		sunxi_flash_write(tmp_start, 1024 * 1024 / 512, src_buf);

		if (src_buf) {
			free(src_buf);
		}

		tick_printf("finish erase part\n");
	}

	return 0;
}

static int sprite_erase_in_sysrecovery(void)
{
	int ret;
	ret = sprite_erase_partition_by_name("data");
	if (ret) {
		pr_err("sprite update error: fail to erase data\n");
		return -1;
	}

	ret = sprite_erase_partition_by_name("cache");
	if (ret) {
		pr_err("sprite update error: fail to erase cache\n");
		return -1;
	}

	ret = sprite_erase_partition_by_name("Reserve0");
	if (ret) {
		pr_err("sprite update error: fail to erase cache\n");
		return -1;
	}

	ret = sprite_erase_partition_by_name("Reserve1");
	if (ret) {
		pr_err("sprite update error: fail to erase cache\n");
		return -1;
	}

	ret = sprite_erase_partition_by_name("Reserve2");
	if (ret) {
		pr_err("sprite update error: fail to erase cache\n");
		return -1;
	}
	return 0;
}

int sprite_form_sysrecovery(void)
{
	HIMAGEITEM imghd = 0;
	__u32 part_size;
	__u32 img_start;
	sunxi_download_info *dl_info = NULL;
	char *src_buf		     = NULL;
	int ret			     = -1;
	int production_media	 = get_boot_storage_type();

#ifdef CONFIG_SUNXI_SPRITE_CARTOON
	int nodeoffset;
	int processbar_direct = 0;
	;
	nodeoffset = fdt_path_offset(working_fdt, FDT_PATH_CARD_BOOT);
	if (nodeoffset >= 0) {
		if (fdt_getprop_u32(working_fdt, nodeoffset,
				    "processbar_direct",
				    (uint32_t *)&processbar_direct) < 0)
			processbar_direct = 0;
	}

	sprite_cartoon_create(processbar_direct);
#endif
	tick_printf("sunxi sprite begin\n");

	src_buf = (char *)malloc(1024 * 1024);
	if (!src_buf) {
		pr_err("sprite update error: fail to get memory for tmpdata\n");
		goto _update_error_;
	}

	ret = sunxi_partition_get_info_byname("sysrecovery", &img_start, &part_size);
	if (ret) {
		pr_err("sprite update error: read image start error\n");
		goto _update_error_;
	}
	tick_printf("part start = %d\n", img_start);
	imghd = Img_Open_from_sysrecovery(img_start);
	if (!imghd) {
		pr_err("sprite update error: fail to open img\n");
		goto _update_error_;
	}
	__imagehd(imghd);

#ifdef CONFIG_SUNXI_SPRITE_CARTOON
	sprite_cartoon_upgrade(10);
#endif

	//erase the data partition.
	ret = sprite_erase_in_sysrecovery();
	if (ret) {
		goto _update_error_;
	}

	dl_info = (sunxi_download_info *)malloc(sizeof(sunxi_download_info));
	if (!dl_info) {
		pr_err("sprite update error: fail to get memory for download map\n");
		goto _update_error_;
	}
	memset(dl_info, 0, sizeof(sunxi_download_info));

	ret = sprite_card_fetch_download_map(dl_info);
	if (ret) {
		pr_err("sunxi sprite error: donn't download dl_map\n");
		goto _update_error_;
	}
#ifdef CONFIG_SUNXI_SPRITE_CARTOON
	sprite_cartoon_upgrade(20);
#endif

	if (sunxi_sprite_deal_part_from_sysrevoery(dl_info)) {
		pr_err("sunxi sprite error : download part error\n");
		goto _update_error_;
	}

	if (sunxi_sprite_deal_recorvery_boot(production_media)) {
		pr_err("recovery error : download uboot or boot0 error!\n");
		goto _update_error_;
	}
	tick_printf("successed in downloading uboot and boot0\n");
#ifdef CONFIG_SUNXI_SPRITE_CARTOON
	sprite_cartoon_upgrade(100);
#endif
	mdelay(3000);

	Img_Close(imghd);
	if (dl_info) {
		free(dl_info);
	}
	if (src_buf) {
		free(src_buf);
	}

	sunxi_board_restart(0);
	return 0;

_update_error_:
	if (dl_info) {
		free(dl_info);
	}
	if (src_buf) {
		free(src_buf);
	}
	pr_err("sprite update error: current card sprite failed\n");
	pr_err("now hold the machine\n");
	return -1;
}
