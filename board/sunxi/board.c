// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2012-2013 Henrik Nordstrom <henrik@henriknordstrom.net>
 * (C) Copyright 2013 Luke Kenneth Casson Leighton <lkcl@lkcl.net>
 *
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Some board init for the Allwinner A10-evb board.
 */

#include <common.h>
#include <mmc.h>
#include <axp_pmic.h>
#include <asm/arch/clock.h>
#include <asm/arch/cpu.h>
#include <asm/arch/display.h>
#include <asm/arch/dram.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mmc.h>
#include <asm/arch/spl.h>
#include <asm/arch/usb_phy.h>
#ifdef CONFIG_ARM
#include <asm/setup.h>
#ifndef CONFIG_ARM64
#include <asm/armv7.h>
#endif
#endif
#include <asm/gpio.h>
#include <asm/io.h>
#include <crc.h>
#include <environment.h>
#include <linux/libfdt.h>
#include <nand.h>
#include <net.h>
#include <spl.h>
#include <sy8106a.h>
#include <private_uboot.h>
#include <sys_config.h>
#include <sunxi_board.h>
#ifdef CONFIG_SUNXI_POWER
#include <sunxi_power/axp.h>
#include <sunxi_power/power_manage.h>
#endif
#ifdef CONFIG_SUNXI_DMA
#include <asm/arch/dma.h>
#endif
#include <mapmem.h>
#include <smc.h>


int  __attribute__((weak)) sunxi_set_sramc_mode(void)
{
	return 0;
}

int  __attribute__((weak)) clock_set_corepll(int frequency)
{
	return 0;
}

void  __attribute__((weak)) rtc_set_vccio_det_spare(void)
{
	return ;
}

int  __attribute__((weak)) rtc_set_dcxo_off(void)
{
	return 0;
}

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SUNXI_OVERLAY
__weak int sunxi_overlay_apply_merged(void *dtb_base, void *dtbo_base)
{
	return fdt_check_header(dtbo_base);
}

#endif

