/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <power/sunxi/pmu.h>
#include <power/sunxi/power.h>
#include <asm/arch/intc.h>
#include <sunxi_bmp.h>
#include <sunxi_board.h>
#include <sys_config_old.h>
#include <lzma/LzmaTools.h>
#include <smc.h>
#include <asm/arch/platsmp.h>
#include <cputask.h>
#include <smc.h>
#include <securestorage.h>
#include <fdt_support.h>
#include <bmp_layout.h>
#include <malloc.h>

DECLARE_GLOBAL_DATA_PTR;

typedef enum __CPU_STATE
{
	CPU_IDLE = 0,
	CPU_WORK,
	CPU_QUIT,
}CPU_STATE;

typedef enum __BMP_DECODE_STATE
{
	BMP_DECODE_IDLE = 0,
	BMP_DECODE_FAIL = 1,
	BMP_DECODE_SUCCESS =2,
}BMP_DECODE_STATE;


volatile unsigned int bmp_decode_flag = BMP_DECODE_IDLE;
volatile unsigned int cpu2_work_flag = CPU_IDLE;
volatile unsigned int cpu1_quit_flag = -1;

static sunxi_bmp_store_t bmp_info;
static unsigned char *ready_to_decode_buf;

__attribute__((section(".data")))
unsigned long secondary_cpu_data_groups[4];
__attribute__((section(".data")))
unsigned long secondary_cpu_mode_stack[4];
__attribute__((section(".data")))
uint *p_spin_lock_uart;
__attribute__((section(".data")))
uint *p_spin_lock_heap;
__attribute__((section(".data")))
volatile uint cpu_running[4] = {0};

extern int initr_sunxi_display(void);
extern int sunxi_usb_dev_register(uint dev_name);
extern void sunxi_usb_main_loop(int mode);

static inline uint get_cpu_running(uint cpu)
{
	return cpu_running[cpu];
}

static inline void set_cpu_running(uint cpu)
{
	cpu_running[cpu] = 1;
}

uint *request_spin_lock(void)
{
	uint *p_spin_lock = NULL;
	p_spin_lock = malloc_noncache(4);
	if (p_spin_lock == NULL)
		return NULL;

	*p_spin_lock = 0;
	return p_spin_lock;
}

void sunxi_multi_core_boot(void)
{
	secondary_cpu_data_groups[0] = gd->arch.tlb_addr;
	secondary_cpu_data_groups[1] = (unsigned long)gd;

	/* cpu1 svc sp */
	secondary_cpu_mode_stack[0] = gd->secondary_cpu_svc_sp[1];
	/* cpu1 irq sp */
	secondary_cpu_mode_stack[1] = gd->secondary_cpu_irq_sp[1];
	/* cpu2 svc sp */
	secondary_cpu_mode_stack[2] = gd->secondary_cpu_svc_sp[2];

	flush_cache((long unsigned int)secondary_cpu_data_groups, 16);
	flush_cache((long unsigned int)secondary_cpu_mode_stack, 16);

	p_spin_lock_uart = request_spin_lock();
	if (p_spin_lock_uart == NULL) {
		pr_msg("error:p_spin_lock_uart is NULL: %s %d\n",
			__func__, __LINE__);
		return;
	}
	p_spin_lock_heap = request_spin_lock();
	if (p_spin_lock_heap == NULL) {
		pr_msg("error:p_spin_lock_heap is NULL: %s %d\n",
			__func__, __LINE__);
		return;
	}

	asm volatile("isb");
	asm volatile("dmb");

	pr_msg("power on cpu1\n");
	sunxi_set_cpu_on(1, (uint)secondary_cpu_start);
	mdelay(10);
	while (!get_cpu_running(1))
		;
	pr_msg("power on cpu2\n");
	sunxi_set_cpu_on(2, (uint)third_cpu_start);
	while (!get_cpu_running(2))
		;
}

