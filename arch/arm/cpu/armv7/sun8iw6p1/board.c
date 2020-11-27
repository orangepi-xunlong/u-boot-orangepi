/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software;you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation;either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY;without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 */

#include <common.h>
#include <asm/armv7.h>
#include <asm/io.h>
#include <power/sunxi/pmu.h>
#include <power/sunxi/axp81X_reg.h>
#include <asm/arch/timer.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/key.h>
#include <asm/arch/clock.h>
#include <asm/arch/efuse.h>
#include <asm/arch/cpu.h>
#include <asm/arch/sys_proto.h>
#include <boot_type.h>
#include <sys_partition.h>
#include <sys_config_old.h>
#include <smc.h>
#include <rsb.h>
#include <sunxi_board.h>
/* The sunxi internal brom will try to loader external bootloader
 * from mmc0, nannd flash, mmc2.
 * We check where we boot from by checking the config
 * of the gpio pin.
 */
DECLARE_GLOBAL_DATA_PTR;


extern int sunxi_clock_get_axi(void);
extern int sunxi_clock_get_ahb(void);
extern int sunxi_clock_get_apb1(void);
extern int sunxi_clock_get_pll6(void);
extern void power_limit_init(void);

u32 get_base(void)
{

	u32 val;

	__asm__ __volatile__("mov %0, pc \n" : "=r"(val) : : "memory");
	val &= 0xF0000000;
	val >>= 28;
	return val;
}

#if 0

/* do some early init */
void s_init(void)
{
	watchdog_disable();
}


void reset_cpu(ulong addr)
{
	watchdog_enable();
#ifndef CONFIG_A73_FPGA
loop_to_die:
	goto loop_to_die;
#endif
}
#endif

void v7_outer_cache_enable(void)
{
	return ;
}

void v7_outer_cache_inval_all(void)
{
	return ;
}

void v7_outer_cache_flush_range(u32 start, u32 stop)
{
	return ;
}



struct bias_set {
	int  vol;
	int  index;
};

int power_config_gpio_bias(void)
{
	char gpio_bias[32], gpio_name[32];
	char *gpio_name_const = "pa_bias";
	char port_index;
	char *axp = NULL, *supply = NULL, *vol = NULL;
	/* uint main_hd;*/
	uint bias_vol_set;
	int  index1, index2, ret, i;
	uint port_bias_addr;
	uint vol_index, config_type;
	int  pmu_vol;
	struct bias_set bias_vol_config[8] = { {1800, 0}, {2500, 6}, {2800, 9}, {3000, 0xa}, {3300, 0xd}, {0, 0} };

/* 	main_hd = script_parser_fetch_subkey_start("gpio_bias");*/

	index1 = 0;
	index2 = 0;
	while (1) {
		memset(gpio_bias, 0, 32);
		memset(gpio_name, 0, 32);
		ret = script_parser_fetch_subkey_next(
		    "gpio_bias", gpio_name, (int *)gpio_bias, &index1, &index2);
		if (!ret) {
			/* lower(gpio_name); */
			/* lower(gpio_bias); */

			port_index = gpio_name[1];
			gpio_name[1] = 'a';
			if (strcmp(gpio_name_const, gpio_name)) {
				printf("invalid gpio bias name %s\n",
				       gpio_name);

				continue;
			}
			gpio_name[1] = port_index;
			i = 0;
			axp = gpio_bias;
			while ((gpio_bias[i] != ':') &&
			       (gpio_bias[i] != '\0')) {
				i++;
			}
			gpio_bias[i++] = '\0';

			if (!strcmp(axp, "constant")) {
				config_type = 1;
			} else if (!strcmp(axp, "floating")) {
				printf("ignore %s bias config\n", gpio_name);

				continue;
			} else {
				config_type = 0;
			}

			if (config_type == 0) {
				supply = gpio_bias + i;
				while ((gpio_bias[i] != ':') &&
				       (gpio_bias[i] != '\0')) {
					i++;
				}
				gpio_bias[i++] = '\0';
			}

			printf("supply=%s\n", supply);
			vol = gpio_bias + i;
			while ((gpio_bias[i] != ':') &&
			       (gpio_bias[i] != '\0')) {
				i++;
			}

			bias_vol_set = simple_strtoul(vol, NULL, 10);
			for (i = 0; i < 5; i++) {
				if (bias_vol_config[i].vol == bias_vol_set) {
					break;
				}
			}
			if (i == 5) {
				printf("invalid gpio bias set vol %d, at name "
				       "%s\n",
				       bias_vol_set, gpio_name);

				break;
			}
			vol_index = bias_vol_config[i].index;

			if ((port_index >= 'a') && (port_index <= 'h')) {
				/* 获取寄存器地址 */
				port_bias_addr = SUNXI_PIO_BASE + 0x300 +
						 0x4 * (port_index - 'a');
			} else if (port_index == 'j') {
				/* 获取寄存器地址 */
				port_bias_addr = SUNXI_PIO_BASE + 0x300 +
						 0x4 * (port_index - 'a');
			} else if ((port_index == 'l') || (port_index == 'm')) {
				/* 获取寄存器地址 */
				port_bias_addr = SUNXI_R_PIO_BASE + 0x300 +
						 0x4 * (port_index - 'l');
			} else {
				printf("invalid gpio port at name %s\n",
				       gpio_name);

				continue;
			}
			printf("axp=%s, supply=%s, vol=%d\n", axp, supply,
			       bias_vol_set);
			if (config_type == 1) {
				smc_writel(vol_index, port_bias_addr);
			} else {
				pmu_vol =
				    axp_probe_supply_status_byname(axp, supply);
				if (pmu_vol < 0) {
					printf(
					    "sunxi board read %s %s failed\n",
					    axp, supply);

					continue;
				}

				if (pmu_vol >
				    bias_vol_set) /* pmu实际电压超过需要设置的电压
						     */
				{
					/* 电压降低到需要电压 */
					axp_set_supply_status_byname(
					    axp, supply, bias_vol_set, 1);
					/* 设置寄存器 */
					smc_writel(vol_index, port_bias_addr);
				} else if (
				    pmu_vol <
				    bias_vol_set) /* pmu实际电压低于需要设置的电压
						     */
				{
					/* 设置寄存器 */
					smc_writel(vol_index, port_bias_addr);
					/* 把pmu电压调整到需要的电压 */
					axp_set_supply_status_byname(
					    axp, supply, bias_vol_set, 1);
				} else {
					/* 如果实际电压等于需要设置电压，直接设置即可
					 */
					smc_writel(vol_index, port_bias_addr);
				}
			}
			printf("reg addr=0x%x, value=0x%x, pmu_vol=%d\n",
			       port_bias_addr, smc_readl(port_bias_addr),
			       bias_vol_set);
		} else {
			printf("config gpio bias voltage finish\n");

			break;
		}
	}

	return 0;
}

