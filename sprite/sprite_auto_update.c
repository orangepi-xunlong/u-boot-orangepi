/* uboot/sprite/sprite_auto_update.c
 *
 * Copyright (c) 2016 Allwinnertech Co., Ltd.
 * Author: zhouhuacai <zhouhuacai@allwinnertech.com>
 *
 * Update firmware with sdcard storage
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <common.h>
#include <malloc.h>
#include <sprite.h>
#include <sunxi_mbr.h>
#include <sunxi_nand.h>
#include <private_toc.h>
#include <private_boot0.h>
#include <sunxi_board.h>
#include <spare_head.h>
#include "sprite_card.h"
#include "sparse/sparse.h"
#include "sprite_verify.h"
#include "firmware/imgdecode.h"
#include <sys_config_old.h>
#include <fs.h>
#include "sprite_auto_update.h"

DECLARE_GLOBAL_DATA_PTR;

#define IMG_NAME "FIRMWARE.bin"

#define  AU_HEAD_BUFF		   (32 * 1024)
#if defined (CONFIG_SUNXI_SPINOR)
#define  AU_ONCE_DATA_DEAL    (2 * 1024 * 1024)
#else
#define  AU_ONCE_DATA_DEAL    (16 * 1024 * 1024)
#endif
#define  AU_ONCE_SECTOR_DEAL  (AU_ONCE_DATA_DEAL/512)

static void *imghd = NULL;
static void *imgitemhd = NULL;
static char *imgname = NULL;

extern int do_card0_probe(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern void __dump_mbr(sunxi_mbr_t *mbr_info);
extern void __dump_dlmap(sunxi_download_info *dl_info);
extern int sunxi_sprite_verify_dlmap(void *buffer);
extern int unsparse_probe(char *source, uint length, uint android_format_flash_start);
extern void dump_dram_para(void* dram, uint size);
extern int sunxi_set_secure_mode(void);
extern int sunxi_sprite_download_boot0(void *buffer, int production_media);
extern int sunxi_sprite_download_uboot(void *buffer, int production_media, int generate_checksum);

int __attribute__((weak)) nand_get_mbr(char* buffer, uint len)
 {
	return -1;
 }

int auto_update_check(void)
{
	int keyvalue;
	int ret;
	int key;

	keyvalue = uboot_spare_head.boot_ext[0].data[2];
	printf("key %d\n", keyvalue);

	ret = script_parser_fetch("auto_update_key", "key_val", &key, 1);
	if (ret)
	{
		printf("cant find auto update key value\n");
	}
	else
	{
		if(keyvalue == key)
		{
			printf("auto update key found,update image...\n");
			uboot_spare_head.boot_data.work_mode = WORK_MODE_CARD_UPDATE;
		}
	}

	return 0;
}

static int au_check_img_valid(void)
{
	/*TBD: we should check the img to make sure updating the correct firmware*/
	return 0;
}

static int auto_update_firmware_probe(char *name)
{
	imghd = Img_Fat_Open(name);

	if(!imghd)
	{
		return -1;
	}

	return 0;
}

int fat_fs_read(const char *filename, void *buf, int offset, int len)
{
	unsigned long time;
	int len_read;
	ulong load_addr;

	if ((buf ==NULL)||(filename == NULL))
		return -1;

	load_addr = (ulong)buf;

	if (fs_set_blk_dev("mmc","0:0" , FS_TYPE_FAT))
	{
		printf("fs set mmc blk dev fail!\n");
		return -1;
	}

	time = get_timer(0);
	len_read = fs_read(filename, load_addr, offset, len);
	time = get_timer(time);
	if (len_read <= 0)
		return -1;

	printf("%d bytes read in %lu ms", len_read, time);
	if (time > 0)
	{
		puts(" (");
		print_size(len_read / time * 1000, "/s");
		puts(")");
	}
	puts("\n");

	return len_read;
}

static int auto_update_fetch_download_map(sunxi_download_info  *dl_map)
{
	imgitemhd = Img_OpenItem(imghd, "12345678", "1234567890DLINFO");
	if(!imgitemhd)
	{
		return -1;
	}

	if (!Img_Fat_ReadItem(imghd, imgitemhd, imgname,(void *)dl_map,  sizeof(sunxi_download_info)))
	{
		printf("sunxi sprite error : read dl map failed\n");

		return -1;
	}

	Img_CloseItem(imghd, imgitemhd);
	imgitemhd = NULL;

	return sunxi_sprite_verify_dlmap(dl_map);
}

