/* SPDX-License-Identifier: GPL-2.0+
 *
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Some init for sunxi platform.
 */

#include "sunxi_serial.h"
#include <asm/arch/clock.h>
#include <asm/arch/dram.h>
#include <asm/arch/efuse.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mmc.h>
#include <asm/arch/rtc.h>
#include <asm/arch/spl.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/timer.h>
#include <asm/arch/tzpc.h>
#include <asm/arch/efuse.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <common.h>
#include <i2c.h>
#include <linux/compiler.h>
#include <mmc.h>
#include <private_uboot.h>
#include <linux/mtd/aw-spinand.h>
#include <serial.h>
#include <spare_head.h>
#include <spl.h>
#include <sunxi_board.h>
#include <sunxi_flash.h>
#include <sys_config.h>
#include <fdt_support.h>
#include <efuse_map.h>
#include "board_helper.h"
#include <sunxi_image_verifier.h>
#include <efuse_map.h>
#ifdef CONFIG_SUNXI_CE_DRIVER
#include <asm/arch/ce.h>
#endif
#include <android_image.h>
#include <sunxi_power/axp.h>
#include <sunxi_power/power_manage.h>
#include <sunxi_keybox.h>
#include <boot_gui.h>
#include <sunxi_bmp.h>
#include <smc.h>
#include <fdt_support.h>
#include <asm/arch/dma.h>
#ifdef CONFIG_SUNXI_REPLACE_FDT_FROM_PARTITION
#include "sunxi_replace_fdt.h"
#endif
#include <sunxi_logo_display.h>
#ifdef CONFIG_SUNXI_LRADC_VOL
#include "sunxi_lradc_vol.h"
#endif
#include <fastlogo.h>
#include <sunxi_eink.h>

int  __attribute__((weak)) sunxi_platform_power_off(void)
{
	return 0;
}

void set_boot_work_mode(int work_mode)
{
       uboot_spare_head.boot_data.work_mode = work_mode;
}

int get_boot_work_mode(void)
{
	return uboot_spare_head.boot_data.work_mode;
}

void set_boot_debug_mode(int debug_mode)
{
       gd->debug_mode = debug_mode;
}

int get_boot_debug_mode(void)
{
	return gd->debug_mode;
}

int get_boot_storage_type_ext(void)
{
	/* get real storage type that from BROM at boot mode*/
	return uboot_spare_head.boot_data.storage_type;
}

int get_boot_storage_type(void)
{
	/* we think that nand and spi-nand are the same storage medium */
	/* so we can use the same process to deal with them */
	if (uboot_spare_head.boot_data.storage_type == STORAGE_NAND ||
	    uboot_spare_head.boot_data.storage_type == STORAGE_SPI_NAND) {
		return STORAGE_NAND;
	}
	return uboot_spare_head.boot_data.storage_type;
}

void set_boot_storage_type(int storage_type)
{
	uboot_spare_head.boot_data.storage_type = storage_type;
}

#ifdef CONFIG_SUNXI_GETH
extern int geth_initialize(bd_t *bis);
#ifdef CONFIG_PHY_SUNXI_ACX00
extern int ephy_init(void);
#endif
#endif

int board_eth_init(bd_t *bis)
{
	int rc = 0;

	int workmode = uboot_spare_head.boot_data.work_mode;
	if (!((workmode == WORK_MODE_BOOT) ||
		(workmode == WORK_MODE_CARD_PRODUCT) ||
		(workmode == WORK_MODE_SPRITE_RECOVERY))) {
		return 0;
	}

#ifdef CONFIG_SUNXI_GETH
#ifdef CONFIG_PHY_SUNXI_ACX00
	ephy_init();
#endif
	rc = geth_initialize(bis);
#endif

	return rc;
}

int sunxi_probe_secure_monitor(void)
{
	return uboot_spare_head.boot_data.monitor_exist ==
			       SUNXI_SECURE_MODE_USE_SEC_MONITOR ?
		       1 :
		       0;
}

