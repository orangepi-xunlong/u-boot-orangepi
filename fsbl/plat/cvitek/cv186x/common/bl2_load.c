#include <debug.h>
#include <bitwise_ops.h>
#include <console.h>
#include <platform.h>
#include <rom_api.h>
#include <bl2.h>
#include <ddr.h>
#include <cli.h>
#include <string.h>
#include <decompress.h>
#include <delay_timer.h>
#include <security/security.h>
#include <tempsen.h>
#include <emmc/emmc.h>
#include <pinmux/cv186x_pinmux.h>

#define ADC_BASE 0x270E0000
#define ADC_CTL 0x04
#define ADC_BUSY_STATUS 0x8
#define ADC_CYC_SET 0xc
#define ADC_RESULT3 0x1c

uint32_t adc_val_ddr_array[7];
uint32_t adc_val_ddr_array_sum = 0;
uint32_t adc_val_ddr_array_min = 4096;
uint32_t adc_val_ddr_array_max = 0;
uint32_t adc_val_ddr;
uint32_t vol_val_ddr;

struct rom_api rom_api_peri = {
	.get_boot_src = (void *)0x0000000029400020,
	.set_boot_src = (void *)0x0000000029400040,
	.load_image = (void *)0x0000000029400060,
	.flash_init = (void *)0x0000000029400080,
	.image_crc = (void *)0x00000000294000a0,
	.get_number_of_retries = (void *)0x00000000294000c0,
	.verify_rsa = (void *)0x00000000294000e0,
	.cryptodma_aes_decrypt = (void *)0x0000000029400100
};

struct rom_api rom_api_nor = {
	.get_boot_src = (void *)0x0000000005400020,
	.set_boot_src = (void *)0x0000000005400040,
	.load_image = (void *)0x0000000005400060,
	.flash_init = (void *)0x0000000005400080,
	.image_crc = (void *)0x00000000054000a0,
	.get_number_of_retries = (void *)0x00000000054000c0,
	.verify_rsa = (void *)0x00000000054000e0,
	.cryptodma_aes_decrypt = (void *)0x0000000005400100
};

struct rom_api *p_rom_api = &rom_api_peri;

//#define BL2_USE_CLI
//#define BL2_ACCESS_BT256MB //bl2 cpu access bigthan 256M byte addrspace
struct _time_records *time_records = (void *)TIME_RECORDS_ADDR;
struct fip_param1 *fip_param1 = (void *)PARAM1_BASE;
static struct fip_param2 fip_param2 __aligned(BLOCK_SIZE);
static union {
	struct ddr_param ddr_param;
	struct loader_2nd_header loader_2nd_header;
	uint8_t buf[BLOCK_SIZE];
} sram_union_buf __aligned(BLOCK_SIZE);

#define BL2_LOAD_IMAGE_SPLIT	1

void print_sram_log(void)
{
	uint32_t *const log_size = (void *)BOOT_LOG_LEN_ADDR;
	uint8_t *const log_buf = (void *)phys_to_dma(BOOT_LOG_BUF_BASE);
	uint32_t i;

	const char m1[] = "\nSRAM Log: ========================================\n";
	const char m2[] = "\nSRAM Log end: ====================================\n";

	for (i = 0; m1[i]; i++)
		console_putc(m1[i]);

	for (i = 0; i < *log_size; i++)
		console_putc(log_buf[i]);

	for (i = 0; m2[i]; i++)
		console_putc(m2[i]);
}

void rom_api_redirect(void)
{
	if (mmio_read_32(REG_TOP_CONF_INFO) & 0x00100000)
		return;

	p_rom_api = &rom_api_nor;
}