static int auto_update_fetch_mbr(void  *img_mbr)
{
	int mbr_num = SUNXI_MBR_COPY_NUM;

	if (get_boot_storage_type() == STORAGE_NOR)
	{
		mbr_num = 1;
	}

	imgitemhd = Img_OpenItem(imghd, "12345678", "1234567890___MBR");
	if(!imgitemhd)
	{
		return -1;
	}

	if (!Img_Fat_ReadItem(imghd, imgitemhd, imgname, img_mbr,sizeof(sunxi_mbr_t) * mbr_num))
	{
		printf("sunxi sprite error : read mbr failed\n");

		return -1;
	}

	Img_CloseItem(imghd, imgitemhd);
	imgitemhd = NULL;

	return sunxi_sprite_verify_mbr(img_mbr);
}

static int __download_normal_part(dl_one_part_info *part_info,  uchar *source_buff)
{
	int  ret = -1;
	uint partstart_by_sector;
	uint tmp_partstart_by_sector;
	s64  partsize_by_byte;
	s64  partdata_by_byte;
	s64  tmp_partdata_by_bytes;
	uint onetime_read_sectors;
	uint first_write_bytes;
	uint imgfile_start;
	uint tmp_imgfile_start;
	int  partdata_format;
	uint active_verify;
	uint origin_verify;
	uchar verify_data[1024];
	uint *tmp;
	u8 *down_buffer = source_buff + AU_HEAD_BUFF;


	tmp_partstart_by_sector = partstart_by_sector = part_info->addrlo;
	partsize_by_byte     = part_info->lenlo;
	partsize_by_byte   <<= 9;

	imgitemhd = Img_OpenItem(imghd, "RFSFAT16", (char *)part_info->dl_filename);
	if(!imgitemhd)
	{
		printf("sunxi sprite error: open part %s failed\n", part_info->dl_filename);

		return -1;
	}

	partdata_by_byte = Img_GetItemSize(imghd, imgitemhd);
	if (partdata_by_byte <= 0)
	{
		printf("sunxi sprite error: fetch part len %s failed\n", part_info->dl_filename);

		goto __download_normal_part_err1;
	}
	printf("partdata hi 0x%x\n", (uint)(partdata_by_byte>>32));
	printf("partdata lo 0x%x\n", (uint)partdata_by_byte);

	if(partdata_by_byte > partsize_by_byte)
	{
		printf("sunxi sprite: data size 0x%x is larger than part %s size 0x%x\n", (uint)(partdata_by_byte/512), part_info->dl_filename, (uint)(partsize_by_byte/512));

		goto __download_normal_part_err1;
	}

	tmp_partdata_by_bytes = partdata_by_byte;
	if(tmp_partdata_by_bytes >= AU_ONCE_DATA_DEAL)
	{
		onetime_read_sectors = AU_ONCE_SECTOR_DEAL;
		first_write_bytes    = AU_ONCE_DATA_DEAL;
	}
	else
	{
		onetime_read_sectors = (tmp_partdata_by_bytes + 511)>>9;
		first_write_bytes    = (uint)tmp_partdata_by_bytes;
	}

	imgfile_start = Img_GetItemOffset(imghd, imgitemhd);
	if(!imgfile_start)
	{
		printf("sunxi sprite err : cant get part data imgfile_start %s\n", part_info->dl_filename);

		goto __download_normal_part_err1;
	}
	tmp_imgfile_start = imgfile_start;

	if (fat_fs_read(imgname, down_buffer,imgfile_start,onetime_read_sectors*512) != onetime_read_sectors*512)
	{
		printf("sunxi sprite error : read sdcard start %d, total %d failed\n", tmp_imgfile_start, onetime_read_sectors);

		goto __download_normal_part_err1;
	}
	/* position of next data to be read*/
	tmp_imgfile_start += onetime_read_sectors*512;

	/* check sparse format or not */
	partdata_format = unsparse_probe((char *)down_buffer, first_write_bytes, partstart_by_sector);
	if(partdata_format != ANDROID_FORMAT_DETECT)
	{
		if(sunxi_sprite_write(tmp_partstart_by_sector, onetime_read_sectors, down_buffer) != onetime_read_sectors)
		{
			printf("sunxi sprite error: download rawdata error %s\n", part_info->dl_filename);

			goto __download_normal_part_err1;
		}
		tmp_partdata_by_bytes   -= first_write_bytes;
		tmp_partstart_by_sector += onetime_read_sectors;

		while(tmp_partdata_by_bytes >= AU_ONCE_DATA_DEAL)
		{
			/* continue read partition data from img*/
			if (fat_fs_read(imgname, down_buffer,tmp_imgfile_start,AU_ONCE_DATA_DEAL) != AU_ONCE_DATA_DEAL)
			{
				printf("sunxi sprite error : read sdcard start %d, total %d failed\n", tmp_imgfile_start, AU_ONCE_DATA_DEAL);

				goto __download_normal_part_err1;
			}
			if(sunxi_sprite_write(tmp_partstart_by_sector, AU_ONCE_SECTOR_DEAL, down_buffer) != AU_ONCE_SECTOR_DEAL)
			{
				printf("sunxi sprite error: download rawdata error %s, start 0x%x, sectors 0x%x\n", part_info->dl_filename, tmp_partstart_by_sector, AU_ONCE_SECTOR_DEAL);

				goto __download_normal_part_err1;
			}
			tmp_imgfile_start       += AU_ONCE_SECTOR_DEAL*512;
			tmp_partdata_by_bytes   -= AU_ONCE_DATA_DEAL;
			tmp_partstart_by_sector += AU_ONCE_SECTOR_DEAL;
		}
		if(tmp_partdata_by_bytes > 0)
		{
			uint rest_sectors = (tmp_partdata_by_bytes + 511)>>9;
			if (fat_fs_read(imgname, down_buffer,tmp_imgfile_start,rest_sectors*512) != rest_sectors*512)
			{
				printf("sunxi sprite error : read sdcard start %d, total %d failed\n", tmp_imgfile_start, rest_sectors*512);

				goto __download_normal_part_err1;
			}
			if(sunxi_sprite_write(tmp_partstart_by_sector, rest_sectors, down_buffer) != rest_sectors)
			{
				printf("sunxi sprite error: download rawdata error %s, start 0x%x, sectors 0x%x\n", part_info->dl_filename, tmp_partstart_by_sector, rest_sectors);

				goto __download_normal_part_err1;
			}
		}
	}
	else
	{
		if(unsparse_direct_write(down_buffer, first_write_bytes))
		{
			printf("sunxi sprite error: download sparse error %s\n", part_info->dl_filename);

			goto __download_normal_part_err1;
		}
		tmp_partdata_by_bytes   -= first_write_bytes;

		while(tmp_partdata_by_bytes >= AU_ONCE_DATA_DEAL)
		{
			if (fat_fs_read(imgname, down_buffer,tmp_imgfile_start,AU_ONCE_DATA_DEAL) != AU_ONCE_DATA_DEAL)
			{
				printf("sunxi sprite error : read sdcard start 0x%x, total 0x%x failed\n", tmp_imgfile_start, AU_ONCE_DATA_DEAL);

				goto __download_normal_part_err1;
			}
			if(unsparse_direct_write(down_buffer, AU_ONCE_DATA_DEAL))
			{
				printf("sunxi sprite error: download sparse error %s\n", part_info->dl_filename);

				goto __download_normal_part_err1;
			}
			tmp_imgfile_start       += AU_ONCE_SECTOR_DEAL*512;
			tmp_partdata_by_bytes   -= AU_ONCE_DATA_DEAL;
		}
		if(tmp_partdata_by_bytes > 0)
		{
			uint rest_sectors = (tmp_partdata_by_bytes + 511)>>9;
			if (fat_fs_read(imgname, down_buffer,tmp_imgfile_start,rest_sectors*512) != rest_sectors*512)
			{
				printf("sunxi sprite error : read sdcard start 0x%x, total 0x%x failed\n", tmp_imgfile_start, rest_sectors*512);

				goto __download_normal_part_err1;
			}
			if(unsparse_direct_write(down_buffer, tmp_partdata_by_bytes))
			{
				printf("sunxi sprite error: download sparse error %s\n", part_info->dl_filename);

				goto __download_normal_part_err1;
			}
		}
	}

	tick_printf("successed in writting part %s\n", part_info->name);
	ret = 0;
	if(imgitemhd)
	{
		Img_CloseItem(imghd, imgitemhd);
		imgitemhd = NULL;
	}
	/* verify */
	if(part_info->verify)
	{
		ret = -1;
		if(part_info->vf_filename[0])
		{
			imgitemhd = Img_OpenItem(imghd, "RFSFAT16", (char *)part_info->vf_filename);
			if(!imgitemhd)
			{
				printf("sprite update warning: open part %s failed\n", part_info->vf_filename);

				goto __download_normal_part_err1;
			}
			if(!Img_Fat_ReadItem(imghd, imgitemhd, imgname, (void *)verify_data, 1024))
			{
				printf("sprite update warning: fail to read data from %s\n", part_info->vf_filename);

				goto __download_normal_part_err1;
			}
			if(partdata_format == ANDROID_FORMAT_DETECT)
			{
				active_verify = sunxi_sprite_part_sparsedata_verify();
			}
			else
			{
				active_verify = sunxi_sprite_part_rawdata_verify(partstart_by_sector, partdata_by_byte);
			}
			tmp = (uint *)verify_data;
			origin_verify = *tmp;
			printf("origin_verify value = %x, active_verify value = %x\n", origin_verify, active_verify);
			if(origin_verify != active_verify)
			{
				printf("origin checksum=%x, active checksum=%x\n", origin_verify, active_verify);
				printf("sunxi sprite: part %s verify error\n", part_info->dl_filename);

				goto __download_normal_part_err1;
			}
			ret = 0;
	    }
	    else
	    {
			printf("sunxi sprite err: part %s unablt to find verify file\n", part_info->dl_filename);
	    }
	    tick_printf("successed in verify part %s\n", part_info->name);
	}
	else
	{
		printf("sunxi sprite err: part %s not need to verify\n", part_info->dl_filename);
	}

__download_normal_part_err1:
	if(imgitemhd)
	{
		Img_CloseItem(imghd, imgitemhd);
		imgitemhd = NULL;
	}

	return ret;
}

