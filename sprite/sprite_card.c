/*
 *  * Copyright 2000-2009
 *   * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *    *
 *     * SPDX-License-Identifier:	GPL-2.0+
 *     */
#include <config.h>
#include <common.h>
#include <malloc.h>
#include "sparse/sparse.h"
//#include <asm/arch/queue.h>
#include <sunxi_mbr.h>
#include <sys_partition.h>
#include <private_boot0.h>
#include <private_uboot.h>
#include <private_toc.h>
//#include "encrypt/encrypt.h"
//#include "sprite_queue.h"
#include "sprite_download.h"
#include "sprite_verify.h"
#include "firmware/imgdecode.h"
#include "dos_part.h"
#include "private_uboot.h"
#include <sunxi_flash.h>
#include <mmc.h>
#include <sys_config.h>
#include <private_boot0.h>
#include <sunxi_board.h>
#include "sprite.h"


#define SPRITE_CARD_HEAD_BUFF (32 * 1024)
#if defined(CONFIG_SUNXI_SPINOR)
#define SPRITE_CARD_ONCE_DATA_DEAL (2 * 1024 * 1024)
#else
#define SPRITE_CARD_ONCE_DATA_DEAL (16 * 1024 * 1024)
#endif
#define SPRITE_CARD_ONCE_SECTOR_DEAL (SPRITE_CARD_ONCE_DATA_DEAL / 512)


static void *imghd;
static void *imgitemhd;

DECLARE_GLOBAL_DATA_PTR;