/* for A83 bug fix */
#define RSB_SADDR_AC100         0xE89
#define AC100_ADDR              0xAC  /* --chip id */
/* disable RTC interrupt */
static void disable_rtc_int(void)
{
	u16 reg_val = 0;
	if (0 != sunxi_rsb_config(AC100_ADDR, RSB_SADDR_AC100)) {
		printf("RSB: config fail for ac100\n");
		return;
	}

	if (sunxi_rsb_read(AC100_ADDR, 0x0, (u8 *)&reg_val, 2)) {
		printf("=====cannot read rtc 0xd0 =====\n");
		goto __END;
	}
	printf("ac100 reg 0x00 = 0x%x\n", reg_val);

	if (sunxi_rsb_read(AC100_ADDR, 0xd0, (u8 *)&reg_val, 2)) {
		printf("=====cannot read rtc 0xd0 =====\n");
		goto __END;
	}
	/* printf("before====rtc 0xd0 = 0x%x\n", reg_val);*/
	if (sunxi_rsb_read(AC100_ADDR, 0xd1, (u8 *)&reg_val, 2)) {
		printf("=====cannot read rtc 0xd0 =====\n");
		goto __END;
	}
	/* printf("before====rtc 0xd1 = 0x%x\n", reg_val);*/

	/* set d0 d1 */
	reg_val = 0;
	if (sunxi_rsb_write(AC100_ADDR, 0xd0, (u8 *)&reg_val, 2)) {
		printf("==== can't disable rtc 0xd0 int ====\n");
		goto __END;
	}

	reg_val = 1;
	if (sunxi_rsb_write(AC100_ADDR, 0xd1, (u8 *)&reg_val, 2)) {
		printf("==== cant disable rtc 0xd1 int =====\n");
		goto __END;
	}

	/* read d0 d1 */
	if (sunxi_rsb_read(AC100_ADDR, 0xd0, (u8 *)&reg_val, 2)) {
		printf("=====cannot read rtc 0xd0 =====\n");
		goto __END;
	}
	printf("ac100 reg 0xd0 = 0x%x\n", reg_val);
	if (sunxi_rsb_read(AC100_ADDR, 0xd1, (u8 *)&reg_val, 2)) {
		printf("=====cannot read rtc 0xd0 =====\n");
		goto __END;
	}
	printf("ac100 reg 0xd1 = 0x%x\n", reg_val);

__END:
	return;
}

