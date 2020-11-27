/*
 * (C) Copyright 2016
 *Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <android_image.h>
#include <sys_partition.h>
#include <sunxi_flash.h>
#include <sunxi_board.h>
#include <image.h>


#define  SUNXI_FLASH_READ_FIRST_SIZE      (32 * 1024)

int __attribute__((weak)) nand_uboot_init_force_sprite(int boot_mode)
{
	return 0;
}
int __attribute__((weak)) sunxi_sprite_download_boot0(void *buffer, int production_media)
{
	return 0;
}
int __attribute__((weak)) spinor_download_boot0(uint length, void *buffer)
{
	return 0;
}

extern int nand_uboot_init_force_sprite(int boot_mode);

static inline int check_readall_flag(const char *part_name)
{
	if((!strncmp(part_name, "boot", strlen("boot"))) ||
	    (!strncmp(part_name, "recovery", strlen("recovery"))))
		return 1;

	return 0;
}

static int get_partition_flash_info(const char *part_name, u32 *start_block,u32 *rblock)
{
	if ((NULL == start_block)||(rblock == NULL))
		return -1;

	*start_block = sunxi_partition_get_offset_byname((const char *)part_name);
	if(!*start_block) {
		printf("cant find part named %s\n", (char *)part_name);
		return -1;
	}
	*rblock = sunxi_partition_get_size_byname((const char *)part_name);

	return 0;
}

static int sunxi_flash_read_all(u32 start, ulong buf, const char *part_name)
{
	int ret;
	u32 rbytes = 0, rblock;
	u32 start_block = start;
	void *addr;
	struct andr_img_hdr *fb_hdr;
	image_header_t *uz_hdr;
	struct boot_img_hdr_ex *fb_hdr_ex;

	addr = (void *)buf;
	ret = sunxi_flash_read(start_block, SUNXI_FLASH_READ_FIRST_SIZE/512, addr);
	if(!ret) {
		printf("read all error: flash start block =%x, dest buffer addr=%lx\n", start_block, (ulong)addr);

		return 1;
	}

	fb_hdr = (struct andr_img_hdr *)addr;
	uz_hdr = (image_header_t *)addr;
	fb_hdr_ex = (struct boot_img_hdr_ex *)addr;
	if (!memcmp(fb_hdr->magic, ANDR_BOOT_MAGIC, 8)) {
		rbytes += fb_hdr->page_size;
		rbytes += ALIGN(fb_hdr->kernel_size, fb_hdr->page_size);
		if (fb_hdr->second_size)
			rbytes += ALIGN(fb_hdr->second_size, fb_hdr->page_size);
		if (fb_hdr->ramdisk_size)
			rbytes += ALIGN(fb_hdr->ramdisk_size, fb_hdr->page_size);
		if (fb_hdr->recovery_dtbo_size)
			rbytes += ALIGN(fb_hdr->recovery_dtbo_size, fb_hdr->page_size);
		if (fb_hdr_ex->cert_size)
			rbytes += ALIGN(fb_hdr_ex->cert_size, fb_hdr->page_size);

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

static int sunxi_flash_write_boot0(void *addr, int storage_type)
{
	int ret = -1;
	int boot0_len = 0;

	switch (storage_type) {
		case STORAGE_NAND:
			nand_uboot_init_force_sprite(0); /*for nand para storage*/
			ret = sunxi_sprite_download_boot0((void *)addr, storage_type);
			sunxi_sprite_exit(0);
			break;
		case STORAGE_NOR:
			ret = spinor_download_boot0(boot0_len, (void *)addr);
			break;
		case STORAGE_SD:
		case STORAGE_EMMC:
		case STORAGE_EMMC3:
			ret = sunxi_sprite_download_boot0((void *)addr, storage_type);
			break;
		default:
			printf("Unsupport storage %d\n", storage_type);
			break;
	}

	return ret;
}

int do_sunxi_flash(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;
	ulong addr;
	char *cmd;
	char *part_name;
	int readall_flag = 0;
	int storage_type;
	u32 start_block = 0;
	u32 rblock = 0;

	/* at least four arguments please */
	if ((argc != 4) && (argc != 5))
		goto usage;

	cmd = argv[1];
	part_name = argv[3];
	storage_type = get_boot_storage_type();

	if (strncmp(cmd, "read", strlen("read")) == 0) {
		addr = (ulong)simple_strtoul(argv[2], NULL, 16);
		readall_flag = check_readall_flag(part_name);

		if (!strncmp("uboot", part_name, 5))
			return read_boot_package(storage_type, (void *)addr);
		else if (!strncmp("boot0", part_name, 5))
			return sunxi_flash_upload_boot0((void *)addr, sunxi_flash_get_boot0_size());

		if (argc == 4) {	/* read size: indecated on partemeter 1 */
			if (get_partition_flash_info(part_name, &start_block,&rblock))
				goto usage;
		} else { /* read size: partemeter 2 */
			start_block = (u32)simple_strtoul(argv[3], NULL, 16)/512;
			rblock = (u32)simple_strtoul(argv[4], NULL, 16)/512;
		}

		if (readall_flag) {
			pr_msg("read partition: boot or recovery\n");

			return sunxi_flash_read_all(start_block, addr, (const char *)part_name);
		}

#ifdef DEBUG
		printf("part name   = %s\n", part_name);
		printf("addr        = %ld\n", addr);
		printf("start block = 0x%x\n", start_block);
		printf("nblock 		= 0x%x\n", rblock);
#endif
		ret = sunxi_flash_read(start_block, rblock, (void *)addr);

		pr_msg("sunxi flash read :offset %x, %d bytes %s\n", start_block<<9, rblock<<9,
		            ret ? "OK" : "ERROR");

		return ret == 0 ? 1 : 0;
	} else if(strncmp(cmd, "log_read", strlen("log_read")) == 0) {
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
	} else if(strncmp(cmd, "phy_read", strlen("phy_read")) == 0) {
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
	} else if (strncmp(cmd, "write", 4) == 0) {
		addr = (ulong)simple_strtoul(argv[2], NULL, 16);

		if (!strncmp("uboot", part_name, 5))
			return sunxi_sprite_download_uboot((void *)addr, storage_type, 0);
		else if (!strncmp("boot0", part_name, 5))
			return sunxi_flash_write_boot0((void *)addr, storage_type);

		/* write size: indecated on partemeter 1 */
		if (argc == 4) {
			if (get_partition_flash_info(part_name, &start_block,&rblock))
				goto usage;
		} else {
			/* write size: partemeter 2 */
			start_block = (u32)simple_strtoul(argv[3], NULL, 16)/512;
			rblock = (u32)simple_strtoul(argv[4], NULL, 16)/512;
		}
		ret = sunxi_flash_write(start_block, rblock, (void *)addr);

		pr_msg("sunxi flash write :offset %x, %d bytes %s\n", start_block<<9, rblock<<9,
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