#if BL2_LOAD_IMAGE_SPLIT
#define ALIGNMENT_THRESHOLD (512 * 1024) // 512KB threshold for alignment
int bl2_load_image_split(void *buf, uint32_t offset, size_t image_size, int retry_num)
{
	unsigned int ret;
	unsigned char *split_start, *split_end;
	unsigned char *start_addr = buf;
	unsigned char *end_addr = buf + image_size;
	unsigned char *aligned_start = (unsigned char *)((size_t)start_addr & ~(ALIGNMENT_THRESHOLD - 1));
	unsigned char *aligned_end = (unsigned char *)(((size_t)end_addr + ALIGNMENT_THRESHOLD - 1)
				& ~(ALIGNMENT_THRESHOLD - 1));

	INFO("Aligned memory segments: start: %p, end: %p.\n", aligned_start, aligned_end);
	for (unsigned char *addr = aligned_start; addr < aligned_end; addr += ALIGNMENT_THRESHOLD) {
		if (addr < start_addr)
			split_start = start_addr;
		else
			split_start = addr;

		if ((addr + ALIGNMENT_THRESHOLD) > end_addr)
			split_end = end_addr;
		else
			split_end = addr + ALIGNMENT_THRESHOLD;

		INFO("Start: %p, End: %p\n", split_start, split_end);
		ret = p_rom_api->load_image(split_start, offset, split_end - split_start, retry_num);
		if (ret < 0) {
			ERROR("fail to load image split: %d.\n", ret);
			return ret;
		}
		offset += split_end - split_start;
	}

	return 0;
}
#endif

int load_param2(int retry)
{
	uint32_t crc;
	int ret = -1;

	NOTICE("P2S/0x%lx/%p.\n", sizeof(fip_param2), &fip_param2);
#if BL2_LOAD_IMAGE_SPLIT
	ret = bl2_load_image_split(&fip_param2, fip_param1->param2_loadaddr, PARAM2_SIZE, retry);
#else
	ret = p_rom_api->load_image(&fip_param2, fip_param1->param2_loadaddr, PARAM2_SIZE, retry);
#endif
	if (ret < 0) {
		return ret;
	}

	if (fip_param2.magic1 != FIP_PARAM2_MAGIC1) {
		WARN("LP2_NOMAGIC\n");
		return -1;
	}

	crc = p_rom_api->image_crc(&fip_param2.reserved1, sizeof(fip_param2) - 12);
	if (crc != fip_param2.param2_cksum) {
		ERROR("param2_cksum (0x%x/0x%x)\n", crc, fip_param2.param2_cksum);
		return -1;
	}

	NOTICE("P2E.\n");

	return 0;
}

int load_ddr_param(int retry)
{
	return 0;
}

int load_ddr(void)
{
	int retry = 0;

retry_from_flash:
	for (retry = 0; retry < p_rom_api->get_number_of_retries(); retry++) {
		if (load_param2(retry) < 0)
			continue;

		if (load_ddr_param(retry) < 0)
			continue;

		break;
	}

	if (retry >= p_rom_api->get_number_of_retries()) {
		switch (p_rom_api->get_boot_src()) {
		case BOOT_SRC_UART:
		case BOOT_SRC_SD:
		case BOOT_SRC_USB:
			WARN("DL cancelled. Load flash. (%d).\n", retry);
			// Continue to boot from flash if boot from external source
			p_rom_api->flash_init();
			goto retry_from_flash;
		default:
			ERROR("Failed to load DDR param (%d).\n", retry);
			plat_panic_handler();
		}
	}

	time_records->ddr_init_start = read_time_ms();
	VERBOSE("\n#ddr_int_start at %d ms#\n", time_records->ddr_init_start);
	// read adc3 value
	PINMUX_CONFIG(ADC3, ADC3, G12);         // gpio3_26
	// mmio_clrbits_32(0x28104c34, 1 << 14);
	mmio_write_32(ADC_BASE + ADC_CTL, 0x80);
	mmio_clrsetbits_32(ADC_BASE + ADC_CYC_SET, 0xf000, 0xf << 12);
	mmio_clrsetbits_32(ADC_BASE + ADC_CYC_SET, 0xf000, 0x2 << 12);
	mmio_setbits_32(ADC_BASE + ADC_CTL, 0x01);
	mdelay(10);
	for(int i = 0 ; i < 7 ; i++){
		// check adc busy
		while(mmio_read_32(ADC_BASE + ADC_BUSY_STATUS) & 0x1);
		adc_val_ddr_array[i] = mmio_read_32(ADC_BASE + ADC_RESULT3) & 0xfff;
		adc_val_ddr_array_sum += adc_val_ddr_array[i];
		adc_val_ddr_array_min = (adc_val_ddr_array_min < adc_val_ddr_array[i] ? adc_val_ddr_array_min : adc_val_ddr_array[i]);
		adc_val_ddr_array_max = (adc_val_ddr_array_max > adc_val_ddr_array[i] ? adc_val_ddr_array_max : adc_val_ddr_array[i]);
	}
	adc_val_ddr = (adc_val_ddr_array_sum - adc_val_ddr_array_min - adc_val_ddr_array_max)/5;
	vol_val_ddr = (adc_val_ddr * 1500) / 4096;

#ifndef CONFIG_BOARD_fpga
#ifdef BL2_ACCESS_BT256MB
	disable_mmu_el3();//close mmu
#endif
	ddr_init(&sram_union_buf.ddr_param);
#ifdef  BL2_USE_CLI
	tempsen_init();
	cli_simple_loop(1);
#endif
	#endif
	time_records->ddr_init_end = read_time_ms();
	VERBOSE("\n#ddr_int_end at %d ms#\n", time_records->ddr_init_end);
	return 0;
}

