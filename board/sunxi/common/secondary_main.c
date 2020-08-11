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
#ifdef CONFIG_BOOT_GUI
#include <boot_gui.h>
#endif
extern int sunxi_bmp_display_multiboot(char *name,char *bmp_buf_addr);

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
static sunxi_bmp_store_t bmp_info;
static unsigned char *ready_to_decode_buf;


DECLARE_GLOBAL_DATA_PTR;

#define SUNXI_GP_STATUS_REG			(SUNXI_RTC_BASE + 0x100 + 0x04)

extern int initr_sunxi_display(void);
extern int sunxi_usb_dev_register(uint dev_name);
extern void sunxi_usb_main_loop(int mode);

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


//check battery and voltage
static int __battery_ratio_calucate( void)
{
	int  Ratio ;

	Ratio = axp_probe_rest_battery_capacity();
	if(Ratio < 1)
	{
		//some board coulombmeter value is not precise whit low capacity, so open it again here
		//note :in this case ,you should wait at least 1s berfore you read battery ratio again
		axp_set_coulombmeter_onoff(0);
		axp_set_coulombmeter_onoff(1);
	}
	return Ratio;
}


typedef enum __BOOT_POWER_STATE
{
	SUNXI_STATE_SHUTDOWN_DIRECTLY = 0,
	SUNXI_STATE_SHUTDOWN_CHARGE,
	SUNXI_STATE_ANDROID_CHARGE,
	SUNXI_STATE_NORMAL_BOOT
}SUNXI_BOOT_POWER_STATE_E;

//function : PowerCheck
//para: null
//note:  Decide whether to boot

static int sunxi_probe_power_state(void)
{
	int __bat_exist, __power_source;
	int __bat_vol, __bat_ratio;
	int __safe_vol, power_start = 0;
	int ret = 0, __power_on_cause;
	SUNXI_BOOT_POWER_STATE_E __boot_state;

//power_start
//0:  not allow boot by insert dcin: press power key in long time & pre state is system state(Battery ratio shouled enough)
//0: 关机状态下，插入外部电源时，电池电量充足时，不允许开机，会进入充电模式；电池电量不足，则关机
//1: allow boot directly  by insert dcin:( Battery ratio shouled enough)
//1: 关机状态下，插入外部电源，电池电量充足时，直接开机进入系统；电池电量不足，则关机
//2: not allow boot by insert dcin:press power key in long time & pre state is system state(do not check battery ratio)
//2: 关机状态下，插入外部电源时，不允许开机，会进入充电模式；无视电池电量
//3: allow boot directly  by insert dcin:( do not check battery ratio)
//3: 关机状态下，插入外部电源，直接开机进入系统；无视电池电量。

	script_parser_fetch("target", "power_start", &power_start, 1);
	//check battery
	__bat_exist = axp_probe_battery_exist();
	//check power source，vbus or dcin, if both，treat it as vbus
	__power_source = axp_probe_power_source();
	pr_notice("PowerBus = %d( %d:vBus %d:acBus other: not exist)\n",
		__power_source,AXP_VBUS_EXIST,AXP_DCIN_EXIST);


	if (__bat_exist <= 0)
	{
		pr_notice("no battery exist\n");
		//EnterNormalBootMode();
		return SUNXI_STATE_NORMAL_BOOT;
	}

	if (PMU_PRE_CHARGE_MODE == gd->pmu_saved_status) {
		if (__power_source) {
			printf("SUNXI_STATE_ANDROID_CHARGE");
			return SUNXI_STATE_ANDROID_CHARGE;
		} else {
			printf("SUNXI_STATE_SHUTDOWN_DIRECTLY");
			return SUNXI_STATE_SHUTDOWN_DIRECTLY;
		}
	}

	//check battery ratio and vol
	__bat_ratio = __battery_ratio_calucate();
	__bat_vol = axp_probe_battery_vol();
	printf("Battery Voltage=%d, Ratio=%d\n", __bat_vol, __bat_ratio);

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
			printf("STATE_SHUTDOWN_DIRECTLY\n");
			break;
		case SUNXI_STATE_SHUTDOWN_CHARGE:
			printf("STATE_SHUTDOWN_CHARGE\n");
			break;
		case SUNXI_STATE_ANDROID_CHARGE:
			printf("STATE_ANDROID_CHARGE\n");
			break;
		case SUNXI_STATE_NORMAL_BOOT:
		default:
			printf("STATE_NORMAL_BOOT\n");
			break;
	}
	return __boot_state;

}