void i2c_init_board(void)
{
	__maybe_unused char fdt_node_str[8] = {0};
#ifdef CONFIG_I2C0_ENABLE
#if defined(CONFIG_MACH_SUN8IW18)
	#if 0 /*twi0 & uart0 use the same pin*/
	sunxi_gpio_set_cfgpin(SUNXI_GPH(0), SUN8I_GPH_TWI0);
	sunxi_gpio_set_cfgpin(SUNXI_GPH(1), SUN8I_GPH_TWI0);
	clock_twi_onoff(0,1);
	#endif
#else
	sprintf(fdt_node_str, "twi0");
	fdt_set_all_pin(fdt_node_str, "pinctrl-0");
#endif
#endif

#ifdef CONFIG_I2C1_ENABLE
#if defined(CONFIG_MACH_SUN8IW18)
	sunxi_gpio_set_cfgpin(SUNXI_GPH(2), SUN8I_GPH_TWI1);
	sunxi_gpio_set_cfgpin(SUNXI_GPH(3), SUN8I_GPH_TWI1);
	/*clock_twi_onoff(1, 1);*/
#else
	sprintf(fdt_node_str, "twi1");
	fdt_set_all_pin(fdt_node_str, "pinctrl-0");
#endif
#endif

#ifdef CONFIG_I2C2_ENABLE
	sprintf(fdt_node_str, "twi2");
	fdt_set_all_pin(fdt_node_str, "pinctrl-0");
#endif

#ifdef CONFIG_I2C3_ENABLE
	sprintf(fdt_node_str, "twi3");
	fdt_set_all_pin(fdt_node_str, "pinctrl-0");
#endif

#ifdef CONFIG_I2C4_ENABLE
	sprintf(fdt_node_str, "twi4");
	fdt_set_all_pin(fdt_node_str, "pinctrl-0");
#endif

#ifdef CONFIG_I2C5_ENABLE
	sprintf(fdt_node_str, "twi5");
	fdt_set_all_pin(fdt_node_str, "pinctrl-0");
#endif

#ifdef CONFIG_R_I2C0_ENABLE
#if defined(CONFIG_MACH_SUN50IW11)
	sunxi_gpio_set_cfgpin(SUNXI_GPL(5), SUN8I_H3_GPL_R_TWI);
	sunxi_gpio_set_cfgpin(SUNXI_GPL(6), SUN8I_H3_GPL_R_TWI);
#else
#if defined(CONFIG_MACH_SUN50IW9)
	sprintf(fdt_node_str, "twi5");
#elif defined(CONFIG_MACH_SUN50IW10)
	sprintf(fdt_node_str, "twi6");
#else
	sprintf(fdt_node_str, "twi4");
#endif
	fdt_set_all_pin(fdt_node_str, "pinctrl-0");
#endif
#ifdef CONFIG_R_I2C1_ENABLE
	sprintf(fdt_node_str, "twi7");
	fdt_set_all_pin(fdt_node_str, "pinctrl-0");
#endif
#endif
}
#ifdef CONFIG_ARM
void enable_smp(void)
{
   /* SMP status is controlled by bit 6 of the CP15 Aux Ctrl Reg:ACTLR */
   asm volatile("MRC     p15, 0, r0, c1, c0, 1");
   asm volatile("ORR     r0, r0, #0x040");
   asm volatile("MCR     p15, 0, r0, c1, c0, 1");
}
#else
void __attribute__((weak)) enable_smp(void)
{
	return ;
}
#endif
void smp_init(void)
{
    int cpu_status = 0;
#if defined(CONFIG_SUNXI_NCAT) || defined(CONFIG_SUNXI_NCAT_V2)
    cpu_status = readl(IOMEM_ADDR(SUNXI_CPUXCFG_BASE + 0x80));
    cpu_status &= (0xf<<24);
#else
    /*old platform enable smp unconditionally*/
    cpu_status = 1;
#endif

    /* note:
          sbrom will enable smp bit when jmp to non-secure fel.
          but normal brom not do this operation.
          so should enable smp when run uboot by normal fel mode.
      */
    if(!cpu_status)
        enable_smp();

}


int sunxi_plat_init(void)
{
	sunxi_probe_securemode();
#ifdef CONFIG_SUNXI_DMA
	sunxi_dma_init();
#endif

#ifdef CONFIG_ARM
	extern int secure_os_memory_init(void);
	if (sunxi_probe_secure_os()) {
		smc_tee_inform_fdt((uint64_t)(unsigned long)working_fdt, gd->fdt_size);
		secure_os_memory_init();
	}
#endif
	return 0;
}

/* add board specific code here */
int board_init(void)
{
	__maybe_unused int id_pfr1, ret, satapwr_pin, macpwr_pin;

	gd->bd->bi_boot_params = (PHYS_SDRAM_0 + 0x100);

	sunxi_plat_init();

	int work_mode = get_boot_work_mode();

	ret = axp_gpio_init();
	if (ret)
		return ret;
#ifdef CONFIG_SUNXI_POWER
	if (!axp_probe()) {
		axp_set_dcdc_mode();
		axp_set_power_supply_output();
#if defined CONFIG_SUNXI_BMU && !defined CONFIG_AXP_LATE_INFO
		gd->pmu_saved_status = bmu_get_poweron_source();
		gd->pmu_runtime_chgcur = axp_get_battery_status();

		if (work_mode != WORK_MODE_BOOT) {
			ret = axp_reset_capacity();
			if (!ret) {
				pr_msg("axp reset capacity fail!\n");
			}
		}
#else
		gd->pmu_saved_status = -1;
#endif
	}
#endif

	rtc_set_dcxo_off();

	if ((work_mode == WORK_MODE_BOOT) ||
		(work_mode == WORK_MODE_CARD_PRODUCT) ||
		(work_mode == WORK_MODE_CARD_UPDATE))
		sunxi_set_sramc_mode();
	int boot_clock;
	script_parser_fetch(FDT_PATH_TARGET, "boot_clock", &boot_clock, uboot_spare_head.boot_data.run_clock);
	clock_set_corepll(boot_clock);
	/*fix reset circuit detection threshold*/
	rtc_set_vccio_det_spare();
	tick_printf("CPU=%d MHz,PLL6=%d Mhz,AHB=%d Mhz, APB1=%dMhz  MBus=%dMhz\n",
		clock_get_corepll(),
		clock_get_pll6(), clock_get_ahb(),
		clock_get_apb1(),clock_get_mbus());

#ifdef CONFIG_SUNXI_OVERLAY
	if (get_boot_work_mode() != WORK_MODE_BOOT) {
		return 0;
	}
	if (!sunxi_overlay_apply_merged(working_fdt, gd->new_dtbo)) {
		pr_err("sunxi overlay merged %sqv\n",
			(fdt_overlay_apply_verbose(working_fdt, gd->new_dtbo) ? "fail":"ok"));
	} else {
		pr_msg("not need merged sunxi overlay\n");
	}
#endif

	return 0;
}

