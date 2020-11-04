// SPDX-License-Identifier: GPL-2.0+
/*
 * sunxi SPI driver for uboot.
 *
 * Copyright (C) 2018
 * 2018.11.7 wangwei <wangwei@allwinnertech.com>
 */
#include <common.h>
#include <div64.h>
#include <dm.h>
#include <malloc.h>
#include <mapmem.h>
#include <spi.h>
#include <spi_flash.h>
#include <jffs2/jffs2.h>
#include <linux/mtd/mtd.h>
#include <sunxi_board.h>
#include <private_boot0.h>
#include <sprite_verify.h>
#include <private_toc.h>
#include <private_boot0.h>
#include <private_uboot.h>
#include <asm/io.h>

#include "../flash_interface.h"


static struct spi_flash *flash;

#define SPINOR_DEBUG 0

#if SPINOR_DEBUG
#define spinor_debug(fmt, arg...)    printf("%s()%d - "fmt, __func__, __LINE__, ##arg)
#else
#define spinor_debug(fmt, arg...)
#endif


/**
 * Write a block of data to SPI flash, first checking if it is different from
 * what is already there.
 *
 * If the data being written is the same, then *skipped is incremented by len.
 *
 * @param flash		flash context pointer
 * @param offset	flash offset to write
 * @param len		number of bytes to write
 * @param buf		buffer to write from
 * @param cmp_buf	read buffer to use to compare data
 * @param skipped	Count of skipped data (incremented by this function)
 * @return NULL if OK, else a string containing the stage which failed
 */
static const char *_spi_flash_update_block(struct spi_flash *flash, u32 offset,
		size_t len, const char *buf, char *cmp_buf, size_t *skipped)
{
	char *ptr = (char *)buf;

	spinor_debug("offset=%x sector, nor_sector_size=%d bytes, len=%d bytes\n",
	      offset/flash->sector_size, flash->sector_size, len);
	/* Read the entire sector so to allow for rewriting */
	if (spi_flash_read(flash, offset, flash->sector_size, cmp_buf))
		return "read";
	/* Compare only what is meaningful (len) */
	if (memcmp(cmp_buf, buf, len) == 0) {
		spinor_debug("Skip region %x size %zx: no change\n",
		      offset, len);
		*skipped += len;
		return NULL;
	}
	/* Erase the entire sector */
	if (spi_flash_erase(flash, offset, flash->sector_size))
		return "erase";
	/* If it's a partial sector, copy the data into the temp-buffer */
	if (len != flash->sector_size) {
		memcpy(cmp_buf, buf, len);
		ptr = cmp_buf;
	}
	/* Write one complete sector */
	if (spi_flash_write(flash, offset, flash->sector_size, ptr))
		return "write";

	return NULL;
}


/**
 * Update an area of SPI flash by erasing and writing any blocks which need
 * to change. Existing blocks with the correct data are left unchanged.
 *
 * @param flash		flash context pointer
 * @param offset	flash offset to write
 * @param len		number of bytes to write
 * @param buf		buffer to write from
 * @return 0 if ok, 1 on error
 */
static int _spi_flash_update(struct spi_flash *flash, u32 offset,
		size_t len, const char *buf)
{
	const char *err_oper = NULL;
	char *cmp_buf;
	const char *end = buf + len;
	size_t todo;		/* number of bytes to do in this pass */
	size_t skipped = 0;	/* statistics */

	cmp_buf = memalign(ARCH_DMA_MINALIGN, flash->sector_size);
	if (cmp_buf) {
		for (; buf < end && !err_oper; buf += todo, offset += todo) {
			todo = min_t(size_t, end - buf, flash->sector_size);
			err_oper = _spi_flash_update_block(flash, offset, todo,
					buf, cmp_buf, &skipped);
		}
	} else {
		err_oper = "malloc";
	}
	free(cmp_buf);

	if (err_oper) {
		printf("SPI flash failed in %s step\n", err_oper);
		return 1;
	}
	return 0;
}


static int
_sunxi_flash_spinor_read(uint start_block, uint nblock, void *buffer)
{
	int ret = 0;
	u32 offset;
	u32 len;

	if(!flash)
		return 0;

	spinor_debug("start: 0x%x, len: 0x%x\n", start_block, nblock);
	offset = start_block*512;
	len = nblock*512;

	/*1 block = 512 bytes*/
	if (offset + len > flash->size) {
		printf("ERROR: attempting read past flash size \n");
		return 0;
	}
	ret = spi_flash_read(flash, offset, len, buffer);
	return ret == 0 ? nblock : 0;
}

static int
sunxi_flash_spinor_read(uint start_block, uint nblock, void *buffer)
{
	return _sunxi_flash_spinor_read(CONFIG_SPINOR_LOGICAL_OFFSET+start_block, nblock, buffer);
}