//extern int sunxi_flash_mmc_phywipe(unsigned long start_block, unsigned long nblock, unsigned long *skip);
static int __download_normal_part(dl_one_part_info *part_info,
				  uchar *source_buff);
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
lbaint_t sprite_card_firmware_start(void)
{
	return sunxi_partition_get_offset(1);
}
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
int sprite_card_firmware_probe(char *name)
{
	printf("firmware name %s\n", name);
	imghd = Img_Open(name);
	if (!imghd) {
		return -1;
	}

	return 0;
}
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
int sprite_card_fetch_download_map(sunxi_download_info *dl_map)
{
	imgitemhd = Img_OpenItem(imghd, "12345678", "1234567890DLINFO");
	if (!imgitemhd) {
		return -1;
	}
	debug("try to read item dl map\n");
	if (!Img_ReadItem(imghd, imgitemhd, (void *)dl_map,
			  sizeof(sunxi_download_info))) {
		printf("sunxi sprite error : read dl map failed\n");

		return -1;
	}
	Img_CloseItem(imghd, imgitemhd);
	imgitemhd = NULL;
	//检查获取的dlinfo是否正确
	return sunxi_sprite_verify_dlmap(dl_map);
}
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
int sprite_card_fetch_mbr(void *img_mbr)
{
	int mbr_num = SUNXI_MBR_COPY_NUM;

	if (get_boot_storage_type() == STORAGE_NOR) {
		mbr_num = 1;
	}

	imgitemhd = Img_OpenItem(imghd, "12345678", "1234567890___MBR");
	if (!imgitemhd) {
		return -1;
	}
	debug("try to read item mbr\n");
	if (!Img_ReadItem(imghd, imgitemhd, img_mbr,
			  sizeof(sunxi_mbr_t) * mbr_num)) {
		printf("sunxi sprite error : read mbr failed\n");

		return -1;
	}
	Img_CloseItem(imghd, imgitemhd);
	imgitemhd = NULL;

	return sunxi_sprite_verify_mbr(img_mbr);
}
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
static int __download_udisk(dl_one_part_info *part_info, uchar *source_buff)
{
	HIMAGEITEM imgitemhd = NULL;
	u32 flash_sector;
	s64 packet_len;
	s32 ret = -1, ret1;

	//打开分区镜像
	imgitemhd =
		Img_OpenItem(imghd, "RFSFAT16", (char *)part_info->dl_filename);
	if (!imgitemhd) {
		printf("sunxi sprite error: open part %s failed\n",
		       part_info->dl_filename);

		return -1;
	}
	//获取分区镜像字节数
	packet_len = Img_GetItemSize(imghd, imgitemhd);
	if (packet_len <= 0) {
		printf("sunxi sprite error: fetch part len %s failed\n",
		       part_info->dl_filename);

		goto __download_udisk_err1;
	}
	if (packet_len <= CONFIG_FW_BURN_UDISK_MIN_SIZE) {
		printf("download UDISK: the data length of udisk is too small, ignore it\n");

		ret = 1;
		goto __download_udisk_err1;
	}
	//分区镜像够大，需要进行烧录
	flash_sector = sunxi_sprite_size();
	if (!flash_sector) {
		printf("sunxi sprite error: download_udisk, the flash size is invalid(0)\n");

		goto __download_udisk_err1;
	}
	printf("the flash size is %d MB\n",
	       flash_sector / 2 / 1024); //计算出M单位
	part_info->lenlo = flash_sector - part_info->addrlo;
	part_info->lenhi = 0;
	printf("UDISK low is 0x%x Sectors\n", part_info->lenlo);
	printf("UDISK high is 0x%x Sectors\n", part_info->lenhi);

	ret = __download_normal_part(part_info, source_buff);
__download_udisk_err1:
	ret1 = Img_CloseItem(imghd, imgitemhd);
	if (ret1 != 0) {
		printf("sunxi sprite error: __download_udisk, close udisk image failed\n");

		return -1;
	}

	return ret;
}
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
static int __download_normal_part(dl_one_part_info *part_info,
				  uchar *source_buff)
{
	uint partstart_by_sector; //分区起始扇区
	uint tmp_partstart_by_sector;

	s64 partsize_by_byte; //分区大小(字节单位)

	s64 partdata_by_byte; //需要下载的分区数据(字节单位)
	s64 tmp_partdata_by_bytes;

	uint onetime_read_sectors; //一次读写的扇区数
	uint first_write_bytes;

	uint imgfile_start; //分区数据所在的扇区
	uint tmp_imgfile_start;

	u8 *down_buffer = source_buff + SPRITE_CARD_HEAD_BUFF;

	int partdata_format;

	int ret = -1;
	//*******************************************************************
	//获取分区起始扇区

	tmp_partstart_by_sector = partstart_by_sector = part_info->addrlo;
	debug("line:%d tmp_partstart_by_sector=0x%x\n", __LINE__,
	      tmp_partstart_by_sector);
	//获取分区大小，字节数
	partsize_by_byte = part_info->lenlo;
	partsize_by_byte <<= 9;
	debug("line:%d partsize_by_byte=0x%llx\n", __LINE__, partsize_by_byte);
	//打开分区镜像
	debug("line:%d part_info->dl_filename=%s\n", __LINE__,
	      part_info->dl_filename);
	imgitemhd =
		Img_OpenItem(imghd, "RFSFAT16", (char *)part_info->dl_filename);
	if (!imgitemhd) {
		printf("sunxi sprite error: open part %s failed\n",
		       part_info->dl_filename);

		return -1;
	}
	//获取分区镜像字节数
	partdata_by_byte = Img_GetItemSize(imghd, imgitemhd);
	if (partdata_by_byte <= 0) {
		printf("sunxi sprite error: fetch part len %s failed\n",
		       part_info->dl_filename);

		goto __download_normal_part_err1;
	}
	printf("partdata hi 0x%x\n", (uint)(partdata_by_byte >> 32));
	printf("partdata lo 0x%x\n", (uint)partdata_by_byte);
	//如果分区数据超过分区大小
	if (partdata_by_byte > partsize_by_byte) {
		printf("sunxi sprite: data size 0x%x is larger than part %s size 0x%x\n",
		       (uint)(partdata_by_byte / 512), part_info->dl_filename,
		       (uint)(partsize_by_byte / 512));

		goto __download_normal_part_err1;
	}
	//准备读取分区镜像数据
	tmp_partdata_by_bytes = partdata_by_byte;
	if (tmp_partdata_by_bytes >= SPRITE_CARD_ONCE_DATA_DEAL) {
		onetime_read_sectors = SPRITE_CARD_ONCE_SECTOR_DEAL;
		first_write_bytes    = SPRITE_CARD_ONCE_DATA_DEAL;
		debug("line:%d onetime_read_sectors=0x%x\n", __LINE__,
		      onetime_read_sectors);
		debug("line:%d first_write_bytes=0x%x\n", __LINE__,
		      first_write_bytes);
	} else {
		onetime_read_sectors = (tmp_partdata_by_bytes + 511) >> 9;
		first_write_bytes    = (uint)tmp_partdata_by_bytes;
		debug("line:%d onetime_read_sectors=0x%x\n", __LINE__,
		      onetime_read_sectors);
		debug("line:%d first_write_bytes=0x%x\n", __LINE__,
		      first_write_bytes);
	}
	//开始获取分区数据
	imgfile_start = Img_GetItemStart(imghd, imgitemhd);
	debug("line:%d imgfile_start=tmp_imgfile_start=0x%x\n", __LINE__,
	      imgfile_start);
	if (!imgfile_start) {
		printf("sunxi sprite err : cant get part data imgfile_start %s\n",
		       part_info->dl_filename);

		goto __download_normal_part_err1;
	}
	tmp_imgfile_start = imgfile_start;
	//读出第一笔固件中的分区数据，大小为buffer字节数
	if (sunxi_flash_read(tmp_imgfile_start, onetime_read_sectors,
			     down_buffer) != onetime_read_sectors) {
		printf("sunxi sprite error : read sdcard block %d, total %d failed\n",
		       tmp_imgfile_start, onetime_read_sectors);

		goto __download_normal_part_err1;
	}
	//下一个要读出的数据
	tmp_imgfile_start += onetime_read_sectors;
	//尝试查看是否sparse格式
	partdata_format = unsparse_probe((char *)down_buffer, first_write_bytes,
					 partstart_by_sector); //判断数据格式
	if (partdata_format != ANDROID_FORMAT_DETECT) {
		//写入第一笔数据
		if (sunxi_sprite_write(tmp_partstart_by_sector,
				       onetime_read_sectors,
				       down_buffer) != onetime_read_sectors) {
			printf("sunxi sprite error: download rawdata error %s\n",
			       part_info->dl_filename);

			goto __download_normal_part_err1;
		}
		tmp_partdata_by_bytes -= first_write_bytes;
		tmp_partstart_by_sector += onetime_read_sectors;

		while (tmp_partdata_by_bytes >= SPRITE_CARD_ONCE_DATA_DEAL) {
			//继续读出固件中的分区数据，大小为buffer字节数
			if (sunxi_flash_read(tmp_imgfile_start,
					     SPRITE_CARD_ONCE_SECTOR_DEAL,
					     down_buffer) !=
			    SPRITE_CARD_ONCE_SECTOR_DEAL) {
				printf("sunxi sprite error : read sdcard block %d, total %d failed\n",
				       tmp_imgfile_start,
				       SPRITE_CARD_ONCE_SECTOR_DEAL);

				goto __download_normal_part_err1;
			}
			//写入flash
			if (sunxi_sprite_write(tmp_partstart_by_sector,
					       SPRITE_CARD_ONCE_SECTOR_DEAL,
					       down_buffer) !=
			    SPRITE_CARD_ONCE_SECTOR_DEAL) {
				printf("sunxi sprite error: download rawdata error %s, start 0x%x, sectors 0x%x\n",
				       part_info->dl_filename,
				       tmp_partstart_by_sector,
				       SPRITE_CARD_ONCE_SECTOR_DEAL);

				goto __download_normal_part_err1;
			}
			tmp_imgfile_start += SPRITE_CARD_ONCE_SECTOR_DEAL;
			tmp_partdata_by_bytes -= SPRITE_CARD_ONCE_DATA_DEAL;
			tmp_partstart_by_sector += SPRITE_CARD_ONCE_SECTOR_DEAL;
		}
		if (tmp_partdata_by_bytes > 0) {
			uint rest_sectors = (tmp_partdata_by_bytes + 511) >> 9;
			//继续读出固件中的分区数据，大小为buffer字节数
			if (sunxi_flash_read(tmp_imgfile_start, rest_sectors,
					     down_buffer) != rest_sectors) {
				printf("sunxi sprite error : read sdcard block %d, total %d failed\n",
				       tmp_imgfile_start, rest_sectors);

				goto __download_normal_part_err1;
			}
			//写入flash
			if (sunxi_sprite_write(tmp_partstart_by_sector,
					       rest_sectors,
					       down_buffer) != rest_sectors) {
				printf("sunxi sprite error: download rawdata error %s, start 0x%x, sectors 0x%x\n",
				       part_info->dl_filename,
				       tmp_partstart_by_sector, rest_sectors);

				goto __download_normal_part_err1;
			}
		}
	} else {
		if (unsparse_direct_write(down_buffer, first_write_bytes)) {
			printf("sunxi sprite error: download sparse error %s\n",
			       part_info->dl_filename);

			goto __download_normal_part_err1;
		}
		tmp_partdata_by_bytes -= first_write_bytes;

		while (tmp_partdata_by_bytes >= SPRITE_CARD_ONCE_DATA_DEAL) {
			//继续读出固件中的分区数据，大小为buffer字节数
			if (sunxi_flash_read(tmp_imgfile_start,
					     SPRITE_CARD_ONCE_SECTOR_DEAL,
					     down_buffer) !=
			    SPRITE_CARD_ONCE_SECTOR_DEAL) {
				printf("sunxi sprite error : read sdcard block 0x%x, total 0x%x failed\n",
				       tmp_imgfile_start,
				       SPRITE_CARD_ONCE_SECTOR_DEAL);

				goto __download_normal_part_err1;
			}
			//写入flash
			if (unsparse_direct_write(down_buffer,
						  SPRITE_CARD_ONCE_DATA_DEAL)) {
				printf("sunxi sprite error: download sparse error %s\n",
				       part_info->dl_filename);

				goto __download_normal_part_err1;
			}
			tmp_imgfile_start += SPRITE_CARD_ONCE_SECTOR_DEAL;
			tmp_partdata_by_bytes -= SPRITE_CARD_ONCE_DATA_DEAL;
		}
		if (tmp_partdata_by_bytes > 0) {
			uint rest_sectors = (tmp_partdata_by_bytes + 511) >> 9;
			//继续读出固件中的分区数据，大小为buffer字节数
			if (sunxi_flash_read(tmp_imgfile_start, rest_sectors,
					     down_buffer) != rest_sectors) {
				printf("sunxi sprite error : read sdcard block 0x%x, total 0x%x failed\n",
				       tmp_imgfile_start, rest_sectors);

				goto __download_normal_part_err1;
			}
			//写入flash
			if (unsparse_direct_write(down_buffer,
						  tmp_partdata_by_bytes)) {
				printf("sunxi sprite error: download sparse error %s\n",
				       part_info->dl_filename);

				goto __download_normal_part_err1;
			}
		}
	}

	tick_printf("successed in writting part %s\n", part_info->name);
	ret = 0;
	if (imgitemhd) {
		Img_CloseItem(imghd, imgitemhd);
		imgitemhd = NULL;
	}
	//判断是否需要进行校验
	if (part_info->verify) {
		uint active_verify;
		uint origin_verify;
		uchar *verify_data;
		verify_data = (uchar *)memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN(1024, CONFIG_SYS_CACHELINE_SIZE));
		memset(verify_data, 0, ALIGN(1024, CONFIG_SYS_CACHELINE_SIZE));
		ret = -1;
		if (part_info->vf_filename[0]) {
			imgitemhd =
				Img_OpenItem(imghd, "RFSFAT16",
					     (char *)part_info->vf_filename);
			if (!imgitemhd) {
				printf("sprite update warning: open part %s failed\n",
				       part_info->vf_filename);

				goto __download_normal_part_err1;
			}
			//读出数据
			if (!Img_ReadItem(imghd, imgitemhd, (void *)verify_data,
					  1024)) {
				printf("sprite update warning: fail to read data from %s\n",
				       part_info->vf_filename);

				goto __download_normal_part_err1;
			}
			if (partdata_format == ANDROID_FORMAT_DETECT) {
				active_verify =
					sunxi_sprite_part_sparsedata_verify();
			} else {
				active_verify =
					sunxi_sprite_part_rawdata_verify(
						partstart_by_sector,
						partdata_by_byte);
			}
			{
				uint *tmp = (uint *)verify_data;

				origin_verify = *tmp;
			}
			printf("origin_verify value = %x, active_verify value = %x\n",
			       origin_verify, active_verify);
			if (origin_verify != active_verify) {
				printf("origin checksum=%x, active checksum=%x\n",
				       origin_verify, active_verify);
				printf("sunxi sprite: part %s verify error\n",
				       part_info->dl_filename);

				goto __download_normal_part_err1;
			}
			ret = 0;
		} else {
			printf("sunxi sprite err: part %s unablt to find verify file\n",
			       part_info->dl_filename);
		}
		tick_printf("successed in verify part %s\n", part_info->name);
		free(verify_data);
	} else {
		printf("sunxi sprite err: part %s not need to verify\n",
		       part_info->dl_filename);
	}

