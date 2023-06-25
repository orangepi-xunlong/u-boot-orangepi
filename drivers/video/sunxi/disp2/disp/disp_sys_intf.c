/*
 * disp2/disp/disp_sys_intf.c
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
#include "de/bsp_display.h"
#include "disp_sys_intf.h"
#include "asm/io.h"
#ifdef CONFIG_FAT_WRITE
int do_fat_fswrite(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
#endif
extern int sunxi_partition_get_partno_byname(const char *part_name);
/* cache flush flags */
#define  CACHE_FLUSH_I_CACHE_REGION       0
#define  CACHE_FLUSH_D_CACHE_REGION       1
#define  CACHE_FLUSH_CACHE_REGION         2
#define  CACHE_CLEAN_D_CACHE_REGION       3
#define  CACHE_CLEAN_FLUSH_D_CACHE_REGION 4
#define  CACHE_CLEAN_FLUSH_CACHE_REGION   5
#define  LCD_CONFIG_SIZE                  10
void mutex_destroy(struct mutex* lock)
{
	return;
}

/*
*******************************************************************************
*                     OSAL_CacheRangeFlush
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
void disp_sys_cache_flush(void*address, u32 length, u32 flags)
{
	if (address == NULL || length == 0) {
		return;
	}

	switch(flags) {
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
	return;
}

/*
*******************************************************************************
*                     disp_sys_register_irq
*
* Description:
*    irq register
*
* Parameters:
*    irqno    	    ��input.  irq no
*    flags    	    ��input.
*    Handler  	    ��input.  isr handler
*    pArg 	        ��input.  para
*    DataSize 	    ��input.  len of para
*    prio	        ��input.    priority

*
* Return value:
*
*
* note:
*    typedef s32 (*ISRCallback)( void *pArg)��
*
*******************************************************************************
*/
int disp_sys_register_irq(u32 IrqNo, u32 Flags, void* Handler,void *pArg,u32 DataSize,u32 Prio)
{
	__inf("%s, irqNo=%d, Handler=0x%p, pArg=0x%p\n", __func__, IrqNo, Handler, pArg);
	irq_install_handler(IrqNo, (interrupt_handler_t *)Handler,  pArg);

	return 0;
}

