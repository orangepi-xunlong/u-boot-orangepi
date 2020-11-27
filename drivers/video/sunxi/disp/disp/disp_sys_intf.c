/* display driver
 *
 * Copyright (c) 2017 Allwinnertech Co., Ltd.
 * Author: Tyle <tyle@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "de/bsp_display.h"
#include "disp_sys_intf.h"
#include "asm/io.h"
#include <sys_config_old.h>
#include <malloc.h>


/* cache flush flags */
#define  CACHE_FLUSH_I_CACHE_REGION       0
#define  CACHE_FLUSH_D_CACHE_REGION       1
#define  CACHE_FLUSH_CACHE_REGION         2
#define  CACHE_CLEAN_D_CACHE_REGION       3
#define  CACHE_CLEAN_FLUSH_D_CACHE_REGION 4
#define  CACHE_CLEAN_FLUSH_CACHE_REGION   5

/*
*******************************************************************************
*                     CacheRangeFlush
*
* Description:
*    Cache flush
*
* Parameters:
*    address    :  start address to be flush
*    length     :  size
*    flags      :  flush flags
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
void disp_sys_cache_flush(void *address, u32 length, u32 flags)
{
	if (address == NULL || length == 0)
		return;

	switch (flags) {
	case CACHE_FLUSH_I_CACHE_REGION:
		break;

	case CACHE_FLUSH_D_CACHE_REGION:
		break;

	case CACHE_FLUSH_CACHE_REGION:
		break;

	case CACHE_CLEAN_D_CACHE_REGION:
		break;

	case CACHE_CLEAN_FLUSH_D_CACHE_REGION:
		break;

	case CACHE_CLEAN_FLUSH_CACHE_REGION:
		break;

	default:
		break;
	}
}

/*
*******************************************************************************
*                     request_irq
*
* Description:
*    irq register
*
* Parameters:
*    irqno	    ï¿½ï¿½input.  irq no
*    flags	    ï¿½ï¿½input.
*    Handler	    ï¿½ï¿½input.  isr handler
*    pArg	        ï¿½ï¿½input.  para
*    DataSize	    ï¿½ï¿½input.  len of para
*    prio	        ï¿½ï¿½input.    priority

*
* Return value:
*
*
* note:
*    s32 (*ISRCallback)( void *pArg)ï¿½ï¿½
*
*******************************************************************************
*/
int request_irq(unsigned int irq, void *handler, unsigned long flags,
	const char *name, void *dev)
{
	__inf("%s, irq=%d, handler=0x%p, dev=0x%p\n", __func__, irq, handler, dev);

	irq_install_handler(irq, (interrupt_handler_t *)handler,  dev);

	return 0;
}

/*******************************************************************************
*                     free_irq
*
* Description:
*    irq unregister
*
* Parameters:
*    irqno	ï¿½ï¿½input.  irq no
*    handler	ï¿½ï¿½input.  isr handler
*    Argment	ï¿½ï¿½input.    para
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
void free_irq(unsigned int irq, void *dev_id)
{
	irq_free_handler(irq);
}

/*
*******************************************************************************
*                     enable_irq
*
* Description:
*    enable irq
*
* Parameters:
*    irqno ï¿½ï¿½input.  irq no
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
void enable_irq(u32 irq)
{
	irq_enable(irq);
}

/*
*******************************************************************************
*                     disable_irq
*
* Description:
*    disable irq
*
* Parameters:
*     irqno ï¿½ï¿½input.  irq no
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
void disable_irq(u32 irq)
{
	irq_disable(irq);
}

void spin_lock_init(spinlock_t* lock)
{
	return;
}

void mutex_init(struct mutex* lock)
{
	return;
}

void mutex_destroy(struct mutex* lock)
{
	return;
}

void mutex_lock(struct mutex* lock)
{
	return;
}

void mutex_unlock(struct mutex* lock)
{
	return;
}

void tasklet_init(struct tasklet_struct *tasklet, void (*func), unsigned long data)
{
	if ((NULL == tasklet) || (NULL == func)) {
		__wrn("tasklet_init, para is NULL, tasklet=0x%p, func=0x%p\n", tasklet, func);
		return ;
	}
	tasklet->func = func;
	tasklet->data = data;

	return ;
}

void tasklet_schedule(struct tasklet_struct *tasklet)
{
	if (NULL == tasklet) {
		__wrn("tasklet_schedule, para is NULL, tasklet=0x%p\n", tasklet);
		return ;
	}
	tasklet->func(tasklet->data);
}

void *kmalloc(u32 Size, u32 flag)
{
	void * addr;

	addr = malloc(Size);
	if(addr)
		memset(addr, 0, Size);

	if (addr == NULL)
		printf("kmalloc fail, size=0x%x\n", Size);

	return addr;
}

void kfree(void *Addr)
{
	free(Addr);
}

/* ÐéÄâÄÚ´æºÍÎïÀíÄÚ´æÖ®¼äµÄ×ª»¯ */
unsigned int disp_vatopa(void *va)
{
#if defined(CONFIG_ARCH_SUN8IW1P1) || \
defined(CONFIG_ARCH_SUN8IW3P1) || \
defined(CONFIG_ARCH_SUN6I)
	if ((unsigned int)(va) > 0x40000000)
		return (unsigned int)(va) - 0x40000000;
#endif
	return (unsigned int)(va);
}

