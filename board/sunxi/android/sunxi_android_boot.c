/*
 * (C) Copyright 2017  <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <sunxi_mbr.h>
#include <android_image.h>
#include <sunxi_image_verifier.h>
#include <sunxi_avb.h>
#include <sunxi_board.h>
#include <sunxi_verify_boot_info.h>
#include <sys_partition.h>
DECLARE_GLOBAL_DATA_PTR;

__weak int sunxi_bmp_display(__maybe_unused char *name)
{
	return 0;
}
__weak int axp_probe_key(void)
{
	return 0;
}
__weak int sunxi_board_shutdown(void)
{
	return 0;
}

#ifdef CONFIG_SUNXI_AVB
static int android_vbmeta_avb_verify(char *lock_state)
{
	u8 addr[512]	= { 0 };
	u8 addr_system[512] = { 0 };
	u8 addr_vendor[512] = { 0 };
	char sha1_hash[64];
	int total_size = 0;
	AvbVBMetaImageHeader *vbmeta_hdr;
	AvbVBMetaImageHeader *vbmeta_hdr_system;
	AvbVBMetaImageHeader *vbmeta_hdr_vendor;
	int rbyte = 0;
	int ret   = 0;
	rbyte     = sunxi_avb_read_vbmeta_head("vbmeta", addr);
	if (rbyte != 0) {
		vbmeta_hdr = (AvbVBMetaImageHeader *)addr;
		sunxi_avb_slot_vbmeta_head(vbmeta_hdr);
		sunxi_avb_dump_vbmeta(vbmeta_hdr);
	}
	rbyte = sunxi_avb_read_vbmeta_head("vbmeta_system", addr_system);
	if (rbyte != 0) {
		vbmeta_hdr_system = (AvbVBMetaImageHeader *)addr_system;
		sunxi_avb_slot_vbmeta_head(vbmeta_hdr_system);
		sunxi_avb_dump_vbmeta(vbmeta_hdr_system);
	}
	rbyte = sunxi_avb_read_vbmeta_head("vbmeta_vendor", addr_vendor);
	if (rbyte != 0) {
		vbmeta_hdr_vendor = (AvbVBMetaImageHeader *)addr_vendor;
		sunxi_avb_slot_vbmeta_head(vbmeta_hdr_vendor);
		sunxi_avb_dump_vbmeta(vbmeta_hdr_vendor);
	}
	total_size = sunxi_avb_vbmeta_size(vbmeta_hdr) +
		     sunxi_avb_vbmeta_size(vbmeta_hdr_system) +
		     sunxi_avb_vbmeta_size(vbmeta_hdr_vendor);
	pr_msg("total_size = %d \n", total_size);
	if (sunxi_avb_check_magic(vbmeta_hdr)) {
		if (sunxi_avb_verify_all_vbmeta(vbmeta_hdr, vbmeta_hdr_system,
						vbmeta_hdr_vendor, sha1_hash)) {
			sunxi_avb_setup_env("2.0", "sha256", total_size,
					    sha1_hash, lock_state);
			ret = 1;
		}
	}
	return ret;
}
#endif /*CONFIG_SUNXI_AVB*/

