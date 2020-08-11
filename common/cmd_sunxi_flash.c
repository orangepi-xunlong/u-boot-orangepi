/*
 * Driver for NAND support, Rick Bronson
 * borrowed heavily from:
 * (c) 1999 Machine Vision Holdings, Inc.
 * (c) 1999, 2000 David Woodhouse <dwmw2@infradead.org>
 *
 * Ported 'dynenv' to 'nand env.oob' command
 * (C) 2010 Nanometrics, Inc.
 * 'dynenv' -- Dynamic environment offset in NAND OOB
 * (C) Copyright 2006-2007 OpenMoko, Inc.
 * Added 16-bit nand support
 * (C) 2004 Texas Instruments
 *
 * Copyright 2010 Freescale Semiconductor
 * The portions of this file whose copyright is held by Freescale and which
 * are not considered a derived work of GPL v2-only code may be distributed
 * and/or modified under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 */

#include <common.h>
#include <command.h>
#include <android_image.h>
#include <sys_partition.h>
#include <sunxi_flash.h>
#include <private_toc.h>
#include <private_boot0.h>
#include <private_uboot.h>
#include <sunxi_board.h>
#include <image.h>


#define  SUNXI_FLASH_READ_FIRST_SIZE      (32 * 1024)
extern int nand_init_for_sprite(int workmode);
extern int nand_uboot_init_force_sprite(int boot_mode);

static int sunxi_flash_read_all(u32 start, ulong buf, const char *part_name)
{
	int ret;
	u32 rbytes, rblock;
	u32 start_block = start;
	void *addr;
	struct andr_img_hdr *fb_hdr;
	image_header_t *uz_hdr;

	addr = (void *)buf;
	ret = sunxi_flash_read(start_block, SUNXI_FLASH_READ_FIRST_SIZE/512, addr);
	if(!ret)
	{
		printf("read all error: flash start block =%x, dest buffer addr=%lx\n", start_block, (ulong)addr);

		return 1;
	}

	fb_hdr = (struct andr_img_hdr *)addr;
	uz_hdr = (image_header_t *)addr;

	if (!memcmp(fb_hdr->magic, ANDR_BOOT_MAGIC, 8)) {
		rbytes = fb_hdr->kernel_size + fb_hdr->ramdisk_size + fb_hdr->second_size + 1024 * 1024 + 511;

	} else if (image_check_magic(uz_hdr)) {
		rbytes = image_get_data_size(uz_hdr) + image_get_header_size() + 512;

	} else {
		printf("boota: bad boot image magic, maybe not a boot.img?\n");
		printf("try to read partition(%s) all\n",part_name);
		rbytes = sunxi_partition_get_size_byname(part_name) * 512;
	}
	rblock = rbytes/512 - SUNXI_FLASH_READ_FIRST_SIZE/512;
	debug("rblock=%d, start=%d\n", rblock, start_block);
	start_block += SUNXI_FLASH_READ_FIRST_SIZE/512;
	addr = (void *)(buf + SUNXI_FLASH_READ_FIRST_SIZE);

	ret = sunxi_flash_read(start_block, rblock, addr);

	pr_msg("sunxi flash read :offset %x, %d bytes %s\n", start<<9, rbytes,
	            ret ? "OK" : "ERROR");

	return ret == 0 ? 1 : 0;

}

