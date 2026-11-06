/*
 * Copyright (c) 2015-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <bl_common.h>		/* For ARRAY_SIZE */
#include <debug.h>
#include <emmc.h>
#include <firmware_image_package.h>
#include <io_block.h>
#include <io_driver.h>
#include <io_fip.h>
#include <io_memmap.h>
#include <io_semihosting.h>
#include <io_storage.h>
#include <platform_def.h>
#include <semihosting.h>
#include <string.h>
#include <io_fatfs.h>
#include <io_spi_nand.h>
#include <cv_private.h>
#include <cv_spinor.h>
#include <cv_sd.h>
#include <spi_nand.h>
#include <platform_common_def.h>
#include "ff.h"

/* Semihosting filenames */
#define BL2_IMAGE_NAME			"bl2.bin"
#define BL31_IMAGE_NAME			"bl31.bin"
#define BL33_IMAGE_NAME			"bl33.bin"

/* IO devices */
static const io_dev_connector_t *fip_dev_con;
static uintptr_t fip_dev_handle;
static const io_dev_connector_t *memmap_dev_con;
static uintptr_t memmap_dev_handle;
static const io_dev_connector_t *emmc_dev_con;
static uintptr_t emmc_dev_handle;
#if (defined(IMAGE_BL1) && defined(BL1_INIT_SPI_NAND)) || (defined(IMAGE_BL2) && defined(BL2_INIT_SPI_NAND))
static const io_dev_connector_t *spi_nand_dev_con;
static uintptr_t spi_nand_dev_handle;
#endif

static const io_block_dev_spec_t emmc_dev_spec = {
	/* It's used as temp buffer in block driver. */
	.buffer         = {
		.offset = BM_EMMC_BUF_BASE,
		.length = BM_EMMC_BUF_SIZE,
	},
	.ops            = {
		.read   = emmc_boot1_read_blocks,
		.write  = emmc_boot1_write_blocks,
	},
	.block_size     = EMMC_BLOCK_SIZE,
};

static const io_block_spec_t fip_in_memmap_1st_block_spec = {
	.offset = PLAT_FIP_1ST_BASE,
	.length = PLAT_FIP_1ST_SIZE
};

static const io_block_spec_t fip_in_memmap_dram_block_spec = {
	.offset = PLAT_BM_FIP_BASE,
	.length = PLAT_BM_FIP_MAX_SIZE
};

const io_block_spec_t *memmap_spec = &fip_in_memmap_1st_block_spec;

static size_t emmc_boot1_offset;

static io_block_spec_t fip_in_emmc_block_spec = {
	.offset = 0,
	.length = PLAT_BM_FIP_MAX_SIZE,
};

#if (defined(IMAGE_BL1) && defined(BL1_INIT_SPI_NOR)) || (defined(IMAGE_BL2) && defined(BL2_INIT_SPI_NOR))
static size_t spi_dmmr_boot_offset = SPIF_BASE + SPIF_FIP_OFFSET;
static io_block_spec_t fip_in_spi_dmmr_block_spec = {
	.offset = SPIF_BASE + SPIF_FIP_OFFSET,
	.length = PLAT_BM_FIP_MAX_SIZE,
};
#endif

#if (defined(IMAGE_BL1) && defined(BL1_INIT_SPI_NAND)) || (defined(IMAGE_BL2) && defined(BL2_INIT_SPI_NAND))
static io_block_spec_t fip_in_spi_nand_block_spec = {
	.offset = 0,
	.length = PLAT_BM_FIP_MAX_SIZE
};
#endif

static const io_uuid_spec_t bl2_uuid_spec = {
	.uuid = UUID_TRUSTED_BOOT_FIRMWARE_BL2,
};

static const io_uuid_spec_t bl31_uuid_spec = {
	.uuid = UUID_EL3_RUNTIME_FIRMWARE_BL31,
};

static const io_uuid_spec_t bl32_uuid_spec = {
	.uuid = UUID_SECURE_PAYLOAD_BL32,
};

