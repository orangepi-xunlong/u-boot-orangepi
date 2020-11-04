/*
 * drivers/video/sunxi/disp2/tv/drv_tv.h
 *
 * Copyright (c) 2007-2019 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef  _DRV_TV_H_
#define  _DRV_TV_H_

#if defined(__LINUX_PLAT__)
#include <linux/module.h>
#include <asm/uaccess.h>
#include <asm/memory.h>
#include <asm/unistd.h>
#include "asm-generic/int-ll64.h"
#include "linux/kernel.h"
#include "linux/mm.h"
#include "linux/semaphore.h"
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/dma-mapping.h>
#include <linux/sched.h>   //wake_up_process()
#include <linux/kthread.h> //kthread_create()??kthread_run()
#include <linux/err.h> //IS_ERR()??PTR_ERR()
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/switch.h>
#include <linux/types.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_iommu.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>

#include <video/sunxi_display2.h>
#include "video/drv_tv.h"
#else
#include "../disp/disp_sys_intf.h"
#include "../disp/de/bsp_display.h"
#include "de_tvec.h"
#endif

typedef unsigned char      	u8;
typedef signed char        	s8;
typedef unsigned short     	u16;
typedef signed short       	s16;
typedef unsigned int       	u32;
typedef signed int         	s32;
typedef unsigned long long 	u64;


enum hpd_status
{
	STATUE_CLOSE = 0,
	STATUE_OPEN  = 1,
};

#define    SCREEN_COUNT    TVE_DEVICE_NUM
#define    DAC_COUNT       TVE_DAC_NUM
#define    DAC_INVALID_SOURCE 0xff

//extern interface exported by display driver
extern s32 disp_tv_register(struct disp_tv_func * func);
extern unsigned int disp_boot_para_parse(void);

struct tv_screen_t
{
	enum  disp_tv_mode      tv_mode;
	void __iomem *base_addr;
	u32                     sid;
	u32                     cali_offset;
	u32                     enable;
	u32			dac_no[DAC_COUNT];
	u32			dac_type[DAC_COUNT];
	u32			dac_num;
	enum disp_tv_dac_source dac_source;
	struct clk*		clk;
	bool			suspend;
	bool			used;
	struct mutex mlock;
};

struct tv_info_t
{
	struct device *dev;
	struct tv_screen_t screen[SCREEN_COUNT];
	struct work_struct hpd_work;
	struct clk *clk;
	struct clk *clk_parent;
	void __iomem *base_addr;
	u32 tv_number;
};

extern struct tv_info_t g_tv_info;
static inline bool is_vga_mode(enum disp_tv_mode mode)
{
	if ((mode >= DISP_VGA_MOD_640_480P_60) &&
	    (mode < DISP_VGA_MOD_MAX_NUM))
		return true;
	return false;
}
s32 tv_init(void);
s32 tv_exit(void);
s32 tv_get_mode(u32 sel);
s32 tv_set_mode(u32 sel, enum disp_tv_mode tv_mod);
s32 tv_get_input_csc(u32 sel);
s32 tv_get_video_timing_info(u32 sel, struct disp_video_timings **video_info);
s32 tv_get_hpd_status(u32 sel);
s32 tv_enable(u32 sel);
s32 tv_disable(u32 sel);
s32 tv_suspend(u32 sel);
s32 tv_resume(u32 sel);
s32 tv_mode_support(u32 sel, enum disp_tv_mode mode);
void tv_report_hpd_work(u32 sel, u32 hpd);
s32 tv_detect_thread(void *parg);
s32 tv_detect_enable(u32 sel);
s32 tv_detect_disable(u32 sel);
s32 tv_get_dac_hpd(u32 sel);
s32 tv_hot_plugging_detect (u32 state);

#endif
