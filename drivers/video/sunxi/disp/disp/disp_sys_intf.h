/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef _DISP_SYS_INTF_
#define _DISP_SYS_INTF_
#include <common.h>
#include <clk/clk.h>

#define pr_warn printk

/* cache flush flags */
#define  CACHE_FLUSH_I_CACHE_REGION       0
#define  CACHE_FLUSH_D_CACHE_REGION       1
#define  CACHE_FLUSH_CACHE_REGION         2
#define  CACHE_CLEAN_D_CACHE_REGION       3
#define  CACHE_CLEAN_FLUSH_D_CACHE_REGION 4
#define  CACHE_CLEAN_FLUSH_CACHE_REGION   5

void disp_fdt_init(void);
int  disp_fdt_nodeoffset(char *main_name);
struct disp_gpio_set_t {
	char gpio_name[32];
	int port;
	int port_num;
	int mul_sel;
	int pull;
	int drv_level;
	int data;
	int gpio;
};

#define DISP_IRQ_RETURN IRQ_HANDLED

struct mutex
{
	int data;
};

#define spinlock_t int

struct file;

struct tasklet_struct
{
	void (*func)(unsigned long data);
	unsigned long data;
};

struct work_struct
{
	int i;
};

#define spin_lock_irqsave(lock, cpu_sr) do {*lock=0;cpu_sr=1;}while(0)
#define spin_unlock_irqrestore(lock, cpu_sr) do {*lock=0;int i=cpu_sr;cpu_sr=i;}while(0)

#define DISP_PIN_STATE_ACTIVE "active"
#define DISP_PIN_STATE_SLEEP "sleep"
#define GFP_KERNEL 0
#define __GFP_ZERO 0
#define EFAULT 1
#define EINVAL 1
#define EXPORT_SYMBOL(val)
#define IS_ERR(val) (NULL == (val))
#define __pa(addr) (addr)
#define irq_handler_t void *
#define IRQF_DISABLED 0
#define kmalloc_array(num, size, flag) kmalloc((num) * (size), flag)
#define printk printf

void disp_sys_cache_flush(void *address, u32 length, u32 flags);

void spin_lock_init(spinlock_t* lock);
void mutex_init(struct mutex* lock);
void mutex_destroy(struct mutex* lock);
void mutex_lock(struct mutex* lock);
void mutex_unlock(struct mutex* lock);

void tasklet_init(struct tasklet_struct *tasklet, void (*func), unsigned long data);
void tasklet_schedule(struct tasklet_struct *tasklet);

void * kmalloc(u32 Size, u32 flag);
void kfree(void *Addr);

int request_irq(unsigned int irq, void *handler, unsigned long flags,
	const char *name, void *dev);
void free_irq(unsigned int irq, void *dev_id);
void disable_irq(u32 irq);
void enable_irq(u32 irq);

/* returns: 0:invalid, 1: int; 2:str, 3: gpio */
int disp_sys_script_get_item(char *main_name,
			     char *sub_name, int value[], int count);

int disp_sys_get_ic_ver(void);

int disp_sys_gpio_request(struct disp_gpio_set_t *gpio_list,
			  u32 group_count_max);
int disp_sys_gpio_request_simple(struct disp_gpio_set_t *gpio_list,
				 u32 group_count_max);
int disp_sys_gpio_release(int p_handler, s32 if_release_to_default_status);

/* direction: 0:input, 1:output */
int disp_sys_gpio_set_direction(u32 p_handler,
				u32 direction, const char *gpio_name);
int disp_sys_gpio_get_value(u32 p_handler, const char *gpio_name);
int disp_sys_gpio_set_value(u32 p_handler,
			    u32 value_to_gpio, const char *gpio_name);
int disp_sys_pin_set_state(char *dev_name, char *name);

int disp_sys_power_enable(char *name);
int disp_sys_power_disable(char *name);

#if 0
uintptr_t pwm_request(u32 pwm_id);
int pwm_free(uintptr_t p_handler);
int pwm_enable(uintptr_t p_handler);
int pwm_disable(uintptr_t p_handler);
int pwm_config(uintptr_t p_handler, int duty_ns, int period_ns);
int pwm_set_polarity(uintptr_t p_handler, int polarity);
#endif


unsigned int disp_vatopa(void *va);
void *disp_patova(unsigned int pa);

#endif