int sunxi_probe_secure_os(void)
{
	return uboot_spare_head.boot_data.secureos_exist;
}

int sunxi_get_secureboard(void)
{
#ifdef SID_SECURE_MODE
	return readl(IOMEM_ADDR(SID_SECURE_MODE)) & 1;
#elif defined(EFUSE_ANTI_BRUSH)
	return (readl(ANTI_BRUSH_MODE) >> ANTI_BRUSH_BIT_OFFSET) & 1;
#elif defined(SECURE_READ_TEST_REG)
	/*
	 * two way start up uboot:
	 * 1.fel:
	 * 		board secure = ~(cpu secure)
	 * 2.from optee(boot0/sboot)
	 * 		optee help read secure enable bit
	 */
	if (sunxi_probe_secure_os()) {
		return (((arm_svc_read_sec_reg(SUNXI_SID_SRAM_BASE + EFUSE_LCJS)) &
					(1 << 11)) != 0);
	} else {
		return (readl(SECURE_READ_TEST_REG) == 0);
	}
#else
	return 0;
#endif
}

int sunxi_probe_securemode(void)
{
	int secure_mode = 0;

	secure_mode = sunxi_get_secureboard();
	tick_printf("secure enable bit: %d\n", secure_mode);

	if (secure_mode) {
		// sbrom  set  secureos_exist flag,
		// 1: secure os exist 0: secure os not exist
		if (uboot_spare_head.boot_data.secureos_exist == 1) {
			gd->securemode = SUNXI_SECURE_MODE_WITH_SECUREOS;
			debug("secure mode: with secureos\n");
		} else {
			gd->securemode = SUNXI_SECURE_MODE_NO_SECUREOS;
			debug("secure mode: no secureos\n");
		}
		gd->bootfile_mode = SUNXI_BOOT_FILE_TOC;
#ifdef CONFIG_SUNXI_ANTI_BRUSH
		if (get_boot_work_mode() == WORK_MODE_BOOT) {
			debug("init preserve toc1\n");
			if (sunxi_verify_preserve_toc1((void *)CONFIG_SUNXI_BOOTPKG_BASE)) {
				pr_err("%s: preserve toc1 error\n", __func__);
			}
		}
#endif
	} else {
		//boot0  set  secureos_exist flag,
		//1: secure monitor exist 0: secure monitor  not exist
		int burn_secure_mode=0;

		gd->securemode = SUNXI_NORMAL_MODE;
		gd->bootfile_mode = SUNXI_BOOT_FILE_PKG;

		if (get_boot_work_mode() != WORK_MODE_BOOT) {
			debug("check if downloading secure img\n");
			if (script_parser_fetch("/soc/target",
						"burn_secure_mode",
						&burn_secure_mode, 1))
				burn_secure_mode = uboot_spare_head.boot_data.secure_mode;

			if (burn_secure_mode != 1) {
				return 0;
			}
			printf("normal mode: download secure firmware\n");
			gd->bootfile_mode = SUNXI_BOOT_FILE_TOC;
		}
	}
	return 0;
}

#if defined(CONFIG_SUNXI_BURN_ROTPK_ON_SPRITE) || \
	defined(CONFIG_SUNXI_ROTPK_BURN_ENABLE_BY_TOOL)