int load_blcp_2nd(int retry)
{
	uint32_t crc, rtos_base;
	int ret = -1;

	// if no blcp_2nd, release_blcp_2nd should be ddr_init_end

	NOTICE("C2S/0x%x/0x%x/0x%x.\n", fip_param2.blcp_2nd_loadaddr, fip_param2.blcp_2nd_runaddr,
	       fip_param2.blcp_2nd_size);

	if (!fip_param2.blcp_2nd_runaddr) {
		NOTICE("No C906L image.\n");
		return 0;
	}

	if (!IN_RANGE(fip_param2.blcp_2nd_runaddr, DRAM_BASE, DRAM_SIZE)) {
		ERROR("blcp_2nd_runaddr (0x%x) is not in DRAM.\n", fip_param2.blcp_2nd_runaddr);
		plat_panic_handler();
	}

	if (!IN_RANGE(fip_param2.blcp_2nd_runaddr + fip_param2.blcp_2nd_size, DRAM_BASE, DRAM_SIZE)) {
		ERROR("blcp_2nd_size (0x%x) is not in DRAM.\n", fip_param2.blcp_2nd_size);
		plat_panic_handler();
	}

// #ifdef USB_DL_BY_FSBL
// 	if (p_rom_api->get_boot_src() == BOOT_SRC_USB)
// 		ret = load_image_by_usb((void *)(uintptr_t)fip_param2.blcp_2nd_runaddr, fip_param2.blcp_2nd_loadaddr,
// 					fip_param2.blcp_2nd_size, retry);
// 	else
// #endif
#if BL2_LOAD_IMAGE_SPLIT
	ret = bl2_load_image_split((void *)(uintptr_t)fip_param2.blcp_2nd_runaddr, fip_param2.blcp_2nd_loadaddr,
				   fip_param2.blcp_2nd_size, retry);
#else
	ret = p_rom_api->load_image((void *)(uintptr_t)fip_param2.blcp_2nd_runaddr, fip_param2.blcp_2nd_loadaddr,
				   fip_param2.blcp_2nd_size, retry);
#endif
	if (ret < 0) {
		return ret;
	}

	crc = p_rom_api->image_crc((void *)(uintptr_t)fip_param2.blcp_2nd_runaddr, fip_param2.blcp_2nd_size);
	if (crc != fip_param2.blcp_2nd_cksum) {
		ERROR("blcp_2nd_cksum (0x%x/0x%x)\n", crc, fip_param2.blcp_2nd_cksum);
		return -1;
	}

	ret = dec_verify_image((void *)(uintptr_t)fip_param2.blcp_2nd_runaddr, fip_param2.blcp_2nd_size, 0, fip_param1);
	if (ret < 0) {
		ERROR("verify blcp 2nd (%d)\n", ret);
		return ret;
	}

	flush_dcache_range(fip_param2.blcp_2nd_runaddr, fip_param2.blcp_2nd_size);

	rtos_base = mmio_read_32(AXI_SRAM_RTOS_BASE);
	init_comm_info();

	if (rtos_base == CVI_RTOS_MAGIC_CODE) {
		mmio_write_32(AXI_SRAM_RTOS_BASE, fip_param2.blcp_2nd_runaddr);
	} else {
		reset_c906l(fip_param2.blcp_2nd_runaddr);
	}

	NOTICE("C2E.\n");
	return 0;
}