/*
*******************************************************************************
*                     disp_sys_unregister_irq
*
* Description:
*    irq unregister
*
* Parameters:
*    irqno    	��input.  irq no
*    handler  	��input.  isr handler
*    Argment 	��input.    para
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
void disp_sys_unregister_irq(u32 IrqNo, void* Handler, void *pArg)
{
	irq_free_handler(IrqNo);
}

/*
*******************************************************************************
*                     disp_sys_enable_irq
*
* Description:
*    enable irq
*
* Parameters:
*    irqno ��input.  irq no
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
void disp_sys_enable_irq(u32 IrqNo)
{
	irq_enable(IrqNo);
}

/*
*******************************************************************************
*                     disp_sys_disable_irq
*
* Description:
*    disable irq
*
* Parameters:
*     irqno ��input.  irq no
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
void disp_sys_disable_irq(u32 IrqNo)
{
	irq_disable(IrqNo);
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
	{"/soc/lcd0_1", -1},
	{"/soc/lcd0_2", -1},
	{"/soc/lcd0_3", -1},
	{"/soc/lcd0_4", -1},
	{"/soc/lcd0_5", -1},
	{"/soc/lcd0_6", -1},
	{"/soc/lcd0_7", -1},
	{"/soc/lcd0_8", -1},
	{"/soc/lcd0_9", -1},
#ifdef CONFIG_DISP2_TV_AC200
	{FDT_AC200_PATH, -1},
#endif
	{FDT_BOOT_DISP_PATH, -1},
#ifdef SUPPORT_TV
	{FDT_TV0_PATH, -1},
	{FDT_TV1_PATH, -1},
#endif
#if defined(SUPPORT_EDP) && defined(CONFIG_EDP_DISP2_SUNXI)
	{FDT_EDP0_PATH, -1},
	{FDT_EDP1_PATH, -1},
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

/*now only support lcd0*/
int disp_get_compat_lcd_panel_num(int disp)
{
	char dts_node_path[] = "/soc/lcd0_0";
	int node;
	int i;
	static int count = -1;

	if (disp > 0)
		return 0;
	if (count == -1) {
		count = 0;
		for (i = 1; i <= 9; i++) {
			dts_node_path[10] = '0' + i;
			node = fdt_path_offset(working_fdt, dts_node_path);
			if (node < 0) {
				break;
			}
			count++;
		}
	}
		return count;
}
/*
*******************************************************************************
*                     disp_get_set_lcd_param_index_from_flash
*
* Description:
*    Get or set lcd_param_index from flash.
*
* Parameters:
*    is_set: true when set, false when get
*    idx: index to set, not used when get
*
* Return value:
*    >0 for index of lcd param
*     0 for using original lcd param--lcd0
*    -1 for err
* note:
*
*
*******************************************************************************
*/
int disp_get_set_lcd_param_index_from_flash(bool is_set, int idx)
{
	char *argv[6];
	char part_num[16] = {0};
	char len[16] = {0};
	char addr[32] = {0};
	char file_name[32] = "lcd_compatible_index.txt";
	int partno = -1;
	char *buf;
	unsigned int lcd_param_index;
	int ret = -1;
	static int idx_get = -1;
	if (!is_set && idx_get != -1) {
		return idx_get;
	}

	partno = sunxi_partition_get_partno_byname("bootloader"); /*android*/
	if (partno < 0) {
		partno = sunxi_partition_get_partno_byname("boot-resource"); /*linux*/
		if (partno < 0) {
			printf("Get bootloader and boot-resource partition number fail!\n");
		ret = -1;
		goto exit;
		}
	}


	buf = kmalloc(LCD_CONFIG_SIZE, GFP_KERNEL | __GFP_ZERO);
	memset(buf, 0, LCD_CONFIG_SIZE);
	snprintf(part_num, 16, "0:%x", partno);
	snprintf(len, 16, "%ld", (ulong)LCD_CONFIG_SIZE);
	snprintf(addr, 32, "%lx", (ulong)buf);
	argv[1] = "sunxi_flash";
	argv[2] = part_num;
	argv[3] = addr;
	argv[4] = file_name;
	argv[5] = len;

	if (!is_set) {
		argv[0] = "fatload";
		if (do_fat_fsload(0, 0, 6, argv)) {
			pr_error("do_fat_fsload for lcd config failed\n");
			ret = -1;
			goto exit_free;
		}
		if (!strncmp("lcd", buf, 3)) {

			lcd_param_index = simple_strtoul(buf + 3, NULL, 10);
			ret = idx_get = lcd_param_index;
			if (lcd_param_index > 9) {
				pr_error("error: lcd_param_index must less than 10\n");
				ret = -1;
				goto exit_free;
			}

		} else {
			pr_error("lcd_param_index format err, should be lcdn, n=0,1,2,3...\n");
			ret = -1;
		}
	} else {
#ifdef CONFIG_FAT_WRITE
		if (idx < 0 || idx > 9) {
			ret = -1;
			pr_error("set_lcd_param_index_from_flash param err\n");
			goto exit_free;
		}
		memcpy(buf, "lcd0", 5);
		buf[3] = '0' + idx;
		argv[0] = "fatwrite";
		if (do_fat_fswrite(0, 0, 6, argv)) {
			pr_error("do_fat_fswrite for lcd config failed\n");
			ret = -1;
			goto exit_free;
		} else {
			ret = idx_get = idx;
			printf("save lcd compatible index %d to flash\n", idx);
		}
#else
		pr_error("please enable FAT_WRITE for lcd compatible first\n");
		ret = -1;
#endif
}
exit_free:
	kfree(buf);
exit:
return ret;

}