void *disp_patova(unsigned int pa)
{
	return (void *)(pa);
}

typedef struct __disp_node_map
{
	char node_name[16];
	int  nodeoffset;
}disp_fdt_node_map_t;

static disp_fdt_node_map_t g_disp_fdt_node_map[] ={
	{FDT_DISP_PATH, -1},
#ifdef SUPPORT_HDMI
	{FDT_HDMI_PATH, -1},
#endif
	{FDT_LCD0_PATH, -1},
	{FDT_LCD1_PATH, -1},
#ifdef CONFIG_USE_AC200
	{FDT_AC200_PATH, -1},
#endif
	{FDT_BOOT_DISP_PATH, -1},
#ifdef SUPPORT_TV
	{FDT_TV0_PATH, -1},
	{FDT_TV1_PATH, -1},
#endif
	{"",-1}
};

void disp_fdt_init(void)
{
	int i = 0;
	while(strlen(g_disp_fdt_node_map[i].node_name))
	{
		g_disp_fdt_node_map[i].nodeoffset =
			fdt_path_offset(working_fdt,g_disp_fdt_node_map[i].node_name);
		i++;
	}
}

int  disp_fdt_nodeoffset(char *main_name)
{
	int i = 0;
	for(i = 0; ; i++)
	{
		if( 0 == strcmp(g_disp_fdt_node_map[i].node_name, main_name))
		{
			return g_disp_fdt_node_map[i].nodeoffset;
		}
		if( 0 == strlen(g_disp_fdt_node_map[i].node_name) )
		{
			//last
			return -1;
		}
	}
	return -1;
}

/* type: 0:invalid, 1: int; 2:str, 3: gpio */
int disp_sys_script_get_item(char *main_name,
			     char *sub_name, int value[], int type)
{
	int ret;
	script_parser_value_type_t ret_type;
	ret = script_parser_fetch_ex(main_name, sub_name, value, &ret_type,(sizeof(script_gpio_set_t)));

	if (ret == SCRIPT_PARSER_OK)
	{
		if(ret_type == 4)
			return (ret_type-1);
		else
		return ret_type;
	}

	return 0;

}
EXPORT_SYMBOL(disp_sys_script_get_item);

int disp_sys_get_ic_ver(void)
{
	return 0;
}

int disp_sys_gpio_request(struct disp_gpio_set_t *gpio_list,
			  u32 group_count_max)
{
	user_gpio_set_t gpio_info;
	gpio_info.port = gpio_list->port;
	gpio_info.port_num = gpio_list->port_num;
	gpio_info.mul_sel = gpio_list->mul_sel;
	gpio_info.drv_level = gpio_list->drv_level;
	gpio_info.data = gpio_list->data;

	__inf("disp_sys_gpio_request, port:%d, port_num:%d, mul_sel:%d, pull:%d, drv_level:%d, data:%d\n", gpio_list->port, gpio_list->port_num, gpio_list->mul_sel, gpio_list->pull, gpio_list->drv_level, gpio_list->data);
	 //gpio_list->port, gpio_list->port_num, gpio_list->mul_sel, gpio_list->pull, gpio_list->drv_level, gpio_list->data);
#if 0
	if(gpio_list->port == 0xffff) {
		__u32 on_off;
		on_off = gpio_list->data;
		//axp_set_dc1sw(on_off);
		axp_set_supply_status(0, PMU_SUPPLY_DC1SW, 0, on_off);

		return 0xffff;
	}
#endif
	return gpio_request(&gpio_info, group_count_max);
}
EXPORT_SYMBOL(disp_sys_gpio_request);

