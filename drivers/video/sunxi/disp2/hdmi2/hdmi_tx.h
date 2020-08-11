/*
 * Allwinner SoCs hdmi2.0 driver.
 *
 * Copyright (C) 2016 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#ifndef __INCLUDES_H__
#define __INCLUDES_H__
#include "config.h"
#if defined(__LINUX_PLAT__)

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/semaphore.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/of_irq.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include <linux/types.h>
#include <video/drv_hdmi.h>
#endif
#include "hdmi_core/hdmi_core.h"
#include "hdmi_core/core_cec.h"

#define FUNC_NAME __func__

/**
 * @short Main structures to instantiate the driver
 */
struct hdmi_tx_drv {
#if defined(__LINUX_PLAT__)

	struct platform_device		*pdev;
	/* Device node */
	struct device			*parent_dev;
#endif
	/* Device list */
	struct list_head		devlist;

	/* Interrupts */
	u32				irq;

	/* HDMI TX Controller */
	uintptr_t			reg_base;
#if defined(__LINUX_PLAT__)

	struct pinctrl			*pctl;
#endif
	struct clk			*hdmi_clk;
	struct clk			*hdmi_ddc_clk;
	struct clk			*hdmi_hdcp_clk;
	struct clk			*hdmi_cec_clk;

	struct task_struct		*hdmi_task;
	struct task_struct		*cec_task;

	struct mutex			ctrl_mutex;
	struct mutex			dmutex;

	u8                      hdcp_on;

	struct hdmi_tx_core		*hdmi_core;
};

extern s32 disp_set_hdmi_func(struct disp_device_func *func);
extern unsigned int disp_boot_para_parse(const char *name);
extern unsigned long long disp_boot_get_video_paras(unsigned int screen_num);
extern unsigned int disp_boot_para_parse_array(const char *name, unsigned int *value,
							  unsigned int count);
extern uintptr_t disp_getprop_regbase(
	char *main_name, char *sub_name, u32 index);

#endif /* __INCLUDES_H__ */