/*
*******************************************************************************
*                     disp_update_lcd_param
*
* Description:
*    Update lcd0 param with lcd0_<lcd_param_index> in dts for driver to reopen lcd driver.
*
* Parameters:
*    lcd_param_index : input, the index of compatible lcd param.
*
* Return value:
*    0 for success
*    -1 for err
* note:
*    if lcd_param_index = 0, we will use lcd0 param, and make no update.
*      lcd_param_index must <= 9.
*    lcd_param_index = -1 means using previous lcd_param_index;
*
*******************************************************************************
*/
int disp_update_lcd_param(int lcd_param_index)
{
	int node;
	int level = 0;
	static int pre_index;
	uint32_t tag;
	int  nodeoffset;
	int  nextoffset;
	const struct fdt_property *fdt_prop;
	const char *pathp;
	int prop_len;
	char dts_node_path[] = "/soc/lcd0_0";
	int len;
	int depth = 1;
	const void *nodep;
	unsigned int success_count = 0;
	unsigned int retry_skip_count = 0;
	u8 prop_tmp[32] = {0};
	u8 prop_name_tmp[32] = {0};
	int flash_idx = 0;


	if (lcd_param_index == -1) {

		/*if not using compatible panel, following operation is useless and wasting time, so just return.*/
		if (disp_get_compat_lcd_panel_num(0/*FIXME*/) == 0)
			return 0;
	/*
	    4 case:
	     1. first time      compatible panel -> need update kernel fdt   ;  pre_index=idx;  flash_idx=-1   ;   need to write idx to flash
	     2. not first time  compatible panel -> need update kernel fdt   ;  pre_index=0;    flash_idx=idx  ;   no need to write flash
	     3. first time      default panel    -> no need update kernel fdt;  pre_index=0;    flash_idx=-1   ;   need write 0 to flash
	     4. not first time  defalut panel    -> no need update kernel fdt;  pre_index=0;    flash_idx=0    ;   no need to write to flash
	*/

		flash_idx = disp_get_set_lcd_param_index_from_flash(false, flash_idx);
		if (flash_idx == -1) {
			disp_get_set_lcd_param_index_from_flash(true, pre_index);
			flash_idx = disp_get_set_lcd_param_index_from_flash(false, flash_idx);
		}
		lcd_param_index = flash_idx;

		char *ctp_path;
		char val[2];
		int nodeoff;

		// set ctp fw idx
		nodeoff = fdt_path_offset(working_fdt, "/aliases");
		fdt_getprop_string(working_fdt, nodeoff, "ctp", &ctp_path);
		snprintf(val, 2, "%d", lcd_param_index);
		fdt_find_and_setprop(working_fdt, ctp_path, "ctp_fw_idx", val, 2, 1);
	} else if (lcd_param_index != 0) {
		pre_index = lcd_param_index;
		/*
		    now using uboot fdt, no need to update.
		*/
		return 0;
	}
	dts_node_path[10] = '0' + lcd_param_index;
	if (lcd_param_index == 0) {
		/*use default lcd0 param*/
		return 0;
	}

//	pre_index = lcd_param_index;
	nodeoffset = fdt_path_offset(working_fdt, dts_node_path);
	node = fdt_path_offset(working_fdt, "lcd0");

	if (nodeoffset < 0 || node < 0) {
		/*
		 * Not found or something else bad happened.
		 */
		pr_error("libfdt fdt_path_offset() for lcd\n");
		return -1;
	}

	while (level >= 0) {
		tag = fdt_next_tag(working_fdt, nodeoffset, &nextoffset);
		switch (tag) {
		case FDT_BEGIN_NODE:
			pathp = fdt_get_name(working_fdt, nodeoffset, NULL);
			if (level <= depth) {
				if (pathp == NULL)
					pathp = "/* NULL pointer error */";
				if (*pathp == '\0')
					pathp = "/";	/* root is nameless */
			}
			level++;
			break;
		case FDT_END_NODE:
			level--;
			if (level == 0) {
				level = -1;		/* exit the loop */
			}
			break;
		case FDT_PROP:
			if (success_count < retry_skip_count) {
				success_count++;
				break;
			}
			fdt_prop = fdt_offset_ptr(working_fdt, nodeoffset,
					sizeof(*fdt_prop));
			pathp    = fdt_string(working_fdt,
					fdt32_to_cpu(fdt_prop->nameoff));
			prop_len      = fdt32_to_cpu(fdt_prop->len);
			nodep    = fdt_prop->data;
			if (prop_len < 0) {
				printf ("libfdt fdt_getprop(): %s\n",
					fdt_strerror(prop_len));
				return -1;
			} else if (prop_len == 0) {
				/* the property has no value */
				if (level <= depth)
					printf("NULL value for lcd0 param\n");
			} else {
				if (level <= depth) {
					/*update this prop for lcd0*/
					fdt_getprop(working_fdt, node, pathp, &len);
					success_count++;
					if (len != prop_len) {
						/*need memcpy and update node offset to avoid modify by fdt_setprop.*/
						level = 0;
						retry_skip_count = success_count;
						success_count = 0;
						memcpy(prop_name_tmp, pathp, strlen(pathp)+1);
						memcpy(prop_tmp, nodep, prop_len);
						fdt_setprop(working_fdt, node, pathp, prop_tmp, prop_len);
						nodeoffset = fdt_path_offset(working_fdt, dts_node_path);
						node = fdt_path_offset(working_fdt, "lcd0");
						continue;
					} else {
						fdt_setprop(working_fdt, node, pathp, nodep, prop_len);
					}
				}
			}
			break;
		case FDT_NOP:
			break;
		case FDT_END:
			disp_fdt_init();
			return 0;
		default:
			if (level <= depth)
				printf("Unknown tag 0x%08X\n", tag);
			return 0;
		}
		nodeoffset = nextoffset;
	}

	disp_fdt_init();
	return 0;
}
/* type: 0:invalid, 1: int; 2:str, 3: gpio */
int disp_sys_script_get_item(char *main_name, char *sub_name, int value[], int type)
{
	int node;
	int ret = 0;
	user_gpio_set_t  gpio_info;
	disp_gpio_set_t  *gpio_list;
	node = disp_fdt_nodeoffset(main_name);
        if (node < 0) {
          printf("fdt get node offset faill: %s\n", main_name);
          return ret;
        }

        if (1 == type) {
          if (fdt_getprop_u32(working_fdt, node, sub_name, (uint32_t *)value) >=
              0)
            ret = type;
        } else if (2 == type) {
		const char *str;
                if (fdt_getprop_string(working_fdt, node, sub_name,
                                       (char **)&str) >= 0) {
                  ret = type;
                  memcpy((void *)value, str, strlen(str) + 1);
                }
        } else if (3 == type) {
		ret = fdt_get_one_gpio_by_offset(node, sub_name, &gpio_info);
		if (ret >= 0) {
			if (ret == 4) {
				gpio_list = (disp_gpio_set_t  *)value;
				gpio_list->port = gpio_info.port;
				gpio_list->port_num = gpio_info.port_num;
				gpio_list->mul_sel = 1;
				gpio_list->drv_level = 1;
				gpio_list->pull = 0;
				gpio_list->data = gpio_info.mul_sel;
			} else {
				gpio_list = (disp_gpio_set_t  *)value;
				gpio_list->port = gpio_info.port;
				gpio_list->port_num = gpio_info.port_num;
				gpio_list->mul_sel = gpio_info.mul_sel;
				gpio_list->drv_level = gpio_info.drv_level;
				gpio_list->pull = gpio_info.pull;
				gpio_list->data = gpio_info.data;
			}

			memcpy(gpio_info.gpio_name, sub_name, strlen(sub_name)+1);
			debug("%s.%s gpio=%d,mul_sel=%d,data:%d\n",main_name, sub_name, gpio_list->gpio, gpio_list->mul_sel, gpio_list->data);
			ret = type;
		}
	}
	return ret;
}