int sunxi_burn_rotpk(void)
{
	u8 hash[32] = { 0 }, readback_hash[32] = { 0 }, hash_zero[32] = { 0 };
	int hash_len = 0;
	int ret = 0;
	efuse_key_info_t efuse_key_info;

#if defined(CONFIG_SUNXI_ROTPK_BURN_ENABLE_BY_TOOL)
	if ((uboot_spare_head.boot_data.func_mask &
	     UBOOT_FUNC_MASK_BIT_BURN_ROTPK) !=
	    UBOOT_FUNC_MASK_BIT_BURN_ROTPK) {
		pr_msg("tool did not set rotpk burn flag, skip rotpk burn\n");
		/* tool did not set uboot to burn rotpk, do not burn */
		return 0;
	}
#endif

	sunxi_ss_open();
	if (sunxi_verify_get_rotpk_hash(hash)) {
		pr_err("get rotpk failed\n");
		return -1;
	}

	memset(&efuse_key_info, 0, sizeof(efuse_key_info));
	efuse_key_info.len = 32;
	memcpy(efuse_key_info.name, "rotpk", sizeof("rotpk"));
	efuse_key_info.key_data = hash;

	memset(readback_hash, 0, 32);
	if (gd->securemode == SUNXI_NORMAL_MODE) {
		sunxi_efuse_read("rotpk", readback_hash, &hash_len);
	} else {
		arm_svc_efuse_read("rotpk", readback_hash);
	}

	printf("read rotpk before write:\n");
	sunxi_dump(readback_hash, 32);

	if (memcmp(readback_hash, hash_zero, 32)) {
		printf("puk hash not zero\n");
		if (memcmp(readback_hash, hash_zero, 32)) {
			printf("puk hash not same\n");
			return -1;
		} else {
			printf("puk hash is same, skip burn rotpk\n");
			return 0;
		}
	}

	memset(readback_hash, 0, 32);
	printf("now write rotpk:\n");
	if (gd->securemode == SUNXI_NORMAL_MODE) {
		if (sunxi_efuse_write(&efuse_key_info)) {
			pr_err("burn rotpk failed");
			return -1;
		}
		if (sunxi_efuse_read("rotpk", readback_hash, &hash_len)) {
			pr_err("read puk hash fail\n");
			return -1;
		}
	} else {
		ret = arm_svc_efuse_write(&efuse_key_info);
		if (ret) {
			pr_err("svc burn rotpk failed with %d\n", ret);
			return -1;
		}
		if (arm_svc_efuse_read("rotpk", readback_hash) != 32) {
			pr_err("svc read puk hash fail\n");
			return -1;
		}
	}
	printf("rotpk burn done, using rotpk:\n");
	sunxi_dump(readback_hash, 32);
	if (memcmp(efuse_key_info.key_data, readback_hash, 32) != 0) {
		pr_err("verify rotpk failed, firmware rotpk:\n");
		sunxi_dump(efuse_key_info.key_data, 32);
		return -1;
	}

	return 0;
}
#endif

int sunxi_set_secure_mode(void)
{
	int mode;
	u8  hash[32] = {0},hash_tmp[32] = {0};
	int hash_len = 0;

	if ((gd->securemode == SUNXI_NORMAL_MODE) &&
	    (gd->bootfile_mode == SUNXI_BOOT_FILE_TOC)) {
		mode = sid_probe_security_mode();
		if (!mode) {
			int ret = sunxi_efuse_get_rotpk_status();

			if (ret == -1) {
				//api not supported, try read rotpk directly
				if (sunxi_efuse_read("rotpk", hash,
						     &hash_len)) {
					printf("read puk hash fail\n");
					return -1;
				}
				printf("read puk finished,len:%d\n", hash_len);
				if (memcmp(hash, hash_tmp, sizeof(hash))) {
					printf("puk hash not zero,fail\n");
					return -1;
				}
			} else if (ret == 1) {
				printf("puk burned,stop set secure mode!\n");
				return -1;
			}

#if defined(CONFIG_SUNXI_BURN_ROTPK_ON_SPRITE) || \
	defined(CONFIG_SUNXI_ROTPK_BURN_ENABLE_BY_TOOL)
			if (sunxi_burn_rotpk())
				return -1;
#endif

			if (sid_set_security_mode()) {
				printf("burn secure bit fail\n");
				return -1;
			}
			gd->bootfile_mode = SUNXI_BOOT_FILE_TOC;
			printf("burn done, now secure bit is:%d\n",
			       sid_probe_security_mode());
		} else {
			printf("secure chip, don't repeat burn secure bit\n");
		}
	}

	return 0;
}

int sunxi_get_securemode(void)
{
	return gd->securemode;
}

int sunxi_get_uboot_shell(void)
{
	return gd->uboot_shell;
}

