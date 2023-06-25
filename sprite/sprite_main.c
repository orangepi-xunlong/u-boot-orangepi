/*
 *  * Copyright 2000-2009
 *   * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *    *
 *     * SPDX-License-Identifier:	GPL-2.0+
 *     */
#include <config.h>
#include <common.h>
#include <sunxi_mbr.h>
#include <sys_config.h>
#include "sprite_card.h"
#include "sprite_download.h"
//#include "sprite_erase.h"
#include <private_boot0.h>
#include <malloc.h>
#include <sunxi_board.h>
#include <fdt_support.h>
#include "sunxi_flash.h"
#include "./cartoon/sprite_cartoon.h"
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void __dump_dlmap(sunxi_download_info *dl_info)
{
	dl_one_part_info *part_info;
	u32 i;
	char buffer[32];

	printf("*************DOWNLOAD MAP DUMP************\n");
	printf("total download part %d\n", dl_info->download_count);
	printf("\n");
	for (part_info = dl_info->one_part_info, i = 0;
	     i < dl_info->download_count; i++, part_info++) {
		memset(buffer, 0, 32);
		memcpy(buffer, part_info->name, 16);
		printf("download part[%d] name          :%s\n", i, buffer);
		memset(buffer, 0, 32);
		memcpy(buffer, part_info->dl_filename, 16);
		printf("download part[%d] download file :%s\n", i, buffer);
		memset(buffer, 0, 32);
		memcpy(buffer, part_info->vf_filename, 16);
		printf("download part[%d] verify file   :%s\n", i, buffer);
		printf("download part[%d] lenlo         :0x%x\n", i,
		       part_info->lenlo);
		printf("download part[%d] addrlo        :0x%x\n", i,
		       part_info->addrlo);
		printf("download part[%d] encrypt       :0x%x\n", i,
		       part_info->encrypt);
		printf("download part[%d] verify        :0x%x\n", i,
		       part_info->verify);
		printf("\n");
	}
}

#ifdef CONFIG_DISABLE_SUNXI_PART_DOWNLOAD
void __modify_dlmap_to_disable_mbr(sunxi_download_info *dl_info)
{
	dl_one_part_info *part_info;
	u32 i;
	printf("*************modify dlmay to disable mbr************\n");
	for (part_info = dl_info->one_part_info, i = 0;
	     i < dl_info->download_count; i++, part_info++) {
		part_info->addrlo -= SUNXI_MBR_SIZE / 512;
	}
}
#endif

void __dump_mbr(sunxi_mbr_t *mbr_info)
{
	sunxi_partition *part_info;
	u32 i;
	char buffer[32];

	printf("*************MBR DUMP***************\n");
	printf("total mbr part %d\n", mbr_info->PartCount);
	printf("\n");
	for (part_info = mbr_info->array, i = 0; i < mbr_info->PartCount;
	     i++, part_info++) {
		memset(buffer, 0, 32);
		memcpy(buffer, part_info->name, 16);
		printf("part[%d] name      :%s\n", i, buffer);
		memset(buffer, 0, 32);
		memcpy(buffer, part_info->classname, 16);
		printf("part[%d] classname :%s\n", i, buffer);
		printf("part[%d] addrlo    :0x%x\n", i, part_info->addrlo);
		printf("part[%d] lenlo     :0x%x\n", i, part_info->lenlo);
		printf("part[%d] user_type :0x%x\n", i, part_info->user_type);
		printf("part[%d] keydata   :0x%x\n", i, part_info->keydata);
		printf("part[%d] ro        :0x%x\n", i, part_info->ro);
		printf("\n");
	}
}

/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :	workmode: 升级模式选择：0，卡上固件形式升级；1，文件形式升级
*
*						name    : 文件升级时的名词
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_card_sprite_main(int workmode, char *name)
{
	int production_media; //升级介质
	uchar *img_mbr; //mbr
	sunxi_download_info *dl_map; //dlinfo
	int sprite_next_work;
	int nodeoffset;
	int processbar_direct = 0;
	dl_map = (sunxi_download_info *)memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN(sizeof(sunxi_download_info), CONFIG_SYS_CACHELINE_SIZE));
	img_mbr = (uchar *)memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN(1024*1024, CONFIG_SYS_CACHELINE_SIZE));
	memset(img_mbr, 0, ALIGN(1024*1024, CONFIG_SYS_CACHELINE_SIZE));
	memset(dl_map, 0, ALIGN(sizeof(sunxi_download_info), CONFIG_SYS_CACHELINE_SIZE));
	tick_printf("sunxi sprite begin\n");
	nodeoffset = fdt_path_offset(working_fdt, FDT_PATH_CARD_BOOT);

	if (nodeoffset >= 0) {
		if (fdt_getprop_u32(working_fdt, nodeoffset,
				    "processbar_direct",
				    (uint32_t *)&processbar_direct) < 0)
			processbar_direct = 0;
	}

	//获取当前是量产介质是nand或者卡
	production_media = get_boot_storage_type();
	//启动动画显示
#ifdef CONFIG_SUNXI_SPRITE_CARTOON
		sprite_cartoon_create(processbar_direct);