static const io_uuid_spec_t bl33_uuid_spec = {
	.uuid = UUID_NON_TRUSTED_FIRMWARE_BL33,
};

#if TRUSTED_BOARD_BOOT
static const io_uuid_spec_t trusted_boot_fw_cert_uuid_spec = {
	.uuid = UUID_TRUSTED_BOOT_FW_CERT,
};

static const io_uuid_spec_t bld1_key_cert_uuid_spec = {
	.uuid = UUID_BLD1_KEY_CERT,
};

static const io_uuid_spec_t bld2_key_cert_uuid_spec = {
	.uuid = UUID_BLD2_KEY_CERT,
};

static const io_uuid_spec_t bld_content_cert_uuid_spec = {
	.uuid = UUID_BLD_CONTENT_CERT,
};

static const io_uuid_spec_t cv_trusted_key_cert_uuid_spec = {
	.uuid = UUID_CV_TRUSTED_KEY_CERT,
};

static const io_uuid_spec_t cv_non_trusted_key_cert_uuid_spec = {
	.uuid = UUID_CV_NON_TRUSTED_KEY_CERT,
};

static const io_uuid_spec_t soc_fw_key_cert_uuid_spec = {
	.uuid = UUID_SOC_FW_KEY_CERT,
};

static const io_uuid_spec_t soc_fw_content_cert_uuid_spec = {
	.uuid = UUID_SOC_FW_CONTENT_CERT,
};

static const io_uuid_spec_t trusted_os_fw_key_cert_uuid_spec = {
	.uuid = UUID_TRUSTED_OS_FW_KEY_CERT,
};

static const io_uuid_spec_t trusted_os_fw_content_cert_uuid_spec = {
	.uuid = UUID_TRUSTED_OS_FW_CONTENT_CERT,
};

static const io_uuid_spec_t bl33_key_cert_uuid_spec = {
	.uuid = UUID_BL33_KEY_CERT,
};

static const io_uuid_spec_t bl33_content_cert_uuid_spec = {
	.uuid = UUID_BL33_CONTENT_CERT,
};

static const io_uuid_spec_t trusted_fwu_cert_uuid_spec = {
	.uuid = UUID_TRUSTED_FWU_CERT,
};
#endif

static const io_uuid_spec_t bld_cert_uuid_spec = {
	.uuid = UUID_BLD,
};

static const io_uuid_spec_t blparam_cert_uuid_spec = {
	.uuid = UUID_BLPARAM,
};

static const io_uuid_spec_t ddrc_cert_uuid_spec = {
	.uuid = UUID_DDRC,
};

static const io_uuid_spec_t license_file_cert_uuid_spec = {
	.uuid = UUID_LICENSE_FILE,
};

static int open_fip(const uintptr_t spec);
static int check_fip_source(const uintptr_t spec);

struct plat_io_policy {
	uintptr_t *dev_handle;
	uintptr_t image_spec;
	int (*check)(const uintptr_t spec);
};