int load_monitor(int retry, uint64_t *monitor_entry)
{
	uint32_t crc;
	int ret = -1;

	NOTICE("MS/0x%lx/0x%lx/0x%x.\n", fip_param2.monitor_loadaddr, fip_param2.monitor_runaddr,
	       fip_param2.monitor_size);

	if (!fip_param2.monitor_runaddr) {
		NOTICE("No monitor.\n");
		return 0;
	}

	if (!IN_RANGE(fip_param2.monitor_runaddr, DRAM_BASE, DRAM_SIZE)) {
		ERROR("monitor_runaddr (0x%lx) is not in DRAM.\n", fip_param2.monitor_runaddr);
		plat_panic_handler();
	}

	if (!IN_RANGE(fip_param2.monitor_runaddr + fip_param2.monitor_size, DRAM_BASE, DRAM_SIZE)) {
		ERROR("monitor_size (0x%x) is not in DRAM.\n", fip_param2.monitor_size);
		plat_panic_handler();
	}

// #ifdef USB_DL_BY_FSBL
// 	if (p_rom_api->get_boot_src() == BOOT_SRC_USB)
// 		ret = load_image_by_usb((void *)(uintptr_t)fip_param2.monitor_runaddr, fip_param2.monitor_loadaddr,
// 					fip_param2.monitor_size, retry);
// 	else
// #endif
#if BL2_LOAD_IMAGE_SPLIT
	ret = bl2_load_image_split((void *)(uintptr_t)fip_param2.monitor_runaddr, fip_param2.monitor_loadaddr,
				   fip_param2.monitor_size, retry);
#else
	ret = p_rom_api->load_image((void *)(uintptr_t)fip_param2.monitor_runaddr, fip_param2.monitor_loadaddr,
				   fip_param2.monitor_size, retry);
#endif
	if (ret < 0) {
		return ret;
	}

	crc = p_rom_api->image_crc((void *)(uintptr_t)fip_param2.monitor_runaddr, fip_param2.monitor_size);
	if (crc != fip_param2.monitor_cksum) {
		ERROR("monitor_cksum (0x%x/0x%x)\n", crc, fip_param2.monitor_cksum);
		return -1;
	}

	ret = dec_verify_image((void *)(uintptr_t)fip_param2.monitor_runaddr, fip_param2.monitor_size, 0, fip_param1);
	if (ret < 0) {
		ERROR("verify monitor (%d)\n", ret);
		return ret;
	}

	flush_dcache_range(fip_param2.monitor_runaddr, fip_param2.monitor_size);
	NOTICE("ME.\n");

	*monitor_entry = fip_param2.monitor_runaddr;

	return 0;
}