static int sunxi_pmu_treatment(void)
{
	if (uboot_spare_head.boot_ext[0].data[0] <= 0) {
		printf("no pmu exist\n");
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
	int ret;
	int advert_enable=0;
	enable_caches();
	pr_notice("task entry\n");
	sunxi_pmu_treatment();

	next_mode = sunxi_probe_power_state();

	if (next_mode == SUNXI_STATE_NORMAL_BOOT) {
		set_decode_buffer((unsigned char *)(SUNXI_LOGO_COMPRESSED_LOGO_BUFF));
	} else if (next_mode == SUNXI_STATE_SHUTDOWN_CHARGE) {
		gd->need_shutdown = 1;
		set_decode_buffer( (unsigned char *)(SUNXI_SHUTDOWN_CHARGE_COMPRESSED_LOGO_BUFF));
	} else if (next_mode == SUNXI_STATE_ANDROID_CHARGE) {
		gd->chargemode = 1;
		set_decode_buffer( (unsigned char *)(SUNXI_ANDROID_CHARGE_COMPRESSED_LOGO_BUFF));
	} else if (next_mode == SUNXI_STATE_SHUTDOWN_DIRECTLY) {
		gd->need_shutdown = 1;
		set_decode_buffer((unsigned char *)
			(SUNXI_SHUTDOWN_CHARGE_COMPRESSED_LOGO_BUFF));
	}
	set_cpu2_state(CPU_WORK);
	enable_interrupts();
	sunxi_gic_cpu_interface_init(get_core_pos());
#ifndef CONFIG_BOOTLOGO_DISABLE
	initr_sunxi_display();
	ret = script_parser_fetch("target", "advert_enable", (int *)&advert_enable, sizeof(int) / 4);
	if(ret || !advert_enable)
	{
		printf("advert_enable not set\n");
		while(BMP_DECODE_IDLE == get_bmp_decode_flag());

		if (BMP_DECODE_SUCCESS == get_bmp_decode_flag())
		{
			ret=sunxi_bmp_dipslay_screen(bmp_info);
			if(ret!=0)
				pr_error("boot logo from boot_package/toc1 item display fail\n");
			else
				pr_notice("boot logo from boot_package/toc1 item display ok\n");
		}
	}
	else
	{
		printf("advert_enable set\n");
		while(BMP_DECODE_IDLE == get_bmp_decode_flag());// ensure cpu2 off
		gd->logo_status_multiboot = DISPLAY_DRIVER_INIT_OK;
		while(gd->logo_status_multiboot == DISPLAY_DRIVER_INIT_OK);//wait cpu0 logo load
#ifdef CONFIG_BOOT_GUI
                /*
                 * if load bootlogo from Reserve0 successfully, use it;
                 * otherwise, will use one in boot_package/toc1_item.
                 */
                if(gd->logo_status_multiboot == DISPLAY_LOGO_LOAD_OK)
                {
                        ret = show_bmp_on_fb((char *)CONFIG_SYS_SDRAM_BASE, FB_ID_0);
                        if(ret != 0)
                                pr_error("boot logo from bootloader/reserve0 partition display fail\n");
                        else
                                pr_notice("boot logo from bootloader/reserve0 partition display ok\n");
                }
                else if(gd->logo_status_multiboot == DISPLAY_LOGO_LOAD_FAIL)
                {
                        ret = sunxi_bmp_dipslay_screen(bmp_info);
                        if(ret != 0)
                                 pr_error("boot logo from boot_package/toc1 item display fail\n");
                        else
                                 pr_notice("boot logo from boot_package/toc1 item display ok\n");
                }
#endif
	}

	//board_display_wait_lcd_open();
	//board_display_set_exit_mode(1);
	/* wait bmp show on screen */
	mdelay(50);
#endif
	/* show low power logo */
	if (gd->need_shutdown)
		mdelay(3000);

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
	while(get_cpu2_state() == CPU_IDLE);
	pr_msg("ready to work\n");

	compress_buf = get_decode_buffer();
	if (compress_buf == NULL) {
		printf("bmp compress_buf empty,quit\n");
		set_bmp_decode_flag(BMP_DECODE_FAIL);
		goto __third_cpu_end;
	}

	buf = (unsigned char *)(SUNXI_DISPLAY_FRAME_BUFFER_ADDR + SUNXI_DISPLAY_FRAME_BUFFER_SIZE);

	ret = sunxi_bmp_decode_from_compress(buf, compress_buf);
	if(ret) {
		printf("bmp lzma decode err\n");
		set_bmp_decode_flag(BMP_DECODE_FAIL);
		goto __third_cpu_end;
	}
#if defined(CONFIG_BOOT_GUI)
	bmp_info.buffer = buf;
	ret = 0;
#else
	ret = sunxi_bmp_display_mem(buf, &bmp_info);
#endif
	if (ret) {
		printf("bmp decode err\n");
		set_bmp_decode_flag(BMP_DECODE_FAIL);
		goto __third_cpu_end;
	}

	set_bmp_decode_flag(BMP_DECODE_SUCCESS);


__third_cpu_end:
	pr_msg("cpu %d enter wfi mode\n", get_core_pos());
	disable_interrupts();
	sunxi_gic_cpu_interface_exit();
	cleanup_before_powerdown();
	sunxi_set_cpu_off();

	return 0;
}

int sunxi_secondary_cpu_poweroff(void)
{
	pr_msg("check cpu power status\n");
	while(sunxi_probe_cpu_power_status(2));
	pr_notice("cpu2 has poweroff\n");

	while(sunxi_probe_cpu_power_status(1));
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