static int __download_udisk(dl_one_part_info *part_info,  uchar *source_buff)
{
	HIMAGEITEM imgitemhd = NULL;
	u32  flash_sector;
	s64  packet_len;
	s32  ret = -1, ret1;

	imgitemhd = Img_OpenItem(imghd, "RFSFAT16", (char *)part_info->dl_filename);
	if(!imgitemhd)
	{
		printf("sunxi sprite error: open part %s failed\n", part_info->dl_filename);

		return -1;
	}

	packet_len = Img_GetItemSize(imghd, imgitemhd);
	if (packet_len <= 0)
	{
		printf("sunxi sprite error: fetch part len %s failed\n", part_info->dl_filename);

		goto __download_udisk_err1;
	}
	if (packet_len <= FW_BURN_UDISK_MIN_SIZE)
	{
		printf("download UDISK: the data length of udisk is too small, ignore it\n");

		ret = 1;
		goto __download_udisk_err1;
	}

	flash_sector = sunxi_sprite_size();
	if(!flash_sector)
	{
		printf("sunxi sprite error: download_udisk, the flash size is invalid(0)\n");

		goto __download_udisk_err1;
	}
	printf("the flash size is %d MB\n", flash_sector/2/1024);
	part_info->lenlo = flash_sector - part_info->addrlo;
	part_info->lenhi = 0;
	printf("UDISK low is 0x%x Sectors\n", part_info->lenlo);
	printf("UDISK high is 0x%x Sectors\n", part_info->lenhi);

	ret = __download_normal_part(part_info, source_buff);
__download_udisk_err1:
	ret1 = Img_CloseItem(imghd, imgitemhd);
	if(ret1 != 0 )
	{
		printf("sunxi sprite error: __download_udisk, close udisk image failed\n");

		return -1;
	}

	return ret;
}