/* By default, ARM platforms load images from the FIP */
static struct plat_io_policy policies[] = {
	[FIP_IMAGE_ID] = {
		NULL,
		(uintptr_t)NULL,
		check_fip_source
	},
	[BL2_IMAGE_ID] = {
		&fip_dev_handle,
		(uintptr_t)&bl2_uuid_spec,
		open_fip
	},
	[BL31_IMAGE_ID] = {
		&fip_dev_handle,
		(uintptr_t)&bl31_uuid_spec,
		open_fip
	},
	[BL32_IMAGE_ID] = {
		&fip_dev_handle,
		(uintptr_t)&bl32_uuid_spec,
		open_fip
	},
	[BL33_IMAGE_ID] = {
		&fip_dev_handle,
		(uintptr_t)&bl33_uuid_spec,
		open_fip
	},
#if TRUSTED_BOARD_BOOT
	[TRUSTED_BOOT_FW_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&trusted_boot_fw_cert_uuid_spec,
		open_fip
	},
	[BLD1_KEY_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&bld1_key_cert_uuid_spec,
		open_fip
	},
	[BLD2_KEY_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&bld2_key_cert_uuid_spec,
		open_fip
	},
	[BLD_CONTENT_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&bld_content_cert_uuid_spec,
		open_fip
	},
	[CV_TRUSTED_KEY_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&cv_trusted_key_cert_uuid_spec,
		open_fip
	},
	[CV_NON_TRUSTED_KEY_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&cv_non_trusted_key_cert_uuid_spec,
		open_fip
	},
	[SOC_FW_KEY_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&soc_fw_key_cert_uuid_spec,
		open_fip
	},
	[SOC_FW_CONTENT_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&soc_fw_content_cert_uuid_spec,
		open_fip
	},
	[TRUSTED_OS_FW_KEY_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&trusted_os_fw_key_cert_uuid_spec,
		open_fip
	},
	[TRUSTED_OS_FW_CONTENT_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&trusted_os_fw_content_cert_uuid_spec,
		open_fip
	},
	[BL33_KEY_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&bl33_key_cert_uuid_spec,
		open_fip
	},
	[BL33_CONTENT_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&bl33_content_cert_uuid_spec,
		open_fip
	},
	[FWU_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&trusted_fwu_cert_uuid_spec,
		open_fip
	},
#endif

	[BLD_IMAGE_ID] = {
		&fip_dev_handle,
		(uintptr_t)&bld_cert_uuid_spec,
		open_fip
	},

	[CV_BLPARAM_IMAGE_ID] = {
		&fip_dev_handle,
		(uintptr_t)&blparam_cert_uuid_spec,
		open_fip
	},

	[CV_DDRC_IMAGE_ID] = {
		&fip_dev_handle,
		(uintptr_t)&ddrc_cert_uuid_spec,
		open_fip
	},
	[CV_LICENSE_FILE_IMAGE_ID] = {
		&fip_dev_handle,
		(uintptr_t)&license_file_cert_uuid_spec,
		open_fip
	},
};

// For loading file from sd card
static FATFS SDC_FS;
static FIL fp;

static int open_fip(const uintptr_t spec)
{
	int result;
	uintptr_t local_image_handle;

	/* See if a Firmware Image Package is available */
	result = io_dev_init(fip_dev_handle, (uintptr_t)FIP_IMAGE_ID);
	if (result == 0) {
		result = io_open(fip_dev_handle, spec, &local_image_handle);
		if (result == 0) {
			VERBOSE("Using FIP\n");
			io_close(local_image_handle);
		}
	}
	return result;
}

static int open_memmap(const uintptr_t spec)
{
	int result;
	uintptr_t local_image_handle;

	result = io_dev_init(memmap_dev_handle, (uintptr_t)NULL);
	if (result == 0) {
		result = io_open(memmap_dev_handle, spec, &local_image_handle);
		if (result == 0) {
			VERBOSE("Using Memmap\n");
			io_close(local_image_handle);
		}
	}
	return result;
}

static int open_emmc(const uintptr_t spec)
{
	int result;
	uintptr_t local_image_handle;

	result = io_dev_init(emmc_dev_handle, (uintptr_t)NULL);
	if (result == 0) {
		result = io_open(emmc_dev_handle, spec, &local_image_handle);
		if (result == 0) {
			VERBOSE("Using eMMC\n");
			io_close(local_image_handle);
		}
	}
	return result;
}

#if (defined(IMAGE_BL1) && defined(BL1_INIT_SPI_NAND)) || (defined(IMAGE_BL2) && defined(BL2_INIT_SPI_NAND))
static int open_spi_nand(const uintptr_t spec)
{
	int result;
	uintptr_t local_image_handle;

	result = io_dev_init(spi_nand_dev_handle, (uintptr_t)NULL);
	if (result == 0) {
		result = io_open(spi_nand_dev_handle, spec, &local_image_handle);
		if (result == 0) {
			VERBOSE("Using SPI NAND\n");
			io_close(local_image_handle);
		}
	}
	return result;
}
#endif

