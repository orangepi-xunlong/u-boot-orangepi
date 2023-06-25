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
#include <sunxi_nand.h>
#include <sunxi_flash.h>
#include <sunxi_board.h>
#include <malloc.h>
#include <sprite_verify.h>
#include <part_efi.h>
#include <fdt_support.h>
#include <sunxi_image_verifier.h>
#include <asm/arch/rtc.h>
#include <sys_partition.h>

DECLARE_GLOBAL_DATA_PTR;

#define MMC_LOGICAL_OFFSET   (20 * 1024 * 1024/512)
#define GPT_BUFF_SIZE        (8*1024)

#ifdef CONFIG_SUNXI_SPINOR
#ifdef CONFIG_SUNXI_RTOS
#define SPINOR_LOGICAL_OFFSET CONFIG_SUNXI_RTOS_LOGICAL_OFFSET
#else
#define SPINOR_LOGICAL_OFFSET CONFIG_SPINOR_LOGICAL_OFFSET
#endif
#endif

int sunxi_mbr_convert_to_gpt(void *sunxi_mbr_buf, char *gpt_buf,int storage_type);
int download_standard_gpt(void *sunxi_mbr_buf, size_t buf_size, int storage_type);
extern int sunxi_set_secure_mode(void);

static void dump_dram_para(void* dram, uint size)
{
	int i;
	uint *addr = (uint *)dram;

	for(i = 0; i < size; i++) {
		printf("dram para[%d] = %x\n", i, addr[i]);
	}
}
static void prepare_backup_gpt_header(gpt_header *gpt_h)
{
	uint32_t calc_crc32;
	uint64_t val;

	/* recalculate the values for the Backup GPT Header */
	val = le64_to_cpu(gpt_h->my_lba);
	gpt_h->my_lba = gpt_h->alternate_lba; /*total_sectors - 1*/
	gpt_h->alternate_lba = cpu_to_le64(val);
	gpt_h->partition_entry_lba =
			cpu_to_le64(le64_to_cpu(gpt_h->last_usable_lba) + 1);
	gpt_h->header_crc32 = 0;

	calc_crc32 = crc32(0,(const unsigned char *)gpt_h,
			       le32_to_cpu(gpt_h->header_size));
	gpt_h->header_crc32 = cpu_to_le32(calc_crc32);
}

#ifdef CONFIG_SUNXI_COMPARE_IMAGE_AND_FLASH
static int compare_image_and_flash_size(u8 *buf)
{
	sunxi_mbr_t *mbr_info = (sunxi_mbr_t *)buf;
	sunxi_partition *part_info;
	u32 i;
	char buffer[32];
	uint logic_size = 0;
	uint flash_size = 0;

	for (part_info = mbr_info->array, i = 0; i < mbr_info->PartCount;
		     i++, part_info++) {
		memset(buffer, 0, 32);
		memcpy(buffer, part_info->name, 16);
		//NOTE : Use UDISK addrlo as the size of image
		if ((!strcmp(buffer, "UDISK")) || (!strcmp(buffer, CONFIG_LAST_PARTITION_NAME))) {
			logic_size = part_info->addrlo;
		}
	}
	flash_size = sunxi_flash_size();
	pr_msg("logic size : 0x%x sector(512 byte)\n", logic_size);
	pr_msg("flash size : 0x%x sector(512 byte)\n", flash_size);

	if (flash_size == 0) {
		pr_msg("flash size is zero, ignore check\n");
		return 0;
	}
	if (flash_size > logic_size) {
		return 0;
	} else {
		pr_err("error : logic_size biger than flash_size\n");
		return -1;
	}
}
#endif

int sunxi_sprite_download_mbr(void *buffer, uint buffer_size)
{
	int ret = 0;
	int storage_type = 0;
	int mbr_num = SUNXI_MBR_COPY_NUM;

	if (get_boot_storage_type() == STORAGE_NOR) {
		  mbr_num = 1;
	}

	if(buffer_size != (SUNXI_MBR_SIZE * mbr_num)) {
		printf("the mbr size is bad\n");
		return -1;
	}

#ifdef CONFIG_SUNXI_COMPARE_IMAGE_AND_FLASH
	if (compare_image_and_flash_size(buffer)) {
		pr_err("please check you image and flash size!\n");
		pr_err("=============Exit burning now=============\n");
		return -1;
	}
#endif

	storage_type = get_boot_storage_type();
	if ((storage_type == STORAGE_NAND) && (sunxi_sprite_init(0))) {
		return -2;
	}
	/*write GPT Table*/
	ret = download_standard_gpt(buffer,buffer_size,storage_type);
	if(ret) {
		return -3;
	}
	pr_msg("update partition map\n");
	sunxi_probe_partition_map();

	return ret;

}

int sunxi_sprite_download_uboot(void *buffer, int production_media, int generate_checksum)
{
	u32 length = 0;

	sbrom_toc1_head_info_t *toc1 = (sbrom_toc1_head_info_t *)buffer;

	if(toc1->magic != TOC_MAIN_INFO_MAGIC)
	{
		printf("sunxi sprite: toc magic is error\n");
		printf("need %s image\n", gd->bootfile_mode == SUNXI_BOOT_FILE_TOC ? "secure" : "normal");
		return -1;
	}
	length = toc1->valid_len;
	if(generate_checksum) {
		toc1->add_sum = sunxi_sprite_generate_checksum(buffer,
					toc1->valid_len,toc1->add_sum);
	}

	printf("uboot size = 0x%x\n", length);
	printf("storage type = %d\n", production_media);
#if defined(CONFIG_SUNXI_BURN_ROTPK_ON_SPRITE) || \
	defined(CONFIG_SUNXI_ROTPK_BURN_ENABLE_BY_TOOL)
	sunxi_verify_preserve_toc1(buffer);
#endif

	return sunxi_sprite_download_toc(buffer, length, production_media);

}

