// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2011 Andes Technology Corporation
 * Shawn Lin, Andes Technology Corporation <nobuhiro@andestech.com>
 * Macpaul Lin, Andes Technology Corporation <macpaul@andestech.com>
 * Rick Chen, Andes Technology Corporation <rick@andestech.com>
 */

#include <common.h>
#include <command.h>
#include <dm.h>
#include <fdt_support.h>
#include <hang.h>
#include <dm/root.h>
#include <image.h>
#include <asm/byteorder.h>
#include <asm/csr.h>
#include <asm/smp.h>
#include <dm/device.h>
#include <dm/root.h>
#include <u-boot/zlib.h>
#include <private_opensbi.h>

DECLARE_GLOBAL_DATA_PTR;

int check_dtbo_idx(void);
__weak void board_quiesce_devices(void)
{
}

void  boot_jmp_opensbi(ulong opensbi_addr, u32 core_id, ulong dtb_addr,
					struct fw_dynamic_info *opensbi_info)
{
		asm volatile ("mv s1, %0" :: "r" (opensbi_addr) : "memory");
		asm volatile ("mv a0, %0" :: "r" (core_id) : "memory");
		asm volatile ("mv a1, %0" :: "r" (dtb_addr) : "memory");
		asm volatile ("mv a2, %0" :: "r" (opensbi_info) : "memory");
		asm volatile ("jr s1");
}

/**
 * announce_and_cleanup() - Print message and prepare for kernel boot
 *
 * @fake: non-zero to do everything except actually boot
 */
static void announce_and_cleanup(int fake)
{
	pr_emerg("\nStarting kernel ...%s\n\n", fake ?
		"(fake run for tracing)" : "");
	bootstage_mark_name(BOOTSTAGE_ID_BOOTM_HANDOFF, "start_kernel");
#ifdef CONFIG_BOOTSTAGE_FDT
	bootstage_fdt_add_report();
#endif
#ifdef CONFIG_BOOTSTAGE_REPORT
	bootstage_report();
#endif

#ifdef CONFIG_USB_DEVICE
	udc_disconnect();
#endif

	board_quiesce_devices();

	/*
	 * Call remove function of all devices with a removal flag set.
	 * This may be useful for last-stage operations, like cancelling
	 * of DMA operation or releasing device internal buffers.
	 */
	dm_remove_devices_flags(DM_REMOVE_ACTIVE_ALL);

	cleanup_before_linux();
}

static void boot_prep_linux(bootm_headers_t *images)
{
	if (IMAGE_ENABLE_OF_LIBFDT && images->ft_len) {
#ifdef CONFIG_OF_LIBFDT
		debug("using: FDT\n");
		if (image_setup_linux(images)) {
			printf("FDT creation failed! hanging...");
			hang();
		}
#endif
	} else {
		printf("Device tree not found or missing FDT support\n");
		hang();
	}
}
#ifdef CONFIG_ARCH_SUNXI
static void boot_jump_linux(bootm_headers_t *images, int flag)
{
	void (*kernel)(ulong hart, void *dtb);
	int fake = (flag & BOOTM_STATE_OS_FAKE_GO);
#ifdef CONFIG_SMP
	int ret;
#endif
	unsigned long r2;
	unsigned long opensbi_run_addr = env_get_hex("opensbi_run_addr", 0);
	struct fw_dynamic_info opensbi_info;
	kernel = (void (*)(ulong, void *))images->ep;

	bootstage_mark(BOOTSTAGE_ID_RUN_OS);

	debug("## Transferring control to kernel (at address %08lx) ...\n",
	      (ulong)kernel);

	announce_and_cleanup(fake);

	if (!fake) {
		if (IMAGE_ENABLE_OF_LIBFDT && images->ft_len) {
			r2 = env_get_hex("load_dtb_addr", CONFIG_SUNXI_FDT_ADDR);
#ifdef CONFIG_SUNXI_ANDROID_OVERLAY
		if (check_dtbo_idx() == 0) {
			if (sunxi_support_ufdt((void *)images->ft_addr, fdt_totalsize(images->ft_addr)) == NULL) {
				pr_err("sunxi android dto merge fail\n");
			}
		}
#endif
		if (!opensbi_run_addr) {
			memcpy((void *)r2, images->ft_addr, images->ft_len);
#ifdef CONFIG_SMP
			ret = smp_call_function(images->ep,
						(ulong)images->ft_addr, 0, 0);
			if (ret)
				hang();
#endif
			debug("## Linux machid: %08lx, FDT addr: %08lx\n", gd->arch.boot_hart, (ulong)images->ft_addr);
			kernel(gd->arch.boot_hart, (void *)r2);
		} else {
			printf("start opensbi\n");
			opensbi_info.magic = FW_DYNAMIC_INFO_MAGIC_VALUE;
			opensbi_info.version = 0x1;
			opensbi_info.next_addr = images->ep;
			opensbi_info.next_mode = FW_DYNAMIC_INFO_NEXT_MODE_S;
			opensbi_info.options = 0;
			opensbi_info.boot_hart = 0;

			boot_jmp_opensbi(opensbi_run_addr, gd->arch.boot_hart, r2, &opensbi_info);
		}
	}
	}
}

#else
static void boot_jump_linux(bootm_headers_t *images, int flag)
{
	void (*kernel)(ulong hart, void *dtb);
	int fake = (flag & BOOTM_STATE_OS_FAKE_GO);
#ifdef CONFIG_SMP
	int ret;
#endif

	kernel = (void (*)(ulong, void *))images->ep;

	bootstage_mark(BOOTSTAGE_ID_RUN_OS);

	debug("## Transferring control to kernel (at address %08lx) ...\n",
	      (ulong)kernel);

	announce_and_cleanup(fake);

	if (!fake) {
		if (IMAGE_ENABLE_OF_LIBFDT && images->ft_len) {
#ifdef CONFIG_SMP
			ret = smp_call_function(images->ep,
						(ulong)images->ft_addr, 0, 0);
			if (ret)
				hang();
#endif
			kernel(gd->arch.boot_hart, images->ft_addr);
		}
	}
}
#endif

int do_bootm_linux(int flag, int argc, char * const argv[],
		   bootm_headers_t *images)
{
	/* No need for those on RISC-V */
	if (flag & BOOTM_STATE_OS_BD_T || flag & BOOTM_STATE_OS_CMDLINE)
		return -1;

	if (flag & BOOTM_STATE_OS_PREP) {
		boot_prep_linux(images);
		return 0;
	}

	if (flag & (BOOTM_STATE_OS_GO | BOOTM_STATE_OS_FAKE_GO)) {
		boot_jump_linux(images, flag);
		return 0;
	}

	boot_prep_linux(images);
	boot_jump_linux(images, flag);
	return 0;
}

int do_bootm_vxworks(int flag, int argc, char * const argv[],
		     bootm_headers_t *images)
{
	return do_bootm_linux(flag, argc, argv, images);
}
