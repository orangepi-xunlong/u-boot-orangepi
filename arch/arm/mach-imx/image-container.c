// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2019 NXP
 */

#include <common.h>
#include <errno.h>
#include <log.h>
#include <malloc.h>
#include <asm/io.h>
#include <mmc.h>
#include <spi_flash.h>
#include <nand.h>
#include <asm/mach-imx/image.h>
#include <asm/arch/sys_proto.h>
#include <asm/mach-imx/boot_mode.h>

#define MMC_DEV		0
#define QSPI_DEV	1
#define NAND_DEV	2
#define QSPI_NOR_DEV	3
#define ROM_API_DEV	4

int get_container_size(ulong addr, u16 *header_length)
{
	struct container_hdr *phdr;
	struct boot_img_t *img_entry;
	struct signature_block_hdr *sign_hdr;
	u8 i = 0;
	u32 max_offset = 0, img_end;

	phdr = (struct container_hdr *)addr;
	if (phdr->tag != 0x87 && phdr->version != 0x0) {
		debug("Wrong container header\n");
		return -EFAULT;
	}

	max_offset = phdr->length_lsb + (phdr->length_msb << 8);
	if (header_length)
		*header_length = max_offset;

	img_entry = (struct boot_img_t *)(addr + sizeof(struct container_hdr));
	for (i = 0; i < phdr->num_images; i++) {
		img_end = img_entry->offset + img_entry->size;
		if (img_end > max_offset)
			max_offset = img_end;

		debug("img[%u], end = 0x%x\n", i, img_end);

		img_entry++;
	}

	if (phdr->sig_blk_offset != 0) {
		sign_hdr = (struct signature_block_hdr *)(addr + phdr->sig_blk_offset);
		u16 len = sign_hdr->length_lsb + (sign_hdr->length_msb << 8);

		if (phdr->sig_blk_offset + len > max_offset)
			max_offset = phdr->sig_blk_offset + len;

		debug("sigblk, end = 0x%x\n", phdr->sig_blk_offset + len);
	}

	return max_offset;
}

static int get_dev_container_size(void *dev, int dev_type, unsigned long offset, u16 *header_length)
{
	u8 *buf = malloc(CONTAINER_HDR_ALIGNMENT);
	int ret = 0;

	if (!buf) {
		printf("Malloc buffer failed\n");
		return -ENOMEM;
	}

#ifdef CONFIG_SPL_MMC_SUPPORT
	if (dev_type == MMC_DEV) {
		unsigned long count = 0;
		struct mmc *mmc = (struct mmc *)dev;

		count = blk_dread(mmc_get_blk_desc(mmc),
				  offset / mmc->read_bl_len,
				  CONTAINER_HDR_ALIGNMENT / mmc->read_bl_len,
				  buf);
		if (count == 0) {
			printf("Read container image from MMC/SD failed\n");
			return -EIO;
		}
	}
#endif

#ifdef CONFIG_SPL_SPI_LOAD
	if (dev_type == QSPI_DEV) {
		struct spi_flash *flash = (struct spi_flash *)dev;

		ret = spi_flash_read(flash, offset,
				     CONTAINER_HDR_ALIGNMENT, buf);
		if (ret != 0) {
			printf("Read container image from QSPI failed\n");
			return -EIO;
		}
	}
#endif

#ifdef CONFIG_SPL_NAND_SUPPORT
	if (dev_type == NAND_DEV) {
		ret = nand_spl_load_image(offset, CONTAINER_HDR_ALIGNMENT,
					  buf);
		if (ret != 0) {
			printf("Read container image from NAND failed\n");
			return -EIO;
		}
	}
#endif

#ifdef CONFIG_SPL_NOR_SUPPORT
	if (dev_type == QSPI_NOR_DEV)
		memcpy(buf, (const void *)offset, CONTAINER_HDR_ALIGNMENT);
#endif

#ifdef CONFIG_SPL_BOOTROM_SUPPORT
	if (dev_type == ROM_API_DEV) {
		ret = spl_romapi_raw_seekable_read(offset, CONTAINER_HDR_ALIGNMENT, buf);
		if (!ret) {
			printf("Read container image from ROM API failed\n");
			return -EIO;
		}
	}
#endif

	ret = get_container_size((ulong)buf, header_length);

	free(buf);

	return ret;
}