int sunxi_set_uboot_shell(bool flag)
{
	gd->uboot_shell = flag;
	return 0;
}

int sunxi_set_force_32bit_os(int forced)
{
	gd->force_32bit_os = forced;
	return 0;
}

int sunxi_get_force_32bit_os(void)
{
	return gd->force_32bit_os;
}

void reset_boot_dram_update_flag(u32 *dram_para)
{
	u32 flag	     = 0;
	unsigned int *pdram = dram_para;
	flag		     = pdram[23];
	flag &= ~(0x1U << 31);
	pdram[23] = flag;
}

void set_boot_dram_update_flag(u32 *dram_para)
{
	/* [23]:bit31 */
	/* 0:uboot update boot0  1: not */
	u32 flag	     = 0;
	unsigned int *pdram = dram_para;
	flag		     = pdram[23];
	flag |= (0x1U << 31);
	pdram[23] = flag;
}

u32 get_boot_dram_update_flag(void)
{
	u32 flag = 0;
	/* boot pass dram para to uboot */
	/* [23]:bit31 */
	/* 0:uboot update boot0  1: not */
	unsigned int *pdram = (unsigned int *)uboot_spare_head.boot_data.dram_para;
	flag = (pdram[23] >> 31) & 0x1;
	return flag == 0 ? 1:0;
}

/*
 * sunxi_parsed_specific_string() - Parse the string skipped by skip_character at intervals of space_character
 * @intput_string: the string to be parsed
 * @output_para[][16]: store the parsed string
 * @space_character: characters separated by space_character
 * @skip_character: skip when encountering skip_character, 0 is invalid
 *
 * exp:
 * 	the string to be parsed:"bootloader, env,boot,vendor_boot, dtbo"
 * 	space_character = ','
 * 	skip_character = ' '
 * output:
 * 	output_para[0] = bootloader
 * 	output_para[1] = env
 * 	output_para[2] = boot
 * 	output_para[3] = vendor_boot
 * 	output_para[4] = dtbo
 *
 */
int sunxi_parsed_specific_string(char *intput_string, char output_para[][16], char space_character, char skip_character)
{
	int i, j, k;
	for (i = 0, j = 0, k = 0;; i++) {
		if ((skip_character != 0) && (intput_string[i] == skip_character)) {
			continue;
		} else if ((intput_string[i] == space_character) || (intput_string[i] == 0)) {
			output_para[k][j] = 0;
			k++;
			j = 0;
			if (intput_string[i] == 0)
				break;
		} else {
			output_para[k][j] = intput_string[i];
			j++;
		}
	}
	/* for (i = 0; i < k; i++)
	 *         printf("output_para[%d]:%s\n", i, output_para[i]); */
	return 0;
}

int initr_sunxi_display(void)
{
	int ret = 0;
#ifdef CONFIG_DISP2_SUNXI
	int workmode = get_boot_work_mode();
	if ((workmode == WORK_MODE_USB_PRODUCT) ||
		(workmode == WORK_MODE_USB_UPDATE) ||
		(workmode == WORK_MODE_USB_DEBUG)) {
		return 0;
	}
	ret = drv_disp_init();
#endif
	return ret;
}

#ifdef CONFIG_EINK200_SUNXI
int initr_sunxi_eink(void)
{
	int workmode = get_boot_work_mode();

	if ((workmode == WORK_MODE_USB_PRODUCT) ||
	    (workmode == WORK_MODE_USB_UPDATE) ||
		(workmode == WORK_MODE_USB_DEBUG)) {
		return 0;
	}
	eink_driver_init();
	eink_framebuffer_init();
#ifdef CONFIG_PMIC_TPS65185
		tps65185_modules_init();
#endif
	return 0;
}
#endif