__download_normal_part_err1:
	if (imgitemhd) {
		Img_CloseItem(imghd, imgitemhd);
		imgitemhd = NULL;
	}
	return ret;
}
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
static int __download_sysrecover_part(dl_one_part_info *part_info,
				      uchar *source_buff)
{
	uint partstart_by_sector; //分区起始扇区
	uint tmp_partstart_by_sector;

	s64 partsize_by_byte; //分区大小(字节单位)

	s64 partdata_by_byte; //需要下载的分区数据(字节单位)
	s64 tmp_partdata_by_bytes;

	uint onetime_read_sectors; //一次读写的扇区数

	uint imgfile_start; //分区数据所在的扇区
	uint tmp_imgfile_start;

	u8 *down_buffer = source_buff + SPRITE_CARD_HEAD_BUFF;

	int ret = -1;
	//*******************************************************************
	//获取分区起始扇区
	tmp_partstart_by_sector = partstart_by_sector = part_info->addrlo;
	//获取分区大小，字节数
	partsize_by_byte = part_info->lenlo;
	partsize_by_byte <<= 9;
	//打开分区镜像

	//获取分区镜像字节数
	partdata_by_byte = Img_GetSize(imghd);
	if (partdata_by_byte <= 0) {
		printf("sunxi sprite error: fetch part len %s failed\n",
		       part_info->dl_filename);

		goto __download_sysrecover_part_err1;
	}
	//如果分区数据超过分区大小
	if (partdata_by_byte > partsize_by_byte) {
		printf("sunxi sprite: data size 0x%x is larger than part %s size 0x%x\n",
		       (uint)(partdata_by_byte / 512), part_info->dl_filename,
		       (uint)(partsize_by_byte / 512));

		goto __download_sysrecover_part_err1;
	}
	//准备读取分区镜像数据
	tmp_partdata_by_bytes = partdata_by_byte;
	if (tmp_partdata_by_bytes >= SPRITE_CARD_ONCE_DATA_DEAL) {
		onetime_read_sectors = SPRITE_CARD_ONCE_SECTOR_DEAL;
	} else {
		onetime_read_sectors = (tmp_partdata_by_bytes + 511) >> 9;
	}
	//开始获取分区数据
	imgfile_start = sprite_card_firmware_start();
	if (!imgfile_start) {
		printf("sunxi sprite err : cant get part data imgfile_start %s\n",
		       part_info->dl_filename);

		goto __download_sysrecover_part_err1;
	}
	tmp_imgfile_start = imgfile_start;

	while (tmp_partdata_by_bytes >= SPRITE_CARD_ONCE_DATA_DEAL) {
		//继续读出固件中的分区数据，大小为buffer字节数
		if (sunxi_flash_read(tmp_imgfile_start, onetime_read_sectors,
				     down_buffer) != onetime_read_sectors) {
			printf("sunxi sprite error : read sdcard block %d, total %d failed\n",
			       tmp_imgfile_start, onetime_read_sectors);

			goto __download_sysrecover_part_err1;
		}
		//写入flash
		if (sunxi_sprite_write(tmp_partstart_by_sector,
				       onetime_read_sectors,
				       down_buffer) != onetime_read_sectors) {
			printf("sunxi sprite error: download rawdata error %s, start 0x%x, sectors 0x%x\n",
			       part_info->dl_filename, tmp_partstart_by_sector,
			       onetime_read_sectors);

			goto __download_sysrecover_part_err1;
		}
		tmp_imgfile_start += onetime_read_sectors;
		tmp_partdata_by_bytes -= onetime_read_sectors * 512;
		tmp_partstart_by_sector += onetime_read_sectors;
	}
	if (tmp_partdata_by_bytes > 0) {
		uint rest_sectors = (tmp_partdata_by_bytes + 511) / 512;
		//继续读出固件中的分区数据，大小为buffer字节数
		if (sunxi_flash_read(tmp_imgfile_start, rest_sectors,
				     down_buffer) != rest_sectors) {
			printf("sunxi sprite error : read sdcard block %d, total %d failed\n",
			       tmp_imgfile_start, rest_sectors);

			goto __download_sysrecover_part_err1;
		}
		//写入flash
		if (sunxi_sprite_write(tmp_partstart_by_sector, rest_sectors,
				       down_buffer) != rest_sectors) {
			printf("sunxi sprite error: download rawdata error %s, start 0x%x, sectors 0x%x\n",
			       part_info->dl_filename, tmp_partstart_by_sector,
			       rest_sectors);

			goto __download_sysrecover_part_err1;
		}
	}
	ret = 0;

__download_sysrecover_part_err1:
	if (imgitemhd) {
		Img_CloseItem(imghd, imgitemhd);
		imgitemhd = NULL;
	}

	return ret;
}
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
int sunxi_sprite_deal_part(sunxi_download_info *dl_map)
{
	dl_one_part_info *part_info;
	int ret = -1;
	int ret1;
	int i		 = 0;
	uchar *down_buff = NULL;

	if (!dl_map->download_count) {
		printf("sunxi sprite: no part need to write\n");

		return 0;
	}
	//初始化flash，nand或者mmc
	//	if(sunxi_sprite_init(1))
	//	{
	//		printf("sunxi sprite err: init flash err\n");

	//		return -1;
	//	}
	//申请内存
	down_buff = (uchar *)memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN(SPRITE_CARD_ONCE_DATA_DEAL +
				    SPRITE_CARD_HEAD_BUFF, CONFIG_SYS_CACHELINE_SIZE));
	if (!down_buff) {
		printf("sunxi sprite err: unable to malloc memory for sunxi_sprite_deal_part\n");

		goto __sunxi_sprite_deal_part_err1;
	}
	for (part_info = dl_map->one_part_info, i = 0;
	     i < dl_map->download_count; i++, part_info++) {
		tick_printf("begin to download part %s\n", part_info->name);

		if (!strncmp("UDISK", (char *)part_info->name,
			     strlen("UDISK"))) {
			ret1 = __download_udisk(part_info, down_buff);
			if (ret1 < 0) {
				printf("sunxi sprite err: sunxi_sprite_deal_part, download_udisk failed\n");

				goto __sunxi_sprite_deal_part_err2;
			} else if (ret1 > 0) {
				printf("do NOT need download UDISK\n");
			}
		} //如果是sysrecovery分区，烧录完整分区镜像
		else if (!strncmp("sysrecovery", (char *)part_info->name,
				  strlen("sysrecovery"))) {
			ret1 = __download_sysrecover_part(part_info, down_buff);
			if (ret1 != 0) {
				printf("sunxi sprite err: sunxi_sprite_deal_part, download sysrecovery failed\n");

				goto __sunxi_sprite_deal_part_err2;
			}
		} //如果是private分区，检查是否需要烧录
		else if (!strncmp("private", (char *)part_info->name,
				  strlen("private"))) {
			if (1) {
				//需要烧录此分区
				printf("NEED down private part\n");
				ret1 = __download_normal_part(part_info,
							      down_buff);
				if (ret1 != 0) {
					printf("line:%d sunxi sprite err: sunxi_sprite_deal_part, download private failed\n",
					       __LINE__);

					goto __sunxi_sprite_deal_part_err2;
				}
			} else {
				printf("IGNORE private part\n");
			}
		} else {
			ret1 = __download_normal_part(part_info, down_buff);
			if (ret1 != 0) {
				printf("line:%d sunxi sprite err: sunxi_sprite_deal_part, download private failed\n",
				       __LINE__);

				goto __sunxi_sprite_deal_part_err2;
			}
		}
		tick_printf("successed in download part %s\n", part_info->name);
	}

	ret = 0;