EXPORT_SYMBOL(disp_sys_script_get_item);

int disp_sys_get_ic_ver(void)
{
    return 0;
}

int disp_sys_gpio_request(disp_gpio_set_t *gpio_list, u32 group_count_max)
{
	user_gpio_set_t gpio_info;
	gpio_info.port = gpio_list->port;
	gpio_info.port_num = gpio_list->port_num;
	gpio_info.mul_sel = gpio_list->mul_sel;
	gpio_info.drv_level = gpio_list->drv_level;
	gpio_info.data = gpio_list->data;

        __inf("disp_sys_gpio_request, port:%d, port_num:%d, mul_sel:%d, "
              "pull:%d, drv_level:%d, data:%d\n",
              gpio_list->port, gpio_list->port_num, gpio_list->mul_sel,
              gpio_list->pull, gpio_list->drv_level, gpio_list->data);
        /*TODO:different name*/
	return sunxi_gpio_request(&gpio_info, group_count_max);
}
EXPORT_SYMBOL(disp_sys_gpio_request);

int disp_sys_gpio_request_simple(disp_gpio_set_t *gpio_list, u32 group_count_max)
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
int disp_sys_gpio_set_direction(u32 p_handler, u32 direction, const char *gpio_name)
{
	return gpio_set_one_pin_io_status(p_handler, direction, gpio_name);
}