static int __download_sysrecover_part(dl_one_part_info *part_info,  uchar *source_buff)
{
	uint partstart_by_sector;
	uint tmp_partstart_by_sector;
	s64  partsize_by_byte;
	s64  partdata_by_byte;
	s64  tmp_partdata_by_bytes;
	uint onetime_read_sectors;
	uint tmp_imgfile_start = 0;
	u8 *down_buffer = source_buff + AU_HEAD_BUFF;
	int  ret = -1;

	tmp_partstart_by_sector = partstart_by_sector = part_info->addrlo;
	partsize_by_byte     = part_info->lenlo;
	partsize_by_byte   <<= 9;

	partdata_by_byte = Img_GetSize(imghd);
	if (partdata_by_byte <= 0)
	{
		printf("sunxi sprite error: fetch part len %s failed\n", part_info->dl_filename);

		goto __download_sysrecover_part_err1;
	}

	if(partdata_by_byte > partsize_by_byte)
	{
		printf("sunxi sprite: data size 0x%x is larger than part %s size 0x%x\n", (uint)(partdata_by_byte/512), part_info->dl_filename, (uint)(partsize_by_byte/512));

		goto __download_sysrecover_part_err1;
	}

	tmp_partdata_by_bytes = partdata_by_byte;
	if(tmp_partdata_by_bytes >= AU_ONCE_DATA_DEAL)
	{
		onetime_read_sectors = AU_ONCE_SECTOR_DEAL;
	}
	else
	{
		onetime_read_sectors = (tmp_partdata_by_bytes + 511)>>9;
	}

	while(tmp_partdata_by_bytes >= AU_ONCE_DATA_DEAL)
	{
		if (fat_fs_read(imgname, down_buffer, 0, onetime_read_sectors*512) != onetime_read_sectors*512)
		{
			printf("sunxi sprite error : read sdcard start %d, total %d failed\n", tmp_imgfile_start, onetime_read_sectors*512);

			goto __download_sysrecover_part_err1;
		}
		if(sunxi_sprite_write(tmp_partstart_by_sector, onetime_read_sectors, down_buffer) != onetime_read_sectors)
		{
			printf("sunxi sprite error: download rawdata error %s, start 0x%x, sectors 0x%x\n", part_info->dl_filename, tmp_partstart_by_sector, onetime_read_sectors);

			goto __download_sysrecover_part_err1;
		}
		tmp_imgfile_start       += onetime_read_sectors*512;
		tmp_partdata_by_bytes   -= onetime_read_sectors*512;
		tmp_partstart_by_sector += onetime_read_sectors;
	}
	if(tmp_partdata_by_bytes > 0)
	{
		uint rest_sectors = (tmp_partdata_by_bytes + 511)/512;
		if (fat_fs_read(imgname, down_buffer, tmp_imgfile_start, rest_sectors*512) != rest_sectors*512)
		{
			printf("sunxi sprite error : read sdcard start %d, total %d failed\n", tmp_imgfile_start, rest_sectors*512);

			goto __download_sysrecover_part_err1;
		}
		if(sunxi_sprite_write(tmp_partstart_by_sector, rest_sectors, down_buffer) != rest_sectors)
		{
			printf("sunxi sprite error: download rawdata error %s, start 0x%x, sectors 0x%x\n", part_info->dl_filename, tmp_partstart_by_sector, rest_sectors);

			goto __download_sysrecover_part_err1;
		}
	}
	ret = 0;

__download_sysrecover_part_err1:
	if(imgitemhd)
	{
		Img_CloseItem(imghd, imgitemhd);
		imgitemhd = NULL;
	}

	return ret;
}