__sunxi_sprite_deal_part_err1:
	sunxi_sprite_exit(1);

__sunxi_sprite_deal_part_err2:

	if (down_buff) {
		free(down_buff);
	}

	return ret;
}
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
int sunxi_sprite_deal_uboot(int production_media)
{
	char *buffer;
	buffer = (char *)memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN(4 * 1024 * 1024, CONFIG_SYS_CACHELINE_SIZE));
	uint item_original_size;
	if (gd->bootfile_mode == SUNXI_BOOT_FILE_NORMAL) {
		imgitemhd = Img_OpenItem(imghd, "12345678", "UBOOT_0000000000");
	} else if (gd->bootfile_mode == SUNXI_BOOT_FILE_PKG) {
		if (get_boot_storage_type() != STORAGE_NOR) {
			imgitemhd = Img_OpenItem(imghd, "12345678",
						 "BOOTPKG-00000000");
		} else {
			imgitemhd = Img_OpenItem(imghd, "12345678",
						 "BOOTPKG-NOR00000");
		}
	} else {
		imgitemhd = Img_OpenItem(imghd, "12345678", "TOC1_00000000000");
	}

	if (!imgitemhd) {
		printf("sprite update error: fail to open uboot item\n");
		return -1;
	}
	//uboot长度
	item_original_size = Img_GetItemSize(imghd, imgitemhd);
	if (!item_original_size) {
		printf("sprite update error: fail to get uboot item size\n");
		return -1;
	}
	/*获取uboot的数据*/
	if (!Img_ReadItem(imghd, imgitemhd, (void *)buffer, 4 * 1024 * 1024)) {
		printf("update error: fail to read data from for uboot\n");
		return -1;
	}
	Img_CloseItem(imghd, imgitemhd);
	imgitemhd = NULL;

	if (sunxi_sprite_download_uboot(buffer, production_media, 0)) {
		printf("update error: fail to write uboot\n");
		return -1;
	}
	printf("sunxi_sprite_deal_uboot ok\n");
	free(buffer);
	return 0;
}
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
int sunxi_sprite_deal_boot0(int production_media)
{
	char *buffer = NULL;
	uint item_original_size;
	buffer = (char *)memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN(1*1024*1024, CONFIG_SYS_CACHELINE_SIZE));
	if (!buffer) {
		printf("err: unable to malloc memory\n");
		return -1;
	}
	if (gd->bootfile_mode == SUNXI_BOOT_FILE_NORMAL ||
	    gd->bootfile_mode == SUNXI_BOOT_FILE_PKG) {
		if (production_media == STORAGE_NAND) {
			imgitemhd = Img_OpenItem(imghd, "BOOT    ",
						 "BOOT0_0000000000");
		} else if (production_media == STORAGE_NOR) {
			imgitemhd = Img_OpenItem(imghd, "12345678",
						 "1234567890BNOR_0");
		} else {
			imgitemhd = Img_OpenItem(imghd, "12345678",
						 "1234567890BOOT_0");
		}
	} else {
		imgitemhd = Img_OpenItem(imghd, "12345678", "TOC0_00000000000");
	}

	if (!imgitemhd) {
		printf("sprite update error: fail to open boot0 item\n");
		goto sprite_boot0_fail;
	}
	//boot0长度
	item_original_size = Img_GetItemSize(imghd, imgitemhd);
	if (!item_original_size) {
		printf("sprite update error: fail to get boot0 item size\n");
		goto sprite_boot0_fail;
	}
	/*获取boot0的数据*/
	if (!Img_ReadItem(imghd, imgitemhd, (void *)buffer, 1 * 1024 * 1024)) {
		printf("update error: fail to read data from for boot0\n");
		goto sprite_boot0_fail;
	}
	Img_CloseItem(imghd, imgitemhd);
	imgitemhd = NULL;

	if (sunxi_sprite_download_boot0(buffer, production_media)) {
		printf("update error: fail to write boot0\n");
		goto sprite_boot0_fail;
	}
	return 0;
sprite_boot0_fail:
	free(buffer);
	return -1;
}