int sunxi_boot_init_gpio(void)
{
#define FDT_PATH_BOOT_INIT_GPIO "/soc/boot_init_gpio"
	user_gpio_set_t gpio_init;
	int nodeoffset;
	if (get_boot_work_mode() != WORK_MODE_BOOT) {
		return 0;
	}
	nodeoffset = fdt_path_offset(working_fdt, FDT_PATH_BOOT_INIT_GPIO);
	if (IS_ERR_VALUE(nodeoffset)) {
		return 0;
	}
	if (!fdtdec_get_is_enabled(working_fdt, nodeoffset)) {
		return 0;
	}
	if (fdt_get_one_gpio(FDT_PATH_BOOT_INIT_GPIO, "gpio0", &gpio_init) ==
	    0) {
		sunxi_gpio_request(&gpio_init, 1);
	}
	if (fdt_get_one_gpio(FDT_PATH_BOOT_INIT_GPIO, "gpio1", &gpio_init) ==
	    0) {
		sunxi_gpio_request(&gpio_init, 1);
	}

	return 0;
}

#ifdef CONFIG_PMIC_TPS65185
int tps65185_modules_init(void)
{
	int ret = 0;

	ret = tps65185_init();

	if (ret)
		pr_msg("tps65185 init failed ret = %d\n", ret);
	return 0;
}
#endif

int board_early_init_r(void)
{
	int ret = 0;
#ifdef CONFIG_CLK_SUNXI
	int work_mode = get_boot_work_mode();
	if ((work_mode == WORK_MODE_BOOT) ||
		(work_mode == WORK_MODE_CARD_PRODUCT) ||
		(work_mode == WORK_MODE_USB_PRODUCT) ||
		(work_mode == WORK_MODE_CARD_UPDATE)) {
		clk_init();
		ret  = initr_sunxi_display();
#ifdef CONFIG_EINK200_SUNXI
		ret = initr_sunxi_eink();
#endif
	}
#endif

	return ret;
}

/*
 * logo display priority:
 * advert picture > bootlogo in boot package > bootlogo in boot-resources/bootloader
 *
 * when /soc/target/advert_enable=1, display advert picture in Reserve0 partition
 * gd->boot_logo_addr is used to  indicate whether bootlogo in boot package exist/used
 * when no advert picture and gd->boot_logo_addr, bootlogo in boot-resouce is used as
 * default
 */
#ifdef CONFIG_BOOT_GUI
void sunxi_early_logo_display(void)
{
	void *bpk_logo_addr;
	int ret		  = -1;
	int work_mode = get_boot_work_mode();
#ifdef CONFIG_SUNXI_ADVERT_PICTURE
	int advert_enable = 0;
	ret = script_parser_fetch("/soc/target", "advert_enable",
				  (int *)&advert_enable, sizeof(int) / 4);
	if ((ret == 0) && (advert_enable == 1)) {
		gd->boot_logo_addr = 0;
		/* card product need boot_gui_init, so do not return */
		if (work_mode != WORK_MODE_CARD_PRODUCT
			&& work_mode != WORK_MODE_CARD_UPDATE)
			return;
	}
#endif

	if (work_mode == WORK_MODE_CARD_PRODUCT
		|| work_mode == WORK_MODE_CARD_UPDATE) {
		boot_gui_init();
	} else if (gd->boot_logo_addr) {
		boot_gui_init();
		bpk_logo_addr = sunxi_prepare_bpk_bootlogo();
		if (bpk_logo_addr) {
			ret = show_bmp_on_fb(bpk_logo_addr, FB_ID_0);
		}
		if ((ret != 0) || (bpk_logo_addr == 0)) {
			gd->boot_logo_addr = 0;
		}
	}
}

void board_bootlogo_display(void)
{
	if (gd->boot_logo_addr) {
		return;
	}
	boot_gui_init();

#ifdef CONFIG_SUNXI_POWER
	if (gd->chargemode)
		return;
#endif
#ifdef CONFIG_SUNXI_ADVERT_PICTURE
	int ret		  = -1;
	int advert_enable = 0;
	ret = script_parser_fetch("/soc/target", "advert_enable",
				  (int *)&advert_enable, sizeof(int) / 4);
	if ((ret == 0) && (advert_enable == 1)) {
		ret = sunxi_advert_display("Reserve0", "advert.bmp");
		if (ret == 0) {
			return;
		}
	}
#endif

//#if defined(CONFIG_CMD_SUNXI_BMP)
//	sunxi_bmp_display("/boot/boot.bmp");
//#elif defined(CONFIG_CMD_SUNXI_JPEG)
//	sunxi_jpeg_display("/boot/boot.jpg");
//#endif
}
#endif