static int check_fip_source(const uintptr_t spec)
{
	int result = -ENOENT;
	int fip_src = FIP_SRC_MEMMAP;
	static int print_source = 1;

	/*
	 * change fip_src according to GPIO code.
	 * and we don't have a fail-then-try-alternative-sources design, as eMMC
	 * code now would stick in enumeration function even there is no card.
	 */
	fip_src = plat_bm_gpio_read(BIT_MASK_GPIO_BOOT_SEL);

	switch (fip_src) {
	case FIP_SRC_MEMMAP:
		result = open_memmap((uintptr_t)memmap_spec);
		if (result == 0) {
			if (print_source) {
				NOTICE("Locate FIP in memory\n");
				print_source = 0;
			}
			policies[FIP_IMAGE_ID].dev_handle = &memmap_dev_handle;
			policies[FIP_IMAGE_ID].image_spec = (uintptr_t)memmap_spec;
		}
		break;
	case FIP_SRC_EMMC:
		fip_in_emmc_block_spec.offset = emmc_boot1_offset;
		result = open_emmc((uintptr_t)&fip_in_emmc_block_spec);
		if (result == 0) {
			if (print_source) {
				NOTICE("Locate FIP in eMMC, offset 0x%lx of BOOT1\n", emmc_boot1_offset);
				print_source = 0;
			}
			policies[FIP_IMAGE_ID].dev_handle = &emmc_dev_handle;
			policies[FIP_IMAGE_ID].image_spec = (uintptr_t)&fip_in_emmc_block_spec;
		}
		break;
	case FIP_SRC_SPI_NOR:
#if (defined(IMAGE_BL1) && defined(BL1_INIT_SPI_NOR)) || (defined(IMAGE_BL2) && defined(BL2_INIT_SPI_NOR))
		fip_in_spi_dmmr_block_spec.offset = spi_dmmr_boot_offset;
		result = open_memmap((uintptr_t)&fip_in_spi_dmmr_block_spec);
		if (result == 0) {
			if (print_source) {
				NOTICE("Locate FIP in SPI flash (DMMR)\n");
				print_source = 0;
			}

			policies[FIP_IMAGE_ID].dev_handle = &memmap_dev_handle;
			policies[FIP_IMAGE_ID].image_spec = (uintptr_t)&fip_in_spi_dmmr_block_spec;
		}
#endif
		break;
#if (defined(IMAGE_BL1) && defined(BL1_INIT_SPI_NAND)) || (defined(IMAGE_BL2) && defined(BL2_INIT_SPI_NAND))
		case FIP_SRC_SPI_NAND:
			result = open_spi_nand((uintptr_t)&fip_in_spi_nand_block_spec);
			if (result == 0) {
				if (print_source) {
					NOTICE("Locate FIP in SPI NAND\n");
					print_source = 0;
				}

				policies[FIP_IMAGE_ID].dev_handle = &spi_nand_dev_handle;
				policies[FIP_IMAGE_ID].image_spec = (uintptr_t)&fip_in_spi_nand_block_spec;
			}
			break;
#endif
	default:
		ERROR("illegal fip source 0x%x\n", fip_src);
		break;
	};

	return result;
}

void plat_bm_io_setup(void)
{
	int io_result;

	io_result = register_io_dev_fip(&fip_dev_con);
	assert(io_result == 0);

	io_result = register_io_dev_memmap(&memmap_dev_con);
	assert(io_result == 0);

	io_result = register_io_dev_block(&emmc_dev_con);
	assert(io_result == 0);

#if (defined(IMAGE_BL1) && defined(BL1_INIT_SPI_NAND)) || (defined(IMAGE_BL2) && defined(BL2_INIT_SPI_NAND))
	io_result = register_io_dev_spi_nand(&spi_nand_dev_con);
	assert(io_result == 0);
#endif

	/* Open connections to devices and cache the handles */
	io_result = io_dev_open(fip_dev_con, (uintptr_t)NULL,
				&fip_dev_handle);
	assert(io_result == 0);

	io_result = io_dev_open(memmap_dev_con, (uintptr_t)NULL,
				&memmap_dev_handle);
	assert(io_result == 0);

	io_result = io_dev_open(emmc_dev_con, (uintptr_t)&emmc_dev_spec,
				&emmc_dev_handle);
	assert(io_result == 0);

#if (defined(IMAGE_BL1) && defined(BL1_INIT_SPI_NAND)) || (defined(IMAGE_BL2) && defined(BL2_INIT_SPI_NAND))
		io_result = io_dev_open(spi_nand_dev_con, (uintptr_t)NULL, &spi_nand_dev_handle);
		assert(io_result == 0);
#endif

	/* Ignore improbable errors in release builds */
	(void)io_result;
}