int sunxi_sprite_deal_recorvery_boot(int production_media)
{
	char buffer_uboot[4 * 1024 * 1024];
	char buffer_boot0[1 * 1024 * 1024];
	uint item_size_uboot;
	uint item_size_boot0;
	if (gd->bootfile_mode == SUNXI_BOOT_FILE_NORMAL) {
		imgitemhd = Img_OpenItem(imghd, "12345678", "UBOOT_0000000000");
	} else if (gd->bootfile_mode == SUNXI_BOOT_FILE_PKG) {
		imgitemhd = Img_OpenItem(imghd, "12345678", "BOOTPKG-00000000");
	} else {
		imgitemhd = Img_OpenItem(imghd, "12345678", "TOC1_00000000000");
	}

	if (!imgitemhd) {
		printf("sprite update error: fail to open uboot item\n");
		return -1;
	}

	item_size_uboot = Img_GetItemSize(imghd, imgitemhd);
	if (!item_size_uboot) {
		printf("sprite update error: fail to get uboot item size\n");
		return -1;
	}
	if (!Img_ReadItem(imghd, imgitemhd, (void *)buffer_uboot,
			  4 * 1024 * 1024)) {
		printf("update error: fail to read data from for uboot\n");
		return -1;
	}
	Img_CloseItem(imghd, imgitemhd);
	imgitemhd = NULL;

	if (gd->bootfile_mode == SUNXI_BOOT_FILE_NORMAL ||
	    gd->bootfile_mode == SUNXI_BOOT_FILE_PKG) {
		if (production_media == 0) {
			imgitemhd = Img_OpenItem(imghd, "BOOT    ",
						 "BOOT0_0000000000");
		} else {
			imgitemhd = Img_OpenItem(imghd, "12345678",
						 "1234567890BOOT_0");
		}
	} else {
		imgitemhd = Img_OpenItem(imghd, "12345678", "TOC0_00000000000");
	}

	if (!imgitemhd) {
		printf("sprite update error: fail to open boot0 item\n");
		return -1;
	}

	item_size_boot0 = Img_GetItemSize(imghd, imgitemhd);
	if (!item_size_boot0) {
		printf("sprite update error: fail to get boot0 item size\n");
		return -1;
	}

	if (!Img_ReadItem(imghd, imgitemhd, (void *)buffer_boot0,
			  1 * 1024 * 1024)) {
		printf("update error: fail to read data from for boot0\n");
		return -1;
	}
	Img_CloseItem(imghd, imgitemhd);
	imgitemhd = NULL;

	/*write boot data*/
	if (sunxi_sprite_download_uboot(buffer_uboot, production_media, 0)) {
		printf("update error: fail to write uboot\n");
		return -1;
	}
	tick_printf("successed in downloading uboot\n");
	if (sunxi_sprite_download_boot0(buffer_boot0, production_media)) {
		printf("update error: fail to write boot0\n");
		return -1;
	}
	tick_printf("successed in downloading boot0\n");
	printf("sunxi_sprite_deal_recorvery_boot ok\n");
	return 0;
}
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
int card_verify_uboot(uint start_block, uint length)
{
	int ret      = 0;
	char *buffer = NULL;
	buffer       = (char *)memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN(length, CONFIG_SYS_CACHELINE_SIZE));
	if (!buffer) {
		printf("%s: malloc %d byte memory fail\n", __func__, length);
		return -1;
	}
	memset(buffer, 0, ALIGN(length, CONFIG_SYS_CACHELINE_SIZE));

	ret = sunxi_flash_phyread(start_block, length / 512, buffer);
	if (!ret) {
		printf("%s: write boot0 from %d fail\n", __func__, start_block);
		goto ERR_OUT;
	}
	if (gd->bootfile_mode == SUNXI_BOOT_FILE_NORMAL) {
		struct spare_boot_head_t *uboot =
			(struct spare_boot_head_t *)buffer;
		printf("uboot magic %s\n", uboot->boot_head.magic);
		if (strncmp((const char *)uboot->boot_head.magic, UBOOT_MAGIC,
			    MAGIC_SIZE)) {
			printf("sunxi sprite: uboot magic is error\n");
			return -1;
		}
		length = uboot->boot_head.length;
		if (sunxi_sprite_verify_checksum(buffer,
						 uboot->boot_head.length,
						 uboot->boot_head.check_sum)) {
			printf("sunxi sprite: boot0 checksum is error flash_sum=0x%x\n",
			       uboot->boot_head.check_sum);
			goto ERR_OUT;
		}
		ret = 1;
	} else {
		sbrom_toc1_head_info_t *toc1 = (sbrom_toc1_head_info_t *)buffer;
		if (gd->bootfile_mode == SUNXI_BOOT_FILE_PKG) {
			printf("uboot_pkg magic 0x%x\n", toc1->magic);
		} else {
			printf("toc magic 0x%x\n", toc1->magic);
		}

		if (toc1->magic != TOC_MAIN_INFO_MAGIC) {
			printf("sunxi sprite: toc magic is error\n");
			return -1;
		}
		length = toc1->valid_len;

		if (sunxi_sprite_verify_checksum(buffer, toc1->valid_len,
						 toc1->add_sum)) {
			printf("sunxi sprite: toc1 checksum is error flash_sum=0x%x\n",
			       toc1->add_sum);
			goto ERR_OUT;
		}
		ret = 1;
	}

ERR_OUT:
	if (buffer != NULL)
		free(buffer);

	if (!ret)
		return -1;
	else
		return 0;
}

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
int card_download_uboot(uint length, void *buffer)
{
	int ret;

	ret = sunxi_flash_phywrite(UBOOT_START_SECTOR_IN_SDMMC, length / 512,
				   buffer);
	if (!ret) {
		printf("%s: write uboot from %d fail\n", __func__,
		       UBOOT_START_SECTOR_IN_SDMMC);
		return -1;
	}
	ret = card_verify_uboot(UBOOT_START_SECTOR_IN_SDMMC, length);
	if (ret < 0) {
		printf("%s: verify uboot checksum from %d fail\n", __func__,
		       UBOOT_START_SECTOR_IN_SDMMC);
		return -1;
	}
	ret = sunxi_flash_phywrite(UBOOT_BACKUP_START_SECTOR_IN_SDMMC,
				   length / 512, buffer);
	if (!ret) {
		printf("%s: write uboot from %d fail\n", __func__,
		       UBOOT_BACKUP_START_SECTOR_IN_SDMMC);
		return -1;
	}

	return 0;
}
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
int card_verify_boot0(uint start_block, uint length)
{
	int ret      = 0;
	char *buffer = NULL;
	buffer       = (char *)memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN(length, CONFIG_SYS_CACHELINE_SIZE));
	if (!buffer) {
		printf("%s: malloc %d byte memory fail\n", __func__, length);
		return -1;
	}
	memset(buffer, 0, ALIGN(length, CONFIG_SYS_CACHELINE_SIZE));

	ret = sunxi_flash_phyread(start_block, length / 512, buffer);
	if (!ret) {
		printf("%s: write boot0 from %d fail\n", __func__, start_block);
		goto ERR_OUT;
	}

	if (gd->bootfile_mode == SUNXI_BOOT_FILE_NORMAL ||
	    gd->bootfile_mode == SUNXI_BOOT_FILE_PKG) {
		boot0_file_head_t *boot0 = (boot0_file_head_t *)buffer;
		debug("%s\n", boot0->boot_head.magic);
		if (strncmp((const char *)boot0->boot_head.magic, BOOT0_MAGIC,
			    MAGIC_SIZE)) {
			printf("sunxi sprite: boot0 magic is error\n");
			goto ERR_OUT;
		}
		if (sunxi_sprite_verify_checksum(buffer,
						 boot0->boot_head.length,
						 boot0->boot_head.check_sum)) {
			printf("sunxi sprite: boot0 checksum is error flash_check_sum=0x%x\n",
			       boot0->boot_head.check_sum);
			goto ERR_OUT;
		}
		ret = 1;
	} else {
		toc0_private_head_t *toc0 = (toc0_private_head_t *)buffer;
		debug("%s\n", (char *)toc0->name);
		if (strncmp((const char *)toc0->name, TOC0_MAGIC, MAGIC_SIZE)) {
			printf("sunxi sprite: toc0 magic is error\n");
			goto ERR_OUT;
		}
		if (sunxi_sprite_verify_checksum(buffer, toc0->length,
						 toc0->check_sum)) {
			printf("sunxi sprite: toc0 checksum is error flash_check_sum=0x%x\n",
			       toc0->check_sum);
			goto ERR_OUT;
		}
		ret = 1;
	}

ERR_OUT:
	if (buffer != NULL)
		free(buffer);

	if (!ret)
		return -1;
	else
		return 0;
}

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
int card_erase_boot0(uint length, void *buffer, uint storage_type)
{
	int ret		   = 0;
	char *erase_buffer = NULL;

	erase_buffer = (char *)memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN(length, CONFIG_SYS_CACHELINE_SIZE));
	if (!erase_buffer) {
		debug("%s: malloc %d byte memory fail\n", __func__, length);
		return -1;
	}
	memset(erase_buffer, 0, length);

	//for card2
	if ((storage_type == STORAGE_EMMC) || (storage_type == STORAGE_EMMC0)) {
		printf("card2 erase boot0 \n");
		//write boot0 bankup copy firstly
		ret = sunxi_sprite_phywrite(CONFIG_SUNXI_BOOT0_SDMMC_BACKUP_START_ADDR,
					   length / 512, buffer);
		if (!ret) {
			debug("%s: write boot0 from %d fail\n", __func__,
			       CONFIG_SUNXI_BOOT0_SDMMC_BACKUP_START_ADDR);
			goto ERR_OUT;
		}
		ret = sunxi_sprite_phywrite(BOOT0_SDMMC_START_ADDR, length / 512,
					   buffer);
		if (!ret) {
			debug("%s: write boot0 from %d fail\n", __func__,
			       BOOT0_SDMMC_START_ADDR);
			goto ERR_OUT;
		}
#ifdef PLATFORM_SUPPORT_EMMC3
		ret = sunxi_sprite_phywrite(BOOT0_EMMC3_START_ADDR, length / 512,
					   erase_buffer);
		if (!ret) {
			debug("%s: write boot0 from %d fail\n", __func__,
			       BOOT0_EMMC3_START_ADDR);
			goto ERR_OUT;
		}
		ret = sunxi_sprite_phywrite(BOOT0_EMMC3_BACKUP_START_ADDR,
					   length / 512, erase_buffer);
		if (!ret) {
			debug("%s: write boot0 from %d fail\n", __func__,
			       BOOT0_EMMC3_BACKUP_START_ADDR);
			goto ERR_OUT;
		}
#endif
	} else {
//for card3
#ifdef PLATFORM_SUPPORT_EMMC3
		printf("card3 erase boot0 \n");
		//write boot0 bankup copy firstly
		ret = sunxi_sprite_phywrite(BOOT0_EMMC3_BACKUP_START_ADDR,
					   length / 512, buffer);
		if (!ret) {
			debug("%s: write boot0 from %d fail\n", __func__,
			       BOOT0_EMMC3_BACKUP_START_ADDR);
			goto ERR_OUT;
		}
		ret = sunxi_sprite_phywrite(BOOT0_EMMC3_START_ADDR, length / 512,
					   buffer);
		if (!ret) {
			debug("%s: write boot0 from %d fail\n", __func__,
			       BOOT0_EMMC3_START_ADDR);
			goto ERR_OUT;
		}

		ret = sunxi_sprite_phywrite(BOOT0_SDMMC_START_ADDR, length / 512,
					   erase_buffer);
		if (!ret) {
			debug("%s: write boot0 from %d fail\n", __func__,
			       BOOT0_SDMMC_START_ADDR);
			goto ERR_OUT;
		}
		ret = sunxi_sprite_phywrite(CONFIG_SUNXI_BOOT0_SDMMC_BACKUP_START_ADDR,
					   length / 512, erase_buffer);
		if (!ret) {
			debug("%s: write boot0 from %d fail\n", __func__,
			       CONFIG_SUNXI_BOOT0_SDMMC_BACKUP_START_ADDR);
			goto ERR_OUT;
		}
#endif
	}