#if defined(CONFIG_SUNXI_ROTPK_BURN_ENABLE_BY_TOOL)
int set_rotpk_flag(unsigned char *flag)
{
	if ((uboot_spare_head.boot_data.func_mask &
			UBOOT_FUNC_MASK_BIT_BURN_ROTPK) !=
			UBOOT_FUNC_MASK_BIT_BURN_ROTPK) {
		printf("tool did not set rotpk burn flag, skip rotpk burn\n");
		*flag = 0;
		return 0;
	}
	if (gd->securemode == SUNXI_NORMAL_MODE) {
		printf("normal mode, don't need set rotpk flag\n");
		*flag = 0;
		return 0;
	}

	printf("set rotpk flag to toc0 header\n");
	*flag = 1;

	return 0;
}
#endif

int download_secure_boot0(void *buffer, int production_media)
{

	toc0_private_head_t  *toc0	 = (toc0_private_head_t *)buffer;
	sbrom_toc0_config_t  *toc0_config = NULL;
	int ret = 0;

	if (toc0->items_nr == 3)
		toc0_config = (sbrom_toc0_config_t *)(buffer + 0xa0);
	else
		toc0_config = (sbrom_toc0_config_t *)(buffer + 0x80);

	debug("%s\n", (char *)toc0->name);
	if(strncmp((const char *)toc0->name, TOC0_MAGIC, MAGIC_SIZE))
	{
		printf("sunxi sprite: toc0 magic is error, need secure image\n");

		return -1;
	}

	if(sunxi_sprite_verify_checksum(buffer, toc0->length, toc0->check_sum))
	{
		printf("sunxi sprite: toc0 checksum is error\n");

		return -1;
	}
#ifdef CONFIG_SUNXI_TURNNING_FLASH
	//update flash param
	if(!production_media)
	{
#ifdef CONFIG_SUNXI_UBIFS
		if (nand_use_ubi()) {
			ubi_nand_get_flash_info(
					(void *)toc0_config->storage_data,
					STORAGE_BUFFER_SIZE);
		}
#elif defined(CONFIG_SUNXI_COMM_NAND_V1) ||  defined(CONFIG_SUNXI_COMM_NAND)
		nand_uboot_get_flash_info(
				(void *)toc0_config->storage_data,
				STORAGE_BUFFER_SIZE);
#else
	printf("not define RAWNAND & SPINAND\n");
	return -1;
#endif
	} else {
#ifdef CONFIG_SUNXI_SDMMC
		//storage_data[384];  // 0-159:nand info  160-255:card info
		if (production_media == STORAGE_EMMC) {
			if (mmc_write_info(2, (void *)(toc0_config->storage_data+160), 384-160)) {
				printf("add sdmmc2 gpio info fail!\n");
				return -1;
			}
		} else if (production_media == STORAGE_EMMC3) {
			if (mmc_write_info(3, (void *)(toc0_config->storage_data+160), 384-160)) {
				printf("add sdmmc3 gpio info fail!\n");
				return -1;
			}
		} else if (production_media == STORAGE_EMMC0) {
			if (mmc_write_info(0, (void *)(toc0_config->storage_data+160), 384-160)) {
				printf("add sdmmc0 gpio info fail!\n");
				return -1;
			}
		}
#endif
	}
#endif

#ifdef CONFIG_SUNXI_TURNNING_DRAM
	//update dram param
	if (uboot_spare_head.boot_data.work_mode == WORK_MODE_CARD_PRODUCT ||
		(uboot_spare_head.boot_data.work_mode == WORK_MODE_BOOT && get_boot_dram_update_flag())) {
		memcpy((void *)toc0_config->dram_para, (void *)(uboot_spare_head.boot_data.dram_para), 32 * 4);
		/*update dram flag*/
		set_boot_dram_update_flag(toc0_config->dram_para);
	} else if (uboot_spare_head.boot_data.work_mode == WORK_MODE_UDISK_UPDATE ||
			uboot_spare_head.boot_data.work_mode == WORK_MODE_CARD_UPDATE ||
			uboot_spare_head.boot_data.work_mode == WORK_MODE_SPRITE_RECOVERY ||
			uboot_spare_head.boot_data.work_mode ==  WORK_MODE_BOOT) {
		printf("skip memcpy dram para for work_mode: %d \n", uboot_spare_head.boot_data.work_mode);
	} else {
		memcpy((void *)toc0_config->dram_para, (void *)CONFIG_DRAM_PARA_ADDR, 32 * 4);
		/*update dram flag*/
		set_boot_dram_update_flag(toc0_config->dram_para);
	}
#endif
	dump_dram_para(toc0_config->dram_para, 32);

#if defined(CONFIG_SUNXI_ROTPK_BURN_ENABLE_BY_TOOL)
	set_rotpk_flag(&toc0_config->rotpk_flag);
#endif

	/* regenerate check sum */
	toc0->check_sum = sunxi_sprite_generate_checksum(buffer, toc0->length, toc0->check_sum);
	if (sunxi_sprite_verify_checksum(buffer, toc0->length, toc0->check_sum)) {
		printf("sunxi sprite: boot0 checksum is error\n");
		return -1;
	}
	printf("storage type = %d\n", production_media);
	ret = sunxi_sprite_download_spl(buffer, toc0->length, production_media);

	if (!ret) {
		printf("burn sboot ok, set secure bit\n");
		ret = sunxi_set_secure_mode();
	}

	return ret;


}