static int
sunxi_flash_spinor_phyread(uint start_block, uint nblock, void *buffer)
{
	return _sunxi_flash_spinor_read(start_block, nblock, buffer);
}



static int
_sunxi_flash_spinor_write(uint start_block, uint nblock, void *buffer)
{
	int ret = 0;
	u32 offset = start_block*512;
	u32 len = nblock<<9;
	u32 erase_size = 0;
	u32 erase_align_addr = 0;
	u32 erase_align_ofs = 0;
	u32 erase_align_size = 0;
	char * align_buf = NULL;

	if(!flash)
		return 0;

	spinor_debug("start: 0x%x, len: 0x%x\n", start_block, nblock);

	offset = start_block*512;
	len = nblock*512;
	if (offset + len > flash->size) {
		printf("ERROR: attempting write past flash size \n");
		return 0;
	}

	erase_size = flash->erase_size;
	if (offset % erase_size) {
		printf("SF: write offset not multiple of erase size\n");
		align_buf = memalign(ARCH_DMA_MINALIGN, erase_size);
		if(!align_buf) {
			printf("%s: malloc error\n", __func__);
			return 0;
		}
		erase_align_addr = (offset/erase_size)*erase_size;
		/*
		|-------|-------|---------|
		     |<----data---->|
		    offset          end
		*/
		erase_align_ofs = offset % erase_size;
		erase_align_size = erase_size - erase_align_ofs;
	
		/*read data from flash*/
		if(spi_flash_read(flash, erase_align_addr, erase_size, align_buf)) {
			spinor_debug("read error\n");
			goto __err;
		}
		/* Erase the entire sector */
		if (spi_flash_erase(flash, erase_align_addr, flash->sector_size)) {
			spinor_debug("erase error\n");
			goto __err;
		}

		/*fill data to write*/
		memcpy(align_buf + erase_align_ofs, buffer, erase_align_size);

		/* write 1 sector */
		if(spi_flash_write(flash, erase_align_addr, erase_size, align_buf)) {
			spinor_debug("write error\n");
			goto __err;
		}
		/* update info */
		len -= erase_align_size;
		offset += erase_align_size;
		buffer += erase_align_size;
	}
	if(len)
		ret = _spi_flash_update(flash, offset, len, buffer);
	return ret == 0 ? nblock : 0;

__err:
	if(align_buf)
		free(align_buf);
	return 0;
}

static int
sunxi_flash_spinor_write(uint start_block, uint nblock, void *buffer)
{
	return _sunxi_flash_spinor_write(CONFIG_SPINOR_LOGICAL_OFFSET + start_block, nblock, buffer);
}

static int
sunxi_flash_spinor_phywrite(uint start_block, uint nblock, void *buffer)
{
	return _sunxi_flash_spinor_write(start_block, nblock, buffer);
}


static int
sunxi_flash_spinor_erase(int erase, void *mbr_buffer)
{
	u32 sector_cnt = 0;
	int i = 0;

	if(!flash)
		return -1;

	if(!erase)
		return 0;

	sector_cnt = flash->size/flash->sector_size;
	printf("erase size: %d ,sector size: %d\n",flash->erase_size, flash->sector_size);
	for(i = 0; i < sector_cnt; i++) {
		if(i*flash->sector_size % (1<<20) == 0)
			printf("total %d sectors, erase index %d\n", sector_cnt, i);
		/* Erase the entire sector */
		if (spi_flash_erase(flash, i*flash->sector_size, flash->sector_size)) {
			spinor_debug("erase error\n");
			return -1;
		}
	}
	return 0;
}

static int
sunxi_flash_spinor_erase_area(uint start_block, uint nblock)
{
	int i;
	u32 sector_cnt = 0;
	u32 offset, size;

	if (!flash)
		return -1;

	/*section to byte*/
	offset = (start_block + CONFIG_SPINOR_LOGICAL_OFFSET) * 512;
	size   = nblock * 512;

	sector_cnt = size/flash->sector_size;

	for (i = 0; i < sector_cnt; i++) {
		if ((offset + (i*flash->sector_size)) % (1<<20) == 0)
			printf("total %d sectors, erase index %d\n", sector_cnt, i);
		/* Erase the entire sector */
		if (spi_flash_erase(flash, (offset + (i*flash->sector_size)), flash->sector_size)) {
			spinor_debug("erase error\n");
			return -1;
		}
	}
	return 0;
}

static uint
sunxi_flash_spinor_size(void)
{
	return flash ? flash->size/512 : 0;
}