ERR_OUT:
	if (erase_buffer != NULL)
		free(erase_buffer);

	if (!ret)
		return -1;
	else
		return 0;
}

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
//static void buffer_dump(void *buffer, int len)
//{
//	int i;
//	char *data = (char *)buffer;
//
//	for(i=0;i<len;i++)
//	{
//		printf("%02x", data[i]);
//		if((i & 0x07) == 7)
//		{
//			printf("\n");
//		}
//		else
//		{
//			puts("  ");
//		}
//	}
//}

int card_download_standard_mbr(void *buffer)
{
	mbr_stand *mbrst;
	sunxi_mbr_t *mbr = (sunxi_mbr_t *)buffer;
	char mbr_bufst[512];
	int i;
	int sectors;
	int unusd_sectors;

	sectors = 0;
	for (i = 1; i < mbr->PartCount - 1; i++) {
		memset(mbr_bufst, 0, 512);
		mbrst = (mbr_stand *)mbr_bufst;

		sectors += mbr->array[i].lenlo;

		mbrst->part_info[0].part_type = 0x83;
		mbrst->part_info[0].start_sectorl =
			((mbr->array[i].addrlo - i + 20 * 1024 * 1024 / 512) &
			 0x0000ffff) >>
			0;
		mbrst->part_info[0].start_sectorh =
			((mbr->array[i].addrlo - i + 20 * 1024 * 1024 / 512) &
			 0xffff0000) >>
			16;
		mbrst->part_info[0].total_sectorsl =
			(mbr->array[i].lenlo & 0x0000ffff) >> 0;
		mbrst->part_info[0].total_sectorsh =
			(mbr->array[i].lenlo & 0xffff0000) >> 16;

		if (i != mbr->PartCount - 2) {
			mbrst->part_info[1].part_type     = 0x05;
			mbrst->part_info[1].start_sectorl = i;
			mbrst->part_info[1].start_sectorh = 0;
			mbrst->part_info[1].total_sectorsl =
				(mbr->array[i].lenlo & 0x0000ffff) >> 0;
			mbrst->part_info[1].total_sectorsh =
				(mbr->array[i].lenlo & 0xffff0000) >> 16;
		}

		mbrst->end_flag = 0xAA55;
		if (!sunxi_flash_phywrite(i, 1, mbr_bufst)) {
			printf("write standard mbr %d failed\n", i);

			return -1;
		}
	}
	memset(mbr_bufst, 0, 512);
	mbrst = (mbr_stand *)mbr_bufst;

	unusd_sectors = sunxi_flash_size() - 20 * 1024 * 1024 / 512 - sectors;
	mbrst->part_info[0].indicator = 0x80;
	mbrst->part_info[0].part_type = 0x0B;
	mbrst->part_info[0].start_sectorl =
		((mbr->array[mbr->PartCount - 1].addrlo +
		  20 * 1024 * 1024 / 512) &
		 0x0000ffff) >>
		0;
	mbrst->part_info[0].start_sectorh =
		((mbr->array[mbr->PartCount - 1].addrlo +
		  20 * 1024 * 1024 / 512) &
		 0xffff0000) >>
		16;
	mbrst->part_info[0].total_sectorsl = (unusd_sectors & 0x0000ffff) >> 0;
	mbrst->part_info[0].total_sectorsh = (unusd_sectors & 0xffff0000) >> 16;

	mbrst->part_info[1].part_type = 0x06;
	mbrst->part_info[1].start_sectorl =
		((mbr->array[0].addrlo + 20 * 1024 * 1024 / 512) &
		 0x0000ffff) >>
		0;
	mbrst->part_info[1].start_sectorh =
		((mbr->array[0].addrlo + 20 * 1024 * 1024 / 512) &
		 0xffff0000) >>
		16;
	mbrst->part_info[1].total_sectorsl =
		(mbr->array[0].lenlo & 0x0000ffff) >> 0;
	mbrst->part_info[1].total_sectorsh =
		(mbr->array[0].lenlo & 0xffff0000) >> 16;

	mbrst->part_info[2].part_type      = 0x05;
	mbrst->part_info[2].start_sectorl  = 1;
	mbrst->part_info[2].start_sectorh  = 0;
	mbrst->part_info[2].total_sectorsl = (sectors & 0x0000ffff) >> 0;
	mbrst->part_info[2].total_sectorsh = (sectors & 0xffff0000) >> 16;

	mbrst->end_flag = 0xAA55;
	if (!sunxi_flash_phywrite(0, 1, mbr_bufst)) {
		printf("write standard mbr 0 failed\n");

		return -1;
	}

	return 0;
}

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
#define CARD_ERASE_BLOCK_BYTES (8 * 1024 * 1024)
#define CARD_ERASE_BLOCK_SECTORS (CARD_ERASE_BLOCK_BYTES / 512)