static void set_cpu2_state(int state)
{
	cpu2_work_flag = state;
}

static int get_cpu2_state(void)
{
	return cpu2_work_flag;
}
static void set_decode_buffer(void* buf)
{
	ready_to_decode_buf = buf;
}

static void* get_decode_buffer(void)
{
	return (void*)ready_to_decode_buf;
}

void inline set_bmp_decode_flag(int flag)
{
	bmp_decode_flag = flag;
}

int inline get_bmp_decode_flag(void)
{
	return bmp_decode_flag;
}


/* check battery ratio */
static int __battery_ratio_calucate( void)
{
	int  Ratio;

	Ratio = axp_probe_rest_battery_capacity();
	return Ratio;
}


typedef enum __BOOT_POWER_STATE
{
	SUNXI_STATE_SHUTDOWN_DIRECTLY = 0,
	SUNXI_STATE_SHUTDOWN_CHARGE,
	SUNXI_STATE_ANDROID_CHARGE,
	SUNXI_STATE_NORMAL_BOOT,
	SUNXI_STATE_SHUTDOWN_WITHOUT_SRC
}SUNXI_BOOT_POWER_STATE_E;


static int sunxi_probe_power_state(void)
{
	int __bat_exist, __power_source;
	int __bat_vol, __bat_ratio;
	int __safe_vol, power_start = 0;
	int ret = 0, __power_on_cause;
	SUNXI_BOOT_POWER_STATE_E __boot_state;
	int pmu_bat_unused = 0;

/* power_start
0: not allow boot by insert dcin,boot condition:
    a.press power key in long time & pre state is system state
    b.Battery ratio shouled enough
1: allow boot directly  by insert dcin,boot condition:
    a.Battery ratio shouled enough
2: not allow boot by insert dcin,boot condition:
    a. press power key in long time & pre state is system state
    b. do not check battery ratio
3: allow boot directly  by insert dcin,boot condition:
    do not check battery ratio
*/
	script_parser_fetch(PMU_SCRIPT_NAME, "power_start", &power_start, 1);
	script_parser_fetch(PMU_SCRIPT_NAME, "pmu_bat_unused",
		(int *)&pmu_bat_unused, 1);

	/* check battery */
	__bat_exist = axp_probe_battery_exist();
	/* check power source,vbus or dcin, if both,treat it as vbus */
	__power_source = axp_probe_power_source();
	pr_notice("PowerBus = %d( %d:vBus %d:acBus other: not exist)\n",
		__power_source,AXP_VBUS_EXIST,AXP_DCIN_EXIST);


	if (pmu_bat_unused  || (__bat_exist <= 0))
	{
		pr_notice("no battery exist\n");
		return SUNXI_STATE_NORMAL_BOOT;
	}

	if (PMU_PRE_CHARGE_MODE == gd->pmu_saved_status) {
		if (__power_source) {
			pr_msg("SUNXI_STATE_ANDROID_CHARGE");
			return SUNXI_STATE_ANDROID_CHARGE;
		} else {
			pr_msg("SUNXI_STATE_SHUTDOWN_WITHOUT_SRC");
			return SUNXI_STATE_SHUTDOWN_WITHOUT_SRC;
		}
	}

	//check battery ratio and vol
	__bat_ratio = __battery_ratio_calucate();
	__bat_vol = axp_probe_battery_vol();
	pr_msg("Battery Voltage=%d, Ratio=%d\n", __bat_vol, __bat_ratio);

	//probe the vol threshold
	ret = script_parser_fetch(PMU_SCRIPT_NAME, "pmu_safe_vol", &__safe_vol, 1);
	if ((ret) || (__safe_vol < 3000)) {
		__safe_vol = 3500;
	}

	//judge if boot or charge
	__power_on_cause = axp_probe_startup_cause();
	pr_msg("power on by %s\n",
		__power_on_cause ==AXP_POWER_ON_BY_POWER_TRIGGER ?
		"power trigger" : "key trigger");

	switch (power_start) {
		case 0:
			if (__bat_ratio < 1) {
				if (!__power_source) {
					__boot_state = SUNXI_STATE_SHUTDOWN_DIRECTLY;
					break;
				}

				if (__bat_vol < __safe_vol) {
					//低电量，低电压情况下，进入关机充电模式
					__boot_state = SUNXI_STATE_SHUTDOWN_CHARGE;
					break;
				}
			}

			if (__power_on_cause == AXP_POWER_ON_BY_POWER_KEY)
				__boot_state = SUNXI_STATE_NORMAL_BOOT;
			else
				__boot_state = SUNXI_STATE_ANDROID_CHARGE;

			break;
		case 1:
			if (__bat_ratio < 1) {
				if (!__power_source) {
					__boot_state = SUNXI_STATE_SHUTDOWN_DIRECTLY;
					break;
				}

				if (__bat_vol < __safe_vol) {
					//低电量，低电压情况下，进入关机充电模式
					__boot_state = SUNXI_STATE_SHUTDOWN_CHARGE;
					break;
				}
			}
			__boot_state = SUNXI_STATE_NORMAL_BOOT;
			break;
		case 2:
			//判断，如果是按键开机
			if (__power_on_cause == AXP_POWER_ON_BY_POWER_KEY)
				__boot_state = SUNXI_STATE_NORMAL_BOOT;
			else
				__boot_state = SUNXI_STATE_ANDROID_CHARGE;

			break;
		case 3:
		default:
			//直接开机进入系统，不进充电模式
			__boot_state = SUNXI_STATE_NORMAL_BOOT;
			break;
	}


	switch(__boot_state)
	{
		case SUNXI_STATE_SHUTDOWN_DIRECTLY:
			pr_msg("STATE_SHUTDOWN_DIRECTLY\n");
			break;
		case SUNXI_STATE_SHUTDOWN_CHARGE:
			pr_msg("STATE_SHUTDOWN_CHARGE\n");
			break;
		case SUNXI_STATE_ANDROID_CHARGE:
			pr_msg("STATE_ANDROID_CHARGE\n");
			break;
		case SUNXI_STATE_SHUTDOWN_WITHOUT_SRC:
			pr_msg("STATE_SHUTDOWN_WITHOUT_SRC\n");
			break;
		case SUNXI_STATE_NORMAL_BOOT:
		default:
			pr_msg("STATE_NORMAL_BOOT\n");
			break;
	}
	return __boot_state;

}