static int
sunxi_flash_spinor_probe(void)
{
	if (spi_init())
		return -1;

	flash = spi_flash_probe(CONFIG_SF_DEFAULT_BUS, CONFIG_SF_DEFAULT_CS,
		CONFIG_SF_DEFAULT_SPEED, CONFIG_SF_DEFAULT_MODE);

	if (!flash) {
		spinor_debug("Failed to initialize SPI flash at %u:%u\n", CONFIG_SF_DEFAULT_BUS, CONFIG_SF_DEFAULT_CS);
		return -ENODEV;
	}
	set_boot_storage_type(STORAGE_NOR);

	return 0;

}

static int
sunxi_flash_spinor_init(int boot_mode, int res)
{
	spinor_debug("ENTER\n");
	if(flash)
		return 0;

	if (spi_init())
		return -1;

	flash = spi_flash_probe(CONFIG_SF_DEFAULT_BUS, CONFIG_SF_DEFAULT_CS,
		CONFIG_SF_DEFAULT_SPEED, CONFIG_SF_DEFAULT_MODE);

	if (!flash) {
		spinor_debug("Failed to initialize SPI flash at %u:%u\n", CONFIG_SF_DEFAULT_BUS, CONFIG_SF_DEFAULT_CS);
		return -ENODEV;
	}

	return 0;
}

static int
sunxi_flash_spinor_exit(int force)
{
	if(flash) {
		spinor_debug("EXIT\n");
		spi_nor_reset_device(flash);

		/*get efex cmd when finish partitions.
		  but we still need to dwonload boot0 and uboot
		  so can't free flash at this moment*/

		/*spi_flash_free(flash);
		flash = NULL;*/
	}
	return 0;
}

static int
sunxi_flash_spinor_flush(void)
{
	return 0;
}

static int
sunxi_flash_spinor_force_erase(void)
{
	return sunxi_flash_spinor_erase(1 , NULL);
}

static int
sunxi_flash_spinor_download_spl(unsigned char *buffer, int len, unsigned int ext)
{
	u8 read_cmd = flash->read_opcode;

	debug("%s read cmd: 0x%x\n",__func__, read_cmd);
	if(gd->bootfile_mode  == SUNXI_BOOT_FILE_NORMAL
		 || gd->bootfile_mode  == SUNXI_BOOT_FILE_PKG) {

		boot0_file_head_t    *boot0  = (boot0_file_head_t *)buffer;

		/* set read cmd for boot0: single/dual/quad */
		boot0->prvt_head.storage_data[0] = read_cmd;
		/* regenerate check sum */
		boot0->boot_head.check_sum = sunxi_sprite_generate_checksum(buffer,
		boot0->boot_head.length, boot0->boot_head.check_sum);
		if(sunxi_sprite_verify_checksum(buffer, boot0->boot_head.length,
			boot0->boot_head.check_sum)) {
			return -1;
		}
	} else {

		toc0_private_head_t  *toc0	 = (toc0_private_head_t *)buffer;
		sbrom_toc0_config_t  *toc0_config = NULL;

		toc0_config = (sbrom_toc0_config_t *)(buffer + 0x80);
		toc0_config->storage_data[0] = read_cmd;
		toc0->check_sum = sunxi_sprite_generate_checksum(buffer,
			toc0->length, toc0->check_sum);
		if (sunxi_sprite_verify_checksum(buffer, toc0->length,
			toc0->check_sum)) {
			debug("toc0 checksum is error\n");
			return -1;
		}
	}

	return (len/512) == _sunxi_flash_spinor_write(0, len/512, buffer) ? 0 : -1;
}

static int
sunxi_flash_spinor_download_toc(unsigned char *buffer, int len,  unsigned int ext)
{

	return (len/512) == _sunxi_flash_spinor_write(CONFIG_SPINOR_UBOOT_OFFSET, len/512, buffer) ? 0 : -1;

}


int spinor_secure_storage_read( int item, unsigned char *buf, unsigned int len)
{
	return -1;
}
int spinor_secure_storage_write(int item, unsigned char *buf, unsigned int len)
{
	return -1;
}

sunxi_flash_desc sunxi_spinor_desc =
{
	.probe = sunxi_flash_spinor_probe,
	.init = sunxi_flash_spinor_init,
	.exit = sunxi_flash_spinor_exit,
	.read = sunxi_flash_spinor_read,
	.write = sunxi_flash_spinor_write,
	.erase = sunxi_flash_spinor_erase,
	.phyread = sunxi_flash_spinor_phyread,
	.phywrite = sunxi_flash_spinor_phywrite,
	.force_erase = sunxi_flash_spinor_force_erase,
	.flush = sunxi_flash_spinor_flush,
	.size = sunxi_flash_spinor_size,
	.secstorage_read = spinor_secure_storage_read,
	.secstorage_write = spinor_secure_storage_write,
	.download_spl = sunxi_flash_spinor_download_spl,
	.download_toc = sunxi_flash_spinor_download_toc,
	.erase_area = sunxi_flash_spinor_erase_area,
};