int plat_bm_io_load_fip_from_sd(void *buf, uint32_t max_size)
{
	UINT br;
	FRESULT f_ret;
	uint32_t length = 0;

	NOTICE("Load fip from SD...\n");

	f_ret = f_mount(&SDC_FS, "0:", 1);

	if (f_ret) {
		WARN("f_mount failed\n");
		return -1;
	}

	f_ret = f_open(&fp, "0:fip.bin", FA_READ);

	if (f_ret) {
		WARN("fip.bin not found on ");
		return -1;
	}

	length = f_size(&fp);
	NOTICE("SD: length=0x%x max_size=0x%x\n", length, max_size);
	if (length > max_size)
		length = max_size;

	f_lseek(&fp, 0);

	f_ret = f_read(&fp, (void *) buf, length, &br);

	INFO("Read file size : %d\n", br);

	return 0;
}

void bm_plat_reboot(void)
{
	rst_assert(0);
}

int plat_retry_image_source(void)
{
	int fip_src = plat_bm_gpio_read(BIT_MASK_GPIO_BOOT_SEL);

	switch (fip_src) {
	case FIP_SRC_EMMC:
		emmc_boot1_offset += EMMC_BOOT1_COPY_SIZE;
		if (emmc_boot1_offset >= EMMC_BOOT1_COPY_NUM * EMMC_BOOT1_COPY_SIZE) {
			emmc_boot1_offset = 0;
			return 0;
		} else {
			return 1;
		}
	break;
	case FIP_SRC_SPI_NOR:
		#if (defined(IMAGE_BL1) && defined(BL1_INIT_SPI_NOR)) || \
			(defined(IMAGE_BL2) && defined(BL2_INIT_SPI_NOR))
		spi_dmmr_boot_offset += PLAT_BM_FIP_MAX_SIZE;
		if (spi_dmmr_boot_offset >= (SPIF_BASE + SPIF_FIP_OFFSET + 2 * PLAT_BM_FIP_MAX_SIZE))
			return 0; // stop
		else
			return 1; // load the backup fip

		#endif
	break;
	case FIP_SRC_SPI_NAND:
		#if (defined(IMAGE_BL1) && defined(BL1_INIT_SPI_NAND)) || \
			(defined(IMAGE_BL2) && defined(BL2_INIT_SPI_NAND))

		fip_in_spi_nand_block_spec.offset += PLAT_BM_FIP_MAX_SIZE;

		if (fip_in_spi_nand_block_spec.offset >= 2 * PLAT_BM_FIP_MAX_SIZE)
			return 0;
		else
			return 1;
		#endif
		break;
	}

	return 0;
}

/*
 * Return an IO device handle and specification which can be used to access
 * an image. Use this to enforce platform load policy
 */
int plat_get_image_source(unsigned int image_id, uintptr_t *dev_handle,
			  uintptr_t *image_spec)
{
	int result;
	const struct plat_io_policy *policy;

	assert(image_id < ARRAY_SIZE(policies));

	policy = &policies[image_id];
	result = policy->check(policy->image_spec);
	if (result == 0) {
		*image_spec = policy->image_spec;
		*dev_handle = *(policy->dev_handle);
	} else {
		ERROR("No suitable image source for %d\n", image_id);
	}

	return result;
}

void plat_update_memmap_src(int is_dram)
{
	if (is_dram)
		memmap_spec = &fip_in_memmap_dram_block_spec;
	else
		memmap_spec = &fip_in_memmap_1st_block_spec;
}