static int sunxi_pmu_treatment(void)
{
	if (uboot_spare_head.boot_ext[0].data[0] <= 0) {
		pr_msg("no pmu exist\n");
		return -1;
	}

	return 0;
}

int sunxi_bmp_decode_from_compress(unsigned char *dst_buf, unsigned char *src_buf)
{
	unsigned long dst_len, src_len;

	src_len = *(uint *)(src_buf - 16);
	return lzmaBuffToBuffDecompress(dst_buf, (SizeT *)&dst_len,
			(void *)src_buf, (SizeT)src_len);
}

int sunxi_secendary_cpu_task(void)
{
	int next_mode;
	int ret = -1;
	int advert_enable = 0;

	enable_caches();
	pr_notice("task entry\n");
	set_cpu_running(1);
	sunxi_pmu_treatment();
	next_mode = sunxi_probe_power_state();

	if (next_mode == SUNXI_STATE_NORMAL_BOOT) {
		set_decode_buffer((unsigned char *)
			(SUNXI_LOGO_COMPRESSED_LOGO_BUFF));
	} else if (next_mode == SUNXI_STATE_SHUTDOWN_CHARGE) {
		gd->need_shutdown = 1;
		set_decode_buffer((unsigned char *)
			(SUNXI_SHUTDOWN_CHARGE_COMPRESSED_LOGO_BUFF));
	} else if (next_mode == SUNXI_STATE_ANDROID_CHARGE) {
		gd->chargemode = 1;
		set_decode_buffer((unsigned char *)
			(SUNXI_ANDROID_CHARGE_COMPRESSED_LOGO_BUFF));
	} else if (next_mode == SUNXI_STATE_SHUTDOWN_DIRECTLY || next_mode == SUNXI_STATE_SHUTDOWN_WITHOUT_SRC) {
		gd->need_shutdown = 1;
		set_decode_buffer((unsigned char *)
			(SUNXI_SHUTDOWN_CHARGE_COMPRESSED_LOGO_BUFF));
	}
	set_cpu2_state(CPU_WORK);
	enable_interrupts();
	sunxi_gic_cpu_interface_init(get_core_pos());
#ifndef CONFIG_BOOTLOGO_DISABLE
	if (next_mode != SUNXI_STATE_SHUTDOWN_WITHOUT_SRC) {
		initr_sunxi_display();
	}
#if defined(ENABLE_ADVERT_PICTURE)
	ret = script_parser_fetch("target", "advert_enable",
				  (int *)&advert_enable, sizeof(int) / 4);
#endif
	if (ret || !advert_enable) {
		while (BMP_DECODE_IDLE == get_bmp_decode_flag())
			;

		if (BMP_DECODE_SUCCESS == get_bmp_decode_flag()) {
			sunxi_bmp_dipslay_screen(bmp_info);
			pr_notice("boot logo display ok\n");
		}
		board_display_wait_lcd_open();
		board_display_set_exit_mode(1);
	} else {
		while (BMP_DECODE_IDLE == get_bmp_decode_flag())
			; /* ensure cpu2 off */
		gd->logo_status_multiboot = DISPLAY_DRIVER_INIT_OK;
		while (gd->logo_status_multiboot == DISPLAY_DRIVER_INIT_OK)
			; /* wait cpu0 logo load */
		/*
		 * if load bootlogo from Reserve0 successfully, use it;
		 * otherwise, will use one in boot_package/toc1_item.
		 */
#if defined(CONFIG_BOOT_GUI)
		if (gd->logo_status_multiboot == DISPLAY_LOGO_LOAD_OK)
			bmp_info.buffer = (char *)CONFIG_SYS_SDRAM_BASE;
#else
		sunxi_bmp_display_mem((unsigned char *)CONFIG_SYS_SDRAM_BASE,
				      &bmp_info);
#endif
		if (next_mode != SUNXI_STATE_SHUTDOWN_WITHOUT_SRC) {
			ret = sunxi_bmp_dipslay_screen(bmp_info);
		}
		if (ret != 0)
			pr_error("sunxi_bmp_dipslay_screen fail\n");
	}

	/* wait bmp show on screen */
	mdelay(50);
#endif
	/* show low power logo */
	if (gd->need_shutdown && next_mode != SUNXI_STATE_SHUTDOWN_WITHOUT_SRC)
		mdelay(3000);

	while (cpu1_quit_flag != CPU_QUIT)
		;
	pr_msg("cpu %d enter wfi mode\n", get_core_pos());
	disable_interrupts();
	sunxi_gic_cpu_interface_exit();
	cleanup_before_powerdown();
	sunxi_set_cpu_off();

	return 0;
}