int disp_sys_gpio_get_value(u32 p_handler, const char *gpio_name)
{
	return gpio_read_one_pin_value(p_handler, gpio_name);
}

int disp_sys_gpio_set_value(u32 p_handler, u32 value_to_gpio, const char *gpio_name)
{
	return gpio_write_one_pin_value(p_handler, value_to_gpio, gpio_name);
}

extern int fdt_set_all_pin(const char* node_path,const char* pinctrl_name);
int disp_sys_pin_set_state(char *dev_name, char *name)
{
	int ret = -1;

	if (!strcmp(name, DISP_PIN_STATE_ACTIVE))
		ret = fdt_set_all_pin(dev_name, "pinctrl-0");
	else
		ret = fdt_set_all_pin(dev_name, "pinctrl-1");

	if (ret != 0)
		printf("%s, fdt_set_all_pin, ret=%d\n", __func__, ret);
	return ret;
}
EXPORT_SYMBOL(disp_sys_pin_set_state);

int disp_sys_power_enable(char *name)
{
	int ret = 0;
	if(0 == strlen(name)) {
		return 0;
	}
#if defined(CONFIG_SUNXI_PMU)
	/*TODO:bmu*/
	ret = pmu_set_voltage(name, 0, 1);
	if(!ret)
		__wrn("enable power %s, ret=%d\n", name, ret);
#endif

	return ret;
}
EXPORT_SYMBOL(disp_sys_power_enable);

int disp_sys_power_disable(char *name)
{
	int ret = 0;
	/*TODO:bmu*/
#if 0
	/*
	 * NOTE: DO NOT enable this.
	 * Because compatible panel driver may call this,
	 * which may cause other modules get wrong when using
	 * the same regulator.
	 */
	ret = pmu_set_voltage(name, 0, 0);
	if(!ret)
		__wrn("disable power %s, ret=%d\n", name, ret);
#endif
	return ret;
}
EXPORT_SYMBOL(disp_sys_power_disable);

#if defined(CONFIG_PWM_SUNXI) || defined(CONFIG_PWM_SUNXI_NEW)
uintptr_t disp_sys_pwm_request(u32 pwm_id)
{
	pwm_request(pwm_id, "lcd");
	return (pwm_id + 0x100);
}

int disp_sys_pwm_free(uintptr_t p_handler)
{
	return 0;
}

int disp_sys_pwm_enable(uintptr_t p_handler)
{
	int ret = 0;
	int pwm_id = p_handler - 0x100;

	ret = pwm_enable(pwm_id);
	return ret;

}

int disp_sys_pwm_disable(uintptr_t p_handler)
{
	int ret = 0;
	int pwm_id = p_handler - 0x100;

	pwm_disable(pwm_id);
	return ret;
}

int disp_sys_pwm_config(uintptr_t p_handler, int duty_ns, int period_ns)
{
	int ret = 0;
	int pwm_id = p_handler - 0x100;

	ret = pwm_config(pwm_id, duty_ns, period_ns);
	return ret;
}

int disp_sys_pwm_set_polarity(uintptr_t p_handler, int polarity)
{
	int ret = 0;
	int pwm_id = p_handler - 0x100;

	ret = pwm_set_polarity(pwm_id, polarity);
	return ret;
}
#else
uintptr_t disp_sys_pwm_request(u32 pwm_id)
{
	return 0;
}

int disp_sys_pwm_free(uintptr_t p_handler)
{
	return 0;
}

int disp_sys_pwm_enable(uintptr_t p_handler)
{
	return 0;
}

int disp_sys_pwm_disable(uintptr_t p_handler)
{
	return 0;
}

int disp_sys_pwm_config(uintptr_t p_handler, int duty_ns, int period_ns)
{
	return 0;
}

int disp_sys_pwm_set_polarity(uintptr_t p_handler, int polarity)
{
	return 0;
}
#endif