int sunxi_android_boot(const char *image_name, ulong os_load_addr)
{
	uint count		= 0;
	uint wait_for_power_key = 1;
	if (gd->securemode) {
		/* ORANGE, indicating a device may be freely modified.
		 * Device integrity is left to the user to verify out-of-band.
		 * The bootloader displays a warning to the user before allowing
		 * the boot process to continue.*/
		if (gd->lockflag == SUNXI_UNLOCKED) {
			sunxi_set_verify_boot_number(SUNXI_VB_INFO_LOCK, 0);
			sunxi_set_verify_boot_number(
				SUNXI_VB_INFO_BOOTSTATE,
				KM_VERIFIED_BOOT_UNVERIFIED);
			env_set("verifiedbootstate", "orange");
			pr_msg("Your device software can't be checked for corruption.\n");
			pr_msg("Please lock the bootloader.\n");
			sunxi_bmp_display("orange_warning.bmp");
#ifdef CONFIG_SUNXI_AVB
			android_vbmeta_avb_verify("unlocked");
#endif
			do {
				if (axp_probe_key()) {
					/* PAUSE BOOT */
					pr_msg("pause boot,shutdown machine\n");
					sunxi_board_shutdown();
				}
				mdelay(1000);
				count++;
			} while (count < 5);
			if (gd->chargemode != 1) {
				pr_msg("orange state:start to display bootlogo\n");
				sunxi_bmp_display("bootlogo.bmp");
			}
		} else {
			int ret;
			sunxi_set_verify_boot_number(SUNXI_VB_INFO_LOCK, 1);
#ifndef CONFIG_SUNXI_AVB
			ret = sunxi_verify_os(os_load_addr, image_name);
#else
			if (!android_vbmeta_avb_verify("locked")) {
				ret = sunxi_verify_os(os_load_addr, image_name);
			} else {
				struct andr_img_hdr *fb_hdr = (struct andr_img_hdr *)os_load_addr;
				uint8_t *vb_meta_data;
				size_t vb_len;
				ulong total_len;
				ret = sunxi_avb_read_vbmeta_data(&vb_meta_data,
								 &vb_len);
				if (ret == 0) {
					total_len =
						android_image_get_end(fb_hdr) -
						(ulong)fb_hdr;
					ret = verify_image_by_vbmeta(
						image_name,
						(uint8_t *)os_load_addr,
						total_len, vb_meta_data, vb_len,
						"vbmeta");
				} else {
					pr_error(
						"read vbmeta data for verification failed\n");
				}
				free(vb_meta_data);
			}
#endif /*CONFIG_SUNXI_AVB*/
			/* YELLOW, indicating the boot partition has been verified using the
			 * embedded certificate, and the signature is valid. The bootloader
			 * displays a warning and the fingerprint of the public key before
			 * allowing the boot process to continue.*/
			if (!strcmp(env_get("verifiedbootstate"), "yellow")) {
				sunxi_set_verify_boot_number(SUNXI_VB_INFO_BOOTSTATE, KM_VERIFIED_BOOT_SELF_SIGNED);
				pr_msg("Your device has loaded a different operating system.\n");
				pr_msg("stop booting until the power key is pressed\n");
				sunxi_bmp_display("yellow_pause_warning.bmp");
				do {
					if (axp_probe_key()) {
						/* PAUSE BOOT */
						pr_msg("pause boot,waiting for press power key again\n");
						sunxi_bmp_display(
							"yellow_continue_warning.bmp");
						do {
							if (axp_probe_key()) {
								/* CONTINUE BOOT */
								wait_for_power_key =
									0;
								/* timeout,continue to bootup */
								count = 5;
							}
						} while (wait_for_power_key);
					}
					mdelay(1000);
					count++;
				} while (count < 5);
				if (gd->chargemode != 1) {
					pr_msg("yellow state:start to display bootlogo\n");
					sunxi_bmp_display("bootlogo.bmp");
				}
			} else {
				/* GREEN, indicating a full chain of trust extending from the
				 * bootloader to verified partitions, including the bootloader,
				 * boot partition, and all verified partitions.*/
				env_set("verifiedbootstate", "green");
				sunxi_set_verify_boot_number(SUNXI_VB_INFO_BOOTSTATE, KM_VERIFIED_BOOT_VERIFIED);
			}

			/* RED, indicating the device has failed verification. The bootloader
			 * displays an error and stops the boot process.*/
			if (ret) {
				pr_msg("boota: verify the %s failed\n",
				       image_name);
				env_set("verifiedbootstate", "red");
				sunxi_set_verify_boot_number(SUNXI_VB_INFO_BOOTSTATE, KM_VERIFIED_BOOT_FAILED);
				pr_msg("Your device is corrupt.It can't be truseted and will not boot.\n");
				sunxi_bmp_display("red_warning.bmp");
				mdelay(30000);
				sunxi_board_shutdown();
				return -1;
			}
		}
	}
#ifdef CONFIG_SUNXI_AVB
	else {
		u32 start_block;
		/*
		 * if vbmeta partition exist, android is compile with AVB
		 * report AVB info for android AVB
		 */
		start_block = sunxi_partition_get_offset_byname("vbmeta");
		if (start_block) {
			if (gd->lockflag == SUNXI_UNLOCKED) {
				android_vbmeta_avb_verify("unlocked");
			} else {
				android_vbmeta_avb_verify("locked");
			}
		}
	}
#endif

	return 0;
}

