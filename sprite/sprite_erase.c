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
#include <sunxi_mbr.h>
#include <sunxi_nand.h>
#include <sunxi_flash.h>
#include <sunxi_board.h>
#include <malloc.h>
#include <sprite_verify.h>
#include <part_efi.h>
#include <fdt_support.h>
#include <securestorage.h>
#include "sprite_privatedata.h"
#include <nand.h>
#include <sprite_download.h>

DECLARE_GLOBAL_DATA_PTR;

int sunxi_sprite_erase_flash(void *img_mbr_buffer)
{

	int nodeoffset;
	uint32_t need_erase_flag = 0;
	int mbr_num = SUNXI_MBR_COPY_NUM;
	char buf[SUNXI_MBR_SIZE * SUNXI_MBR_COPY_NUM];

	/* nand will erase boot block at this stage */
	if (sunxi_sprite_erase(0, img_mbr_buffer) > 0) {
		printf("flash already erased\n");
		return 0;
	}

	nodeoffset = fdt_path_offset (working_fdt,"/soc/platform" );
	fdt_getprop_u32(working_fdt, nodeoffset, "eraseflag", &need_erase_flag);

	printf("need erase flash: %d\n", need_erase_flag);

	if (need_erase_flag == 0x12) {
#ifdef CONFIG_SUNXI_UBIFS
		if ((get_boot_storage_type() == STORAGE_NAND) && nand_use_ubi()) {
			ubi_nand_probe_uboot();
			ubi_nand_attach_mtd();
		}
#endif
		sunxi_flash_force_erase();
#if CONFIG_SUNXI_SECURE_STORAGE
		sunxi_secure_storage_erase_all();
#endif
		return 0;
	}

	if (need_erase_flag == 0x11) {
#ifdef CONFIG_SUNXI_UBIFS
		if ((get_boot_storage_type() == STORAGE_NAND) && nand_use_ubi()) {
			ubi_nand_probe_uboot();
			ubi_nand_attach_mtd();
		}
#endif
		printf("force erase flash\n");
		sunxi_flash_erase(1, img_mbr_buffer);
		return 0;
	}

	if (get_boot_storage_type() == STORAGE_NAND ? sunxi_sprite_init(1) : 0) {
		debug("sunxi sprite pre init fail, we have to erase it\n");
		goto __ERROR_END;
	}

	if (get_boot_storage_type() == STORAGE_NOR)
		mbr_num = 1;

#ifdef CONFIG_SUNXI_UBIFS
	if ((get_boot_storage_type() == STORAGE_NAND) && nand_use_ubi()) {
		ubi_nand_probe_uboot();
		ubi_nand_attach_mtd();
	}
#endif

	if (!sunxi_sprite_probe_prvt(img_mbr_buffer)) {
		printf("no part need to protect user data\n");
		sunxi_sprite_erase(need_erase_flag, img_mbr_buffer);
		get_boot_storage_type() == STORAGE_NAND ? sunxi_sprite_exit(1) : 0;
		return 0;
	}

	if (sunxi_sprite_read_mbr(buf,  mbr_num)) {
		printf("read local mbr on flash failed\n");
		goto __ERROR_END;
	}

	if (sunxi_sprite_verify_mbr(buf)) {
		printf("the mbr on flash is bad\n");
		goto __ERROR_END;
	}
	printf("begin to store data\n");
	if (sunxi_sprite_store_part_data(buf) < 0) {
		get_boot_storage_type() == STORAGE_NAND ? sunxi_sprite_exit(1) : 0;
		return -1;
	}
	if (get_boot_storage_type() == STORAGE_NAND)
		sunxi_sprite_exit(1);

	printf("begin to erase\n");
	sunxi_sprite_erase(need_erase_flag, img_mbr_buffer);
	printf("finish erase\n");
	get_boot_storage_type() == STORAGE_NAND ? sunxi_sprite_init(0) : 0;
	printf("rewrite\n");
	sunxi_sprite_download_mbr(img_mbr_buffer, ALIGN(sizeof(sunxi_mbr_t) * mbr_num, CONFIG_SYS_CACHELINE_SIZE));
	sunxi_flash_write_end();
	sunxi_flash_flush();

	memset(buf, 0, SUNXI_MBR_SIZE * SUNXI_MBR_COPY_NUM);
	if (sunxi_sprite_read_mbr(buf,  mbr_num)) {
		printf("read local mbr on flash failed\n");
		goto __ERROR_END;
	}

	if (sunxi_sprite_restore_part_data(buf)) {
		sunxi_flash_write_end();
		sunxi_flash_flush();
		get_boot_storage_type() == STORAGE_NAND ? sunxi_sprite_exit(0) : 0;
		return -1;
	}
	sunxi_flash_write_end();
	sunxi_flash_flush();
	printf("flash exit\n");
	get_boot_storage_type() == STORAGE_NAND ? sunxi_sprite_exit(0) : 0;

	return 0;
__ERROR_END:
	get_boot_storage_type() == STORAGE_NAND ? sunxi_sprite_exit(1) : 0;
	sunxi_sprite_erase(need_erase_flag, img_mbr_buffer);
	return 0;
}

int sunxi_sprite_force_erase_key(void)
{
	char buf[SUNXI_MBR_SIZE * SUNXI_MBR_COPY_NUM];
	int mbr_num = SUNXI_MBR_COPY_NUM;
	int ret     = 0;

	if (get_boot_storage_type() == STORAGE_NAND ? sunxi_sprite_init(1) : 0) {
		printf("sunxi sprite pre init fail\n");
		return -1;
	}

	if (get_boot_storage_type() == STORAGE_NOR)
		mbr_num = 1;

	if (sunxi_sprite_read_mbr(buf, mbr_num)) {
		printf("read local mbr on flash failed\n");
		get_boot_storage_type() == STORAGE_NAND ? sunxi_sprite_exit(1) : 0;
		return -1;
	}

	if (sunxi_sprite_verify_mbr(buf)) {
		printf("the mbr on flash is bad\n");
		get_boot_storage_type() == STORAGE_NAND ? sunxi_sprite_exit(1) : 0;
		return -1;
	}

	ret = sunxi_sprite_erase_private_key(buf);
	if (ret) {
		if (ret == -2) {
			/*
			 * private not exist, no private key
			 * assume erase done, not failed
			 */
		} else {
			printf("erase private key fail\n");
			return -1;
		}
	}

#ifdef CONFIG_SUNXI_SECURE_STORAGE
	if (sunxi_secure_storage_init())
		return -1;
	if (sunxi_secure_storage_erase_all() == -1)
		return -1;
#endif
	printf("erase key success\n");
	return 0;
}