int disp_sys_gpio_request_simple(struct disp_gpio_set_t *gpio_list,
				 u32 group_count_max)
{
	int ret = 0;
	user_gpio_set_t gpio_info;
	gpio_info.port = gpio_list->port;
	gpio_info.port_num = gpio_list->port_num;
	gpio_info.mul_sel = gpio_list->mul_sel;
	gpio_info.drv_level = gpio_list->drv_level;
	gpio_info.data = gpio_list->data;

	__inf("OSAL_GPIO_Request, port:%d, port_num:%d, mul_sel:%d, "\
		"pull:%d, drv_level:%d, data:%d\n",
		gpio_list->port, gpio_list->port_num, gpio_list->mul_sel,
		gpio_list->pull, gpio_list->drv_level, gpio_list->data);
	ret = gpio_request_early(&gpio_info, group_count_max,1);
	return ret;
}
int disp_sys_gpio_release(int p_handler, s32 if_release_to_default_status)
{
	if(p_handler != 0xffff)
	{
		gpio_release(p_handler, if_release_to_default_status);
	}
	return 0;
}
EXPORT_SYMBOL(disp_sys_gpio_release);

/* direction: 0:input, 1:output */
int disp_sys_gpio_set_direction(u32 p_handler,
				u32 direction, const char *gpio_name)
{
	return gpio_set_one_pin_io_status(p_handler, direction, gpio_name);
}

int disp_sys_gpio_get_value(u32 p_handler, const char *gpio_name)
{
	return gpio_read_one_pin_value(p_handler, gpio_name);
}

int disp_sys_gpio_set_value(u32 p_handler,
			    u32 value_to_gpio, const char *gpio_name)
{
	return gpio_write_one_pin_value(p_handler, value_to_gpio, gpio_name);
}

extern int fdt_set_all_pin(const char* node_path,const char* pinctrl_name);
int disp_sys_pin_set_state(char *dev_name, char *name)
{
int ret = -1;

	if (!strcmp(name, DISP_PIN_STATE_ACTIVE))
		ret = gpio_request_by_function("lcd0", NULL);
	else
		ret = gpio_request_by_function("lcd0_suspend", NULL);

	return ret;
}
EXPORT_SYMBOL(disp_sys_pin_set_state);

int disp_sys_power_enable(char *name)
{
	int ret = 0;
	if(0 == strlen(name)) {
		return 0;
	}
	ret = axp_set_supply_status_byregulator(name, 1);
	printf("enable power %s, ret=%d\n", name, ret);

	return 0;
}
EXPORT_SYMBOL(disp_sys_power_enable);

int disp_sys_power_disable(char *name)
{
	int ret = 0;

	printf("to disable power %s, ret=%d\n", name, ret);
	ret = axp_set_supply_status_byregulator(name, 0);
	printf("disable power %s, ret=%d\n", name, ret);

	return 0;
}
EXPORT_SYMBOL(disp_sys_power_disable);

#if 0
#if defined(CONFIG_PWM_SUNXI) || defined(CONFIG_PWM_SUNXI_NEW)
uintptr_t pwm_request(u32 pwm_id)
{
	pwm_request(pwm_id, "lcd");
	return (pwm_id + 0x100);

}

int pwm_free(uintptr_t p_handler)
{
	return 0;
}

int pwm_enable(uintptr_t p_handler)
{
	int ret = 0;
	int pwm_id = p_handler - 0x100;

	ret = pwm_enable(pwm_id);

	return ret;
}

int pwm_disable(uintptr_t p_handler)
{
	int ret = 0;
	int pwm_id = p_handler - 0x100;

	pwm_disable(pwm_id);

	return ret;
}

int pwm_config(uintptr_t p_handler, int duty_ns, int period_ns)
{
	int ret = 0;
	int pwm_id = p_handler - 0x100;

	ret = pwm_config(pwm_id, duty_ns, period_ns);

	return ret;
}

int pwm_set_polarity(uintptr_t p_handler, int polarity)
{
	int ret = 0;
	int pwm_id = p_handler - 0x100;

	ret = pwm_set_polarity(pwm_id, polarity);

	return ret;
}
#else
uintptr_t pwm_request(u32 pwm_id)
{
	uintptr_t ret = 0;

	return ret;
}

int pwm_free(uintptr_t p_handler)
{
	int ret = 0;

	return ret;
}

int pwm_enable(uintptr_t p_handler)
{
	int ret = 0;

	return ret;
}

int pwm_disable(uintptr_t p_handler)
{
	int ret = 0;

	return ret;
}

int pwm_config(uintptr_t p_handler, int duty_ns, int period_ns)
{
	int ret = 0;

	return ret;
}

int pwm_set_polarity(uintptr_t p_handler, int polarity)
{
	int ret = 0;

	return ret;
}

#endif

#endif

