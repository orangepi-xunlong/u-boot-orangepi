/*
 * (C) Copyright 2018 allwinnertech  <xulu@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 * xulu@allwinnertech.com
 */

#include <common.h>
#include <asm/global_data.h>
#include <command.h>
#include <malloc.h>
#include <rtos_image.h>
#include <private_uboot.h>
#include <sunxi_image_verifier.h>
#include <mapmem.h>

DECLARE_GLOBAL_DATA_PTR;

struct spare_rtos_head_t {
	struct spare_boot_ctrl_head    boot_head;
	uint8_t   rotpk_hash[32];
	unsigned int rtos_dram_size;     /* rtos dram size, passed by uboot*/
};

static int do_sunxi_boot_rtos(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	unsigned long src_addr = 0, dst_addr = 0;
	unsigned long src_len = 0, dst_len = 0;
	struct rtos_img_hdr *rtos_hdr;
	int ret = 0;
	void (*rtos_entry)(void);

	if (argc < 3) {
		printf("parameters error\n");
		return -1;
	} else {
		/* use argument only*/
		src_addr = simple_strtoul(argv[1], NULL, 16);
		dst_addr = simple_strtoul(argv[2], NULL, 16);
	}

	rtos_hdr = (struct rtos_img_hdr *)src_addr;

	src_len = rtos_hdr->rtos_size;
	// in case of unaligned address
	dst_len |= *((unsigned char *)rtos_hdr + rtos_hdr->rtos_size + rtos_hdr->rtos_offset - 4);
	dst_len |=  (*((unsigned char *)rtos_hdr + rtos_hdr->rtos_size + rtos_hdr->rtos_offset - 3) << 8);
	dst_len |=  (*((unsigned char *)rtos_hdr + rtos_hdr->rtos_size + rtos_hdr->rtos_offset - 2) << 16);
	dst_len |=  (*((unsigned char *)rtos_hdr + rtos_hdr->rtos_size + rtos_hdr->rtos_offset - 1) << 24);

	ret = gunzip((void *)dst_addr, dst_len, (void *)(src_addr + rtos_hdr->rtos_offset), &src_len);
	if (ret) {
		printf("Error uncompressing freertos-gz\n");
		return ret;
	}

	// prepare for rtos
	board_quiesce_devices();
	cleanup_before_linux();

	// save dram_size and rotpk_hash to rtos header
	((struct spare_rtos_head_t *)dst_addr)->rtos_dram_size = uboot_spare_head.boot_data.dram_scan_size;
	memcpy(((struct spare_rtos_head_t *)dst_addr)->rotpk_hash, uboot_spare_head.hash, 32);

	printf("jump to rtos!\n\n\n");
	rtos_entry = (void (*)(void))dst_addr;
	rtos_entry();

	return 0;
}


U_BOOT_CMD(
	boot_rtos,	3,	1,	do_sunxi_boot_rtos,
	"boot rtos",
	"rtos_gz_addr rtos_addr"
);