int load_bl32(int retry)
{
	uint32_t crc;
	int ret = -1;

	NOTICE("BL32/0x%lx/0x%lx/0x%x.\n", fip_param2.bl32_loadaddr, fip_param2.bl32_runaddr,
	       fip_param2.bl32_size);

	if (!fip_param2.bl32_runaddr) {
		NOTICE("No monitor.\n");
		return 0;
	}

	if (!IN_RANGE(fip_param2.bl32_runaddr, DRAM_BASE, DRAM_SIZE)) {
		ERROR("bl32_runaddr (0x%lx) is not in DRAM.\n", fip_param2.bl32_runaddr);
		plat_panic_handler();
	}

	if (!IN_RANGE(fip_param2.bl32_runaddr + fip_param2.bl32_size, DRAM_BASE, DRAM_SIZE)) {
		ERROR("bl32_size (0x%x) is not in DRAM.\n", fip_param2.bl32_size);
		plat_panic_handler();
	}

// #ifdef USB_DL_BY_FSBL
// 	if (p_rom_api->get_boot_src() == BOOT_SRC_USB)
// 		ret = load_image_by_usb((void *)(uintptr_t)fip_param2.bl32_runaddr, fip_param2.monitor_loadaddr,
// 					fip_param2.bl32_size, retry);
// 	else
// #endif
#if BL2_LOAD_IMAGE_SPLIT
	ret = bl2_load_image_split((void *)(uintptr_t)fip_param2.bl32_runaddr, fip_param2.bl32_loadaddr,
				   fip_param2.bl32_size, retry);
#else
	ret = p_rom_api->load_image((void *)(uintptr_t)fip_param2.bl32_runaddr, fip_param2.bl32_loadaddr,
				   fip_param2.bl32_size, retry);
#endif
	if (ret < 0) {
		return ret;
	}

	crc = p_rom_api->image_crc((void *)(uintptr_t)fip_param2.bl32_runaddr, fip_param2.bl32_size);
	if (crc != fip_param2.bl32_cksum) {
		ERROR("monitor_cksum (0x%x/0x%x)\n", crc, fip_param2.bl32_cksum);
		return -1;
	}

	ret = dec_verify_image((void *)(uintptr_t)fip_param2.bl32_runaddr, fip_param2.bl32_size, 0, fip_param1);
	if (ret < 0) {
		ERROR("verify monitor (%d)\n", ret);
		return ret;
	}

	flush_dcache_range(fip_param2.bl32_runaddr, fip_param2.bl32_size);
	NOTICE("BL32.E.\n");

	return 0;
}

int load_blmcu(int retry)
{
	uint32_t crc;
	int ret = -1;

	NOTICE("BLMCU/0x%lx/0x%lx/0x%x.\n", fip_param2.blmcu_loadaddr, fip_param2.blmcu_runaddr,
	       fip_param2.blmcu_size);

	if (!fip_param2.blmcu_runaddr) {
		NOTICE("No blmcu.\n");
		return 0;
	}
	
#if BL2_LOAD_IMAGE_SPLIT
	ret = bl2_load_image_split((void *)(uintptr_t)fip_param2.blmcu_runaddr, fip_param2.blmcu_loadaddr,
				   fip_param2.blmcu_size, retry);
#else
	ret = p_rom_api->load_image((void *)(uintptr_t)fip_param2.blmcu_runaddr, fip_param2.blmcu_loadaddr,
				   fip_param2.blmcu_size, retry);
#endif
	if (ret < 0) {
		return ret;
	}

	crc = p_rom_api->image_crc((void *)(uintptr_t)fip_param2.blmcu_runaddr, fip_param2.blmcu_size);
	if (crc != fip_param2.blmcu_cksum) {
		ERROR("blmcu_cksum (0x%x/0x%x)\n", crc, fip_param2.blmcu_cksum);
		return -1;
	}

	ret = dec_verify_image((void *)(uintptr_t)fip_param2.blmcu_runaddr, fip_param2.blmcu_size, 0, fip_param1);
	if (ret < 0) {
		ERROR("verify blmcu (%d)\n", ret);
		return ret;
	}

	mmio_write_32(0x28100248, 0x1);
	mmio_write_32(0x05025020, 0x5200080);
	mmio_write_32(0x05025024, 0x5200000);

	time_records->blmcu_start = read_time_ms();
	VERBOSE("\n#blmcu start at %dms#\n", time_records->blmcu_start);
	NOTICE("BLMCU.E.\n");

	return 0;
}