int download_normal_boot0(void *buffer, int production_media)
{

	boot0_file_head_t    *boot0  = (boot0_file_head_t *)buffer;

	debug("%s\n", boot0->boot_head.magic);
	if (strncmp((const char *)boot0->boot_head.magic, BOOT0_MAGIC, MAGIC_SIZE)) {
		printf("sunxi sprite: boot0 magic is error\n");
		return -1;
	}

	if (sunxi_sprite_verify_checksum(buffer, boot0->boot_head.length, boot0->boot_head.check_sum)) {
		printf("sunxi sprite: boot0 checksum is error\n");
		return -1;
	}
#ifdef CONFIG_SUNXI_TURNNING_FLASH
	if(!production_media) {
#ifdef CONFIG_SUNXI_UBIFS
		if (nand_use_ubi()) {
			ubi_nand_get_flash_info(
					(void *)boot0->prvt_head.storage_data,
					STORAGE_BUFFER_SIZE);
		}
#elif defined(CONFIG_SUNXI_COMM_NAND_V1) ||  defined(CONFIG_SUNXI_COMM_NAND)
		nand_uboot_get_flash_info(
			(void *)boot0->prvt_head.storage_data,
			STORAGE_BUFFER_SIZE);

#else
		printf("not define RAWNAND & SPINAND\n");
		return -1;
#endif
	} else {
#ifdef CONFIG_SUNXI_SDMMC
		if (production_media == STORAGE_EMMC) {
			if (mmc_write_info(2, (void *)boot0->prvt_head.storage_data, STORAGE_BUFFER_SIZE)) {
				printf("add sdmmc2 private info fail!\n");
				return -1;
			}
		} else if (production_media == STORAGE_EMMC3) {
			if (mmc_write_info(3, (void *)boot0->prvt_head.storage_data, STORAGE_BUFFER_SIZE)) {
				printf("add sdmmc3 private info fail!\n");
				return -1;
			}
		} else if (production_media == STORAGE_EMMC0) {
			if (mmc_write_info(0, (void *)boot0->prvt_head.storage_data, STORAGE_BUFFER_SIZE)) {
				printf("add sdmmc0 private info fail!\n");
				return -1;
			}
		}
#endif
	}
#endif

#ifdef CONFIG_SUNXI_TURNNING_DRAM
	if (uboot_spare_head.boot_data.work_mode == WORK_MODE_CARD_PRODUCT ||
		(uboot_spare_head.boot_data.work_mode == WORK_MODE_BOOT && get_boot_dram_update_flag())) {
		memcpy((void *)&boot0->prvt_head.dram_para, (void *)(uboot_spare_head.boot_data.dram_para), 32 * 4);
		/*update dram flag*/
		set_boot_dram_update_flag(boot0->prvt_head.dram_para);
	} else if (uboot_spare_head.boot_data.work_mode == WORK_MODE_UDISK_UPDATE ||
			uboot_spare_head.boot_data.work_mode == WORK_MODE_CARD_UPDATE ||
			uboot_spare_head.boot_data.work_mode == WORK_MODE_SPRITE_RECOVERY ||
			uboot_spare_head.boot_data.work_mode ==  WORK_MODE_BOOT) {
		printf("skip memcpy dram para for work_mode: %d \n", uboot_spare_head.boot_data.work_mode);
	} else {
		memcpy((void *)boot0->prvt_head.dram_para, (void *)CONFIG_DRAM_PARA_ADDR, 32 * 4);
		/*update dram flag*/
		set_boot_dram_update_flag(boot0->prvt_head.dram_para);
	}
#endif
	dump_dram_para(boot0->prvt_head.dram_para,32);

	/* regenerate check sum */
	boot0->boot_head.check_sum = sunxi_sprite_generate_checksum(buffer, boot0->boot_head.length, boot0->boot_head.check_sum);
	if(sunxi_sprite_verify_checksum(buffer, boot0->boot_head.length, boot0->boot_head.check_sum)) {
		printf("sunxi sprite: boot0 checksum is error\n");
		return -1;
	}
	printf("storage type = %d\n", production_media);
	return sunxi_sprite_download_spl(buffer, boot0->boot_head.length, production_media);
}

int sunxi_sprite_download_boot0(void *buffer, int production_media)
{
	if(gd->bootfile_mode  == SUNXI_BOOT_FILE_NORMAL || gd->bootfile_mode  == SUNXI_BOOT_FILE_PKG){
		return download_normal_boot0(buffer, production_media);
	} else {
		return download_secure_boot0(buffer, production_media);
	}
	/* usb dma recv time out maybe enter usb product twice
	 * brom enter fel mode or boot0 enter fel mode
	 */
	rtc_set_bootmode_flag(0);
}

void sunxi_get_logical_offset_param(int storage_type, u32 *logic_offset,
				    int *total_sectors)
{
	/*
	 * for nand, no phyread available, so this is not relevant
	 * for mmc, part offset is physical offset, offset should be taken care in start value, refer by logic_offset here
	 * for nor, part offset is logical offset, offset should be taken care in end value, refer by total_sectors here
	 */
	if (storage_type == STORAGE_EMMC || storage_type == STORAGE_EMMC3 ||
	    storage_type == STORAGE_SD || storage_type == STORAGE_EMMC0) {
		*logic_offset = MMC_LOGICAL_OFFSET;
	} else {
		*logic_offset = 0;
	}

	*total_sectors = sunxi_sprite_size();
#ifdef CONFIG_SUNXI_SPINOR
	if (storage_type == STORAGE_NOR) {
		*total_sectors -= SPINOR_LOGICAL_OFFSET;
	}
#endif
}