int dram_init(void)
{
	uint dram_size = 0;
	dram_size = uboot_spare_head.boot_data.dram_scan_size;

	dram_size = dram_size > 2048 ? 2048 : dram_size;

	if(dram_size)
		gd->ram_size = dram_size * 1024 * 1024;
	else
		gd->ram_size = get_ram_size((long *)PHYS_SDRAM_0, PHYS_SDRAM_0_SIZE);

	return 0;
}

#ifdef CONFIG_MMC
static void mmc_pinmux_setup(int sdc)
{
	#if 0
	unsigned int pin;
	__maybe_unused int pins;

	switch (sdc) {
	case 0:
		/* SDC0: PF0-PF5 */
		for (pin = SUNXI_GPF(0); pin <= SUNXI_GPF(5); pin++) {
			sunxi_gpio_set_cfgpin(pin, SUNXI_GPF_SDC0);
			sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(pin, 2);
		}
		break;

	case 2:
		pins = sunxi_name_to_gpio_bank(CONFIG_MMC2_PINS);

#if defined(CONFIG_MACH_SUN50IW6)
		/* SDC2: PC4-PC14 */
		for (pin = SUNXI_GPC(4); pin <= SUNXI_GPC(14); pin++) {
			sunxi_gpio_set_cfgpin(pin, SUNXI_GPC_SDC2);
			sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(pin, 2);
		}
#endif
		break;

	default:
		printf("sunxi: invalid MMC slot %d for pinmux setup\n", sdc);
		break;
	}
	#endif
}

/*

int board_mmc_init(bd_t *bis)
{
    sunxi_mmc_init(bis->bi_card_num);

    return 0;
}
*/
void board_mmc_pre_init(int card_num)
{
    bd_t *bd;

    bd = gd->bd;
    //gd->bd->bi_card_num = card_num;
    mmc_initialize(bd);

}

int board_mmc_get_num(void)
{
    return gd->boot_card_num;
}


void board_mmc_set_num(int num)
{
    gd->boot_card_num = num;
}


int board_mmc_init(bd_t *bis)
{
	__maybe_unused struct mmc *mmc0, *mmc1;
	__maybe_unused char buf[512];

	mmc_pinmux_setup(board_mmc_get_num());
	mmc0 = sunxi_mmc_init(board_mmc_get_num());
	if (!mmc0)
		return -1;
#if 0
#if CONFIG_MMC_SUNXI_SLOT_EXTRA != -1
	mmc_pinmux_setup(CONFIG_MMC_SUNXI_SLOT_EXTRA);
	mmc1 = sunxi_mmc_init(CONFIG_MMC_SUNXI_SLOT_EXTRA);
	if (!mmc1)
		return -1;
#endif
#endif

	return 0;
}
#endif



#ifdef CONFIG_USB_GADGET
int g_dnl_board_usb_cable_connected(void)
{
	return sunxi_usb_phy_vbus_detect(0);
}
#endif



/*
 * Note this function gets called multiple times.
 * It must not make any changes to env variables which already exist.
 */
