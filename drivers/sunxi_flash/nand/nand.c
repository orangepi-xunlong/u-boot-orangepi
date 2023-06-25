/*
 *  * Copyright 2000-2009
 *   * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *    *
 *     * SPDX-License-Identifier:	GPL-2.0+
 *     */

#include <common.h>
#include "../flash_interface.h"
#include <spare_head.h>
#include <sunxi_nand.h>
#include <sunxi_board.h>

static int
sunxi_flash_nand_read(uint start_block, uint nblock, void *buffer)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (nand_use_ubi())
		return ubi_nand_read(start_block, nblock, buffer);
#endif
	return nand_uboot_read(start_block, nblock, buffer);
}

static int
sunxi_flash_nand_write(uint start_block, uint nblock, void *buffer)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (nand_use_ubi())
		return ubi_nand_write(start_block, nblock, buffer);
#endif
	return nand_uboot_write(start_block, nblock, buffer);
}

static int
sunxi_flash_nand_erase(int erase, void *mbr_buffer)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (nand_use_ubi())
		return ubi_nand_erase(erase);
#endif
	return nand_uboot_erase(erase);
}

static uint
sunxi_flash_nand_size(void)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (nand_use_ubi())
		return ubi_nand_size();
#endif
	return nand_uboot_get_flash_size();
}

static int
sunxi_flash_nand_probe(void)
{

#ifdef CONFIG_SUNXI_UBIFS
	if (nand_use_ubi())
		/* the storage type will be set inside */
		return ubi_nand_probe_uboot();
#endif
	if (nand_uboot_probe()) {
		pr_err("Failed to probe nand flash\n");
		return -1;
	}
	set_boot_storage_type(STORAGE_NAND);
	return 0;
}

static int
sunxi_flash_nand_init(int boot_mode, int res)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (nand_use_ubi())
		return ubi_nand_init_uboot(boot_mode);
#endif
	return nand_uboot_init(boot_mode);
}

static int
sunxi_flash_nand_exit(int force)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (nand_use_ubi())
		return ubi_nand_exit_uboot(force);
#endif
	return nand_uboot_exit(force);
}

static int
sunxi_flash_nand_flush(void)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (nand_use_ubi())
		return ubi_nand_flush();
#endif
	return nand_uboot_flush();
}

static int
sunxi_flash_nand_force_erase(void)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (nand_use_ubi())
		return ubi_nand_force_erase();
#endif
	return NAND_Uboot_Force_Erase();
}

static int
sunxi_flash_nand_download_spl(unsigned char *buf, int len, unsigned int ext)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (nand_use_ubi())
		return ubi_nand_download_boot0(len, buf);
#endif
	return nand_download_boot0(len, buf);
}

static int
sunxi_flash_nand_download_toc(unsigned char *buf, int len,  unsigned int ext)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (nand_use_ubi())
		return ubi_nand_download_uboot(len, buf);
#endif
	return nand_download_uboot(len, buf);
}

static int
sunxi_flash_nand_write_end(void)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (nand_use_ubi())
		return ubi_nand_write_end();
#endif
	return 0;
}

static int
sunxi_flash_nand_secstorage_read(int item, unsigned char *buf, unsigned int len)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (nand_use_ubi())
		return ubi_nand_secure_storage_read(item, buf, len);
#endif
	return nand_secure_storage_read(item, buf, len);
}

static int
sunxi_flash_nand_secstorage_write(int item, unsigned char *buf, unsigned int len)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (nand_use_ubi())
		return ubi_nand_secure_storage_write(item, buf, len);
#endif
	return nand_secure_storage_write(item, buf, len);
}

#ifdef CONFIG_SUNXI_FAST_BURN_KEY
static int
sunxi_flash_nand_secstorage_flush(void)
{
	return nand_secure_storage_flush();
}

static int
sunxi_flash_nand_secstorage_fast_write(int item, unsigned char *buf, unsigned int len)
{
	return nand_secure_storage_fast_write(item, buf, len);
}
#endif

sunxi_flash_desc sunxi_nand_desc =
{
	.probe = sunxi_flash_nand_probe,
	.init = sunxi_flash_nand_init,
	.exit = sunxi_flash_nand_exit,
	.read = sunxi_flash_nand_read,
	.write = sunxi_flash_nand_write,
	.erase = sunxi_flash_nand_erase,
	.force_erase = sunxi_flash_nand_force_erase,
	.flush = sunxi_flash_nand_flush,
	.size = sunxi_flash_nand_size,
	.secstorage_read = sunxi_flash_nand_secstorage_read,
	.secstorage_write = sunxi_flash_nand_secstorage_write,
#ifdef CONFIG_SUNXI_FAST_BURN_KEY
	.secstorage_fast_write = sunxi_flash_nand_secstorage_fast_write,
	.secstorage_flush = sunxi_flash_nand_secstorage_flush,
#endif
	.download_spl = sunxi_flash_nand_download_spl,
	.download_toc = sunxi_flash_nand_download_toc,
	.write_end = sunxi_flash_nand_write_end,
};