int gpt_convert_to_sunxi_mbr(void *sunxi_mbr_buf, char *gpt_buf, int storage_type)
{
	u32 data_len	  = 0;
	char *pbuf	    = gpt_buf;
	gpt_entry *pgpt_entry = NULL;
	char *gpt_entry_start = NULL;
	int PartCount	 = 0;
	int pos		      = 0;
	int i, j;
	int crc32_total;
	int mbr_size = 0;
	u64 start_sector;
	int total_sectors;
	u32 logic_offset = 0;

	/* mbr_size = 256; [> hardcode, TODO: fixit <] */
	/* mbr_size = mbr_size * (1024/512); */

	sunxi_mbr_t *sunxi_mbr = (sunxi_mbr_t *)sunxi_mbr_buf;
	memset(sunxi_mbr, 0, sizeof(sunxi_mbr_t));

	sunxi_mbr->version = 0x00000200;
	memcpy(sunxi_mbr->magic, SUNXI_MBR_MAGIC, 8);
	sunxi_mbr->copy = 1;

	data_len = 0;
	data_len += 512; /* 0 to gpt->head */
	data_len += 512; /* gpt->head to gpt_entry */
	gpt_entry_start = (pbuf + data_len);
	while (1) {
		pgpt_entry = (gpt_entry *)(gpt_entry_start + (pos)*GPT_ENTRY_SIZE);
		if ((long)(pgpt_entry->starting_lba) > 0) {
			printf("gpt pos:%d s:0x%llx e:0x%llx\n", pos, pgpt_entry->starting_lba, pgpt_entry->ending_lba);
			PartCount++;
		} else
			break;

		pos++;
	}
	sunxi_mbr->PartCount = PartCount;
	printf("PartCount = %d\n", PartCount);
	sunxi_mbr->PartCount += 1; //need UDISK
	printf("fix, now PartCount = %d\n", sunxi_mbr->PartCount);

	for (i = sunxi_mbr->PartCount - 1; i >= 0; i--) {
		/* udisk is the first part */
		/* pos = (i == sunxi_mbr->PartCount-1) ? 0: i+1; */
		pos	= i;
		pgpt_entry = (gpt_entry *)(gpt_entry_start + (pos)*GPT_ENTRY_SIZE);

		sunxi_mbr->array[i].lenhi = ((pgpt_entry->ending_lba - pgpt_entry->starting_lba + 1) >> 32) & 0xffffffff;
		sunxi_mbr->array[i].lenlo = ((pgpt_entry->ending_lba - pgpt_entry->starting_lba + 1) >> 0) & 0xffffffff;
		if (i == sunxi_mbr->PartCount - 1) {
			sunxi_mbr->array[i].lenhi = 0;
			sunxi_mbr->array[i].lenlo = 0;
		};
		printf("%d starting_lba:%llx ending_lba:%llx\n", i, pgpt_entry->starting_lba, pgpt_entry->ending_lba);
		if (i == 0) {
			/* mbr_size = pgpt_entry->ending_lba - 511; */
			sunxi_get_logical_offset_param(storage_type, &logic_offset, &total_sectors);
			mbr_size = pgpt_entry->starting_lba - logic_offset;
			printf("mbr sector size:%x\n", mbr_size);
		}

		if (pgpt_entry->attributes.fields.type_guid_specific == 0x6000)
			sunxi_mbr->array[i].ro = 1;
		else if (pgpt_entry->attributes.fields.type_guid_specific == 0x8000)
			sunxi_mbr->array[i].ro = 0;

		strcpy((char *)sunxi_mbr->array[i].classname, "DISK");
		memset(sunxi_mbr->array[i].name, 0, 16);

		for (j = 0; j < 16; j++) {
			if (pgpt_entry->partition_name[j])
				sunxi_mbr->array[i].name[j] = pgpt_entry->partition_name[j];
			else
				break;
		}
	}

	for (i = 0; i < sunxi_mbr->PartCount; i++) {
		if (i == 0) {
			sunxi_mbr->array[i].addrhi = 0;
			sunxi_mbr->array[i].addrlo = mbr_size;
		} else {
			start_sector = sunxi_mbr->array[i - 1].addrlo;
			start_sector |= (u64)sunxi_mbr->array[i - 1].addrhi << 32;
			start_sector += sunxi_mbr->array[i - 1].lenlo;

			sunxi_mbr->array[i].addrlo = (u32)(start_sector & 0xffffffff);
			sunxi_mbr->array[i].addrhi = (u32)((start_sector >> 32) & 0xffffffff);
		}
		printf("i=%d, addrhi=%d addrlo=0x%x, lenhi=0x%x, lenlo=0x%x, name=%s\n", i, sunxi_mbr->array[i].addrhi,
		       sunxi_mbr->array[i].addrlo, sunxi_mbr->array[i].lenhi, sunxi_mbr->array[i].lenlo, sunxi_mbr->array[i].name);
	}

	crc32_total      = crc32(0, (const unsigned char *)(sunxi_mbr_buf + 4), SUNXI_MBR_SIZE - 4);
	sunxi_mbr->crc32 = crc32_total;

	return 0;
}