static int auto_update_deal_part(sunxi_download_info *dl_map)
{
	dl_one_part_info  	*part_info;
	int ret  = -1;
	int ret1;
	int i  = 0;
	uchar *down_buff  = NULL;
	int rate;

	if(!dl_map->download_count)
	{
		printf("sunxi sprite: no part need to write\n");
		return 0;
	}

	rate = (70-10)/dl_map->download_count;

	if(sunxi_sprite_init(1))
	{
		printf("sunxi sprite err: init flash err\n");
		return -1;
	}

	/*
	* for fatload, if not aligned,a misaligned buffer warning will be printed
	* and performance will suffer for the load.
	*/
	down_buff = (uchar *)malloc_aligned(AU_ONCE_DATA_DEAL + AU_HEAD_BUFF,ARCH_DMA_MINALIGN);
	if(!down_buff)
	{
		printf("sunxi sprite err: unable to malloc memory for sunxi_sprite_deal_part\n");
		goto __auto_update_deal_part_err1;
	}

	for(part_info = dl_map->one_part_info, i = 0; i < dl_map->download_count; i++, part_info++)
	{
		tick_printf("begin to download part %s\n", part_info->name);
		if(!strncmp("UDISK", (char*)part_info->name, strlen("UDISK")))
		{
			ret1 = __download_udisk(part_info, down_buff);
			if(ret1 < 0)
			{
				printf("sunxi sprite err: sunxi_sprite_deal_part, download_udisk failed\n");

				goto __auto_update_deal_part_err2;
			}
			else if(ret1 > 0)
			{
				printf("do NOT need download UDISK\n");
			}
		}
		/* sysrecovery partition: burn the whole img*/
		else if(!strncmp("sysrecovery", (char*)part_info->name, strlen("sysrecovery")))
		{
			ret1 = __download_sysrecover_part(part_info, down_buff);
			if(ret1 != 0)
			{
				printf("sunxi sprite err: sunxi_sprite_deal_part, download sysrecovery failed\n");

				goto __auto_update_deal_part_err2;
			}
		}
		/*private partition: check if need to burn private data*/
		else if(!strncmp("private", (char*)part_info->name, strlen("private")))
		{
			if(1)
			{
				/*need to burn  private part*/
				printf("NEED down private part\n");
				ret1 = __download_normal_part(part_info, down_buff);
				if(ret1 != 0)
				{
					printf("sunxi sprite err: sunxi_sprite_deal_part, download private failed\n");

					goto __auto_update_deal_part_err2;
				}
			}
			else
			{
				printf("IGNORE private part\n");
			}
		}
		else
		{
			ret1 = __download_normal_part(part_info, down_buff);
			if(ret1 != 0)
			{
				printf("sunxi sprite err: sunxi_sprite_deal_part, download normal failed\n");

				goto __auto_update_deal_part_err2;
			}
		}
		sprite_cartoon_upgrade(10 + rate * (i+1));
		tick_printf("successed in download part %s\n", part_info->name);
	}

	ret = 0;

__auto_update_deal_part_err1:
	sunxi_sprite_exit(1);

__auto_update_deal_part_err2:

	if(down_buff)
	{
		free_aligned(down_buff);
	}

	return ret;
}