int sunxi_third_cpu_task(void)
{
	int ret;
	unsigned char *buf;
	unsigned char *compress_buf;

	enable_caches();
	pr_notice("task entry\n");
	set_cpu_running(2);
	while(get_cpu2_state() == CPU_IDLE);
	pr_msg("ready to work\n");

	compress_buf = get_decode_buffer();
	if (compress_buf == NULL) {
		pr_msg("bmp compress_buf empty,quit\n");
		set_bmp_decode_flag(BMP_DECODE_FAIL);
		goto __third_cpu_end;
	}

	buf = (unsigned char *)(SUNXI_DISPLAY_FRAME_BUFFER_ADDR + SUNXI_DISPLAY_FRAME_BUFFER_SIZE);

	ret = sunxi_bmp_decode_from_compress(buf, compress_buf);
	if(ret) {
		pr_msg("bmp lzma decode err\n");
		set_bmp_decode_flag(BMP_DECODE_FAIL);
		goto __third_cpu_end;
	}

#if defined(CONFIG_BOOT_GUI)
	bmp_info.buffer = buf;
	ret = 0;
#else
	ret = sunxi_bmp_display_mem(buf, &bmp_info);
	if (ret) {
		pr_msg("bmp decode err\n");
		set_bmp_decode_flag(BMP_DECODE_FAIL);
		goto __third_cpu_end;
	}
#endif

	set_bmp_decode_flag(BMP_DECODE_SUCCESS);

#ifndef CONFIG_BOOT_GUI
	sunxi_fdt_getprop_store(working_fdt, "disp", "fb_base", (u32) bmp_info.buffer- sizeof(bmp_header_t));
#endif

__third_cpu_end:
	pr_msg("cpu %d enter wfi mode\n", get_core_pos());
	disable_interrupts();
	sunxi_gic_cpu_interface_exit();
	cleanup_before_powerdown();
	sunxi_set_cpu_off();

	return 0;

}
#ifdef CONFIG_ARM_A7
int sunxi_secondary_cpu_poweroff(void)
{
	cpu1_quit_flag = CPU_QUIT;
	pr_msg("check cpu power status\n");
	while (sunxi_probe_cpu_power_status(2)) {
		if (sunxi_probe_wfi_mode(2)) {
			sunxi_disable_cpu(2);
		}
	}
	pr_notice("cpu2 has poweroff\n");
	while (sunxi_probe_cpu_power_status(1)) {
		if (sunxi_probe_wfi_mode(1)) {
			sunxi_disable_cpu(1);
		}
	}
	pr_notice("cpu1 has poweroff\n");

	return 0;

}