static unsigned long get_boot_device_offset(void *dev, int dev_type)
{
	unsigned long offset = 0;

	if (dev_type == MMC_DEV) {
		struct mmc *mmc = (struct mmc *)dev;

		if (IS_SD(mmc) || mmc->part_config == MMCPART_NOAVAILABLE) {
			offset = CONTAINER_HDR_MMCSD_OFFSET;
		} else {
			u8 part = EXT_CSD_EXTRACT_BOOT_PART(mmc->part_config);

			if (part == 1 || part == 2) {
				if (is_imx8qxp() && is_soc_rev(CHIP_REV_B))
					offset = CONTAINER_HDR_MMCSD_OFFSET;
				else
					offset = CONTAINER_HDR_EMMC_OFFSET;
			} else {
				offset = CONTAINER_HDR_MMCSD_OFFSET;
			}
		}
	} else if (dev_type == QSPI_DEV) {
		offset = CONTAINER_HDR_QSPI_OFFSET;
	} else if (dev_type == NAND_DEV) {
		offset = CONTAINER_HDR_NAND_OFFSET;
	} else if (dev_type == QSPI_NOR_DEV) {
		offset = CONTAINER_HDR_QSPI_OFFSET + 0x08000000;
	} else if (dev_type == ROM_API_DEV) {
		offset = (unsigned long)dev;
	}

	return offset;
}

static int get_imageset_end(void *dev, int dev_type)
{
	unsigned long offset1 = 0, offset2 = 0;
	int value_container[2];
	u16 hdr_length;

	offset1 = get_boot_device_offset(dev, dev_type);
	offset2 = CONTAINER_HDR_ALIGNMENT + offset1;

	value_container[0] = get_dev_container_size(dev, dev_type, offset1, &hdr_length);
	if (value_container[0] < 0) {
		printf("Parse seco container failed %d\n", value_container[0]);
		return value_container[0];
	}

	debug("seco container size 0x%x\n", value_container[0]);

	value_container[1] = get_dev_container_size(dev, dev_type, offset2, &hdr_length);
	if (value_container[1] < 0) {
		debug("Parse scu container failed %d, only seco container\n",
		      value_container[1]);
		/* return seco container total size */
		return value_container[0] + offset1;
	}

	debug("scu container size 0x%x\n", value_container[1]);

	return value_container[1] + offset2;
}

#ifdef CONFIG_SPL_SPI_LOAD
unsigned long spl_spi_get_uboot_offs(struct spi_flash *flash)
{
	int end;

	end = get_imageset_end(flash, QSPI_DEV);
	end = ROUND(end, SZ_1K);

	printf("Load image from QSPI 0x%x\n", end);

	return end;
}
#endif

#ifdef CONFIG_SPL_MMC_SUPPORT
unsigned long spl_mmc_get_uboot_raw_sector(struct mmc *mmc,
					   unsigned long raw_sect)
{
	int end;

	end = get_imageset_end(mmc, MMC_DEV);
	end = ROUND(end, SZ_1K);

	printf("Load image from MMC/SD 0x%x\n", end);

	return end / mmc->read_bl_len;
}
#endif

#ifdef CONFIG_SPL_NAND_SUPPORT
uint32_t spl_nand_get_uboot_raw_page(void)
{
	int end;

	end = get_imageset_end((void *)NULL, NAND_DEV);
	end = ROUND(end, SZ_16K);

	printf("Load image from NAND 0x%x\n", end);

	return end;
}
#endif

#ifdef CONFIG_SPL_NOR_SUPPORT
unsigned long spl_nor_get_uboot_base(void)
{
	int end;

	/* Calculate the image set end,
	 * if it is less than CONFIG_SYS_UBOOT_BASE(0x8281000),
	 * we use CONFIG_SYS_UBOOT_BASE
	 * Otherwise, use the calculated address
	 */
	end = get_imageset_end((void *)NULL, QSPI_NOR_DEV);
	if (end <= CONFIG_SYS_UBOOT_BASE)
		end = CONFIG_SYS_UBOOT_BASE;
	else
		end = ROUND(end, SZ_1K);

	printf("Load image from NOR 0x%x\n", end);

	return end;
}
#endif

#ifdef CONFIG_SPL_BOOTROM_SUPPORT
u32 __weak spl_arch_boot_image_offset(u32 image_offset, u32 rom_bt_dev)
{
	return image_offset;
}

ulong spl_romapi_get_uboot_base(u32 image_offset, u32 rom_bt_dev)
{
	ulong end;

	image_offset = spl_arch_boot_image_offset(image_offset, rom_bt_dev);

	end = get_imageset_end((void *)(ulong)image_offset, ROM_API_DEV);
	end = ROUND(end, SZ_1K);

	printf("Load image from 0x%lx by ROM_API\n", end);

	return end;
}
#endif