int load_loader_2nd(int retry, uint64_t *loader_2nd_entry)
{
	struct loader_2nd_header *loader_2nd_header = &sram_union_buf.loader_2nd_header;
	uint32_t crc;
	int ret = -1;
	const int cksum_offset =
		offsetof(struct loader_2nd_header, cksum) + sizeof(((struct loader_2nd_header *)0)->cksum);

	enum COMPRESS_TYPE comp_type = COMP_NONE;
	int reading_size;
	void *image_buf;

	NOTICE("L2/0x%lx.\n", fip_param2.loader_2nd_loadaddr);

// #ifdef USB_DL_BY_FSBL
// 	if (p_rom_api->get_boot_src() == BOOT_SRC_USB)
// 		ret = load_image_by_usb(loader_2nd_header, fip_param2.loader_2nd_loadaddr, BLOCK_SIZE, retry);
// 	else
// #endif
#if BL2_LOAD_IMAGE_SPLIT
	ret = bl2_load_image_split(loader_2nd_header, fip_param2.loader_2nd_loadaddr, BLOCK_SIZE, retry);
#else
	ret = p_rom_api->load_image(loader_2nd_header, fip_param2.loader_2nd_loadaddr, BLOCK_SIZE, retry);
#endif
	if (ret < 0) {
		return ret;
	}

	reading_size = ROUND_UP(loader_2nd_header->size, BLOCK_SIZE);

	NOTICE("L2/0x%x/0x%x/0x%lx/0x%x/0x%x\n", loader_2nd_header->magic, loader_2nd_header->cksum,
	       loader_2nd_header->runaddr, loader_2nd_header->size, reading_size);

	switch (loader_2nd_header->magic) {
	case LOADER_2ND_MAGIC_LZMA:
		comp_type = COMP_LZMA;
		break;
	case LOADER_2ND_MAGIC_LZ4:
		comp_type = COMP_LZ4;
		break;
	default:
		comp_type = COMP_NONE;
		break;
	}

	if (comp_type) {
		NOTICE("COMP/%d.\n", comp_type);
		image_buf = (void *)DECOMP_BUF_ADDR;
	} else {
		image_buf = (void *)loader_2nd_header->runaddr;
	}

// #ifdef USB_DL_BY_FSBL
// 	if (p_rom_api->get_boot_src() == BOOT_SRC_USB)
// 		ret = load_image_by_usb(image_buf, fip_param2.loader_2nd_loadaddr, reading_size, retry);
// 	else
// #endif
#if BL2_LOAD_IMAGE_SPLIT
	ret = bl2_load_image_split(image_buf, fip_param2.loader_2nd_loadaddr, reading_size, retry);
#else
	ret = p_rom_api->load_image(image_buf, fip_param2.loader_2nd_loadaddr, reading_size, retry);
#endif
	if (ret < 0) {
		return ret;
	}

	crc = p_rom_api->image_crc(image_buf + cksum_offset, loader_2nd_header->size - cksum_offset);
	if (crc != loader_2nd_header->cksum) {
		ERROR("loader_2nd_cksum (0x%x/0x%x)\n", crc, loader_2nd_header->cksum);
		return -1;
	}

	ret = dec_verify_image(image_buf + cksum_offset, loader_2nd_header->size - cksum_offset,
			       sizeof(struct loader_2nd_header) - cksum_offset, fip_param1);
	if (ret < 0) {
		ERROR("verify loader 2nd (%d)\n", ret);
		return ret;
	}


	sys_switch_all_to_pll();

	if (comp_type) {
		size_t dst_size = DECOMP_DST_SIZE;

		// header is not compressed.
		void *dst = (void *)loader_2nd_header->runaddr;

		memcpy(dst, image_buf, sizeof(struct loader_2nd_header));
		image_buf += sizeof(struct loader_2nd_header);

		ret = decompress(dst + sizeof(struct loader_2nd_header), &dst_size, image_buf, loader_2nd_header->size,
				 comp_type);
		if (ret < 0) {
			ERROR("Failed to decompress loader_2nd (%d/%lu)\n", ret, dst_size);
			return -1;
		}

		reading_size = dst_size;
	}

	flush_dcache_range(loader_2nd_header->runaddr, reading_size);
	NOTICE("Loader_2nd loaded.\n");

	*loader_2nd_entry = loader_2nd_header->runaddr + sizeof(struct loader_2nd_header);

	return 0;
}