int board_env_late_init(void)
{
	if (get_boot_work_mode() == WORK_MODE_BOOT) {

#ifdef CONFIG_SUNXI_POWER
		sunxi_update_axp_info();
#endif

#ifdef CONFIG_BOOT_GUI
		void board_bootlogo_display(void);
		board_bootlogo_display();
#else
#ifdef CONFIG_SUNXI_SPINOR_JPEG
		int sunxi_jpeg_display(const char *filename);
		sunxi_jpeg_display("bootlogo.jpg");
#endif

#ifdef CONFIG_SUNXI_SPINOR_BMP
#if defined(CONFIG_CMD_FAT)
		fat_read_logo_to_kernel("bootlogo.bmp");
#else
		read_bmp_to_kernel("bootlogo");
#endif

#endif /* CONFIG_SUNXI_SPINOR_BMP */

#endif /* CONFIG_BOOT_GUI */

#ifdef CONFIG_SUNXI_KEYBOX
		sunxi_keybox_init();
#endif
	}
	return 0;
}

#ifdef CONFIG_SUNXI_OTA_TURNNING
int update_boot0_head_for_ota(void)
{
#ifdef FPGA_PLATFORM
	return 0;
#else
	int ret = 0;
#ifdef CONFIG_MMC_SUNXI
	int storage_type = get_boot_storage_type();
	int card_num = 2;

	if (storage_type == STORAGE_EMMC3) {
		card_num = 3;
	} else if (storage_type == STORAGE_SD) {
		return 0;
	}
	ret = mmc_request_update_boot0(card_num);
#endif
#ifdef CONFIG_SUNXI_TURNNING_DRAM
	if (!ret) {
		ret = get_boot_dram_update_flag();
	}
#endif
	if (ret) {
		pr_debug("begin to update boot0 atfer ota\n");
		ret = sunxi_flash_update_boot0();
		pr_msg("update boot0 %s\n", ret == 0 ? "success" : "fail");
		/* if update fail, should reset the system */
		/* if(ret) */
		/* { */
		/* do_reset(NULL, 0, 0,NULL); */
		/* } */
	}
	return ret;
#endif
}
#endif