int sunxi_set_cpu_off(void)
{
	if (sunxi_probe_secure_monitor() || sunxi_probe_secure_os()) {
		pr_msg("set cpu off by secure os\n");
#ifdef CONFIG_ARCH_SUN8IW17P1
		sunxi_smc_set_cpu_off();
		sunxi_set_wfi_mode(get_core_pos());
#else
		arm_svc_set_cpu_off(get_core_pos());
#endif
	} else {
		pr_msg("set cpu off by nromal\n");
		sunxi_set_wfi_mode(get_core_pos());
	}
	return 0;
}

int sunxi_set_cpu_on(int cpu, uint entry)
{
	if (sunxi_probe_secure_monitor() || sunxi_probe_secure_os()) {

#ifdef CONFIG_ARCH_SUN8IW17P1
		sunxi_smc_set_cpu_entry(entry, cpu);
		sunxi_enable_cpu(cpu);
#else
		pr_msg("set cpu on by secure os\n");
		arm_svc_set_cpu_on(cpu, entry);
#endif
	} else {
		pr_msg("set cpu on by nromal\n");
		sunxi_set_secondary_entry((void *)entry);
		sunxi_enable_cpu(cpu);
	}
	return 0;
}
#else
int sunxi_secondary_cpu_poweroff(void)
{
	cpu1_quit_flag = CPU_QUIT;
	pr_msg("check cpu power status\n");
	while (sunxi_probe_cpu_power_status(2))
		;
	pr_notice("cpu2 has poweroff\n");
	while (sunxi_probe_cpu_power_status(1))
		;
	pr_notice("cpu1 has poweroff\n");

	return 0;
}

int sunxi_set_cpu_off(void)
{
	return arm_svc_set_cpu_off( get_core_pos());
}

int sunxi_set_cpu_on(int cpu,uint entry )
{
	return arm_svc_set_cpu_on(cpu, entry);
}
#endif /*CONFIG_ARM_A7*/