static int auto_update_deal_boot0(int production_media)
{
	char buffer[1*1024*1024];
	uint item_original_size;

	if(gd->bootfile_mode  == SUNXI_BOOT_FILE_NORMAL || gd->bootfile_mode  == SUNXI_BOOT_FILE_PKG )
	{
		if (production_media == STORAGE_NAND)
		{
			imgitemhd = Img_OpenItem(imghd, "BOOT    ", "BOOT0_0000000000");
		}
		else if (production_media == STORAGE_NOR)
		{
			imgitemhd = Img_OpenItem(imghd, "12345678", "1234567890BNOR_0");
		}
		else
		{
			imgitemhd = Img_OpenItem(imghd, "12345678", "1234567890BOOT_0");
		}
	}
	else
	{
		imgitemhd = Img_OpenItem(imghd, "12345678", "TOC0_00000000000");
	}

	if(!imgitemhd)
	{
		printf("sprite update error: fail to open boot0 item\n");
		return -1;
	}

	/*get boot0 size*/
	item_original_size = Img_GetItemSize(imghd, imgitemhd);
	if(!item_original_size)
	{
		printf("sprite update error: fail to get boot0 item size\n");
		return -1;
	}
    /* read boot0 */
	 if (!Img_Fat_ReadItem(imghd, imgitemhd, imgname, (void *)buffer, 1 * 1024 * 1024))
	{
		printf("update error: fail to read data from for boot0\n");
		return -1;
	}
	Img_CloseItem(imghd, imgitemhd);
	imgitemhd = NULL;
	 if(sunxi_sprite_download_boot0(buffer, production_media))
	{
		printf("update error: fail to write boot0\n");
		return -1;
	}

    return 0;
}