//#ifdef	BOOT_FROM_EMMC
#define OEM_INFO_MAX_BYTE_SIZE	256
int load_oem_info(void)
{
	static int is_emmc_init = 0;
	int ret;
	char oem_info[EMMC_BLOCK_SIZE] __attribute__((aligned(EMMC_BLOCK_SIZE)));
	uint8_t dram_size_GB = 0;

	if (p_rom_api->get_boot_src() == BOOT_SRC_EMMC && is_emmc_init == 0) {
		bm_emmc_init();
		is_emmc_init = 1;
	}

	/* OEM information is save in eMMC BOOT2 partition
	 * @para buf: Must be aligned EMMC_BLOCK_SIZE
	 * @para size: Must be greater than 512
	 */
	if (p_rom_api->get_boot_src() == BOOT_SRC_EMMC) {
		ret = emmc_boot2_read_blocks(0, (uintptr_t)oem_info, EMMC_BLOCK_SIZE);
		if (ret < 0)
			return -1;
	}

	/* Write ddr size into OEM */
	dram_size_GB = (uint8_t)((usys0_cap_in_mbyte + usys1_cap_in_mbyte) / 1024);

	if (p_rom_api->get_boot_src() == BOOT_SRC_EMMC && dram_size_GB != ((uint8_t)oem_info[0xF0])) {
		oem_info[0xF0] = (dram_size_GB & 0xFF);
		NOTICE("Rewrite ddr size into OEM, ddr_size = %d GB\n", oem_info[0xF0]);
		ret = emmc_boot2_write_blocks(0, (uintptr_t)oem_info, EMMC_BLOCK_SIZE);
		if (ret < 0)
			return -1;
	} else
		oem_info[0xF0] = (dram_size_GB & 0xFF);

	printf("===================dram_size_GB = %d\n", dram_size_GB);

	/* Write OEM information to the last OEM_INFO_MAX_BYTE_SIZE bytes of RTC SRAM */
	for (int i = 0; i < OEM_INFO_MAX_BYTE_SIZE; i ++)
		mmio_write_8(OEM_INFO_RTC_SRAM_ADDR+i, oem_info[i]);

	NOTICE("Load OEM info end.\n");

// #define FSBL_PRINT_OEM_INFORMATION
#ifdef FSBL_PRINT_OEM_INFORMATION
	NOTICE("OEM info: \n");
	NOTICE("  SN0	: %s.\n", &oem_info[0x00]);	//SN0
	NOTICE("  SN1	: %s.\n", &oem_info[0x20]);	//SN1
	NOTICE("  MAC0	: %02x:%02x:%02x:%02x:%02x:%02x\n", 
			oem_info[0x40],oem_info[0x41],oem_info[0x42],oem_info[0x43],oem_info[0x44],oem_info[0x45]);	//MAC0
	NOTICE("  MAC1	: %02x:%02x:%02x:%02x:%02x:%02x\n", 
			oem_info[0x50],oem_info[0x51],oem_info[0x52],oem_info[0x53],oem_info[0x54],oem_info[0x55]);	//MAC1
	NOTICE("  product type	: %s.\n", &oem_info[0x60]);	//product type
	NOTICE("  module type	: %s.\n", &oem_info[0x70]);	//module type
	NOTICE("  inter flag	: %d.\n", oem_info[0x80]);	//interface flag
	NOTICE("  aging flag	: %d.\n", oem_info[0x81]);	//aging flag
	NOTICE("  vendor	: %s.\n", &oem_info[0x90]);		//vendor
	NOTICE("  dts type	: %s.\n", &oem_info[0xA0]);		//dts type
	NOTICE("  hw version	: %s.\n", &oem_info[0xC0]);	//hw version
	NOTICE("  produce	: %s.\n", &oem_info[0xD0]);		//product
	NOTICE("  chip		: %s.\n", &oem_info[0xE0]);		//chip
	NOTICE("  ddr size	: %s.\n", &oem_info[0xF0]);		//ddr size

	/* readback check*/
	for (int i = 0; i < OEM_INFO_MAX_BYTE_SIZE; i ++) {
		if (oem_info[i] != mmio_read_8(OEM_INFO_RTC_SRAM_ADDR+i)) {
			ERROR("cmp err %d. \n", i);
			break;
		}
	}
	NOTICE("OEM info readback check OK.\n");
#endif
	NOTICE("  ddr size	: %s.\n", &oem_info[0xF0]);		//ddr size
	return 0;
}
//#endif

