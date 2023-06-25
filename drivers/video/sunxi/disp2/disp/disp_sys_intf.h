/*
 * drivers/video/sunxi/disp2/disp/disp_sys_intf.h
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
#ifndef _DISP_SYS_INTF_
#define _DISP_SYS_INTF_
#include <common.h>
#include <clk/clk.h>
#include <asm/arch/gic.h>
#include <linux/compat.h>

/* cache flush flags */
#define  CACHE_FLUSH_I_CACHE_REGION       0
#define  CACHE_FLUSH_D_CACHE_REGION       1
#define  CACHE_FLUSH_CACHE_REGION         2
#define  CACHE_CLEAN_D_CACHE_REGION       3
#define  CACHE_CLEAN_FLUSH_D_CACHE_REGION 4
#define  CACHE_CLEAN_FLUSH_CACHE_REGION   5


void disp_fdt_init(void);
int  disp_fdt_nodeoffset(char *main_name);


typedef struct
{
	char  gpio_name[32];
	int port;
	int port_num;
	int mul_sel;
	int pull;
	int drv_level;
	int data;
	int gpio;
} disp_gpio_set_t;

struct file;

struct tasklet_struct
{
	void (*func)(unsigned long data);
	unsigned long data;
};

#define DISP_PIN_STATE_ACTIVE "active"
#define DISP_PIN_STATE_SLEEP "sleep"

void disp_sys_cache_flush(void*address, u32 length, u32 flags);

void mutex_destroy(struct mutex* lock);

void tasklet_init(struct tasklet_struct *tasklet, void (*func), unsigned long data);
void tasklet_schedule(struct tasklet_struct *tasklet);


int disp_sys_register_irq(u32 IrqNo, u32 Flags,void* Handler,void *pArg,u32 DataSize,u32 Prio);
void disp_sys_unregister_irq(u32 IrqNo, void * Handler, void *pArg);
void disp_sys_disable_irq(u32 IrqNo);
void disp_sys_enable_irq(u32 IrqNo);

/* returns: 0:invalid, 1: int; 2:str, 3: gpio */
int disp_sys_script_get_item(char *main_name, char *sub_name, int value[], int count);
uintptr_t disp_sys_getprop_regbase(char *main_name, char *sub_name, u32 index);
u32 disp_sys_getprop_irq(char *main_name, char *sub_name, u32 index);
int disp_get_compat_lcd_panel_num(int disp);
int disp_get_set_lcd_param_index_from_flash(bool is_set, int idx);

int disp_sys_get_ic_ver(void);

int disp_sys_gpio_request(disp_gpio_set_t *gpio_list, u32 group_count_max);
int disp_sys_gpio_request_simple(disp_gpio_set_t *gpio_list, u32 group_count_max);
int disp_sys_gpio_release(int p_handler, s32 if_release_to_default_status);

/* direction: 0:input, 1:output */
int disp_sys_gpio_set_direction(u32 p_handler, u32 direction, const char *gpio_name);
int disp_sys_gpio_get_value(u32 p_handler, const char *gpio_name);
int disp_sys_gpio_set_value(u32 p_handler, u32 value_to_gpio, const char *gpio_name);
int disp_sys_pin_set_state(char *dev_name, char *name);

int disp_sys_power_enable(char *name);
int disp_sys_power_disable(char *name);


uintptr_t disp_sys_pwm_request(u32 pwm_id);
int disp_sys_pwm_free(uintptr_t p_handler);
int disp_sys_pwm_enable(uintptr_t p_handler);
int disp_sys_pwm_disable(uintptr_t p_handler);
int disp_sys_pwm_config(uintptr_t p_handler, int duty_ns, int period_ns);
int disp_sys_pwm_set_polarity(uintptr_t p_handler, int polarity);


#endif
