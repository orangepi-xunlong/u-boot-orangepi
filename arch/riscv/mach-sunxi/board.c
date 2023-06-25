// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Some init for sunxi platform.
 */

#include <common.h>
#include <mmc.h>
#include <i2c.h>
#include <serial.h>
#include <spl.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/gpio.h>
#include <asm/arch/spl.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/timer.h>
#include <asm/arch/tzpc.h>
#include <asm/arch/mmc.h>

#include <linux/compiler.h>

#include <spare_head.h>
#include <private_uboot.h>
#include <asm/arch/efuse.h>
#include <asm/arch/dram.h>
#include <asm/sections.h>

static int gpio_init(void)
{
	return 0;
}

void s_init(void)
{

#if !defined(CONFIG_ARM_CORTEX_CPU_IS_UP) && !defined(CONFIG_ARM64) \
	&& defined(CONFIG_ARM)
	/* Enable SMP mode for CPU0, by setting bit 6 of Auxiliary Ctl reg */
	asm volatile(
		"mrc p15, 0, r0, c1, c0, 1\n"
		"orr r0, r0, #1 << 6\n"
		"mcr p15, 0, r0, c1, c0, 1\n"
		::: "r0");
#endif

	clock_init();
	timer_init();
	gpio_init();

	eth_init_board();
}


/* The sunxi internal brom will try to loader external bootloader
 * from mmc0, nand flash, mmc2.
 */
uint32_t sunxi_get_boot_device(void)
{
	int boot_source;

	/*
	 * When booting from the SD card or NAND memory, the "eGON.BT0"
	 * signature is expected to be found in memory at the address 0x0004
	 * (see the "mksunxiboot" tool, which generates this header).
	 *
	 * When booting in the FEL mode over USB, this signature is patched in
	 * memory and replaced with something else by the 'fel' tool. This other
	 * signature is selected in such a way, that it can't be present in a
	 * valid bootable SD card image (because the BROM would refuse to
	 * execute the SPL in this case).
	 *
	 * This checks for the signature and if it is not found returns to
	 * the FEL code in the BROM to wait and receive the main u-boot
	 * binary over USB. If it is found, it determines where SPL was
	 * read from.
	 */
	if (!is_boot0_magic(SPL_ADDR + 4)) /* eGON.BT0 */
		return BOOT_DEVICE_BOARD;

	boot_source = readb(IOMEM_ADDR(SPL_ADDR + 0x28));
	switch (boot_source) {
	case SUNXI_BOOTED_FROM_MMC0:
		return BOOT_DEVICE_MMC1;
	case SUNXI_BOOTED_FROM_NAND:
		return BOOT_DEVICE_NAND;
	case SUNXI_BOOTED_FROM_MMC2:
		return BOOT_DEVICE_MMC2;
	case SUNXI_BOOTED_FROM_SPI:
		return BOOT_DEVICE_SPI;
	}

	panic("Unknown boot source %d\n", boot_source);
	return -1;		/* Never reached */
}

void reset_cpu(ulong addr)
{
#if defined(CONFIG_SUNXI_WDT_V2)
	static const struct sunxi_wdog *wdog = (struct sunxi_wdog *)SUNXI_WDT_BASE;
	/* Set the watchdog for its shortest interval (.5s) and wait */
	writel(((WDT_CFG_KEY << 16) | WDT_MODE_EN), IOMEM_ADDR(&wdog->srst));
	while (1) { }
#elif defined(CONFIG_SUNXI_GEN_SUN4I) || defined(CONFIG_MACH_SUN8I_R40)
	static const struct sunxi_wdog *wdog =
		 &((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->wdog;

	/* Set the watchdog for its shortest interval (.5s) and wait */
	writel(WDT_MODE_RESET_EN | WDT_MODE_EN, &wdog->mode);
	writel(WDT_CTRL_KEY | WDT_CTRL_RESTART, &wdog->ctl);

	while (1) {
		/* sun5i sometimes gets stuck without this */
		writel(WDT_MODE_RESET_EN | WDT_MODE_EN, &wdog->mode);
	}
#elif defined(CONFIG_SUNXI_GEN_SUN6I)
	static const struct sunxi_wdog *wdog =
		 ((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->wdog;

	/* Set the watchdog for its shortest interval (.5s) and wait */
	writel(WDT_CFG_RESET, &wdog->cfg);
	writel(WDT_MODE_EN, &wdog->mode);
	writel(WDT_CTRL_KEY | WDT_CTRL_RESTART, &wdog->ctl);
	while (1) { }
#elif defined(CONFIG_SUNXI_NCAT) || defined(CONFIG_SUNXI_NCAT_V2)
		static const struct sunxi_wdog *wdog =
			 ((struct sunxi_timer_reg *)SUNXI_TIMER_BASE)->wdog;

		/* Set the watchdog for its shortest interval (.5s) and wait */
		writel(WDT_CFG_RESET, IOMEM_ADDR(&wdog->cfg));
		writel(WDT_MODE_EN, IOMEM_ADDR(&wdog->mode));

		while (1) { }
#elif defined(CONFIG_SUNXI_VERSION1)
		struct sunxi_timer_reg *timer_reg = (struct sunxi_timer_reg *)SUNXI_TIMER_BASE;
		struct sunxi_wdog *wdog = &timer_reg->wdog[0];
		/* enable watchdog */
		wdog->mode |= 3;
		while (1) { }

#endif
}

void *board_fdt_blob_setup(void)
{
	/* check u-boot with dtb.bin*/
	if (fdt_check_header(&_end) == 0)
		return (void *)&_end;

	/* uboot base + 2M offset */
	return (void *)(CONFIG_SYS_TEXT_BASE + SUNXI_DTB_OFFSET);
}


#if !CONFIG_IS_ENABLED(SYS_DCACHE_OFF) && !defined(CONFIG_ARM64)
void enable_caches(void)
{
	/* Enable D-cache. I-cache is already enabled in start.S */
	dcache_enable();
}
#endif