#if 0
__weak int sunxi_sprite_erase_flash(void *mbr)
{

	int nodeoffset;
	uint32_t erase_flag = 0;

	nodeoffset = fdt_path_offset (working_fdt,"/soc/platform" );
	fdt_getprop_u32(working_fdt,nodeoffset,"eraseflag",&erase_flag);
	sunxi_flash_erase(erase_flag, mbr);

	return 0;
}
#endif

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

	if(strncmp((const char*)sunxi_mbr->magic, SUNXI_MBR_MAGIC, 8)) {
		debug("%s:not sunxi mbr, can't convert to GPT partition\n", __func__);
		return 0;
	}

	if(crc32(0, (const unsigned char *)(sunxi_mbr_buf + 4), SUNXI_MBR_SIZE - 4) != sunxi_mbr->crc32)
	{
		debug("%s:sunxi mbr crc error, can't convert to GPT partition\n",__func__);
		return 0;
	}

	sunxi_get_logical_offset_param(storage_type, &logic_offset, &total_sectors);

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
	gpt_head->header_size = sizeof(gpt_header);
	gpt_head->header_crc32 = 0x00;
	gpt_head->reserved1 = 0x0;
	gpt_head->my_lba = 0x01;
	gpt_head->alternate_lba = total_sectors - 1;
	gpt_head->first_usable_lba = sunxi_mbr->array[0].addrlo + logic_offset;
	if (storage_type == STORAGE_NOR) {
		/*spinor do not have much space, drop backup gpt to enlarge UDISK*/
		gpt_head->last_usable_lba = total_sectors - 1;
	} else {
		/*room for backup GPT consider "unsable":1 GPT head + 32 GPT entry*/
		gpt_head->last_usable_lba = total_sectors - (1 + 32) - 1;
	}
	memcpy(gpt_head->disk_guid.b,guid,16);
	gpt_head->partition_entry_lba = 2;
	gpt_head->num_partition_entries = sunxi_mbr->PartCount;
	gpt_head->sizeof_partition_entry = GPT_ENTRY_SIZE;
	gpt_head->partition_entry_array_crc32 = 0;
	data_len += 512;

	/* 3. LBA2~LBAn: fill gpt entry */
	gpt_entry_start = (pbuf + data_len);
	for(i=0;i<sunxi_mbr->PartCount;i++) {
		pgpt_entry = (gpt_entry *)(gpt_entry_start + (i)*GPT_ENTRY_SIZE);

		pgpt_entry->partition_type_guid = PARTITION_BASIC_DATA_GUID;

		memcpy(pgpt_entry->unique_partition_guid.b,part_guid,16);
		pgpt_entry->unique_partition_guid.b[15] = part_guid[15]+i;

		pgpt_entry->starting_lba = ((u64)sunxi_mbr->array[i].addrhi<<32) + sunxi_mbr->array[i].addrlo + logic_offset;
		pgpt_entry->ending_lba = pgpt_entry->starting_lba \
			+((u64)sunxi_mbr->array[i].lenhi<<32)  \
			+ sunxi_mbr->array[i].lenlo-1;

		/* UDISK partition */
		if(i == sunxi_mbr->PartCount-1) {
			pgpt_entry->ending_lba = gpt_head->last_usable_lba;
#ifdef CONFIG_SUNXI_UBIFS
			if (((STORAGE_SPI_NAND == get_boot_storage_type_ext()) ||
				(STORAGE_NAND == get_boot_storage_type_ext())) && nand_use_ubi()) {
				/* backup gpt not belong to any volumes */
				pgpt_entry->ending_lba = pgpt_entry->starting_lba + sunxi_mbr->array[i].lenlo - 1;
			}
#endif
		}

		debug("GPT:%-12s: %-12llx  %-12llx\n", sunxi_mbr->array[i].name, pgpt_entry->starting_lba, pgpt_entry->ending_lba);
		if(sunxi_mbr->array[i].ro == 1) {
			pgpt_entry->attributes.fields.type_guid_specific = 0x6000;
		}
		else {
			pgpt_entry->attributes.fields.type_guid_specific = 0x8000;
		}

		if(sunxi_mbr->array[i].keydata == 0x8000){
			pgpt_entry->attributes.fields.keydata = 1;
		}

		//ASCII to unicode
		memset(pgpt_entry->partition_name, 0,PARTNAME_SZ*sizeof(efi_char16_t));
		if (!strncmp((char *)sunxi_mbr->array[i].name, "UDISK", sizeof("UDISK"))) {
			char temp_partition_name[16] = {CONFIG_LAST_PARTITION_NAME};
			for (j = 0; j < strlen((const char *)temp_partition_name); j++) {
				pgpt_entry->partition_name[j] = (efi_char16_t)temp_partition_name[j];
			}
			/* update last partiton name ok set gpt_head->reserved1 = 0x1 */
			gpt_head->reserved1 = 0x1;
		} else {
			for (j = 0; j < strlen((const char *)sunxi_mbr->array[i].name); j++) {
				pgpt_entry->partition_name[j] = (efi_char16_t)sunxi_mbr->array[i].name[j];
			}
		}
		data_len += GPT_ENTRY_SIZE;

	}

	//entry crc
	gpt_head->partition_entry_array_crc32 = crc32(0, (unsigned char const *)gpt_entry_start,
	             (gpt_head->num_partition_entries)*(gpt_head->sizeof_partition_entry));

	debug("gpt_head->partition_entry_array_crc32 = 0x%x\n",gpt_head->partition_entry_array_crc32);
	//gpt crc
	gpt_head->header_crc32 = crc32(0,(const unsigned char *)gpt_head, sizeof(gpt_header));
	debug("gpt_head->header_crc32 = 0x%x\n",gpt_head->header_crc32);

	/* 4. LBA-1: the last sector fill backup gpt header */

	return data_len;
}

