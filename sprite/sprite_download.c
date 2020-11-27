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
#include <config.h>
#include <common.h>
#include <private_toc.h>
#include <private_boot0.h>
#include <private_uboot.h>
#include <sunxi_mbr.h>
#include "sprite_verify.h"
#include "sprite_card.h"
#include <sunxi_nand.h>
#include <sunxi_flash.h>
#include <sunxi_board.h>
#include <malloc.h>
#ifdef CONFIG_SUNXI_GPT
#include <gpt.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

extern int sunxi_set_secure_mode(void);

int download_standard_gpt(void *sunxi_mbr_buf, size_t buf_size, int storage_type);


int  __attribute__((weak)) nand_force_download_uboot(uint length,void *buffer)
{
	return -1;
}

int  __attribute__((weak))  nand_download_uboot(uint length, void *buffer)
{
	return -1;
}

int  __attribute__((weak))  nand_write_boot0(void *buffer,uint length)
{
	return -1;
}
uint __attribute__((weak)) nand_uboot_get_flash_info(void *buffer, uint length)
{
	return 0;
}
int __attribute__((weak)) nand_download_boot0(uint length, void *buffer)
{
	return -1;
}
int __attribute__((weak)) spinor_download_uboot(uint length, void *buffer)
{
	return -1;
}
int __attribute__((weak)) spinor_download_boot0(uint length, void *buffer)
{
	return -1;
}
int __attribute__((weak)) card_download_standard_mbr(void *buffer)
{
       return -1;
}
int __attribute__((weak)) mmc_write_info(int dev_num, void *buffer, u32 buffer_size)
{
       return -1;
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
void dump_dram_para(void* dram, uint size)
{
	int i;
	uint *addr = (uint *)dram;

	for(i=0;i<size;i++)
	{
		pr_msg("dram para[%d] = %x\n", i, addr[i]);
	}
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
int sunxi_sprite_download_mbr(void *buffer, uint buffer_size)
{
	int ret;
	int storage_type = 0;
	int mbr_num = SUNXI_MBR_COPY_NUM;

	if (get_boot_storage_type() == STORAGE_NOR)
	{
		  mbr_num = 1;
	}

	if(buffer_size != (SUNXI_MBR_SIZE * mbr_num))
	{
		printf("the mbr size is bad\n");

		return -1;
	}
	if(sunxi_sprite_init(0))
	{
		printf("sunxi sprite init fail when downlaod mbr\n");

		return -1;
	}

	storage_type = get_boot_storage_type();
#ifdef CONFIG_SUNXI_GPT
	/*write sunxi mbr for compatible*/
	if(!sunxi_sprite_write(0, buffer_size/512, buffer) == (buffer_size/512))
	{
		printf("mbr write ok\n");
		ret = 0;
	}
	/*write GPT Table*/
	ret = download_standard_gpt(buffer,buffer_size,storage_type);
	if(ret)
	{
		printf("gpt write fail\n");
	}

#else
	if(sunxi_sprite_write(0, buffer_size/512, buffer) == (buffer_size/512))
	{
		debug("mbr write ok\n");

		ret = 0;
	}
	else
	{
		debug("mbr write fail\n");

		ret = -1;
	}
	if(STORAGE_EMMC == storage_type || STORAGE_EMMC3 == storage_type)
	{
		printf("begin to write standard mbr\n");
		if(card_download_standard_mbr(buffer))
		{
			printf("write standard mbr err\n");

			return -1;
		}
		printf("successed to write standard mbr\n");
	}
#endif
	if (sunxi_sprite_verify_mbr_from_flash(buffer_size / 512, mbr_num) < 0) {
		printf("sunxi_sprite_verify_mbr_from_flash fail\n");
		ret = -1;
	}
	if(sunxi_sprite_exit(0))
	{
		printf("sunxi sprite exit fail when downlaod mbr\n");

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
int sunxi_sprite_download_uboot(void *buffer, int production_media, int generate_checksum)
{
	int length = 0;
	if(gd->bootfile_mode  == SUNXI_BOOT_FILE_NORMAL)
	{
		struct spare_boot_head_t    *uboot  = (struct spare_boot_head_t *)buffer;
		printf("uboot magic %s\n", uboot->boot_head.magic);
		if(strncmp((const char *)uboot->boot_head.magic, UBOOT_MAGIC, MAGIC_SIZE))
		{
			printf("sunxi sprite: uboot magic is error\n");
			return -1;
		}
		length = uboot->boot_head.length;

	}
	else
	{
		sbrom_toc1_head_info_t *toc1 = (sbrom_toc1_head_info_t *)buffer;
		if(gd->bootfile_mode  == SUNXI_BOOT_FILE_PKG )
		{
			printf("uboot_pkg magic 0x%x\n", toc1->magic);
		}
		else
		{
			printf("toc magic 0x%x\n", toc1->magic);
		}
		if(toc1->magic != TOC_MAIN_INFO_MAGIC)
		{
			printf("sunxi sprite: toc magic is error\n");
			return -1;
		}
		length = toc1->valid_len;
		if(generate_checksum)
		{
			toc1->add_sum = sunxi_sprite_generate_checksum(buffer,
		                toc1->valid_len,toc1->add_sum);
		}
	}

	printf("uboot size = 0x%x\n", length);
	printf("storage type = %d\n", production_media);
	if (production_media == STORAGE_NAND)
	{
		debug("nand down uboot\n");
		if(uboot_spare_head.boot_data.work_mode == WORK_MODE_BOOT)
		{
			printf("work_mode_boot \n");
			return nand_force_download_uboot(length,buffer);
		}
		else
		{
#ifdef CONFIG_SUNXI_UBIFS
			if (sunxi_get_mtd_ubi_mode_status())
				return sunxi_nand_download_uboot(
					length, buffer);
			else
#endif
				return nand_download_uboot(length, buffer);
		}
	}
	else if (production_media == STORAGE_NOR)
	{
		printf("spinor down uboot\n");
		return spinor_download_uboot(length, buffer);
	}
	else
	{
		printf("mmc down uboot\n");
		return card_download_uboot(length, buffer);
	}
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
int sunxi_sprite_download_boot0(void *buffer, int production_media)
{
	if(gd->bootfile_mode  == SUNXI_BOOT_FILE_NORMAL || gd->bootfile_mode  == SUNXI_BOOT_FILE_PKG)
	{
		boot0_file_head_t    *boot0  = (boot0_file_head_t *)buffer;

		debug("%s\n", boot0->boot_head.magic);
		if(strncmp((const char *)boot0->boot_head.magic, BOOT0_MAGIC, MAGIC_SIZE))
		{
			printf("sunxi sprite: boot0 magic is error\n");

			return -1;
		}

		if(sunxi_sprite_verify_checksum(buffer, boot0->boot_head.length, boot0->boot_head.check_sum))
		{
			printf("sunxi sprite: boot0 checksum is error\n");

			return -1;
		}

		if(!production_media)
		{
#ifdef CONFIG_SUNXI_UBIFS
			if (sunxi_get_mtd_ubi_mode_status())
				sunxi_nand_uboot_get_flash_info(
					(void *)boot0->prvt_head.storage_data,
					STORAGE_BUFFER_SIZE);
			else
#endif
				nand_uboot_get_flash_info(
				(void *)boot0->prvt_head.storage_data,
				STORAGE_BUFFER_SIZE);
		}
		else
		{
			if (production_media == STORAGE_EMMC) {
				if (mmc_write_info(2,(void *)boot0->prvt_head.storage_data, STORAGE_BUFFER_SIZE)) {
					printf("add sdmmc2 private info fail!\n");
					return -1;
				}

			} else if (production_media == STORAGE_EMMC3) {
				if (mmc_write_info(3,(void *)boot0->prvt_head.storage_data,STORAGE_BUFFER_SIZE)) {
					printf("add sdmmc3 private info fail!\n");
					return -1;
				}
			}
		}
		if (uboot_spare_head.boot_data.work_mode != WORK_MODE_SPRITE_RECOVERY)
		{
			memcpy((void *)&boot0->prvt_head.dram_para, (void *)DRAM_PARA_STORE_ADDR, 32 * 4);
			/*update dram flag*/
			set_boot_dram_update_flag(boot0->prvt_head.dram_para);
		}
		dump_dram_para(boot0->prvt_head.dram_para,32);

		/* regenerate check sum */
		boot0->boot_head.check_sum = sunxi_sprite_generate_checksum(buffer, boot0->boot_head.length, boot0->boot_head.check_sum);
		if(sunxi_sprite_verify_checksum(buffer, boot0->boot_head.length, boot0->boot_head.check_sum))
		{
			printf("sunxi sprite: boot0 checksum is error\n");

			return -1;
		}
		printf("storage type = %d\n", production_media);
		if (production_media == STORAGE_NAND)
		{
#ifdef CONFIG_SUNXI_UBIFS
			if (sunxi_get_mtd_ubi_mode_status())
				return sunxi_nand_download_boot0(
						boot0->boot_head.length,
						buffer);
			else
#endif
				return nand_download_boot0(
						boot0->boot_head.length,
						buffer);
		}
		else if (production_media == STORAGE_NOR)
		{
			return  spinor_download_boot0(boot0->boot_head.length, buffer);
		}
		else
		{
			return card_download_boot0(boot0->boot_head.length, buffer,production_media);
		}
	}
	else
	{
		toc0_private_head_t  *toc0   = (toc0_private_head_t *)buffer;
		int ret;
		sbrom_toc0_config_t  *toc0_config = NULL;

		if (toc0->items_nr == 3)
			toc0_config = (sbrom_toc0_config_t *)(buffer + 0xa0);
		else
			toc0_config = (sbrom_toc0_config_t *)(buffer + 0x80);

		debug("%s\n", (char *)toc0->name);
		if(strncmp((const char *)toc0->name, TOC0_MAGIC, MAGIC_SIZE))
		{
			printf("sunxi sprite: toc0 magic is error\n");

			return -1;
		}
		//
		if(sunxi_sprite_verify_checksum(buffer, toc0->length, toc0->check_sum))
		{
			printf("sunxi sprite: toc0 checksum is error\n");

			return -1;
		}
		//update flash param
		if(!production_media)
		{
#ifdef CONFIG_SUNXI_UBIFS
			if (sunxi_get_mtd_ubi_mode_status())
				sunxi_nand_uboot_get_flash_info(
					(void *)toc0_config->storage_data,
					STORAGE_BUFFER_SIZE);
			else
#endif
				nand_uboot_get_flash_info(
					(void *)toc0_config->storage_data,
					STORAGE_BUFFER_SIZE);
		}else{
			//storage_data[384];  // 0-159:nand info  160-255:card info
			if (production_media == STORAGE_EMMC) {
				if (mmc_write_info(2,(void *)(toc0_config->storage_data+160),384-160)){
					printf("add sdmmc2 gpio info fail!\n");
					return -1;
				}
			} else if (production_media == STORAGE_EMMC3) {
				if (mmc_write_info(3,(void *)(toc0_config->storage_data+160),384-160)){
					printf("add sdmmc3 gpio info fail!\n");
					return -1;
				}
			}
		}

		//update dram param
		if (uboot_spare_head.boot_data.work_mode == WORK_MODE_CARD_PRODUCT ||
			uboot_spare_head.boot_data.work_mode == WORK_MODE_UDISK_UPDATE)
		{
			memcpy((void *)toc0_config->dram_para, (void *)(uboot_spare_head.boot_data.dram_para), 32 * 4);
			//toc0_config->dram_para[4] += toc0_config->secure_dram_mbytes;
			/*update dram flag*/
			set_boot_dram_update_flag(toc0_config->dram_para);
		}
		else if (uboot_spare_head.boot_data.work_mode == WORK_MODE_SPRITE_RECOVERY)
		{
			printf("skip memcpy dram para for work_mode recovery\n");
		}
		else
		{
			memcpy((void *)toc0_config->dram_para, (void *)DRAM_PARA_STORE_ADDR, 32 * 4);
			/*update dram flag*/
			set_boot_dram_update_flag(toc0_config->dram_para);
		}

		dump_dram_para( toc0_config->dram_para,32);

		/* regenerate check sum */
		toc0->check_sum = sunxi_sprite_generate_checksum(buffer, toc0->length, toc0->check_sum);
		if(sunxi_sprite_verify_checksum(buffer, toc0->length, toc0->check_sum))
		{
			printf("sunxi sprite: boot0 checksum is error\n");

			return -1;
		}
		printf("storage type = %d\n", production_media);
		if (production_media == STORAGE_NAND)
		{
#ifdef CONFIG_SUNXI_UBIFS
			if (sunxi_get_mtd_ubi_mode_status())
				ret = sunxi_nand_download_boot0(
					toc0->length, buffer);
			else
#endif
				ret = nand_download_boot0(toc0->length, buffer);
		}
		else if (production_media == STORAGE_NOR)
		{
			ret = spinor_download_boot0(toc0->length, buffer);
		}
		else
		{
			ret = card_download_boot0(toc0->length, buffer,production_media);
		}
		if(!ret)
		{
			sunxi_set_secure_mode();
		}

		return ret;
	}
}

/*
************************************************************************************************************
*
*                                             function
*
*    name          : sunxi_download_boot0_atfter_ota
*
*    parmeters     :
*
*    return        :
*
*    note          : only for dram para update ater ota
*
*
************************************************************************************************************
*/
int sunxi_download_boot0_atfter_ota(void *buffer, int production_media)
{
	u32 length;
	int card_num = 2;

	if (production_media == STORAGE_EMMC3)
	{
		card_num = 3;
	}

	if(SUNXI_NORMAL_MODE == sunxi_get_securemode())
	{
		boot0_file_head_t    *boot0  = (boot0_file_head_t *)buffer;

		printf("normal mode\n");
		if(strncmp((const char *)boot0->boot_head.magic, BOOT0_MAGIC, MAGIC_SIZE))
		{
			printf("sunxi sprite: boot0 magic is error\n");
			return -1;
		}

		if(sunxi_sprite_verify_checksum(buffer, boot0->boot_head.length, boot0->boot_head.check_sum))
		{
			printf("sunxi sprite: boot0 checksum is error\n");
			return -1;
		}

		if(get_boot_dram_update_flag())
		{
			memcpy((void *)&boot0->prvt_head.dram_para,
				(void *)get_boot_dram_para_addr(), get_boot_dram_para_size());
			/*update ota flag*/
			set_boot_dram_update_flag(boot0->prvt_head.dram_para);
			dump_dram_para(boot0->prvt_head.dram_para,32);
		}

		/* udpate mmc private info */
		if (mmc_request_update_boot0(card_num))
		{
			if (mmc_write_info(card_num,(void *)boot0->prvt_head.storage_data, STORAGE_BUFFER_SIZE)) {
				printf("%s: update mmc private info fail!\n", __FUNCTION__);
				return -1;
			}
		}

		/* regenerate check sum */
		boot0->boot_head.check_sum = sunxi_sprite_generate_checksum(buffer, boot0->boot_head.length, boot0->boot_head.check_sum);
		if(sunxi_sprite_verify_checksum(buffer, boot0->boot_head.length, boot0->boot_head.check_sum))
		{
			printf("sunxi sprite: boot0 checksum is error\n");

			return -1;
		}
		length = boot0->boot_head.length;

	}
	else
	{
		toc0_private_head_t  *toc0   = (toc0_private_head_t *)buffer;
		sbrom_toc0_config_t  *toc0_config = NULL;

		if (toc0->items_nr == 3)
			toc0_config = (sbrom_toc0_config_t *)(buffer + 0xa0);
		else
			toc0_config = (sbrom_toc0_config_t *)(buffer + 0x80);

		printf("secure mode\n");
		if(strncmp((const char *)toc0->name, TOC0_MAGIC, MAGIC_SIZE))
		{
			printf("sunxi sprite: toc0 magic is error\n");
			return -1;
		}

		if(sunxi_sprite_verify_checksum(buffer, toc0->length, toc0->check_sum))
		{
			printf("sunxi sprite: toc0 checksum is error\n");
			return -1;
		}

		//update dram param
		if(get_boot_dram_update_flag())
		{
			memcpy((void *)toc0_config->dram_para,
				(void *)get_boot_dram_para_addr(), get_boot_dram_para_size());
			/*update dram flag*/
			set_boot_dram_update_flag(toc0_config->dram_para);
			dump_dram_para(toc0_config->dram_para,32);
		}


		/* udpate mmc private info */
		if (mmc_request_update_boot0(card_num))
		{
			if (mmc_write_info(2,(void *)(toc0_config->storage_data+160),384-160)){
				printf("%s: update sdmmc2 gpio info fail!\n", __FUNCTION__);
				return -1;
			}
		}

		/* regenerate check sum */
		toc0->check_sum = sunxi_sprite_generate_checksum(buffer, toc0->length, toc0->check_sum);
		if(sunxi_sprite_verify_checksum(buffer, toc0->length, toc0->check_sum))
		{
			printf("sunxi sprite: boot0 checksum is error\n");
			return -1;
		}
		length = toc0->length;
	}

	if(!production_media)
	{
		return nand_write_boot0(buffer,length);
	}
	else
	{
		return card_download_boot0(length, buffer,production_media);
	}
}

#ifdef CONFIG_SUNXI_GPT

int sunxi_mbr_convert_to_gpt(void *sunxi_mbr_buf, char *gpt_buf,int storage_type)
{
	legacy_mbr   *remain_mbr;
	sunxi_mbr_t  *sunxi_mbr = (sunxi_mbr_t *)sunxi_mbr_buf;

	char         *pbuf = gpt_buf;
	gpt_header   *gpt_head;
	gpt_entry    *pgpt_entry = NULL;
	char*         gpt_entry_start=NULL;
	u32           data_len = 0;
	int           total_sectors;
	u32           logic_offset = 0;
	int           i,j = 0;

	unsigned char guid[16] = {0x88,0x38,0x6f,0xab,0x9a,0x56,0x26,0x49,0x96,0x68,0x80,0x94,0x1d,0xcb,0x40,0xbc};
	unsigned char part_guid[16] = {0x46,0x55,0x08,0xa0,0x66,0x41,0x4a,0x74,0xa3,0x53,0xfc,0xa9,0x27,0x2b,0x8e,0x45};

	if(strncmp((const char*)sunxi_mbr->magic, SUNXI_MBR_MAGIC, 8))
	{
		printf("%s:not sunxi mbr, can't convert to GPT partition\n", __func__);
		return 0;
	}

	if(crc32(0, (const unsigned char *)(sunxi_mbr_buf + 4), SUNXI_MBR_SIZE - 4) != sunxi_mbr->crc32)
	{
		printf("%s:sunxi mbr crc error, can't convert to GPT partition\n",__func__);
		return 0;
	}

	if(storage_type == STORAGE_EMMC || storage_type == STORAGE_EMMC3
		|| storage_type == STORAGE_SD)
	{
		logic_offset = CONFIG_MMC_LOGICAL_OFFSET;
	}
	else
	{
		logic_offset = 0;
	}

	if(WORK_MODE_BOOT == get_boot_work_mode())
		total_sectors = sunxi_flash_size();
	else
		total_sectors = sunxi_sprite_size();
	/* 1. LBA0: write legacy mbr,part type must be 0xee */
	remain_mbr = (legacy_mbr *)pbuf;
	memset(remain_mbr, 0x0, 512);
	remain_mbr->partition_record[0].sector = 0x2;
	remain_mbr->partition_record[0].cyl = 0x0;
	remain_mbr->partition_record[0].sys_ind = EFI_PMBR_OSTYPE_EFI_GPT;
	remain_mbr->partition_record[0].end_head = 0xFF;
	remain_mbr->partition_record[0].end_sector = 0xFF;
	remain_mbr->partition_record[0].end_cyl = 0xFF;
	remain_mbr->partition_record[0].start_sect = 1UL;
	remain_mbr->partition_record[0].nr_sects = 0xffffffff;
	remain_mbr->signature = MSDOS_MBR_SIGNATURE;
	data_len += 512;

	/* 2. LBA1: fill primary gpt header */
	gpt_head = (gpt_header *)(pbuf + data_len);
	gpt_head->signature= GPT_HEADER_SIGNATURE;
	gpt_head->revision = GPT_HEADER_REVISION_V1;
	gpt_head->header_size = GPT_HEADER_SIZE;
	gpt_head->header_crc32 = 0x00;
	gpt_head->reserved1 = 0x0;
	gpt_head->my_lba = 0x01;
	gpt_head->alternate_lba = total_sectors - 1;
	gpt_head->first_usable_lba = sunxi_mbr->array[0].addrlo + logic_offset;
	/*1 GPT head + 32 GPT entry*/
	gpt_head->last_usable_lba = total_sectors - (1 + 32) - 1;
	memcpy(gpt_head->disk_guid.b,guid,16);
	gpt_head->partition_entry_lba = (storage_type == STORAGE_NAND) ? 2 : PRIMARY_GPT_ENTRY_OFFSET;
	gpt_head->num_partition_entries = 0x80;
	gpt_head->sizeof_partition_entry = GPT_ENTRY_SIZE;
	gpt_head->partition_entry_array_crc32 = 0;
	data_len += 512;

	/* 3. LBA2~LBAn: fill gpt entry */
	gpt_entry_start = (pbuf + data_len);
	for(i=0;i<sunxi_mbr->PartCount;i++)
	{
		/*udisk is the first part*/
		int pos = (i == sunxi_mbr->PartCount-1) ? 0: i+1;
		pgpt_entry = (gpt_entry *)(gpt_entry_start + (pos)*GPT_ENTRY_SIZE);

		pgpt_entry->partition_type_guid = PARTITION_BASIC_DATA_GUID;

		memcpy(pgpt_entry->unique_partition_guid.b,part_guid,16);
		pgpt_entry->unique_partition_guid.b[15] = part_guid[15]+i;

		pgpt_entry->starting_lba = ((u64)sunxi_mbr->array[i].addrhi<<32) + sunxi_mbr->array[i].addrlo + logic_offset;
		pgpt_entry->ending_lba = pgpt_entry->starting_lba \
			+((u64)sunxi_mbr->array[i].lenhi<<32)  \
			+ sunxi_mbr->array[i].lenlo-1;

		//UDISK partition
		if(i == sunxi_mbr->PartCount-1)
		{
			pgpt_entry->ending_lba = gpt_head->last_usable_lba - 1;
		}

		printf("GPT:%-12s: %-12llx  %-12llx\n", sunxi_mbr->array[i].name, pgpt_entry->starting_lba, pgpt_entry->ending_lba);
		if(sunxi_mbr->array[i].ro == 1)
		{
			pgpt_entry->attributes.fields.type_guid_specific = 0x6000;
		}
		else
		{
			pgpt_entry->attributes.fields.type_guid_specific = 0x8000;
		}

		//ASCII to unicode
		memset(pgpt_entry->partition_name, 0,PARTNAME_SZ*sizeof(efi_char16_t));
		for(j=0;j < strlen((const char *)sunxi_mbr->array[i].name);j++ )
		{
			pgpt_entry->partition_name[j] = (efi_char16_t)sunxi_mbr->array[i].name[j];
		}
		data_len += GPT_ENTRY_SIZE;

	}

	//entry crc
	gpt_head->partition_entry_array_crc32 = crc32(0, (unsigned char const *)gpt_entry_start,
	             (gpt_head->num_partition_entries)*(gpt_head->sizeof_partition_entry));


	//gpt crc
	gpt_head->header_crc32 = crc32(0,(const unsigned char *)gpt_head, sizeof(gpt_header));
	printf("gpt_head->header_crc32 = 0x%x\n",gpt_head->header_crc32);

	/* 4. LBA-1: the last sector fill backup gpt header */

	return data_len;
}

int down_primary_gpt_for_sdmmc(void *gpt_buf, int len)
{
	typedef int (*FLASH_WIRTE)(uint start_block, uint nblock, void *buffer);
	FLASH_WIRTE flash_write_pt = NULL;
	char  gpt_buf_tmp[1024] = {0};
	int ret = 0;

	flash_write_pt = WORK_MODE_BOOT == get_boot_work_mode()  ? \
		sunxi_flash_phywrite : sunxi_sprite_phywrite;

	memcpy(gpt_buf_tmp, gpt_buf, 1024);

	/*write GPT entries: GPT_ENTRY_NUMBERS* GPT_ENTRY_SIZE/512 */
	/*to avoid  the first boot0(offset is 16 sectors), so not use the pre area in eMMC.*/
	ret = flash_write_pt(PRIMARY_GPT_ENTRY_OFFSET,32,gpt_buf+GPT_ENTRY_OFFSET);
	if(!ret)
	{
		printf("emmc write primary gpt entry fail\n");
		return -1;
	}
	/*write legacy MBR and primary GPT Head*/
	ret = flash_write_pt(0, 2, gpt_buf_tmp);
	if(!ret)
	{
		printf("emmc write primary gpt head fail\n");
		return -1;
	}
	return 0;
}

int download_standard_gpt(void *sunxi_mbr_buf, size_t buf_size, int storage_type)
{
	char  *gpt_buf = NULL;
	int   data_len = 0;
	int   ret = 0;
	__maybe_unused gpt_header   *gpt_head;
	int  gpt_buf_len = GPT_BUF_MAX_SIZE;

	gpt_buf = malloc(gpt_buf_len);
	if(gpt_buf == NULL)
	{
		printf("malloc for GPT  fail\n");
		return -1;
	}
	memset(gpt_buf, 0x0, gpt_buf_len);

	data_len = sunxi_mbr_convert_to_gpt(sunxi_mbr_buf, gpt_buf, storage_type);
	if(data_len == 0)
	{
		return -1;
	}

	/*sprite for nand/eMMC, u-boot will check this GPT*/
	ret = sunxi_sprite_write(0, gpt_buf_len>>9, gpt_buf);
	if(!ret)
	{
		printf("%s:write gpt sectors fail\n",__func__);
		return -1;
	}
	/*sprite for eMMC*/
	if(STORAGE_EMMC == storage_type || STORAGE_EMMC3 == storage_type)
	{
		if(down_primary_gpt_for_sdmmc(gpt_buf, gpt_buf_len))
		{
			return -1;
		}
	}
	printf("write gpt success\n");
	return 0;
}

int card0_convert_to_gpt(void *sunxi_mbr_buf, int mode)
{
	char  *gpt_buf = NULL;
	int   data_len = 0;
	int   ret = 0;

	int  gpt_buf_len = GPT_BUF_MAX_SIZE;

	gpt_buf = malloc(gpt_buf_len);
	if(gpt_buf == NULL)
	{
		printf("malloc for GPT  fail\n");
		return -1;
	}
	memset(gpt_buf, 0x0, gpt_buf_len);

	data_len = sunxi_mbr_convert_to_gpt(sunxi_mbr_buf, gpt_buf, STORAGE_SD);
	if(data_len == 0)
	{
		return -1;
	}

	if(mode&GPT_UPDATE_PRIMARY_MBR)
	{
		if(down_primary_gpt_for_sdmmc(gpt_buf, gpt_buf_len))
		{
			return -1;
		}
	}

	if(mode&GPT_UPDATE_SUNXI_MBR)
	{
		/*step 1: write GPT entries, we write entries at first to avoid convert fail*/
		ret = sunxi_flash_write(2, gpt_buf_len-2, gpt_buf+2*512);
		if(!ret)
		{
			printf("%s: write sunxi gpt entry fail\n",__func__);
			return -1;
		}
		/*step 2: write legacy MBR and primary GPT Head*/
		ret = sunxi_flash_write(0, 2, gpt_buf);
		if(!ret)
		{
			printf("%s: write sunxi gpt head fail\n",__func__);
			return -1;
		}
	}

	printf("convert SunxiMBR to GPT success\n");
	return 0;
}


#endif