int do_sunxi_flash(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;
	ulong addr;
	char *cmd;
	char *part_name;
	int storage_type = get_boot_storage_type();

	/* at least four arguments please */
	if ((argc != 4) && (argc != 5))
		goto usage;

	cmd = argv[1];
	part_name = argv[3];

/*
************************************************
*************  read only   *********************
************************************************
*/

	if (strncmp(cmd, "read", 4) == 0)
	{
		u32 start_block;
		u32 rblock;
		int readall_flag = 0;

		addr = (ulong)simple_strtoul(argv[2], NULL, 16);

		if((!strncmp(part_name, "boot", 4)) || (!strncmp(part_name, "recovery", 8)))
		{
			readall_flag = 1;
		}

		if(argc == 4)
		{
			if(!strncmp("uboot", part_name, 5)){
				read_boot_package(storage_type, (void *)addr); //if(gd->bootfile_mode  == SUNXI_BOOT_FILE_NORMAL )
			    return 0;
			}else if (!strncmp("boot0", part_name, 5)){
			    sunxi_flash_upload_boot0((void *)addr, sunxi_flash_get_boot0_size()); //sunxi_flash_upload_boot0();
			    return 0;
			}else{
				start_block = sunxi_partition_get_offset_byname((const char *)part_name);
				if(!start_block)
				{
					printf("cant find part named %s\n", (char *)part_name);
					goto usage;
				}
				if(readall_flag)
				{
					pr_msg("read partition: boot or recovery\n");

					return sunxi_flash_read_all(start_block, addr, (const char *)part_name);
				}
				rblock = sunxi_partition_get_size_byname((const char *)part_name);
			}
		}
		else
		{
			start_block = (u32)simple_strtoul(argv[3], NULL, 16)/512;
			rblock = (u32)simple_strtoul(argv[4], NULL, 16)/512;
		}
#ifdef DEBUG
		printf("part name   = %s\n", part_name);
		printf("addr        = %ld\n", addr);
		printf("start block = 0x%x\n", start_block);
		printf("     nblock = 0x%x\n", rblock);
#endif
		ret = sunxi_flash_read(start_block, rblock, (void *)addr);

		pr_msg("sunxi flash read :offset %x, %d bytes %s\n", start_block<<9, rblock<<9,
		       ret ? "OK" : "ERROR");

		return ret == 0 ? 1 : 0;
	}
	else if (strncmp(cmd, "write", 4) == 0)
	{
		u32 start_block;
		u32 rblock;

		addr = (ulong)simple_strtoul(argv[2], NULL, 16);

		if(argc == 4)
		{
			if(!strncmp("uboot", part_name, 5)){
			    switch (storage_type)
			    {
			        case STORAGE_NOR:
#ifdef CONFIG_SUNXI_SPINOR
			            printf("write uboot spinor\n");
			            int uboot_len = spinor_fetch_uboot_length((void *)addr);
			            if (uboot_len > 0)
			            {
			                spinor_flash_download_uboot(uboot_len, (void *)addr);
			                sunxi_flash_flush();
			            }else{
                            printf("spinor uboot magic is error, please check download data!\n");
			            }
#endif
			        break;
			        default:;
#if defined(CONFIG_SUNXI_MODULE_NAND) || defined(CONFIG_SUNXI_MODULE_SDMMC)
			            printf("write uboot nand emmc\n");
				        sunxi_sprite_download_uboot((void *)addr, storage_type, 0); //read_boot_package()
#endif
			    }
				return 0;
			}else if (!strncmp("boot0", part_name, 5)){
			switch (storage_type)
			{
			    case STORAGE_NOR:
#ifdef CONFIG_SUNXI_SPINOR
			        printf("write boot0 spinor\n");
			        int boot0_len = sunxi_flash_get_boot0_size();
			        if (boot0_len > 0)
			        {
			            spinor_flash_download_boot0(boot0_len, (void *)addr);
			            sunxi_flash_flush();
			        }else{
                        printf("spinot boot0 magic is error, please check download data!\n");
			        }
#endif
			    break;
			    case STORAGE_NAND:
#if defined(CONFIG_SUNXI_MODULE_NAND)
			        printf("write boot0 nand\n");
			        nand_init_for_sprite(uboot_spare_head.boot_data.work_mode);
			        nand_uboot_init_force_sprite(0);// for nand para storage

			        sunxi_sprite_download_boot0((void *)addr, storage_type); //sunxi_flash_upload_boot0();

			        sunxi_sprite_exit(0);// for nand para storage
#endif
			    break;
			    default:;
#if defined(CONFIG_SUNXI_MODULE_SDMMC)
			        printf("write boot0 emmc\n");
			        sunxi_sprite_download_boot0((void *)addr, storage_type); //for emmc
#endif
			}
			    return 0;
			}else{
			    start_block = sunxi_partition_get_offset_byname((const char *)part_name);
				if(!start_block)
				{
					printf("cant find part named %s\n", (char *)part_name);
					goto usage;
				}
				rblock = sunxi_partition_get_size_byname((const char *)part_name);
			}
		}
		else
		{
			start_block = (u32)simple_strtoul(argv[3], NULL, 16)/512;
			rblock = (u32)simple_strtoul(argv[4], NULL, 16)/512;
		}
#ifdef DEBUG
		printf("part name   = %s\n", part_name);
		printf("addr        = %ld\n", addr);
		printf("start block = 0x%x\n", start_block);
		printf("     nblock = 0x%x\n", rblock);
#endif
		ret = sunxi_flash_write(start_block, rblock, (void *)addr);
#ifdef CONFIG_SUNXI_SPINOR
		sunxi_flash_flush();
#endif

		pr_msg("sunxi flash read :offset %x, %d bytes %s\n", start_block<<9, rblock<<9,
		            ret ? "OK" : "ERROR");

		return ret == 0 ? 1 : 0;
	}
	else if(strncmp(cmd, "log_read", strlen("log_read")) == 0)
	{
		u32 start_block;
		u32 rblock;

		printf("read logical\n");

		addr = (ulong)simple_strtoul(argv[2], NULL, 16);
		start_block = (ulong)simple_strtoul(argv[3], NULL, 16);
		rblock = (ulong)simple_strtoul(argv[4], NULL, 16);

		ret = sunxi_flash_read(start_block, rblock, (void *)addr);

		pr_msg("sunxi flash log_read :offset %x, %d sectors %s\n", start_block, rblock,
		            ret ? "OK" : "ERROR");

		return ret == 0 ? 1 : 0;
	}
	else if(strncmp(cmd, "phy_read", strlen("phy_read")) == 0)
	{
		u32 start_block;
		u32 rblock;

		printf("read physical\n");

		addr = (ulong)simple_strtoul(argv[2], NULL, 16);
		start_block = (ulong)simple_strtoul(argv[3], NULL, 16);
		rblock = (ulong)simple_strtoul(argv[4], NULL, 16);

		ret = sunxi_flash_phyread(start_block, rblock, (void *)addr);

		pr_msg("sunxi flash phy_read :offset %x, %d sectors %s\n", start_block, rblock,
		            ret ? "OK" : "ERROR");

		return ret == 0 ? 1 : 0;
	}

usage:
	return cmd_usage(cmdtp);
}

U_BOOT_CMD(
	sunxi_flash, CONFIG_SYS_MAXARGS, 1, do_sunxi_flash,
	"sunxi_flash sub-system",
	"read command parmeters : \n"
	"parmeters 0 : addr to load(hex only)\n"
	"parmeters 1 : the name of the part to be load or the flash offset\n"
	"[parmeters 2] : the number of bytes to be load(hex only)\n"
	"if [parmeters 2] not exist, the number of bytes to be load "
	"is the size of the part indecated on partemeter 1\n"
	"\nwrite command parmeters : \n"
	"parmeters 0 : addr to save(hex only)\n"
	"parmeters 1 : the name of the part to be write or the flash offset\n"
	"[parmeters 2] : the number of bytes to be write(hex only)\n"
	"if [parmeters 2] not exist, the number of bytes to be write "
	"is the size of the part indecated on partemeter 1"
);