static int load_param1(int retry)
{
       uint32_t fip_param1_size = PARAM1_SIZE;
       int ret = 0;

       ret = p_rom_api->load_image(fip_param1, 0, fip_param1_size, retry);
       if (ret < 0) {
               ERROR("load param1 (%d)\n", ret);
               return ret;
       }

       if (fip_param1->magic1 != FIP_PARAM1_MAGIC1) {
               ERROR("PARAM1 magic (0x%lx)\n", fip_param1->magic1);
               return -1;
       }

       return ret;
}

void fip_src_check(void)
{
	int retry = 0;

	if (p_rom_api->get_boot_src() != BOOT_SRC_RTC_NOR)
		return;

	mmio_setbits_32(PCIE_BOOT_REG, 0x3 << 16);
	NOTICE("Waiting for boot image in position\n");
	while (1) {
		feed_dog();
		if(mmio_read_32(PCIE_BOOT_REG) & 0x10) {
			NOTICE("The image is in position\n");
			mmio_clrbits_32(PCIE_BOOT_REG, 0x10010);
			p_rom_api->set_boot_src(BOOT_SRC_PCIE);

			inv_dcache_range(0x102000000, 0x80000);

			for (retry = 0; retry < p_rom_api->get_number_of_retries(); retry++) {
				if (load_param1(retry) < 0)
					continue;
				if (load_param2(retry) < 0)
					continue;
				break;
			}
			break;
		}
		mdelay(100);
	}
}

int load_rest(void)
{
	int retry = 0;

	uint64_t monitor_entry = 0;
	uint64_t loader_2nd_entry = 0;

	fip_src_check();

	// Init sys PLL and switch clocks to PLL
	sys_pll_init();

retry_from_flash:
	for (retry = 0; retry < p_rom_api->get_number_of_retries(); retry++) {
		mmio_write_32(0x05025018, 0x1ffffd);
//#ifdef	BOOT_FROM_EMMC
		if (p_rom_api->get_boot_src() != BOOT_SRC_PCIE)
			if (load_oem_info() < 0)
				ERROR("Fail to load OEM info.\n");
//#endif

		if (load_blcp_2nd(retry) < 0)
			continue;

		if (load_monitor(retry, &monitor_entry) < 0)
			continue;

		if(load_bl32(retry) < 0)
			continue;

		if (load_blmcu(retry) < 0)
			continue;

		if (load_loader_2nd(retry, &loader_2nd_entry) < 0)
			continue;

		//MCU reset shoule deassert after image loaded from nor flash
		mmio_write_32(0x05025018, 0x1fffff);
		break;
	}

	if (retry >= p_rom_api->get_number_of_retries()) {
		switch (p_rom_api->get_boot_src()) {
		case BOOT_SRC_UART:
		case BOOT_SRC_SD:
		case BOOT_SRC_USB:
			WARN("DL cancelled. Load flash. (%d).\n", retry);
			// Continue to boot from flash if boot from external source
			p_rom_api->flash_init();
			goto retry_from_flash;
		default:
			ERROR("Failed to load rest (%d).\n", retry);
			plat_panic_handler();
		}
	}

	otp_enter_power_save_mode();

	sync_cache();
	console_flush();
	//switch_rtc_mode_2nd_stage();
	//NOTICE("load_rest \n");
	// #ifndef CONFIG_BOARD_fpga
	// monitor_entry = 0x100000000;
	// loader_2nd_entry = 0x100040000;
	// #endif

	feed_dog();

	NOTICE("M/%lx/N/%lx/",monitor_entry,loader_2nd_entry);
	if (monitor_entry) {
		INFO("From BL2 jump to BL31 at 0x%lx.\n", monitor_entry);
		jump_to_monitor(monitor_entry, loader_2nd_entry);
	} else {
		INFO("Jump to loader_2nd at 0x%lx.\n", loader_2nd_entry);
		jump_to_loader_2nd(loader_2nd_entry);
	}

	return 0;
}
