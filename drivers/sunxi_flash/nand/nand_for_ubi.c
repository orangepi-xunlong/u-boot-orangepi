/*
 * (C) Copyright 2007-2017
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * zhongguizhao <zhongguizhao@allwinnertech.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <malloc.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/aw-spinand.h>
#include <fdt_support.h>
#include <sunxi_board.h>
#include <sys_config.h>

static int use_ubi = -1;
static int init_end;

bool nand_use_ubi(void)
{
	int ret, val;

	if (use_ubi != -1)
		return use_ubi;

	ret = script_parser_fetch("/soc/target", "nand_use_ubi", &val, 1);
	if (ret < 0) {
		pr_debug("get nand_use_bui failed, default nand with nftl\n");
		return false;
	}

	use_ubi = val == 1 ? true : false;
	return  use_ubi;
}

int ubi_nand_init_uboot(int boot_mode)
{
	int ret = 0;

	if (init_end)
		return 0;

	if (support_spinand())
		ret = spinand_mtd_init();
	else
		ret = -ENODEV;

	init_end = ret == 0 ? true : false;
	return ret;
}

int ubi_nand_exit_uboot(int force)
{
	int ret = 0;

	if (init_end)
		return 0;

	if (support_spinand())
		spinand_mtd_exit();
	else
		ret = -ENODEV;

	return ret;
}

int ubi_nand_probe_uboot(void)
{
	int ret = 0;

	if (init_end)
		return 0;

	if (support_spinand()) {
		/* the same as @ubi_nand_init_uboot */
		ret = spinand_mtd_init();
		if (!ret)
			set_boot_storage_type(STORAGE_SPI_NAND);
	} else {
		ret = -ENODEV;
	}

	init_end = ret == 0 ? true : false;
	return ret;
}

int ubi_nand_attach_mtd(void)
{
	int ret = 0;

	if (!init_end)
		return -EBUSY;

	if (support_spinand()) {
		/* the same as @ubi_nand_init_uboot */
		ret = spinand_mtd_attach_mtd();
		if (!ret)
			set_boot_storage_type(STORAGE_SPI_NAND);
	} else {
		ret = -ENODEV;
	}
	return 0;
}

unsigned int ubi_nand_size(void)
{
	if (!init_end)
		return -EINVAL;

	if (support_spinand())
		return spinand_mtd_size() >> 9;
	else
		return -EINVAL;
}

int ubi_nand_erase(int flag)
{
	if (!init_end)
		return -EBUSY;

	if (support_spinand())
		return spinand_mtd_erase(flag);
	else
		return -EINVAL;
}

int ubi_nand_force_erase(void)
{
	if (!init_end)
		return -EBUSY;

	if (support_spinand())
		return spinand_mtd_force_erase();
	else
		return -EINVAL;
}

int ubi_nand_download_uboot(unsigned int len, void *buf)
{
	if (!init_end)
		return -EBUSY;

	if (support_spinand())
		return spinand_mtd_download_uboot(len, buf);
	else
		return -EINVAL;
}

int ubi_nand_download_boot0(unsigned int len, void *buf)
{
	if (!init_end)
		return -EBUSY;

	if (support_spinand())
		return spinand_mtd_download_boot0(len, buf);
	else
		return -EINVAL;
}

int ubi_nand_flush(void)
{
	if (!init_end)
		return -EBUSY;

	if (support_spinand())
		return spinand_mtd_flush();
	else
		return -EINVAL;
}

int ubi_nand_write_end(void)
{
	if (!init_end)
		return -EBUSY;

	if (support_spinand())
		return spinand_mtd_write_end();
	else
		return -EINVAL;
}

int ubi_nand_write(unsigned int start, unsigned int sectors, void *buf)
{
	if (!init_end)
		return -EBUSY;

	if (support_spinand())
		return spinand_mtd_write(start, sectors, buf);
	else
		return -EINVAL;
}

int ubi_nand_read(unsigned int start, unsigned int sectors, void *buf)
{
	if (!init_end)
		return -EBUSY;

	if (support_spinand())
		return spinand_mtd_read(start, sectors, buf);
	else
		return -EINVAL;
}

int ubi_nand_get_flash_info(void *info, unsigned int len)
{
	if (!init_end)
		return -EBUSY;

	if (support_spinand())
		return spinand_mtd_get_flash_info(info, len);
	else
		return -EINVAL;
}

int ubi_nand_update_ubi_env(void)
{
	if (!init_end)
		return -EBUSY;

	if (support_spinand())
		return spinand_mtd_update_ubi_env();
	else
		return -EINVAL;
}

int ubi_nand_secure_storage_read(int item, void *buf, unsigned int len)
{
	if (!init_end)
		return -EBUSY;

	if (support_spinand())
		return spinand_mtd_secure_storage_read(item, buf, len);
	else
		return -EINVAL;
}

int ubi_nand_secure_storage_write(int item, void *buf, unsigned int len)
{
	if (!init_end)
		return -EBUSY;

	if (support_spinand())
		return spinand_mtd_secure_storage_write(item, buf, len);
	else
		return -EINVAL;
}