int card_erase(int erase, void *mbr_buffer)
{
	char *erase_buffer;
	sunxi_mbr_t *mbr = (sunxi_mbr_t *)mbr_buffer;
	unsigned int erase_head_sectors;
	unsigned int erase_head_addr;
	unsigned int erase_tail_sectors;
	unsigned int erase_tail_addr;
	unsigned int skip_space[1 + 2 * 2] = { 0 };
	unsigned int from, nr;
	int k, ret = 0;
	int i;

	tick_printf("erase all part start\n");
	if (!erase) {
		return 0;
	}
	erase_buffer = (char *)memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN(CARD_ERASE_BLOCK_BYTES, CONFIG_SYS_CACHELINE_SIZE));
	if (!erase_buffer) {
		debug("card erase fail: unable to malloc memory for card erase\n");

		return -1;
	}
	memset(erase_buffer, 0, ALIGN(CARD_ERASE_BLOCK_BYTES, CONFIG_SYS_CACHELINE_SIZE));

	//erase boot0,write 0x00
	if (card_erase_boot0(32 * 1024, erase_buffer, get_boot_storage_type())) {
		return -1;
	}

	for (i = 1; i < mbr->PartCount; i++) {
		printf("erase %s part\n", mbr->array[i].name);
		// part > 16M
		if (mbr->array[i].lenlo > CARD_ERASE_BLOCK_SECTORS * 2) {
			erase_head_sectors = CARD_ERASE_BLOCK_SECTORS;
			erase_head_addr    = mbr->array[i].addrlo;
			//erase_tail_sectors = CARD_ERASE_BLOCK_SECTORS;
			erase_tail_sectors = 2 * 1024 * 1024 / 512;
			erase_tail_addr    = mbr->array[i].addrlo +
					  mbr->array[i].lenlo -
					  CARD_ERASE_BLOCK_SECTORS;
			// 8M < part <= 16M
		} else if (mbr->array[i].lenlo > CARD_ERASE_BLOCK_SECTORS) {
			erase_head_sectors = CARD_ERASE_BLOCK_SECTORS;
			erase_head_addr    = mbr->array[i].addrlo;
			//erase_tail_sectors = mbr->array[i].lenlo - CARD_ERASE_BLOCK_SECTORS;
			erase_tail_sectors = 2 * 1024 * 1024 / 512;
			erase_tail_addr    = mbr->array[i].addrlo +
					  mbr->array[i].lenlo -
					  erase_tail_sectors;
			// 0 < part <= 8M
		} else if (mbr->array[i].lenlo > 0) {
			erase_head_sectors = mbr->array[i].lenlo;
			erase_head_addr    = mbr->array[i].addrlo;
			erase_tail_sectors = 0;
			erase_tail_addr    = mbr->array[i].addrlo;
		} else {
			//printf("don't deal prat's length is 0 (%s) \n", mbr->array[i].name);
			//break;
			erase_head_sectors = CARD_ERASE_BLOCK_SECTORS;
			erase_head_addr    = mbr->array[i].addrlo;
			erase_tail_sectors = 0;
			erase_tail_addr    = mbr->array[i].addrlo;
		}

		from = mbr->array[i].addrlo + CONFIG_MMC_LOGICAL_OFFSET;
		nr   = mbr->array[i].lenlo;
		ret  = sunxi_sprite_phyerase(from, nr, skip_space);
		if (ret == 0) {
			//printf("erase part from sector 0x%x to 0x%x ok\n", from, (from+nr-1));
		} else if (ret == 1) {
			for (k = 0; k < 2; k++) {
				if (skip_space[0] & (1 << k)) {
					debug("write zeros-%d: from 0x%x to 0x%x\n",
					       k, skip_space[2 * k + 1],
					       (skip_space[2 * k + 1] +
						skip_space[2 * k + 2] - 1));
					from = skip_space[2 * k + 1];
					nr   = skip_space[2 * k + 2];
					if (!sunxi_sprite_phywrite(
						    from, nr, erase_buffer)) {
						debug("card erase fail in erasing part %s\n",
						       mbr->array[i].name);
						free(erase_buffer);
						return -1;
					}
				}
			}
		} else if (ret == -1) {
			// erase head for partition
			if (!sunxi_sprite_write(erase_head_addr,
					       erase_head_sectors,
					       erase_buffer)) {
				debug("card erase fail in erasing part %s\n",
				       mbr->array[i].name);
				free(erase_buffer);
				return -1;
			}
			debug("erase prat's head from sector 0x%x to 0x%x\n",
			       erase_head_addr,
			       erase_head_addr + erase_head_sectors);

			// erase tail for partition
			if (erase_tail_sectors) {
				if (!sunxi_sprite_write(erase_tail_addr,
						       erase_tail_sectors,
						       erase_buffer)) {
					debug("card erase fail in erasing part %s\n",
					       mbr->array[i].name);
					free(erase_buffer);
					return -1;
				}
				debug("erase part's tail from sector 0x%x to 0x%x\n",
				       erase_tail_addr,
				       erase_tail_addr + erase_tail_sectors);
			}
		}
	}
	printf("card erase all\n");
	free(erase_buffer);

	//while((*(volatile unsigned int *)0) != 1);
	//tick_printf("erase all part end\n");
	return 0;
}

#define BOOT0_MAX_SIZE (32 * 1024)
int sunxi_card_fill_boot0_magic(void)
{
	uchar buffer[BOOT0_MAX_SIZE];
	boot0_file_head_t *boot0_head;
	uint src_sum, cal_sum;
	char debug_info[1024];
	int ret = -1;

	puts("probe mmc0 if exist\n");
	memset(debug_info, 0, 1024);

	memset(buffer, 0, BOOT0_MAX_SIZE);
	if (sunxi_sprite_read(16, BOOT0_MAX_SIZE / 512, (void *)buffer))
	//	if(sunxi_sprite_mmc_phyread(16, BOOT0_MAX_SIZE/512, (void *)buffer) != BOOT0_MAX_SIZE/512)
	{
		strcpy(debug_info, "read mmc boot0 failed");

		goto __sunxi_card_fill_boot0_magic_exit;
	}
	//compare data
	boot0_head = (boot0_file_head_t *)buffer;
	//fill boot0 magic
	memcpy((char *)boot0_head->boot_head.magic, BOOT0_MAGIC, 8);
	printf("boot0_head->boot_head.magic   == %s \n",
	       (char *)boot0_head->boot_head.magic);
	src_sum = boot0_head->boot_head.check_sum;
	printf("src_sum = %x \n", src_sum);
	//boot0_head->boot_head.check_sum = STAMP_VALUE;
	printf("boot0_head->boot_head.length  =  %d \n",
	       boot0_head->boot_head.length);
	boot0_head->boot_head.check_sum = STAMP_VALUE;
	cal_sum = add_sum(buffer, boot0_head->boot_head.length);
	if (src_sum != cal_sum) {
		puts("boot0 addsum error\n");

		return ret;
	}

	boot0_head->boot_head.check_sum = src_sum;
	flush_cache((ulong)buffer, BOOT0_MAX_SIZE);
	//	if(mmc0->block_dev.block_write_mass_pro(mmc0->block_dev.devnum, 16, BOOT0_MAX_SIZE/512,  (void *)buffer) != BOOT0_MAX_SIZE/512)
	if (sunxi_sprite_write(16, BOOT0_MAX_SIZE / 512, (void *)buffer) !=
	    BOOT0_MAX_SIZE / 512) {
		strcpy(debug_info, "write mmc boot0 failed");

		goto __sunxi_card_fill_boot0_magic_exit;
	}

	ret = 0;
	return ret;

__sunxi_card_fill_boot0_magic_exit:
	printf("%s\n", debug_info);
	return ret;
}

int sunxi_sprite_deal_part_from_sysrevoery(sunxi_download_info *dl_map)
{
	dl_one_part_info *part_info;
	int ret = -1;
	int ret1;
	int i		 = 0;
	uchar *down_buff = NULL;
	__maybe_unused int rate;

	if (!dl_map->download_count) {
		printf("sunxi sprite: no part need to write\n");
		return 0;
	}
	rate = (80) / (dl_map->download_count + 1);
	down_buff = (uchar *)memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN(SPRITE_CARD_ONCE_DATA_DEAL +
				    SPRITE_CARD_HEAD_BUFF, CONFIG_SYS_CACHELINE_SIZE));
	if (!down_buff) {
		printf("sunxi sprite err: unable to malloc memory for sunxi_sprite_deal_part\n");
		goto __sunxi_sprite_deal_part_err;
	}

	for (part_info = dl_map->one_part_info, i = 0;
	     i < dl_map->download_count; i++, part_info++) {
		tick_printf("begin to download part %s\n", part_info->name);
		if (!strcmp("env", (const char *)part_info->name)) {
			printf("env part do not need to rewrite\n");
			//			sprite_cartoon_upgrade(20 + rate * (i+1));
			continue;
		} else if (!strcmp("sysrecovery",
				   (const char *)part_info->name)) {
			printf("THIS_IMG_SELF_00 do not need to rewrite\n");
			//			sprite_cartoon_upgrade(20 + rate * (i+1));
			continue;
		} else if (!strcmp("UDISK", (const char *)part_info->name)) {
			printf("UDISK do not need to rewrite\n");
			//			sprite_cartoon_upgrade(20 + rate * (i+1));
			continue;
		} else if (!strcmp("private", (const char *)part_info->name)) {
			printf("private do not need to rewrite\n");
			//			sprite_cartoon_upgrade(20 + rate * (i+1));
			continue;
		} else {
			ret1 = __download_normal_part(part_info, down_buff);
			if (ret1 != 0) {
				printf("sunxi sprite err: sunxi_sprite_deal_part, download normal failed\n");
				goto __sunxi_sprite_deal_part_err;
			}
		}
		//		sprite_cartoon_upgrade(20 + rate * (i+1));
		tick_printf("successed in download part %s\n", part_info->name);
	}

	ret = 0;