static void setup_environment(const void *fdt)
{
#ifdef CONFIG_USB
	__maybe_unused unsigned int sid[4];
	__maybe_unused uint8_t mac_addr[6];
	__maybe_unused char ethaddr[16];
	__maybe_unused int  ret;


	ret = sunxi_get_sid(sid);
	if (ret == 0 && sid[0] != 0) {
		/*
		 * The single words 1 - 3 of the SID have quite a few bits
		 * which are the same on many models, so we take a crc32
		 * of all 3 words, to get a more unique value.
		 *
		 * Note we only do this on newer SoCs as we cannot change
		 * the algorithm on older SoCs since those have been using
		 * fixed mac-addresses based on only using word 3 for a
		 * long time and changing a fixed mac-address with an
		 * u-boot update is not good.
		 */

		sid[3] = crc32(0, (unsigned char *)&sid[1], 12);


		/* Ensure the NIC specific bytes of the mac are not all 0 */
		if ((sid[3] & 0xffffff) == 0)
			sid[3] |= 0x800000;

		strcpy(ethaddr, "ethaddr");

		/* Non OUI / registered MAC address */
		mac_addr[0] = (0 << 4) | 0x02;
		mac_addr[1] = (sid[0] >>  0) & 0xff;
		mac_addr[2] = (sid[3] >> 24) & 0xff;
		mac_addr[3] = (sid[3] >> 16) & 0xff;
		mac_addr[4] = (sid[3] >>  8) & 0xff;
		mac_addr[5] = (sid[3] >>  0) & 0xff;

		eth_env_set_enetaddr(ethaddr, mac_addr);

	}
#endif
}

int misc_init_r(void)
{
	__maybe_unused int ret;

	setup_environment(gd->fdt_blob);
#if 0
	ret = sunxi_usb_phy_probe();
	if (ret)
		return ret;
#endif

#ifdef CONFIG_USB_ETHER
	usb_ether_init();
#endif

	return 0;
}

int ft_board_setup(void *blob, bd_t *bd)
{
	int __maybe_unused r;

	/*
	 * Call setup_environment again in case the boot fdt has
	 * ethernet aliases the u-boot copy does not have.
	 */
	setup_environment(blob);

#ifdef CONFIG_VIDEO_DT_SIMPLEFB
	r = sunxi_simplefb_setup(blob);
	if (r)
		return r;
#endif
	return 0;
}

static int reserve_bootlogo(void)
{
#ifndef FPGA_PLATFORM
	uint32_t *compressed_logo_size =
		(uint32_t *)(CONFIG_SYS_TEXT_BASE + (3 * 1024 * 1024));
	uint32_t *compressed_logo_buf =
		(uint32_t *)(CONFIG_SYS_TEXT_BASE + (3 * 1024 * 1024) + 16);

	gd->boot_logo_addr = 0;
	if (get_boot_work_mode() != WORK_MODE_BOOT) {
		debug("no boot mode, dont read bootlogo\n");
		return 0;
	}
	if (((uboot_spare_head.boot_data.func_mask &
	      UBOOT_FUNC_MASK_BIT_BOOTLOGO) != UBOOT_FUNC_MASK_BIT_BOOTLOGO) ||
	    (*compressed_logo_size == 0)) {
		debug("no bootlogo from boot_package\n");
		return 0;
	}
	gd->start_addr_sp -= ALIGN(*compressed_logo_size, 16);
	gd->boot_logo_addr =
		(ulong)map_sysmem(gd->start_addr_sp, *compressed_logo_size);
	/* reserve logo_buf size addr */
	gd->start_addr_sp -= 16;
	*(uint *)(gd->boot_logo_addr - 16) = *compressed_logo_size;
	memcpy((void *)gd->boot_logo_addr, compressed_logo_buf,
	       *compressed_logo_size);
	debug("reserve: 0x%lx from 0x%x: for boot logo in boot package\n",
	      gd->boot_logo_addr, *compressed_logo_size);
#endif
	return 0;
}

int reserve_arch(void)
{
	reserve_bootlogo();
	return 0;
}