int download_standard_gpt(void *sunxi_mbr_buf, size_t buf_size, int storage_type)
{
	typedef int (*FLASH_WIRTE)(uint start_block, uint nblock, void *buffer);
	FLASH_WIRTE flash_write_pt = NULL;
	char  *gpt_buf = NULL;
	int   data_len = 0;
	int   ret = 0;
	gpt_header   *gpt_head;
	int  gpt_buf_len = 8*1024;

#ifdef CONFIG_SUNXI_UBIFS
	if (((STORAGE_SPI_NAND == get_boot_storage_type_ext()) ||
			(STORAGE_NAND == get_boot_storage_type_ext())) && nand_use_ubi()) {
		printf("force mbr\n");
		ret = sunxi_sprite_write(0, buf_size>>9, sunxi_mbr_buf);
		if (!ret) {
			printf("%s:write mbr sectors fail ret = %d\n", __func__, ret);
			sunxi_sprite_write_end();
			sunxi_sprite_flush();
			return -1;
		}
		sunxi_sprite_write_end();
		sunxi_sprite_flush();
		return 0;
	}
#endif
	gpt_buf = memalign(CONFIG_SYS_CACHELINE_SIZE, ALIGN(gpt_buf_len, CONFIG_SYS_CACHELINE_SIZE));
	if(gpt_buf == NULL) {
		debug("malloc for GPT  fail\n");
		return -1;
	}
	memset(gpt_buf, 0x0, gpt_buf_len);

	data_len = sunxi_mbr_convert_to_gpt(sunxi_mbr_buf, gpt_buf, storage_type);
	if(data_len == 0) {
		goto __err_end;
	}

	/*write GPT for u-boot use*/
	ret = sunxi_sprite_write(0, (data_len+511)>>9, gpt_buf);
	if(!ret) {
		debug("%s:write gpt sectors fail\n",__func__);
		goto __err_end;
	}
	/*write GPT for kenerl use if sdmmc*/
	if (STORAGE_EMMC == storage_type || STORAGE_EMMC3 == storage_type
			|| storage_type == STORAGE_EMMC0) {
		ret = sunxi_sprite_phywrite(0, (data_len + 511) >> 9, gpt_buf);
		if (!ret)
			goto __err_end;
	}
	printf("write primary GPT success\n");

	gpt_head = (gpt_header *)(gpt_buf + GPT_HEAD_OFFSET);
	prepare_backup_gpt_header(gpt_head);

	if (STORAGE_NOR == storage_type) {
		printf("spinor: skip backup GPT\n");
	} else {
		if (STORAGE_EMMC == storage_type ||
		    STORAGE_EMMC3 == storage_type ||
		    storage_type == STORAGE_EMMC0)
			flash_write_pt = sunxi_sprite_phywrite;
		else
			flash_write_pt = sunxi_sprite_write;

		/* write back-up gpt PTE */
		ret = flash_write_pt(gpt_head->last_usable_lba + 1,
				     (GPT_BUFF_SIZE - GPT_ENTRY_OFFSET) / 512,
				     gpt_buf + GPT_ENTRY_OFFSET);
		if (!ret) {
			goto __err_end;
		}

		/* write back-up gpt HEAD */
		ret = flash_write_pt(gpt_head->my_lba, 1,
				     gpt_buf + GPT_HEAD_OFFSET);
		if (!ret) {
			goto __err_end;
		}
		printf("write Backup GPT success\n");
	}

	free(gpt_buf);
	return 0;
__err_end:
	if(gpt_buf)
		free(gpt_buf);
	return -1;

}

#define GPT_UPDATE_BUF_SIZE (GPT_BUFF_SIZE - GPT_ENTRY_OFFSET)

#ifdef CONFIG_SUNXI_UPDATE_GPT
int sunxi_update_gpt(void)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (((STORAGE_SPI_NAND == get_boot_storage_type_ext()) ||
			(STORAGE_NAND == get_boot_storage_type_ext())) && nand_use_ubi()) {
		return 0;
	}