__sunxi_sprite_deal_part_err:

	if (down_buff) {
		free(down_buff);
	}

	return ret;
}

int __imagehd(HIMAGE tmp_himage)
{
	imghd = tmp_himage;
	if (imghd) {
		return 0;
	}
	return -1;
}

#ifdef CONFIG_SUNXI_SPINOR
extern int sunxi_sprite_setdata_finish(void);
static int __download_fullimg_part(uchar *source_buff)
{
	uint tmp_partstart_by_sector;

	s64 partdata_by_byte;
	s64 tmp_partdata_by_bytes;

	uint onetime_read_sectors;
	uint first_write_bytes;

	uint imgfile_start;
	uint tmp_imgfile_start;

	u8 *down_buffer = source_buff + SPRITE_CARD_HEAD_BUFF;

	int ret			= -1;
	tmp_partstart_by_sector = 0;
	imgitemhd = Img_OpenItem(imghd, "12345678", "FULLIMG_00000000");
	if (!imgitemhd) {
		printf("sunxi sprite error: open part FULLIMG failed\n");
		return -1;
	}
	partdata_by_byte = Img_GetItemSize(imghd, imgitemhd);
	if (partdata_by_byte <= 0) {
		printf("sunxi sprite error: fetch part len FULLIMG failed\n");
		goto __download_fullimg_part_err1;
	}
	printf("partdata hi 0x%x\n", (uint)(partdata_by_byte >> 32));
	printf("partdata lo 0x%x\n", (uint)partdata_by_byte);
	tmp_partdata_by_bytes = partdata_by_byte;
	if (tmp_partdata_by_bytes >= SPRITE_CARD_ONCE_DATA_DEAL) {
		onetime_read_sectors = SPRITE_CARD_ONCE_SECTOR_DEAL;
		first_write_bytes    = SPRITE_CARD_ONCE_DATA_DEAL;
	} else {
		onetime_read_sectors = (tmp_partdata_by_bytes + 511) >> 9;
		first_write_bytes    = (uint)tmp_partdata_by_bytes;
	}
	imgfile_start = Img_GetItemStart(imghd, imgitemhd);
	if (!imgfile_start) {
		printf("sunxi sprite err : cant get part data imgfile_start FULLIMG\n");

		goto __download_fullimg_part_err1;
	}
	tmp_imgfile_start = imgfile_start;
	if (sunxi_flash_read(tmp_imgfile_start, onetime_read_sectors,
			     down_buffer) != onetime_read_sectors) {
		printf("sunxi sprite error : read sdcard block %d, total %d failed\n",
		       tmp_imgfile_start, onetime_read_sectors);
		goto __download_fullimg_part_err1;
	}
	tmp_imgfile_start += onetime_read_sectors;
	if (sunxi_flash_write(tmp_partstart_by_sector, onetime_read_sectors,
			      down_buffer) != onetime_read_sectors) {
		printf("sunxi sprite error: download rawdata error FULLIMG\n");
		goto __download_fullimg_part_err1;
	}
	tmp_partdata_by_bytes -= first_write_bytes;
	tmp_partstart_by_sector += onetime_read_sectors;

	while (tmp_partdata_by_bytes >= SPRITE_CARD_ONCE_DATA_DEAL) {
		if (sunxi_flash_read(
			    tmp_imgfile_start, SPRITE_CARD_ONCE_SECTOR_DEAL,
			    down_buffer) != SPRITE_CARD_ONCE_SECTOR_DEAL) {
			printf("sunxi sprite error : read sdcard block %d, total %d failed\n",
			       tmp_imgfile_start, SPRITE_CARD_ONCE_SECTOR_DEAL);

			goto __download_fullimg_part_err1;
		}
		//D′è?flash
		if (sunxi_flash_write(tmp_partstart_by_sector,
				      SPRITE_CARD_ONCE_SECTOR_DEAL,
				      down_buffer) !=
		    SPRITE_CARD_ONCE_SECTOR_DEAL) {
			printf("sunxi sprite error: download rawdata error FULLIMG, start 0x%x, sectors 0x%x\n",
			       tmp_partstart_by_sector,
			       SPRITE_CARD_ONCE_SECTOR_DEAL);
			goto __download_fullimg_part_err1;
		}
		tmp_imgfile_start += SPRITE_CARD_ONCE_SECTOR_DEAL;
		tmp_partdata_by_bytes -= SPRITE_CARD_ONCE_DATA_DEAL;
		tmp_partstart_by_sector += SPRITE_CARD_ONCE_SECTOR_DEAL;
	}
	if (tmp_partdata_by_bytes > 0) {
		uint rest_sectors = (tmp_partdata_by_bytes + 511) >> 9;
		if (sunxi_flash_read(tmp_imgfile_start, rest_sectors,
				     down_buffer) != rest_sectors) {
			printf("sunxi sprite error : read sdcard block %d, total %d failed\n",
			       tmp_imgfile_start, rest_sectors);

			goto __download_fullimg_part_err1;
		}
		if (sunxi_flash_write(tmp_partstart_by_sector, rest_sectors,
				      down_buffer) != rest_sectors) {
			printf("sunxi sprite error: download rawdata error FULLIMG, start 0x%x, sectors 0x%x\n",
			       tmp_partstart_by_sector, rest_sectors);

			goto __download_fullimg_part_err1;
		}
	}
	printf("successed in writting part FULLIMG\n");
	ret = 0;

__download_fullimg_part_err1:
	if (imgitemhd) {
		Img_CloseItem(imghd, imgitemhd);
		imgitemhd = NULL;
	}
	return ret;
}

int sunxi_sprite_deal_fullimg(void)
{
	int ret = -1;
	int ret1;
	uchar *down_buff = NULL;

	if (get_boot_storage_type() == STORAGE_NAND ? sunxi_sprite_init(1) : 0) {
		printf("sunxi sprite err: init flash err\n");
		return -1;
	}

	down_buff = (uchar *)memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN(SPRITE_CARD_ONCE_DATA_DEAL +
				    SPRITE_CARD_HEAD_BUFF, CONFIG_SYS_CACHELINE_SIZE));
	if (!down_buff) {
		printf("sunxi sprite err: unable to malloc memory for sunxi_sprite_deal_part\n");

		goto __sunxi_sprite_deal_fullimg_err1;
	}

	ret1 = __download_fullimg_part(down_buff);
	if (ret1 != 0) {
		printf("line:%d sunxi sprite err: sunxi_sprite_deal_part, download private failed\n",
		       __LINE__);

		goto __sunxi_sprite_deal_fullimg_err2;
	}
	//while(*(volatile uint *)0 != 0x12);
	sunxi_sprite_setdata_finish();

	printf("sunxi card sprite trans finish\n");

	ret = 0;

__sunxi_sprite_deal_fullimg_err1:
	get_boot_storage_type() == STORAGE_NAND ? sunxi_sprite_exit(1) : 0;
__sunxi_sprite_deal_fullimg_err2:
	if (down_buff) {
		free(down_buff);
	}
	return ret;
}
#endif
