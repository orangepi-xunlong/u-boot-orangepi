
#include <common.h>
#include <sys_config.h>
#include <sys_partition.h>
#include <boot_type.h>
#include <sunxi_board.h>
#include <sunxi_nand.h>
#include "flash_interface.h"

#ifdef CONFIG_SUNXI_UBIFS
int sunxi_mtd_ubi_mode;
u8 ubifs_sb_packed[4096] = {0};
static int chk_ubifs_sb_status;
#endif

static int
sunxi_flash_nand_read(uint start_block, uint nblock, void *buffer)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (sunxi_get_mtd_ubi_mode_status())
		return sunxi_nand_mtd_ubi_read(start_block, nblock, buffer);
	else
#endif
		return nand_uboot_read(start_block, nblock, buffer);
}

static int
sunxi_flash_nand_write(uint start_block, uint nblock, void *buffer)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (sunxi_get_mtd_ubi_mode_status())
		if (!chk_ubifs_sb_status &&
			sunxi_chk_ubifs_sb(ubifs_sb_packed)) {
			printf("Err: ubifs super blk check fail.");
			printf("please check ubifs pack config.\n");
			return -1;
		} else {
			chk_ubifs_sb_status = 1;
			return sunxi_nand_mtd_ubi_write(start_block,
					nblock, buffer);
		}
	else
#endif
		return nand_uboot_write(start_block, nblock, buffer);
}

static int
sunxi_flash_nand_erase(int erase, void *mbr_buffer)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (sunxi_get_mtd_ubi_mode_status())
		return sunxi_nand_uboot_erase(erase);
	else
#endif
		return nand_uboot_erase(erase);
}

static uint
sunxi_flash_nand_size(void)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (sunxi_get_mtd_ubi_mode_status())
		return sunxi_nand_mtd_ubi_cap();
	else
#endif
		return nand_uboot_get_flash_size();
}

static int
sunxi_flash_nand_init(int stage)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (sunxi_get_mtd_ubi_mode_status())
		return sunxi_nand_uboot_init(stage);
	else
#endif
		return nand_uboot_init(stage);
}

static int
sunxi_flash_nand_exit(int force)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (sunxi_get_mtd_ubi_mode_status())
		return sunxi_nand_uboot_exit(force);
	else
#endif
		return nand_uboot_exit(force);
}

static int
sunxi_flash_nand_flush(void)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (sunxi_get_mtd_ubi_mode_status())
		return 0;
#endif
	return nand_uboot_flush();
}

static int
sunxi_flash_nand_force_erase(int erase, void *mbr_buffer)
{
#ifdef CONFIG_SUNXI_UBIFS
	if (sunxi_get_mtd_ubi_mode_status())
		return sunxi_nand_uboot_force_erase();
	else
#endif
		return NAND_Uboot_Force_Erase();
}


int  nand_init_for_boot(int workmode)
{
	int bootmode = 0;
	int ret;

	tick_printf("NAND: ");
	bootmode = workmode == WORK_MODE_SPRITE_RECOVERY ? 0 : 1;

#ifdef CONFIG_SUNXI_UBIFS
	sunxi_open_ubifs_interface();
	if (sunxi_get_mtd_ubi_mode_status())
		ret = sunxi_nand_uboot_init(bootmode);
	else
#endif
		ret = nand_uboot_init(bootmode);

	if (ret) {
		tick_printf("nand init fail\n");
		return -1;
	}
	sunxi_flash_read_pt  = sunxi_flash_nand_read;
	sunxi_flash_write_pt = sunxi_flash_nand_write;
	sunxi_flash_size_pt  = sunxi_flash_nand_size;
	sunxi_flash_exit_pt  = sunxi_flash_nand_exit;
	sunxi_flash_flush_pt = sunxi_flash_nand_flush;

	sunxi_secstorage_read_pt  = nand_secure_storage_read;
	sunxi_secstorage_write_pt = nand_secure_storage_write;

	sunxi_sprite_read_pt  = sunxi_flash_read_pt;
	sunxi_sprite_write_pt = sunxi_flash_write_pt;

	return 0;
}

int nand_init_for_sprite(int workmode)
{
	int ret;

#ifdef CONFIG_SUNXI_UBIFS
	sunxi_open_ubifs_interface();
	if (sunxi_get_mtd_ubi_mode_status())
		ret = sunxi_nand_uboot_probe();
	else
#endif
		ret = nand_uboot_probe();

	if (ret)
		return -1;

	printf("nand found\n");
	sunxi_sprite_init_pt  = sunxi_flash_nand_init;
	sunxi_sprite_exit_pt  = sunxi_flash_nand_exit;
	sunxi_sprite_read_pt  = sunxi_flash_nand_read;
	sunxi_sprite_write_pt = sunxi_flash_nand_write;
	sunxi_sprite_erase_pt = sunxi_flash_nand_erase;
	sunxi_sprite_size_pt  = sunxi_flash_nand_size;
	sunxi_sprite_flush_pt = sunxi_flash_nand_flush;
	sunxi_sprite_force_erase_pt = sunxi_flash_nand_force_erase;

	sunxi_secstorage_read_pt  = nand_secure_storage_read;
	sunxi_secstorage_write_pt = nand_secure_storage_write;

	if (workmode == 0x30) {
		if (sunxi_sprite_init(1)) {
			printf("nand init fail\n");
			return -1;
		}
	}
	set_boot_storage_type(STORAGE_NAND);
	return 0;
}