int auto_update_deal_uboot(int production_media)
{
	char buffer[4 * 1024 * 1024];
	uint item_original_size;
	if(gd->bootfile_mode  == SUNXI_BOOT_FILE_NORMAL )
	{
		imgitemhd = Img_OpenItem(imghd, "12345678", "UBOOT_0000000000");
	}
	else if(gd->bootfile_mode  == SUNXI_BOOT_FILE_PKG)
	{
		imgitemhd = Img_OpenItem(imghd, "12345678", "BOOTPKG-00000000");
	}
	else
	{
		imgitemhd = Img_OpenItem(imghd, "12345678", "TOC1_00000000000");
	}

	if(!imgitemhd)
	{
	    printf("sprite update error: fail to open uboot item\n");
	    return -1;
	}
	/* get uboot size */
	item_original_size = Img_GetItemSize(imghd, imgitemhd);
	if(!item_original_size)
	{
	    printf("sprite update error: fail to get uboot item size\n");
	    return -1;
	}
	/* read uboot */
	if (!Img_Fat_ReadItem(imghd, imgitemhd, imgname, (void *)buffer, 4 * 1024 * 1024))
	{
	    printf("update error: fail to read data from for uboot\n");
	    return -1;
	}
	Img_CloseItem(imghd, imgitemhd);
	imgitemhd = NULL;

	if(sunxi_sprite_download_uboot(buffer, production_media, 0))
	{
		printf("update error: fail to write uboot\n");
	    return -1;
	}
	printf("sunxi_sprite_deal_uboot ok\n");

	return 0;
}

int sunxi_card_update_main(void)
{
	int production_media;
	uchar  img_mbr[1024 * 1024];
	sunxi_download_info  dl_map;
	int mbr_num = SUNXI_MBR_COPY_NUM;

	if (do_card0_probe(NULL,0,0,NULL))
	{
		return -1;
	}

	tick_printf("sunxi update begin\n");

	production_media = get_boot_storage_type();

	imgname = malloc(strlen(IMG_NAME)+1);
	if (imgname == NULL)
		return -1;

	strcpy(imgname,IMG_NAME);
	sprite_cartoon_create();

	if (auto_update_firmware_probe(imgname))
	{
		printf("sunxi sprite firmware probe fail\n");

		return -1;
	}

	if (au_check_img_valid() < 0)
	{
		printf("image file invalid! \n");
		return -1;
	}

	sprite_cartoon_upgrade(5);

		/* download dlmap file to get the download files*/
		tick_printf("fetch download map\n");
		if(auto_update_fetch_download_map(&dl_map))
		{
			printf("sunxi sprite error : fetch download map error\n");

			return -1;
		}
		__dump_dlmap(&dl_map);

		//fetch mbr iterm
		tick_printf("fetch mbr\n");
		if(auto_update_fetch_mbr(&img_mbr))
		{
			printf("sunxi sprite error : fetch mbr error\n");

			return -1;
		}
		__dump_mbr((sunxi_mbr_t *)img_mbr);

		/* according to the mbr,erase or protect data*/
		tick_printf("begin to erase flash\n");
		nand_get_mbr((char *)img_mbr, 16 * 1024);
		if(sunxi_sprite_erase_flash(img_mbr))
		{
			printf("sunxi sprite error: erase flash err\n");

			return -1;
		}
		tick_printf("successed in erasing flash\n");

	if (production_media == STORAGE_NOR)
	{
		mbr_num = 1;
	}

	if(sunxi_sprite_download_mbr(img_mbr, sizeof(sunxi_mbr_t) * mbr_num))
	{
		printf("sunxi sprite error: download mbr err\n");

			return -1;
		}
		sprite_cartoon_upgrade(10);

		tick_printf("begin to download part\n");
		/* start burning partition data*/
		if (auto_update_deal_part(&dl_map))
		{
			printf("sunxi sprite error : download part error\n");

			return -1;
		}
		tick_printf("successed in downloading part\n");
		sprite_cartoon_upgrade(80);
		sunxi_sprite_exit(1);

		if(auto_update_deal_uboot(production_media))
		{
			printf("sunxi sprite error : download uboot error\n");

			return -1;
		}
		tick_printf("successed in downloading uboot\n");
		sprite_cartoon_upgrade(90);

		if(auto_update_deal_boot0(production_media))
		{
			printf("sunxi sprite error : download boot0 error\n");

			return -1;
		}
	tick_printf("successed in downloading boot0\n");
	sprite_cartoon_upgrade(100);

	sprite_uichar_printf("CARD OK\n");
	tick_printf("update firmware success \n");
	__msdelay(3000);

	return 0;
}


