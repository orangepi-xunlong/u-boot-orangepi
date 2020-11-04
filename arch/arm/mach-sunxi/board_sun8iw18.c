/*
 * Allwinner Sun50iw3 clock register definitions
 *
 * (C) Copyright 2017  <wangwei@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <asm/arch/timer.h>


#define SIZE_128B  (128)
#define SIZE_256B  (256)
#define SIZE_512B  (512)
#define SIZE_640B  (640)
#define SIZE_1K    (1024)
#define SIZE_2K    (2*SIZE_1K)
#define SIZE_4K    (4*SIZE_1K)
#define SIZE_8K    (8*SIZE_1K)
#define SIZE_16K   (16*SIZE_1K)
#define SIZE_64K   (64*SIZE_1K)
#define SIZE_128K  (128*SIZE_1K)
#define SIZE_1M  (1024*SIZE_1K)

struct mem_item {
	u32 start;
	u32 size;
};

int mem_mapping_check(u32 addr)
{
	struct mem_item mem_map[] = {
		{ SUNXI_SRAM_A1_BASE, SIZE_128K },
		{ SUNXI_CCM_BASE, SIZE_4K },
		{ SUNXI_DMA_BASE, SIZE_4K },
		{ SUNXI_SID_BASE, SIZE_640B },
		{ SUNXI_TIMER_BASE, SIZE_1K },
		{ SUNXI_PIO_BASE, SIZE_1K },
		{ SUNXI_PSI_BASE, SIZE_1K },
		{ SUNXI_GIC400_BASE, SIZE_64K },
		{ SUNXI_RTC_BASE, SIZE_1K },
		{ SUNXI_DRAM_COM_BASE, SIZE_16K },
		{ SUNXI_NFC_BASE, SIZE_4K },
		{ SUNXI_MMC1_BASE, SIZE_4K },
		{ SUNXI_UART0_BASE, SIZE_1K },
		{ SUNXI_UART1_BASE, SIZE_1K },
		{ SUNXI_UART2_BASE, SIZE_1K },
		{ SUNXI_UART3_BASE, SIZE_1K },
		{ SUNXI_TWI0_BASE, SIZE_1K },
		{ SUNXI_TWI1_BASE, SIZE_1K },
		{ SUNXI_SPI0_BASE, SIZE_4K },
		{ SUNXI_SPI1_BASE, SIZE_4K },
		{ SUNXI_USB0_BASE, SIZE_1M },
		{ SUNXI_SS_BASE, SIZE_4K },
		{ 0x03000000, SIZE_4K },
		{ 0x06700000, SIZE_1K }, /*LEDC*/
		{ 0x03007000, SIZE_1K }, /*SMC*/
		{ 0x03008000, SIZE_1K }, /*SPC*/
	};

	u32 i = 0;

	if (addr >= 0x40000000)
		return 1;

	for (i = 0; i < ARRAY_SIZE(mem_map); i++) {
		if ((addr >= mem_map[i].start) &&
		    (addr < mem_map[i].start + mem_map[i].size)) {
			return 1;
		}
	}

	return 0;
}