#endif
	typedef int (*FLASH_WIRTE)(uint start_block, uint nblock, void *buffer);
	FLASH_WIRTE flash_write_pt = NULL;
	char *buf;
	int   data_len = GPT_UPDATE_BUF_SIZE;
	int   ret = 0;
	int storage_type = get_boot_storage_type();
	int i, j;

	char char8_name[PARTNAME_SZ] = { 0 };
	buf = (char *)memalign(CONFIG_SYS_CACHELINE_SIZE, GPT_UPDATE_BUF_SIZE);
	if (buf == NULL) {
		pr_err("malloc fail\n");
		return -1;
	}
	char  *gpt_buf = buf;
	gpt_header *gpt_head     = (gpt_header *)(buf + GPT_HEAD_OFFSET);
	gpt_entry *entry             = (gpt_entry *)(buf + GPT_ENTRY_OFFSET);
	if ((storage_type == STORAGE_SD) || (storage_type == STORAGE_EMMC)
		|| (storage_type == STORAGE_EMMC0))
		sunxi_flash_phyread(0, GPT_BUFF_SIZE/512, buf);
	else
		sunxi_flash_read(0, GPT_BUFF_SIZE/512, buf);
	if (gpt_head->reserved1 != 1) {
		if (gpt_head->signature == GPT_HEADER_SIGNATURE) {
			u32 calc_crc32   = 0;
			u32 backup_crc32 = 0;

			backup_crc32	   = gpt_head->header_crc32;
			gpt_head->header_crc32 = 0;
			calc_crc32 = crc32(0, (const unsigned char *)gpt_head,
					   sizeof(gpt_header));
			gpt_head->header_crc32 = backup_crc32;
			if (calc_crc32 != backup_crc32) {
				printf("the GPT table is bad\n");
				goto __err_end;
			}
			/* gpt_show_partition_info(buffer); */
		}
		for (i = 0; i < gpt_head->num_partition_entries; i++) {
			for (j = 0; j < PARTNAME_SZ; j++) {
				char8_name[j] = (char)(entry[i].partition_name[j]);
			}

			if ((get_boot_work_mode() == WORK_MODE_BOOT) && (!strncmp(char8_name, "UDISK", sizeof("UDISK")))) {
				char temp_partition_name[16] = {CONFIG_LAST_PARTITION_NAME};
				for (j = 0; j < strlen((const char *)temp_partition_name); j++) {
					entry[i].partition_name[j] = (efi_char16_t)temp_partition_name[j];
				}
				/* update last partiton name ok set gpt_head->reserved1 = 0x1 */
				gpt_head->reserved1 = 0x1;
				for (j = 0; j < PARTNAME_SZ; j++) {
					char8_name[j] = (char)(entry[i].partition_name[j]);
				}
				pr_msg("GPT:%-12s: %-12llx  %-12llx\n", char8_name,
						entry[i].starting_lba, entry[i].ending_lba);
				break;
			}
			pr_msg("GPT:%-12s: %-12llx  %-12llx\n", char8_name,
					entry[i].starting_lba, entry[i].ending_lba);
		}

		gpt_head->partition_entry_array_crc32 = 0;
		gpt_head->partition_entry_array_crc32 = crc32(0, (unsigned char const *)entry,
			(gpt_head->num_partition_entries)*(gpt_head->sizeof_partition_entry));

		//gpt crc32
		gpt_head->header_crc32 = 0;
		gpt_head->header_crc32 = crc32(0, (const unsigned char *)gpt_head, gpt_head->header_size);
		/*write GPT for u-boot use*/
		ret = sunxi_sprite_write(0, (data_len+511)>>9, gpt_buf);
		if (!ret) {
			debug("%s:write gpt sectors fail\n", __func__);
			goto __err_end;
		}
		/*write GPT for kenerl use if sdmmc*/
		if (STORAGE_EMMC == storage_type || STORAGE_EMMC3 == storage_type
				|| storage_type == STORAGE_EMMC0 || storage_type == STORAGE_SD) {
			ret = sunxi_sprite_phywrite(0, (data_len + 511) >> 9, gpt_buf);
			if (!ret)
				goto __err_end;
		}
		pr_msg("write primary GPT success\n");

		prepare_backup_gpt_header(gpt_head);

		if (STORAGE_NOR == storage_type) {
			pr_msg("spinor: skip backup GPT\n");
		} else {
			if (STORAGE_EMMC == storage_type ||
				STORAGE_EMMC3 == storage_type ||
				storage_type == STORAGE_EMMC0 ||
				storage_type == STORAGE_SD)
				flash_write_pt = sunxi_sprite_phywrite;
			else
				flash_write_pt = sunxi_sprite_write;

			ret = flash_write_pt((u32)(gpt_head->last_usable_lba + 1),
					(GPT_BUFF_SIZE - GPT_ENTRY_OFFSET) / 512,
					gpt_buf + GPT_ENTRY_OFFSET);
			if (!ret) {
				goto __err_end;
			}

			/* write back-up gpt HEAD */
			ret = flash_write_pt((u32)gpt_head->my_lba, 1,
				gpt_buf + GPT_HEAD_OFFSET);
			if (!ret) {
				goto __err_end;
			}
			pr_msg("write Backup GPT success\n");
		}
	}
	free(buf);
	return 0;
__err_end:
	free(buf);
	pr_err("update gpt fail\n");
	return -1;
}
#endif

#ifdef CONFIG_OFFLINE_BURN
static int gpt_fix_flash_size(char *gpt_buf, int storage_type)
{
	int           total_sectors;
	u32           logic_offset = 0;
	gpt_header   *gpt_head;
	gpt_entry    *pgpt_entry = NULL;
	char         *gpt_entry_start = NULL;
	int          i = 0;

	sunxi_get_logical_offset_param(storage_type, &logic_offset, &total_sectors);

	/*LBA1: fill primary gpt header */
	gpt_head = (gpt_header *)(gpt_buf + GPT_HEAD_OFFSET);
	gpt_head->alternate_lba = total_sectors - 1;
	gpt_head->first_usable_lba += logic_offset;
	if (storage_type == STORAGE_NOR) {
		/*spinor do not have much space, drop backup gpt to enlarge UDISK*/
		gpt_head->last_usable_lba = total_sectors - 1;
	} else {
		/*room for backup GPT consider "unsable":1 GPT head + 32 GPT entry*/
		gpt_head->last_usable_lba = total_sectors - (1 + 32) - 1;
	}

	/* fix entry logic offset*/
	gpt_entry_start = (gpt_buf + GPT_ENTRY_OFFSET);
	for(i = 0; i < gpt_head->num_partition_entries; i++ ){
		pgpt_entry = (gpt_entry *)(gpt_entry_start + (i)*GPT_ENTRY_SIZE);
		pgpt_entry->starting_lba += logic_offset;
		pgpt_entry->ending_lba += logic_offset;
		/*find last part */
		if(i == gpt_head->num_partition_entries - 1) {
			pgpt_entry->ending_lba = gpt_head->last_usable_lba;
		}
	}

	/* entry crc */
	gpt_head->partition_entry_array_crc32 = 0;
	gpt_head->partition_entry_array_crc32 = crc32(0, (unsigned char const *)gpt_entry_start,
				 (gpt_head->num_partition_entries)*(gpt_head->sizeof_partition_entry));

	printf("gpt_head->partition_entry_array_crc32 = 0x%x\n",gpt_head->partition_entry_array_crc32);
	/*gpt crc */
	gpt_head->header_crc32 = 0;
	gpt_head->header_crc32 = crc32(0,(const unsigned char *)gpt_head, sizeof(gpt_header));
	printf("gpt_head->header_crc32 = 0x%x\n",gpt_head->header_crc32);
	return 0;

}