int board_late_init(void)
{
	int work_mode = get_boot_work_mode();
#ifdef CONFIG_SUNXI_TV_FASTLOGO
	struct fastlogo_t *p_fastlogo = NULL;
	if (work_mode == WORK_MODE_BOOT || work_mode == WORK_MODE_CARD_UPDATE ||
	    work_mode == WORK_MODE_CARD_PRODUCT) {
		p_fastlogo =
			create_fastlogo_inst("bootlogo.bmp", "bootloader",
					     "LogoRegData.bin", "bootloader");
		if (p_fastlogo) {
			p_fastlogo->display_fastlogo(p_fastlogo);
		}
	}
#endif
	if (work_mode == WORK_MODE_BOOT) {
#ifdef CONFIG_SUNXI_SWITCH_SYSTEM
		sunxi_auto_switch_system();
#endif


#ifdef CONFIG_SUNXI_LRADC_VOL
/* Note: lradc should be initialized 30ms before
 * sunxi_read_lradc_vol() which lradc-sample-rate
 * is 500Hz.
 */
		lradc_reg_init();
#endif
#ifdef CONFIG_EINK200_SUNXI
		sunxi_bmp_display("bootlogo.bmp");
#endif

#ifdef CONFIG_SUNXI_ARISC_EXIST
#ifndef CONFIG_ARISC_DEASSERT_BEFORE_KERNEL
		sunxi_arisc_probe();
#endif
#endif
#ifdef CONFIG_SUNXI_OTA_TURNNING
		update_boot0_head_for_ota();
#endif

#ifdef CONFIG_SUNXI_ANDROID_BOOT
		sunxi_fastboot_status_read();
#endif

#ifdef CONFIG_SUNXI_USER_KEY
		/* update mac/wifi serial info in env */
		extern int update_user_data(void);
		update_user_data();
#endif
#ifdef CONFIG_SUNXI_CHECK_LIMIT_VERIFY
		int sunxi_check_cpu_gpu_verify(void);
		sunxi_check_cpu_gpu_verify();
#endif

#ifdef CONFIG_SUNXI_CHECK_CUSTOMER_RESERVED_ID
		int sunxi_check_customer_reserved_id(void);
		sunxi_check_customer_reserved_id();
#endif

#ifdef CONFIG_SUNXI_UBIFS
		if ((get_boot_storage_type() == STORAGE_NAND) && nand_use_ubi())
			ubi_nand_update_ubi_env();
		else
#endif
		//sunxi_update_partinfo();
		//if (sunxi_update_rotpk_info()) {
		//	return -1;
		//}
#ifdef CONFIG_SUNXI_POWER
#ifdef CONFIG_SUNXI_BMU
		axp_battery_status_handle();
#endif
#endif
		sunxi_respond_ir_key_action();
		//sunxi_update_bootcmd();
#ifdef CONFIG_SUNXI_SERIAL
		sunxi_set_serial_num();
#endif
#ifdef CONFIG_SUNXI_LRADC_VOL
		sunxi_read_lradc_vol();
#endif
#if !defined(CONFIG_OF_SEPARATE)
		sunxi_update_fdt_para_for_kernel();
#elif defined(CONFIG_SUNXI_NECESSARY_REPLACE_FDT)
		sunxi_replace_fdt_v2();
		sunxi_update_fdt_para_for_kernel();
#elif defined(CONFIG_SUNXI_REPLACE_FDT_FROM_PARTITION)
		sunxi_replace_fdt();
		sunxi_update_fdt_para_for_kernel();
#endif
#ifdef CONFIG_SUNXI_TV_FASTLOGO
		if (p_fastlogo) {
			p_fastlogo->reserve_memory(p_fastlogo);
		}
#endif
	}
	return 0;
}


uint sunxi_generate_checksum(void *buffer, uint length, uint div, uint src_sum)
{
	uint *buf;
	int count;
	uint sum;

	count = length >> 2;
	sum   = 0;
	buf   = (__u32 *)buffer;
	do {
		sum += *buf++;
		sum += *buf++;
		sum += *buf++;
		sum += *buf++;
	} while ((count -= (4*div)) > (4 - 1));

	while (count-- > 0)
		sum += *buf++;

	sum = sum - src_sum + STAMP_VALUE;

	return sum;
}


int sunxi_verify_checksum(void *buffer, uint length, uint src_sum)
{
	uint sum;
	sum = sunxi_generate_checksum(buffer, length, 1, src_sum);

	debug("src sum=%x, check sum=%x\n", src_sum, sum);
	if (sum == src_sum)
		return 0;
	else
		return -1;
}
void reset_misc(void)
{
	sunxi_board_restart(0);
}
/*
  * note:
  * call driver exit here.
  * this func be called before enter linux.
  */
void board_quiesce_devices(void)
{
	sunxi_flash_flush();
	/*modify 2 for nor to finally exit*/
	sunxi_flash_exit(2);
#ifdef CONFIG_SUNXI_DMA
	sunxi_dma_exit();
#endif
}

void sunxi_board_close_source(void)
{
	board_quiesce_devices();
	disable_interrupts();
	return;
}

int sunxi_board_restart(int next_mode)
{
	rtc_set_bootmode_flag(next_mode);
	sunxi_board_close_source();
	reset_cpu(0);

	return 0;
}