#endif
	//检查固件合法性
	if (sprite_card_firmware_probe(name)) {
		printf("sunxi sprite firmware probe fail\n");

		return -1;
	}
#ifdef CONFIG_SUNXI_SPRITE_CARTOON
		sprite_cartoon_upgrade(5);
#endif
	tick_printf("firmware probe ok\n");
	//获取dl_map文件，用于指引下载的数据
	tick_printf("fetch download map\n");
	if (sprite_card_fetch_download_map(dl_map)) {
		printf("sunxi sprite error : fetch download map error\n");

		return -1;
	}
#ifdef CONFIG_DISABLE_SUNXI_PART_DOWNLOAD
	__modify_dlmap_to_disable_mbr(dl_map);
#endif
	__dump_dlmap(dl_map);
	//获取mbr
	tick_printf("fetch mbr\n");
	if (sprite_card_fetch_mbr(img_mbr)) {
		printf("sunxi sprite error : fetch mbr error\n");

		return -1;
	}
	__dump_mbr((sunxi_mbr_t *)img_mbr);
	//根据mbr，决定擦除时候是否要保留数据
	tick_printf("begin to erase flash\n");
#ifdef CONFIG_SUNXI_NAND
	 nand_get_mbr((char *)img_mbr, 16 * 1024);
#endif
	if (sunxi_sprite_erase_flash(img_mbr)) {
		printf("sunxi sprite error: erase flash err\n");

		return -1;
	}

#ifndef CONFIG_DISABLE_SUNXI_PART_DOWNLOAD
	int mbr_num	   = SUNXI_MBR_COPY_NUM;
	tick_printf("successed in erasing flash\n");
	if (production_media == STORAGE_NOR) {
		mbr_num = 1;
	}
	if (sunxi_sprite_download_mbr(img_mbr, ALIGN(sizeof(sunxi_mbr_t) * mbr_num, CONFIG_SYS_CACHELINE_SIZE))) {
		printf("sunxi sprite error: download mbr err\n");

		return -1;
	}
#endif

#ifdef CONFIG_SUNXI_SPRITE_CARTOON
		sprite_cartoon_upgrade(10);
#endif
	tick_printf("begin to download part\n");
	//开始烧写分区
	if (sunxi_sprite_deal_part(dl_map)) {
		printf("sunxi sprite error : download part error\n");

		return -1;
	}
	tick_printf("successed in downloading part\n");
#ifdef CONFIG_SUNXI_SPRITE_CARTOON
		sprite_cartoon_upgrade(80);
#endif
	get_boot_storage_type() == STORAGE_NAND ? sunxi_sprite_exit(1) : 0;

	if (sunxi_sprite_deal_uboot(production_media)) {
		printf("sunxi sprite error : download uboot error\n");

		return -1;
	}
	tick_printf("successed in downloading uboot\n");
#ifdef CONFIG_SUNXI_SPRITE_CARTOON
		sprite_cartoon_upgrade(90);
#endif
	if (sunxi_sprite_deal_boot0(production_media)) {
		printf("sunxi sprite error : download boot0 error\n");

		return -1;
	}
	tick_printf("successed in downloading boot0\n");
#ifdef CONFIG_SUNXI_SPRITE_CARTOON
		sprite_cartoon_upgrade(100);
#endif
	printf("CARD OK\n");
	tick_printf("sprite success \n");
	//烧写结束
	mdelay(3000);
	//处理烧写完成后的动作

	if (fdt_getprop_u32(working_fdt, nodeoffset, "next_work",
			    (uint32_t *)&sprite_next_work) < 0)
	//if(script_parser_fetch("card_boot", "next_work", &sprite_next_work, 1))
	{
		sprite_next_work = SUNXI_UPDATE_NEXT_ACTION_SHUTDOWN;
		//sprite_next_work = SUNXI_UPDATE_NEXT_ACTION_REUPDATE;
	}
	if (sprite_next_work == SUNXI_UPDATA_NEXT_ACTION_SPRITE_TEST) {
		//erase the magic from boot0 head
		printf("try to earse boot0 head :");
		char *boot0_head_buf	  = NULL;
		boot0_file_head_t *boot0_head = NULL;
		boot0_head_buf		      = (char *)memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN(1024, CONFIG_SYS_CACHELINE_SIZE));
		memset(boot0_head_buf, 0x00, ALIGN(1024, CONFIG_SYS_CACHELINE_SIZE));
		if (!sunxi_flash_phyread(BOOT0_SDMMC_START_ADDR, 1024 / 512,
					 boot0_head_buf)) {
			printf("sunxi_sprite_error : read boot0 head failed\n");
		}
		boot0_head = (boot0_file_head_t *)boot0_head_buf;
		printf("%s \n", boot0_head->boot_head.magic);
		memset(boot0_head->boot_head.magic, 0x00, 8);
		printf("=====check_sum======%x \n",
		       boot0_head->boot_head.check_sum);
		if (!sunxi_flash_phywrite(BOOT0_SDMMC_START_ADDR, 1024 / 512,
					  boot0_head_buf)) {
			printf("sunxi_sprite_error : can not write boot0 head into card \n");
		}
	}
	printf("sprite_next_work=%d\n", sprite_next_work);
	sunxi_update_subsequent_processing(sprite_next_work);

	return 0;
}