int power_source_init(void)
{
	int pll1;
	int cpu_vol;
	int dcdc_vol;

#ifdef CONFIG_ARCH_SUN8IW6P1
	if (uboot_spare_head.boot_data.work_mode == WORK_MODE_USB_PRODUCT) {
		printf("skip setting axp in usb product mode\n");
		return 0;
	}
#endif

	if (script_parser_fetch("power_sply", "dcdc2_vol", &dcdc_vol, 1)) {
		cpu_vol = 900;
	} else {
		cpu_vol = dcdc_vol % 10000;
	}

	sunxi_rsb_config(AXP81X_ADDR, 0x3a3);
	if (axp_probe() > 0) {
		axp_probe_factory_mode();
		if (!axp_probe_power_supply_condition()) {
			/* PMU_SUPPLY_DCDC2 is for cpua */
			if (!axp_set_supply_status(0, PMU_SUPPLY_DCDC2, cpu_vol,
						   -1)) {
				tick_printf("PMU: dcdc2 %d\n", cpu_vol);
				sunxi_clock_set_corepll(
				    uboot_spare_head.boot_data.run_clock, 0);
			} else {
				printf("axp_set_dcdc2 fail\n");
			}
		} else {
			printf("axp_probe_power_supply_condition error\n");
		}
	} else {
		printf("axp_probe error\n");
	}

	pll1 = sunxi_clock_get_corepll();

	tick_printf("PMU: pll1 %d Mhz\n", pll1);
	printf("AXI0=%d Mhz, PLL_PERIPH =%d Mhz AHB1=%d Mhz, APB1=%d Mhz \n",
	       sunxi_clock_get_axi(), sunxi_clock_get_pll6(),
	       sunxi_clock_get_ahb(), sunxi_clock_get_apb1());

	axp_set_charge_vol_limit();
	axp_set_all_limit();
	axp_set_hardware_poweron_vol();

	axp_set_power_supply_output();
	power_config_gpio_bias();

	power_limit_init();
	/* AXP and RTC use the same interrupt line, so disable RTC INT in uboot
	 */
	disable_rtc_int();

	return 0;
}


/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_probe_securemode(void)
{
	uint reg_val;
	int secure_mode = 0;

	u32 reserved_value = readl(SUNXI_SECURE_SRAM_BASE);
	u32 reserved_data  = 0x5a5a;

	if (reserved_value == reserved_data)
		reserved_data = 0xa5a5;

	writel(reserved_data, SUNXI_SECURE_SRAM_BASE);
	reg_val = readl(SUNXI_SECURE_SRAM_BASE);
	printf("reg=0x%x\n", reg_val);
	if (reg_val != reserved_data) {
		/* all 0, secure is enable */
		secure_mode = 1;
	} else {
		writel(reserved_value, SUNXI_SECURE_SRAM_BASE);
	}

	if (secure_mode) {
		/* sbrom  set  secureos_exist flag, */
		/* 1: secure os exist 0: secure os not exist */
		if (uboot_spare_head.boot_data.secureos_exist == 1) {
			gd->securemode = SUNXI_SECURE_MODE_WITH_SECUREOS;
			printf("secure mode: with secureos\n");
		} else {
			gd->securemode = SUNXI_SECURE_MODE_NO_SECUREOS;
			printf("secure mode: no secureos\n");
		}
		gd->bootfile_mode = SUNXI_BOOT_FILE_TOC;
	} else {
		/* boot0  set  secureos_exist flag, */
		/* 1: secure monitor exist 0: secure monitor  not exist */
		int burn_secure_mode = 0;
		/* int nodeoffset;*/

		gd->securemode = SUNXI_NORMAL_MODE;
		gd->bootfile_mode = SUNXI_BOOT_FILE_PKG;
		printf("normal mode\n");

		if (script_parser_fetch("target", "burn_secure_mode",
					&burn_secure_mode, 1))
			return 0;

		if (burn_secure_mode != 1)
			return 0;

		gd->bootfile_mode = SUNXI_BOOT_FILE_TOC;
	}
	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_set_secure_mode(void)
{
    int mode;

	if ((gd->securemode == SUNXI_NORMAL_MODE) &&
	(gd->bootfile_mode == SUNXI_BOOT_FILE_TOC)) {
		mode = sid_probe_security_mode();
		if (!mode) {
		    sid_set_security_mode();
		    gd->bootfile_mode = SUNXI_BOOT_FILE_TOC;
		}
	}

    return 0;
}

/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int sunxi_probe_enable_securebit(void)
{
    int burn_secure_mode = 0;

    if (gd->securemode != SUNXI_NORMAL_MODE) {
	    return 0;
    }
    script_parser_fetch("target", "burn_secure_mode", &burn_secure_mode, 1);
    if (burn_secure_mode == 1) {
	    gd->bootfile_mode = SUNXI_BOOT_FILE_TOC;
	    printf("Ready to set the secure enable bit\n");
    }

    return 0;
}

int sunxi_set_bootmode_flag(u8 flag)
{
	volatile uint reg_val;

	do {
		smc_writel((3 << 16) | (flag << 8), SUNXI_RPRCM_BASE + 0x1f0);
		smc_writel((3 << 16) | (flag << 8) | (1U << 31), SUNXI_RPRCM_BASE + 0x1f0);
		__usdelay(10);
		CP15ISB;
		CP15DMB;
		smc_writel((3 << 16) | (flag << 8), SUNXI_RPRCM_BASE + 0x1f0);
		reg_val = smc_readl(SUNXI_RPRCM_BASE + 0x1f0);
	} while ((reg_val & 0xff) != flag);

	return 0;
}

int sunxi_get_bootmode_flag(void)
{
	uint fel_flag;

	smc_writel(smc_readl(SUNXI_RPRCM_BASE + 0x1f0) | (3 << 16), SUNXI_RPRCM_BASE + 0x1f0);
	CP15ISB;
	CP15DMB;
	fel_flag = smc_readl(SUNXI_RPRCM_BASE + 0x1f0) & 0xff;

	return fel_flag;
}