int sunxi_board_shutdown(void)
{
	sunxi_board_close_source();
#ifdef CONFIG_SUNXI_UBOOT_POWER_OFF
	sunxi_platform_power_off();
#endif
#ifdef CONFIG_SUNXI_BMU
	bmu_set_power_off();
#endif
#ifdef CONFIG_SUNXI_PMU
	pmu_set_power_off();
#endif
	while (1) {
		asm volatile ("wfi");
	}
	return 0;
}

int sunxi_board_run_fel(void)
{
	rtc_set_fel_flag();
	sunxi_board_close_source();
	reset_cpu(0);

	return 0;
}

void sunxi_update_subsequent_processing(int next_work)
{
	printf("next work %d\n", next_work);
	switch (next_work) {
	case SUNXI_UPDATE_NEXT_ACTION_REBOOT:
	case SUNXI_UPDATA_NEXT_ACTION_SPRITE_TEST:
		printf("SUNXI_UPDATE_NEXT_ACTION_REBOOT\n");
		sunxi_board_restart(0);
		break;

	case SUNXI_UPDATE_NEXT_ACTION_SHUTDOWN:
		printf("SUNXI_UPDATE_NEXT_ACTION_SHUTDOWN\n");
#ifdef CONFIG_SUNXI_UPDATE_REMIND
		sunxi_update_remind();
#endif
		sunxi_board_shutdown();
		break;

	case SUNXI_UPDATE_NEXT_ACTION_REUPDATE:
		printf("SUNXI_UPDATE_NEXT_ACTION_REUPDATE\n");
		sunxi_board_run_fel();
		break;

	case SUNXI_UPDATE_NEXT_ACTION_BOOT:
	case SUNXI_UPDATE_NEXT_ACTION_NORMAL:
	default:
		printf("SUNXI_UPDATE_NEXT_ACTION_NULL\n");
#ifdef CONFIG_SUNXI_UPDATE_REMIND
		sunxi_update_remind();
#endif
		break;
	}

	return;
}

int sunxi_boot_image_get_embbed_cert_len(const void *hdr)
{
	struct boot_img_hdr_ex *hdr_ex = (struct boot_img_hdr_ex *)hdr;
	if (strncmp((void *)(hdr_ex->cert_magic), AW_CERT_MAGIC,
		    strlen(AW_CERT_MAGIC)) != 0)
		return 0;
	else
		return hdr_ex->cert_size;
}

void *sunxi_prepare_bpk_bootlogo(void)
{
	int ret;
	unsigned char *buf;
	unsigned char *compress_buf;
	if (!gd->boot_logo_addr) {
		pr_msg("no bootlogo in boot package\n");
		return 0;
	}
	buf = (unsigned char *)malloc(SUNXI_DISPLAY_FRAME_BUFFER_SIZE);
	if (!buf) {
		pr_err("malloc logo buf failed\n");
		return 0;
	}
	compress_buf = (unsigned char *)gd->boot_logo_addr;
	ret	  = sunxi_bmp_decode_from_compress(buf, compress_buf);
	if (ret) {
		pr_msg("bootlogo lzma decode err\n");
		return 0;
	}
	return buf;
}

#ifdef CONFIG_SUNXI_USB_DETECT
extern volatile int sunxi_usb_detect_flag;
extern int sunxi_usb_init(int delaytime);
extern int sunxi_usb_exit(void);
int sunxi_usb_detect(void)
{
	int ret = -1;
	ulong begin_time = 0, over_time = 0;

	if (sunxi_usb_dev_register(6) < 0) {
		printf("usb detect fail: not support usb detect \n");
		return ret;
	}
	if (sunxi_usb_init(0)) {
		printf("%s usb init fail\n", __func__);
		sunxi_usb_exit();
		return ret;
	}

	begin_time = get_timer(0);
	over_time = 800;
	while (1) {
		if (sunxi_usb_detect_flag) {
			printf("[%s] usb detect ok\n", __func__);
			ret = 0;
			break;
		}
		if (get_timer(begin_time) > over_time) {
			printf("overtime\n");
			printf("usb : no usb exist\n");
			ret = -1;
			break;
		}
	}
	sunxi_usb_exit();
	return ret;
}
#endif