int sunxi_sprite_download_mbr(void *buffer, uint buffer_size)
{
	typedef int (*FLASH_WIRTE)(uint start_block, uint nblock, void *buffer);
	FLASH_WIRTE flash_write_pt = NULL;
	int ret = 0;
	int storage_type = 0;
	gpt_header *gpt_head  = (gpt_header*)(buffer + GPT_HEAD_OFFSET);

	aastorage_type = get_boot_storage_type();

	if ((storage_type == STORAGE_NAND) && (sunxi_flash_init())) {
		return -1;
	}

	if (buffer_size != GPT_BUFF_SIZE) {
		printf("the GPT size is bad\n");
		return -1;
	}
	if(gpt_head->signature != GPT_HEADER_SIGNATURE) {
		printf("gpt magic error, %llx != %llx\n",gpt_head->signature,
			GPT_HEADER_SIGNATURE);
		return 0;
	}

	gpt_fix_flash_size(buffer, storage_type);

	/*write to logic addr for uboot use*/
	ret = sunxi_flash_write(0, (buffer_size+511)>>9, buffer);
	if(!ret) {
		debug("%s:write gpt sectors fail\n",__func__);
		goto __err_end;
	}
	/*write to phy addr for kernel use*/
	if (STORAGE_EMMC == storage_type || STORAGE_EMMC3 == storage_type
			|| storage_type == STORAGE_EMMC0) {
		ret = sunxi_flash_phywrite(0, buffer_size>>9, buffer);
		if(!ret)
			goto __err_end;
	}
	printf("write primary GPT success\n");
	prepare_backup_gpt_header(gpt_head);

	if (STORAGE_EMMC == storage_type || STORAGE_EMMC3 == storage_type
		storage_type == STORAGE_EMMC0)
		flash_write_pt  = sunxi_flash_phywrite;
	else
		flash_write_pt  = sunxi_flash_write;

	if (STORAGE_NOR == storage_type) {
		printf("spinor: skip backup GPT\n");
	} else {
		/* write back-up gpt PTE */
		ret = flash_write_pt(gpt_head->last_usable_lba + 1,
				     (GPT_BUFF_SIZE - GPT_ENTRY_OFFSET) / 512,
				     buffer + GPT_ENTRY_OFFSET);
		if (!ret) {
			goto __err_end;
		}

		/* write back-up gpt HEAD */
		ret = flash_write_pt(gpt_head->my_lba, 1,
				     buffer + GPT_HEAD_OFFSET);
		if (!ret) {
			goto __err_end;
		}
		printf("write Backup GPT success\n");
	}
	return 0;
__err_end:
	return -1;
}

static int gpt_show_partition_info(char *buf)
{
	int			i, j;

	char   char8_name[PARTNAME_SZ] = {0};
	gpt_header     *gpt_head  = (gpt_header*)(buf + GPT_HEAD_OFFSET);
	gpt_entry      *entry  = (gpt_entry*)(buf + GPT_ENTRY_OFFSET);

	for(i = 0; i < gpt_head->num_partition_entries; i++ ){
		for(j = 0; j < PARTNAME_SZ; j++ ) {
			char8_name[j] = (char)(entry[i].partition_name[j]);
		}
		printf("GPT:%-12s: %-12llx  %-12llx\n", char8_name, entry[i].starting_lba, entry[i].ending_lba);
	}

	return 0;
}


static int is_gpt_valid(void *buffer)
{
	u32 calc_crc32 = 0;
	u32 backup_crc32 = 0;
	gpt_header *gpt_head = (gpt_header *)(buffer + GPT_HEAD_OFFSET);

	if(gpt_head->signature != GPT_HEADER_SIGNATURE)
	{
		printf("gpt magic error, %llx != %llx\n",gpt_head->signature, GPT_HEADER_SIGNATURE);
		return 0;
	}
	gpt_show_partition_info(buffer);
	return 1;

	backup_crc32 = gpt_head->header_crc32;
	gpt_head->header_crc32 = 0;
	calc_crc32 = crc32(0,(const unsigned char *)gpt_head, sizeof(gpt_header));
	gpt_head->header_crc32 = backup_crc32;
	if(calc_crc32 == backup_crc32)
	{
		gpt_show_partition_info(buffer);
		return 1;
	}

	printf("gpt crc error, 0x%x != 0x%x\n",backup_crc32, calc_crc32);
	return 0;

}


int nand_get_mbr(char *buf, uint len)
{
	int		 i, j;

	char	char8_name[PARTNAME_SZ] = {0};
	gpt_header 	*gpt_head   = (gpt_header*)(buf + 512);
	gpt_entry    *entry	    = (gpt_entry*)(buf + 1024);
	int index = 0;

	nand_mbr.PartCount = gpt_head->num_partition_entries + 1;
	nand_mbr.array[index].addr = 0;
	nand_mbr.array[index].len = entry[0].starting_lba;
	nand_mbr.array[index].user_type = 0x8000;
	nand_mbr.array[index].classname[0] = 'm';
	nand_mbr.array[index].classname[1] = 'b';
	nand_mbr.array[index].classname[2] = 'r';
	nand_mbr.array[index].classname[3] = '\0';
	index++;

	for(i = 0; i < gpt_head->num_partition_entries; i++, index++ ){
		for(j = 0; j < 16; j++ ) {
			nand_mbr.array[index].classname[j] = (char)(entry[i].partition_name[j]);
		}
		nand_mbr.array[index].addr = nand_mbr.array[i].addr + nand_mbr.array[i-1].len;
		nand_mbr.array[index].len  = entry[i].ending_lba - entry[i].starting_lba;
		nand_mbr.array[index].user_type = entry[i].attributes.fields.user_type;
		if(i == 0)
			nand_mbr.array[0].user_type = nand_mbr.array[1].user_type;
	}
	/* for DEBUG */
	{
		printf("total part: %d\n", nand_mbr.PartCount);
		for(i=0; i < nand_mbr.PartCount; i++)
		{
			printf("%s %d, %x, %x\n",nand_mbr.array[i].classname,i, nand_mbr.array[i].len, nand_mbr.array[i].user_type);
		}
	}

	return 0;
}



#endif
